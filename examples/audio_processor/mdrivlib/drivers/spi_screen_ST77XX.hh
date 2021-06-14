#pragma once
#include <cstdint>

struct ST77XX {
	constexpr static uint8_t ST_CMD_DELAY = 0x80;

	constexpr static uint8_t NOP = 0x00;
	constexpr static uint8_t SWRESET = 0x01;
	constexpr static uint8_t RDDID = 0x04;
	constexpr static uint8_t RDDST = 0x09;

	constexpr static uint8_t SLPIN = 0x10;
	constexpr static uint8_t SLPOUT = 0x11;
	constexpr static uint8_t PTLON = 0x12;
	constexpr static uint8_t NORON = 0x13;

	constexpr static uint8_t INVOFF = 0x20;
	constexpr static uint8_t INVON = 0x21;
	constexpr static uint8_t DISPOFF = 0x28;
	constexpr static uint8_t DISPON = 0x29;
	constexpr static uint8_t CASET = 0x2A;
	constexpr static uint8_t RASET = 0x2B;
	constexpr static uint8_t RAMWR = 0x2C;
	constexpr static uint8_t RAMRD = 0x2E;

	constexpr static uint8_t PTLAR = 0x30;
	constexpr static uint8_t TEOFF = 0x34;
	constexpr static uint8_t TEON = 0x35;
	constexpr static uint8_t MADCTL = 0x36;
	constexpr static uint8_t COLMOD = 0x3A;

	constexpr static uint8_t MADCTL_MY = 0x80;
	constexpr static uint8_t MADCTL_MX = 0x40;
	constexpr static uint8_t MADCTL_MV = 0x20;
	constexpr static uint8_t MADCTL_ML = 0x10;
	constexpr static uint8_t MADCTL_RGB = 0x00;

	constexpr static uint8_t RDID1 = 0xDA;
	constexpr static uint8_t RDID2 = 0xDB;
	constexpr static uint8_t RDID3 = 0xDC;
	constexpr static uint8_t RDID4 = 0xDD;

	constexpr static uint16_t BLACK = 0x0000;
	constexpr static uint16_t WHITE = 0xFFFF;
	constexpr static uint16_t RED = 0xF800;
	constexpr static uint16_t GREEN = 0x07E0;
	constexpr static uint16_t BLUE = 0x001F;
	constexpr static uint16_t CYAN = 0x07FF;
	constexpr static uint16_t MAGENTA = 0xF81F;
	constexpr static uint16_t YELLOW = 0xFFE0;
	constexpr static uint16_t ORANGE = 0xFC00;
};

// clang-format off

static const uint8_t generic_st7789[] =  {                
									// Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX::SWRESET,   ST77XX::ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      150,                          //     ~150 ms delay
    ST77XX::SLPOUT ,   ST77XX::ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      10,                           //      10 ms delay
    ST77XX::COLMOD , 1+ST77XX::ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX::MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX::CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      0,        					//     XSTART = 0
      0,
      240,  						//     XEND = 240
    ST77XX::RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      0,             				//     YSTART = 0
      //320>>8,
	  0,
	  240,
      //320&0xFF, 			 		//     YEND = 320
    ST77XX::INVON  ,   ST77XX::ST_CMD_DELAY, //  7: hack
      10,
    ST77XX::NORON  ,   ST77XX::ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX::DISPON ,   ST77XX::ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
      10 
};

// clang-format on

