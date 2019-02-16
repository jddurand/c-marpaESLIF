#define PERL_NO_GET_CONTEXT 1     /* we want efficiency */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#include <marpaESLIF.h>
#include <genericLogger.h>
#include <genericStack.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>

/* Try to play with LDBL_DIG */
#ifndef DBL_DIG
# define DBL_DIG 15
#endif
#ifndef LDBL_DIG
  /* Compiler will optimize that */
#define LDBL_DIG ((sizeof(long double) == 10) ? 18 : (sizeof(long double) == 12) ? 18 : (sizeof(long double) == 16) ? 33 : DBL_DIG)
#endif
static char long_double_fmts[128];      /* Pre-filled format string for double */

#define MARPAESLIFPERL_CHUNKED_SIZE_UPPER(size, chunk) ((size) < (chunk)) ? (chunk) : ((1 + ((size) / (chunk))) * (chunk))
typedef struct marpaESLIFPerl_stringGenerator {
#ifdef PERL_IMPLICIT_CONTEXT
  tTHX          PerlInterpreterp;
#endif
  char         *s;      /* Pointer */
  size_t        l;      /* Used size */
  short         okb;    /* Status */
  size_t        allocl; /* Allocated size */
} marpaESLIFPerl_stringGenerator_t;


#include "c-constant-types.inc"
#include "c-event-types.inc"
#include "c-value-types.inc"
#include "c-loggerLevel-types.inc"
#include "c-rulePropertyBitSet-types.inc"
#include "c-symbolPropertyBitSet-types.inc"
#include "c-symbol-types.inc"

/* Encode constants as per the documentation */
#define MARPAESLIFPERL_ENCODE_DIE_ON_ERR    0x0001
#define MARPAESLIFPERL_ENCODE_WARN_ON_ERR   0x0002
#define MARPAESLIFPERL_ENCODE_RETURN_ON_ERR 0x0004
#define MARPAESLIFPERL_ENCODE_LEAVE_SRC     0x0008
#define MARPAESLIFPERL_ENCODE_PERLQQ        0x0100
#define MARPAESLIFPERL_ENCODE_HTMLCREF      0x0200
#define MARPAESLIFPERL_ENCODE_XMLCREF       0x0400

#define MARPAESLIFPERL_ENCODE_FB_DEFAULT    0
#define MARPAESLIFPERL_ENCODE_FB_CROAK      MARPAESLIFPERL_ENCODE_DIE_ON_ERR
#define MARPAESLIFPERL_ENCODE_FB_QUIET      MARPAESLIFPERL_ENCODE_RETURN_ON_ERR
#define MARPAESLIFPERL_ENCODE_FB_WARN       MARPAESLIFPERL_ENCODE_RETURN_ON_ERR | MARPAESLIFPERL_ENCODE_WARN_ON_ERR
#define MARPAESLIFPERL_ENCODE_FB_PERLQQ     MARPAESLIFPERL_ENCODE_LEAVE_SRC     | MARPAESLIFPERL_ENCODE_PERLQQ

/* #define MARPAESLIFPERL_UTF8_CROSSCHECK */ /* This will cross-check all marpESLIFValueString that claims to be UTF-8 */

/* Use the inc and dec macros that fit the best our code */
#ifdef SvREFCNT_dec_NN
#  define MARPAESLIFPERL_SvREFCNT_dec(svp) SvREFCNT_dec_NN(svp)
#else
#  define MARPAESLIFPERL_SvREFCNT_dec(svp) SvREFCNT_dec(svp)
#endif

#define MARPAESLIFPERL_ENCODING_IS_UTF8(encodings, encodingl)           \
  (                                                                     \
    /* UTF-8 */                                                         \
    (                                                                   \
      (encodingl == 5)                                 &&               \
      ((encodings[0] == 'U') || (encodings[0] == 'u')) &&               \
      ((encodings[1] == 'T') || (encodings[1] == 't')) &&               \
      ((encodings[2] == 'F') || (encodings[2] == 'f')) &&               \
       (encodings[3] == '-') &&                                         \
       (encodings[4] == '8')                                            \
    )                                                                   \
    ||                                                                  \
    /* UTF8 */                                                          \
    (                                                                   \
      (encodingl == 4)                                 &&               \
      ((encodings[0] == 'U') || (encodings[0] == 'u')) &&               \
      ((encodings[1] == 'T') || (encodings[1] == 't')) &&               \
      ((encodings[2] == 'F') || (encodings[2] == 'f')) &&               \
       (encodings[3] == '8')                                            \
    )                                                                   \
  )

#if defined(SvREFCNT_inc_simple_void_NN)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_simple_void_NN(svp)
#elif defined(SvREFCNT_inc_void_NN)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_void_NN(svp)
#elif defined(SvREFCNT_inc_simple_NN)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_simple_NN(svp)
#elif defined(SvREFCNT_inc_NN)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_NN(svp)
#elif defined(SvREFCNT_inc_simple_void)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_simple_void(svp)
#elif defined(SvREFCNT_inc_void)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_void(svp)
#elif defined(SvREFCNT_inc_simple)
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc_simple(svp)
#else
#  define MARPAESLIFPERL_SvREFCNT_inc(svp) SvREFCNT_inc(svp)
#endif

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

static void *marpaESLIFPerl_GENERICSTACK_NEW() {
  genericStack_t *stackp;

  GENERICSTACK_NEW(stackp);
  if (GENERICSTACK_ERROR(stackp)) {
    croak("GENERICSTACK_NEW failure, %s", strerror(errno));
  }

  return stackp;
}

static void marpaESLIFPerl_SYSTEM_FREE(void *p) {
  free(p);
}

static void marpaESLIFPerl_GENERICSTACK_INIT(genericStack_t *stackp) {
  GENERICSTACK_INIT(stackp);
  if (GENERICSTACK_ERROR(stackp)) {
    croak("GENERICSTACK_INIT failure, %s", strerror(errno));
  }
}

static void marpaESLIFPerl_GENERICSTACK_RESET(genericStack_t *stackp) {
  GENERICSTACK_RESET(stackp);
}

static void marpaESLIFPerl_GENERICSTACK_FREE(genericStack_t *stackp) {
  GENERICSTACK_FREE(stackp);
}

static void *marpaESLIFPerl_GENERICSTACK_GET_PTR(genericStack_t *stackp, int indicei) {
  return GENERICSTACK_GET_PTR(stackp, indicei);
}

static void *marpaESLIFPerl_GENERICSTACK_POP_PTR(genericStack_t *stackp) {
  return GENERICSTACK_POP_PTR(stackp);
}

static short marpaESLIFPerl_GENERICSTACK_IS_PTR(genericStack_t *stackp, int indicei) {
  return GENERICSTACK_IS_PTR(stackp, indicei);
}

static void marpaESLIFPerl_GENERICSTACK_PUSH_PTR(genericStack_t *stackp, void *p) {
  GENERICSTACK_PUSH_PTR(stackp, p);
}

static void marpaESLIFPerl_GENERICSTACK_SET_PTR(genericStack_t *stackp, void *p, int i) {
  GENERICSTACK_SET_PTR(stackp, p, i);
}

static void marpaESLIFPerl_GENERICSTACK_SET_NA(genericStack_t *stackp, int indicei) {
  GENERICSTACK_SET_NA(stackp, indicei);
}

static short marpaESLIFPerl_GENERICSTACK_ERROR(genericStack_t *stackp) {
  return GENERICSTACK_ERROR(stackp);
}

static int marpaESLIFPerl_GENERICSTACK_USED(genericStack_t *stackp) {
  return GENERICSTACK_USED(stackp);
}

static int marpaESLIFPerl_GENERICSTACK_SET_USED(genericStack_t *stackp, int usedi) {
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
static char _MARPAESLIFPERL_CONTEXT;
#define MARPAESLIFPERL_CONTEXT &_MARPAESLIFPERL_CONTEXT

/* For perl interpret retrieval */
#ifdef PERL_IMPLICIT_CONTEXT
#  ifndef tTHX
#    define tTHX PerlInterpreter*
#  endif
#  define marpaESLIFPerlMYTHX(contextp) tTHX aTHX = contextp->PerlInterpreterp
#else
#  define marpaESLIFPerlMYTHX(contextp) dNOOP
#endif

typedef struct MarpaX_ESLIF_Engine {
  SV              *Perl_loggerInterfacep;    /* inc()/dec()'ed to ensure proper DESTROY order */
  genericLogger_t *genericLoggerp;
  marpaESLIF_t    *marpaESLIFp;
#ifdef PERL_IMPLICIT_CONTEXT
  tTHX             PerlInterpreterp;
#endif
} MarpaX_ESLIF_Engine_t;

/* Nothing special for the grammar type */
typedef struct MarpaX_ESLIF_Grammar {
  SV                  *Perl_MarpaX_ESLIF_Enginep;    /* inc()/dec()'ed to ensure proper DESTROY order */
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
} MarpaX_ESLIF_Grammar_t;

/* Recognizer context */
typedef struct MarpaX_ESLIF_Recognizer {
  SV                     *Perl_MarpaX_ESLIF_Grammarp; /* inc()/dec()'ed to ensure proper DESTROY order */
  SV                     *Perl_recognizerInterfacep;  /* inc()/dec()'ed to ensure proper DESTROY order */
  SV                     *Perl_MarpaX_ESLIF_Recognizer_origp; /* Ditto - this is the SV when we are created using newFrom, or explicitely shared */
  SV                     *previous_Perl_datap;
  SV                     *previous_Perl_encodingp;
  genericStack_t          lexemeStack;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
#ifdef PERL_IMPLICIT_CONTEXT
  tTHX                    PerlInterpreterp;
#endif
} MarpaX_ESLIF_Recognizer_t;

/* Value context */
typedef struct MarpaX_ESLIF_Value {
  SV                     *Perl_valueInterfacep;          /* inc()/dec()'ed to ensure proper DESTROY order */
  SV                     *Perl_MarpaX_ESLIF_Recognizerp; /* inc()/dec()'ed to ensure proper DESTROY order - can be NULL */
  SV                     *Perl_MarpaX_ESLIF_Grammarp;    /* inc()/dec()'ed to ensure proper DESTROY order */
  char                   *actions;                       /* Shallow copy of last resolved name */
  void                   *previous_strings;              /* Latest stringification result */
  marpaESLIFValue_t      *marpaESLIFValuep;
  short                   canSetSymbolNameb;
  short                   canSetSymbolNumberb;
  short                   canSetRuleNameb;
  short                   canSetRuleNumberb;
  short                   canSetGrammarb;
  char                   *symbols;
  int                     symboli;
  char                   *rules;
  int                     rulei;
  genericStack_t          valueStack;
#ifdef PERL_IMPLICIT_CONTEXT
  tTHX                    PerlInterpreterp;
#endif
} MarpaX_ESLIF_Value_t;


/* For typemap */
typedef MarpaX_ESLIF_Engine_t     *MarpaX_ESLIF_Engine;
typedef MarpaX_ESLIF_Grammar_t    *MarpaX_ESLIF_Grammar;
typedef MarpaX_ESLIF_Recognizer_t *MarpaX_ESLIF_Recognizer;
typedef MarpaX_ESLIF_Value_t      *MarpaX_ESLIF_Value;

/* Static functions declarations */
static int                             marpaESLIFPerl_getTypei(pTHX_ SV* svp);
static short                           marpaESLIFPerl_canb(pTHX_ SV *svp, const char *methods);
static void                            marpaESLIFPerl_call_methodv(pTHX_ SV *interfacep, const char *methods, SV *argsvp);
static SV                             *marpaESLIFPerl_call_methodp(pTHX_ SV *interfacep, const char *methods);
static SV                             *marpaESLIFPerl_call_actionp(pTHX_ SV *interfacep, const char *methods, AV *avp, MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, short evalb, short evalSilentb);
static IV                              marpaESLIFPerl_call_methodi(pTHX_ SV *interfacep, const char *methods);
static short                           marpaESLIFPerl_call_methodb(pTHX_ SV *interfacep, const char *methods);
static void                            marpaESLIFPerl_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short                           marpaESLIFPerl_readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t   marpaESLIFPerl_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t marpaESLIFPerl_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueFreeCallback_t   marpaESLIFPerl_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static SV                             *marpaESLIFPerl_getSvp(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, char *bytep, size_t bytel);
static short                           marpaESLIFPerl_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           marpaESLIFPerl_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void                            marpaESLIFPerl_valueFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static void                            marpaESLIFPerl_ContextFreev(pTHX_ MarpaX_ESLIF_Engine_t *Perl_MarpaX_ESLIF_Enginep);
static void                            marpaESLIFPerl_grammarContextFreev(pTHX_ MarpaX_ESLIF_Grammar_t *Perl_MarpaX_ESLIF_Grammarp);
static void                            marpaESLIFPerl_valueContextFreev(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, short onStackb);
static void                            marpaESLIFPerl_valueContextCleanupv(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep);
static void                            marpaESLIFPerl_recognizerContextFreev(pTHX_ MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp, short onStackb);
static void                            marpaESLIFPerl_recognizerContextCleanupv(pTHX_ MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp);
static void                            marpaESLIFPerl_grammarContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Enginep, MarpaX_ESLIF_Grammar_t *Perl_MarpaX_ESLIF_Grammarp);
static void                            marpaESLIFPerl_recognizerContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Grammarp, SV *Perl_recognizerInterfacep, MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp, SV *Perl_MarpaX_ESLIF_Recognizer_origp);
static void                            marpaESLIFPerl_valueContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Recognizerp, SV *Perl_MarpaX_ESLIF_Grammarp, SV *Perl_valueInterfacep, MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep);
static void                            marpaESLIFPerl_paramIsGrammarv(pTHX_ SV *sv);
static void                            marpaESLIFPerl_paramIsEncodingv(pTHX_ SV *sv);
static short                           marpaESLIFPerl_paramIsLoggerInterfaceOrUndefb(pTHX_ SV *sv);
static void                            marpaESLIFPerl_paramIsRecognizerInterfacev(pTHX_ SV *sv);
static void                            marpaESLIFPerl_paramIsValueInterfacev(pTHX_ SV *sv);
static short                           marpaESLIFPerl_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp);
static char                           *marpaESLIFPerl_sv2byte(pTHX_ SV *svp, char **bytepp, size_t *bytelp, short encodingInformationb, short *characterStreambp, char **encodingsp, size_t *encodinglp, short warnIsFatalb);
static short                           marpaESLIFPerl_importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static void                            marpaESLIFPerl_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short                           marpaESLIFPerl_appendOpaqueDataToStringGenerator(marpaESLIFPerl_stringGenerator_t *marpaESLIFPerl_stringGeneratorp, char *p, size_t sizel);
static short                           marpaESLIFPerl_is_undef(pTHX_ SV *svp);
static short                           marpaESLIFPerl_is_arrayref(pTHX_ SV *svp);
static short                           marpaESLIFPerl_is_hashref(pTHX_ SV *svp);
static short                           marpaESLIFPerl_is_Types__Standard(pTHX_ SV *svp, const char *types);
static void                            marpaESLIFPerl_stack_setv(pTHX_ marpaESLIFValue_t *marpaESLIFValuep, short resulti, SV *svp);

/* Static constants */
static const char   *UTF8s = "UTF-8";
static const size_t  UTF8l = 5; /* "UTF-8" is 5 bytes in ASCII encoding */

/*****************************************************************************/
/* Static variables initialized at boot                                      */
/*****************************************************************************/
SV *boot_MarpaX__ESLIF__Grammar__Properties_svp;
SV *boot_MarpaX__ESLIF__Grammar__Rule__Properties_svp;
SV *boot_MarpaX__ESLIF__Grammar__Symbol__Properties_svp;
SV *boot_MarpaX__ESLIF__String_svp;
SV *boot_MarpaX__ESLIF_svp;
SV *boot_MarpaX__ESLIF__UTF_8_svp;
SV *boot_MarpaX__ESLIF__Math__BigFloat_svp;
short boot_nvtype_is_long_doubleb;
short boot_nvtype_is___float128;

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/
#define MARPAESLIFPERL_FILENAMES "ESLIF.xs"

#define MARPAESLIFPERL_CROAK(msgs)       croak("[In %s at %s:%d] %s", funcs, MARPAESLIFPERL_FILENAMES, __LINE__, msgs)
#define MARPAESLIFPERL_CROAKF(fmts, ...) croak("[In %s at %s:%d] " fmts, funcs, MARPAESLIFPERL_FILENAMES, __LINE__, __VA_ARGS__)
#define MARPAESLIFPERL_WARN(msgs)        warn("[In %s at %s:%d] %s", funcs, MARPAESLIFPERL_FILENAMES, __LINE__, msgs)
#define MARPAESLIFPERL_WARNF(fmts, ...)  warn("[In %s at %s:%d] " fmts, funcs, MARPAESLIFPERL_FILENAMES, __LINE__, __VA_ARGS__)
#define MARPAESLIFPERL_FREE_SVP(svp) do {        \
    SV *_svp = svp;                              \
    if (((_svp) != NULL)         &&              \
        ((_svp) != &PL_sv_undef) &&              \
        ((_svp) != &PL_sv_yes) &&                \
        ((_svp) != &PL_sv_no)) {                 \
      while (SvREFCNT(_svp) > 0) {               \
        MARPAESLIFPERL_SvREFCNT_dec(_svp);       \
      }                                          \
    }                                            \
  } while (0)

