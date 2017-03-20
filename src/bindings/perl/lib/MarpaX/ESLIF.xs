#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>
#include <genericLogger.h>
#include <genericStack.h>
#include <stdio.h>

/* ESLIF context */
typedef struct MarpaX_ESLIF {
  SV              *Perl_loggerInterfacep;
  genericLogger_t *genericLoggerp;
  marpaESLIF_t    *marpaESLIFp;
} marpaESLIFContext_t, MarpaX_ESLIF_t;

/* Recognizer context */
typedef struct marpaESLIFRecognizerContext {
  SV                     *Perl_recognizerInterfacep;  /* Current recognizer interface instance */
  SV                     *previous_Perl_datap;
  SV                     *previous_Perl_encodingp;
  genericStack_t         *svStackp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
} marpaESLIFRecognizerContext_t, MarpaX_ESLIF_Recognizer_t;

/* Value context */
typedef struct marpaESLIFValueContext {
  SV             *Perl_valueInterfacep;                        /* Current eslifValueInterface instance */
  char           *actions;                                     /* shallow copy of last resolved name */
  genericStack_t *svStackp;                                    /* Stack of objects */
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp; /* Shallow copy of associated recognizer context */
  marpaESLIFValue_t             *marpaESLIFValuep;
} marpaESLIFValueContext_t, MarpaX_ESLIF_Value_t;

/* Nothing special for the grammar type */
typedef marpaESLIFGrammar_t    MarpaX_ESLIF_Grammar_t;

/* For typemap */
typedef MarpaX_ESLIF_t            *MarpaX_ESLIF;
typedef MarpaX_ESLIF_Grammar_t    *MarpaX_ESLIF_Grammar;
typedef MarpaX_ESLIF_Recognizer_t *MarpaX_ESLIF_Recognizer;
typedef MarpaX_ESLIF_Value_t      *MarpaX_ESLIF_Value;

