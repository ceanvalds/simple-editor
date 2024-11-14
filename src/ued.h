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

/*** file i/o ***/
char* editorRowsToString(int* buflen);
void editorOpen(char* filename);
void editorSave();

#endif
