#ifndef TCONV_TRACE_H
#define TCONV_TRACE_H

/* Util macros used internally */

#ifndef TCONV_NTRACE
#define TCONV_TRACE(contextp, msgs) do {                          \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_TRACE(contextp->genericLoggerp, msgs);              \
    }                                                                   \
  } while (0)
#define TCONV_TRACEF(contextp, fmts, ...) do {                          \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_TRACEF(contextp->genericLoggerp, fmts, __VA_ARGS__); \
    }                                                                   \
  } while (0)
#define TCONV_WARN(contextp, msgs) do {                                 \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_WARN(contextp->genericLoggerp, msgs);               \
    }                                                                   \
  } while (0)
#define TCONV_WARNF(contextp, fmts, ...) do {                           \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_WARNF(contextp->genericLoggerp, fmts, __VA_ARGS__); \
    }                                                                   \
  } while (0)
#define TCONV_ERROR(contextp, msgs) do {                                \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_ERROR(contextp->genericLoggerp, msgs);              \
    }                                                                   \
  } while (0)
#define TCONV_ERRORF(contextp, fmts, ...) do {                          \
    if ((contextp != NULL) && (contextp->genericLoggerp != NULL)) {     \
      GENERICLOGGER_ERRORF(contextp->genericLoggerp, fmts, __VA_ARGS__); \
    }                                                                   \
  } while (0)
#else
#define TCONV_TRACE(contextp, msgs)
#define TCONV_TRACEF(contextp, fmts, ...)
#define TCONV_WARN(contextp, msgs)
#define TCONV_WARNF(contextp, fmts, ...)
#define TCONV_ERROR(contextp, msgs)
#define TCONV_ERRORF(contextp, fmts, ...)
#endif

#endif /* TCONV_TRACE_H */
