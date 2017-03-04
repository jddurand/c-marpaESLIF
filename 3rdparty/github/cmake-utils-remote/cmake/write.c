#if HAVE_STDIO_H
#include <stdio.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

int main() {
  C_WRITE(stderr, "", 1);
  return 0;
}
