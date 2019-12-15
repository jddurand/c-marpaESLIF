#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_NAN_REPLACEMENT_
#  undef NAN
#  define NAN (__builtin_nanf(""))
#endif
int main() {
  float x = C_NAN;
  return 0;
}
