#include <stdio.h>
#include <stdlib.h>
#include "genericStack.h"

void myFunction(int arg1, GENERICSTACK_DECL(myStack), int argn);

int main() {
  GENERICSTACK_DECL(myStack);

  GENERICSTACK_NEW(myStack);
  GENERICSTACK_PUSH_INT(myStack, 1);
  GENERICSTACK_PUSH_FLOAT(myStack, 2);

  myFunction(-1, myStack, 999);

  return 0;
}

void myFunction(int arg1, GENERICSTACK_DECL(myStack), int argn) {
  printf("arg1: %d\n", arg1);

  printf("float: %f\n", GENERICSTACK_POP_FLOAT(myStack));
  printf("int: %d\n", GENERICSTACK_POP_INT(myStack));
  printf("argn: %d\n", argn);

  GENERICSTACK_FREE(myStack);
}

