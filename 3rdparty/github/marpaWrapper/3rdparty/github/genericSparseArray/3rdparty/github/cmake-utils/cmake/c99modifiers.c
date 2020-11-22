#if HAVE_STDINT_H
#  include <stdint.h>
#endif
#if HAVE_STDDEF_H
#  include <stddef.h>
#endif
#if HAVE_SYS_STDINT_H
#  include <sys/stdint.h>
#endif
#include <stdio.h>
#include <string.h>

int main()
{
  char buf[64];

  if (sprintf(buf, "%zu", (size_t)1234) != 4) {
    return 1;
  }
  else if (strcmp(buf, "1234")) {
    return 2;
  }

  return 0;
}
