#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include <genericStack.h>
#include <tconv.h>
#include <ctype.h>
#include "config.h"
#include "marpaESLIF/internal/structures.h"
#include "marpaESLIF/internal/logging.h"

#ifndef MARPAESLIF_INITIAL_REPLACEMENT_LENGTH
#define MARPAESLIF_INITIAL_REPLACEMENT_LENGTH 8096  /* Subjective number */
#endif

/* This is very internal: I use the genericLogger to generate strings */
typedef struct _marpaESLIF_stringGenerator {
  marpaESLIF_t *marpaESLIFp;
  char         *s;
  size_t        l;
  short         okb;
} _marpaESLIF_stringGenerator_t;

const static char *GENERICSTACKITEMTYPE_NA_STRING      = "NA";
const static char *GENERICSTACKITEMTYPE_CHAR_STRING    = "CHAR";
const static char *GENERICSTACKITEMTYPE_SHORT_STRING   = "SHORT";
const static char *GENERICSTACKITEMTYPE_INT_STRING     = "INT";
const static char *GENERICSTACKITEMTYPE_LONG_STRING    = "LONG";
const static char *GENERICSTACKITEMTYPE_FLOAT_STRING   = "FLOAT";
const static char *GENERICSTACKITEMTYPE_DOUBLE_STRING  = "DOUBLE";
const static char *GENERICSTACKITEMTYPE_PTR_STRING     = "PTR";
const static char *GENERICSTACKITEMTYPE_ARRAY_STRING   = "ARRAY";
const static char *GENERICSTACKITEMTYPE_UNKNOWN_STRING = "UNKNOWN";

static inline marpaESLIF_string_t   *_marpaESLIF_string_newp(marpaESLIF_t *marpaESLIFp, char *encodings, char *bytep, size_t bytel);
static inline marpaESLIF_string_t   *_marpaESLIF_string_shallowp(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp);
static inline marpaESLIF_string_t   *_marpaESLIF_string_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp);
static inline void                   _marpaESLIF_string_freev(marpaESLIF_string_t *stringp);
static inline short                  _marpaESLIF_string_eqb(marpaESLIF_string_t *string1p, marpaESLIF_string_t *string2p);
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, char *utf8s, size_t utf8l, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descs, size_t descl);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp);

static inline void                   _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp);
static inline void                   _marpaESLIF_lexemeStack_freev(genericStack_t *lexemeStackp);
static inline void                   _marpaESLIF_lexemeStack_resetv(genericStack_t *lexemeStackp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_ix_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix);
static inline short                  _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, size_t *sizelp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, char **pp);
static inline const char            *_marpaESLIF_genericStack_ix_types(genericStack_t *stackp, int ix);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp);

static inline void                   _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
                                                                    int leveli,
                                                                    char  *descs,
                                                                    size_t descl,
                                                                    short warningIsErrorb,
                                                                    short warningIsIgnoredb,
                                                                    short autorankb,
                                                                    int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
                                                                    int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
                                                                    int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep);
static inline short                  _marpaESLIFGrammar_validateb(marpaESLIFGrammar_t *marpaESLIFGrammar);

static inline short                  _marpaESLIFGrammar_terminal_string_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp);
static inline short                  _marpaESLIFGrammar_terminal_regex_matcherb(marpaESLIFGrammar_t *marpaESLIF_grammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp);
static inline short                  _marpaESLIFGrammar_meta_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp);
static inline short                  _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *desclp, char **fromEncodingsp);
static inline void                   _marpaESLIF_tconv_freev(char *utf8s);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip);
static        short                  _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp);
static inline short                  _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb, short *continuebp, short *exhaustedbp);
static inline marpaESLIF_symbol_t   *_marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *descs, size_t descl);
static inline marpaESLIF_grammar_t  *_marpaESLIFGrammar_grammar_findp(marpaESLIFGrammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int leveli);
static inline marpaESLIF_rule_t     *_marpaESLIF_rule_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int rulei);
static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int symboli);
static inline short                  _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei);
static inline void                   _marpaESLIF_freeCallbackv(void *userDatavp, void *p);
static inline void                   _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp);
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp);
static inline short                  _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp);
static        void                   _marpaESLIF_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
#ifndef MARPAESLIF_NTRACE
static inline void                   _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *headers, char *asciidescs, const char *p, size_t lengthl);
#endif
static inline short                  _marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

static        short                  _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static        short                  _marpaESLIFValueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static        short                  _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti);
static        short                  _marpaESLIFActionCallback(void *userDatavp, char *names, size_t namel, char *inputs, size_t inputl, int arg0i, int argni, int resulti);
static inline void                   _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb);
static        char                  *_marpaESLIFGrammar_symbolDescriptionCallback(void *userDatavp, int symboli);
static        short                  _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
static inline void                   _marpaESLIF_rule_createshowv(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep, char *asciishows, size_t *asciishowlp);

