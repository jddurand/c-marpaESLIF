#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <marpaESLIF.h>
#include <genericLogger.h>
#include <genericStack.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "c-constant-types.inc"
#include "c-event-types.inc"
#include "c-value-types.inc"
#include "c-loggerLevel-types.inc"

/* Perl wrapper around malloc, free, etc... are just painful for genericstack, which is */
/* is implemented using header files, not a library... */
#ifdef malloc
#define current_malloc malloc
#endif
#ifdef calloc
#define current_calloc calloc
#endif
#ifdef realloc
#define current_realloc realloc
#endif
#ifdef free
#define current_free free
#endif
#ifdef memset
#define current_memset memset
#endif
#ifdef memcpy
#define current_memcpy memcpy
#endif
#ifdef memmove
#define current_memmove memmove
#endif

#undef malloc
#undef calloc
#undef realloc
#undef free
#undef memset
#undef memcpy
#undef memmove

static void *marpaESLIF_GENERICSTACK_NEW() {
  genericStack_t *stackp;

  GENERICSTACK_NEW(stackp);

  return stackp;
}

static void *marpaESLIF_GENERICSTACK_FREE(genericStack_t *stackp) {
  GENERICSTACK_FREE(stackp);
}

static void *marpaESLIF_GENERICSTACK_GET_PTR(genericStack_t *stackp, int indicei) {
  return GENERICSTACK_GET_PTR(stackp, indicei);
}

static void *marpaESLIF_GENERICSTACK_POP_PTR(genericStack_t *stackp) {
  return GENERICSTACK_POP_PTR(stackp);
}

static short marpaESLIF_GENERICSTACK_IS_PTR(genericStack_t *stackp, int indicei) {
  return GENERICSTACK_IS_PTR(stackp, indicei);
}

static void marpaESLIF_GENERICSTACK_PUSH_PTR(genericStack_t *stackp, void *p) {
  GENERICSTACK_PUSH_PTR(stackp, p);
}

static void marpaESLIF_GENERICSTACK_SET_PTR(genericStack_t *stackp, void *p, int i) {
  GENERICSTACK_SET_PTR(stackp, p, i);
}

static void marpaESLIF_GENERICSTACK_SET_NA(genericStack_t *stackp, int indicei) {
  GENERICSTACK_SET_NA(stackp, indicei);
}

static short marpaESLIF_GENERICSTACK_ERROR(genericStack_t *stackp) {
  return GENERICSTACK_ERROR(stackp);
}

static int marpaESLIF_GENERICSTACK_USED(genericStack_t *stackp) {
  return GENERICSTACK_USED(stackp);
}

static int marpaESLIF_GENERICSTACK_SET_USED(genericStack_t *stackp, int usedi) {
  return GENERICSTACK_USED(stackp) = usedi;
}

#ifdef current_malloc
#define malloc current_malloc
#endif
#ifdef current_calloc
#define calloc current_calloc
#endif
#ifdef current_free
#define free current_free
#endif
#ifdef current_memset
#define memset current_memset
#endif
#ifdef current_memcpy
#define memcpy current_memcpy
#endif
#ifdef current_memmove
#define memmove current_memmove
#endif

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
  genericStack_t         *lexemeStackp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   exhaustedb;
  short                   canContinueb;
} marpaESLIFRecognizerContext_t, MarpaX_ESLIF_Recognizer_t;

