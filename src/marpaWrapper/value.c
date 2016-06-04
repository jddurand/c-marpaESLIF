#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/value.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/logging.h"

static marpaWrapperRecognizerOption_t marpaWrapperRecognizerOptionDefault = {
  NULL,    /* genericLoggerp   */
  1        /* latmb            */
};

