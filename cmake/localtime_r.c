#include <time.h>
int main() {
  time_t time;
  struct tm result;
  C_LOCALTIME_R(&time, &result);

  return 0;
}
