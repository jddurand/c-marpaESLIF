#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "const-c-marpaESLIFEventType.inc"

MODULE = MarpaX::ESLIF::Event::Type            PACKAGE = MarpaX::ESLIF::Event::Type

PROTOTYPES: ENABLE

INCLUDE: const-xs-marpaESLIFEventType.inc
