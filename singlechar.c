#include "common.h"
#include "singlechar.h"

SingleChar* deleteChar(SingleChar* curr) {
	SingleChar* temp = NULL;
	// if empty
	if (curr->prev == NULL) {
		return curr;
	}

	// end of line (including newline)
	else if (curr->next == NULL) {
		temp = curr->prev;
		temp->next = NULL;
		free(curr);

		return temp;
	}

	// at the middle
	else {
		temp = curr->prev;

		curr->next->prev = temp;
		temp->next = curr->next;
		free(curr);

		return temp;
	}
}

SingleChar* addChar(SingleChar* curr, char c) {
	SingleChar* newNode = (SingleChar*)malloc(sizeof(SingleChar));
	if (newNode != NULL) {
		newNode->next = NULL;
		newNode->prev = NULL;

		// last char
		if (curr->next == NULL) {
			newNode->prev = curr;
			newNode->ch = c;

			curr->next = newNode;
		}

		// char in the middle
		else {
			newNode->ch = c;
			newNode->next = curr->next;
			newNode->prev = curr;

			curr->next->prev = newNode;
			curr->next = newNode;
		}

		return newNode;
	}

	else {
		fprintf(stderr, "Internal error: Out Of Memory\n");
		exit(1);
	}
}
