#include "../Inc/display.hpp"





/*****************************************************************************/
/*                    Globals and Static Initialization					 						 */
/*****************************************************************************/

SECTION(Display::m_sdramBuffer)	Color Display::m_sdramBuffer[Display::sdramSizeInBytes / sizeof(Color)];



/*****************************************************************************/
/*                      						Private	  			 						 						 */
/*****************************************************************************/

Display::Display()
	:	m_brightness(0),
		m_displayState(false)
{
	//	Init pins
	init_pins();
	
	
	//	Init external SDRAM
	if(init_sdram() != OK)
	{
		return;
	}
	
	
	//	Init Graphics stack
	if(init_graphics() != OK)
	{
		return;
	}
	
	
	//	Init timer with PWM for background light
	STM32H753BIT6& stm32 = STM32H753BIT6::get();
	Timer_12& timer_12 = stm32.get_timer_12();
	timer_12.init(PCB::TFT_BACKLIGHT::TIMER::PWM_FREQUENCY, false);
	timer_12.init_pwm(PCB::TFT_BACKLIGHT::TIMER::PWM_CHANNEL);
	timer_12.set_pwm(PCB::TFT_BACKLIGHT::TIMER::PWM_CHANNEL, 0);
	
	
	//	Sleep a bit to let display content stabilize
	CMOS::get().sleep_ms(100);
	
	
	//	Enable and turn on display
	set_brightness(50);
	set_state(true);
}


Display::~Display()
{
	
}







