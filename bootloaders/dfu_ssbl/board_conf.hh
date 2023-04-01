#pragma once

// You must select your board:
// #define BOARD_CONF_DK2 to select the MP153 Discovery Board;
// #define BOARD_CONF_OSD32 to select the OSD32 Board;
// or #define BOARD_CONF_PATH "path/to/board_conf.hh" to set the path to a custom board conf file.
// Passing a compile flag -DBOARD_CONF_XXX is the preferred method, but you may also place the #define before any
// #include "board_conf.hh"

#ifdef BOARD_CONF_PATH
#define HEADER_STRING_I(s) #s
#define HEADER_STRING(s) HEADER_STRING_I(s)
#include HEADER_STRING(BOARD_CONF_PATH)
#elif defined(BOARD_CONF_OSD32)
#include "osd32brk_conf.hh"
#elif defined(BOARD_CONF_DK2)
#include "stm32disco_conf.hh"
#else
#error                                                                                                                 \
	"You must select your board: #define BOARD_CONF_DK2 to select the MP153 Discovery Board; #define BOARD_CONF_OSD32 to select the OSD32 Board; or #define BOARD_CONF_PATH \"path/to/board_conf.hh\" to set the path to a custom board conf file. Passing a compile flag -DBOARD_CONF_XXX is the preferred method, but you may also place the #define before any #include \"board_conf.hh\""
#endif
