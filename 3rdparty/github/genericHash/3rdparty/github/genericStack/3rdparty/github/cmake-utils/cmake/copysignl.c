#if HAVE_MATH_H
#include <math.h>
#endif

int main() {
  long double neg = -1.0;
  long double pos = 1.0;
  long double res = copysignl(pos, neg);

  return 0;
}
