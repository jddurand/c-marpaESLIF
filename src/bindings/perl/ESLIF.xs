#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>
#include <genericLogger.h>

typedef struct genericLoggerContextp {
  SV *Perl_loggerInterfacep;
} genericLoggerContext_t;

/* ------------------------------------------------------- */
/* Exception raise                                         */
/* ------------------------------------------------------- */
static void _ESLIF_croak(pTHX_ char *message) {
  dSP;

  ENTER;
  SAVETMPS;
  EXTEND(SP, 2);
  PUSHMARK(SP);
  XPUSHs(sv_2mortal(newSVpv("MarpaX::ESLIF::Exception", 0)));
  XPUSHs(sv_2mortal(newSVpv(message, 0)));
  PUTBACK;
  call_method("throw", G_DISCARD);
  FREETMPS;
  LEAVE;
}

/* ------------------------------------------------------- */
/* All the getters and setters we need are always          */
/* $self->_setXxx(void *p)                                 */
/* ... and we always store it as an IV                     */
/* or                                                      */
/* void *p = $self->_getXxx()                              */
/* ... and we always convert the IV to a void *            */
/* ------------------------------------------------------- */
static void _ESLIF_set(pTHX_ SV *Perl_selfp, char *method, void *p) {
  dSP;

  ENTER;
  SAVETMPS;
  EXTEND(SP, 2);
  PUSHMARK(SP);
  XPUSHs(sv_2mortal(newSVsv(Perl_selfp)));
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
    _ESLIF_croak(aTHX_ method);
  }
  /* We get an SV that we convert to a void *, it is ok if we loose the SV */
  sv = POPs;
  p = INT2PTR(void *, SvIV(sv));
  PUTBACK;
  FREETMPS;
  LEAVE;
  
  return p;
}

/* ----------------------- */
/* generic Logger Callback */
/* ----------------------- */
static void marpaESLIF_genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  genericLoggerContext_t *genericLoggerContextp = (genericLoggerContext_t *) userDatavp;
  char                   *method;

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:     method = "trace";     break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:     method = "debug";     break;
  case GENERICLOGGER_LOGLEVEL_INFO:      method = "info";      break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:    method = "notice";    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:   method = "warning";   break;
  case GENERICLOGGER_LOGLEVEL_ERROR:     method = "error";     break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:  method = "critical";  break;
  case GENERICLOGGER_LOGLEVEL_ALERT:     method = "alert";     break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY: method = "emergency"; break;
  default:                               method = NULL;        break;
  }

  if (method != NULL) {
    dTHX;
    dSP;
    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(genericLoggerContextp->Perl_loggerInterfacep);
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
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Thin::new                                                */
  /* ----------------------------------------------------------------------- */
=cut

void
new(Perl_selfp, Perl_loggerInterfacep)
  SV *Perl_selfp;
  SV *Perl_loggerInterfacep;
CODE:
{
  genericLoggerContext_t *genericLoggerContextp = NULL;
  genericLogger_t        *genericLoggerp = NULL;
  marpaESLIF_t           *marpaESLIFp    = NULL;
  marpaESLIFOption_t      marpaESLIFOption;

  /* -------------------------------------- */
  /* Create and save generic logger context */
  /* -------------------------------------- */
  Newx(genericLoggerContextp, 1, genericLoggerContext_t);
  genericLoggerContextp->Perl_loggerInterfacep = &PL_sv_undef;
  _ESLIF_set(aTHX_ Perl_selfp, "_setGenericLoggerContextp", genericLoggerContextp);
  /* Intentionnaly done after the set */
  genericLoggerContextp->Perl_loggerInterfacep = newSVsv(Perl_loggerInterfacep);

  /* ------------------------------ */
  /* Create and save generic logger */
  /* ------------------------------ */
  genericLoggerp = genericLogger_newp(marpaESLIF_genericLoggerCallback,
                                      genericLoggerContextp,
                                      GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    _ESLIF_croak(aTHX_ strerror(errno));
  }
  _ESLIF_set(aTHX_ Perl_selfp, "_setGenericLoggerp", genericLoggerp);

  /* --------------------------- */
  /* Create and save marpaESLIFp */
  /* --------------------------- */
  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    _ESLIF_croak(aTHX_ "marpaESLIF_newp failure");
  }
  _ESLIF_set(aTHX_ Perl_selfp, "_setMarpaESLIFp", marpaESLIFp);
}

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Thin::free                                               */
  /* ----------------------------------------------------------------------- */
=cut

void
free(Perl_selfp, Perl_marpaESLIFp, Perl_genericLoggerp, Perl_genericLoggerContextp)
  SV   *Perl_selfp;
  IV    Perl_marpaESLIFp;
  IV    Perl_genericLoggerp;
  IV    Perl_genericLoggerContextp;
