#include <stdlib.h>
#include "tconv.h"
#include "genericLogger.h"

int main(int argc, char **argv)
{
  tconv_t tconvp;
  tconv_charset_t tconvCharset;
  tconv_option_t tconvOption;


  tconvCharset.charseti = TCONV_CONVERT_PLUGIN;
  tconvCharset.u.plugin.filenames = "TOTO";

  tconvOption.charsetp = NULL;
  tconvOption.convertp = NULL;
  tconvOption.genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);

  tconvp = tconv_open(NULL, NULL);
  tconv_close(tconvp);

  tconvp = tconv_open_ext("TO", "FROM", &tconvOption);
  tconv_close(tconvp);

  GENERICLOGGER_FREE(tconvOption.genericLoggerp);

  return 0;
}