void Display::init_pins()
{
	GPIO& gpio = STM32H753BIT6::get().get_gpio();
	
	
	//	LTDC
	for(auto& i: PCB::TFT_LTDC::R)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	for(auto& i: PCB::TFT_LTDC::G)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	for(auto& i: PCB::TFT_LTDC::B)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	
	gpio.init_pin(PCB::TFT_LTDC::DE,				GPIO::e_mode::AF_PP, 			GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::TFT_LTDC::CLK,				GPIO::e_mode::AF_PP, 			GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::TFT_LTDC::HSYNC,			GPIO::e_mode::AF_PP, 			GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::TFT_LTDC::VSYNC,			GPIO::e_mode::AF_PP, 			GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	
	gpio.init_pin(PCB::TFT_BACKLIGHT::PWM,	GPIO::e_mode::AF_PP,			GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::TFT_BACKLIGHT::EN,	GPIO::e_mode::OUT_GP_PP,	GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	
	
	
	
	
	for(auto& i: PCB::TFT_LTDC::AF::R)
	{
		gpio.set_AF(i);
	}
	for(auto& i: PCB::TFT_LTDC::AF::G)
	{
		gpio.set_AF(i);
	}
	for(auto& i: PCB::TFT_LTDC::AF::B)
	{
		gpio.set_AF(i);
	}
	
	gpio.set_AF(PCB::TFT_LTDC::AF::CLK);
	gpio.set_AF(PCB::TFT_LTDC::AF::DE);
	gpio.set_AF(PCB::TFT_LTDC::AF::HSYNC);
	gpio.set_AF(PCB::TFT_LTDC::AF::VSYNC);
	
	gpio.set_AF(PCB::TFT_BACKLIGHT::AF::PWM);
	
	
	
	
	
	
	
	//	FMC
	for(auto& i: PCB::SDRAM_FMC::D)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	for(auto& i: PCB::SDRAM_FMC::A)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	for(auto& i: PCB::SDRAM_FMC::B)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	for(auto& i: PCB::SDRAM_FMC::DM)
	{
		gpio.init_pin(i, GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	}
	
	gpio.init_pin(PCB::SDRAM_FMC::CLK,		GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::SDRAM_FMC::CLK_EN,	GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::SDRAM_FMC::CS,			GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::SDRAM_FMC::RAS,		GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::SDRAM_FMC::CAS,		GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	gpio.init_pin(PCB::SDRAM_FMC::WE,			GPIO::e_mode::AF_PP, GPIO::e_speed::VERY_HIGH, GPIO::e_pupd::NONE);
	
	
	
	
	
	for(auto& i: PCB::SDRAM_FMC::AF::D)
	{
		gpio.set_AF(i);
	}
	for(auto& i: PCB::SDRAM_FMC::AF::A)
	{
		gpio.set_AF(i);
	}
	for(auto& i: PCB::SDRAM_FMC::AF::B)
	{
		gpio.set_AF(i);
	}
	for(auto& i: PCB::SDRAM_FMC::AF::DM)
	{
		gpio.set_AF(i);
	}
	
	gpio.set_AF(PCB::SDRAM_FMC::AF::CLK);
	gpio.set_AF(PCB::SDRAM_FMC::AF::CLK_EN);
	gpio.set_AF(PCB::SDRAM_FMC::AF::CS);
	gpio.set_AF(PCB::SDRAM_FMC::AF::RAS);
	gpio.set_AF(PCB::SDRAM_FMC::AF::CAS);
	gpio.set_AF(PCB::SDRAM_FMC::AF::WE);
}


feedback Display::init_sdram()
{
	constexpr float t = 1.0 / 120e6;
	
	
	//	Datasheet values from the SDRAM (IS42S32200L-5TL)
	constexpr float sdram_t_RC = 55e-9;
	constexpr float sdram_t_RAS = 38.7e-9;
	constexpr float sdram_t_RP = 15e-9;
	constexpr float sdram_t_RCD = 15e-9;
	constexpr float sdram_t_WR = t + 5e-9;
	constexpr float sdram_t_XSR = 60e-9;
	constexpr float sdram_t_MRD = 2 * t;
	
	
	//	Convert to clock cycles (round up to be safe)
	constexpr uint8 sdram_RC	= 1 + sdram_t_RC	/ t;
	constexpr uint8 sdram_RAS	= 1 + sdram_t_RAS	/ t;
	constexpr uint8 sdram_RP	= 1 + sdram_t_RP	/ t;
	constexpr uint8 sdram_RCD	= 1 + sdram_t_RCD	/ t;
	constexpr uint8 sdram_WR	= 1 + sdram_t_WR	/ t;
	constexpr uint8 sdram_XSR	= 1 + sdram_t_XSR	/ t;
	constexpr uint8 sdram_MRD	= 1 + sdram_t_MRD	/ t;
	
	
	//	Apply formulas from the FMC datasheet
	//	WR >= RAS - RCD
	//	WR >= RC - RCD - RP
	constexpr uint8 RAS_minus_RCD = sdram_RAS - sdram_RCD;
	constexpr uint8 RC_minus_RCD_minus_RP = sdram_RC - sdram_RCD - sdram_RP;
	constexpr uint8 sdram_WR_correct = Math::max(sdram_WR, RAS_minus_RCD, RC_minus_RCD_minus_RP);
	
	
	//	Generate SDRAM configuration
	FMC::s_SDRAM_config sdramConfig;
	sdramConfig.readPipeDelay										= FMC::e_readPipeDelay::_2;
	sdramConfig.readBurst												= true;
	sdramConfig.clockDivision										= FMC::e_clockDivision::DIV_2;
	sdramConfig.writeProtection									= false;
	sdramConfig.CAS_Latency											= FMC::e_CAS_Latency::_3;
	sdramConfig.numberOfInternalBanks						= FMC::e_numberOfInternalBanks::_4;
	sdramConfig.dataBusWidth										= FMC::e_dataBusWidth::_32_BIT;
	sdramConfig.rowAddressBits									= FMC::e_rowAddressBits::_11;
	sdramConfig.columnAddressBits								= FMC::e_columnAddressBits::_8;
	
	sdramConfig.t_RC_rowCycleDelay							= sdram_RC;
	sdramConfig.t_RAS_selfRefreshTime						= sdram_RAS;
	sdramConfig.t_RP_rowPrechargeDelay					= sdram_RP;
	sdramConfig.t_RCD_rowToColumnDelay					= sdram_RCD;
	sdramConfig.t_WR_recoveryDelay							= sdram_WR_correct;
	sdramConfig.t_XSR_exitSelfRefreshDelay			= sdram_XSR;
	sdramConfig.t_MRD_loadModeRegisterToActive	= sdram_MRD;
	
	
	//	Init FMC with the generated configuration
	FMC& fmc = STM32H753BIT6::get().get_fmc();
	return(fmc.init(RCC::e_clockSource_fmc::PLL_2_R, FMC::e_SDRAM_bank::BANK_1, sdramConfig));
}


feedback Display::init_graphics()
{
	//	Newhaven 10.1" NHD-10.1-1024600BF-ASXP-CTP
	I_DisplayDriver::s_displayData displayData;
	displayData.h_active			=  1024;
	displayData.h_backporch		=    60;
	displayData.h_frontporch	=   160;
	displayData.h_syncwidth		=   100;
	
	displayData.v_active			=  600;
	displayData.v_backporch		=   13;
	displayData.v_frontporch	=   12;
	displayData.v_syncwidth		=   10;
	
	displayData.h_total = displayData.h_active + displayData.h_backporch + displayData.h_frontporch + displayData.h_syncwidth;
	displayData.v_total = displayData.v_active + displayData.v_backporch + displayData.v_frontporch + displayData.v_syncwidth;
	
	
	//	Init LTDC
	STM32H753BIT6& stm32h753bit6 = STM32H753BIT6::get();
	LTDC& ltdc = stm32h753bit6.get_ltdc();
	if(ltdc.init(displayData) != OK)
	{
		return(FAIL);
	}
	
	
	//	Framebuffer data
	const Rect frameShape(Vec2(0, 0), Vec2(displayData.h_active, displayData.v_active));
	
	constexpr uint32 sdramSizeInPixel = sdramSizeInBytes / sizeof(Color);
	Color* framebuffer1 = m_sdramBuffer;
	Color* framebuffer2 = m_sdramBuffer + (sdramSizeInPixel / 2);
	
	
	//	Init Graphics
	Graphics& graphics = Graphics::get();
	return(graphics.init(stm32h753bit6.get_dma2d(), ltdc, frameShape, framebuffer1, framebuffer2));
}



/*****************************************************************************/
/*                      						Public	  			 						 						 */
/*****************************************************************************/

Display& Display::get()
{
	static Display display;
	return(display);
}







feedback Display::set_brightness(uint8 percent)
{
	if(percent > 100)
	{
		percent = 100;
	}
	STM32H753BIT6::get().get_timer_12().set_pwm(PCB::TFT_BACKLIGHT::TIMER::PWM_CHANNEL, percent * 10000);
	
	m_brightness = percent;
	return(OK);
}


feedback Display::set_state(bool enable)
{
	GPIO& gpio = STM32H753BIT6::get().get_gpio();
	if(m_displayState != enable)
	{
		if(enable == true)
		{
			gpio.set_pin(PCB::TFT_BACKLIGHT::EN);
		}
		else
		{
			gpio.clear_pin(PCB::TFT_BACKLIGHT::EN);
		}
		m_displayState = enable;
	}
	
	return(OK);
}




