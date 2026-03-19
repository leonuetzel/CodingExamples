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
	
	
	//	Example: Toggle a GPIO pin
	
	//	Select the pin
	constexpr MCU::PIN pin = MCU::PIN::A0;
	
	//	Get the GPIO instance
	I_GPIO& gpio = stm32.get_gpio();
	
	
	//	Initialize the pin as output and set the speed (view the datasheet for details on the different modes and speeds)
	//	Mode Shorts:
	//	- OUT / IN / ANALOG: Output / Input mode / Analog mode (used for the ADC, DAC, etc...)
	//	- GP / AF: General Purpose (the GPIO controller controls the pin) / Alternate Function (another peripheral controls the pin, e.g. USART, SPI, etc...)
	//	- PP / OD: Push-Pull (the pin is driven high or low) / Open-Drain (the pin can only be driven low, to drive it high you need an external pull-up resistor)
	if(gpio.init_pin(pin, I_GPIO::e_mode::OUT_GP_PP, I_GPIO::e_speed::HIGH) != OK)
	{
		return(FAIL);
	}
	
	
	//	Toggle the pin in an infinite loop with a delay of 1 second
	while(1)
	{
		gpio.toggle_pin(pin);
		cmos.sleep_sec(1);
	}
	return(OK);
}