/* Static functions declarations */
static int                             marpaESLIF_getTypei(pTHX_ SV* sv);
static short                           marpaESLIF_canb(pTHX_ SV *sv, char *method);
static SV                             *marpaESLIF_canp(pTHX_ SV *sv, char *method);
static void                            marpaESLIF_call_methodv(pTHX_ SV *svp, char *methods, SV *argsvp);
static SV                             *marpaESLIF_call_methodp(pTHX_ SV *svp, char *methods);
static SV                             *marpaESLIF_call_actionp(pTHX_ SV *svp, char *methods, AV *avp);
static IV                              marpaESLIF_call_methodi(pTHX_ SV *svp, char *methods);
static short                           marpaESLIF_call_methodb(pTHX_ SV *svp, char *methods);
static void                            marpaESLIF_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short                           marpaESLIF_recognizerReaderCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t   marpaESLIF_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t marpaESLIF_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                           marpaESLIF_valueRuleCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           marpaESLIF_valueSymbolCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void                            marpaESLIF_valueContextFreev(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, short onStackb);
static void                            marpaESLIF_valueContextCleanup(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp);
static void                            marpaESLIF_recognizerContextFreev(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp, short onStackb);
static void                            marpaESLIF_recognizerContextCleanupv(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp);
static void                            marpaESLIF_recognizerContextInit(pTHX_ SV *Perl_recognizerInterfacep, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp);
static void                            marpaESLIF_valueContextInit(pTHX_ SV *Perl_valueInterfacep, marpaESLIFValueContext_t *marpaESLIFValueContextp, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp);
static void                            marpaESLIF_paramIsGrammarv(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsEncodingv(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsLoggerInterfacev(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsRecognizerInterfacev(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsValueInterfacev(pTHX_ SV *sv);

/* ------ */
/* Macros */
/* ------ */
#define MARPAESLIF_FILENAMES "ESLIF.xs"

#define MARPAESLIF_CROAK(msgs)       croak("[In %s at %s:%d] %s", funcs, MARPAESLIF_FILENAMES, __LINE__, msgs)
#define MARPAESLIF_CROAKF(fmts, ...) croak("[In %s at %s:%d]" fmts, funcs, MARPAESLIF_FILENAMES, __LINE__, __VA_ARGS__)

/* ------------------------------------------------------ */
/* Copy of Params-Validate-1.26/lib/Params/Validate/XS.xs */
/* ------------------------------------------------------ */
#define SCALAR    1
#define ARRAYREF  2
#define HASHREF   4
#define CODEREF   8
#define GLOB      16
#define GLOBREF   32
#define SCALARREF 64
#define UNKNOWN   128
#define UNDEF     256
#define OBJECT    512
static int marpaESLIF_getTypei(pTHX_ SV* sv) {
  int type = 0;

  if (SvTYPE(sv) == SVt_PVGV) {
    return GLOB;
  }
  if (!SvOK(sv)) {
    return UNDEF;
  }
  if (!SvROK(sv)) {
    return SCALAR;
  }

  switch (SvTYPE(SvRV(sv))) {
  case SVt_NULL:
  case SVt_IV:
  case SVt_NV:
  case SVt_PV:
#if PERL_VERSION <= 10
  case SVt_RV:
#endif
  case SVt_PVMG:
  case SVt_PVIV:
  case SVt_PVNV:
#if PERL_VERSION <= 8
  case SVt_PVBM:
#elif PERL_VERSION >= 11
  case SVt_REGEXP:
#endif
    type = SCALARREF;
    break;
  case SVt_PVAV:
    type = ARRAYREF;
    break;
  case SVt_PVHV:
    type = HASHREF;
    break;
  case SVt_PVCV:
    type = CODEREF;
    break;
  case SVt_PVGV:
    type = GLOBREF;
    break;
    /* Perl 5.10 has a bunch of new types that I don't think will ever
       actually show up here (I hope), but not handling them makes the
       C compiler cranky. */
  default:
    type = UNKNOWN;
    break;
  }

  if (type) {
    if (sv_isobject(sv)) return type | OBJECT;
    return type;
  }

  /* Getting here should not be possible */
  return UNKNOWN;
}

/* ----------------------------------------------- */
/* Use UNIVERSAL::can, taken from Haskells's Plugs */
/* ----------------------------------------------- */
static short marpaESLIF_canb(pTHX_ SV *sv, char *method) {
  SV *svp  = marpaESLIF_canp(aTHX_ sv, method);
  int type = marpaESLIF_getTypei(aTHX_ svp);
  return (type & CODEREF) == CODEREF;
}

/* ----------------------------------------------- */
/* Use UNIVERSAL::can, taken from Haskells's Plugs */
/* ----------------------------------------------- */
static SV *marpaESLIF_canp(pTHX_ SV *sv, char *method) {
  SV *svp;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  XPUSHs(sv);
  XPUSHs(newSVpv(method, 0));
  PUTBACK;

  call_pv("UNIVERSAL::can", G_SCALAR);

  SPAGAIN;

  svp = newSVsv(POPs);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return svp;
}

/* -------------------------- */
/* Generic void method caller */
/* -------------------------- */
static void marpaESLIF_call_methodv(pTHX_ SV *svp, char *methods, SV *argsvp) {
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + ((argsvp != NULL) ? 1 : 0));
  PUSHs(svp);
  if (argsvp != NULL) {
    PUSHs(argsvp);
  }
  PUTBACK;

  call_method(methods, G_DISCARD);

  FREETMPS;
  LEAVE;
}

/* ------------------------ */
/* Generic SV method caller */
/* ------------------------ */
static SV *marpaESLIF_call_methodp(pTHX_ SV *svp, char *methods) {
  return marpaESLIF_call_actionp(aTHX_ svp, methods, NULL);
}

/* ------------------------ */
/* Generic SV action caller */
/* ------------------------ */
static SV *marpaESLIF_call_actionp(pTHX_ SV *svp, char *methods, AV *avp) {
  static const char *funcs = "marpaESLIF_call_actionp";
  SV *rcp;
  SSize_t avsizel = (avp != NULL) ? av_len(avp) + 1 : 0;
  SSize_t aviteratol;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + avsizel);
  PUSHs(svp);
  for (aviteratol = 0; aviteratol < avsizel; aviteratol++) {
    SV **svpp = av_fetch(avp, aviteratol, 0); /* We manage ourself the avp, SV's are real */
    if (svpp == NULL) {
      MARPAESLIF_CROAK("av_fetch returned NULL");
    }
    PUSHs(sv_2mortal(newSVsv(*svpp)));
  }
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rcp = newSVsv(POPs);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rcp;
}

/* ------------------------ */
/* Generic IV action caller */
/* ------------------------ */
static IV marpaESLIF_call_methodi(pTHX_ SV *svp, char *methods) {
  IV rci;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(svp);
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rci = POPi;

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rci;
}

/* ----------------------------- */
/* Generic boolean method caller */
/* ----------------------------- */
static short marpaESLIF_call_methodb(pTHX_ SV *svp, char *methods) {
  short rcb;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(svp);
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rcb = (POPi != 0);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rcb;
}

/* ----------------------- */
/* Generic Logger Callback */
/* ----------------------- */
static void marpaESLIF_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
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

    ENTER;
    SAVETMPS;

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

/* -------------------------- */
/* Recognizer Reader Callback */
/* -------------------------- */
static short marpaESLIF_recognizerReaderCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp) {
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp;
  SV                            *Perl_recognizerInterfacep;
  SV                            *Perl_datap;
  SV                            *Perl_encodingp;
  char                          *inputs = NULL;
  STRLEN                         inputl = 0;
  char                          *encodings = NULL;
  STRLEN                         encodingl = 0;
  dTHX;

  marpaESLIFRecognizerContextp = (marpaESLIFRecognizerContext_t *) userDatavp;
  Perl_recognizerInterfacep   = marpaESLIFRecognizerContextp->Perl_recognizerInterfacep;

  marpaESLIF_recognizerContextCleanupv(aTHX_ marpaESLIFRecognizerContextp);

  /* Call the read interface */
  marpaESLIF_call_methodv(aTHX_ Perl_recognizerInterfacep, "read", NULL);

  /* Call the data interface */
  Perl_datap = marpaESLIF_call_methodp(aTHX_ Perl_recognizerInterfacep, "data");
  if (SvOK(Perl_datap)) {
    inputs = SvPV(Perl_datap, inputl);
  }

  /* Call the encoding interface */
  Perl_encodingp  = marpaESLIF_call_methodp(aTHX_ Perl_recognizerInterfacep, "encoding");
  if (SvOK(Perl_encodingp)) {
    encodings = SvPV(Perl_encodingp, encodingl); /* May be {NULL, 0} */
  }

  *inputcpp             = inputs;
  *inputlp              = (size_t) inputl;
  *eofbp                = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isEof");
  *characterStreambp    = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isCharacterStream");
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  marpaESLIFRecognizerContextp->previous_Perl_datap     = Perl_datap;
  marpaESLIFRecognizerContextp->previous_Perl_encodingp = Perl_encodingp;

  return 1;
}

/* -------------------------- */
/* Value rule action resolver */
/* -------------------------- */
static marpaESLIFValueRuleCallback_t  marpaESLIF_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions) {
  marpaESLIFValueContext_t *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  marpaESLIFValueContextp->actions = actions;

  return marpaESLIF_valueRuleCallback;
}

