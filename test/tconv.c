#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <genericLogger.h>
#include "tconv.h"

void _trace(void *userDatavp, const char *msgs);

int main(int argc, char **argv)
{
  tconv_t tconvp;
  tconv_charset_t tconvCharset;
  tconv_option_t tconvOption;
  char  *inputs = "My Input";
  size_t inputl = strlen(inputs) + 1;
  char  *outputs = "    ";
  size_t outputl = strlen(outputs) + 1;

  tconvOption.charsetp       = &tconvCharset;
  tconvOption.convertp       = NULL;
  tconvOption.traceCallbackp = _trace;

  tconvCharset.charseti = TCONV_CHARSET_ICU;
  tconvCharset.u.ICUOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv_trace_on(tconvp);
  tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  tconv_close(tconvp);

  tconvCharset.charseti = TCONV_CHARSET_CCHARDET;
  tconvCharset.u.cchardetOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv_trace_on(tconvp);
  tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  tconv_close(tconvp);

  return 0;
}

void _trace(void *userDatavp, const char *msgs) {
  GENERICLOGGER_TRACE(NULL, msgs);
}

