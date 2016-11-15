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
#include "marpaESLIF/internal/bootstrap_actions.h"

#ifndef MARPAESLIF_INITIAL_REPLACEMENT_LENGTH
#define MARPAESLIF_INITIAL_REPLACEMENT_LENGTH 8096  /* Subjective number */
#endif

/* This is very internal: I use the genericLogger to generate strings */
typedef struct marpaESLIF_stringGenerator {
  marpaESLIF_t *marpaESLIFp;
  char         *s;
  size_t        l;
  short         okb;
} marpaESLIF_stringGenerator_t;

static const char *FILENAMES                           = "marpaESLIF.c"; /* For logging */

static const char *GENERICSTACKITEMTYPE_NA_STRING      = "NA";
static const char *GENERICSTACKITEMTYPE_CHAR_STRING    = "CHAR";
static const char *GENERICSTACKITEMTYPE_SHORT_STRING   = "SHORT";
static const char *GENERICSTACKITEMTYPE_INT_STRING     = "INT";
static const char *GENERICSTACKITEMTYPE_LONG_STRING    = "LONG";
static const char *GENERICSTACKITEMTYPE_FLOAT_STRING   = "FLOAT";
static const char *GENERICSTACKITEMTYPE_DOUBLE_STRING  = "DOUBLE";
static const char *GENERICSTACKITEMTYPE_PTR_STRING     = "PTR";
static const char *GENERICSTACKITEMTYPE_ARRAY_STRING   = "ARRAY";
static const char *GENERICSTACKITEMTYPE_UNKNOWN_STRING = "UNKNOWN";

const marpaESLIF_uint32_t pcre2_option_binary_default  = PCRE2_NOTEMPTY;
const marpaESLIF_uint32_t pcre2_option_char_default    = PCRE2_NOTEMPTY|PCRE2_NO_UTF_CHECK;
const marpaESLIF_uint32_t pcre2_option_partial_default = PCRE2_NOTEMPTY|PCRE2_PARTIAL_HARD;  /* No PCRE2_NO_UTF_CHECK c.f. regex_match to know why */

/* Please note that EVERY _marpaESLIFRecognizer_xxx() method is logging at start and at return */

static inline marpaESLIF_string_t   *_marpaESLIF_string_newp(marpaESLIF_t *marpaESLIFp, char *encodingasciis, char *bytep, size_t bytel, short asciib);
static inline marpaESLIF_string_t   *_marpaESLIF_string_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp);
static inline void                   _marpaESLIF_string_freev(marpaESLIF_string_t *stringp);
static inline short                  _marpaESLIF_string_eqb(marpaESLIF_string_t *string1p, marpaESLIF_string_t *string2p);
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descEncodings, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, char *utf8s, size_t utf8l, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *asciinames, char *descEncodings, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descEncodings, char *descs, size_t descl, short latmb, char *defaultSymbolActions, char *defaultRuleActions, char *defaultFreeActions, char *defaultDiscardEvents, short defaultDiscardEventb);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp);

static inline void                   _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp);
static inline void                   _marpaESLIFrecognizer_lexemeStack_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp);
static inline void                   _marpaESLIFrecognizer_lexemeStack_resetv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp);

static inline short                  _marpaESLIFRecognizer_lexemeStack_i_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i);
static inline short                  _marpaESLIFRecognizer_lexemeStack_i_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, size_t *sizelp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_i_p_and_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, char **pp, size_t *sizelp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_i_setarraypb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, GENERICSTACKITEMTYPE2TYPE_ARRAYP arrayp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, void *p, size_t sizel);
static inline short                  _marpaESLIFRecognizer_lexemeStack_i_moveb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackDstp, int dsti, genericStack_t *lexemeStackSrcp, int srci);
static inline const char            *_marpaESLIF_genericStack_i_types(genericStack_t *stackp, int i);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descEncodings, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb, char *actions, short passthroughb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp);

static inline void                   _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
                                                                    int leveli,
                                                                    char *descEndocings,
                                                                    char  *descs,
                                                                    size_t descl,
                                                                    short latmb,
                                                                    short warningIsErrorb,
                                                                    short warningIsIgnoredb,
                                                                    short autorankb,
                                                                    int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
                                                                    int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
                                                                    int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep,
                                                                    char *defaultSymbolActions,
                                                                    char *defaultRuleActions,
                                                                    char *defaultFreeActions,
                                                                    char *defaultDiscardEvents,
                                                                    short defaultDiscardEventb);
static inline short                  _marpaESLIFGrammar_validateb(marpaESLIFGrammar_t *marpaESLIFGrammar);

static inline short                  _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_terminal_t *terminalp, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static inline short                  _marpaESLIFRecognizer_meta_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIF_matcher_value_t *rcip, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static inline short                  _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

static const  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_charconvp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *srcs, size_t srcl, size_t *dstlp, char **fromEncodingsp, tconv_t *tconvpp);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip);
static        short                  _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static inline short                  _marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb, short *continuebp, short *exhaustedbp);
static inline marpaESLIF_grammar_t  *_marpaESLIFGrammar_grammar_findp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammari, marpaESLIF_string_t *descp);
static inline marpaESLIF_rule_t     *_marpaESLIF_rule_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int rulei);
static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *asciis, int symboli);
static inline short                  _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei);
static inline void                   _marpaESLIFRecognizer_reset_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp);
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short fakeb);
static inline short                  _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static        void                   _marpaESLIF_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static inline short                  _marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_flush_charconv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_start_charconvp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *encodingOfEncodings, char *encodings, size_t encodingl, char *srcs, size_t srcl);

/* All wrappers, even the Lexeme and Grammar wrappers go through these routines */
static        short                  _marpaESLIFValue_ruleCallbackWrapperb(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static        short                  _marpaESLIFValue_symbolCallbackWrapperb(void *userDatavp, int symboli, int argi, int resulti);
static        short                  _marpaESLIFValue_nullingCallbackWrapperb(void *userDatavp, int symboli, int resulti);
static inline short                  _marpaESLIFValue_anySymbolCallbackWrapperb(void *userDatavp, int symboli, int argi, int resulti, short nullableb);

static inline void                   _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb);
static inline void                   _marpaESLIFGrammar_grammarStack_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, genericStack_t *grammarStackp);
static        char                  *_marpaESLIFGrammar_symbolDescriptionCallback(void *userDatavp, int symboli);
static        short                  _marpaESLIFGrammar_symbolOptionSetterDiscardTop(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
static        short                  _marpaESLIFGrammar_symbolOptionSetterNoEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
static inline void                   _marpaESLIF_rule_createshowv(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep, char *asciishows, size_t *asciishowlp);
static inline int                    _marpaESLIF_utf82ordi(PCRE2_SPTR8 utf8bytes, marpaESLIF_uint32_t *uint32p);
static inline short                  _marpaESLIFRecognizer_matchPostProcessingb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t matchl);
static inline short                  _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *datas, size_t datal);
static inline short                  _marpaESLIFRecognizer_encoding_eqb(marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_terminal_t *terminalp, char *encodings, char *inputs, size_t inputl);
static inline void                   _marpaESLIFValueErrorProgressReportv(marpaESLIFValue_t *marpaESLIFValuep);
static inline short                  _marpaESLIF_resolveGrammarb(marpaESLIF_t *marpaESLIFp, genericStack_t *grammarStackp, marpaESLIF_grammar_t *current_grammarp, char *asciis, int lookupLevelDeltai, marpaESLIF_string_t *lookupGrammarStringp, marpaESLIF_grammar_t **grammarpp, marpaESLIF_symbol_t **symbolpp);

static inline short                  _marpaESLIF_generateHelper_includeb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, char *userspaceincludes);
static inline short                  _marpaESLIF_generateHelper_externb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, char *freeMethodNames, char *copyMethodNames);
static inline short                  _marpaESLIF_generateHelper_macrosb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_structureb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_newb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_ruleCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_symbolCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_freeb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_managedStack_enumb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp);
static inline short                  _marpaESLIF_generateHelper_by_grammarb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari, char *freeMethodNames, char *copyMethodNames);
static inline short                  _marpaESLIF_generateHelper_by_grammar_symbolCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari);
static inline short                  _marpaESLIF_generateHelper_by_grammar_ruleCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari);
static inline char                  *_marpaESLIF_ascii2ids(marpaESLIF_t *marpaESLIFp, char *asciis);
static inline short                  _marpaESLIFValue_stack_i_resetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, void **newpp, short *shallowbp, short forgetb);
static        short                  _marpaESLIF_lexeme_transferb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static        short                  _marpaESLIF_lexeme_concatb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        void                   _marpaESLIF_lexeme_freeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel);
static        void                   _marpaESLIF_rule_freeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel);
static inline marpaESLIFValue_t     *_marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short fakeb);
static inline short                  _marpaESLIFValue_stack_newb(marpaESLIFValue_t *marpaESLIFValuep);
static inline short                  _marpaESLIFValue_stack_freeb(marpaESLIFValue_t *marpaESLIFValuep);
static inline short                  _marpaESLIFValue_stack_set_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
static inline short                  _marpaESLIFValue_stack_set_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, char c);
static inline short                  _marpaESLIFValue_stack_set_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, short b);
static inline short                  _marpaESLIFValue_stack_set_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, int i);
static inline short                  _marpaESLIFValue_stack_set_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, long l);
static inline short                  _marpaESLIFValue_stack_set_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, float f);
static inline short                  _marpaESLIFValue_stack_set_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, double d);
static inline short                  _marpaESLIFValue_stack_set_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, short shallowb);
static inline short                  _marpaESLIFValue_stack_set_array(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, size_t l, short shallowb);
static inline short                  _marpaESLIFValue_stack_get_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, char *cp);
static inline short                  _marpaESLIFValue_stack_get_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, short *bp);
static inline short                  _marpaESLIFValue_stack_get_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, int *ip);
static inline short                  _marpaESLIFValue_stack_get_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, long *lp);
static inline short                  _marpaESLIFValue_stack_get_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, float *fp);
static inline short                  _marpaESLIFValue_stack_get_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, double *dp);
static inline short                  _marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
static inline short                  _marpaESLIFValue_stack_is_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *undefbp);
static inline short                  _marpaESLIFValue_stack_is_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *charbp);
static inline short                  _marpaESLIFValue_stack_is_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *shortbp);
static inline short                  _marpaESLIFValue_stack_is_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *intbp);
static inline short                  _marpaESLIFValue_stack_is_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *longbp);
static inline short                  _marpaESLIFValue_stack_is_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *floatbp);
static inline short                  _marpaESLIFValue_stack_is_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *doublebp);
static inline short                  _marpaESLIFValue_stack_is_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *ptrbp);
static inline short                  _marpaESLIFValue_stack_is_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *arraybp);
static inline short                  _marpaESLIFValue_stack_get_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp);
static inline short                  _marpaESLIFValue_stack_get_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp);
static inline short                  _marpaESLIFValue_context_symbolib(marpaESLIFValue_t *marpaESLIFValuep, int *symbolip);
static inline short                  _marpaESLIFValue_context_ruleib(marpaESLIFValue_t *marpaESLIFValuep, int *ruleip);
static        short                  _marpaESLIFValue_traverserb(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);
static        short                  _marpaESLIFValue_okRuleCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei);
static        short                  _marpaESLIFValue_okSymbolCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi);
static        short                  _marpaESLIFValue_okNullingCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli);
static inline short                  _marpaESLIFValue_okAnySymbolCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi);
static        short                  _marpaESLIF_rule_action___shiftb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short                  _marpaESLIF_rule_action___undefb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short                  _marpaESLIF_rule_action___asciib(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short                  _marpaESLIF_rule_action___translitb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static inline short                  _marpaESLIF_rule_action___charconvb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, char *toEncodings);
static        short                  _marpaESLIF_symbol_action___shiftb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static        short                  _marpaESLIF_symbol_action___undefb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static        short                  _marpaESLIF_symbol_action___asciib(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static        short                  _marpaESLIF_symbol_action___translitb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static inline short                  _marpaESLIF_symbol_action___charconvb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, char *toEncodings);

/*****************************************************************************/
static inline marpaESLIF_string_t *_marpaESLIF_string_newp(marpaESLIF_t *marpaESLIFp, char *encodingasciis, char *bytep, size_t bytel, short asciib)
/*****************************************************************************/
{
  static const char   *funcs = "_marpaESLIF_string_newp";
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
  stringp->bytep          = NULL;
  stringp->bytel          = bytel;
  stringp->encodingasciis = NULL;
  stringp->asciis         = NULL;

  stringp->bytep = dstbytep = (char *) malloc(bytel + 1); /* To ease debugging we ADD ONE BYTE, and put NUL in it */
  if (dstbytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(dstbytep, bytep, bytel);
  dstbytep[bytel] = '\0';  /* Developers fighting with debuggers will understand why -; */

  if (asciib) {
    stringp->asciis = dstasciis = _marpaESLIF_charconvp(marpaESLIFp, "ASCII//TRANSLIT//IGNORE", encodingasciis, bytep, bytel, NULL, &(stringp->encodingasciis), NULL /* tconvpp */);
    if (dstasciis == NULL) {
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_string_freev(stringp);
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
static inline marpaESLIF_string_t *_marpaESLIF_string_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_string_t *rcp = NULL;
  char                *bytep;
  size_t               bytel;
  char                *asciis;
  char                *encodingasciis;
  
  if (stringp == NULL) {
    goto err;
  }

  rcp = (marpaESLIF_string_t *) malloc(sizeof(marpaESLIF_string_t));
  if (rcp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rcp->bytep          = NULL;
  rcp->bytel          = 0;
  rcp->encodingasciis = NULL;
  rcp->asciis         = NULL;

  bytep = rcp->bytep = (char *) malloc((rcp->bytel = bytel = stringp->bytel));
  if (bytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(bytep, stringp->bytep, bytel);

  if (stringp->asciis != NULL) {
    rcp->asciis = asciis = strdup(stringp->asciis);
    if (asciis == NULL) {
      goto err;
    }
  }

  if (stringp->encodingasciis != NULL) {
    rcp->encodingasciis = encodingasciis = strdup(stringp->encodingasciis);
    if (encodingasciis == NULL) {
      goto err;
    }
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
    if (stringp->bytep != NULL) {
      free(stringp->bytep);
    }
    if (stringp->encodingasciis != NULL) {
      free(stringp->encodingasciis);
    }
    if (stringp->asciis != NULL) {
      free(stringp->asciis);
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
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descEncodings, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, char *utf8s, size_t utf8l, char *testFullMatchs, char *testPartialMatchs)
/*****************************************************************************/
/* This method is bootstraped at marpaESLIFp creation itself to have the internal regexps, with grammarp being NULL... */
/*****************************************************************************/
{
  static const char                *funcs = "_marpaESLIF_terminal_newp";
  char                             *strings               = NULL;
  marpaESLIFRecognizer_t           *marpaESLIFRecognizerp = NULL;
#ifndef MARPAESLIF_NTRACE
  marpaESLIFRecognizer_t           *marpaESLIFRecognizerTestp = NULL;
#endif
  marpaESLIF_string_t              *content2descp         = NULL;
  char                             *generatedasciis       = NULL;
  marpaESLIF_terminal_t            *terminalp;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;
  marpaESLIF_uint32_t               pcre2Optioni;
  int                               pcre2Errornumberi;
  PCRE2_SIZE                        pcre2ErrorOffsetl;
  PCRE2_UCHAR                       pcre2ErrorBuffer[256];
  int                               i;
  marpaESLIFGrammar_t               marpaESLIFGrammar;
  char                             *inputs;
  size_t                            inputl;
  marpaESLIF_matcher_value_t        rci;
  marpaESLIF_uint32_t               codepointi;
  short                             utfflagb;
  size_t                            stringl;
  char                             *tmps;
  size_t                            hexdigitl;
  int                               utf82ordi;
  marpaESLIFValueResult_t           marpaESLIFValueResult;
  char                             *matchedp;
  size_t                            matchedl;
  int                               regexdescriptori;

  /* Check some required parameters */
  if ((utf8s == NULL) || (utf8l <= 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Invalid terminal origin");
    goto err;
  }

  switch (type) {
  case MARPAESLIF_TERMINAL_TYPE_STRING:
  case MARPAESLIF_TERMINAL_TYPE_REGEX:
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported terminal type %d", type);
    goto err;
    break;
  }
  
  /* Please note the "fakeb" parameter below */

  terminalp = (marpaESLIF_terminal_t *) malloc(sizeof(marpaESLIF_terminal_t));
  if (terminalp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  terminalp->idi                 = -1;
  terminalp->descp               = NULL;
  terminalp->regex.patternp      = NULL;
  terminalp->regex.match_datap   = NULL;
#ifdef PCRE2_CONFIG_JIT
  terminalp->regex.jitCompleteb  = 0;
  terminalp->regex.jitPartialb   = 0;
#endif
  terminalp->regex.isAnchoredb   = 0;
  terminalp->regex.utfb          = 0;

  /* ----------- Terminal Identifier ------------ */
  if (grammarp != NULL) { /* Here is the bootstrap dependency with grammarp == NULL */
    marpaWrapperGrammarSymbolOption.terminalb = 1;
    marpaWrapperGrammarSymbolOption.startb    = 0;
    marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;
    terminalp->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
    if (terminalp->idi < 0) {
      goto err;
    }
  }

  /* ----------- Terminal Description ------------ */
  if (descs == NULL) {
    /* Get an ASCII version of the content */
    content2descp = _marpaESLIF_string_newp(marpaESLIFp, "UTF-8", utf8s, utf8l, 1);
    if (content2descp == NULL) {
      goto err;
    }
    if (type == MARPAESLIF_TERMINAL_TYPE_STRING) {
      /* Try to be clever, using ' or ", otherwise fall back to the q{} version (without escaping {}) */
      if (strchr(content2descp->asciis, '\'') == NULL) {
        /* 'XXX' */
        generatedasciis = (char *) malloc(1 + strlen(content2descp->asciis) + 1 + 1);
        if (generatedasciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        strcpy(generatedasciis, "'");
        strcat(generatedasciis, content2descp->asciis);
        strcat(generatedasciis, "'");
      } else if (strchr(content2descp->asciis, '"') == NULL) {
        /* "XXX" */
        generatedasciis = (char *) malloc(1 + strlen(content2descp->asciis) + 1 + 1);
        if (generatedasciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        strcpy(generatedasciis, "\"");
        strcat(generatedasciis, content2descp->asciis);
        strcat(generatedasciis, "\"");
      } else {
        /* {XXX} */
        generatedasciis = (char *) malloc(1 + strlen(content2descp->asciis) + 1 + 1);
        if (generatedasciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        strcpy(generatedasciis, "{");
        strcat(generatedasciis, content2descp->asciis);
        strcat(generatedasciis, "}");
      }
    } else {
      /* /XXX/ (without escaping) */
      generatedasciis = (char *) malloc(1 + strlen(content2descp->asciis) + 1 + 1);
      if (generatedasciis == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      strcpy(generatedasciis, "/");
      strcat(generatedasciis, content2descp->asciis);
      strcat(generatedasciis, "/");
    }
    /*
     * Look to modifiers
     */
    terminalp->descp = _marpaESLIF_string_newp(marpaESLIFp, "ASCII", generatedasciis, strlen(generatedasciis), 1);
  } else {
    terminalp->descp = _marpaESLIF_string_newp(marpaESLIFp, descEncodings, descs, descl, 1);
  }
  if (terminalp->descp == NULL) {
    goto err;
  }

  /* ----------- Terminal Implementation ------------ */
  switch (type) {

  case MARPAESLIF_TERMINAL_TYPE_STRING:
    /* We convert a string terminal into a regexp */
    /* By construction we are coming from the parsing of a grammar, that previously translated the whole */
    /* grammar into an UTF-8 string. We use PCRE2 to extract all code points, and create a new string that */
    /* is a concatenation of \x{} thingies. By doing so, btw, we are able to know if we need PCRE2_UTF flag. */

    marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
    inputs = utf8s;
    inputl = utf8l;

    /* Fake a recognizer. EOF flag will be set automatically in fake mode */
    marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
    if (marpaESLIFRecognizerp == NULL) {
      goto err;
    }

    while (inputl > 0) {
      if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, marpaESLIFp->anycharp, inputs, inputl, 1 /* eofb */, &rci, &marpaESLIFValueResult)) {
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_OK) {
        MARPAESLIF_ERROR(marpaESLIFp, "Failed to detect all characters of terminal string");
        goto err;
      }
      /* It is a non-sense to have a regex match returning something else but MARPAESLIF_STACK_TYPE_ARRAY */
      if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "newline regex returned type %d}", marpaESLIFValueResult.type);
        goto err;
      }
      /* It is a non-sense to have a match returning nothing */
      if (marpaESLIFValueResult.u.p == NULL) {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "anychar regex return NULL pointer");
        goto err;
      }
      if (marpaESLIFValueResult.sizel <= 0) {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "anychar regex matched zero byte");
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizerp, marpaESLIFRecognizerp->lexemeInputStackp, GENERICSTACK_USED(marpaESLIFRecognizerp->lexemeInputStackp), marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel)) {
        free(marpaESLIFValueResult.u.p);
        goto err;
      }
      inputs += marpaESLIFValueResult.sizel;
      inputl -= marpaESLIFValueResult.sizel;
    }
    /* All matches are in the recognizer's lexeme input stack, in order. Take all unicode code points to generate a regex out of this string. */
    utfflagb = 0;
    stringl = 0;
    /* Remember that lexeme input stack is putting a fake value at indice 0, because marpa does not like it */
    for (i = 1; i < GENERICSTACK_USED(marpaESLIFRecognizerp->lexemeInputStackp); i++) {
      if (! _marpaESLIFRecognizer_lexemeStack_i_p_and_sizeb(marpaESLIFRecognizerp, marpaESLIFRecognizerp->lexemeInputStackp, i, &matchedp, &matchedl)) {
        goto err;
      }
      /* Get the code point from the UTF-8 representation */
      utf82ordi = _marpaESLIF_utf82ordi((PCRE2_SPTR8) matchedp, &codepointi);
      if (utf82ordi <= 0) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Malformed UTF-8 character at offset %d", -utf82ordi);
        goto err;
      } else if (utf82ordi != (int) matchedl) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Not all bytes consumed: %d instead of %ld", utf82ordi, (unsigned long) matchedl);
        goto err;
      }
      /* Determine the number of hex digits to fully represent the code point, remembering if we need PCRE2_UTF flag */
      hexdigitl = 4; /* \x{} */
      if ((codepointi & 0xFF000000) != 0x00000000) {
        hexdigitl += 8;
        utfflagb = 1;
      } else if ((codepointi & 0x00FF0000) != 0x00000000) {
        hexdigitl += 6;
        utfflagb = 1;
      } else if ((codepointi & 0x0000FF00) != 0x00000000) {
        hexdigitl += 4;
        utfflagb = 1;
      } else {
        hexdigitl += 2;
      }
      /* Append the ASCII representation */
      stringl += hexdigitl;
      if (strings == NULL) {
        strings = (char *) malloc(stringl + 1);
        if (strings == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        strings[0] = '\0'; /* Start with an empty string */
      } else {
        tmps = (char *) realloc(strings, stringl + 1);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        strings = tmps;
      }
      strings[stringl] = '\0'; /* Make sure the string always end with NUL */
      hexdigitl -= 4; /* \x{} */
      sprintf(strings + strlen(strings), "\\x{%0*x}", (int) hexdigitl, codepointi);
    }
    /* Done - now we can generate a regexp out of that UTF-8 compatible string */
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: content string converted to regex %s (UTF=%d)", terminalp->descp->asciis, strings, utfflagb);
    utf8s = strings;
    utf8l = stringl;
    /* opti for string is compatible with opti for regex - just that the lexer accept less options - in particular the UTF flag */
    if (utfflagb) {
      opti |= MARPAESLIF_REGEX_OPTION_UTF;
    }

    /* ********************************************************************************************************** */
    /*                                   THERE IS NO BREAK INTENTIONALY HERE                                      */
    /* ********************************************************************************************************** */
    /* break; */

  case MARPAESLIF_TERMINAL_TYPE_REGEX:

    /* By default patterns are always anchored and only that EXCEPT in one case... see regex_match */
    pcre2Optioni = PCRE2_ANCHORED;
    for (i = 0; i < _MARPAESLIF_REGEX_OPTION_ID_MAX; i++) {
      if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
        /* It is important to process pcre2OptionNoti first */
	if (marpaESLIF_regex_option_map[i].pcre2OptionNoti != 0) {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: regex modifier %s: removing %s", terminalp->descp->asciis, marpaESLIF_regex_option_map[i].modifiers, marpaESLIF_regex_option_map[i].pcre2OptionNots);
	  pcre2Optioni &= ~marpaESLIF_regex_option_map[i].pcre2OptionNoti;
	}
	if (marpaESLIF_regex_option_map[i].pcre2Optioni != 0) {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: regex modifier %s: adding %s", terminalp->descp->asciis, marpaESLIF_regex_option_map[i].modifiers, marpaESLIF_regex_option_map[i].pcre2Options);
	  pcre2Optioni |= marpaESLIF_regex_option_map[i].pcre2Optioni;
	}
      }
    }

    terminalp->regex.patternp = pcre2_compile(
                                              (PCRE2_SPTR) utf8s,      /* An UTF-8 pattern */
                                              (PCRE2_SIZE) utf8l,      /* In code units (!= code points) - in UTF-8 a code unit is a byte */
                                              pcre2Optioni,
                                              &pcre2Errornumberi, /* for error number */
                                              &pcre2ErrorOffsetl, /* for error offset */
                                              NULL);        /* use default compile context */
    if (terminalp->regex.patternp == NULL) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_compile failure at offset %ld: %s", terminalp->descp->asciis, (unsigned long) pcre2ErrorOffsetl, pcre2ErrorBuffer);
      goto err;
    }
    terminalp->regex.match_datap = pcre2_match_data_create(1 /* We are interested in the string that matched the full pattern */,
                                                             NULL /* Default memory allocation */);
    if (terminalp->regex.match_datap == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_match_data_create_from_pattern failure, %s", terminalp->descp->asciis, strerror(errno));
      goto err;
    }
    /* Determine if we can do JIT */
#ifdef PCRE2_CONFIG_JIT
    if ((pcre2_config(PCRE2_CONFIG_JIT, &pcre2Optioni) >= 0) && (pcre2Optioni == 1)) {
#ifdef PCRE2_JIT_COMPLETE
      terminalp->regex.jitCompleteb = (pcre2_jit_compile(terminalp->regex.patternp, PCRE2_JIT_COMPLETE) == 0) ? 1 : 0;
#else
      terminalp->regex.jitCompleteb = 0;
#endif
#ifdef PCRE2_JIT_PARTIAL_HARD
      terminalp->regex.jitPartialb = (pcre2_jit_compile(terminalp->regex.patternp, PCRE2_JIT_PARTIAL_HARD) == 0) ? 1 : 0;
#else
      terminalp->regex.jitPartialb = 0;
#endif /*  PCRE2_CONFIG_JIT */
    } else {
      terminalp->regex.jitCompleteb = 0;
      terminalp->regex.jitPartialb = 0;
    }
#endif /*  PCRE2_CONFIG_JIT */
    /* And some modes after the pattern was allocated */
    pcre2Errornumberi = pcre2_pattern_info(terminalp->regex.patternp, PCRE2_INFO_ALLOPTIONS, &pcre2Optioni);
    if (pcre2Errornumberi != 0) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_pattern_info failure: %s", terminalp->descp->asciis, pcre2ErrorBuffer);
      goto err;
    }
    terminalp->regex.utfb        = ((pcre2Optioni & PCRE2_UTF) == PCRE2_UTF);
    terminalp->regex.isAnchoredb = ((pcre2Optioni & PCRE2_ANCHORED) == PCRE2_ANCHORED);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: UTF mode is %s, Anchored mode is %s",
                      terminalp->descp->asciis,
                      terminalp->regex.utfb ? "on" : "off",
                      terminalp->regex.isAnchoredb ? "on" : "off"
                      );

    /* If opti is set, revisit the automatic description */
    if ((pcre2Optioni != PCRE2_ANCHORED) & (generatedasciis != NULL)) {
      regexdescriptori =  0;
      for (i = 0; i < _MARPAESLIF_REGEX_OPTION_ID_MAX; i++) {
        if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
          regexdescriptori++;
        }
      }
      if (regexdescriptori > 0) {
        free(generatedasciis);
        generatedasciis = (char *) malloc(strlen(terminalp->descp->asciis) + regexdescriptori + 1);
        if (generatedasciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        strcpy(generatedasciis, terminalp->descp->asciis);
        for (i = 0; i < _MARPAESLIF_REGEX_OPTION_ID_MAX; i++) {
          if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
            strcat(generatedasciis, marpaESLIF_regex_option_map[i].modifiers);
          }
        }
        /* No need for a new call to string */
        free(terminalp->descp->asciis);
        terminalp->descp->asciis = generatedasciis;
        generatedasciis = NULL;
      }
    }
    break;

  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "%s: Unsupported terminal type %d", terminalp->descp->asciis, type);
    goto err;
    break;
  }

#ifndef MARPAESLIF_NTRACE
  {
    marpaESLIFGrammar_t     marpaESLIFGrammar;

    marpaESLIFGrammar.marpaESLIFp   = marpaESLIFp;
    marpaESLIFGrammar.grammarStackp = NULL;
    marpaESLIFGrammar.grammarp      = grammarp;

    /* Fake a recognizer. EOF flag will be set automatically in fake mode */
    marpaESLIFRecognizerTestp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
    if (marpaESLIFRecognizerTestp == NULL) {
      goto err;
    }

    /* Look to the implementations of terminal matchers: they NEVER use leveli, nor marpaWrapperGrammarp, nor grammarStackp -; */
    /* Also, note that we always end up with a regex. */
    
    if (testFullMatchs != NULL) {

      if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerTestp, terminalp, testFullMatchs, strlen(testFullMatchs), 1, &rci, NULL /* marpaESLIFValueResultp */)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s: testing full match: matcher general failure", terminalp->descp->asciis);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_OK) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s: testing full match: matcher returned rci = %d", terminalp->descp->asciis, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: testing full match is successful on %s", terminalp->descp->asciis, testFullMatchs); */
    }

    if (testPartialMatchs != NULL) {

      if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerTestp, terminalp, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, NULL /* marpaESLIFValueResultp */)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s: testing partial match: matcher general failure", terminalp->descp->asciis);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_AGAIN) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s: testing partial match: matcher returned rci = %d", terminalp->descp->asciis, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s: testing partial match is successful on %s when not at EOF", terminalp->descp->asciis, testPartialMatchs); */
    }
  }

#endif

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  terminalp = NULL;

 done:
  if (strings != NULL) {
    free(strings);
  }
  if (generatedasciis != NULL) {
    free(generatedasciis);
  }
  _marpaESLIF_string_freev(content2descp);
#ifndef MARPAESLIF_NTRACE
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerTestp);
#endif
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp); */
  return terminalp;
}

