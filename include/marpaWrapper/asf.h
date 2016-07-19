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

typedef void *(*traverserCallback_t)(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp);

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp);
  marpaWrapper_EXPORT void                     *marpaWrapperAsf_traversep(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_rh_lengthb(marpaWrapperAsfTraverser_t *traverserp, size_t *lengthlp);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_symbolIdb(marpaWrapperAsfTraverser_t *traverserp, int *symbolIdip);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_ruleIdb(marpaWrapperAsfTraverser_t *traverserp, int *ruleIdip);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_nextFactoringb(marpaWrapperAsfTraverser_t *traverserp, int *factoringIxip);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_nextSymchb(marpaWrapperAsfTraverser_t *traverserp, int *symchIxip);
  marpaWrapper_EXPORT short                     marpaWrapperAsf_traverse_nextb(marpaWrapperAsfTraverser_t *traverserp, int *idip);
  marpaWrapper_EXPORT genericStack_t           *marpaWrapperAsf_traverse_rh_valuesStackp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT genericStack_t           *marpaWrapperAsf_traverse_rh_allChoicesStackp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT void                     *marpaWrapperAsf_traverse_rh_valuep(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi);
  marpaWrapper_EXPORT void                     *marpaWrapperAsf_traverse_rh_valuep(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi);
  marpaWrapper_EXPORT marpaWrapperAsf_t        *marpaWrapperAsf_traverse_asfp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT marpaWrapperRecognizer_t *marpaWrapperAsf_recognizerp(marpaWrapperAsf_t *marpaWrapperAsfp);
  marpaWrapper_EXPORT void                      marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_ASF_H */