/* Value context */
typedef struct marpaESLIFValueContext {
  SV                            *Perl_valueInterfacep;         /* Current eslifValueInterface instance */
  char                          *actions;                      /* Shallow copy of last resolved name */
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
static int                             marpaESLIF_getTypei(pTHX_ SV* svp);
static short                           marpaESLIF_canb(pTHX_ SV *svp, char *method);
static void                            marpaESLIF_call_methodv(pTHX_ SV *svp, char *methods, SV *argsvp);
static SV                             *marpaESLIF_call_methodp(pTHX_ SV *svp, char *methods);
static SV                             *marpaESLIF_call_actionp(pTHX_ SV *svp, char *methods, AV *avp);
static SV                             *marpaESLIF_call_actionv(pTHX_ SV *svp, char *methods, AV *avp);
static IV                              marpaESLIF_call_methodi(pTHX_ SV *svp, char *methods);
static short                           marpaESLIF_call_methodb(pTHX_ SV *svp, char *methods);
static void                            marpaESLIF_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short                           marpaESLIF_recognizerReaderCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t   marpaESLIF_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t marpaESLIF_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueFreeCallback_t   marpaESLIF_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static SV                             *marpaESLIF_getSvFromStack(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, marpaESLIFValue_t *marpaESLIFValuep, int i, char *bytep, size_t bytel, short trueSVOnlyb);
static short                           marpaESLIF_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           marpaESLIF_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void                            marpaESLIF_valueFreeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel);
static void                            marpaESLIF_valueContextFreev(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, short onStackb);
static void                            marpaESLIF_recognizerContextFreev(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp, short onStackb);
static void                            marpaESLIF_recognizerContextCleanupv(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp);
static void                            marpaESLIF_recognizerContextInit(pTHX_ SV *Perl_recognizerInterfacep, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp);
static void                            marpaESLIF_valueContextInit(pTHX_ SV *Perl_valueInterfacep, marpaESLIFValueContext_t *marpaESLIFValueContextp);
static void                            marpaESLIF_paramIsGrammarv(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsEncodingv(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsLoggerInterfacev(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsRecognizerInterfacev(pTHX_ SV *sv);
static void                            marpaESLIF_paramIsValueInterfacev(pTHX_ SV *sv);

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/
#define MARPAESLIF_FILENAMES "ESLIF.xs"

#define MARPAESLIF_CROAK(msgs)       croak("[In %s at %s:%d] %s", funcs, MARPAESLIF_FILENAMES, __LINE__, msgs)
#define MARPAESLIF_CROAKF(fmts, ...) croak("[In %s at %s:%d]" fmts, funcs, MARPAESLIF_FILENAMES, __LINE__, __VA_ARGS__)

/*****************************************************************************/
/* Copy of Params-Validate-1.26/lib/Params/Validate/XS.xs                    */
/*****************************************************************************/
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
static int marpaESLIF_getTypei(pTHX_ SV* svp) {
  int type = 0;

  if (SvTYPE(svp) == SVt_PVGV) {
    return GLOB;
  }
  if (!SvOK(svp)) {
    return UNDEF;
  }
  if (!SvROK(svp)) {
    return SCALAR;
  }

  switch (SvTYPE(SvRV(svp))) {
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
    if (sv_isobject(svp)) return type | OBJECT;
    return type;
  }

  /* Getting here should not be possible */
  return UNKNOWN;
}

/*****************************************************************************/
static short marpaESLIF_canb(pTHX_ SV *svp, char *method)
/*****************************************************************************/
{
  AV *list = newAV();
  SV *rcp;
  int type;

  av_push(list, newSVpv(method, 0));
  rcp = marpaESLIF_call_actionp(aTHX_ svp, "can", list);
  av_undef(list);

  type = marpaESLIF_getTypei(aTHX_ rcp);
  SvREFCNT_dec(rcp);

  return (type & CODEREF) == CODEREF;
}

/*****************************************************************************/
static void marpaESLIF_call_methodv(pTHX_ SV *svp, char *methods, SV *argsvp)
/*****************************************************************************/
{
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + ((argsvp != NULL) ? 1 : 0));
  PUSHs(sv_2mortal(newSVsv(svp)));
  if (argsvp != NULL) {
    PUSHs(sv_2mortal(newSVsv(argsvp)));
  }
  PUTBACK;

  call_method(methods, G_DISCARD);

  FREETMPS;
  LEAVE;
}

/*****************************************************************************/
static SV *marpaESLIF_call_methodp(pTHX_ SV *svp, char *methods)
/*****************************************************************************/
{
  return marpaESLIF_call_actionp(aTHX_ svp, methods, NULL);
}

/*****************************************************************************/
static SV *marpaESLIF_call_actionp(pTHX_ SV *svp, char *methods, AV *avp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIF_call_actionp";
  SV *rcp;
  SSize_t avsizel = (avp != NULL) ? av_len(avp) + 1 : 0;
  SSize_t aviteratol;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + avsizel);
  PUSHs(sv_2mortal(newSVsv(svp)));
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

  rcp = SvREFCNT_inc(POPs);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rcp;
}

/*****************************************************************************/
static SV *marpaESLIF_call_actionv(pTHX_ SV *svp, char *methods, AV *avp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIF_call_actionp";
  SSize_t avsizel = (avp != NULL) ? av_len(avp) + 1 : 0;
  SSize_t aviteratol;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + avsizel);
  PUSHs(sv_2mortal(newSVsv(svp)));
  for (aviteratol = 0; aviteratol < avsizel; aviteratol++) {
    SV **svpp = av_fetch(avp, aviteratol, 0); /* We manage ourself the avp, SV's are real */
    if (svpp == NULL) {
      MARPAESLIF_CROAK("av_fetch returned NULL");
    }
    PUSHs(sv_2mortal(newSVsv(*svpp)));
  }
  PUTBACK;

  call_method(methods, G_DISCARD);

  SPAGAIN;

  PUTBACK;
  FREETMPS;
  LEAVE;
}

/*****************************************************************************/
static IV marpaESLIF_call_methodi(pTHX_ SV *svp, char *methods)
/*****************************************************************************/
{
  IV rci;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(sv_2mortal(newSVsv(svp)));
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rci = POPi;

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rci;
}

/*****************************************************************************/
static short marpaESLIF_call_methodb(pTHX_ SV *svp, char *methods)
/*****************************************************************************/
{
  short rcb;
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(sv_2mortal(newSVsv(svp)));
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rcb = (POPi != 0);

  PUTBACK;
  FREETMPS;
  LEAVE;

  return rcb;
}

/*****************************************************************************/
static void marpaESLIF_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
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
    PUSHs(sv_2mortal(newSVsv(Perl_loggerInterfacep)));
    PUSHs(sv_2mortal(newSVpv(msgs,0)));
    PUTBACK;

    call_method(method, G_DISCARD);

    FREETMPS;
    LEAVE;
  }
}

/*****************************************************************************/
static short marpaESLIF_recognizerReaderCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  static const char             *funcs = "marpaESLIF_recognizerReaderCallbackb";
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp;
  SV                            *Perl_recognizerInterfacep;
  SV                            *Perl_datap;
  SV                            *Perl_encodingp;
  char                          *inputs = NULL;
  STRLEN                         inputl = 0;
  char                          *encodings = NULL;
  STRLEN                         encodingl = 0;
  int                            type;
  dTHX;

  marpaESLIFRecognizerContextp = (marpaESLIFRecognizerContext_t *) userDatavp;
  Perl_recognizerInterfacep   = marpaESLIFRecognizerContextp->Perl_recognizerInterfacep;

  marpaESLIF_recognizerContextCleanupv(aTHX_ marpaESLIFRecognizerContextp);

  /* Call the read interface */
  if (! marpaESLIF_call_methodb(aTHX_ Perl_recognizerInterfacep, "read")) {
    MARPAESLIF_CROAK("Recognizer->read() method failure");
  }

  /* Call the data interface */
  Perl_datap = marpaESLIF_call_methodp(aTHX_ Perl_recognizerInterfacep, "data");
  type = marpaESLIF_getTypei(aTHX_ Perl_datap);
  if ((type & SCALAR) != SCALAR) {
    /* This is an error unless it is undef */
    if ((type & UNDEF) != UNDEF) {
      MARPAESLIF_CROAK("Recognizer->data() method must return a scalar or undef");
    }
  }
  if (SvOK(Perl_datap)) {
    inputs = SvPV(Perl_datap, inputl);
  }

  /* Call the encoding interface */
  Perl_encodingp  = marpaESLIF_call_methodp(aTHX_ Perl_recognizerInterfacep, "encoding");
  type = marpaESLIF_getTypei(aTHX_ Perl_datap);
  if ((type & SCALAR) != SCALAR) {
    /* This is an error unless it is undef */
    if ((type & UNDEF) != UNDEF) {
      MARPAESLIF_CROAK("Recognizer->encoding() method must return a scalar or undef");
    }
  }
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

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t  marpaESLIF_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFValueContext_t *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  marpaESLIFValueContextp->actions = actions;

  return marpaESLIF_valueRuleCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t marpaESLIF_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFValueContext_t *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  marpaESLIFValueContextp->actions = actions;

  return marpaESLIF_valueSymbolCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueFreeCallback_t marpaESLIF_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFValueContext_t *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;

  /* It HAS to be ":defaultFreeActions" */
  if (strcmp(actions, ":defaultFreeActions") != 0) {
    return NULL;
  }

  /* Remember the action name - perl will croak if calling this method fails */
  marpaESLIFValueContextp->actions = actions;

  

  return marpaESLIF_valueFreeCallbackv;
}

