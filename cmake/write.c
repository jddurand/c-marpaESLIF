#if HAVE_STDIO_H
#include <stdio.h>
#endif

int main() {
  C_WRITE(stderr, "", 1);
  return 0;
}
