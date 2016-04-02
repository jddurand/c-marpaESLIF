#include "genericLogger.h"

#include <stdlib.h>

int main() {
  genericLogger_t *loggerp = genericLogger_newp(NULL, NULL, GENERICLOGGER_LOGLEVEL_TRACE);

  GENERICLOGGER_TRACE (loggerp, "Single message");
  GENERICLOGGER_TRACEF(loggerp, "%s %s", "Formatted", "Message");

  genericLogger_freev(&loggerp);
  
  return 0;
}
