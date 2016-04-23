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
  char  *inputs;
  size_t inputl;
  char  *outputs;
  size_t outputl;
  char  *origInputs;
  char  *endInputs;
  char  *origOutputs;

  tconvOption.charsetp       = &tconvCharset;
  tconvOption.convertp       = NULL;
  tconvOption.traceCallbackp = _trace;

  tconvCharset.charseti = TCONV_CHARSET_ICU;
  tconvCharset.u.ICUOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv_trace_on(tconvp);
  
  inputs = "My Input";
  origInputs = inputs;
  inputl = strlen(inputs) + 1;
  endInputs = inputs + inputl;
  outputs = "                             ";
  outputl = strlen(outputs) + 1;
  origOutputs = outputs;
  while (inputs < endInputs) {
    tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  }
  tconv_close(tconvp);
  printf("\"%s\" => \"%s\"\n", origInputs, origOutputs);

  tconvCharset.charseti = TCONV_CHARSET_CCHARDET;
  tconvCharset.u.cchardetOptionp = NULL;
  tconvp = tconv_open_ext(NULL, NULL, &tconvOption);
  tconv_trace_on(tconvp);
  
  inputs = "My Input";
  origInputs = inputs;
  inputl = strlen(inputs) + 1;
  endInputs = inputs + inputl;
  outputs = "                             ";
  outputl = strlen(outputs) + 1;
  origOutputs = outputs;
  while (inputs < endInputs) {
    tconv(tconvp, &inputs, &inputl, &outputs, &outputl);
  }
  tconv_close(tconvp);
  printf("\"%s\" => \"%s\"\n", origInputs, origOutputs);

  return 0;
}

void _trace(void *userDatavp, const char *msgs) {
  GENERICLOGGER_TRACE(NULL, msgs);
}

