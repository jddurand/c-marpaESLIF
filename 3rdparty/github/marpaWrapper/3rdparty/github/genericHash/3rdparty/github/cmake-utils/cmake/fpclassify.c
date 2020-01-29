#if HAVE_MATH_H
#include <math.h>
#endif
#if HAVE_FLOAT_H
#include <float.h>
#endif
int main() {
  int x = C_FPCLASSIFY(0.0);
  return 0;
}
