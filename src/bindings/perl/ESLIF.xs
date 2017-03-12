#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>

/* ------------------------------------------------------- */
/* All the getters and setters we need are always          */
/* $self->_set_xxx(void *p)                                */
/* or                                                      */
/* void *p = $self->_get_xxx()                             */
/* ------------------------------------------------------- */
static void _ESLIF_set(pTHX_ SV *Perl_selfp, char *method, void *p) {
  dSP;

  ENTER;
  SAVETMPS;
  EXTEND(SP, 2);
  PUSHMARK(SP);
  XPUSHs(sv_2mortal(newSVsv(Perl_selfp)));
  /* We store a SV that is an IV */
  XPUSHs(sv_2mortal(newSViv(PTR2IV(p))));
  PUTBACK;
  call_method(method, G_DISCARD);
  FREETMPS;
  LEAVE;
}

static void *_ESLIF_get(pTHX_ SV *Perl_selfp, char *method) {
  SV *sv;
  void *p;
  int count;
  dSP;

  ENTER;
  SAVETMPS;
  EXTEND(SP, 1);
  PUSHMARK(SP);
  XPUSHs(sv_2mortal(newSVsv(Perl_selfp)));
  PUTBACK;
  count = call_method(method, G_SCALAR);
  SPAGAIN;
  if (count != 1) {
    croak(method);
  }
  /* We get an SV that we convert to a void * */
  sv = POPs;
  p = INT2PTR(void *, SvIV(sv));
  PUTBACK;
  FREETMPS;
  LEAVE;
  
  return p;
}

static void marpaESLIF_genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  /* Here userDatavp is the SV of MarpaX::ESLIF */
  char *method;

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:     method = "_trace";     break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:     method = "_debug";     break;
  case GENERICLOGGER_LOGLEVEL_INFO:      method = "_info";      break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:    method = "_notice";    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:   method = "_warning";   break;
  case GENERICLOGGER_LOGLEVEL_ERROR:     method = "_error";     break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:  method = "_critical";  break;
  case GENERICLOGGER_LOGLEVEL_ALERT:     method = "_alert";     break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY: method = "_emergency"; break;
  default:                               method = NULL;        break;
  }

  if (method != NULL) {
    dTHX;
    dSP;
    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(userDatavp); /* This is $self -; */
    PUSHs(sv_2mortal(newSVpv(msgs,0)));
    PUTBACK;

    call_method(method, G_DISCARD);           /* method call */
  }
}

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Thin                                                     */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Thin

PROTOTYPES: ENABLE

=for comment
  /* ------------------------ */
  /* MarpaX::ESLIF::Thin::new */
  /* ------------------------ */
=cut

void *
new(Perl_selfp)
  SV *Perl_selfp;
CODE:
  genericLogger_t    *genericLoggerp;
  marpaESLIFOption_t  marpaESLIFOption;
  marpaESLIF_t       *marpaESLIFp;

  genericLoggerp = genericLogger_newp(marpaESLIF_genericLoggerCallback, Perl_selfp, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    croak("genericLogger_newp failure");
  }
  _ESLIF_set(aTHX_ Perl_selfp, "_set_genericLoggerp", genericLoggerp);

  /* Prepare marpaESLIF */
  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    croak("marpaESLIF_newp failure");
  }
  _ESLIF_set(aTHX_ Perl_selfp, "_set_marpaESLIFp", marpaESLIFp);

  RETVAL = marpaESLIFp;
OUTPUT:
  RETVAL

=for comment
  /* ------------------------- */
  /* MarpaX::ESLIF::Thin::free */
  /* ------------------------- */
=cut

void
free(Perl_selfp)
  SV *Perl_selfp;
CODE:
  genericLogger_t   *genericLoggerp;
  marpaESLIF_t      *marpaESLIFp;

  genericLoggerp = (genericLogger_t *) _ESLIF_get(aTHX_ Perl_selfp, "_get_genericLoggerp");
  marpaESLIFp    = (marpaESLIF_t *) _ESLIF_get(aTHX_ Perl_selfp, "_get_marpaESLIFp");

  marpaESLIF_freev(marpaESLIFp);
  genericLogger_freev(&genericLoggerp);