/*****************************************************************************/
static inline marpaESLIF_meta_t *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *asciinames, char *descEncodings, char *descs, size_t descl)
/*****************************************************************************/
{
  static const char                *funcs = "_marpaESLIF_meta_newp";
  marpaESLIF_meta_t                *metap = NULL;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building meta"); */

  if (asciinames == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No name for meta symbol");
    goto err;
  }
  if (strlen(asciinames) <= 0) {
    MARPAESLIF_ERROR(marpaESLIFp, "Meta symbol name is empty");
    goto err;
  }

  metap = (marpaESLIF_meta_t *) malloc(sizeof(marpaESLIF_meta_t));
  if (metap == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  metap->idi                             = -1;
  metap->asciinames                      = NULL;
  metap->descp                           = NULL;
  metap->marpaWrapperGrammarLexemeClonep = NULL; /* Eventually changed when validating the grammar */
  metap->prioritizedb                    = 0;    /* Internal flag to prevent a prioritized symbol to appear more than once as an LHS */

  marpaWrapperGrammarSymbolOption.terminalb = 0;
  marpaWrapperGrammarSymbolOption.startb    = 0;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* -------- Meta name -------- */
  metap->asciinames = strdup(asciinames);
  if (metap->asciinames == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }

  /* -------- Meta Description - default to meta name -------- */
  if ((descs == NULL) || (descl <= 0)) {
    metap->descp = _marpaESLIF_string_newp(marpaESLIFp, "ASCII", asciinames, strlen(asciinames), 1 /* asciib */);
  } else {
    metap->descp = _marpaESLIF_string_newp(marpaESLIFp, descEncodings, descs, descl, 1 /* asciib */);
  }
  if (metap->descp == NULL) {
    goto err;
  }

  /* ----------- Meta Identifier ------------ */
  metap->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
  if (metap->idi < 0) {
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
    if (metap->asciinames != NULL) {
      free(metap->asciinames);
    }
    _marpaESLIF_string_freev(metap->descp);
    if (metap->marpaWrapperGrammarLexemeClonep != NULL) {
      marpaWrapperGrammar_freev(metap->marpaWrapperGrammarLexemeClonep);
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
                                        "ASCII", /* "L0" is an ASCII thingy */
                                        "L0",
                                        strlen("L0"),
                                        1, /* latmb */
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_L0_terminals) / sizeof(bootstrap_grammar_L0_terminals[0]),
					bootstrap_grammar_L0_terminals,
					sizeof(bootstrap_grammar_L0_metas) / sizeof(bootstrap_grammar_L0_metas[0]),
					bootstrap_grammar_L0_metas,
					sizeof(bootstrap_grammar_L0_rules) / sizeof(bootstrap_grammar_L0_rules[0]),
					bootstrap_grammar_L0_rules,
                                        NULL, /* defaultSymbolActions */
                                        NULL, /* defaultRuleActions */
                                        NULL, /* char *defaultFreeActions */
                                        NULL, /* defaultDiscardEvents */
                                        0 /* defaultDiscardEventsb */);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_grammarp(marpaESLIFp,
					0, /* G1 in Marpa::R2 terminology is level No 0 for us */
                                        "ASCII", /* "G1" is an ASCII thingy */
                                        "G1",
                                        strlen("G1"),
                                        1, /* latmb */
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_G1_terminals) / sizeof(bootstrap_grammar_G1_terminals[0]),
					bootstrap_grammar_G1_terminals,
					sizeof(bootstrap_grammar_G1_metas) / sizeof(bootstrap_grammar_G1_metas[0]),
					bootstrap_grammar_G1_metas,
					sizeof(bootstrap_grammar_G1_rules) / sizeof(bootstrap_grammar_G1_rules[0]),
					bootstrap_grammar_G1_rules,
                                        "::shift", /* defaultSymbolActions */
                                        "::shift", /* defaultRuleActions */
                                        "_marpaESLIF_bootstrap_freeDefaultActionv", /* defaultFreeActions */
                                        NULL, /* defaultDiscardEvents */
                                        0 /* defaultDiscardEventsb */);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
								   int leveli,
                                                                   char *descEncodings,
                                                                   char *descs,
                                                                   size_t descl,
                                                                   short latmb,
								   short warningIsErrorb,
								   short warningIsIgnoredb,
								   short autorankb,
								   int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
								   int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
								   int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep,
                                                                   char *defaultSymbolActions,
                                                                   char *defaultRuleActions,
                                                                   char *defaultFreeActions,
                                                                   char *defaultDiscardEvents,
                                                                   short defaultDiscardEventb)
