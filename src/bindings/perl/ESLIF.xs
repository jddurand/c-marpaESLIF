#define PERL_NO_GET_CONTEXT     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Thin

const char *
version()
  CODE:
    RETVAL = marpaESLIF_versions();
  OUTPUT:
    RETVAL