/* ---------------------------- */
/* Value symbol action resolver */
/* ---------------------------- */
static marpaESLIFValueSymbolCallback_t marpaESLIF_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions) {
  marpaESLIFValueContext_t *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  marpaESLIFValueContextp->actions = actions;

  return marpaESLIF_valueSymbolCallback;
}

/* ------------------- */
/* Value rule callback */
/* ------------------- */
static short marpaESLIF_valueRuleCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb) {
  dTHX;
  static const char             *funcs = "marpaESLIF_valueRuleCallback";
  marpaESLIFValueContext_t      *marpaESLIFValueContextp      = (marpaESLIFValueContext_t *) userDatavp;
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp = marpaESLIFValueContextp->marpaESLIFRecognizerContextp;
  genericStack_t                *svStackp                     = marpaESLIFValueContextp->svStackp;
  AV                            *list                         = NULL;
  SV                            *actionResult;
  short                          rcb;
  int                            i;
  short                          intb;
  int                            indicei;
  short                          arrayb;
  void                          *bytep;
  size_t                         bytel;
  marpaESLIFValueResult_t       *marpaESLIFValueResultp;
  SV                            *objectp;
  int                            contexti;

  /* Prototype of a rule action is: public Object action(List<Object> list) */
  if (! nullableb) {
    /* Make the list */
    list = newAV();
    for (i = arg0i; i <= argni; i++) {
      if (! marpaESLIFValue_stack_is_intb(marpaESLIFValuep, i, &intb)) {
        MARPAESLIF_CROAK("marpaESLIFValue_stack_is_intb");
      }
      if (intb) {
        if (! marpaESLIFValue_stack_get_intb(marpaESLIFValuep, i, &contexti, &indicei)) {
          MARPAESLIF_CROAK("marpaESLIFValue_stack_get_intb failure");
        }
        objectp = (SV *) GENERICSTACK_GET_PTR(svStackp, indicei);
        /* This is an object created by the user interface, we manage its ref count */
        av_push(list, newSVsv(objectp));
      } else {
        /* This must be a lexeme - always in the form of an array */
        if (! marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, i, &arrayb)) {
          MARPAESLIF_CROAK("marpaESLIFValue_stack_is_arrayb failure");
        }
        if (! arrayb) {
          MARPAESLIF_CROAK("Internal stack error, item not an ARRAY");
        }
        if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, &contexti, &bytep, &bytel, NULL /* shallowbp */)) {
          MARPAESLIF_CROAK("marpaESLIFValue_stack_get_arrayb failure");
        }
        /* We never push array, i.e. contexti must be 0 in any case here */
        if (contexti != 0) {
          MARPAESLIF_CROAKF("marpaESLIFValue_stack_get_array success but contexti is %d instead of 0", contexti);
        }
        /* Either bytel is > 0, then this is the input, else this is a user-defined object */
        if (bytel > 0) {
          av_push(list, newSVpvn(bytep, bytel));
        } else {
          marpaESLIFValueResultp = (marpaESLIFValueResult_t *) bytep;
          if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
            MARPAESLIF_CROAKF("User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
          }
          objectp = (SV *) marpaESLIFValueResultp->u.p;
          av_push(list, newSVsv(objectp));
        }
      }
    }
  }

  /* Call the action */
  actionResult = marpaESLIF_call_actionp(aTHX_ marpaESLIFValueContextp->Perl_valueInterfacep, marpaESLIFValueContextp->actions, list);

  /* Remember the result (this is an SV that we own) */
  GENERICSTACK_PUSH_PTR(svStackp, actionResult);
  if (GENERICSTACK_ERROR(svStackp)) {
    MARPAESLIF_CROAKF("Stack push failure, %s", strerror(errno));
  }

  /* shallowb to a true value is very important because we get independant of an eventual free-action in the grammar */
  rcb =  marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, 1 /* context: any value != 0 */, GENERICSTACK_USED(svStackp) - 1);
  if (! rcb) {
    MARPAESLIF_CROAK("marpaESLIFValue_stack_set_intb failure");
  }

  if (list != NULL) {
    av_undef(list);
  }

  return rcb;
}

