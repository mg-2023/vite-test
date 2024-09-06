#include "common.h"
#include "line.h"
#include "singlechar.h"

#pragma warning(disable:4996) // safe use of unsafe functions is guaranteed (e.g. strncpy)

typedef struct {
	int vPos;
	int hPos;
	int startLine;
	int colOffset;
	int totalLine;
	char filename[81];

	SingleChar* charHere;
	Line* lineHere;
} Cursor;

// globals (console width, height)
int conW, conH;

void showBuffer(int startLine, int startOffset, Line* head) {
	int r = 0, c = 0;
	char** buf;
	// dynamically allocate buffer and initialize to 0
	buf = (char**)calloc(conH - 2, sizeof(char*));
	for (int i = 0; i < conH - 2; i++) {
		buf[i] = (char*)calloc(conW + 1, sizeof(char));
	}

	for (int i = 1; i < startLine; i++) {
		head = head->next;
	}

	while (r < conH - 2 && head != NULL) {
		SingleChar* charHere = head->start;
		for (int i = 0; i <= startOffset; i++) {
			charHere = charHere->next;
			if (charHere == NULL || charHere->ch == '\n') {
				goto lineOver;
			}
		}

		for (c = 0; c < conW; c++) {
			if (charHere == NULL || charHere->ch == '\n') {
				break;
			}

			buf[r][c] = charHere->ch;
			charHere = charHere->next;
		}

lineOver:;
		head = head->next;
		r += 1;
	}

	for (; r < conH - 2; r++) {
		buf[r][0] = '~';
	}

	printf(CLEAR_SCR MOVE_LINE1);
	for (int i = 0; i < conH - 2; i++) {
		printf("%s\n", buf[i]);
	}

	for (int i = 0; i < conH - 2; i++) {
		free(buf[i]);
	}
	free(buf);
}

void showInfo(int vPos, int hPos, int flag, int fixed, Cursor* c, Line* head) {
	int realV = vPos, realH = hPos;
	char* spaces = (char*)calloc(conW + 1, sizeof(char));
	// char spaces[conW + 1] = { 0 };
	memset(spaces, ' ', conW);
	realH -= c->colOffset;
	realV -= (c->startLine - 1);

	MOVE_LINE(conH - 1);
	printf(INV_COLOR);
	if (strlen(c->filename) == 0) {
		printf("New file - %d Lines, [%d, %d]", c->totalLine, vPos, hPos);
	}

	else {
		if(fixed) {
			printf("\"%s*\" - %d Lines, [%d, %d]", c->filename, c->totalLine, vPos, hPos);
		}
		
		else {
			printf("\"%s\" - %d Lines, [%d, %d]", c->filename, c->totalLine, vPos, hPos);
		}
	}
	fputs(spaces, stdout);
	free(spaces);

	MOVE_LINE(conH);
	printf(RESET_COLOR);
	CLEAR_LINE;
	if (flag == F_QUIT_WITHOUT_SAVE) {
		printf(RED_COLOR "Warning! All changes after last save will be lost (Press ^Q 1 more time to exit)" RESET_COLOR);
	}

	else if (flag == F_SAVE_FILE) {
		if (strlen(c->filename) == 0) {
			printf("Enter file name (max 80 chars): ");
			do {
				fgets(c->filename, 80, stdin);
			} while (c->filename[0] == '\n');

			size_t len = strlen(c->filename);
			c->filename[(len == 80) ? 80 : len - 1] = 0;
		}

		FILE* f = fopen((const char*)(c->filename), "wt");
		SingleChar* start = head->start->next;
		while (start != NULL) {
			fputc(start->ch, f);
			start = start->next;
		}

		fclose(f);
	}

	else {
		printf("Save: ^S | Quit: ^Q | Search: ^F");
	}

	printf("\x1b[%d;%df", realV, realH);
}

