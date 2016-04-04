#include "genericLoggerRuntime.h"

#ifdef C_VA_COPY
#define REAL_AP ap2
#else
#define REAL_AP ap
#endif

#include "genericLogger.h"

#define DATEBUILDER_MAX_SIZE sizeof(char) * (1024+1)

struct genericLogger {
  genericLoggerCallback_t  logCallbackp;
  void                    *userDatavp;
  genericLoggerLevel_t     genericLoggerLeveli;
};

static const char *_dateBuilder_internalErrors  = "Internal error when building date";
static const char *_messageBuilder_internalErrors = "Internal error when building message";

static void _genericLogger_defaultCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static char *dateBuilder_internalErrors(void); /* This is returning a STATIC adress */
static char *dateBuilder(const char *fmts);
static char *messageBuilder_internalErrors(void); /* This is returning a STATIC adress */
static char *messageBuilder(const char *fmts, ...);
static char *messageBuilder_ap(const char *fmts, va_list ap);

/*********************/
/* genericLogger_new */
/*********************/
GENERICLOGGER_EXPORT genericLogger_t *genericLogger_new(genericLoggerCallback_t logCallbackp, void *userDatavp, genericLoggerLevel_t genericLoggerLeveli) {
  genericLogger_t *genericLoggerp = malloc(sizeof(genericLogger_t));

  if (genericLoggerp == NULL) {
    /* Well, shall we log about this - a priori no: the caller wanted to set up a particular */
    /* logging system, and not use our default */
    return NULL;
  }

  genericLoggerp->logCallbackp        = logCallbackp;
  genericLoggerp->userDatavp          = userDatavp;
  genericLoggerp->genericLoggerLeveli = genericLoggerLeveli;

  return genericLoggerp;
}

/******************************/
/* genericLogger_logLevel_set */
/******************************/
GENERICLOGGER_EXPORT genericLoggerLevel_t genericLogger_logLevel_set(genericLogger_t *genericLoggerp, genericLoggerLevel_t logLeveli) {
  genericLoggerp->genericLoggerLeveli = logLeveli;
  return genericLoggerp->genericLoggerLeveli;
}

/******************************/
/* genericLogger_logLevel_get */
/******************************/
GENERICLOGGER_EXPORT genericLoggerLevel_t genericLogger_logLevel_get(genericLogger_t *genericLoggerp) {
  return genericLoggerp->genericLoggerLeveli;
}

/***************************************/
/* genericLogger_defaultLogCallback */
/***************************************/
GENERICLOGGER_EXPORT genericLoggerCallback_t genericLogger_defaultLogCallback(void) {
  return &_genericLogger_defaultCallback;
}

/**********************/
/* genericLogger_free */
/**********************/
GENERICLOGGER_EXPORT void genericLogger_free(genericLogger_t **genericLoggerpp) {

  if (genericLoggerpp != NULL) {
    if (*genericLoggerpp != NULL) {
      free(*genericLoggerpp);
      *genericLoggerpp = NULL;
    }
  }
}

/*************************************/
/* _genericLogger_defaultCallback */
/*************************************/
static C_INLINE void _genericLogger_defaultCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  /* We are NOT going to do a general log4c mechanism (this can come later), using genericLogger in fact */
  /* I.e. we are fixing the default output to be: DD/MM/YYYY hh::mm::ss PREFIX MESSAGE */
  const char   *prefixs =
    (logLeveli == GENERICLOGGER_LOGLEVEL_TRACE    ) ? "TRACE"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_DEBUG    ) ? "DEBUG"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_INFO     ) ? "INFO"      :
    (logLeveli == GENERICLOGGER_LOGLEVEL_NOTICE   ) ? "NOTICE"    :
    (logLeveli == GENERICLOGGER_LOGLEVEL_WARNING  ) ? "WARN"      :
    (logLeveli == GENERICLOGGER_LOGLEVEL_ERROR    ) ? "ERROR"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_CRITICAL ) ? "CRITICAL"  :
    (logLeveli == GENERICLOGGER_LOGLEVEL_ALERT    ) ? "ALERT"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_EMERGENCY) ? "EMERGENCY" :
    "UNKOWN";
  char   *dates        = dateBuilder("%d/%m/%Y %H:%M:%S");
  char   *localMsgs    = messageBuilder("%s %9s %s\n", dates, prefixs, (msgs != NULL) ? msgs : "");
  int     filenoStderr = C_FILENO(stderr);
  size_t  bytesWriten  = 0;
  char   *p            = localMsgs;
  size_t  count        = strlen(p);

  while (bytesWriten < count) {
    bytesWriten += C_WRITE(filenoStderr, p+bytesWriten, count-bytesWriten);
  }

  if (dates != dateBuilder_internalErrors()) {
    free(dates);
  }
  if (localMsgs != messageBuilder_internalErrors()) {
    free(localMsgs);
  }
}

