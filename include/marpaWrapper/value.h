#ifndef MARPAWRAPPER_VALUE_H
#define MARPAWRAPPER_VALUE_H

#include "genericStack.h"
#include "genericLogger.h"
#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperValue marpaWrapperValue_t;

/* Callbacks */
/* --------- */
typedef int (*marpaWrapperValueCallback_t)(void *userDatap, genericStack_t *stackp);


/* --------------- */
/* General options */
/* --------------- */
typedef struct marpaWrapperValueOption {
  genericLogger_t                   *genericLoggerp;             /* Default: NULL */
  short                              highRankOnlyb;              /* Default: 1 */
  short                              orderByRankb;               /* Default: 1 */
  short                              ambiguousb;                 /* Default: 0 */
  short                              nullb;                      /* Default: 0 */
  marpaWrapperValueCallback_t        valueRuleCallbackDefault;   /* Default: NULL */
  marpaWrapperValueCallback_t        valueSymbolCallbackDefault; /* Default: NULL */
  marpaWrapperValueCallback_t        valueNullingCallback;       /* Default: NULL */
} marpaWrapperValueOption_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperValue_t *marpaWrapperValue_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperValueOption_t *marpaWrapperValueOptionp);
  marpaWrapper_EXPORT short                marpaWrapperValue_valueb(marpaWrapperValue_t *marpaWrapperValuep, void *userDatap);
  marpaWrapper_EXPORT void                 marpaWrapperValue_freev(marpaWrapperValue_t *marpaWrapperValuep);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_VALUE_H */