/*****************************************************************************/
static inline marpaESLIF_string_t *_marpaESLIF_string_newp(marpaESLIF_t *marpaESLIFp, char *encodings, char *bytep, size_t bytel)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_string_newp";
  marpaESLIF_string_t *stringp = NULL;
  char                *dstbytep;
  char                *dstasciis;

  if ((bytep == NULL) || (bytel <= 0)) {
    errno = EINVAL;
    MARPAESLIF_ERROR(marpaESLIFp, "Invalid input");
    goto err;
  }

  stringp = (marpaESLIF_string_t *) malloc(sizeof(marpaESLIF_string_t));
  if (stringp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  stringp->bytep     = NULL;
  stringp->bytel     = bytel;
  stringp->encodings = NULL;
  stringp->asciis    = NULL;
  stringp->shallowb  = 0;
  stringp->stringp   = 0;

  stringp->bytep = dstbytep = (char *) malloc(bytel);
  if (dstbytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(dstbytep, bytep, bytel);

  stringp->asciis = dstasciis = _marpaESLIF_tconv_newp(marpaESLIFp, "ASCII//TRANSLIT//IGNORE", encodings, bytep, bytel, NULL, &(stringp->encodings));
  if (dstasciis == NULL) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_string_freev(stringp);
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
static inline marpaESLIF_string_t *_marpaESLIF_string_shallowp(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_string_t *rcp = NULL;
  
  if (stringp == NULL) {
    goto err;
  }

  rcp = (marpaESLIF_string_t *) malloc(sizeof(marpaESLIF_string_t));
  if (rcp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(rcp, stringp, sizeof(marpaESLIF_string_t));
  rcp->shallowb = 1;
  rcp->stringp = stringp;

  goto done;

 err:
  _marpaESLIF_string_freev(rcp);
  rcp = NULL;

 done:
  return rcp;
}

/*****************************************************************************/
static inline marpaESLIF_string_t *_marpaESLIF_string_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_string_t *rcp = NULL;
  char                *bytep;
  size_t               bytel;
  char                *asciis;
  char                *encodings;
  
  if (stringp == NULL) {
    goto err;
  }

  rcp = (marpaESLIF_string_t *) malloc(sizeof(marpaESLIF_string_t));
  if (rcp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rcp->bytep     = NULL;
  rcp->bytel     = 0;
  rcp->encodings = NULL;
  rcp->asciis    = NULL;
  rcp->shallowb  = 0;
  rcp->stringp   = 0;

  bytep = rcp->bytep = (char *) malloc((rcp->bytel = bytel = stringp->bytel));
  if (bytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(bytep, stringp->bytep, bytel);

  rcp->asciis = asciis = strdup(stringp->asciis);
  if (asciis == NULL) {
    goto err;
  }

  rcp->encodings = encodings = strdup(stringp->encodings);
  if (encodings == NULL) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_string_freev(rcp);
  rcp = NULL;

 done:
  return rcp;
}

/*****************************************************************************/
static inline void _marpaESLIF_string_freev(marpaESLIF_string_t *stringp)
/*****************************************************************************/
{
  if (stringp != NULL) {
    if (! stringp->shallowb) {
      if (stringp->bytep != NULL) {
        free(stringp->bytep);
      }
      if (stringp->encodings != NULL) {
        free(stringp->encodings);
      }
      if (stringp->asciis != NULL) {
        free(stringp->asciis);
      }
    }
    free(stringp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIF_string_eqb(marpaESLIF_string_t *string1p, marpaESLIF_string_t *string2p)
/*****************************************************************************/
{
  char  *byte1p;
  char  *byte2p;
  size_t bytel;
  
  if ((string1p == NULL) || (string2p == NULL)) {
    return 0;
  }
  if (((byte1p = string1p->bytep) == NULL) || ((byte2p = string2p->bytep) == NULL)) {
    return 0;
  }
  if ((bytel = string1p->bytel) != string2p->bytel) {
    return 0;
  }
  return (memcmp(byte1p, byte2p, bytel) == 0) ? 1 : 0;
}

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, char *utf8s, size_t utf8l, char *testFullMatchs, char *testPartialMatchs)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_terminal_newp";
  marpaESLIF_terminal_t            *terminalp;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;
  marpaESLIF_uint32_t               pcre2Optioni;
  int                               pcre2Errornumberi;
  PCRE2_SIZE                        pcre2ErrorOffsetl;
  PCRE2_UCHAR                       pcre2ErrorBuffer[256];
  int                               i;
#ifndef MARPAESLIF_NTRACE
  marpaESLIF_matcher_value_t        rci;
#endif

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building terminal"); */

  terminalp = (marpaESLIF_terminal_t *) malloc(sizeof(marpaESLIF_terminal_t));
  if (terminalp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  terminalp->idi          = -1;
  terminalp->descp        = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;
  terminalp->matcherbp    = NULL;

  marpaWrapperGrammarSymbolOption.terminalb = 1;
  marpaWrapperGrammarSymbolOption.startb    = 0;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Terminal Identifier ------------ */
  terminalp->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
  if (terminalp->idi < 0) {
    goto err;
  }

  /* ----------- Terminal Description ------------ */
  terminalp->descp = _marpaESLIF_string_newp(marpaESLIFp, NULL, descs, descl);
  if (terminalp->descp == NULL) {
    goto err;
  }

  /* ----------- Terminal Implementation ------------ */
  terminalp->type = type;
  switch (type) {

  case MARPAESLIF_TERMINAL_TYPE_STRING:
    terminalp->matcherbp = _marpaESLIFGrammar_terminal_string_matcherb;
    terminalp->u.stringp = _marpaESLIF_string_newp(marpaESLIFp, "UTF-8", utf8s, utf8l);
    if (terminalp->u.stringp == NULL) {
      goto err;
    }

    break;

  case MARPAESLIF_TERMINAL_TYPE_REGEX:
    terminalp->u.regex.patternp      = NULL;
    terminalp->u.regex.match_datap   = NULL;
#ifdef PCRE2_CONFIG_JIT
    terminalp->u.regex.jitCompleteb = 0;
    terminalp->u.regex.jitPartialb  = 0;
#endif
    terminalp->matcherbp = _marpaESLIFGrammar_terminal_regex_matcherb;
    
    if ((utf8s == NULL) || (utf8l <= 0)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - invalid terminal origin", terminalp->descp->asciis);
      goto err;
    }
    pcre2Optioni = PCRE2_ANCHORED;      /* By default patterns are always anchored and only that */
    for (i = 0; i < _MARPAESLIF_REGEX_OPTION_ID_MAX; i++) {
      if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
	/* It is important to process pcre2OptionNoti first */
	if (marpaESLIF_regex_option_map[i].pcre2OptionNoti != 0) {
	  pcre2Optioni &= ~marpaESLIF_regex_option_map[i].pcre2OptionNoti;
	}
	if (marpaESLIF_regex_option_map[i].pcre2Optioni != 0) {
	  pcre2Optioni |= marpaESLIF_regex_option_map[i].pcre2Optioni;
	}
      }
    }
    terminalp->u.regex.patternp = pcre2_compile(
						(PCRE2_SPTR) utf8s,      /* An UTF-8 pattern */
						(PCRE2_SIZE) utf8l,      /* In code units (!= code points) - in UTF-8 a code unit is a byte */
						pcre2Optioni,
						&pcre2Errornumberi, /* for error number */
						&pcre2ErrorOffsetl, /* for error offset */
						NULL);        /* use default compile context */
    if (terminalp->u.regex.patternp == NULL) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_compile failure at offset %ld: %s", terminalp->descp->asciis, (unsigned long) pcre2ErrorOffsetl, pcre2ErrorBuffer);
      goto err;
    }
    terminalp->u.regex.match_datap = pcre2_match_data_create(1 /* We are interested in the string that matched the full pattern */,
                                                             NULL /* Default memory allocation */);
    if (terminalp->u.regex.match_datap == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_match_data_create_from_pattern failure, %s", terminalp->descp->asciis, strerror(errno));
      goto err;
    }
    /* Determine if we can do JIT */
#ifdef PCRE2_CONFIG_JIT
    if ((pcre2_config(PCRE2_CONFIG_JIT, &pcre2Optioni) >= 0) && (pcre2Optioni == 1)) {
#ifdef PCRE2_JIT_COMPLETE
      terminalp->u.regex.jitCompleteb = (pcre2_jit_compile(terminalp->u.regex.patternp, PCRE2_JIT_COMPLETE) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitCompleteb = 0;
#endif
#ifdef PCRE2_JIT_PARTIAL_HARD
      terminalp->u.regex.jitPartialb = (pcre2_jit_compile(terminalp->u.regex.patternp, PCRE2_JIT_PARTIAL_HARD) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitPartialb = 0;
#endif /*  PCRE2_CONFIG_JIT */
    } else {
      terminalp->u.regex.jitCompleteb = 0;
      terminalp->u.regex.jitPartialb = 0;
    }
#endif /*  PCRE2_CONFIG_JIT */
    /* And in the UTF mode is on */
    pcre2Errornumberi = pcre2_pattern_info(terminalp->u.regex.patternp, PCRE2_INFO_ALLOPTIONS, &pcre2Optioni);
    if (pcre2Errornumberi != 0) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_pattern_info failure: %s", terminalp->descp->asciis, pcre2ErrorBuffer);
      goto err;
    }
    terminalp->u.regex.utf8b = ((pcre2Optioni & PCRE2_UTF) == PCRE2_UTF);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - UTF mode is %s", terminalp->descp->asciis, terminalp->u.regex.utf8b ? "on" : "off");
    break;

  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "%s - unsupported terminal type %d", terminalp->descp->asciis, type);
    goto err;
    break;
  }

#ifndef MARPAESLIF_NTRACE
  {
    marpaESLIFGrammar_t marpaESLIFGrammar;

    marpaESLIFGrammar.marpaESLIFp   = marpaESLIFp;
    marpaESLIFGrammar.grammarStackp = NULL;
    marpaESLIFGrammar.grammarp      = grammarp;

    /* Look to the implementations of terminal matchers: they NEVER use leveli, nor marpaWrapperGrammarp, nor grammarStackp -; */
    
    if (testFullMatchs != NULL) {

      if (! terminalp->matcherbp(&marpaESLIFGrammar, 0, NULL, terminalp, NULL, testFullMatchs, strlen(testFullMatchs), 1, &rci, NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher general failure", terminalp->descp->asciis);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_OK) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher returned rci = %d", terminalp->descp->asciis, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s", terminalp->descp->asciis, testFullMatchs); */
    }

    if (testPartialMatchs != NULL) {

      if (! terminalp->matcherbp(&marpaESLIFGrammar, 0, NULL, terminalp, NULL, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher general failure", terminalp->descp->asciis);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_AGAIN) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher returned rci = %d", terminalp->descp->asciis, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing partial match is successful on %s when not at EOF", terminalp->descp->asciis, testPartialMatchs); */
    }
  }

#endif

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  terminalp = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp); */
  return terminalp;
}

/*****************************************************************************/
static inline marpaESLIF_meta_t *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_meta_newp";
  marpaESLIF_meta_t                *metap;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building meta"); */

  metap = (marpaESLIF_meta_t *) malloc(sizeof(marpaESLIF_meta_t));
  if (metap == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  metap->idi                       = -1;
  metap->descp                     = NULL;
  metap->matcherbp                 = NULL;
  metap->marpaWrapperGrammarClonep = NULL; /* Eventually changed when validating the grammar */

  marpaWrapperGrammarSymbolOption.terminalb = 0;
  marpaWrapperGrammarSymbolOption.startb    = 0;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Meta Identifier ------------ */
  metap->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
  if (metap->idi < 0) {
    goto err;
  }

  /* -------- Meta Description -------- */
  metap->descp = _marpaESLIF_string_newp(marpaESLIFp, NULL, descs, descl);
  if (metap->descp == NULL) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_meta_freev(metap);
  metap = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", metap); */
  return metap;
}

/*****************************************************************************/
static inline void _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap)
/*****************************************************************************/
{
  if (metap != NULL) {
    _marpaESLIF_string_freev(metap->descp);
    if (metap->marpaWrapperGrammarClonep != NULL) {
      marpaWrapperGrammar_freev(metap->marpaWrapperGrammarClonep);
    }
    /* All the rest are shallow pointers */
    free(metap);
  }
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_grammarp(marpaESLIFp,
					1, /* L0 in Marpa::R2 terminology is level No 1 for us */
                                        "L0",
                                        strlen("L0"),
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_L0_terminals) / sizeof(bootstrap_grammar_L0_terminals[0]),
					bootstrap_grammar_L0_terminals,
					sizeof(bootstrap_grammar_L0_metas) / sizeof(bootstrap_grammar_L0_metas[0]),
					bootstrap_grammar_L0_metas,
					sizeof(bootstrap_grammar_L0_rules) / sizeof(bootstrap_grammar_L0_rules[0]),
					bootstrap_grammar_L0_rules);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_grammarp(marpaESLIFp,
					0, /* G1 in Marpa::R2 terminology is level No 0 for us */
                                        "G1",
                                        strlen("G1"),
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_G1_terminals) / sizeof(bootstrap_grammar_G1_terminals[0]),
					bootstrap_grammar_G1_terminals,
					sizeof(bootstrap_grammar_G1_metas) / sizeof(bootstrap_grammar_G1_metas[0]),
					bootstrap_grammar_G1_metas,
					sizeof(bootstrap_grammar_G1_rules) / sizeof(bootstrap_grammar_G1_rules[0]),
					bootstrap_grammar_G1_rules);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
								   int leveli,
                                                                   char  *descs,
                                                                   size_t descl,
								   short warningIsErrorb,
								   short warningIsIgnoredb,
								   short autorankb,
								   int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
								   int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
								   int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep)
/*****************************************************************************/
{
  const static char          *funcs        = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_symbol_t        *symbolp      = NULL;
  marpaESLIF_rule_t          *rulep        = NULL;
  marpaESLIF_terminal_t      *terminalp    = NULL;
  marpaESLIF_meta_t          *metap        = NULL;
  marpaESLIF_grammar_t       *grammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  int                         i;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Bootstrapping grammar at level %d", (int) leveli);

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = warningIsErrorb;
  marpaWrapperGrammarOption.warningIsIgnoredb = warningIsIgnoredb;
  marpaWrapperGrammarOption.autorankb         = autorankb;
  marpaWrapperGrammarOption.exhaustionEventb  = 1;
  
  grammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, leveli, descs, descl);
  if (grammarp == NULL) {
    goto err;
  }

  /* First the terminals */
  for (i = 0; i < bootstrap_grammar_terminali; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
					  grammarp,
					  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
					  bootstrap_grammar_terminalp[i].descs,
					  strlen(bootstrap_grammar_terminalp[i].descs),
					  bootstrap_grammar_terminalp[i].terminalType,
					  bootstrap_grammar_terminalp[i].optioni,
					  bootstrap_grammar_terminalp[i].utf8s,
					  (bootstrap_grammar_terminalp[i].utf8s != NULL) ? strlen(bootstrap_grammar_terminalp[i].utf8s) : 0,
					  bootstrap_grammar_terminalp[i].testFullMatchs,
					  bootstrap_grammar_terminalp[i].testPartialMatchs
					  );
    if (terminalp == NULL) {
      goto err;
    }
    /* When bootstrapping the grammar, we expect terminal IDs to be exactly the value of the enum */
    if (terminalp->idi != bootstrap_grammar_terminalp[i].idi) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Got symbol ID %d from Marpa while we were expecting %d", terminalp->idi, bootstrap_grammar_terminalp[i].idi);
      goto err;
    }

    symbolp->type        = MARPAESLIF_SYMBOL_TYPE_TERMINAL;
    symbolp->u.terminalp = terminalp;
    symbolp->idi         = terminalp->idi;
    symbolp->descp       = terminalp->descp;
    symbolp->matcherbp   = terminalp->matcherbp;
    /* Terminal is now in symbol */
    terminalp = NULL;

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->u.terminalp->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the non-terminals */
  for (i = 0; i < bootstrap_grammar_metai; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }
    metap = _marpaESLIF_meta_newp(marpaESLIFp,
				  grammarp,
				  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
				  bootstrap_grammar_metap[i].descs,
				  strlen(bootstrap_grammar_metap[i].descs)
				  );
    if (metap == NULL) {
      goto err;
    }
    /* When bootstrapping the grammar, we expect meta IDs to be exactly the value of the enum */
    if (metap->idi != bootstrap_grammar_metap[i].idi) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Got symbol ID %d from Marpa while we were expecting %d", metap->idi, bootstrap_grammar_metap[i].idi);
      goto err;
    }

    symbolp->type       = MARPAESLIF_SYMBOL_TYPE_META;
    symbolp->startb     = bootstrap_grammar_metap[i].startb;
    symbolp->discardb   = bootstrap_grammar_metap[i].discardb;
    symbolp->u.metap    = metap;
    symbolp->idi        = metap->idi;
    symbolp->descp      = metap->descp;
    symbolp->matcherbp  = metap->matcherbp;
    /* Terminal is now in symbol */
    metap = NULL;

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->u.metap->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp set failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the rules */
  for (i = 0; i < bootstrap_grammar_rulei; i++) {
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
				  grammarp,
				  bootstrap_grammar_rulep[i].descs,
				  strlen(bootstrap_grammar_rulep[i].descs),
				  bootstrap_grammar_rulep[i].lhsi,
				  bootstrap_grammar_rulep[i].nrhsl,
				  bootstrap_grammar_rulep[i].rhsip,
				  bootstrap_grammar_rulep[i].maskbp,
				  0, /* nexceptionl */
				  NULL, /* exceptionip */
				  0, /* ranki */
				  0, /* nullRanksHighb */
				  (bootstrap_grammar_rulep[i].type == MARPAESLIF_RULE_TYPE_ALTERNATIVE) ? 0 : 1, /* sequenceb */
				  bootstrap_grammar_rulep[i].minimumi,
				  bootstrap_grammar_rulep[i].separatori,
				  bootstrap_grammar_rulep[i].properb
				  );
    if (rulep == NULL) {
      goto err;
    }
    GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
    if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: rulep is in grammarp->ruleStackp */
    rulep = NULL;
  }

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  _marpaESLIF_meta_freev(metap);
  _marpaESLIF_rule_freev(rulep);
  _marpaESLIF_symbol_freev(symbolp);
  _marpaESLIF_grammar_freev(grammarp);
  grammarp = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp); */
  return grammarp;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_validateb(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  const static char     *funcs                     = "_marpaESLIFGrammar_validateb";
  marpaESLIF_t          *marpaESLIFp               = marpaESLIFGrammarp->marpaESLIFp;
  genericStack_t        *grammarStackp             = marpaESLIFGrammarp->grammarStackp;
  marpaWrapperGrammar_t *marpaWrapperGrammarClonep = NULL;
  marpaESLIF_meta_t     *metap;
  genericStack_t        *symbolStackp;
  genericStack_t        *subSymbolStackp;
  genericStack_t        *ruleStackp;
  int                    grammari;
  int                    subGrammari;
  marpaESLIF_symbol_t   *symbolp;
  marpaESLIF_symbol_t   *tmpSymbolp;
  marpaESLIF_symbol_t   *subSymbolp;
  int                    symboli;
  int                    subSymboli;
  marpaESLIF_rule_t     *rulep;
  int                    rulei;
  marpaESLIF_grammar_t  *grammarp;
  marpaESLIF_grammar_t  *subGrammarp;
  marpaESLIF_grammar_t  *tmpGrammarp;
  short                  lhsb;
  marpaESLIF_symbol_t   *lhsp;
  int                    lookupLevelDeltai;
  marpaESLIF_string_t   *lookupGrammarStringp;
  marpaESLIF_symbol_t   *startp;
  marpaESLIF_symbol_t   *discardp;
  short                  rcb;
  size_t                 asciishowl;
  marpaESLIF_cloneContext_t marpaESLIF_cloneContext = {
    marpaESLIFp,
    NULL /* grammarp */
  };
  marpaWrapperGrammarCloneOption_t marpaWrapperGrammarCloneOption = {
    (void *) &marpaESLIF_cloneContext,
    NULL,
    NULL /* ruleOptionSetterp */
  };

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Validating ESLIF grammar"); */

  /* The rules are:

   1. There must be a grammar at level 0
   2. Grammar at any level must precompute at its start symbol and its eventual discard symbol
     a. Only one symbol can have the start flag
     b. Only one symbol can have the discard flag
   3. At any grammar level n, if a symbol never appear as an LHS of a rule, then
      it must be an LHS of grammar at level leveli, which must de-factor must also exist.
   4. At any grammar level n, if there is a :discard it must be precompute at this symbol

      It is not illegal to have sparse items in grammarStackp.

      The end of this routine is filling grammar information.
  */

  /*
   1. There must be a grammar at level 0
  */
  if (! GENERICSTACK_IS_PTR(grammarStackp, 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "No top-level grammar");
    goto err;
  }
  grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, 0);
  /*
   2. Grammar at level 0 must precompute at its start symbol, grammar at level n at its eventual discard symbol
     a. Only one symbol can have the start flag
     b. Only one symbol can have the discard flag
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    symbolStackp = grammarp->symbolStackp;
    ruleStackp = grammarp->ruleStackp;

    /* :start meta symbol check */
    startp = NULL;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (symbolp->startb) {
        if (startp == NULL) {
          startp = symbolp;
        } else {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "More than one :start symbol at grammar level %d (%s): symbols No %d (%s) and %d (%s)", grammari, grammarp->descp->asciis, startp->idi, startp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
          goto err;
        }
      }
    }
    if (startp == NULL) {
      /* Use the first rule */
      rulep = NULL;
      for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
        if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
          /* Should never happen, but who knows */
          continue;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
        break;
      }
      if (rulep == NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) is impossible: no rule", grammari, grammarp->descp->asciis);
        goto err;
      }
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at its default start symbol No %d (%s)", grammari, grammarp->descp->asciis, rulep->lhsp->idi, rulep->lhsp->descp->asciis);
      if (! marpaWrapperGrammar_precompute_startb(grammarp->marpaWrapperGrammarStartp, rulep->lhsp->idi)) {
        goto err;
      }
      grammarp->starti = rulep->lhsp->idi;
    } else {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at start symbol No %d (%s)", grammari, grammarp->descp->asciis, startp->idi, startp->descp->asciis);
      if (! marpaWrapperGrammar_precompute_startb(grammarp->marpaWrapperGrammarStartp, startp->idi)) {
        goto err;
      }
      grammarp->starti = startp->idi;
    }

    /* :discard meta symbol check */
    discardp = NULL;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (symbolp->discardb) {
        if (discardp == NULL) {
          discardp = symbolp;
        } else {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "More than one :discard symbol at grammar level %d (%s): symbols No %d (%s) and %d (%s)", grammari, grammarp->descp->asciis, discardp->idi, discardp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
          goto err;
        }
      }
    }
    if (discardp != NULL) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at discard symbol No %d (%s)", grammari, grammarp->descp->asciis, discardp->idi, discardp->descp->asciis);
      /* Please note that we disable ALL events in any discarded precomputed grammar except completion on start symbol */
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarCloneOption.symbolOptionSetterp = _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Precomputing grammar level %d (%s) at discard symbol No %d (%s): cloning failure", grammari, grammarp->descp->asciis, discardp->idi, discardp->descp->asciis);
        goto err;
      }
      if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, discardp->idi)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Precomputing grammar level %d (%s) at discard symbol No %d (%s): precompute failure", grammari, grammarp->descp->asciis, discardp->idi, discardp->descp->asciis);
        goto err;
      }
      grammarp->marpaWrapperGrammarDiscardp = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;
    }
  }
  
  /*
    3. In any rule of any grammar, an RHS can be at any level as well. Default being the current one.
    When the RHS level is the current level, if this RHS never appear as an LHS of another rule at the
    same level, then it must be an LHS of grammar at a resolved level, which must de-factor must also exist.
    
    Therefore every grammar is first scanned to detect all symbols that are truely LHS's at this level.
    Then every RHS of every rule is verified: it is must an LHS at its specified grammar level. When found,
    This resolved grammar is precomputed at this found LHS and the result is attached to the symbol of the
    parent grammar.
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at symbols in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    /* Loop on symbols */
    symbolStackp = grammarp->symbolStackp;
    ruleStackp = grammarp->ruleStackp;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      /* Only meta symbols should be looked at: if not an LHS then it is a dependency on a LHS of another grammar */
      if (symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) {
        continue;
      }

      lhsb = 0;
      for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
        if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
          /* Should never happen, but who knows */
          continue;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
        lhsp = rulep->lhsp;
        if (lhsp->lhsb) {
          /* Symbol already marked */
        }
        if (_marpaESLIF_string_eqb(lhsp->descp, symbolp->descp)) {
          /* Found */
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Grammar level %d (%s): symbol %d (%s) marked as LHS", grammari, grammarp->descp->asciis, lhsp->idi, lhsp->descp->asciis);
          lhsb = 1;
          break;
        }
      }
      symbolp->lhsb = lhsb;
    }
  }

  /* From grammar point of view, an expected symbol will always be either symbols explicitely created as terminals,
     either symbols not being an LHS. Per definition symbols created as terminals cannot be LHS symbols: precomputing
     the grammar will automatically fail. This is made sure by always precomputing at least grammar at level 0, and
     by precomputing any needed grammar at any other level with an alternative starting symbol.
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {

    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at rules in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    symbolStackp = grammarp->symbolStackp;
    for (symboli = 0; symboli <= GENERICSTACK_USED(symbolStackp); symboli++) {
      if (symboli < GENERICSTACK_USED(symbolStackp)) {
        if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
          /* Should never happen, but who knows */
          continue;
        }
        symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      } else {
        /* Faked additional entry */
        if (rulep->separatorp == NULL) {
          break;
        } else {
          symbolp = rulep->separatorp;
        }
      }
      /* Only non LHS meta symbols should be looked at */
      if ((symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) || symbolp->lhsb) {
        continue;
      }
      metap = symbolp->u.metap;
      /* Since we loop on symbols of every rule, it can very well happen that we hit */
      /* the same meta symbol more than once.                                        */
      if (metap->marpaWrapperGrammarClonep != NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) already processed", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
        continue;
      }

      /* Level of RHS is either via a grammar description, either via a number */
      /* They are exclusive (c.f. the grammar).                                */

      lookupLevelDeltai    = symbolp->lookupLevelDeltai;
      lookupGrammarStringp = symbolp->lookupGrammarStringp;
      subGrammarp = NULL;
      /* We always look to the eventual string first */
      if (lookupGrammarStringp != NULL) {
        /* Look for such a grammar description */
        for (subGrammari = 0; subGrammari < GENERICSTACK_USED(grammarStackp); subGrammari++) {
          if (! GENERICSTACK_IS_PTR(grammarStackp, subGrammari)) {
            continue;
          }
          tmpGrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
          if (_marpaESLIF_string_eqb(tmpGrammarp->descp, lookupGrammarStringp)) {
            break;
          }
        }
      } else {
        /* RHS level is relative - if RHS level is 0 the we fall back to current grammar */
        subGrammari = grammari + lookupLevelDeltai;
        if ((subGrammari >= 0) && GENERICSTACK_IS_PTR(grammarStackp, subGrammari)) {
          subGrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, subGrammari);
        }
      }
      /* Impossible */
      if (subGrammarp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing a non-existing grammar ", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
        goto err;
      }
      /* Make sure this RHS is an LHS in sub grammar, ignoring the case where sub grammar would be current grammar */
      subSymbolStackp = subGrammarp->symbolStackp;
      subSymbolp = NULL;
      for (subSymboli = 0; subSymboli < GENERICSTACK_USED(subSymbolStackp); subSymboli++) {
        if (! GENERICSTACK_IS_PTR(subSymbolStackp, subSymboli)) {
          /* Should never happen, but who knows */
          continue;
        }
        tmpSymbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(subSymbolStackp, subSymboli);
        if (_marpaESLIF_string_eqb(tmpSymbolp->descp, symbolp->descp)) {
          subSymbolp = tmpSymbolp;
          break;
        }
      }
      if (subSymbolp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing a non-existing symbol at grammar level %d (%s)", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, subGrammarp->leveli, subGrammarp->descp->asciis);
        goto err;
      }
      if (! subSymbolp->lhsb) {
        /* When sub grammar is current grammar, this mean that we require that this RHS is also an LHS - which is correct because we restricted symbol loop on meta symbols */
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing existing symbol No %d (%s) at grammar level %d (%s) but it is not an LHS", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, subSymbolp->idi, subSymbolp->descp->asciis, subGrammarp->leveli, subGrammarp->descp->asciis);
        goto err;
      }
      /* Very good - attach a precomputed grammar to this symbol, unless this is current grammar! */
      if (subGrammarp->leveli == grammarp->leveli) {
        continue;
      }
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(subGrammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        goto err;
      }
      if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, subSymbolp->idi)) {
        goto err;
      }
      /* Commit resolved level in symbol */
      symbolp->resolvedLeveli = subGrammarp->leveli;
      metap->marpaWrapperGrammarClonep = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;
      MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) have grammar resolved level set to %d", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, symbolp->resolvedLeveli);
    }
  }

  /* Fill grammars information */
  /* - rule IDs, rule show (ASCII) */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Filling rule IDs array in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    ruleStackp = grammarp->ruleStackp;
    grammarp->rulel = 0;
    for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
        continue;
      }
      grammarp->rulel++;
    }
    if (grammarp->rulel > 0) {
      grammarp->ruleip = (int *) malloc(grammarp->rulel * sizeof(int));
      if (grammarp->ruleip == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      grammarp->rulel = 0;
      for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
        if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
          continue;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
        grammarp->ruleip[grammarp->rulel++] = rulep->idi;
        _marpaESLIF_rule_createshowv(marpaESLIFp, rulep, NULL, &asciishowl);
        rulep->asciishows = (char *) malloc(asciishowl);
        if (rulep->asciishows == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        _marpaESLIF_rule_createshowv(marpaESLIFp, rulep, rulep->asciishows, NULL);
      }
    }
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  if (marpaWrapperGrammarClonep != NULL) {
    marpaWrapperGrammar_freev(marpaWrapperGrammarClonep);
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char             *funcs          = "_marpaESLIF_grammar_newp";
  genericLogger_t               *genericLoggerp = NULL;
  marpaESLIF_grammar_t          *grammarp       = NULL;
  _marpaESLIF_stringGenerator_t  _marpaESLIF_stringGenerator;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building ESLIF grammar"); */

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->marpaESLIFp                 = marpaESLIFp;
  grammarp->leveli                      = leveli;
  grammarp->descp                       = NULL;
  grammarp->marpaWrapperGrammarStartp   = NULL;
  grammarp->marpaWrapperGrammarDiscardp = NULL;
  grammarp->discardSymbolp              = NULL;
  grammarp->symbolStackp                = NULL;
  grammarp->ruleStackp                  = NULL;
  grammarp->actionp                     = NULL;
  grammarp->starti                      = 0;    /* Filled during grammar validation */
  grammarp->ruleip                      = NULL; /* Filled by grammar validation */
  grammarp->rulel                       = 0;    /* Filled by grammar validation */

  grammarp->marpaWrapperGrammarStartp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarStartp == NULL) {
    goto err;
  }
  if (descs == NULL) {
    /* Generate a default description */
    _marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;
    _marpaESLIF_stringGenerator.s           = NULL;
    _marpaESLIF_stringGenerator.l           = 0;
    _marpaESLIF_stringGenerator.okb         = 0;

    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &_marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      goto err;
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "Grammar level %d (autogenerated description)", leveli);
    if (! _marpaESLIF_stringGenerator.okb) {
      goto err;
    }
    grammarp->descp = _marpaESLIF_string_newp(marpaESLIFp, NULL, _marpaESLIF_stringGenerator.s, strlen(_marpaESLIF_stringGenerator.s));
    free(_marpaESLIF_stringGenerator.s);
  } else {
    grammarp->descp = _marpaESLIF_string_newp(marpaESLIFp, NULL, descs, descl);
  }
  if (grammarp->descp == NULL) {
    goto err;
  }
  GENERICSTACK_NEW(grammarp->symbolStackp);
  if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(grammarp->ruleStackp);
  if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_grammar_freev(grammarp);
  grammarp = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp); */
  GENERICLOGGER_FREE(genericLoggerp);
  return grammarp;
}

