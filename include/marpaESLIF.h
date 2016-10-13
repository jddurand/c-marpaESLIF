#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <genericLogger.h>
#include <marpaESLIF/export.h>

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;
} marpaESLIFOption_t;

typedef struct marpaESLIF marpaESLIF_t;
typedef struct marpaESLIFGrammar marpaESLIFGrammar_t;
typedef struct marpaESLIFRecognizer marpaESLIFRecognizer_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, char *grammars);
  marpaESLIF_EXPORT short                   marpaESLIF_grammar_rule_expandb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int ruleIdi, int *lhsIdip, size_t *rhsIdlp, int *rhsIdip);
  marpaESLIF_EXPORT short                   marpaESLIF_grammar_rule_idb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, size_t *ruleIdlp, int *ruleIdip);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_rule_namep(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int ruleIdi);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_rule_showp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int ruleIdi);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_symbol_descriptionp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int symbolIdi);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_symbol_displayformp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int symbolIdi);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_symbol_dslformp(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int symbolIdi);
  marpaESLIF_EXPORT short                   marpaESLIF_grammar_symbol_idb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, size_t *symbolIdlp, int *symbolIdip);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_symbol_namep(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, int symbolIdi);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_show_rulesb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, genericLoggerLevel_t genericLoggerLeveli);
  marpaESLIF_EXPORT char                   *marpaESLIF_grammar_show_symbolsb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, genericLoggerLevel_t genericLoggerLeveli);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIF_recognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT short                  *marpaESLIF_recognizer_readb(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT void                    marpaESLIF_recognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT void                    marpaESLIF_grammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
