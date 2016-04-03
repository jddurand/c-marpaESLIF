#if HAVE_STDIO_H
#include <stdio.h>
#endif
int main() {
  C_FILENO(stdin);
  return 0;
}