/*****************************************************************************/
static inline void _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  if (grammarp != NULL) {
    _marpaESLIF_string_freev(grammarp->descp);
    if (grammarp->marpaWrapperGrammarStartp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarStartp);
    }			       
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarDiscardp);
    }			       
    _marpaESLIF_symbolStack_freev(grammarp->symbolStackp);
    _marpaESLIF_ruleStack_freev(grammarp->ruleStackp);
    if (grammarp->ruleip != NULL) {
      free(grammarp->ruleip);
    }
    /* TO DO : actionp */
    free(grammarp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp)
/*****************************************************************************/
{
  if (ruleStackp != NULL) {
    while (GENERICSTACK_USED(ruleStackp) > 0) {
      if (GENERICSTACK_IS_PTR(ruleStackp, GENERICSTACK_USED(ruleStackp) - 1)) {
	marpaESLIF_rule_t *rulep = (marpaESLIF_rule_t *) GENERICSTACK_POP_PTR(ruleStackp);
	_marpaESLIF_rule_freev(rulep);
      } else {
	GENERICSTACK_USED(ruleStackp)--;
      }
    }
    GENERICSTACK_FREE(ruleStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_lexemeStack_freev(genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  if (lexemeStackp != NULL) {
    _marpaESLIF_lexemeStack_resetv(lexemeStackp);
    GENERICSTACK_FREE(lexemeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_lexemeStack_resetv(genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  if (lexemeStackp != NULL) {
    while (GENERICSTACK_USED(lexemeStackp) > 0) {
      if (GENERICSTACK_IS_ARRAY(lexemeStackp, GENERICSTACK_USED(lexemeStackp) - 1)) {
        GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_POP_ARRAY(lexemeStackp);
        if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
          free(GENERICSTACK_ARRAY_PTR(array));
        }
      } else {
	GENERICSTACK_USED(lexemeStackp)--;
      }
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_ix_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFRecognizer_lexemeStack_ix_resetb";
  short              rcb;

  if (lexemeStackp != NULL) {
    if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
      GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_GET_ARRAY(lexemeStackp, ix);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Resetting at indice %d of lexemeStackp", ix);
      if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
        free(GENERICSTACK_ARRAY_PTR(array));
      }
      GENERICSTACK_SET_NA(lexemeStackp, ix);
      if (GENERICSTACK_ERROR(lexemeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "lexemeStackp set failure, %s", strerror(errno));
        goto err;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb)
/*****************************************************************************/
{
  const static char               *funcs        = "_marpaESLIF_rule_newp";
  genericStack_t                  *symbolStackp = grammarp->symbolStackp;
  short                            symbolFoundb = 0;
  short                            separatorFoundb;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_rule_t               *rulep;
  marpaWrapperGrammarRuleOption_t  marpaWrapperGrammarRuleOption;
  size_t                           i;
  short                            maskb;
  int                              symboli;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building rule"); */

  rulep = (marpaESLIF_rule_t *) malloc(sizeof(marpaESLIF_rule_t));
  if (rulep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rulep->idi             = -1;
  rulep->descp           = NULL;
  rulep->asciishows      = NULL; /* Filled by grammar validation */
  rulep->lhsp            = NULL;
  rulep->separatorp      = NULL;
  rulep->rhsStackp       = NULL;
  rulep->maskStackp      = NULL;
  rulep->exceptionStackp = NULL;
  rulep->actionp         = NULL;
  rulep->ranki           = ranki;
  rulep->nullRanksHighb  = nullRanksHighb;
  rulep->sequenceb       = sequenceb;
  rulep->properb         = properb;
  rulep->minimumi        = minimumi;

  /* -------- Meta Description -------- */
  rulep->descp = _marpaESLIF_string_newp(marpaESLIFp, NULL, descs, descl);
  if (rulep->descp == NULL) {
    goto err;
  }

  /* Look to the symbol itself, and remember it is an LHS - this is used when validating the grammar */
  for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      /* Should never happen, but who knows */
      continue;
    }
    symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
    if (symbolp->idi == lhsi) {
      symbolFoundb = 1;
      break;
    }
  }
  if (! symbolFoundb) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbol No %d does not exist", grammarp->leveli, rulep->descp->asciis, lhsi);
    goto err;
  }
  symbolp->lhsb = 1;
  rulep->lhsp = symbolp;

  /* Idem for the separator */
  if (sequenceb && (separatori >= 0)) {
    separatorFoundb = 0;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (symbolp->idi == separatori) {
        separatorFoundb = 1;
        break;
      }
    }
    if (! separatorFoundb) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS separator No %d does not exist", grammarp->leveli, rulep->descp->asciis, separatori);
      goto err;
    }
    rulep->separatorp = symbolp;
  }

  GENERICSTACK_NEW(rulep->rhsStackp);
  if (GENERICSTACK_ERROR(rulep->rhsStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(rulep->maskStackp);
  if (GENERICSTACK_ERROR(rulep->maskStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "maskStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(rulep->exceptionStackp);
  if (GENERICSTACK_ERROR(rulep->exceptionStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* Fill rhs symbol stack */
  for (i = 0; i < nrhsl; i++) {
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, rhsip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS symbol No %d", grammarp->leveli, rulep->descp->asciis, rhsip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->rhsStackp, GENERICSTACK_GET_PTR(grammarp->symbolStackp, rhsip[i]));
    if (GENERICSTACK_ERROR(rulep->rhsStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* It is ASSUMED that the caller allocated as many items as there are RHSs */
    maskb = (maskbp != NULL) ? maskbp[i] : 0;
    GENERICSTACK_PUSH_SHORT(rulep->maskStackp, maskb);
    if (GENERICSTACK_ERROR(rulep->maskStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "maskStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  
  /* Fill exception symbol stack */
  for (i = 0; i < nexceptionl; i++) {
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, exceptionip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS exception symbol No %d", grammarp->leveli, rulep->descp->asciis, exceptionip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->exceptionStackp, GENERICSTACK_GET_PTR(grammarp->symbolStackp, exceptionip[i]));
    if (GENERICSTACK_ERROR(rulep->exceptionStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  
  marpaWrapperGrammarRuleOption.ranki            = ranki;
  marpaWrapperGrammarRuleOption.nullRanksHighb   = nullRanksHighb;
  marpaWrapperGrammarRuleOption.sequenceb        = sequenceb;
  marpaWrapperGrammarRuleOption.separatorSymboli = separatori;
  marpaWrapperGrammarRuleOption.properb          = properb;
  marpaWrapperGrammarRuleOption.minimumi         = minimumi;

  /* ----------- Meta Identifier ------------ */
  rulep->idi = marpaWrapperGrammar_newRulei(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarRuleOption, lhsi, nrhsl, rhsip);
  if (rulep->idi < 0) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_rule_freev(rulep);
  rulep = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rulep); */
  return rulep;
}

/*****************************************************************************/
static inline void _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep)
/*****************************************************************************/
{
  if (rulep != NULL) {
    _marpaESLIF_string_freev(rulep->descp);
    if (rulep->asciishows != NULL) {
      free(rulep->asciishows);
    }
    /* In the rule structure, lhsp, rhsStackp and exceptionStackp contain shallow pointers */
    /* Only the stack themselves should be freed. */
    /*
    _marpaESLIF_symbol_freev(rulep->lhsp);
    _marpaESLIF_symbolStack_freev(rulep->rhsStackp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, exceptionStackp);
    */
    GENERICSTACK_FREE(rulep->maskStackp);
    GENERICSTACK_FREE(rulep->rhsStackp);
    GENERICSTACK_FREE(rulep->exceptionStackp);
    /* TO DO : actionp */
    free(rulep);
  }
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_symbol_newp";
  marpaESLIF_symbol_t *symbolp;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building symbol"); */

  symbolp = (marpaESLIF_symbol_t *) malloc(sizeof(marpaESLIF_symbol_t));
  if (symbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto done;
  }

  symbolp->type   = MARPAESLIF_SYMBOL_TYPE_NA;
  /* Union itself is undetermined at this stage */
  symbolp->startb               = 0;
  symbolp->discardb             = 0;
  symbolp->lhsb                 = 0;
  symbolp->idi                  = -1;
  symbolp->descp                = NULL;
  symbolp->matcherbp            = NULL;
  symbolp->pauseb               = 0;
  symbolp->pauseIsOnb           = 0;
  symbolp->pausep               = NULL;
  symbolp->asciipauses          = NULL;
  symbolp->eventp               = NULL;
  symbolp->asciievents          = NULL;
  symbolp->lookupLevelDeltai    = 1;   /* Default lookup is the next grammar level */
  symbolp->lookupGrammarStringp = NULL;
  symbolp->resolvedLeveli       = -1; /* This will be overwriten by _marpaESLIFGrammar_validateb() and used only when symbol is a lexeme from another grammar */
  symbolp->priorityi            = 0; /* Default priority is 0 */
  symbolp->actionp              = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", symbolp); */
  return symbolp;
}

/*****************************************************************************/
static inline void _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  if (symbolp != NULL) {
    /* All pointers are the top level of this structure are shallow pointers */
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      _marpaESLIF_terminal_freev(symbolp->u.terminalp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      _marpaESLIF_meta_freev(symbolp->u.metap);
      break;
    default:
      break;
    }
    _marpaESLIF_string_freev(symbolp->pausep);
    _marpaESLIF_utf82printableascii_freev(symbolp->asciipauses);
    _marpaESLIF_string_freev(symbolp->eventp);
    _marpaESLIF_utf82printableascii_freev(symbolp->asciievents);
    /* TO DO : actionp */
    free(symbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp)
/*****************************************************************************/
{
  if (symbolStackp != NULL) {
    while (GENERICSTACK_USED(symbolStackp) > 0) {
      if (GENERICSTACK_IS_PTR(symbolStackp, GENERICSTACK_USED(symbolStackp) - 1)) {
	marpaESLIF_symbol_t *symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_POP_PTR(symbolStackp);
	_marpaESLIF_symbol_freev(symbolp);
      } else {
	GENERICSTACK_USED(symbolStackp)--;
      }
    }
    GENERICSTACK_FREE(symbolStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  if (terminalp != NULL) {
    _marpaESLIF_string_freev(terminalp->descp);
    switch (terminalp->type) {
    case MARPAESLIF_TERMINAL_TYPE_STRING:
      _marpaESLIF_string_freev(terminalp->u.stringp);
      break;
    case MARPAESLIF_TERMINAL_TYPE_REGEX:
      if (terminalp->u.regex.match_datap != NULL) {
	pcre2_match_data_free(terminalp->u.regex.match_datap);
      }
      if (terminalp->u.regex.patternp != NULL) {
	pcre2_code_free(terminalp->u.regex.patternp);
      }
      break;
    default:
      break;
    }
    free(terminalp);
  }
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp)
/*****************************************************************************/
{
  const static char    *funcs       = "marpaESLIF_newp";
  marpaESLIF_grammar_t *grammarp    = NULL;
  marpaESLIF_t         *marpaESLIFp = NULL;
  genericLogger_t      *genericLoggerp;

  if (marpaESLIFOptionp == NULL) {
    marpaESLIFOptionp = &marpaESLIFOption_default;
  }

  genericLoggerp = marpaESLIFOptionp->genericLoggerp;

#ifndef MARPAESLIF_NTRACE
  if (genericLoggerp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Building ESLIF", funcs);
  }
#endif

  marpaESLIFp = (marpaESLIF_t *) malloc(sizeof(marpaESLIF_t));
  if (marpaESLIFp == NULL) {
    if (marpaESLIFOptionp->genericLoggerp != NULL) {
      GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "malloc failure, %s", strerror(errno));
      goto err;
    }
  }

  /* Create internal ESLIF grammar - it is important to set the option first */
  marpaESLIFp->marpaESLIFOption   = *marpaESLIFOptionp;
  marpaESLIFp->marpaESLIFGrammarp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));

  if (marpaESLIFp->marpaESLIFGrammarp == NULL) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFp->marpaESLIFGrammarp->marpaESLIFp   = marpaESLIFp;
  marpaESLIFp->marpaESLIFGrammarp->grammarStackp = NULL;
  marpaESLIFp->marpaESLIFGrammarp->grammarp      = NULL;

  GENERICSTACK_NEW(marpaESLIFp->marpaESLIFGrammarp->grammarStackp);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* G1 */
  grammarp = _marpaESLIF_bootstrap_grammar_G1p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL;

  /* L0 */
  grammarp = _marpaESLIF_bootstrap_grammar_L0p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL;

  /* Validate the bootstrap grammar */
  if (! _marpaESLIFGrammar_validateb(marpaESLIFp->marpaESLIFGrammarp)) {
    goto err;
  }

  /* Commit G1 grammar as the current grammar */
  if (! GENERICSTACK_IS_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, 0)) {
    GENERICLOGGER_ERROR(marpaESLIFOptionp->genericLoggerp, "No top level grammar after bootstrap");
    goto err;
  }
  marpaESLIFp->marpaESLIFGrammarp->grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, 0);
  goto done;
  
 err:
  marpaESLIF_freev(marpaESLIFp);
  marpaESLIFp = NULL;

 done:
  _marpaESLIF_grammar_freev(grammarp);
#ifndef MARPAESLIF_NTRACE
  if (genericLoggerp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] return %p", funcs, marpaESLIFp);
  }
#endif
	
  return marpaESLIFp;
}


/*****************************************************************************/
void marpaESLIF_freev(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  if (marpaESLIFp != NULL) {
    marpaESLIFGrammar_freev(marpaESLIFp->marpaESLIFGrammarp);
    free(marpaESLIFp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_terminal_string_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp)
/*****************************************************************************/
{
  const static char          *funcs = "_marpaESLIFGrammar_terminal_string_matcherb";
  marpaESLIF_t               *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIF_matcher_value_t  rci;
  marpaESLIF_string_t        *stringp = terminalp->u.stringp;

  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*                                                                               */
  /* When a string terminal is created in the grammar, it's always stored in UTF-8 */
  /* therefore the input must be in UTF-8 as well. We make sure that eventual BOM  */
  /* in input is discarded. There is no UTF-8 BOM in the string pattern.           */
  /*                                                                               */
  /* This is STRING matcher: we think in terms of characters here. Therefore the   */
  /* input has to be in UTF-8.                                                     */
  /* Then we do a memcmp() outself, believed to be faster than PCRE2 pattern.      */
  /*********************************************************************************/

  if (inputl > 0) {

    if (inputl >= stringp->bytel) {
      if (memcmp(inputs, stringp->bytep, stringp->bytel) == 0) { /* Per def a terminal have bytel > 0 */
        rci = MARPAESLIF_MATCH_OK;
      } else {
        rci = MARPAESLIF_MATCH_FAILURE;
      }
    } else {
      /* Partial match never returns OK -; */
      rci = (memcmp(inputs, stringp->bytep, inputl) == 0) ? (eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN) : MARPAESLIF_MATCH_FAILURE;
    }
  } else {
    rci = eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN;
  }

  if (rcip != NULL) {
    *rcip = rci;
  }

  if (rci == MARPAESLIF_MATCH_OK) {
    if (outputStackp != NULL) {
      /* Symbol callback will realize it is a PTR, i.e. a pointer to a marpaESLIF_string_t */
      GENERICSTACK_PUSH_PTR(outputStackp, stringp);
      if (GENERICSTACK_ERROR(outputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "stack push failure, %s", strerror(errno));
        goto err;
      }
    }
  }

  goto done;

 err:
  return 0;

 done:
  /* Here it is guaranteed that tmps is always NULL as well */
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_terminal_regex_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp)
/*****************************************************************************/
{
  const static char              *funcs = "_marpaESLIFGrammar_terminal_regex_matcherb";
  marpaESLIF_t                   *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  char                           *tmps  = NULL;
  marpaESLIF_matcher_value_t      rci;
  marpaESLIF_regex_t              marpaESLIF_regex;
  int                             pcre2Errornumberi;
  PCRE2_UCHAR                     pcre2ErrorBuffer[256];
  PCRE2_SIZE                     *pcre2_ovectorp;
  size_t                          matchedLengthl;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
 
  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*                                                                               */
  /* There is a fundamental difference between regex matching and string matching: */
  /* Even in the regex pattern was compiled using the grammar source converted in  */
  /* UTF-8, we did that ONLY because we are using the UTF-8 API of PCRE2, which is */
  /* expecting CHARACTERS in the pattern. Okay, this for the pattern.              */
  /* Now comes the subject: if the pattern was NOT compiled with UTF support, then */
  /* this mean we work on binary data. If the pattern was compiled with UTF, then  */
  /* we work on characters.                                                        */
  /* If its exactly for this reason that the input to this method is the input     */
  /* AS-IS from the stream. We have to check the pattern properties first.         */
  /* So, back to the pattern property. If we had the full control on it with only  */
  /* the modifiers, things would be easy:                                          */
  /* - "u" modifier   set PCRE2_UTF                                                */
  /* - "a" modifier unset PCRE2_UTF                                                */
  /* (Note that the "n" and "N" modifiers are irrelevant here)                     */
  /* But the user can change the pattern behaviour without our knowledge, using    */
  /* the (*XXX) hooks. This is why a call to pcre2_pattern_info() is always done   */
  /* AFTER pattern is compiled. This information is used to set the utf8b flag     */
  /* in the marpaESLIF_regex_t type.                                               */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_regex = terminalp->u.regex;

    /* --------------------------------------------------------- */
    /* EOF mode:                                                 */
    /* return full match status: OK or FAILURE.                  */
    /* --------------------------------------------------------- */
    /* NOT EOF mode:                                             */
    /* If the full match is successful:                          */
    /* - if it reaches the end of the buffer, return EGAIN.      */
    /* - if it does not reach the end of the buffer, return OK.  */
    /* Else if the partial match is successul:                   */
    /* - return EGAIN.                                           */
    /* Else                                                      */
    /* - return FAILURE.                                         */
    /*                                                           */
    /* In conclusion we always start with the full match.        */
    /* --------------------------------------------------------- */
#ifdef PCRE2_CONFIG_JIT
    if (marpaESLIF_regex.jitCompleteb) {
      pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,     /* code */
                                          (PCRE2_SPTR) inputs,          /* subject */
                                          (PCRE2_SIZE) inputl,          /* length */
                                          (PCRE2_SIZE) 0,               /* startoffset */
                                          PCRE2_NOTEMPTY,               /* options - this one is supported in JIT mode */
                                          marpaESLIF_regex.match_datap, /* match data */
                                          NULL                          /* match context - used default */
                                          );
      if (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) {
        /* Back luck, out of stack for JIT */
        pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
        goto eof_nojitcomplete;
      }
    } else {
    eof_nojitcomplete:
#endif
      pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
                                      (PCRE2_SPTR) inputs,          /* subject */
                                      (PCRE2_SIZE) inputl,          /* length */
                                      (PCRE2_SIZE) 0,               /* startoffset */
                                      PCRE2_NOTEMPTY,               /* options */
                                      marpaESLIF_regex.match_datap, /* match data */
                                      NULL                          /* match context - used default */
                                      );
#ifdef PCRE2_CONFIG_JIT
    }
#endif

    if (eofb) {
      if (pcre2Errornumberi < 0) {
        /* Only PCRE2_ERROR_NOMATCH is an acceptable error. */
        if (pcre2Errornumberi != PCRE2_ERROR_NOMATCH) {
          pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
          MARPAESLIF_WARNF(marpaESLIFp, "%s - Uncaught pcre2 match failure: %s", terminalp->descp->asciis, pcre2ErrorBuffer);
        }
        rci = MARPAESLIF_MATCH_FAILURE;
      } else {
        /* Check the length of matched data */
        if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_count returned no number of pairs of values", terminalp->descp->asciis);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_pointer returned NULL", terminalp->descp->asciis);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - Empty match when it is configured as not possible", terminalp->descp->asciis);
          goto err;
        }
        /* Very good -; */
        rci = MARPAESLIF_MATCH_OK;
      }
    } else {
      if (pcre2Errornumberi >= 0) {
        /* Full match is successful. */
        /* Check the length of matched data */
        if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_count returned no number of pairs of values", terminalp->descp->asciis);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_pointer returned NULL", terminalp->descp->asciis);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - Empty match when it is configured as not possible", terminalp->descp->asciis);
          goto err;
        }
        if (matchedLengthl >= inputl) {
          /* But end of the buffer is reached, and we are not at the eof! We have to ask for more bytes. */
          rci = MARPAESLIF_MATCH_AGAIN;
        } else {
          /* And end of the buffer is not reached */
          rci = MARPAESLIF_MATCH_OK;
        }
      } else {
        /* Do a partial match. This section cannot return MARPAESLIF_MATCH_OK. */
#ifdef PCRE2_CONFIG_JIT
        if (marpaESLIF_regex.jitPartialb) {
          pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,     /* code */
                                              (PCRE2_SPTR) inputs,          /* subject */
                                              (PCRE2_SIZE) inputl,          /* length */
                                              (PCRE2_SIZE) 0,               /* startoffset */
                                              PCRE2_NOTEMPTY                /* options - this one is supported in JIT mode */
                                              |
                                              PCRE2_PARTIAL_HARD,           /* and this one as well, we want partial match to have precedence */
                                              marpaESLIF_regex.match_datap, /* match data */
                                              NULL                          /* match context - used default */
                                              );
          if (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) {
            /* Back luck, out of stack for JIT */
            pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
            goto eof_nojitpartial;
          }
        } else {
        eof_nojitpartial:
#endif
          pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
                                          (PCRE2_SPTR) inputs,          /* subject */
                                          (PCRE2_SIZE) inputl,          /* length */
                                          (PCRE2_SIZE) 0,               /* startoffset */
                                          PCRE2_NOTEMPTY                /* options */
                                          |
                                          PCRE2_PARTIAL_HARD,           /* we want partial match to have precedence */
                                          marpaESLIF_regex.match_datap, /* match data */
                                          NULL                          /* match context - used default */
                                          );
#ifdef PCRE2_CONFIG_JIT
        }
#endif
        /* Only PCRE2_ERROR_PARTIAL is an acceptable error */
        if (pcre2Errornumberi == PCRE2_ERROR_PARTIAL) {
          /* Partial match is successful */
          rci = MARPAESLIF_MATCH_AGAIN;
        } else {
          /* Partial match is not successful */
          rci = MARPAESLIF_MATCH_FAILURE;
        }
      }
    }
  } else {
    rci = eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN;
  }

  if (rcip != NULL) {
    *rcip = rci;
  }

  if (rci == MARPAESLIF_MATCH_OK) {
    if (outputStackp != NULL) {
      tmps = (char *) malloc(matchedLengthl);
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(tmps, inputs, matchedLengthl);
      GENERICSTACK_ARRAY_LENGTH(array) = matchedLengthl;
      GENERICSTACK_ARRAY_PTR(array) = tmps;
      /* Symbol callback will realize it is an ARRAY, i.e. allocated memory that should be released. */
      GENERICSTACK_PUSH_ARRAY(outputStackp, array);
      if (GENERICSTACK_ERROR(outputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "stack push failure, %s", strerror(errno));
        goto err;
      }
      tmps = NULL;
    }
  }

  goto done;

 err:
  /* Only here it is possible that tmps is != NULL */
  if (tmps != NULL) {
    free(tmps);
  }
  return 0;

 done:
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_meta_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module, and the cause of recursion -; */
  const static char           *funcs       = "_marpaESLIFGrammar_meta_matcherb";
  marpaESLIF_t                *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  short                        rcb;
  marpaESLIFGrammar_t          marpaESLIFGrammar; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t         grammar;
  marpaESLIF_grammar_t        *grammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption; /* This is an internal recognizer */
  marpaESLIFValueOption_t      marpaESLIFValueOption = marpaESLIFValueOption_default_template;
  
  /* A meta matcher is always using ANOTHER grammar at level grammarLeveli (validator guaranteed that is exists) that is sent on the stack. */
  /* Though the precomputed grammar is known to the symbol that called us, also sent on the stack. */
  if (! GENERICSTACK_IS_PTR(marpaESLIFGrammarp->grammarStackp, grammarLeveli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar No %d (%s), meta symbol No %d (%s) resolve to a grammar level %d that do not exist", marpaESLIFGrammarp->grammarp->leveli, marpaESLIFGrammarp->grammarp->descp->asciis, metap->idi, metap->descp->asciis, grammarLeveli);
    goto err;
  }
  grammarp                           = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammarLeveli);
  grammar                            = *grammarp;
  grammar.marpaWrapperGrammarStartp  = marpaWrapperGrammarp;
  marpaESLIFGrammar                  = *marpaESLIFGrammarp;
  marpaESLIFGrammar.grammarp         = &grammar;

  marpaESLIFRecognizerOption                   = marpaESLIFRecognizerParentp->marpaESLIFRecognizerOption;
  marpaESLIFRecognizerOption.disableThresholdb = 1;
  marpaESLIFRecognizerOption.exhaustedb        = 1;
  marpaESLIFRecognizerOption.latmb             = 1;

  marpaESLIFValueOption.userDatavp      = NULL; /* This is OUR internal action - we know that we will be called with a marpaESLIFValue_t pointer, not yet available */
  marpaESLIFValueOption.actionCallbackp = _marpaESLIFActionCallback;
  marpaESLIFValueOption.outputStackp    = outputStackp;

  if (! _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, 0 /* discardb */, marpaESLIFRecognizerParentp, exhaustedbp)) {
    goto err;
  }

  *rcip = MARPAESLIF_MATCH_OK;
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIFRecognizer_symbol_matcherb";
  short              rcb;

  switch (symbolp->type) {
  case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
    /* A terminal matcher NEVER updates the stream : inputs, inputl and eof can be passed as is. */
    rcb = symbolp->matcherbp(marpaESLIFRecognizerp->marpaESLIFGrammarp,
                             -1, /* grammarLeveli */
                             NULL /* marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp */,
                             symbolp->u.terminalp,
                             NULL /* metap */,
                             marpaESLIFRecognizerp->inputs,
                             marpaESLIFRecognizerp->inputl,
                             marpaESLIFRecognizerp->eofb,
                             rcip,
                             outputStackp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      /* A terminal matcher MAY recursively call other recognizers, reading new data, etc... : this will update current recognizer inputs, inputl and eof. */
      /* The result will be a parse tree value, at indice 0 of outputStackp */
      rcb = _marpaESLIFGrammar_meta_matcherb(marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                             symbolp->resolvedLeveli, /* Computed by grammar validator */
                                             symbolp->u.metap->marpaWrapperGrammarClonep,
                                             NULL /* terminalp */,
                                             symbolp->u.metap,
                                             marpaESLIFRecognizerp,
                                             rcip,
                                             outputStackp,
                                             exhaustedbp);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Unknown symbol type %d", symbolp->type);
      goto err;
    }

#ifndef MARPAESLIF_NTRACE
  if (rcb) {
    if (*rcip == MARPAESLIF_MATCH_OK) {
      size_t sizel;
      char  *p;

      if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, 0, &sizel)) {
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, outputStackp, 0, &p)) {
        goto err;
      }

      if (sizel > 0) {
        _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizerp, "Match for ", symbolp->descp->asciis, p, sizel);
      } else {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match for %s is the empty string", symbolp->descp->asciis);
      }
    }
  }
#endif

  goto done;

 err:
  rcb = 0;

 done:
  /*
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  */
  return rcb;
}

#ifndef MARPAESLIF_NTRACE
/*****************************************************************************/
static void _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs)
/*****************************************************************************/
{
  const static char *funcs  = "_marpaESLIF_tconvTraceCallback";
  marpaESLIF_t *marpaESLIFp = (marpaESLIF_t *) userDatavp;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s", msgs);
}
#endif

/*****************************************************************************/
static inline char *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_utf82printableascii_newp";
  char              *p;
  char              *asciis;
  size_t             asciil;
  unsigned char      c;

  asciis = _marpaESLIF_tconv_newp(marpaESLIFp, "ASCII//TRANSLIT//IGNORE", "UTF-8", descs, descl, &asciil, NULL);
  if (asciis == NULL) {
    asciis = (char *) _marpaESLIF_utf82printableascii_defaultp;
    asciil = strlen(asciis);
  } else {
    /* Remove by hand any ASCII character not truely printable.      */
    /* Only the historical ASCII table [0-127] is a portable thingy. */
    p = asciis;
    while ((c = (unsigned char) *p) != '\0') {
      if ((c >= 128) || (! isprint(c & 0xFF))) {
        *p = ' ';
      }
      p++;
    }
  }

  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return \"%s\"", asciis); */
  return asciis;
}