#define MARPAESLIFPERL_REFCNT_DEC(svp) do {      \
    SV *_svp = svp;                              \
    if (((_svp) != NULL)         &&              \
        ((_svp) != &PL_sv_undef) &&              \
        ((_svp) != &PL_sv_yes) &&                \
        ((_svp) != &PL_sv_no)) {                 \
      if (SvREFCNT(_svp) > 0) {                  \
        MARPAESLIFPERL_SvREFCNT_dec(_svp);       \
      }                                          \
    }                                            \
  } while (0)

#define MARPAESLIFPERL_REFCNT_INC(svp) do {      \
    SV *_svp = svp;                              \
    if (((_svp) != NULL)         &&              \
        ((_svp) != &PL_sv_undef) &&              \
        ((_svp) != &PL_sv_yes) &&                \
        ((_svp) != &PL_sv_no)) {                 \
      MARPAESLIFPERL_SvREFCNT_inc(_svp);         \
    }                                            \
  } while (0)

/* In this macro we hack the difference between hv_store and av_push by testing xvp type */
/* Remember that hv_store() and av_push() takes over one reference count. */
#define MARPAESLIFPERL_XV_STORE(xvp, key, svp) do {     \
    if (SvTYPE((SV *)xvp) == SVt_PVHV) {                \
      hv_store((HV *) xvp, key, strlen(key), svp, 0);   \
    } else {                                            \
      av_push((AV *) xvp, newSVpvn((const char *) key, (STRLEN) strlen(key))); \
      av_push((AV *) xvp, svp);                         \
    }                                                   \
  } while (0)

#define MARPAESLIFPERL_XV_STORE_UNDEF(xvp, key) MARPAESLIFPERL_XV_STORE(xvp, key, newSVsv(&PL_sv_undef))

#define MARPAESLIFPERL_XV_STORE_ACTION(hvp, key, actionp) do {          \
    SV *_svp;                                                           \
                                                                        \
    if (actionp != NULL) {                                              \
      switch (actionp->type) {                                          \
      case MARPAESLIF_ACTION_TYPE_NAME:                                 \
        MARPAESLIFPERL_XV_STORE(hvp, key, newSVpv(actionp->u.names, 0)); \
        break;                                                          \
      case MARPAESLIF_ACTION_TYPE_STRING:                               \
        _svp = newSVpvn((const char *) actionp->u.stringp->bytep, (STRLEN) actionp->u.stringp->bytel); \
        if (is_utf8_string((const U8 *) actionp->u.stringp->bytep, (STRLEN) actionp->u.stringp->bytel)) { \
          SvUTF8_on(_svp);                                              \
        }                                                               \
        MARPAESLIFPERL_XV_STORE(hvp, key, _svp);                        \
        break;                                                          \
      default:                                                          \
        warn("Unsupported action type %d", actionp->type);              \
        MARPAESLIFPERL_XV_STORE_UNDEF(hvp, key);                        \
        break;                                                          \
      }                                                                 \
    } else {                                                            \
      MARPAESLIFPERL_XV_STORE_UNDEF(hvp, key);                          \
    }                                                                   \
  } while (0)

#define MARPAESLIFPERL_XV_STORE_STRING(hvp, key, stringp) do {          \
    SV *_svp;                                                           \
                                                                        \
    if (stringp != NULL) {                                              \
      _svp = newSVpvn((const char *) stringp->bytep, (STRLEN) stringp->bytel); \
      if (is_utf8_string((const U8 *) stringp->bytep, (STRLEN) stringp->bytel)) { \
        SvUTF8_on(_svp);                                                \
      }                                                                 \
      MARPAESLIFPERL_XV_STORE(hvp, key, _svp);                          \
    } else {                                                            \
      MARPAESLIFPERL_XV_STORE_UNDEF(hvp, key);                          \
    }                                                                   \
  } while (0)

#define MARPAESLIFPERL_XV_STORE_ASCIISTRING(hvp, key, asciis) do {      \
    if (asciis != NULL) {                                               \
      MARPAESLIFPERL_XV_STORE(hvp, key, newSVpv(asciis, 0));            \
    } else {                                                            \
      MARPAESLIFPERL_XV_STORE_UNDEF(hvp, key);                          \
    }                                                                   \
  } while (0)

#define MARPAESLIFPERL_XV_STORE_IV(hvp, key, iv) do {                   \
    MARPAESLIFPERL_XV_STORE(hvp, key, newSViv((IV) iv));                \
  } while (0)

#define MARPAESLIFPERL_XV_STORE_IVARRAY(hvp, key, ivl, ivp) do {        \
    AV *_avp;                                                           \
    size_t _i;                                                          \
                                                                        \
    if (ivp != NULL) {                                                  \
      _avp = newAV();                                                   \
      if (ivl > 0) {                                                    \
        for (_i = 0; _i < ivl; _i++) {                                  \
          av_push(_avp, newSViv((IV) ivp[_i]));                         \
        }                                                               \
      }                                                                 \
      MARPAESLIFPERL_XV_STORE(hvp, key, newRV_inc((SV *) _avp));        \
    } else {                                                            \
      MARPAESLIFPERL_XV_STORE_UNDEF(hvp, key);                          \
    }                                                                   \
  } while (0)

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
static int marpaESLIFPerl_getTypei(pTHX_ SV* svp) {
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
static short marpaESLIFPerl_canb(pTHX_ SV *svp, const char *methods)
/*****************************************************************************/
{
  AV *list = newAV();
  SV *rcp;
  int type;

  /*
    fprintf(stderr, "START marpaESLIFPerl_canb(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  /* We always check methods that have ASCII only characters */
  av_push(list, newSVpv(methods, 0));
  rcp = marpaESLIFPerl_call_actionp(aTHX_ svp, "can", list, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(list);

  type = marpaESLIFPerl_getTypei(aTHX_ rcp);
  MARPAESLIFPERL_REFCNT_DEC(rcp);

  /*
    fprintf(stderr, "END marpaESLIFPerl_canb(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  return (type & CODEREF) == CODEREF;
}

/*****************************************************************************/
static void marpaESLIFPerl_call_methodv(pTHX_ SV *interfacep, const char *methods, SV *argsvp)
/*****************************************************************************/
{
  dSP;

  /*
    fprintf(stderr, "START marpaESLIFPerl_call_methodv(pTHX_ SV *svp, \"%s\", SV *argsvp)\n", methods);
  */
  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1 + ((argsvp != NULL) ? 1 : 0));
  PUSHs(sv_2mortal(newSVsv(interfacep)));
  if (argsvp != NULL) {
    PUSHs(sv_2mortal(newSVsv(argsvp)));
  }
  PUTBACK;

  call_method(methods, G_DISCARD);

  FREETMPS;
  LEAVE;
  /*
    fprintf(stderr, "END marpaESLIFPerl_call_methodv(pTHX_ SV *svp, \"%s\", SV *argsvp)\n", methods);
  */
}

/*****************************************************************************/
static SV *marpaESLIFPerl_call_methodp(pTHX_ SV *interfacep, const char *methods)
/*****************************************************************************/
{
  SV *rcp;

  /*
    fprintf(stderr, "START marpaESLIFPerl_call_methodp(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  rcp = marpaESLIFPerl_call_actionp(aTHX_ interfacep, methods, NULL /* avp */, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  /*
    fprintf(stderr, "END marpaESLIFPerl_call_methodp(pTHX_ SV *svp, \"%s\")\n", methods);
  */

  return rcp;
}

/*****************************************************************************/
static SV *marpaESLIFPerl_call_actionp(pTHX_ SV *interfacep, const char *methods, AV *avp, MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, short evalb, short evalSilentb)
/*****************************************************************************/
{
  static const char        *funcs      = "marpaESLIFPerl_call_actionp";
  SSize_t                   avsizel    = (avp != NULL) ? av_len(avp) + 1 : 0;
  SV                      **svargs     = NULL;
  I32                       flags      = G_SCALAR;
  SV                       *rcp;
  SV                       *Perl_valueInterfacep;
  SV                       *Perl_MarpaX_ESLIF_Grammarp;
  SV                       *svlocalp;
  char                     *symbols;
  int                       symboli;
  char                     *rules;
  int                       rulei;
  SSize_t                   aviteratorl;
  SV                        *err_tmp;
  dSP;

  if (evalb) {
    flags |= G_EVAL;
  }

  ENTER;
  SAVETMPS;

  if (Perl_MarpaX_ESLIF_Valuep != NULL) {
    /* This is an action context - we localize some variable */
    /* For GV_ADD: value is created once if needed - Perl will destroy it at exit */

    Perl_valueInterfacep       = Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep;
    Perl_MarpaX_ESLIF_Grammarp = Perl_MarpaX_ESLIF_Valuep->Perl_MarpaX_ESLIF_Grammarp;

    symbols = Perl_MarpaX_ESLIF_Valuep->symbols;
    svlocalp = get_sv("MarpaX::ESLIF::Context::symbolName", GV_ADD);
    save_item(svlocalp); /* We control this variable - no magic involved */
    if (symbols != NULL) {
      sv_setpvn(svlocalp, symbols, strlen(symbols));
    } else {
      sv_setsv(svlocalp, &PL_sv_undef);
    }
    if (Perl_MarpaX_ESLIF_Valuep->canSetSymbolNameb) {
      marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setSymbolName", svlocalp);
    }

    symboli = Perl_MarpaX_ESLIF_Valuep->symboli;
    svlocalp = get_sv("MarpaX::ESLIF::Context::symbolNumber", GV_ADD);
    save_item(svlocalp); /* We control this variable - no magic involved */
    sv_setiv(svlocalp, symboli);
    if (Perl_MarpaX_ESLIF_Valuep->canSetSymbolNumberb) {
      marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setSymbolNumber", svlocalp);
    }

    rules = Perl_MarpaX_ESLIF_Valuep->rules;
    svlocalp = get_sv("MarpaX::ESLIF::Context::ruleName", GV_ADD);
    save_item(svlocalp); /* We control this variable - no magic involved */
    if (rules != NULL) {
      sv_setpvn(svlocalp, rules, strlen(rules));
    } else {
      sv_setsv(svlocalp, &PL_sv_undef);
    }
    if (Perl_MarpaX_ESLIF_Valuep->canSetRuleNameb) {
      marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setRuleName", svlocalp);
    }

    rulei = Perl_MarpaX_ESLIF_Valuep->rulei;
    svlocalp = get_sv("MarpaX::ESLIF::Context::ruleNumber", GV_ADD);
    save_item(svlocalp); /* We control this variable - no magic involved */
    sv_setiv(svlocalp, rulei);
    if (Perl_MarpaX_ESLIF_Valuep->canSetRuleNumberb) {
      marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setRuleNumber", svlocalp);
    }

    svlocalp = get_sv("MarpaX::ESLIF::Context::grammar", GV_ADD);
    save_item(svlocalp); /* We control this variable - no magic involved */
    sv_setsv(svlocalp, Perl_MarpaX_ESLIF_Grammarp);
    if (Perl_MarpaX_ESLIF_Valuep->canSetGrammarb) {
      marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setGrammar", svlocalp);
    }

  }

  PUSHMARK(SP);
  if (interfacep != NULL) {
    EXTEND(SP, 1 + avsizel);
    PUSHs(sv_2mortal(newSVsv(interfacep)));
    for (aviteratorl = 0; aviteratorl < avsizel; aviteratorl++) {
      SV **svpp = av_fetch(avp, aviteratorl, 0); /* We manage ourself the avp, SV's are real */
      if (svpp == NULL) {
        MARPAESLIFPERL_CROAKF("av_fetch returned NULL during arguments preparation for method %s", (methods != NULL) ? methods : "undef");
      }
      PUSHs(sv_2mortal(newSVsv(*svpp)));
    }
  } else {
    if (avsizel > 0) {
      EXTEND(SP, avsizel);
      for (aviteratorl = 0; aviteratorl < avsizel; aviteratorl++) {
        SV **svpp = av_fetch(avp, aviteratorl, 0); /* We manage ourself the avp, SV's are real */
        if (svpp == NULL) {
          MARPAESLIFPERL_CROAKF("av_fetch returned NULL during arguments preparation for method %s", (methods != NULL) ? methods : "undef");
        }
        PUSHs(sv_2mortal(newSVsv(*svpp)));
      }
    }
  }
  PUTBACK;

  if (interfacep != NULL) {
    call_method(methods, flags);
  } else {
    call_pv(methods, flags);
  }

  if (evalb && (! evalSilentb)) {
    /* Check the eval */
    err_tmp = ERRSV;
    if (SvTRUE(err_tmp)) {
      warn("%s\n", SvPV_nolen(err_tmp));
     }
  }

  SPAGAIN;

  rcp = POPs;
  MARPAESLIFPERL_REFCNT_INC(rcp);

  PUTBACK;
  FREETMPS;
  LEAVE;


  return rcp;
}

/*****************************************************************************/
static IV marpaESLIFPerl_call_methodi(pTHX_ SV *interfacep, const char *methods)
/*****************************************************************************/
{
  IV rci;
  dSP;

  /*
    fprintf(stderr, "START marpaESLIFPerl_call_methodi(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(sv_2mortal(newSVsv(interfacep)));
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rci = POPi;

  PUTBACK;
  FREETMPS;
  LEAVE;

  /*
    fprintf(stderr, "END marpaESLIFPerl_call_methodi(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  return rci;
}

/*****************************************************************************/
static short marpaESLIFPerl_call_methodb(pTHX_ SV *interfacep, const char *methods)
/*****************************************************************************/
{
  short rcb;
  dSP;

  /*
    fprintf(stderr, "START marpaESLIFPerl_call_methodb(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  ENTER;
  SAVETMPS;

  PUSHMARK(SP);
  EXTEND(SP, 1);
  PUSHs(sv_2mortal(newSVsv(interfacep)));
  PUTBACK;

  call_method(methods, G_SCALAR);

  SPAGAIN;

  rcb = (POPi != 0);

  PUTBACK;
  FREETMPS;
  LEAVE;

  /*
    fprintf(stderr, "END marpaESLIFPerl_call_methodb(pTHX_ SV *svp, \"%s\")\n", methods);
  */
  return rcb;
}

/*****************************************************************************/
static void marpaESLIFPerl_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  MarpaX_ESLIF_Engine_t *Perl_MarpaX_ESLIF_Enginep = (MarpaX_ESLIF_Engine_t *) userDatavp;
  SV                    *Perl_loggerInterfacep = Perl_MarpaX_ESLIF_Enginep->Perl_loggerInterfacep;
  char *method;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Enginep);

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
    /* In addition ESLIF rarelly logs, propagating envp in the context */
    /* is an optimization that is almost useless */
    dSP;

    ENTER;
    SAVETMPS;

    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(sv_2mortal(newSVsv(Perl_loggerInterfacep)));
    /* We always log only with ASCII characters */
    PUSHs(sv_2mortal(newSVpv(msgs,0)));
    PUTBACK;

    call_method(method, G_DISCARD);

    FREETMPS;
    LEAVE;
  }
}

