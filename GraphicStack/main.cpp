#include "../Inc/main.hpp"





feedback clocksInit()
{
	CMOS& cmos = CMOS::get();
	RCC& rcc = STM32H753BIT6::get().get_rcc();
	
	//					PLL 1													PLL 2													PLL 3
	//				 960 MHz											 460 MHz											 400 MHz
	//						|															|															|
	//			 -----------									 -----------									 -----------
	//			| 		|			|									|			|			|									|			|			|
	//			P			Q			R									P			Q			R									P			Q			R
	//			2			8			10								1			1			2									1			1			10
	//			|			|			|									|			|			|									|			|			|
	//		480MHz																		230MHz												 40MHz
	//	Cortex M7		 												 				 FMC													 LTDC
	
	
	constexpr uint32 systemClock = 480000000;
	if(rcc.HSE_init(true) != OK)
	{
		return(FAIL);
	}
	if(rcc.set_clockSource_pll(RCC::e_clockSource_pll::HSE) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_1_init(2 * systemClock, 2, 8, 10) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_2_init(480000000, 1, 1, 2) != OK)
	{
		return(FAIL);
	}
	if(rcc.pll_3_init(400000000, 1, 1, 10) != OK)
	{
		return(FAIL);
	}
	if(rcc.set_clockSource(RCC::e_clockSource_perck::HSE) != OK)
	{
		return(FAIL);
	}
	if(cmos.set_systemClock(systemClock) != OK)
	{
		return(FAIL);
	}
	if(rcc.set_clockSource_system(RCC::e_clockSource_system::PLL1) != OK)
	{
		return(FAIL);
	}
	return(OK);
}





int main()
{
	CMOS& cmos = CMOS::get();
	if(clocksInit() != OK)
	{
		return(0);
	}
	
	
	//	Display and SDRAM
	Display::get();
	
	
	//	GUI
	GUI::get();
	
	
	//	Sleep forever
	while(1)
	{
		cmos.sleep_sec(1);
	}
	return(OK);
}
