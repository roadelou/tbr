#include "error.h"

Error error(const int code, const char *msg) {
  Error val;
  val.code = code;
  if (msg == NULL) {
    memset(val.msg, 0, sizeof(val.msg));
  } else {
    strncpy(val.msg, msg, ERROR_MSG_SIZE);
  }
  if (code != 0) {
    fprintf(stderr, "%s\n", val.msg);
  }
  return val;
}

Error errorf(const int code, const char *format, ...) {
  Error val;
  val.code = code;
  if (format == NULL) {
    memset(val.msg, 0, sizeof(val.msg));
  } else {
    va_list args;
    va_start(args, format);
    vsnprintf(val.msg, ERROR_MSG_SIZE, format, args);
    va_end(args);
  }
  if (code != 0) {
    fprintf(stderr, "%s\n", val.msg);
  }
  return val;
}