/*****************************************************************************/
{
  static const char          *funcs        = "_marpaESLIF_bootstrap_grammarp";
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
  
  grammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, leveli, descEncodings, descs, descl, latmb, defaultSymbolActions, defaultRuleActions, defaultFreeActions, defaultDiscardEvents, defaultDiscardEventb);
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
                                          NULL, /* descEncodings */
					  NULL, /* descs */
                                          0, /* descl */
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
                                  NULL, /* descEncodings */
				  NULL, /* descs */
				  0 /* descl */
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
    /* Meta is now in symbol */
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
                                  NULL, /* descEncodings */
                                  bootstrap_grammar_rulep[i].descs,
                                  strlen(bootstrap_grammar_rulep[i].descs),
				  bootstrap_grammar_rulep[i].lhsi,
				  bootstrap_grammar_rulep[i].nrhsl,
				  bootstrap_grammar_rulep[i].rhsip,
				  0, /* nexceptionl */
				  NULL, /* exceptionip */
				  0, /* ranki */
				  0, /* nullRanksHighb */
				  (bootstrap_grammar_rulep[i].type == MARPAESLIF_RULE_TYPE_ALTERNATIVE) ? 0 : 1, /* sequenceb */
				  bootstrap_grammar_rulep[i].minimumi,
				  bootstrap_grammar_rulep[i].separatori,
				  bootstrap_grammar_rulep[i].properb,
                                  bootstrap_grammar_rulep[i].actions,
                                  0 /* passthroughb */
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
  static const char     *funcs                            = "_marpaESLIFGrammar_validateb";
  marpaESLIF_t          *marpaESLIFp                      = marpaESLIFGrammarp->marpaESLIFp;
  genericStack_t        *grammarStackp                    = marpaESLIFGrammarp->grammarStackp;
  marpaWrapperGrammar_t *marpaWrapperGrammarClonep        = NULL;
  marpaESLIF_meta_t     *metap;
  genericStack_t        *symbolStackp;
  genericStack_t        *subSymbolStackp;
  genericStack_t        *ruleStackp;
  int                    grammari;
  marpaESLIF_symbol_t   *symbolp;
  marpaESLIF_symbol_t   *tmpSymbolp;
  marpaESLIF_symbol_t   *subSymbolp;
  int                    symboli;
  int                    subSymboli;
  marpaESLIF_rule_t     *rulep;
  marpaESLIF_rule_t     *ruletmpp;
  int                    rulei;
  int                    rulej;
  marpaESLIF_grammar_t  *grammarp;
  marpaESLIF_grammar_t  *sub_grammarp;
  short                  lhsb;
  marpaESLIF_symbol_t   *lhsp;
  marpaESLIF_symbol_t   *startp;
  marpaESLIF_symbol_t   *discardp;
  short                  rcb;
  int                    rhsi;
  size_t                 asciishowl;
  int                    marpaWrapperRulePropertyBitSet;
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
   4. If at least one rule have rejection, rejection mode is on at the grammar level.
   5. For every rule that is a passthrough, then it is illegal to have its lhs appearing as an lhs is any other rule
   6. The RHS of every :discard rule must be a top-level symbol (.i.e a symbol never appearing as an RHS) in the resolved grammar
       (the ESLIF grammar made sure that there can be only one symbol on the right-hande side of a :discard rule)
   7. The semantic of a nullable LHS must be unique

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
     c. The RHS of every :discard rule must be a top-level symbol (.i.e a symbol never appearing as an RHS) in the resolved grammar
       (the ESLIF grammar made sure that there can be only one symbol on the right-hande side of a :discard rule)
  */
  /* Pre-scan all grammars to set the topb attribute of every symbol */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    symbolStackp = grammarp->symbolStackp;
    ruleStackp = grammarp->ruleStackp;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      symbolp->topb = 1;
      for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
        if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
          /* Should never happen, but who knows */
          continue;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
        for (rhsi = 0; rhsi < GENERICSTACK_USED(rulep->rhsStackp); rhsi++) {
          if (! GENERICSTACK_IS_PTR(rulep->rhsStackp, rhsi)) {
            continue;
          }
          if (GENERICSTACK_GET_PTR(rulep->rhsStackp, rhsi) == (void *) symbolp) {
            symbolp->topb = 0;
            break;
          }
        }
        if (! symbolp->topb) {
          break;
        }
      }
    }
  }
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
      marpaESLIF_cloneContext.grammarp = grammarp;
      /* Clone for the discard mode at grammar level */
      marpaWrapperGrammarCloneOption.symbolOptionSetterp = _marpaESLIFGrammar_symbolOptionSetterDiscardTop;
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
      if (metap->marpaWrapperGrammarLexemeClonep != NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) already processed", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
        continue;
      }

      /* Level of RHS is either via a grammar description, either via a number */
      /* They are exclusive (c.f. the grammar). Please note that we made sure */
      /* that symbolp is a meta symbol -; */
      if (! _marpaESLIF_resolveGrammarb(marpaESLIFp, grammarStackp, grammarp, symbolp->u.metap->asciinames, symbolp->lookupLevelDeltai, symbolp->lookupGrammarStringp, &sub_grammarp, NULL /* symbolpp */)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing a non-existing grammar ", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis);
        goto err;
      }
      /* Commit resolved level in symbol */
      symbolp->resolvedLeveli = sub_grammarp->leveli;
      /* Make sure this RHS is an LHS in the sub grammar, ignoring the case where sub grammar would be current grammar */
      if (sub_grammarp == grammarp) {
        continue;
      }
      
      subSymbolStackp = sub_grammarp->symbolStackp;
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
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing a non-existing symbol at grammar level %d (%s)", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, sub_grammarp->leveli, sub_grammarp->descp->asciis);
        goto err;
      }
      if (! subSymbolp->lhsb) {
        /* When sub grammar is current grammar, this mean that we require that this RHS is also an LHS - which is correct because we restricted symbol loop on meta symbols */
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing existing symbol No %d (%s) at grammar level %d (%s) but it is not an LHS", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, subSymbolp->idi, subSymbolp->descp->asciis, sub_grammarp->leveli, sub_grammarp->descp->asciis);
        goto err;
      }

      /* Clone for the symbol in lexeme mode */
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarCloneOption.symbolOptionSetterp = _marpaESLIFGrammar_symbolOptionSetterNoEvent;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(sub_grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        goto err;
      }
      if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, subSymbolp->idi)) {
        goto err;
      }
      metap->marpaWrapperGrammarLexemeClonep = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;

      MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) have grammar resolved level set to   %d", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, symbolp->resolvedLeveli);
    }
  }

  /*
    4. If at least one rule have rejection, rejection mode is on at the grammar level.
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at rejection in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    /* Loop on rules */
    ruleStackp = grammarp->ruleStackp;
    for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
        /* Should never happen, but who knows */
        continue;
      }
      rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
      if (rulep->exceptionStackp != NULL) {
        if ((! grammarp->haveRejectionb) && (GENERICSTACK_USED(rulep->exceptionStackp) > 0)) {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Marking rejection flag for grammar at level %d (%s)", grammari, grammarp->descp->asciis);
          grammarp->haveRejectionb = 1;
          break;
        }
      }
    }
  }

  /*
   5. For every rule that is a passthrough, then it is illegal to have its lhs appearing as an lhs is any other rule
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at rejection in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    /* Loop on rules */
    ruleStackp = grammarp->ruleStackp;
    for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
        /* Should never happen, but who knows */
        continue;
      }
      rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
      if (rulep->passthroughb) {
        for (rulej = 0; rulej < GENERICSTACK_USED(ruleStackp); rulej++) {
          if (rulei == rulej) {
            continue;
          }
          if (! GENERICSTACK_IS_PTR(ruleStackp, rulej)) {
            /* Should never happen, but who knows */
            continue;
          }
          ruletmpp = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulej);
          if (rulep->lhsp == ruletmpp->lhsp) {
            MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is an LHS of a prioritized rule and cannot be appear as an LHS is any other rule", grammari, grammarp->descp->asciis, rulep->lhsp->idi, rulep->lhsp->descp->asciis);
            goto err;
          }
        }
      }
    }
  }

  /*
   6. The RHS of every :discard rule must be a top-level symbol (.i.e a symbol never appearing as an RHS) in the resolved grammar
       (the ESLIF grammar made sure that there can be only one symbol on the right-hande side of a :discard rule)
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at rejection in grammar level %d (%s)", grammari, grammarp->descp->asciis);

    /* Loop on rules */
    ruleStackp = grammarp->ruleStackp;
    for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
        /* Should never happen, but who knows */
        continue;
      }
      rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
      /* Does this rule have the :discard symbol as LHS ? */
      if (rulep->lhsp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) has no LHS pointer !?", grammari, grammarp->descp->asciis, rulep->idi, rulep->descp->asciis);
        goto err;
      }
      if (! rulep->lhsp->discardb) {
        continue;
      }
      /* There should be a single RHS in this rule */
      if (GENERICSTACK_USED(rulep->rhsStackp) != 1) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) must have a single RHS", grammari, grammarp->descp->asciis, rulep->idi, rulep->descp->asciis);
        goto err;
      }
      if (! GENERICSTACK_IS_PTR(rulep->rhsStackp, 0)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s)'s rhs stack does not have a pointer at indice 0 !?", grammari, grammarp->descp->asciis, rulep->idi, rulep->descp->asciis);
        goto err;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(rulep->rhsStackp, 0);

      /* If current symbol is a lexeme, per def it is "top-level" in any discard context */
      if (symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) {
        symbolp->isDiscardTopb = 1;
        continue;
      }

      /* If current symbol is a meta, we already made sure 
      /* Look to the resolved grammar (this can be current grammar) */
      if (! GENERICSTACK_IS_PTR(grammarStackp, symbolp->resolvedLeveli)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s)'s RHS symbol %d (%s) resolve to an unknown grammar !?", grammari, grammarp->descp->asciis, rulep->idi, rulep->descp->asciis, symbolp->idi, symbolp->descp->asciis);
        goto err;
      }
      sub_grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, symbolp->resolvedLeveli);
      /* Look for this symbol in the sub grammar */
      subSymbolStackp = sub_grammarp->symbolStackp;
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
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing a non-existing symbol at grammar level %d (%s)", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, sub_grammarp->leveli, sub_grammarp->descp->asciis);
        goto err;
      }
      if (! subSymbolp->topb) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): symbol %d (%s) is referencing existing symbol No %d (%s) at grammar level %d (%s) but it is not a top level symbol", grammari, grammarp->descp->asciis, symbolp->idi, symbolp->descp->asciis, subSymbolp->idi, subSymbolp->descp->asciis, sub_grammarp->leveli, sub_grammarp->descp->asciis);
        goto err;
      }
      symbolp->isDiscardTopb = 1;
    }
  }

  /*
   7. The semantic of a nullable LHS must be unique
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Checking nullable LHS semantic in grammar level %d (%s)", grammarp->leveli, grammarp->descp->asciis);
    /* First we collect the nullable rule Ids by LHS Id */
    ruleStackp = grammarp->ruleStackp;
    for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
      if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
        continue;
      }
      rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
      if (! marpaWrapperGrammar_rulePropertyb(grammarp->marpaWrapperGrammarStartp, rulep->idi, &marpaWrapperRulePropertyBitSet)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "marpaWrapperGrammar_rulePropertyb failure for grammar level %d (%s)", grammarp->leveli, grammarp->descp->asciis);
        goto err;
      }
      if ((marpaWrapperRulePropertyBitSet & MARPAWRAPPER_RULE_IS_NULLABLE) == MARPAWRAPPER_RULE_IS_NULLABLE) {
        GENERICSTACK_PUSH_PTR(rulep->lhsp->nullableRuleStackp, rulep);
        if (GENERICSTACK_ERROR(rulep->lhsp->nullableRuleStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "rulep->lhsp->nullableRuleStackp push failure, %s", strerror(errno));
          goto err;
        }
      }
    }

    /* Then we determine the nullable semantic */
    symbolStackp = grammarp->symbolStackp;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (GENERICSTACK_USED(symbolp->nullableRuleStackp) <= 0) {
        continue;
      }

      if (GENERICSTACK_USED(symbolp->nullableRuleStackp) == 1) {
        /* Just one nullable rule: nullable semantic is this rule's semantic */
        if (! GENERICSTACK_IS_PTR(symbolp->nullableRuleStackp, 0)) {
          /* Impossible */
          MARPAESLIF_ERRORF(marpaESLIFp, "symbolp->nullableRuleStackp at indice 0 is not PTR (internal type: %d)", GENERICSTACKITEMTYPE(symbolp->nullableRuleStackp, 0));
          goto err;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(symbolp->nullableRuleStackp, 0);
        symbolp->nullableActions = rulep->actions;
#ifndef MARPAESLIF_NTRACE
        if (symbolp->nullableActions != NULL) {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is %s", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
        } else {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is grammar's default", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
        }
#endif
      } else {
        short foundEmptyb = 0;
        /* More than one rule. If there is an empty rule, use it. Please note that Marpa precomputation made sure that the */
        /* empty rule is unique (there cannot be LHS ::= ; twice). */
        for (rulei = 0; rulei < GENERICSTACK_USED(symbolp->nullableRuleStackp); rulei++) {
          if (! GENERICSTACK_IS_PTR(symbolp->nullableRuleStackp, rulei)) {
            /* Impossible */
            MARPAESLIF_ERRORF(marpaESLIFp, "symbolp->nullableRuleStackp at indice %d is not PTR (internal type: %d)", rulei, GENERICSTACKITEMTYPE(symbolp->nullableRuleStackp, 0));
            goto err;
          }
          rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(symbolp->nullableRuleStackp, rulei);
          if (GENERICSTACK_USED(rulep->rhsStackp) <= 0) {
            foundEmptyb = 1;
            symbolp->nullableActions = rulep->actions;
#ifndef MARPAESLIF_NTRACE
            if (symbolp->nullableActions != NULL) {
              MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is %s", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
            } else {
              MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is grammar's default", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
            }
#endif
          }
        }
        if (! foundEmptyb) {
          short doneFirstSemanticb = 0;
          char *firstSemantics;

          /* None of the rules is empty. Then the all must have the same semantic */
          for (rulei = 0; rulei < GENERICSTACK_USED(symbolp->nullableRuleStackp); rulei++) {
            if (! GENERICSTACK_IS_PTR(symbolp->nullableRuleStackp, rulei)) {
              /* Impossible */
              MARPAESLIF_ERRORF(marpaESLIFp, "symbolp->nullableRuleStackp at indice %d is not PTR (internal type: %d)", rulei, GENERICSTACKITEMTYPE(symbolp->nullableRuleStackp, 0));
              goto err;
            }
            rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(symbolp->nullableRuleStackp, rulei);
            if (! doneFirstSemanticb) {
              firstSemantics = rulep->actions;
              doneFirstSemanticb = 1;
            } else {
              /* This is is ok if it is NULL btw */
              if (firstSemantics != rulep->actions) {
                MARPAESLIF_ERRORF(marpaESLIFp, "When nulled, symbol %d (%s) can have more than once semantic, and this is not allowed", symbolp->idi, symbolp->descp->asciis);
                goto err;
              }
            }
          }
          symbolp->nullableActions = firstSemantics;
#ifndef MARPAESLIF_NTRACE
          if (symbolp->nullableActions != NULL) {
            MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is %s", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
          } else {
            MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Nullable semantic of symbol %d (%s) is grammar's default", symbolp->idi, symbolp->descp->asciis, symbolp->nullableActions);
          }
#endif
        }
      }
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
        /* It is guaranteed that asciishowl is >= 1 - c.f. _marpaESLIF_rule_createshowv() */
        rulep->asciishows[0] = '\0';
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
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descEncodings, char *descs, size_t descl, short latmb, char *defaultSymbolActions, char *defaultRuleActions, char *defaultFreeActions, char *defaultDiscardEvents, short defaultDiscardEventb)
/*****************************************************************************/
{
  static const char             *funcs          = "_marpaESLIF_grammar_newp";
  genericLogger_t               *genericLoggerp = NULL;
  marpaESLIF_grammar_t          *grammarp       = NULL;
  marpaESLIF_stringGenerator_t  marpaESLIF_stringGenerator;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building ESLIF grammar"); */

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->marpaESLIFp                 = marpaESLIFp;
  grammarp->selfp                       = NULL;
  grammarp->leveli                      = leveli;
  grammarp->descp                       = NULL;
  grammarp->latmb                       = latmb;
  grammarp->marpaWrapperGrammarStartp   = NULL;
  grammarp->marpaWrapperGrammarDiscardp = NULL;
  grammarp->discardSymbolp              = NULL;
  grammarp->symbolStackp                = NULL;
  grammarp->ruleStackp                  = NULL;
  grammarp->defaultSymbolActions        = NULL;
  grammarp->defaultRuleActions          = NULL;
  grammarp->defaultFreeActions          = NULL;
  grammarp->defaultDiscardEvents        = NULL;
  grammarp->defaultDiscardEventb        = defaultDiscardEventb;
  grammarp->starti                      = 0;    /* Filled during grammar validation */
  grammarp->ruleip                      = NULL; /* Filled by grammar validation */
  grammarp->rulel                       = 0;    /* Filled by grammar validation */
  grammarp->haveRejectionb              = 0;    /* Filled by grammar validation */
  grammarp->nbupdateviaStarti           = 0;    /* Used by ESLIF grammar actions */
  grammarp->nbupdateviaLexemei          = 0;    /* Used by ESLIF grammar actions */
  grammarp->nbupdateviaDiscardi         = 0;    /* Used by ESLIF grammar actions */

  grammarp->marpaWrapperGrammarStartp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarStartp == NULL) {
    goto err;
  }

  /* ----------- Grammar description ------------- */
  if ((descs == NULL) || (descl <= 0)) {
    /* Generate a default description */
    marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;
    marpaESLIF_stringGenerator.s           = NULL;
    marpaESLIF_stringGenerator.l           = 0;
    marpaESLIF_stringGenerator.okb         = 0;

    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      goto err;
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "Grammar level %d (autogenerated description)", leveli);
    if (! marpaESLIF_stringGenerator.okb) {
      goto err;
    }
    grammarp->descp = _marpaESLIF_string_newp(marpaESLIFp, "ASCII" /* We KNOW we generated an ASCII stringy */, marpaESLIF_stringGenerator.s, strlen(marpaESLIF_stringGenerator.s), 1 /* asciib */);
    free(marpaESLIF_stringGenerator.s);
  } else {
    grammarp->descp = _marpaESLIF_string_newp(marpaESLIFp, descEncodings, descs, descl, 1 /* asciib */);
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

  if (defaultSymbolActions != NULL) {
    grammarp->defaultSymbolActions = strdup(defaultSymbolActions);
    if (grammarp->defaultSymbolActions == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  if (defaultRuleActions != NULL) {
    grammarp->defaultRuleActions = strdup(defaultRuleActions);
    if (grammarp->defaultRuleActions == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  if (defaultFreeActions != NULL) {
    grammarp->defaultFreeActions = strdup(defaultFreeActions);
    if (grammarp->defaultFreeActions == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  if (defaultDiscardEvents != NULL) {
    grammarp->defaultDiscardEvents = strdup(defaultDiscardEvents);
    if (grammarp->defaultDiscardEvents == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  grammarp->selfp = grammarp;
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
    if (grammarp->defaultSymbolActions != NULL) {
      free(grammarp->defaultSymbolActions);
    }
    if (grammarp->defaultRuleActions != NULL) {
      free(grammarp->defaultRuleActions);
    }
    if (grammarp->defaultFreeActions != NULL) {
      free(grammarp->defaultFreeActions);
    }
    if (grammarp->defaultDiscardEvents != NULL) {
      free(grammarp->defaultDiscardEvents);
    }
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
static inline void _marpaESLIFrecognizer_lexemeStack_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFrecognizer_lexemeStack_freev";

  if (lexemeStackp != NULL) {
    marpaESLIFRecognizerp->callstackCounteri++;
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

    _marpaESLIFrecognizer_lexemeStack_resetv(marpaESLIFRecognizerp, lexemeStackp);
    GENERICSTACK_FREE(lexemeStackp);

    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
    marpaESLIFRecognizerp->callstackCounteri--;
  }

}

/*****************************************************************************/
static inline void _marpaESLIFrecognizer_lexemeStack_resetv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFrecognizer_lexemeStack_resetv";
  int                i;
  int                usedi;

  if (lexemeStackp != NULL) {

    /* This is vicious but it can happen that we are called with marpaESLIFRecognizerp == NULL, in case of */
    /* error recovery - c.f. _marpaESLIF_terminal_newp. */
    if (marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizerp->callstackCounteri++;
      MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");
    }

    usedi = (int) GENERICSTACK_USED(lexemeStackp);
    for (i = usedi - 1; i >= 0; i--) {
      /* if it failed, we will have a memory leak - error will be logged */
      /* In practice it never fails, because _marpaESLIFRecognizer_lexemeStack_i_resetb() */
      /* frees() only something that it recognizes, and afterwards set it to NA - and the */
      /* to NA cannot fail, because this place place in stack memory is already there. */
      /* Anyway, suppose that it would fail, I repeat, at most there is a memory leak */
      _marpaESLIFRecognizer_lexemeStack_i_resetb(marpaESLIFRecognizerp, lexemeStackp, i);
      GENERICSTACK_USED(lexemeStackp)--;
    }

    if (marpaESLIFRecognizerp != NULL) {
      MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
      marpaESLIFRecognizerp->callstackCounteri--;
    }
  }

}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFRecognizer_lexemeStack_i_resetb";
  short              rcb;

  /* This is vicious but it can happen that we are called with marpaESLIFRecognizerp == NULL, in case of */
  /* error recovery - c.f. _marpaESLIF_terminal_newp. */
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizerp->callstackCounteri++;
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");
  }

  if (lexemeStackp != NULL) {
    if (GENERICSTACK_IS_ARRAY(lexemeStackp, i)) {
      GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_GET_ARRAY(lexemeStackp, i);
      if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
#ifndef MARPAESLIF_NTRACE
        if (marpaESLIFRecognizerp != NULL) {
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing %p->[%d] = {%p, %d}", lexemeStackp, i, GENERICSTACK_ARRAY_PTR(array), GENERICSTACK_ARRAY_LENGTH(array));
        }
#endif
        free(GENERICSTACK_ARRAY_PTR(array));
      }
#ifndef MARPAESLIF_NTRACE
      if (marpaESLIFRecognizerp != NULL) {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Resetting %p->[%d]", lexemeStackp, i);
      }
#endif
      GENERICSTACK_SET_NA(lexemeStackp, i);
      if (GENERICSTACK_ERROR(lexemeStackp)) {
        if (marpaESLIFRecognizerp != NULL) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "lexemeStackp %p->[%d] set failure, %s", lexemeStackp, i, strerror(errno));
          }
        goto err;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (marpaESLIFRecognizerp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
    marpaESLIFRecognizerp->callstackCounteri--;
  }
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_setarraypb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, GENERICSTACKITEMTYPE2TYPE_ARRAYP arrayp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFRecognizer_lexemeStack_i_setarraypb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFRecognizer_lexemeStack_i_resetb(marpaESLIFRecognizerp, lexemeStackp, i)) {
    goto err;
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setting %p->[%d] = {%p, %d}", lexemeStackp, i, GENERICSTACK_ARRAYP_PTR(arrayp), GENERICSTACK_ARRAYP_LENGTH(arrayp));
  GENERICSTACK_SET_ARRAYP(lexemeStackp, arrayp, i);
  if (GENERICSTACK_ERROR(lexemeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "lexemeStackp %p->[%d] set failure, %s", lexemeStackp, i, strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char              *funcs = "_marpaESLIFRecognizer_lexemeStack_i_setb";
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  int                             rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  GENERICSTACK_ARRAY_PTR(array)    = p;
  GENERICSTACK_ARRAY_LENGTH(array) = sizel;
  rcb = _marpaESLIFRecognizer_lexemeStack_i_setarraypb(marpaESLIFRecognizerp, lexemeStackp, i, &array);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_moveb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackDstp, int dsti, genericStack_t *lexemeStackSrcp, int srci)
/*****************************************************************************/
{
  static const char              *funcs = "_marpaESLIFRecognizer_lexemeStack_i_moveb";
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  short                           rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  GENERICSTACK_ARRAY_PTR(array)    = NULL;
  GENERICSTACK_ARRAY_LENGTH(array) = 0;
  
  if (! GENERICSTACK_IS_ARRAY(lexemeStackSrcp, srci)) {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexemeStackSrcp %p at indice %d", _marpaESLIF_genericStack_i_types(lexemeStackSrcp, srci), lexemeStackSrcp, srci);
    goto err;
  }
  array = GENERICSTACK_GET_ARRAY(lexemeStackSrcp, srci);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Got %p->[%d] = {%p, %d}", lexemeStackSrcp, srci, GENERICSTACK_ARRAY_PTR(array), GENERICSTACK_ARRAY_LENGTH(array));
  GENERICSTACK_SET_NA(lexemeStackSrcp, srci);
  if (GENERICSTACK_ERROR(lexemeStackSrcp)) {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "lexemeStackSrcp set failure, %s", strerror(errno));
    goto err;
  }
  if (! _marpaESLIFRecognizer_lexemeStack_i_setarraypb(marpaESLIFRecognizerp, lexemeStackDstp, dsti, &array)) {
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
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descEncodings, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb, char *actions, short passthroughb)
/*****************************************************************************/
{
  static const char               *funcs          = "_marpaESLIF_rule_newp";
  genericStack_t                  *symbolStackp   = grammarp->symbolStackp;
  marpaESLIF_rule_t               *rulep          = NULL;
  short                            symbolFoundb   = 0;
  genericLogger_t                 *genericLoggerp = NULL;
  marpaESLIF_stringGenerator_t     marpaESLIF_stringGenerator;
  short                            separatorFoundb;
  marpaESLIF_symbol_t             *symbolp;
  marpaWrapperGrammarRuleOption_t  marpaWrapperGrammarRuleOption;
  size_t                           i;
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
  rulep->exceptionStackp = NULL;
  rulep->actions         = NULL;
  rulep->ranki           = ranki;
  rulep->nullRanksHighb  = nullRanksHighb;
  rulep->sequenceb       = sequenceb;
  rulep->properb         = properb;
  rulep->minimumi        = minimumi;
  rulep->passthroughb    = passthroughb;

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

  GENERICSTACK_NEW(rulep->exceptionStackp);
  if (GENERICSTACK_ERROR(rulep->exceptionStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* Fill rhs symbol stack */
  if (rhsip != NULL) {
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
    }
  }
  
  /* Fill exception symbol stack */
  if (exceptionip != NULL) {
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

  /* ---------------- Action ---------------- */
  if (actions != NULL) {
    rulep->actions = strdup(actions);
    if (rulep->actions == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  /* -------- Rule Description -------- */
  if ((descs == NULL) || (descl <= 0)) {
    /* Generate a default description */
    marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;
    marpaESLIF_stringGenerator.s           = NULL;
    marpaESLIF_stringGenerator.l           = 0;
    marpaESLIF_stringGenerator.okb         = 0;

    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      goto err;
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "Rule No %d (autogenerated description)", rulep->idi);
    if (! marpaESLIF_stringGenerator.okb) {
      goto err;
    }
    rulep->descp = _marpaESLIF_string_newp(marpaESLIFp, "ASCII" /* We KNOW we generated an ASCII stringy */, marpaESLIF_stringGenerator.s, strlen(marpaESLIF_stringGenerator.s), 1 /* asciib */);
    free(marpaESLIF_stringGenerator.s);
  } else {
    rulep->descp = _marpaESLIF_string_newp(marpaESLIFp, descEncodings, descs, descl, 1 /* asciib */);
  }
  if (rulep->descp == NULL) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_rule_freev(rulep);
  rulep = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rulep); */
  GENERICLOGGER_FREE(genericLoggerp);
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
    if (rulep->actions != NULL) {
      free(rulep->actions);
    }
    /* In the rule structure, lhsp, rhsStackp and exceptionStackp contain shallow pointers */
    /* Only the stack themselves should be freed. */
    /*
    _marpaESLIF_symbol_freev(rulep->lhsp);
    _marpaESLIF_symbolStack_freev(rulep->rhsStackp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, exceptionStackp);
    */
    GENERICSTACK_FREE(rulep->rhsStackp);
    GENERICSTACK_FREE(rulep->exceptionStackp);
    free(rulep);
  }
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  static const char   *funcs   = "_marpaESLIF_symbol_newp";
  marpaESLIF_symbol_t *symbolp = NULL;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building symbol"); */

  symbolp = (marpaESLIF_symbol_t *) malloc(sizeof(marpaESLIF_symbol_t));
  if (symbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  symbolp->type   = MARPAESLIF_SYMBOL_TYPE_NA;
  /* Union itself is undetermined at this stage */
  symbolp->startb                 = 0;
  symbolp->discardb               = 0;
  symbolp->lhsb                   = 0;
  symbolp->topb                   = 0; /* Revisited by grammar validation */
  symbolp->isDiscardTopb          = 0; /* Ditto */
  symbolp->idi                    = -1;
  symbolp->descp                  = NULL;
  symbolp->eventBefores           = NULL;
  symbolp->eventBeforeb           = 1; /* An event is on by default */
  symbolp->eventAfters            = NULL;
  symbolp->eventAfterb            = 1; /* An event is on by default */
  symbolp->eventPredicteds        = NULL;
  symbolp->eventPredictedb        = 1; /* An event is on by default */
  symbolp->eventNulleds           = NULL;
  symbolp->eventNulledb           = 1; /* An event is on by default */
  symbolp->eventCompleteds        = NULL;
  symbolp->eventCompletedb        = 1; /* An event is on by default */
  symbolp->discardEvents          = NULL;
  symbolp->discardEventb          = 1; /* An event is on by default */
  symbolp->lookupLevelDeltai      = 1;   /* Default lookup is the next grammar level */
  symbolp->lookupGrammarStringp   = NULL;
  symbolp->resolvedLeveli         = -1; /* This will be overwriten by _marpaESLIFGrammar_validateb() and used only when symbol is a lexeme from another grammar */
  symbolp->priorityi              = 0; /* Default priority is 0 */
  symbolp->actions                = NULL;
  symbolp->nbupdatei              = 0;
  symbolp->nullableRuleStackp     = NULL;
  symbolp->nullableActions        = NULL;

  GENERICSTACK_NEW(symbolp->nullableRuleStackp);
  if (GENERICSTACK_ERROR(symbolp->nullableRuleStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolp->nullableRuleStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  goto done;

 err:
  _marpaESLIF_symbol_freev(symbolp);
  symbolp = NULL;

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
    if (symbolp->eventBefores != NULL) {
      free(symbolp->eventBefores);
    }
    if (symbolp->eventAfters != NULL) {
      free(symbolp->eventAfters);
    }
    if (symbolp->eventPredicteds) {
      free(symbolp->eventPredicteds);
    }
    if (symbolp->eventNulleds) {
      free(symbolp->eventNulleds);
    }
    if (symbolp->eventCompleteds) {
      free(symbolp->eventCompleteds);
    }
    if (symbolp->discardEvents) {
      free(symbolp->discardEvents);
    }
    if (symbolp->actions != NULL) {
      free(symbolp->actions);
    }
    if (symbolp->nullableRuleStackp != NULL) {
      GENERICSTACK_FREE(symbolp->nullableRuleStackp);
    }
    /* Take care, when not NULL, this will be anyway a shallow pointer */
    /*
    if (symbolp->nullableActions != NULL) {
      free(symbolp->nullableActions);
    }
    */
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
    if (terminalp->regex.match_datap != NULL) {
      pcre2_match_data_free(terminalp->regex.match_datap);
    }
    if (terminalp->regex.patternp != NULL) {
      pcre2_code_free(terminalp->regex.patternp);
    }
    free(terminalp);
  }
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp)
/*****************************************************************************/
{
  static const char     *funcs       = "marpaESLIF_newp";
  marpaESLIF_grammar_t  *grammarp    = NULL;
  marpaESLIF_t          *marpaESLIFp = NULL;
  genericLogger_t       *genericLoggerp;
  genericLoggerLevel_t   genericLoggerLeveli;

  if (marpaESLIFOptionp == NULL) {
    marpaESLIFOptionp = &marpaESLIFOption_default_template;
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

  marpaESLIFp->marpaESLIFOption   = *marpaESLIFOptionp;
  marpaESLIFp->marpaESLIFGrammarp = NULL;
  marpaESLIFp->anycharp           = NULL;
  marpaESLIFp->utf8bomp           = NULL;
  marpaESLIFp->newlinep           = NULL;

  /* Create internal anychar regex */
  marpaESLIFp->anycharp = _marpaESLIF_terminal_newp(marpaESLIFp,
                                                    NULL, /* grammarp */
                                                    MARPAESLIF_EVENTTYPE_NONE, /* eventSeti */
                                                    "ASCII", /* We KNOW this is an ASCII thingy */
                                                    INTERNAL_ANYCHAR_PATTERN, /* descs */
                                                    strlen(INTERNAL_ANYCHAR_PATTERN), /* descl */
                                                    MARPAESLIF_TERMINAL_TYPE_REGEX, /* type */
                                                    MARPAESLIF_REGEX_OPTION_DOTALL|MARPAESLIF_REGEX_OPTION_UTF, /* opti */
                                                    INTERNAL_ANYCHAR_PATTERN, /* utf8s */
                                                    strlen(INTERNAL_ANYCHAR_PATTERN), /* utf8l */
                                                    NULL, /* testFullMatchs */
                                                    NULL  /* testPartialMatchs */
                                                    );
  if (marpaESLIFp->anycharp == NULL) {
    goto err;
  }

  /* Create internal utf8bom regex */
  marpaESLIFp->utf8bomp = _marpaESLIF_terminal_newp(marpaESLIFp,
                                                    NULL /* grammarp */,
                                                    MARPAESLIF_EVENTTYPE_NONE, /* eventSeti */
                                                    "ASCII", /* We KNOW this is an ASCII thingy */
                                                    INTERNAL_UTF8BOM_PATTERN, /* descs */
                                                    strlen(INTERNAL_UTF8BOM_PATTERN), /* descl */
                                                    MARPAESLIF_TERMINAL_TYPE_REGEX, /* type */
                                                    MARPAESLIF_REGEX_OPTION_UTF, /* opti */
                                                    INTERNAL_UTF8BOM_PATTERN, /* utf8s */
                                                    strlen(INTERNAL_UTF8BOM_PATTERN), /* utf8l */
                                                    NULL, /* testFullMatchs */
                                                    NULL  /* testPartialMatchs */
                                                    );
  if (marpaESLIFp->utf8bomp == NULL) {
    goto err;
  }

  /* Create internal newline regex */
  /* Please note that the newline regexp does NOT require UTF-8 correctness -; */
  marpaESLIFp->newlinep = _marpaESLIF_terminal_newp(marpaESLIFp,
                                                    NULL /* grammarp */,
                                                    MARPAESLIF_EVENTTYPE_NONE, /* eventSeti */
                                                    "ASCII", /* We KNOW this is an ASCII thingy */
                                                    INTERNAL_NEWLINE_PATTERN /* descs */,
                                                    strlen(INTERNAL_NEWLINE_PATTERN) /* descl */,
                                                    MARPAESLIF_TERMINAL_TYPE_REGEX, /* type */
                                                    MARPAESLIF_REGEX_OPTION_NA, /* opti */
                                                    INTERNAL_NEWLINE_PATTERN, /* utf8s */
                                                    strlen(INTERNAL_NEWLINE_PATTERN), /* utf8l */
                                                    NULL, /* testFullMatchs */
                                                    NULL  /* testPartialMatchs */
                                                    );
  if (marpaESLIFp->newlinep == NULL) {
    goto err;
  }

  /* Create internal ESLIF grammar - it is important to set the option first */
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

  /* When we bootstrap we do no want to log unless there is an error */
  if (marpaESLIFOptionp->genericLoggerp != NULL) {
    genericLoggerLeveli = genericLogger_logLevel_geti(marpaESLIFOptionp->genericLoggerp);
    genericLogger_logLevel_seti(marpaESLIFOptionp->genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO);
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
  /* Restore log-level if user provided one */
  if (marpaESLIFOptionp->genericLoggerp != NULL) {
    genericLogger_logLevel_seti(marpaESLIFOptionp->genericLoggerp, genericLoggerLeveli);
  }
  
  _marpaESLIF_grammar_freev(grammarp);
#ifndef MARPAESLIF_NTRACE
  if ((marpaESLIFp != NULL) && (genericLoggerp != NULL)) {
    int     ngrammari;
    int    *ruleip;
    size_t  rulel;
    int     i;
    size_t  l;

    if (marpaESLIFGrammar_ngrammari(marpaESLIFp->marpaESLIFGrammarp, &ngrammari)) {
      for (i = 0; i < ngrammari; i++) {
        if (marpaESLIFGrammar_rules_by_grammarb(marpaESLIFp->marpaESLIFGrammarp, &ruleip, &rulel, i /* grammari */, NULL /* descp */)) {
          GENERICLOGGER_TRACEF(genericLoggerp, "[%s] -------------------------", funcs);
          GENERICLOGGER_TRACEF(genericLoggerp, "[%s] ESLIF grammar at level %d:", funcs, i);
          GENERICLOGGER_TRACEF(genericLoggerp, "[%s] -------------------------", funcs);
          for (l = 0; l < rulel; l++) {
            char *ruleshows;
            if (marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFp->marpaESLIFGrammarp, l, &ruleshows, i, NULL /* descp */)) {
              GENERICLOGGER_TRACEF(genericLoggerp, "[%s] %s", funcs, ruleshows);
            }
          }
        }
      }
    }
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
    _marpaESLIF_terminal_freev(marpaESLIFp->anycharp);
    _marpaESLIF_terminal_freev(marpaESLIFp->utf8bomp);
    _marpaESLIF_terminal_freev(marpaESLIFp->newlinep);
    free(marpaESLIFp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_terminal_t *terminalp, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char              *funcs = "_marpaESLIFRecognizer_regex_matcherb";
  marpaESLIF_t                   *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;

  marpaESLIF_matcher_value_t      rci;
  marpaESLIF_regex_t              marpaESLIF_regex;
  int                             pcre2Errornumberi;
  PCRE2_UCHAR                     pcre2ErrorBuffer[256];
  PCRE2_SIZE                     *pcre2_ovectorp;
  size_t                          matchedLengthl;
  marpaESLIF_uint32_t             pcre2_optioni;
  short                           binmodeb;
  short                           rcb;
#ifdef PCRE2_CONFIG_JIT
  /* A priori we can call JIT - this is switched off in the exceptional case of */
  /* needing anchoring but the JIT pattern was compiled without anchoring */
  short                       canJitb = 1;
#endif

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_regex = terminalp->regex;

    /* If the regexp is working in UTF mode then we check that character conversion   */
    /* was done. This is how we are sure that calling regexp with PCRE2_NO_UTF_CHECK  */
    /* is ok: we have done ourself the UTF-8 validation on the subject.               */
    /* This is a boost in performance also when we are using the built-in PCRE2: the  */
    /* later is never compiled with JIT support, which mean that UTF checking is done */
    /* by default, unless PCRE2_NO_UTF_CHECK is set.                                  */
    if (marpaESLIF_regex.utfb) {                     /* UTF-8 correctness is required */
      if (! *(marpaESLIFRecognizerp->utfbp)) {
        pcre2_optioni = pcre2_option_binary_default;  /* We have done no conversion : PCRE2 will check */
        binmodeb = 1;
      } else {
        pcre2_optioni = pcre2_option_char_default;    /* We made sure this is ok */
        binmodeb = 0;
      }
    } else {
      pcre2_optioni = pcre2_option_binary_default;    /* Not needed */
      binmodeb = 1;
    }

    /* --------------------------------------------------------- */
    /* Anchored regex...                                         */
    /* --------------------------------------------------------- */
    /*
     Patterns are always compiled with PCRE2_ANCHORED by default,
     except when there is the "A" modifier. In this case, it allowed
     to execute the regex ONLY if the whole stream was read in one
     call to the user's read callback.
    */
    if (! terminalp->regex.isAnchoredb) {
      if (! *(marpaESLIFRecognizerp->noAnchorIsOkbp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s: You used the \"A\" modifier to set the pattern non-anchored, but then you must read the whole input in one go", terminalp->descp->asciis);
        goto err;
      }
    }
    
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
      pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,    /* code */
                                          (PCRE2_SPTR) inputs,          /* subject */
                                          (PCRE2_SIZE) inputl,          /* length */
                                          (PCRE2_SIZE) 0,               /* startoffset */
                                          pcre2_optioni,                /* options */
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
                                      pcre2_optioni,                /* options */
                                      marpaESLIF_regex.match_datap, /* match data */
                                      NULL                          /* match context - used default */
                                      );
#ifdef PCRE2_CONFIG_JIT
    }
#endif

    /* In any case - set UTF buffer correctness if needed and if possible */
    if (binmodeb && marpaESLIF_regex.utfb) {
      if ((pcre2Errornumberi >= 0) || (pcre2Errornumberi == PCRE2_ERROR_NOMATCH)) {
        /* Either regex is successful, either it failed with the accepted failure code PCRE2_ERROR_NOMATCH */
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s: UTF-8 correctness successful and remembered", terminalp->descp->asciis);
        *(marpaESLIFRecognizerp->utfbp) = 1;
      }
    }

    if (eofb) {
      if (pcre2Errornumberi < 0) {
        /* Only PCRE2_ERROR_NOMATCH is an acceptable error. */
        if (pcre2Errornumberi != PCRE2_ERROR_NOMATCH) {
          pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
          MARPAESLIF_WARNF(marpaESLIFp, "%s: Uncaught pcre2 match failure: %s", terminalp->descp->asciis, pcre2ErrorBuffer);
        }
        rci = MARPAESLIF_MATCH_FAILURE;
      } else {
        /* Check the length of matched data */
        if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_get_ovector_count returned no number of pairs of values", terminalp->descp->asciis);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_get_ovector_pointer returned NULL", terminalp->descp->asciis);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: Empty match when it is configured as not possible", terminalp->descp->asciis);
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
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_get_ovector_count returned no number of pairs of values", terminalp->descp->asciis);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: pcre2_get_ovector_pointer returned NULL", terminalp->descp->asciis);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s: Empty match when it is configured as not possible", terminalp->descp->asciis);
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
        /* Please note that we NEVER ask for UTF-8 correctness here, because previous section */
        /* made sure it has always been done. */
#ifdef PCRE2_CONFIG_JIT
        if (marpaESLIF_regex.jitPartialb) {
          pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,    /* code */
                                              (PCRE2_SPTR) inputs,          /* subject */
                                              (PCRE2_SIZE) inputl,          /* length */
                                              (PCRE2_SIZE) 0,               /* startoffset */
                                              pcre2_option_partial_default, /* options - this one is supported in JIT mode */
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
                                          pcre2_option_partial_default, /* options - this one is supported in JIT mode */
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

  if ((rci == MARPAESLIF_MATCH_OK) && (marpaESLIFValueResultp != NULL)) {
    marpaESLIFValueResultp->type = MARPAESLIF_STACK_TYPE_ARRAY;
    marpaESLIFValueResultp->u.p = malloc(matchedLengthl);
    if (marpaESLIFValueResultp->u.p == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(marpaESLIFValueResultp->u.p, (void *) inputs, matchedLengthl);
    marpaESLIFValueResultp->sizel = matchedLengthl;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_meta_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIF_matcher_value_t *rcip, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module, and the cause of recursion -; */
  static const char              *funcs              = "_marpaESLIFRecognizer_meta_matcherb";
  marpaESLIF_t                   *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  short                           rcb;
  marpaESLIFGrammar_t             marpaESLIFGrammar; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t            grammar;
  marpaESLIF_grammar_t           *grammarp;
  marpaESLIFRecognizerOption_t    marpaESLIFRecognizerOption; /* This is an internal recognizer */
  marpaESLIFValueOption_t         marpaESLIFValueOption = marpaESLIFValueOption_default_template;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

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

  marpaESLIFRecognizerOption                   = marpaESLIFRecognizerp->marpaESLIFRecognizerOption;
  marpaESLIFRecognizerOption.disableThresholdb = 1;
  marpaESLIFRecognizerOption.exhaustedb        = 1;

  /* The context of an internal match is the grammar on which we try to match is the current recognizer */
  marpaESLIFValueOption.userDatavp            = (void *) marpaESLIFRecognizerp; /* Used by _marpaESLIF_lexeme_freeCallbackv */

  if (! _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, 0 /* discardb */, marpaESLIFRecognizerp /* parentRecognizerp */, exhaustedbp, marpaESLIFValueResultp)) {
    goto err;
  }

  if (rcip != NULL) {
    *rcip = MARPAESLIF_MATCH_OK;
  }
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char *   funcs = "_marpaESLIFRecognizer_symbol_matcherb";
  short                 rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  switch (symbolp->type) {
  case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
    /* A terminal matcher NEVER updates the stream : inputs, inputl and eof can be passed as is. */
    if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp,
                                               symbolp->u.terminalp,
                                               marpaESLIFRecognizerp->inputs,
                                               marpaESLIFRecognizerp->inputl,
                                               *(marpaESLIFRecognizerp->eofbp),
                                               rcip,
                                               marpaESLIFValueResultp)) {
      goto err;
    }
    break;
  case MARPAESLIF_SYMBOL_TYPE_META:
    /* A terminal matcher MAY recursively call other recognizers, reading new data, etc... : this will update current recognizer inputs, inputl and eof. */
    /* The result will be a parse tree value, at indice 0 of outputStackp */
    if (! _marpaESLIFRecognizer_meta_matcherb(marpaESLIFRecognizerp,
                                              symbolp->resolvedLeveli, /* Computed by grammar validator */
                                              symbolp->u.metap->marpaWrapperGrammarLexemeClonep,
                                              NULL /* terminalp */,
                                              symbolp->u.metap,
                                              rcip,
                                              exhaustedbp,
                                              marpaESLIFValueResultp)) {
      goto err;
    }
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Unknown symbol type %d", symbolp->type);
    goto err;
  }


#ifndef MARPAESLIF_NTRACE
  if ((rcip != NULL) && (*rcip == MARPAESLIF_MATCH_OK) && (marpaESLIFValueResultp != NULL)) {
    switch (marpaESLIFValueResultp->type) {
    case MARPAESLIF_STACK_TYPE_UNDEF:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is UNDEF", symbolp->descp->asciis);
      break;
    case MARPAESLIF_STACK_TYPE_CHAR:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is CHAR: %c (0x%02x)", symbolp->descp->asciis, marpaESLIFValueResultp->u.c, (unsigned int) marpaESLIFValueResultp->u.c);
      break;
    case MARPAESLIF_STACK_TYPE_SHORT:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is SHORT: %d", symbolp->descp->asciis, (int) marpaESLIFValueResultp->u.b);
      break;
    case MARPAESLIF_STACK_TYPE_INT:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is INT: %d", symbolp->descp->asciis, marpaESLIFValueResultp->u.i);
      break;
    case MARPAESLIF_STACK_TYPE_LONG:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is LONG: %ld", symbolp->descp->asciis, marpaESLIFValueResultp->u.l);
      break;
    case MARPAESLIF_STACK_TYPE_FLOAT:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is FLOAT: %f", symbolp->descp->asciis, (double) marpaESLIFValueResultp->u.f);
      break;
    case MARPAESLIF_STACK_TYPE_DOUBLE:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is DOUBLE: %f", symbolp->descp->asciis, marpaESLIFValueResultp->u.d);
      break;
    case MARPAESLIF_STACK_TYPE_PTR:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is PTR: %p", symbolp->descp->asciis, marpaESLIFValueResultp->u.p);
      break;
    case MARPAESLIF_STACK_TYPE_PTR_SHALLOW:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is PTR (shallow): %p", symbolp->descp->asciis, marpaESLIFValueResultp->u.p);
      break;
    case MARPAESLIF_STACK_TYPE_ARRAY:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is ARRAY: {%p,%ld}", symbolp->descp->asciis, marpaESLIFValueResultp->u.p, (unsigned long) marpaESLIFValueResultp->sizel);
      if (marpaESLIFValueResultp->sizel > 0) {
        MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp, "Match for ", symbolp->descp->asciis, marpaESLIFValueResultp->u.p, marpaESLIFValueResultp->sizel, 1 /* traceb */);
      }
      break;
    case MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is ARRAY (shallow): {%p,%ld}", symbolp->descp->asciis, marpaESLIFValueResultp->u.p, (unsigned long) marpaESLIFValueResultp->sizel);
      if (marpaESLIFValueResultp->sizel > 0) {
        MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp, "Match for ", symbolp->descp->asciis, marpaESLIFValueResultp->u.p, marpaESLIFValueResultp->sizel, 1 /* traceb */);
      }
    default:
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match result for %s is UNKNOWN (!?)", symbolp->descp->asciis);
      break;
    }
  }
#endif

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  /*
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  */
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

#ifndef MARPAESLIF_NTRACE
/*****************************************************************************/
static void _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs)
/*****************************************************************************/
{
  static const char *funcs  = "_marpaESLIF_tconvTraceCallback";
  marpaESLIF_t *marpaESLIFp = (marpaESLIF_t *) userDatavp;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s", msgs);
}
#endif

/*****************************************************************************/
static inline char *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl)
/*****************************************************************************/
{
  static const char      *funcs  = "_marpaESLIF_utf82printableascii_newp";
  size_t                  asciil;
  char                   *p;
  char                   *asciis;
  unsigned char           c;

  asciis = _marpaESLIF_charconvp(marpaESLIFp, "ASCII//TRANSLIT//IGNORE", "UTF-8", descs, descl, &asciil, NULL /* fromEncodingsp */, NULL /* tconvpp */);
  if (asciis == NULL) {
    asciis = (char *) _marpaESLIF_utf82printableascii_defaultp;
    asciil = strlen(asciis);
  } else {
    /* We are doing this only on descriptions - which are always small amount of bytes  */
    /* (will the user ever write a description taking megabytes !?). Therefore if it ok */
    /* to remove by hand bom and realloc if necessary.                                  */

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
static inline char *_marpaESLIF_charconvp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *srcs, size_t srcl, size_t *dstlp, char **fromEncodingsp, tconv_t *tconvpp)
/*****************************************************************************/
/* _marpaESLIF_charconvp is ALWAYS returning a non-NULL pointer in case of success (it allocates always one byte more, and put a NUL in it). */
/* Still, the number of converted bytes remain correct. */
{
  static const char *funcs       = "_marpaESLIF_charconvp";
  char              *inbuforigp  = srcs;
  size_t             inleftorigl = srcl;
  char              *outbuforigp = NULL;
  size_t             outbuforigl = 0;
  tconv_option_t     tconvOption = { NULL /* charsetp */, NULL /* convertp */, NULL /* traceCallbackp */, NULL /* traceUserDatavp */ };
  tconv_t            tconvp      = NULL;
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
  if (tconvpp != NULL) {
    tconvp = *tconvpp;
  }
  if (tconvp == NULL) {
    tconvp = tconv_open_ext(toEncodings, fromEncodings, &tconvOption);
    if (tconvp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "tconv_open failure, %s", strerror(errno));
      goto err;
    }
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
  outbuforigp = (char *) malloc(srcl + 1);
  if (outbuforigp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  outbuforigp[srcl] = '\0';
  outbuforigl = srcl;

  /* We want to translate descriptions in trace or error cases - these are short things, and */
  /* it does not really harm if we redo the whole translation stuff in case of E2BIG:        */
  /* - in trace mode it is documented that performance is awful                              */
  /* - in error mode this call will happen once                                              */

  inbufp   = inbuforigp;
  inleftl  = inleftorigl;
  outbufp  = outbuforigp;
  outleftl = outbuforigl;
  
  while (1) {
  again:
    nconvl = tconv(tconvp, (inbufp != NULL) ? &inbufp : NULL, &inleftl, &outbufp, &outleftl);

    if (nconvl == (size_t) -1) {
      char  *tmps;
      size_t outleftdeltal;
      size_t outbufdeltal;
      
      /* The only acceptable error is E2BIG */
      if (errno != E2BIG) {
	MARPAESLIF_ERRORF(marpaESLIFp, "tconv failure, %s", tconv_error(tconvp));
	goto err;
      }
      /* Try to alloc more */
      if (outbuforigl > 0) {
        outbuforigl *= 2;
        outleftdeltal = outbuforigl;
      } else {
        outbuforigl = 1023;    /* Totally subjective number, just to avoid to do 1, then 2, then 4, then 8, then etc... */
        outleftdeltal = 1023;  /* when 1024 is suppoidly enough. */
      }
      outbufdeltal = outbufp - outbuforigp; /* Destination buffer is never NULL because of the +1 above */
      /* Will this ever happen ? */
      if (outbuforigl < srcl) {
	MARPAESLIF_ERROR(marpaESLIFp, "size_t flip");
	goto err;
      }
      /* Note the "+ 1" */
      tmps = realloc(outbuforigp, outbuforigl + 1); /* Still the +1 to put a NUL just to ease debug of UTF-8 but also its makes sure that ASCII string are ALWAYS NUL terminated */
      if (tmps == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
	goto err;
      }
      outbuforigp = tmps;
      outbuforigp[outbuforigl] = '\0';
      outleftl   += outleftdeltal;
      outbufp = outbuforigp + outbufdeltal;
      goto again;
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

  if (dstlp != NULL) {
    *dstlp = outbufp - outbuforigp;
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
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Encoding guessed to %s", *fromEncodingsp);
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
  if (outbuforigp != NULL) {
    free(outbuforigp);
  }
  outbuforigp = NULL;

 done:
  if (tconvpp != NULL) {
    *tconvpp = tconvp;
  } else {
    if (tconvp != NULL) {
      if (tconv_close(tconvp) != 0) {
        MARPAESLIF_ERRORF(marpaESLIFp, "tconv_close failure, %s", strerror(errno));
      }
    }
  }

  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", outbuforigp); */
  return outbuforigp;
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp)
/*****************************************************************************/
{
  static const char                *funcs              = "marpaESLIFGrammar_newp";
  genericStack_t                   *localOutputStackp  = NULL;
  marpaESLIFGrammar_t              *marpaESLIFGrammarp = NULL;
  marpaESLIF_readerContext_t        marpaESLIF_readerContext;
  marpaESLIFRecognizerOption_t      marpaESLIFRecognizerOption = marpaESLIFRecognizerOption_default_template;
  marpaESLIFValueOption_t           marpaESLIFValueOption      = marpaESLIFValueOption_default_template;
  marpaESLIFValueResult_t           marpaESLIFValueResult;
  int                               grammari;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Grammar"); */

  if (marpaESLIFGrammarOptionp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFGrammarOptionp must be set");
    goto err;
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

  /* Our internal grammar reader callback */
  marpaESLIF_readerContext.marpaESLIFp = marpaESLIFp;
  marpaESLIF_readerContext.marpaESLIFGrammarOptionp = marpaESLIFGrammarOptionp;

  /* Overwrite things not setted in the template, or with which we want a change */
  marpaESLIFRecognizerOption.userDatavp                  = (void *) &marpaESLIF_readerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp   = _marpaESLIFReader_grammarReader;
  marpaESLIFRecognizerOption.disableThresholdb           = 1; /* No threshold warning when parsing a grammar */
  marpaESLIFRecognizerOption.newlineb                    = 1; /* Grammars are short - we can count line/columns numbers */

  marpaESLIFValueOption.userDatavp            = (void *) marpaESLIFGrammarp; /* Used by _marpaESLIF_bootstrap_freeCallbackv */
  marpaESLIFValueOption.ruleActionResolverp   = _marpaESLIF_bootstrap_ruleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp = NULL; /* We use ::shift */
  marpaESLIFValueOption.freeActionResolverp   = _marpaESLIF_bootstrap_freeActionResolver;

  /* Parser will automatically create marpaESLIFValuep and assign an internal recognizer to its userDatavp */
  /* The value of our internal parser is a grammar stack */
  if (! _marpaESLIFGrammar_parseb(marpaESLIFp->marpaESLIFGrammarp,
                                  &marpaESLIFRecognizerOption,
                                  &marpaESLIFValueOption,
                                  0, /* discardb */
                                  NULL, /* marpaESLIFRecognizerParentp */
                                  NULL, /* exhaustedbp */
                                  &marpaESLIFValueResult)) {
    goto err;
  }

  /* It is a non-sense that the grammar parsing result is not a non-NULL PTR */
  if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_PTR) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Grammar parsing result type is not PTR but %d", marpaESLIFValueResult.type);
    goto err;
  }
  if (marpaESLIFValueResult.u.p == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Grammar parsing result is a NULL PTR");
    goto err;
  }
  /* Here we go */
  marpaESLIFGrammarp->grammarStackp = marpaESLIFValueResult.u.p;
  /* Put current current grammar to the first of them in the grammar stack */
  for (grammari = 0; grammari < GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(marpaESLIFGrammarp->grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    marpaESLIFGrammarp->grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammari);
    break;
  }

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIFGrammar_eslifp(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  return marpaESLIFGrammarp->marpaESLIFp;
}

/*****************************************************************************/
short marpaESLIFGrammar_levelib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *levelip)
/*****************************************************************************/
{
  return marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, levelip, 0 /* grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *levelip, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
  if (grammarp == NULL) {
    goto err;
  }

  if (levelip != NULL) {
    *levelip = grammarp->leveli;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFGrammar_startib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip)
/*****************************************************************************/
{
  return marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammarp, startip, 0 /*grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
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
  return marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammarp, ruleipp, rulelp, 0 /* grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
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
short marpaESLIFGrammar_ruledisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammarp, rulei, ruledisplaysp, 0 /* grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
  if (grammarp == NULL) {
    goto err;
  }
  rulep = _marpaESLIF_rule_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, rulei);
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
  return marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammarp, rulei, ruleshowsp, 0 /* grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
  if (grammarp == NULL) {
    goto err;
  }
  rulep = _marpaESLIF_rule_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, rulei);
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
short marpaESLIFGrammar_symboldisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammarp, symboli, symboldisplaysp, 0 /* grammari */, NULL /* descp */);
}

/*****************************************************************************/
short marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, int grammari, marpaESLIFString_t *descp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp;
  marpaESLIF_symbol_t  *symbolp;
  short                 rcb;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
  if (grammarp == NULL) {
    goto err;
  }
  symbolp = _marpaESLIF_symbol_findp(marpaESLIFGrammarp->marpaESLIFp, grammarp, NULL /* asciis */, symboli);
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
  return _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 0 /* fakeb */);
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIFRecognizer_eslifp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  return marpaESLIFRecognizerp->marpaESLIFp;
}

/*****************************************************************************/
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_scanb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan can be done one once only");
    goto err;
  }

  if (! marpaESLIFRecognizerp->marpaESLIFGrammarp->grammarp->latmb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan requires your grammar to have: latm => 1, or its alias: forgiving => 1");
    goto err;
  }

  marpaESLIFRecognizerp->scanb = 1;
  rcb = _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, initialEventsb, continuebp, exhaustedbp);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_resumeb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0 /* initialEventsb */, continuebp, exhaustedbp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  static const char               *funcs                             = "_marpaESLIFRecognizer_resumeb";
  marpaESLIF_t                    *marpaESLIFp                       = marpaESLIFRecognizerp->marpaESLIFp;
  genericStack_t                  *lexemeInputStackp                 = marpaESLIFRecognizerp->lexemeInputStackp;
  marpaESLIFGrammar_t             *marpaESLIFGrammarp                = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t            *grammarp                          = marpaESLIFGrammarp->grammarp;
  genericStack_t                  *symbolStackp                      = grammarp->symbolStackp;
  /* Left commented to show where the test should be */
  /*
  short                            latmb                             = marpaESLIFGrammarp->latmb;
  */
  short                            exhaustedb                        = 0;
  short                            completedb                        = 0;
  short                            collectb                          = 0;
  marpaESLIFValueOption_t          marpaESLIFValueOptionDiscard      = marpaESLIFValueOption_default_template;
  marpaESLIFRecognizerOption_t     marpaESLIFRecognizerOptionDiscard = marpaESLIFRecognizerp->marpaESLIFRecognizerOption; /* Things overwriten, see below */
  short                            continueb;
  size_t                           maxMatchedl;
  int                              maxPriorityi;
  short                            maxPriorityInitializedb = 0;
  genericStack_t                   alternativeStack;
  genericStack_t                  *alternativeStackp = &alternativeStack;
  int                              alternativeOki;
  size_t                           nSymboll;
  int                             *symbolArrayp;
  size_t                           symboll;
  int                              symboli;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_matcher_value_t       rci;
  short                            rcb;
  size_t                           sizel;
  marpaESLIFGrammar_t              marpaESLIFGrammarDiscard = *marpaESLIFGrammarp; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t             grammarDiscard = *grammarp;
  marpaESLIFValueResult_t          marpaESLIFValueResult;
    
  marpaESLIFGrammarDiscard.grammarp = &grammarDiscard;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFRecognizerOptionDiscard.disableThresholdb = 1; /* If discard, prepare the option to disable threshold */
  marpaESLIFRecognizerOptionDiscard.exhaustedb        = 1; /* ... and have the exhausted event */
  marpaESLIFRecognizerOptionDiscard.newlineb          = 0; /* ... and not count line/column numbers */

  /* The context of a discard is current recognizer */
  marpaESLIFValueOptionDiscard.userDatavp            = (void *) marpaESLIFRecognizerp; /* Used by _marpaESLIF_lexeme_freeCallbackv */

#undef MARPAESLIFRECOGNIZER_COLLECT_EVENTS
#define MARPAESLIFRECOGNIZER_COLLECT_EVENTS(forceb, exhaustedbp)  do {  \
    if (forceb || ! collectb) {                                         \
      _marpaESLIFRecognizer_reset_eventsb(marpaESLIFRecognizerp);       \
      if (! _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizerp, NULL, NULL, NULL, exhaustedbp)) { \
        goto err;                                                       \
      }                                                                 \
      collectb = 1;                                                     \
    }                                                                   \
  } while (0)

  /* Checks */
  if (! marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan must be called first");
    goto err;
  }

  /* Initializations */
  GENERICSTACK_INIT(alternativeStackp);
  if (GENERICSTACK_ERROR(alternativeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp init failure, %s", strerror(errno));
    alternativeStackp = NULL;
    goto err;
  }
  marpaESLIFRecognizerp->resumeCounteri++; /* Increment internal counter for tracing */

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
    if (! _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizerp, symbolp, &rci, &exhaustedb, &marpaESLIFValueResult)) {
      if (! _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, NULL, 0)) {
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
        } else {
          goto match_again;
        }
      } else {
        if (! _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, NULL, 0)) {
          goto err;
        }
      }
      break;
    case MARPAESLIF_MATCH_FAILURE:
      if (! _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, NULL, 0)) {
        goto err;
      }
      break;
    case MARPAESLIF_MATCH_OK:
      /* It is a non-sense if matchedMarpaESLIFStackTypei is not MARPAESLIF_STACK_TYPE_ARRAY */
      if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
        MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValueResult.type is %d instead of %d (MARPAESLIF_STACK_TYPE_ARRAY)", marpaESLIFValueResult.type, MARPAESLIF_STACK_TYPE_ARRAY);
        goto err;
      }
      /* It is a non-sense that we matched a lexeme a null size */
      if ((marpaESLIFValueResult.u.p == NULL) || (marpaESLIFValueResult.sizel <= 0)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "symbol matcher returned {%p, %ld}", marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_i_setb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel)) {
        free(marpaESLIFValueResult.u.p);
        goto err;
      }
      /* Remember max matched length */
      if (marpaESLIFValueResult.sizel > maxMatchedl) {
        maxMatchedl = marpaESLIFValueResult.sizel;
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
      /* Always reset this shallow pointer */
      marpaESLIFRecognizerp->discardEvents = NULL;
      if (_marpaESLIFGrammar_parseb(&marpaESLIFGrammarDiscard, &marpaESLIFRecognizerOptionDiscard, &marpaESLIFValueOptionDiscard, 1 /* discardb */, marpaESLIFRecognizerp /* marpaESLIFRecognizerParentp */, NULL /* exhaustedbp */, &marpaESLIFValueResult)) {
        /* It is a non-sense if matchedMarpaESLIFStackTypei is not MARPAESLIF_STACK_TYPE_ARRAY */
        if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
          MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValueResult.type is %d instead of %d (MARPAESLIF_STACK_TYPE_ARRAY)", marpaESLIFValueResult.type, MARPAESLIF_STACK_TYPE_ARRAY);
          goto err;
        }
        /* So ptr must not be NULL nor size <= 0 */
        if ((marpaESLIFValueResult.u.p == NULL) || (marpaESLIFValueResult.sizel <= 0)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "Discard matcher returned {%p, %ld}", marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
          goto err;
        }
        /* New line processing, etc... */
        if (! _marpaESLIFRecognizer_matchPostProcessingb(marpaESLIFRecognizerp, marpaESLIFValueResult.sizel)) {
          goto err;
        }
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Advancing stream internal position by %ld bytes", (unsigned long) marpaESLIFValueResult.sizel);
        marpaESLIFRecognizerp->inputs += marpaESLIFValueResult.sizel;
        marpaESLIFRecognizerp->inputl -= marpaESLIFValueResult.sizel;
        free(marpaESLIFValueResult.u.p);
        /* Well, we KNOW that no lexeme matched. This mean that alternative stack contains only N/A's: no need to free anything */
        GENERICSTACK_USED(alternativeStackp) = 0;
        goto retry;
      }
    }

    /* Discard failure - this is an error unless lexemes were read and:
       - exhaustion is on, or
       - eof flags is true and all the data is consumed */
    if (marpaESLIFRecognizerp->haveLexemeb && (marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb || (*(marpaESLIFRecognizerp->eofbp) && (marpaESLIFRecognizerp->inputl <= 0)))) {
      continueb = 0;
      rcb = 1;
      goto done;
    } else {
      rcb = 0;
      goto err;
    }
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

    /* Get lexeme length */
    if (! _marpaESLIFRecognizer_lexemeStack_i_sizeb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, &sizel)) {
      goto err;
    }
    if (sizel <= 0) {
      /* This symbol did not match */
      continue;
    }

 /* if (latmb) { */
      /* In latm mode, keep only the longests alternatives */
      if (sizel < maxMatchedl) {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp,
                                    funcs,
                                    "Alternative %s is skipped (length %ld < max length %ld)",
                                    symbolp->descp->asciis,
                                    (unsigned long) sizel,
                                    (unsigned long) maxMatchedl);
        continue;
      }      
 /* } */

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

    /* Get lexeme length */
    if (! _marpaESLIFRecognizer_lexemeStack_i_sizeb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, &sizel)) {
      goto err;
    }
    if (sizel <= 0) {
      /* This symbol did not match */
      continue;
    }

 /* if (latmb) { */
      /* If latm mode is true, keep only the longests alternatives */
      /* Already traced few lines higher */
      if (sizel < maxMatchedl) {
        continue;
      }      
 /* } */

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
    if (! _marpaESLIFRecognizer_lexemeStack_i_moveb(marpaESLIFRecognizerp, lexemeInputStackp, GENERICSTACK_USED(lexemeInputStackp), alternativeStackp, (int) symboll)) {
      goto err;
    }

    /* And to Marpa */
    if (! _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp,
                                             symbolp,
                                             GENERICSTACK_USED(lexemeInputStackp) - 1)) {
      goto err;
    }
  }

  /* Commit */
  if (! _marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
#ifndef MARPAESLIF_NTRACE
    marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp,
                                      -1,
                                      -1,
                                      GENERICLOGGER_LOGLEVEL_TRACE,
                                      marpaESLIFGrammarp,
                                      _marpaESLIFGrammar_symbolDescriptionCallback);
