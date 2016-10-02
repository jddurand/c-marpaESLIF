#include <stdlib.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

int main() {
  marpaESLIFOption_t marpaESLIFOption;
  marpaESLIF_t      *marpaESLIFp;
  int                exiti = 0;

  marpaESLIFOption.genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    exiti = 1;
  }

  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(marpaESLIFOption.genericLoggerp);

  return exiti;
}
