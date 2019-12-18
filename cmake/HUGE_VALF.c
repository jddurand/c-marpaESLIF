#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_HUGE_VALF_REPLACEMENT
#  undef HUGE_VALF
#  define HUGE_VALF (__builtin_huge_valf())
#endif
int main() {
  float x = -C_HUGE_VALF;
  return 0;
}
