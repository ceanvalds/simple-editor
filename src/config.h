#ifndef __CONFIG__H_
#define __CONFIG__H_

#include <termios.h>

#define UED_VERSION "0.0.1"
#define UED_TAB_STOP 8
#define UED_QUIT_TIMES 2

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};

typedef struct erow {
	int size;
	int rsize;
	char* chars;
	char* render;
} erow;

struct editorConfig {
	int cx, cy;
	int rx;
	int rowoff;
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	erow* row;
	int dirty;
	char* filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct termios orig_termios;
};

#endif
