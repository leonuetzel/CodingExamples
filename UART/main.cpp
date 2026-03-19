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
	
	
	//	Example: UART
	//	Initialize the UART peripheral and provide various examples what can be transmitted and how to transmit it (single bytes, strings, numbers, etc...)
	
	//	Select the UART pins and the alternate function (view the datasheet for details on the alternate functions and pinout)
	constexpr MCU::PIN pin_tx = MCU::PIN::A9;
	constexpr MCU::PIN pin_rx = MCU::PIN::A10;
	constexpr MCU::AF af_tx = MCU::AF::A9_USART1TX;
	constexpr MCU::AF af_rx = MCU::AF::A10_USART1RX;
	
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
	
	
	//	Get the UART instance (choose the UART with the pins you provided above)
	UART& uart = stm32.get_usart_1();
	
	
	//	UART instances are always protected by a semaphore against concurrent access,
	//	so we need to acquire the semaphore before using the UART (this is needed because the UART is a shared resource and can be used by multiple threads)
	if(cmos.semaphore_lock(&uart) != OK)
	{
		return(FAIL);
	}
	
	
	//	Now we can initialize the UART with the desired settings (baudrate, data bits, stop bits, parity, etc...)
	//	The databuffer sizes are needed to initialize the ringbuffers for the UART, which are used to store the received and transmitted data
	//	The sizes are just examples, you can change them as you wish, but keep in mind that the buffers are located on the heap memory (which size is set in the linker script)
	constexpr uint32 baudrate = 115200;
	constexpr UART::e_databits	databits	= UART::e_databits::_8;
	constexpr UART::e_parity 		parity		= UART::e_parity::NONE;
	constexpr UART::e_stopbits	stopbits	= UART::e_stopbits::_1;
	
	constexpr uint32 rxBufferSize = 128;
	constexpr uint32 txBufferSize = 128;
	
	if(uart.init(baudrate, databits, parity, stopbits, new uint8[rxBufferSize], rxBufferSize, new uint8[txBufferSize], txBufferSize) != OK)
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
	
	
	//	Initially clear the UART buffers to avoid any garbage data being transmitted or received from previous operations (this is optional, but recommended)
	uart.clear();
	
	
	//	Now we can use the UART to transmit and receive data
	while(1)
	{
		//	Example: Transmit a single byte
		uart << 'X';
		uart.tx();
		
		
		//	Example: Transmit a string
		uart << "Hello, World!";
		uart.tx();
		
		
		//	Example: Transmit a number (the number will be converted to a string and then transmitted)
		uart << String((uint32) 12345);
		uart.tx();
		
		
		//	Example: Transmit a number in binary format (this will be done MSB first)
		//	The number will be transmitted as:
		//	1 Byte	if casted to uint8	or int8
		//	2 Bytes if casted to uint16 or int16
		//	4 Bytes if casted to uint32 or int32
		constexpr uint32 number = 1234567890;
		
		uart << (uint32) number;
		uart.tx();
		
		uart << (uint16) number;
		uart.tx();
		
		uart << (uint8) number;
		uart.tx();
		
		
		//	Example: Transmit a float in binary format (this will be done MSB first)
		uart << 123.45f;
		uart.tx();
		
		
		//	Example: Transmit a double in binary format (this will be done MSB first)
		uart << 123.45;
		uart.tx();
		
		
		
		
		
		//	Example: Receive data
		//	The received data will be stored in the RX buffer of the UART, which is a ringbuffer (the size is set during the initialization of the UART)
		
		//	The rx() function is a blocking function and reads only one byte from the RX buffer, so you need to call it multiple times to read multiple bytes (or use a loop)
		//	This function wont block the whole system, but only the thread that calls it, so other threads can still run while waiting for data to be received
		uart.rx();
		
		//	The >> operator is also blocking, but it can read multiple bytes from the RX buffer and store them in a variable
		//	The variable must be of the same type as the data you want to read, e.g. uint8 for a single byte, String for a string, etc...
		//	It is recommended to use the >> operator always together with the << operator on the other side to avoid interpreting issues (MSB vs. LSB, etc...)
		//	For every << operator from above, there exists a corresponding >> operator
		//	This function wont block the whole system, but only the thread that calls it, so other threads can still run while waiting for data to be received
		uint32 receivedNumber;
		uart >> receivedNumber;
		
		String receivedString;
		uart >> receivedString;
	}
	return(OK);
}
