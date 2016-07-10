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
  marpaWrapper_EXPORT marpaWrapperAsf_t     *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp);
  marpaWrapper_EXPORT void                  *marpaWrapperAsf_traversep(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp);
  marpaWrapper_EXPORT short                  marpaWrapperAsf_rh_lengthb(marpaWrapperAsfTraverser_t *traverserp, size_t *lengthlp);
  marpaWrapper_EXPORT genericStack_t        *marpaWrapperAsf_rh_valuesp(marpaWrapperAsfTraverser_t *traverserp);
  marpaWrapper_EXPORT void                  *marpaWrapperAsf_rh_valuep(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi);
  marpaWrapper_EXPORT genericStack_t        *marpaWrapperAsf_allchoicesp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfGlade_t *gladep);
  marpaWrapper_EXPORT marpaWrapperGrammar_t *marpaWrapperAsf_grammarp(marpaWrapperAsf_t *marpaWrapperAsfp);
  marpaWrapper_EXPORT void                   marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_ASF_H */
