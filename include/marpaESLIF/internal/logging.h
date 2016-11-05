#ifndef MARPAESLIF_INTERNAL_LOGGING_H
#define MARPAESLIF_INTERNAL_LOGGING_H
#include <stdio.h>

#define MARPAESLIF_LOC_FMT "[v%s/%s at %s:%04d]"
#define MARPAESLIF_LOC_VAR MARPAESLIF_VERSION, funcs, FILENAMES, __LINE__

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
  MARPAESLIF2LOG(marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d Iter %4d][%s%-47s at %s:%04d]%*s" fmts, marpaESLIFRecognizerp->leveli, marpaESLIFRecognizerp->resumeCounteri, marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", __VA_ARGS__))
#define MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, msgs) \
  MARPAESLIF2LOG(marpaESLIFRecognizerp->marpaESLIFp, GENERICLOGGER_TRACEF(genericLoggerp, "[Level %2d Iter %4d][%s%-47s at %s:%04d]%*s%s", marpaESLIFRecognizerp->leveli, marpaESLIFRecognizerp->resumeCounteri, marpaESLIFRecognizerp->discardb ? "!" : " ", funcs, FILENAMES, __LINE__, marpaESLIFRecognizerp->leveli + marpaESLIFRecognizerp->callstackCounteri, " ", msgs))
#else
#define MARPAESLIF_TRACEF(marpaESLIFp, funcs, fmts, ...)
#define MARPAESLIF_TRACE(marpaESLIFp, funcs, msgs)
#define MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFp, funcs, fmts, ...)
#define MARPAESLIFRECOGNIZER_TRACE(marpaESLIFp, funcs, msgs)
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

/* C.f. http://grapsus.net/blog/post/Hexadecimal-dump-in-C */
#ifndef MARPAESLIF_HEXDUMP_COLS
#define MARPAESLIF_HEXDUMP_COLS 16
#endif
#define MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp, headers, asciidescs, p, lengthl, traceb) do { \
    marpaESLIFRecognizer_t       *_marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) (marpaESLIFRecognizerp); \
    marpaESLIF_t                 *_marpaESLIFp = _marpaESLIFRecognizerp->marpaESLIFp; \
    marpaESLIF_stringGenerator_t  _marpaESLIF_stringGenerator;          \
    char                          *_headers = (char *) (headers);       \
    char                          *_asciidescs = (char *) (asciidescs); \
    char                          *_p = (char *) (p);                   \
    size_t                         _lengthl = (size_t) (lengthl);       \
    short                         _traceb = (short) (traceb);           \
    genericLogger_t               *_genericLoggerp;                     \
    size_t  _i;                                                         \
    size_t  _j;                                                         \
                                                                        \
    _marpaESLIF_stringGenerator.marpaESLIFp = _marpaESLIFp;             \
    _marpaESLIF_stringGenerator.s           = NULL;                     \
    _marpaESLIF_stringGenerator.l           = 0;                        \
    _marpaESLIF_stringGenerator.okb         = 0;                        \
                                                                        \
    _genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &_marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
    if (_genericLoggerp != NULL) {                                      \
      if (_traceb) {                                                    \
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------"); \
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s%s (%ld bytes)", _headers, _asciidescs, (unsigned long) _lengthl); \
      } else {                                                          \
        MARPAESLIF_ERROR(_marpaESLIFp, "--------------------------------------------"); \
        MARPAESLIF_ERRORF(_marpaESLIFp, "%s%s (%ld bytes)", _headers, _asciidescs, (unsigned long) _lengthl); \
      }                                                                 \
      for (_i = 0; _i < _lengthl + ((_lengthl % MARPAESLIF_HEXDUMP_COLS) ? (MARPAESLIF_HEXDUMP_COLS - _lengthl % MARPAESLIF_HEXDUMP_COLS) : 0); _i++) { \
        /* print offset */                                              \
        if (_i % MARPAESLIF_HEXDUMP_COLS == 0) {                        \
          GENERICLOGGER_TRACEF(_genericLoggerp, "0x%06x: ", _i);        \
        }                                                               \
        /* print hex data */                                            \
        if (_i < _lengthl) {                                             \
          GENERICLOGGER_TRACEF(_genericLoggerp, "%02x ", 0xFF & _p[_i]); \
        } else { /* end of block, just aligning for ASCII dump */       \
          GENERICLOGGER_TRACE(_genericLoggerp, "   ");                  \
        }                                                               \
        /* print ASCII dump */                                          \
        if (_i % MARPAESLIF_HEXDUMP_COLS == (MARPAESLIF_HEXDUMP_COLS - 1)) { \
          for (_j = _i - (MARPAESLIF_HEXDUMP_COLS - 1); _j <= _i; _j++) { \
            if (_j >= _lengthl) { /* end of block, not really printing */ \
              GENERICLOGGER_TRACE(_genericLoggerp, " ");                \
            }                                                           \
            else if (isprint(0xFF & _p[_j])) { /* printable char */     \
              GENERICLOGGER_TRACEF(_genericLoggerp, "%c", 0xFF & _p[_j]); \
            }                                                           \
            else { /* other char */                                     \
              GENERICLOGGER_TRACE(_genericLoggerp, ".");                \
            }                                                           \
          }                                                             \
          if (_marpaESLIF_stringGenerator.okb) {                        \
            if (_traceb) {                                              \
              MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, _marpaESLIF_stringGenerator.s); \
            } else {                                                    \
              MARPAESLIF_ERROR(_marpaESLIFp, _marpaESLIF_stringGenerator.s); \
            }                                                           \
            free(_marpaESLIF_stringGenerator.s);                        \
            _marpaESLIF_stringGenerator.s = NULL;                       \
            _marpaESLIF_stringGenerator.okb = 0;                        \
          }                                                             \
        }                                                               \
      }                                                                 \
      if (_traceb) {                                                    \
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------"); \
      } else {                                                          \
        MARPAESLIF_ERROR(_marpaESLIFp, "--------------------------------------------"); \
      }                                                                 \
      GENERICLOGGER_FREE(_genericLoggerp);                              \
    }                                                                   \
  } while (0)

#endif /* MARPAESLIF_INTERNAL_LOGGING_H */
