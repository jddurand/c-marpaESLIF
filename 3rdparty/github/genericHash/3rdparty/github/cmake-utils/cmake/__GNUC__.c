#include <stdlib.h>

#ifdef __GNUC__
int main() { exit(0); }
#else
#  error __GNUC__ is not defined
#endif