/*****************************************************************************/
static short marpaESLIFPerl_readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  static const char         *funcs = "marpaESLIFPerl_readerCallbackb";
  MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp = (MarpaX_ESLIF_Recognizer_t *) userDatavp;
  SV                        *Perl_recognizerInterfacep = Perl_MarpaX_ESLIF_Recognizerp->Perl_recognizerInterfacep;
  SV                        *Perl_datap;
  SV                        *Perl_encodingp;
  char                      *inputs = NULL;
  STRLEN                     inputl = 0;
  char                      *encodings = NULL;
  STRLEN                     encodingl = 0;
  int                        type;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Recognizerp);

  marpaESLIFPerl_recognizerContextCleanupv(aTHX_ Perl_MarpaX_ESLIF_Recognizerp);

  /* Call the read interface */
  if (! marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "read")) {
    MARPAESLIFPERL_CROAK("Recognizer->read() method failure");
  }

  /* Call the data interface */
  Perl_datap = marpaESLIFPerl_call_methodp(aTHX_ Perl_recognizerInterfacep, "data");
  type = marpaESLIFPerl_getTypei(aTHX_ Perl_datap);
  if ((type & SCALAR) != SCALAR) {
    /* This is an error unless it is undef */
    if ((type & UNDEF) != UNDEF) {
      MARPAESLIFPERL_CROAK("Recognizer->data() method must return a scalar or undef");
    }
  }
  if (SvOK(Perl_datap)) {
    inputs = SvPV(Perl_datap, inputl);
  }

  /* Call the encoding interface */
  Perl_encodingp  = marpaESLIFPerl_call_methodp(aTHX_ Perl_recognizerInterfacep, "encoding");
  type = marpaESLIFPerl_getTypei(aTHX_ Perl_datap);
  if ((type & SCALAR) != SCALAR) {
    /* This is an error unless it is undef */
    if ((type & UNDEF) != UNDEF) {
      MARPAESLIFPERL_CROAK("Recognizer->encoding() method must return a scalar or undef");
    }
  }
  if (SvOK(Perl_encodingp)) {
    encodings = SvPV(Perl_encodingp, encodingl); /* May be {NULL, 0} */
  } else {
    /* User gave no encoding hint - we can use Perl_datap itself */
    /* This will be ignored if *characterStreambp is false */
    if ((inputs != NULL) && DO_UTF8(Perl_datap)) {
      encodings           = (char *) UTF8s;
      encodingl           = UTF8l;
    }
  }

  *inputcpp             = inputs;
  *inputlp              = (size_t) inputl;
  *eofbp                = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isEof");
  *characterStreambp    = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isCharacterStream");
  *encodingsp           = encodings;
  *encodinglp           = encodingl;

  Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_datap     = Perl_datap;
  Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_encodingp = Perl_encodingp;

  return 1;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t  marpaESLIFPerl_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  Perl_MarpaX_ESLIF_Valuep->actions = actions;

  return marpaESLIFPerl_valueRuleCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t marpaESLIFPerl_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;

  /* Just remember the action name - perl will croak if calling this method fails */
  Perl_MarpaX_ESLIF_Valuep->actions = actions;

  return marpaESLIFPerl_valueSymbolCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueFreeCallback_t marpaESLIFPerl_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;

  /* It HAS to be ":defaultFreeActions" */
  if (strcmp(actions, ":defaultFreeActions") != 0) {
    return NULL;
  }

  /* Remember the action name - perl will croak if calling this method fails */
  Perl_MarpaX_ESLIF_Valuep->actions = actions;

  

  return marpaESLIFPerl_valueFreeCallbackv;
}

/*****************************************************************************/
static SV *marpaESLIFPerl_getSvp(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, char *bytep, size_t bytel)
/*****************************************************************************/
/* This function is guaranteed to return an SV in any case that will HAVE TO BE refcount_dec'ed: either this is a new SV, either this is a casted SV. */
/* The ref count of the returned SV is always incremented (1 when it is new, ++ when this is a casted SV) */
{
  static const char       *funcs = "marpaESLIFPerl_getSvp";
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;

  /* fprintf(stderr, "In %s for indice %d, bytep %p, bytel %ld\n", funcs, i, bytep, (unsigned long) bytel); */
  if (bytep != NULL) {
    /* Fake a marpaESLIFValueResult */
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_ARRAY;
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.u.a.p           = bytep;
    marpaESLIFValueResult.u.a.sizel       = bytel;
    marpaESLIFValueResult.u.a.shallowb    = 1;

    marpaESLIFValueResultp = &marpaESLIFValueResult;
  } else {
    marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, stackindicei);
    if (marpaESLIFValueResultp == NULL) {
      MARPAESLIFPERL_CROAKF("marpaESLIFValueResultp is NULL at stack indice %d", stackindicei);
    }
  }

  if (! marpaESLIFValue_importb(marpaESLIFValuep, marpaESLIFValueResultp, NULL /* marpaESLIFValueResultResolvedp */)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_importb failure, %s", strerror(errno));
  }

  if (marpaESLIFPerl_GENERICSTACK_USED(&(Perl_MarpaX_ESLIF_Valuep->valueStack)) != 1) {
    MARPAESLIFPERL_CROAKF("Internal value stack is %d instead of 1", marpaESLIFPerl_GENERICSTACK_USED(&(Perl_MarpaX_ESLIF_Valuep->valueStack)));
  }

  return (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
}


/*****************************************************************************/
static short marpaESLIFPerl_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char        *funcs                    = "marpaESLIFPerl_valueRuleCallbackb";
  MarpaX_ESLIF_Value_t     *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;
  SV                       *Perl_valueInterfacep     = Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep;
  AV                       *list                     = NULL;
  SV                       *actionResult;
  SV                       *svp;
  int                       i;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Valuep);

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(Perl_MarpaX_ESLIF_Valuep->symbols), &(Perl_MarpaX_ESLIF_Valuep->symboli), &(Perl_MarpaX_ESLIF_Valuep->rules), &(Perl_MarpaX_ESLIF_Valuep->rulei))) {
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  if (! nullableb) {
    list = newAV();
    for (i = arg0i; i <= argni; i++) {
      svp = marpaESLIFPerl_getSvp(aTHX_ Perl_MarpaX_ESLIF_Valuep, marpaESLIFValuep, i, NULL /* bytep */, 0 /* bytel */);
      /* One reference count ownership is transfered to the array */
      av_push(list, svp);
    }
  }

  actionResult = marpaESLIFPerl_call_actionp(aTHX_ Perl_valueInterfacep, Perl_MarpaX_ESLIF_Valuep->actions, list, Perl_MarpaX_ESLIF_Valuep, 0 /* evalb */, 0 /* evalSilentb */);
  if (list != NULL) {
    /* This will decrement all elements reference count */
    av_undef(list);
  }

  marpaESLIFPerl_stack_setv(aTHX_ marpaESLIFValuep, resulti, actionResult);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFPerl_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIFPerl_valueRuleCallbackb except that we construct a list of one element containing a byte array that we do ourself */
  static const char        *funcs                    = "marpaESLIFPerl_valueSymbolCallbackb";
  MarpaX_ESLIF_Value_t     *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;
  AV                       *list                     = NULL;
  SV                       *svp;
  SV                       *actionResult;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Valuep);

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(Perl_MarpaX_ESLIF_Valuep->symbols), &(Perl_MarpaX_ESLIF_Valuep->symboli), &(Perl_MarpaX_ESLIF_Valuep->rules), &(Perl_MarpaX_ESLIF_Valuep->rulei))) {
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  list = newAV();
  svp = marpaESLIFPerl_getSvp(aTHX_ Perl_MarpaX_ESLIF_Valuep, marpaESLIFValuep, -1 /* stackindicei */, bytep, bytel);
  /* One reference count ownership is transfered to the array */
  av_push(list, svp);
  actionResult = marpaESLIFPerl_call_actionp(aTHX_ Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep, Perl_MarpaX_ESLIF_Valuep->actions, list, Perl_MarpaX_ESLIF_Valuep, 0 /* evalb */, 0 /* evalSilentb */);
  /* This will decrement by one the inner element reference count */
  av_undef(list);

  marpaESLIFPerl_stack_setv(aTHX_ marpaESLIFValuep, resulti, actionResult);

  return 1;
}

/*****************************************************************************/
static void marpaESLIFPerl_valueFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Valuep);

  /* This only things we can de-allocate is what we created: STRING, ARRAY, PTR (== SV *) */

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_STRING:
    Safefree(marpaESLIFValueResultp->u.s.p);
    /* encoding is always refering to the constant UTF8s */
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    Safefree(marpaESLIFValueResultp->u.a.p);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    MARPAESLIFPERL_REFCNT_DEC(marpaESLIFValueResultp->u.p.p);
    break;
  default:
    break;
  }
}

/*****************************************************************************/
static void marpaESLIFPerl_ContextFreev(pTHX_ MarpaX_ESLIF_Engine_t *Perl_MarpaX_ESLIF_Enginep)
/*****************************************************************************/
{
  if (Perl_MarpaX_ESLIF_Enginep != NULL) {
    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Enginep->Perl_loggerInterfacep);
    if (Perl_MarpaX_ESLIF_Enginep->marpaESLIFp != NULL) {
      marpaESLIF_freev(Perl_MarpaX_ESLIF_Enginep->marpaESLIFp);
    }
    genericLogger_freev(&(Perl_MarpaX_ESLIF_Enginep->genericLoggerp)); /* This is NULL aware */
    Safefree(Perl_MarpaX_ESLIF_Enginep);
  }
}

/*****************************************************************************/
static void marpaESLIFPerl_grammarContextFreev(pTHX_ MarpaX_ESLIF_Grammar_t *Perl_MarpaX_ESLIF_Grammarp)
/*****************************************************************************/
{
  if (Perl_MarpaX_ESLIF_Grammarp != NULL) {
    SV *Perl_MarpaX_ESLIF_Enginep = Perl_MarpaX_ESLIF_Grammarp->Perl_MarpaX_ESLIF_Enginep;

    if (Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp != NULL) {
      marpaESLIFGrammar_freev(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp);
    }
    Safefree(Perl_MarpaX_ESLIF_Grammarp);

    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Enginep);
  }
}
 
/*****************************************************************************/
static void marpaESLIFPerl_valueContextFreev(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep, short onStackb)
/*****************************************************************************/
{
  int             i;
  SV             *svp;
  SV             *Perl_MarpaX_ESLIF_Recognizerp;
  SV             *Perl_MarpaX_ESLIF_Grammarp;
  SV             *Perl_valueInterfacep;
  genericStack_t *valueStackp;

  if (Perl_MarpaX_ESLIF_Valuep != NULL) {
    Perl_MarpaX_ESLIF_Recognizerp = Perl_MarpaX_ESLIF_Valuep->Perl_MarpaX_ESLIF_Recognizerp;
    Perl_MarpaX_ESLIF_Grammarp    = Perl_MarpaX_ESLIF_Valuep->Perl_MarpaX_ESLIF_Grammarp;
    Perl_valueInterfacep          = Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep;
    valueStackp                   = &(Perl_MarpaX_ESLIF_Valuep->valueStack);

    marpaESLIFPerl_valueContextCleanupv(aTHX_ Perl_MarpaX_ESLIF_Valuep);

    /* It is important to delete references in the reverse order of their creation */
    while (marpaESLIFPerl_GENERICSTACK_USED(valueStackp) > 0) {
      /* Last indice ... */
      i = marpaESLIFPerl_GENERICSTACK_USED(valueStackp) - 1;
      /* ... is cleared ... */
      if (marpaESLIFPerl_GENERICSTACK_IS_PTR(valueStackp, i)) {
        svp = (SV *) marpaESLIFPerl_GENERICSTACK_GET_PTR(valueStackp, i);
        MARPAESLIFPERL_FREE_SVP(svp);
      }
      /* ... and becomes current used size */
      marpaESLIFPerl_GENERICSTACK_SET_USED(valueStackp, i);
    }
    marpaESLIFPerl_GENERICSTACK_RESET(valueStackp);

    if (Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep);
    }

    /* Decrement dependencies */
    MARPAESLIFPERL_REFCNT_DEC(Perl_valueInterfacep);
    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Recognizerp); /* Note that Perl_MarpaX_ESLIF_Recognizerp is NULL in case of parse() */
    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Grammarp);

    if (! onStackb) {
      Safefree(Perl_MarpaX_ESLIF_Valuep);
    }
  }
}
 
/*****************************************************************************/
static void marpaESLIFPerl_valueContextCleanupv(pTHX_ MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep)
/*****************************************************************************/
{
  if (Perl_MarpaX_ESLIF_Valuep != NULL) {
    if (Perl_MarpaX_ESLIF_Valuep->previous_strings != NULL) {
      Safefree(Perl_MarpaX_ESLIF_Valuep->previous_strings);
      Perl_MarpaX_ESLIF_Valuep->previous_strings = NULL;
    }
  }
}

/*****************************************************************************/
static void marpaESLIFPerl_recognizerContextFreev(pTHX_ MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp, short onStackb)
/*****************************************************************************/
{
  int             i;
  SV             *svp;
  SV             *Perl_MarpaX_ESLIF_Grammarp;
  SV             *Perl_recognizerInterfacep;
  SV             *Perl_MarpaX_ESLIF_Recognizer_origp;
  genericStack_t *lexemeStackp;

  if (Perl_MarpaX_ESLIF_Recognizerp != NULL) {
    Perl_MarpaX_ESLIF_Grammarp         = Perl_MarpaX_ESLIF_Recognizerp->Perl_MarpaX_ESLIF_Grammarp;
    Perl_recognizerInterfacep          = Perl_MarpaX_ESLIF_Recognizerp->Perl_recognizerInterfacep;
    Perl_MarpaX_ESLIF_Recognizer_origp = Perl_MarpaX_ESLIF_Recognizerp->Perl_MarpaX_ESLIF_Recognizer_origp;
    lexemeStackp                       = &(Perl_MarpaX_ESLIF_Recognizerp->lexemeStack);

    marpaESLIFPerl_recognizerContextCleanupv(aTHX_ Perl_MarpaX_ESLIF_Recognizerp);

    /* It is important to delete references in the reverse order of their creation */
    while (marpaESLIFPerl_GENERICSTACK_USED(lexemeStackp) > 0) {
      /* Last indice ... */
      i = marpaESLIFPerl_GENERICSTACK_USED(lexemeStackp) - 1;
      /* ... is cleared ... */
      if (marpaESLIFPerl_GENERICSTACK_IS_PTR(lexemeStackp, i)) {
        svp = (SV *) marpaESLIFPerl_GENERICSTACK_GET_PTR(lexemeStackp, i);
        MARPAESLIFPERL_REFCNT_DEC(svp);
      }
      /* ... and becomes current used size */
      marpaESLIFPerl_GENERICSTACK_SET_USED(lexemeStackp, i);
    }
    marpaESLIFPerl_GENERICSTACK_RESET(lexemeStackp);

    if (Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizer_freev(Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp);
    }
    MARPAESLIFPERL_REFCNT_DEC(Perl_recognizerInterfacep);
    /* Note that Perl_MarpaX_ESLIF_Grammarp is NULL in the context of parse() */
    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Grammarp);
    /* Note that Perl_MarpaX_ESLIF_Recognizer_origp is NULL if not in the context of a shared recognizer */
    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Recognizer_origp);
    if (! onStackb) {
      Safefree(Perl_MarpaX_ESLIF_Recognizerp);
    }
  }
}

/*****************************************************************************/
static void marpaESLIFPerl_recognizerContextCleanupv(pTHX_ MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp)
/*****************************************************************************/
{
  if (Perl_MarpaX_ESLIF_Recognizerp != NULL) {

    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_datap);
    Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_datap = NULL;

    MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_encodingp);
    Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_encodingp = NULL;

  }
}

/*****************************************************************************/
static void marpaESLIFPerl_grammarContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Enginep, MarpaX_ESLIF_Grammar_t *Perl_MarpaX_ESLIF_Grammarp)
/*****************************************************************************/
{
  /* Perl_MarpaX_ESLIF_Enginep is an SvIV */
  MARPAESLIFPERL_REFCNT_INC(Perl_MarpaX_ESLIF_Enginep);
  Perl_MarpaX_ESLIF_Grammarp->Perl_MarpaX_ESLIF_Enginep = Perl_MarpaX_ESLIF_Enginep;
  Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp = NULL;
}

/*****************************************************************************/
static void marpaESLIFPerl_recognizerContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Grammarp, SV *Perl_recognizerInterfacep, MarpaX_ESLIF_Recognizer_t *Perl_MarpaX_ESLIF_Recognizerp, SV *Perl_MarpaX_ESLIF_Recognizer_origp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_recognizerContextInitv";

  marpaESLIFPerl_GENERICSTACK_INIT(&(Perl_MarpaX_ESLIF_Recognizerp->lexemeStack));
  if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Recognizerp->lexemeStack))) {
    int save_errno = errno;
    MARPAESLIFPERL_CROAKF("GENERICSTACK_INIT() failure, %s", strerror(save_errno));
  }

  /* All SVs are SvRV's - when coming from newFrom(): Perl_recognizerInterfacep is NULL and Perl_MarpaX_ESLIF_Recognizer_origp is not NULL */
  Perl_MarpaX_ESLIF_Recognizerp->Perl_MarpaX_ESLIF_Grammarp         = newRV(SvRV(Perl_MarpaX_ESLIF_Grammarp));
  Perl_MarpaX_ESLIF_Recognizerp->Perl_recognizerInterfacep          = (Perl_recognizerInterfacep != NULL) ? newRV(SvRV(Perl_recognizerInterfacep)) : 0;
  Perl_MarpaX_ESLIF_Recognizerp->Perl_MarpaX_ESLIF_Recognizer_origp = (Perl_MarpaX_ESLIF_Recognizer_origp != NULL) ? newRV(SvRV(Perl_MarpaX_ESLIF_Recognizer_origp)) : NULL;
  Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_datap                = NULL;
  Perl_MarpaX_ESLIF_Recognizerp->previous_Perl_encodingp            = NULL;
  Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp              = NULL;
#ifdef PERL_IMPLICIT_CONTEXT
  Perl_MarpaX_ESLIF_Recognizerp->PerlInterpreterp                   = aTHX;
#endif
}