/*****************************************************************************/
static inline void _marpaESLIF_utf82printableascii_freev(char *asciis)
/*****************************************************************************/
{
  if ((asciis != NULL) && (asciis != _marpaESLIF_utf82printableascii_defaultp)) {
    free(asciis);
  }
}

/*****************************************************************************/
static inline char *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *desclp, char **fromEncodingsp)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_utf8_newp";
  tconv_t            tconvp      = NULL;
  char              *inbuforigp  = descs;
  size_t             inleftorigl = descl;
  char              *outbuforigp = NULL;
  size_t             outbuforigl = 0;
  tconv_option_t     tconvOption = { NULL /* charsetp */, NULL /* convertp */, NULL /* traceCallbackp */, NULL /* traceUserDatavp */ };
  char              *rcp;
  char              *inbufp;
  size_t             inleftl;
  char              *outbufp;
  size_t             outleftl;
  size_t             nconvl;

  /* ------- Our input is always a well formed UTF-8 */
#ifndef MARPAESLIF_NTRACE
  tconvOption.traceCallbackp  = _marpaESLIF_tconvTraceCallback;
  tconvOption.traceUserDatavp = marpaESLIFp;
#endif
  tconvp = tconv_open_ext(toEncodings, fromEncodings, &tconvOption);
  if (tconvp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "tconv_open failure, %s", strerror(errno));
    goto err;
  }
#ifndef MARPAESLIF_NTRACE
  tconv_trace_on(tconvp);
