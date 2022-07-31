#if HAVE_MATH_H
#include <math.h>
#endif

int main() {
  float neg = -1.0;
  float pos = 1.0;
  float res = copysignf(pos, neg);

  return 0;
}
