#include <stdio.h>
#include <stdarg.h>

int main() {
  char *p;
  va_list ap;
  C_VSNPRINTF(p, 1, "", ap);
  return 0;
}
