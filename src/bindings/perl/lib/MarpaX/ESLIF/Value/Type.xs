#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "const-c-marpaESLIFValueType.inc"

MODULE = MarpaX::ESLIF::Value::Type            PACKAGE = MarpaX::ESLIF::Value::Type

PROTOTYPES: ENABLE

INCLUDE: const-xs-marpaESLIFValueType.inc
