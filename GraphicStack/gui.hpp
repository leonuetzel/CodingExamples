#pragma once
#include "cmos.hpp"
#include "graphics/graphics.hpp"



class GUI
{
	public:
		
		
		
		
		
		
		
	private:
		
		//	Static Member
		
		
		//	Non-static Member
		
		
		//	Constructor and Destructor
		GUI();
		GUI(const GUI& gui) = delete;
		~GUI();
		
		
		//	Member Functions
		static void onUpdate_elementClock(Element& element);
		static void onUpdate_textboxDiagnostic(Element& element);
		
		static Color colorFunction(Vec2 position, Vec2 size);
		
		
		//	Friends
		
		
		
		
		
	public:
		
		static GUI& get();
};



/*****************************************************************************/
/*                    Globals and Static Initialization					 						 */
/*****************************************************************************/





/*****************************************************************************/
/*                      						Private	  			 						 						 */
/*****************************************************************************/





/*****************************************************************************/
/*                      					Protected	  			 						 						 */
/*****************************************************************************/





/*****************************************************************************/
/*                      						Public	  			 						 						 */
/*****************************************************************************/