/*****************************************************************************/
static void marpaESLIFPerl_valueContextInitv(pTHX_ SV *Perl_MarpaX_ESLIF_Recognizerp, SV *Perl_MarpaX_ESLIF_Grammarp, SV *Perl_valueInterfacep, MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_valueContextInitv";

  marpaESLIFPerl_GENERICSTACK_INIT(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
  if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
    int save_errno = errno;
    MARPAESLIFPERL_CROAKF("GENERICSTACK_INIT() failure, %s", strerror(save_errno));
  }

  /* All SVs are SvRV's */

  /* Perl_MarpaX_ESLIF_Recognizerp is NULL in the context of parseb() */
  Perl_MarpaX_ESLIF_Valuep->Perl_MarpaX_ESLIF_Recognizerp = (Perl_MarpaX_ESLIF_Recognizerp != NULL) ? newRV(SvRV(Perl_MarpaX_ESLIF_Recognizerp)) : NULL;
  Perl_MarpaX_ESLIF_Valuep->Perl_MarpaX_ESLIF_Grammarp    = newRV(SvRV(Perl_MarpaX_ESLIF_Grammarp));
  Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep          = newRV(SvRV(Perl_valueInterfacep));
  Perl_MarpaX_ESLIF_Valuep->actions                       = NULL;
  Perl_MarpaX_ESLIF_Valuep->previous_strings              = NULL;
  Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep              = NULL;
  Perl_MarpaX_ESLIF_Valuep->canSetSymbolNameb             = marpaESLIFPerl_canb(aTHX_ Perl_valueInterfacep, "setSymbolName");
  Perl_MarpaX_ESLIF_Valuep->canSetSymbolNumberb           = marpaESLIFPerl_canb(aTHX_ Perl_valueInterfacep, "setSymbolNumber");
  Perl_MarpaX_ESLIF_Valuep->canSetRuleNameb               = marpaESLIFPerl_canb(aTHX_ Perl_valueInterfacep, "setRuleName");
  Perl_MarpaX_ESLIF_Valuep->canSetRuleNumberb             = marpaESLIFPerl_canb(aTHX_ Perl_valueInterfacep, "setRuleNumber");
  Perl_MarpaX_ESLIF_Valuep->canSetGrammarb                = marpaESLIFPerl_canb(aTHX_ Perl_valueInterfacep, "setGrammar");
  Perl_MarpaX_ESLIF_Valuep->symbols                       = NULL;
  Perl_MarpaX_ESLIF_Valuep->symboli                       = -1;
  Perl_MarpaX_ESLIF_Valuep->rules                         = NULL;
  Perl_MarpaX_ESLIF_Valuep->rulei                         = -1;
#ifdef PERL_IMPLICIT_CONTEXT
  Perl_MarpaX_ESLIF_Valuep->PerlInterpreterp              = aTHX;
#endif
}

/*****************************************************************************/
static void marpaESLIFPerl_paramIsGrammarv(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_paramIsGrammarv";
  int                type  = marpaESLIFPerl_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIFPERL_CROAK("Grammar must be a scalar");
  }
}

/*****************************************************************************/
static void marpaESLIFPerl_paramIsEncodingv(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_paramIsEncodingv";
  int                type  = marpaESLIFPerl_getTypei(aTHX_ sv);

  if ((type & SCALAR) != SCALAR) {
    MARPAESLIFPERL_CROAK("Encoding must be a scalar");
  }
}

/*****************************************************************************/
static short marpaESLIFPerl_paramIsLoggerInterfaceOrUndefb(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_paramIsLoggerInterfaceOrUndefb";
  int                type  = marpaESLIFPerl_getTypei(aTHX_ sv);

  if ((type & UNDEF) == UNDEF) {
    return 0;
  }

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIFPERL_CROAK("Logger interface must be an object");
  }

  if (! marpaESLIFPerl_canb(aTHX_ sv, "trace"))     MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"trace\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "debug"))     MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"debug\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "info"))      MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"info\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "notice"))    MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"notice\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "warning"))   MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"warning\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "error"))     MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"error\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "critical"))  MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"critical\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "alert"))     MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"alert\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "emergency")) MARPAESLIFPERL_CROAK("Logger interface must be an object that can do \"emergency\"");

  return 1;
}

/*****************************************************************************/
static void marpaESLIFPerl_paramIsRecognizerInterfacev(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_paramIsRecognizerInterfacev";
  int                type  = marpaESLIFPerl_getTypei(aTHX_ sv);

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIFPERL_CROAK("Recognizer interface must be an object");
  }

  if (! marpaESLIFPerl_canb(aTHX_ sv, "read"))                   MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"read\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isEof"))                  MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isEof\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isCharacterStream"))      MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isCharacterStream\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "encoding"))               MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"encoding\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "data"))                   MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"data\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithDisableThreshold")) MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isWithDisableThreshold\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithExhaustion"))       MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isWithExhaustion\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithNewline"))          MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isWithNewline\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithTrack"))            MARPAESLIFPERL_CROAK("Recognizer interface must be an object that can do \"isWithTrack\"");
}

/*****************************************************************************/
static void marpaESLIFPerl_paramIsValueInterfacev(pTHX_ SV *sv)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_paramIsValueInterfacev";
  int                type  = marpaESLIFPerl_getTypei(aTHX_ sv);

  if ((type & OBJECT) != OBJECT) {
    MARPAESLIFPERL_CROAK("Value interface must be an object");
  }

  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithHighRankOnly")) MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"isWithHighRankOnly\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithOrderByRank"))  MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"isWithOrderByRank\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithAmbiguous"))    MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"isWithAmbiguous\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "isWithNull"))         MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"isWithNull\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "maxParses"))          MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"maxParses\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "setResult"))          MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"setResult\"");
  if (! marpaESLIFPerl_canb(aTHX_ sv, "getResult"))          MARPAESLIFPERL_CROAK("Value interface must be an object that can do \"getResult\"");
}

/*****************************************************************************/
static short marpaESLIFPerl_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp)
/*****************************************************************************/
{
  static const char    *funcs                    = "marpaESLIFPerl_representationb";
  MarpaX_ESLIF_Value_t *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;
  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Valuep);

  marpaESLIFPerl_valueContextCleanupv(aTHX_ Perl_MarpaX_ESLIF_Valuep);

  /* We always push a PTR */
  if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
    MARPAESLIFPERL_CROAKF("User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
  }
  /* Our context is always MARPAESLIFPERL_CONTEXT */
  if (marpaESLIFValueResultp->contextp != MARPAESLIFPERL_CONTEXT) {
    MARPAESLIFPERL_CROAKF("User-defined value context is not MARPAESLIFPERL_CONTEXT but %p", marpaESLIFValueResultp->contextp);
  }
  Perl_MarpaX_ESLIF_Valuep->previous_strings = marpaESLIFPerl_sv2byte(aTHX_ (SV *) marpaESLIFValueResultp->u.p.p, inputcpp, inputlp, 1 /* encodingInformationb */, NULL /* characterStreambp */, encodingasciisp, NULL /* encodinglp */, 0 /* warnIsFatalb */);

  /* Always return a true value, else ::concat will abort */
  return 1;
}

/*****************************************************************************/
static char *marpaESLIFPerl_sv2byte(pTHX_ SV *svp, char **bytepp, size_t *bytelp, short encodingInformationb, short *characterStreambp, char **encodingsp, size_t *encodinglp, short warnIsFatalb)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_sv2byte";
  char              *rcp = NULL;
  short              okb   = 0;
  char              *strings;
  STRLEN             len;
  char              *bytep;
  size_t             bytel;
  short              characterStreamb;
  char              *encodings;
  size_t             encodingl;

  /* svp == NULL should never happen because we always push an SV* out of actions
     but &PL_sv_undef is of course possible */
  if ((svp == NULL) || (svp == &PL_sv_undef)) {
    return NULL;
  }

  strings = SvPV(svp, len);

  if ((strings != NULL) && (len > 0)) {
    okb = 1;
    /* We voluntarily ignore the eventual :bytes pragma: encoding information is for export to marpaESLIF */
    /* We assume there is no character outside of UTF-8 (utf8 != UTF-8 -;) */
    if (encodingInformationb && (SvUTF8(svp) || is_utf8_string((const U8 *) strings, len))) {
      characterStreamb    = 1;
      encodings           = (char *) UTF8s;
      encodingl           = UTF8l;
    } else {
      characterStreamb    = 0;
      encodings           = NULL;
      encodingl           = 0;
    }
  } else {
    if (warnIsFatalb) {
      MARPAESLIFPERL_CROAKF("SvPV() returned {pointer,length}={%p,%ld}", strings, (unsigned long) len);
    }
  }

  if (okb) { /* Else nothing will be appended */
    Newx(rcp, (int) len, char);
    bytep = CopyD(strings, rcp, (int) len, char);
    bytel = (size_t) len;
  }

  if (okb) {
    if (bytepp != NULL) {
      *bytepp = bytep;
    }
    if (bytelp != NULL) {
      *bytelp = bytel;
    }
    if (characterStreambp != NULL) {
      *characterStreambp = characterStreamb;
    }
    if (encodingsp != NULL) {
      *encodingsp = encodings;
    }
    if (encodinglp != NULL) {
      *encodinglp = encodingl;
    }
  }

  return rcp;
}

/*****************************************************************************/
static short marpaESLIFPerl_importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char                *funcs                    = "marpaESLIFPerl_importb";
  MarpaX_ESLIF_Value_t             *Perl_MarpaX_ESLIF_Valuep = (MarpaX_ESLIF_Value_t *) userDatavp;
  AV                               *avp;
  HV                               *hvp;
  SV                               *keyp;
  SV                               *valuep;
  SV                               *svp;
  SV                               *stringp;
  SV                               *encodingp;
  AV                               *listp;
  SV                               *checkp;
  size_t                            i;
  short                             utf8b;
  marpaESLIFPerl_stringGenerator_t  marpaESLIFPerl_stringGenerator;
  genericLogger_t                  *genericLoggerp;

  marpaESLIFPerlMYTHX(Perl_MarpaX_ESLIF_Valuep);

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    svp = &PL_sv_undef;
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    svp = newSVpvn((const char *) &(marpaESLIFValueResultp->u.c), (STRLEN) 1);
    if (is_utf8_string((const U8 *) &(marpaESLIFValueResultp->u.c), (STRLEN) 1)) {
      SvUTF8_on(svp);
    }
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    svp = newSViv((IV) marpaESLIFValueResultp->u.b);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    svp = newSViv((IV) marpaESLIFValueResultp->u.i);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    svp = newSViv((IV) marpaESLIFValueResultp->u.l);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    /* NVTYPE is at least double */
    svp = newSVnv((NVTYPE) marpaESLIFValueResultp->u.f);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    /* NVTYPE is at least double */
    svp = newSVnv((NVTYPE) marpaESLIFValueResultp->u.d);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    if (marpaESLIFValueResultp->contextp == MARPAESLIFPERL_CONTEXT) {
      /* This is an SV that we pushed */
      svp = (SV *) marpaESLIFValueResultp->u.p.p;
      /* Increase ref count */
      MARPAESLIFPERL_REFCNT_INC(svp);
    } else {
      /* This is a pointer coming from another source */
      svp = newSViv(PTR2IV(marpaESLIFValueResultp->u.p.p));
    }
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    svp = newSVpvn((const char *) marpaESLIFValueResultp->u.a.p, (STRLEN) marpaESLIFValueResultp->u.a.sizel);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    svp = (marpaESLIFValueResultp->u.y == MARPAESLIFVALUERESULTBOOL_FALSE) ? &PL_sv_no : &PL_sv_yes;
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    stringp = newSVpvn((const char *) marpaESLIFValueResultp->u.s.p, (STRLEN) marpaESLIFValueResultp->u.s.sizel);
    utf8b = 0;
    if (MARPAESLIFPERL_ENCODING_IS_UTF8(marpaESLIFValueResultp->u.s.encodingasciis, strlen(marpaESLIFValueResultp->u.s.encodingasciis))) {
#ifdef MARPAESLIFPERL_UTF8_CROSSCHECK
      /* Cross-check it is a string UTF-8 string */
#ifdef is_strict_utf8_string
      /* Since perl-5.26 */
      if (is_strict_utf8_string((const U8 *) marpaESLIFValueResultp->u.s.p, (STRLEN) marpaESLIFValueResultp->u.s.sizel)) {
        svp = stringp;
        SvUTF8_on(svp);
        utf8b = 1;
      } else {
	warn("is_strict_utf8_string() failure on a string claimed to be in UTF encoding\n");
      }
#else
      /* Use Encode::decode("UTF-8", octets, CHECK) - Note that Encode module is always loaded, c.f. MarpaX::ESLIF::String */
      listp = newAV();
      av_push(listp, newSVsv(boot_MarpaX__ESLIF__UTF_8_svp));
      av_push(listp, newSVsv(stringp));
      av_push(listp, newSViv(MARPAESLIFPERL_ENCODE_FB_CROAK));
      /* Call Encode::decode static method */
      checkp = marpaESLIFPerl_call_actionp(aTHX_ NULL /* interfacep */, "Encode::decode", listp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 1 /* evalb */, 0 /* evalSilentb */);
      /* The object also has an utf8 flag */
      av_undef(listp);
      /* If we are here this did not croak -; */
      if (SvOK(checkp) && (! SvROK(checkp))) {
	/* It returned a defined scalar, so this is ok */
        svp = stringp;
        SvUTF8_on(svp);
        utf8b = 1;
      }
      MARPAESLIFPERL_REFCNT_DEC(checkp);
#endif /* is_strict_utf8_string */
#else
      /* We trust the encoding */
      svp = stringp;
      SvUTF8_on(svp);
      utf8b = 1;
#endif
    }
    if (! utf8b) {
      /* This will be a MarpaX::ESLIF::String */
      encodingp = newSVpv(marpaESLIFValueResultp->u.s.encodingasciis, 0);
      listp = newAV();
      av_push(listp, stringp);
      av_push(listp, encodingp);
      /* Gets the object and create a reference to it */
      svp = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__String_svp, "new", listp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
      /* The object also has an utf8 flag */
      av_undef(listp);
    }
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    /* We pushed in the internal valueStack r.sizel elements */
    avp = newAV();
    for (i = marpaESLIFValueResultp->u.r.sizel; i > 0; i--) {
      svp = (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
      av_push(avp, svp);
    }
    svp = newRV((SV *)avp);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
  /* We pushed in the internal valueStack t.sizel*2 elements */
    hvp = newHV();
    for (i = marpaESLIFValueResultp->u.t.sizel; i > 0; i -= 2) {
      /* Note that Perl_MarpaX_ESLIF_Valuep->valueStack contains only new SV's, or &PL_sv_undef, &PL_sv_yes, &PL_sv_no */
      /* This is why it is not necessary to SvREFCNT_inc/SvREFCNT_dec on valuep: all we do is create an SV and move it in the hash */
      valuep = (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
      keyp = (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
      if (hv_store_ent(hvp, keyp, valuep, 0) == NULL) {
        /* We never play with tied hashes, so hv_store_ent() must return a non-NULL value */
        MARPAESLIFPERL_CROAK("hv_store_ent failure");
      }
    }
    svp = newRV((SV *)hvp);
    marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
    if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
      MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_LONG_DOUBLE:
    if ((! boot_nvtype_is_long_doubleb) && (! boot_nvtype_is___float128)) {
      /* Switch to Math::BigFloat - we must first generate a string representation of this long double. */
#ifdef PERL_IMPLICIT_CONTEXT
      marpaESLIFPerl_stringGenerator.PerlInterpreterp = Perl_MarpaX_ESLIF_Valuep->PerlInterpreterp;
#endif
      marpaESLIFPerl_stringGenerator.s      = NULL;
      marpaESLIFPerl_stringGenerator.l      = 0;
      marpaESLIFPerl_stringGenerator.okb    = 0;
      marpaESLIFPerl_stringGenerator.allocl = 0;
      genericLoggerp = GENERICLOGGER_CUSTOM(marpaESLIFPerl_generateStringWithLoggerCallback, (void *) &marpaESLIFPerl_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        MARPAESLIFPERL_CROAKF("GENERICLOGGER_CUSTOM failure, %s", strerror(errno));
      }
      GENERICLOGGER_TRACEF(genericLoggerp, long_double_fmts, marpaESLIFValueResultp->u.ld); /* This will croak by itself if needed */
      if ((marpaESLIFPerl_stringGenerator.s == NULL) || (marpaESLIFPerl_stringGenerator.l <= 1)) {
        /* This should never happen */
        GENERICLOGGER_FREE(genericLoggerp);
        MARPAESLIFPERL_CROAKF("Internal error when doing string representation of long double %Lf", marpaESLIFValueResultp->u.ld);
      }
      stringp = newSVpvn((const char *) marpaESLIFPerl_stringGenerator.s, (STRLEN) (marpaESLIFPerl_stringGenerator.l - 1));
      free(marpaESLIFPerl_stringGenerator.s);
      marpaESLIFPerl_stringGenerator.s = NULL;
      GENERICLOGGER_FREE(genericLoggerp);

      /* Representation is in stringp. Call Math::BigFloat->new(stringp). */
      listp = newAV();
      av_push(listp, stringp); /* Ref count of stringp is transfered to av -; */
      svp = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Math__BigFloat_svp, "new", listp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
      av_undef(listp);

    } else {
      /* NVTYPE here is long double or __float128 */
      svp = newSVnv((NVTYPE) marpaESLIFValueResultp->u.ld);
      marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack), svp);
      if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Valuep->valueStack))) {
        MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Valuep->valueStack push failure, %s", strerror(errno));
      }
    }
    break;
  default:
    break;
  }

  return 1;
}

