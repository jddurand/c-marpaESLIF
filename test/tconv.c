#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <genericLogger.h>
#include "tconv.h"

void _trace(void *userDatavp, const char *msgs);

void _loop(tconv_t tconvp, char *inbufs, size_t inbytesleftl, char *outbufs, size_t outbytesleftl);

int main(int argc, char **argv)
{
  tconv_t tconvp;
  tconv_charset_t tconvCharset;
  tconv_option_t tconvOption;
  char   *inputs;
  size_t  inputl;
  char   *outputs;
  size_t  outputl;

  tconvOption.charsetp       = &tconvCharset;
  tconvOption.convertp       = NULL;
  tconvOption.traceCallbackp = _trace;

  tconvCharset.charseti = TCONV_CHARSET_ICU;
  tconvCharset.u.ICUOptionp = NULL;
  tconvp = tconv_open_ext("ISO-8859-1", NULL, &tconvOption);
  tconv_trace_on(tconvp);
  inputs = "ßàéöø";
  inputl = strlen(inputs) + 1;
  outputs = malloc(200);
  outputl = 200;
  _loop(tconvp, inputs, inputl, outputs, outputl);
  tconv_close(tconvp);
  printf("\"%s\" => \"%s\"\n", inputs, outputs);
  free(outputs);

  tconvCharset.charseti = TCONV_CHARSET_CCHARDET;
  tconvCharset.u.cchardetOptionp = NULL;
  tconvp = tconv_open_ext("ASCII", NULL, &tconvOption);
  tconv_trace_on(tconvp);
  inputs = "ßàéöø";
  inputl = strlen(inputs) + 1;
  outputs = malloc(200);
  outputl = 200;
  _loop(tconvp, inputs, inputl, outputs, outputl);
  tconv_close(tconvp);
  printf("\"%s\" => \"%s\"\n", inputs, outputs);
  free(outputs);

  return 0;
}

void _trace(void *userDatavp, const char *msgs)
{
  GENERICLOGGER_TRACE(NULL, msgs);
}

void _loop(tconv_t tconvp, char *inbufs, size_t inbytesleftl, char *outbufs, size_t outbytesleftl)
{
  while (inbytesleftl > 0) {
    if (tconv(tconvp, &inbufs, &inbytesleftl, &outbufs, &outbytesleftl) == (size_t)-1) {
      fprintf(stderr, "tconv failure, %s", tconv_error(tconvp));
      break;
    }
  }
}

