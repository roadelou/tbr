#include "log.h"

// The static value used to set the program wide log level.
static enum LOG LEVEL = LOW;

// self explanatory, is it not ?
int loglvl(const enum LOG threshold) {
  LEVEL = threshold;
  return 0;
}

// Hig priority printf.
// Notice that because there is no log level above HIGH the message will always
// be displayed, which is the reason why there are no branches in this routine.
int hlog(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char nformat[LOG_MSG_SIZE];
  snprintf(nformat, LOG_MSG_SIZE, "---! %s", format);
  return vprintf(nformat, args);
}

// printf for medium priority message, only displays if LEVEL is LOW or MED
int mlog(const char *format, ...) {
  if (MED >= LEVEL) {
    va_list args;
    va_start(args, format);
    char nformat[LOG_MSG_SIZE];
    snprintf(nformat, LOG_MSG_SIZE, "---> %s", format);
    return vprintf(nformat, args);
  } else {
    return 0;
  }
}

// printf for low priority informations.
// Note that because LOW is the lowest possible priority, we only display the
// message if LEVEL is low.
int llog(const char *format, ...) {
  if (LOW == LEVEL) {
    va_list args;
    va_start(args, format);
    char nformat[LOG_MSG_SIZE];
    snprintf(nformat, LOG_MSG_SIZE, "---  %s", format);
    return vprintf(nformat, args);
  } else {
    return 0;
  }
}
