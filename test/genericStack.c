#include <stdio.h>
#include <stdlib.h>
#include "genericStack.h"

void myFunction(int arg1, GENERICSTACK_DECL(myStack), int argn);

int main() {
  GENERICSTACK_DECL(myStack);

  GENERICSTACK_NEW(myStack);
  GENERICSTACK_PUSH_INT(myStack, 1);
  GENERICSTACK_PUSH_DOUBLE(myStack, 2);
  GENERICSTACK_SET_FLOAT(myStack, 3, 2);

  myFunction(-1, myStack, 999);

  return 0;
}

void myFunction(int arg1, GENERICSTACK_DECL(myStack), int argn) {
  printf("arg1: %d\n", arg1);
  {
    float f = GENERICSTACK_POP_FLOAT(myStack);
    printf("float: %f\n", f);
  }
  {
    float f = GENERICSTACK_POP_DOUBLE(myStack);
    printf("float: %f\n", f);
  }
  printf("int: %d\n", GENERICSTACK_POP_INT(myStack));
  printf("argn: %d\n", argn);

  GENERICSTACK_FREE(myStack);
}

