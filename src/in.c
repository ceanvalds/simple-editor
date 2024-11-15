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

extern struct editorConfig E;

/*** input ***/

char* editorPrompt(char* prompt)
{
	size_t bufsize = 128;
	char* buf = malloc(bufsize);

	size_t buflen = 0;
	buf[0] = '\0';

	while(1) {
		editorSetStatusMessage(prompt, buf);
		editorRefreshScreen();

		int c = editorReadKey();
		if(c == '\x1b') {
			editorSetStatusMessage("");
			free(buf);
			return NULL;
		} else if(c == '\r') {
			if(buflen != 0) {
				editorSetStatusMessage("");
				return buf;
			}
		} else if(!iscntrl(c) && c < 128) {
			if(buflen == bufsize - 1) {
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
	}
}

void editorMoveCursor(int key)
{
	erow* row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

	switch(key) {
	case ARROW_LEFT:
		if(E.cx != 0) {
			E.cx--;
		} else if(E.cy > 0) {
			E.cy--;
			E.cx = E.row[E.cy].size;
		}
		break;
	case ARROW_RIGHT:
		if(row && E.cx < row->size) {
			E.cx++;
		} else if(row && E.cx == row->size) {
			E.cy++;
			E.cx = 0;
		}
		break;
	case ARROW_UP:
		if(E.cy != 0) {
			E.cy--;
		}
		break;
	case ARROW_DOWN:
		if(E.cy < E.numrows) {
			E.cy++;
		}
		break;
	}

	row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	int rowlen = row ? row->size : 0;
	if(E.cx > rowlen) {
		E.cx = rowlen;
	}
}


void editorProcessCommand(char *command) {
    if (strcmp(command, "q") == 0) { 
        if (E.dirty) {
            editorSetStatusMessage("no write since last change! use :q! to override");
        } else {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
        }
    } else if (strcmp(command, "w") == 0) { 
        editorSave();
    } else if (strcmp(command, "q!") == 0) {
        system("clear");
        exit(0);
    } else if (strcmp(command, "wq") == 0) { 
        editorSave();
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else {
        editorSetStatusMessage("not an editor command: %s", command);
    }
}


void editorProcessKeypress() {
    static int quit_times = UED_QUIT_TIMES;
    static char command_buffer[100];
    static int command_buffer_len = 0;
    static int in_command_mode = 0;

    int c = editorReadKey();

    if (in_command_mode) {
        switch (c) {
            case '\r':
                editorProcessCommand(command_buffer);
                in_command_mode = 0;  
                command_buffer_len = 0;  
                memset(command_buffer, 0, sizeof(command_buffer));  
                editorSetStatusMessage("ued v%s. press ctrl + h for help", UED_VERSION);
                break;

            case BACKSPACE:
                if (command_buffer_len > 0) {
                    command_buffer_len--;
                    command_buffer[command_buffer_len] = '\0';  
                    editorSetStatusMessage("command: %s", command_buffer);
                }
                break;

            default:
                if (command_buffer_len < sizeof(command_buffer) - 1) {  
                    command_buffer[command_buffer_len++] = c;
                    command_buffer[command_buffer_len] = '\0';  
                    editorSetStatusMessage("command: %s", command_buffer);
                }
                break;
        }
        return;  
    }

    switch (c) {
        case '\r':
            editorInsertNewline();
            break;

        case CTRL_KEY('q'):
            if (E.dirty && quit_times > 0) {
                editorSetStatusMessage("no write since last change! "
                                       "press ctrl+q %d more times to quit without saving", quit_times);
                quit_times--;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);

        case CTRL_KEY('s'):
            editorSave();
            break;

        case CTRL_KEY('h'):
            editorSetStatusMessage("help: ctrl+s : save | ctrl+q : quit | ctrl+x : save and quit");
            break;

        case CTRL_KEY('x'):
            editorSave();
            system("clear");
            exit(0);

        case ':': 
            in_command_mode = 1;
            editorSetStatusMessage("command: ");
            break;

        case HOME_KEY:
            E.cx = 0;
            break;

        case END_KEY:
            if (E.cy < E.numrows) {
                E.cx = E.row[E.cy].size;
            }
            break;

        case BACKSPACE:
        case DEL_KEY:
            if (c == DEL_KEY) { editorMoveCursor(ARROW_RIGHT); }
            editorDelChar();
            break;

        case PAGE_UP:
        case PAGE_DOWN: {
            if (c == PAGE_UP) {
                E.cy = E.rowoff;
            } else if (c == PAGE_DOWN) {
                E.cy = E.rowoff + E.screenrows - 1;
                if (E.cy > E.numrows) { E.cy = E.numrows; }
            }

            int times = E.screenrows;
            while (times--) {
                editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
        }
        break;

        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editorMoveCursor(c);
            break;

        case CTRL_KEY('l'):
        case '\x1b':
            break;

        default:
            editorInsertChar(c); 
            break;
    }

    quit_times = UED_QUIT_TIMES; 
}

