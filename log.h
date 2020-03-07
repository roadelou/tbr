#ifndef LOG_INCLUDED
#define LOG_INCLUDED

#ifndef LOG_MSG_SIZE
#define LOG_MSG_SIZE 128
#endif

#include <stdio.h>
#include <stdarg.h>

enum LOG {
	LOW,	// 0
	MED,	// 1
	HIGH	// 2
};

int loglvl(const enum LOG threshold);
// Priority high
int hlog(const char *format, ...);
// Priority medium
int mlog(const char *format, ...);
// Priority low
int llog(const char *format, ...);

// end of once include header guard
#endif