#endif
    goto err;
  }

  /* Remember this recognizer have at least one lexeme */
  marpaESLIFRecognizerp->haveLexemeb = 1;
  
  /* New line processing, etc... */
  if (! _marpaESLIFRecognizer_matchPostProcessingb(marpaESLIFRecognizerp, maxMatchedl)) {
    goto err;
  }
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
  _marpaESLIFrecognizer_lexemeStack_resetv(marpaESLIFRecognizerp, alternativeStackp);
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

  /* In discard mode, if successful, per definition we have fetched the events. An eventual completion event will be our parent's discard event */
  /* If there is a completion it is unique per def because discard mode is always launched with ambiguity turned off. */
  if (rcb && (! continueb) && marpaESLIFRecognizerp->discardb && (marpaESLIFRecognizerp->lastCompletionEvents != NULL)) {
    /* In theory it is not possible to not have a parent recognizer here */
    if (marpaESLIFRecognizerp->parentRecognizerp != NULL) {
      marpaESLIFRecognizerp->parentRecognizerp->discardEvents = marpaESLIFRecognizerp->lastCompletionEvents;
    }
  }
  /* At level 0, this is the final value - we generate error information if there is input */
  if (! rcb) {
    if ((marpaESLIFRecognizerp->leveli == 0) && (! marpaESLIFRecognizerp->discardb)) {
      MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "--------------------------------------------");
      MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "Recognizer failure. Current state:");
      marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp,
                                        -1,
                                        -1,
                                        GENERICLOGGER_LOGLEVEL_ERROR,
                                        marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
      MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "--------------------------------------------");
      if (nSymboll <= 0) {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "There was no expected terminal");
      } else {
        for (symboll = 0; symboll < nSymboll; symboll++) {
          symboli = symbolArrayp[symboll];
          if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
            continue;
          }
          symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Expected terminal: %s", symbolp->descp->asciis);
        }
      }
      /* If there is some information before, show it */
      if ((marpaESLIFRecognizerp->inputs != NULL) && ((*marpaESLIFRecognizerp->buffersp) != NULL) && (marpaESLIFRecognizerp->inputs > *(marpaESLIFRecognizerp->buffersp))) {
        char  *dumps;
        size_t dumpl;

        if ((marpaESLIFRecognizerp->inputs - *(marpaESLIFRecognizerp->buffersp)) > 128) {
          dumps = marpaESLIFRecognizerp->inputs - 128;
          dumpl = 128;
        } else {
          dumps = *(marpaESLIFRecognizerp->buffersp);
          dumpl = marpaESLIFRecognizerp->inputs - *(marpaESLIFRecognizerp->buffersp);
        }
        MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp,
                            "",
                            *(marpaESLIFRecognizerp->utfbp) ? "UTF-8 converted data before the failure" : "Raw data before the failure",
                            dumps,
                            dumpl,
                            0 /* traceb */);
      }
      if (marpaESLIFRecognizerp->marpaESLIFRecognizerOption.newlineb) {
        if (marpaESLIFRecognizerp->columnl > 0) {
          /* Column is known (in terms of character count) */
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "RECOGNIZER FAILURE AFTER LINE No %ld COLUMN No %ld, HERE:", (unsigned long) marpaESLIFRecognizerp->linel, (unsigned long) marpaESLIFRecognizerp->columnl);
        } else {
          /* Column is not known */
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "RECOGNIZER FAILURE AFTER LINE No %ld, HERE:", (unsigned long) marpaESLIFRecognizerp->linel);
        }
      } else {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "RECOGNIZER FAILURE HERE:");
      }
      /* If there is some information after, show it */
      if ((marpaESLIFRecognizerp->inputs != NULL) && (marpaESLIFRecognizerp->inputl > 0)) {
        char  *dumps;
        size_t dumpl;

        dumps = marpaESLIFRecognizerp->inputs;
        dumpl = marpaESLIFRecognizerp->inputl > 128 ? 128 : marpaESLIFRecognizerp->inputl;
        MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp,
                            "",
                            *(marpaESLIFRecognizerp->utfbp) ? "UTF-8 converted data after the failure" : "Raw data after the failure",
                            dumps,
                            dumpl,
                            0 /* traceb */);
      }
    }
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *asciis, int valuei)
/*****************************************************************************/
{
  static const char    *funcs = "marpaESLIFRecognizer_alternativeb";
  marpaESLIF_symbol_t  *symbolp;
  short                 rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  symbolp = _marpaESLIF_symbol_findp(marpaESLIFRecognizerp->marpaESLIFp, marpaESLIFRecognizerp->marpaESLIFGrammarp->grammarp, asciis, -1 /* symboli */);
  if (symbolp == NULL) {
    return 0;
  }

  rcb = _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp, symbolp, valuei);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFRecognizer_alternativeb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

#ifndef MARPAESLIF_NTRACE
  if (symbolp->type == MARPAESLIF_SYMBOL_TYPE_TERMINAL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Committing terminal alternative %s at input stack %d", symbolp->descp->asciis, valuei);
  } else {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Committing meta alternative <%s> at input stack %d", symbolp->descp->asciis, valuei);
  }
#endif

  rcb = marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symbolp->idi, valuei, 1);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_completeb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Completing alternatives");
  rcb = _marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_eofb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_eofb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Ending stream");
  *(marpaESLIFRecognizerp->eofbp) = 1;
  marpaESLIFRecognizerp->inputl = 0;

  rcb = 1;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFRecognizer_completeb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = marpaWrapperRecognizer_completeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_event_onoffb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* Of course, our marpaESLIFEventType_t is strictly equivalent to marpaWrapperGrammarEventType_t -; */
  rcb = marpaWrapperRecognizer_event_onoffb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symboli, eventSeti, onoffb);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_expectedb";
  short              rcb;
  
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = marpaWrapperRecognizer_expectedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, nSymbollp, symbolArraypp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
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
  static const char *funcs = "marpaESLIFRecognizer_freev";

  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp = marpaESLIFRecognizerp->parentRecognizerp;
    
    marpaESLIFRecognizerp->callstackCounteri++;
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

    _marpaESLIFrecognizer_lexemeStack_freev(marpaESLIFRecognizerp, marpaESLIFRecognizerp->lexemeInputStackp);
    if (marpaESLIFRecognizerp->marpaWrapperRecognizerp != NULL) {
      marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    }
    if (marpaESLIFRecognizerp->eventArrayp != NULL) {
      free(marpaESLIFRecognizerp->eventArrayp);
    }
    if (marpaESLIFRecognizerParentp == NULL) {
      /* These area are managed by the parent recognizer */
      if (marpaESLIFRecognizerp->_buffers != NULL) {
        free(marpaESLIFRecognizerp->_buffers);
      }
      if (marpaESLIFRecognizerp->_encodings != NULL) {
        free(marpaESLIFRecognizerp->_encodings);
      }
      if (marpaESLIFRecognizerp->_encodingp != NULL) {
        _marpaESLIF_terminal_freev(marpaESLIFRecognizerp->_encodingp);
      }
      /* The situation where _tconvp is different than NULL should never happen except in case */
      /* of error processing */
      if (marpaESLIFRecognizerp->_tconvp != NULL) {
        tconv_close(marpaESLIFRecognizerp->_tconvp);
      }
    } else {
      /* Parent's "current" position have to be updated */
      marpaESLIFRecognizerParentp->inputs = *(marpaESLIFRecognizerp->buffersp) + marpaESLIFRecognizerp->parentDeltal;
      marpaESLIFRecognizerParentp->inputl = *(marpaESLIFRecognizerp->bufferlp) - marpaESLIFRecognizerp->parentDeltal;
    }

    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
    marpaESLIFRecognizerp->callstackCounteri--;

    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  return marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, marpaESLIFValueOptionp, exhaustedbp, 0 /* grammari */, NULL /* descp */, marpaESLIFValueResultp);
}

/*****************************************************************************/
short marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, int grammari, marpaESLIFString_t *descp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char          *funcs        = "marpaESLIFGrammar_parse_by_grammarb";
  marpaESLIF_grammar_t       *grammarp;
  short                       rcb;
  marpaESLIFGrammar_t         marpaESLIFGrammar;

  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, grammari, descp);
  if (grammarp == NULL) {
    goto err;
  }

  /* Use a local marpaESLIFGrammar and change current gramamar */
  marpaESLIFGrammar          = *marpaESLIFGrammarp;
  marpaESLIFGrammar.grammarp = grammarp;
  rcb = _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, marpaESLIFRecognizerOptionp, marpaESLIFValueOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, exhaustedbp, marpaESLIFValueResultp);
  goto done;

 err:
  rcb = 0;

 done:
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  static const char          *funcs                     = "marpaESLIFReader_grammarReader";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  *inputsp              = marpaESLIF_readerContextp->marpaESLIFGrammarOptionp->grammars;
  *inputlp              = marpaESLIF_readerContextp->marpaESLIFGrammarOptionp->grammarl;
  *eofbp                = 1;
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingOfEncodingsp = marpaESLIF_readerContextp->marpaESLIFGrammarOptionp->encodingOfEncodings;
  *encodingsp           = marpaESLIF_readerContextp->marpaESLIFGrammarOptionp->encodings;
  *encodinglp           = marpaESLIF_readerContextp->marpaESLIFGrammarOptionp->encodingl;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (*inputsp=%p, *inputlp=%ld, *eofbp=%d, *characterStreambp=%d)", *inputsp, (unsigned long) *inputlp, (int) *eofbp, (int) *characterStreambp);
  return 1;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIFGrammar_grammar_findp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammari, marpaESLIF_string_t *descp)
/*****************************************************************************/
{
  static const char    *funcs         = "_marpaESLIFGrammar_grammar_findp";
  genericStack_t       *grammarStackp = marpaESLIFGrammarp->grammarStackp;
  marpaESLIF_grammar_t *rcp           = NULL;
  marpaESLIF_grammar_t *grammarp;
  int                   i;

  if (descp != NULL) {
    /* Search by description has precedence */
    for (i = 0; i < GENERICSTACK_USED(grammarStackp); i++) {
      if (! GENERICSTACK_IS_PTR(grammarStackp, i)) {
        continue;
      }
      grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, i);
      if (_marpaESLIF_string_eqb(grammarp->descp, descp)) {
        rcp = grammarp;
        break;
      }
    }
  } else if (grammari >= 0) {
    if (GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      rcp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    }
  }

  return rcp;
}
 
/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int rulei)
/*****************************************************************************/
{
  static const char    *funcs        = "_marpaESLIF_rule_findp";
  genericStack_t       *ruleStackp   = grammarp->ruleStackp;
  marpaESLIF_rule_t    *rcp          = NULL;

  if (rulei >= 0) {
    if (GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
      rcp = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
    }
  }

  return rcp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_symbol_findp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *asciis, int symboli)
/*****************************************************************************/
{
  static const char    *funcs        = "_marpaESLIF_symbol_findp";
  genericStack_t       *symbolStackp = grammarp->symbolStackp;
  marpaESLIF_symbol_t  *rcp          = NULL;
  marpaESLIF_symbol_t  *symbolp;
  int                   i;

  if (asciis != NULL) {
    /* Give precedence to symbol by name - which is possible only for meta symbols */
    for (i = 0; i < GENERICSTACK_USED(symbolStackp); i++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, i)) {
        /* Should never happen */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, i);
      if (symbolp->type == MARPAESLIF_SYMBOL_TYPE_META) {
        if (strcmp(asciis, symbolp->u.metap->asciinames) == 0) {
          rcp = symbolp;
          break;
        }
      }
    }
  } else if (symboli >= 0) {
    if (GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      rcp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
    }
  }

  return rcp;
}

/*****************************************************************************/
void marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *eventArraylp, marpaESLIFEvent_t **eventArraypp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_eventb";

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (eventArraylp != NULL) {
    *eventArraylp = marpaESLIFRecognizerp->eventArrayl;
  }
  if (eventArraypp != NULL) {
    *eventArraypp = marpaESLIFRecognizerp->eventArrayp;
  }

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
  marpaESLIFRecognizerp->callstackCounteri--;
}

/*****************************************************************************/
static inline void _marpaESLIFRecognizer_reset_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFRecognizer_reset_eventsb";

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFRecognizerp->eventArrayp != NULL) {
    free(marpaESLIFRecognizerp->eventArrayp);
    marpaESLIFRecognizerp->eventArrayp = NULL;
  }
  marpaESLIFRecognizerp->eventArrayl = 0;

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
  marpaESLIFRecognizerp->callstackCounteri--;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp)
/*****************************************************************************/
{
  static const char          *funcs              = "_marpaESLIFRecognizer_grammar_eventsb";
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
  char                       *events;
  size_t                      i;
  marpaESLIFEventType_t       type;
  marpaESLIFEvent_t           *eventArrayp;      /* For eventual realloc */

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFRecognizerp->lastCompletionEvents = NULL;

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
      events  = NULL;
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
          /* The discard event is only possible on completion in discard mode */
          marpaESLIFRecognizerp->lastCompletionEvents = events = marpaESLIFRecognizerp->discardb ? symbolp->discardEvents : symbolp->eventCompleteds;
        }
        completedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s: completion event", symbolp->descp->asciis);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
        type        = MARPAESLIF_EVENTTYPE_NULLED;
        if (symbolp != NULL) {
          events = symbolp->eventNulleds;
        }
        nulledb     = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s: nullable event", symbolp->descp->asciis);
      case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
        type        = MARPAESLIF_EVENTTYPE_PREDICTED;
        if (symbolp != NULL) {
          events = symbolp->eventPredicteds;
        }
        predictedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s: prediction event", symbolp->descp->asciis);
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
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s: unsupported event type %d", symbolp->descp->asciis, grammarEventp[i].eventType);
        break;
      }

      if (type != MARPAESLIF_EVENTTYPE_EXHAUSTED) {
        marpaESLIFRecognizerp->eventArrayp[okeventl].type   = type;
        marpaESLIFRecognizerp->eventArrayp[okeventl].events = events;
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
        marpaESLIFRecognizerp->eventArrayp[0].events  = NULL;
        marpaESLIFRecognizerp->eventArrayl = 1;
      } else {
        eventArrayp = realloc(marpaESLIFRecognizerp->eventArrayp, (okeventl + 1) + sizeof(marpaESLIFEvent_t));
        if (eventArrayp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        marpaESLIFRecognizerp->eventArrayp = eventArrayp;
        marpaESLIFRecognizerp->eventArrayp[okeventl].type    = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        marpaESLIFRecognizerp->eventArrayp[okeventl].events  = NULL;
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
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short fakeb)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFRecognizer_newp";
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
    marpaESLIFRecognizerOptionp = &marpaESLIFRecognizerOption_default_template;
  }

  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null grammar pointer");
    goto err;
  }

  /* We request a stream reader callback unless eof flag is set by an eventual parent recognizer or we are in fake mode */
  if (marpaESLIFRecognizerOptionp->marpaESLIFReaderCallbackp == NULL) {
    if (! (((marpaESLIFRecognizerParentp != NULL) && *(marpaESLIFRecognizerParentp->eofbp))
           ||
           fakeb)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Null reader callback");
      goto err;
    }
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
  marpaESLIFRecognizerp->lastCompletionEvents       = NULL;
  marpaESLIFRecognizerp->discardEvents              = NULL;
  marpaESLIFRecognizerp->resumeCounteri             = 0;
  marpaESLIFRecognizerp->callstackCounteri          = 0;
  marpaESLIFRecognizerp->_buffers                   = NULL;
  marpaESLIFRecognizerp->_bufferl                   = 0;
  marpaESLIFRecognizerp->_bufferallocl              = 0;
  marpaESLIFRecognizerp->_eofb                      = fakeb;  /* In fake mode, always make sure there is no reader needed */
  marpaESLIFRecognizerp->_utfb                      = 0;
  marpaESLIFRecognizerp->_charconvb                 = 0;
  marpaESLIFRecognizerp->_encodings                 = 0;
  marpaESLIFRecognizerp->_encodingp                 = NULL;
  marpaESLIFRecognizerp->_tconvp                    = NULL;
  marpaESLIFRecognizerp->_nextReadIsFirstReadb      = 1;
  marpaESLIFRecognizerp->_noAnchorIsOkb             = 0;
  /* If this is a parent recognizer get its stream information */
  if (marpaESLIFRecognizerParentp != NULL) {
    marpaESLIFRecognizerp->leveli                     = marpaESLIFRecognizerParentp->leveli + 1;
    marpaESLIFRecognizerp->buffersp                   = marpaESLIFRecognizerParentp->buffersp;
    marpaESLIFRecognizerp->bufferlp                   = marpaESLIFRecognizerParentp->bufferlp;
    marpaESLIFRecognizerp->bufferalloclp              = marpaESLIFRecognizerParentp->bufferalloclp;
    marpaESLIFRecognizerp->eofbp                      = marpaESLIFRecognizerParentp->eofbp;
    marpaESLIFRecognizerp->utfbp                      = marpaESLIFRecognizerParentp->utfbp;
    marpaESLIFRecognizerp->charconvbp                 = marpaESLIFRecognizerParentp->charconvbp;
    marpaESLIFRecognizerp->encodingsp                 = marpaESLIFRecognizerParentp->encodingsp;
    marpaESLIFRecognizerp->encodingpp                 = marpaESLIFRecognizerParentp->encodingpp;
    marpaESLIFRecognizerp->tconvpp                    = marpaESLIFRecognizerParentp->tconvpp;
    marpaESLIFRecognizerp->parentDeltal               = marpaESLIFRecognizerParentp->inputs - *(marpaESLIFRecognizerParentp->buffersp);
    marpaESLIFRecognizerp->nextReadIsFirstReadbp      = marpaESLIFRecognizerParentp->nextReadIsFirstReadbp;
    marpaESLIFRecognizerp->noAnchorIsOkbp             = marpaESLIFRecognizerParentp->noAnchorIsOkbp;
    /* New recognizer is starting at the parent's inputs pointer */
    marpaESLIFRecognizerp->inputs                     = marpaESLIFRecognizerParentp->inputs;
    marpaESLIFRecognizerp->inputl                     = marpaESLIFRecognizerParentp->inputl;
    marpaESLIFRecognizerp->bufsizl                    = marpaESLIFRecognizerParentp->bufsizl;
    marpaESLIFRecognizerp->buftriggerl                = marpaESLIFRecognizerParentp->buftriggerl;
  } else {
    marpaESLIFRecognizerp->leveli                     = 0;
    marpaESLIFRecognizerp->buffersp                   = &(marpaESLIFRecognizerp->_buffers);
    marpaESLIFRecognizerp->bufferlp                   = &(marpaESLIFRecognizerp->_bufferl);
    marpaESLIFRecognizerp->bufferalloclp              = &(marpaESLIFRecognizerp->_bufferallocl);
    marpaESLIFRecognizerp->eofbp                      = &(marpaESLIFRecognizerp->_eofb);
    marpaESLIFRecognizerp->utfbp                      = &(marpaESLIFRecognizerp->_utfb);
    marpaESLIFRecognizerp->charconvbp                 = &(marpaESLIFRecognizerp->_charconvb);
    marpaESLIFRecognizerp->encodingsp                 = &(marpaESLIFRecognizerp->_encodings);
    marpaESLIFRecognizerp->encodingpp                 = &(marpaESLIFRecognizerp->_encodingp);
    marpaESLIFRecognizerp->tconvpp                    = &(marpaESLIFRecognizerp->_tconvp);
    marpaESLIFRecognizerp->parentDeltal               = 0;
    marpaESLIFRecognizerp->nextReadIsFirstReadbp      = &(marpaESLIFRecognizerp->_nextReadIsFirstReadb);
    marpaESLIFRecognizerp->noAnchorIsOkbp             = &(marpaESLIFRecognizerp->_noAnchorIsOkb);
    /* New recognizer is starting nowhere for the moment - it will ask for more data, c.f. recognizer's read() */
    marpaESLIFRecognizerp->inputs                     = NULL;
    marpaESLIFRecognizerp->inputl                     = 0;
    marpaESLIFRecognizerp->bufsizl                    = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.bufsizl;
    if (marpaESLIFRecognizerp->bufsizl <= 0) {
      marpaESLIFRecognizerp->bufsizl = MARPAESLIF_BUFSIZ;
      /* Still ?! */
      if (marpaESLIFRecognizerp->bufsizl <= 0) {
        MARPAESLIF_ERROR(marpaESLIFp, "Please recompile this project with a default buffer value > 0 !");
        goto err;
      }
    }
    marpaESLIFRecognizerp->buftriggerl                = (marpaESLIFRecognizerp->bufsizl * (100 + marpaESLIFRecognizerp->marpaESLIFRecognizerOption.buftriggerperci)) / 100;
  }
  marpaESLIFRecognizerp->scanb                      = 0;
  marpaESLIFRecognizerp->discardb                   = discardb;
  marpaESLIFRecognizerp->haveLexemeb                = 0;
  marpaESLIFRecognizerp->linel                      = 1;
  marpaESLIFRecognizerp->columnl                    = 0;

  marpaWrapperRecognizerOption.genericLoggerp       = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb    = marpaESLIFRecognizerOptionp->disableThresholdb;

  if (! fakeb) {
    marpaESLIFRecognizerp->marpaWrapperRecognizerp    = marpaWrapperRecognizer_newp(discardb ?
                                                                                    marpaESLIFGrammarp->grammarp->marpaWrapperGrammarDiscardp
                                                                                    :
                                                                                    marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp
                                                                                    , &marpaWrapperRecognizerOption);
    if (marpaESLIFRecognizerp->marpaWrapperRecognizerp == NULL) {
      goto err;
    }
  } else {
    marpaESLIFRecognizerp->marpaWrapperRecognizerp = NULL;
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
static inline short _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFGrammar_parseb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t marpaESLIFValueOption = (marpaESLIFValueOptionp != NULL) ? *marpaESLIFValueOptionp : marpaESLIFValueOption_default_template;
  marpaESLIFValue_t      *marpaESLIFValuep      = NULL;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  short                   exhaustedb;
  short                   continueb;
  short                   rcb;

  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, discardb, marpaESLIFRecognizerParentp, 0 /* fakeb */);
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
  if (marpaESLIFRecognizerParentp != NULL) {
    /* Lexeme mode: we want to be sure that we will ALWAYS have a result in order to check it. */
    /* This result must be an ARRAY and the parsing will be ok only if the matched size is > 0. */
    if (marpaESLIFValue_valueb(marpaESLIFValuep, &marpaESLIFValueResult) <= 0) {
      goto err;
    }
    if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValueResult.type is not ARRAY (internal type: %d)", marpaESLIFValueResult.type);
      goto err;
    }
    if ((marpaESLIFValueResult.u.p == NULL) || (marpaESLIFValueResult.sizel <= 0)) {
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Lexeme valuation returned {%d,%ld}", marpaESLIFValueResult.u.p, (unsigned long) marpaESLIFValueResult.sizel);
      goto err;
    }
    if (marpaESLIFValueResultp != NULL) {
      *marpaESLIFValueResultp = marpaESLIFValueResult;
    } else {
      /* Free it - IMHO this case never happen, but who knows */
      free(marpaESLIFValueResult.u.p);
    }
  } else {
    if (marpaESLIFValue_valueb(marpaESLIFValuep, marpaESLIFValueResultp) <= 0) {
      goto err;
    }
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
  marpaESLIF_stringGenerator_t *contextp = (marpaESLIF_stringGenerator_t *) userDatavp;
  char                         *tmps;

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

/*****************************************************************************/
marpaESLIFValue_t *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
{
  return _marpaESLIFValue_newp(marpaESLIFRecognizerp, marpaESLIFValueOptionp, 0 /* fakeb */);
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIFValue_eslifp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return NULL;
  }

  return marpaESLIFValuep->marpaESLIFp;
}