/*****************************************************************************/
static short marpaESLIF_valueSymbolCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIF_valueRuleCallback except that we construct a list of one element containing a byte array that we do ourself */
  static const char             *funcs                        = "marpaESLIF_valueSymbolCallback";
  marpaESLIFValueContext_t      *marpaESLIFValueContextp      = (marpaESLIFValueContext_t *) userDatavp;
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp = marpaESLIFValueContextp->marpaESLIFRecognizerContextp;
  genericStack_t                *svStackp                     = marpaESLIFRecognizerContextp->svStackp;
  AV                            *list                         = NULL;
  SV                            *actionResult;
  SV                            *objectp;
  marpaESLIFValueResult_t       *marpaESLIFValueResultp;
  short                          rcb;
  dTHX;

  /* Either bytel is > 0, then this is the input, else this is a user-defined object */
  if (bytel > 0) {
    objectp = newSVpvn(bytep, bytel);
  } else {
    marpaESLIFValueResultp = (marpaESLIFValueResult_t *) bytep;
    if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
      MARPAESLIF_CROAKF("User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
    }
    objectp = (SV *) marpaESLIFValueResultp->u.p;
  }

  list = newAV();
  av_push(list, newSVsv(objectp));

  /* Call the action */
  actionResult = marpaESLIF_call_actionp(aTHX_ marpaESLIFValueContextp->Perl_valueInterfacep, marpaESLIFValueContextp->actions, list);

  /* Remember the result (this is an SV that we own) */
  GENERICSTACK_PUSH_PTR(svStackp, actionResult);
  if (GENERICSTACK_ERROR(svStackp)) {
    MARPAESLIF_CROAKF("Stack push failure, %s", strerror(errno));
  }

  /* shallowb to a true value is very important because we get independant of an eventual free-action in the grammar */
  rcb =  marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, 1 /* context: any value != 0 */, GENERICSTACK_USED(svStackp) - 1);
  if (! rcb) {
    MARPAESLIF_CROAK("marpaESLIFValue_stack_set_intb failure");
  }

  if (list != NULL) {
    av_undef(list);
  }

  return rcb;
}

/*****************************************************************************/
static void marpaESLIF_valueContextFreev(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, short onStackb)
/*****************************************************************************/
{
  int i; 

  if (marpaESLIFValueContextp != NULL) {
    marpaESLIF_valueContextCleanup(aTHX_ marpaESLIFValueContextp);
    if (marpaESLIFValueContextp->svStackp != NULL) {
      GENERICSTACK_FREE(marpaESLIFValueContextp->svStackp);
    }
    if (! onStackb) {
      free(marpaESLIFValueContextp);
    }
    SvREFCNT_dec(marpaESLIFValueContextp->Perl_valueInterfacep);
    if (marpaESLIFValueContextp->marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(marpaESLIFValueContextp->marpaESLIFValuep);
    }
  }
}
 
/*****************************************************************************/
static void marpaESLIF_valueContextCleanup(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp)
/*****************************************************************************/
{
  genericStack_t *svStackp;
  SV             *svp;
  int             i; 

  if (marpaESLIFValueContextp != NULL) {
    svStackp = marpaESLIFValueContextp->svStackp;
    if (svStackp != NULL) {
      /* It is important to removed references in the reverse order of their creation */
      while (GENERICSTACK_USED(svStackp) > 0) {
        i = GENERICSTACK_USED(svStackp) - 1;
        if (GENERICSTACK_IS_PTR(svStackp, i)) {
          svp = (SV *) GENERICSTACK_POP_PTR(svStackp);
          SvREFCNT_dec(svp);
        } else {
          GENERICSTACK_USED(svStackp)--;
        }
      }
    }
  }
}
 