/*****************************************************************************/
static SV *marpaESLIF_getSvFromStack(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, marpaESLIFValue_t *marpaESLIFValuep, int i, char *bytep, size_t bytel, short trueSVOnlyb)
/*****************************************************************************/
/* When called with !trueSVOnlyb, this function is guaranteed to return an SV in any case, it will HAVE TO BE refcount_dec'ed */
/* When called with  trueSVOnlyb, this function is guaranteed to return only an SV returned by the user, its refcount is not affected */
/* When called with bytep != NULL, then this will take that as a forced ARRAY element instead of inspecting the stack */
{
  static const char       *funcs = "marpaESLIF_getSvFromStack";
  SV                      *objectp;
  short                    ptrb;
  int                      indicei;
  short                    arrayb;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  int                      contexti;

  /* fprintf(stderr, "In %s for indice %d, bytep %p, bytel %ld\n", funcs, i, bytep, (unsigned long) bytel); */
  if (bytep != NULL) {
    /* Go immediately to array processing */
    goto is_array;
  }
  if (! marpaESLIFValue_stack_is_ptrb(marpaESLIFValuep, i, &ptrb)) {
    MARPAESLIF_CROAKF("marpaESLIFValue_stack_is_ptrb failure, %s", strerror(errno));
  }
  if (ptrb) {
    if (! marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &objectp, NULL /* shallowbp */)) {
      MARPAESLIF_CROAKF("marpaESLIFValue_stack_get_ptrb failure, %s", strerror(errno));
    }
    if (! trueSVOnlyb) {
      objectp = SvREFCNT_inc(objectp);
    }
  } else {
    /* This must be a lexeme or user-land object - always in the form of an array */
    if (! marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, i, &arrayb)) {
      MARPAESLIF_CROAK("marpaESLIFValue_stack_is_arrayb failure");
    }
    if (! arrayb) {
      MARPAESLIF_CROAK("Internal stack error, item not an ARRAY");
    }
    if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, &contexti, (void **) &bytep, &bytel, NULL /* shallowbp */)) {
      MARPAESLIF_CROAK("marpaESLIFValue_stack_get_arrayb failure");
    }
    /* We never push array, i.e. contexti must be 0 in any case here */
    if (contexti != 0) {
      MARPAESLIF_CROAKF("marpaESLIFValue_stack_get_array success but contexti is %d instead of 0", contexti);
    }
  is_array:
    /* Either bytel is > 0, then this is the input, else this is a user-defined object */
    if (bytel > 0) {
      objectp = trueSVOnlyb ? NULL : newSVpvn(bytep, bytel);
    } else {
      marpaESLIFValueResultp = (marpaESLIFValueResult_t *) bytep;
      if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
        MARPAESLIF_CROAKF("User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
      }
      objectp = (SV *) marpaESLIFValueResultp->u.p;
      if (! trueSVOnlyb) {
        objectp = SvREFCNT_inc(objectp);
      }
    }
  }

  /*
  if (objectp != NULL) {
    if (SvOK(objectp)) {
      char *s;
      STRLEN l;
      s = SvPV(objectp, l);
      fprintf(stderr, "... ... Retreived %s\n", s);
    } else {
      fprintf(stderr, "... ... OUPS !?\n");
      sv_dump(objectp);
    }
  } else {
    fprintf(stderr, "... ... Retreived NULL\n");
  }
  */

  return objectp;
}


/*****************************************************************************/
static short marpaESLIF_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  dTHX;
  static const char             *funcs = "marpaESLIF_valueRuleCallbackb";
  marpaESLIFValueContext_t      *marpaESLIFValueContextp = (marpaESLIFValueContext_t *) userDatavp;
  AV                            *list                    = NULL;
  SV                            *actionResult;
  int                            i;
  short                          intb;
  int                            indicei;
  short                          arrayb;
  void                          *bytep;
  size_t                         bytel;
  marpaESLIFValueResult_t       *marpaESLIFValueResultp;
  SV                            *objectp;
  int                            contexti;

  /* fprintf(stderr, "... Rule action %s Stack[%d..%d] => Stack[%d]\n", marpaESLIFValueContextp->actions, arg0i, argni, resulti); */

  if (! nullableb) {
    list = newAV();
    for (i = arg0i; i <= argni; i++) {
      av_push(list, marpaESLIF_getSvFromStack(aTHX_ marpaESLIFValueContextp, marpaESLIFValuep, i, NULL /* bytep */, 0 /* bytel */, 0 /* trueSVOnlyb */));
    }
  }
  actionResult = marpaESLIF_call_actionp(aTHX_ marpaESLIFValueContextp->Perl_valueInterfacep, marpaESLIFValueContextp->actions, list);
  if (list != NULL) {
    av_undef(list);
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, 1 /* context: any value != 0 */, (void *) actionResult, 0 /* shallowb */)) {
    MARPAESLIF_CROAK("marpaESLIFValue_stack_set_ptrb failure");
  }

  return 1;
}

