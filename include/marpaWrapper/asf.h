#ifndef MARPAWRAPPER_ASF_H
#define MARPAWRAPPER_ASF_H

#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/glade.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperAsf marpaWrapperAsf_t;

/* --------------- */
/* General options */
/* --------------- */
typedef struct marpaWrapperAsfOption {
  genericLogger_t *genericLoggerp;             /* Default: NULL. */
  int              factoringMaxi;              /* Default: 42    */
  short            highRankOnlyb;              /* Default: 1 */
  short            orderByRankb;               /* Default: 1 */
  short            ambiguousb;                 /* Default: 0 */
} marpaWrapperAsfOption_t;

typedef short (*traverserCallback_t)(void *userDatavp, marpaWrapperGlade_t *marpaWrapperGladep);

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperAsf_t    *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp);
  marpaWrapper_EXPORT short                 marpaWrapperAsf_traverseb(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp);
  marpaWrapper_EXPORT void                  marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_ASF_H */
