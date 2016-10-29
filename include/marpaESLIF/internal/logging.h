#ifndef MARPAESLIF_INTERNAL_LOGGING_H
#define MARPAESLIF_INTERNAL_LOGGING_H
#include <stdio.h>

#define MARPAESLIF_LOC_FMT "[v%s/%s:%d]"
#define MARPAESLIF_LOC_VAR MARPAESLIF_VERSION, funcs, __LINE__

#define MARPAESLIF2LOG(marpaESLIFp, rest) do {				\
    genericLogger_t *genericLoggerp = ((marpaESLIFp) != NULL) ? (marpaESLIFp)->marpaESLIFOption.genericLoggerp : NULL; \
    if (genericLoggerp != NULL) {					\
      rest;								\
    }									\
  } while (0)

#ifndef MARPAESLIF_NTRACE
#define MARPAESLIF_TRACEF(marpaESLIFp, funcs, fmts, ...) MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[%s at %s:%04d] " fmts, funcs, FILENAMES, __LINE__, __VA_ARGS__))
#define MARPAESLIF_TRACE(marpaESLIFp, funcs, msgs)       MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[%s at %s:%04d] %s", funcs, FILENAMES, __LINE__, msgs))
#define MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, fmts, ...) \
  MARPAESLIF2LOG(marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d Iter %3i][%s%-47s at %s:%04d]%*s" fmts, marpaESLIFRecognizerp->leveli, marpaESLIFRecognizerp->resumeCounteri, marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", __VA_ARGS__))
#define MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, msgs) \
  MARPAESLIF2LOG(marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d Iter %3i][%s%-47s at %s:%04d]%*s%s", marpaESLIFRecognizerp->leveli, marpaESLIFRecognizerp->resumeCounteri, marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", msgs))
#define MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, fmts, ...) \
  MARPAESLIF2LOG(marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d         ][%s%-47s at %s:%04d]%*s " fmts, marpaESLIFValuep->marpaESLIFRecognizerp->leveli, marpaESLIFValuep->marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", __VA_ARGS__))
#define MARPAESLIFVALUE_TRACE(marpaESLIFValuep, funcs, msgs)       \
  MARPAESLIF2LOG(marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d         ][%s%-47s at %s:%04d]%*s%s", marpaESLIFValuep->marpaESLIFRecognizerp->leveli, marpaESLIFValuep->marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", msgs))
#else
#define MARPAESLIF_TRACEF(marpaESLIFp, funcs, fmts, ...)
#define MARPAESLIF_TRACE(marpaESLIFp, funcs, msgs)
#define MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFp, funcs, fmts, ...)
#define MARPAESLIFRECOGNIZER_TRACE(marpaESLIFp, funcs, msgs)
#define MARPAESLIFVALUE_TRACEF(marpaESLIFp, funcs, fmts, ...)
#define MARPAESLIFVALUE_TRACE(marpaESLIFp, funcs, msgs)
#endif

#define MARPAESLIF_DEBUGF(marpaESLIFp, fmts, ...)     MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_DEBUGF    ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_DEBUG(marpaESLIFp, ...)            MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_DEBUG     ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_INFOF(marpaESLIFp, fmts, ...)      MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_INFOF     ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_INFO(marpaESLIFp, ...)             MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_INFO      ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_NOTICEF(marpaESLIFp, fmts, ...)    MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_NOTICEF   ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_NOTICE(marpaESLIFp, ...)           MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_NOTICE    ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_WARNF(marpaESLIFp, fmts, ...)      MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_WARNF     ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_WARN(marpaESLIFp, ...)             MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_WARN      ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_ERRORF(marpaESLIFp, fmts, ...)     MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_ERRORF    ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_ERROR(marpaESLIFp, ...)            MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_ERROR     ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_CRITICALF(marpaESLIFp, fmts, ...)  MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_CRITICALF ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_CRITICAL(marpaESLIFp, ...)         MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_CRITICAL  ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_ALERTF(marpaESLIFp, fmts, ...)     MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_ALERTF    ((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_ALERT(marpaESLIFp, ...)            MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_ALERT     ((genericLoggerp),         __VA_ARGS__))
#define MARPAESLIF_EMERGENCYF(marpaESLIFp, fmts, ...) MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_EMERGENCYF((genericLoggerp), (fmts), __VA_ARGS__))
#define MARPAESLIF_EMERGENCY(marpaESLIFp, ...)        MARPAESLIF2LOG(marpaESLIFp, GENERICLOGGER_EMERGENCY ((genericLoggerp),         __VA_ARGS__))

#endif /* MARPAESLIF_INTERNAL_LOGGING_H */
