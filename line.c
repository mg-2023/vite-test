#include "common.h"
#include "line.h"
#include "singlechar.h"

Line* deleteLine(Line* curr) {
	Line* temp;
	// only 1 line and/or first line
	if (curr->prev == NULL) {
		return curr;
	}

	// last line
	else if (curr->next == NULL) {
		temp = curr->prev;
		temp->next = NULL;
		free(curr);
	}

	// at the middle
	else {
		temp = curr->prev;

		curr->next->prev = temp;
		temp->next = curr->next;
		free(curr);
	}

	return temp;
}

Line* addLine(Line* curr) {
	Line* newNode = (Line*)malloc(sizeof(Line));
	if (newNode != NULL) {
		newNode->len = 0;
		newNode->prev = NULL;
		newNode->next = NULL;

		// last line or only 1 line
		if (curr->next == NULL) {
			newNode->prev = curr;

			curr->next = newNode;
		}

		// next line exists
		else {
			newNode->next = curr->next;
			newNode->prev = curr;

			curr->next->prev = newNode;
			curr->next = newNode;
		}

		return newNode;
	}

	else {
		fprintf(stderr, "Internal error: Out of memory\n");
		exit(1);
	}
}

void destroyLineList(Line* head) {
	Line* temp = NULL;
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
}