#endif

  /* We start with an output buffer of the same size of input buffer.                  */
  /* Whatever the destination encoding, we always reserve one byte more to place a NUL */
  /* just in case. This NUL is absolutetly harmless but is usefull if one want to look */
  /* at the variables via a debugger -;.                                               */
  /* It is more than useful when the destination encoding is ASCII: string will be NUL */
  /* terminated by default.                                                            */
  outbuforigp = (char *) malloc(descl + 1);
  if (outbuforigp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  outbuforigp[descl] = '\0';
  outbuforigl = descl;

  /* We want to translate descriptions in trace or error cases - these are short things, and */
  /* it does not really harm if we redo the whole translation stuff in case of E2BIG:        */
  /* - in trace mode it is documented that performance is awful                              */
  /* - in error mode this call will happen once                                              */

  inbufp   = inbuforigp;
  inleftl  = inleftorigl;
  outbufp  = outbuforigp;
  outleftl = outbuforigl;
  
  while (1) {

    nconvl = tconv(tconvp, (inbufp != NULL) ? &inbufp : NULL, &inleftl, &outbufp, &outleftl);

    if (nconvl == (size_t) -1) {
      char  *tmp;
      size_t deltal = outbuforigl;
      
      /* The only acceptable error is E2BIG */
      if (errno != E2BIG) {
	MARPAESLIF_ERRORF(marpaESLIFp, "tconv failure, %s", strerror(errno));
	goto err;
      }
      /* Try to alloc more */
      outbuforigl *= 2;
      /* Will this ever happen ? */
      if (outbuforigl < descl) {
	MARPAESLIF_ERROR(marpaESLIFp, "size_t flip");
	goto err;
      }
      /* Note the "+ 1" */
      tmp = realloc(outbuforigp, outbuforigl + 1);
      if (tmp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
	goto err;
      }
      outbuforigp[outbuforigl] = '\0';
      outbuforigp = tmp;
      outleftl   += deltal;
    }

    if (inbufp == NULL) {
      /* This was the last round */
      break;
    }

    if (inleftl <= 0) {
      /* Next round is the last one */
      inbufp = NULL;
    }
  }

  rcp = outbuforigp;
  if (desclp != NULL) {
    *desclp = outbuforigl;
  }
  if (fromEncodingsp != NULL) {
    if (fromEncodings != NULL) {
      *fromEncodingsp = strdup(fromEncodings);
      if (*fromEncodingsp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
	goto err;
      }
    } else {
      /* Get the guess from tconv */
      *fromEncodingsp = tconv_fromcode(tconvp);
      if (*fromEncodingsp == NULL) {
        /* Should never happen */
	MARPAESLIF_ERROR(marpaESLIFp, "tconv returned a NULL origin encoding");
        errno = EINVAL;
	goto err;
      }
      /* We do not mind if we loose the original - it is inside tconv that will be freed */
      *fromEncodingsp = strdup(*fromEncodingsp);
      if (*fromEncodingsp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
	goto err;
      }
    }
  }
  goto done;

 err:
  rcp = NULL;

 done:
  if (tconvp != NULL) {
    if (tconv_close(tconvp) != 0) {
      MARPAESLIF_ERRORF(marpaESLIFp, "tconv_close failure, %s", strerror(errno));
    }
  }

  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp); */
  return rcp;
}

/*****************************************************************************/
static inline void _marpaESLIF_tconv_freev(char *p)
/*****************************************************************************/
{
  if (p != NULL) {
    free(p);
  }
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp)
/*****************************************************************************/
{
  const static char           *funcs              = "marpaESLIFGrammar_newp";
  char                        *utf8s              = NULL;
  genericStack_t              *outputStackp       = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  size_t                       utf8l;
  marpaESLIF_readerContext_t   marpaESLIF_readerContext;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Grammar"); */

  if (marpaESLIFGrammarOptionp == NULL) {
    marpaESLIFGrammarOptionp = &marpaESLIFGrammarOption_default;
  }

  if (marpaESLIFGrammarOptionp->grammars == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null source grammar pointer");
    goto err;
  }

  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFGrammarp->marpaESLIFp   = marpaESLIFp;
  marpaESLIFGrammarp->grammarStackp = NULL;
  marpaESLIFGrammarp->grammarp      = NULL;

  GENERICSTACK_NEW(marpaESLIFGrammarp->grammarStackp);
  if (GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  /* We want to parse the incoming grammar in UTF-8 */
  utf8s = _marpaESLIF_tconv_newp(marpaESLIFp, "UTF-8", marpaESLIFGrammarOptionp->encodings, marpaESLIFGrammarOptionp->grammars, marpaESLIFGrammarOptionp->grammarl, &utf8l, NULL);
  if (utf8s == NULL) {
    goto err;
  }

  marpaESLIF_readerContext.marpaESLIFp = marpaESLIFp; /* Our reader is special and will always say eof */
  marpaESLIF_readerContext.utf8s       = utf8s;       /* Full buffer already known */
  marpaESLIF_readerContext.utf8l       = utf8l;       /* Full buffer size */
  marpaESLIF_readerContext.p           = utf8s;       /* Reader current position */

  marpaESLIFRecognizerOption.userDatavp                  = (void *) &marpaESLIF_readerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp   = _marpaESLIFReader_grammarReader;
  marpaESLIFRecognizerOption.disableThresholdb           = 1; /* No threshold warning when parsing a grammar */
  marpaESLIFRecognizerOption.exhaustedb                  = 0; /* Exhaustion is not allowed */
  marpaESLIFRecognizerOption.latmb                       = 1; /* Our internal grammar is working in latm mode */
  marpaESLIFRecognizerOption.rejectionb                  = 1; /* Our internal grammar have no rejection */
  marpaESLIFRecognizerOption.rememberb                   = 1; /* We want to remember the whole input */

  GENERICSTACK_NEW(outputStackp);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  marpaESLIFValueOption.userDatavp      = NULL; /* This is OUR internal action - we know that we will be called with a marpaESLIFValue_t pointer, not yet available */
  marpaESLIFValueOption.actionCallbackp = _marpaESLIFActionCallback;
  marpaESLIFValueOption.highRankOnlyb   = 1;
  marpaESLIFValueOption.orderByRankb    = 1;
  marpaESLIFValueOption.ambiguousb      = 0;
  marpaESLIFValueOption.nullb           = 0;
  marpaESLIFValueOption.maxParsesi      = 1;
  marpaESLIFValueOption.outputStackp    = outputStackp;

  /* Parser will automatically create marpaESLIFValuep */
  if (! _marpaESLIFGrammar_parseb(marpaESLIFp->marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, NULL /* exhaustedbp */)) {
    goto err;
  }

  /* Per def result is in marpaESLIFGrammar.outputStackp at indice 0, itself beeing a grammar instance -; */
  if (! GENERICSTACK_IS_PTR(outputStackp, 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Not a pointer in outputStackp at indice 0");
    goto err;
  }

  marpaESLIFGrammarp = GENERICSTACK_GET_PTR(outputStackp, 0);
  /* This case is vicious: we want to free everything but the first item on the outputstack... */
  GENERICSTACK_SET_NA(outputStackp, 0);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStack set failure, %s", strerror(errno));
    goto err;
  }
  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  _marpaESLIF_lexemeStack_freev(outputStackp); /* We own this stack, and know what is in here */
  _marpaESLIF_tconv_freev(utf8s);
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp); */
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
short marpaESLIFGrammar_startib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip)
/*****************************************************************************/
{
  return marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammarp, startip, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }

  if (startip != NULL) {
    *startip = grammarp->starti;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_rulesb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammarp, ruleipp, rulelp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }

  if (ruleipp != NULL) {
    *ruleipp = grammarp->ruleip;
  }
  if (rulelp != NULL) {
    *rulelp = grammarp->rulel;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_rulenameb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledecsp, size_t *ruledesclp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_rulename_by_grammarb(marpaESLIFGrammarp, rulei, ruledecsp, ruledesclp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_rulename_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledecsp, size_t *ruledesclp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;
  marpaESLIF_string_t  *stringp;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }
  rulep = _marpaESLIF_rule_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL, 0, rulei);
  if (rulep == NULL) {
    goto err;
  }
  stringp = rulep->descp;
 
  if (ruledecsp != NULL) {
    *ruledecsp = stringp->bytep;
  }
  if (ruledesclp != NULL) {
    *ruledesclp = stringp->bytel;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_ruledisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammarp, rulei, ruledisplaysp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }
  rulep = _marpaESLIF_rule_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL, 0, rulei);
  if (rulep == NULL) {
    goto err;
  }
 
  if (ruledisplaysp != NULL) {
    *ruledisplaysp = rulep->descp->asciis;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_ruleshowformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammarp, rulei, ruleshowsp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }
  rulep = _marpaESLIF_rule_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL, 0, rulei);
  if (rulep == NULL) {
    goto err;
  }
 
  if (ruleshowsp != NULL) {
    *ruleshowsp = rulep->asciishows;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_symbolnameb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldecsp, size_t *symboldesclp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_symbolname_by_grammarb(marpaESLIFGrammarp, symboli, symboldecsp, symboldesclp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_symbolname_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldecsp, size_t *symboldesclp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_symbol_t  *symbolp;
  short                 rcb;
  marpaESLIF_string_t  *stringp;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }
  symbolp = _marpaESLIF_symbol_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL, 0, symboli);
  if (symbolp == NULL) {
    goto err;
  }
  stringp = symbolp->descp;
  
  if (symboldecsp != NULL) {
    *symboldecsp = stringp->bytep;
  }
  if (symboldesclp != NULL) {
    *symboldesclp = stringp->bytel;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_symboldisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammarp, symboli, symboldisplaysp, NULL, 0, 0);
}

/*****************************************************************************/
short marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_symbol_t  *symbolp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }
  symbolp = _marpaESLIF_symbol_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL, 0, symboli);
  if (symbolp == NULL) {
    goto err;
  }
 
  if (symboldisplaysp != NULL) {
    *symboldisplaysp = symbolp->descp->asciis;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */);
}

/*****************************************************************************/
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;

  if (marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan can be done one once only");
    return 0;
  }

  marpaESLIFRecognizerp->scanb = 1;
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, initialEventsb, continuebp, exhaustedbp);
}

/*****************************************************************************/
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0 /* initialEventsb */, continuebp, exhaustedbp);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char               *funcs                             = "_marpaESLIFRecognizer_resumeb";
  marpaESLIF_t                    *marpaESLIFp                       = marpaESLIFRecognizerp->marpaESLIFp;
  genericStack_t                  *lexemeInputStackp                 = marpaESLIFRecognizerp->lexemeInputStackp;
  marpaESLIFGrammar_t             *marpaESLIFGrammarp                = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t            *grammarp                          = marpaESLIFGrammarp->grammarp;
  genericStack_t                  *symbolStackp                      = grammarp->symbolStackp;
  short                            latmb                             = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.latmb;
  short                            exhaustedb                        = 0;
  short                            completedb                        = 0;
  short                            collectb                          = 0;
  marpaESLIFValueOption_t          marpaESLIFValueOptionDiscard      = marpaESLIFValueOption_default_template;
  marpaESLIFRecognizerOption_t     marpaESLIFRecognizerOptionDiscard = marpaESLIFRecognizerp->marpaESLIFRecognizerOption; /* Things overwriten, see below */
  short                            continueb;
  size_t                           maxMatchedl;
  int                              maxPriorityi;
  short                            maxPriorityInitializedb = 0;
  genericStack_t                  *matchedStackp;
  genericStack_t                  *alternativeStackp;
  int                              alternativeOki;
  size_t                           nSymboll;
  int                             *symbolArrayp;
  size_t                           symboll;
  int                              symboli;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_matcher_value_t       rci;
  size_t                           matchedl;
  short                            rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP arrayp;
  marpaESLIF_string_t             *marpaESLIF_stringp;
  int                              valuei;
  size_t                           sizel;

#undef MARPAESLIFRECOGNIZER_COLLECT_EVENTS
#define MARPAESLIFRECOGNIZER_COLLECT_EVENTS(forceb, exhaustedbp)  do {  \
    if (forceb || ! collectb) {                                         \
      _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizerp);        \
      if (! _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizerp, NULL, NULL, NULL, exhaustedbp)) { \
        goto err;                                                       \
      }                                                                 \
      collectb = 1;                                                     \
    }                                                                   \
  } while (0)

  /* Checks */
  if (! marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan must be called first");
    return 0;
  }

  /* Initializations */
  GENERICSTACK_NEW(alternativeStackp);
  if (GENERICSTACK_ERROR(alternativeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp init failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(matchedStackp);
  if (GENERICSTACK_ERROR(matchedStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "matchedStackp init failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFRecognizerp->resumeCounterl++; /* Increment internal counter for tracing */
  marpaESLIFRecognizerOptionDiscard.disableThresholdb = 1; /* If discard, prepare the option to disable threshold */
  marpaESLIFRecognizerOptionDiscard.exhaustedb        = 1; /* ... and have the exhausted event */

  if (initialEventsb) {
    MARPAESLIFRECOGNIZER_COLLECT_EVENTS(0, &exhaustedb);
    if (marpaESLIFRecognizerp->eventArrayl > 0) {
      continueb = ! exhaustedb;
      rcb = 1;
      goto done;
    }
  }
  
  /* Ask for expected lexemes */
  if (! marpaESLIFRecognizer_expectedb(marpaESLIFRecognizerp, &nSymboll, &symbolArrayp)) {
    goto err;
  }

  if (nSymboll <= 0) {
    /* No symbol expected: this is an error unless:
       - discard mode and completion is reached, or
       - exhaustion
    */
    MARPAESLIFRECOGNIZER_COLLECT_EVENTS(0, &exhaustedb);
    if ((marpaESLIFRecognizerp->discardb && completedb) || exhaustedb) {
      continueb = 0;
      rcb = 1;
      goto done;
    }
    goto err;
  }

  /* Try to match */
  alternativeOki = 0;
  maxMatchedl = 0;
  retry:
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Trying to match %s", symbolp->descp->asciis);
  match_again:
    _marpaESLIF_lexemeStack_resetv(matchedStackp);
    if (! _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizerp, symbolp, &rci, matchedStackp, &exhaustedb)) {
      GENERICSTACK_PUSH_NA(alternativeStackp);
      if (GENERICSTACK_ERROR(alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
        goto err;
      } else {
        continue;
      }
    }
    switch (rci) {
    case MARPAESLIF_MATCH_AGAIN:
      /* We have to load more unless already at EOF */
      if (! *(marpaESLIFRecognizerp->eofbp)) {
        if (! _marpaESLIFRecognizer_readb(marpaESLIFRecognizerp)) {
          goto err;
        }
        goto match_again;
      } else {
        GENERICSTACK_PUSH_NA(alternativeStackp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
      }
      break;
    case MARPAESLIF_MATCH_FAILURE:
      /* No op */
      GENERICSTACK_PUSH_NA(alternativeStackp);
      if (GENERICSTACK_ERROR(alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
        goto err;
      }
      break;
    case MARPAESLIF_MATCH_OK:
      /* Only in this case, it is possible that matchedp contains allocated memory */

      /* When we do internal matching, we impose that the result is a genericStack ARRAY or a genericStack PTR */
      /* Per def marpaESLIFValuep is defined. */
      if (GENERICSTACK_IS_ARRAY(matchedStackp, 0)) {
        arrayp = GENERICSTACK_GET_ARRAYP(matchedStackp, 0);
        matchedl = GENERICSTACK_ARRAYP_LENGTH(arrayp);
        /* Remember this alternative */
        GENERICSTACK_PUSH_ARRAYP(alternativeStackp, arrayp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
        /* Prevent it to be freed if there is an error afterwards -; */
        GENERICSTACK_SET_NA(matchedStackp, 0);
        /* IMHO this can never fail because indice 0 is proved to exist */
      } else if (GENERICSTACK_IS_PTR(matchedStackp, 0)) {
        marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(matchedStackp, 0);
        matchedl = marpaESLIF_stringp->bytel;
        /* Remember this alternative */
        GENERICSTACK_PUSH_PTR(alternativeStackp, marpaESLIF_stringp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
        GENERICSTACK_SET_NA(matchedStackp, 0);
        /* IMHO this can never fail because indice 0 is proved to exist */
      } else {
        MARPAESLIF_ERROR(marpaESLIFp, "matchedStackp first member is not a genericStack array");
        goto err;
      }
      /* Remember max matched length */
      if (matchedl > maxMatchedl) {
        maxMatchedl = matchedl;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted maxMatchedl to %ld", (unsigned long) maxMatchedl);
      }
      /* Count */
      ++alternativeOki;
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported matcher return code %d", rci);
      goto err;
    }
  }
  
  if (alternativeOki <= 0) {
    /* If we already doing discard and discard failed, this is end of the story */
    if (marpaESLIFRecognizerp->discardb) {
      goto err;
    }
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      /* We reuse the matched Stack even if this is no-op here for us */
      _marpaESLIF_lexemeStack_resetv(matchedStackp);
      marpaESLIFValueOptionDiscard.userDatavp      = NULL; /* This is OUR internal action - we know that we will be called with a marpaESLIFValue_t pointer, not yet available */
      marpaESLIFValueOptionDiscard.actionCallbackp = _marpaESLIFActionCallback;
      marpaESLIFValueOptionDiscard.outputStackp    = matchedStackp;
      if (_marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOptionDiscard, &marpaESLIFValueOptionDiscard, 1 /* discardb */, marpaESLIFRecognizerp /* marpaESLIFRecognizerParentp */, NULL /* exhaustedbp */)) {
        /* Discard symbol got matched. Retry our our own match */
        /* This our own stack, so we know that per definition output will an genericStack's ARRAY at indice 0 of matchedStackp */
        if (! GENERICSTACK_IS_ARRAY(matchedStackp, 0)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "Bad type %s in matched stack at indice 0", _marpaESLIF_genericStack_ix_types(alternativeStackp, 0));
          goto err;
        }
        arrayp = GENERICSTACK_GET_ARRAYP(matchedStackp, 0);
        matchedl = GENERICSTACK_ARRAYP_LENGTH(arrayp);
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Advancing stream internal position by %ld bytes", (unsigned long) matchedl);
        marpaESLIFRecognizerp->inputs += matchedl;
        marpaESLIFRecognizerp->inputl -= matchedl;
        /* Well, we KNOW that no lexeme matched. This mean that alternative stack contains only N/A's: no need to free anything */
        GENERICSTACK_USED(alternativeStackp) = 0;
        /* _marpaESLIF_lexemeStack_resetv(alternativeStackp); */
        goto retry;
      }
    }

    /* Discard failure - this is an error unless lexemes were read */
    continueb = 0;
    rcb = marpaESLIFRecognizerp->haveLexemeb;
    goto done;
  }

  /* Generate the eventual pre-lexeme events */
  /* Switch to user space now */

  /* Prepare filterings */
  maxPriorityInitializedb = 0;
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);

    if (GENERICSTACK_IS_NA(alternativeStackp, (int) symboll)) {
      continue;
    }

    if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, &sizel)) {
      goto err;
    }

    if (latmb) {
      /* If latm mode is true, keep only the longests alternatives */
      if (sizel < maxMatchedl) {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp,
                                    funcs,
                                    "Alternative %s is skipped (length %ld < max length %ld)",
                                    symbolp->descp->asciis,
                                    (unsigned long) sizel,
                                    (unsigned long) maxMatchedl);
        continue;
      }      
    }

    if ((! maxPriorityInitializedb) || (symbolp->priorityi > maxPriorityi)) {
      maxPriorityi = symbolp->priorityi;
    }
  }
  
  /* Push alternatives - there are as many entries in alternativeStackp than the number of expected symbols */
  /* We have done it like this to be able to recover symbolp easily. */
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);

    if (GENERICSTACK_IS_NA(alternativeStackp, (int) symboll)) {
      continue;
    }

    if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, &sizel)) {
      goto err;
    }

    if (latmb) {
      /* If latm mode is true, keep only the longests alternatives */
      /* Already traced few lines higher */
      if (sizel < maxMatchedl) {
        continue;
      }      
    }

    if (symbolp->priorityi < maxPriorityi) {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp,
                                    funcs,
                                    "Alternative %s is out-prioritized (priority %d < max priority %d)",
                                    symbolp->descp->asciis,
                                    symbolp->priorityi,
                                    maxPriorityi);
        continue;
    }

    /* Commit in the lexeme input stack */
    if (GENERICSTACK_IS_PTR(alternativeStackp, (int) symboll)) {
      GENERICSTACK_PUSH_PTR(lexemeInputStackp, GENERICSTACK_GET_PTR(alternativeStackp, (int) symboll));
      if (GENERICSTACK_ERROR(lexemeInputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp array push failure, %s", strerror(errno));
        goto err;
      }
      /* IMHO this can never fail because indice (int) symboll is proved to exist */
      GENERICSTACK_SET_NA(alternativeStackp, (int) symboll);
    } else if (GENERICSTACK_IS_ARRAY(alternativeStackp, (int) symboll)) {
      GENERICSTACK_PUSH_ARRAY(lexemeInputStackp, GENERICSTACK_GET_ARRAY(alternativeStackp, (int) symboll));
      if (GENERICSTACK_ERROR(lexemeInputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp array push failure, %s", strerror(errno));
        goto err;
      }
      /* IMHO this can never fail because indice (int) symboll is proved to exist */
      GENERICSTACK_SET_NA(alternativeStackp, (int) symboll);
    } else {
      /* Should never happen */
      MARPAESLIF_ERRORF(marpaESLIFp, "Bad type %s in alternative stack at indice %ld", _marpaESLIF_genericStack_ix_types(alternativeStackp, (int) symboll), (unsigned long) symboll);
      goto err;
    }

    valuei = GENERICSTACK_USED(lexemeInputStackp) - 1;

    /* And to Marpa */
    if (! _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp,
                                             symbolp,
                                             valuei)) {
      goto err;
    }
  }

  /* Commit */
  if (! marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
#ifndef MARPAESLIF_NTRACE
    marpaWrapperRecognizer_progressLogb(marpaESLIFRecognizerp->marpaWrapperRecognizerp,
                                        0, -1,
                                        GENERICLOGGER_LOGLEVEL_TRACE,
                                        marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
#endif
    goto err;
  }

  /* Remember this recognizer have at least one lexeme */
  marpaESLIFRecognizerp->haveLexemeb = 1;
  
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Advancing stream internal position by %ld bytes", (unsigned long) maxMatchedl);
  marpaESLIFRecognizerp->inputs += maxMatchedl;
  marpaESLIFRecognizerp->inputl -= maxMatchedl;

  /* Collect events */
  MARPAESLIFRECOGNIZER_COLLECT_EVENTS(1, &exhaustedb);

  rcb = 1;
  continueb = ! exhaustedb;
  goto done;

 err:
  rcb = 0;

 done:
  _marpaESLIF_lexemeStack_freev(alternativeStackp);
  _marpaESLIF_lexemeStack_freev(matchedStackp);
  if (rcb) {
    if (exhaustedbp != NULL) {
      *exhaustedbp = exhaustedb;
    }
    if (continuebp != NULL) {
      *continuebp = continueb;
    }
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d (*continuebp=%d, *exhaustedbp=%d)", (int) rcb, (int) continueb, (int) exhaustedb);
  } else {
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return 0");
  }

  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, int valuei)
