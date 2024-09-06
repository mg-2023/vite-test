#pragma once

#ifndef _LINE_BUFFER_H_
#define _LINE_BUFFER_H_

// node for containing single characters, double linked list
typedef struct singleChar {
	char ch;
	struct singleChar* prev;
	struct singleChar* next;
} SingleChar;
#endif

SingleChar* deleteChar(SingleChar* curr);
SingleChar* addChar(SingleChar* curr, char c);