/*****************************************************************************/
short marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char                *funcs                 = "marpaESLIFValue_valueb";
  marpaESLIFRecognizer_t           *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                               indicei               = 0;
  GENERICSTACKITEMTYPE2TYPE_ARRAY   array;
  short                             rcb;
  marpaESLIFValueResult_t           marpaESLIFValueResult;

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFValue_stack_newb(marpaESLIFValuep)) {
    goto err;
  }

  if (marpaESLIFValuep->marpaWrapperValuep != NULL) {
    rcb = marpaWrapperValue_valueb(marpaESLIFValuep->marpaWrapperValuep,
                                   (void *) marpaESLIFValuep,
                                   _marpaESLIFValue_ruleCallbackWrapperb,
                                   _marpaESLIFValue_symbolCallbackWrapperb,
                                   _marpaESLIFValue_nullingCallbackWrapperb);
  } else {
    if (! marpaESLIFValuep->marpaESLIFValueOption.ambiguousb) {
      /* ASF have a built-in facility for pruning */
      rcb = marpaWrapperAsf_prunedValueb(marpaESLIFValuep->marpaWrapperAsfp,
                                         (void *) marpaESLIFValuep,
                                         _marpaESLIFValue_okRuleCallbackWrapperb,
                                         _marpaESLIFValue_okSymbolCallbackWrapperb,
                                         _marpaESLIFValue_okNullingCallbackWrapperb,
                                         _marpaESLIFValue_ruleCallbackWrapperb,
                                         _marpaESLIFValue_symbolCallbackWrapperb,
                                         _marpaESLIFValue_nullingCallbackWrapperb);
    } else {
      rcb = marpaWrapperAsf_traverseb(marpaESLIFValuep->marpaWrapperAsfp,
                                      _marpaESLIFValue_traverserb,
                                      (void *) marpaESLIFValuep,
                                      &indicei);
    }
  }

  if (rcb > 0) {
    /* The output is at position indicei of valueStackp */
    /* This indice must be something we know about */
    if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, indicei)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->typeStackp, indicei));
      goto err;
    }
    if (! GENERICSTACK_IS_INT(marpaESLIFValuep->contextStackp, indicei)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->contextStackp, indicei));
      goto err;
    }

    marpaESLIFValueResult.type     = GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei);
    marpaESLIFValueResult.contexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
    marpaESLIFValueResult.sizel    = 0;

    switch (marpaESLIFValueResult.type) {
    case MARPAESLIF_STACK_TYPE_CHAR:
      if (! GENERICSTACK_IS_CHAR(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not CHAR (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.c = GENERICSTACK_GET_CHAR(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_SHORT:
      if (! GENERICSTACK_IS_SHORT(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not SHORT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.b = GENERICSTACK_GET_SHORT(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_INT:
      if (! GENERICSTACK_IS_INT(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.i = GENERICSTACK_GET_INT(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_LONG:
      if (! GENERICSTACK_IS_LONG(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not LONG (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.l = GENERICSTACK_GET_LONG(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_FLOAT:
      if (! GENERICSTACK_IS_FLOAT(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not FLOAT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.f = GENERICSTACK_GET_FLOAT(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_DOUBLE:
      if (! GENERICSTACK_IS_DOUBLE(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not DOUBLE (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.d = GENERICSTACK_GET_DOUBLE(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_PTR:
    case MARPAESLIF_STACK_TYPE_PTR_SHALLOW:
      if (! GENERICSTACK_IS_PTR(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not PTR (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      marpaESLIFValueResult.u.p = GENERICSTACK_GET_PTR(marpaESLIFValuep->valueStackp, indicei);
      break;
    case MARPAESLIF_STACK_TYPE_ARRAY:
    case MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW:
      if (! GENERICSTACK_IS_ARRAY(marpaESLIFValuep->valueStackp, indicei)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not ARRAY (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
        goto err;
      }
      array = GENERICSTACK_GET_ARRAY(marpaESLIFValuep->valueStackp, indicei);
      marpaESLIFValueResult.u.p   = GENERICSTACK_ARRAY_PTR(array);
      marpaESLIFValueResult.sizel = GENERICSTACK_ARRAY_LENGTH(array);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp value at indice %d is not supported (got %d)", indicei, marpaESLIFValueResult.type);
      goto err;
    }

    if (marpaESLIFValueResultp != NULL) {
      *marpaESLIFValueResultp = marpaESLIFValueResult;
      /* User is aking for the value: he will responsible to manage its eventual free */
      /* We have to set the corresponding entries to NA */
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp set to MARPAESLIF_STACK_TYPE_UNDEF failure at indice %d, %s", strerror(errno), indicei);
        goto err;
      }
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp set to NA failure at indice %d, %s", strerror(errno), indicei);
        goto err;
      }
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp set to NA failure at indice %d, %s", strerror(errno), indicei);
        goto err;
      }
    }
  }

  goto done;

 err:
  rcb = -1;

 done:
  if (! _marpaESLIFValue_stack_freeb(marpaESLIFValuep)) {
    rcb = -1;
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
void marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFValue_freev";

  if (marpaESLIFValuep != NULL) {
    /* Take care: last value is under the USER's responsibility */
    marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
    marpaWrapperValue_t    *marpaWrapperValuep    = marpaESLIFValuep->marpaWrapperValuep;
    marpaWrapperAsf_t      *marpaWrapperAsfp      = marpaESLIFValuep->marpaWrapperAsfp;

    marpaESLIFRecognizerp->callstackCounteri++;
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

    if (marpaWrapperValuep != NULL) {
      marpaWrapperValue_freev(marpaWrapperValuep);
    }
    if (marpaWrapperAsfp != NULL) {
      marpaWrapperAsf_freev(marpaWrapperAsfp);
    }
    /* The stacks should never be something different than NULL */
    /* The methods to use them are protected so that it is impossible */
    /* to use them outside of valuation mode. Except the versions starting with "_" */
    /* that we use internally. Then the methods doing so are responsible to free the stacks. */
    /*
    GENERICSTACK_FREE(marpaESLIFValuep->valueStackp);
    GENERICSTACK_FREE(marpaESLIFValuep->typeStackp);
    GENERICSTACK_FREE(marpaESLIFValuep->contextStackp);
    */

    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
    marpaESLIFRecognizerp->callstackCounteri--;

    free(marpaESLIFValuep);
  }
}

/*****************************************************************************/
static short _marpaESLIFValue_ruleCallbackWrapperb(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  static const char                  *funcs                 = "_marpaESLIFValue_ruleCallbackWrapperb";
  marpaESLIFValue_t                  *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t             *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFValueOption_t             marpaESLIFValueOption = marpaESLIFValuep->marpaESLIFValueOption;
  marpaESLIFValueRuleActionResolver_t ruleActionResolverp   = marpaESLIFValueOption.ruleActionResolverp;
  marpaESLIFGrammar_t                *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t               *grammarp              = marpaESLIFGrammarp->grammarp;
  marpaESLIFValueRuleCallback_t       ruleCallbackp         = NULL;
  char                               *actions               = NULL;
  marpaESLIF_rule_t                  *rulep;
  short                               rcb;
  
  marpaESLIFValuep->inValuationb = 1;
  marpaESLIFValuep->symboli      = -1;
  marpaESLIFValuep->rulei        = rulei;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start [%d] = [%d-%d]", resulti, arg0i, argni);

  rulep = _marpaESLIF_rule_findp(marpaESLIFValuep->marpaESLIFp, grammarp, rulei);
  if (rulep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "No such rule No %d", rulei);
    goto err;
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Rule %d %s", rulep->idi, rulep->asciishows);

  /* Passthrough mode:

     This is a vicious case: we have created a rule that is passthough. This can happen only in
     prioritized rules and our internal rules made sure that this situation is unique in the whole grammar.
     That is, for example:

       <Expression> ::=
         Number
          | '(' Expression ')' assoc => group
         || Expression '**' Expression assoc => right
         || Expression '*' Expression 
          | Expression '/' Expression
         || Expression '+' Expression
          | Expression '-' Expression

     is internally converted to:

          Expression    ::= Expression[0]
          Expression[3] ::= '(' Expression[0] ')'
          Expression[2] ::= Expression[3] '**' Expression[2]
          Expression[1] ::= Expression[1] '*'  Expression[2]
                          | Expression[1] '/'  Expression[2]
          Expression[0] ::= Expression[0] '+'  Expression[1]
                          | Expression[0] '+'  Expression[1]

     i.e. the rule

          Expression    ::= Expression[0]

     is a passtrough. Now, again, we made sure that this rule that we call a "passthrough" can happen only
     once in the grammar. This mean that when we evaluate Expression[0] we are sure that the next rule to
     evaluate will be Expression.

     In Marpa native valuation methods, from stack point of view, we know that if the stack numbers for
     Expression[0] and Expression will be the same. In the ASF valuation mode, this will not be true.

     In Marpa, for instance, Expression[0] evaluates in stack number resulti, then Expression will also
     evaluate to the same numberi.

     In ASF mode, Expression[0] is likely to be one plus the stack number for Expression.

     A general implementation can just say the following:

     Expression[0] will evaluate stack [arg0i[0]..argni[0]] to resulti[0]
     Expression    will evaluate stack resulti[0] to resulti

     i.e. both are equivalent to: stack [arg0i[0]..argni[0]] evaluating to resulti.

     This mean that we can skip the passthrough valuation if we remember its stack input: [arg0i[0]..argn[0]].

     Implementation is:
     * If current rule is a passthrough, remember arg0i and argni, action and remember we have done a passthrough
     * Next pass will check if previous call was a passthrough, and if true, will reuse these remembered arg0i and argni.
  */
  if (rulep->passthroughb) {
    if (marpaESLIFValuep->previousPassWasPassthroughb) {
      /* Extra protection - this should never happen */
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "Passthrough rule but previous rule was already a passthrough");
      goto err;
    }
    marpaESLIFValuep->previousPassWasPassthroughb = 1;
    marpaESLIFValuep->previousArg0i               = arg0i;
    marpaESLIFValuep->previousArgni               = argni;

  } else {
    
    if (marpaESLIFValuep->previousPassWasPassthroughb) {
      /* Previous rule was a passthrough */
      arg0i   = marpaESLIFValuep->previousArg0i;
      argni   = marpaESLIFValuep->previousArgni;
      marpaESLIFValuep->previousPassWasPassthroughb = 0;
    }

    /* If we have a parent recognizer, this mean that we are in the lexemode mode. Then */
    /* the action is forced to be our lexeme concatenation action. */
    if (marpaESLIFRecognizerp->parentRecognizerp != NULL) {
      ruleCallbackp = _marpaESLIF_lexeme_concatb;
    } else {
      actions = rulep->actions;
      if (actions == NULL) {
        actions = grammarp->defaultRuleActions;
      }
      if (actions == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Rule No %d (%s) from grammar level %d (%s) requires: action => action_name, or that your grammar have: :default ::= action => action_name",
                          rulei,
                          rulep->descp->asciis,
                          grammarp->leveli,
                          grammarp->descp->asciis);
        goto err;
      }

      /* If this is a built-in action, we do not need the resolver */
      if (strcmp(actions, "::shift") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___shiftb;
      } else if (strcmp(actions, "::undef") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___undefb;
      } else if (strcmp(actions, "::ascii") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___asciib;
      } else if (strcmp(actions, "::translit") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___translitb;
      } else {
        if (ruleActionResolverp == NULL) {
          MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "No rule action resolver");
          goto err;
        }
        ruleCallbackp = ruleActionResolverp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, actions);
      }
      if (ruleCallbackp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Rule %d action \"%s\" resolved to NULL", rulei, actions);
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Rule description is: %s", rulep->asciishows);
        goto err;
      } else {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Rule %d action \"%s\" resolved to %p", rulei, actions, ruleCallbackp);
      }
    }

    if (! ruleCallbackp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, arg0i, argni, resulti, 0 /* nullableb */)) {
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIFValueErrorProgressReportv(marpaESLIFValuep);
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  marpaESLIFValuep->inValuationb = 0;
  marpaESLIFValuep->symboli      = -1;
  marpaESLIFValuep->rulei        = -1;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_anySymbolCallbackWrapperb(void *userDatavp, int symboli, int argi, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char                    *funcs                 = "_marpaESLIFValue_anySymbolCallbackWrapperb";
  marpaESLIFValue_t                    *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t               *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFValueOption_t               marpaESLIFValueOption = marpaESLIFValuep->marpaESLIFValueOption;
  marpaESLIFValueSymbolActionResolver_t symbolActionResolverp = marpaESLIFValueOption.symbolActionResolverp;
  marpaESLIFValueRuleActionResolver_t   ruleActionResolverp   = marpaESLIFValueOption.ruleActionResolverp;
  marpaESLIFGrammar_t                  *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t                 *grammarp              = marpaESLIFGrammarp->grammarp;
  char                                 *bytep                 = NULL;
  size_t                                bytel                 = 0;
  marpaESLIFValueSymbolCallback_t       symbolCallbackp       = NULL;
  marpaESLIFValueRuleCallback_t         ruleCallbackp         = NULL;
  char                                 *actions               = NULL;
  marpaESLIF_symbol_t                  *symbolp;
  short                                 rcb;
  
  marpaESLIFValuep->inValuationb = 1;
  marpaESLIFValuep->symboli      = symboli;
  marpaESLIFValuep->rulei        = -1;

  marpaESLIFRecognizerp->callstackCounteri++;
#ifndef MARPAESLIF_NTRACE
  if (nullableb) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start(nullable=%d) [%d] ::=", (int) nullableb, resulti);
  } else {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start(nullable=%d) [%d] ~ [%d]", (int) nullableb, resulti, argi);
  }
#endif

  symbolp = _marpaESLIF_symbol_findp(marpaESLIFValuep->marpaESLIFp, grammarp, NULL /* asciis */, symboli);
  if (symbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Symbol %d %s", symbolp->idi, symbolp->descp->asciis);

  if (! nullableb) {
    if (! _marpaESLIFRecognizer_lexemeStack_i_p_and_sizeb(marpaESLIFRecognizerp, marpaESLIFRecognizerp->lexemeInputStackp, argi, &bytep, &bytel)) {
      goto err;
    }
  }

  /* If we have a parent recognizer, this mean that we are in the lexemode mode. Then */
  /* the action is forced to be our lexeme-to-stack. */
  if (marpaESLIFRecognizerp->parentRecognizerp != NULL) {
    symbolCallbackp = _marpaESLIF_lexeme_transferb;
  } else {
    /* If this is a nullable, we use the symbol's nullable semantic determined by grammar validation */
    if (nullableb) {
      actions = symbolp->nullableActions;
      if (actions == NULL) {
        actions = grammarp->defaultRuleActions;
      }
      if (actions == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Nullable symbol No %d (%s) from grammar level %d (%s) requires: action => action_name, or that your grammar have: :default ::= action => action_name",
                          symboli,
                          symbolp->descp->asciis,
                          grammarp->leveli,
                          grammarp->descp->asciis);
        goto err;
      }

      /* If this is a built-in action, we do not need the resolver */
      if (strcmp(actions, "::shift") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___shiftb;
      } else if (strcmp(actions, "::undef") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___undefb;
      } else if (strcmp(actions, "::ascii") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___asciib;
      } else if (strcmp(actions, "::translit") == 0) {
        ruleCallbackp = _marpaESLIF_rule_action___translitb;
      } else {
        if (ruleActionResolverp == NULL) {
          MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "No rule action resolver");
          goto err;
        }
        ruleCallbackp = ruleActionResolverp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, actions);
      }
      if (ruleCallbackp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Nullable symbol %d action \"%s\" resolved to NULL", symboli, actions);
        goto err;
      } else {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Nullable symbol %d action \"%s\" resolved to %p", symboli, actions, ruleCallbackp);
      }
    } else {
      actions = grammarp->defaultSymbolActions;
      if (actions == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Grammar No %d (%s) has no lexeme default action", grammarp->leveli, grammarp->descp->asciis);
        goto err;
      } else {
        /* If this is a built-in action, we do not need the resolver */
        if (strcmp(actions, "::shift") == 0) {
          symbolCallbackp = _marpaESLIF_symbol_action___shiftb;
        } else if (strcmp(actions, "::undef") == 0) {
          symbolCallbackp = _marpaESLIF_symbol_action___undefb;
        } else if (strcmp(actions, "::ascii") == 0) {
          symbolCallbackp = _marpaESLIF_symbol_action___asciib;
        } else if (strcmp(actions, "::translit") == 0) {
          symbolCallbackp = _marpaESLIF_symbol_action___translitb;
        } else {
          if (symbolActionResolverp == NULL) {
            MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "No symbol action resolver");
            goto err;
          }
          symbolCallbackp = symbolActionResolverp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, actions);
        }
        if (symbolCallbackp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Symbol %d action \"%s\" resolved to NULL", symboli, actions);
          goto err;
        } else {
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Symbol %d action \"%s\" resolved to %p", symboli, actions, symbolCallbackp);
        }
      }
    }
  }

  /* Here, one of symbolCallbackp or ruleCallbackp is guaranteed to be set */
  if (symbolCallbackp != NULL) {
    if (! symbolCallbackp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, bytep, bytel, resulti)) {
      goto err;
    }
  } else {
    if (! ruleCallbackp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, -1, -1, resulti, nullableb)) {
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIFValueErrorProgressReportv(marpaESLIFValuep);
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  marpaESLIFValuep->inValuationb = 0;
  marpaESLIFValuep->symboli      = -1;
  marpaESLIFValuep->rulei        = -1;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_symbolCallbackWrapperb(void *userDatavp, int symboli, int argi, int resulti)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_symbolCallbackWrapperb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFValue_anySymbolCallbackWrapperb(userDatavp, symboli, argi, resulti, 0 /* nullableb */);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_nullingCallbackWrapperb(void *userDatavp, int symboli, int resulti)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_nullingCallbackWrapperb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFValue_anySymbolCallbackWrapperb(userDatavp, symboli, -1 /* arg0i - not used when nullable is true */, resulti, 1 /* nullableb */);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  static const char      *funcs = "marpaESLIFValue_grammarp";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  marpaESLIFGrammar_t    *marpaESLIFGrammarp;;

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return NULL;
  }

  marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFGrammarp = marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFGrammarp;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %p", marpaESLIFGrammarp);
  marpaESLIFRecognizerp->callstackCounteri--;
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
short marpaESLIFValue_grammarib(marpaESLIFValue_t *marpaESLIFValuep, int *grammarip)
/*****************************************************************************/
{
  static const char      *funcs = "marpaESLIFValue_grammarib";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  marpaESLIFGrammar_t    *marpaESLIFGrammarp;;
  marpaESLIF_grammar_t   *grammarp;
  marpaESLIF_grammar_t   *thisgrammarp;
  int                     grammari;
  short                   foundb;
  genericStack_t         *grammarStackp;

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFGrammarp = marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFGrammarp;
  grammarp           = marpaESLIFGrammarp->grammarp; /* Current grammar */
  grammarStackp      = marpaESLIFGrammarp->grammarStackp;
  foundb = 0;
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    thisgrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    if (grammarp->selfp == thisgrammarp->selfp) {
      foundb = 1;
      if (grammarip != NULL) {
        *grammarip = grammari;
      }
    }
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) foundb);
  marpaESLIFRecognizerp->callstackCounteri--;

  return foundb;
}

/*****************************************************************************/
static inline void _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp != NULL) {
    _marpaESLIFGrammar_grammarStack_freev(marpaESLIFGrammarp, marpaESLIFGrammarp->grammarStackp);
    if (! onStackb) {
      free(marpaESLIFGrammarp);
    }
  }
}

/*****************************************************************************/
static inline void _marpaESLIFGrammar_grammarStack_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, genericStack_t *grammarStackp)
/*****************************************************************************/
{
  if (grammarStackp != NULL) {
    while (GENERICSTACK_USED(grammarStackp) > 0) {
      if (GENERICSTACK_IS_PTR(grammarStackp, GENERICSTACK_USED(grammarStackp) - 1)) {
        marpaESLIF_grammar_t *grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_POP_PTR(grammarStackp);
        _marpaESLIF_grammar_freev(grammarp);
      } else {
        GENERICSTACK_USED(grammarStackp)--;
      }
    }
    GENERICSTACK_FREE(grammarStackp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_p_and_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, char **pp, size_t *sizelp)
/*****************************************************************************/
{
  static const char                *funcs = "_marpaESLIFRecognizer_lexemeStack_i_p_and_sizeb";
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  char                             *p;
  size_t                            sizel;
  short                             rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (GENERICSTACK_IS_ARRAY(lexemeStackp, i)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, i);
    p = GENERICSTACK_ARRAYP_PTR(arrayp);
    sizel = GENERICSTACK_ARRAYP_LENGTH(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_i_types(lexemeStackp, i), i);
    goto err;
  }

  rcb = 1;
  *pp = p;
  *sizelp = sizel;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_i_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int i, size_t *sizelp)
/*****************************************************************************/
{
  static const char                *funcs = "_marpaESLIFRecognizer_lexemeStack_i_sizeb";
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  size_t                            sizel;
  short                             rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (GENERICSTACK_IS_ARRAY(lexemeStackp, i)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, i);
    sizel = GENERICSTACK_ARRAYP_LENGTH(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_i_types(lexemeStackp, i), i);
    goto err;
  }

  rcb = 1;
  *sizelp = sizel;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline const char *_marpaESLIF_genericStack_i_types(genericStack_t *stackp, int i)
/*****************************************************************************/
{
  const char *s;

  switch (GENERICSTACKITEMTYPE(stackp, i)) {
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
  static const char    *funcs              = "_marpaESLIFGrammar_symbolDescriptionCallback";
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
 static short _marpaESLIFGrammar_symbolOptionSetterNoEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/*****************************************************************************/
{
  static const char         *funcs                    = "_marpaESLIFGrammar_symbolOptionSetterNoEvent";
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

  if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE) {
    MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Resetting event set for symbol No %d (%s) at grammar level %d (%s)", symbolp->idi, symbolp->descp->asciis, grammarp->leveli, grammarp->descp->asciis);
    marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
 static short _marpaESLIFGrammar_symbolOptionSetterDiscardTop(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/*****************************************************************************/
{
  static const char         *funcs                    = "_marpaESLIFGrammar_symbolOptionSetterDiscardTop";
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

  /* A "discard" event is possible only for symbols that at "top-level" symbols in the current grammar */
  /* AND for which there is a ":discard ~ SYMBOL event => EVENTNAME setting */
  /* This will look like an internal event that the recognizer will catchup to create pause events on discard */
  if (symbolp->isDiscardTopb && symbolp->discardEvents) {
    marpaWrapperGrammarSymbolOptionp->eventSeti &= MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION;
    MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Setting discard event set for symbol No %d (%s) at grammar level %d (%s) on completion", symbolp->idi, symbolp->descp->asciis, grammarp->leveli, grammarp->descp->asciis);
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
  - buffers is containing unparsed data, and its size can vary at any time. ALWAYS SHARED with all parents.
  - inputs is always a pointer within buffers.                              ALWAYS SPECIFIC to current recognizer.
  - eofb is when EOF is hit.                                                ALWAYS SHARED with all parents.

  Handled in regex match:
  - encodings is eventual encoding information as per the user              ALWAYS SHARED with all parents.
  - utf8s is the UTF-8 conversion of buffer. Handled in regex match.        ALWAYS SHARED with all parents.
  
  Remember the semantics: from our point of view, reader is reading NEW data. We always append.
*/
{
  static const char            *funcs                      = "_marpaESLIFRecognizer_readb";
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption = marpaESLIFRecognizerp->marpaESLIFRecognizerOption;
  marpaESLIF_t                 *marpaESLIFp                = marpaESLIFRecognizerp->marpaESLIFp;
  char                         *inputs                     = NULL;
  char                         *encodingOfEncodings        = NULL;
  char                         *encodings                  = NULL;
  size_t                        encodingl                  = 0;
  size_t                        inputl                     = 0;
  short                         eofb                       = 0;
  short                         characterStreamb           = 0;
  char                         *utf8s                      = NULL;
  size_t                        utf8l;
  short                         rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp(marpaESLIFRecognizerOption.userDatavp, &inputs, &inputl, &eofb, &characterStreamb, &encodingOfEncodings, &encodings, &encodingl)) {
    MARPAESLIF_ERROR(marpaESLIFp, "reader failure");
    goto err;
  }

  /* We maintain here a very special thing: if there is EOF at the very first read, this mean that the user gave the whole stream */
  /* in ONE step: then removing PCRE2_ANCHORED is allowed. */
  if (*(marpaESLIFRecognizerp->nextReadIsFirstReadbp)) {
    *(marpaESLIFRecognizerp->noAnchorIsOkbp) = eofb;
    *(marpaESLIFRecognizerp->nextReadIsFirstReadbp) = 0; /* Next read will not be the first read */
  }

  if ((inputs != NULL) && (inputl > 0)) {
    /* Some new data is coming - remember delta before doing anything */
    size_t deltal = marpaESLIFRecognizerp->inputs - *(marpaESLIFRecognizerp->buffersp);

    if (characterStreamb) {
      /* ************************************************************************************************************************************************* */
      /* User say this is a stream of characters.                                                                                                          */
      /* ************************************************************************************************************************************************* */
      /* Here are the possible cases:                                                                                                                      */
      /* - Previous read was a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is true).                                                        */
      /*   [We MUST have the current input encodings in *(marpaESLIFRecognizerp->encodingsp) and a conversion engine in *(marpaESLIFRecognizerp->tconvpp)] */
      /*   [We MUST have a fake terminal associated to input encoding]                                                                                     */
      /*   - user gave encoding (encodings != NULL)                                                                                                        */
      /*     - If encodings and *(marpaESLIFRecognizerp->encodingsp) differ, current conversion engine is flushed, last state clean. A new one start.      */
      /*       >> Encoding aliases are not supported.                                                                                                      */
      /*       >> This mode does not support incomplete characters in the input streaming.                                                                 */
      /*     - If encodings and *(marpaESLIFRecognizerp->encodingsp) are the same, current conversion engine continue.                                     */
      /*       >> Encoding aliases are not supported.                                                                                                      */
      /*       >> This mode support incomplete characters in the input streaming.                                                                          */
      /*   - user gave NO encoding (encodings == NULL)                                                                                                     */
      /*     - It is assumed that current conversion can continue.                                                                                         */
      /*       >> This mode support incomplete characters in the input streaming.                                                                          */
      /* - Previous read was NOT a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is false).                                                   */
      /*   [Input encodings in *(marpaESLIFRecognizerp->encodingsp) should be NULL and current conversion in *(marpaESLIFRecognizerp->tconvpp) as well.]   */
      /*   - user gave encoding (encodings != NULL) or not                                                                                                 */
      /*     - This is used as-is in the call to _marpaESLIF_charconvp(). Current encoding and conversion engine are initialized.                          */
      /*                                                                                                                                                   */
      /* Input is systematically converted into UTF-8. If user said "UTF-8" it is equivalent to                                                            */
      /* an UTF-8 validation. The user MUST send a buffer information that contain full characters.                                                        */
      /* ************************************************************************************************************************************************* */
      if (*(marpaESLIFRecognizerp->charconvbp)) {
        /* ************************************************************************************************************************************************* */
        /* - Previous read was a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is true).                                                        */
        /* ************************************************************************************************************************************************* */
        if (*(marpaESLIFRecognizerp->encodingsp) == NULL) {
          MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is unknown");
          goto err;
        }
        if (*(marpaESLIFRecognizerp->tconvpp) == NULL) {
          MARPAESLIF_ERROR(marpaESLIFp, "Previous conversion engine is not set");
          goto err;
        }
        if (*(marpaESLIFRecognizerp->encodingpp) == NULL) {
          MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is not associated to a fake terminal");
          goto err;
        }
        if (encodings != NULL) {
          /* ************************************************************************************************************************************************* */
          /*   - user gave encoding (encodings != NULL)                                                                                                        */
          /* ************************************************************************************************************************************************* */
          if (! _marpaESLIFRecognizer_encoding_eqb(marpaESLIFRecognizerp, *(marpaESLIFRecognizerp->encodingpp), encodingOfEncodings, encodings, encodingl)) {
            /* ************************************************************************************************************************************************* */
            /*     - If encodings and *(marpaESLIFRecognizerp->encodingsp) differ, current conversion engine is flushed. A new one start.                        */
            /* ************************************************************************************************************************************************* */
            /* Flush current conversion engine */
            if (! _marpaESLIFRecognizer_flush_charconv(marpaESLIFRecognizerp)) {
              goto err;
            }
            /* Start a new one */
            if (! _marpaESLIFRecognizer_start_charconvp(marpaESLIFRecognizerp, encodingOfEncodings, encodings, encodingl, inputs, inputl)) {
              goto err;
            }
          } else {
            /* ************************************************************************************************************************************************* */
            /*     - If encodings and *(marpaESLIFRecognizerp->encodingsp) are the same, current conversion engine continue.                                     */
            /* ************************************************************************************************************************************************* */
            /* Continue with current conversion engine */
            utf8s = _marpaESLIF_charconvp(marpaESLIFp, "UTF-8", *(marpaESLIFRecognizerp->encodingsp), inputs, inputl, &utf8l, marpaESLIFRecognizerp->encodingsp, marpaESLIFRecognizerp->tconvpp);
            if (utf8s == NULL) {
              goto err;
            }
            if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, utf8s, utf8l)) {
              goto err;
            }
          }
        } else {
          /* ************************************************************************************************************************************************* */
          /*   - user gave NO encoding (encodings == NULL)                                                                                                     */
          /* ************************************************************************************************************************************************* */
          /* Continue with current conversion engine */
          utf8s = _marpaESLIF_charconvp(marpaESLIFp, "UTF-8", *(marpaESLIFRecognizerp->encodingsp), inputs, inputl, &utf8l, marpaESLIFRecognizerp->encodingsp, marpaESLIFRecognizerp->tconvpp);
          if (utf8s == NULL) {
            goto err;
          }
          if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, utf8s, utf8l)) {
            goto err;
          }
        }
      } else {
        /* ************************************************************************************************************************************************* */
        /* - Previous read was NOT a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is false).                                                   */
        /* ************************************************************************************************************************************************* */
        /* Start a new conversion engine */
        if (! _marpaESLIFRecognizer_start_charconvp(marpaESLIFRecognizerp, encodingOfEncodings, encodings, encodingl, inputs, inputl)) {
          goto err;
        }
      }
    } else {
      /* ************************************************************************************************************************************************* */
      /* User say this is not a stream of characters.                                                                                                      */
      /* ************************************************************************************************************************************************* */
      /* Here are the possible cases:                                                                                                                      */
      /* - Previous read was a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is true).                                                        */
      /*   [We MUST have the input encodings in *(marpaESLIFRecognizerp->encodingsp) and a current conversion engine in *(marpaESLIFRecognizerp->tconvpp)] */
      /*   - Current encoding is flushed.                                                                                                                  */
      /*   - Data is appended as-is.                                                                                                                       */
      /* - Previous read was NOT a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is false).                                                   */
      /*   - Data is appended as-is.                                                                                                                       */
      /* ************************************************************************************************************************************************* */
      if (*(marpaESLIFRecognizerp->charconvbp)) {
        /* ************************************************************************************************************************************************* */
        /* - Previous read was a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is true).                                                        */
        /* ************************************************************************************************************************************************* */
        /* Flush current conversion engine */
        if (! _marpaESLIFRecognizer_flush_charconv(marpaESLIFRecognizerp)) {
          goto err;
        }
        /* Data is appended as-is */
        if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, inputs, inputl)) {
          goto err;
        }
      } else {
        /* ************************************************************************************************************************************************* */
        /* - Previous read was NOT a stream of characters (*(marpaESLIFRecognizerp->charconvbp) is false).                                                   */
        /* ************************************************************************************************************************************************* */
        /* Data is appended as-is */
        if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, inputs, inputl)) {
          goto err;
        }
      }
    }
    /* We do not know UTF-8 correctness. */
    *(marpaESLIFRecognizerp->utfbp) = 0;
  }

  rcb = 1;
  (*marpaESLIFRecognizerp->eofbp) = eofb;
  goto done;

 err:
  rcb = 0;

 done:
  if (utf8s != NULL) {
    free(utf8s);
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIF_rule_createshowv(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep, char *asciishows, size_t *asciishowlp)
/*****************************************************************************/
{
  genericStack_t      *rhsStackp       = rulep->rhsStackp;
  genericStack_t      *exceptionStackp = rulep->exceptionStackp;
  marpaESLIF_symbol_t *symbolp;
  size_t               asciishowl;
  int                  exceptioni;
  size_t               exceptionl;
  int                  rhsi;
  char                 tmps[1024];
  char                 quote[2][2];

  /* Calculate the size needed to show the rule in ASCII form */
  asciishowl = 1 + strlen(rulep->lhsp->descp->asciis) + 1; /* "<LHS>" */
  if (asciishows != NULL) {
    strcpy(asciishows, "<");
    strcat(asciishows, rulep->lhsp->descp->asciis);
    strcat(asciishows, ">");
  }
  asciishowl += 4;                              /* " ::=" */
  if (asciishows != NULL) {
    strcat(asciishows, " ::=");
  }
  for (rhsi = 0; rhsi < GENERICSTACK_USED(rhsStackp); rhsi++) {
    if (! GENERICSTACK_IS_PTR(rhsStackp, rhsi)) {
      continue;
    }
    symbolp = GENERICSTACK_GET_PTR(rhsStackp, rhsi);

    asciishowl++;                              /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      asciishowl += strlen(symbolp->descp->asciis); /* RHS description (ASCII approximation) */
      if (asciishows != NULL) {
        strcat(asciishows, symbolp->descp->asciis);
      }
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      asciishowl += 1 + strlen(symbolp->u.metap->asciinames) + 1; /* RHS description (ASCII) */
      if (asciishows != NULL) {
        strcat(asciishows, "<");
        strcat(asciishows, symbolp->u.metap->asciinames);
        strcat(asciishows, ">");
      }
    default:
      break;
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
      asciishowl += strlen(rulep->separatorp->descp->asciis); /* Separator description (ASCII) - Note: separator can be a direct terminal */
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
      switch (symbolp->type) {
      case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
        asciishowl += strlen(symbolp->descp->asciis); /* RHS description (ASCII approximation) */
        if (asciishows != NULL) {
          strcat(asciishows, symbolp->descp->asciis);
        }
        break;
      case MARPAESLIF_SYMBOL_TYPE_META:
        asciishowl += strlen(symbolp->u.metap->asciinames); /* RHS description (ASCII) */
        if (asciishows != NULL) {
          strcat(asciishows, symbolp->u.metap->asciinames);
        }
      default:
        break;
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
  if (rulep->actions != NULL) {
    asciishowl++;                                 /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    asciishowl += strlen("action => "); /* "action => 'actionname' */
    if (asciishows != NULL) {
      strcat(asciishows, "action => ");
    }
    asciishowl += 1 + strlen(rulep->actions) + 1; /* 'actionname' */
    if (asciishows != NULL) {
      strcat(asciishows, "'");
      strcat(asciishows, rulep->actions);
      strcat(asciishows, "'");
    }
  }
  if (strcmp(rulep->lhsp->descp->asciis, rulep->descp->asciis) != 0) {
    asciishowl++;                                 /* space */
    if (asciishows != NULL) {
      strcat(asciishows, " ");
    }
    asciishowl += strlen("name => "); /* "name => naming */
    if (asciishows != NULL) {
      strcat(asciishows, "name => ");
    }
    /* Try to be clever by determining the quote possibility */
    if (strchr(rulep->descp->asciis, '\'') == NULL) {
      strcpy(quote[0], "'");
      strcpy(quote[1], "'");
    } else if (strchr(rulep->descp->asciis, '"') == NULL) {
      strcpy(quote[0], "\"");
      strcpy(quote[1], "\"");
    } else if (strchr(rulep->descp->asciis, '}') == NULL) {
      strcpy(quote[0], "{");
      strcpy(quote[1], "}");
    } else {
      strcpy(quote[0], "");
      strcpy(quote[1], "");
    }
    if (strlen(quote[0]) > 0) {
      asciishowl += 1;
      if (asciishows != NULL) {
        strcat(asciishows, quote[0]);
      }
    }
    asciishowl += strlen(rulep->descp->asciis);
    if (asciishows != NULL) {
      strcat(asciishows, rulep->descp->asciis);
    }
    if (strlen(quote[1]) > 0) {
      asciishowl += 1;
      if (asciishows != NULL) {
        strcat(asciishows, quote[1]);
      }
    }
  }
  asciishowl++; /* NUL byte */

  if (asciishowlp != NULL) {
    *asciishowlp = asciishowl;
  }
}

/*****************************************************************************/
static inline int _marpaESLIF_utf82ordi(PCRE2_SPTR8 utf8bytes, marpaESLIF_uint32_t *uint32p)
/*****************************************************************************/
/* This is a copy of utf2ord from pcre2test.c
-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
/* This function reads one or more bytes that represent a UTF-8 character,
and returns the codepoint of that character. Note that the function supports
the original UTF-8 definition of RFC 2279, allowing for values in the range 0
to 0x7fffffff, up to 6 bytes long. This makes it possible to generate
codepoints greater than 0x10ffff which are useful for testing PCRE2's error
checking, and also for generating 32-bit non-UTF data values above the UTF
limit.

Argument:
  utf8bytes   a pointer to the byte vector
  vptr        a pointer to an int to receive the value

Returns:      >  0 => the number of bytes consumed
              -6 to 0 => malformed UTF-8 character at offset = (-return)
*/
/*****************************************************************************/
{
  marpaESLIF_uint32_t c = *utf8bytes++;
  marpaESLIF_uint32_t d = c;
  int i, j, s;
  const int utf8_table1[] = { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};
  const int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};
  const int utf8_table1_size = sizeof(utf8_table1) / sizeof(int);

  for (i = -1; i < 6; i++) {               /* i is number of additional bytes */
    if ((d & 0x80) == 0) break;
    d <<= 1;
  }

  if (i == -1) {
    /* ascii character */
    *uint32p = c;
    return 1;
  }
  if (i == 0 || i == 6) {
    return 0;
  } /* invalid UTF-8 */

  /* i now has a value in the range 1-5 */

  s = 6*i;
  d = (c & utf8_table3[i]) << s;

  for (j = 0; j < i; j++) {
    c = *utf8bytes++;
    if ((c & 0xc0) != 0x80) {
      return -(j+1);
    }
    s -= 6;
    d |= (c & 0x3f) << s;
  }

  /* Check that encoding was the correct unique one */

  for (j = 0; j < utf8_table1_size; j++) {
    if (d <= (uint32_t)utf8_table1[j]) {
      break;
    }
  }
  if (j != i) {
    return -(i+1);
  }

  /* Valid value */

  *uint32p = d;
  return i+1;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_matchPostProcessingb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t matchl)
/*****************************************************************************/
{
  static const char          *funcs = "_marpaESLIFRecognizer_matchPostProcessingb";
  marpaESLIF_terminal_t      *newlinep;
  marpaESLIF_terminal_t      *anycharp;
  char                       *linep;
  size_t                      linel;
  marpaESLIF_matcher_value_t  rci;
  short                       rcb;
  marpaESLIFValueResult_t     marpaESLIFValueResult;
    
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* If newline counting is on, so do we - only at first level */
  if (marpaESLIFRecognizerp->marpaESLIFRecognizerOption.newlineb && (marpaESLIFRecognizerp->leveli == 0)) {
    newlinep = marpaESLIFRecognizerp->marpaESLIFp->newlinep;
    anycharp = marpaESLIFRecognizerp->marpaESLIFp->anycharp;
    linep = marpaESLIFRecognizerp->inputs;
    linel = matchl;

    /* Check newline */
    while (1) {
      /* We count newlines only when a discard or a complete has happened. So by definition */
      /* character sequences are complete. This is why we fake EOF to true. */
      if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp,
                                                 newlinep,
                                                 linep,
                                                 linel,
                                                 1, /* eofb */
                                                 &rci,
                                                 &marpaESLIFValueResult)) {
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_OK) {
        break;
      }
      /* It is a non-sense to have a regex match returning something else but MARPAESLIF_STACK_TYPE_ARRAY */
      if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "newline regex returned type %d}", marpaESLIFValueResult.type);
        goto err;
      }
      /* It is a non-sense to have a match returning nothing */
      if (marpaESLIFValueResult.u.p == NULL) {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex return NULL pointer");
        goto err;
      }
      /* It is a non-sense to have a match returning nothing */
      if (marpaESLIFValueResult.sizel <= 0) {
        MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex matched zero byte");
        goto err;
      }
      linep += marpaESLIFValueResult.sizel;
      linel -= marpaESLIFValueResult.sizel;
      free(marpaESLIFValueResult.u.p);
      /* A new line, reset column count */
      marpaESLIFRecognizerp->linel++;
      marpaESLIFRecognizerp->columnl = 0;
    }

    if (linel > 0) {
      /* Count characters */
      while (1) {
        /* We count newlines only when a discard or a complete has happened. So by definition */
        /* character sequences are complete. This is why we fake EOF to true. */
        if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp,
                                                   anycharp,
                                                   linep,
                                                   linel,
                                                   1, /* eofb */
                                                   &rci,
                                                   &marpaESLIFValueResult)) {
          goto err;
        }
        if (rci != MARPAESLIF_MATCH_OK) {
          break;
        }
        /* It is a non-sense to have a regex match returning something else but MARPAESLIF_STACK_TYPE_ARRAY */
        if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
          MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "newline regex returned type %d}", marpaESLIFValueResult.type);
          goto err;
        }
        /* It is a non-sense to have a match returning nothing */
        if (marpaESLIFValueResult.u.p == NULL) {
          MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex return NULL pointer");
          goto err;
        }
        /* It is a non-sense to have a match returning nothing */
        if (marpaESLIFValueResult.sizel <= 0) {
          MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex matched zero byte");
          goto err;
        }
        linep += marpaESLIFValueResult.sizel;
        linel -= marpaESLIFValueResult.sizel;
        free(marpaESLIFValueResult.u.p);
        /* A new character */
        marpaESLIFRecognizerp->columnl++;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli, void *userDatavp, marpaESLISymbolDescriptionCallback_t symbolDescriptionCallbackp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFRecognizer_progressLogb";
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = marpaWrapperRecognizer_progressLogb(marpaESLIFRecognizerp->marpaWrapperRecognizerp,
                                            starti,
                                            endi,
                                            logleveli,
                                            userDatavp,
                                            symbolDescriptionCallbackp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
marpaESLIFRecognizer_t *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  static const char      *funcs = "marpaESLIFValue_recognizerp";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return NULL;
  }

  marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %p", marpaESLIFRecognizerp);
  marpaESLIFRecognizerp->callstackCounteri--;
  return marpaESLIFRecognizerp;
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFRecognizer_grammarp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char   *funcs = "marpaESLIFRecognizer_grammarp";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %p", marpaESLIFGrammarp);
  marpaESLIFRecognizerp->callstackCounteri--;
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *datas, size_t datal)
/*****************************************************************************/
{
  static const char *funcs        = "_marpaESLIFRecognizer_appendDatab";
  char              *buffers      = *(marpaESLIFRecognizerp->buffersp);
  size_t             bufferallocl = *(marpaESLIFRecognizerp->bufferalloclp);
  size_t             bufferl      = *(marpaESLIFRecognizerp->bufferlp);
  size_t             inputl       = marpaESLIFRecognizerp->inputl;
  size_t             deltal       = marpaESLIFRecognizerp->inputs - buffers;
  size_t             bufsizl      = marpaESLIFRecognizerp->bufsizl;
  size_t             buftriggerl  = marpaESLIFRecognizerp->buftriggerl;
  unsigned int       bufaddperci  = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.bufaddperci;
  size_t             wantedl;
  size_t             minwantedl;
  char              *tmps;
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start (datas=%p, datal=%ld)", datas, (unsigned long) datal);

  if (datal <= 0) {
    /* Nothing to do */
    rcb = 1;
    goto done;
  }
  if (marpaESLIFRecognizerp->parentRecognizerp == NULL) {
    /* We can crunch data at any time. */

    if ((bufferallocl > buftriggerl) /* If we allocated more than the trigger */
        &&                           /* and */
        (inputl < bufsizl)           /* there is less remaining bytes to process than minimum buffer size */
        ) {
      /* ... then we can realloc to minimum buffer size */

      /* Before reallocating, we need to move the remaining bytes at the beginning */
      memmove(buffers, buffers + inputl, inputl);
      /* Try to realloc */
      wantedl = bufsizl;
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Resizing internal buffer size from %ld bytes to %ld bytes", (unsigned long) bufferallocl, (unsigned long) wantedl);
      tmps = realloc(buffers, wantedl);
      if (tmps == NULL) {
        /* We could have continue, this is not truely fatal - but we are in a bad shape anyway -; */
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }
      buffers      = *(marpaESLIFRecognizerp->buffersp)      = tmps;        /* Buffer pointer */
      bufferallocl = *(marpaESLIFRecognizerp->bufferalloclp) = wantedl;     /* Allocated size */
      bufferl      = *(marpaESLIFRecognizerp->bufferlp)      = inputl;      /* Number of valid bytes */
      /* Pointer inside internal buffer is back to the beginning */
      marpaESLIFRecognizerp->inputs = buffers;
    }
  }

  /* Append data */
  if (buffers == NULL) {
    /* First time we put in the buffer */
    wantedl = (bufsizl < datal) ? datal : bufsizl;
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Creating an internal buffer of %ld bytes", (unsigned long) wantedl);
    tmps = (char *) malloc(wantedl);
    if (tmps == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    buffers      = *(marpaESLIFRecognizerp->buffersp)      = tmps;        /* Buffer pointer */
    bufferallocl = *(marpaESLIFRecognizerp->bufferalloclp) = wantedl;     /* Allocated size */
    bufferl      = *(marpaESLIFRecognizerp->bufferlp)      = 0;           /* Number of valid bytes */
    /* Pointer inside internal buffer is at the beginning */
    marpaESLIFRecognizerp->inputs = buffers;
  } else {
    wantedl = bufferl + datal;
    if (wantedl > bufferallocl) {
      /* We need more bytes than what has been allocated. Apply augment policy */
      minwantedl = (bufferallocl * (1 + bufaddperci)) / 100;
      if (wantedl < minwantedl) {
        wantedl = minwantedl;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Resizing an internal buffer from %ld bytes to %ld bytes", (unsigned long) bufferl, (unsigned long) wantedl);
      tmps = realloc(buffers, wantedl);
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }
      buffers      = *(marpaESLIFRecognizerp->buffersp)      = tmps;        /* Buffer pointer */
      bufferallocl = *(marpaESLIFRecognizerp->bufferalloclp) = wantedl;     /* Allocated size */
      /* Pointer inside internal buffer is moving */
      marpaESLIFRecognizerp->inputs = buffers + deltal;
    }
  }

  /* In any case, append data just after the valid bytes */
  memcpy(buffers + bufferl, datas, datal);

  /* Commit number of valid bytes, and number of remaining bytes to process */
  (*marpaESLIFRecognizerp->bufferlp) += datal;
  marpaESLIFRecognizerp->inputl      += datal;

  /* Please see the free method for the impact on parent's current pointer in input   */
  /* This need to be done once only, at return, this is why it is done at free level. */
  /* Note that when we create a grand child we strip off ALL events, so the user can */
  /* never got control back until we are finished. I.e. until all the free methods of */
  /* all the children are executed -; */

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_encoding_eqb(marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_terminal_t *terminalp, char *encodings, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char         *funcs                 = "_marpaESLIFRecognizer_encoding_eqb";
  marpaESLIF_t              *marpaESLIFp           = marpaESLIFRecognizerParentp->marpaESLIFp;
  marpaESLIFRecognizer_t    *marpaESLIFRecognizerp = NULL;
  char                      *utf8s;
  size_t                     utf8l;
  marpaESLIFGrammar_t        marpaESLIFGrammar;
  marpaESLIF_matcher_value_t rci;
  short                      rcb;

  marpaESLIFRecognizerParentp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerParentp, funcs, "start");

  /* First we want to make sure that the inputs is in UTF-8 */
  utf8s = _marpaESLIF_charconvp(marpaESLIFRecognizerp->marpaESLIFp, "UTF-8", encodings, inputs, inputl, &utf8l, NULL /* fromEncodingsp */, NULL /* tconvpp */);
  if (utf8s == NULL) {
    goto err;
  }

  /* terminalp points to a case-insensitive string match terminal */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
#ifndef MARPAESLIF_NTRACE
  MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerParentp, "Trying to match", terminalp->descp->asciis, utf8s, utf8l, 1 /* traceb */);
#endif
  /* Try to match */
  if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, terminalp, utf8s, utf8l, 1 /* eofb */, &rci, NULL /* marpaESLIFValueResult */)) {
    goto err;
  }
  if (rci != MARPAESLIF_MATCH_OK) {
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerParentp, funcs, "Encodings do not match");
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (utf8s != NULL) {
    free(utf8s);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerParentp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerParentp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_flush_charconv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_flush_charconv";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  char              *utf8s;
  size_t             utf8l;
  short              rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* It is a non-sense to flush a character conversion engine if we were not already in this state */
  if (! *(marpaESLIFRecognizerp->charconvbp)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous state says character conversion is off");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->encodingsp) == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is unknown");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->tconvpp) == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous conversion engine is not set");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->encodingpp) == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is not associated to a fake terminal");
    goto err;
  }

  utf8s = _marpaESLIF_charconvp(marpaESLIFp, NULL /* toEncodings, was "UTF-8" */, NULL /* fromEncodings */, NULL /* srcs */, 0 /* srcl */, &utf8l /* dstlp */, NULL /* fromEncodingsp */, marpaESLIFRecognizerp->tconvpp);
  if (utf8s == NULL) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, utf8s, utf8l)) {
    goto err;
  }

  /* last state is cleaned */
  free(*(marpaESLIFRecognizerp->encodingsp));
  *(marpaESLIFRecognizerp->encodingsp) = NULL;

  _marpaESLIF_terminal_freev(*(marpaESLIFRecognizerp->encodingpp));
  *(marpaESLIFRecognizerp->encodingpp) = NULL;
  
  if (tconv_close(*(marpaESLIFRecognizerp->tconvpp)) != 0) {
    MARPAESLIF_ERRORF(marpaESLIFp, "tconv_close failure, %s", strerror(errno));
    *(marpaESLIFRecognizerp->tconvpp) = NULL; /* A priori a retry is a bad idea, even during general cleanup... */
    goto err;
  }
  *(marpaESLIFRecognizerp->tconvpp) = NULL;

  /* Put global flag to off */
  *(marpaESLIFRecognizerp->charconvbp) = 0;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (utf8s != NULL) {
    free(utf8s);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_start_charconvp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *encodingOfEncodings, char *encodings, size_t encodingl, char *srcs, size_t srcl)