/*****************************************************************************/
static void marpaESLIFPerl_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIFPerl_appendOpaqueDataToStringGenerator((marpaESLIFPerl_stringGenerator_t *) userDatavp, (char *) msgs, strlen(msgs));
}

/*****************************************************************************/
static short marpaESLIFPerl_appendOpaqueDataToStringGenerator(marpaESLIFPerl_stringGenerator_t *marpaESLIFPerl_stringGeneratorp, char *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFPerl_appendOpaqueDataToStringGenerator";
  char              *tmpp;
  short              rcb;
  size_t             allocl;
  size_t             wantedl;
  marpaESLIFPerlMYTHX(marpaESLIFPerl_stringGeneratorp);

  /* Note: caller must guarantee that marpaESLIFPerl_stringGeneratorp->p != NULL and l > 0 */

  if (marpaESLIFPerl_stringGeneratorp->s == NULL) {
    /* Get an allocl that is a multiple of 1024, taking into account the hiden NUL byte */
    /* 1023 -> 1024 */
    /* 1024 -> 2048 */
    /* 2047 -> 2048 */
    /* 2048 -> 3072 */
    /* ... */
    /* i.e. this is the upper multiple of 1024 and have space for the NUL byte */
    allocl = MARPAESLIFPERL_CHUNKED_SIZE_UPPER(sizel, 1024);
    /* Check for turn-around, should never happen */
    if (allocl < sizel) {
      MARPAESLIFPERL_CROAK("size_t turnaround detected");
      goto err;
    }
    marpaESLIFPerl_stringGeneratorp->s  = malloc(allocl);
    if (marpaESLIFPerl_stringGeneratorp->s == NULL) {
      MARPAESLIFPERL_CROAKF("malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(marpaESLIFPerl_stringGeneratorp->s, p, sizel);
    marpaESLIFPerl_stringGeneratorp->l      = sizel + 1;  /* NUL byte is set at exit of the routine */
    marpaESLIFPerl_stringGeneratorp->allocl = allocl;
    marpaESLIFPerl_stringGeneratorp->okb    = 1;
  } else if (marpaESLIFPerl_stringGeneratorp->okb) {
    wantedl = marpaESLIFPerl_stringGeneratorp->l + sizel; /* +1 for the NUL is already accounted in marpaESLIFPerl_stringGeneratorp->l */
    allocl = MARPAESLIFPERL_CHUNKED_SIZE_UPPER(wantedl, 1024);
    /* Check for turn-around, should never happen */
    if (allocl < wantedl) {
      MARPAESLIFPERL_CROAK("size_t turnaround detected");
      goto err;
    }
    if (allocl > marpaESLIFPerl_stringGeneratorp->allocl) {
      tmpp = realloc(marpaESLIFPerl_stringGeneratorp->s, allocl); /* The +1 for the NULL byte is already in */
      if (tmpp == NULL) {
        MARPAESLIFPERL_CROAKF("realloc failure, %s", strerror(errno));
        goto err;
      }
      marpaESLIFPerl_stringGeneratorp->s      = tmpp;
      marpaESLIFPerl_stringGeneratorp->allocl = allocl;
    }
    memcpy(marpaESLIFPerl_stringGeneratorp->s + marpaESLIFPerl_stringGeneratorp->l - 1, p, sizel);
    marpaESLIFPerl_stringGeneratorp->l = wantedl; /* Already contains the +1 fir the NUL byte */
  } else {
    MARPAESLIFPERL_CROAKF("Invalid internal call to %s", funcs);
    goto err;
  }

  marpaESLIFPerl_stringGeneratorp->s[marpaESLIFPerl_stringGeneratorp->l - 1] = '\0';
  rcb = 1;
  goto done;

 err:
  if (marpaESLIFPerl_stringGeneratorp->s != NULL) {
    free(marpaESLIFPerl_stringGeneratorp->s);
    marpaESLIFPerl_stringGeneratorp->s = NULL;
  }
  marpaESLIFPerl_stringGeneratorp->okb    = 0;
  marpaESLIFPerl_stringGeneratorp->l      = 0;
  marpaESLIFPerl_stringGeneratorp->allocl = 0;
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFPerl_is_undef(pTHX_ SV *svp)
/*****************************************************************************/
{
  return (marpaESLIFPerl_getTypei(aTHX_ svp) == UNDEF);
}

/*****************************************************************************/
static short marpaESLIFPerl_is_arrayref(pTHX_ SV *svp)
/*****************************************************************************/
{
  return (marpaESLIFPerl_getTypei(aTHX_ svp) == ARRAYREF);
}

/*****************************************************************************/
static short marpaESLIFPerl_is_hashref(pTHX_ SV *svp)
/*****************************************************************************/
{
  return (marpaESLIFPerl_getTypei(aTHX_ svp) == HASHREF);
}

/*****************************************************************************/
static short marpaESLIFPerl_is_Types__Standard(pTHX_ SV *svp, const char *types)
/*****************************************************************************/
{
  AV    *avp;
  short rcb;

  avp = newAV();
  av_push(avp, newSVsv(svp)); /* Ref count of stringp is transfered to av -; */
  svp = marpaESLIFPerl_call_actionp(aTHX_ NULL /* interfacep */, types, avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);

  rcb = SvTRUE(svp);
  MARPAESLIFPERL_REFCNT_DEC(svp);

  return rcb;
}

/*****************************************************************************/
static void marpaESLIFPerl_stack_setv(pTHX_ marpaESLIFValue_t *marpaESLIFValuep, short resulti, SV *svp)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFPerl_stack_setv";
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  short                    opaqueb = 1;
  char                    *bytep;
  size_t                   bytel;
  char                    *encodings;
  IV                       iv;
  NV                       nv;

  if (marpaESLIFPerl_is_undef(aTHX_ svp)) {
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
    marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
    marpaESLIFValueResult.representationp = NULL;
    opaqueb = 0;
  } else if (marpaESLIFPerl_is_Types__Standard(aTHX_ svp, "Types::Standard::is_Int")) {
    iv = SvIV(svp);
    if (sv_cmp(svp, sv_2mortal(newSViv(iv))) == 0) {
      /* Ok if we loosed nothing */
      if ((iv >= SHRT_MIN) && (iv <= SHRT_MAX)) {
        /* Ok if it fits into [SHRT_MIN,SHRT_MAX] */
        marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_SHORT;
        marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp = NULL;
        marpaESLIFValueResult.u.b             = (short) iv;
        opaqueb = 0;
      } else if ((iv >= INT_MIN) && (iv <= INT_MAX)) {
        /* Ok if it fits into [INT_MIN,INT_MAX] */
        marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT;
        marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp = NULL;
        marpaESLIFValueResult.u.i             = (int) iv;
        opaqueb = 0;
      } else if ((iv >= LONG_MIN) && (iv <= LONG_MAX)) {
        /* Ok if it fits into [LONG_MIN,LONG_MAX] */
        marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG;
        marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp = NULL;
        marpaESLIFValueResult.u.l             = (long) iv;
        opaqueb = 0;
      }
    }
  } else if (marpaESLIFPerl_is_Types__Standard(aTHX_ svp, "Types::Standard::is_StrictNum")) {
    /* Ok if it fits into [DBL_MIN,DBL_MAX] and we loosed nothing */
    nv = SvNV(svp);
    if (sv_cmp(svp, sv_2mortal(newSVnv(nv))) == 0) {
      if (boot_nvtype_is_long_doubleb) {
        /* NVTYPE is long double -; */
        marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG_DOUBLE;
        marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp = NULL;
        marpaESLIFValueResult.u.ld            = (long double) nv;
        opaqueb = 0;
      } else if (! boot_nvtype_is___float128) {
        /* NVTYPE is double -; */
        marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_DOUBLE;
        marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp = NULL;
        marpaESLIFValueResult.u.d             = (double) nv;
        opaqueb = 0;
      }
    }
  } else if (marpaESLIFPerl_is_Types__Standard(aTHX_ svp, "Types::Standard::is_Str")) {
    /* Ok it this is a scalar in general - perl will recoerce it back */
                                                                        \
    if (marpaESLIFPerl_sv2byte(aTHX_ svp, &bytep, &bytel, 1 /* encodingInformationb */, NULL /* characterStreambp */, &encodings, NULL /* encodinglp */, 0 /* warnIsFatalb */) != NULL) {
      if (encodings != NULL) {
        marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_STRING;
        marpaESLIFValueResult.contextp           = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp    = NULL;
        marpaESLIFValueResult.u.s.p              = bytep;
        marpaESLIFValueResult.u.s.sizel          = bytel;
        marpaESLIFValueResult.u.s.encodingasciis = encodings;
        marpaESLIFValueResult.u.s.shallowb       = 0;
        opaqueb = 0;
      } else if (bytel > 0) {
        marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
        marpaESLIFValueResult.contextp           = MARPAESLIFPERL_CONTEXT;
        marpaESLIFValueResult.representationp    = NULL;
        marpaESLIFValueResult.u.a.p              = bytep;
        marpaESLIFValueResult.u.a.sizel          = bytel;
        marpaESLIFValueResult.u.a.shallowb       = 0;
        opaqueb = 0;
      }
    }
  }

  if (opaqueb) {
    marpaESLIFValueResult.u.p.shallowb    = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.contextp        = MARPAESLIFPERL_CONTEXT;
    marpaESLIFValueResult.representationp = marpaESLIFPerl_representationb;
    marpaESLIFValueResult.u.p.p           = svp;
    marpaESLIFValueResult.u.p.shallowb    = 0;
  } else {
    /* We do not need this svp anymore */
    MARPAESLIFPERL_REFCNT_DEC(svp);
  }

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }
}

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Engine                                                   */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Engine

BOOT:
  boot_MarpaX__ESLIF__String_svp  = newSVpvn("MarpaX::ESLIF::String", strlen("MarpaX::ESLIF::String"));
  boot_MarpaX__ESLIF_svp  = newSVpvn("MarpaX::ESLIF", strlen("MarpaX::ESLIF"));
  boot_MarpaX__ESLIF__UTF_8_svp  = newSVpvn("UTF-8", strlen("UTF-8"));
  sprintf(long_double_fmts, "%%%d.%dLe", LDBL_DIG + 8, LDBL_DIG);
  boot_MarpaX__ESLIF__Math__BigFloat_svp = newSVpvn("Math::BigFloat", strlen("Math::BigFloat"));
  boot_nvtype_is_long_doubleb = marpaESLIFPerl_call_methodb(aTHX_ boot_MarpaX__ESLIF_svp, "_nvtype_is_long_double");
  boot_nvtype_is___float128 = marpaESLIFPerl_call_methodb(aTHX_ boot_MarpaX__ESLIF_svp, "_nvtype_is___float128");

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Engine::allocate                                         */
  /* ----------------------------------------------------------------------- */
=cut

void *
allocate(Perl_packagep, ...)
  SV *Perl_packagep;
PREINIT:
  static const char  *funcs                    = "MarpaX::ESLIF::Engine::allocate";
CODE:
  SV                 *Perl_loggerInterfacep    = &PL_sv_undef;
  short               loggerInterfaceIsObjectb = 0;
  MarpaX_ESLIF_Engine Perl_MarpaX_ESLIF_Enginep;
  marpaESLIFOption_t  marpaESLIFOption;

  if(items > 1) {
    loggerInterfaceIsObjectb = marpaESLIFPerl_paramIsLoggerInterfaceOrUndefb(aTHX_ Perl_loggerInterfacep = ST(1));
  }

  Newx(Perl_MarpaX_ESLIF_Enginep, 1, MarpaX_ESLIF_Engine_t);
  Perl_MarpaX_ESLIF_Enginep->Perl_loggerInterfacep = &PL_sv_undef;
  Perl_MarpaX_ESLIF_Enginep->genericLoggerp        = NULL;
  Perl_MarpaX_ESLIF_Enginep->marpaESLIFp           = NULL;
#ifdef PERL_IMPLICIT_CONTEXT
  Perl_MarpaX_ESLIF_Enginep->PerlInterpreterp      = aTHX;
#endif

  /* ------------- */
  /* genericLogger */
  /* ------------- */
  if (loggerInterfaceIsObjectb) {
    MARPAESLIFPERL_REFCNT_INC(Perl_loggerInterfacep);
    Perl_MarpaX_ESLIF_Enginep->Perl_loggerInterfacep = Perl_loggerInterfacep;
    Perl_MarpaX_ESLIF_Enginep->genericLoggerp        = genericLogger_newp(marpaESLIFPerl_genericLoggerCallbackv, Perl_MarpaX_ESLIF_Enginep, GENERICLOGGER_LOGLEVEL_TRACE);
    if (Perl_MarpaX_ESLIF_Enginep->genericLoggerp == NULL) {
      int save_errno = errno;
      marpaESLIFPerl_ContextFreev(aTHX_ Perl_MarpaX_ESLIF_Enginep);
      MARPAESLIFPERL_CROAKF("genericLogger_newp failure, %s", strerror(save_errno));
    }
  }

  /* ---------- */
  /* marpaESLIF */
  /* ---------- */
  marpaESLIFOption.genericLoggerp = Perl_MarpaX_ESLIF_Enginep->genericLoggerp;
  Perl_MarpaX_ESLIF_Enginep->marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (Perl_MarpaX_ESLIF_Enginep->marpaESLIFp == NULL) {
    int save_errno = errno;
    marpaESLIFPerl_ContextFreev(aTHX_ Perl_MarpaX_ESLIF_Enginep);
    MARPAESLIFPERL_CROAKF("marpaESLIF_newp failure, %s", strerror(save_errno));
  }

  RETVAL = Perl_MarpaX_ESLIF_Enginep;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Engine::dispose                                          */
  /* ----------------------------------------------------------------------- */
=cut

void
dispose(Perl_packagep, Perl_MarpaX_ESLIF_Enginep)
  SV *Perl_packagep;
  void *Perl_MarpaX_ESLIF_Enginep;
PREINIT:
  static const char  *funcs = "MarpaX::ESLIF::Engine::dispose";
CODE:
  marpaESLIFPerl_ContextFreev(aTHX_ (MarpaX_ESLIF_Engine) Perl_MarpaX_ESLIF_Enginep);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Engine::version                                          */
  /* ----------------------------------------------------------------------- */
=cut

char *
version(Perl_packagep, Perl_MarpaX_ESLIF_Enginep)
  SV *Perl_packagep;
  void *Perl_MarpaX_ESLIF_Enginep;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Engine::version";
CODE:
  char              *versions;

  if (! marpaESLIF_versionb(((MarpaX_ESLIF_Engine) Perl_MarpaX_ESLIF_Enginep)->marpaESLIFp, &versions)) {
    MARPAESLIFPERL_CROAKF("marpaESLIF_versionb failure, %s", strerror(errno));
  }
  RETVAL = versions;
OUTPUT:
  RETVAL

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Grammar                                                  */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Grammar

PROTOTYPES: ENABLE

BOOT:
  boot_MarpaX__ESLIF__Grammar__Properties_svp = newSVpvn("MarpaX::ESLIF::Grammar::Properties", strlen("MarpaX::ESLIF::Grammar::Properties"));
  boot_MarpaX__ESLIF__Grammar__Rule__Properties_svp    = newSVpvn("MarpaX::ESLIF::Grammar::Rule::Properties", strlen("MarpaX::ESLIF::Grammar::Rule::Properties"));
  boot_MarpaX__ESLIF__Grammar__Symbol__Properties_svp  = newSVpvn("MarpaX::ESLIF::Grammar::Symbol::Properties", strlen("MarpaX::ESLIF::Grammar::Symbol::Properties"));

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::_new                                            */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Grammar
_new(Perl_packagep, Perl_MarpaX_ESLIF_Enginep, Perl_grammarp, ...)
  SV   *Perl_packagep;
  void *Perl_MarpaX_ESLIF_Enginep;
  SV   *Perl_grammarp;
PREINIT:
  static const char           *funcs          = "MarpaX::ESLIF::Grammar::_new";
CODE:
  SV                          *Perl_encodingp = &PL_sv_undef;
  void                        *string1s       = NULL;
  void                        *string2s       = NULL;
  void                        *string3s       = NULL;
  MarpaX_ESLIF_Grammar_t      *Perl_MarpaX_ESLIF_Grammarp;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          ngrammari;
  int                          i;
  marpaESLIFGrammarDefaults_t  marpaESLIFGrammarDefaults;
  marpaESLIFAction_t           defaultFreeAction;

  marpaESLIFPerl_paramIsGrammarv(aTHX_ Perl_grammarp);
  if (items > 3) {
    marpaESLIFPerl_paramIsEncodingv(aTHX_ Perl_encodingp = ST(3));
    string1s = marpaESLIFPerl_sv2byte(aTHX_ Perl_encodingp,
                                  &(marpaESLIFGrammarOption.encodings),
                                  &(marpaESLIFGrammarOption.encodingl),
                                  1, /* encodingInformationb */
                                  NULL, /* characterStreambp */
                                  NULL, /* encodingsp */
                                  NULL, /* encodinglp */
                                  1 /* warnIsFatalb */);
    string2s = marpaESLIFPerl_sv2byte(aTHX_ Perl_grammarp,
                                  (char **) &(marpaESLIFGrammarOption.bytep),
                                  &(marpaESLIFGrammarOption.bytel),
                                  0, /* encodingInformationb */
                                  NULL, /* characterStreambp */
                                  NULL, /* encodingsp */
                                  NULL, /* encodinglp */
                                  1 /* warnIsFatalb */);
  } else {
    string3s = marpaESLIFPerl_sv2byte(aTHX_ Perl_grammarp,
                                  (char **) &(marpaESLIFGrammarOption.bytep),
                                  &(marpaESLIFGrammarOption.bytel),
                                  1, /* encodingInformationb */
                                  NULL, /* characterStreambp */
                                  &(marpaESLIFGrammarOption.encodings),
                                  &(marpaESLIFGrammarOption.encodingl),
                                  1 /* warnIsFatalb */);
  }

  Newx(Perl_MarpaX_ESLIF_Grammarp, 1, MarpaX_ESLIF_Grammar_t);
  marpaESLIFPerl_grammarContextInitv(aTHX_ ST(1) /* SV of eslif */, Perl_MarpaX_ESLIF_Grammarp);

  /* We use the "unsafe" version because we made sure in ESLIF.pm that marpaESLIFp was reallocated at every new interpreter (== perl thread) */
  marpaESLIFGrammarp = marpaESLIFGrammar_unsafe_newp(((MarpaX_ESLIF_Engine) Perl_MarpaX_ESLIF_Enginep)->marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    int save_errno = errno;
    marpaESLIFPerl_grammarContextFreev(aTHX_ Perl_MarpaX_ESLIF_Grammarp);
    MARPAESLIFPERL_CROAKF("marpaESLIFGrammar_newp failure, %s", strerror(save_errno));
  }
  Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp = marpaESLIFGrammarp;

  /* We want to take control over the free default action, and put something that is illegal via normal parse */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    int save_errno = errno;
    marpaESLIFPerl_grammarContextFreev(aTHX_ Perl_MarpaX_ESLIF_Grammarp);
    if (string1s != NULL) { Safefree(string1s); }
    if (string2s != NULL) { Safefree(string2s); }
    if (string3s != NULL) { Safefree(string3s); }
    MARPAESLIFPERL_CROAKF("marpaESLIFGrammar_ngrammarib failure, %s", strerror(save_errno));
  }
  for (i = 0; i < ngrammari; i++) {
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      int save_errno = errno;
      marpaESLIFPerl_grammarContextFreev(aTHX_ Perl_MarpaX_ESLIF_Grammarp);
      if (string1s != NULL) { Safefree(string1s); }
      if (string2s != NULL) { Safefree(string2s); }
      if (string3s != NULL) { Safefree(string3s); }
      MARPAESLIFPERL_CROAKF("marpaESLIFGrammar_defaults_by_levelb failure, %s", strerror(save_errno));
    }
    defaultFreeAction.type    = MARPAESLIF_ACTION_TYPE_NAME;
    defaultFreeAction.u.names = ":defaultFreeActions";
    marpaESLIFGrammarDefaults.defaultFreeActionp = &defaultFreeAction;
    if (! marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      int save_errno = errno;
      marpaESLIFPerl_grammarContextFreev(aTHX_ Perl_MarpaX_ESLIF_Grammarp);
      if (string1s != NULL) { Safefree(string1s); }
      if (string2s != NULL) { Safefree(string2s); }
      if (string3s != NULL) { Safefree(string3s); }
      MARPAESLIFPERL_CROAKF("marpaESLIFGrammar_defaults_by_levelb failure, %s", strerror(save_errno));
    }
  }

  if (string1s != NULL) { Safefree(string1s); }
  if (string2s != NULL) { Safefree(string2s); }
  if (string3s != NULL) { Safefree(string3s); }

  RETVAL = Perl_MarpaX_ESLIF_Grammarp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::DESTROY                                         */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::DESTROY";
