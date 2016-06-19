#include "genericHash.h"
#include <genericLogger.h>

typedef struct myContext {
  genericLogger_t *genericLoggerp;
} myContext_t;

static size_t myHashFunction(void *userDatavp, genericStackItemType_t itemType, void *p);
static short  myHashCmpAnyFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);

int main(int argc, char **argv) {
  myContext_t      myContext;
  myContext_t     *myContextp = &myContext;
  int              rci = 0;
  genericLogger_t *genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  GENERICHASH_DECL(myHashp);
  short            findResultb;

  myContext.genericLoggerp = genericLoggerp;

  GENERICHASH_NEW(myHashp, myHashFunction);
  if (GENERICHASH_ERROR(myHashp)) {
    GENERICLOGGER_ERROR(genericLoggerp, "Error when creating generic hash");
    rci = 1;
    goto err;
  }
  GENERICLOGGER_TRACEF(genericLoggerp, "Created hash at %p", myHashp);

  GENERICHASH_INSERT(myHashp, myContextp, PTR, myContextp);
  GENERICHASH_INSERT(myHashp, myContextp, PTR, myContextp);
  GENERICHASH_INSERT(myHashp, myContextp, PTR, myContextp);
  GENERICLOGGER_TRACEF(genericLoggerp, "... Inserted PTR %p", myContextp);

  GENERICLOGGER_TRACEF(genericLoggerp, "... Looking for PTR %p", myContextp);
  GENERICHASH_FIND(myHashp, myContextp, PTR, myContextp, myHashCmpAnyFunction, findResultb);
  if (! findResultb) {
    GENERICLOGGER_ERRORF(genericLoggerp, "... Failed to find PTR %p", myContextp);
  } else {
    GENERICLOGGER_ERRORF(genericLoggerp, "... Success searching for PTR %p", myContextp);
  }

  rci = 0;
  goto done;

 err:
  rci = 1;

 done:
  GENERICLOGGER_TRACEF(genericLoggerp, "Freeing hash at %p", myHashp);
  GENERICHASH_FREE(myHashp);
  
  GENERICLOGGER_INFOF(genericLoggerp, "exit %d", rci);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(rci);
}

static size_t myHashFunction(void *userDatavp, genericStackItemType_t itemType, void *p) {
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  size_t           rcl = (size_t)-1;

  GENERICLOGGER_TRACEF(genericLoggerp, "... Computing index of object of type %d pointed by %p", (int) itemType, p);

  switch (itemType) {
  case GENERICSTACKITEMTYPE_PTR:
    {
      void **pp = (void **) p;
      int    absi;

      p = *pp;

      GENERICLOGGER_TRACEF(genericLoggerp, "... ... PTR content is %p", p);
      absi = abs((int) p);
      rcl = (size_t) (absi % 50);
      GENERICLOGGER_TRACEF(genericLoggerp, "... ... Index %d", (int) rcl);
    }
    break;
  default:
    GENERICLOGGER_TRACEF(genericLoggerp, "... Unsupported object type %d", (int) itemType);
    break;
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "... Return %d", (int) rcl);
  return rcl;
}

static short  myHashCmpAnyFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2) {
  myContext_t     *myContextp = (myContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = myContextp->genericLoggerp;
  short            rcb = 0;

  GENERICLOGGER_TRACEF(genericLoggerp, "... Doing a comparison of objects of type %d pointed by %p and %p", (int) itemType, p1, p2);

  switch (itemType) {
  case GENERICSTACKITEMTYPE_PTR:
    {
      void **pp1 = (void **) p1;
      void **pp2 = (void **) p2;

      GENERICLOGGER_TRACEF(genericLoggerp, "... ... PTR contents are %p and %p", *pp1, *pp2);
      rcb = (*pp1 == *pp2);
    }
    break;
  default:
    GENERICLOGGER_TRACEF(genericLoggerp, "... Unsupported object type %d", (int) itemType);
    break;
  }
  
  GENERICLOGGER_TRACEF(genericLoggerp, "... Return %d", (int) rcb);
  return rcb;
}