void specialKey(int kbInput, Cursor* c) {
	// left
	if (kbInput == LEFT) {
		// if not SOF
		if (c->charHere->prev != NULL) {
			c->charHere = c->charHere->prev;

			if (c->hPos > 1) {
				c->hPos -= 1;
			}

			else {
				if (c->vPos > 1) {
					c->lineHere = c->lineHere->prev;

					c->vPos -= 1;
					c->hPos = c->lineHere->len + 1;
				}
			}
		}
	}

	// right
	else if (kbInput == RIGHT) {
		// if not EOF
		if (c->charHere->next != NULL) {
			c->charHere = c->charHere->next;

			if (c->hPos <= c->lineHere->len) {
				c->hPos += 1;
			}

			else {
				if (c->lineHere->next != NULL) {
					c->lineHere = c->lineHere->next;

					c->vPos += 1;
					c->hPos = 1;
				}
			}
		}
	}

	// up
	else if (kbInput == UP) {
		if (c->lineHere->prev != NULL) {
			int prevLen = c->lineHere->prev->len;
			if (prevLen < c->hPos) {
				c->hPos = prevLen + 1;
			}

			c->vPos -= 1;
			c->lineHere = c->lineHere->prev;
			c->charHere = c->lineHere->start;

			for (int i = 0; i < c->hPos-1; i++) {
				c->charHere = c->charHere->next;
			}
		}
	}

	// down
	else if (kbInput == DOWN) {
		if (c->lineHere->next != NULL) {
			int nextLen = c->lineHere->next->len;
			if (nextLen < c->hPos) {
				c->hPos = nextLen + 1;
			}

			c->vPos += 1;
			c->lineHere = c->lineHere->next;
			c->charHere = c->lineHere->start;

			for (int i = 0; i < c->hPos-1; i++) {
				c->charHere = c->charHere->next;
			}
		}
	}

	// home, end
	else if(kbInput == HOME) {
		c->charHere = c->lineHere->start;
		c->hPos = 1;
	}

	else if(kbInput == END) {
		c->charHere = c->lineHere->start;
		while(c->charHere->next != NULL && c->charHere->next->ch != '\n') {
			c->charHere = c->charHere->next;
		}

		c->hPos = c->lineHere->len + 1;
	}

	// pgup, pgdn
	else if (kbInput == PGUP) {
		int tmp = 1;
		for (int i = 0; i < conH - 2; i++) {
			if (c->vPos == 1) {
				break;
			}
			c->vPos -= 1;
			c->lineHere = c->lineHere->prev;
		}
		c->charHere = c->lineHere->start;
		for (int i = 1; i < c->hPos; i++) {
			if (c->charHere->next == NULL || c->charHere->next->ch == '\n') {
				break;
			}

			c->charHere = c->charHere->next;
			tmp += 1;
		}

		c->hPos = tmp;
	}

	else if (kbInput == PGDN) {
		int tmp = 1;
		for (int i = 0; i < conH - 2; i++) {
			if (c->vPos == c->totalLine) {
				break;
			}
			c->vPos += 1;
			c->lineHere = c->lineHere->next;
		}
		c->charHere = c->lineHere->start;
		for (int i = 1; i < c->hPos; i++) {
			if (c->charHere->next == NULL || c->charHere->next->ch == '\n') {
				break;
			}

			c->charHere = c->charHere->next;
			tmp += 1;
		}

		c->hPos = tmp;
	}

// especially for macOS which home/end/pgup/pgdn keys don't work
#ifndef _WIN32
	// home, end
	else if(kbInput == HOME2) {
		c->charHere = c->lineHere->start;
		c->hPos = 1;
	}

	else if(kbInput == END2) {
		c->charHere = c->lineHere->start;
		while(c->charHere->next != NULL && c->charHere->next->ch != '\n') {
			c->charHere = c->charHere->next;
		}

		c->hPos = c->lineHere->len + 1;
	}

	// pgup, pgdn
	else if (kbInput == PGUP2) {
		int tmp = 1;
		for (int i = 0; i < conH - 2; i++) {
			if (c->vPos == 1) {
				break;
			}
			c->vPos -= 1;
			c->lineHere = c->lineHere->prev;
		}
		c->charHere = c->lineHere->start;
		for (int i = 1; i < c->hPos; i++) {
			if (c->charHere->next == NULL || c->charHere->next->ch == '\n') {
				break;
			}

			c->charHere = c->charHere->next;
			tmp += 1;
		}

		c->hPos = tmp;
	}

	else if (kbInput == PGDN2) {
		int tmp = 1;
		for (int i = 0; i < conH - 2; i++) {
			if (c->vPos == c->totalLine) {
				break;
			}
			c->vPos += 1;
			c->lineHere = c->lineHere->next;
		}
		c->charHere = c->lineHere->start;
		for (int i = 1; i < c->hPos; i++) {
			if (c->charHere->next == NULL || c->charHere->next->ch == '\n') {
				break;
			}

			c->charHere = c->charHere->next;
			tmp += 1;
		}

		c->hPos = tmp;
	}
#endif
}

