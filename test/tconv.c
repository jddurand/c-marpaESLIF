#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "tconv.h"
#include "genericLogger.h"

int main(int argc, char **argv)
{
  tconv_t tconvp;
  tconv_charset_t tconvCharset;
  tconv_option_t tconvOption;
  char  *inputs = "My Input";
  size_t inputl = strlen(inputs) + 1;
  char  *outputs = "    ";
  size_t outputl = strlen(outputs) + 1;

  tconvOption.charsetp = &tconvCharset;
  tconvOption.convertp = NULL;
  tconvOption.genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);

  tconvCharset.charseti = TCONV_CHARSET_ICU;
  tconvCharset.u.ICUOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  tconv_close(tconvp);

  tconvCharset.charseti = TCONV_CHARSET_CCHARDET;
  tconvCharset.u.cchardetOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  tconv_close(tconvp);

  GENERICLOGGER_FREE(tconvOption.genericLoggerp);

  return 0;
}
