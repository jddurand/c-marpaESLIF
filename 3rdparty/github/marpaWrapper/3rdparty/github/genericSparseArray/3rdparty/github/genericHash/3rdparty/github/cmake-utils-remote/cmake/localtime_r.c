#if HAVE_TIME_H
#include <time.h>
#endif

int main() {
  time_t time;
  struct tm result;
  C_LOCALTIME_R(&time, &result);

  return 0;
}
