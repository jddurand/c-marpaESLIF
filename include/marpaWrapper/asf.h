#ifndef MARPAWRAPPER_ASF_H
#define MARPAWRAPPER_ASF_H

#include <genericStack.h>
#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/glade.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperAsf          marpaWrapperAsf_t;
typedef struct marpaWrapperAsfGlade     marpaWrapperAsfGlade_t;
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

/* A traverser always returns an int, whose semantic is similar to the "int" of an alternative: */
/* think to it as convienent ways to have an index in an output stack, managed in user-space.   */
/* A negative return value is indicating a failure and traversing will stop.                    */
typedef int (*traverserCallback_t)(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp);

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp);
  marpaWrapper_EXPORT int                       marpaWrapperAsf_traversei(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp);
  marpaWrapper_EXPORT size_t                    marpaWrapperAsf_traverse_rh_lengthl(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT int                       marpaWrapperAsf_traverse_symbolIdi(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT int                       marpaWrapperAsf_traverse_ruleIdi(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_nextb(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT genericStack_t           *marpaWrapperAsf_traverse_rh_valuesStackp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT genericStack_t           *marpaWrapperAsf_traverse_rh_allChoicesStackp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT int                       marpaWrapperAsf_traverse_rh_valuei(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi);
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_traverse_asfp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT marpaWrapperRecognizer_t *marpaWrapperAsf_recognizerp(marpaWrapperAsf_t *marpaWrapperAsfp);
  marpaWrapper_EXPORT void                      marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_ASF_H */
