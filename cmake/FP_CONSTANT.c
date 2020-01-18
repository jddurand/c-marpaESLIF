#if HAVE_MATH_H
#include <math.h>
#endif
#if HAVE_FLOAT_H
#include <float.h>
#endif
int main() {
#if defined(C_FP_NAN)
  short x = C_FP_NAN;
#elif defined(C__FPCLASS_SNAN)
  short x = C__FPCLASS_SNAN;
#elif defined(C__FPCLASS_QNAN)
  short x = C__FPCLASS_QNAN;
#elif defined(C_FP_INFINITE)
  short x = C_FP_INFINITE;
#elif defined(C__FPCLASS_NINF)
  short x = C__FPCLASS_NINF;
#elif defined(C__FPCLASS_PINF)
  short x = C__FPCLASS_PINF;
#else
#error Nothing
#endif
    
  return 0;
}
