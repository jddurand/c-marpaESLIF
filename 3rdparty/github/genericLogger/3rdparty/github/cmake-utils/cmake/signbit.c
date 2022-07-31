#if HAVE_MATH_H
#include <math.h>
#endif

int main() {
  float f = -1.0;
  int i = signbit(f);

  return 0;
}