CODE:
  marpaESLIFPerl_grammarContextFreev(aTHX_ Perl_MarpaX_ESLIF_Grammarp);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ngrammar                                        */
  /* ----------------------------------------------------------------------- */
=cut

IV
ngrammar(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::ngrammar";
CODE:
  int                ngrammari;

  if (! marpaESLIFGrammar_ngrammarib(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &ngrammari)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ngrammarib failure");
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
currentLevel(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::currentLevel";
CODE:
  int                leveli;

  if (! marpaESLIFGrammar_grammar_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammar_currentb failure");
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
currentDescription(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::currentDescription";
CODE:
  marpaESLIFString_t  *descp;
  SV                  *svp;

  if (! marpaESLIFGrammar_grammar_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, NULL, &descp)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammar_currentb failure");
  }
  /* It is in the same encoding as original grammar */
  svp = newSVpvn((const char *) descp->bytep, (STRLEN) descp->bytel);
  if (is_utf8_string((const U8 *) descp->bytep, (STRLEN) descp->bytel)) {
    SvUTF8_on(svp);
  }
  RETVAL = svp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::descriptionByLevel                              */
  /* ----------------------------------------------------------------------- */
=cut

SV *
descriptionByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::descriptionByLevel";
CODE:
  marpaESLIFString_t  *descp;
  SV                  *svp;

  if (! marpaESLIFGrammar_grammar_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_leveli, NULL, NULL, &descp)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammar_by_levelb failure");
  }
  /* It is in the same encoding as original grammar */
  svp = newSVpvn((const char *) descp->bytep, (STRLEN) descp->bytel);
  if (is_utf8_string((const U8 *) descp->bytep, (STRLEN) descp->bytel)) {
    SvUTF8_on(svp);
  }
  RETVAL = svp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentRuleIds                                  */
  /* ----------------------------------------------------------------------- */
=cut

AV *
currentRuleIds(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::currentRuleIds";
CODE:
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &ruleip, &rulel)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_rulearray_currentb failure");
  }
  if (rulel <= 0) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_rulearray_currentb returned no rule");
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
ruleIdsByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::ruleIdsByLevel";
CODE:
  int                 *ruleip;
  size_t               rulel;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_rulearray_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &ruleip, &rulel, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_rulearray_by_levelb failure");
  }
  if (rulel <= 0) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_rulearray_by_levelb returned no rule");
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
  /* MarpaX::ESLIF::Grammar::currentSymbolIds                                */
  /* ----------------------------------------------------------------------- */
=cut

AV *
currentSymbolIds(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::currentSymbolIds";
CODE:
  int                 *symbolip;
  size_t               symboll;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_symbolarray_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &symbolip, &symboll)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolarray_currentb failure");
  }
  if (symboll <= 0) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolarray_currentb returned no symbol");
  }
  av = newAV();
  for (i = 0; i < symboll; i++) {
    av_push(av, newSViv((IV) symbolip[i]));
  }
  RETVAL = av;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::symbolIdsByLevel                                */
  /* ----------------------------------------------------------------------- */
=cut

AV *
symbolIdsByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
PREINIT:
  static const char   *funcs = "MarpaX::ESLIF::Grammar::symbolIdsByLevel";
CODE:
  int                 *symbolip;
  size_t               symboll;
  size_t               i;
  AV                  *av;

  if (! marpaESLIFGrammar_symbolarray_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &symbolip, &symboll, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolarray_by_levelb failure");
  }
  if (symboll <= 0) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolarray_by_levelb returned no symbol");
  }
  av = newAV();
  for (i = 0; i < symboll; i++) {
    av_push(av, newSViv((IV) symbolip[i]));
  }
  RETVAL = av;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentProperties                               */
  /* ----------------------------------------------------------------------- */
=cut

SV *
currentProperties(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char           *funcs = "MarpaX::ESLIF::Grammar::currentProperties";
CODE:
  marpaESLIFGrammarProperty_t  grammarProperty;
  AV                          *avp;

  if (! marpaESLIFGrammar_grammarproperty_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &grammarProperty)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammarproperty_currentb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV     (avp, "level",               grammarProperty.leveli);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "maxLevel",            grammarProperty.maxLeveli);
  MARPAESLIFPERL_XV_STORE_STRING (avp, "description",         grammarProperty.descp);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "latm",                grammarProperty.latmb);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultSymbolAction", grammarProperty.defaultSymbolActionp);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultRuleAction",   grammarProperty.defaultRuleActionp);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultFreeAction",   grammarProperty.defaultFreeActionp);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "startId",             grammarProperty.starti);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "discardId",           grammarProperty.discardi);
  MARPAESLIFPERL_XV_STORE_IVARRAY(avp, "symbolIds",           grammarProperty.nsymboll, grammarProperty.symbolip);
  MARPAESLIFPERL_XV_STORE_IVARRAY(avp, "ruleIds",             grammarProperty.nrulel, grammarProperty.ruleip);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::propertiesByLevel                               */
  /* ----------------------------------------------------------------------- */
=cut

SV *
propertiesByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
PREINIT:
  static const char           *funcs = "MarpaX::ESLIF::Grammar::propertiesByLevel";
CODE:
  marpaESLIFGrammarProperty_t  grammarProperty;
  AV                          *avp;

  if (! marpaESLIFGrammar_grammarproperty_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &grammarProperty, (int) Perl_leveli, NULL /* descp */)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammarproperty_by_levelb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV     (avp, "level",               grammarProperty.leveli);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "maxLevel",            grammarProperty.maxLeveli);
  MARPAESLIFPERL_XV_STORE_STRING (avp, "description",         grammarProperty.descp);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "latm",                grammarProperty.latmb);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultSymbolAction", grammarProperty.defaultSymbolActionp);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultRuleAction",   grammarProperty.defaultRuleActionp);
  MARPAESLIFPERL_XV_STORE_ACTION (avp, "defaultFreeAction",   grammarProperty.defaultFreeActionp);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "startId",             grammarProperty.starti);
  MARPAESLIFPERL_XV_STORE_IV     (avp, "discardId",           grammarProperty.discardi);
  MARPAESLIFPERL_XV_STORE_IVARRAY(avp, "symbolIds",           grammarProperty.nsymboll, grammarProperty.symbolip);
  MARPAESLIFPERL_XV_STORE_IVARRAY(avp, "ruleIds",             grammarProperty.nrulel, grammarProperty.ruleip);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentRuleProperties                           */
  /* ----------------------------------------------------------------------- */
=cut

SV *
currentRuleProperties(Perl_MarpaX_ESLIF_Grammarp, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_rulei;
PREINIT:
  static const char        *funcs = "MarpaX::ESLIF::Grammar::currentRuleProperties";
CODE:
  marpaESLIFRuleProperty_t  ruleProperty;
  AV                       *avp;

  if (! marpaESLIFGrammar_ruleproperty_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruleProperty)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruleproperty_currentb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV         (avp, "id",                       ruleProperty.idi);
  MARPAESLIFPERL_XV_STORE_STRING     (avp, "description",              ruleProperty.descp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "show",                     ruleProperty.asciishows);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lhsId",                    ruleProperty.lhsi);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "separatorId",              ruleProperty.separatori);
  MARPAESLIFPERL_XV_STORE_IVARRAY    (avp, "rhsIds",                   ruleProperty.nrhsl, ruleProperty.rhsip);
  MARPAESLIFPERL_XV_STORE_IVARRAY    (avp, "skipIndices",              ruleProperty.nrhsl, ruleProperty.skipbp);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "exceptionId",              ruleProperty.exceptioni);
  MARPAESLIFPERL_XV_STORE_ACTION     (avp, "action",                   ruleProperty.actionp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "discardEvent",             ruleProperty.discardEvents);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardEventInitialState", ruleProperty.discardEventb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "rank",                     ruleProperty.ranki);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "nullRanksHigh",            ruleProperty.nullRanksHighb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "sequence",                 ruleProperty.sequenceb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "proper",                   ruleProperty.properb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "minimum",                  ruleProperty.minimumi);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "internal",                 ruleProperty.internalb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "propertyBitSet",           ruleProperty.propertyBitSet);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "hideseparator",            ruleProperty.hideseparatorb);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Rule__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::rulePropertiesByLevel                           */
  /* ----------------------------------------------------------------------- */
=cut

SV *
rulePropertiesByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
  IV                   Perl_rulei;
PREINIT:
  static const char        *funcs = "MarpaX::ESLIF::Grammar::rulePropertiesByLevel";
CODE:
  marpaESLIFRuleProperty_t  ruleProperty;
  AV                       *avp;

  if (! marpaESLIFGrammar_ruleproperty_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruleProperty, (int) Perl_leveli, NULL /* descp */)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruleproperty_by_levelb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV         (avp, "id",                       ruleProperty.idi);
  MARPAESLIFPERL_XV_STORE_STRING     (avp, "description",              ruleProperty.descp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "show",                     ruleProperty.asciishows);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lhsId",                    ruleProperty.lhsi);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "separatorId",              ruleProperty.separatori);
  MARPAESLIFPERL_XV_STORE_IVARRAY    (avp, "rhsIds",                   ruleProperty.nrhsl, ruleProperty.rhsip);
  MARPAESLIFPERL_XV_STORE_IVARRAY    (avp, "skipIndices",              ruleProperty.nrhsl, ruleProperty.skipbp);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "exceptionId",              ruleProperty.exceptioni);
  MARPAESLIFPERL_XV_STORE_ACTION     (avp, "action",                   ruleProperty.actionp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "discardEvent",             ruleProperty.discardEvents);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardEventInitialState", ruleProperty.discardEventb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "rank",                     ruleProperty.ranki);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "nullRanksHigh",            ruleProperty.nullRanksHighb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "sequence",                 ruleProperty.sequenceb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "proper",                   ruleProperty.properb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "minimum",                  ruleProperty.minimumi);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "internal",                 ruleProperty.internalb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "propertyBitSet",           ruleProperty.propertyBitSet);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "hideseparator",            ruleProperty.hideseparatorb);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Rule__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::currentSymbolProperties                         */
  /* ----------------------------------------------------------------------- */
=cut

SV *
currentSymbolProperties(Perl_MarpaX_ESLIF_Grammarp, Perl_symboli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_symboli;
PREINIT:
  static const char          *funcs = "MarpaX::ESLIF::Grammar::currentSymbolProperties";
CODE:
  marpaESLIFSymbolProperty_t  symbolProperty;
  AV                         *avp;

  if (! marpaESLIFGrammar_symbolproperty_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_symboli, &symbolProperty)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolproperty_currentb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV         (avp, "type",                       symbolProperty.type);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "start",                      symbolProperty.startb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discard",                    symbolProperty.discardb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardRhs",                 symbolProperty.discardRhsb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lhs",                        symbolProperty.lhsb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "top",                        symbolProperty.topb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "id",                         symbolProperty.idi);
  MARPAESLIFPERL_XV_STORE_STRING     (avp, "description",                symbolProperty.descp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventBefore",                symbolProperty.eventBefores);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventBeforeInitialState",    symbolProperty.eventBeforeb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventAfter",                 symbolProperty.eventAfters);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventAfterInitialState",     symbolProperty.eventAfterb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventPredicted",             symbolProperty.eventPredicteds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventPredictedInitialState", symbolProperty.eventPredictedb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventNulled",                symbolProperty.eventNulleds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventNulledInitialState",    symbolProperty.eventNulledb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventCompleted",             symbolProperty.eventCompleteds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventCompletedInitialState", symbolProperty.eventCompletedb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "discardEvent",               symbolProperty.discardEvents);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardEventInitialState",   symbolProperty.discardEventb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lookupResolvedLeveli",       symbolProperty.lookupResolvedLeveli);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "priority",                   symbolProperty.priorityi);
  MARPAESLIFPERL_XV_STORE_ACTION     (avp, "nullableAction",             symbolProperty.nullableActionp);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "propertyBitSet",             symbolProperty.propertyBitSet);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Symbol__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::symbolPropertiesByLevel                           */
  /* ----------------------------------------------------------------------- */
=cut

SV *
symbolPropertiesByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_symboli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
  IV                   Perl_symboli;
PREINIT:
  static const char          *funcs = "MarpaX::ESLIF::Grammar::symbolPropertiesByLevel";
