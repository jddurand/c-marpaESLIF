#include <stdio.h>
#include <stdlib.h>
#include "genericStack.h"

void myFunction1(genericStack_t *myStackp);
void myFunction2(genericStack_t *myStackp);
void myFunction3(genericStack_t *myStackp);

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

  GENERICSTACK_NEW(myStackp); if (GENERICSTACK_ERROR(myStackp)) { return 1; }

  printf("\NEW interface:\n");
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

  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  myFunction1(myStackp);

  printf("\nSET interface:\n");
  printf("-------------:\n\n");
  
  printf("[10] float : 50\n"); GENERICSTACK_SET_FLOAT(myStackp, 50, 10); if (GENERICSTACK_ERROR(myStackp)) { return 1; }
  printf("[16] N/A\n"); GENERICSTACK_SET_NA(myStackp, 16); if (GENERICSTACK_ERROR(myStackp)) { return 1; }

  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  myFunction2(myStackp);
  myFunction3(myStackp);

  return 0;
}

void myFunction1(genericStack_t *myStackp) {
  myStruct1_t *myStruct1p;
  myStruct2_t *myStruct2p;
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[ 1] double   : %f\n", (float) GENERICSTACK_GET_DOUBLE(myStackp, 1));
  printf("[ 0] int      : %d\n",         GENERICSTACK_GET_INT   (myStackp, 0));
  printf("[ 3] short    : %d\n",         GENERICSTACK_GET_SHORT (myStackp, 3));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_PTR (myStackp, 5))->s);
  printf("[ 4] myStruct1: {%d}\n",         ((myStruct1_t *) GENERICSTACK_GET_PTR (myStackp, 4))->i);
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 2));
  
  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

  printf("\nPOP interface:\n");
  printf("-------------:\n\n");
  myStruct2p = (myStruct2_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", myStruct2p->i, myStruct2p->s);
  myStruct1p = (myStruct1_t *) GENERICSTACK_POP_PTR(myStackp);
  printf("[ 4] myStruct1: {%d}\n", myStruct1p->i);
  printf("[ 3] short : %d\n", (int)   GENERICSTACK_POP_SHORT(myStackp));
  printf("[ 2] float : %f\n",         GENERICSTACK_POP_FLOAT(myStackp));
  printf("[ 1] double: %f\n", (float) GENERICSTACK_POP_DOUBLE(myStackp));
  printf("[ 0] int   : %d\n",         GENERICSTACK_POP_INT(myStackp));

  printf("Current use/size: %d/%d\n", GENERICSTACK_USED(myStackp), GENERICSTACK_SIZE(myStackp));

}

void myFunction2(genericStack_t *myStackp) {
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[10] float : %f\n",         GENERICSTACK_GET_FLOAT (myStackp, 10));
}

void myFunction3(genericStack_t *myStackp) {
  
  printf("\nFREE interface:\n");
  printf("--------------:\n\n");

  GENERICSTACK_FREE(myStackp);
}