/*****************************************************************************/
{
  const static char    *funcs              = "marpaESLIFRecognizer_alternativeb";
  marpaESLIF_symbol_t  *symbolp;

  symbolp = _marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizerp, symbolnamecp, symbolnamel);
  if (symbolp == NULL) {
    return 0;
  }

  return _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp, symbolp, valuei);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFRecognizer_alternativeb";

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Committing alternative %s at input stack %d", symbolp->descp->asciis, valuei);
  return marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symbolp->idi, valuei, 1);
}

/*****************************************************************************/
short marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  const static char *funcs = "marpaESLIFRecognizer_completeb";

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Completing alternatives");
  return marpaWrapperRecognizer_completeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
}

/*****************************************************************************/
short marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb)
/*****************************************************************************/
{
  /* Of course, our marpaESLIFEventType_t is strictly equivalent to marpaWrapperGrammarEventType_t -; */
  return marpaWrapperRecognizer_event_onoffb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symboli, eventSeti, onoffb);
}

/*****************************************************************************/
short marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp)
/*****************************************************************************/
{
  return marpaWrapperRecognizer_expectedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, nSymbollp, symbolArraypp);
}

/*****************************************************************************/
void marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  _marpaESLIFGrammar_freev(marpaESLIFGrammarp, 0 /* onStackb */);
}

/*****************************************************************************/
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp = marpaESLIFRecognizerp->parentRecognizerp;
    
    _marpaESLIF_lexemeStack_freev(marpaESLIFRecognizerp->lexemeInputStackp);
    marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    if (marpaESLIFRecognizerp->eventArrayp != NULL) {
      free(marpaESLIFRecognizerp->eventArrayp);
    }
    if (marpaESLIFRecognizerParentp == NULL) {
      /* The "remember" and "buffer" areas are managed by the parent recognizer */
      if (marpaESLIFRecognizerp->remembers != NULL) {
        /* Then buffers is the same value than remembers */
        free(marpaESLIFRecognizerp->remembers);
      } else if (marpaESLIFRecognizerp->buffers != NULL) {
        free(marpaESLIFRecognizerp->buffers);
      }
    } else {
      /* Parent's "current" position have to be updated */
      marpaESLIFRecognizerParentp->inputs = *(marpaESLIFRecognizerp->buffersp) + marpaESLIFRecognizerp->parentDeltal;
      marpaESLIFRecognizerParentp->inputl = *(marpaESLIFRecognizerp->bufferlp) - marpaESLIFRecognizerp->parentDeltal;
    }
    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, marpaESLIFValueOptionp, exhaustedbp, NULL /* grammardescs */, 0 /* grammardescl */, 0 /* grammarleveli */);
}

/*****************************************************************************/
short marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, char *grammardescs, size_t grammardescl, int grammarleveli)
/*****************************************************************************/
{
  const static char          *funcs        = "marpaESLIFGrammar_parse_by_grammarb";
  marpaESLIF_grammar_t       *grammarp;
  short                       rcb;
  marpaESLIFGrammar_t         marpaESLIFGrammar;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammardescs, grammardescl, grammarleveli);
  if (grammarp == NULL) {
    goto err;
  }

  /* Use a local marpaESLIFGrammar and change current gramamar */
  marpaESLIFGrammar          = *marpaESLIFGrammarp;
  marpaESLIFGrammar.grammarp = grammarp;
  rcb = _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, marpaESLIFRecognizerOptionp, marpaESLIFValueOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, exhaustedbp);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFGrammarp->marpaESLIFp, funcs, "return %d", (int) rcb);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarReader";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  if (marpaESLIF_readerContextp->p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Stream is closed");
    return 0;
  }

  *inputsp = marpaESLIF_readerContextp->p;
#ifndef INTERNAL_TEST_OF_BUFFER_STREAM_MANAGEMENT
  *inputlp  = (marpaESLIF_readerContextp->p - marpaESLIF_readerContextp->utf8s) + marpaESLIF_readerContextp->utf8l;
  *eofbp    = 1;
#else
  *inputlp  = 10;
  marpaESLIF_readerContextp->p += 10;
  if ((*inputsp + *inputlp) >= (marpaESLIF_readerContextp->utf8s + marpaESLIF_readerContextp->utf8l)) {
    *inputlp  = (marpaESLIF_readerContextp->p - marpaESLIF_readerContextp->utf8s) + marpaESLIF_readerContextp->utf8l;
    *eofbp    = 1;
  } else {
    *eofbp    = 0;
  }
#endif

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (*inputsp=%p, *inputlp=%ld, *eofbp=%d)", *inputsp, (unsigned long) *inputlp, (int) *eofbp);
  return 1;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *descs, size_t descl)
/*****************************************************************************/
{
  return _marpaESLIF_symbol_findp(marpaESLIFRecognizerp->marpaESLIFp, marpaESLIFRecognizerp->marpaESLIFGrammarp->grammarp, descs, descl, -1);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIFGrammar_grammar_findp(marpaESLIFGrammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int leveli)
/*****************************************************************************/
{
  const static char    *funcs         = "_marpaESLIFGrammar_grammar_findp";
  genericStack_t       *grammarStackp = marpaESLIFGrammarp->grammarStackp;
  marpaESLIF_grammar_t *rcp           = NULL;
  marpaESLIF_string_t   desc          = { descs /* bytep */, descl /* bytel */ };
  marpaESLIF_grammar_t *grammarp;
  int                   grammari;

  if (descs != NULL) {
    /* Give precedence to grammar by name */
    for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
      if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
        /* Should never happen */
        continue;
      }
      grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
      if (_marpaESLIF_string_eqb(grammarp->descp, &desc)) {
        rcp = grammarp;
        break;
      }
    }
  } else {
    if (GENERICSTACK_IS_PTR(grammarStackp, leveli)) {
      grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, leveli);
    }
  }

  if (rcp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "No such grammar");
  }

  MARPAESLIF_TRACEF(marpaESLIFGrammarp->marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}
 
