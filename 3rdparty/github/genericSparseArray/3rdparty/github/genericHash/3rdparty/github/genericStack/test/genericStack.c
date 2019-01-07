#include <stdio.h>
#include <stdlib.h>

typedef struct myStruct {
  short isstring;
  union {
    int   i;
    char *s;
  } u;
} myStruct_t;

#define GENERICSTACK_CUSTOM myStruct_t
#include "genericStack.h"

static short myFunction1(genericStack_t *myStackp);
static short myFunction2(genericStack_t *myStackp);
static short subMain(genericStack_t *myStackp);

typedef struct myStruct1 {
  int i;
} myStruct1_t;

typedef struct myStruct2 {
  int i;
  char *s;
} myStruct2_t;

int main(int argc, char **argv) {
  genericStack_t  stackOnStack;
  genericStack_t *myStackp;

  printf("==================:\n");
  printf("STACK on the stack:\n");
  printf("==================:\n\n");
  myStackp = &stackOnStack;
  GENERICSTACK_INIT_SIZED(myStackp, 65536); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  if (subMain(myStackp)) {
    GENERICSTACK_RESET(myStackp);
    return 1;
  }
  printf("\nRESET interface:\n");
  printf("---------------:\n\n");
  GENERICSTACK_RESET(myStackp);
  
  printf("=================:\n");
  printf("STACK on the heap:\n");
  printf("=================:\n\n");
  GENERICSTACK_NEW_SIZED(myStackp, 65536); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  if (subMain(myStackp)) {
    GENERICSTACK_RESET(myStackp);
    return 1;
  }
  printf("\nFREE interface:\n");
  printf("----------------:\n\n");
  GENERICSTACK_FREE(myStackp);

  return 0;
}

static short subMain(genericStack_t *myStackp) {
  myStruct1_t     myStruct1 = { 50 };
  myStruct2_t     myStruct2 = { 60, "70" };
  myStruct_t      myCustomStruct;
  myStruct_t      myCustomStruct2;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array2;

  GENERICSTACK_ARRAY_LENGTH(array) = 10;
  GENERICSTACK_ARRAY_PTR(array) = malloc(GENERICSTACK_ARRAY_LENGTH(array));

  GENERICSTACK_ARRAYP_LENGTH(&array2) = 20;
  GENERICSTACK_ARRAYP_PTR(&array2) = malloc(GENERICSTACK_ARRAYP_LENGTH(&array2));

  myCustomStruct.isstring = 1;
  myCustomStruct.u.s      = strdup("OK");

  myCustomStruct2.isstring = 1;
  myCustomStruct2.u.s      = strdup("OK2");

  GENERICSTACK_ERROR_RESET(myStackp);  /* Not meaningful here, but just to test it */

  printf("\nUSED/LENGTH interface:\n");
  printf("--------------:\n\n");
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("\nPUSH interface:\n");
  printf("--------------:\n\n");
  
  printf("[ 0] int      : 10\n");                                    GENERICSTACK_PUSH_INT   (myStackp, 10);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 1] double   : 20\n");                                    GENERICSTACK_PUSH_DOUBLE(myStackp, 20);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 2] float    : 30\n");                                    GENERICSTACK_PUSH_FLOAT (myStackp, 30);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 3] short    : 40\n");                                    GENERICSTACK_PUSH_SHORT (myStackp, 40);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 4] myStruct1: {%d}\n", myStruct1.i);                     GENERICSTACK_PUSH_PTR(myStackp, &myStruct1); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 5] myStruct2: {%d,\"%s\"}\n", myStruct2.i, myStruct2.s); GENERICSTACK_PUSH_PTR(myStackp, &myStruct2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 6] N/A\n");                                              GENERICSTACK_PUSH_NA    (myStackp);          if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 7] Array\n");                                            GENERICSTACK_PUSH_ARRAY (myStackp, array);   if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 8] Arrayp\n");                                           GENERICSTACK_PUSH_ARRAYP(myStackp, &array2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 9] Custom\n");                                           GENERICSTACK_PUSH_CUSTOM(myStackp, myCustomStruct); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[10] Customp\n");                                          GENERICSTACK_PUSH_CUSTOMP(myStackp, &myCustomStruct2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  if (myFunction1(myStackp) == 0) { return 1; }

  printf("\nSET interface:\n");
  printf("-------------:\n\n");
  
  GENERICSTACK_DUMP(myStackp);
  printf("[10] float : 50\n"); GENERICSTACK_SET_FLOAT(myStackp, 50, 10); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  GENERICSTACK_DUMP(myStackp);
  printf("[16] N/A\n"); GENERICSTACK_SET_NA(myStackp, 16); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  GENERICSTACK_DUMP(myStackp);
  printf("[17] CUSTOM\n"); GENERICSTACK_SET_CUSTOM(myStackp, myCustomStruct, 17); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  GENERICSTACK_DUMP(myStackp);
  printf("[18] CUSTOMP\n"); GENERICSTACK_SET_CUSTOMP(myStackp, &myCustomStruct2, 18); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  GENERICSTACK_DUMP(myStackp);

  if (myFunction2(myStackp) == 0) { return 1; }

  return 0;
}

