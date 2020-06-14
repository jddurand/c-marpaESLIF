#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_ISINF_REPLACEMENT
#  undef C_ISINF
#  define C_ISINF(x) (__builtin_isinf(x))
#endif
int main() {
  short x = C_ISINF(0.0);
  return 0;
}
