#ifndef LOG_INCLUDED
#define LOG_INCLUDED

#ifndef LOG_MSG_SIZE
#define LOG_MSG_SIZE 128
#endif

#include <stdarg.h> // for variadic functions support
#include <stdio.h>  // for vprintf

// An enum that represents the wished for verbosity of the program.
enum LOG {
  LOW, // 0
  MED, // 1
  HIGH // 2
};

// Sets the verbosity of the current program.
// This can be used several times, but since it relies on a static values,
// you cannot have several different log levels at the same time.
int loglvl(const enum LOG threshold);

// The three following functions are log-level-filtered versions of printf.

// Priority high
int hlog(const char *format, ...);
// Priority medium
int mlog(const char *format, ...);
// Priority low
int llog(const char *format, ...);

// end of once include header guard
#endif