/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int rulei)
/*****************************************************************************/
{
  const static char    *funcs        = "_marpaESLIF_rule_findp";
  genericStack_t       *ruleStackp   = grammarp->ruleStackp;
  marpaESLIF_string_t   desc          = { descs /* bytep */, descl /* bytel */ };
  marpaESLIF_rule_t    *rcp          = NULL;
  marpaESLIF_rule_t    *rulep;
  int                   i;

  if (descs != NULL) {
    /* Give precedence to rule by name */
    for (i = 0; i < GENERICSTACK_USED(ruleStackp); i++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, i)) {
        /* Should never happen */
        continue;
      }
      rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, i);
      if (_marpaESLIF_string_eqb(rulep->descp, &desc)) {
        rcp = rulep;
        break;
      }
    }
  } else {
    if (GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
      rcp = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
    }
  }

  if (rcp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No such rule");
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_symbol_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int symboli)
/*****************************************************************************/
{
  const static char    *funcs        = "_marpaESLIF_symbol_findp";
  genericStack_t       *symbolStackp = grammarp->symbolStackp;
  marpaESLIF_string_t   desc         = { descs /* bytep */, descl /* bytel */ };
  marpaESLIF_symbol_t  *rcp          = NULL;
  marpaESLIF_symbol_t  *symbolp;
  int                   i;

  if (descs != NULL) {
    /* Give precedence to symbol by name */
    for (i = 0; i < GENERICSTACK_USED(symbolStackp); i++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, i)) {
        /* Should never happen */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, i);
      if (_marpaESLIF_string_eqb(symbolp->descp, &desc)) {
        rcp = symbolp;
        break;
      }
    }
  } else {
    if (GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      rcp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
    }
  }

  if (rcp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No such symbol");
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
void marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *eventArraylp, marpaESLIFEvent_t **eventArraypp)
/*****************************************************************************/
{
  if (eventArraylp != NULL) {
    *eventArraylp = marpaESLIFRecognizerp->eventArrayl;
  }
  if (eventArraypp != NULL) {
    *eventArraypp = marpaESLIFRecognizerp->eventArrayp;
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_freeCallbackv(void *userDatavp, void *p)
/*****************************************************************************/
{
  if (p != NULL) {
    free(p);
  }
}

/*****************************************************************************/
static inline void _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp->eventArrayp != NULL) {
    free(marpaESLIFRecognizerp->eventArrayp);
    marpaESLIFRecognizerp->eventArrayp = NULL;
  }
  marpaESLIFRecognizerp->eventArrayl = 0;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIFRecognizer_grammar_eventsb";
  marpaESLIF_t               *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t       *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t             *symbolStackp       = grammarp->symbolStackp;
  short                       completedb         = 0;
  short                       nulledb            = 0;
  short                       predictedb         = 0;
  short                       exhaustedb         = 0;
  size_t                      okeventl           = 0;
  marpaESLIF_symbol_t        *symbolp;
  int                         symboli;
  size_t                      grammarEventl;
  marpaWrapperGrammarEvent_t *grammarEventp;
  short                       rcb;
  marpaESLIFString_t         *stringp;
  size_t                      i;
  marpaESLIFEventType_t       type;
  marpaESLIFEvent_t           *eventArrayp;      /* For eventual realloc */

  /* Collect grammar native events and push them in the events stack */
  if (! marpaWrapperGrammar_eventb(marpaESLIFRecognizerp->discardb ? grammarp->marpaWrapperGrammarDiscardp : grammarp->marpaWrapperGrammarStartp, &grammarEventl, &grammarEventp, 0)) {
    goto err;
  }
  if (grammarEventl > 0) {
    marpaESLIFRecognizerp->eventArrayp = malloc(grammarEventl * sizeof(marpaESLIFEvent_t));
    if (marpaESLIFRecognizerp->eventArrayp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->eventArrayl = 0;

    for (i = 0; i < grammarEventl; i++) {
      symboli = grammarEventp[i].symboli;
      type    = MARPAESLIF_EVENTTYPE_NONE;
      stringp = NULL;
      if (symboli >= 0) {
        /* Look for the symbol */
        if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
          goto err;
        }
        symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
      } else {
        symbolp = NULL;
      }
      
      /* Our grammar made sure there can by only one named event per symbol */
      /* In addition, marpaWrapper guarantee there is a symbol associated to */
      /* completion, nulled or prediction events */
      switch (grammarEventp[i].eventType) {
      case MARPAWRAPPERGRAMMAR_EVENT_COMPLETED:
        type        = MARPAESLIF_EVENTTYPE_COMPLETED;
        if (symbolp != NULL) {
          stringp = symbolp->eventp;
        }
        completedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - completion event", symbolp->descp->asciis);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
        type        = MARPAESLIF_EVENTTYPE_NULLED;
        if (symbolp != NULL) {
          stringp = symbolp->eventp;
        }
        nulledb     = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - nulling event", symbolp->descp->asciis);
      case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
        type        = MARPAESLIF_EVENTTYPE_PREDICTED;
        if (symbolp != NULL) {
          stringp = symbolp->eventp;
        }
        predictedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - prediction event", symbolp->descp->asciis);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_EXHAUSTED:
        /* This is ok at EOF or if the recognizer is ok with exhaustion */
        if ((! *(marpaESLIFRecognizerp->eofbp)) && (! marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb)) {
          MARPAESLIF_ERROR(marpaESLIFp, "Grammar is exhausted but lexeme remains");
          goto err;
        }
        type        = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        exhaustedb  = 1;
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Exhausted event");
        /* symboli will be -1 as per marpaWrapper spec */
        break;
      default:
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - unsupported event type %d", symbolp->descp->asciis, grammarEventp[i].eventType);
        break;
      }

      if (type != MARPAESLIF_EVENTTYPE_EXHAUSTED) {
        marpaESLIFRecognizerp->eventArrayp[okeventl].type    = type;
        marpaESLIFRecognizerp->eventArrayp[okeventl].stringp = stringp;
        marpaESLIFRecognizerp->eventArrayl = ++okeventl;
      }
    }
  }

  if (! exhaustedb) {
    /* The event on exhaustion only occurs if needed to provide a reason to return. */
    /* If not sent by the grammar, we check explicitely ourself.                    */
    if (! marpaWrapperRecognizer_exhaustedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &exhaustedb)) {
      goto err;
    }
    if (exhaustedb) {
      /* This is ok at EOF or if the recognizer is ok with exhaustion */
      if ((! *(marpaESLIFRecognizerp->eofbp)) && (! marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb)) {
        MARPAESLIF_ERROR(marpaESLIFp, "Grammar is exhausted but lexeme remains");
        goto err;
      }
      /* Extend of create the array */
      if (okeventl <= 0) {
        marpaESLIFRecognizerp->eventArrayp = malloc(sizeof(marpaESLIFEvent_t));
        if (marpaESLIFRecognizerp->eventArrayp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        marpaESLIFRecognizerp->eventArrayp[0].type    = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        marpaESLIFRecognizerp->eventArrayp[0].stringp = NULL;
        marpaESLIFRecognizerp->eventArrayl = 1;
      } else {
        eventArrayp = realloc(marpaESLIFRecognizerp->eventArrayp, (okeventl + 1) + sizeof(marpaESLIFEvent_t));
        if (eventArrayp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        marpaESLIFRecognizerp->eventArrayp = eventArrayp;
        marpaESLIFRecognizerp->eventArrayp[okeventl].type    = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        marpaESLIFRecognizerp->eventArrayp[okeventl].stringp = NULL;
        marpaESLIFRecognizerp->eventArrayl++;
      }
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Exhausted event");
    }
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  if (completedbp != NULL) {
    *completedbp = completedb;
  }
  if (nulledbp != NULL) {
    *nulledbp = nulledb;
  }
  if (predictedbp != NULL) {
    *predictedbp = predictedb;
  }
  if (exhaustedbp != NULL) {
    *exhaustedbp = exhaustedb;
  }
  /* MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d (*exhaustedbp=%d)", (int) rcb, (int) exhaustedb); */
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp)
/*****************************************************************************/
{
  const static char             *funcs                 = "_marpaESLIFRecognizer_newp";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = NULL;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;

#ifndef MARPAESLIF_NTRACE
  /*
  if (marpaESLIFRecognizerParentp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerParentp, funcs, "Building %s Recognizer", discardb ? ":discard" : ":start");
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Building %s Recognizer", discardb ? ":discard" : ":start");
  }
  */
#endif
  
  if (marpaESLIFRecognizerOptionp == NULL) {
    marpaESLIFRecognizerOptionp = &marpaESLIFRecognizerOption_default;
  }

  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null grammar pointer");
    goto err;
  }

  /* We request a stream reader callback */
  if (marpaESLIFRecognizerOptionp->marpaESLIFReaderCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null reader callback");
    goto err;
  }
  
  marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) malloc(sizeof(marpaESLIFRecognizer_t));
  if (marpaESLIFRecognizerp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFRecognizerp->marpaESLIFp                = marpaESLIFp;
  marpaESLIFRecognizerp->marpaESLIFGrammarp         = marpaESLIFGrammarp;
  marpaESLIFRecognizerp->marpaESLIFRecognizerOption = *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = NULL;
  marpaESLIFRecognizerp->lexemeInputStackp          = NULL;
  marpaESLIFRecognizerp->eventArrayp                = NULL;
  marpaESLIFRecognizerp->eventArrayl                = 0;
  marpaESLIFRecognizerp->parentRecognizerp          = marpaESLIFRecognizerParentp;
  marpaESLIFRecognizerp->resumeCounterl             = 0;
  marpaESLIFRecognizerp->buffers                    = NULL;
  marpaESLIFRecognizerp->bufferl                    = 0;
  marpaESLIFRecognizerp->eofb                       = 0;
  marpaESLIFRecognizerp->remembers                  = NULL;
  marpaESLIFRecognizerp->rememberl                  = 0;
  /* If this is a parent recognizer get its stream information */
  if (marpaESLIFRecognizerParentp != NULL) {
    marpaESLIFRecognizerp->buffersp                   = marpaESLIFRecognizerParentp->buffersp;
    marpaESLIFRecognizerp->bufferlp                   = marpaESLIFRecognizerParentp->bufferlp;
    marpaESLIFRecognizerp->eofbp                      = marpaESLIFRecognizerParentp->eofbp;
    marpaESLIFRecognizerp->remembersp                 = marpaESLIFRecognizerParentp->remembersp;
    marpaESLIFRecognizerp->rememberlp                 = marpaESLIFRecognizerParentp->rememberlp;
    marpaESLIFRecognizerp->parentDeltal               = marpaESLIFRecognizerParentp->inputs - *(marpaESLIFRecognizerParentp->buffersp);
    /* New recognizer is starting at the parent's inputs pointer */
    marpaESLIFRecognizerp->inputs                     = marpaESLIFRecognizerParentp->inputs;
    marpaESLIFRecognizerp->inputl                     = marpaESLIFRecognizerParentp->inputl;
  } else {
    marpaESLIFRecognizerp->buffersp                   = &(marpaESLIFRecognizerp->buffers);
    marpaESLIFRecognizerp->bufferlp                   = &(marpaESLIFRecognizerp->bufferl);
    marpaESLIFRecognizerp->eofbp                      = &(marpaESLIFRecognizerp->eofb);
    marpaESLIFRecognizerp->remembersp                 = &(marpaESLIFRecognizerp->remembers);
    marpaESLIFRecognizerp->rememberlp                 = &(marpaESLIFRecognizerp->rememberl);
    marpaESLIFRecognizerp->parentDeltal               = 0;
    /* New recognizer is starting nowhere for the moment - it will ask for more data, c.f. recognizer's read() */
    marpaESLIFRecognizerp->inputs                     = NULL;
    marpaESLIFRecognizerp->inputl                     = 0;
  }
  marpaESLIFRecognizerp->scanb                      = 0;
  marpaESLIFRecognizerp->discardb                   = discardb;
  marpaESLIFRecognizerp->haveLexemeb                = 0;

  marpaWrapperRecognizerOption.genericLoggerp       = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb    = marpaESLIFRecognizerOptionp->disableThresholdb;

  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = marpaWrapperRecognizer_newp(discardb ?
                                                                                  marpaESLIFGrammarp->grammarp->marpaWrapperGrammarDiscardp
                                                                                  :
                                                                                  marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp
                                                                                  , &marpaWrapperRecognizerOption);
  if (marpaESLIFRecognizerp->marpaWrapperRecognizerp == NULL) {
    goto err;
  }
  GENERICSTACK_NEW(marpaESLIFRecognizerp->lexemeInputStackp);
  if (marpaESLIFRecognizerp->lexemeInputStackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  /* Marpa does not like the indice 0 */
  GENERICSTACK_PUSH_NA(marpaESLIFRecognizerp->lexemeInputStackp);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->lexemeInputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp push failure, %s", strerror(errno));
    goto err;
  }

  goto done;

 err:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

 done:
#ifndef MARPAESLIF_NTRACE
  /*
  if (marpaESLIFRecognizerParentp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerParentp, funcs, "return %p", marpaESLIFRecognizerp);
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFRecognizerp);
  }
  */
#endif
  return marpaESLIFRecognizerp;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char      *funcs                 = "_marpaESLIFGrammar_parseb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t marpaESLIFValueOption = (marpaESLIFValueOptionp != NULL) ? *marpaESLIFValueOptionp : marpaESLIFValueOption_default_template;
  marpaESLIFValue_t      *marpaESLIFValuep      = NULL;
  short                   exhaustedb;
  short                   continueb;
  short                   rcb;

  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, discardb, marpaESLIFRecognizerParentp);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
      goto err;
    }
  }

  /* Force unambiguity */
  marpaESLIFValueOption.ambiguousb = 0;
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    goto err;
  }
  /* No loop because we ask for a non-ambigous parse tree value */
  if (! marpaESLIFValue_valueb(marpaESLIFValuep)) {
    goto err;
  }

  rcb = 1;
  if (exhaustedbp != NULL) {
    *exhaustedbp = exhaustedb;
  }
  goto done;
  
 err:
  rcb = 0;

 done:
  marpaESLIFValue_freev(marpaESLIFValuep);
#ifndef MARPAESLIF_NTRACE
  if (marpaESLIFRecognizerp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  }
#endif
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  _marpaESLIF_stringGenerator_t *contextp = (_marpaESLIF_stringGenerator_t *) userDatavp;
  char                         *tmps;
  size_t                        l;

  if (contextp->s == NULL) {
    /* First time */
    contextp->l = strlen(msgs) + 1;
    contextp->s = strdup(msgs);
    if (contextp->s == NULL) {
      contextp->l = 0;
      MARPAESLIF_ERRORF(contextp->marpaESLIFp, "strdup failure, %s", strerror(errno));
      /* contextp->okb = 0; */ /* We initialized it with a false value -; */
    } else {
      contextp->okb = 1;
    }
  } else if (contextp->okb) {
    /* Only if previous round was ok */
    l = contextp->l;
    contextp->l = strlen(contextp->s) + strlen(msgs) + 1;
    tmps = (char *) realloc(contextp->s, contextp->l);
    if (tmps != NULL) {
      strcat(tmps, msgs);
      contextp->s = tmps;
    } else {
      contextp->okb = 0;
      contextp->l = 0;
      free(contextp->s);
      contextp->s = NULL;
      MARPAESLIF_ERRORF(contextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
    }
  }
}

#ifndef MARPAESLIF_NTRACE
/* C.f. http://grapsus.net/blog/post/Hexadecimal-dump-in-C */
#ifndef MARPAESLIF_HEXDUMP_COLS
#define MARPAESLIF_HEXDUMP_COLS 16
#endif
/*****************************************************************************/
static inline void _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *headers, char *asciidescs, const char *p, size_t lengthl)
/*****************************************************************************/
{
  const static char             *funcs       = "_marpaESLIFRecognizer_hexdumpv";
  _marpaESLIF_stringGenerator_t _marpaESLIF_stringGenerator;
  genericLogger_t               *genericLoggerp;
  size_t  i;
  size_t  j;

  _marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  _marpaESLIF_stringGenerator.s           = NULL;
  _marpaESLIF_stringGenerator.l           = 0;
  _marpaESLIF_stringGenerator.okb         = 0;

  genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &_marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    return;
  }
  
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------");
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s%s, length %ld", headers, asciidescs, (unsigned long) lengthl);
  for (i = 0; i < lengthl + ((lengthl % MARPAESLIF_HEXDUMP_COLS) ? (MARPAESLIF_HEXDUMP_COLS - lengthl % MARPAESLIF_HEXDUMP_COLS) : 0); i++) {
    /* print offset */
    if (i % MARPAESLIF_HEXDUMP_COLS == 0) {
      GENERICLOGGER_TRACEF(genericLoggerp, "0x%06x: ", i);
    }
    /* print hex data */
    if (i < lengthl) {
      GENERICLOGGER_TRACEF(genericLoggerp, "%02x ", 0xFF & ((char*)p)[i]);
    } else { /* end of block, just aligning for ASCII dump */
      GENERICLOGGER_TRACE(genericLoggerp, "   ");
    }
    /* print ASCII dump */
    if (i % MARPAESLIF_HEXDUMP_COLS == (MARPAESLIF_HEXDUMP_COLS - 1)) {
      for (j = i - (MARPAESLIF_HEXDUMP_COLS - 1); j <= i; j++) {
        if (j >= lengthl) { /* end of block, not really printing */
          GENERICLOGGER_TRACE(genericLoggerp, " ");
        }
        else if (isprint(0xFF & ((char*)p)[j])) { /* printable char */
          GENERICLOGGER_TRACEF(genericLoggerp, "%c", 0xFF & ((char*)p)[j]);
        }
        else { /* other char */
          GENERICLOGGER_TRACE(genericLoggerp, ".");
        }
      }
      if (_marpaESLIF_stringGenerator.okb) {
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, _marpaESLIF_stringGenerator.s);
        free(_marpaESLIF_stringGenerator.s);
        _marpaESLIF_stringGenerator.s = NULL;
        _marpaESLIF_stringGenerator.okb = 0;
      }
    }
  }

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------");
  GENERICLOGGER_FREE(genericLoggerp);
}
#endif

/*****************************************************************************/
marpaESLIFValue_t *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
{
  const static char        *funcs             = "marpaESLIFValue_newp";
  marpaESLIF_t             *marpaESLIFp       = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFValue_t        *marpaESLIFValuep  = NULL;
  marpaWrapperValue_t      *marpaWrapperValuep = NULL;
  marpaWrapperValueOption_t marpaWrapperValueOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Value"); */

  if (marpaESLIFValueOptionp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Value option structure must not be NULL");
    goto err;
  }
  if (marpaESLIFValueOptionp->outputStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Value option structure's outputStackp must not be NULL");
    goto err;
  }

  marpaESLIFValuep = (marpaESLIFValue_t *) malloc(sizeof(marpaESLIFValue_t));
  if (marpaESLIFValuep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFValuep->marpaESLIFp            = marpaESLIFp;
  marpaESLIFValuep->marpaESLIFRecognizerp  = marpaESLIFRecognizerp;
  marpaESLIFValuep->marpaESLIFValueOption  = *marpaESLIFValueOptionp;
  marpaESLIFValuep->marpaWrapperValuep     = NULL;
  marpaESLIFValuep->context.ruleIdi        = -1;

  marpaWrapperValueOption.genericLoggerp = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperValueOption.highRankOnlyb  = marpaESLIFValueOptionp->highRankOnlyb;
  marpaWrapperValueOption.orderByRankb   = marpaESLIFValueOptionp->orderByRankb;
  marpaWrapperValueOption.ambiguousb     = marpaESLIFValueOptionp->ambiguousb;
  marpaWrapperValueOption.nullb          = marpaESLIFValueOptionp->nullb;
  marpaWrapperValueOption.maxParsesi     = marpaESLIFValueOptionp->maxParsesi;
  marpaWrapperValuep = marpaWrapperValue_newp(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &marpaWrapperValueOption);
  if (marpaWrapperValuep == NULL) {
#ifndef MARPAESLIF_NTRACE
    marpaWrapperRecognizer_progressLogb(marpaESLIFRecognizerp->marpaWrapperRecognizerp,
                                        0, -1,
                                        GENERICLOGGER_LOGLEVEL_TRACE,
                                        marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
#endif
    goto err;
  }

  marpaESLIFValuep->marpaWrapperValuep = marpaWrapperValuep;
  goto done;

 err:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFValuep = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFValuep); */
  return marpaESLIFValuep;
}

/*****************************************************************************/
short marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaWrapperValue_valueb(marpaESLIFValuep->marpaWrapperValuep,
                                  marpaESLIFValuep, /* Our value context */
                                  _marpaESLIFValueRuleCallback,
                                  _marpaESLIFValueSymbolCallback,
                                  _marpaESLIFValueNullingCallback);
}

/*****************************************************************************/
void marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  if (marpaESLIFValuep != NULL) {
    marpaWrapperValue_t *marpaWrapperValuep = marpaESLIFValuep->marpaWrapperValuep;
    if (marpaWrapperValuep != NULL) {
      marpaWrapperValue_freev(marpaWrapperValuep);
    }
    free(marpaESLIFValuep);
  }
}

/*****************************************************************************/
static short _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "_marpaESLIFValueRuleCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *ruleStackp            = grammarp->ruleStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  size_t                          totalSizel = 0;
  size_t                          previousTotalSizel = 0;
  size_t                          sizel;
  marpaESLIF_rule_t              *rulep;
  int                             i;
  short                           rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  char                           *p;
  char                           *q;

  /* First thing to do -; */
  GENERICSTACK_ARRAY_PTR(array) = NULL;
  GENERICSTACK_ARRAY_LENGTH(array) = 0;

  if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such rule No %d", rulei);
    goto err;
  }
  rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "[%d..%d] -> [%d] %s", arg0i, argni, resulti, rulep->asciishows);

  /* Collect final size */
  for (i = arg0i; i <= argni; i++) {
    if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, i, &sizel)) {
      goto err;
    }
    /* Who knows */
    previousTotalSizel = totalSizel;
    totalSizel += sizel;
    if (totalSizel < previousTotalSizel) {
      MARPAESLIF_ERROR(marpaESLIFp, "size_t turnaround");
      goto err;
    }
  }

  if (totalSizel > 0) {
    q = GENERICSTACK_ARRAY_PTR(array) = malloc(totalSizel);
    if (GENERICSTACK_ARRAY_PTR(array) == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    for (i = arg0i; i <= argni; i++) {
      if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, i, &sizel)) {
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, outputStackp, i, &p)) {
        goto err;
      }
      if ((sizel > 0) && (p == NULL)) {
        MARPAESLIF_ERROR(marpaESLIFp, "sizel > 0 but p is NULL");
        goto err;
      }
      if (p != NULL) {
        memcpy(q, p, sizel);
        GENERICSTACK_ARRAY_LENGTH(array) += sizel;
        q += sizel;
      }
    }
  }

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIFRecognizer_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_ARRAY(outputStackp, array, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizerp, "Value result ", rulep->descp->asciis, GENERICSTACK_ARRAY_PTR(array), GENERICSTACK_ARRAY_LENGTH(array));
  goto done;

 err:
  if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFValueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "_marpaESLIFValueSymbolCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *symbolStackp          = grammarp->symbolStackp;
  genericStack_t                 *inputStackp           = marpaESLIFRecognizerp->lexemeInputStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  size_t                          sizel;
  marpaESLIF_symbol_t            *symbolp;
  short                           rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  char                           *p;

  /* First thing to do -; */
  GENERICSTACK_ARRAY_PTR(array) = NULL;
  GENERICSTACK_ARRAY_LENGTH(array) = 0;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "[%d] -> [%d] %s", argi, resulti, symbolp->descp->asciis);

  /* Get size */
  if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, inputStackp, argi, &sizel)) {
    goto err;
  }

  if (sizel > 0) {
    GENERICSTACK_ARRAY_PTR(array) = malloc(sizel);
    if (GENERICSTACK_ARRAY_PTR(array) == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, inputStackp, argi, &p)) {
      goto err;
    }
    if ((sizel > 0) && (p == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "sizel > 0 but p is NULL");
      goto err;
    }
    memcpy(GENERICSTACK_ARRAY_PTR(array), p, sizel);
    GENERICSTACK_ARRAY_LENGTH(array) = sizel;
  }

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIFRecognizer_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_ARRAY(outputStackp, array, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "_marpaESLIFValueNullingCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *symbolStackp          = grammarp->symbolStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  marpaESLIF_symbol_t            *symbolp;
  short                           rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "%s -> [%d]", symbolp->descp->asciis, resulti);

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIFRecognizer_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(outputStackp, NULL, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
}


