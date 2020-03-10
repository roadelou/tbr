#include "error.h"

// Documented in error.h
Error error(const int code, const char *msg) {
  Error val;
  val.code = code;
  if (msg == NULL) {
    // stncpy fails on a null pointer, hence this.
    memset(val.msg, 0, sizeof(val.msg));
  } else {
    strncpy(val.msg, msg, ERROR_MSG_SIZE);
  }
  if (code != 0) {
    // To get some basic error log.
    fprintf(stderr, "%s\n", val.msg);
  }
  return val;
}

// Documented in error.h
Error errorf(const int code, const char *format, ...) {
  Error val;
  val.code = code;
  if (format == NULL) {
    // Same as error, the other branch would fail with a NULL pointer.
    memset(val.msg, 0, sizeof(val.msg));
  } else {
    // simple vsnprintf pattern to fill val.msg with the right informations.
    va_list args;
    va_start(args, format);
    vsnprintf(val.msg, ERROR_MSG_SIZE, format, args);
    va_end(args);
  }
  if (code != 0) {
    // To get some error logs
    fprintf(stderr, "%s\n", val.msg);
  }
  return val;
}
