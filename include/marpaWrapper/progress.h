#ifndef MARPAWRAPPER_PROGRESS
#define MARPAWRAPPER_PROGRESS

#include <stddef.h>
#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/export.h"

typedef struct marpaWrapperProgress {
  int earleySetIdi;
  int earleySetOriginIdi;
  int rulei;
  int positioni;
} marpaWrapperProgress_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT size_t marpaWrapperProgressb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int starti, int endi, marpaWrapperProgress_t **marpaWrapperProgresspp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_PROGRESS */