/*****************************************************************************/
int marpaESLIFValue_rulei(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaESLIFValuep->context.ruleIdi;
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFGrammarp;
}

/*****************************************************************************/
static short _marpaESLIFActionCallback(void *userDatavp, char *names, size_t namel, char *inputs, size_t inputl, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  const static char *funcs            = "_marpaESLIFActionCallback";
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;

  /* This is our stuff, we know what we are doing */
}

/*****************************************************************************/
static inline void _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp != NULL) {
    if (marpaESLIFGrammarp->grammarStackp != NULL) {
      while (GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp) > 0) {
	if (GENERICSTACK_IS_PTR(marpaESLIFGrammarp->grammarStackp, GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp) - 1)) {
	  marpaESLIF_grammar_t *grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_POP_PTR(marpaESLIFGrammarp->grammarStackp);
	  _marpaESLIF_grammar_freev(grammarp);
	} else {
	  GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp)--;
	}
      }
      GENERICSTACK_FREE(marpaESLIFGrammarp->grammarStackp);
    }
    if (! onStackb) {
      free(marpaESLIFGrammarp);
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, size_t *sizelp)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIFRecognizer_lexemeStack_ix_sizeb";
  marpaESLIF_string_t              *marpaESLIF_stringp;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  size_t                            sizel;
  short                             rcb;

  if (GENERICSTACK_IS_PTR(lexemeStackp, ix)) {
    marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(lexemeStackp, ix);
    if (marpaESLIF_stringp == NULL) {
      /* Nulling */
      sizel = 0;
    } else {
      sizel = marpaESLIF_stringp->bytel;
      if (sizel == 0) {
        /* It is illegal to have a marpaESLIF_string with a zero size */
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad string size in lexeme stack at indice %d", ix);
        goto err;
      }
    }
  } else if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, ix);
    sizel = GENERICSTACK_ARRAYP_LENGTH(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_ix_types(lexemeStackp, ix), ix);
    goto err;
  }

  rcb = 1;
  *sizelp = sizel;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, char **pp)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIFRecognizer_lexemeStack_ix_p";
  marpaESLIF_string_t              *marpaESLIF_stringp;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  short                             rcb;
  char                             *p;

  if (GENERICSTACK_IS_PTR(lexemeStackp, ix)) {
    marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(lexemeStackp, ix);
    if (marpaESLIF_stringp == NULL) {
      /* Nulling */
      p = NULL;
    } else {
      p = marpaESLIF_stringp->bytep;
      if (p == NULL) {
        /* It is illegal to have a marpaESLIF_string with a NULL pointer */
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad string pointer in lexeme stack at indice %d", ix);
        goto err;
      }
    }
  } else if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, ix);
    p = GENERICSTACK_ARRAYP_PTR(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_ix_types(lexemeStackp, ix), ix);
    goto err;
  }

  rcb = 1;
  *pp = p;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline const char *_marpaESLIF_genericStack_ix_types(genericStack_t *stackp, int ix)
/*****************************************************************************/
{
  const char *s;

  switch (GENERICSTACKITEMTYPE(stackp, ix)) {
  case GENERICSTACKITEMTYPE_NA:
    s = GENERICSTACKITEMTYPE_NA_STRING;
    break;
  case GENERICSTACKITEMTYPE_CHAR:
    s = GENERICSTACKITEMTYPE_CHAR_STRING;
    break;
  case GENERICSTACKITEMTYPE_SHORT:
    s = GENERICSTACKITEMTYPE_SHORT_STRING;
    break;
  case GENERICSTACKITEMTYPE_INT:
    s = GENERICSTACKITEMTYPE_INT_STRING;
    break;
  case GENERICSTACKITEMTYPE_LONG:
    s = GENERICSTACKITEMTYPE_LONG_STRING;
    break;
  case GENERICSTACKITEMTYPE_FLOAT:
    s = GENERICSTACKITEMTYPE_FLOAT_STRING;
    break;
  case GENERICSTACKITEMTYPE_DOUBLE:
    s = GENERICSTACKITEMTYPE_DOUBLE_STRING;
    break;
  case GENERICSTACKITEMTYPE_PTR:
    s = GENERICSTACKITEMTYPE_PTR_STRING;
    break;
  case GENERICSTACKITEMTYPE_ARRAY:
    s = GENERICSTACKITEMTYPE_ARRAY_STRING;
    break;
  default:
    s = GENERICSTACKITEMTYPE_UNKNOWN_STRING;
    break;
  }

  return s;
}

/*****************************************************************************/
static char *_marpaESLIFGrammar_symbolDescriptionCallback(void *userDatavp, int symboli)
/*****************************************************************************/
{
  const static char    *funcs              = "_marpaESLIFGrammar_symbolDescriptionCallback";
  marpaESLIFGrammar_t  *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_grammar_t *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t       *symbolStackp       = grammarp->symbolStackp;
  marpaESLIF_symbol_t  *symbolp;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    return NULL;
  }
  symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);

  return symbolp->descp->asciis;
}

/*****************************************************************************/
 static short _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/*****************************************************************************/
{
  const static char         *funcs                    = "_marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent";
  marpaESLIF_cloneContext_t *marpaESLIF_cloneContextp = (marpaESLIF_cloneContext_t *) userDatavp;
  marpaESLIF_grammar_t      *grammarp                 = marpaESLIF_cloneContextp->grammarp;
  genericStack_t            *symbolStackp             = grammarp->symbolStackp;
  marpaESLIF_symbol_t       *symbolp;
  short                     rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Cannot find symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);

  /* Consistenty check */
  if (symbolp->idi != symboli) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Clone symbol callback for symbol No %d while we have %d !?", symboli, symbolp->idi);
    goto err;
  }

  if (symbolp->discardb) {
    if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
      MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Setting event set for symbol No %d (%s) at grammar level %d (%s) on completion only", symbolp->idi, symbolp->descp->asciis, grammarp->leveli, grammarp->descp->asciis);
      marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION;
    }
  } else {
    if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE) {
      MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Resetting event set for symbol No %d (%s) at grammar level %d (%s)", symbolp->idi, symbolp->descp->asciis, grammarp->leveli, grammarp->descp->asciis);
      marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
/*
  In the recognizer:
  - buffers is containing unparsed data, and its size can vary at any time
  - inputs is always a pointer within buffers
  - remembers is when we ask the recognizer to remember everything
  
  Obviously, when remembers is in use, it is enough to always set buffers to remembers
  (and take care of that when freeing the recognizer -;)

  Remember the semantics: from our point of view, reader is reading NEW data. We always append.
*/
{
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption = marpaESLIFRecognizerp->marpaESLIFRecognizerOption;
  char                         *inputs = NULL;
  size_t                        inputl = 0;
  short                         eofb   = 0;
  short                         rcb;

  if (! marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp(marpaESLIFRecognizerOption.userDatavp, &inputs, &inputl, &eofb)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "reader failure");
    goto err;
  }

  if ((inputs != NULL) && (inputl > 0)) {
    /* Some new data is coming - remember delta before doing anything */
    size_t deltal = marpaESLIFRecognizerp->inputs - *(marpaESLIFRecognizerp->buffersp);

    if (marpaESLIFRecognizerOption.rememberb) {
      char  *remembers = *(marpaESLIFRecognizerp->remembersp);
      size_t rememberl = *(marpaESLIFRecognizerp->rememberlp);

      if (remembers == NULL) {
        /* First time we remember the input */
        remembers = (char *) malloc(inputl);
        if (remembers == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
      } else {
        char *tmps = realloc(remembers, rememberl + inputl);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        remembers = tmps;
      }
      memcpy(remembers + rememberl, inputs, inputl);
      rememberl += inputl;

      (*marpaESLIFRecognizerp->remembersp) = remembers;
      (*marpaESLIFRecognizerp->rememberlp) = rememberl;
      /* In remember mode, it is ok to set buffer to the remember mode - there is no need for bufferisation since we remember everything -; */
      /* Nevertheless this has an impact on the marpaESLIFRecognizer_freev() method. */
      (*marpaESLIFRecognizerp->buffersp) = remembers;
      (*marpaESLIFRecognizerp->bufferlp) = rememberl;

    } else {
      char  *buffers = *(marpaESLIFRecognizerp->buffersp);
      size_t bufferl = *(marpaESLIFRecognizerp->bufferlp);

      if (marpaESLIFRecognizerp->parentRecognizerp == NULL) {
        /* When we are not in remember mode, buffer management is different: we can crunch data at any time */
        /* Unless we are a child reognizer: our parent is stucked on its {inputs,inputl} tuple. */
        /* We decide abritrarily than when inputl is < half of buffer size, the later can be reduced */
        size_t bufferl2 = (size_t) (bufferl / (size_t) 2);

        if (marpaESLIFRecognizerp->inputl < bufferl2) {
          size_t newbufferl = bufferl - marpaESLIFRecognizerp->inputl;
          char  *tmps;

          /* This mean that buffers is not NULL and its size is at least two bytes btw */
          memmove(buffers, buffers + marpaESLIFRecognizerp->inputl, newbufferl);
          /* Try to realloc */
          tmps = realloc(buffers, newbufferl);
          if (tmps == NULL) {
            /* We could have continue, this is not truely fatal - but we are in a bad shape anyway -; */
            MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "realloc failure, %s", strerror(errno));
            goto err;
          }
          *(marpaESLIFRecognizerp->buffersp) = tmps;
          *(marpaESLIFRecognizerp->bufferlp) = newbufferl;
          marpaESLIFRecognizerp->inputs  = *(marpaESLIFRecognizerp->buffersp);
          /* inputl does not change yet - we just crunched data */
          buffers = tmps;
          bufferl = newbufferl;
        }
      }

      /* Append data, as we do in the remembering mode */
      if (buffers == NULL) {
        /* First time we put in the buffer */
        buffers = (char *) malloc(inputl);
        if (buffers == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
      } else {
        char *tmps = realloc(buffers, bufferl + inputl);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        buffers = tmps;
      }
      memcpy(buffers + bufferl, inputs, inputl);
      bufferl += inputl;

      (*marpaESLIFRecognizerp->buffersp) = buffers;
      (*marpaESLIFRecognizerp->bufferlp) = bufferl;

    }

    /* In any case, update current pointer */
    marpaESLIFRecognizerp->inputs = (*marpaESLIFRecognizerp->buffersp) + deltal;
    marpaESLIFRecognizerp->inputl = (*marpaESLIFRecognizerp->bufferlp) - deltal;

    /* Please see the free method for the impact on parent's current pointer in input   */
    /* This need to be done once only, at return, this is why it is done at free level. */
    /* Note that when we create a grand child we strip off ALL events, so the user can */
    /* never got control back until we are finished. I.e. until all the free methods of */
    /* all the children are executed -; */
  }

  rcb = 1;
  (*marpaESLIFRecognizerp->eofbp) = eofb;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIF_rule_createshowv(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep, char *asciishows, size_t *asciishowlp)
/*****************************************************************************/
{
  genericStack_t      *rhsStackp       = rulep->rhsStackp;
  genericStack_t      *maskStackp      = rulep->maskStackp;
  genericStack_t      *exceptionStackp = rulep->exceptionStackp;
  marpaESLIF_symbol_t *symbolp;
  short                maskb;
  size_t               asciishowl;
  int                  exceptioni;
  size_t               exceptionl;
  int                  rhsi;
  char                 tmps[1024];

  /* Calculate the size needed to show the rule in ASCII form */
  asciishowl = strlen(rulep->lhsp->descp->asciis); /* "LHS" */
  if (asciishows != NULL) {
    strcpy(asciishows, rulep->lhsp->descp->asciis);
  }
  asciishowl += 4;                              /* " ::=" */
  if (asciishows != NULL) {
    strcat(asciishows, " ::=");
  }
  for (rhsi = 0; rhsi < GENERICSTACK_USED(rhsStackp); rhsi++) {
    if (! GENERICSTACK_IS_PTR(rhsStackp, rhsi)) {
      continue;
    }
    if (! GENERICSTACK_IS_SHORT(maskStackp, rhsi)) {
      continue;
    }
    symbolp = GENERICSTACK_GET_PTR(rhsStackp, rhsi);
    maskb   = GENERICSTACK_GET_SHORT(maskStackp, rhsi);

    asciishowl++;                              /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    if (maskb) {
      asciishowl++;                         /* Surrounded by ( */
      if (asciishows != NULL) {
        strcat(asciishows, "(");
      }
    }
    asciishowl += strlen(symbolp->descp->asciis); /* RHS description (ASCII) */
    if (asciishows != NULL) {
      strcat(asciishows, symbolp->descp->asciis);
    }
    if (maskb) {
      asciishowl++;                         /* Surrounded by ) */
      if (asciishows != NULL) {
        strcat(asciishows, ")");
      }
    }
  }
  exceptionl = 0;
  for (exceptioni = 0; exceptioni < GENERICSTACK_USED(exceptionStackp); exceptioni++) {
    if (! GENERICSTACK_IS_PTR(exceptionStackp, exceptioni)) {
      continue;
    }
    exceptionl++;
  }
  if (rulep->sequenceb) {
    asciishowl++;                                 /* '*' or '+' */
    if (asciishows != NULL) {
      strcat(asciishows, (rulep->minimumi == 0) ? "*" : "+");
    }
    if (rulep->separatorp != NULL) {
      asciishowl++;                                        /* space */
      if (asciishows != NULL) {
        strcat(asciishows, " ");
      }
      asciishowl += strlen("separator => ");               /* "separator =>" */
      if (asciishows != NULL) {
        strcat(asciishows, "separator => ");
      }
      asciishowl += strlen(rulep->separatorp->descp->asciis); /* Separator description (ASCII) */
      if (asciishows != NULL) {
        strcat(asciishows, rulep->separatorp->descp->asciis);
      }
    }
    if (rulep->properb) {
      asciishowl++;                                 /* space */
      if (asciishows != NULL) {
        strcat(asciishows, " ");
      }
      asciishowl += strlen("proper => 1");          /* "proper => 1" */
      if (asciishows != NULL) {
        strcat(asciishows, "proper => 1");
      }
    }
  }
  if (exceptionl > 0) {
    asciishowl++;                            /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    asciishowl++;                            /* Surround exception by ( */
    if (asciishows != NULL) {
      strcat(asciishows, "(");
    }
    exceptionl = 0;
    for (exceptioni = 0; exceptioni < GENERICSTACK_USED(exceptionStackp); exceptioni++) {
      if (! GENERICSTACK_IS_PTR(exceptionStackp, exceptioni)) {
        continue;
      }
      symbolp = GENERICSTACK_GET_PTR(exceptionStackp, exceptioni);
      if (exceptionl > 0) {
        asciishowl++;                            /* space */
        if (asciishows != NULL) {
          strcat(asciishows, " ");
        }
      }
      asciishowl += strlen(symbolp->descp->asciis); /* Exception description (ASCII) */
      if (asciishows != NULL) {
        strcat(asciishows, symbolp->descp->asciis);
      }
    }
    asciishowl++;                             /* Surround exception by ) */
    if (asciishows != NULL) {
      strcat(asciishows, ")");
    }
  }
  if (rulep->ranki != 0) {
    asciishowl++;                            /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    asciishowl += strlen("rank => ");        /* "rank => " */
    if (asciishows != NULL) {
      strcat(asciishows, "rank => ");
    }
    /* Will an "int" ever have more than 1023 digits ? */
    sprintf(tmps, "%d", rulep->ranki);
    asciishowl += strlen(tmps);
    if (asciishows != NULL) {
      strcat(asciishows, tmps);
    }
  }
  if (rulep->nullRanksHighb) {
    asciishowl++;                                 /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    asciishowl += strlen("null-ranking => high"); /* "null-ranking => high */
    if (asciishows != NULL) {
      strcat(asciishows, "null-ranking => high");
    }
  }
  asciishowl++; /* NUL byte */

  if (asciishowlp != NULL) {
    *asciishowlp = asciishowl;
  }
}
