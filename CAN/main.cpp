#include "../Inc/main.hpp"

feedback clocksInit()
{
	//	Get the CMOS and RCC instances
	CMOS& cmos = CMOS::get();
	RCC& rcc = STM32H753BIT6::get().get_rcc();
	
	
	//	This is the clock tree (the values are just examples, you can change them as you wish but keep the maximum frequencies in mind -> datasheet of the MCU)
	
	//				PLL 1													PLL 2													PLL 3
	//			 960 MHz											 460 MHz											 460 MHz
	//		 -----------									 -----------									 -----------
	//		| 		|			|									|			|			|									|			|			|
	//		P			Q			R									P			Q			R									P			Q			R
	//		2			8			10								6			1			2									1			1			26
	//		|			|			|									|			|			|									|			|			|
	//	 M7		 												 				 													 
	//	 																					 
	
	
	//	The systemclock we want to achieve
	constexpr uint32 systemClock = 480000000;
	
	
	//	Initialize the HSE oscillator (only do so if the PCB has one soldered!)
	if(rcc.HSE_init(true) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set the HSE as the clock source for the PLLs (the PLLs will multiply this frequency to achieve the desired system clock)
	if(rcc.set_clockSource_pll(RCC::e_clockSource_pll::HSE) != OK)
	{
		return(FAIL);
	}
	
	
	//	Initialize the PLLs with the desired frequencies and dividers (see clock tree above)
	if(rcc.pll_1_init(2 * systemClock, 2, 8, 10) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_2_init(460000000, 6, 1, 2) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_3_init(460000000, 1, 1, 26) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set HSE as the clock source for the PERCK clock
	if(rcc.set_clockSource(RCC::e_clockSource_perck::HSE) != OK)
	{
		return(FAIL);
	}
	
	
	//	Important: inform the OS about the new system clock frequency (this is needed for the OS to calculate the correct timings for the sleep functions, etc...)
	//	Do this BEFORE changing the clock source, if you increase the systemclock
	//	Do this AFTER changing the clock source, if you decrease the systemclock (to avoid timing issues during the transition)
	if(cmos.set_systemClock(systemClock) != OK)
	{
		return(FAIL);
	}
	
	
	//	Set the PLL1 as the clock source for the system clock
	if(rcc.set_clockSource_system(RCC::e_clockSource_system::PLL1) != OK)
	{
		return(FAIL);
	}
	return(OK);
}





int main()
{
	//	Initialize the clocks - this is MCU specific
	if(clocksInit() != OK)
	{
		return(FAIL);
	}
	

	//	Get the CMOS instance - this instance is MCU independent and must be used to access the OS functions and the core peripherals (NVIC, etc...)
	CMOS& cmos = CMOS::get();
	
	
	//	Get the STM32 instance - this is MCU specific and is used to access the peripherals (except the core peripherals which are accessed through the CMOS instance)
	STM32H753BIT6& stm32 = STM32H753BIT6::get();
	
	
	//	Example: CAN
	//	Initialize the CAN peripheral and provide an example on how to use it
	
	//	Select the CAN pins and the alternate function (view the datasheet for details on the alternate functions and pinout)
	constexpr MCU::PIN pin_rx = MCU::PIN::A11;
	constexpr MCU::PIN pin_tx = MCU::PIN::A12;
	constexpr MCU::AF af_rx = MCU::AF::A11_FDCAN1RX;
	constexpr MCU::AF af_tx = MCU::AF::A12_FDCAN1TX;
	
	//	Get the GPIO instance
	I_GPIO& gpio = stm32.get_gpio();
	
	
	//	Set the alternate function for the pins
	if(gpio.set_AF(af_tx) != OK)
	{
		return(FAIL);
	}
	if(gpio.set_AF(af_rx) != OK)
	{
		return(FAIL);
	}
	
	
	//	Get the CAN instance (choose the CAN with the pins you provided above)
	I_CAN& can = stm32.get_can_1();
	
	
	//	CAN instances are always protected by a semaphore against concurrent access,
	//	so we need to acquire the semaphore before using the CAN (this is needed because the CAN is a shared resource and can be used by multiple threads)
	if(cmos.semaphore_lock(&can) != OK)
	{
		return(FAIL);
	}
	
	
	//	Now we can initialize the CAN with the desired settings (baudrate, filters, silent mode, etc...)
	//	The databuffer sizes are needed to initialize the ringbuffers for the CAN, which are used to store the received and transmitted data
	//	The sizes are just examples, you can change them as you wish, but keep in mind that the buffers are located on the heap memory (which size is set in the linker script)
	constexpr uint32 baudrate = 500000;
	constexpr bool silentMode = false;
	
	constexpr uint32 rxBufferSize = 128;
	constexpr uint32 txBufferSize = 128;
	
	//	Configure the filter elements
	//	We need one array for the standard filters and one for the extended filters
	//	We configure a filter that passes all messages
	Array<I_CAN::s_filterElement> standardFilters;
	Array<I_CAN::s_filterElement> extendedFilters;
	
	I_CAN::s_filterElement standardFilterElement_allPass;
	standardFilterElement_allPass.behavior = I_CAN::e_filterBehavior::STORE_IN_RX_FIFO_0;
	standardFilterElement_allPass.type = I_CAN::e_filterType::RANGE;
	standardFilterElement_allPass.ID1_or_filter = 0x000;
	standardFilterElement_allPass.ID2_or_mask = 0x7FF;
	
	I_CAN::s_filterElement extendedFilterElement_allPass;
	extendedFilterElement_allPass.behavior = I_CAN::e_filterBehavior::STORE_IN_RX_FIFO_0;
	extendedFilterElement_allPass.type = I_CAN::e_filterType::RANGE;
	extendedFilterElement_allPass.ID1_or_filter = 0x000;
	extendedFilterElement_allPass.ID2_or_mask = 0x1FFFFFFF;
	
	standardFilters += standardFilterElement_allPass;
	extendedFilters += extendedFilterElement_allPass;
	
	
	if(can.init(baudrate, standardFilters, extendedFilters, silentMode, rxBufferSize, txBufferSize) != OK)
	{
		return(FAIL);
	}
	
	
	//	Initialize the pin as alternate function and set the speed (view the datasheet for details on the different modes and speeds)
	//	Mode Shorts:
	//	- OUT / IN / ANALOG: Output / Input mode / Analog mode (used for the ADC, DAC, etc...)
	//	- GP / AF: General Purpose (the GPIO controller controls the pin) / Alternate Function (another peripheral controls the pin, e.g. USART, SPI, etc...)
	//	- PP / OD: Push-Pull (the pin is driven high or low) / Open-Drain (the pin can only be driven low, to drive it high you need an external pull-up resistor)
	if(gpio.init_pin(pin_rx, I_GPIO::e_mode::AF_PP, I_GPIO::e_speed::HIGH) != OK)
	{
		return(FAIL);
	}
	if(gpio.init_pin(pin_tx, I_GPIO::e_mode::AF_PP, I_GPIO::e_speed::HIGH) != OK)
	{
		return(FAIL);
	}
	
	
	//	Now we can use the CAN to transmit and receive data
	while(1)
	{
		//	Example: Transmit a frame (details to CAN frames can be found on the internet)
		CAN_Frame txFrame;
		txFrame.remoteTransmissionRequest = false;
		txFrame.extendedID = false;
		txFrame.ID = 0x123;
		txFrame.dataSize = 8;
		txFrame.data[0] = 0xDE;
		txFrame.data[1] = 0xAD;
		txFrame.data[2] = 0xBE;
		txFrame.data[3] = 0xEF;
		txFrame.data[4] = 0xCA;
		txFrame.data[5] = 0xFE;
		txFrame.data[6] = 0xBA;
		txFrame.data[7] = 0xBE;
		
		can.tx(txFrame);
		
		
		//	Example: Receive data
		//	The received data will be stored in the RX buffer of the CAN, which is a ringbuffer (the size is set during the initialization of the CAN)
		//	The rx() function is a blocking function and reads only one frame from the RX buffer
		//	This function wont block the whole system, but only the thread that calls it, so other threads can still run while waiting for data to be received
		//	The received data can then be read from the object "rxFrame"
		CAN_Frame rxFrame;
		can.rx(rxFrame);
	}
	return(OK);
}