/* ----------------------- */
/* Recognizer Context free */
/* ----------------------- */
static void marpaESLIF_recognizerContextFreev(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp, short onStackb) {
  int             i;
  SV             *svp;
  genericStack_t *svStackp;

  if (marpaESLIFRecognizerContextp != NULL) {
    marpaESLIF_recognizerContextCleanupv(aTHX_ marpaESLIFRecognizerContextp);
    svStackp = marpaESLIFRecognizerContextp->svStackp;
    if (svStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (GENERICSTACK_USED(svStackp) > 0) {
        i = GENERICSTACK_USED(svStackp) - 1;
        if (GENERICSTACK_IS_PTR(svStackp, i)) {
          svp = (SV *) GENERICSTACK_POP_PTR(svStackp);
          SvREFCNT_dec(svp);
        } else {
          GENERICSTACK_USED(svStackp)--;
        }
      }
    }
    if (! onStackb) {
      Safefree(marpaESLIFRecognizerContextp);
    }
    SvREFCNT_dec(marpaESLIFRecognizerContextp->Perl_recognizerInterfacep);
    if (marpaESLIFRecognizerContextp->marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizer_freev(marpaESLIFRecognizerContextp->marpaESLIFRecognizerp);
    }
  }
}

/* -------------------------- */
/* Recognizer Context cleanup */
/* -------------------------- */
static void marpaESLIF_recognizerContextCleanupv(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp) {
  if (marpaESLIFRecognizerContextp != NULL) {
    if (marpaESLIFRecognizerContextp->previous_Perl_datap != NULL) {
      SvREFCNT_dec(marpaESLIFRecognizerContextp->previous_Perl_datap);
    }
    marpaESLIFRecognizerContextp->previous_Perl_datap = NULL;

    if (marpaESLIFRecognizerContextp->previous_Perl_encodingp != NULL) {
      SvREFCNT_dec(marpaESLIFRecognizerContextp->previous_Perl_encodingp);
    }
    marpaESLIFRecognizerContextp->previous_Perl_encodingp = NULL;
  }
}

/* --------------------------------- */
/* Recognizer context initialization */
/* --------------------------------- */
static void marpaESLIF_recognizerContextInit(pTHX_ SV *Perl_recognizerInterfacep, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp) {
  marpaESLIFRecognizerContextp->Perl_recognizerInterfacep = newSVsv(Perl_recognizerInterfacep);
  marpaESLIFRecognizerContextp->previous_Perl_datap       = NULL;
  marpaESLIFRecognizerContextp->previous_Perl_encodingp   = NULL;
  marpaESLIFRecognizerContextp->svStackp                  = NULL;
  marpaESLIFRecognizerContextp->marpaESLIFRecognizerp     = NULL;
}

/* ---------------------------- */
/* Value context initialization */
/* ---------------------------- */
static void marpaESLIF_valueContextInit(pTHX_ SV *Perl_valueInterfacep, marpaESLIFValueContext_t *marpaESLIFValueContextp, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp) {
  static const char *funcs = "marpaESLIF_valueContextInit";

  marpaESLIFValueContextp->Perl_valueInterfacep           = newSVsv(Perl_valueInterfacep);
  marpaESLIFValueContextp->actions                        = NULL;
  marpaESLIFValueContextp->svStackp                       = NULL;
  marpaESLIFValueContextp->marpaESLIFRecognizerContextp   = marpaESLIFRecognizerContextp;
  marpaESLIFValueContextp->marpaESLIFValuep               = NULL;

  GENERICSTACK_NEW(marpaESLIFValueContextp->svStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValueContextp->svStackp)) {
    MARPAESLIF_CROAKF("Stack initialization failure, %s", strerror(errno));
  }

}

/* ----------------------------------- */
/* Check if SV can be a grammar scalar */
/* ----------------------------------- */
static void marpaESLIF_paramIsGrammarv(pTHX_ SV *sv) {
  static const char *funcs = "marpaESLIF_paramIsGrammarv";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIF_CROAK("Grammar must be a scalar");
  }
}

/* ------------------------------ */
/* Check if SV can be an encoding */
/* ------------------------------ */
static void marpaESLIF_paramIsEncodingv(pTHX_ SV *sv) {
  static const char *funcs = "marpaESLIF_paramIsEncodingv";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIF_CROAK("Encoding must be a scalar");
  }
}