/*****************************************************************************/
{
  static const char          *funcs           = "_marpaESLIFRecognizer_start_charconvp";
  marpaESLIF_t               *marpaESLIFp     = marpaESLIFRecognizerp->marpaESLIFp;
  char                       *encodingasciis  = NULL;
  char                       *encodingutf8s   = NULL;
  char                       *utf8s           = NULL;
  char                       *utf8withoutboms;
  marpaESLIF_matcher_value_t  rci;
  marpaESLIFValueResult_t     marpaESLIFValueResult;
  size_t                      utf8withoutboml;
  size_t                      utf8l;
  short                       rcb;
  size_t                      encodingutf8l;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* It is a non-sense to start a character conversion engine if we were already in this state */
  if (*(marpaESLIFRecognizerp->charconvbp)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous state says character conversion is on");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->encodingsp) != NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is already known");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->tconvpp) != NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous conversion engine is already set");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->encodingpp) != NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Previous encoding is already associated to a fake terminal");
    goto err;
  }

  /* Get an eventual ASCII version of input encoding */
  if ((encodings != NULL) && (encodingl > 0)) {
    /* Encoding is in ASCII as per IANA - we do not not conform exactly to IANA in the sense they say that non ASCII should be translated to "_" if I remember well */
    /* We ignore non-ASCII characters. */
    encodingasciis = _marpaESLIF_charconvp(marpaESLIFp, "ASCII//IGNORE", encodingOfEncodings, encodings, encodingl, NULL /* dstlp */, NULL /* fromEncodingsp */, NULL /* tconvpp */);
    if (encodingasciis == NULL) {
      goto err;
    }
  }

  /* Convert input */
  utf8s = _marpaESLIF_charconvp(marpaESLIFp, "UTF-8", encodingasciis, srcs, srcl, &utf8l, marpaESLIFRecognizerp->encodingsp, marpaESLIFRecognizerp->tconvpp);
  if (utf8s == NULL) {
    goto err;
  }

  /* Verify information is set */
  if (*(marpaESLIFRecognizerp->encodingsp) == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Encoding has not been set");
    goto err;
  }
  if (*(marpaESLIFRecognizerp->tconvpp) == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Conversion engine has not been set");
    goto err;
  }

  /* Get an UTF-8 version of encoding - always set at this stage in *(marpaESLIFRecognizerp->encodingsp) */
  encodingutf8s = _marpaESLIF_charconvp(marpaESLIFp, "UTF-8", "ASCII", *(marpaESLIFRecognizerp->encodingsp), strlen(*(marpaESLIFRecognizerp->encodingsp)), &encodingutf8l, NULL /* fromEncodingsp */, NULL /* tconvpp */);
  if (encodingutf8s == NULL) {
    goto err;
  }

  /* Create terminal regexp corresponding to the encoding */
  *(marpaESLIFRecognizerp->encodingpp) = _marpaESLIF_terminal_newp(marpaESLIFp,
                                                                   NULL /* grammarp */,
                                                                   MARPAESLIF_EVENTTYPE_NONE, /* eventSeti */
                                                                   "ASCII", /* descEncodings */
                                                                   *(marpaESLIFRecognizerp->encodingsp) /* descs */,
                                                                   strlen(*(marpaESLIFRecognizerp->encodingsp)) /* descl */,
                                                                   MARPAESLIF_TERMINAL_TYPE_REGEX, /* type */
                                                                   MARPAESLIF_REGEX_OPTION_CASELESS, /* opti */
                                                                   encodingutf8s, /* utf8s */
                                                                   encodingutf8l, /* utf8l */
                                                                   NULL, /* testFullMatchs */
                                                                   NULL  /* testPartialMatchs */
                                                                   );
  if (*(marpaESLIFRecognizerp->encodingpp) == NULL) {
    goto err;
  }

  /* Remove eventually the BOM */
  if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, marpaESLIFp->utf8bomp, utf8s, utf8l, 1 /* eofb */, &rci, &marpaESLIFValueResult)) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* It is a non-sense to have a regex match returning something else but MARPAESLIF_STACK_TYPE_ARRAY */
    if (marpaESLIFValueResult.type != MARPAESLIF_STACK_TYPE_ARRAY) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "newline regex returned type %d}", marpaESLIFValueResult.type);
      goto err;
    }
    /* It is a non-sense to have a match returning nothing */
    if (marpaESLIFValueResult.u.p == NULL) {
      MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex return NULL pointer");
      goto err;
    }
    /* It is a non-sense to have a match returning nothing */
    if (marpaESLIFValueResult.sizel <= 0) {
      MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "newline regex matched zero byte");
      goto err;
    }
    utf8withoutboms = utf8s + marpaESLIFValueResult.sizel;
    utf8withoutboml = utf8l - marpaESLIFValueResult.sizel;
    free(marpaESLIFValueResult.u.p);
  } else {
    utf8withoutboms = utf8s;
    utf8withoutboml = utf8l;
  }
  if (! _marpaESLIFRecognizer_appendDatab(marpaESLIFRecognizerp, utf8withoutboms, utf8withoutboml)) {
    goto err;
  }

  /* Put global flag to on */
  *(marpaESLIFRecognizerp->charconvbp) = 1;
  /* And because we hardcode conversion to UTF-8, we know UTF-8 correctness is on. This has a major impact on regex in UTF mode. */
  *(marpaESLIFRecognizerp->utfbp) = 1;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (encodingasciis != NULL) {
    free(encodingasciis);
  }
  if (encodingutf8s != NULL) {
    free(encodingutf8s);
  }
  if (utf8s != NULL) {
    free(utf8s);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFValue_value_startb(marpaESLIFValue_t *marpaESLIFValuep, int *startip)
/*****************************************************************************/
{
  return marpaWrapperValue_value_startb(marpaESLIFValuep->marpaWrapperValuep, startip);
}

/*****************************************************************************/
short marpaESLIFValue_value_lengthb(marpaESLIFValue_t *marpaESLIFValuep, int *lengthip)
/*****************************************************************************/
{
  return marpaWrapperValue_value_lengthb(marpaESLIFValuep->marpaWrapperValuep, lengthip);
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIF_grammarp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  if (marpaESLIFp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  return marpaESLIFp->marpaESLIFGrammarp;
}

/*****************************************************************************/
short marpaESLIFGrammar_ngrammari(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *ngrammarip)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (ngrammarip != NULL) {
    *ngrammarip = GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp);
  }

  return 1;
}

/*****************************************************************************/
static inline void _marpaESLIFValueErrorProgressReportv(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValueErrorProgressReportv";
  marpaESLIF_t           *marpaESLIFp                = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp      = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                     starti;
  int                     lengthi;

  /* If we were generating the value of the top level recognizer, modulo discard that is also at same level, log the error */
  if ((marpaESLIFRecognizerp->leveli == 0) && (! marpaESLIFRecognizerp->discardb)) {
    if (marpaESLIFValue_value_startb(marpaESLIFValuep, &starti) &&
        marpaESLIFValue_value_lengthb(marpaESLIFValuep, &lengthi)) {
      marpaESLIFRecognizer_progressLogb(marpaESLIFValuep->marpaESLIFRecognizerp,
                                        starti,
                                        /* lengthi is zero when this is a MARPA_STEP_NULLABLE_SYMBOL */
                                        (lengthi > 0) ? starti+lengthi-1 : starti,
                                        GENERICLOGGER_LOGLEVEL_ERROR,
                                        marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIF_resolveGrammarb(marpaESLIF_t *marpaESLIFp, genericStack_t *grammarStackp, marpaESLIF_grammar_t *current_grammarp, char *asciis, int lookupLevelDeltai, marpaESLIF_string_t *lookupGrammarStringp, marpaESLIF_grammar_t **grammarpp, marpaESLIF_symbol_t **symbolpp)
/*****************************************************************************/
{
  static const char     *funcs = "_marpaESLIF_resolveGrammarb";
  marpaESLIF_grammar_t  *thisGrammarp;
  marpaESLIF_grammar_t  *grammarp;
  marpaESLIF_symbol_t   *symbolp;
  int                    grammari;
  short                  rcb;

  if (grammarStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "grammarStackp is NULL");
    goto err;
  }
  if (current_grammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "current_grammarp is NULL");
    goto err;
  }
  if (asciis == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "asciis is NULL");
    goto err;
  }
  
  grammarp = NULL;
  /* First look for the grammar */
  if (lookupGrammarStringp != NULL) {
    /* Look for such a grammar description */
    for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
      if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
        continue;
      }
      thisGrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
      if (_marpaESLIF_string_eqb(thisGrammarp->descp, lookupGrammarStringp)) {
        grammarp = thisGrammarp;
        break;
      }
    }
  } else {
    /* RHS level is relative - if RHS level is 0 the we fall back to current grammar */
    grammari = current_grammarp->leveli + lookupLevelDeltai;
    if ((grammari >= 0) && GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    }
  }

  if (grammarp == NULL) {
    goto err;
  }

  /* Then look into this grammar */
  symbolp = _marpaESLIF_symbol_findp(marpaESLIFp, grammarp, asciis, -1 /* symboli */);
  if (symbolp == NULL) {
    goto err;
  }

  if (grammarpp != NULL) {
    *grammarpp = grammarp;
  }
  if (symbolpp != NULL) {
    *symbolpp = symbolp;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIF_generateHelperb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, char *userspaceincludes, char *freeMethodNames, char *copyMethodNames, char **helpersp)
/*****************************************************************************/
{
  /* Given a grammar, generate helper source code that handle stack management */
  static const char            *funcs = "marpaESLIF_generateHelper";
  marpaESLIF_stringGenerator_t  stringGenerator;
  genericLogger_t              *genericLoggerp = NULL;
  genericStack_t               *grammarStackp;
  int                           grammari;
  short                         rcb;

  if ((marpaESLIFp == NULL) || (marpaESLIFGrammarp == NULL)) {
    errno = EINVAL;
    goto err;
  }

  stringGenerator.marpaESLIFp = marpaESLIFp;
  stringGenerator.s           = NULL;
  stringGenerator.l           = 0;
  stringGenerator.okb         = 0;

  genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    goto err;
  }

  if (! _marpaESLIF_generateHelper_includeb(marpaESLIFp, genericLoggerp, &stringGenerator, userspaceincludes)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_externb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, &stringGenerator, freeMethodNames, copyMethodNames)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_macrosb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, &stringGenerator)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_managedStack_structureb(marpaESLIFp, genericLoggerp, &stringGenerator)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_managedStack_freeb(marpaESLIFp, genericLoggerp, &stringGenerator)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_managedStack_enumb(marpaESLIFp, genericLoggerp, &stringGenerator)) {
    goto err;
  }

  /* We loop on all the grammars */
  grammarStackp = marpaESLIFGrammarp->grammarStackp;
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    if (! _marpaESLIF_generateHelper_by_grammarb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, &stringGenerator, grammari, freeMethodNames, copyMethodNames)) {
      goto err;
    }
  }

  /* Use the per-grammar methods */
  if (! _marpaESLIF_generateHelper_managedStack_ruleCallbackb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, &stringGenerator)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_managedStack_symbolCallbackb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, &stringGenerator)) {
    goto err;
  }
  /* Use the top-level grammar wrappers */
  if (! _marpaESLIF_generateHelper_managedStack_newb(marpaESLIFp, genericLoggerp, &stringGenerator)) {
    goto err;
  }

  if (! stringGenerator.okb) {
    goto err;
  }

  if (helpersp != NULL) {
    *helpersp = stringGenerator.s;
  } else {
    free(stringGenerator.s);
  }
  rcb = 1;
  goto done;
  
 err:
  if (stringGenerator.s != NULL) {
    free(stringGenerator.s);
  }
  rcb = 0;

 done:
  GENERICLOGGER_FREE(genericLoggerp);
  return rcb;
}

