#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STDIO_H
#include <stdio.h>
#endif
#if HAVE_LIMITS_H
#include <limits.h>
#endif

int main() {
  fprintf(stdout, "%d", CHAR_BIT);
  exit(0);
}