/*****************************************************************************/
static short marpaESLIF_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIF_valueRuleCallbackb except that we construct a list of one element containing a byte array that we do ourself */
  static const char             *funcs                        = "marpaESLIF_valueSymbolCallbackb";
  marpaESLIFValueContext_t      *marpaESLIFValueContextp      = (marpaESLIFValueContext_t *) userDatavp;
  AV                            *list                         = NULL;
  SV                            *actionResult;
  SV                            *objectp;
  marpaESLIFValueResult_t       *marpaESLIFValueResultp;
  dTHX;

  /* fprintf(stderr, "... Symbol action %s => Stack[%d]\n", marpaESLIFValueContextp->actions, resulti); */

  list = newAV();
  av_push(list, marpaESLIF_getSvFromStack(aTHX_ marpaESLIFValueContextp, marpaESLIFValuep, -1 /* not used */, bytep, bytel, 0 /* trueSVOnlyb */));
  actionResult = marpaESLIF_call_actionp(aTHX_ marpaESLIFValueContextp->Perl_valueInterfacep, marpaESLIFValueContextp->actions, list);
  av_undef(list);

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, 1 /* context: any value != 0 */, (void *) actionResult, 0 /* shallowb */)) {
    MARPAESLIF_CROAK("marpaESLIFValue_stack_set_ptrb failure");
  }

  return 1;
}

/*****************************************************************************/
static void marpaESLIF_valueFreeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  dTHX;

  /* We are called when valuation is doing to withdraw an item in the stack that is a PTR or an ARRAY that we own */
  /* It is guaranteed to be non-NULL at this stage */
  /*
  fprintf(stderr, "Withdrawing:\n");
  sv_dump((SV *) p);
  */
  SvREFCNT_dec(p);
}

/*****************************************************************************/
static void marpaESLIF_valueContextFreev(pTHX_ marpaESLIFValueContext_t *marpaESLIFValueContextp, short onStackb)
/*****************************************************************************/
{
  int i; 

  if (marpaESLIFValueContextp != NULL) {
    SvREFCNT_dec(marpaESLIFValueContextp->Perl_valueInterfacep);
    if (marpaESLIFValueContextp->marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(marpaESLIFValueContextp->marpaESLIFValuep);
    }
    if (! onStackb) {
      Safefree(marpaESLIFValueContextp);
    }
  }
}
 
/*****************************************************************************/
static void marpaESLIF_recognizerContextFreev(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp, short onStackb)
/*****************************************************************************/
{
  int             i;
  SV             *svp;
  genericStack_t *lexemeStackp;

  if (marpaESLIFRecognizerContextp != NULL) {
    marpaESLIF_recognizerContextCleanupv(aTHX_ marpaESLIFRecognizerContextp);
    lexemeStackp = marpaESLIFRecognizerContextp->lexemeStackp;
    if (lexemeStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (marpaESLIF_GENERICSTACK_USED(lexemeStackp) > 0) {
        /* Last indice ... */
        i = marpaESLIF_GENERICSTACK_USED(lexemeStackp) - 1;
        /* ... is cleared ... */
        if (marpaESLIF_GENERICSTACK_IS_PTR(lexemeStackp, i)) {
          svp = (SV *) marpaESLIF_GENERICSTACK_GET_PTR(lexemeStackp, i);
          SvREFCNT_dec(svp);
        }
        /* ... and becomes current used size */
        marpaESLIF_GENERICSTACK_SET_USED(lexemeStackp, i);
      }
    }
    SvREFCNT_dec(marpaESLIFRecognizerContextp->Perl_recognizerInterfacep);
    if (marpaESLIFRecognizerContextp->marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizer_freev(marpaESLIFRecognizerContextp->marpaESLIFRecognizerp);
    }
    if (! onStackb) {
      Safefree(marpaESLIFRecognizerContextp);
    }
  }
}

/*****************************************************************************/
static void marpaESLIF_recognizerContextCleanupv(pTHX_ marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp)
/*****************************************************************************/
{
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

/*****************************************************************************/
static void marpaESLIF_recognizerContextInit(pTHX_ SV *Perl_recognizerInterfacep, marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp)
/*****************************************************************************/
{
  marpaESLIFRecognizerContextp->Perl_recognizerInterfacep = SvREFCNT_inc(Perl_recognizerInterfacep);
  marpaESLIFRecognizerContextp->previous_Perl_datap       = NULL;
  marpaESLIFRecognizerContextp->previous_Perl_encodingp   = NULL;
  marpaESLIFRecognizerContextp->lexemeStackp              = NULL;
  marpaESLIFRecognizerContextp->marpaESLIFRecognizerp     = NULL;
  marpaESLIFRecognizerContextp->exhaustedb                = 0;
  marpaESLIFRecognizerContextp->canContinueb              = 0;
}

/*****************************************************************************/
static void marpaESLIF_valueContextInit(pTHX_ SV *Perl_valueInterfacep, marpaESLIFValueContext_t *marpaESLIFValueContextp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIF_valueContextInit";

  marpaESLIFValueContextp->Perl_valueInterfacep  = SvREFCNT_inc(Perl_valueInterfacep);
  marpaESLIFValueContextp->actions               = NULL;
  marpaESLIFValueContextp->marpaESLIFValuep      = NULL;
}

/*****************************************************************************/
static void marpaESLIF_paramIsGrammarv(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIF_paramIsGrammarv";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIF_CROAK("Grammar must be a scalar");
  }
}

/*****************************************************************************/
static void marpaESLIF_paramIsEncodingv(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIF_paramIsEncodingv";
  int                type  = marpaESLIF_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIF_CROAK("Encoding must be a scalar");
  }
}

