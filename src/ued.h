#ifndef __ued__
#define __ued__

#include "config.h"

void editorSetStatusMessage(const char* fmt, ...);
void editorRefreshScreen();
char* editorPrompt(char* prompt);

void die(const char* s);
void disableRawMode();
void enableRawMode();
int editorReadKey();
int getCursorPosition(int* rows, int* cols);
int getWindowSize(int* rows, int* cols);

/*** row operations ***/
int editorRowCxToRx(erow* row, int cx);
void editorUpdateRow(erow* row);
void editorInsertRow(int at, char* s, size_t len);
void editorFreeRow(erow* row);
void editorDelRow(int at);
void editorRowInsertChar(erow* row, int at, int c);
void editorRowAppendString(erow* row, char* s, size_t len);
void editorRowDelChar(erow* row, int at);

/*** editor operations ***/
void editorInsertChar(int c);
void editorInsertNewline();
void editorDelChar();
void editorProcessKeypress(); 

/*** file i/o ***/
char* editorRowsToString(int* buflen);
void editorOpen(char* filename);
void editorSave();

/*** buffer stuff ***/
#define ABUF_INIT {NULL, 0}

/*** append buffer ***/

struct abuf {
	char* b;
	int len;
};

void abAppend(struct abuf* ab, const char* s, int len);
void abFree(struct abuf* ab);

#endif