// if OS == windows
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <sys/ioctl.h>
#endif

int main(int argc, char** argv) {
	/* Initialize section */
	if (argc >= 3) {
		fprintf(stdout, "You can only open 1 file at a time\n");
		fprintf(stdout, "Usage: Windows - vite [filename], Other OS - ./vite [filename]\n");
		exit(1);
	}

	// contains each line's length
	Line* lineData = (Line*)malloc(sizeof(Line));
	if (lineData == NULL) {
		fprintf(stderr, "Failed to allocate line data, try running this program again?\n");
		exit(1);
	}

	else {
		lineData->len = 0;
		lineData->prev = NULL;
		lineData->next = NULL;
		lineData->start = (SingleChar*)malloc(sizeof(SingleChar));
		if (lineData->start == NULL) {
			fprintf(stderr, "Failed to allocate line buffer data, try running this program again?\n");
			exit(1);
		}
	}

	/* Running section */

	// wipe screen when initializing succeeded
	printf(CLEAR_SCR);

	Cursor c = { 1, 1, 1, 0, 1, {0} };
	int flag = F_NORMAL;
	int fixed = 0;

	// load file when given file name
	if (argc <= 2) {
		c.lineHere = lineData;
		c.charHere = c.lineHere->start;
		c.charHere->ch = 0;
		c.charHere->next = NULL;
		c.charHere->prev = NULL;
		if (argc == 1) {
			goto editor; // new file edit
		}
		strncpy(c.filename, argv[1], 80);

		FILE* f = fopen((const char*)(argv[1]), "rt");
		if (f == NULL) {
			goto editor; // new file edit with given file name
		}

		while (1) {
			int ch = fgetc(f);
			if (ch == EOF) {
				break;
			}
			if (ch == 10) {
				c.charHere = addChar(c.charHere, ch);
				c.lineHere = addLine(c.lineHere);
				c.lineHere->start = c.charHere;
				c.totalLine += 1;
			}

			else {
				c.charHere = addChar(c.charHere, ch);
				c.lineHere->len += 1;
			}
		}

		c.lineHere = lineData;
		c.charHere = lineData->start;
		fclose(f);
	}

editor:;
	// Continuously getting keyboard input
#ifdef _WIN32 /* Windows */
	while (1) {
		// get console size
		CONSOLE_SCREEN_BUFFER_INFO console;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console);
		conH = console.dwSize.Y;
		conW = console.dwSize.X;
		showBuffer(c.startLine, c.colOffset, lineData);
		showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);

		int kbInput = _getch();
		int curLen = c.lineHere->len;

		// backspace
		if (kbInput == 8) {
			if (c.hPos > 1) {
				c.charHere = deleteChar(c.charHere);
				c.hPos -= 1;
				c.lineHere->len -= 1;
			}

			else {
				int tempLen = c.lineHere->len;
				c.lineHere = deleteLine(c.lineHere);

				if (c.vPos > 1) {
					c.charHere = deleteChar(c.charHere);
					c.vPos -= 1;
					c.hPos = (c.lineHere->len) + 1;
					c.lineHere->len += tempLen;
					c.totalLine -= 1;
				}
			}

			flag = F_NORMAL;
		}

		// enter
		else if (kbInput == 13) {
			int temp = c.lineHere->len - (c.hPos - 1);

			c.lineHere->len = (c.hPos - 1);
			c.lineHere = addLine(c.lineHere);

			c.charHere = addChar(c.charHere, '\n');
			c.lineHere->len = temp;
			c.lineHere->start = c.charHere;

			c.hPos = 1;
			c.vPos += 1;
			c.totalLine += 1;

			flag = F_NORMAL;
		}

		// ^Q
		else if (kbInput == 17) {
			if (fixed) {
				if (flag != F_QUIT_WITHOUT_SAVE) {
					flag = F_QUIT_WITHOUT_SAVE;
				}

				else {
					break;
				}
			}

			else {
				break;
			}
		}

		// ^S
		else if (kbInput == 19) {
			flag = F_SAVE_FILE;
			showBuffer(c.startLine, c.colOffset, lineData);
			showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);
			fixed = 0;
			flag = F_NORMAL;

			showBuffer(c.startLine, c.colOffset, lineData);
			showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);
			continue;
		}

		// special key
		else if (kbInput == 0 || kbInput == 224) {
			kbInput = _getch();
			specialKey(kbInput, &c);
			flag = F_NORMAL;
		}

		// any other key
		else {
			c.charHere = addChar(c.charHere, kbInput);
			c.lineHere->len += 1;
			c.hPos += 1;
			flag = F_NORMAL;
		}

		c.startLine = (c.vPos - 1) / (conH - 2) * (conH - 2) + 1;
		c.colOffset = (c.hPos - 1) / conW * conW;
		fixed = 1;
	}
