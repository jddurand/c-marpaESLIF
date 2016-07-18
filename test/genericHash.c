#include <stdlib.h>
#include "genericHash.h"
#include <genericLogger.h>

typedef struct myContext {
  genericLogger_t *genericLoggerp;
} myContext_t;

static size_t myHashIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp);
static short  myHashKeyCmpFunction(void *userDatavp, void *p1, void *p2);
static void  *myHashCopyFunction(void *userDatavp, void *p);
static void   myHashFreeFunction(void *userDatavp, void *p);
static int    myHashTest(short withAllocb);

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
  genericHash_t   *myHashp;
  short            findResultb;
  short            removeResultb;

  myContext.genericLoggerp = genericLoggerp;

  GENERICHASH_NEW(myHashp, myHashIndFunction);
  if (GENERICHASH_ERROR(myHashp)) {
    GENERICLOGGER_ERROR(genericLoggerp, "Error when creating generic hash");
    rci = 1;
    goto err;
  }
  if (withAllocb) {
    GENERICHASH_KEYCMPFUNCTION(myHashp) = myHashKeyCmpFunction;
    GENERICHASH_KEYCOPYFUNCTION(myHashp) = myHashCopyFunction;
    GENERICHASH_KEYFREEFUNCTION(myHashp) = myHashFreeFunction;
    GENERICHASH_VALCOPYFUNCTION(myHashp) = myHashCopyFunction;
    GENERICHASH_VALFREEFUNCTION(myHashp) = myHashFreeFunction;
  }
  GENERICLOGGER_TRACEF(genericLoggerp, "Created hash at %p, alloc mode=%d", myHashp, (int) withAllocb);

  /* hash->{myContextp} = myContextp */
  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
  /* hash->{myContextp} = myContextp */
  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
  /* hash->{myContextp} = myContextp */
  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
  GENERICLOGGER_TRACEF(genericLoggerp, "... Inserted PTR %p indexed by itself", myContextp);

  /* hash->{NULL} = NULL */
  GENERICHASH_SET(myHashp, myContextp, PTR, NULL, PTR, NULL);
  GENERICLOGGER_TRACE(genericLoggerp, "... Inserted NULL indexed by NULL");

  GENERICHASH_SET(myHashp, myContextp, PTR, myContextp, PTR, myContextp);
  GENERICLOGGER_TRACEF(genericLoggerp, "... Inserted PTR %p without using IND", myContextp);
  GENERICHASH_SET_BY_IND(myHashp, myContextp, PTR, myContextp, PTR, myContextp, myHashIndFunction(myContextp, GENERICSTACKITEMTYPE_PTR, &myContextp));
  GENERICLOGGER_TRACEF(genericLoggerp, "... Inserted PTR %p using IND", myContextp);

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
  GENERICHASH_FIND_BY_IND(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, findResultb, myHashIndFunction(myContextp, GENERICSTACKITEMTYPE_PTR, &myContextp));
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
  GENERICHASH_FIND(myHashp, myContextp, PTR, myContextp, PTR, &myContextFoundp, removeResultb);
  if (! removeResultb) {
    GENERICLOGGER_ERRORF(genericLoggerp, "... Failed to remove PTR %p", myContextp);
  } else {
    if (myContextp->genericLoggerp == myContextFoundp->genericLoggerp) {
      /* Because we are using shallow pointers in this test, no need to free */
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

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  GENERICLOGGER_TRACEF(genericLoggerp, "Freeing hash at %p", myHashp);
  GENERICHASH_FREE(myHashp, myContextp);
  
  GENERICLOGGER_INFOF(genericLoggerp, "exit %d", rci);
  GENERICLOGGER_FREE(genericLoggerp);

  return rci;
}

/*********************************************************************/
static size_t myHashIndFunction(void *userDatavp, genericStackItemType_t itemType, void **pp)
/*********************************************************************/
{
  char             funcs[] = "myHashIndFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  size_t           rcl = (size_t)-1;

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Computing index of object of type %d pointed by %p", funcs, (int) itemType, pp);

  switch (itemType) {
  case GENERICSTACKITEMTYPE_PTR:
    {
      void *p = *pp;
      int   absi;

      GENERICLOGGER_TRACEF(genericLoggerp, "... ... [%s] PTR content is %p", funcs, p);
      absi = abs((int) p);
      rcl = (size_t) (absi % 50);
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
static short myHashKeyCmpFunction(void *userDatavp, void *p1, void *p2)
/*********************************************************************/
{
  char             funcs[] = "myHashKeyCmpFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  short            rcb = 0;
  myContext_t     *c1 = (myContext_t *) p1;
  myContext_t     *c2 = (myContext_t *) p2;

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Doing a comparison of pointers %p and %p", funcs, p1, p2);

  if ((c1 != NULL) && (c2 != NULL)) {
    rcb = (c1->genericLoggerp == c2->genericLoggerp);
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return %d", funcs, (int) rcb);
  return rcb;
}

/*********************************************************************/
static void  *myHashCopyFunction(void *userDatavp, void *p)
/*********************************************************************/
{
  char             funcs[] = "myHashCopyFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  myContext_t     *c = malloc(sizeof(myContext_t));

  if (c == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "... [%s] malloc error, %s", funcs, strerror(errno));
  } else {
    c->genericLoggerp = myContextp->genericLoggerp;
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "... [%s] Return malloced area %p", funcs, c);
  return c;
}

/*********************************************************************/
static void  myHashFreeFunction(void *userDatavp, void *p)
/*********************************************************************/
{
  char             funcs[] = "myHashFreeFunction";
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;

  GENERICLOGGER_ERRORF(genericLoggerp, "... [%s] Freeing malloced area %p", funcs, p);
  free(p);
}
