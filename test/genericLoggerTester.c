#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "genericLogger.h"

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

  /* With a wanted level, no user-level implementation */
  loggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    GENERICLOGGER_TRACE (loggerp, "Single message");
    GENERICLOGGER_TRACEF(loggerp, "%s %s", "Formatted", "Message");
    GENERICLOGGER_FREE(loggerp);
  }

  /* With a user-level implementation */
  loggerp = GENERICLOGGER_CUSTOM(localLogger, &localStruct, GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    localStruct.i = 10;
    GENERICLOGGER_TRACE (loggerp, "Single message");
    GENERICLOGGER_TRACEF(loggerp, "%s %s", "Formatted", "Message");
    GENERICLOGGER_FREE(loggerp);
  }
  
  return 0;
}

static void localLogger(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  localStruct_t *localStructp = (localStruct_t *) userDatavp;

  fprintf(stderr, "I am the local logger, localStruct.i = %d, msgs = %s\n", localStructp->i, msgs);
}
