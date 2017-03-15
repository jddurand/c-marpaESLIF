#include <stdlib.h>
#include "genericSparseArray.h"
#include <genericLogger.h>

typedef struct myContext {
  genericLogger_t *genericLoggerp;
} myContext_t;

static int    mySparseArrayIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp);
static void   mySparseArrayKeyFreeFunction(void *userDatavp, void **pp);
static void  *mySparseArrayValCopyFunction(void *userDatavp, void **pp);
static void   mySparseArrayValFreeFunction(void *userDatavp, void **pp);
static int    mySparseArrayTest(short withAllocb);
static void   mySparseArrayDump(myContext_t *myContextp, genericSparseArray_t *mySparseArrayp);

int main(int argc, char **argv) {
  if (mySparseArrayTest(0) == 0) {
    return 1;
  }
  return 0;
  if (mySparseArrayTest(1) == 0) {
    return 1;
  }
  return 0;
}

static int mySparseArrayTest(short withAllocb) {
  myContext_t           myContext;
  myContext_t          *myContextp = &myContext;
  myContext_t          *myContextFoundp;
  int                   rci = 0;
  genericLogger_t      *genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  genericSparseArray_t  mySparseArray;
  genericSparseArray_t *mySparseArrayp;
  short                 findResultb;
  short                 removeResultb;
  int                   i;

  myContext.genericLoggerp = genericLoggerp;

  if (withAllocb) {
    GENERICSPARSEARRAY_NEW(mySparseArrayp, mySparseArrayIndFunction);
  } else {
    mySparseArrayp = &mySparseArray;
    GENERICSPARSEARRAY_INIT(mySparseArrayp, mySparseArrayIndFunction);
  }
  if (GENERICSPARSEARRAY_ERROR(mySparseArrayp)) {
    GENERICLOGGER_ERROR(genericLoggerp, "Error when creating generic sparse array");
    rci = 1;
    goto err;
  }
  if (withAllocb) {
    GENERICSPARSEARRAY_VALCOPYFUNCTION(mySparseArrayp) = mySparseArrayValCopyFunction;
    GENERICSPARSEARRAY_VALFREEFUNCTION(mySparseArrayp) = mySparseArrayValFreeFunction;
  }
  if (withAllocb) {
    GENERICLOGGER_TRACEF(genericLoggerp, "Created sparse array at %p, alloc mode=%d", mySparseArrayp, (int) withAllocb);
  } else {
    GENERICLOGGER_TRACEF(genericLoggerp, "Initialized sparse array at %p, alloc mode=%d", mySparseArrayp, (int) withAllocb);
  }

  for (i = 0; i < 2; i++) {
    mySparseArrayDump(myContextp, mySparseArrayp);
  
    GENERICSPARSEARRAY_SET(mySparseArrayp, myContextp, 0, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "Pushed PTR %p at indice 0", myContextp);
    mySparseArrayDump(myContextp, mySparseArrayp);

    GENERICSPARSEARRAY_SET(mySparseArrayp, myContextp, 100, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "Setted again PTR %p at indice 100", myContextp);
    mySparseArrayDump(myContextp, mySparseArrayp);

    GENERICSPARSEARRAY_SET(mySparseArrayp, myContextp, 100, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "Setted again PTR %p at indice 100", myContextp);
    mySparseArrayDump(myContextp, mySparseArrayp);

    GENERICSPARSEARRAY_SET(mySparseArrayp, myContextp, 101, PTR, NULL);
    GENERICLOGGER_TRACE(genericLoggerp, "Pushed NULL at indice 101");
    mySparseArrayDump(myContextp, mySparseArrayp);

    GENERICSPARSEARRAY_SET(mySparseArrayp, myContextp, 101, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "Setted again PTR %p at indice 101", myContextp);
    mySparseArrayDump(myContextp, mySparseArrayp);

    GENERICLOGGER_TRACEF(genericLoggerp, "Looking for PTR %p at indice 101", myContextp);
    GENERICSPARSEARRAY_FIND(mySparseArrayp, myContextp, 101, PTR, &myContextFoundp, findResultb);
    if (! findResultb) {
      GENERICLOGGER_ERRORF(genericLoggerp, "Failed to find PTR %p at indice 101", myContextp);
    } else {
      if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
	GENERICLOGGER_TRACEF(genericLoggerp, "Success searching for PTR %p", myContextFoundp);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "Success searching for PTR but found a bad pointer %p", myContextFoundp);
      }
    }

    GENERICLOGGER_TRACEF(genericLoggerp, "Removing PTR %p at indice 101", myContextp);
    GENERICSPARSEARRAY_REMOVE(mySparseArrayp, myContextp, 101, PTR, &myContextFoundp, removeResultb);
    mySparseArrayDump(myContextp, mySparseArrayp);
    if (! removeResultb) {
      GENERICLOGGER_ERRORF(genericLoggerp, "Failed to remove PTR %p at indice 101", myContextp);
    } else {
      if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
	if (withAllocb) {
	  mySparseArrayValFreeFunction(myContextp, (void **) &myContextFoundp);
	}
	GENERICLOGGER_TRACEF(genericLoggerp, "Success removing PTR %p at indice 101", myContextFoundp);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "Success removing PTR at indice 101 but found a bad pointer %p", myContextFoundp);
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "Looking again for PTR %p at indice 101", myContextp);
      GENERICSPARSEARRAY_FIND(mySparseArrayp, myContextp, 101, PTR, &myContextFoundp, removeResultb);
      if (! removeResultb) {
	GENERICLOGGER_TRACEF(genericLoggerp, "Failed to find PTR %p at indice 101 and this is ok", myContextp);
      } else {
	GENERICLOGGER_ERRORF(genericLoggerp, "Unexpected success searching for PTR %p at indice 101, got %p", myContextp, myContextFoundp);
      }
    }

    GENERICLOGGER_TRACE(genericLoggerp, ".................................");
    GENERICLOGGER_TRACE(genericLoggerp, "... Relaxing the sparse array ...");
    GENERICLOGGER_TRACE(genericLoggerp, ".................................");
    GENERICSPARSEARRAY_RELAX(mySparseArrayp, myContextp);
  }

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  if (withAllocb) {
    GENERICLOGGER_TRACEF(genericLoggerp, "Freeing sparse array at %p", mySparseArrayp);
    GENERICSPARSEARRAY_FREE(mySparseArrayp, myContextp);
  } else {
    GENERICLOGGER_TRACEF(genericLoggerp, "Resetting sparse array at %p", mySparseArrayp);
    GENERICSPARSEARRAY_RESET(mySparseArrayp, myContextp);
  }
  
  GENERICLOGGER_INFOF(genericLoggerp, "return %d", rci);
  GENERICLOGGER_FREE(genericLoggerp);

  return rci;
}

