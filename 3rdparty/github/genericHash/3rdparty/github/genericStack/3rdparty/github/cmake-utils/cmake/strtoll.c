#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
/*     Just because of versions of these compilers might not have long long, but they */
/*     always had __int64. Note that on Windows short is always 2, int is always 4,   */
/*     long is always 4, __int64 is always 8 */
#  define LONG_LONG __int64
#else
#  define LONG_LONG long long
#endif


int main() {
  char      *p = "123";
  char      *endptrp;
  LONG_LONG  ll;

  ll = C_STRTOLL(p, &endptrp, 10);
  return 0;
}
