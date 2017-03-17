#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>
#include <genericLogger.h>

/* Creating marpaESLIF includes genericLogger */
typedef struct MarpaX_ESLIF {
  SV              *Perl_loggerInterfacep;
  genericLogger_t *genericLoggerp;
  marpaESLIF_t    *marpaESLIFp;
} MarpaX_ESLIF_t;
/* Nothing special for the other types */
typedef marpaESLIFGrammar_t MarpaX_ESLIF_Grammar_t;

/* For typemap */
typedef MarpaX_ESLIF_t         *MarpaX_ESLIF;
typedef MarpaX_ESLIF_Grammar_t *MarpaX_ESLIF_Grammar;

/* ----------------------------------------------- */
/* Use UNIVERSAL::can, taken from Haskells's Plugs */
/* ----------------------------------------------- */
static short marpaESLIF_can(pTHX_ SV *sv, char *method) {
  short ok;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  XPUSHs(sv);
  XPUSHs(newSVpv(method, 0));
  PUTBACK;

  call_pv("UNIVERSAL::can", G_SCALAR);

  SPAGAIN;

  ok = (POPi != 0);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return ok;
}

/* ----------------------- */
/* generic Logger Callback */
/* ----------------------- */
static void marpaESLIF_genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  SV   *Perl_loggerInterfacep = (SV *) userDatavp;
  char *method;

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
    /* It should never happen that method is NULL -; */
    dTHX;
    dSP;

    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(Perl_loggerInterfacep);
    PUSHs(sv_2mortal(newSVpv(msgs,0)));
    PUTBACK;

    call_method(method, G_DISCARD);

    FREETMPS;
    LEAVE;
  }
}

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF                                                           */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::new                                                      */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF
new(Perl_packagep, ...)
  SV *Perl_packagep;
PREINIT:
  SV *Perl_loggerInterfacep = &PL_sv_undef;
CODE:
  MarpaX_ESLIF       MarpaX_ESLIFp;
  marpaESLIFOption_t marpaESLIFOption;

  if(items > 1) {
    Perl_loggerInterfacep = ST(1);
    /* Check this is an SV that can do logging */
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "trace"))     croak("Logger interface must be an object that can do \"trace\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "debug"))     croak("Logger interface must be an object that can do \"debug\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "info"))      croak("Logger interface must be an object that can do \"info\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "notice"))    croak("Logger interface must be an object that can do \"notice\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "warning"))   croak("Logger interface must be an object that can do \"warning\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "error"))     croak("Logger interface must be an object that can do \"error\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "critical"))  croak("Logger interface must be an object that can do \"critical\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "alert"))     croak("Logger interface must be an object that can do \"alert\"");
    if (! marpaESLIF_can(aTHX_ Perl_loggerInterfacep, "emergency")) croak("Logger interface must be an object that can do \"emergency\"");
  }

  Newx(MarpaX_ESLIFp, 1, MarpaX_ESLIF_t);
  MarpaX_ESLIFp->Perl_loggerInterfacep = &PL_sv_undef;
  MarpaX_ESLIFp->genericLoggerp        = NULL;
  MarpaX_ESLIFp->marpaESLIFp           = NULL;

  /* ------------- */
  /* genericLogger */
  /* ------------- */
  if (Perl_loggerInterfacep != &PL_sv_undef) {
    MarpaX_ESLIFp->Perl_loggerInterfacep = SvREFCNT_inc(Perl_loggerInterfacep);
    MarpaX_ESLIFp->genericLoggerp        = genericLogger_newp(marpaESLIF_genericLoggerCallback,
                                                                        MarpaX_ESLIFp->Perl_loggerInterfacep,
                                                                        GENERICLOGGER_LOGLEVEL_TRACE);
    if (MarpaX_ESLIFp->genericLoggerp == NULL) {
      croak(strerror(errno));
    }
  }

  /* ---------- */
  /* marpaESLIF */
  /* ---------- */
  marpaESLIFOption.genericLoggerp = MarpaX_ESLIFp->genericLoggerp;
  MarpaX_ESLIFp->marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (MarpaX_ESLIFp->marpaESLIFp == NULL) {
      croak(strerror(errno));
  }

  RETVAL = MarpaX_ESLIFp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::DESTROY                                                  */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(MarpaX_ESLIFp)
  MarpaX_ESLIF MarpaX_ESLIFp;
CODE:
  if (MarpaX_ESLIFp->Perl_loggerInterfacep != &PL_sv_undef) {
    SvREFCNT_dec(MarpaX_ESLIFp->Perl_loggerInterfacep);
  }
  marpaESLIF_freev(MarpaX_ESLIFp->marpaESLIFp);
  genericLogger_freev(&(MarpaX_ESLIFp->genericLoggerp));
  Safefree(MarpaX_ESLIFp);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::version                                                  */
  /* ----------------------------------------------------------------------- */
=cut

const char *
version(MarpaX_ESLIFp)
  MarpaX_ESLIF MarpaX_ESLIFp;
CODE:
  /* In reality MarpaX_ESLIFp is not needed -; */
  RETVAL = marpaESLIF_versions();
OUTPUT:
  RETVAL

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Grammar                                                  */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Grammar

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::new                                             */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Grammar
new(Perl_packagep, MarpaX_ESLIFp, Perl_grammarp, ...)
  SV           *Perl_packagep;
  MarpaX_ESLIF  MarpaX_ESLIFp;
  SV           *Perl_grammarp;
PREINIT:
  SV           *Perl_encodingp = &PL_sv_undef;
