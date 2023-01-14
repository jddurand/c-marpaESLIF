#include <stdlib.h>

#define C_LIKELY(x)    C__BUILTIN_EXPECT(!!(x), 1)
#define C_UNLIKELY(x)  C__BUILTIN_EXPECT(!!(x), 0)

/* Copied from https://kernelnewbies.org/FAQ/LikelyUnlikely */
int main(int argc, char *argv[])
{
   int a;

   /* Get the value from somewhere GCC can't optimize */
   a = atoi (argv[1]);

   if (C_UNLIKELY(a == 2))
      a++;
   else
      a--;

   return 0;
}
