#ifndef ERROR_INCLUDED
#define ERROR_INCLUDED

// Maximum size of error messages, can be overriden at compile time
#ifndef ERROR_MSG_SIZE
#define ERROR_MSG_SIZE 128
#endif

#include <stdarg.h> // for vsnprintf
#include <stdio.h>  // to show error stack
#include <string.h> // for string manipulations

typedef struct Error Error;

// The error struct, just an error code with a message.
// One important convention is that should the error code be the 0,
// The program should not try to read the value in msg (it is likely NULL).
struct Error {
  char code;
  char msg[ERROR_MSG_SIZE];
};

// What error does is fairly simple to guess. Just note that if msg is too long
// for the 128 char buffer the message will be cut.
extern Error error(const int code, const char *msg);
// Same as error but supports format strings (thus almost completely
// superseeding error.
extern Error errorf(const int code, const char *format, ...);

#endif