#undef  MARPAESLIF_GEN_OK
#define MARPAESLIF_GEN_OK do { if (! stringGeneratorp->okb) { goto err;  } } while (0)
/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_by_grammarb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari, char *freeMethodNames, char *copyMethodNames)
/*****************************************************************************/
{
  static const char    *funcs    = "_marpaESLIF_generateHelper_by_grammarb";
  marpaESLIF_grammar_t *grammarp = GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammari);
  short                 rcb;

  GENERICLOGGER_TRACE (genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "/* ---------------------------------------- */\n", grammarp->leveli); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "/* Grammar level %d stack management helpers */\n", grammarp->leveli); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "/* ---------------------------------------- */\n", grammarp->leveli); MARPAESLIF_GEN_OK;

  if (! _marpaESLIF_generateHelper_by_grammar_symbolCallbackb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, stringGeneratorp, grammari)) {
    goto err;
  }
  if (! _marpaESLIF_generateHelper_by_grammar_ruleCallbackb(marpaESLIFp, marpaESLIFGrammarp, genericLoggerp, stringGeneratorp, grammari)) {
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_by_grammar_symbolCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari)
/*****************************************************************************/
{
  static const char    *funcs        = "_marpaESLIF_generateHelper_by_grammar_symbolCallbackb";
  marpaESLIF_grammar_t *grammarp     = GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammari);
  genericStack_t       *symbolStackp = grammarp->symbolStackp;
  char                 *ascii2ids    = NULL;
  int                   marpaWrapperSymbolPropertyBitSet;
  char                 *actions;
  int                   symboli;
  marpaESLIF_symbol_t  *symbolp;
  short                 rcb;

  /* Generate all symbol callbacks - a symbol callback is a "terminal" - where a terminal is everything that is not used as an LHS */
  
  GENERICLOGGER_TRACE (genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "/*\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, " * Symbol callbacks of grammar No %d (%s)\n", grammarp->leveli, grammarp->descp->asciis); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, " */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "static int marpaESLIF_managedStack_symbolCallback_level%d(marpaESLIF_managedStack_t *managedStackp, marpaESLIFValue_t *marpaESLIFValuep, int symboli, char *bytep, size_t bytel, int resulti) {\n", grammarp->leveli); MARPAESLIF_GEN_OK;
  actions = grammarp->defaultSymbolActions;
  if (actions == NULL) {
    MARPAESLIF_WARNF(marpaESLIFp, "  /* WARNING: Grammar No %d (%s) has no symbol action */", grammarp->leveli, grammarp->descp->asciis);
    GENERICLOGGER_TRACE (genericLoggerp, "  /* WARNING: This grammar has no symbol action */\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "  return 0;\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;
  } else {
    ascii2ids = _marpaESLIF_ascii2ids(marpaESLIFp, actions);
    if (ascii2ids == NULL) {
      goto err;
    }
    GENERICLOGGER_TRACE (genericLoggerp, "  if (managedStackp == NULL) {\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "    return 0; /* Should never happen -; */\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACEF(genericLoggerp, "  switch (symboli) {\n", grammarp->leveli); MARPAESLIF_GEN_OK;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      /* Only for meta symbols */
      if (symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) {
        continue;
      }
      /* Nullable meta symbol ? */
      if (! marpaWrapperGrammar_symbolPropertyb(grammarp->marpaWrapperGrammarStartp, symboli, &marpaWrapperSymbolPropertyBitSet)) {
        goto err;
      }
      /* Only for non-LHS meta symbols OR nullable LHS's */
      if (symbolp->lhsb && ((marpaWrapperSymbolPropertyBitSet & MARPAWRAPPER_SYMBOL_IS_NULLABLE) != MARPAWRAPPER_SYMBOL_IS_NULLABLE)) {
        continue;
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "  case %d:\n", symbolp->idi); MARPAESLIF_GEN_OK;
      if (! symbolp->lhsb) {
        /* Then it is a nullable LHS */
        GENERICLOGGER_TRACEF(genericLoggerp, "    /* NULLABLE LHS: %s, bytep will be NULL and bytel will be 0 */\n", symbolp->descp->asciis); MARPAESLIF_GEN_OK;
      } else {
        GENERICLOGGER_TRACEF(genericLoggerp, "    /* %s */\n", symbolp->descp->asciis); MARPAESLIF_GEN_OK;
      }
      if (actions != NULL) {
        GENERICLOGGER_TRACEF(genericLoggerp, "    return %s(managedStackp->userDatavp, bytep, bytel, resulti);\n", ascii2ids); MARPAESLIF_GEN_OK;
      } else {
        GENERICLOGGER_TRACE(genericLoggerp, "    return 0;\n"); MARPAESLIF_GEN_OK;
      }
    }
    GENERICLOGGER_TRACE (genericLoggerp, "  default:\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACEF(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"Unsupported symbol callback number %%d at grammar No %d (%s)\", symboli);\n", grammarp->leveli, grammarp->descp->asciis); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "    return 0;\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (ascii2ids != NULL) {
    free(ascii2ids);
  }
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_by_grammar_ruleCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, int grammari)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_by_grammar_ruleCallbackb";
  marpaESLIF_grammar_t *grammarp   = GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammari);
  genericStack_t       *ruleStackp = grammarp->ruleStackp;
  char                 *ascii2ids  = NULL;
  char                 *ruleshows;
  char                 *actions;
  int                   rulei;
  marpaESLIF_rule_t    *rulep;
  short                 rcb;

  /* Generate all rule callbacks - please note that if a rule produces an empty string, then valuator goes to the nulling callback */
  
  GENERICLOGGER_TRACE (genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "/*\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, " * Rule callbacks of grammar No %d (%s)\n", grammarp->leveli, grammarp->descp->asciis); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, " */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "static int marpaESLIF_managedStack_ruleCallback_level%d(marpaESLIF_managedStack_t *managedStackp, marpaESLIFValue_t *marpaESLIFValuep, int rulei, int arg0i, int argni, int resulti) {\n", grammarp->leveli); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "  if (managedStackp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "    return 0; /* Should never happen -; */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "  switch (rulei) {\n", grammarp->leveli); MARPAESLIF_GEN_OK;
  for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
    if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
      /* Should never happen, but who knows */
      continue;
    }
    rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);

    GENERICLOGGER_TRACEF(genericLoggerp, "  case %d:\n", rulep->idi); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACEF(genericLoggerp, "    /* %s */\n", rulep->asciishows); MARPAESLIF_GEN_OK;
    /* If there is a rule specific action */
    actions = rulep->actions;
    if (actions == NULL) {
      actions = grammarp->defaultRuleActions;
    }
    if (actions != NULL) {
      ascii2ids = _marpaESLIF_ascii2ids(marpaESLIFp, actions);
      if (ascii2ids == NULL) {
        goto err;
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "    return %s(managedStackp->userDatavp, arg0i, argni, resulti);\n", ascii2ids);
      free(ascii2ids);
      ascii2ids = NULL;
    } else {
      if (marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammarp, rulei, &ruleshows, grammarp->leveli, NULL /* descp */)) {
        MARPAESLIF_WARNF(marpaESLIFp, "Rule No %d (%s) of grammar No %d (%s) has no action", rulep->idi, ruleshows, grammarp->leveli, grammarp->descp->asciis);
      } else {
        MARPAESLIF_WARNF(marpaESLIFp, "Rule No %d (%s) of grammar No %d (%s) has no action", rulep->idi, rulep->descp->asciis, grammarp->leveli, grammarp->descp->asciis);
      }
      GENERICLOGGER_TRACE(genericLoggerp, "    /* WARNING : this rule has no action */\n"); MARPAESLIF_GEN_OK;
      GENERICLOGGER_TRACE(genericLoggerp, "    return 0;\n"); MARPAESLIF_GEN_OK;
    }
  }
  GENERICLOGGER_TRACE (genericLoggerp, "  default:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"Unsupported rule callback number %%d at grammar No %d (%s)\", rulei);\n", grammarp->leveli, grammarp->descp->asciis); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "    return 0;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (ascii2ids != NULL) {
    free(ascii2ids);
  }
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_includeb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, char *userspaceincludes)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_includeb";
  short                 rcb;

  if (userspaceincludes == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "userspaceinclude is NULL");
    goto err;
  }
  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE (genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <stdlib.h>        /* malloc etc */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <stddef.h>        /* size_t */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <errno.h>         /* errno */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <string.h>        /* strerror */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <genericStack.h>  /* generic Stack */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <genericLogger.h> /* generic Logger */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "#include <marpaESLIF.h>\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "%s\n", userspaceincludes); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_externb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp, char *freeMethodNames, char *copyMethodNames)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_generateHelper_externb";
  short              rcb;

  if (freeMethodNames == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "freeMethodNames is NULL");
    goto err;
  }
  if (copyMethodNames == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "copyMethodNames is NULL");
    goto err;
  }
  /* Generate dependencies as per the end-user */
  GENERICLOGGER_TRACE (genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE (genericLoggerp, "/* External dependencies */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "extern void %s(int leveli, char *contexts, void *p);\n", freeMethodNames); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACEF(genericLoggerp, "extern void %s(int leveli, char *contexts, void *p);\n", copyMethodNames); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_macrosb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_generateHelper_macrosb";
  short              rcb;

  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Internal macros */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#undef  MANAGEDSTACK_ERROR\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#define MANAGEDSTACK_ERROR(managedStackp, errors) do { \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp->genericLoggerp != NULL) { \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    GENERICLOGGER_ERROR(managedStackp->genericLoggerp, errors); \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  } \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "} while (0)\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#undef  MANAGEDSTACK_ERRORF\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#define MANAGEDSTACK_ERRORF(managedStackp, fmts, ...) do { \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp->genericLoggerp != NULL) { \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    GENERICLOGGER_ERRORF(managedStackp->genericLoggerp, fmts, __VA_ARGS__); \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  } \\\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "} while (0)\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_structureb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_structureb";
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Internal structure offering stack management */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "typedef struct marpaESLIF_managedStack {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  genericLogger_t *genericLoggerp; /* Caller's eventual logger */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  void            *userDatavp;     /* Caller's opaque context */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  genericStack_t  *userStackp;     /* The stack itself */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  genericStack_t  *typeStackp;     /* Type of every item in the stack */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  genericStack_t  *contextStackp;  /* Context of every each item in the stack */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "} marpaESLIF_managedStack_t;\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_freeb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_freeb";
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Stack manager destructor */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "void marpaESLIF_managedStack_freev(marpaESLIF_managedStack_t *managedStackp) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp != NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    GENERICSTACK_FREE(managedStackp->userStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    GENERICSTACK_FREE(managedStackp->typeStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    GENERICSTACK_FREE(managedStackp->contextStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    free(managedStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_newb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_newb";
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Stack manager constructor - it is THIS structure that you will use in marpaESLIFValueOption.userDatavp */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "marpaESLIF_managedStack_t *marpaESLIF_managedStack_newp(void *userDatavp, genericLogger_t *genericLoggerp, marpaESLIFValueRuleCallback_t *ruleCallbackpp, marpaESLIFValueSymbolCallback_t *symbolCallbackpp) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_t *managedStackp = NULL;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp = (marpaESLIF_managedStack_t *) malloc(sizeof(marpaESLIF_managedStack_t));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp->genericLoggerp = genericLoggerp; /* NULL is ok */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp->userStackp     = NULL;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp->typeStackp     = NULL;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp->contextStackp  = NULL;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  GENERICSTACK_NEW(managedStackp->userStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (GENERICSTACK_ERROR(managedStackp->userStackp)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"managedStackp->userStackp initialization failure, %%s\", strerror(errno));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  GENERICSTACK_NEW(managedStackp->typeStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (GENERICSTACK_ERROR(managedStackp->typeStackp)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"managedStackp->typeStackp initialization failure, %%s\", strerror(errno));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  GENERICSTACK_NEW(managedStackp->contextStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (GENERICSTACK_ERROR(managedStackp->contextStackp)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"managedStackp->contextStackp initialization failure, %%s\", strerror(errno));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (ruleCallbackpp != NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    *ruleCallbackpp = marpaESLIF_managedStack_ruleCallbacki;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (symbolCallbackpp != NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    *symbolCallbackpp = marpaESLIF_managedStack_symbolCallbacki;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  goto done;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " err:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_freev(managedStackp);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  managedStackp = NULL;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " done:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  return managedStackp;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_ruleCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_ruleCallbackb";
  genericStack_t       *grammarStackp = marpaESLIFGrammarp->grammarStackp;
  marpaESLIF_grammar_t *grammarp;
  int                   grammari;
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Stack manager rule callback main entry point */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "static int marpaESLIF_managedStack_ruleCallbacki(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions, int rulei, int arg0i, int argni, int resulti) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_t *managedStackp = (marpaESLIF_managedStack_t *) userDatavp;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIFGrammar_t       *marpaESLIFGrammarp;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        grammari;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        leveli;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        typei;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        rcb;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (marpaESLIFGrammarp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERROR(managedStackp, \"marpaESLIFValue_grammarp returned NULL\");\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERROR(managedStackp, \"marpaESLIFValue_grammarib failure\");\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"marpaESLIFGrammar_leveli_by_grammarb failure for grammari %%d\", grammari);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  switch (leveli) {\n"); MARPAESLIF_GEN_OK;
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = GENERICSTACK_GET_PTR(grammarStackp, grammari);
    GENERICLOGGER_TRACEF(genericLoggerp, "  case %d:\n", grammarp->leveli); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACEF(genericLoggerp, "    typei = marpaESLIF_managedStack_ruleCallback_level%d(managedStackp, marpaESLIFValuep, rulei, arg0i, argni, resulti);\n", grammarp->leveli); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp,  "    break;\n"); MARPAESLIF_GEN_OK;
  }
  GENERICLOGGER_TRACE(genericLoggerp, "  default:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"Unsupported grammar level %%d\", leveli);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! typei) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  GENERICSTACK_SET_INT(managedStackp->typeStackp, typei, resulti);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (GENERICSTACK_ERROR(managedStackp->typeStackp)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"managedStackp->typeStackp set failure, %%s\", strerror(errno));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  rcb = 1;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  goto done;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " err:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  rcb = 0;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " done:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  return rcb;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_symbolCallbackb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_symbolCallbackb";
  genericStack_t       *grammarStackp = marpaESLIFGrammarp->grammarStackp;
  marpaESLIF_grammar_t *grammarp;
  int                   grammari;
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Stack manager symbol callback main entry point */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "static int marpaESLIF_managedStack_symbolCallbacki(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions, char *bytep, size_t bytel, int symboli, int resulti) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_t *managedStackp = (marpaESLIF_managedStack_t *) userDatavp;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIFGrammar_t       *marpaESLIFGrammarp;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        grammari;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        leveli;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        typei;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  int                        rcb;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (managedStackp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (marpaESLIFGrammarp == NULL) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERROR(managedStackp, \"marpaESLIFValue_grammarp returned NULL\");\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERROR(managedStackp, \"marpaESLIFValue_grammarib failure\");\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"marpaESLIFGrammar_leveli_by_grammarb failure for grammari %%d\", grammari);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  switch (leveli) {\n"); MARPAESLIF_GEN_OK;
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = GENERICSTACK_GET_PTR(grammarStackp, grammari);
    GENERICLOGGER_TRACEF(genericLoggerp, "  case %d:\n", grammarp->leveli); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACEF(genericLoggerp, "    typei = marpaESLIF_managedStack_symbolCallback_level%d(managedStackp, marpaESLIFValuep, symboli, bytep, bytel, resulti);\n", grammarp->leveli); MARPAESLIF_GEN_OK;
    GENERICLOGGER_TRACE (genericLoggerp,  "    break;\n"); MARPAESLIF_GEN_OK;
  }
  GENERICLOGGER_TRACE(genericLoggerp, "  default:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"Unsupported grammar level %%d\", leveli);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (! typei) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  GENERICSTACK_SET_INT(managedStackp->typeStackp, typei, resulti);\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  if (GENERICSTACK_ERROR(managedStackp->typeStackp)) {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    MANAGEDSTACK_ERRORF(managedStackp, \"managedStackp->typeStackp set failure, %%s\", strerror(errno));\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "    goto err;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  }\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  rcb = 1;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  goto done;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " err:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  rcb = 0;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, " done:\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  return rcb;\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "}\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_generateHelper_managedStack_enumb(marpaESLIF_t *marpaESLIFp, genericLogger_t *genericLoggerp, marpaESLIF_stringGenerator_t *stringGeneratorp)
/*****************************************************************************/
{
  static const char    *funcs      = "_marpaESLIF_generateHelper_managedStack_enumb";
  short                 rcb;

  /* Generate the API of our stack manager */
  
  GENERICLOGGER_TRACE(genericLoggerp, "\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "/* Internal enum used to type items in the stack */\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "typedef enum marpaESLIF_managedStack_type {\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_CHAR = GENERICSTACKITEMTYPE_CHAR,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_SHORT = GENERICSTACKITEMTYPE_SHORT,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_INT = GENERICSTACKITEMTYPE_INT,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_LONG = GENERICSTACKITEMTYPE_LONG,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_FLOAT = GENERICSTACKITEMTYPE_FLOAT,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_DOUBLE = GENERICSTACKITEMTYPE_DOUBLE,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_PTR = GENERICSTACKITEMTYPE_PTR,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_PTR_SHALLOW = -GENERICSTACKITEMTYPE_PTR,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_ARRAY = GENERICSTACKITEMTYPE_ARRAY,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_ARRAY_SHALLOW = -GENERICSTACKITEMTYPE_ARRAY,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#if (defined(GENERICSTACK_HAVE_LONG_LONG) && GENERICSTACK_HAVE_LONG_LONG)\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_LONG_LONG = GENERICSTACKITEMTYPE_LONG_LONG,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#endif\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#if (defined(GENERICSTACK_HAVE__BOOL) && GENERICSTACK_HAVE__BOOL)\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type__BOOL = GENERICSTACKITEMTYPE__BOOL,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#endif\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#if (defined(GENERICSTACK_HAVE__COMPLEX) && GENERICSTACK_HAVE__COMPLEX)\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_FLOAT__COMPLEX = GENERICSTACKITEMTYPE_FLOAT__COMPLEX,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_DOUBLE__COMPLEX = GENERICSTACKITEMTYPE_DOUBLE__COMPLEX,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_LONG_DOUBLE__COMPLEX = GENERICSTACKITEMTYPE_LONG_DOUBLE__COMPLEX,\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "#endif\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "  marpaESLIF_managedStack_type_UNDEF = GENERICSTACKITEMTYPE_NA\n"); MARPAESLIF_GEN_OK;
  GENERICLOGGER_TRACE(genericLoggerp, "} marpaESLIF_managedStack_type_t;\n"); MARPAESLIF_GEN_OK;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline char *_marpaESLIF_ascii2ids(marpaESLIF_t *marpaESLIFp, char *asciis)
/*****************************************************************************/
{
  /* Produces a C identifier-compatible version of ascii string */
  static const char   *funcs = "_marpaESLIF_ascii2ids";
  char                *rcs   = NULL;
  char                *p;

  if (asciis == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "asciis is NULL");
    goto err;
  }

  rcs = strdup(asciis);
  if (rcs == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }

  p = rcs;
  while (*p != '\0') {
    if ((*p == '_')                  ||
        ((*p >= 'a') && (*p <= 'z')) ||
        ((*p >= 'A') && (*p <= 'Z')) ||
        ((*p >= '0') && (*p <= '9'))) {
      goto next;
    }
    *p  = '_';
  next:
    ++p;
  }

  goto done;

 err:
  if (rcs != NULL) {
    free(rcs);
    rcs = NULL;
  }

 done:
  return rcs;
}

#ifndef MARPAESLIF_NTRACE
#define MARPAESLIF_STACK_SETTER_GENERATOR_TRACE(ESLIFTYPE) do {         \
    switch (ESLIFTYPE) {                                                \
    case MARPAESLIF_STACK_TYPE_CHAR:                                    \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %c (0x%02x) (type=CHAR,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (char) value, (unsigned char) value, contexti); \
      break;                                                            \
    case MARPAESLIF_STACK_TYPE_SHORT:                                   \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %d (type=SHORT,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (int) value, contexti); \
      break;                                                            \
    case MARPAESLIF_STACK_TYPE_INT:                                     \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %d (type=INT,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (int) value, contexti); \
      break;                                                            \
    case MARPAESLIF_STACK_TYPE_LONG:                                    \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %ld (type=LONG,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (long) value, contexti); \
      break;                                                            \
    case MARPAESLIF_STACK_TYPE_FLOAT:                                   \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %ld (type=FLOAT,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (double) value, contexti); \
      break;                                                            \
    case MARPAESLIF_STACK_TYPE_DOUBLE:                                  \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %ld (type=DOUBLE,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, (double) value, contexti); \
    default:                                                            \
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = ??? (type=???,contexti=%d)", marpaESLIFValuep->marpaESLIFp, indicei, contexti); \
      break;                                                            \
    }                                                                   \
  } while (0)
#else
#define MARPAESLIF_STACK_SETTER_GENERATOR_TRACE
#endif
/* All the stack setters have exactly the same body except for PTR and ARRAY */
#define MARPAESLIF_STACK_SETTER_GENERATOR(type, STACKTYPE, ESLIFTYPE, ...) \
  static inline short _marpaESLIFValue_stack_set_##type##b(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, __VA_ARGS__) \
  {                                                                     \
    static const char      *funcs = "_marpaESLIFValue_stack_set_" #type "b"; \
    marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp; \
    short rcb;                                                          \
                                                                        \
    marpaESLIFRecognizerp->callstackCounteri++;                         \
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");  \
                                                                        \
    if (marpaESLIFValuep == NULL) {                                     \
      errno = EINVAL;                                                   \
      return 0;                                                         \
    }                                                                   \
                                                                        \
    if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, indicei, NULL, NULL, 0)) { \
      goto err;                                                         \
    }                                                                   \
    GENERICSTACK_SET_##STACKTYPE(marpaESLIFValuep->valueStackp, value, indicei);   \
    if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {            \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp set at indice %d failure, %s", indicei, strerror(errno)); \
      goto err;                                                         \
    }                                                                   \
    GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, ESLIFTYPE, indicei); \
    if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {             \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp set at indice %d failure, %s", indicei, strerror(errno)); \
      goto err;                                                         \
    }                                                                   \
    GENERICSTACK_SET_INT(marpaESLIFValuep->contextStackp, contexti, indicei); \
    if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {          \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp set at indice %d failure, %s", indicei, strerror(errno)); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIF_STACK_SETTER_GENERATOR_TRACE(ESLIFTYPE);                 \
    rcb = 1;                                                            \
    goto done;                                                          \
                                                                        \
  err:                                                                  \
    rcb = 0;                                                            \
  done:                                                                 \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb); \
    marpaESLIFRecognizerp->callstackCounteri--;                         \
    return rcb;                                                         \
  }
  
/*****************************************************************************/
MARPAESLIF_STACK_SETTER_GENERATOR(char,   CHAR,   MARPAESLIF_STACK_TYPE_CHAR, char value)
MARPAESLIF_STACK_SETTER_GENERATOR(short,  SHORT,  MARPAESLIF_STACK_TYPE_SHORT, short value)
MARPAESLIF_STACK_SETTER_GENERATOR(int,    INT,    MARPAESLIF_STACK_TYPE_INT, int value)
MARPAESLIF_STACK_SETTER_GENERATOR(long,   LONG,   MARPAESLIF_STACK_TYPE_LONG, long value)
MARPAESLIF_STACK_SETTER_GENERATOR(float,  FLOAT,  MARPAESLIF_STACK_TYPE_FLOAT, float value)
MARPAESLIF_STACK_SETTER_GENERATOR(double, DOUBLE, MARPAESLIF_STACK_TYPE_DOUBLE, double value)
/*****************************************************************************/

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_set_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_set_undefb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, indicei, NULL /* newpp */, NULL /* shallowbp */, 0 /* forgetb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
    rcb = 0;
 done:
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
    marpaESLIFRecognizerp->callstackCounteri--;
    return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_set_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, short shallowb)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_set_ptrb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, indicei, &p, &shallowb, 0)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFValuep->valueStackp, p, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, shallowb ? MARPAESLIF_STACK_TYPE_PTR_SHALLOW : MARPAESLIF_STACK_TYPE_PTR, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  GENERICSTACK_SET_INT(marpaESLIFValuep->contextStackp, contexti, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = %p (type=%s,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, p, shallowb ? "PTR_SHALLOW" : "PTR", contexti);
  rcb = 1;
  goto done;

 err:
    rcb = 0;
 done:
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
    marpaESLIFRecognizerp->callstackCounteri--;
    return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_set_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, size_t l, short shallowb)
/*****************************************************************************/
{
  static const char              *funcs = "_marpaESLIFValue_stack_set_arrayb";
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                           rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, indicei, &p, &shallowb, 0)) {
    goto err;
  }
  GENERICSTACK_ARRAY_PTR(array) = p;
  GENERICSTACK_ARRAY_LENGTH(array) = l;
  GENERICSTACK_SET_ARRAY(marpaESLIFValuep->valueStackp, array, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, shallowb ? MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW : MARPAESLIF_STACK_TYPE_ARRAY, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  GENERICSTACK_SET_INT(marpaESLIFValuep->contextStackp, contexti, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp set at indice %d failure, %s", indicei, strerror(errno));
    goto err;
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = {%p,%ld} (type=%s,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, p, (unsigned long) l, shallowb ? "ARRAY_SHALLOW" : "ARRAY", contexti);
  rcb = 1;
  goto done;

 err:
    rcb = 0;
 done:
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
    marpaESLIFRecognizerp->callstackCounteri--;
    return rcb;
}

/* All the stack getters have exactly the same body except for PTR and ARRAY */
#define MARPAESLIF_STACK_GETTER_GENERATOR(type, STACKTYPE, ESLIFTYPE, ...) \
  static inline short _marpaESLIFValue_stack_get_##type##b(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, __VA_ARGS__) \
  {                                                                     \
    short rcb;                                                          \
                                                                        \
    if (! GENERICSTACK_IS_##STACKTYPE(marpaESLIFValuep->valueStackp, indicei)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not " #STACKTYPE " (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei)); \
      goto err;                                                         \
    }                                                                   \
    if (valuep != NULL) {                                               \
      *valuep = GENERICSTACK_GET_##STACKTYPE(marpaESLIFValuep->valueStackp, indicei); \
    }                                                                   \
    if (contextip != NULL) {                                            \
      *contextip = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei); \
    }                                                                   \
    rcb = 1;                                                            \
    goto done;                                                          \
                                                                        \
  err:                                                                  \
    rcb = 0;                                                            \
  done:                                                                 \
    return rcb;                                                         \
  }

/*****************************************************************************/
MARPAESLIF_STACK_GETTER_GENERATOR(char,   CHAR,   MARPAESLIF_STACK_TYPE_CHAR, char *valuep)
MARPAESLIF_STACK_GETTER_GENERATOR(short,  SHORT,  MARPAESLIF_STACK_TYPE_SHORT, short *valuep)
MARPAESLIF_STACK_GETTER_GENERATOR(int,    INT,    MARPAESLIF_STACK_TYPE_INT, int *valuep)
MARPAESLIF_STACK_GETTER_GENERATOR(long,   LONG,   MARPAESLIF_STACK_TYPE_LONG, long *valuep)
MARPAESLIF_STACK_GETTER_GENERATOR(float,  FLOAT,  MARPAESLIF_STACK_TYPE_FLOAT, float *valuep)
MARPAESLIF_STACK_GETTER_GENERATOR(double, DOUBLE, MARPAESLIF_STACK_TYPE_DOUBLE, double *valuep)
/*****************************************************************************/

/* All the stack inspectors have exactly the same body, except for ptr and array */
#define MARPAESLIF_STACK_IS_GENERATOR(type, ESLIFTYPE) \
  static inline short _marpaESLIFValue_stack_is_##type##b(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *flagbp) \
  {                                                                     \
    short rcb;                                                          \
                                                                        \
    if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, indicei)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei)); \
      goto err;                                                         \
    }                                                                   \
    if (flagbp != NULL) {                                               \
      *flagbp = (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == ESLIFTYPE); \
    }                                                                   \
    rcb = 1;                                                            \
    goto done;                                                          \
                                                                        \
  err:                                                                  \
    rcb = 0;                                                            \
  done:                                                                 \
    return rcb;                                                         \
  }

