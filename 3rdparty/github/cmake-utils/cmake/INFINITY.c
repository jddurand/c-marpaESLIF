#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_INFINITY_REPLACEMENT_
#  undef INFINITY
#  define INFINITY (__builtin_inff())
#endif
int main() {
  float x = -C_INFINITY;
  return 0;
}
