#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_NAN_REPLACEMENT_USING_DIVISION
#  undef C_NAN
#  define C_NAN (0.0 / 0.0)
#else
#  ifdef HAVE_NAN_REPLACEMENT
#    undef C_NAN
#    define C_NAN (__builtin_nanf(""))
#  endif
#endif
int main() {
  float x = C_NAN;
  return 0;
}
