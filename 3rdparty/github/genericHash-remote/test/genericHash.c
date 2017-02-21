#include <stdlib.h>
#include "genericHash.h"
#include <genericLogger.h>

typedef struct myContext {
  genericLogger_t *genericLoggerp;
} myContext_t;

static int    myHashIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp);
static short  myHashKeyCmpFunction(void *userDatavp, void **pp1, void **pp2);
static void  *myHashKeyCopyFunction(void *userDatavp, void **pp);
static void   myHashKeyFreeFunction(void *userDatavp, void **pp);
static void  *myHashValCopyFunction(void *userDatavp, void **pp);
static void   myHashValFreeFunction(void *userDatavp, void **pp);
static int    myHashTest(short withAllocb);
static void   myHashDump(myContext_t *myContextp, genericHash_t *myHashp);

int main(int argc, char **argv) {
  if (myHashTest(0) == 0) {
    return 1;
  }
  if (myHashTest(1) == 0) {
    return 1;
  }
  return 0;
}

static int myHashTest(short withAllocb) {
  myContext_t      myContext;
  myContext_t     *myContextp = &myContext;
  myContext_t     *myContextFoundp;
  int              rci = 0;
  genericLogger_t *genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  genericHash_t    myHash;
  genericHash_t   *myHashp;
  short            findResultb;
  short            removeResultb;
  int              i;

  myContext.genericLoggerp = genericLoggerp;

  if (withAllocb) {
    GENERICHASH_NEW(myHashp, myHashIndFunction);
  } else {
    myHashp = &myHash;
    GENERICHASH_INIT(myHashp, myHashIndFunction);
  }
  if (GENERICHASH_ERROR(myHashp)) {
    GENERICLOGGER_ERROR(genericLoggerp, "Error when creating generic hash");
    rci = 1;
    goto err;
  }
  if (withAllocb) {
    GENERICHASH_KEYCMPFUNCTION(myHashp)  = myHashKeyCmpFunction;
    GENERICHASH_KEYCOPYFUNCTION(myHashp) = myHashKeyCopyFunction;
    GENERICHASH_KEYFREEFUNCTION(myHashp) = myHashKeyFreeFunction;
    GENERICHASH_VALCOPYFUNCTION(myHashp) = myHashValCopyFunction;
    GENERICHASH_VALFREEFUNCTION(myHashp) = myHashValFreeFunction;
  }
  if (withAllocb) {
    GENERICLOGGER_TRACEF(genericLoggerp, "Created hash at %p, alloc mode=%d", myHashp, (int) withAllocb);
  } else {
    GENERICLOGGER_TRACEF(genericLoggerp, "Initialized hash at %p, alloc mode=%d", myHashp, (int) withAllocb);
  }

  GENERICHASH_SET(myHashp, myContextp, SHORT, 1001, PTR, myContextp);
  GENERICHASH_SET(myHashp, myContextp, SHORT, 1002, SHORT, 1003);
  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, SHORT, 1004);
  for (i = 0; i < 2; i++) {
    myHashDump(myContextp, myHashp);
  
    /* hash->{myContextp} = myContextp */
    GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "... Setted PTR %p indexed by itself", myContextp);
    myHashDump(myContextp, myHashp);
    /* hash->{myContextp} = myContextp */
    GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "... Setted again PTR %p indexed by itself", myContextp);
    myHashDump(myContextp, myHashp);
    /* hash->{myContextp} = myContextp */
    GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "... Setted again PTR %p indexed by itself", myContextp);
    myHashDump(myContextp, myHashp);

    /* hash->{NULL} = NULL */
    GENERICHASH_SET(myHashp, myContextp, PTR, NULL, PTR, NULL);
    GENERICLOGGER_TRACE(genericLoggerp, "... Setted NULL indexed by NULL");
    myHashDump(myContextp, myHashp);

    GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
    GENERICLOGGER_TRACEF(genericLoggerp, "... Setted again PTR %p indexed by itself", myContextp);
    myHashDump(myContextp, myHashp);
    GENERICHASH_SET_BY_IND(myHashp, myContextp, PTR, myContextp, PTR, myContextp, myHashIndFunction(myContextp, GENERICSTACKITEMTYPE_PTR, (void **) &myContextp));
    GENERICLOGGER_TRACEF(genericLoggerp, "... Setted again PTR %p using IND indexed by itself", myContextp);
    myHashDump(myContextp, myHashp);

    GENERICLOGGER_TRACEF(genericLoggerp, "... Looking for PTR %p", myContextp);
    GENERICHASH_FIND(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, findResultb);
    if (! findResultb) {
      GENERICLOGGER_ERRORF(genericLoggerp, "... Failed to find PTR %p in keys", myContextp);
    } else {
      if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success searching for PTR %p", myContextFoundp);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success searching for PTR but found a bad pointer %p", myContextFoundp);
      }
    }

    GENERICLOGGER_TRACEF(genericLoggerp, "... Looking for PTR %p using IND", myContextp);
    GENERICHASH_FIND_BY_IND(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, findResultb, myHashIndFunction(myContextp, GENERICSTACKITEMTYPE_PTR, (void **) &myContextp));
    if (! findResultb) {
      GENERICLOGGER_ERRORF(genericLoggerp, "... Failed to find PTR %p", myContextp);
    } else {
      if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success searching for PTR %p", myContextFoundp);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success searching for PTR but found a bad pointer %p", myContextFoundp);
      }
    }

    GENERICLOGGER_TRACEF(genericLoggerp, "... Removing PTR %p", myContextp);
    GENERICHASH_REMOVE(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, removeResultb);
    myHashDump(myContextp, myHashp);
    if (! removeResultb) {
      GENERICLOGGER_ERRORF(genericLoggerp, "... Failed to remove PTR %p", myContextp);
    } else {
      if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
	if (withAllocb) {
	  myHashValFreeFunction(myContextp, (void **) &myContextFoundp);
	}
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success removing PTR %p", myContextFoundp);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Success removing PTR but found a bad pointer %p", myContextFoundp);
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "... Looking again for PTR %p", myContextp);
      GENERICHASH_FIND(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, removeResultb);
      if (! removeResultb) {
	GENERICLOGGER_TRACEF(genericLoggerp, "... Failed to find PTR %p and this is ok", myContextp);
      } else {
	GENERICLOGGER_ERRORF(genericLoggerp, "... Unexpected success searching for PTR %p, got %p", myContextp, myContextFoundp);
      }
    }

    GENERICLOGGER_TRACE(genericLoggerp, ".........................");
    GENERICLOGGER_TRACE(genericLoggerp, "... Relaxing the hash ...");
    GENERICLOGGER_TRACE(genericLoggerp, ".........................");
    GENERICHASH_RELAX(myHashp, myContextp);
  }

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  if (withAllocb) {
    GENERICLOGGER_TRACEF(genericLoggerp, "Freeing hash at %p", myHashp);
    GENERICHASH_FREE(myHashp, myContextp);
  } else {
    GENERICLOGGER_TRACEF(genericLoggerp, "Resetting hash at %p", myHashp);
    GENERICHASH_RESET(myHashp, myContextp);
  }
  
  GENERICLOGGER_INFOF(genericLoggerp, "return %d", rci);
  GENERICLOGGER_FREE(genericLoggerp);

  return rci;
}