CODE:
{
  marpaESLIF_t           *marpaESLIFp           = INT2PTR(marpaESLIF_t *,           Perl_marpaESLIFp);
  genericLogger_t        *genericLoggerp        = INT2PTR(genericLogger_t *,        Perl_genericLoggerp);
  genericLoggerContext_t *genericLoggerContextp = INT2PTR(genericLoggerContext_t *, Perl_genericLoggerContextp);

  marpaESLIF_freev(marpaESLIFp);        /* This is NULL protected */
  genericLogger_freev(&genericLoggerp); /* Ditto */

  if (genericLoggerContextp != NULL) {
    if (genericLoggerContextp->Perl_loggerInterfacep != &PL_sv_undef) {
      SvREFCNT_dec(genericLoggerContextp->Perl_loggerInterfacep);
    }
    Safefree(genericLoggerContextp);
  }
}

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Thin::version                                            */
  /* ----------------------------------------------------------------------- */
=cut

const char *
version()
CODE:
{
  /* In reality Perl_selfp is not needed -; */
  RETVAL = marpaESLIF_versions();
}
OUTPUT:
  RETVAL

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Grammar::Thin

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::new                                       */
  /* ----------------------------------------------------------------------- */
=cut

void
new(Perl_selfp, Perl_eslifp, Perl_utf8octets)
  SV     *Perl_selfp;
  SV     *Perl_eslifp;
  SV     *Perl_utf8octets;
CODE:
{
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammar_t       *marpaESLIFGrammarp;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;
  char                      *utf8p;
  STRLEN                     utf8l;

  utf8p = SvPV(Perl_utf8octets, utf8l);

  /* Create C object */
  marpaESLIFGrammarOption.bytep               = (void *) utf8p;
  marpaESLIFGrammarOption.bytel               = (size_t) utf8l;
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = strlen("UTF-8");
  marpaESLIFGrammarOption.encodingOfEncodings = "ASCII";

  marpaESLIFp = _ESLIF_get(aTHX_ Perl_eslifp, "_getMarpaESLIFp");
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    _ESLIF_croak(aTHX_ strerror(errno));
  }
  _ESLIF_set(aTHX_ Perl_selfp, "_setMarpaESLIFGrammarp", marpaESLIFGrammarp);
}

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::free                                      */
  /* ----------------------------------------------------------------------- */
=cut

void
free(Perl_selfp, Perl_marpaESLIFGrammarp)
  SV *Perl_selfp;
  IV  Perl_marpaESLIFGrammarp;
CODE:
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = INT2PTR(marpaESLIFGrammar_t *, Perl_marpaESLIFGrammarp);

  marpaESLIFGrammar_freev(marpaESLIFGrammarp); /* This is NULL protected */
}

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::ngrammar                                  */
  /* ----------------------------------------------------------------------- */
=cut

IV
ngrammar(Perl_selfp, Perl_marpaESLIFGrammarp)
  SV *Perl_selfp;
  IV  Perl_marpaESLIFGrammarp;
CODE:
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = INT2PTR(marpaESLIFGrammar_t *, Perl_marpaESLIFGrammarp);
  int                  ngrammari;

  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    _ESLIF_croak(aTHX_ "marpaESLIFGrammar_ngrammarib failure");
  }
  RETVAL = (IV) ngrammari;
}
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::currentLevel                              */
  /* ----------------------------------------------------------------------- */
=cut

IV
currentLevel(Perl_selfp, Perl_marpaESLIFGrammarp)
  SV *Perl_selfp;
  IV  Perl_marpaESLIFGrammarp;
CODE:
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = INT2PTR(marpaESLIFGrammar_t *, Perl_marpaESLIFGrammarp);
  int                  leveli;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL)) {
    _ESLIF_croak(aTHX_ "marpaESLIFGrammar_grammar_currentb failure");
  }
  RETVAL = (IV) leveli;
}
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::currentDescription                        */
  /* ----------------------------------------------------------------------- */
=cut

SV *
currentDescription(Perl_selfp, Perl_marpaESLIFGrammarp)
  SV *Perl_selfp;
  IV  Perl_marpaESLIFGrammarp;
CODE:
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = INT2PTR(marpaESLIFGrammar_t *, Perl_marpaESLIFGrammarp);
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, NULL, &descp)) {
    _ESLIF_croak(aTHX_ "marpaESLIFGrammar_grammar_currentb failure");
  }
  /* We enforced UTF-8 when parsing the grammar, so description is also in UTF-8 */
  sv = newSVpv(descp->bytep, descp->bytel);
  sv_utf8_upgrade(sv);
  RETVAL = sv;
}
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::Thin::descriptionByLevel                        */
  /* ----------------------------------------------------------------------- */
=cut

SV *
descriptionByLevel(Perl_selfp, Perl_marpaESLIFGrammarp, Perl_leveli)
  SV *Perl_selfp;
  IV  Perl_marpaESLIFGrammarp;
  IV  Perl_leveli;
CODE:
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = INT2PTR(marpaESLIFGrammar_t *, Perl_marpaESLIFGrammarp);
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammarp, (int) Perl_leveli, NULL, NULL, &descp)) {
    _ESLIF_croak(aTHX_ "marpaESLIFGrammar_grammar_by_levelb failure");
  }
  /* We enforced UTF-8 when parsing the grammar, so description is also in UTF-8 */
  sv = newSVpv(descp->bytep, descp->bytel);
  sv_utf8_upgrade(sv);
  RETVAL = sv;
}
OUTPUT:
  RETVAL
