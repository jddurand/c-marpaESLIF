#ifndef MARPAWRAPPER_GLADE_H
#define MARPAWRAPPER_GLADE_H

#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperGlade marpaWrapperGlade_t;

/* A glade instance is created and freed internally */

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT short marpaWrapperAsf_symbolId(marpaWrapperGlade_t *marpaWrapperGladep, int *symbolIdp);
  marpaWrapper_EXPORT short marpaWrapperAsf_ruleId(marpaWrapperGlade_t *marpaWrapperGladep, int *ruleIdp);
  marpaWrapper_EXPORT short marpaWrapperAsf_rhLength(marpaWrapperGlade_t *marpaWrapperGladep, int *rhLengthp);
  marpaWrapper_EXPORT short marpaWrapperAsf_rhValue(marpaWrapperGlade_t *marpaWrapperGladep, int positioni, int *rhValuep);
  marpaWrapper_EXPORT short marpaWrapperAsf_rhValues(marpaWrapperGlade_t *marpaWrapperGladep, int nRhValuel, int *rhValueArrayp);
  marpaWrapper_EXPORT short marpaWrapperAsf_allChoices(marpaWrapperGlade_t *marpaWrapperGladep, int nChoicel, int *choiceArrayp);
  marpaWrapper_EXPORT int   marpaWrapperAsf_next(marpaWrapperGlade_t *marpaWrapperGladep);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_GLADE_H */
