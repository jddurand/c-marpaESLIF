#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "genericLogger.h"

static void localLogger(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
typedef struct localStruct {
  int i;
} localStruct_t;

static void mainap(genericLogger_t *loggerp, localStruct_t *localStructp, const char *fmts, ...);

int main() {
  genericLogger_t *loggerp;
  genericLogger_t *loggerClonep;
  localStruct_t    localStruct;

  /* With no logger allocated - it still work - and defaults to the WARN level */
  GENERICLOGGER_WARN (NULL, NULL);
  GENERICLOGGER_WARNF(NULL, NULL, NULL);
  GENERICLOGGER_WARN (NULL, "[no argument - no logger] Single message");
  GENERICLOGGER_WARNF(NULL, "[two arguments - no logger] Formatted message: %s %s", "argument1", "argument2");

  /* With a wanted level, no user-level implementation */
  loggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    GENERICLOGGER_TRACE (loggerp, "[no argument - allocated logger] Single message");
    GENERICLOGGER_TRACEF(loggerp, "[two arguments - allocated logger] Formatted message: %s %s", "argument1", "argument2");
    GENERICLOGGER_FREE(loggerp);
  }

  /* With a user-level implementation */
  loggerp = GENERICLOGGER_CUSTOM(localLogger, &localStruct, GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    if (genericLogger_userDatavp_getp(loggerp) != &localStruct) {
      GENERICLOGGER_ERRORF(loggerp, "genericLogger_userDatavp_getp returns userDatavp = %p != %p", genericLogger_userDatavp_getp(loggerp), &localStruct);
    }
    if (genericLogger_userDatavp_setp(loggerp, NULL) != &localStruct) {
      GENERICLOGGER_ERRORF(loggerp, "genericLogger_userDatavp_setp returns userDatavp = %p != %p", genericLogger_userDatavp_getp(loggerp), &localStruct);
    }
    if (genericLogger_userDatavp_setp(loggerp, &localStruct) != NULL) {
      GENERICLOGGER_ERRORF(loggerp, "genericLogger_userDatavp_setp returns userDatavp = %p != %p", genericLogger_userDatavp_getp(loggerp), NULL);
    }
    localStruct.i = 10;
    GENERICLOGGER_TRACE (loggerp, "[no argument - custom logger] Single message");
    GENERICLOGGER_TRACEF(loggerp, "[two arguments - custom logger] Formatted message: %s %s", "argument1", "argument2");
    /* ap */
    mainap(loggerp, &localStruct, "[ap argument - custom logger] Formatted message: %s %s", "argument1", "argument2");
    GENERICLOGGER_FREE(loggerp);
  }

  /* Clone */
  loggerp = GENERICLOGGER_CUSTOM(localLogger, &localStruct, GENERICLOGGER_LOGLEVEL_TRACE);
  if (loggerp != NULL) {
    loggerClonep = GENERICLOGGER_CLONE(loggerp);
    if (loggerClonep != NULL) {
      GENERICLOGGER_TRACE (loggerClonep, "[no argument - cloned logger] Single message");
      GENERICLOGGER_TRACEF(loggerClonep, "[two arguments - cloned logger] Formatted message: %s %s", "argument1", "argument2");
      GENERICLOGGER_FREE(loggerClonep);
    }
    GENERICLOGGER_FREE(loggerp);
  }

  return 0;
}

static void mainap(genericLogger_t *loggerp, localStruct_t *localStructp, const char *fmts, ...) {
  va_list ap;

  localStructp->i = 20;

  va_start(ap, fmts);
  GENERICLOGGER_TRACEAP(loggerp, fmts, ap);
  va_end(ap);
}

static void localLogger(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  localStruct_t *localStructp = (localStruct_t *) userDatavp;

  fprintf(stderr, "I am the local logger, localStruct.i = %d, msgs = %s\n", localStructp->i, msgs);
}