/* ------------------------------------- */
/* Check if SV can be a logger interface */
/* ------------------------------------- */
static void marpaESLIF_paramIsLoggerInterfacev(pTHX_ SV *sv) {
  static const char *funcs = "marpaESLIF_paramIsLoggerInterfacev";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIF_CROAK("Logger interface must be an object");
  }

  if (! marpaESLIF_canb(aTHX_ sv, "trace"))     MARPAESLIF_CROAK("Logger interface must be an object that can do \"trace\"");
  if (! marpaESLIF_canb(aTHX_ sv, "debug"))     MARPAESLIF_CROAK("Logger interface must be an object that can do \"debug\"");
  if (! marpaESLIF_canb(aTHX_ sv, "info"))      MARPAESLIF_CROAK("Logger interface must be an object that can do \"info\"");
  if (! marpaESLIF_canb(aTHX_ sv, "notice"))    MARPAESLIF_CROAK("Logger interface must be an object that can do \"notice\"");
  if (! marpaESLIF_canb(aTHX_ sv, "warning"))   MARPAESLIF_CROAK("Logger interface must be an object that can do \"warning\"");
  if (! marpaESLIF_canb(aTHX_ sv, "error"))     MARPAESLIF_CROAK("Logger interface must be an object that can do \"error\"");
  if (! marpaESLIF_canb(aTHX_ sv, "critical"))  MARPAESLIF_CROAK("Logger interface must be an object that can do \"critical\"");
  if (! marpaESLIF_canb(aTHX_ sv, "alert"))     MARPAESLIF_CROAK("Logger interface must be an object that can do \"alert\"");
  if (! marpaESLIF_canb(aTHX_ sv, "emergency")) MARPAESLIF_CROAK("Logger interface must be an object that can do \"emergency\"");
}

/* ----------------------------------------- */
/* Check if SV can be a recognizer interface */
/* ----------------------------------------- */
static void marpaESLIF_paramIsRecognizerInterfacev(pTHX_ SV *sv) {
  static const char *funcs = "marpaESLIF_paramIsRecognizerInterfacev";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIF_CROAK("Recognizer interface must be an object");
  }

  if (! marpaESLIF_canb(aTHX_ sv, "read"))                   MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"read\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isEof"))                  MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"isEof\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isCharacterStream"))      MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"isCharacterStream\"");
  if (! marpaESLIF_canb(aTHX_ sv, "encoding"))               MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"encoding\"");
  if (! marpaESLIF_canb(aTHX_ sv, "data"))                   MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"data\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithDisableThreshold")) MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"isWithDisableThreshold\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithExhaustion"))       MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"isWithExhaustion\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithNewline"))          MARPAESLIF_CROAK("Recognizer interface must be an object that can do \"isWithNewline\"");
}

