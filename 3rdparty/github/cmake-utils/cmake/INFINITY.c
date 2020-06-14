#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_INFINITY_REPLACEMENT_USING_DIVISION
#  undef C_INFINITY
#  define C_INFINITY (1.0 / 0.0)
#else
#  ifdef HAVE_INFINITY_REPLACEMENT
#    undef C_INFINITY
#    define C_INFINITY (__builtin_inff())
#  endif
#endif
int main() {
  float x = -C_INFINITY;
  return 0;
}
