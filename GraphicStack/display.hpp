#pragma once
#include "pcb.hpp"
#include "graphics/graphics.hpp"





class Display
{
	public:
		
		static constexpr uint32 displayHeight				= 1024;
		static constexpr uint32 displayWidth				= 600;
		static constexpr uint32 sdramSizeInBytes		= 8 * 1024 * 1024;
		static constexpr uint32 sdramNumberOfBanks	= 4;
		
		
		
	private:
		
		//	Static Member
		static constexpr bool c_sdram_A11_used = false;
		
		static Color m_sdramBuffer[sdramSizeInBytes / sizeof(Color)];
		
		
		//	Non-static Member
		uint8 m_brightness;
		bool m_displayState;
		
		
		//	Constructor and Destructor
		Display();
		Display(const Display& display) = delete;
		~Display();
		
		
		//	Member Functions
		void init_pins();
		feedback init_sdram();
		feedback init_graphics();
		
		
		//	Friends
		
		
		
		
		
	public:
		
		static Display& get();
		
		feedback set_brightness(uint8 percent);
		feedback set_state(bool enable);
		
		constexpr inline bool get_state() const;
		constexpr inline uint8 get_brightness();
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

constexpr inline bool Display::get_state() const
{
	return(m_displayState);
}


constexpr inline uint8 Display::get_brightness()
{
	return(m_brightness);
}