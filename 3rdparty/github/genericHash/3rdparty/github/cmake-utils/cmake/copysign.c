#if HAVE_MATH_H
#include <math.h>
#endif

int main() {
  double neg = -1.0;
  double pos = 1.0;
  double res = copysign(pos, neg);

  return 0;
}