/*****************************************************************************/
static void marpaESLIF_paramIsLoggerInterfacev(pTHX_ SV *sv)
/*****************************************************************************/
{
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

/*****************************************************************************/
static void marpaESLIF_paramIsRecognizerInterfacev(pTHX_ SV *sv)
/*****************************************************************************/
{
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

/*****************************************************************************/
static void marpaESLIF_paramIsValueInterfacev(pTHX_ SV *sv)
/*****************************************************************************/
{
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
    MarpaX_ESLIFp->Perl_loggerInterfacep = SvREFCNT_inc(Perl_loggerInterfacep);
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
  static const char           *funcs = "MarpaX::ESLIF::Grammar::new";
  MarpaX_ESLIF_Grammar         MarpaX_ESLIF_Grammarp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  char                        *bytep;
  STRLEN                       bytel;
  char                        *encodings = NULL;
  STRLEN                       encodingl = 0;
  int                          ngrammari;
  int                          i;
  marpaESLIFGrammarDefaults_t  marpaESLIFGrammarDefaults;

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

  /* We want to take control over the free default action, and put something that is illegal via normal parse */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    MARPAESLIF_CROAKF("marpaESLIFGrammar_ngrammarib failure, %s", strerror(errno));
  }
  for (i = 0; i < ngrammari; i++) {
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      MARPAESLIF_CROAKF("marpaESLIFGrammar_defaults_by_levelb failure, %s", strerror(errno));
    }
    marpaESLIFGrammarDefaults.defaultFreeActions = ":defaultFreeActions";
    if (! marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      MARPAESLIF_CROAKF("marpaESLIFGrammar_defaults_by_levelb failure, %s", strerror(errno));
    }
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

  if (! marpaESLIFGrammar_ruledisplayform_currentb(MarpaX_ESLIF_Grammarp, (int) Perl_rulei, &ruledisplays)) {
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
  marpaESLIF_valueContextInit(aTHX_ Perl_valueInterfacep, &marpaESLIFValueContext);
  
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
  marpaESLIFValueOption.freeActionResolverp            = marpaESLIF_valueFreeActionResolver;
  marpaESLIFValueOption.highRankOnlyb                  = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithHighRankOnly");
  marpaESLIFValueOption.orderByRankb                   = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithOrderByRank");
  marpaESLIFValueOption.ambiguousb                     = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithAmbiguous");
  marpaESLIFValueOption.nullb                          = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithNull");
  marpaESLIFValueOption.maxParsesi                     = (int) marpaESLIF_call_methodi(aTHX_ Perl_valueInterfacep, "maxParses");

  /* We never need marpaESLIF to keep the stack of values because it is managed directly in Java */
  if (! marpaESLIFGrammar_parseb(MarpaX_ESLIF_Grammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, &exhaustedb, &marpaESLIFValueResult)) {
    goto err;
  }
  switch (marpaESLIFValueResult.type) {
  case MARPAESLIF_VALUE_TYPE_PTR:
    svp = (SV *) marpaESLIFValueResult.u.p;
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    svp = newSVpvn(marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
    break;
  default:
    MARPAESLIF_CROAKF("marpaESLIFValueResult.type is not MARPAESLIF_VALUE_TYPE_PTR nor MARPAESLIF_VALUE_TYPE_ARRAY but %d", marpaESLIFValueResult.type);
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
new(Perl_packagep, Perl_MarpaX_ESLIF_Grammar, Perl_recognizerInterfacep)
  SV *Perl_packagep;
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammar;
  SV *Perl_recognizerInterfacep;
CODE:
  static const char             *funcs = "MarpaX::ESLIF::Recognizer::new";
  marpaESLIFRecognizerOption_t   marpaESLIFRecognizerOption;
  marpaESLIFRecognizerContext_t *marpaESLIFRecognizerContextp;
  marpaESLIFGrammar_t           *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) Perl_MarpaX_ESLIF_Grammar;

  marpaESLIF_paramIsRecognizerInterfacev(aTHX_ Perl_recognizerInterfacep);

  Newx(marpaESLIFRecognizerContextp, 1, marpaESLIFRecognizerContext_t);
  marpaESLIF_recognizerContextInit(aTHX_ Perl_recognizerInterfacep, marpaESLIFRecognizerContextp);

  /* We need a lexeme stack in this mode (in contrary to the parse() method never calls back) */
  marpaESLIFRecognizerContextp->lexemeStackp = marpaESLIF_GENERICSTACK_NEW();
  if (marpaESLIFRecognizerContextp->lexemeStackp == NULL) {
    MARPAESLIF_CROAKF("GENERICSTACK_NEW() failure, %s", strerror(errno));
  }

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

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::DESTROY                                      */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(MarpaX_ESLIF_Recognizerp)
  MarpaX_ESLIF_Recognizer MarpaX_ESLIF_Recognizerp;
CODE:
  marpaESLIF_recognizerContextFreev(aTHX_ MarpaX_ESLIF_Recognizerp, 0 /* onStackb */);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::isCanContinue                                */
  /* ----------------------------------------------------------------------- */
=cut

bool
isCanContinue(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  RETVAL = Perl_MarpaX_ESLIF_Recognizer->canContinueb;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::isExhausted                                  */
  /* ----------------------------------------------------------------------- */
=cut

bool
isExhausted(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  RETVAL = Perl_MarpaX_ESLIF_Recognizer->exhaustedb;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::scan                                         */
  /* ----------------------------------------------------------------------- */
=cut

bool
scan(Perl_MarpaX_ESLIF_Recognizer, ...)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::scan";
  short initialEventsb;

  if (items > 1) {
    SV *Perl_initialEvents = ST(1);
    if ((marpaESLIF_getTypei(aTHX_ Perl_initialEvents) & SCALAR) != SCALAR) {
      MARPAESLIF_CROAK("First argument must be a scalar");
    }
    initialEventsb = SvIV(Perl_initialEvents) ? 1 : 0;
  } else {
    initialEventsb = 0;
  }

  RETVAL = marpaESLIFRecognizer_scanb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, initialEventsb, &(Perl_MarpaX_ESLIF_Recognizer->canContinueb), &(Perl_MarpaX_ESLIF_Recognizer->exhaustedb));
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::resume                                       */
  /* ----------------------------------------------------------------------- */
=cut

bool
resume(Perl_MarpaX_ESLIF_Recognizer, deltaLength)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  int deltaLength;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::resume";

  if (deltaLength < 0) {
    MARPAESLIF_CROAK("Resume delta length cannot be negative");
  }
RETVAL = marpaESLIFRecognizer_resumeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (size_t) deltaLength, &(Perl_MarpaX_ESLIF_Recognizer->canContinueb), &(Perl_MarpaX_ESLIF_Recognizer->exhaustedb));
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::events                                       */
  /* ----------------------------------------------------------------------- */
=cut

SV *
events(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::events";
  AV                *list = (AV *)sv_2mortal((SV *)newAV());
  HV                *hv;
  SV                *sv;
  int                i;
  size_t             eventArrayl;
  marpaESLIFEvent_t *eventArrayp;

  if (! marpaESLIFRecognizer_eventb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_eventb failure, %s", strerror(errno));
  }
  for (i = 0; i < eventArrayl; i++) {
    hv = (HV *)sv_2mortal((SV *)newHV());

    if (hv_store(hv, "type", strlen("type"), newSViv(eventArrayp[i].type), 0) == NULL) {
      MARPAESLIF_CROAKF("hv_store failure for type => %d", eventArrayp[i].type);
    }
    if (hv_store(hv, "symbol", strlen("symbol"), (eventArrayp[i].symbols != NULL) ? newSVpv(eventArrayp[i].symbols, 0) : &PL_sv_undef, 0) == NULL) {
      MARPAESLIF_CROAKF("hv_store failure for symbol => %s", (eventArrayp[i].symbols != NULL) ? eventArrayp[i].symbols : "");
    }
    if (hv_store(hv, "event",  strlen("event"),  (eventArrayp[i].events != NULL) ? newSVpv(eventArrayp[i].events, 0) : &PL_sv_undef, 0) == NULL) {
      MARPAESLIF_CROAKF("hv_store failure for event => %s", (eventArrayp[i].events != NULL) ? eventArrayp[i].events : "");
    }

    av_push(list, newRV((SV *)hv));
  }

  RETVAL = newRV((SV *)list);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::eventOnOff                                   */
  /* ----------------------------------------------------------------------- */
=cut

void
eventOnOff(Perl_MarpaX_ESLIF_Recognizer, symbol, eventTypes, onOff)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *symbol;
  AV                     *eventTypes;
  bool                    onOff;
CODE:
  static const char     *funcs = "MarpaX::ESLIF::Recognizer::eventOnOff";
  SSize_t                avsizel = av_len(eventTypes);
  SSize_t                aviteratol;
  marpaESLIFEventType_t  eventSeti  = MARPAESLIF_EVENTTYPE_NONE;

  for (aviteratol = 0; aviteratol < avsizel; aviteratol++) {
    int  codei;
    SV **svpp = av_fetch(eventTypes, aviteratol, 0);
    if (svpp == NULL) {
      MARPAESLIF_CROAK("av_fetch returned NULL");
    }
    if ((marpaESLIF_getTypei(aTHX_ *svpp) & SCALAR) != SCALAR) {
      MARPAESLIF_CROAKF("Element No %d of array must be a scalar", (int) aviteratol);
    }
    codei = (int) SvIV(*svpp);
    switch (codei) {
    case MARPAESLIF_EVENTTYPE_NONE:
      break;
    case MARPAESLIF_EVENTTYPE_COMPLETED:
    case MARPAESLIF_EVENTTYPE_NULLED:
    case MARPAESLIF_EVENTTYPE_PREDICTED:
    case MARPAESLIF_EVENTTYPE_BEFORE:
    case MARPAESLIF_EVENTTYPE_AFTER:
    case MARPAESLIF_EVENTTYPE_EXHAUSTED:
    case MARPAESLIF_EVENTTYPE_DISCARD:
      eventSeti |= codei;
      break;
    default:
      MARPAESLIF_CROAKF("Unknown code %d", (int) codei);
      break;
    }
  }

  if (! marpaESLIFRecognizer_event_onoffb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (char *) symbol, eventSeti, onOff ? 1 : 0)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_event_onoffb failure, %s", strerror(errno));
  }

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeAlternative                            */
  /* ----------------------------------------------------------------------- */
=cut

bool
lexemeAlternative(Perl_MarpaX_ESLIF_Recognizer, name, sv, ...)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
  SV                     *sv;
CODE:
  static const char       *funcs = "MarpaX::ESLIF::Recognizer::lexemeAlternative";
  marpaESLIFAlternative_t  marpaESLIFAlternative;
  int                      grammarLength;

  if (items > 3) {
    SV *Perl_grammarLength = ST(3);
    if ((marpaESLIF_getTypei(aTHX_ Perl_grammarLength) & SCALAR) != SCALAR) {
      MARPAESLIF_CROAK("Third argument must be a scalar");
    }
    grammarLength = (int) SvIV(Perl_grammarLength);
  } else {
    grammarLength = 1;
  }

  if (grammarLength <= 0) {
    MARPAESLIF_CROAK("grammarLength cannot be <= 0");
  }
  /* We maintain lifetime of this object */
  sv = SvREFCNT_inc(sv);
  marpaESLIF_GENERICSTACK_PUSH_PTR(Perl_MarpaX_ESLIF_Recognizer->lexemeStackp, sv);
  if (marpaESLIF_GENERICSTACK_ERROR(Perl_MarpaX_ESLIF_Recognizer->lexemeStackp)) {
    MARPAESLIF_CROAKF("Perl_MarpaX_ESLIF_Recognizer->lexemeStackp push failure, %s", strerror(errno));
  }

  marpaESLIFAlternative.lexemes        = (char *) name;
  marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.u.p      = sv;
  marpaESLIFAlternative.value.contexti = 0; /* Not used */
  marpaESLIFAlternative.value.sizel    = 0; /* Not used */
  marpaESLIFAlternative.grammarLengthl = (size_t) grammarLength;

  RETVAL = marpaESLIFRecognizer_lexeme_alternativeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &marpaESLIFAlternative);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeComplete                               */
  /* ----------------------------------------------------------------------- */
=cut

bool
lexemeComplete(Perl_MarpaX_ESLIF_Recognizer, length)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  int                     length;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeComplete";

  if (length < 0) {
    MARPAESLIF_CROAK("Length cannot be < 0");
  }

  RETVAL = marpaESLIFRecognizer_lexeme_completeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (size_t) length);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeRead                                   */
  /* ----------------------------------------------------------------------- */
=cut

bool
lexemeRead(Perl_MarpaX_ESLIF_Recognizer, name, sv, length, ...)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
  SV                     *sv;
  int                     length;
CODE:
  static const char       *funcs = "MarpaX::ESLIF::Recognizer::lexemeRead";
  marpaESLIFAlternative_t  marpaESLIFAlternative;
  int                      grammarLength = 1;

  if (items > 4) {
    SV *Perl_grammarLength = ST(4);
    if ((marpaESLIF_getTypei(aTHX_ Perl_grammarLength) & SCALAR) != SCALAR) {
      MARPAESLIF_CROAK("Fourth argument must be a scalar");
    }
    grammarLength = (int) SvIV(Perl_grammarLength);
  }

  if (grammarLength <= 0) {
    MARPAESLIF_CROAK("grammarLength cannot be <= 0");
  }
  /* We maintain lifetime of this object */
  sv = SvREFCNT_inc(sv);
  marpaESLIF_GENERICSTACK_PUSH_PTR(Perl_MarpaX_ESLIF_Recognizer->lexemeStackp, sv);
  if (marpaESLIF_GENERICSTACK_ERROR(Perl_MarpaX_ESLIF_Recognizer->lexemeStackp)) {
    MARPAESLIF_CROAKF("Perl_MarpaX_ESLIF_Recognizer->lexemeStackp push failure, %s", strerror(errno));
  }

  marpaESLIFAlternative.lexemes        = (char *) name;
  marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.u.p      = sv;
  marpaESLIFAlternative.value.contexti = 0; /* Not used */
  marpaESLIFAlternative.value.sizel    = 0; /* Not used */
  marpaESLIFAlternative.grammarLengthl = (size_t) grammarLength;

  RETVAL = marpaESLIFRecognizer_lexeme_readb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &marpaESLIFAlternative, (size_t) length);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeTry                                    */
  /* ----------------------------------------------------------------------- */
=cut

bool
lexemeTry(Perl_MarpaX_ESLIF_Recognizer, name)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeTry";
  short              rcb;

  if (! marpaESLIFRecognizer_lexeme_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, &rcb)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_lexeme_tryb failure, %s", strerror(errno));
  }
  RETVAL = rcb;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::discardTry                                   */
  /* ----------------------------------------------------------------------- */