CODE:
  MarpaX_ESLIF_Grammar       MarpaX_ESLIF_Grammarp;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;
  marpaESLIFGrammar_t       *marpaESLIFGrammarp;
  char                      *bytep;
  STRLEN                     bytel;
  char                      *encodings = NULL;
  STRLEN                     encodingl = 0;

  if (items > 3) {
    Perl_encodingp = ST(3);
    encodings = SvPV(Perl_encodingp, encodingl);
  }
  bytep = SvPV(Perl_grammarp, bytel);

  marpaESLIFGrammarOption.bytep               = (void *) bytep;
  marpaESLIFGrammarOption.bytel               = (size_t) bytel;
  marpaESLIFGrammarOption.encodings           = (char *) encodings;
  marpaESLIFGrammarOption.encodingl           = (size_t) encodingl;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(MarpaX_ESLIFp->marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    croak(strerror(errno));
  }

  RETVAL = marpaESLIFGrammarp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::DESTROY                                         */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  marpaESLIFGrammar_freev(MarpaX_ESLIF_Grammarp);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ngrammar                                        */
  /* ----------------------------------------------------------------------- */
=cut

IV
ngrammar(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  int ngrammari;

  if (! marpaESLIFGrammar_ngrammarib(MarpaX_ESLIF_Grammarp, &ngrammari)) {
    croak("marpaESLIFGrammar_ngrammarib failure");
  }

  RETVAL = (IV) ngrammari;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentLevel                                    */
  /* ----------------------------------------------------------------------- */
=cut

IV
currentLevel(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  int leveli;

  if (! marpaESLIFGrammar_grammar_currentb(MarpaX_ESLIF_Grammarp, &leveli, NULL)) {
    croak("marpaESLIFGrammar_grammar_currentb failure");
  }
  RETVAL = (IV) leveli;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentDescription                              */
  /* ----------------------------------------------------------------------- */
=cut

SV *
currentDescription(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_currentb(MarpaX_ESLIF_Grammarp, NULL, &descp)) {
    croak("marpaESLIFGrammar_grammar_currentb failure");
  }
  /* It is in the same encoding as original grammar */
  RETVAL = newSVpvn(descp->bytep, descp->bytel);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::descriptionByLevel                              */
  /* ----------------------------------------------------------------------- */
=cut

SV *
descriptionByLevel(MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_leveli;
CODE:
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_leveli, NULL, NULL, &descp)) {
    croak("marpaESLIFGrammar_grammar_by_levelb failure");
  }
  /* It is in the same encoding as original grammar */
  RETVAL = newSVpvn(descp->bytep, descp->bytel);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentRuleIds                                  */
  /* ----------------------------------------------------------------------- */
=cut

AV *
currentRuleIds(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_currentb(MarpaX_ESLIF_Grammarp, &ruleip, &rulel)) {
    croak("marpaESLIFGrammar_rulearray_currentb failure");
  }
  if (rulel <= 0) {
    croak("marpaESLIFGrammar_rulearray_currentb returned no rule");
  }
  av = newAV();
  for (i = 0; i < rulel; i++) {
    av_push(av, newSViv((IV) ruleip[i]));
  }
  RETVAL = av;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleIdsByLevel                                  */
  /* ----------------------------------------------------------------------- */
=cut

AV *
ruleIdsByLevel(MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_leveli;
CODE:
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_by_levelb(MarpaX_ESLIF_Grammarp, &ruleip, &rulel, (int) Perl_leveli, NULL)) {
    croak("marpaESLIFGrammar_rulearray_by_levelb failure");
  }
  if (rulel <= 0) {
    croak("marpaESLIFGrammar_rulearray_by_levelb returned no rule");
  }
  av = newAV();
  for (i = 0; i < rulel; i++) {
    av_push(av, newSViv((IV) ruleip[i]));
  }
  RETVAL = av;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleDisplay                                     */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleDisplay(MarpaX_ESLIF_Grammarp, Perl_rulei)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_rulei;
CODE:
  char *ruledisplays;

  if (! marpaESLIFGrammar_ruleshowform_currentb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruledisplays)) {
    croak("marpaESLIFGrammar_ruledisplayform_currentb failure");
  }
  RETVAL = ruledisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleShow                                        */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleShow(MarpaX_ESLIF_Grammarp, Perl_rulei)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_rulei;
CODE:
  char *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_currentb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruleshows)) {
    croak("marpaESLIFGrammar_ruleshowform_currentb failure");
  }
  RETVAL = ruleshows;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleDisplayByLevel                              */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleDisplayByLevel(MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_rulei)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_leveli;
  IV  Perl_rulei;
CODE:
  char *ruledisplays;

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruledisplays, (int) Perl_leveli, NULL)) {
    croak("marpaESLIFGrammar_ruledisplayform_by_levelb failure");
  }
  RETVAL = ruledisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleShowByLevel                                 */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleShowByLevel(MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_rulei)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV  Perl_leveli;
  IV  Perl_rulei;
CODE:
  char *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruleshows, (int) Perl_leveli, NULL)) {
    croak("marpaESLIFGrammar_ruleshowform_by_levelb failure");
  }
  RETVAL = ruleshows;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::show                                            */
  /* ----------------------------------------------------------------------- */
=cut

char *
show(MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
CODE:
  char *shows;

  if (! marpaESLIFGrammar_grammarshowform_currentb(MarpaX_ESLIF_Grammarp, &shows)) {
    croak("marpaESLIFGrammar_ruleshowform_by_levelb failure");
  }
  RETVAL = shows;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::showByLevel                                     */
  /* ----------------------------------------------------------------------- */
=cut

char *
showByLevel(MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar MarpaX_ESLIF_Grammarp;
  IV Perl_leveli;
CODE:
  char *shows;

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(MarpaX_ESLIF_Grammarp, &shows, (int) Perl_leveli, NULL)) {
    croak("marpaESLIFGrammar_grammarshowform_by_levelb failure");
  }
  RETVAL = shows;
OUTPUT:
  RETVAL