CODE:
  marpaESLIFSymbolProperty_t  symbolProperty;
  AV                         *avp;

  if (! marpaESLIFGrammar_symbolproperty_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_symboli, &symbolProperty, (int) Perl_leveli, NULL /* descp */)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symbolproperty_by_levelb failure");
  }

  avp = newAV();
  MARPAESLIFPERL_XV_STORE_IV         (avp, "type",                       symbolProperty.type);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "start",                      symbolProperty.startb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discard",                    symbolProperty.discardb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardRhs",                 symbolProperty.discardRhsb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lhs",                        symbolProperty.lhsb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "top",                        symbolProperty.topb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "id",                         symbolProperty.idi);
  MARPAESLIFPERL_XV_STORE_STRING     (avp, "description",                symbolProperty.descp);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventBefore",                symbolProperty.eventBefores);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventBeforeInitialState",    symbolProperty.eventBeforeb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventAfter",                 symbolProperty.eventAfters);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventAfterInitialState",     symbolProperty.eventAfterb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventPredicted",             symbolProperty.eventPredicteds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventPredictedInitialState", symbolProperty.eventPredictedb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventNulled",                symbolProperty.eventNulleds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventNulledInitialState",    symbolProperty.eventNulledb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "eventCompleted",             symbolProperty.eventCompleteds);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "eventCompletedInitialState", symbolProperty.eventCompletedb);
  MARPAESLIFPERL_XV_STORE_ASCIISTRING(avp, "discardEvent",               symbolProperty.discardEvents);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "discardEventInitialState",   symbolProperty.discardEventb);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "lookupResolvedLeveli",       symbolProperty.lookupResolvedLeveli);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "priority",                   symbolProperty.priorityi);
  MARPAESLIFPERL_XV_STORE_ACTION     (avp, "nullableAction",             symbolProperty.nullableActionp);
  MARPAESLIFPERL_XV_STORE_IV         (avp, "propertyBitSet",             symbolProperty.propertyBitSet);

  RETVAL = marpaESLIFPerl_call_actionp(aTHX_ boot_MarpaX__ESLIF__Grammar__Symbol__Properties_svp, "new", avp, NULL /* Perl_MarpaX_ESLIF_Valuep */, 0 /* evalb */, 0 /* evalSilentb */);
  av_undef(avp);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleDisplay                                     */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleDisplay(Perl_MarpaX_ESLIF_Grammarp, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_rulei;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleDisplay";
CODE:
  char              *ruledisplays;

  if (! marpaESLIFGrammar_ruledisplayform_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruledisplays)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruledisplayform_currentb failure");
  }
  RETVAL = ruledisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::symbolDisplay                                   */
  /* ----------------------------------------------------------------------- */
=cut

char *
symbolDisplay(Perl_MarpaX_ESLIF_Grammarp, Perl_symboli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_symboli;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::symbolDisplay";
CODE:
  char              *symboldisplays;

  if (! marpaESLIFGrammar_symboldisplayform_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_symboli, &symboldisplays)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symboldisplayform_currentb failure");
  }
  RETVAL = symboldisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleShow                                        */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleShow(Perl_MarpaX_ESLIF_Grammarp, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_rulei;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleShow";
CODE:
  char              *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruleshows)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruleshowform_currentb failure");
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
ruleDisplayByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
  IV                   Perl_rulei;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleDisplayByLevel";
CODE:
  char              *ruledisplays;

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruledisplays, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruledisplayform_by_levelb failure");
  }
  RETVAL = ruledisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::symbolDisplayByLevel                            */
  /* ----------------------------------------------------------------------- */
=cut

char *
symbolDisplayByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_symboli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
  IV                   Perl_symboli;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::symbolDisplayByLevel";
CODE:
  char              *symboldisplays;

  if (! marpaESLIFGrammar_symboldisplayform_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_symboli, &symboldisplays, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_symboldisplayform_by_levelb failure");
  }
  RETVAL = symboldisplays;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Grammar::ruleShowByLevel                                 */
  /* ----------------------------------------------------------------------- */
=cut

char *
ruleShowByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli, Perl_rulei)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
  IV                   Perl_rulei;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::ruleShowByLevel";
CODE:
  char              *ruleshows;

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, (int) Perl_rulei, &ruleshows, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruleshowform_by_levelb failure");
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
show(Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::show";
CODE:
  char              *shows;

  if (! marpaESLIFGrammar_grammarshowform_currentb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &shows)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_ruleshowform_by_levelb failure");
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
showByLevel(Perl_MarpaX_ESLIF_Grammarp, Perl_leveli)
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  IV                   Perl_leveli;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::showByLevel";
CODE:
  char              *shows;

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &shows, (int) Perl_leveli, NULL)) {
    MARPAESLIFPERL_CROAK("marpaESLIFGrammar_grammarshowform_by_levelb failure");
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
parse(Perl_MarpaX_ESLIF_Grammarp, Perl_recognizerInterfacep, Perl_valueInterfacep)
  MarpaX_ESLIF_Grammar  Perl_MarpaX_ESLIF_Grammarp;
  SV                   *Perl_recognizerInterfacep;
  SV                   *Perl_valueInterfacep;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Grammar::parse";
CODE:
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  MarpaX_ESLIF_Recognizer_t     marpaESLIFRecognizerContext;
  MarpaX_ESLIF_Value_t          marpaESLIFValueContext;
  bool                          rcb;
  SV                           *svp;

  marpaESLIFPerl_paramIsRecognizerInterfacev(aTHX_ Perl_recognizerInterfacep);
  marpaESLIFPerl_paramIsValueInterfacev(aTHX_ Perl_valueInterfacep);

  marpaESLIFPerl_recognizerContextInitv(aTHX_ ST(0) /* SV of grammar */, ST(1) /* SV of recognizer interface */, &marpaESLIFRecognizerContext, NULL /* Perl_MarpaX_ESLIF_Recognizer_origp */);
  marpaESLIFPerl_valueContextInitv(aTHX_ NULL /* No recognizer */, ST(0) /* SV of grammar */, ST(2) /* SV of value interface */, &marpaESLIFValueContext);
  
  marpaESLIFRecognizerOption.userDatavp        = &marpaESLIFRecognizerContext;
  marpaESLIFRecognizerOption.readerCallbackp   = marpaESLIFPerl_readerCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithDisableThreshold");
  marpaESLIFRecognizerOption.exhaustedb        = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithExhaustion");
  marpaESLIFRecognizerOption.newlineb          = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithNewline");
  marpaESLIFRecognizerOption.trackb            = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithTrack");
  marpaESLIFRecognizerOption.bufsizl           = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */
  
  marpaESLIFValueOption.userDatavp             = &marpaESLIFValueContext;
  marpaESLIFValueOption.ruleActionResolverp    = marpaESLIFPerl_valueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp  = marpaESLIFPerl_valueSymbolActionResolver;
  marpaESLIFValueOption.freeActionResolverp    = marpaESLIFPerl_valueFreeActionResolver;
  marpaESLIFValueOption.importerp              = marpaESLIFPerl_importb;
  marpaESLIFValueOption.highRankOnlyb          = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithHighRankOnly");
  marpaESLIFValueOption.orderByRankb           = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithOrderByRank");
  marpaESLIFValueOption.ambiguousb             = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithAmbiguous");
  marpaESLIFValueOption.nullb                  = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithNull");
  marpaESLIFValueOption.maxParsesi             = (int) marpaESLIFPerl_call_methodi(aTHX_ Perl_valueInterfacep, "maxParses");

  if (! marpaESLIFGrammar_parseb(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL)) {
    goto err;
  }
  if (marpaESLIFPerl_GENERICSTACK_USED(&(marpaESLIFValueContext.valueStack)) != 1) {
    MARPAESLIFPERL_CROAKF("Internal value stack is %d instead of 1", marpaESLIFPerl_GENERICSTACK_USED(&(marpaESLIFValueContext.valueStack)));
  }
  svp = (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(marpaESLIFValueContext.valueStack));
  marpaESLIFPerl_call_methodv(aTHX_ Perl_valueInterfacep, "setResult", svp);

  rcb = 1;
  goto done;

  err:
  rcb = 0;

  done:
  marpaESLIFPerl_valueContextFreev(aTHX_ &marpaESLIFValueContext, 1 /* onStackb */);
  marpaESLIFPerl_recognizerContextFreev(aTHX_ &marpaESLIFRecognizerContext, 1 /* onStackb */);
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
new(Perl_packagep, Perl_MarpaX_ESLIF_Grammarp, Perl_recognizerInterfacep)
  SV *Perl_packagep;
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
  SV *Perl_recognizerInterfacep;
PREINIT:
  static const char            *funcs = "MarpaX::ESLIF::Recognizer::new";
CODE:
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  MarpaX_ESLIF_Recognizer_t    *Perl_MarpaX_ESLIF_Recognizerp;

  marpaESLIFPerl_paramIsRecognizerInterfacev(aTHX_ Perl_recognizerInterfacep);

  Newx(Perl_MarpaX_ESLIF_Recognizerp, 1, MarpaX_ESLIF_Recognizer_t);
  marpaESLIFPerl_recognizerContextInitv(aTHX_ ST(1) /* SV of MarpaX::ESLIF::Grammar */, ST(2) /* SV of recognizer interface */, Perl_MarpaX_ESLIF_Recognizerp, NULL /* Perl_MarpaX_ESLIF_Recognizer_origp */);

  marpaESLIFRecognizerOption.userDatavp        = Perl_MarpaX_ESLIF_Recognizerp;
  marpaESLIFRecognizerOption.readerCallbackp   = marpaESLIFPerl_readerCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithDisableThreshold");
  marpaESLIFRecognizerOption.exhaustedb        = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithExhaustion");
  marpaESLIFRecognizerOption.newlineb          = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithNewline");
  marpaESLIFRecognizerOption.trackb            = marpaESLIFPerl_call_methodb(aTHX_ Perl_recognizerInterfacep, "isWithTrack");
  marpaESLIFRecognizerOption.bufsizl           = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */

  Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp == NULL) {
    int save_errno = errno;
    marpaESLIFPerl_recognizerContextFreev(aTHX_ Perl_MarpaX_ESLIF_Recognizerp, 0 /* onStackb */);
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_newp failure, %s", strerror(errno));
  }

  RETVAL = Perl_MarpaX_ESLIF_Recognizerp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::newFrom                                      */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Recognizer
newFrom(Perl_MarpaX_ESLIF_Recognizer_origp, Perl_MarpaX_ESLIF_Grammarp)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer_origp;
  MarpaX_ESLIF_Grammar Perl_MarpaX_ESLIF_Grammarp;
PREINIT:
  static const char            *funcs = "MarpaX::ESLIF::Recognizer::newFrom";
CODE:
  MarpaX_ESLIF_Recognizer_t    *Perl_MarpaX_ESLIF_Recognizerp;

  Newx(Perl_MarpaX_ESLIF_Recognizerp, 1, MarpaX_ESLIF_Recognizer_t);
  marpaESLIFPerl_recognizerContextInitv(aTHX_ ST(1) /* SV of MarpaX::ESLIF::Grammar */, NULL /* Perl_recognizerInterfacep */, Perl_MarpaX_ESLIF_Recognizerp, ST(0) /* SV of Perl_MarpaX_ESLIF_Recognizer_origp */);

  Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newFromp(Perl_MarpaX_ESLIF_Grammarp->marpaESLIFGrammarp, Perl_MarpaX_ESLIF_Recognizer_origp->marpaESLIFRecognizerp);
  if (Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp == NULL) {
    int save_errno = errno;
    marpaESLIFPerl_recognizerContextFreev(aTHX_ Perl_MarpaX_ESLIF_Recognizerp, 0 /* onStackb */);
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_newp failure, %s", strerror(errno));
  }

  RETVAL = Perl_MarpaX_ESLIF_Recognizerp;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::set_exhausted_flag                           */
  /* ----------------------------------------------------------------------- */
=cut

void
set_exhausted_flag(Perl_MarpaX_ESLIF_Recognizer, flag)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  bool flag;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::set_exhausted_flag";
CODE:

  if (! marpaESLIFRecognizer_set_exhausted_flagb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, flag ? 1 : 0)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_set_exhausted_flagb failure, %s", strerror(errno));
  }

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::share                                        */
  /* ----------------------------------------------------------------------- */
=cut

void
share(Perl_MarpaX_ESLIF_Recognizer, Perl_MarpaX_ESLIF_RecognizerShared)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_RecognizerShared;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::share";
CODE:

  /*
   * The eventual previous reference on another shared recognizer has its refcount decreased.
   */
  MARPAESLIFPERL_REFCNT_DEC(Perl_MarpaX_ESLIF_Recognizer->Perl_MarpaX_ESLIF_Recognizer_origp);
  /*
   * We explicily create a reference on the shared SV to ensure proper destroy order - per def ST(1) is an SvRV
   */
  Perl_MarpaX_ESLIF_Recognizer->Perl_MarpaX_ESLIF_Recognizer_origp = newRV(SvRV(ST(1)));

  if (! marpaESLIFRecognizer_shareb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, Perl_MarpaX_ESLIF_RecognizerShared->marpaESLIFRecognizerp)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_shareb failure, %s", strerror(errno));
  }

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::DESTROY                                      */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(Perl_MarpaX_ESLIF_Recognizerp)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizerp;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::DESTROY";
CODE:
  marpaESLIFPerl_recognizerContextFreev(aTHX_ Perl_MarpaX_ESLIF_Recognizerp, 0 /* onStackb */);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::isCanContinue                                */
  /* ----------------------------------------------------------------------- */
=cut

bool
isCanContinue(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::isCanContinue";
CODE:
  short isCanContinueb;
  if (! marpaESLIFRecognizer_isCanContinueb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &(isCanContinueb))) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_isCanContinueb failure, %s", strerror(errno));
  }
  RETVAL = (bool) isCanContinueb;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::isExhausted";
CODE:
  short exhaustedb;
  if (! marpaESLIFRecognizer_isExhaustedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &(exhaustedb))) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_isExhaustedb failure, %s", strerror(errno));
  }
  RETVAL = (bool) exhaustedb;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::scan";
CODE:
  short initialEventsb;

  if (items > 1) {
    SV *Perl_initialEvents = ST(1);
    if ((marpaESLIFPerl_getTypei(aTHX_ Perl_initialEvents) & SCALAR) != SCALAR) {
      MARPAESLIFPERL_CROAK("First argument must be a scalar");
    }
    initialEventsb = SvIV(Perl_initialEvents) ? 1 : 0;
  } else {
    initialEventsb = 0;
  }

  RETVAL = (bool) marpaESLIFRecognizer_scanb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, initialEventsb, NULL /* continuebp */, NULL /* exhaustedbp */);
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::resume                                       */
  /* ----------------------------------------------------------------------- */
=cut

bool
resume(Perl_MarpaX_ESLIF_Recognizer, ...)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::resume";
CODE:
  int deltaLength;

  if (items > 1) {
    SV *Perl_deltaLength = ST(1);
    if ((marpaESLIFPerl_getTypei(aTHX_ Perl_deltaLength) & SCALAR) != SCALAR) {
      MARPAESLIFPERL_CROAK("First argument must be a scalar");
    }
    deltaLength = (int) SvIV(Perl_deltaLength);
  } else {
    deltaLength = 0;
  }

  if (deltaLength < 0) {
    MARPAESLIFPERL_CROAK("Resume delta length cannot be negative");
  }
  RETVAL = (bool) marpaESLIFRecognizer_resumeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (size_t) deltaLength, NULL /* continuebp */, NULL /* exhaustedbp */);
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::events";
CODE:
  AV                *list  = (AV *)sv_2mortal((SV *)newAV());
  HV                *hv;
  size_t             i;
  size_t             eventArrayl;
  marpaESLIFEvent_t *eventArrayp;
  SV                *svp;

  if (! marpaESLIFRecognizer_eventb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_eventb failure, %s", strerror(errno));
  }
  for (i = 0; i < eventArrayl; i++) {
    hv = (HV *)sv_2mortal((SV *)newHV());

    if (hv_store(hv, "type", strlen("type"), newSViv(eventArrayp[i].type), 0) == NULL) {
      MARPAESLIFPERL_CROAKF("hv_store failure for type => %d", eventArrayp[i].type);
    }

    if (eventArrayp[i].symbols != NULL) {
      svp = newSVpv(eventArrayp[i].symbols, 0);
      if (is_utf8_string((const U8 *) eventArrayp[i].symbols, 0)) {
        SvUTF8_on(svp);
      }
    } else {
      svp = &PL_sv_undef;
    }
    if (hv_store(hv, "symbol", strlen("symbol"), svp, 0) == NULL) {
      MARPAESLIFPERL_CROAKF("hv_store failure for symbol => %s", (eventArrayp[i].symbols != NULL) ? eventArrayp[i].symbols : "");
    }

    if (eventArrayp[i].events != NULL) {
      svp = newSVpv(eventArrayp[i].events, 0);
      if (is_utf8_string((const U8 *) eventArrayp[i].events, 0)) {
        SvUTF8_on(svp);
      }
    } else {
      svp = &PL_sv_undef;
    }
    if (hv_store(hv, "event",  strlen("event"),  svp, 0) == NULL) {
      MARPAESLIFPERL_CROAKF("hv_store failure for event => %s", (eventArrayp[i].events != NULL) ? eventArrayp[i].events : "");
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
PREINIT:
  static const char     *funcs     = "MarpaX::ESLIF::Recognizer::eventOnOff";
CODE:
  marpaESLIFEventType_t  eventSeti = MARPAESLIF_EVENTTYPE_NONE;
  SSize_t                avsizel   = av_len(eventTypes) + 1;
  SSize_t                aviteratorl;

  for (aviteratorl = 0; aviteratorl < avsizel; aviteratorl++) {
    int  codei;
    SV **svpp = av_fetch(eventTypes, aviteratorl, 0);
    if (svpp == NULL) {
      MARPAESLIFPERL_CROAK("av_fetch returned NULL");
    }
    if ((marpaESLIFPerl_getTypei(aTHX_ *svpp) & SCALAR) != SCALAR) {
      MARPAESLIFPERL_CROAKF("Element No %d of array must be a scalar", (int) aviteratorl);
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
      MARPAESLIFPERL_CROAKF("Unknown code %d", (int) codei);
      break;
    }
  }

  if (! marpaESLIFRecognizer_event_onoffb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, symbol, eventSeti, onOff ? 1 : 0)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_event_onoffb failure, %s", strerror(errno));
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
PREINIT:
  static const char       *funcs = "MarpaX::ESLIF::Recognizer::lexemeAlternative";
CODE:
  marpaESLIFAlternative_t  marpaESLIFAlternative;
  int                      grammarLength;

  if (items > 3) {
    SV *Perl_grammarLength = ST(3);
    if ((marpaESLIFPerl_getTypei(aTHX_ Perl_grammarLength) & SCALAR) != SCALAR) {
      MARPAESLIFPERL_CROAK("Third argument must be a scalar");
    }
    grammarLength = (int) SvIV(Perl_grammarLength);
  } else {
    grammarLength = 1;
  }

  if (grammarLength <= 0) {
    MARPAESLIFPERL_CROAK("grammarLength cannot be <= 0");
  }
  /* We maintain lifetime of this object */
  MARPAESLIFPERL_REFCNT_INC(sv);
  marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Recognizer->lexemeStack), sv);
  if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Recognizer->lexemeStack))) {
    MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Recognizer->lexemeStackp push failure, %s", strerror(errno));
  }

  marpaESLIFAlternative.lexemes               = (char *) name;
  marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.contextp        = MARPAESLIFPERL_CONTEXT;
  marpaESLIFAlternative.value.representationp = marpaESLIFPerl_representationb;
  marpaESLIFAlternative.value.u.p.p           = sv;
  marpaESLIFAlternative.value.u.p.shallowb    = 0; /* C.f. marpaESLIFPerl_valueFreeCallbackv */
  marpaESLIFAlternative.grammarLengthl        = (size_t) grammarLength;

  RETVAL = (bool) marpaESLIFRecognizer_lexeme_alternativeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &marpaESLIFAlternative);
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeComplete";
CODE:

  if (length < 0) {
    MARPAESLIFPERL_CROAK("Length cannot be < 0");
  }

  RETVAL = (bool) marpaESLIFRecognizer_lexeme_completeb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (size_t) length);
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
PREINIT:
  static const char       *funcs         = "MarpaX::ESLIF::Recognizer::lexemeRead";