/*********************************************************************/
static int mySparseArrayIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayIndFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  int              rci = -1;

  GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Computing index of object of type %d pointed by %p", funcs, (int) itemType, pp);

  switch (itemType) {
  case GENERICSTACKITEMTYPE_INT:
    {
      int *ip = (int *) pp;
      int  i = *ip;

      GENERICLOGGER_TRACEF(genericLoggerp, "[%s] i is %d", funcs, i);
      rci = i % 50;
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Index %d", funcs, (int) rci);
    }
    break;
  default:
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Unsupported object type %d", funcs, (int) itemType);
    break;
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Return %d", funcs, (int) rci);
  return rci;
}

/*********************************************************************/
static short mySparseArrayKeyCmpFunction(void *userDatavp, void **pp1, void **pp2)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayKeyCmpFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  short            rcb = 0;
  myContext_t     *c1p = (myContext_t *) *pp1;
  myContext_t     *c2p = (myContext_t *) *pp2;

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Doing a comparison of pointers %p and %p", funcs, c1p, c2p);

  if ((c1p != NULL) && (c2p != NULL)) {
    rcb = (c1p->genericLoggerp == c2p->genericLoggerp);
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return %d", funcs, (int) rcb);
  return rcb;
}

/*********************************************************************/
static void *mySparseArrayKeyCopyFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayKeyCopyFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  myContext_t     *cp = (myContext_t *) *pp;
  myContext_t     *c = NULL;

  if (cp != NULL) {
    c = malloc(sizeof(myContext_t));
    if (c == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "... [%s] malloc error, %s", funcs, strerror(errno));
    } else {
      c->genericLoggerp = cp->genericLoggerp;
    }
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return %p", funcs, c);
  return c;
}

/*********************************************************************/
static void *mySparseArrayValCopyFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayValCopyFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  myContext_t     *cp = (myContext_t *) *pp;
  myContext_t     *c = NULL;

  if (cp != NULL) {
    c = malloc(sizeof(myContext_t));
    if (c == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "... [%s] malloc error, %s", funcs, strerror(errno));
    } else {
      c->genericLoggerp = cp->genericLoggerp;
    }
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return %p", funcs, c);
  return c;
}

/*********************************************************************/
static void  mySparseArrayKeyFreeFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayKeyFreeFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  myContext_t     *cp = (myContext_t *) *pp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;

  if (cp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Freeing malloced area %p", funcs, cp);
    free(cp);
  }
}

/*********************************************************************/
static void  mySparseArrayValFreeFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "mySparseArrayValFreeFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  myContext_t     *cp = (myContext_t *) *pp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;

  if (cp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Freeing malloced area %p", funcs, cp);
    free(cp);
  }
}

/*********************************************************************/
static void mySparseArrayDump(myContext_t *myContextp, genericSparseArray_t *mySparseArrayp)
/*********************************************************************/
{
  int i;
  int j;

  GENERICLOGGER_TRACE(myContextp->genericLoggerp, "... SparseArray Dump");
  for (i = 0; i < GENERICSTACK_USED(mySparseArrayp->keyStackp); i++) {
    genericStack_t *subKeyStackp;
    genericStack_t *subValStackp;
      
    if (! GENERICSTACK_IS_PTR(mySparseArrayp->keyStackp, i)) {
      continue;
    }
    subKeyStackp = GENERICSTACK_GET_PTR(mySparseArrayp->keyStackp, i);
    subValStackp = GENERICSTACK_GET_PTR(mySparseArrayp->valStackp, i);

    for (j = 0; j < GENERICSTACK_USED(subKeyStackp); j++) {
      int key;
      myContext_t *val;

      if (! GENERICSTACK_IS_INT(subKeyStackp, j)) {
	continue;
      }
      key = GENERICSTACK_GET_INT(subKeyStackp, j);
      val = GENERICSTACK_GET_PTR(subValStackp, j);
      GENERICLOGGER_TRACEF(myContextp->genericLoggerp, "... [Row/Col=%d/%d] %d => %p", (int) i, (int) j, key, val);
    }
  }
}