=cut

bool
discardTry(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::discardTry";
  short              rcb;

  if (! marpaESLIFRecognizer_discard_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &rcb)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_discard_tryb failure, %s", strerror(errno));
  }
  RETVAL = rcb;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeExpected                               */
  /* ----------------------------------------------------------------------- */
=cut

SV *
lexemeExpected(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeExpected";
  AV                *list = (AV *)sv_2mortal((SV *)newAV());
  size_t             nLexeme;
  size_t             i;
  char             **lexemesArrayp;

  if (! marpaESLIFRecognizer_lexeme_expectedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &nLexeme, &lexemesArrayp)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_lexeme_expectedb failure, %s", strerror(errno));
  }
  if (nLexeme > 0) {
    EXTEND(sp, nLexeme);
    for (i = 0; i < nLexeme; i++) {
      av_push(list, (lexemesArrayp[i] != NULL) ? newSVpv(lexemesArrayp[i], 0) : &PL_sv_undef);
    }
  }
  RETVAL = newRV((SV *)list);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeLastPause                              */
  /* ----------------------------------------------------------------------- */
=cut

SV *
lexemeLastPause(Perl_MarpaX_ESLIF_Recognizer, lexeme)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *lexeme;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeLastPause";
  char              *pauses;
  size_t             pausel;

  if (!  marpaESLIFRecognizer_lexeme_last_pauseb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (char *) lexeme, &pauses, &pausel)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_lexeme_last_pauseb failure, %s", strerror(errno));
  }
  RETVAL = ((pauses != NULL) && (pausel > 0)) ? newSVpvn(pauses, pausel) : &PL_sv_undef;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lexemeLastTry                                */
  /* ----------------------------------------------------------------------- */