CODE:
  int                      grammarLength = 1;
  marpaESLIFAlternative_t  marpaESLIFAlternative;

  if (items > 4) {
    SV *Perl_grammarLength = ST(4);
    if ((marpaESLIFPerl_getTypei(aTHX_ Perl_grammarLength) & SCALAR) != SCALAR) {
      MARPAESLIFPERL_CROAK("Fourth argument must be a scalar");
    }
    grammarLength = (int) SvIV(Perl_grammarLength);
  }

  if (grammarLength <= 0) {
    MARPAESLIFPERL_CROAK("grammarLength cannot be <= 0");
  }
  /* We maintain lifetime of this object */
  MARPAESLIFPERL_REFCNT_INC(sv);
  marpaESLIFPerl_GENERICSTACK_PUSH_PTR(&(Perl_MarpaX_ESLIF_Recognizer->lexemeStack), sv);
  if (marpaESLIFPerl_GENERICSTACK_ERROR(&(Perl_MarpaX_ESLIF_Recognizer->lexemeStack))) {
    MARPAESLIFPERL_CROAKF("Perl_MarpaX_ESLIF_Recognizer->lexemeStackp push failure, %s", strerror(errno));
  }

  marpaESLIFAlternative.lexemes               = (char *) name;
  marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.contextp        = MARPAESLIFPERL_CONTEXT;
  marpaESLIFAlternative.value.representationp = marpaESLIFPerl_representationb;
  marpaESLIFAlternative.value.u.p.p           = sv;
  marpaESLIFAlternative.value.u.p.shallowb    = 0; /* C.f. marpaESLIFPerl_valueFreeCallbackv */
  marpaESLIFAlternative.grammarLengthl        = (size_t) grammarLength;

  RETVAL = (bool) marpaESLIFRecognizer_lexeme_readb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &marpaESLIFAlternative, (size_t) length);
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeTry";
CODE:
  short              rcb;

  if (! marpaESLIFRecognizer_lexeme_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, &rcb)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_lexeme_tryb failure, %s", strerror(errno));
  }
  RETVAL = (bool) rcb;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::discardTry";
CODE:
  short              rcb;

  if (! marpaESLIFRecognizer_discard_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &rcb)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_discard_tryb failure, %s", strerror(errno));
  }
  RETVAL = (bool) rcb;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeExpected";
CODE:
  AV                *list  = (AV *)sv_2mortal((SV *)newAV());
  size_t             nLexeme;
  size_t             i;
  char             **lexemesArrayp;
  SV                *svp;

  if (! marpaESLIFRecognizer_lexeme_expectedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &nLexeme, &lexemesArrayp)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_lexeme_expectedb failure, %s", strerror(errno));
  }
  if (nLexeme > 0) {
    EXTEND(sp, (int) nLexeme);
    for (i = 0; i < nLexeme; i++) {
      if (lexemesArrayp[i] != NULL) {
        svp = newSVpv(lexemesArrayp[i], 0);
        if (is_utf8_string((const U8 *) lexemesArrayp[i], 0)) {
          SvUTF8_on(svp);
        }
      } else {
        svp = &PL_sv_undef;
      }
      av_push(list, svp);
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeLastPause";
CODE:
  char              *pauses;
  size_t             pausel;
  SV                *svp;

  if (!  marpaESLIFRecognizer_lexeme_last_pauseb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (char *) lexeme, &pauses, &pausel)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_lexeme_last_pauseb failure, %s", strerror(errno));
  }
  if ((pauses != NULL) && (pausel > 0)) {
    svp = newSVpvn((const char *) pauses, (STRLEN) pausel);
    if (is_utf8_string((const U8 *) pauses, (STRLEN) pausel)) {
      SvUTF8_on(svp);
    }
  } else {
    svp = &PL_sv_undef;
  }
  RETVAL = svp;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lexemeLastTry";
CODE:
  char              *trys;
  size_t             tryl;
  SV                *svp;

  if (!  marpaESLIFRecognizer_lexeme_last_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (char *) lexeme, &trys, &tryl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_lexeme_last_tryb failure, %s", strerror(errno));
  }
  if ((trys != NULL) && (tryl > 0)) {
    svp = newSVpvn((const char *) trys, (STRLEN) tryl);
    if (is_utf8_string((const U8 *) trys, (STRLEN) tryl)) {
      SvUTF8_on(svp);
    }
  } else {
    svp = &PL_sv_undef;
  }
  RETVAL = svp;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::discardLastTry";
CODE:
  char              *discards;
  size_t             discardl;
  SV                *svp;

  if (!  marpaESLIFRecognizer_discard_last_tryb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &discards, &discardl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_discard_last_tryb failure, %s", strerror(errno));
  }
  if ((discards != NULL) && (discardl > 0)) {
    svp = newSVpvn((const char *) discards, (STRLEN) discardl);
    if (is_utf8_string((const U8 *) discards, (STRLEN) discardl)) {
      SvUTF8_on(svp);
    }
  } else {
    svp = &PL_sv_undef;
  }
  RETVAL = svp;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::isEof";
CODE:
  short              eofb;

  if (! marpaESLIFRecognizer_isEofb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &eofb)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_isEofb failure, %s", strerror(errno));
  }
  RETVAL = (bool) eofb;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::read";
CODE:
  RETVAL = (bool) marpaESLIFRecognizer_readb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, NULL, NULL);
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::input";
CODE:
  char              *inputs;
  size_t             inputl;
  SV                *svp;

  if (! marpaESLIFRecognizer_inputb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &inputs, &inputl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_inputb failure, %s", strerror(errno));
  }
  if ((inputs != NULL) && (inputl > 0)) {
    svp = newSVpvn((const char *) inputs, (STRLEN) inputl);
    if (is_utf8_string((const U8 *) inputs, (STRLEN) inputl)) {
      SvUTF8_on(svp);
    }
  } else {
    svp = &PL_sv_undef;
  }
  RETVAL = svp;
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::progressLog";
CODE:
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
    MARPAESLIFPERL_CROAKF("Unknown logger level %d", (int) level);
    break;
  }

  if (! marpaESLIFRecognizer_progressLogb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, (int) start, (int) end, (int) level)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_progressLogb failure, %s", strerror(errno));
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lastCompletedOffset";
CODE:
  char              *offsetp;

  if (!  marpaESLIFRecognizer_last_completedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, &offsetp, NULL /* lengthlp */)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
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
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lastCompletedLength";
CODE:
  size_t             lengthl;

  if (!  marpaESLIFRecognizer_last_completedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, NULL /* offsetpp */, &lengthl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  RETVAL = (IV) lengthl;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::lastCompletedLocation                        */
  /* ----------------------------------------------------------------------- */
=cut

void
lastCompletedLocation(Perl_MarpaX_ESLIF_Recognizer, name)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  char                   *name;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::lastCompletedLocation";
PPCODE:
  size_t             lengthl;
  char              *offsetp;
  if (!  marpaESLIFRecognizer_last_completedb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, name, &offsetp, &lengthl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  EXTEND(SP, 2);
  PUSHs(sv_2mortal(newSViv(PTR2IV(offsetp))));
  PUSHs(sv_2mortal(newSViv((IV) lengthl)));

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::line                                         */
  /* ----------------------------------------------------------------------- */
=cut

IV
line(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::line";
CODE:
  size_t             linel;

  if (!  marpaESLIFRecognizer_locationb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &linel, NULL /* columnlp */)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }
  RETVAL = (IV) linel;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::column                                       */
  /* ----------------------------------------------------------------------- */
=cut

IV
column(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::column";
CODE:
  size_t             columnl;

  if (!  marpaESLIFRecognizer_locationb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, NULL /* linelp */, &columnl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }
  RETVAL = (IV) columnl;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::location                                     */
  /* ----------------------------------------------------------------------- */
=cut

void
location(Perl_MarpaX_ESLIF_Recognizer)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::location";
PPCODE:
  size_t             linel;
  size_t             columnl;
  if (!  marpaESLIFRecognizer_locationb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, &linel, &columnl)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }
  EXTEND(SP, 2);
  PUSHs(sv_2mortal(newSViv((IV) linel)));
  PUSHs(sv_2mortal(newSViv((IV) columnl)));

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Recognizer::hookDiscard                                  */
  /* ----------------------------------------------------------------------- */
=cut

void
hookDiscard(Perl_MarpaX_ESLIF_Recognizer, discardOnOffb)
  MarpaX_ESLIF_Recognizer Perl_MarpaX_ESLIF_Recognizer;
  short discardOnOffb;
PREINIT:
  static const char *funcs = "MarpaX::ESLIF::Recognizer::hookDiscard";
PPCODE:
  if (!  marpaESLIFRecognizer_hook_discardb(Perl_MarpaX_ESLIF_Recognizer->marpaESLIFRecognizerp, discardOnOffb)) {
    MARPAESLIFPERL_CROAKF("marpaESLIFRecognizer_hook_discardb failure, %s", strerror(errno));
  }

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Value

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::new                                               */
  /* ----------------------------------------------------------------------- */
=cut

MarpaX_ESLIF_Value
new(Perl_packagep, Perl_MarpaX_ESLIF_Recognizerp, Perl_valueInterfacep)
  SV                      *Perl_packagep;
  MarpaX_ESLIF_Recognizer  Perl_MarpaX_ESLIF_Recognizerp;
  SV                      *Perl_valueInterfacep;
PREINIT:
  static const char        *funcs = "MarpaX::ESLIF::Value::new";
CODE:
  MarpaX_ESLIF_Value        Perl_MarpaX_ESLIF_Valuep;
  marpaESLIFValueOption_t   marpaESLIFValueOption;

  marpaESLIFPerl_paramIsValueInterfacev(aTHX_ Perl_valueInterfacep);

  Newx(Perl_MarpaX_ESLIF_Valuep, 1, MarpaX_ESLIF_Value_t);
  marpaESLIFPerl_valueContextInitv(aTHX_ ST(1) /* SV of recognizer */, Perl_MarpaX_ESLIF_Recognizerp->Perl_MarpaX_ESLIF_Grammarp /* internal SvRV of grammar */, ST(2) /* SV of value interface */, Perl_MarpaX_ESLIF_Valuep);

  marpaESLIFValueOption.userDatavp            = Perl_MarpaX_ESLIF_Valuep;
  marpaESLIFValueOption.ruleActionResolverp   = marpaESLIFPerl_valueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp = marpaESLIFPerl_valueSymbolActionResolver;
  marpaESLIFValueOption.freeActionResolverp   = marpaESLIFPerl_valueFreeActionResolver;
  marpaESLIFValueOption.importerp             = marpaESLIFPerl_importb;
  marpaESLIFValueOption.highRankOnlyb         = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithHighRankOnly");
  marpaESLIFValueOption.orderByRankb          = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithOrderByRank");
  marpaESLIFValueOption.ambiguousb            = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithAmbiguous");
  marpaESLIFValueOption.nullb                 = marpaESLIFPerl_call_methodb(aTHX_ Perl_valueInterfacep, "isWithNull");
  marpaESLIFValueOption.maxParsesi            = (int) marpaESLIFPerl_call_methodi(aTHX_ Perl_valueInterfacep, "maxParses");

  Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep = marpaESLIFValue_newp(Perl_MarpaX_ESLIF_Recognizerp->marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep == NULL) {
    int save_errno = errno;
    marpaESLIFPerl_valueContextFreev(aTHX_ Perl_MarpaX_ESLIF_Valuep, 0 /* onStackb */);
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_newp failure, %s", strerror(save_errno));
  }

  RETVAL = Perl_MarpaX_ESLIF_Valuep;
OUTPUT:
  RETVAL

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::DESTROY                                           */
  /* ----------------------------------------------------------------------- */
=cut

void
DESTROY(Perl_MarpaX_ESLIF_Valuep)
  MarpaX_ESLIF_Value Perl_MarpaX_ESLIF_Valuep;
CODE:
  marpaESLIFPerl_valueContextFreev(aTHX_ Perl_MarpaX_ESLIF_Valuep, 0 /* onStackb */);

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Value::value                                             */
  /* ----------------------------------------------------------------------- */
=cut

bool
value(Perl_MarpaX_ESLIF_Valuep)
  MarpaX_ESLIF_Value Perl_MarpaX_ESLIF_Valuep;
PREINIT:
  static const char       *funcs = "MarpaX::ESLIF::Value::value";
CODE:
  short                    valueb;
  SV                      *svp;

  valueb = marpaESLIFValue_valueb(Perl_MarpaX_ESLIF_Valuep->marpaESLIFValuep);
  if (valueb < 0) {
    MARPAESLIFPERL_CROAKF("marpaESLIFValue_valueb failure, %s", strerror(errno));
  }
  if (valueb > 0) {
    if (marpaESLIFPerl_GENERICSTACK_USED(&(Perl_MarpaX_ESLIF_Valuep->valueStack)) != 1) {
      MARPAESLIFPERL_CROAKF("Internal value stack is %d instead of 1", marpaESLIFPerl_GENERICSTACK_USED(&(Perl_MarpaX_ESLIF_Valuep->valueStack)));
    }
    svp = (SV *) marpaESLIFPerl_GENERICSTACK_POP_PTR(&(Perl_MarpaX_ESLIF_Valuep->valueStack));
    marpaESLIFPerl_call_methodv(aTHX_ Perl_MarpaX_ESLIF_Valuep->Perl_valueInterfacep, "setResult", svp);
    RETVAL = (bool) 1;
  } else {
   RETVAL = (bool) 0;
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

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Rule::PropertyBitSet                                     */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Rule::PropertyBitSet

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Rule::PropertyBitSet::constant                           */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-rulePropertyBitSet-types.inc

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Symbol::PropertyBitSet                                   */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Symbol::PropertyBitSet

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Symbol::PropertyBitSet::constant                         */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-symbolPropertyBitSet-types.inc

=for comment
  /* ======================================================================= */
  /* MarpaX::ESLIF::Symbol::Type                                             */
  /* ======================================================================= */
=cut

MODULE = MarpaX::ESLIF            PACKAGE = MarpaX::ESLIF::Symbol::Type

PROTOTYPES: ENABLE

=for comment
  /* ----------------------------------------------------------------------- */
  /* MarpaX::ESLIF::Symbol::Type::constant                                   */
  /* ----------------------------------------------------------------------- */
=cut

INCLUDE: xs-symbol-types.inc
