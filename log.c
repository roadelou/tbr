#include "log.h"

static enum LOG LEVEL = LOW;

int loglvl(const enum LOG threshold) {
	LEVEL = threshold;
	return 0;
}

int hlog(const char *format, ...) {
	va_list args;
	va_start(args, format);
	char nformat[LOG_MSG_SIZE];
	snprintf(nformat, LOG_MSG_SIZE, "---! %s", format);
	return vprintf(nformat, args);
}


int mlog(const char *format, ...) {
	if (MED >= LEVEL) {	
		va_list args;
		va_start(args, format);
		char nformat[LOG_MSG_SIZE];
		snprintf(nformat, LOG_MSG_SIZE, "---> %s", format);
		return vprintf(nformat, args);
	}
	else {
		return 0;
	}
}


int llog(const char *format, ...) {
	if (LOW == LEVEL) {	
		va_list args;
		va_start(args, format);
		char nformat[LOG_MSG_SIZE];
		snprintf(nformat, LOG_MSG_SIZE, "---  %s", format);
		return vprintf(nformat, args);
	}
	else {
		return 0;
	}
}