/*********************************************************************/
static int myHashIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashIndFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  int              rcl = -1;

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Computing index of object of type %d pointed by %p", funcs, (int) itemType, pp);

  switch (itemType) {
  case GENERICSTACKITEMTYPE_PTR:
    {
      myContext_t *cp = (myContext_t *) *pp;
      int  i  = (int) cp;
      int  absi;

      GENERICLOGGER_TRACEF(genericLoggerp, "... ... [%s] PTR is %p", funcs, cp);
      absi = abs(i);
      rcl = (absi % 50);
      GENERICLOGGER_TRACEF(genericLoggerp, "... ... [%s] Index %d", funcs, (int) rcl);
    }
    break;
  default:
    GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Unsupported object type %d", funcs, (int) itemType);
    break;
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return %d", funcs, (int) rcl);
  return rcl;
}

/*********************************************************************/
static short myHashKeyCmpFunction(void *userDatavp, void **pp1, void **pp2)
/*********************************************************************/
{
  char             funcs[] = "myHashKeyCmpFunction";
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
static void *myHashKeyCopyFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashKeyCopyFunction";
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
static void *myHashValCopyFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashValCopyFunction";
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
static void  myHashKeyFreeFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashKeyFreeFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  myContext_t     *cp = (myContext_t *) *pp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;

  if (cp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Freeing malloced area %p", funcs, cp);
    free(cp);
  }
}

/*********************************************************************/
static void  myHashValFreeFunction(void *userDatavp, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashValFreeFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  myContext_t     *cp = (myContext_t *) *pp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;

  if (cp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Freeing malloced area %p", funcs, cp);
    free(cp);
  }
}

/*********************************************************************/
static void myHashDump(myContext_t *myContextp, genericHash_t *myHashp)
/*********************************************************************/
{
  int i;
  int j;

  GENERICLOGGER_TRACEF(myContextp->genericLoggerp, "Hash Dump (used: %d)", GENERICHASH_USED(myHashp));
  for (i = 0; i < GENERICSTACK_USED(myHashp->keyStackp); i++) {
    genericStack_t *subKeyStackp;
    genericStack_t *subValStackp;
      
    if (! GENERICSTACK_IS_PTR(myHashp->keyStackp, i)) {
      continue;
    }
    subKeyStackp = GENERICSTACK_GET_PTR(myHashp->keyStackp, i);
    subValStackp = GENERICSTACK_GET_PTR(myHashp->valStackp, i);

    GENERICLOGGER_TRACEF(myContextp->genericLoggerp, "[Row=%d] Key used/length/initial length/heap length=%d/%d/%d/%d",
			 i,
			 GENERICSTACK_USED(subKeyStackp),
			 GENERICSTACK_LENGTH(subKeyStackp),
			 GENERICSTACK_INITIAL_LENGTH(subKeyStackp),
			 GENERICSTACK_HEAP_LENGTH(subKeyStackp));
    GENERICLOGGER_TRACEF(myContextp->genericLoggerp, "[Row=%d] Val used/length/initial length/heap length=%d/%d/%d/%d",
			 i,
			 GENERICSTACK_USED(subValStackp),
			 GENERICSTACK_LENGTH(subValStackp),
			 GENERICSTACK_INITIAL_LENGTH(subValStackp),
			 GENERICSTACK_HEAP_LENGTH(subValStackp));

    for (j = 0; j < GENERICSTACK_USED(subKeyStackp); j++) {
      myContext_t *key;
      myContext_t *val;

      if (! GENERICSTACK_IS_PTR(subKeyStackp, j)) {
	continue;
      }
      key = GENERICSTACK_GET_PTR(subKeyStackp, j);
      val = GENERICSTACK_GET_PTR(subValStackp, j);
      GENERICLOGGER_TRACEF(myContextp->genericLoggerp, "[Row/Col=%d/%d] %p => %p", (int) i, (int) j, key, val);
    }
  }
}

