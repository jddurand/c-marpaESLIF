#ifndef MARPAWRAPPER_ASF_H
#define MARPAWRAPPER_ASF_H

#include <genericStack.h>
#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperAsf          marpaWrapperAsf_t;
typedef struct marpaWrapperAsfTraverser marpaWrapperAsfTraverser_t;

/* --------------- */
/* General options */
/* --------------- */
typedef struct marpaWrapperAsfOption {
  genericLogger_t *genericLoggerp;             /* Default: NULL. */
  short            highRankOnlyb;              /* Default: 1 */
  short            orderByRankb;               /* Default: 1 */
  short            ambiguousb;                 /* Default: 0 */
} marpaWrapperAsfOption_t;

/* A traverser always returns a false or a true value, and a "user-space" value in *valueip.  */
/* think to it as convienent ways to have an index in an output stack, managed in user-space. */
/* A false return value is indicating a failure and traversing will stop.                     */
typedef short (*traverserCallback_t)(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverseb(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp, int *valueip);
  marpaWrapper_EXPORT size_t                    marpaWrapperAsf_traverse_rh_lengthl(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_symbolIdb(marpaWrapperAsfTraverser_t *traverserp, int *symbolIdi);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_ruleIdb(marpaWrapperAsfTraverser_t *traverserp, int *ruleIdip);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_nextb(marpaWrapperAsfTraverser_t *traverserp, short *nextbp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_rh_valueb(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi, int *valueip);
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_traverse_asfp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT marpaWrapperRecognizer_t *marpaWrapperAsf_recognizerp(marpaWrapperAsf_t *marpaWrapperAsfp);
  marpaWrapper_EXPORT void                      marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_ASF_H */
