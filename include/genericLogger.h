#ifndef GENERICLOGGER_H
#define GENERICLOGGER_H

#include "genericLoggerConfig.h"

typedef enum genericLoggerLevel {
  GENERICLOGGER_LOGLEVEL_TRACE = 0,
  GENERICLOGGER_LOGLEVEL_DEBUG,
  GENERICLOGGER_LOGLEVEL_INFO,
  GENERICLOGGER_LOGLEVEL_NOTICE,
  GENERICLOGGER_LOGLEVEL_WARNING,
  GENERICLOGGER_LOGLEVEL_ERROR,
  GENERICLOGGER_LOGLEVEL_CRITICAL,
  GENERICLOGGER_LOGLEVEL_ALERT,
  GENERICLOGGER_LOGLEVEL_EMERGENCY
} genericLoggerLevel_t;

/*************************
   Opaque object pointer
 *************************/
typedef struct genericLogger genericLogger_t;
typedef void (*genericLoggerCallback_t)(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

GENERICLOGGEREXPORT genericLoggerCallback_t genericLogger_defaultLogCallback(void);
/* For applications that want to initialize a structure outside of executable blocks: */
GENERICLOGGEREXPORT void _genericLogger_defaultCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

GENERICLOGGEREXPORT genericLoggerLevel_t    genericLogger_logLevel_seti(genericLogger_t *genericLoggerp, genericLoggerLevel_t logLeveli);
GENERICLOGGEREXPORT genericLoggerLevel_t    genericLogger_logLevel_geti(genericLogger_t *genericLoggerp);

GENERICLOGGEREXPORT genericLogger_t        *genericLogger_newp(genericLoggerCallback_t logCallbackp, void *userDatavp, genericLoggerLevel_t genericLoggerLeveli);
GENERICLOGGEREXPORT void                  genericLogger_freev(genericLogger_t **genericLoggerpp);


/*****************************************/
/* Convenience structure for log options */
/*****************************************/
typedef struct genericLogger_Option {
  genericLoggerCallback_t logCallbackp;
  void                   *logCallbackDatavp;
  genericLoggerLevel_t    logLevelWantedi;
} genericLogger_Option_t;

/* C99 has problems with empty __VA_ARGS__ so we split macros in two categories: */
/* logging with no variable parameter */
/* logging with    variable paramerer(s) */

GENERICLOGGEREXPORT void genericLogger_log(genericLogger_t *genericLoggerp, genericLoggerLevel_t genericLoggerLeveli, const char *fmts, ...);

#define GENERICLOGGER_LOG(genericLoggerp, logLeveli, msgs)       genericLogger_log(genericLoggerp, logLeveli, msgs)
#define GENERICLOGGER_LOGF(genericLoggerp, logLeveli, fmts, ...) genericLogger_log(genericLoggerp, logLeveli, fmts, __VA_ARGS__)

#define GENERICLOGGER_TRACE(genericLoggerp, msgs)                GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE, msgs)
#define GENERICLOGGER_TRACEF(genericLoggerp, fmts, ...)          GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE, fmts, __VA_ARGS__)

#define GENERICLOGGER_DEBUG(genericLoggerp, msgs)                GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_DEBUG, msgs)
#define GENERICLOGGER_DEBUGF(genericLoggerp, fmts, ...)          GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_DEBUG, fmts, __VA_ARGS__)

#define GENERICLOGGER_INFO(genericLoggerp, msgs)                 GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO, msgs)
#define GENERICLOGGER_INFOF(genericLoggerp, fmts, ...)           GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO, fmts, __VA_ARGS__)

#define GENERICLOGGER_NOTICE(genericLoggerp, msgs)               GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_NOTICE, msgs)
#define GENERICLOGGER_NOTICEF(genericLoggerp, fmts, ...)         GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_NOTICE, fmts, __VA_ARGS__)

#define GENERICLOGGER_WARN(genericLoggerp, msgs)              GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_WARNING, msgs)
#define GENERICLOGGER_WARNF(genericLoggerp, fmts, ...)        GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_WARNING, fmts, __VA_ARGS__)

#define GENERICLOGGER_ERROR(genericLoggerp, msgs)                GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_ERROR, msgs)
#define GENERICLOGGER_ERRORF(genericLoggerp, fmts, ...)          GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_ERROR, fmts, __VA_ARGS__)

#define GENERICLOGGER_CRITICAL(genericLoggerp, msgs)             GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_CRITICAL, msgs)
#define GENERICLOGGER_CRITICALF(genericLoggerp, fmts, ...)       GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_CRITICAL, fmts, __VA_ARGS__)

#define GENERICLOGGER_ALERT(genericLoggerp, msgs)                GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_ALERT, msgs)
#define GENERICLOGGER_ALERTF(genericLoggerp, fmts, ...)          GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_ALERT, fmts, __VA_ARGS__)

#define GENERICLOGGER_EMERGENCY(genericLoggerp, msgs)            GENERICLOGGER_LOG (genericLoggerp, GENERICLOGGER_LOGLEVEL_EMERGENCY, msgs)
#define GENERICLOGGER_EMERGENCYF(genericLoggerp, fmts, ...)      GENERICLOGGER_LOGF(genericLoggerp, GENERICLOGGER_LOGLEVEL_EMERGENCY, fmts, __VA_ARGS__)

#endif /* GENERICLOGGER_H */
