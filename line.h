#pragma once
#include "singlechar.h"

#ifndef _LINELENGTH_H_
#define _LINELENGTH_H_

// node for containing line length, double linked list
typedef struct line {
	int len;
	struct line* prev;
	struct line* next;
	SingleChar* start;
} Line;
#endif

Line* deleteLine(Line* curr);
Line* addLine(Line* curr);
void destroyLineList(Line* head);
