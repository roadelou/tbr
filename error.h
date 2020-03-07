#ifndef ERROR_INCLUDED
#define ERROR_INCLUDED

// Maximum size of error messages, can be overriden at compile time
#ifndef ERROR_MSG_SIZE
#define ERROR_MSG_SIZE 128
#endif

#include <stdarg.h> // for vsnprintf
#include <stdio.h>  // to get error stack
#include <string.h>

typedef struct Error Error;

struct Error {
  char code;
  char msg[ERROR_MSG_SIZE];
};

extern Error error(const int code, const char *msg);
extern Error errorf(const int code, const char *format, ...);

#endif
