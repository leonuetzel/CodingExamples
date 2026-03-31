#pragma once

#include "stm32h753bit6.hpp"





namespace PCB
{
	namespace TFT_LTDC
	{
		constexpr MCU::PIN R[8] =
		{
			MCU::PIN::I15,
			MCU::PIN::J0,
			MCU::PIN::J1,
			MCU::PIN::J2,
			MCU::PIN::J3,
			MCU::PIN::J4,
			MCU::PIN::J5,
			MCU::PIN::J6
		};
		
		constexpr MCU::PIN G[8] =
		{
			MCU::PIN::J7,
			MCU::PIN::J8,
			MCU::PIN::J9,
			MCU::PIN::J10,
			MCU::PIN::J11,
			MCU::PIN::K0,
			MCU::PIN::K1,
			MCU::PIN::K2
		};
		
		constexpr MCU::PIN B[8] =
		{
			MCU::PIN::G14,
			MCU::PIN::J13,
			MCU::PIN::J14,
			MCU::PIN::J15,
			MCU::PIN::K3,
			MCU::PIN::K4,
			MCU::PIN::K5,
			MCU::PIN::K6
		};
		
		constexpr MCU::PIN CLK					= MCU::PIN::I14;
		constexpr MCU::PIN DE						= MCU::PIN::K7;
		constexpr MCU::PIN HSYNC				= MCU::PIN::I12;
		constexpr MCU::PIN VSYNC				= MCU::PIN::I13;

		
		namespace AF
		{
			constexpr MCU::AF R[8] =
			{
				MCU::AF::I15_LCDR0,
				MCU::AF::J0_LCDR1,
				MCU::AF::J1_LCDR2,
				MCU::AF::J2_LCDR3,
				MCU::AF::J3_LCDR4,
				MCU::AF::J4_LCDR5,
				MCU::AF::J5_LCDR6,
				MCU::AF::J6_LCDR7
			};
			
			constexpr MCU::AF G[8] =
			{
				MCU::AF::J7_LCDG0,
				MCU::AF::J8_LCDG1,
				MCU::AF::J9_LCDG2,
				MCU::AF::J10_LCDG3,
				MCU::AF::J11_LCDG4,
				MCU::AF::K0_LCDG5,
				MCU::AF::K1_LCDG6,
				MCU::AF::K2_LCDG7
			};
			
			constexpr MCU::AF B[8] =
			{
				MCU::AF::G14_LCDB0,
				MCU::AF::J13_LCDB1,
				MCU::AF::J14_LCDB2,
				MCU::AF::J15_LCDB3,
				MCU::AF::K3_LCDB4,
				MCU::AF::K4_LCDB5,
				MCU::AF::K5_LCDB6,
				MCU::AF::K6_LCDB7
			};
			
			constexpr MCU::AF DE				= MCU::AF::K7_LCDDE;
			constexpr MCU::AF CLK				= MCU::AF::I14_LCDCLK;
			constexpr MCU::AF HSYNC			= MCU::AF::I12_LCDHSYNC;
			constexpr MCU::AF VSYNC			= MCU::AF::I13_LCDVSYNC;
		};
	};
	
	namespace TFT_BACKLIGHT
	{
		constexpr MCU::PIN EN					= MCU::PIN::H7;
		constexpr MCU::PIN PWM					= MCU::PIN::H6;
		
		
		
		namespace AF
		{
			constexpr MCU::AF PWM				= MCU::AF::H6_TIMER12CH1;
		};
		
		namespace TIMER
		{
			constexpr Timer_12::e_channel	PWM_CHANNEL		= Timer_12::e_channel::CHANNEL_1;
			constexpr uint32							PWM_FREQUENCY	= 100000;
		};
	};
	
	namespace SDRAM_FMC
	{
		constexpr MCU::PIN D[32] =
		{
			MCU::PIN::D14,
			MCU::PIN::D15,
			MCU::PIN::D0,
			MCU::PIN::D1,
			MCU::PIN::E7,
			MCU::PIN::E8,
			MCU::PIN::E9,
			MCU::PIN::E10,
			MCU::PIN::E11,
			MCU::PIN::E12,
			MCU::PIN::E13,
			MCU::PIN::E14,
			MCU::PIN::E15,
			MCU::PIN::D8,
			MCU::PIN::D9,
			MCU::PIN::D10,
			MCU::PIN::H8,
			MCU::PIN::H9,
			MCU::PIN::H10,
			MCU::PIN::H11,
			MCU::PIN::H12,
			MCU::PIN::H13,
			MCU::PIN::H14,
			MCU::PIN::H15,
			MCU::PIN::I0,
			MCU::PIN::I1,
			MCU::PIN::I2,
			MCU::PIN::I3,
			MCU::PIN::I6,
			MCU::PIN::I7,
			MCU::PIN::I9,
			MCU::PIN::I10
		};
		
