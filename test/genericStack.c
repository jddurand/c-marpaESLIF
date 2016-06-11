#include <stdio.h>
#include <stdlib.h>
#include "genericStack.h"

void myFunction1(int arg1, GENERICSTACK_DECL(myStack), int argn);
void myFunction2(int arg1, GENERICSTACK_DECL(myStack), int argn);
void myFunction3(int arg1, GENERICSTACK_DECL(myStack), int argn);

typedef struct myStruct1 {
  int i;
} myStruct1_t;

typedef struct myStruct2 {
  int i;
  char *s;
} myStruct2_t;

void *myStruct2Clone(void *src) {
  void *p = malloc(sizeof(myStruct2_t));
  memcpy(p, src, sizeof(myStruct2_t));
  ((myStruct2_t *) p)->s = strdup(((myStruct2_t *) src)->s);
  return p;
}

void myStruct2Free(void *src) {
  free(((myStruct2_t *) src)->s);
  free(src);
}

int main() {
  myStruct1_t myStruct1 = { 50 };
  myStruct2_t myStruct2 = { 60, "70" };
  
  GENERICSTACK_DECL(myStack);

  printf("\nNEW interface:\n");
  printf("-------------:\n\n");
  
  GENERICSTACK_NEW_SIZED(myStack, 100);
  if (GENERICSTACK_ERROR(myStack)) { return 1; }
  
  printf("\nPUSH interface:\n");
  printf("--------------:\n\n");
  
  printf("[ 0] int      : 10\n");   GENERICSTACK_PUSH_INT   (myStack, 10);
  printf("[ 1] double   : 20\n");   GENERICSTACK_PUSH_DOUBLE(myStack, 20);
  printf("[ 2] float    : 30\n");   GENERICSTACK_PUSH_FLOAT (myStack, 30);
  printf("[ 3] short    : 40\n");   GENERICSTACK_PUSH_SHORT (myStack, 40);
  printf("[ 4] myStruct1: {%d}\n", myStruct1.i);
  GENERICSTACK_PUSH_ANY(myStack, &myStruct1, NULL, NULL);
  if (GENERICSTACK_ERROR(myStack)) { return 1; }
  printf("[ 5] myStruct2: {%d,\"%s\"}\n", myStruct2.i, myStruct2.s);
  GENERICSTACK_PUSH_ANY(myStack, &myStruct2, myStruct2Clone, myStruct2Free);
  if (GENERICSTACK_ERROR(myStack)) { return 1; }

  myFunction1(-1, myStack, 999);

  printf("\nSET interface:\n");
  printf("-------------:\n\n");
  
  printf("[10] float : 50\n"); GENERICSTACK_SET_FLOAT(myStack, 50, 10);
  if (GENERICSTACK_ERROR(myStack)) { return 1; }

  myFunction2(-1, myStack, 999);
  myFunction3(-1, myStack, 999);

  return 0;
}

void myFunction1(int arg1, GENERICSTACK_DECL(myStack), int argn) {
  myStruct1_t *myStruct1p;
  myStruct2_t *myStruct2p;
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[ 1] double   : %f\n", (float) GENERICSTACK_GET_DOUBLE(myStack, 1));
  printf("[ 0] int      : %d\n",         GENERICSTACK_GET_INT   (myStack, 0));
  printf("[ 3] short    : %d\n",         GENERICSTACK_GET_SHORT (myStack, 3));
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", ((myStruct2_t *) GENERICSTACK_GET_ANY (myStack, 5))->i, ((myStruct2_t *) GENERICSTACK_GET_ANY (myStack, 5))->s);
  printf("[ 4] myStruct1: {%d}\n",         ((myStruct1_t *) GENERICSTACK_GET_ANY (myStack, 4))->i);
  printf("[ 2] float    : %f\n",         GENERICSTACK_GET_FLOAT (myStack, 2));
  
  printf("\nPOP interface:\n");
  printf("-------------:\n\n");
  myStruct2p = (myStruct2_t *) GENERICSTACK_POP_ANY(myStack);
  printf("[ 5] myStruct2: {%d, \"%s\"}\n", myStruct2p->i, myStruct2p->s);
  myStruct1p = (myStruct1_t *) GENERICSTACK_POP_ANY(myStack);
  printf("[ 4] myStruct1: {%d}\n", myStruct1p->i);
  printf("[ 3] short : %d\n", (int)   GENERICSTACK_POP_SHORT(myStack));
  printf("[ 2] float : %f\n",         GENERICSTACK_POP_FLOAT(myStack));
  printf("[ 1] double: %f\n", (float) GENERICSTACK_POP_DOUBLE(myStack));
  printf("[ 0] int   : %d\n",         GENERICSTACK_POP_INT(myStack));
}

void myFunction2(int arg1, GENERICSTACK_DECL(myStack), int argn) {
  
  printf("\nGET interface:\n");
  printf("-------------:\n\n");
  printf("[10] float : %f\n",         GENERICSTACK_GET_FLOAT (myStack, 10));
}

void myFunction3(int arg1, GENERICSTACK_DECL(myStack), int argn) {
  
  printf("\nFREE interface:\n");
  printf("--------------:\n\n");

  GENERICSTACK_FREE(myStack);
}

