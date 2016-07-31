#include <stdio.h>
#include <stdlib.h>
#include "genericStack.h"

short myFunction1(genericStack_t *myStackp);
short myFunction2(genericStack_t *myStackp);
short myFunction3(genericStack_t *myStackp);
short myFunction4(genericStack_t *myStackp);

typedef struct myStruct1 {
  int i;
} myStruct1_t;

typedef struct myStruct2 {
  int i;
  char *s;
} myStruct2_t;

int main() {
  myStruct1_t     myStruct1 = { 50 };
  myStruct2_t     myStruct2 = { 60, "70" };
  genericStack_t *myStackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;

  GENERICSTACKITEMTYPE_ARRAY_LENGTH(array) = 10;
  GENERICSTACKITEMTYPE_ARRAY_PTR(array) = malloc(GENERICSTACKITEMTYPE_ARRAY_LENGTH(array));

  GENERICSTACK_NEW(myStackp); if (GENERICSTACK_ERROR(myStackp)) { return 1; }

  printf("\nNEW interface:\n");
  printf("--------------:\n\n");
  printf("Initial use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("\nPUSH interface:\n");
  printf("--------------:\n\n");
  
  printf("[ 0] int      : 10\n");                                    GENERICSTACK_PUSH_INT   (myStackp, 10);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 1] double   : 20\n");                                    GENERICSTACK_PUSH_DOUBLE(myStackp, 20);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 2] float    : 30\n");                                    GENERICSTACK_PUSH_FLOAT (myStackp, 30);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 3] short    : 40\n");                                    GENERICSTACK_PUSH_SHORT (myStackp, 40);      if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 4] myStruct1: {%d}\n", myStruct1.i);                     GENERICSTACK_PUSH_PTR(myStackp, &myStruct1); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 5] myStruct2: {%d,\"%s\"}\n", myStruct2.i, myStruct2.s); GENERICSTACK_PUSH_PTR(myStackp, &myStruct2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 6] N/A\n");                                              GENERICSTACK_PUSH_NA    (myStackp);          if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 7] Array\n");                                            GENERICSTACK_PUSH_ARRAY (myStackp, array);   if (GENERICSTACK_ERROR(myStackp)) { return 1; }

  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  if (myFunction1(myStackp) == 0) { return 1; }

  printf("\nSET interface:\n");
  printf("-------------:\n\n");
  
  printf("[10] float : 50\n"); GENERICSTACK_SET_FLOAT(myStackp, 50, 10); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[16] N/A\n"); GENERICSTACK_SET_NA(myStackp, 16); if (GENERICSTACK_ERROR(myStackp)) { return 1; }

  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  if (myFunction2(myStackp) == 0) { return 1; }
  if (myFunction3(myStackp) == 0) { return 1; }
  if (myFunction4(myStackp) == 0) { return 1; }

  return 0;
}

short myFunction1(genericStack_t *myStackp) {
  myStruct1_t *myStruct1p;
  myStruct2_t *myStruct2p;
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[ 1] double   : %f\n", (float) GENERICSTACK_GET_DOUBLE(myStackp, 1));
  printf("[ 0] int      : %d\n",         GENERICSTACK_GET_INT   (myStackp, 0));
  printf("[ 3] short    : %d\n",         GENERICSTACK_GET_SHORT (myStackp, 3));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);
  printf("[ 4] myStruct1: {%d}\n",         ((myStruct1_t *) GENERICSTACK_GET_PTR (myStackp, 4))->i);
  printf("[ 7] Array    : {%p,%d}\n",    GENERICSTACKITEMTYPE_ARRAY_PTR(GENERICSTACK_GET_ARRAY(myStackp, 7)), GENERICSTACKITEMTYPE_ARRAY_LENGTH(GENERICSTACK_GET_ARRAY(myStackp, 7)));
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 2));
  
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("\nSWITCH interface:\n");
  printf("------------------:\n\n");
  GENERICSTACK_SWITCH(myStackp, 5, 2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 2] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 2))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 2))->s);
  printf("[ 5] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 5));

  printf("\n...SWITCH again:\n");
  GENERICSTACK_SWITCH(myStackp, 5, 2); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 2));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);

  fprintf(stderr, "GENERICSTACK_USED(myStackp)=%d\n", (int) GENERICSTACK_USED(myStackp));

  printf("\nPOP interface:\n");
  printf("-------------:\n\n");
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));
  {
    GENERICSTACKITEMTYPE2TYPE_ARRAY array;
    printf("[ 7] POP Array\n");  array = GENERICSTACK_POP_ARRAY(myStackp);
    printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));
    free(GENERICSTACKITEMTYPE_ARRAY_PTR(array));
  }
  printf("[ 6] POP N/A\n");  GENERICSTACK_POP_NA(myStackp);
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  myStruct2p = (myStruct2_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 5] POP myStruct2: {%d, \"%s\"}\n", myStruct2p->i, myStruct2p->s);
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  myStruct1p = (myStruct1_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 4] POP myStruct1: {%d}\n", myStruct1p->i);
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("[ 3] POP short : %d\n", (int)   GENERICSTACK_POP_SHORT(myStackp));
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("[ 2] POP float : %f\n",         GENERICSTACK_POP_FLOAT(myStackp));
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("[ 1] POP double: %f\n", (float) GENERICSTACK_POP_DOUBLE(myStackp));
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("[ 0] POP int   : %d\n",         GENERICSTACK_POP_INT(myStackp));
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  return 1;
}

short myFunction2(genericStack_t *myStackp) {
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[10] float : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 10));

  return 1;
}

short myFunction3(genericStack_t *myStackp) {
  short rcb = 1;
  
  printf("\nEXISTS and IF interfaces:\n");
  printf("--------------------------:\n\n");
  printf("EXISTS : %s\n",                     (rcb = GENERICSTACK_EXISTS(myStackp, 10)    ? 1 : 0) ? "OK" : "KO");
  if (rcb != 0) { printf("IS_ARRAY : %s\n",   (rcb = GENERICSTACK_IS_ARRAY(myStackp, 10)  ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_NA : %s\n",      (rcb = GENERICSTACK_IS_NA(myStackp, 10)     ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_CHAR : %s\n",    (rcb = GENERICSTACK_IS_CHAR(myStackp, 10)   ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_SHORT : %s\n",   (rcb = GENERICSTACK_IS_SHORT(myStackp, 10)  ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_INT : %s\n",     (rcb = GENERICSTACK_IS_INT(myStackp, 10)    ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_LONG : %s\n",    (rcb = GENERICSTACK_IS_LONG(myStackp, 10)   ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_FLOAT : %s\n",   (rcb = GENERICSTACK_IS_FLOAT(myStackp, 10)  ? 1 : 0) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_DOUBLE : %s\n",  (rcb = GENERICSTACK_IS_DOUBLE(myStackp, 10) ? 0 : 1) ? "OK" : "KO"); }
  if (rcb != 0) { printf("IS_PTR : %s\n",     (rcb = GENERICSTACK_IS_PTR(myStackp, 10)    ? 0 : 1) ? "OK" : "KO"); }

  return rcb;
}

short myFunction4(genericStack_t *myStackp) {
  
  printf("\nFREE interface:\n");
  printf("--------------:\n\n");

  GENERICSTACK_FREE(myStackp);

  return 1;
}

