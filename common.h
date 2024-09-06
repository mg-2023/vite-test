#pragma once

#ifndef _COMMON_H_
#define _COMMON_H_

// common header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ctype.h>

// escape sequence macros
#define RED_COLOR "\x1b[31;1m"
#define INV_COLOR "\x1b[7m"
#define RESET_COLOR "\x1b[0m"

#define CLEAR_SCR "\x1b[2J"
#define CLEAR_LINE { printf("\r"); for(int i=0; i<conW; i++) { printf(" "); } printf("\r"); }

#define MOVE_LINE1 "\x1b[1;1f"
#define MOVE_LINE29 "\x1b[29;1f"
#define MOVE_LINE30 "\x1b[30;1f"
#define MOVE_LINE(n) printf("\x1b[%d;1f", (n))

// flag macros
#define F_NORMAL 0x0
#define F_SAVE_FILE 0x1
#define F_QUIT_WITHOUT_SAVE 0x2
#define F_FIND_TEXT 0x4
#define F_SHOW_LINE 0x8 // TODO: debug mode

	// special key macro
	#ifdef _WIN32
	#define UP 72
	#define DOWN 80
	#define LEFT 75
	#define RIGHT 77
	#define HOME 71
	#define END 79
	#define PGUP 73
	#define PGDN 81
	#else
	#define UP 65
	#define DOWN 66
	#define LEFT 68
	#define RIGHT 67
	#define HOME 72
	#define END 70
	#define PGUP 53
	#define PGDN 54
	#endif
	#ifndef _WIN32
	#define PGUP2 21
	#define PGDN2 4
	#define HOME2 8
	#define END2 5
	#endif

#endif
