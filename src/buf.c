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

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf* ab, const char* s, int len)
{
	char* new = realloc(ab->b, ab->len + len);

	if(new == NULL) { return; }
	memcpy(&new[ab->len], s, len);
	ab->b = new;
	ab->len += len;
}

void abFree(struct abuf* ab)
{
	free(ab->b);
}