=cut

SV *
lexemeLastTry(Perl_MarpaX_ESLIF_Recognizer, lexeme)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *lexeme;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeLastTry";
  char              *trys;
  size_t             tryl;

  if (!  marpaESLIFRecognizer_lexeme_last_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (char *) lexeme, &trys, &tryl)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_lexeme_last_tryb failure, %s", strerror(errno));
  }
  RETVAL = ((trys != NULL) && (tryl > 0)) ? newSVpvn(trys, tryl) : &PL_sv_undef;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::discardLastTry                               */
  /* ----------------------------------------------------------------------- */
=cut

SV *
discardLastTry(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::discardLastTry";
  char              *discards;
  size_t             discardl;

  if (!  marpaESLIFRecognizer_discard_last_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &discards, &discardl)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_discard_last_tryb failure, %s", strerror(errno));
  }
  RETVAL = ((discards != NULL) && (discardl > 0)) ? newSVpvn(discards, discardl) : &PL_sv_undef;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::isEof                                        */
  /* ----------------------------------------------------------------------- */
=cut

bool
isEof(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::isEof";
  short              eofb;

  if (! marpaESLIFRecognizer_isEofb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &eofb)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_isEofb failure, %s", strerror(errno));
  }
  RETVAL = eofb;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::read                                         */
  /* ----------------------------------------------------------------------- */
=cut

bool
read(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::read";

  RETVAL = marpaESLIFRecognizer_readb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, NULL, NULL);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::input                                        */
  /* ----------------------------------------------------------------------- */
=cut

SV *
input(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::input";
  char              *inputs;
  size_t             inputl;

  if (! marpaESLIFRecognizer_inputb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &inputs, &inputl)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_inputb failure, %s", strerror(errno));
  }
  RETVAL = ((inputs != NULL) && (inputl > 0)) ? newSVpvn(inputs, inputl) : &PL_sv_undef;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::progressLog                                  */
  /* ----------------------------------------------------------------------- */