		constexpr MCU::PIN A[12] =
		{
			MCU::PIN::F0,
			MCU::PIN::F1,
			MCU::PIN::F2,
			MCU::PIN::F3,
			MCU::PIN::F4,
			MCU::PIN::F5,
			MCU::PIN::F12,
			MCU::PIN::F13,
			MCU::PIN::F14,
			MCU::PIN::F15,
			MCU::PIN::G0,
			MCU::PIN::G1
		};
		
		constexpr MCU::PIN B[2] =
		{
			MCU::PIN::G4,
			MCU::PIN::G5
		};
		
		constexpr MCU::PIN DM[4] =
		{
			MCU::PIN::E0,
			MCU::PIN::E1,
			MCU::PIN::I4,
			MCU::PIN::I5
		};
		
		constexpr MCU::PIN CLK					= MCU::PIN::G8;
		constexpr MCU::PIN CLK_EN			= MCU::PIN::C5;
		constexpr MCU::PIN CS					= MCU::PIN::C4;
		constexpr MCU::PIN RAS					= MCU::PIN::F11;
		constexpr MCU::PIN CAS					= MCU::PIN::G15;
		constexpr MCU::PIN WE					= MCU::PIN::H5;
		
		
		
		namespace AF
		{
			constexpr MCU::AF D[32] =
			{
				MCU::AF::D14_FMCD0_FMCDA0,
				MCU::AF::D15_FMCD1_FMCDA1,
				MCU::AF::D0_FMCD2_FMCDA2,
				MCU::AF::D1_FMCD3_FMCDA3,
				MCU::AF::E7_FMCD4_FMCDA4,
				MCU::AF::E8_FMCD5_FMCDA5,
				MCU::AF::E9_FMCD6_FMCDA6,
				MCU::AF::E10_FMCD7_FMCDA7,
				MCU::AF::E11_FMCD8_FMCDA8,
				MCU::AF::E12_FMCD9_FMCDA9,
				MCU::AF::E13_FMCD10_FMCDA10,
				MCU::AF::E14_FMCD11_FMCDA11,
				MCU::AF::E15_FMCD12_FMCDA12,
				MCU::AF::D8_FMCD13_FMCDA13,
				MCU::AF::D9_FMCD14_FMCDA14,
				MCU::AF::D10_FMCD15_FMCDA15,
				MCU::AF::H8_FMCD16,
				MCU::AF::H9_FMCD17,
				MCU::AF::H10_FMCD18,
				MCU::AF::H11_FMCD19,
				MCU::AF::H12_FMCD20,
				MCU::AF::H13_FMCD21,
				MCU::AF::H14_FMCD22,
				MCU::AF::H15_FMCD23,
				MCU::AF::I0_FMCD24,
				MCU::AF::I1_FMCD25,
				MCU::AF::I2_FMCD26,
				MCU::AF::I3_FMCD27,
				MCU::AF::I6_FMCD28,
				MCU::AF::I7_FMCD29,
				MCU::AF::I9_FMCD30,
				MCU::AF::I10_FMCD31
			};
			
			constexpr MCU::AF A[12] =
			{
				MCU::AF::F0_FMCA0,
				MCU::AF::F1_FMCA1,
				MCU::AF::F2_FMCA2,
				MCU::AF::F3_FMCA3,
				MCU::AF::F4_FMCA4,
				MCU::AF::F5_FMCA5,
				MCU::AF::F12_FMCA6,
				MCU::AF::F13_FMCA7,
				MCU::AF::F14_FMCA8,
				MCU::AF::F15_FMCA9,
				MCU::AF::G0_FMCA10,
				MCU::AF::G1_FMCA11
			};
			
			constexpr MCU::AF B[2] =
			{
				MCU::AF::G4_FMCA14_FMCBA0,
				MCU::AF::G5_FMCA15_FMCBA1
			};
			
			constexpr MCU::AF DM[4] =
			{
				MCU::AF::E0_FMCNBL0,
				MCU::AF::E1_FMCNBL1,
				MCU::AF::I4_FMCNBL2,
				MCU::AF::I5_FMCNBL3
			};
			
			constexpr MCU::AF CLK				= MCU::AF::G8_FMCSDCLK;
			constexpr MCU::AF CLK_EN			= MCU::AF::C5_FMCSDCKE0;
			constexpr MCU::AF CS					= MCU::AF::C4_FMCSDNE0;
			constexpr MCU::AF RAS				= MCU::AF::F11_FMCSDNRAS;
			constexpr MCU::AF CAS				= MCU::AF::G15_FMCSDNCAS;
			constexpr MCU::AF WE					= MCU::AF::H5_FMCSDNWE;
		};
	};
	
	namespace XTAL
	{
		constexpr MCU::PIN IN					= MCU::PIN::H0; 
		constexpr MCU::PIN OUT					= MCU::PIN::H1;
	};
	
	namespace SWD
	{
		constexpr MCU::PIN DIO					= MCU::PIN::A13;
		constexpr MCU::PIN CLK					= MCU::PIN::A14;
	};
};