#else /* unix based OS */
	int kbInput;
	int fd;
	struct winsize ws;

	// disable canonical input and echo input (working like windows' _getch())
	system("stty -icanon -echo");

	// disable ^S ^Q terminal function
	system("stty stop \"\"");
	system("stty start \"\"");
	while(1) {
		// getting console width and height
		// from: https://blog.naver.com/cjw8349/20168219472
		fd = fileno(stdin);
		ioctl(fd, TIOCGWINSZ, &ws);
		conH = ws.ws_row;
		conW = ws.ws_col;
		showBuffer(c.startLine, c.colOffset, lineData);
		showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);

		kbInput = getchar();

		// another special key especially for macOS which home/end/pgup/pgdn keys don't work
		if(kbInput == HOME2 || kbInput == END2 || kbInput == PGUP2 || kbInput == PGDN2) {
			specialKey(kbInput, &c);
		}

		// enter
		else if(kbInput == 10) {
			int temp = c.lineHere->len - (c.hPos - 1);

			c.lineHere->len = (c.hPos - 1);
			c.lineHere = addLine(c.lineHere);

			c.charHere = addChar(c.charHere, '\n');
			c.lineHere->len = temp;
			c.lineHere->start = c.charHere;

			c.hPos = 1;
			c.vPos += 1;
			c.totalLine += 1;

			flag = F_NORMAL;
		}

		// ^Q
		else if (kbInput == 17) {
			if (fixed) {
				if (flag != F_QUIT_WITHOUT_SAVE) {
					flag = F_QUIT_WITHOUT_SAVE;
				}

				else {
					break;
				}
			}

			else {
				break;
			}
		}

		// ^S
		else if (kbInput == 19) {
			system("stty icanon echo");
			
			flag = F_SAVE_FILE;
			showBuffer(c.startLine, c.colOffset, lineData);
			showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);
			fixed = 0;
			flag = F_NORMAL;

			system("stty -icanon -echo");

			showBuffer(c.startLine, c.colOffset, lineData);
			showInfo(c.vPos, c.hPos, flag, fixed, &c, lineData);
			continue;
		}

		// special keys
		else if(kbInput == 27) {
			getchar();
			kbInput = getchar();
			specialKey(kbInput, &c);
			// pgup, pgdn generates 4 input sequence
			if (kbInput == PGUP || kbInput == PGDN) {
				getchar();
			}

			flag = F_NORMAL;
		}

		// backspace
		else if(kbInput == 127) {
			if (c.hPos > 1) {
				c.charHere = deleteChar(c.charHere);
				c.hPos -= 1;
				c.lineHere->len -= 1;
			}

			else {
				int tempLen = c.lineHere->len;
				c.lineHere = deleteLine(c.lineHere);

				if (c.vPos > 1) {
					c.charHere = deleteChar(c.charHere);
					c.vPos -= 1;
					c.hPos = (c.lineHere->len) + 1;
					c.lineHere->len += tempLen;
					c.totalLine -= 1;
				}
			}

			flag = F_NORMAL;
		}

		// any other key
		else {
			c.charHere = addChar(c.charHere, kbInput);
			c.lineHere->len += 1;
			c.hPos += 1;
			flag = F_NORMAL;
		}
		
		c.startLine = (c.vPos - 1) / (conH - 2) * (conH - 2) + 1;
		c.colOffset = (c.hPos - 1) / conW * conW;
		fixed = 1;
	}

	// reset console environment/attribute
	system("stty stop \"^S\"");
	system("stty start \"^Q\"");
	system("stty icanon echo");
#endif

	printf(CLEAR_SCR MOVE_LINE1);
	destroyLineList(lineData);
	return 0;

}