/*********************/
/* genericLogger_log */
/*********************/
GENERICLOGGER_EXPORT void genericLogger_log(genericLogger_t *genericLoggerp, genericLoggerLevel_t genericLoggerLeveli, const char *fmts, ...) {
  va_list                  ap;
#ifdef VA_COPY
  va_list                  ap2;
#endif
  char                    *msgs;
  static const char       *emptyMessages = "Empty message";
  genericLoggerCallback_t  logCallbackp;
  void                    *userDatavp;
  genericLoggerLevel_t     genericLoggerDefaultLogLeveli;

  if (genericLoggerp != NULL) {
    if (genericLoggerp->logCallbackp != NULL) {
      logCallbackp = genericLoggerp->logCallbackp;
    } else {
      logCallbackp = &_genericLogger_defaultCallback;
    }
    userDatavp = genericLoggerp->userDatavp;
    genericLoggerDefaultLogLeveli = genericLoggerp->genericLoggerLeveli;
  } else {
    userDatavp = NULL;
    logCallbackp = &_genericLogger_defaultCallback;
    genericLoggerDefaultLogLeveli = GENERICLOGGER_LOGLEVEL_WARNING;
  }

  if (genericLoggerLeveli >= genericLoggerDefaultLogLeveli) {

    va_start(ap, fmts);
#ifdef VA_COPY
    VA_COPY(ap2, ap);
    msgs = (fmts != NULL) ? messageBuilder_ap(fmts, ap2) : (char *) emptyMessages;
    va_end(ap2);
#else
    msgs = (fmts != NULL) ? messageBuilder_ap(fmts, ap) : (char *) emptyMessages;
#endif
    va_end(ap);

    if (msgs != messageBuilder_internalErrors()) {
      logCallbackp(userDatavp, genericLoggerLeveli, msgs);
    } else {
      logCallbackp(userDatavp, GENERICLOGGER_LOGLEVEL_ERROR, msgs);
    }

    if (msgs != emptyMessages && msgs != messageBuilder_internalErrors()) {
      /* No need to assign to NULL, this is a local variable and we will return just after */
      free(msgs);
    }
  }

}

/******************************/
/* dateBuilder_internalErrors */
/******************************/
static C_INLINE char *dateBuilder_internalErrors(void) {
  return (char *) _dateBuilder_internalErrors;
}

/***************/
/* dateBuilder */
/***************/
static C_INLINE char *dateBuilder(const char *fmts) {
  char      *dates;
  time_t     t;
#ifdef C_LOCALTIME_R
  struct tm  tmpLocal;
#endif
  struct tm *tmp;

  /* We assume that a date should never exceed 1024 bytes isn't it */
  dates = malloc(DATEBUILDER_MAX_SIZE);
  if (dates == NULL) {
    dates = (char *) _dateBuilder_internalErrors;
  } else {
    t = time(NULL);
#ifdef C_LOCALTIME_R
    tmp = C_LOCALTIME_R(&t, &tmpLocal);
#else
    tmp = localtime(&t);
#endif
    if (tmp == NULL) {
      dates = (char *) _dateBuilder_internalErrors;
    } else {
      if (strftime(dates, DATEBUILDER_MAX_SIZE, fmts, tmp) == 0) {
	dates = (char *) _dateBuilder_internalErrors;
      }
    }
  }

  return dates;
}

/*********************************/
/* messageBuilder_internalErrors */
/*********************************/
static C_INLINE char *messageBuilder_internalErrors(void) {
  return (char *) _messageBuilder_internalErrors;
}

/******************/
/* messageBuilder */
/******************/
static C_INLINE char *messageBuilder(const char *fmts, ...) {
  va_list ap;
#ifdef C_VA_COPY
  va_list ap2;
#endif
  char   *msgs;

  va_start(ap, fmts);
#ifdef C_VA_COPY
  C_VA_COPY(ap2, ap);
#endif
  msgs = messageBuilder_ap(fmts,  REAL_AP);
#ifdef C_VA_COPY
  va_end(ap2);
#endif
  va_end(ap);

  return msgs;
}

/*********************/
/* messageBuilder_ap */
/*********************/
static C_INLINE char *messageBuilder_ap(const char *fmts, va_list ap) {
  int     n;
  size_t  size = 4096;     /* Guess we need no more than 4096 bytes */
  char   *p, *np;
#ifdef C_VA_COPY
  va_list ap2;
#endif

  /* ----------------------------------------------------------------------------------------------------------------------- */
  /* Take care: Windows's vsnprintf is not like UNIX's, i.e:                                                                 */
  /*                                                                                                                         */
  /* Output:                                                                                                                 */
  /* [Windows] -1 if the number of characters if > count. Minus trailing null character                                      */
  /* [ UNIX  ] number of chars that would have been writen. Minus trailing null character                                    */
  /*                                                                                                                         */
  /* Argument:                                                                                                               */
  /* [Windows] number of characters wanted, does not include the trailing null character                                     */
  /* [ UNIX  ] number of characters wanted + the trailing null character                                                     */
  /* ----------------------------------------------------------------------------------------------------------------------- */

  p = malloc(size);
  if (p == NULL) {
    return messageBuilder_internalErrors();
  }

  while (1) {

    /* Try to print in the allocated space */

#ifdef C_VA_COPY
    C_VA_COPY(ap2, ap);
#endif
    n = C_VSNPRINTF(p, size, fmts, REAL_AP);   /* On Windows, argument does not include space for the NULL */
#ifdef C_VA_COPY
    va_end(ap2);
#endif

    /* Check error code */
#ifndef _WIN32
    /* On not-windows, if output is negative an output error is encountered */
    if (n < 0) {
      free(p);
      return messageBuilder_internalErrors();
    }
#endif

    /* If that worked, return the string, unless not enough space - in which case we retry -; */

    if
#ifdef _WIN32
      (n >= 0 && n < (int) size)
#else
      (n < size)
#endif
        {
      return p;
    }

    /* Else try again with more space */

#ifdef _WIN32
    size *= 2;          /* Maybe enough ? */
#else
    size = n + 1;       /* Precisely what is needed */
#endif

    np = realloc(p, size);
    if (np == NULL) {
      free(p);
      return messageBuilder_internalErrors();
    } else {
      p = np;
    }
  }

  /* Should never happen */
  return messageBuilder_internalErrors();
}