=cut

void
progressLog(Perl_MarpaX_ESLIF_Recognizer, start, end, level)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  int                     start;
  int                     end;
  int                     level;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::progressLog";

  switch (level) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
  case GENERICLOGGER_LOGLEVEL_DEBUG:
  case GENERICLOGGER_LOGLEVEL_INFO:
  case GENERICLOGGER_LOGLEVEL_NOTICE:
  case GENERICLOGGER_LOGLEVEL_WARNING:
  case GENERICLOGGER_LOGLEVEL_ERROR:
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
  case GENERICLOGGER_LOGLEVEL_ALERT:
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    break;
  default:
    MARPAESLIF_CROAKF("Unknown logger level %d", (int) level);
    break;
  }

  if (! marpaESLIFRecognizer_progressLogb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (int) start, (int) end, (int) level)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_progressLogb failure, %s", strerror(errno));
  }

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lastCompletedOffset                          */
  /* ----------------------------------------------------------------------- */
=cut

IV
lastCompletedOffset(Perl_MarpaX_ESLIF_Recognizer, name)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lastCompletedOffset";
  char              *offsetp = NULL;

  if (!  marpaESLIFRecognizer_last_completedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, &offsetp, NULL /* lengthlp */)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  RETVAL = PTR2IV(offsetp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lastCompletedLength                          */
  /* ----------------------------------------------------------------------- */
=cut

IV
lastCompletedLength(Perl_MarpaX_ESLIF_Recognizer, name)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
CODE:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lastCompletedLength";
  size_t             lengthl;

  if (!  marpaESLIFRecognizer_last_completedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, NULL /* offsetpp */, &lengthl)) {
    MARPAESLIF_CROAKF("marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  RETVAL = (IV) lengthl;
OUTPUT:
  RETVAL


MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Value

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::new                                               */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Value
new(Perl_packagep, MarpaX_ESLIF_Recognizerp, Perl_valueInterfacep)
  SV                      *Perl_packagep;
  MarpaX_ESLIF_Recognizer  MarpaX_ESLIF_Recognizerp;
  SV                      *Perl_valueInterfacep;
CODE:
  static const char        *funcs = "MarpaX::ESLIF::Value::new";
  MarpaX_ESLIF_Value        MarpaX_ESLIF_Valuep;
  marpaESLIFValueOption_t   marpaESLIFValueOption;
  marpaESLIFValueContext_t  marpaESLIFValueContext;

  marpaESLIF_paramIsValueInterfacev(aTHX_ Perl_valueInterfacep);

  Newx(MarpaX_ESLIF_Valuep, 1, MarpaX_ESLIF_Value_t);
  marpaESLIF_valueContextInit(aTHX_ Perl_valueInterfacep, MarpaX_ESLIF_Valuep);

  marpaESLIFValueOption.userDatavp            = MarpaX_ESLIF_Valuep;
  marpaESLIFValueOption.ruleActionResolverp   = marpaESLIF_valueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp = marpaESLIF_valueSymbolActionResolver;
  marpaESLIFValueOption.freeActionResolverp   = marpaESLIF_valueFreeActionResolver;
  marpaESLIFValueOption.highRankOnlyb         = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithHighRankOnly");
  marpaESLIFValueOption.orderByRankb          = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithOrderByRank");
  marpaESLIFValueOption.ambiguousb            = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithAmbiguous");
  marpaESLIFValueOption.nullb                 = marpaESLIF_call_methodb(aTHX_ Perl_valueInterfacep, "isWithNull");
  marpaESLIFValueOption.maxParsesi            = (int) marpaESLIF_call_methodi(aTHX_ Perl_valueInterfacep, "maxParses");

  MarpaX_ESLIF_Valuep->marpaESLIFValuep = marpaESLIFValue_newp(MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (MarpaX_ESLIF_Valuep->marpaESLIFValuep == NULL) {
    MARPAESLIF_CROAKF("marpaESLIFValue_newp failure, %s", strerror(errno));
  }

  RETVAL = MarpaX_ESLIF_Valuep;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::DESTROY                                           */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(MarpaX_ESLIF_Valuep)
  MarpaX_ESLIF_Value MarpaX_ESLIF_Valuep;
CODE:
  marpaESLIF_valueContextFreev(aTHX_ MarpaX_ESLIF_Valuep, 0 /* onStackb */);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::value                                             */
  /* ----------------------------------------------------------------------- */
=cut

bool
value(MarpaX_ESLIF_Valuep)
  MarpaX_ESLIF_Value MarpaX_ESLIF_Valuep;
CODE:
  static const char       *funcs = "MarpaX::ESLIF::Value::value";
  short                    valueb;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  SV                      *svp;

  valueb = marpaESLIFValue_valueb(MarpaX_ESLIF_Valuep->marpaESLIFValuep, &marpaESLIFValueResult);
  if (valueb < 0) {
    MARPAESLIF_CROAKF("marpaESLIFValue_valueb failure, %s", strerror(errno));
  }
  if (valueb > 0) {
    if (marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_PTR) {
      MARPAESLIF_CROAKF("marpaESLIFValueResult.type is not MARPAESLIF_VALUE_TYPE_PTR (found %d)", marpaESLIFValueResult.type);
    }
    svp = (SV *) marpaESLIFValueResult.u.p;
    marpaESLIF_call_methodv(aTHX_ MarpaX_ESLIF_Valuep->Perl_valueInterfacep, "setResult", svp);
    RETVAL = 1;
  } else {
    RETVAL = 0;
  }
OUTPUT:
  RETVAL

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Event::Type                                              */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Event::Type

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Event::Type::constant                                    */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-event-types.inc

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Value::Type                                              */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Value::Type

PROTOTYPES: ENABLE


=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::Type::constant                                    */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-value-types.inc

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Logger::Level                                            */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Logger::Level

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Logger::Level::constant                                  */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-loggerLevel-types.inc

