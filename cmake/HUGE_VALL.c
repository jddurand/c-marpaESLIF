#if HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_HUGE_VALL_REPLACEMENT
#  undef HUGE_VALL
#  define HUGE_VALL (__builtin_huge_vall())
#endif
int main() {
  long double x = -C_HUGE_VALL;
  return 0;
}
