#include "genericLogger.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

static void localLogger(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
typedef struct localStruct {
  int i;
} localStruct_t;

int main() {
  genericLogger_t *loggerp;
  localStruct_t    localStruct;

  /* With no logger allocated - it still work - and defaults to the WARN level */
  GENERICLOGGER_WARN (NULL, "Single message");
  GENERICLOGGER_WARNF(NULL, "%s %s", "Formatted", "Message");

  /* With a custom logger */
  loggerp = genericLogger_newp(NULL, NULL, GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    GENERICLOGGER_TRACE (loggerp, "Single message");
    GENERICLOGGER_TRACEF(loggerp, "%s %s", "Formatted", "Message");
    genericLogger_freev(&loggerp);
  } else {
    fprintf(stderr, "Failure to allocate a logger, %s\n", strerror(errno));
  }

  /* With a callback */
  loggerp = genericLogger_newp(localLogger, &localStruct, GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    localStruct.i = 10;
    GENERICLOGGER_TRACE (loggerp, "Single message");
    GENERICLOGGER_TRACEF(loggerp, "%s %s", "Formatted", "Message");
    genericLogger_freev(&loggerp);
  } else {
    fprintf(stderr, "Failure to allocate a logger, %s\n", strerror(errno));
  }
  
  return 0;
}

static void localLogger(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  localStruct_t *localStructp = (localStruct_t *) userDatavp;

  fprintf(stderr, "I am the local logger, localStruct.i = %d, msgs = %s\n", localStructp->i, msgs);
}
