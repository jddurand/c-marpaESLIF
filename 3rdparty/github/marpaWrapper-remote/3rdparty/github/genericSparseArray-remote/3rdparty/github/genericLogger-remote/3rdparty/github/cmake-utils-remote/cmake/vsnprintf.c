#if HAVE_STDIO_H
#include <stdio.h>
#endif

int main() {
  char *p;
  va_list ap;
  C_VSNPRINTF(p, 1, "", ap);
  return 0;
}
