#include "../Inc/gui.hpp"
#include "../Inc/display.hpp"



/*****************************************************************************/
/*                    Globals and Static Initialization					 						 */
/*****************************************************************************/





/*****************************************************************************/
/*                      						Private	  			 						 						 */
/*****************************************************************************/

GUI::GUI()
{
	//	Library references
	Graphics& graphics = Graphics::get();
	
	
	//	Read display size and layer information
	const Vec2 displayDimensions = graphics.get_displayDimensions();
	
	
	//	Loading animation at startup
	{
		constexpr int16 loadingStartupAnimationSize = 300;
		constexpr Vec2 loadingStartupSize(loadingStartupAnimationSize, loadingStartupAnimationSize);
		Loading loadingStartup
		(
			Element
			(
				Rect((displayDimensions - loadingStartupSize) / 2, loadingStartupSize),
				0,
				0,
				nullptr
			),
			Fonts::CalibriLight40,
			Colors::white,
			Colors::transparent,
			Colors::transparent,
			colorFunction,
			8,
			100,
			20,
			12
		);
		uint8 progress = 0;
		loadingStartup.set_progressInformation(&progress);
		graphics += loadingStartup;
		
		
		CMOS& cmos = CMOS::get();
		for(uint32 i = 0; i <= 100; i++)
		{
			progress = i;
			cmos.sleep_ms(20);
		}
		cmos.sleep_ms(250);
		graphics -= loadingStartup;
	}
	
	
	//	Clock
	{
		constexpr Vec2 elementClockSize(700, 140);
		Element* elementClock = new Element
		(
			Rect((displayDimensions - elementClockSize) / 2, elementClockSize),
			0,
			1,
			onUpdate_elementClock
		);
		graphics += elementClock;
	}
	
	
	//	Another loading animation to show that the display is still responsive
	{
		constexpr int16 loadingAnimationSize = 150;
		constexpr Vec2 loadingSize(loadingAnimationSize, loadingAnimationSize);
		Loading* loading = new Loading
		(
			Element
			(
				Rect(Vec2((displayDimensions.x - loadingSize.x) / 2, 430), loadingSize),
				0,
				0,
				nullptr
			),
			Fonts::CalibriLight40,
			Colors::white,
			Colors::transparent,
			Colors::transparent,
			colorFunction,
			8,
			65,
			20,
			7
		);
		graphics += loading;
	}
	
	
	//	Diagnostic textbox
	{
		constexpr Vec2 textboxDiagnosticSize(200, 200);
		Textbox* textboxDiagnostic = new Textbox
		(
			Element
			(
				Rect(Vec2(0, displayDimensions.y - textboxDiagnosticSize.y), textboxDiagnosticSize),
				0,
				20,
				onUpdate_textboxDiagnostic
			),
			Fonts::CalibriLight16
		);
		textboxDiagnostic->set_numberOfLines(6);
		graphics += textboxDiagnostic;
	}
}


GUI::~GUI()
{
	
}


Color GUI::colorFunction(Vec2 position, Vec2 size)
{
	Color color = Colors::black;
	color.red = (255 * position.y) / size.y;
	color.green = 255 - color.red;
	color.blue = 255 - ((255 * position.x) / size.x);
	return(color);
}


void GUI::onUpdate_elementClock(Element& element)
{
	CMOS& cmos = CMOS::get();
	static Time time_old	= cmos.get_time();
	const Time time_now		= cmos.get_time();
	
	if(time_now.second != time_old.second)
	{
		element.clear();
		element.draw_string(String((uint32) (time_now.hour + (time_now.day - 1) * 24), 2) + ":" + String(time_now.minute, 2) + ":" + String(time_now.second, 2), Element::e_align::CENTER, Fonts::CalibriLight128, Colors::white);
		time_old = time_now;
	}
}


void GUI::onUpdate_textboxDiagnostic(Element& element)
{
	Textbox& textbox = (Textbox&) element;
	
	CMOS& cmos = CMOS::get();
	Graphics& graphics = Graphics::get();
	I_DisplayDriver& displayDriver = STM32H753BIT6::get().get_ltdc();
	
	textbox.set_text(String("FPS HW: ")			+ displayDriver.get_fps()													, 0);
	textbox.set_text(String("FPS SW: ")			+ graphics.get_fps()															, 1);
	textbox.set_text(String("FIFO UR: ")		+ displayDriver.get_fifoUnderrunCounter()					, 2);
	textbox.set_text(String("FIFO TE: ")		+ displayDriver.get_fifoTransmitErrorCounter()		, 3);
	textbox.set_text(String("CPU Load: ")		+ String((cmos.getLoadCpuPercent()), 2) + "%"			, 4);
	textbox.set_text(String("Heap Usage: ")	+ String((cmos.getLoadHeapPercent()), 2) + "%"		, 5);
}



/*****************************************************************************/
/*                      						Public	  			 						 						 */
/*****************************************************************************/


GUI& GUI::get()
{
	static GUI gui;
	return(gui);
}