/* ------------------------------------ */
/* Check if SV can be a value interface */
/* ------------------------------------ */
static void marpaESLIF_paramIsValueInterfacev(pTHX_ SV *sv) {
  static const char *funcs = "marpaESLIF_paramIsValueInterfacev";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIF_CROAK("Value interface must be an object");
  }

  if (! marpaESLIF_canb(aTHX_ sv, "isWithHighRankOnly")) MARPAESLIF_CROAK("Value interface must be an object that can do \"isWithHighRankOnly\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithOrderByRank"))  MARPAESLIF_CROAK("Value interface must be an object that can do \"isWithOrderByRank\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithAmbiguous"))    MARPAESLIF_CROAK("Value interface must be an object that can do \"isWithAmbiguous\"");
  if (! marpaESLIF_canb(aTHX_ sv, "isWithNull"))         MARPAESLIF_CROAK("Value interface must be an object that can do \"isWithNull\"");
  if (! marpaESLIF_canb(aTHX_ sv, "maxParses"))          MARPAESLIF_CROAK("Value interface must be an object that can do \"maxParses\"");
  if (! marpaESLIF_canb(aTHX_ sv, "setResult"))          MARPAESLIF_CROAK("Value interface must be an object that can do \"setResult\"");
  if (! marpaESLIF_canb(aTHX_ sv, "getResult"))          MARPAESLIF_CROAK("Value interface must be an object that can do \"getResult\"");
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
  static const char *funcs = "MarpaX::ESLIF::new";
  MarpaX_ESLIF       MarpaX_ESLIFp;
  marpaESLIFOption_t marpaESLIFOption;

  if(items > 1) {
    marpaESLIF_paramIsLoggerInterfacev(aTHX_ Perl_loggerInterfacep = ST(1));
  }

  Newx(MarpaX_ESLIFp, 1, MarpaX_ESLIF_t);
  MarpaX_ESLIFp->Perl_loggerInterfacep = &PL_sv_undef;
  MarpaX_ESLIFp->genericLoggerp        = NULL;
  MarpaX_ESLIFp->marpaESLIFp           = NULL;

  /* ------------- */
  /* genericLogger */
  /* ------------- */
  if (Perl_loggerInterfacep != &PL_sv_undef) {
    MarpaX_ESLIFp->Perl_loggerInterfacep = newSVsv(Perl_loggerInterfacep);
    MarpaX_ESLIFp->genericLoggerp        = genericLogger_newp(marpaESLIF_genericLoggerCallbackv,
                                                                        MarpaX_ESLIFp->Perl_loggerInterfacep,
                                                                        GENERICLOGGER_LOGLEVEL_TRACE);
    if (MarpaX_ESLIFp->genericLoggerp == NULL) {
      MARPAESLIF_CROAKF("genericLogger_newp failure, %s", strerror(errno));
    }
  }

  /* ---------- */
  /* marpaESLIF */
  /* ---------- */
  marpaESLIFOption.genericLoggerp = MarpaX_ESLIFp->genericLoggerp;
  MarpaX_ESLIFp->marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (MarpaX_ESLIFp->marpaESLIFp == NULL) {
      MARPAESLIF_CROAKF("marpaESLIF_newp failure, %s", strerror(errno));
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::new";
  MarpaX_ESLIF_Grammar       MarpaX_ESLIF_Grammarp;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;
  marpaESLIFGrammar_t       *marpaESLIFGrammarp;
  char                      *bytep;
  STRLEN                     bytel;
  char                      *encodings = NULL;
  STRLEN                     encodingl = 0;

  marpaESLIF_paramIsGrammarv(aTHX_ Perl_grammarp);
  if (items > 3) {
    marpaESLIF_paramIsEncodingv(aTHX_ Perl_encodingp = ST(3));
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
    MARPAESLIF_CROAKF("marpaESLIFGrammar_newp failure, %s", strerror(errno));
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::ngrammar";
  int                ngrammari;

  if (! marpaESLIFGrammar_ngrammarib(MarpaX_ESLIF_Grammarp, &ngrammari)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ngrammarib failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::currentLevel";
  int                leveli;

  if (! marpaESLIFGrammar_grammar_currentb(MarpaX_ESLIF_Grammarp, &leveli, NULL)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_grammar_currentb failure");
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
  static const char   *funcs = "MarpaX::ESLIF::Grammar::currentDescription";
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_currentb(MarpaX_ESLIF_Grammarp, NULL, &descp)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_grammar_currentb failure");
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
  static const char   *funcs = "MarpaX::ESLIF::Grammar::descriptionByLevel";
  marpaESLIFString_t  *descp;
  SV                  *sv;

  if (! marpaESLIFGrammar_grammar_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_leveli, NULL, NULL, &descp)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_grammar_by_levelb failure");
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
  static const char   *funcs = "MarpaX::ESLIF::Grammar::currentRuleIds";
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_currentb(MarpaX_ESLIF_Grammarp, &ruleip, &rulel)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_rulearray_currentb failure");
  }
  if (rulel <= 0) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_rulearray_currentb returned no rule");
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
  static const char   *funcs = "MarpaX::ESLIF::Grammar::ruleIdsByLevel";
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_by_levelb(MarpaX_ESLIF_Grammarp, &ruleip, &rulel, (int) Perl_leveli, NULL)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_rulearray_by_levelb failure");
  }
  if (rulel <= 0) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_rulearray_by_levelb returned no rule");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleDisplay";
  char              *ruledisplays;

  if (! marpaESLIFGrammar_ruleshowform_currentb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruledisplays)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ruledisplayform_currentb failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleShow";
  char              *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_currentb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruleshows)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ruleshowform_currentb failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleDisplayByLevel";
  char              *ruledisplays;

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruledisplays, (int) Perl_leveli, NULL)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ruledisplayform_by_levelb failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleShowByLevel";
  char              *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruleshows, (int) Perl_leveli, NULL)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ruleshowform_by_levelb failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::show";
  char              *shows;

  if (! marpaESLIFGrammar_grammarshowform_currentb(MarpaX_ESLIF_Grammarp, &shows)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_ruleshowform_by_levelb failure");
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
  static const char *funcs = "MarpaX::ESLIF::Grammar::showByLevel";
  char              *shows;

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(MarpaX_ESLIF_Grammarp, &shows, (int) Perl_leveli, NULL)) {
    MARPAESLIF_CROAK("marpaESLIFGrammar_grammarshowform_by_levelb failure");
  }
  RETVAL = shows;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::parse                                           */
  /* ----------------------------------------------------------------------- */
=cut

