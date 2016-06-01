#ifndef MARPAWRAPPER_PROGRESS
#define MARPAWRAPPER_PROGRESS

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
  marpaWrapper_EXPORT short marpaWrapperProgressb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int starti, int endi, size_t *nmarpaWrapperProgressip, marpaWrapperProgress_t ***marpaWrapperProgressppp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_PROGRESS */
