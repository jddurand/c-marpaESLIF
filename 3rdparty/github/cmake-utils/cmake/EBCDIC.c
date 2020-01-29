#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

int main() {
  if ('M'==0xd4) {
    exit(0);
  }
  exit(1);
}