bool
parse(MarpaX_ESLIF_Grammarp, Perl_recognizerInterfacep, Perl_valueInterfacep)
  MarpaX_ESLIF_Grammar  MarpaX_ESLIF_Grammarp;
  SV                   *Perl_recognizerInterfacep;
  SV                   *Perl_valueInterfacep;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Grammar::parse";
  marpaESLIFRecognizerOption_t   marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t        marpaESLIFValueOption;
  marpaESLIFValueResult_t        marpaESLIFValueResult;
  marpaESLIFRecognizerContext_t  marpaESLIFRecognizerContext;
  marpaESLIFValueContext_t       marpaESLIFValueContext;
  short                          exhaustedb;
  int                            indicei;
  SV                            *svp;
  bool                           rcb;

  marpaESLIF_paramIsRecognizerInterfacev(aTHX_ Perl_recognizerInterfacep);
  marpaESLIF_paramIsValueInterfacev(aTHX_ Perl_valueInterfacep);

  marpaESLIF_recognizerContextInit(aTHX_ Perl_recognizerInterfacep, &marpaESLIFRecognizerContext);
  marpaESLIF_valueContextInit(aTHX_ Perl_valueInterfacep, &marpaESLIFValueContext, &marpaESLIFRecognizerContext);
  
  marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFRecognizerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = marpaESLIF_recognizerReaderCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb         = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithDisableThreshold");
  marpaESLIFRecognizerOption.exhaustedb                = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithExhaustion");
  marpaESLIFRecognizerOption.newlineb                  = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithNewline");
  marpaESLIFRecognizerOption.bufsizl                   = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */

  
  marpaESLIFValueOption.userDatavp                     = &marpaESLIFValueContext;
  marpaESLIFValueOption.ruleActionResolverp            = marpaESLIF_valueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp          = marpaESLIF_valueSymbolActionResolver;
  marpaESLIFValueOption.freeActionResolverp            = NULL; /* We always push only integers */
  marpaESLIFValueOption.highRankOnlyb                  = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithHighRankOnly");
  marpaESLIFValueOption.orderByRankb                   = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithOrderByRank");
  marpaESLIFValueOption.ambiguousb                     = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithAmbiguous");
  marpaESLIFValueOption.nullb                          = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithNull");
  marpaESLIFValueOption.maxParsesi                     = (int) marpaESLIF_call_methodi(aTHX_ Perl_valueInterfacep, "maxParses");

  /* We never need marpaESLIF to keep the stack of values because it is managed directly in Java */
  if (! marpaESLIFGrammar_parseb(MarpaX_ESLIF_Grammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, &exhaustedb, &marpaESLIFValueResult)) {
    goto err;
  }
  if (marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_INT) {
    MARPAESLIF_CROAKF("marpaESLIFValueResult.type is not MARPAESLIF_VALUE_TYPE_INT but %d", marpaESLIFValueResult.type);
  }
  indicei = marpaESLIFValueResult.u.i;

  if (! GENERICSTACK_IS_PTR(marpaESLIFValueContext.svStackp, indicei)) {
    MARPAESLIF_CROAK("marpaESLIFValueResult.svStackp at resulti is not PTR");
  }

  svp = (SV *) GENERICSTACK_GET_PTR(marpaESLIFValueContext.svStackp, indicei);
  /* We do NOT want this reference to be destroyed */
  GENERICSTACK_SET_NA(marpaESLIFValueContext.svStackp, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValueContext.svStackp)) {
    /* We do not want a double free of result, c.f. the call to marpaESLIFValueContextFree() */
    MARPAESLIF_CROAKF("Stack set failure, %s", strerror(errno));
  }

  marpaESLIF_call_methodv(aTHX_ Perl_valueInterfacep, "setResult", svp);

  rcb = 1;
  goto done;

  err:
  rcb = 0;

  done:
  marpaESLIF_valueContextFreev(aTHX_ &marpaESLIFValueContext, 1 /* onStackb */);
  marpaESLIF_recognizerContextFreev(aTHX_ &marpaESLIFRecognizerContext, 1 /* onStackb */);
  RETVAL = rcb;
OUTPUT:
  RETVAL

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Recognizer                                               */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Recognizer

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::new                                          */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Recognizer
new(Perl_recognizerInterfacep, Perl_MarpaX_ESLIF_Grammar)
  SV *Perl_recognizerInterfacep;
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammar;
CODE:
  static const char             *funcs = "MarpaX::ESLIF::Recognizer::new";
  marpaESLIFRecognizerOption_t   marpaESLIFRecognizerOption;
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp;
  marpaESLIFGrammar_t           *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) Perl_MarpaX_ESLIF_Grammar;

  marpaESLIF_paramIsRecognizerInterfacev(aTHX_ Perl_recognizerInterfacep);

  Newx(marpaESLIFRecognizerContextp, 1, marpaESLIFRecognizerContext_t);
  marpaESLIF_recognizerContextInit(aTHX_ Perl_recognizerInterfacep, marpaESLIFRecognizerContextp);

  marpaESLIFRecognizerOption.userDatavp                = marpaESLIFRecognizerContextp;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = marpaESLIF_recognizerReaderCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb         = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithDisableThreshold");
  marpaESLIFRecognizerOption.exhaustedb                = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithExhaustion");
  marpaESLIFRecognizerOption.newlineb                  = marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithNewline");
  marpaESLIFRecognizerOption.bufsizl                   = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */

  marpaESLIFRecognizerContextp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerContextp->marpaESLIFRecognizerp == NULL) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_newp failure, %s", strerror(errno));
  }

  RETVAL = marpaESLIFRecognizerContextp;
OUTPUT:
  RETVAL