static short myFunction1(genericStack_t *myStackp) {
  myStruct1_t *myStruct1p;
  myStruct2_t *myStruct2p;
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[ 1] double   : %f\n", (float) GENERICSTACK_GET_DOUBLE(myStackp, 1));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 0] int      : %d\n",         GENERICSTACK_GET_INT   (myStackp, 0));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 3] short    : %d\n",         GENERICSTACK_GET_SHORT (myStackp, 3));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 4] myStruct1: {%d}\n",         ((myStruct1_t *) GENERICSTACK_GET_PTR (myStackp, 4))->i);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 7] Array    : {%p,%d}\n",    GENERICSTACK_ARRAY_PTR(GENERICSTACK_GET_ARRAY(myStackp, 7)), (int) GENERICSTACK_ARRAY_LENGTH(GENERICSTACK_GET_ARRAY(myStackp, 7)));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 8] Arrayp   : {%p,%d}\n",    GENERICSTACK_ARRAYP_PTR(GENERICSTACK_GET_ARRAYP(myStackp, 8)), (int) GENERICSTACK_ARRAYP_LENGTH(GENERICSTACK_GET_ARRAYP(myStackp, 8)));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 2));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 9] Custom   : {isstring=%d,u.s=%s}\n", GENERICSTACK_GET_CUSTOM(myStackp, 9).isstring, GENERICSTACK_GET_CUSTOM(myStackp, 9).u.s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[10] Customp  : {isstring=%d,u.s=%s}\n", GENERICSTACK_GET_CUSTOMP(myStackp, 10)->isstring, GENERICSTACK_GET_CUSTOMP(myStackp, 10)->u.s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  
  printf("\nSWITCH interface (indices 2 (FLOAT) and 5 (PTR)):\n");
  printf("------------------------------------------------:\n\n");
  GENERICSTACK_SWITCH(myStackp, 5, 2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 2] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 2))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 2))->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 5] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 5));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("\n...SWITCH again:\n");
  GENERICSTACK_SWITCH(myStackp, 5, 2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 2));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("\nSWITCH interface (indices 5 (PTR) and 9 (CUSTOM)):\n");
  printf("-------------------------------------------------:\n\n");
  GENERICSTACK_SWITCH(myStackp, 5, 9); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 5] Custom   : {isstring=%d,u.s=%s}\n", GENERICSTACK_GET_CUSTOM(myStackp, 5).isstring, GENERICSTACK_GET_CUSTOM(myStackp, 5).u.s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 9] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 9))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 9))->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("\n...SWITCH again:\n");
  GENERICSTACK_SWITCH(myStackp, 5, 9); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  printf("[ 9] Custom   : {isstring=%d,u.s=%s}\n", GENERICSTACK_GET_CUSTOM(myStackp, 9).isstring, GENERICSTACK_GET_CUSTOM(myStackp, 9).u.s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("\nPOP interface:\n");
  printf("-------------:\n\n");
  printf("Current use/size: %d/%d\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp));
  {
    myStruct_t custom;
    printf("[10] POP Custom\n");  custom = GENERICSTACK_POP_CUSTOM(myStackp);
    printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
    /*
     * VOLONTARILY NOT free'ed, because re-used in myFunction2
    free(custom.u.s);
    */
  }
  {
    myStruct_t custom;
    printf("[ 9] POP Custom\n");  custom = GENERICSTACK_POP_CUSTOM(myStackp);
    printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
    /*
     * VOLONTARILY NOT free'ed, because re-used in myFunction2
    free(custom.u.s);
    */
  }
  {
    GENERICSTACKITEMTYPE2TYPE_ARRAY array;
    printf("[ 8] POP Array\n");  array = GENERICSTACK_POP_ARRAY(myStackp);
    printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  {
    GENERICSTACKITEMTYPE2TYPE_ARRAY array;
    printf("[ 7] POP Array\n");  array = GENERICSTACK_POP_ARRAY(myStackp);
    printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  printf("[ 6] POP N/A\n");  GENERICSTACK_POP_NA(myStackp);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  myStruct2p = (myStruct2_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 5] POP myStruct2: {%d, \"%s\"}\n", myStruct2p->i, myStruct2p->s);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  myStruct1p = (myStruct1_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 4] POP myStruct1: {%d}\n", myStruct1p->i);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("[ 3] POP short : %d\n", (int)   GENERICSTACK_POP_SHORT(myStackp));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("[ 2] POP float : %f\n",         GENERICSTACK_POP_FLOAT(myStackp));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("[ 1] POP double: %f\n", (float) GENERICSTACK_POP_DOUBLE(myStackp));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  printf("[ 0] POP int   : %d\n",         GENERICSTACK_POP_INT(myStackp));
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));

  return 1;
}

static short myFunction2(genericStack_t *myStackp) {
  short rcb = 1;
  
  printf("\nEXISTS interface:\n");
  printf("----------------:\n\n");
  printf("EXISTS : %s\n",                     (rcb = GENERICSTACK_EXISTS(myStackp, 10)    ? 1 : 0) ? "OK" : "KO");
  if (rcb != 0) { printf("IS_ARRAY : %s\n",   (rcb = GENERICSTACK_IS_ARRAY(myStackp, 10)  ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_NA : %s\n",      (rcb = GENERICSTACK_IS_NA(myStackp, 10)     ? 0 : 1) ? "OK" : "KO"); }
  GENERICSTACK_DUMP(myStackp);
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_CHAR : %s\n",    (rcb = GENERICSTACK_IS_CHAR(myStackp, 10)   ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_SHORT : %s\n",   (rcb = GENERICSTACK_IS_SHORT(myStackp, 10)  ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_INT : %s\n",     (rcb = GENERICSTACK_IS_INT(myStackp, 10)    ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_LONG : %s\n",    (rcb = GENERICSTACK_IS_LONG(myStackp, 10)   ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_FLOAT : %s\n",   (rcb = GENERICSTACK_IS_FLOAT(myStackp, 10)  ? 1 : 0) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_DOUBLE : %s\n",  (rcb = GENERICSTACK_IS_DOUBLE(myStackp, 10) ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_PTR : %s\n",     (rcb = GENERICSTACK_IS_PTR(myStackp, 10)    ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_CUSTOM : %s\n",  (rcb = GENERICSTACK_IS_CUSTOM(myStackp, 17) ? 1 : 0) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb != 0) { printf("IS_LONG_DOUBLE : %s\n",    (rcb = GENERICSTACK_IS_LONG_DOUBLE(myStackp, 10)   ? 0 : 1) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb) {
    free(GENERICSTACK_GET_CUSTOM(myStackp, 17).u.s);
  }
  if (rcb != 0) { printf("IS_CUSTOM : %s\n",  (rcb = GENERICSTACK_IS_CUSTOM(myStackp, 18) ? 1 : 0) ? "OK" : "KO"); }
  printf("... Use/size: %d/%d (initial+heap=%d+%d)\n", (int) GENERICSTACK_USED(myStackp), (int) GENERICSTACK_LENGTH(myStackp), (int) GENERICSTACK_INITIAL_LENGTH(myStackp), (int) GENERICSTACK_HEAP_LENGTH(myStackp));
  if (rcb) {
    free(GENERICSTACK_GET_CUSTOM(myStackp, 18).u.s);
  }

  return rcb;
}
