#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "internal/manageBuf.h"

/*********************/
/* manageBuf_createp */
/*********************/
void *manageBuf_createp(genericLogger_t *genericLoggerp, void ***ppp, size_t *sizelp, const size_t wantedNumberi, const size_t elementSizel) {
  size_t  sizel     = *sizelp;
  size_t  origSizel = sizel;
  void  **pp        = *ppp;
  size_t  prevSizel;

  /*
   * Per def, this routine is managing an array of pointer
   */

  if (sizel < wantedNumberi) {

    prevSizel = sizel;
    while (sizel < wantedNumberi) {
      if (sizel <= 0) {
	/* Let's start at arbitrary number of elements of 100 */
	sizel = 100;
	pp = malloc(sizel * elementSizel);
	if (pp == NULL) {
	  GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	  return NULL;
	}
      } else {
	sizel *= 2;
	if (sizel < prevSizel) {
	  /* Turnaround */
	  errno = ERANGE;
	  GENERICLOGGER_ERRORF(genericLoggerp, "Turnaround detection: %s", strerror(errno));
	  return NULL;
	}
	pp = realloc(pp, sizel * elementSizel);
	if (pp == NULL) {
	  GENERICLOGGER_ERRORF(genericLoggerp, "realloc failure: %s", strerror(errno));
	  return NULL;
	}
      }
      prevSizel = sizel;
    }
  }

  /*
   * Pre-fill pointers with NULL
   */
#ifdef NULL_IS_ZEROES
  memset(&(pp[origSizel]), 0, (sizel - origSizel) * elementSizel);
#else
  {
    int i;
    for (i = origSizel; i < sizel; i++) {
      pp[i] = NULL;
    }
  }
#endif

  *ppp = pp;
  *sizelp = sizel;

  return pp;
}

/*******************/
/* manageBuf_freev */
/*******************/
void manageBuf_freev(genericLogger_t *genericLoggerp, void ***ppp, size_t usedNumberl) {
  void  **pp;
  size_t  i;

  if (ppp != NULL) {
    pp = *ppp;
    if (pp != NULL) {
      if (usedNumberl > 0) {
        for (i = 0; i < usedNumberl; i++) {
          if (pp[i] != NULL) {
            free(pp[i]);
            pp[i] = NULL;
	  }
	  /* In theory I could that */
	  /*
	  else {
	    last;
	  }
	  */
        }
      }
      free(pp);
    }
    *ppp = NULL;
  }
}