/*****************************************************************************/
MARPAESLIF_STACK_IS_GENERATOR(undef,  MARPAESLIF_STACK_TYPE_UNDEF)
MARPAESLIF_STACK_IS_GENERATOR(char,   MARPAESLIF_STACK_TYPE_CHAR)
MARPAESLIF_STACK_IS_GENERATOR(short,  MARPAESLIF_STACK_TYPE_SHORT)
MARPAESLIF_STACK_IS_GENERATOR(int,    MARPAESLIF_STACK_TYPE_INT)
MARPAESLIF_STACK_IS_GENERATOR(long,   MARPAESLIF_STACK_TYPE_LONG)
MARPAESLIF_STACK_IS_GENERATOR(float,  MARPAESLIF_STACK_TYPE_FLOAT)
MARPAESLIF_STACK_IS_GENERATOR(double, MARPAESLIF_STACK_TYPE_DOUBLE)
/*****************************************************************************/

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_is_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *ptrbp)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_is_ptrb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, indicei)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->typeStackp, indicei));
    goto err;
  }
  if (ptrbp != NULL) {
    *ptrbp =
      (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_PTR)
      ||
      (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_PTR_SHALLOW);
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_is_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *arraybp)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_is_arrayb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, indicei)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp at indice %d is not INT (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->typeStackp, indicei));
    goto err;
  }
  if (arraybp != NULL) {
    *arraybp =
      (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_ARRAY)
      ||
      (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW);
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_get_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_get_ptrb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! GENERICSTACK_IS_PTR(marpaESLIFValuep->valueStackp, indicei)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not PTR (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
    goto err;
  }
  if (pp != NULL) {
    *pp = GENERICSTACK_GET_PTR(marpaESLIFValuep->valueStackp, indicei);
  }
  if (contextip != NULL) {
    *contextip = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
  }
  if (shallowbp != NULL) {
    *shallowbp = (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_PTR_SHALLOW);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Got %p->[%d] = %p (contexti=%d, shallow=%d)", marpaESLIFValuep->valueStackp, indicei, GENERICSTACK_GET_PTR(marpaESLIFValuep->valueStackp, indicei), GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei), (int) ((GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_PTR_SHALLOW)));
  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_get_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp)
/*****************************************************************************/
{
  static const char              *funcs = "_marpaESLIFValue_stack_get_arrayb";
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  short                           rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! GENERICSTACK_IS_ARRAY(marpaESLIFValuep->valueStackp, indicei)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not ARRAY (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
    goto err;
  }
  array = GENERICSTACK_GET_ARRAY(marpaESLIFValuep->valueStackp, indicei);
  if (pp != NULL) {
    *pp = GENERICSTACK_ARRAY_PTR(array);
  }
  if (lp != NULL) {
    *lp = GENERICSTACK_ARRAY_LENGTH(array);
  }
  if (contextip != NULL) {
    *contextip = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
  }
  if (shallowbp != NULL) {
    *shallowbp = (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Got %p->[%d] = {%p,%ld} (contexti=%d, shallow=%d)", marpaESLIFValuep->valueStackp, indicei, GENERICSTACK_ARRAY_PTR(array), (unsigned long) GENERICSTACK_ARRAY_LENGTH(array), GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei), (int) (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW));
  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_forgetb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_forgetb(marpaESLIFValuep, indicei);
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei)
/*****************************************************************************/
{
  static const char      *funcs = "_marpaESLIFValue_stack_forgetb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, indicei, NULL, NULL, 1 /* forgetb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_i_resetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, void **newpp, short *shallowbp, short forgetb)
/*****************************************************************************/
{
  static const char                  *funcs                 = "_marpaESLIFValue_stack_i_resetb";
  marpaESLIFValueOption_t             marpaESLIFValueOption = marpaESLIFValuep->marpaESLIFValueOption;
  marpaESLIFRecognizer_t             *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t                *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t               *grammarp              = marpaESLIFGrammarp->grammarp;
  marpaESLIFValueFreeActionResolver_t freeActionResolverp   = marpaESLIFValueOption.freeActionResolverp;
  short                               origshallowb          = 1;
  void                               *origp                 = NULL;
  char                               *actions;
  marpaESLIFValueFreeCallback_t       freeCallbackp;
  short                               rcb;
  short                               popmodeb;
  short                               freeb;
  int                                 origcontexti;
  int                                 origtypei;
  size_t                              origsizel;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (indicei >= GENERICSTACK_USED(marpaESLIFValuep->typeStackp)) {
    /* reset on something that does not exist */
    rcb = 1;
    goto done;
  }

  if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, indicei)) {
    /* Type must be set */
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "marpaESLIFValuep->typeStackp at indice %d is not set", indicei);
    rcb = 1;
    goto done;
  }

  if (GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei) == MARPAESLIF_STACK_TYPE_UNDEF) {
    /* It i set, but it is undef  */
    rcb = 1;
    goto done;
  }

  if (! GENERICSTACK_IS_INT(marpaESLIFValuep->contextStackp, indicei)) {
    /* Context must be set */
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp at indice %d is not set (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->contextStackp, indicei));
    goto err;
  }

  popmodeb = ((indicei + 1) == GENERICSTACK_USED(marpaESLIFValuep->typeStackp));

  origtypei = GENERICSTACK_GET_INT(marpaESLIFValuep->typeStackp, indicei);
  switch (origtypei) {
  case MARPAESLIF_STACK_TYPE_CHAR:
    if (popmodeb) {
      GENERICSTACK_POP_CHAR(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_CHAR(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_SHORT:
    if (popmodeb) {
      GENERICSTACK_POP_SHORT(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_SHORT(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_INT:
    if (popmodeb) {
      GENERICSTACK_POP_INT(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_INT(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_LONG:
    if (popmodeb) {
      GENERICSTACK_POP_LONG(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_LONG(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_FLOAT:
    if (popmodeb) {
      GENERICSTACK_POP_FLOAT(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_FLOAT(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_DOUBLE:
    if (popmodeb) {
      GENERICSTACK_POP_DOUBLE(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_DOUBLE(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_PTR:
    origp        = GENERICSTACK_GET_PTR(marpaESLIFValuep->valueStackp, indicei);
    origsizel    = 0;
    origshallowb = 0;
  case MARPAESLIF_STACK_TYPE_PTR_SHALLOW:
    if (popmodeb) {
      GENERICSTACK_POP_PTR(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_PTR(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  case MARPAESLIF_STACK_TYPE_ARRAY:
    origp        = GENERICSTACK_ARRAYP_PTR(GENERICSTACK_GET_ARRAYP(marpaESLIFValuep->valueStackp, indicei));
    origsizel    = GENERICSTACK_ARRAYP_LENGTH(GENERICSTACK_GET_ARRAYP(marpaESLIFValuep->valueStackp, indicei));
    origshallowb = 0;
  case MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW:
    if (popmodeb) {
      GENERICSTACK_POP_ARRAY(marpaESLIFValuep->valueStackp);
      origcontexti = GENERICSTACK_POP_INT(marpaESLIFValuep->contextStackp);
      GENERICSTACK_POP_ARRAY(marpaESLIFValuep->typeStackp);
    } else {
      GENERICSTACK_SET_NA(marpaESLIFValuep->valueStackp, indicei);
      origcontexti = GENERICSTACK_GET_INT(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_NA(marpaESLIFValuep->contextStackp, indicei);
      GENERICSTACK_SET_INT(marpaESLIFValuep->typeStackp, MARPAESLIF_STACK_TYPE_UNDEF, indicei);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted %p->[%d] = NA (type=NA,context=NA)", marpaESLIFValuep->valueStackp, indicei);
    }
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp at indice %d is not supported (internal type: %d)", indicei, GENERICSTACKITEMTYPE(marpaESLIFValuep->valueStackp, indicei));
    goto err;
  }
  if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp %s failure at indice %d, %s", popmodeb ? "pop" : "set", indicei, strerror(errno));
    goto err;
  }
  if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp %s failure at indice %d, %s", popmodeb ? "pop" : "set", indicei, strerror(errno));
    goto err;
  }
  if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp %s failure at indice %d, %s", popmodeb ? "pop" : "set", indicei, strerror(errno));
    goto err;
  }

  /* Release the memory if needed */
  if (origp != NULL) {
    /* Take care of the vicious case where we *newpp is equal to p. The the shallowmode is inherited. */
    if ((newpp != NULL) && (*newpp == origp)) {
      /* No free in any case */
      freeb = 0;
      /* Overwrite user's  vision of shallow */
      if (shallowbp != NULL) {
        if (*shallowbp != origshallowb) {
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Shallow setting overwriten to original value %d because user is replacing a pointer by itself", (int) origshallowb);
        }
        *shallowbp = origshallowb;
      }
    } else {
      /* Free this memory if it is not a shallow copy */
      freeb = !origshallowb;
    }
    if (freeb && (! forgetb)) {
      /* And we must not free this memory if the caller say it is doing a shallow */
      if (marpaESLIFRecognizerp->parentRecognizerp != NULL) {
        freeCallbackp = _marpaESLIF_lexeme_freeCallbackv;
      } else {
        /* Remember the context that cannot be == 0 from outside ? If this is the case */
        /* then per def, it is the result of a ::shift operation. Then no need of a resolver */
        if (! origcontexti) {
          freeCallbackp = _marpaESLIF_rule_freeCallbackv;
        } else {
          actions = grammarp->defaultFreeActions;
          if (actions == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Grammar No %d (%s) has no free default action", grammarp->leveli, grammarp->descp->asciis);
            goto err;
          } else {
            if (freeActionResolverp == NULL) {
              MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "No free action resolver");
              goto err;
            }
            freeCallbackp = freeActionResolverp(marpaESLIFValueOption.userDatavp, marpaESLIFValuep, actions);
            if (freeCallbackp == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Grammar No %d (%s) action \"%s\" resolved to NULL", grammarp->leveli, grammarp->descp->asciis, actions);
              goto err;
            } else {
              MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Grammar No %d (%s) action \"%s\" resolved to %p", grammarp->leveli, grammarp->descp->asciis, actions, freeCallbackp);
            }
          }
        }
      }
      /* No test on freeCallbackp here because the upper code guarantees it cannot be NULL */
#ifndef MARPAESLIF_NTRACE
      switch (origtypei) {
      case MARPAESLIF_STACK_TYPE_PTR:
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing previous %p->[%d] = %p (type=PTR,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, origp, origcontexti);
        break;
      case MARPAESLIF_STACK_TYPE_PTR_SHALLOW:
        /* Should never happen ! */
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing previous %p->[%d] = %p (type=PTR_SHALLOW,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, origp, origcontexti);
        break;
      case MARPAESLIF_STACK_TYPE_ARRAY:
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing previous %p->[%d] = {%p,%ld} (type=ARRAY,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, origp, (unsigned long) origsizel, origcontexti);
        break;
      case MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW:
        /* Should never happen ! */
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing previous %p->[%d] = {%p,%ld} (type=ARRAY_SHALLOW,contexti=%d)", marpaESLIFValuep->valueStackp, indicei, origp, (unsigned long) origsizel, origcontexti);
        break;
      default:
        /* Should never happen ! */
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing previous %p->[%d] = %p (type=%d,contexti=%d,sizel=%ld)", marpaESLIFValuep->valueStackp, indicei, origp, origtypei, origcontexti, (unsigned long) origsizel);
        break;
      }
#endif
      freeCallbackp(marpaESLIFValueOption.userDatavp, origcontexti, origp, origsizel);
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;
 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
short marpaESLIFValue_context_symbolib(marpaESLIFValue_t *marpaESLIFValuep, int *symbolip)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_context_symbolib";
  
  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_context_symbolib(marpaESLIFValuep, symbolip);
}

/*****************************************************************************/
short marpaESLIFValue_context_ruleib(marpaESLIFValue_t *marpaESLIFValuep, int *ruleip)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_context_ruleib";
  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_context_ruleib(marpaESLIFValuep, ruleip);
}

/*****************************************************************************/
static short _marpaESLIF_lexeme_transferb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_lexeme_transferb";
  /* It is very important to NOT depend of userDatavp here. This action bypasses any user action. Only marpaESLIFValuep is a known value */
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                      rci;
  int                      symboli;
  char                    *newp;
  
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start [%d] ~ {%p,%ld}", resulti, bytep, (unsigned long) bytel);

  if (! _marpaESLIFValue_context_symbolib(marpaESLIFValuep, &symboli)) {
    goto err;
  }

  /* Case of nullable: p is NULL */
  if ((bytep != NULL) && (bytel > 0)) {
    newp = (char *) malloc(bytel);
    if (newp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy((void *) newp, bytep, bytel);
  } else {
    newp = NULL;
  }
  
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "{%p,%ld} dupped to {%p,%ld} ", bytep, (unsigned long) bytel, newp, (unsigned long) bytel);
  if (! _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, symboli, newp, bytel, 0 /* shallows */)) {
    goto err;
  }

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", rci);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rci;
}

/*****************************************************************************/
static short _marpaESLIF_lexeme_concatb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_lexeme_concatb";
  /* It is very important to NOT depend of userDatavp here. This action bypasses any user action. Only marpaESLIFValuep is a known value */
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  void                    *p                     = NULL;
  size_t                   l                     = 0;
  short                    rcb;
  int                      rulei;
  char                    *tmpp;
  char                    *thisp;
  size_t                   thisl;
  int                      i;
  
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start [%d] = [%d-%d]", resulti, arg0i, argni);

  if (! _marpaESLIFValue_context_ruleib(marpaESLIFValuep, &rulei)) {
    goto err;
  }

  if (nullableb) {
    if (! _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, rulei, NULL, 0, 0 /* shallowb */)) {
      goto err;
    }
  } else {
    /* IF arg0i == argni == resulti, then this is a no-op (for example: LHS ~ lexeme) */
    if ((arg0i == argni) && (arg0i == resulti)) {
      MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No-op");
    } else {
      /* Compute the total size */
      for (i = arg0i; i <= argni; i++) {
        if (!  _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, NULL, (void **) &thisp, &thisl, NULL)) {
          goto err;
        }
        if ((thisp != NULL) && (thisl > 0)) {
          l += thisl;
        }
      }
      if (l > 0) {
        /* Allocate enough space */
        p = malloc(l);
        if (p == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        /* Fill that */
        tmpp = (char *) p;
        for (i = arg0i; i <= argni; i++) {
          if (!  _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, NULL, (void **) &thisp, &thisl, NULL)) {
            goto err;
          }
          if ((thisp != NULL) && (thisl > 0)) {
            memcpy((void *) tmpp, (void *) thisp, thisl);
            tmpp += thisl;
          }
        }
      }
      if (! _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, rulei, p, l, 0 /* shallowb */)) {
        goto err;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  if (p != NULL) {
    free(p);
  }
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_lexeme_freeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_lexeme_freeCallbackv";
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) userDatavp;
  
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* We know this can be only lexemes, per def. */
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing {%p,%ld}", p, (unsigned long) sizel);
  free(p);

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
  marpaESLIFRecognizerp->callstackCounteri--;
}

/*****************************************************************************/
static void _marpaESLIF_rule_freeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_rule_freeCallbackv";
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) userDatavp;
  
  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* This is our internal free for items in the output stack that come from a ::shift */
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Freeing {%p,%ld}", p, (unsigned long) sizel);
  free(p);

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return");
  marpaESLIFRecognizerp->callstackCounteri--;
}
 
/*****************************************************************************/
static inline marpaESLIFValue_t *_marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short fakeb)
/*****************************************************************************/
{
  static const char        *funcs              = "marpaESLIFValue_newp";
  marpaESLIF_t             *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t      *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t     *grammarp           = marpaESLIFGrammarp->grammarp;
  marpaESLIFValue_t        *marpaESLIFValuep   = NULL;
  marpaWrapperValue_t      *marpaWrapperValuep = NULL;
  marpaWrapperAsf_t        *marpaWrapperAsfp   = NULL;
  marpaWrapperValueOption_t marpaWrapperValueOption;
  marpaWrapperAsfOption_t   marpaWrapperAsfOption;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Value"); */

  if (marpaESLIFValueOptionp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Value option structure must not be NULL");
    goto err;
  }

  marpaESLIFValuep = (marpaESLIFValue_t *) malloc(sizeof(marpaESLIFValue_t));
  if (marpaESLIFValuep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFValuep->marpaESLIFp                 = marpaESLIFp;
  marpaESLIFValuep->marpaESLIFRecognizerp       = marpaESLIFRecognizerp;
  marpaESLIFValuep->marpaESLIFValueOption       = *marpaESLIFValueOptionp;
  marpaESLIFValuep->marpaWrapperValuep          = NULL;
  marpaESLIFValuep->marpaWrapperAsfp            = NULL;
  marpaESLIFValuep->previousPassWasPassthroughb = 0;
  marpaESLIFValuep->previousArg0i               = 0;
  marpaESLIFValuep->previousArgni               = 0;
  marpaESLIFValuep->valueStackp                 = NULL;
  marpaESLIFValuep->typeStackp                  = NULL;
  marpaESLIFValuep->contextStackp               = NULL;
  marpaESLIFValuep->inValuationb                = 0;
  marpaESLIFValuep->symboli                     = 0;
  marpaESLIFValuep->rulei                       = 0;
  marpaESLIFValuep->lastValuec                  = '\0';
  marpaESLIFValuep->lastValueb                  = 0;
  marpaESLIFValuep->lastValuei                  = 0;
  marpaESLIFValuep->lastValuel                  = 0;
  marpaESLIFValuep->lastValuef                  = 0.;
  marpaESLIFValuep->lastValued                  = 0.;
  marpaESLIFValuep->lastValuep                  = NULL;
  marpaESLIFValuep->lastSizel                   = 0;
  marpaESLIFValuep->lastTypei                   = MARPAESLIF_STACK_TYPE_UNDEF;
  marpaESLIFValuep->lastContexti                = 0;

  if (! fakeb) {
    if (grammarp->haveRejectionb) {
      marpaWrapperAsfOption.genericLoggerp = marpaESLIFp->marpaESLIFOption.genericLoggerp;
      marpaWrapperAsfOption.highRankOnlyb  = marpaESLIFValueOptionp->highRankOnlyb;
      marpaWrapperAsfOption.orderByRankb   = marpaESLIFValueOptionp->orderByRankb;
      marpaWrapperAsfOption.ambiguousb     = marpaESLIFValueOptionp->ambiguousb;
      marpaWrapperAsfOption.maxParsesi     = marpaESLIFValueOptionp->maxParsesi;
      marpaWrapperAsfp = marpaWrapperAsf_newp(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &marpaWrapperAsfOption);
      if (marpaWrapperAsfp == NULL) {
        goto err;
      }
      marpaESLIFValuep->marpaWrapperAsfp = marpaWrapperAsfp;
    } else {
      marpaWrapperValueOption.genericLoggerp = marpaESLIFp->marpaESLIFOption.genericLoggerp;
      marpaWrapperValueOption.highRankOnlyb  = marpaESLIFValueOptionp->highRankOnlyb;
      marpaWrapperValueOption.orderByRankb   = marpaESLIFValueOptionp->orderByRankb;
      marpaWrapperValueOption.ambiguousb     = marpaESLIFValueOptionp->ambiguousb;
      marpaWrapperValueOption.nullb          = marpaESLIFValueOptionp->nullb;
      marpaWrapperValueOption.maxParsesi     = marpaESLIFValueOptionp->maxParsesi;
      marpaWrapperValuep = marpaWrapperValue_newp(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &marpaWrapperValueOption);
      if (marpaWrapperValuep == NULL) {
        goto err;
      }
      marpaESLIFValuep->marpaWrapperValuep = marpaWrapperValuep;
    }
  }
  goto done;

 err:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFValuep = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFValuep); */
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %p", marpaESLIFValuep);
  marpaESLIFRecognizerp->callstackCounteri--;
  return marpaESLIFValuep;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_newb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  short rcb;

  /* Initialize the stacks */
  GENERICSTACK_NEW(marpaESLIFValuep->valueStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->valueStackp initialization failure, %s", strerror(errno));
    goto err;
  }    
  GENERICSTACK_NEW(marpaESLIFValuep->typeStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->typeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->typeStackp initialization failure, %s", strerror(errno));
    goto err;
  }    
  GENERICSTACK_NEW(marpaESLIFValuep->contextStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->contextStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValuep->contextStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  rcb = 1;
  goto done;
 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_stack_freeb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  short rcb;
  int   valueStacki;

  if (marpaESLIFValuep != NULL) {
    /* Free the stacks */
    if (marpaESLIFValuep->valueStackp != NULL) {
      for (valueStacki = 0; valueStacki < GENERICSTACK_USED(marpaESLIFValuep->valueStackp); valueStacki++) {
        if (! _marpaESLIFValue_stack_i_resetb(marpaESLIFValuep, valueStacki, NULL, 0, 0)) {
          goto err;
        }
      }
      GENERICSTACK_FREE(marpaESLIFValuep->valueStackp);
    }
    GENERICSTACK_FREE(marpaESLIFValuep->typeStackp);
    GENERICSTACK_FREE(marpaESLIFValuep->contextStackp);
  }
  rcb = 1;
  goto done;
 err:
  /* Make sure all pointers are NULL anyway */
  if (marpaESLIFValuep != NULL) {
    marpaESLIFValuep->valueStackp = NULL;
    marpaESLIFValuep->typeStackp = NULL;
    marpaESLIFValuep->contextStackp = NULL;
  }
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, char c)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_charb(marpaESLIFValuep, indicei, contexti, c);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, indicei, contexti, b);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, int i)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_intb(marpaESLIFValuep, indicei, contexti, i);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, long l)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_longb(marpaESLIFValuep, indicei, contexti, l);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, float f)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_floatb(marpaESLIFValuep, indicei, contexti, f);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_undefb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_undefb(marpaESLIFValuep, indicei);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, double d)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_doubleb(marpaESLIFValuep, indicei, contexti, d);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, short shallowb)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, indicei, contexti, p, shallowb);
}

/*****************************************************************************/
short marpaESLIFValue_stack_set_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, size_t l, short shallowb)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_set_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  if (contexti == 0) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called with a context != 0", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, indicei, contexti, p, l, shallowb);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, char *cp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_charb(marpaESLIFValuep, indicei, contextip, cp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, short *bp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_shortb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, indicei, contextip, bp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, int *ip)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_intb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_intb(marpaESLIFValuep, indicei, contextip, ip);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, long *lp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_longb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_longb(marpaESLIFValuep, indicei, contextip, lp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, float *fp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_floatb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_floatb(marpaESLIFValuep, indicei, contextip, fp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, double *dp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_doubleb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_doubleb(marpaESLIFValuep, indicei, contextip, dp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *undefbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_undefb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_undefb(marpaESLIFValuep, indicei, undefbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *charbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_charb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_charb(marpaESLIFValuep, indicei, charbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *shortbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_shortb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_shortb(marpaESLIFValuep, indicei, shortbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *intbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_intb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_intb(marpaESLIFValuep, indicei, intbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *longbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_longb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_longb(marpaESLIFValuep, indicei, longbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *floatbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_floatb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_floatb(marpaESLIFValuep, indicei, floatbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *doublebp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_doubleb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_doubleb(marpaESLIFValuep, indicei, doublebp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *ptrbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_ptrb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_ptrb(marpaESLIFValuep, indicei, ptrbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_is_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *arraybp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_is_arrayb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, indicei, arraybp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_ptrb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, indicei, contextip, pp, shallowbp);
}

/*****************************************************************************/
short marpaESLIFValue_stack_get_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp)
/*****************************************************************************/
{
  static const char *funcs  = "marpaESLIFValue_stack_get_arrayb";

  if (marpaESLIFValuep == NULL) {
    errno = EINVAL;
    return 0;
  }

  if (! marpaESLIFValuep->inValuationb) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s must be called only in an action callback", funcs);
    return 0;
  }

  return _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, indicei, contextip, pp, lp, shallowbp);
}

/*****************************************************************************/
static inline short _marpaESLIFValue_context_symbolib(marpaESLIFValue_t *marpaESLIFValuep, int *symbolip)
/*****************************************************************************/
{
  if (symbolip != NULL) {
    *symbolip = marpaESLIFValuep->symboli;
  }

  return 1;
}

/*****************************************************************************/
short _marpaESLIFValue_context_ruleib(marpaESLIFValue_t *marpaESLIFValuep, int *ruleip)
/*****************************************************************************/
{
  if (ruleip != NULL) {
    *ruleip = marpaESLIFValuep->rulei;
  }

  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFValue_okAnySymbolCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi)
/*****************************************************************************/
/* No notion of nullable flag in our case, argi is not used */
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_okAnySymbolCallbackWrapperb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t    *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t   *grammarp              = marpaESLIFGrammarp->grammarp;
  int                     usedi;
  int                     lasti;
  int                     rulei;
  marpaESLIF_rule_t      *rulep;
  marpaESLIF_symbol_t    *symbolp;
  genericStack_t         *exceptionStackp;
  int                     exceptioni;
  int                     rhsi;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* In ESLIF, rejections are always at the symbol level: an exception is identified in the grammar by: */
  /* <exception statement> ::= lhs <op declare> <rhs primary> '-' <parenthesized rhs exception list> */
  /* where RHS are symbols (themselves can be the LHS of other rules of course) */

  /* This should never happen but who knows -; */
  if (parentRuleiStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "parentRuleiStackp is NULL");
    goto err;
  }

  /* Get the latest parent rule and look if we in its exception list */
  usedi = GENERICSTACK_USED(parentRuleiStackp);
  if (usedi > 0) {
    lasti = usedi - 1;
    if (! GENERICSTACK_IS_INT(parentRuleiStackp, lasti)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "parentRuleiStackp type at indice %d is not INT (internal type: %d)", lasti, GENERICSTACKITEMTYPE(parentRuleiStackp, lasti));
      goto err;
    }
    rulei = GENERICSTACK_GET_INT(parentRuleiStackp, lasti);
    rulep = _marpaESLIF_rule_findp(marpaESLIFp, grammarp, rulei);
    if (rulep == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such rule No %d", rulei);
      goto err;
    }

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Rule %d %s", rulep->idi, rulep->asciishows);
    exceptionStackp = rulep->exceptionStackp;
    if (exceptionStackp != NULL) {
      for (exceptioni = 0; exceptioni < GENERICSTACK_USED(exceptionStackp); exceptioni++) {
        if (! GENERICSTACK_IS_INT(exceptionStackp, exceptioni)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp type at indice %d is not INT (internal type: %d)", lasti, GENERICSTACKITEMTYPE(exceptionStackp, exceptioni));
          goto err;
        }
        rhsi = GENERICSTACK_IS_INT(exceptionStackp, exceptioni);
        if (rhsi == symboli) {
          /* Got an exception */
          symbolp = _marpaESLIF_symbol_findp(marpaESLIFp, grammarp, NULL /* asciis */, symboli);
          if (symbolp == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
            goto err;
          }
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Symbol No %d (%s) exception caught for rule No %d (%s)", symboli, symbolp->descp->asciis, rulei, rulep->descp->asciis);
          goto err;
        }
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_okSymbolCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_okSymbolCallbackWrapperb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFValue_okAnySymbolCallbackWrapperb(userDatavp, parentRuleiStackp, symboli, argi);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_okNullingCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_okNullingCallbackWrapperb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFValue_okAnySymbolCallbackWrapperb(userDatavp, parentRuleiStackp, symboli, -1);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_okRuleCallbackWrapperb(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei)
/*****************************************************************************/
{
  /* In ESLIF, rejections are always at the symbol level: an exception is identified in the grammar by: */
  /* <exception statement> ::= lhs <op declare> <rhs primary> '-' <parenthesized rhs exception list> */
  /* where RHS are symbols (themselves can be the LHS of other rules of course) */
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFValue_traverserb(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_traverserb";
  marpaESLIFValue_t      *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValue_recognizerp(marpaESLIFValuep);
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* TO DO */
  rcb = 0;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_rule_action___shiftb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIF_rule_action___shiftb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValue_recognizerp(marpaESLIFValuep);
  short                   rcb;
  short                   flagb;
  char                    c;
  short                   b;
  int                     i;
  long                    l;
  float                   f;
  double                  d;
  void                   *p;
  short                   shallowb;
  size_t                  sizel;
  int                     contexti;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (nullableb) {
    /* No choice: result is undef */
    rcb = _marpaESLIFValue_stack_set_undefb(marpaESLIFValuep, resulti);
    goto done;
  }

  /* Undef ? */
  if (! _marpaESLIFValue_stack_is_undefb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    rcb = _marpaESLIFValue_stack_set_undefb(marpaESLIFValuep, resulti);
    goto done;
  }

  /* Char ? */
  if (! _marpaESLIFValue_stack_is_charb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_charb(marpaESLIFValuep, arg0i, &contexti, &c)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_charb(marpaESLIFValuep, resulti, contexti, c);
    goto done;
  }
  
  /* short ? */
  if (! _marpaESLIFValue_stack_is_shortb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, arg0i, &contexti, &b)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, resulti, contexti, b);
    goto done;
  }
  
  /* int ? */
  if (! _marpaESLIFValue_stack_is_intb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_intb(marpaESLIFValuep, arg0i, &contexti, &i)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, contexti, i);
    goto done;
  }
  
  /* long ? */
  if (! _marpaESLIFValue_stack_is_longb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_longb(marpaESLIFValuep, arg0i, &contexti, &l)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_longb(marpaESLIFValuep, resulti, contexti, l);
    goto done;
  }
  
  /* float ? */
  if (! _marpaESLIFValue_stack_is_floatb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_floatb(marpaESLIFValuep, arg0i, &contexti, &f)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_floatb(marpaESLIFValuep, resulti, contexti, f);
    goto done;
  }
  
  /* double ? */
  if (! _marpaESLIFValue_stack_is_doubleb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_doubleb(marpaESLIFValuep, arg0i, &contexti, &d)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_doubleb(marpaESLIFValuep, resulti, contexti, d);
    goto done;
  }
  
  /* ptr ? */
  if (! _marpaESLIFValue_stack_is_ptrb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, arg0i, &contexti, &p, &shallowb)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, contexti, p, shallowb);
    goto done;
  }
  
  /* array ? */
  if (! _marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, arg0i, &flagb)) {
    goto err;
  }
  if (flagb) {
    if (! _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, &contexti, &p, &sizel, &shallowb)) {
      goto err;
    }
    rcb = _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, contexti, p, sizel, shallowb);
    goto done;
  }
  
  /* Not a known type !? */
  if (! GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, arg0i)) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Indice %d in stack is not typed (genericStack type: %s)", arg0i, _marpaESLIF_genericStack_i_types(marpaESLIFValuep->typeStackp, arg0i));
  } else {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Indice %d in stack is not supported (internal type: %d)", arg0i, GENERICSTACK_IS_INT(marpaESLIFValuep->typeStackp, arg0i));
  }
  rcb = 0;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_rule_action___undefb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIF_rule_action___undefb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValue_recognizerp(marpaESLIFValuep);
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcb = _marpaESLIFValue_stack_set_undefb(marpaESLIFValuep, resulti);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_rule_action___charconvb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, char *toEncodings)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIF_rule_action___charconvb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValue_recognizerp(marpaESLIFValuep);
  char                   *asciis                = NULL;
  size_t                  asciil;
  char                   *tmps;
  int                     i;
  size_t                  convertedl;
  char                   *converteds;
  void                   *p;
  size_t                  l;
  short                   rcb;
  short                   arrayb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start (toEncodings=%s)", toEncodings);

  if (nullableb) {
    asciis = (char *) malloc(1);
    if (asciis == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    asciis[0] = '\0';
  } else {
    /* This is meaningful only if all arguments are of type ARRAY */
    for (i = arg0i; i <= argni; i++) {
      if (! _marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, i, &arrayb)) {
        goto err;
      }
      if (! arrayb) {
        MARPAESLIF_ERRORF(marpaESLIFp, "The ::ascii rule action requires that all RHS are of ARRAY type, this is not the case of RHS No %d", i);
        goto err;
      }
      if (! _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, NULL /* contextip */, &p, &l, NULL /* shallowbp */)) {
        goto err;
      }
      if ((p == NULL) || (l <= 0)) {
        continue;
      }
      /* Look to _marpaESLIF_charconvp: it always allocate one byte more and already put '\0' in it, though not returning this additional byte in convertedl */
      converteds = _marpaESLIF_charconvp(marpaESLIFp, toEncodings, NULL /* fromEncodings - cross fingers */, (char *) p, l, &convertedl, NULL /* fromEncodingsp */, NULL /* tconvpp */);
      if (converteds == NULL) {
        goto err;
      }
      if (asciis == NULL) {
        asciil = convertedl;
        asciis = converteds;
      } else {
        tmps = (char *) realloc(asciis, asciil + convertedl + 1);
        if (asciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        asciis = tmps;
        strcat(asciis, converteds);
        free(converteds);
      }
    }
  }

  if (! _marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, 0 /* context */, asciis, 0 /* shallowb */)) {
    goto err;
  }
  rcb = 1;
  goto done;

 err:
  if (asciis != NULL) {
    free(asciis);
  }
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_rule_action___asciib(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_rule_action___charconvb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, "ASCII");
}

/*****************************************************************************/
static short _marpaESLIF_rule_action___translitb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_rule_action___charconvb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, "ASCII//TRANSLIT//IGNORE");
}

/*****************************************************************************/
static short _marpaESLIF_symbol_action___shiftb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  /* We say shallow because this is an array that leave in the lexeme stack - and the lexeme stack is absolutely not */
  /* available from outside! Therefore it is very light operation: no malloc, no free when released */
  return _marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, 0 /* contexti here the value not allowed from outside ! */, bytep, bytel, 1 /* shallowb */);
}

/*****************************************************************************/
static short _marpaESLIF_symbol_action___undefb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  return  marpaESLIFValue_stack_set_undefb(marpaESLIFValuep, resulti);
}

/*****************************************************************************/
static short _marpaESLIF_symbol_action___asciib(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  return _marpaESLIF_symbol_action___charconvb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, "ASCII");
}

/*****************************************************************************/
static short _marpaESLIF_symbol_action___translitb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  return _marpaESLIF_symbol_action___charconvb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, "ASCII//TRANSLIT//IGNORE");
}

/*****************************************************************************/
static inline short _marpaESLIF_symbol_action___charconvb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, char *toEncodings)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIF_symbol_action___charconvb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValue_recognizerp(marpaESLIFValuep);
  char                   *asciis                = NULL;
  short                   rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "start (toEncodings=%s)", toEncodings);

  asciis = _marpaESLIF_charconvp(marpaESLIFp, toEncodings, NULL /* fromEncodings */, bytep, bytel, NULL /* dstlp */, NULL /* fromEncodingsp */, NULL /* tconvpp */);
  if (asciis == NULL) {
    goto err;
  }

  if (! _marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, 0 /* context */, asciis, 0 /* shallowb */)) {
    goto err;
  }
  rcb = 1;
  goto done;

 err:
  if (asciis != NULL) {
    free(asciis);
  }
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

#include "bootstrap_actions.c"
