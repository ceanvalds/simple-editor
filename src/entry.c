#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "ued.h"
#include "config.h"

struct editorConfig E;

void editorSetStatusMessage(const char* fmt, ...);
void editorRefreshScreen();
char* editorPrompt(char* prompt);

void die(const char* s)
{
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	perror(s);
	exit(1);
}

void disableRawMode()
{
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
		die("tcsetattr");
	}
}

void enableRawMode()
{
	if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) { die("tcgetattr"); }
	atexit(disableRawMode);

	struct termios raw = E.orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) { die("tcsetattr"); }
}

/*** init ***/

void initEditor()
{
	E.cx = 0;
	E.cy = 0;
	E.rx = 0;
	E.rowoff = 0;
	E.coloff = 0;
	E.numrows = 0;
	E.row = NULL;
	E.dirty = 0;
	E.filename = NULL;
	E.statusmsg[0] = '\0';
	E.statusmsg_time = 0;

	if(getWindowSize(&E.screenrows, &E.screencols) == -1) { die("getWindowSize"); }
	E.screenrows -= 2;
}

int main(int argc, char* argv[])
{
	enableRawMode();
	initEditor();
	if(argc >= 2) {
		editorOpen(argv[1]);
	}

	editorSetStatusMessage("ued v%s. press ctrl + h for help", UED_VERSION);

	while(1) {
		editorRefreshScreen();
		editorProcessKeypress();
	}

	return 0;
}
