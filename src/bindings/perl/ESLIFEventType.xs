#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "const-c-marpaESLIFEventType.inc"

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Event::Type                                              */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Event::Type

INCLUDE: const-xs-marpaESLIFEventType.inc
