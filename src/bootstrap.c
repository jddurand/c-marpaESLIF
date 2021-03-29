#include "marpaESLIF/internal/bootstrap.h"

#undef  FILENAMES
#define FILENAMES "bootstrap.c" /* For logging */

/* For take terminals, avoid an unnecessary call to ASCII conversion of the content */
static const char  *_marpaESLIF_bootstrap_descEncodingInternals = "ASCII";
static const char  *_marpaESLIF_bootstrap_descInternals = "INTERNAL";
static const size_t _marpaESLIF_bootstrap_descInternall = 8; /* strlen("INTERNAL") */

/* For ord2utf */
static const int utf8_table1[] = { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};
static const int utf8_table1_size = sizeof(utf8_table1) / sizeof(int);
static const int utf8_table2[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
static const int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

/* For ::lua-> action prefix */
static const char *LUA_ACTION_PREFIX = "::lua->";

/* This file contain the definition of all bootstrap actions, i.e. the ESLIF grammar itself */
/* This is an example of how to use the API */

static inline void _marpaESLIF_bootstrap_rhs_primary_freev(marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp);
static inline void _marpaESLIF_bootstrap_rhs_alternative_freev(marpaESLIF_bootstrap_rhs_alternative_t *rhsAlternativep);
static inline void _marpaESLIF_bootstrap_symbol_and_reference_freev(marpaESLIF_bootstrap_symbol_and_reference_t *symbolAndReferencep);
static inline void _marpaESLIF_bootstrap_utf_string_freev(marpaESLIF_bootstrap_utf_string_t *stringp);
static inline void _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsAlternativeStackp);
static inline void _marpaESLIF_bootstrap_lhs_freev(marpaESLIF_bootstrap_lhs_t *lhsp);
static inline void _marpaESLIF_bootstrap_argumentStack_resetv(genericStack_t *argumentStackp);
static inline void _marpaESLIF_bootstrap_adverb_list_item_freev(marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp);
static inline void _marpaESLIF_bootstrap_adverb_list_items_freev(genericStack_t *adverbListItemStackp);
static inline void _marpaESLIF_bootstrap_alternative_freev(marpaESLIF_bootstrap_alternative_t *alternativep);
static inline void _marpaESLIF_bootstrap_alternatives_freev(genericStack_t *alternativeStackp);
static inline void _marpaESLIF_bootstrap_priorities_freev(genericStack_t *alternativesStackp);
static inline void _marpaESLIF_bootstrap_single_symbol_freev(marpaESLIF_bootstrap_single_symbol_t *singleSymbolp);
static inline void _marpaESLIF_bootstrap_symbol_freev(marpaESLIF_bootstrap_symbol_t *symbolp);
static inline void _marpaESLIF_bootstrap_parameterized_symbol_freev(marpaESLIF_bootstrap_parameterized_symbol_t *parameterizedSymbolp);
static inline void _marpaESLIF_bootstrap_parameterized_symbol_and_reference_freev(marpaESLIF_bootstrap_parameterized_symbol_and_reference_t *parameterizedSymbolAndReferencep);
static inline void _marpaESLIF_bootstrap_terminal_freev(marpaESLIF_bootstrap_terminal_t *terminalp);
static inline void _marpaESLIF_bootstrap_grammar_reference_freev(marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep);
static inline void _marpaESLIF_bootstrap_event_initialization_freev(marpaESLIF_bootstrap_event_initialization_t *eventInitializationp);
static        void _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel, short allowEmptyStringb);

static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_check_grammarp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, marpaESLIF_bootstrap_utf_string_t *stringp);

/* All the methods *_check*_ methods return a symbol return something that should be used as an LHS. */
/* *rhspp, if not NULL, will return the symbol when it should be used as an RHS */
/* Very often they are the same, EXCEPT for parameterized symbols. */
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_lhsp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, char *asciinames, short createb, short forcecreateb, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_metap(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, char *asciinames, short createb, short forcecreateb, int parameteri, short terminalb, marpaESLIF_symbol_t *lhsp, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp);
static inline short                 _marpaESLIF_bootstrap_search_terminal_by_descriptionb(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp, marpaESLIF_symbol_t **symbolpp);
static inline short                 _marpaESLIF_bootstrap_search_terminal_pseudob(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_symbol_t **symbolpp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp, short createb, short pseudob, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_quotedStringp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *quotedStringp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_builtinp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_terminal_type_t type, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_regexp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *regexp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_single_symbol_t *singleSymbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_symbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_symbol_t *symbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_parameterized_symbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_parameterized_symbol_t *symbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_terminalp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_terminal_t *terminalp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_rhsAlternativep(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_rhs_alternative_t *rhsAlternativep, short createb, marpaESLIF_symbol_t *lhsp, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp);
static inline short _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIF_t                                 *marpaESLIFp,
                                                                      char                                         *contexts,
                                                                      genericStack_t                               *adverbListItemStackp,
                                                                      marpaESLIF_action_t                         **actionpp,
                                                                      short                                        *left_associationbp,
                                                                      short                                        *right_associationbp,
                                                                      short                                        *group_associationbp,
                                                                      marpaESLIF_bootstrap_rhs_primary_t         **separatorRhsPrimarypp,
                                                                      short                                        *properbp,
                                                                      short                                        *hideseparatorbp,
                                                                      int                                          *rankip,
                                                                      short                                        *nullRanksHighbp,
                                                                      int                                         **priorityipp,
                                                                      marpaESLIF_bootstrap_pause_type_t            *pauseip,
                                                                      short                                        *latmbp,
                                                                      marpaESLIF_bootstrap_utf_string_t           **namingpp,
                                                                      marpaESLIF_action_t                         **symbolactionpp,
                                                                      marpaESLIF_bootstrap_event_initialization_t **eventInitializationpp,
                                                                      marpaESLIF_action_t                         **ifactionpp,
                                                                      marpaESLIF_action_t                         **regexactionpp,
                                                                      marpaESLIF_action_t                         **eventactionpp,
                                                                      char                                        **defaultEncodingsp,
                                                                      char                                        **fallbackEncodingsp
                                                                      );
static inline short _marpaESLIF_bootstrap_G1_action_event_declarationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, marpaESLIF_bootstrap_event_declaration_type_t type);
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_regex_to_stringb(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel);
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_characterClass_to_stringb(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel);
static inline int _marpaESLIF_bootstrap_ord2utfb(marpaESLIF_uint32_t uint32, PCRE2_UCHAR *bufferp);
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_2_and_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb);
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_4_and_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb);
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_6_and_7b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb);
static inline short _marpaESLIF_bootstrap_G1_action_terminal_pseudob(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_bootstrap_terminal_type_t type, int resulti);

static        short _marpaESLIF_bootstrap_G1_action_symbol_name_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_op_declare_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_op_declare_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_action_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_action_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_action_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_string_literalb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_string_literal_inside_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_string_literal_inside_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_string_literal_inside_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_string_literal_inside_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_symbolaction_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_symbolaction_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_symbolaction_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_left_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_right_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_group_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_separator_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_6b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_alternative_7b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_alternativeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_alternativesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_prioritiesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_priority_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static inline short _marpaESLIF_bootstrap_G1_action_priority_loosen_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_symbol_t *lhsp, genericStack_t *alternativesStackp, genericStack_t *parameterStackp);
static inline short _marpaESLIF_bootstrap_G1_action_priority_flat_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_symbol_t *lhsp, genericStack_t *alternativesStackp, char *contexts, genericStack_t *parameterStackp);
static        short _marpaESLIF_bootstrap_G1_action_single_symbol_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_single_symbol_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_6b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_grammar_reference_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_grammar_reference_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_grammar_reference_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_inaccessible_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_on_or_off_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_on_or_off_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_autorank_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_quantifier_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_quantifier_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_quantified_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_start_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_desc_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_empty_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_default_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_latm_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_latm_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_proper_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_proper_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_hideseparator_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_hideseparator_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rank_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_null_ranking_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_null_ranking_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_null_ranking_constant_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_null_ranking_constant_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_pause_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_pause_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_priority_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_event_initializer_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_event_initializer_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_event_initializationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_event_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_lexeme_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_terminal_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_discard_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_completion_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_completion_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_alternative_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_namingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_exception_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_luascript_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_ifactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_regexactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_eventactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_defaultencodingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_fallbackencodingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_lhs_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_lhs_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

/* Helpers */
#define MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, indicei, _p, _l) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_ARRAY)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not ARRAY (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p = _marpaESLIFValueResultp->u.a.p;                                \
    _l = _marpaESLIFValueResultp->u.a.sizel;                            \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_STRING(marpaESLIFValuep, indicei, _p, _l, _encodingasciis) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_STRING)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not STRING (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p              = _marpaESLIFValueResultp->u.s.p;                   \
    _l              = _marpaESLIFValueResultp->u.s.sizel;               \
    _encodingasciis = _marpaESLIFValueResultp->u.s.encodingasciis;      \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, indicei, _p) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not PTR (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p = _marpaESLIFValueResultp->u.p.p;                                \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_ROW(marpaESLIFValuep, indicei, _r) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_ROW)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not ROW (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _r = _marpaESLIFValueResultp->u.r;                                  \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, indicei, _p) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_STRING)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not STRING (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
    if (MARPAESLIF_UNLIKELY(strcmp(_marpaESLIFValueResultp->u.s.encodingasciis, "ASCII") != 0)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->u.s.encodingasciis is not \"ASCII\", got \"%s\"", _marpaESLIFValueResultp->u.s.encodingasciis); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p = (char *) _marpaESLIFValueResultp->u.s.p;                       \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, indicei, _p) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_PTR)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.type is not PTR (got %d, %s)", _marpaESLIFValueResult.type, _marpaESLIF_value_types(_marpaESLIFValueResult.type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p        = _marpaESLIFValueResult.u.p.p;                           \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GETANDFORGET_ROW(marpaESLIFValuep, indicei, _r) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_ROW)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.type is not ROW (got %d, %s)", _marpaESLIFValueResult.type, _marpaESLIF_value_types(_marpaESLIFValueResult.type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _r        = _marpaESLIFValueResult.u.r;                             \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GETANDFORGET_ASCII(marpaESLIFValuep, indicei, _p, _shallowb) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_STRING)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.type is not STRING (got %d, %s)", _marpaESLIFValueResult.type, _marpaESLIF_value_types(_marpaESLIFValueResult.type)); \
      goto err;                                                         \
    }                                                                   \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.u.s.encodingasciis == NULL)) { \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.u.s.encodingasciis is NULL"); \
      goto err;                                                         \
    }                                                                   \
    if (MARPAESLIF_UNLIKELY(strcmp(_marpaESLIFValueResult.u.s.encodingasciis, "ASCII") != 0)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.u.s.encodingasciis is not \"ASCII\", got \"%s\"", _marpaESLIFValueResult.u.s.encodingasciis); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    /* Encoding is always allocated */                                  \
    free(_marpaESLIFValueResult.u.s.encodingasciis);                    \
                                                                        \
    _p        = (char *) _marpaESLIFValueResult.u.s.p;                  \
    _shallowb = _marpaESLIFValueResult.u.s.shallowb;                    \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, indicei, _p, _l, _shallowb) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_ARRAY)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.type is not ARRAY (got %d, %s)", _marpaESLIFValueResult.type, _marpaESLIF_value_types(_marpaESLIFValueResult.type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p        = _marpaESLIFValueResult.u.a.p;                           \
    _l        = _marpaESLIFValueResult.u.a.sizel;                       \
    _shallowb = _marpaESLIFValueResult.u.a.shallowb;                    \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GETANDFORGET_STRING(marpaESLIFValuep, indicei, _p, _l, _encodingasciis, _shallowb) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_ARRAY)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResult.type is not ARRAY (got %d, %s)", _marpaESLIFValueResult.type, _marpaESLIF_value_types(_marpaESLIFValueResult.type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _p              = _marpaESLIFValueResult.u.a.p;                     \
    _l              = _marpaESLIFValueResult.u.a.sizel;                 \
    _encodingasciis = _marpaESLIFValueResult.u.a.encodingasciis;        \
    _shallowb       = _marpaESLIFValueResult.u.a.shallowb;              \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, indicei, rcb) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    rcb = (_marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_UNDEF); \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_IS_INT(marpaESLIFValuep, indicei, rcb) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    rcb = (_marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_INT); \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_CONTEXT(marpaESLIFValuep, indicei, _contextp) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _contextp = _marpaESLIFValueResultp->contextp;                      \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, indicei, _y) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_BOOL)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not BOOL (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _y = _marpaESLIFValueResultp->u.y;                                  \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_SHORT(marpaESLIFValuep, indicei, _b) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_SHORT)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not SHORT (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _b = _marpaESLIFValueResultp->u.b;                                  \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, indicei, _i) do { \
    marpaESLIFValueResult_t *_marpaESLIFValueResultp;                   \
                                                                        \
    _marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei); \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp == NULL)) {         \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    if (MARPAESLIF_UNLIKELY(_marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_INT)) { \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not INT (got %d, %s)", _marpaESLIFValueResultp->type, _marpaESLIF_value_types(_marpaESLIFValueResultp->type)); \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    _i = _marpaESLIFValueResultp->u.i;                                  \
  } while (0)

/* Stack set macros - we use the _marpaESLIFValue_stack_setb() instead of marpaESLIFValue_stack_setb() */
/* because we are inside marpaESLIF and know what we are doing -; */
/* The _marpaESLIFValue_stack_setb() is equivalent to marpaESLIFValue_stack_setb(), but with no check on the arguments */

#define MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, indicei, _contextp, _p) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_PTR; \
    _marpaESLIFValueResult.contextp           = _contextp;              \
    _marpaESLIFValueResult.representationp    = NULL;                   \
    _marpaESLIFValueResult.u.p.p              = _p;                     \
    _marpaESLIFValueResult.u.p.freeCallbackp  = _marpaESLIF_bootstrap_freeDefaultActionv; \
    _marpaESLIFValueResult.u.p.freeUserDatavp = marpaESLIFValuep;       \
    _marpaESLIFValueResult.u.p.shallowb       = 0;                      \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, indicei, _contextp, _p, _l) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY; \
    _marpaESLIFValueResult.contextp           = _contextp;              \
    _marpaESLIFValueResult.representationp    = NULL;                   \
    _marpaESLIFValueResult.u.a.p              = _p;                     \
    _marpaESLIFValueResult.u.a.freeCallbackp  = _marpaESLIF_bootstrap_freeDefaultActionv; \
    _marpaESLIFValueResult.u.a.freeUserDatavp = marpaESLIFValuep;       \
    _marpaESLIFValueResult.u.a.shallowb       = 0;                      \
    _marpaESLIFValueResult.u.a.sizel          = _l;                     \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, indicei, _contextp) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF; \
    _marpaESLIFValueResult.contextp        = _contextp;                 \
    _marpaESLIFValueResult.representationp = NULL;                      \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, indicei, _contextp, _i) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT; \
    _marpaESLIFValueResult.contextp        = _contextp;                 \
    _marpaESLIFValueResult.representationp = NULL;                      \
    _marpaESLIFValueResult.u.i             = _i;                        \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, indicei, _contextp, _y) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_BOOL; \
    _marpaESLIFValueResult.contextp        = _contextp;                 \
    _marpaESLIFValueResult.representationp = NULL;                      \
    _marpaESLIFValueResult.u.y             = _y;                        \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, indicei, _contextp, _b) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
                                                                        \
    _marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_SHORT; \
    _marpaESLIFValueResult.contextp        = _contextp;                 \
    _marpaESLIFValueResult.representationp = NULL;                      \
    _marpaESLIFValueResult.u.b             = _b;                        \
                                                                        \
    if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult))) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
  } while (0)

/* We use the \x notation in case the current compiler does not know all the escape sequences */
#define MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(dst, valchar) do {          \
    unsigned char _valchar = (unsigned char) (valchar);                 \
    switch (_valchar) {                                                 \
    case '0':                                                           \
      dst |= 0x00;                                                      \
      break;                                                            \
    case '1':                                                           \
      dst |= 0x01;                                                      \
      break;                                                            \
    case '2':                                                           \
      dst |= 0x02;                                                      \
      break;                                                            \
    case '3':                                                           \
      dst |= 0x03;                                                      \
      break;                                                            \
    case '4':                                                           \
      dst |= 0x04;                                                      \
      break;                                                            \
    case '5':                                                           \
      dst |= 0x05;                                                      \
      break;                                                            \
    case '6':                                                           \
      dst |= 0x06;                                                      \
      break;                                                            \
    case '7':                                                           \
      dst |= 0x07;                                                      \
      break;                                                            \
    case '8':                                                           \
      dst |= 0x08;                                                      \
      break;                                                            \
    case '9':                                                           \
      dst |= 0x09;                                                      \
      break;                                                            \
    case 'a':                                                           \
    case 'A':                                                           \
      dst |= 0x0A;                                                      \
      break;                                                            \
    case 'b':                                                           \
    case 'B':                                                           \
      dst |= 0x0B;                                                      \
      break;                                                            \
    case 'c':                                                           \
    case 'C':                                                           \
      dst |= 0x0C;                                                      \
      break;                                                            \
    case 'd':                                                           \
    case 'D':                                                           \
      dst |= 0x0D;                                                      \
      break;                                                            \
    case 'e':                                                           \
    case 'E':                                                           \
      dst |= 0x0E;                                                      \
      break;                                                            \
    case 'f':                                                           \
    case 'F':                                                           \
      dst |= 0x0F;                                                      \
      break;                                                            \
    default:                                                            \
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported hexadecimal character '%c' (0x%lx)", _valchar, (unsigned long) _valchar); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_rhs_primary_freev(marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp)
/*****************************************************************************/
{
  if (rhsPrimaryp != NULL) {
    switch (rhsPrimaryp->type) {
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL:
      _marpaESLIF_bootstrap_single_symbol_freev(rhsPrimaryp->u.singleSymbolp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_AND_REFERENCE:
      _marpaESLIF_bootstrap_symbol_and_reference_freev(rhsPrimaryp->u.symbolAndReferencep);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL:
      _marpaESLIF_bootstrap_parameterized_symbol_freev(rhsPrimaryp->u.parameterizedSymbolp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL_AND_REFERENCE:
      _marpaESLIF_bootstrap_parameterized_symbol_and_reference_freev(rhsPrimaryp->u.parameterizedSymbolAndReferencep);
      break;
    default:
      break;
    }
    free(rhsPrimaryp);
  }
}

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_rhs_alternative_freev(marpaESLIF_bootstrap_rhs_alternative_t *rhsAlternativep)
/*****************************************************************************/
{
  if (rhsAlternativep != NULL) {
    switch (rhsAlternativep->type) {
    case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_RHS_PRIMARY:
      _marpaESLIF_bootstrap_rhs_primary_freev(rhsAlternativep->u.rhsPrimaryp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_PRIORITIES:
      _marpaESLIF_bootstrap_priorities_freev(rhsAlternativep->u.priorities.alternativesStackp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_EXCEPTION:
      _marpaESLIF_bootstrap_rhs_primary_freev(rhsAlternativep->u.exception.rhsPrimaryp);
      _marpaESLIF_bootstrap_rhs_primary_freev(rhsAlternativep->u.exception.rhsPrimaryExceptionp);
      _marpaESLIF_bootstrap_adverb_list_items_freev(rhsAlternativep->u.exception.adverbListItemStackp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_QUANTIFIED:
      _marpaESLIF_bootstrap_rhs_primary_freev(rhsAlternativep->u.quantified.rhsPrimaryp);
      _marpaESLIF_bootstrap_adverb_list_items_freev(rhsAlternativep->u.quantified.adverbListItemStackp);
      break;
    default:
      break;
    }
    free(rhsAlternativep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_symbol_and_reference_freev(marpaESLIF_bootstrap_symbol_and_reference_t *symbolAndReferencep)
/*****************************************************************************/
{
  if (symbolAndReferencep != NULL) {
    _marpaESLIF_bootstrap_symbol_freev(symbolAndReferencep->symbolp);
    _marpaESLIF_bootstrap_grammar_reference_freev(symbolAndReferencep->grammarReferencep);
    free(symbolAndReferencep);
  }
}

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_utf_string_freev(marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  if (stringp != NULL) {
    if (stringp->bytep != NULL) {
      free(stringp->bytep);
    }
    if (stringp->modifiers != NULL) {
      free(stringp->modifiers);
    }
    free(stringp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsAlternativeStackp)
/*****************************************************************************/
{
  int i;

  if (rhsAlternativeStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(rhsAlternativeStackp); i++) {
      if (GENERICSTACK_IS_PTR(rhsAlternativeStackp, i)) {
        _marpaESLIF_bootstrap_rhs_alternative_freev((marpaESLIF_bootstrap_rhs_alternative_t *) GENERICSTACK_GET_PTR(rhsAlternativeStackp, i));
      }
    }
    GENERICSTACK_FREE(rhsAlternativeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_lhs_freev(marpaESLIF_bootstrap_lhs_t *lhsp)
/*****************************************************************************/
{
  int   i;
  char *parameters;

  if (lhsp != NULL) {
    if (lhsp->parameterStackp != NULL) {
      for (i = 0; i < GENERICSTACK_USED(lhsp->parameterStackp); i++) {
        if (GENERICSTACK_IS_PTR(lhsp->parameterStackp, i)) {
          parameters = GENERICSTACK_GET_PTR(lhsp->parameterStackp, i);
          if (parameters != NULL) {
            free(parameters);
          }
        }
      }
      GENERICSTACK_FREE(lhsp->parameterStackp);
    }
    if (lhsp->symbols != NULL) {
      free(lhsp->symbols);
    }
    free(lhsp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_argumentStack_resetv(genericStack_t *argumentStackp)
/*****************************************************************************/
{
  int i;

  if (argumentStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(argumentStackp); i++) {
      if (GENERICSTACK_IS_PTR(argumentStackp, i)) {
        _marpaESLIF_bootstrap_flatStack_freev((genericStack_t *) GENERICSTACK_GET_PTR(argumentStackp, i));
      }
    }
    GENERICSTACK_RESET(argumentStackp);
  }
}

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_adverb_list_items_freev(genericStack_t *adverbListItemStackp)
/*****************************************************************************/
{
  int i;

  if (adverbListItemStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(adverbListItemStackp); i++) {
      if (GENERICSTACK_IS_PTR(adverbListItemStackp, i)) {
        _marpaESLIF_bootstrap_adverb_list_item_freev((marpaESLIF_bootstrap_adverb_list_item_t *) GENERICSTACK_GET_PTR(adverbListItemStackp, i));
      }
    }
    GENERICSTACK_FREE(adverbListItemStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_alternative_freev(marpaESLIF_bootstrap_alternative_t *alternativep)
/*****************************************************************************/
{
  if (alternativep != NULL) {
    _marpaESLIF_bootstrap_rhs_freev(alternativep->rhsAlternativeStackp);
    _marpaESLIF_bootstrap_adverb_list_items_freev(alternativep->adverbListItemStackp);
    free(alternativep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_alternatives_freev(genericStack_t *alternativeStackp)
/*****************************************************************************/
{
  int i;

  if (alternativeStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(alternativeStackp); i++) {
      if (GENERICSTACK_IS_PTR(alternativeStackp, i)) {
        _marpaESLIF_bootstrap_alternative_freev((marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, i));
      }
    }
    GENERICSTACK_FREE(alternativeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_priorities_freev(genericStack_t *alternativesStackp)
/*****************************************************************************/
{
  int i;

  if (alternativesStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(alternativesStackp); i++) {
      if (GENERICSTACK_IS_PTR(alternativesStackp, i)) {
        _marpaESLIF_bootstrap_alternatives_freev((genericStack_t *) GENERICSTACK_GET_PTR(alternativesStackp, i));
      }
    }
    GENERICSTACK_FREE(alternativesStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_single_symbol_freev(marpaESLIF_bootstrap_single_symbol_t *singleSymbolp)
/*****************************************************************************/
{
  if (singleSymbolp != NULL) {
    switch (singleSymbolp->type) {
    case MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL:
      _marpaESLIF_bootstrap_symbol_freev(singleSymbolp->u.symbolp);
      break;
    case MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_TERMINAL:
      _marpaESLIF_bootstrap_terminal_freev(singleSymbolp->u.terminalp);
      break;
    default:
      break;
    }
    free(singleSymbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_symbol_freev(marpaESLIF_bootstrap_symbol_t *symbolp)
/*****************************************************************************/
{
  if (symbolp != NULL) {
    if (symbolp->symbols != NULL) {
      free(symbolp->symbols);
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_parameterized_symbol_freev(marpaESLIF_bootstrap_parameterized_symbol_t *parameterizedSymbolp)
/*****************************************************************************/
{
  if (parameterizedSymbolp != NULL) {
    _marpaESLIF_bootstrap_symbol_freev(parameterizedSymbolp->symbolp);
    _marpaESLIF_bootstrap_argumentStack_resetv(parameterizedSymbolp->argumentStackp);
    free(parameterizedSymbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_parameterized_symbol_and_reference_freev(marpaESLIF_bootstrap_parameterized_symbol_and_reference_t *parameterizedSymbolAndReferencep)
/*****************************************************************************/
{
  if (parameterizedSymbolAndReferencep != NULL) {
    _marpaESLIF_bootstrap_symbol_freev(parameterizedSymbolAndReferencep->symbolp);
    _marpaESLIF_bootstrap_argumentStack_resetv(parameterizedSymbolAndReferencep->argumentStackp);
    _marpaESLIF_bootstrap_grammar_reference_freev(parameterizedSymbolAndReferencep->grammarReferencep);
    free(parameterizedSymbolAndReferencep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_terminal_freev(marpaESLIF_bootstrap_terminal_t *terminalp)
/*****************************************************************************/
{
  if (terminalp != NULL) {
    switch (terminalp->type) {
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_CHARACTER_CLASS:
      _marpaESLIF_bootstrap_utf_string_freev(terminalp->u.characterClassp);
      break;
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_REGULAR_EXPRESSION:
      _marpaESLIF_bootstrap_utf_string_freev(terminalp->u.regularExpressionp);
      break;
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_QUOTED_STRING:
      _marpaESLIF_bootstrap_utf_string_freev(terminalp->u.stringp);
      break;
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOF:
      /* No op - here for clarity */
      break;
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOL:
      /* No op - here for clarity */
      break;
    case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__SOL:
      /* No op - here for clarity */
      break;
    default:
      break;
    }
    free(terminalp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_grammar_reference_freev(marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep)
/*****************************************************************************/
{
  if (grammarReferencep != NULL) {
    switch (grammarReferencep->type) {
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING:
      _marpaESLIF_bootstrap_utf_string_freev(grammarReferencep->u.quotedStringp);
      break;
    default:
      break;
    }
    free(grammarReferencep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_event_initialization_freev(marpaESLIF_bootstrap_event_initialization_t *eventInitializationp)
/*****************************************************************************/
{
  if (eventInitializationp != NULL) {
    if (eventInitializationp->eventNames != NULL) {
      free(eventInitializationp->eventNames);
    }
    free(eventInitializationp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_check_grammarp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_t                *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_grammar_t        *grammarp = NULL;
  marpaESLIF_string_t          desc;
  marpaESLIF_string_t         *descp = NULL;
  marpaWrapperGrammarOption_t  marpaWrapperGrammarOption;

  if (marpaESLIFGrammarp->grammarStackp == NULL) {
    /* Make sure that the stack of grammars exist - Take care this is a stack inside Grammar structure */
    marpaESLIFGrammarp->grammarStackp = &(marpaESLIFGrammarp->_grammarStack);
    GENERICSTACK_INIT(marpaESLIFGrammarp->grammarStackp);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
      marpaESLIFGrammarp->grammarStackp = NULL;
      goto err;
    }
  }

  if (stringp != NULL) {
    desc.bytep          = stringp->bytep;
    desc.bytel          = stringp->bytel;
    desc.encodingasciis = NULL;
    desc.asciis         = NULL;
    descp = &desc;
  }
  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, leveli, descp);
  if (grammarp == NULL) {
    /* Create it */

    marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->marpaESLIFOption.genericLoggerp;
    marpaWrapperGrammarOption.warningIsErrorb   = marpaESLIFGrammarp->warningIsErrorb;
    marpaWrapperGrammarOption.warningIsIgnoredb = marpaESLIFGrammarp->warningIsIgnoredb;
    marpaWrapperGrammarOption.autorankb         = marpaESLIFGrammarp->autorankb;

    grammarp = _marpaESLIF_grammar_newp(marpaESLIFGrammarp,
                                        &marpaWrapperGrammarOption,
                                        leveli,
                                        NULL, /* descEncodings */
                                        NULL, /* descs */
                                        0, /* descl */
                                        NULL, /* defaultSymbolActionp */
                                        NULL, /* defaultRuleActionp */
                                        NULL, /* defaultEventActionp */
                                        NULL, /* defaultRegexActionp */
                                        NULL, /* defaultEncodings */
                                        NULL /* fallbackEncodings */);
    if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
      goto err;
    }
    GENERICSTACK_SET_PTR(marpaESLIFGrammarp->grammarStackp, grammarp, leveli);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp))) {
      _marpaESLIF_grammar_freev(grammarp);
      goto err;
    }
  }

  /* Note: grammarp may be NULL here */
  goto done;

 err:
  grammarp = NULL;

 done:
  return grammarp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_lhsp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, char *asciinames, short createb, short forcecreateb, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, asciinames, createb, forcecreateb, parameterStackp != NULL ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, NULL /* lhsp */, NULL /* parameterStackp */, lhsAsRhspp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_metap(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, char *asciinames, short createb, short forcecreateb, int parameteri, short terminalb, marpaESLIF_symbol_t *lhsp, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
/* In practice, terminalb is 1 only in the context of explicit               */
/* parameterized symbols.                                                    */
/* This is protected by making sure that, in this case, it is illegal to     */
/* have both terminalb and (lhsp != NULL or parameterStackp != NULL).        */
/*                                                                           */
/* The case lhsp != NULL is reserved to generated rules that may be          */
/* parameterized, e.g.:                                                      */
/*     lhsp(a,b) ::= $&<symbolp>(a, b)  // NOTE THAT PARAMETERS ARE THE SAME */
/*     symbolp(a, b) ::= ... only the caller knows ...                       */
/*     We create symbolp taking given number of parameters, saved in *rhspp  */
/*     We return symbolp                                                     */
/* Else                                                                      */
/*     symbolp is saved in *rhspp                                            */
/*                                                                           */
/* If not NULL, *rhspp contains the RHS form of the symbol, if needed.       */
/*****************************************************************************/
{
  static const char                  *funcs                  = "_marpaESLIF_bootstrap_check_metap";
  marpaESLIF_t                       *marpaESLIFp            = marpaESLIFValuep->marpaESLIFp;
  genericStack_t                     *symbolStackp           = grammarp->symbolStackp;
  marpaESLIF_symbol_t                *symbolp                = NULL;
  short                               canFreeSymbolb         = 1;
  marpaESLIF_meta_t                  *metap                  = NULL;
  marpaESLIF_symbol_t                *symbol_i_p;
  marpaESLIF_symbol_t                *lhsAsRhsp;
  int                                 i;

  if (terminalb && ((lhsp != NULL) || (parameterStackp != NULL))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Illegal call to %s with terminalb=%d and ((lhsp != NULL) or (parameterStackp != NULL)) ", funcs, (int) terminalb);
    symbolp = NULL;
    goto done;
  }

  for (i = 0; i < GENERICSTACK_USED(symbolStackp); i++) {
    MARPAESLIF_INTERNAL_GET_SYMBOL_FROM_STACK(marpaESLIFp, symbol_i_p, symbolStackp, i);
    if (symbol_i_p->type != MARPAESLIF_SYMBOL_TYPE_META) {
      continue;
    }
    if (symbol_i_p->u.metap->terminalb != terminalb) {
      continue;
    }
    if (strcmp(symbol_i_p->u.metap->asciinames, asciinames) != 0) {
      continue;
    }

    symbolp = symbol_i_p;
    canFreeSymbolb = 0;
    break;
  }

  /* It is illegal to have a symbol name defined with different number of parameters */
  if ((symbolp != NULL) && (symbolp->u.metap->parameteri != parameteri)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "The number of parameters for symbol <%s> is fixed to %d, lookup wanted it with %d parameters", asciinames, symbolp->u.metap->parameteri, parameteri);
    /* Do as if we have not found the symbol and jump to error case */
    symbolp = NULL;
    goto done;
  }

  if (forcecreateb || (createb && (symbolp == NULL))) {
    metap = _marpaESLIF_meta_newp(marpaESLIFp, grammarp, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE, asciinames, NULL /* descEncodings */, NULL /* descs */, 0 /* descl */, parameteri, terminalb);
    if (MARPAESLIF_UNLIKELY(metap == NULL)) {
      goto err;
    }
    MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Created meta symbol <%s> with %d parameters, terminalb is %d: symbol Id is %d", funcs, asciinames, parameteri, (int) terminalb, metap->idi);

    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp, NULL /* marpaESLIFSymbolOptionp */);
    if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
      goto err;
    }
    symbolp->type              = MARPAESLIF_SYMBOL_TYPE_META;
    symbolp->u.metap           = metap;
    symbolp->idi               = metap->idi;
    symbolp->descp             = metap->descp;
    metap = NULL; /* metap is now in symbolp */

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->idi);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->symbolStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
    canFreeSymbolb = 0;
  }

  lhsAsRhsp = symbolp; /* May be NULL */
  goto done;
  
 err:
  _marpaESLIF_meta_freev(metap);
  if (canFreeSymbolb && (symbolp != NULL)) {
    /* At a given moment, the symbol will be in grammar's symbolStack, then we do not own it anymore */
    _marpaESLIF_symbol_freev(symbolp);
  }
  symbolp = NULL;

 done:
  /* symbolp and rhsp can be NULL here */
  if (lhsAsRhspp != NULL) {
    *lhsAsRhspp = lhsAsRhsp;
  }

  return symbolp;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_search_terminal_by_descriptionb(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp, marpaESLIF_symbol_t **symbolpp)
/*****************************************************************************/
{
  genericStack_t        *symbolStackp = grammarp->symbolStackp;
  marpaESLIF_symbol_t   *symbolp      = NULL;
  marpaESLIF_symbol_t   *symbol_i_p;
  int                    i;
  marpaESLIF_terminal_t *terminalp;
  short                  rcb;

  /* Create a fake terminal (it has existence only in memory) - the description is the content itself */
  terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
                                        NULL, /* grammarp: this is what make the terminal only in memory */
                                        MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
                                        (char *) _marpaESLIF_bootstrap_descEncodingInternals,
                                        (char *) _marpaESLIF_bootstrap_descInternals,
                                        _marpaESLIF_bootstrap_descInternall,
                                        terminalType,
                                        stringp->modifiers,
                                        stringp->bytep,
                                        stringp->bytel,
                                        NULL /* testFullMatchs */,
                                        NULL /* testPartialMatchs */,
                                        0 /* pseudob */);
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    goto err;
  }

  for (i = 0; i < GENERICSTACK_USED(symbolStackp); i++) {
    MARPAESLIF_INTERNAL_GET_SYMBOL_FROM_STACK(marpaESLIFp, symbol_i_p, symbolStackp, i);
    if (symbol_i_p->type != MARPAESLIF_SYMBOL_TYPE_TERMINAL) {
      continue;
    }
    /* Pattern options */
    if (symbol_i_p->u.terminalp->patterni != terminalp->patterni) {
      continue;
    }
    /* Pattern content */
    if (symbol_i_p->u.terminalp->patternl != terminalp->patternl) {
      continue;
    }
    if (memcmp(symbol_i_p->u.terminalp->patterns, terminalp->patterns, terminalp->patternl) != 0) {
      continue;
    }
    /* Got it */
    symbolp = symbol_i_p;
    break;
  }

  if (symbolpp != NULL) {
    *symbolpp = symbolp;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  _marpaESLIF_terminal_freev(terminalp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_search_terminal_pseudob(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_symbol_t **symbolpp)
/*****************************************************************************/
{
  genericStack_t        *symbolStackp = grammarp->symbolStackp;
  marpaESLIF_symbol_t   *symbolp      = NULL;
  marpaESLIF_symbol_t   *symbol_i_p;
  int                    i;
  short                  rcb;

  for (i = 0; i < GENERICSTACK_USED(symbolStackp); i++) {
    MARPAESLIF_INTERNAL_GET_SYMBOL_FROM_STACK(marpaESLIFp, symbol_i_p, symbolStackp, i);
    if (symbol_i_p->type != MARPAESLIF_SYMBOL_TYPE_TERMINAL) {
      continue;
    }
    if (! symbol_i_p->u.terminalp->pseudob) {
      continue;
    }
    if (symbol_i_p->u.terminalp->type != terminalType) {
      continue;
    }
    /* Got it */
    symbolp = symbol_i_p;
    break;
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
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp, short createb, short pseudob, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  static const char     *funcs       = "_marpaESLIF_bootstrap_check_terminal_by_typep";
  marpaESLIF_t          *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t   *symbolp     = NULL;
  marpaESLIF_terminal_t *terminalp   = NULL;

  if (pseudob) {
    if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_search_terminal_pseudob(marpaESLIFp, grammarp, terminalType, &symbolp))) {
      goto err;
    }
  } else {
    if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_search_terminal_by_descriptionb(marpaESLIFp, grammarp, terminalType, stringp, &symbolp))) {
      goto err;
    }
  }

  if (forcecreateb || (createb && (symbolp == NULL))) {
    terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
                                          grammarp,
                                          MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
                                          NULL /* descEncodings */,
                                          NULL /* descs */,
                                          0 /* descl */,
                                          terminalType,
                                          pseudob ? NULL : stringp->modifiers,
                                          pseudob ? NULL : stringp->bytep,
                                          pseudob ? 0 : stringp->bytel,
                                          NULL /* testFullMatchs */,
                                          NULL /* testPartialMatchs */,
                                          pseudob);
    if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
      goto err;
    }
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Creating terminal symbol %s in grammar level %d", terminalp->descp->asciis, grammarp->leveli);
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp, NULL /* marpaESLIFSymbolOptionp */);
    if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
      goto err;
    }
    symbolp->type        = MARPAESLIF_SYMBOL_TYPE_TERMINAL;
    symbolp->u.terminalp = terminalp;
    symbolp->idi         = terminalp->idi;
    symbolp->descp       = terminalp->descp;
    terminalp = NULL; /* terminalp is now in symbolp */
      
    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->idi);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->symbolStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  _marpaESLIF_symbol_freev(symbolp);
  symbolp = NULL;

 done:
  if (lhsAsRhspp != NULL) {
    *lhsAsRhspp = symbolp;
  }
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_quotedStringp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *quotedStringp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValuep, grammarp, MARPAESLIF_TERMINAL_TYPE_STRING, quotedStringp, createb, 0 /* pseudob */, forcecreateb, lhsAsRhspp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_builtinp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_terminal_type_t type, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t *symbolp;

  switch (type) {
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOF:
    symbolp = _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValuep, grammarp, MARPAESLIF_TERMINAL_TYPE__EOF, NULL, createb, 1 /* pseudob */, forcecreateb, lhsAsRhspp);
    if (symbolp != NULL) {
      /* Remember that the grammar has :eof somewhere */
      grammarp->marpaESLIFGrammarp->hasPseudoTerminalb = 1;
      grammarp->marpaESLIFGrammarp->hasEofPseudoTerminalb = 1;
    }
    break;

  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOL:
    symbolp = _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValuep, grammarp, MARPAESLIF_TERMINAL_TYPE__EOL, NULL, createb, 1 /* pseudob */, forcecreateb, lhsAsRhspp);
    if (symbolp != NULL) {
      /* Remember that the grammar has :eol somewhere */
      grammarp->marpaESLIFGrammarp->hasPseudoTerminalb = 1;
      grammarp->marpaESLIFGrammarp->hasEolPseudoTerminalb = 1;
    }
    break;

  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__SOL:
    symbolp = _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValuep, grammarp, MARPAESLIF_TERMINAL_TYPE__SOL, NULL, createb, 1 /* pseudob */, forcecreateb, lhsAsRhspp);
    if (symbolp != NULL) {
      /* Remember that the grammar has :sol somewhere */
      grammarp->marpaESLIFGrammarp->hasPseudoTerminalb = 1;
      grammarp->marpaESLIFGrammarp->hasSolPseudoTerminalb = 1;
    }
    break;

  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported terminal builtin type %d", (int) type);
    symbolp = NULL;
    break;
  }

  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_regexp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *regexp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFValuep, grammarp, MARPAESLIF_TERMINAL_TYPE_REGEX, regexp, createb, 0 /* pseudob */, forcecreateb, lhsAsRhspp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_single_symbol_t *singleSymbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t *symbolp     = NULL;

  switch (singleSymbolp->type) {
  case MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL:
    symbolp = _marpaESLIF_bootstrap_check_symbolp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, singleSymbolp->u.symbolp, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_TERMINAL:
    symbolp = _marpaESLIF_bootstrap_check_terminalp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, singleSymbolp->u.terminalp, createb, forcecreateb, lhsAsRhspp);
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported singleSymbolp->type = %d", (int) singleSymbolp->type);
    goto err;
  }

  goto done;
  
 err:
  symbolp = NULL;

 done:
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_symbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_symbol_t *symbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, symbolp->symbols, createb, forcecreateb, NULL /* parameterStackp */, lhsAsRhspp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_parameterized_symbolp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_parameterized_symbol_t *parameterizedSymbolp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  static const char                  *funcs          = "_marpaESLIF_bootstrap_check_parameterized_symbolp";
  marpaESLIF_t                       *marpaESLIFp    = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t                *terminalp      = NULL;
  marpaESLIF_symbol_t                *symbolp        = NULL;
  marpaESLIF_symbol_t                *subSymbolp     = NULL;
  marpaESLIF_symbol_t                *subRhsp;
  marpaESLIF_symbol_t                *rhsp;
  genericStack_t                     *flatStackp;
  int                                 i;
  int                                 j;
  int                                 parameteri;
  marpaESLIFValueResult_t            *marpaESLIFValueResultp;
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp;
  genericLogger_t                    *genericLoggerExpressionp = NULL;
  genericLogger_t                    *genericLoggerExpressionShowp = NULL;
  marpaESLIF_stringGenerator_t        marpaESLIF_stringGeneratorExpression;
  marpaESLIF_stringGenerator_t        marpaESLIF_stringGeneratorExpressionShow;
  char                               *tmps;

  marpaESLIF_stringGeneratorExpression.marpaESLIFp = marpaESLIFp;
  marpaESLIF_stringGeneratorExpression.s           = NULL;
  marpaESLIF_stringGeneratorExpression.l           = 0;
  marpaESLIF_stringGeneratorExpression.okb         = 0;
  marpaESLIF_stringGeneratorExpression.allocl      = 0;

  marpaESLIF_stringGeneratorExpressionShow.marpaESLIFp = marpaESLIFp;
  marpaESLIF_stringGeneratorExpressionShow.s           = NULL;
  marpaESLIF_stringGeneratorExpressionShow.l           = 0;
  marpaESLIF_stringGeneratorExpressionShow.okb         = 0;
  marpaESLIF_stringGeneratorExpressionShow.allocl      = 0;

  genericLoggerExpressionp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGeneratorExpression, GENERICLOGGER_LOGLEVEL_TRACE);
  if (MARPAESLIF_UNLIKELY(genericLoggerExpressionp == NULL)) {
    goto err;
  }

  genericLoggerExpressionShowp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGeneratorExpressionShow, GENERICLOGGER_LOGLEVEL_TRACE);
  if (MARPAESLIF_UNLIKELY(genericLoggerExpressionShowp == NULL)) {
    goto err;
  }

  if (parameterizedSymbolp->argumentStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "parameterizedSymbolp->argumentStackp is NULL");
    goto err;
  }

  parameteri = GENERICSTACK_USED(parameterizedSymbolp->argumentStackp);
  /* We crate it twice: as a terminal and as an lhs */
  terminalp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, parameterizedSymbolp->symbolp->symbols, createb, forcecreateb, parameteri, 1 /* terminalb */, NULL /* lhsp */, NULL /* parameterStackp */, NULL /* lhsAsRhspp */);
  if (terminalp == NULL) {
    goto err;
  }
  symbolp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, parameterizedSymbolp->symbolp->symbols, createb, forcecreateb, parameteri, 0 /* terminalb */, NULL /* lhsp */, NULL /* parameterStackp */, NULL /* lhsAsRhsp */);
  if (symbolp == NULL) {
    goto err;
  }

  GENERICLOGGER_TRACEF(genericLoggerExpressionp, "marpaESLIFRecognizer:_symbolMatcher(%d, %d", symbolp->idi, grammarp->leveli);
  if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
    goto err;
  }

  GENERICLOGGER_TRACEF(genericLoggerExpressionShowp, "&<%s>(", parameterizedSymbolp->symbolp->symbols);
  if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
    goto err;
  }

  for (i = 0; i < parameteri; i++) {
    GENERICLOGGER_TRACE(genericLoggerExpressionp, ",");
    if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
      goto err;
    }
    if (i > 0) {
      GENERICLOGGER_TRACE(genericLoggerExpressionShowp, ",");
      if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
        goto err;
      }
    }
    if (! GENERICSTACK_IS_PTR(parameterizedSymbolp->argumentStackp, i)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "parameterizedSymbolp->argumentStackp at indice %d is not PTR (got %s, value %d)", i, _marpaESLIF_genericStack_i_types(parameterizedSymbolp->argumentStackp, i), GENERICSTACKITEMTYPE(parameterizedSymbolp->argumentStackp, i));
      goto err;
    }
    flatStackp = (genericStack_t *) GENERICSTACK_GET_PTR(parameterizedSymbolp->argumentStackp, i);
    for (j = 0; j < GENERICSTACK_USED(flatStackp); j++) {
      if (! GENERICSTACK_IS_PTR(flatStackp, j)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "flatStackp at indice %d is not PTR (got %s, value %d)", j, _marpaESLIF_genericStack_i_types(flatStackp, j), GENERICSTACKITEMTYPE(flatStackp, j));
        goto err;
      }

      GENERICLOGGER_TRACE(genericLoggerExpressionp, " ");
      if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
        goto err;
      }
      if (i > 0) {
        GENERICLOGGER_TRACE(genericLoggerExpressionShowp, " ");
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
          goto err;
        }
      }

      marpaESLIFValueResultp = (marpaESLIFValueResult_t *) GENERICSTACK_GET_PTR(flatStackp, j);
      switch (marpaESLIFValueResultp->type) {
        /* Only PTR, ARRAY or STRING should be available here */
      case MARPAESLIF_VALUE_TYPE_PTR:
        /* This is another rhs primary */
        rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) marpaESLIFValueResultp->u.p.p;
        if (rhsPrimaryp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "flatStackp at indice %d give a NULL rhs primary", j);
          goto err;
        }
        subSymbolp = _marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp, createb, forcecreateb, &subRhsp);
        if (subSymbolp == NULL) {
          goto err;
        }
        GENERICLOGGER_TRACEF(genericLoggerExpressionp, "$%s", subRhsp->descp->asciis);
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
          goto err;
        }
        GENERICLOGGER_TRACEF(genericLoggerExpressionShowp, "$%s", subRhsp->descp->asciis);
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
          goto err;
        }
        break;

      case MARPAESLIF_VALUE_TYPE_ARRAY:
        tmps = (char *) malloc(marpaESLIFValueResultp->u.a.sizel + 1);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        memcpy(tmps, marpaESLIFValueResultp->u.a.p, marpaESLIFValueResultp->u.a.sizel);
        tmps[marpaESLIFValueResultp->u.a.sizel] = '\0';
        GENERICLOGGER_TRACEF(genericLoggerExpressionp, "%s", tmps);
        GENERICLOGGER_TRACEF(genericLoggerExpressionShowp, "%s", tmps);
        free(tmps);
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
          goto err;
        }
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
          goto err;
        }
        break;

      case MARPAESLIF_VALUE_TYPE_STRING:
        tmps = (char *) malloc(marpaESLIFValueResultp->u.s.sizel + 1);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        memcpy(tmps, marpaESLIFValueResultp->u.s.p, marpaESLIFValueResultp->u.s.sizel);
        tmps[marpaESLIFValueResultp->u.s.sizel] = '\0';
        GENERICLOGGER_TRACEF(genericLoggerExpressionp, "%s", tmps);
        GENERICLOGGER_TRACEF(genericLoggerExpressionShowp, "%s", tmps);
        free(tmps);
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
          goto err;
        }
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
          goto err;
        }
        break;

      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported marpaESLIFValueResultp->type %d", (int) marpaESLIFValueResultp->type);
        goto err;
      }
    }
  }

  GENERICLOGGER_TRACE(genericLoggerExpressionp, ")");
  if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpression.okb)) {
    goto err;
  }

  GENERICLOGGER_TRACE(genericLoggerExpressionShowp, ")");
  if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGeneratorExpressionShow.okb)) {
    goto err;
  }

  terminalp->luaExpressions = marpaESLIF_stringGeneratorExpression.s;
  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: terminalp->luaExpressions: %s", funcs, terminalp->luaExpressions);
  marpaESLIF_stringGeneratorExpression.s = NULL;

  terminalp->luaExpressionShows = marpaESLIF_stringGeneratorExpressionShow.s;
  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: terminalp->luaExpressionShows: %s", funcs, terminalp->luaExpressionShows);
  marpaESLIF_stringGeneratorExpressionShow.s = NULL;

  goto done;

 err:
  symbolp = NULL;

 done:
  GENERICLOGGER_FREE(genericLoggerExpressionp);
  if (marpaESLIF_stringGeneratorExpression.s != NULL) {
    free(marpaESLIF_stringGeneratorExpression.s);
  }
  GENERICLOGGER_FREE(genericLoggerExpressionShowp);
  if (marpaESLIF_stringGeneratorExpressionShow.s != NULL) {
    free(marpaESLIF_stringGeneratorExpressionShow.s);
  }
  if (lhsAsRhspp != NULL) {
    *lhsAsRhspp = terminalp;
  }
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_terminalp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_terminal_t *terminalp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t *symbolp     = NULL;

  switch (terminalp->type) {
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_CHARACTER_CLASS:
    symbolp = _marpaESLIF_bootstrap_check_regexp(marpaESLIFValuep, grammarp, terminalp->u.characterClassp, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_REGULAR_EXPRESSION:
    symbolp = _marpaESLIF_bootstrap_check_regexp(marpaESLIFValuep, grammarp, terminalp->u.regularExpressionp, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_QUOTED_STRING:
    symbolp = _marpaESLIF_bootstrap_check_quotedStringp(marpaESLIFValuep, grammarp, terminalp->u.stringp, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOF:
    symbolp = _marpaESLIF_bootstrap_check_builtinp(marpaESLIFValuep, grammarp, terminalp->type, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOL:
    symbolp = _marpaESLIF_bootstrap_check_builtinp(marpaESLIFValuep, grammarp, terminalp->type, createb, forcecreateb, lhsAsRhspp);
    break;
  case MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__SOL:
    symbolp = _marpaESLIF_bootstrap_check_builtinp(marpaESLIFValuep, grammarp, terminalp->type, createb, forcecreateb, lhsAsRhspp);
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported terminalp->type = %d", (int) terminalp->type);
    goto err;
  }

  goto done;
  
 err:
  symbolp = NULL;

 done:
  return symbolp;
}

static void jdd() {
  fprintf(stderr, "JDD\n");
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_rhsAlternativep(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_rhs_alternative_t *rhsAlternativep, short createb, marpaESLIF_symbol_t *lhsp, genericStack_t *parameterStackp, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  static const char                  *funcs        = "_marpaESLIF_bootstrap_check_rhsAlternativep";
  marpaESLIF_t                       *marpaESLIFp  = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t                *symbolp      = NULL;
  marpaESLIF_symbol_t                *lhsAsRhsp;
  char                                tmps[1024];
  marpaESLIF_symbol_t                *rhsp;
  marpaESLIF_symbol_t                *rhsExceptionp;
  marpaESLIF_action_t                *actionp;
  int                                 ranki;
  short                               nullRanksHighb;
  marpaESLIF_bootstrap_utf_string_t  *namingp;
  marpaESLIF_bootstrap_rhs_primary_t *separatorRhsPrimaryp;
  short                               properb;
  short                               hideseparatorb;
  marpaESLIF_rule_t                  *rulep = NULL;
  marpaESLIF_symbol_t                *separatorp;

  /* This can recurse, we want to avoid that */
  if (rhsAlternativep->symbolShallowp != NULL) {
    symbolp = rhsAlternativep->symbolShallowp;
    goto done;
  }
  
  switch (rhsAlternativep->type) {
  case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_RHS_PRIMARY:
    symbolp = _marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep->u.rhsPrimaryp, createb, 0 /* forcecreateb */, &lhsAsRhsp);
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_PRIORITIES:
    /* It is almost exactly like:
       <priority rule>                ::= <lhs> <op declare> <priorities>
       except that <lhs> here is unknown and we create it on the fly.
       <op declare> implicitly correspond to current grammar.
    */
    sprintf(tmps, "Internal[%d]", marpaESLIFGrammarp->internalRuleCounti++);
    jdd();
    symbolp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, tmps, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &lhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
      goto err;
    }
    MARPAESLIF_NOTICEF(marpaESLIFp, "%s: LHS %s RHS %s", funcs, _marpaESLIF_symbol_shows(symbolp), _marpaESLIF_symbol_shows(lhsAsRhsp));
    if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_G1_action_priority_loosen_ruleb(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, symbolp, rhsAlternativep->u.priorities.alternativesStackp, parameterStackp))) {
      goto err;
    }
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_EXCEPTION:
    /* It is almost exactly like:
       <exception statement> ::= lhs <op declare> <rhs primary> '-' <rhs primary> <adverb list>
       except that <lhs> here is unknown and we create it on the fly.
       <op declare> implicitly correspond to current grammar.
    */
    sprintf(tmps, "Internal[%d]", marpaESLIFGrammarp->internalRuleCounti++);
    symbolp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, tmps, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &lhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
      goto err;
    }
    /* Check the rhs primary */
    if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep->u.exception.rhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, &rhsp) == NULL)) {
      goto err;
    }
    /* Check the rhs primary exception */
    if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep->u.exception.rhsPrimaryExceptionp, 1 /* createb */, 0 /* forcecreateb */, &rhsExceptionp) == NULL)) {
      goto err;
    }
    /* Check the adverb list */
    if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                                "exception rule",
                                                                                rhsAlternativep->u.exception.adverbListItemStackp,
                                                                                &actionp,
                                                                                NULL, /* left_associationbp */
                                                                                NULL, /* right_associationbp */
                                                                                NULL, /* group_associationbp */
                                                                                NULL, /* separatorRhsPrimaryp */
                                                                                NULL, /* properbp */
                                                                                NULL, /* hideseparatorbp */
                                                                                &ranki,
                                                                                &nullRanksHighb,
                                                                                NULL, /* priorityipp */
                                                                                NULL, /* pauseip */
                                                                                NULL, /* latmbp */
                                                                                &namingp,
                                                                                NULL, /* symbolactionpp */
                                                                                NULL, /* eventInitializationpp */
                                                                                NULL, /* ifactionpp */
                                                                                NULL, /* regexactionpp */
                                                                                NULL, /* eventactionpp */
                                                                                NULL, /* defaultEncodingsp */
                                                                                NULL /* fallbackEncodings */
                                                                                ))) {
      goto err;
    }
    /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Creating exception rule %s ::= %s - %s", _marpaESLIF_symbol_shows(symbolp), _marpaESLIF_symbol_shows(rhsp), _marpaESLIF_symbol_shows(rhsExceptionp));
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                  grammarp,
                                  (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                  (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                  (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                  symbolp->idi,
                                  1, /* nrhsl */
                                  &(rhsp->idi), /* rhsip */
                                  rhsExceptionp->idi,
                                  ranki,
                                  0, /*nullRanksHighb */
                                  0, /* sequenceb */
                                  0, /* minimumi */
                                  -1, /* separatori */
                                  0, /* properb */
                                  actionp,
                                  0, /* passthroughb */
                                  0 /* hideseparatorb */,
                                  NULL, /* skipbp */
                                  parameterStackp);
    if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
      goto err;
    }
    GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
      goto err;
    }
    rulep = NULL; /* rulep is in grammarp->ruleStackp */
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_QUANTIFIED:
    /* It is almost exactly like:
       <quantified rule> ::= <lhs> <op declare> <rhs primary> <quantifier> <adverb list>
       except that <lhs> here is unknown and we create it on the fly.
       <op declare> implicitly correspond to current grammar.
    */
    sprintf(tmps, "Internal[%d]", marpaESLIFGrammarp->internalRuleCounti++);
    symbolp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, tmps, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &lhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
      goto err;
    }
    /* Check the rhs primary */
    if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep->u.quantified.rhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, &rhsp) == NULL)) {
      goto err;
    }
    /* Check the adverb list */
    if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                                "quantified rule",
                                                                                rhsAlternativep->u.quantified.adverbListItemStackp,
                                                                                &actionp,
                                                                                NULL, /* left_associationbp */
                                                                                NULL, /* right_associationbp */
                                                                                NULL, /* group_associationbp */
                                                                                &separatorRhsPrimaryp,
                                                                                &properb,
                                                                                &hideseparatorb,
                                                                                &ranki,
                                                                                &nullRanksHighb,
                                                                                NULL, /* priorityipp */
                                                                                NULL, /* pauseip */
                                                                                NULL, /* latmbp */
                                                                                &namingp,
                                                                                NULL, /* symbolactionsp */
                                                                                NULL, /* eventInitializationpp */
                                                                                NULL, /* ifactionpp */
                                                                                NULL, /* regexactionpp */
                                                                                NULL, /* eventactionpp */
                                                                                NULL, /* defaultEncodingsp */
                                                                                NULL /* fallbackEncodings */
                                                                                ))) {
      goto err;
    }
    if (separatorRhsPrimaryp != NULL) {
      /* Check the separator */
      if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, separatorRhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, &separatorp) == NULL)) {
        goto err;
      }
    } else {
      separatorp = NULL;
    }
    /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
/* #ifndef MARPAESLIF_NTRACE */
  if (separatorp != NULL) {
    MARPAESLIF_NOTICEF(marpaESLIFp, funcs, "%s: Creating rule %s ::= %s%s ranki=>%d separator=>%s proper=>%d hide-separator=>%d null-ranking=>%s at grammar level %d", funcs, _marpaESLIF_symbol_shows(symbolp), _marpaESLIF_symbol_shows(rhsp), rhsAlternativep->u.quantified.minimumi ? "+" : "*", ranki, _marpaESLIF_symbol_shows(separatorp), (int) properb, (int) hideseparatorb, nullRanksHighb ? "high" : "low", grammarp->leveli);
  } else {
    MARPAESLIF_NOTICEF(marpaESLIFp, funcs, "%s: Creating rule %s ::= %s%s ranki=>%d null-ranking=>%s at grammar level %d", funcs, _marpaESLIF_symbol_shows(symbolp), _marpaESLIF_symbol_shows(rhsp), rhsAlternativep->u.quantified.minimumi ? "+" : "*", ranki, nullRanksHighb ? "high" : "low", grammarp->leveli);
  }
/* #endif */
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                  grammarp,
                                  (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                  (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                  (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                  symbolp->idi,
                                  1, /* nrhsl */
                                  &(rhsp->idi), /* rhsip */
                                  -1, /* exceptioni */
                                  ranki,
                                  nullRanksHighb,
                                  1, /* sequenceb */
                                  rhsAlternativep->u.quantified.minimumi,
                                  (separatorp != NULL) ? separatorp->idi : -1, /* separatori */
                                  properb,
                                  actionp,
                                  0, /* passthroughb */
                                  hideseparatorb,
                                  NULL, /* skipbp */
                                  parameterStackp);
    if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
      goto err;
    }
    GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
      goto err;
    }
    rulep = NULL; /* rulep is in grammarp->ruleStackp */
    break;

  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported RHS alternative type (%d)", (int) rhsAlternativep->type);
    goto err;
  }

  rhsAlternativep->symbolShallowp = symbolp;
  goto done;
  
 err:
  symbolp = NULL;

 done:
  _marpaESLIF_rule_freev(rulep);
  if (lhsAsRhspp != NULL) {
    *lhsAsRhspp = lhsAsRhsp;
  }
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp, short createb, short forcecreateb, marpaESLIF_symbol_t **lhsAsRhspp)
/*****************************************************************************/
{
  marpaESLIF_t                                *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_symbol_t                         *symbolp     = NULL;
  marpaESLIF_symbol_t                         *lhsAsRhsp;
  marpaESLIF_grammar_t                        *referencedGrammarp;
  marpaESLIF_symbol_t                         *referencedSymbolp = NULL;
  marpaESLIF_symbol_t                         *referencedSymbolAsRhsp;
  marpaESLIF_bootstrap_single_symbol_t         singleSymbol; /* Fake single symbol in case of a "referenced-in-any-grammar" symbol */
  marpaESLIF_bootstrap_parameterized_symbol_t  parameterizedSymbol; /* Fake parameterized symbol in case of a "referenced-in-any-grammar" symbol */
  char                                         tmps[1024];
  char                                        *referencedSymbols = NULL;
  genericStack_t                              *alternativesStackp = NULL;
  marpaESLIF_action_t                         *actionp = NULL;
  int                                          ranki = 0;
  marpaESLIF_rule_t                           *rulep  = NULL;
  short                                        nullRanksHighb = 0;
  short                                        properb = 0;
  short                                        hideseparatorb = 0;
  int                                          parameteri;

  /* This can recurse, we want to avoid that */
  if (rhsPrimaryp->symbolShallowp != NULL) {
    symbolp = rhsPrimaryp->symbolShallowp;
    goto done;
  }
  
  switch (rhsPrimaryp->type) {
  case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL:
    symbolp = _marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp->u.singleSymbolp, createb, forcecreateb, &lhsAsRhsp);
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_AND_REFERENCE:
    /* We want to check if referenced grammar is current grammar */
    switch (rhsPrimaryp->u.symbolAndReferencep->grammarReferencep->type) {
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, -1, rhsPrimaryp->u.symbolAndReferencep->grammarReferencep->u.quotedStringp);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp->leveli + rhsPrimaryp->u.symbolAndReferencep->grammarReferencep->u.signedIntegeri, NULL);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_UNSIGNED_INTEGER:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, (int) rhsPrimaryp->u.symbolAndReferencep->grammarReferencep->u.unsignedIntegeri, NULL);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported grammar reference type %d", (int) rhsPrimaryp->u.symbolAndReferencep->grammarReferencep->type);
      goto err;
    }
    singleSymbol.type = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL;
    singleSymbol.u.symbolp = rhsPrimaryp->u.symbolAndReferencep->symbolp;
    referencedSymbolp = _marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFValuep, marpaESLIFGrammarp, referencedGrammarp, &singleSymbol, 1 /* createb */, forcecreateb, &referencedSymbolAsRhsp);
    if (MARPAESLIF_UNLIKELY(referencedSymbolp == NULL)) {
      goto err;
    }
    if (referencedGrammarp == grammarp) {
      symbolp = referencedSymbolp;
      lhsAsRhsp = referencedSymbolAsRhsp;
    } else {
      /* symbol must exist in the current grammar in the form symbol@delta  */
      sprintf(tmps, "%+d", referencedGrammarp->leveli - grammarp->leveli);
      referencedSymbols = (char *) malloc(strlen(rhsPrimaryp->u.symbolAndReferencep->symbolp->symbols) + 1 /* @ */ + strlen(tmps) + 1 /* NUL */);
      if (MARPAESLIF_UNLIKELY(referencedSymbols == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      strcpy(referencedSymbols, rhsPrimaryp->u.symbolAndReferencep->symbolp->symbols);
      strcat(referencedSymbols, "@");
      strcat(referencedSymbols, tmps);
      
      singleSymbol.type = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_NA;
      singleSymbol.u.symbolp = (marpaESLIF_bootstrap_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_t));
      if (MARPAESLIF_UNLIKELY(singleSymbol.u.symbolp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      singleSymbol.type = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL;
      singleSymbol.u.symbolp->symbols = referencedSymbols;
      referencedSymbols = NULL; /* referencedSymbols is in singleSymbol.u.symbolp->symbols */
      symbolp = _marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, &singleSymbol, 1 /* createb */, forcecreateb, &lhsAsRhsp);
      _marpaESLIF_bootstrap_symbol_freev(singleSymbol.u.symbolp);
      if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
        goto err;
      }
      /* We overwrite reference grammar information */
      symbolp->lookupLevelDeltai = referencedGrammarp->leveli - grammarp->leveli;
      /* By definition looked up reference symbol is a meta symbol */
      symbolp->lookupMetas       = referencedSymbolp->u.metap->asciinames;
    }
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL:
    symbolp = _marpaESLIF_bootstrap_check_parameterized_symbolp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp->u.parameterizedSymbolp, createb, forcecreateb, &lhsAsRhsp);
    break;
  case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL_AND_REFERENCE:
    /* We want to check if referenced grammar is current grammar */
    switch (rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep->type) {
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, -1, rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep->u.quotedStringp);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp->leveli + rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep->u.signedIntegeri, NULL);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_UNSIGNED_INTEGER:
      referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, (int) rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep->u.unsignedIntegeri, NULL);
      if (MARPAESLIF_UNLIKELY(referencedGrammarp == NULL)) {
        goto err;
      }
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported grammar reference type %d", (int) rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep->type);
      goto err;
    }

    parameterizedSymbol.symbolp        = rhsPrimaryp->u.parameterizedSymbolAndReferencep->symbolp;
    parameterizedSymbol.argumentStackp = rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp;

    referencedSymbolp = _marpaESLIF_bootstrap_check_parameterized_symbolp(marpaESLIFValuep, marpaESLIFGrammarp, referencedGrammarp, &parameterizedSymbol, 1 /* createb */, forcecreateb, &referencedSymbolAsRhsp);
    if (MARPAESLIF_UNLIKELY(referencedSymbolp == NULL)) {
      goto err;
    }
    if (referencedGrammarp == grammarp) {
      symbolp = referencedSymbolp;
      lhsAsRhsp = referencedSymbolAsRhsp;
    } else {
      /* symbol must exist in the current grammar in the form symbol@delta  */
      sprintf(tmps, "%+d", referencedGrammarp->leveli - grammarp->leveli);
      referencedSymbols = (char *) malloc(strlen(parameterizedSymbol.symbolp->symbols)
                                          + 1 /* @ */
                                          + strlen(tmps)
                                          + 1 /* NUL */);
      if (MARPAESLIF_UNLIKELY(referencedSymbols == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      strcpy(referencedSymbols, parameterizedSymbol.symbolp->symbols);
      strcat(referencedSymbols, "@");
      strcat(referencedSymbols, tmps);
      
      parameterizedSymbol.symbolp = (marpaESLIF_bootstrap_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_t));
      if (MARPAESLIF_UNLIKELY(parameterizedSymbol.symbolp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      parameterizedSymbol.symbolp->symbols = referencedSymbols;
      referencedSymbols = NULL; /* referencedSymbols is in parameterizedSymbol.symbolp->symbols */
      symbolp = _marpaESLIF_bootstrap_check_parameterized_symbolp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, &parameterizedSymbol, 1 /* createb */, forcecreateb, &lhsAsRhsp);
      _marpaESLIF_bootstrap_symbol_freev(parameterizedSymbol.symbolp);
      if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
        goto err;
      }
      /* We overwrite reference grammar information */
      symbolp->lookupLevelDeltai = referencedGrammarp->leveli - grammarp->leveli;
      /* By definition looked up reference symbol is a meta symbol */
      symbolp->lookupMetas       = referencedSymbolp->u.metap->asciinames;
    }
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported RHS primary type %d", (int) rhsPrimaryp->type);
    goto err;
  }

  rhsPrimaryp->symbolShallowp = symbolp;
  goto done;
  
 err:
  symbolp = NULL;

 done:
  if (referencedSymbols != NULL) {
    free(referencedSymbols);
  }
  if (lhsAsRhspp != NULL) {
    *lhsAsRhspp = lhsAsRhsp;
  }
  return symbolp;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIF_t                                 *marpaESLIFp,
                                                                      char                                         *contexts,
                                                                      genericStack_t                               *adverbListItemStackp,
                                                                      marpaESLIF_action_t                        **actionpp,
                                                                      short                                        *left_associationbp,
                                                                      short                                        *right_associationbp,
                                                                      short                                        *group_associationbp,
                                                                      marpaESLIF_bootstrap_rhs_primary_t         **separatorRhsPrimarypp,
                                                                      short                                        *properbp,
                                                                      short                                        *hideseparatorbp,
                                                                      int                                          *rankip,
                                                                      short                                        *nullRanksHighbp,
                                                                      int                                         **priorityipp,
                                                                      marpaESLIF_bootstrap_pause_type_t            *pauseip,
                                                                      short                                        *latmbp,
                                                                      marpaESLIF_bootstrap_utf_string_t           **namingpp,
                                                                      marpaESLIF_action_t                         **symbolactionpp,
                                                                      marpaESLIF_bootstrap_event_initialization_t **eventInitializationpp,
                                                                      marpaESLIF_action_t                         **ifactionpp,
                                                                      marpaESLIF_action_t                         **regexactionpp,
                                                                      marpaESLIF_action_t                         **eventactionpp,
                                                                      char                                        **defaultEncodingsp,
                                                                      char                                        **fallbackEncodingsp
                                                                      )
/*****************************************************************************/
{
  int                                      adverbListItemi;
  marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp;
  short                                    rcb;

  /* Initialisations */
  if (actionpp != NULL) {
    *actionpp = NULL;
  }
  if (left_associationbp != NULL) {
    *left_associationbp = 0;
  }
  if (right_associationbp != NULL) {
    *right_associationbp = 0;
  }
  if (group_associationbp != NULL) {
    *group_associationbp = 0;
  }
  if (separatorRhsPrimarypp != NULL) {
    *separatorRhsPrimarypp = NULL;
  }
  if (properbp != NULL) {
    *properbp = 0;
  }
  if (hideseparatorbp != NULL) {
    *hideseparatorbp = 0;
  }
  if (rankip != NULL) {
    *rankip = 0;
  }
  if (nullRanksHighbp != NULL) {
    *nullRanksHighbp = 0;
  }
  if (priorityipp != NULL) {
    *priorityipp = NULL;
  }
  if (pauseip != NULL) {
    *pauseip = MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_NA;
  }
  if (latmbp != NULL) {
    *latmbp = 1; /* Default is TRUE! */
  }
  if (namingpp != NULL) {
    *namingpp = NULL;
  }
  if (symbolactionpp != NULL) {
    *symbolactionpp = NULL;
  }
  if (eventInitializationpp != NULL) {
    *eventInitializationpp = NULL;
  }
  if (ifactionpp != NULL) {
    *ifactionpp = NULL;
  }
  if (regexactionpp != NULL) {
    *regexactionpp = NULL;
  }
  if (eventactionpp != NULL) {
    *eventactionpp = NULL;
  }
  if (defaultEncodingsp != NULL) {
    *defaultEncodingsp = NULL;
  }
  if (fallbackEncodingsp != NULL) {
    *fallbackEncodingsp = NULL;
  }

  if (adverbListItemStackp != NULL) {
    for (adverbListItemi = 0; adverbListItemi < GENERICSTACK_USED(adverbListItemStackp); adverbListItemi++) {
#ifndef MARPAESLIF_NTRACE
      /* Should never happen */
      if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(adverbListItemStackp, adverbListItemi))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp at indice %d is not PTR (got %s, value %d)", adverbListItemi, _marpaESLIF_genericStack_i_types(adverbListItemStackp, adverbListItemi), GENERICSTACKITEMTYPE(adverbListItemStackp, adverbListItemi));
        goto err;
      }
#endif
      adverbListItemp = (marpaESLIF_bootstrap_adverb_list_item_t *) GENERICSTACK_GET_PTR(adverbListItemStackp, adverbListItemi);
      switch (adverbListItemp->type) {
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION:
        if (MARPAESLIF_UNLIKELY(actionpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "action adverb is not allowed in %s context", contexts);
          goto err;
        }
        *actionpp = adverbListItemp->u.actionp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION:
        if (MARPAESLIF_UNLIKELY(left_associationbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "left adverb is not allowed in %s context", contexts);
          goto err;
        }
        *left_associationbp = adverbListItemp->u.left_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION:
        if (MARPAESLIF_UNLIKELY(right_associationbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "right adverb is not allowed in %s context", contexts);
          goto err;
        }
        *right_associationbp = adverbListItemp->u.right_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION:
        if (MARPAESLIF_UNLIKELY(group_associationbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "group adverb is not allowed in %s context", contexts);
          goto err;
        }
        *group_associationbp = adverbListItemp->u.group_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR:
        if (MARPAESLIF_UNLIKELY(separatorRhsPrimarypp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "separator adverb is not allowed in %s context", contexts);
          goto err;
        }
        *separatorRhsPrimarypp = adverbListItemp->u.separatorRhsPrimaryp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER:
        if (MARPAESLIF_UNLIKELY(properbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "proper adverb is not allowed in %s context", contexts);
          goto err;
        }
        *properbp = adverbListItemp->u.properb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_HIDESEPARATOR:
        if (MARPAESLIF_UNLIKELY(hideseparatorbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "hide-separator adverb is not allowed in %s context", contexts);
          goto err;
        }
        *hideseparatorbp = adverbListItemp->u.hideseparatorb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK:
        if (MARPAESLIF_UNLIKELY(rankip == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "rank adverb is not allowed in %s context", contexts);
          goto err;
        }
        *rankip = adverbListItemp->u.ranki;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING:
        if (MARPAESLIF_UNLIKELY(nullRanksHighbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "null-ranking adverb is not allowed in %s context", contexts);
          goto err;
        }
        *nullRanksHighbp = adverbListItemp->u.nullRanksHighb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY:
        if (MARPAESLIF_UNLIKELY(priorityipp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "priority adverb is not allowed in %s context", contexts);
          goto err;
        }
        *priorityipp = &(adverbListItemp->u.priorityi);
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE:
        if (MARPAESLIF_UNLIKELY(pauseip == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "pause adverb is not allowed in %s context", contexts);
          goto err;
        }
        *pauseip = adverbListItemp->u.pausei;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM:
        if (MARPAESLIF_UNLIKELY(latmbp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "latm or forgiving adverb is not allowed in %s context", contexts);
          goto err;
        }
        *latmbp = adverbListItemp->u.latmb;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING:
        if (MARPAESLIF_UNLIKELY(namingpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "name adverb is not allowed in %s context", contexts);
          goto err;
        }
        *namingpp = adverbListItemp->u.namingp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SYMBOLACTION:
        if (MARPAESLIF_UNLIKELY(symbolactionpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "symbol-action adverb is not allowed in %s context", contexts);
          goto err;
        }
        *symbolactionpp = adverbListItemp->u.symbolactionp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENT_INITIALIZATION:
        if (MARPAESLIF_UNLIKELY(eventInitializationpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "event adverb is not allowed in %s context", contexts);
          goto err;
        }
        *eventInitializationpp = adverbListItemp->u.eventInitializationp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_IFACTION:
        if (MARPAESLIF_UNLIKELY(ifactionpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "if-action adverb is not allowed in %s context", contexts);
          goto err;
        }
        *ifactionpp = adverbListItemp->u.ifactionp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_REGEXACTION:
        if (MARPAESLIF_UNLIKELY(regexactionpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "regex-action adverb is not allowed in %s context", contexts);
          goto err;
        }
        *regexactionpp = adverbListItemp->u.regexactionp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENTACTION:
        if (MARPAESLIF_UNLIKELY(eventactionpp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "event-action adverb is not allowed in %s context", contexts);
          goto err;
        }
        *eventactionpp = adverbListItemp->u.eventactionp;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_DEFAULTENCODING:
        if (MARPAESLIF_UNLIKELY(defaultEncodingsp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "default-encoding adverb is not allowed in %s context", contexts);
          goto err;
        }
        *defaultEncodingsp = adverbListItemp->u.defaultEncodings;
        break;
      case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_FALLBACKENCODING:
        if (MARPAESLIF_UNLIKELY(fallbackEncodingsp == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "fallback-encoding adverb is not allowed in %s context", contexts);
          goto err;
        }
        *fallbackEncodingsp = adverbListItemp->u.fallbackEncodings;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp type at indice %d is not supported (value %d)", adverbListItemi, adverbListItemp->type);
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
static inline void _marpaESLIF_bootstrap_adverb_list_item_freev(marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp)
/*****************************************************************************/
{
  if (adverbListItemp != NULL) {
    switch (adverbListItemp->type) {
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION:
      _marpaESLIF_action_freev(adverbListItemp->u.actionp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR:
      _marpaESLIF_bootstrap_rhs_primary_freev(adverbListItemp->u.separatorRhsPrimaryp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_HIDESEPARATOR:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING:
      _marpaESLIF_bootstrap_utf_string_freev(adverbListItemp->u.namingp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SYMBOLACTION:
      _marpaESLIF_action_freev(adverbListItemp->u.symbolactionp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENT_INITIALIZATION:
      _marpaESLIF_bootstrap_event_initialization_freev(adverbListItemp->u.eventInitializationp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_IFACTION:
      _marpaESLIF_action_freev(adverbListItemp->u.ifactionp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_REGEXACTION:
      _marpaESLIF_action_freev(adverbListItemp->u.regexactionp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENTACTION:
      _marpaESLIF_action_freev(adverbListItemp->u.eventactionp);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_DEFAULTENCODING:
      free(adverbListItemp->u.defaultEncodings);
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_FALLBACKENCODING:
      free(adverbListItemp->u.fallbackEncodings);
      break;
    default:
      break;
    }
    free(adverbListItemp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;

  if      (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE                      ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME                     ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY                     ) { _marpaESLIF_bootstrap_rhs_primary_freev((marpaESLIF_bootstrap_rhs_primary_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_ALTERNATIVE                 ) { _marpaESLIF_bootstrap_rhs_alternative_freev((marpaESLIF_bootstrap_rhs_alternative_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS                             ) { _marpaESLIF_bootstrap_rhs_freev((genericStack_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION              ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION    ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION   ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION   ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR           ) { _marpaESLIF_bootstrap_rhs_primary_freev((marpaESLIF_bootstrap_rhs_primary_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PROPER              ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_HIDESEPARATOR       ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RANK                ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING        ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PRIORITY            ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PAUSE               ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LATM                ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NAMING              ) { _marpaESLIF_bootstrap_utf_string_freev((marpaESLIF_bootstrap_utf_string_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION        ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENT_INITIALIZATION) { _marpaESLIF_bootstrap_event_initialization_freev((marpaESLIF_bootstrap_event_initialization_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_IFACTION            ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_REGEXACTION         ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENTACTION         ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_DEFAULTENCODING     ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_FALLBACKENCODING    ) { free(marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS               ) { _marpaESLIF_bootstrap_adverb_list_items_freev((genericStack_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE                     ) { _marpaESLIF_bootstrap_alternative_freev((marpaESLIF_bootstrap_alternative_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVES                    ) { _marpaESLIF_bootstrap_alternatives_freev((genericStack_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_PRIORITIES                      ) { _marpaESLIF_bootstrap_priorities_freev((genericStack_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL                   ) { _marpaESLIF_bootstrap_single_symbol_freev((marpaESLIF_bootstrap_single_symbol_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL                          ) { _marpaESLIF_bootstrap_symbol_freev((marpaESLIF_bootstrap_symbol_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_TERMINAL                        ) { _marpaESLIF_bootstrap_terminal_freev((marpaESLIF_bootstrap_terminal_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE               ) { _marpaESLIF_bootstrap_grammar_reference_freev((marpaESLIF_bootstrap_grammar_reference_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_INACESSIBLE_TREATMENT           ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ON_OR_OFF                       ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_QUANTIFIER                      ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_EVENT_INITIALIZER               ) { }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_EVENT_INITIALIZATION            ) { _marpaESLIF_bootstrap_event_initialization_freev((marpaESLIF_bootstrap_event_initialization_t *) marpaESLIFValueResultp->u.p.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE_NAME                ) { free(marpaESLIFValueResultp->u.a.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY                           ) { free(marpaESLIFValueResultp->u.a.p); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_STRING                          ) { _marpaESLIF_string_freev((marpaESLIF_string_t *) marpaESLIFValueResultp->u.p.p, 0 /* onStackb */); }
  else if (marpaESLIFValueResultp->contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS                             ) { _marpaESLIF_bootstrap_lhs_freev((marpaESLIF_bootstrap_lhs_t *) marpaESLIFValueResultp->u.p.p); }
  else                                                                                                           { }
}

/*****************************************************************************/
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel, short allowEmptyStringb)
/*****************************************************************************/
{
  static const char                 *funcs       = "_marpaESLIF_bootstrap_unquote_string_literalp";
  marpaESLIF_t                      *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_bootstrap_utf_string_t *rcp         = NULL;
  marpaESLIF_bootstrap_utf_string_t rc;
  unsigned char                     *p;

  if (MARPAESLIF_UNLIKELY((bytep == NULL) || (bytel <= 0))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Invalid quoted string literal: bytep=%p, bytel=%ld", bytep, (unsigned long) bytel);
    goto err;
  }
  p = (char *) bytep;

  /* Supported cases are:
     "xxx"   " is \x{22}
     'xxx'   ' is \x{27}
     \x{201C}xxx\x{201D} where \x{201C} is LEFT DOUBLE QUOTATION MARK = \x{E2}\x{80}\x{9C}, \x{201D} is RIGHT DOUBLE QUOTATION MARK = \x{E2}\x{80}\x{9D}
   */
  switch (p[0]) {
  case 0x22:
  case 0x27:
    if (MARPAESLIF_UNLIKELY(p[bytel-1] != p[0])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Invalid quoted string literal closing character 0x%02x instead of 0x%02x", p[bytel-1], p[0]);
      goto err;
    }
    rc.bytep = ++p;
    rc.bytel = bytel - 2;
    break;
  case 0xE2:
    if (MARPAESLIF_UNLIKELY(bytel <= 6)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Invalid quoted string literal length: %ld <= 6", (unsigned long) bytel);
      goto err;
    }
    if (MARPAESLIF_UNLIKELY((p[1] != 0x80) || (p[2] != 0x9C) || (p[bytel-3] != 0xE2) || (p[bytel-2] != 0x80) || (p[bytel-1] != 0x9D))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Invalid quoted string literal quotes: 0x%02x0x%02x0x%02x ... 0x%02x0x%02x0x%02x", p[0], p[1], p[2], p[bytel-3], p[bytel-2], p[bytel-1]);
      goto err;
    }
    rc.bytep = p + 3;
    rc.bytel = bytel - 6;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Invalid quoted string first byte: 0x%02x", p[0]);
    goto err;
  }

  if (MARPAESLIF_UNLIKELY((rc.bytel <= 0) && !allowEmptyStringb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Empty string is not allowed");
    goto err;
  }
  
  rcp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (MARPAESLIF_UNLIKELY(rcp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rcp->modifiers = NULL;
  rcp->bytel = rc.bytel;
  rcp->bytep = (char *) malloc(rc.bytel + 1); /* NUL byte */
  if (MARPAESLIF_UNLIKELY(rcp->bytep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (rc.bytel > 0) {
    memcpy(rcp->bytep, rc.bytep, rc.bytel);
  }
  rcp->bytep[rc.bytel] = '\0';
  goto done;

 err:
  _marpaESLIF_bootstrap_utf_string_freev(rcp);
  rcp = NULL;

 done:
  return rcp;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t _marpaESLIF_bootstrap_ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFGrammar_t           *marpaESLIFGrammarp = marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep));
  marpaESLIF_t                  *marpaESLIFp        = marpaESLIFGrammarp->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFGrammarp); */
  marpaESLIFValueRuleCallback_t  marpaESLIFValueRuleCallbackp;
  int                            leveli;

  if (MARPAESLIF_UNLIKELY(! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL /* descp */))) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFGrammar_grammar_currentb failure");
    goto err;
  }
  /* We have only one level here */
  if (MARPAESLIF_UNLIKELY(leveli != 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "leveli is %d", leveli);
    goto err;
  }
       if (strcmp(actions, "G1_action_op_declare_1")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_op_declare_1b;                     }
  else if (strcmp(actions, "G1_action_op_declare_2")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_op_declare_2b;                     }
  else if (strcmp(actions, "G1_action_op_declare_3")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_op_declare_3b;                     }
  else if (strcmp(actions, "G1_action_rhs")                              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhsb;                              }
  else if (strcmp(actions, "G1_action_adverb_list_items")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb;                }
  else if (strcmp(actions, "G1_action_action_1")                         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_action_1b;                         }
  else if (strcmp(actions, "G1_action_action_2")                         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_action_2b;                         }
  else if (strcmp(actions, "G1_action_action_3")                         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_action_3b;                         }
  else if (strcmp(actions, "G1_action_string_literal")                   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_string_literalb;                   }
  else if (strcmp(actions, "G1_action_string_literal_inside_2")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_string_literal_inside_2b;          }
  else if (strcmp(actions, "G1_action_string_literal_inside_3")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_string_literal_inside_3b;          }
  else if (strcmp(actions, "G1_action_string_literal_inside_4")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_string_literal_inside_4b;          }
  else if (strcmp(actions, "G1_action_string_literal_inside_5")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_string_literal_inside_5b;          }
  else if (strcmp(actions, "G1_action_symbolaction_1")                   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbolaction_1b;                   }
  else if (strcmp(actions, "G1_action_symbolaction_2")                   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbolaction_2b;                   }
  else if (strcmp(actions, "G1_action_symbolaction_3")                   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbolaction_3b;                   }
  else if (strcmp(actions, "G1_action_left_association")                 == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_left_associationb;                 }
  else if (strcmp(actions, "G1_action_right_association")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_right_associationb;                }
  else if (strcmp(actions, "G1_action_group_association")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_group_associationb;                }
  else if (strcmp(actions, "G1_action_separator_specification")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_separator_specificationb;          }
  else if (strcmp(actions, "G1_action_symbol_name_1")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbol_name_1b;                    }
  else if (strcmp(actions, "G1_action_symbol_name_2")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbol_name_2b;                    }
  else if (strcmp(actions, "G1_action_rhs_alternative_1")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_1b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_2")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_2b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_3")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_3b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_4")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_4b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_5")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_5b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_6")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_6b;                }
  else if (strcmp(actions, "G1_action_rhs_alternative_7")                == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_alternative_7b;                }
  else if (strcmp(actions, "G1_action_rhs_primary_1")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_primary_1b;                    }
  else if (strcmp(actions, "G1_action_rhs_primary_2")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_primary_2b;                    }
  else if (strcmp(actions, "G1_action_parameterized_rhs_primary_1")      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_1b;        }
  else if (strcmp(actions, "G1_action_parameterized_rhs_primary_2")      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_2b;        }
  else if (strcmp(actions, "G1_action_alternative")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_alternativeb;                      }
  else if (strcmp(actions, "G1_action_alternatives")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_alternativesb;                     }
  else if (strcmp(actions, "G1_action_priorities")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_prioritiesb;                       }
  else if (strcmp(actions, "G1_action_priority_rule")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_priority_ruleb;                    }
  else if (strcmp(actions, "G1_action_single_symbol_1")                  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_single_symbol_1b;                  }
  else if (strcmp(actions, "G1_action_single_symbol_2")                  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_single_symbol_2b;                  }
  else if (strcmp(actions, "G1_action_terminal_1")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_1b;                       }
  else if (strcmp(actions, "G1_action_terminal_2")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_2b;                       }
  else if (strcmp(actions, "G1_action_terminal_3")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_3b;                       }
  else if (strcmp(actions, "G1_action_terminal_4")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_4b;                       }
  else if (strcmp(actions, "G1_action_terminal_5")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_5b;                       }
  else if (strcmp(actions, "G1_action_terminal_6")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_6b;                       }
  else if (strcmp(actions, "G1_action_symbol")                           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbolb;                           }
  else if (strcmp(actions, "G1_action_grammar_reference_1")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_grammar_reference_1b;              }
  else if (strcmp(actions, "G1_action_grammar_reference_2")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_grammar_reference_2b;              }
  else if (strcmp(actions, "G1_action_grammar_reference_3")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_grammar_reference_3b;              }
  else if (strcmp(actions, "G1_action_inaccessible_treatment_1")         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_1b;         }
  else if (strcmp(actions, "G1_action_inaccessible_treatment_2")         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_2b;         }
  else if (strcmp(actions, "G1_action_inaccessible_treatment_3")         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_3b;         }
  else if (strcmp(actions, "G1_action_inaccessible_statement")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_inaccessible_statementb;           }
  else if (strcmp(actions, "G1_action_on_or_off_1")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_on_or_off_1b;                      }
  else if (strcmp(actions, "G1_action_on_or_off_2")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_on_or_off_2b;                      }
  else if (strcmp(actions, "G1_action_autorank_statement")               == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_autorank_statementb;               }
  else if (strcmp(actions, "G1_action_quantifier_1")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_quantifier_1b;                     }
  else if (strcmp(actions, "G1_action_quantifier_2")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_quantifier_2b;                     }
  else if (strcmp(actions, "G1_action_quantified_rule")                  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_quantified_ruleb;                  }
  else if (strcmp(actions, "G1_action_start_rule")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_start_ruleb;                       }
  else if (strcmp(actions, "G1_action_desc_rule")                        == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_desc_ruleb;                        }
  else if (strcmp(actions, "G1_action_empty_rule")                       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_empty_ruleb;                       }
  else if (strcmp(actions, "G1_action_default_rule")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_default_ruleb;                     }
  else if (strcmp(actions, "G1_action_latm_specification_1")             == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_latm_specification_1b;             }
  else if (strcmp(actions, "G1_action_latm_specification_2")             == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_latm_specification_2b;             }
  else if (strcmp(actions, "G1_action_proper_specification_1")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_proper_specification_1b;           }
  else if (strcmp(actions, "G1_action_proper_specification_2")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_proper_specification_2b;           }
  else if (strcmp(actions, "G1_action_hideseparator_specification_1")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_hideseparator_specification_1b;    }
  else if (strcmp(actions, "G1_action_hideseparator_specification_2")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_hideseparator_specification_2b;    }
  else if (strcmp(actions, "G1_action_rank_specification")               == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rank_specificationb;               }
  else if (strcmp(actions, "G1_action_null_ranking_specification_1")     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_null_ranking_specification_1b;     }
  else if (strcmp(actions, "G1_action_null_ranking_specification_2")     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_null_ranking_specification_2b;     }
  else if (strcmp(actions, "G1_action_null_ranking_constant_1")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_null_ranking_constant_1b;          }
  else if (strcmp(actions, "G1_action_null_ranking_constant_2")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_null_ranking_constant_2b;          }
  else if (strcmp(actions, "G1_action_pause_specification_1")            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_pause_specification_1b;            }
  else if (strcmp(actions, "G1_action_pause_specification_2")            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_pause_specification_2b;            }
  else if (strcmp(actions, "G1_action_priority_specification")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_priority_specificationb;           }
  else if (strcmp(actions, "G1_action_event_initializer_1")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_event_initializer_1b;              }
  else if (strcmp(actions, "G1_action_event_initializer_2")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_event_initializer_2b;              }
  else if (strcmp(actions, "G1_action_event_initialization")             == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_event_initializationb;             }
  else if (strcmp(actions, "G1_action_event_specification")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_event_specificationb;              }
  else if (strcmp(actions, "G1_action_lexeme_rule")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_lexeme_ruleb;                      }
  else if (strcmp(actions, "G1_action_terminal_rule")                    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_terminal_ruleb;                    }
  else if (strcmp(actions, "G1_action_discard_rule")                     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_discard_ruleb;                     }
  else if (strcmp(actions, "G1_action_completion_event_declaration_1")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_completion_event_declaration_1b;   }
  else if (strcmp(actions, "G1_action_completion_event_declaration_2")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_completion_event_declaration_2b;   }
  else if (strcmp(actions, "G1_action_nulled_event_declaration_1")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_1b;       }
  else if (strcmp(actions, "G1_action_nulled_event_declaration_2")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_2b;       }
  else if (strcmp(actions, "G1_action_predicted_event_declaration_1")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_1b;    }
  else if (strcmp(actions, "G1_action_predicted_event_declaration_2")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_2b;    }
  else if (strcmp(actions, "G1_action_alternative_name_2")               == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_alternative_name_2b;               }
  else if (strcmp(actions, "G1_action_naming")                           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_namingb;                           }
  else if (strcmp(actions, "G1_action_exception_statement")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_exception_statementb;              }
  else if (strcmp(actions, "G1_action_luascript_statement")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_luascript_statementb;              }
  else if (strcmp(actions, "G1_action_ifaction")                         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_ifactionb;                         }
  else if (strcmp(actions, "G1_action_regexaction")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_regexactionb;                      }
  else if (strcmp(actions, "G1_action_eventaction")                      == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_eventactionb;                      }
  else if (strcmp(actions, "G1_action_defaultencoding")                  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_defaultencodingb;                  }
  else if (strcmp(actions, "G1_action_fallbackencoding")                 == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_fallbackencodingb;                 }
  else if (strcmp(actions, "G1_action_lhs_1")                            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_lhs_1b;                            }
  else if (strcmp(actions, "G1_action_lhs_2")                            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_lhs_2b;                            }
  else if (strcmp(actions, "G1_action_lhs_2")                            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_lhs_2b;                            }
  else
  {
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueRuleCallbackp = NULL;
 done:
  return marpaESLIFValueRuleCallbackp;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbol_name_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <symbol name>  ::= <bare name> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *barenames   = NULL;
  char         *asciis; /* bare name is only ASCII letters as per the grammar */
  size_t        asciil;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, arg0i, asciis, asciil); /* This is a lexeme */

  if (MARPAESLIF_UNLIKELY((asciis == NULL) || (asciil <= 0))) {
    /* Should never happen as per the grammar */
    MARPAESLIF_ERROR(marpaESLIFp, "Null bare name");
    goto err;
  }
  barenames = (char *) malloc(asciil + 1);
  if (MARPAESLIF_UNLIKELY(barenames == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  strcpy(barenames, asciis);
  barenames[asciil] = '\0';

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME, barenames);

  rcb = 1;
  goto done;
 err:
  if (barenames != NULL) {
    free(barenames);
  }
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <symbol name>  ::= <bracketed name> */
  marpaESLIF_t *marpaESLIFp      = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *bracketednames   = NULL;
  char         *asciis; /* bare name is only ASCII letters as per the grammar */
  size_t        asciil;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, arg0i, asciis, asciil); /* This is a lexeme */

  if (MARPAESLIF_UNLIKELY((asciis == NULL) || (asciil <= 0))) {
    /* Should never happen as per the grammar */
    MARPAESLIF_ERROR(marpaESLIFp, "Null bare name");
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(asciil < 2)) {
    /* Should never happen neither as per the grammar */
    MARPAESLIF_ERRORF(marpaESLIFp, "Length of bare name is %ld", (unsigned long) asciil);
    goto err;
  }
  /* We just remove the '<' and '>' around... */
  bracketednames = (char *) malloc(asciil - 2 + 1);
  if (MARPAESLIF_UNLIKELY(bracketednames == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  strncpy(bracketednames, asciis + 1, asciil - 2);
  bracketednames[asciil - 2] = '\0';

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME, bracketednames);

  rcb = 1;
  goto done;
 err:
  if (bracketednames != NULL) {
    free(bracketednames);
  }
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <op declare> ::= <op declare top grammar> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 0 /* ::= is level No 0 */);

  rcb = 1;
  goto done;
 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_op_declare_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <op declare> ::= <op declare lex grammar> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 1 /* ~ is level No 0 */);

  rcb = 1;
  goto done;
 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_op_declare_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <op declare> ::= <op declare any grammar> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *asciis; /* <op declare any grammar> is only ASCII letters as per the grammar */
  size_t        asciil;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, arg0i, asciis, asciil);

  /* <op declare any grammar> lexeme definition is /:\[\d+\]:=/ i.e. start with 2 ASCII characters and end with 3 ASCII characters */
  if (MARPAESLIF_UNLIKELY(asciil < 5)) {
    /* Should never happen as per the grammar */
    MARPAESLIF_ERROR(marpaESLIFp, "<op declare any grammar> is not long enough");
    goto err;
  }

  /* atoi() will stop at the ']' by definition: no need to allocate a NUL terminated string just for that */
  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, atoi(asciis + 2));

  rcb = 1;
  goto done;
 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs> ::= <rhs primary>+ */
  marpaESLIF_t                           *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                         *rhsAlternativeStackp = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t *rhsAlternativep      = NULL;
  int                                     i;
  short                                   rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  GENERICSTACK_NEW(rhsAlternativeStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsAlternativeStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsAlternativeStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i++) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, rhsAlternativep);
    if (MARPAESLIF_UNLIKELY(rhsAlternativep == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "An RHS alternative is not set");
      goto err;
    }

    GENERICSTACK_PUSH_PTR(rhsAlternativeStackp, rhsAlternativep);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsAlternativeStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsAlternativeStackp push failure, %s", strerror(errno));
      goto err;
    }
    rhsAlternativep = NULL; /* rhsAlternativep is now in rhsAlternativeStackp */
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS, rhsAlternativeStackp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_freev(rhsAlternativeStackp);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep); /* This is NULL tolerant */
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <adverb list items> ::= <adverb item>* */
  marpaESLIF_t                                *marpaESLIFp            = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                              *adverbListItemStackp   = NULL;
  marpaESLIF_bootstrap_adverb_list_item_t     *adverbListItemp        = NULL;
  marpaESLIF_action_t                         *actionp                = NULL;
  short                                        left_associationb      = 0;
  short                                        right_associationb     = 0;
  short                                        group_associationb     = 0;
  marpaESLIF_bootstrap_rhs_primary_t          *separatorRhsPrimaryp   = NULL;
  short                                        properb                = 0;
  short                                        hideseparatorb         = 0;
  int                                          ranki                  = 0;
  short                                        nullRanksHighb         = 0;
  int                                          priorityi              = 0;
  int                                          pausei                 = 0;
  short                                        latmb                  = 0;
  marpaESLIF_bootstrap_utf_string_t           *namingp                = NULL;
  marpaESLIF_action_t                         *symbolactionp          = NULL;
  marpaESLIF_action_t                         *ifactionp              = NULL;
  marpaESLIF_action_t                         *regexactionp           = NULL;
  marpaESLIF_action_t                         *eventactionp           = NULL;
  char                                        *defaultEncodings       = NULL;
  char                                        *fallbackEncodings      = NULL;
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp   = NULL;
  void                                        *contextp;
  int                                          i;
  short                                        rcb;
  short                                        undefb;

  GENERICSTACK_NEW(adverbListItemStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(adverbListItemStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* In theory, if we are called, this is because there is something on the stack */
  /* In any case, this is okay to have an empty stack -; */
  if (! nullableb) {
    for (i = arg0i; i <= argni; i++) {
      /* The null adverb is pushing undef */
      MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, i, undefb);
      if (undefb) {
        continue;
      }

      MARPAESLIF_BOOTSTRAP_GET_CONTEXT(marpaESLIFValuep, i, contextp);

      adverbListItemp = (marpaESLIF_bootstrap_adverb_list_item_t *) malloc(sizeof(marpaESLIF_bootstrap_adverb_list_item_t));
      if (MARPAESLIF_UNLIKELY(adverbListItemp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      adverbListItemp->type = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NA;

      if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, actionp);
        if (MARPAESLIF_UNLIKELY(actionp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item action is NULL");
          goto err;
        }
        adverbListItemp->type      = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION;
        adverbListItemp->u.actionp = actionp;
        actionp = NULL; /* actionp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, left_associationb);
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION;
        adverbListItemp->u.left_associationb = left_associationb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, right_associationb);
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION;
        adverbListItemp->u.right_associationb = right_associationb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, group_associationb);
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION;
        adverbListItemp->u.group_associationb = group_associationb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, separatorRhsPrimaryp);
        if (MARPAESLIF_UNLIKELY(separatorRhsPrimaryp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item separator is NULL");
          goto err;
        }
        adverbListItemp->type                   = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR;
        adverbListItemp->u.separatorRhsPrimaryp = separatorRhsPrimaryp;
        separatorRhsPrimaryp = NULL; /* separatorRhsPrimaryp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PROPER) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, properb);
        adverbListItemp->type      = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER;
        adverbListItemp->u.properb = properb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_HIDESEPARATOR) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, hideseparatorb);
        adverbListItemp->type      = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_HIDESEPARATOR;
        adverbListItemp->u.properb = hideseparatorb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RANK) {
        MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, i, ranki);
        adverbListItemp->type    = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK;
        adverbListItemp->u.ranki = ranki;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, nullRanksHighb);
        adverbListItemp->type             = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING;
        adverbListItemp->u.nullRanksHighb = nullRanksHighb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PRIORITY) {
        MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, i, priorityi);
        adverbListItemp->type        = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY;
        adverbListItemp->u.priorityi = priorityi;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PAUSE) {
        MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, i, pausei);
        adverbListItemp->type     = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE;
        adverbListItemp->u.pausei = pausei;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LATM) {
        MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, i, latmb);
        adverbListItemp->type    = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM;
        adverbListItemp->u.latmb = latmb;
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NAMING) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, namingp);
        if (MARPAESLIF_UNLIKELY(namingp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item name is NULL");
          goto err;
        }
        adverbListItemp->type      = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING;
        adverbListItemp->u.namingp = namingp;
        namingp = NULL; /* namingp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, symbolactionp);
        if (MARPAESLIF_UNLIKELY(symbolactionp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item symbol-action is NULL");
          goto err;
        }
        adverbListItemp->type           = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SYMBOLACTION;
        adverbListItemp->u.symbolactionp = symbolactionp;
        symbolactionp = NULL; /* symbolactionp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENT_INITIALIZATION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, eventInitializationp);
        if (MARPAESLIF_UNLIKELY(eventInitializationp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item event is NULL");
          goto err;
        }
        adverbListItemp->type                   = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENT_INITIALIZATION;
        adverbListItemp->u.eventInitializationp = eventInitializationp;
        eventInitializationp = NULL; /* eventInitializationp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_IFACTION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, ifactionp);
        if (MARPAESLIF_UNLIKELY(ifactionp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item if-action is NULL");
          goto err;
        }
        adverbListItemp->type        = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_IFACTION;
        adverbListItemp->u.ifactionp = ifactionp;
        ifactionp = NULL; /* ifactionp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_REGEXACTION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, regexactionp);
        if (MARPAESLIF_UNLIKELY(regexactionp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item regex-action is NULL");
          goto err;
        }
        adverbListItemp->type           = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_REGEXACTION;
        adverbListItemp->u.regexactionp = regexactionp;
        regexactionp = NULL; /* regexactionp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENTACTION) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, eventactionp);
        if (MARPAESLIF_UNLIKELY(eventactionp == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item event-action is NULL");
          goto err;
        }
        adverbListItemp->type           = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_EVENTACTION;
        adverbListItemp->u.eventactionp = eventactionp;
        eventactionp = NULL; /* eventactionp is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_DEFAULTENCODING) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, defaultEncodings);
        if (MARPAESLIF_UNLIKELY(defaultEncodings == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item default-encoding is NULL");
          goto err;
        }
        adverbListItemp->type               = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_DEFAULTENCODING;
        adverbListItemp->u.defaultEncodings = defaultEncodings;
        defaultEncodings = NULL; /* defaultEncodings is now in adverbListItemp */
      } else if (contextp == MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_FALLBACKENCODING) {
        MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, fallbackEncodings);
        if (MARPAESLIF_UNLIKELY(fallbackEncodings == NULL)) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item fallback-encoding is NULL");
          goto err;
        }
        adverbListItemp->type               = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_FALLBACKENCODING;
        adverbListItemp->u.fallbackEncodings = fallbackEncodings;
        fallbackEncodings = NULL; /* fallbackEncodings is now in adverbListItemp */
      } else {
        /* bootstrap managed marpaESLIF grammar itself, that is independant */
        MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported adverb list item type %p", contextp);
        goto err;
      }

      GENERICSTACK_PUSH_PTR(adverbListItemStackp, (void *) adverbListItemp);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(adverbListItemStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp push failure, %s", strerror(errno));
        goto err;
      }
      adverbListItemp = NULL; /* adverbListItemp is now in adverbListItemStackp */
    }
  }

  /* It is possible to do a sanity check here */
  if (MARPAESLIF_UNLIKELY(left_associationb +  right_associationb + group_associationb > 1)) {
    MARPAESLIF_ERROR(marpaESLIFp, "assoc => left, assoc => right and assoc => group are mutually exclusive");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS, adverbListItemStackp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(actionp);
  _marpaESLIF_action_freev(symbolactionp);
  _marpaESLIF_action_freev(ifactionp);
  _marpaESLIF_action_freev(regexactionp);
  _marpaESLIF_action_freev(eventactionp);
  if (defaultEncodings != NULL) {
    free(defaultEncodings);
  }
  if (fallbackEncodings != NULL) {
    free(fallbackEncodings);
  }
  _marpaESLIF_bootstrap_event_initialization_freev(eventInitializationp);
  _marpaESLIF_bootstrap_utf_string_freev(namingp);
  _marpaESLIF_bootstrap_rhs_primary_freev(separatorRhsPrimaryp);
  _marpaESLIF_bootstrap_adverb_list_item_freev(adverbListItemp);
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_action_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'action' '=>' <action name> */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *names       = NULL;
  marpaESLIF_action_t *actionp     = NULL;
  char                *actions     = NULL;
  char                *tmps;
  short                luab;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, names);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(names == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }
  if (strstr(names, LUA_ACTION_PREFIX) == names) {
    /* lua action ? */
    tmps = names + strlen(LUA_ACTION_PREFIX);
    if (MARPAESLIF_UNLIKELY(*tmps == '\0')) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No lua identifier after %s", LUA_ACTION_PREFIX);
      goto err;
    }
    actions = strdup(tmps);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 1;
  } else {
    actions = strdup(names);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 0;
  }

  actionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(actionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (luab) {
    actionp->type = MARPAESLIF_ACTION_TYPE_LUA;
    actionp->u.luas = actions;
  } else {
    actionp->type = MARPAESLIF_ACTION_TYPE_NAME;
    actionp->u.names = actions;
  }
  actions = NULL; /* actions is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION, actionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(actionp);
  rcb = 0;

 done:
  if (actions != NULL) {
    free(actions);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_action_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'action' '=>' <string literal> */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_string_t *stringp     = NULL;
  marpaESLIF_action_t *actionp     = NULL;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <string literal> is a PTR */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, argni, stringp);
  /* It is a non-sense to not have no string in this case */
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }

  actionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(actionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  actionp->type      = MARPAESLIF_ACTION_TYPE_STRING;
  actionp->u.stringp = stringp;
  stringp            = NULL; /* stringp is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION, actionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(actionp);
  rcb = 0;

 done:
  _marpaESLIF_string_freev(stringp, 0 /* onStackb */);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_action_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'action' '=>' <quoted string literal> */
  marpaESLIF_t                      *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_string_t               *stringp       = NULL;
  marpaESLIF_action_t               *actionp       = NULL;
  marpaESLIF_bootstrap_utf_string_t *quotedStringp = NULL;
  void                              *bytep         = NULL;
  short                              shallowb      = 0;
  size_t                             bytel;
  short                              rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, argni, bytep, bytel, shallowb);
  quotedStringp = _marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValuep, bytep, bytel, 1 /* allowEmptyStringb */);
  if (MARPAESLIF_UNLIKELY(quotedStringp == NULL)) {
    goto err;
  }

  stringp = _marpaESLIF_string_newp(marpaESLIFp, (char *) MARPAESLIF_UTF8_STRING, quotedStringp->bytep, quotedStringp->bytel);
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }

  actionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(actionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  actionp->type      = MARPAESLIF_ACTION_TYPE_STRING;
  actionp->u.stringp = stringp;
  stringp            = NULL; /* stringp is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION, actionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(actionp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  _marpaESLIF_string_freev(stringp, 0 /* onStackb */);
  _marpaESLIF_bootstrap_utf_string_freev(quotedStringp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_string_literalb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <string literal> ::= <string literal unit>+ */
  static const char   *funcs       = "_marpaESLIF_bootstrap_G1_action_string_literalb";
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_string_t *stringp     = NULL;
  char                *charp       = NULL;
  size_t               charl       = 0;
  char                *converteds  = NULL;
  size_t               convertedl;
  int                  i;
  char                *p;
  void                *bytep;
  size_t               bytel;
  short                undefb;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* Get total size, take care it is possible that one of the string literal unit is empty (aka undef) */
  for (i = arg0i; i<= argni; i++) {
    MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, i, undefb);
    if (undefb) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "String literal indice %d is empty\n", i - arg0i);
      continue;
    }
    MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, i, bytep, bytel);
    charl += bytel;
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "String literal indice %d size is 0x%ld, total size is now 0x%ld\n", i - arg0i, (unsigned long) bytel, (unsigned long) charl);
  }

  /* Total concatenated size is empty ? */
  if (charl <= 0) {
    stringp = _marpaESLIF_string_newp(marpaESLIFp, (char *) MARPAESLIF_UTF8_STRING, NULL, 0); /* Will produce empty string */
  } else {
    charp = (char *) malloc(charl + 1);
    if (MARPAESLIF_UNLIKELY(charp == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    p = charp;
    for (i = arg0i; i<= argni; i++) {
      MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, i, undefb);
      if (undefb) {
      continue;
      }
      MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, i, bytep, bytel);
      memcpy(p, bytep, bytel);
      p += bytel;
    }
    *p = '\0'; /* For convenience */

    /* Call for conversion in any case, this is a way to validate UTF-8 correctness */
    converteds = _marpaESLIF_charconvb(marpaESLIFp,
                                       "UTF-8",
                                       (char *) MARPAESLIF_UTF8_STRING, /* We request that input is an UTF-8 string */
                                       charp,
                                       charl, /* Skip the automatic NUL byte in the source */
                                       &convertedl,
                                       NULL, /* fromEncodingsp */
                                       NULL, /* tconvpp */
                                       1, /* eofb */
                                       NULL, /* byteleftsp */
                                       NULL, /* byteleftlp */
                                       NULL, /* byteleftalloclp */
                                       0, /* tconvsilentb */
                                       NULL, /* defaultEncodings */
                                       NULL /* fallbackEncodings */);
    if (MARPAESLIF_UNLIKELY(converteds == NULL)) {
      goto err;
    }
    stringp = _marpaESLIF_string_newp(marpaESLIFp, (char *) MARPAESLIF_UTF8_STRING, charp, charl);
  }

  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_STRING, stringp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_string_freev(stringp, 0 /* onStackb */);
  rcb = 0;

 done:
  if (converteds != NULL) {
    free(converteds);
  }
  if (charp != NULL) {
    free(charp);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_string_literal_inside_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <string literal inside> ::= '\\' ["'?\\abfnrtve] */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *charp       = NULL;
  size_t        charl       = sizeof(char);
  char          p;
  char          c;
  void         *bytep;
  size_t        bytel;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* ["\\abfnrtve] is a lexeme of size 1 */
  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, argni, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(bytel != 1)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Escaped character must be of size 1");
    goto err;
  }
  /* We use the \x notation in case the compiler does not support the metacharacter */
  p = * (char *) bytep;
  switch (p) {
  case 'a':
    c = 0x07;
    break;
  case 'b':
    c = 0x08;
    break;
  case 'f':
    c = 0x0C;
    break;
  case 'n':
    c = 0x0A;
    break;
  case 'r':
    c = 0x0D;
    break;
  case 't':
    c = 0x09;
    break;
  case 'v':
    c = 0x0B;
    break;
  case '\\':
    c = 0x5C;
    break;
  case '\'':
    c = 0x27;
    break;
  case '"':
    c = 0x22;
    break;
  case '?':
    c = 0x3F;
    break;
  case 'e':
    c = 0x1B;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported escaped character '%c' (0x%lx)", p, (unsigned long) p);
    goto err;
  }

  charp = (char *) malloc(charl + 1);
  if (MARPAESLIF_UNLIKELY(charp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  charp[0] = c;
  charp[1] = '\0'; /* For convenience */
  MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY, charp, charl);

  rcb = 1;
  goto done;

 err:
  if (charp != NULL) {
    free(charp);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_string_literal_inside_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <string literal inside> ::= '\\' /x\{[a-fA-F0-9]{2}\}/ */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *charp       = NULL;
  size_t        charl       = sizeof(char);
  char          c           = 0;
  char         *p;
  void         *bytep;
  size_t        bytel;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* /x\\{[a-fA-F0-9]{2}\\}/ is a lexeme of size 5 */
  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, argni, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(bytel != 5)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Escaped hex character must be of size 5");
    goto err;
  }
  p = (char *) bytep;
  p += 2;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(c, *p++);
  c <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(c, *p);
  
  charp = (char *) malloc(charl + 1);
  if (MARPAESLIF_UNLIKELY(charp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  charp[0] = c;
  charp[1] = '\0'; /* For convenience */
  MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY, charp, charl);
  
  rcb = 1;
  goto done;

 err:
  if (charp != NULL) {
    free(charp);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_string_literal_inside_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <string literal inside> ::= '\\' /u\{[a-fA-F0-9]{4}\}/ */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *charp       = NULL;
  marpaESLIF_uint32_t  uint32      = 0;
  PCRE2_UCHAR          bufferp[6];
  size_t               charl;
  char                *p;
  void                *bytep;
  size_t               bytel;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* /u\{[a-fA-F0-9]{4}\}/ is a lexeme of size 7 */
  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, argni, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(bytel != 7)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Escaped codepoint must be of size 7");
    goto err;
  }
  p = (char *) bytep;
  p += 2;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p);

  /* Transform this codepoint into an UTF-8 character - this is copy/pasted from pcre2_ord2utf.c */
  charl = _marpaESLIF_bootstrap_ord2utfb(uint32, bufferp);
  if (MARPAESLIF_UNLIKELY(charl <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Failed to determine UTF-8 byte size of 0x%ld", (unsigned long) uint32);
    goto err;
  }
  charp = (char *) malloc(charl + 1);
  if (MARPAESLIF_UNLIKELY(charp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(charp, bufferp, charl);
  charp[charl] = '\0'; /* For convenience */
  MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY, charp, charl);

  rcb = 1;
  goto done;

 err:
  if (charp != NULL) {
    free(charp);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_string_literal_inside_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <string literal inside> ::= '\\' /U\{[a-fA-F0-9]{8}\}/ */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *charp       = NULL;
  marpaESLIF_uint32_t  uint32      = 0;
  PCRE2_UCHAR          bufferp[6];
  size_t               charl;
  char                *p;
  void                *bytep;
  size_t               bytel;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* /U\{[a-fA-F0-9]{8}\}/ is a lexeme of size 11 */
  MARPAESLIF_BOOTSTRAP_GET_ARRAY(marpaESLIFValuep, argni, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(bytel != 11)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Escaped codepoint must be of size 11");
    goto err;
  }
  p = (char *) bytep;
  p += 2;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p++);
  uint32 <<= 4;
  MARPAESLIF_BOOTSTRAP_DST_OR_VALCHAR(uint32, *p);

  /* Transform this codepoint into an UTF-8 character - this is copy/pasted from pcre2_ord2utf.c */
  charl = _marpaESLIF_bootstrap_ord2utfb(uint32, bufferp);
  if (MARPAESLIF_UNLIKELY(charl <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Failed to determine UTF-8 byte size of 0x%ld", (unsigned long) uint32);
    goto err;
  }
  charp = (char *) malloc(charl + 1);
  if (MARPAESLIF_UNLIKELY(charp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(charp, bufferp, charl);
  charp[charl] = '\0'; /* For convenience */
  MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY, charp, charl);

  rcb = 1;
  goto done;

 err:
  if (charp != NULL) {
    free(charp);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbolaction_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'symbol-action' '=>' <action name> */
  marpaESLIF_t        *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *names         = NULL;
  marpaESLIF_action_t *symbolactionp = NULL;
  char                *actions       = NULL;
  char                *tmps;
  short                luab;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, names);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(names == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbol-action at indice %d returned NULL", argni);
    goto err;
  }
  if (strstr(names, LUA_ACTION_PREFIX) == names) {
    /* lua action ? */
    tmps = names + strlen(LUA_ACTION_PREFIX);
    if (MARPAESLIF_UNLIKELY(*tmps == '\0')) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No lua identifier after %s", LUA_ACTION_PREFIX);
      goto err;
    }
    actions = strdup(tmps);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 1;
  } else {
    actions = strdup(names);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 0;
  }

  symbolactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(symbolactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (luab) {
    symbolactionp->type = MARPAESLIF_ACTION_TYPE_LUA;
    symbolactionp->u.luas = actions;
  } else {
    symbolactionp->type = MARPAESLIF_ACTION_TYPE_NAME;
    symbolactionp->u.names = actions;
  }
  actions = NULL; /* actions is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION, symbolactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(symbolactionp);
  rcb = 0;

 done:
  if (actions != NULL) {
    free(actions);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbolaction_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'symbol-action' '=>' <string literal> */
  marpaESLIF_t        *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_string_t *stringp       = NULL;
  marpaESLIF_action_t *symbolactionp = NULL;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <string literal> is a PTR */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, argni, stringp);
  /* It is a non-sense to not have no string in this case */
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }

  symbolactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(symbolactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  symbolactionp->type      = MARPAESLIF_ACTION_TYPE_STRING;
  symbolactionp->u.stringp = stringp;
  stringp                  = NULL; /* stringp is now in symbolactionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION, symbolactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(symbolactionp);
  rcb = 0;

 done:
  _marpaESLIF_string_freev(stringp, 0 /* onStackb */);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbolaction_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'symbol-action' '=>' <quoted string literal> */
  marpaESLIF_t                      *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_utf_string_t *quotedStringp = NULL;
  marpaESLIF_string_t               *stringp       = NULL;
  marpaESLIF_action_t               *symbolactionp = NULL;
  void                              *bytep         = NULL;
  short                              shallowb      = 0;
  size_t                             bytel;
  short                              rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, argni, bytep, bytel, shallowb);
  quotedStringp = _marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValuep, bytep, bytel, 1 /* allowEmptyStringb */);
  if (MARPAESLIF_UNLIKELY(quotedStringp == NULL)) {
    goto err;
  }

  stringp = _marpaESLIF_string_newp(marpaESLIFp, (char *) MARPAESLIF_UTF8_STRING, quotedStringp->bytep, quotedStringp->bytel);
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }

  symbolactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(symbolactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  symbolactionp->type      = MARPAESLIF_ACTION_TYPE_STRING;
  symbolactionp->u.stringp = stringp;
  stringp                  = NULL; /* stringp is now in symbolactionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION, symbolactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(symbolactionp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  _marpaESLIF_string_freev(stringp, 0 /* onStackb */);
  _marpaESLIF_bootstrap_utf_string_freev(quotedStringp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_separator_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* 'separator' '=>' <rhs primary> */
  marpaESLIF_t                       *marpaESLIFp  = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp  = NULL;
  short                               rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, argni, rhsPrimaryp);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "separator at indice %d returned NULL", argni);
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR, rhsPrimaryp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_left_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <left association> ::= 'assoc' '=>' 'left' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_right_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <right association> ::= 'assoc' '=>' 'right' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_group_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <group association> ::= 'assoc' '=>' 'group' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= <rhs primary>' */
  marpaESLIF_t                             *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp        = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t   *rhsAlternativep    = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, rhsPrimaryp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp at indice %d is NULL", arg0i);
    goto err;
  }

  /* Make that an rhs alternative structure */
  rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_alternative_t));
  if (MARPAESLIF_UNLIKELY(rhsAlternativep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsAlternativep->symbolShallowp   = NULL;
  rhsAlternativep->type             = MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_RHS_PRIMARY;
  rhsAlternativep->u.rhsPrimaryp    = rhsPrimaryp;
  rhsPrimaryp = NULL; /* rhsPrimaryp is in rhsAlternativep */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_ALTERNATIVE, rhsAlternativep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(-' <priorities> '-)' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_2_and_3b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 1 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(' <priorities> ')' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_2_and_3b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 0 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(-' <rhs primary> '-' <rhs primary> <adverb list> '-)' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_4_and_5b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 1 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(' <rhs primary> '-' <rhs primary> <adverb list> ')' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_4_and_5b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 0 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_6b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(-' <rhs primary> <quantifier> <adverb list> '-)' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_6_and_7b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 1 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_alternative_7b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs alternative> ::= '(' <rhs primary> <quantifier> <adverb list> ')' */
  return _marpaESLIF_bootstrap_G1_action_rhs_alternative_6_and_7b(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, 0 /* skipb */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs primary> ::= <single symbol> */
  /* <single symbol> is on the stack, typed MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL */
  marpaESLIF_bootstrap_rhs_primary_t   *rhsPrimaryp   = NULL;
  marpaESLIF_t                         *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp = NULL;
  short                                 rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, singleSymbolp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(singleSymbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "singleSymbolp at indice %d is NULL", argni);
    goto err;
  }

  /* Make that an rhs primary structure */
  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->symbolShallowp  = NULL;
  rhsPrimaryp->type            = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL;
  rhsPrimaryp->u.singleSymbolp = singleSymbolp;
  singleSymbolp = NULL; /* singleSymbolp is now in rhsPrimaryp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs primary> ::= <symbol> '@' <grammar reference> */
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp = NULL;
  marpaESLIF_t                             *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_symbol_t            *symbolp= NULL;
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, symbolp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolp at indice %d is NULL", arg0i);
    goto err;
  }

  /* <grammar reference> is a pointer */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+2, grammarReferencep);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(grammarReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned NULL", arg0i+2);
    goto err;
  }

  /* Make that an rhs primary structure */
  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rhsPrimaryp->symbolShallowp = NULL;
  rhsPrimaryp->type           = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA;

  rhsPrimaryp->u.symbolAndReferencep = (marpaESLIF_bootstrap_symbol_and_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_and_reference_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.symbolAndReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_AND_REFERENCE;
  rhsPrimaryp->u.symbolAndReferencep->symbolp           = symbolp;
  rhsPrimaryp->u.symbolAndReferencep->grammarReferencep = grammarReferencep;
  symbolp = NULL; /* symbolp is in symbolAndReferencep */
  grammarReferencep = NULL; /* grammarReferencep  is in symbolAndReferencep */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_symbol_freev(symbolp);
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <parameterized rhs primary> ::= '&' <symbol> <arguments> */
  static const char                        *funcs              = "_marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_1b";
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp        = NULL;
  marpaESLIF_t                             *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIFGrammar_t                      *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  short                                     noargumentb;
  marpaESLIFValueResultRow_t                row;
  size_t                                    rowl;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rhsPrimaryp->symbolShallowp = NULL;
  rhsPrimaryp->type           = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA;

  rhsPrimaryp->u.parameterizedSymbolp = (marpaESLIF_bootstrap_parameterized_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_parameterized_symbol_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.parameterizedSymbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type                                   = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL;
  rhsPrimaryp->u.parameterizedSymbolp->symbolp        = NULL;
  rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = NULL;

  rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = &(rhsPrimaryp->u.parameterizedSymbolp->_argumentStack);
  GENERICSTACK_INIT(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp))) {
    rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = NULL;
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp->u.parameterizedSymbolp->argumentStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+1, rhsPrimaryp->u.parameterizedSymbolp->symbolp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.parameterizedSymbolp->symbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolp at indice %d is NULL", arg0i+1);
    goto err;
  }

  /* There may be no argument */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i + 2, noargumentb);
  if (! noargumentb) {
    MARPAESLIF_BOOTSTRAP_GET_ROW(marpaESLIFValuep, arg0i + 2, row);
    /* The number of arguments is the row size */
    for (rowl = 0; rowl < row.sizel; rowl++) {
      if (! marpaESLIFValue_importb(marpaESLIFValuep, &(row.p[rowl]))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Failed to import argument No %ld", (unsigned long) rowl);
        goto err;
      }
      GENERICSTACK_PUSH_PTR(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp, marpaESLIFGrammarp->flatStackp);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp->u.parameterizedSymbolp->argumentStackp push failure, %s", strerror(errno));
        goto err;
      }
      marpaESLIFGrammarp->flatStackp = NULL; /* flatStackp is in rhsPrimaryp->u.parameterizedSymbolp->argumentStackp */
    }
  }

  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: %s with %d parameters", funcs, rhsPrimaryp->u.parameterizedSymbolp->symbolp->symbols, GENERICSTACK_USED(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp));
  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_parameterized_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <parameterized rhs primary> ::= '&' <symbol> <arguments> '@' <grammar reference> */
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp        = NULL;
  marpaESLIF_t                       *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIFGrammar_t                *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  short                               noargumentb;
  marpaESLIFValueResultRow_t          row;
  size_t                              rowl;
  short                               rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rhsPrimaryp->symbolShallowp = NULL;
  rhsPrimaryp->type           = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA;

  rhsPrimaryp->u.parameterizedSymbolAndReferencep = (marpaESLIF_bootstrap_parameterized_symbol_and_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_parameterized_symbol_and_reference_t));
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.parameterizedSymbolAndReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type                                                   = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL_AND_REFERENCE;
  rhsPrimaryp->u.parameterizedSymbolAndReferencep->symbolp            = NULL;
  rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp     = NULL;
  rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep  = NULL;

  rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp = &(rhsPrimaryp->u.parameterizedSymbolAndReferencep->_argumentStack);
  GENERICSTACK_INIT(rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp))) {
    rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp = NULL;
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp->u.parameterizedSymbolAndReferencep->argumentStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+1, rhsPrimaryp->u.parameterizedSymbolAndReferencep->symbolp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.parameterizedSymbolAndReferencep->symbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolp at indice %d is NULL", arg0i+1);
    goto err;
  }

  /* There may be no argument */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i + 2, noargumentb);
  if (! noargumentb) {
    MARPAESLIF_BOOTSTRAP_GET_ROW(marpaESLIFValuep, arg0i + 2, row);
    /* The number of arguments is the row size */
    for (rowl = 0; rowl < row.sizel; rowl++) {
      if (! marpaESLIFValue_importb(marpaESLIFValuep, &(row.p[rowl]))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Failed to import argument No %ld", (unsigned long) rowl);
        goto err;
      }
      GENERICSTACK_PUSH_PTR(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp, marpaESLIFGrammarp->flatStackp);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsPrimaryp->u.parameterizedSymbolp->argumentStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp->u.parameterizedSymbolp->argumentStackp push failure, %s", strerror(errno));
        goto err;
      }
      marpaESLIFGrammarp->flatStackp = NULL; /* flatStackp is in rhsPrimaryp->u.parameterizedSymbolp->argumentStackp */
    }
  }

  /* <grammar reference> is a pointer */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+4, rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(rhsPrimaryp->u.parameterizedSymbolAndReferencep->grammarReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned NULL", arg0i+4);
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_alternativeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* alternative ::= <rhs> <adverb list> */
  marpaESLIF_t                       *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_alternative_t *alternativep         = NULL;
  genericStack_t                     *adverbListItemStackp = NULL;
  genericStack_t                     *rhsAlternativeStackp = NULL;
  short                               undefb;
  short                               rcb;

  /* rhs must be a non-NULL generic stack of the primary */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, rhsAlternativeStackp);
  if (MARPAESLIF_UNLIKELY(rhsAlternativeStackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFp, "rhsAlternativeStackp is NULL");
    goto err;
  }
  
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  alternativep = (marpaESLIF_bootstrap_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_alternative_t));
  if (MARPAESLIF_UNLIKELY(alternativep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  alternativep->rhsAlternativeStackp = rhsAlternativeStackp;
  alternativep->adverbListItemStackp = adverbListItemStackp;
  alternativep->priorityi            = 0;    /* Used when there is the loosen "||" operator */
  alternativep->forcedLhsp           = NULL; /* Ditto */

  rhsAlternativeStackp = NULL;
  adverbListItemStackp = NULL;

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE, alternativep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternative_freev(alternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  _marpaESLIF_bootstrap_rhs_freev(rhsAlternativeStackp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_alternativesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* alternatives ::= alternative+  separator => <op equal priority> proper => 1 hide-separator => 1*/
  marpaESLIF_t                       *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                     *alternativeStackp = NULL;
  marpaESLIF_bootstrap_alternative_t *alternativep      = NULL;
  int                                i;
  short                              rcb;

  GENERICSTACK_NEW(alternativeStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(alternativeStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i++) { /* The separator is skipped from the list of arguments */
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, alternativep);
    GENERICSTACK_PUSH_PTR(alternativeStackp, (void *) alternativep);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(alternativeStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
      goto err;
    }
    alternativep = NULL; /* alternativep is now in alternativeStackp */
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVES, alternativeStackp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternative_freev(alternativep);
  _marpaESLIF_bootstrap_alternatives_freev(alternativeStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_prioritiesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* priorities ::= alternatives+ separator => <op loosen> proper => 1 hide-separator => 1*/
  marpaESLIF_t   *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t *alternativesStackp = NULL;
  genericStack_t *alternativeStackp  = NULL;
  int             i;
  short           rcb;

  GENERICSTACK_NEW(alternativesStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(alternativesStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i++) { /* The separator is skipped from the list of arguments */
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, i, alternativeStackp);
    GENERICSTACK_PUSH_PTR(alternativesStackp, (void *) alternativeStackp);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(alternativesStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp push failure, %s", strerror(errno));
      goto err;
    }
    alternativeStackp = NULL; /* alternativeStackp is now in alternativesStackp */
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_PRIORITIES, alternativesStackp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternatives_freev(alternativeStackp);
  _marpaESLIF_bootstrap_priorities_freev(alternativesStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_priority_loosen_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_symbol_t *lhsp, genericStack_t *alternativesStackp, genericStack_t *parameterStackp)
/*****************************************************************************/
{
  /* <priority rule> ::= lhs <op declare> priorities */
  /* This method is called when there is more than one priority. It reconstruct a flat list with one priority only */
  static const char                       *funcs                      = "_marpaESLIF_bootstrap_G1_action_priority_loosen_ruleb";
  marpaESLIF_t                            *marpaESLIFp                = marpaESLIFValuep->marpaESLIFp;
  genericStack_t                          *flatAlternativesStackp     = NULL;
  genericStack_t                          *flatAlternativeStackp      = NULL;
  char                                    *topasciis                  = NULL;
  char                                    *currentasciis              = NULL;
  char                                    *nextasciis                 = NULL;
  int                                     *arityip                    = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t  *prioritizedRhsAlternativep = NULL;
  genericStack_t                          *flatStackp                 = NULL;
  marpaESLIFValueResult_t                 *marpaESLIFValueResultp     = NULL;
  genericStack_t                          *alternativeStackp;
  genericStack_t                          *rhsAlternativeStackp;
  genericStack_t                          *adverbListItemStackp;
  marpaESLIF_bootstrap_rhs_alternative_t  *rhsAlternativep;
  int                                      priorityCounti;
  int                                      alternativesi;
  int                                      alternativei;
  marpaESLIF_bootstrap_alternative_t      *alternativep;
  marpaESLIF_symbol_t                     *prioritizedLhsp;
  marpaESLIF_symbol_t                     *prioritizedLhsAsRhsp;
  marpaESLIF_symbol_t                     *nextPrioritizedLhsp;
  marpaESLIF_symbol_t                     *nextPrioritizedLhsAsRhsp;
  marpaESLIF_symbol_t                     *rhsp;
  marpaESLIF_symbol_t                     *rhsAsLhsp;
  marpaESLIF_rule_t                       *rulep;
  int                                      priorityi;
  int                                      nextPriorityi;
  int                                      arityi;
  int                                      nrhsi;
  int                                      rhsi;
  short                                    rcb;
  char                                     tmps[1024];
  short                                    left_associationb;
  short                                    right_associationb;
  short                                    group_associationb;
  int                                      ranki;
  short                                    nullRanksHighb;
  marpaESLIF_bootstrap_utf_string_t       *namingp;
  marpaESLIF_action_t                     *actionp;
  marpaESLIF_action_t                      action;
  int                                      arityixi;
  int                                      i;
  char                                    *parameters;

  priorityCounti = GENERICSTACK_USED(alternativesStackp);
  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Processing %d '||' rules", funcs, GENERICSTACK_USED(alternativesStackp));
  if (priorityCounti <= 1) {
    /* No loosen operator: go to flat method */
    return _marpaESLIF_bootstrap_G1_action_priority_flat_ruleb(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, lhsp, alternativesStackp, "non-prioritized alternatives rule", parameterStackp);
  }

  /* Constant action */
  action.type    = MARPAESLIF_ACTION_TYPE_NAME;
  action.u.names = "::shift";

  /* Create a top-version of the LHS, using symbols not allowed from the external */
  /* Per-def lhsp is a meta symbol */
  topasciis = (char *) malloc(strlen(lhsp->u.metap->asciinames) + 3 /* "[0]" */ + 1 /* NUL byte */);
  if (MARPAESLIF_UNLIKELY(topasciis == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  strcpy(topasciis, lhsp->u.metap->asciinames);
  strcat(topasciis, "[0]");

  /* A symbol must appear once as a prioritized LHS in the whole grammar */
  if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, topasciis, 0 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, NULL /* lhsAsRhspp */) != NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "Symbol %s must appear once in the grammar as the LHS of a a prioritized rule", lhsp->u.metap->asciinames);
    goto err;
  }

  prioritizedLhsp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, topasciis, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &prioritizedLhsAsRhsp);
  if (MARPAESLIF_UNLIKELY(prioritizedLhsp == NULL)) {
    goto err;
  }

  /* Create the rule lhs := lhs[0] action => ::shift */
  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Creating rule %s ::= %s at grammar level %d", funcs, _marpaESLIF_symbol_shows(lhsp), _marpaESLIF_symbol_shows(prioritizedLhsAsRhsp), grammarp->leveli);
  rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                grammarp,
                                NULL, /* descEncodings */
                                NULL, /* descs */
                                0, /* descl */
                                lhsp->idi,
                                1, /* nrhsl */
                                &(prioritizedLhsAsRhsp->idi), /* rhsip */
                                -1, /* exceptioni */
                                0, /* ranki */
                                0, /* nullRanksHighb */
                                0, /* sequenceb */
                                -1, /* minimumi */
                                -1, /* separatori */
                                0, /* properb */
                                &action,
                                0, /* passthroughb */
                                0 /* hideseparatorb */,
                                NULL, /* skipbp */
                                parameterStackp);
  if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
    goto err;
  }

  /* We construct a new alternativesStackp as if the loosen operator was absent, as if the user would have writen the BNF the old way. */
  GENERICSTACK_NEW(flatAlternativesStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatAlternativesStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "flatAlternativesStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(flatAlternativeStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatAlternativeStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "flatAlternativeStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_PUSH_PTR(flatAlternativesStackp, flatAlternativeStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatAlternativesStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "flatAlternativesStackp push failure, %s", strerror(errno));
    goto err;
  }

  /* Create transition rules (remember, it is guaranteed that priorityCounti > 1 here */
  for (priorityi = 1; priorityi <= priorityCounti-1; priorityi++) {
    sprintf(tmps, "%d", priorityi - 1);
    if (currentasciis != NULL) {
      free(currentasciis);
    }
    currentasciis = (char *) malloc(strlen(lhsp->u.metap->asciinames) + 1 /* [ */ + strlen(tmps) + 1 /* ] */ + 1 /* NUL */);
    if (MARPAESLIF_UNLIKELY(currentasciis == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    strcpy(currentasciis, lhsp->u.metap->asciinames);
    strcat(currentasciis, "[");
    strcat(currentasciis, tmps);
    strcat(currentasciis, "]");
    prioritizedLhsp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, currentasciis, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &prioritizedLhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(prioritizedLhsp == NULL)) {
      goto err;
    }

    sprintf(tmps, "%d", priorityi);
    if (nextasciis != NULL) {
      free(nextasciis);
    }
    nextasciis = (char *) malloc(strlen(lhsp->u.metap->asciinames) + 1 /* [ */ + strlen(tmps) + 1 /* ] */ + 1 /* NUL */);
    if (MARPAESLIF_UNLIKELY(nextasciis == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    strcpy(nextasciis, lhsp->u.metap->asciinames);
    strcat(nextasciis, "[");
    strcat(nextasciis, tmps);
    strcat(nextasciis, "]");
    nextPrioritizedLhsp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, nextasciis, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, prioritizedLhsp, parameterStackp, &nextPrioritizedLhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(nextPrioritizedLhsp == NULL)) {
      goto err;
    }

    /* Create the transition rule lhs[priorityi-1] := lhs[priorityi] action => ::shift */
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Creating transition rule %s ::= %s at grammar level %d", _marpaESLIF_symbol_shows(prioritizedLhsp), _marpaESLIF_symbol_shows(nextPrioritizedLhsAsRhsp), grammarp->leveli);
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                  grammarp,
                                  NULL, /* descEncodings */
                                  NULL, /* descs */
                                  0, /* descl */
                                  prioritizedLhsp->idi,
                                  1, /* nrhsl */
                                  &(nextPrioritizedLhsAsRhsp->idi), /* rhsip */
                                  -1, /* exceptioni */
                                  0, /* ranki */
                                  0, /* nullRanksHighb */
                                  0, /* sequenceb */
                                  -1, /* minimumi */
                                  -1, /* separatori */
                                  0, /* properb */
                                  &action,
                                  0, /* passthroughb */
                                  0 /* hideseparatorb */,
                                  NULL, /* skipbp */
                                  parameterStackp);
    if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
      goto err;
    }
    GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
      goto err;
    }
  }

  /* Evaluate current priority of every alternative, change symbols, and push it in the flat version */
  for (alternativesi = 0; alternativesi < priorityCounti; alternativesi++) {
#ifndef MARPAESLIF_NTRACE
    /* Should never happen */
    if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(alternativesStackp, alternativesi))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp at indice %d is not PTR (got %s, value %d)", alternativesi, _marpaESLIF_genericStack_i_types(alternativesStackp, alternativesi), GENERICSTACKITEMTYPE(alternativesStackp, alternativesi));
      goto err;
    }
#endif

    priorityi = priorityCounti - (alternativesi + 1);

    /* Rework current LHS to be lhs[priorityi] */
    /* Will an "int" ever have more than 1022 digits ? */
    sprintf(tmps, "%d", priorityi);
    if (currentasciis != NULL) {
      free(currentasciis);
    }
    currentasciis = (char *) malloc(strlen(lhsp->u.metap->asciinames) + 1 /* [ */ + strlen(tmps) + 1 /* ] */ + 1 /* NUL */);
    if (MARPAESLIF_UNLIKELY(currentasciis == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    strcpy(currentasciis, lhsp->u.metap->asciinames);
    strcat(currentasciis, "[");
    strcat(currentasciis, tmps);
    strcat(currentasciis, "]");
    prioritizedLhsp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, currentasciis, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, lhsp, parameterStackp, &prioritizedLhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(prioritizedLhsp == NULL)) {
      goto err;
    }

    /* Rework next LHS to be lhs[nextPriorityi] */
    nextPriorityi = priorityi + 1;
    /* Original Marpa::R2 calculus is $next_priority = 0 if $next_priority >= $priority_count */
    /* And a comment says this is probably a misfeature that the author did not fix for backward */
    /* compatibility issues on a quite rare case. */
    if (nextPriorityi >= priorityCounti) {
      nextPriorityi = priorityi;
    }
    sprintf(tmps, "%d", nextPriorityi);
    if (nextasciis != NULL) {
      free(nextasciis);
    }
    nextasciis = (char *) malloc(strlen(lhsp->u.metap->asciinames) + 1 /* [ */ + strlen(tmps) + 1 /* ] */ + 1 /* NUL */);
    if (MARPAESLIF_UNLIKELY(nextasciis == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    strcpy(nextasciis, lhsp->u.metap->asciinames);
    strcat(nextasciis, "[");
    strcat(nextasciis, tmps);
    strcat(nextasciis, "]");
    nextPrioritizedLhsp = _marpaESLIF_bootstrap_check_metap(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, nextasciis, 1 /* createb */, 0 /* forcecreateb */, (parameterStackp != NULL) ? GENERICSTACK_USED(parameterStackp) : -1, 0 /* terminalb */, prioritizedLhsp, parameterStackp, &nextPrioritizedLhsAsRhsp);
    if (MARPAESLIF_UNLIKELY(nextPrioritizedLhsp == NULL)) {
      goto err;
    }

    /* Things separator by the | operator: this is a stack of <alternative> */
    alternativeStackp = GENERICSTACK_GET_PTR(alternativesStackp, alternativesi);
    for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
#ifndef MARPAESLIF_NTRACE
      /* Should never happen */
      if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(alternativeStackp, alternativei))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", alternativei, _marpaESLIF_genericStack_i_types(alternativeStackp, alternativei), GENERICSTACKITEMTYPE(alternativeStackp, alternativei));
        goto err;
      }
#endif
      alternativep = (marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);
      /* Alternatives is a stack of RHS followed by adverb items */
      alternativep->priorityi = priorityi;

      /* Look for arity */
      arityi = 0;
      if (arityip != NULL) {
        free(arityip);
      }
      rhsAlternativeStackp = alternativep->rhsAlternativeStackp;
      adverbListItemStackp = alternativep->adverbListItemStackp;
      /* As per the grammar, it is not possible that rhsAlternativeStackp is empty */
      nrhsi = GENERICSTACK_USED(rhsAlternativeStackp);
      arityip = (int *) malloc(nrhsi * sizeof(int));
      if (MARPAESLIF_UNLIKELY(arityip == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      /* Every occurence of LHS in the RHS list increases the arity */
      for (rhsi = 0; rhsi < nrhsi; rhsi++) {
#ifndef MARPAESLIF_NTRACE
        /* Should never happen */
        if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(rhsAlternativeStackp, rhsi))) {
          MARPAESLIF_ERRORF(marpaESLIFp, "rhsAlternativeStackp at indice %d is not PTR (got %s, value %d)", rhsi, _marpaESLIF_genericStack_i_types(rhsAlternativeStackp, rhsi), GENERICSTACKITEMTYPE(rhsAlternativeStackp, rhsi));
          goto err;
        }
#endif
        rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) GENERICSTACK_GET_PTR(rhsAlternativeStackp, rhsi);
        rhsAsLhsp = _marpaESLIF_bootstrap_check_rhsAlternativep(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep, 1 /* createb */, lhsp, parameterStackp, &rhsp);
        if (MARPAESLIF_UNLIKELY(rhsAsLhsp == NULL)) {
          goto err;
        }
        if (rhsAsLhsp == lhsp) {
          arityip[arityi++] = rhsi;
        }
      }

      /* Look to association */
      if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                                  "prioritized rule",
                                                                                  adverbListItemStackp,
                                                                                  &actionp,
                                                                                  &left_associationb,
                                                                                  &right_associationb,
                                                                                  &group_associationb,
                                                                                  NULL, /* separatorRhsPrimarypp */
                                                                                  NULL, /* properbp */
                                                                                  NULL, /* hideseparatorbp */
                                                                                  &ranki,
                                                                                  &nullRanksHighb,
                                                                                  NULL, /* priorityipp */
                                                                                  NULL, /* pauseip */
                                                                                  NULL, /* latmbp */
                                                                                  &namingp,
                                                                                  NULL, /* symbolactionpp */
                                                                                  NULL, /* eventInitializationpp */
                                                                                  NULL, /* ifactionpp */
                                                                                  NULL, /* regexactionpp */
                                                                                  NULL, /* eventactionpp */
                                                                                  NULL, /* defaultEncodingsp */
                                                                                  NULL /* fallbackEncodingsp */
                                                                                  ))) {
        goto err;
      }

      /* Associations are mutually exclusive */
      if (MARPAESLIF_UNLIKELY((left_associationb + right_associationb + group_associationb) > 1)) {
        MARPAESLIF_ERROR(marpaESLIFp, "assoc => left, assoc => right and assoc => group are mutually exclusive");
        goto err;
      }
      /* Default assocativity is left */
      if ((left_associationb + right_associationb + group_associationb) <= 0) {
        left_associationb = 1;
      }

      /* Rework the RHS list by replacing the symbols matching the LHS */
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "alternativesStackp[%d] alternativeStackp[%d] currentLeft=<%s> nextLeft=<%s> priorityi=%d nrhsi=%d arityi=%d assoc=%s", alternativesi, alternativei, currentasciis, nextasciis, priorityi, nrhsi, arityi, left_associationb ? "left" : (right_associationb ? "right" : (group_associationb ? "group" : "unknown")));

      if (arityi > 0) {
        if (MARPAESLIF_UNLIKELY((arityi == 1) && (nrhsi == 1))) {
          /* Something like Expression ::= Expression in a prioritized rule -; */
          MARPAESLIF_ERRORF(marpaESLIFp, "Unnecessary unit rule <%s> in priority rule", lhsp->u.metap->asciinames);
          goto err;
        }

        /* Do the association by reworking RHS's matching the LHS */
        for (arityixi = 0; arityixi < arityi; arityixi++) {
          rhsi = arityip[arityixi];
#ifndef MARPAESLIF_NTRACE
          /* Should never happen */
          if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(rhsAlternativeStackp, rhsi))) {
            MARPAESLIF_ERRORF(marpaESLIFp, "rhsAlternativeStackp at indice %d is not PTR (got %s, value %d)", rhsi, _marpaESLIF_genericStack_i_types(rhsAlternativeStackp, rhsi), GENERICSTACKITEMTYPE(rhsAlternativeStackp, rhsi));
            goto err;
          }
#endif
          rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) GENERICSTACK_GET_PTR(rhsAlternativeStackp, rhsi);
          _marpaESLIF_bootstrap_rhs_alternative_freev(prioritizedRhsAlternativep);
          prioritizedRhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *)  malloc(sizeof(marpaESLIF_bootstrap_rhs_alternative_t));
          if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep == NULL)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
            goto err;
          }
          prioritizedRhsAlternativep->symbolShallowp = NULL;
          prioritizedRhsAlternativep->type           = MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_RHS_PRIMARY;
          prioritizedRhsAlternativep->u.rhsPrimaryp  = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
          if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp == NULL)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
            goto err;
          }
          if (parameterStackp != NULL) {
            prioritizedRhsAlternativep->u.rhsPrimaryp->symbolShallowp         = NULL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->type                   = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_PARAMETERIZED_SYMBOL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp = (marpaESLIF_bootstrap_parameterized_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_parameterized_symbol_t));
            if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp == NULL)) {
              MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto err;
            }
            
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp        = NULL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = NULL;

            prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp = (marpaESLIF_bootstrap_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_t));
            if (prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto err;
            }

            if (left_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp->symbols = (arityixi == 0)            ? strdup(currentasciis) : strdup(nextasciis);
            } else if (right_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp->symbols = (arityixi == (arityi - 1)) ? strdup(currentasciis) : strdup(nextasciis);
            } else if (group_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp->symbols = strdup(topasciis);
            } else {
              /* Should never happen */
              MARPAESLIF_ERROR(marpaESLIFp, "No association !?");
              goto err;
            }
            if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->symbolp->symbols == NULL)) {
              MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
              goto err;
            }

            prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = &(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->_argumentStack);
            GENERICSTACK_INIT(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp);
            if (GENERICSTACK_ERROR(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp)) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp = NULL;
              MARPAESLIF_ERRORF(marpaESLIFp, "prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp initialization failure, %s", strerror(errno));
              goto err;
            }

            /* By definition a parameterStack is always composed by only identifiers */
            for (i = 0; i < GENERICSTACK_USED(parameterStackp); i++) {
              if (! GENERICSTACK_IS_PTR(parameterStackp, i)) {
                MARPAESLIF_ERRORF(marpaESLIFp, "parameterStackp at indice %d is not PTR (got %s, value %d)", i, _marpaESLIF_genericStack_i_types(parameterStackp, i), GENERICSTACKITEMTYPE(parameterStackp, i));
                goto err;
              }
              parameters = (char *) GENERICSTACK_GET_PTR(parameterStackp, i);
              if (parameters == NULL) {
                MARPAESLIF_ERRORF(marpaESLIFp, "parameter at indice %d is NULL", i);
                goto err;
              }

              /* In a parameterized rhs primary, every entry is also a stack */
              GENERICSTACK_NEW(flatStackp);
              if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatStackp))) {
                MARPAESLIF_ERRORF(marpaESLIFp, "flatStackp initialization failure, %s", strerror(errno));
                goto err;
              }

              marpaESLIFValueResultp = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
              if (marpaESLIFValueResultp == NULL) {
                MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
                goto err;
              }
      
              marpaESLIFValueResultp->contextp           = NULL;
              marpaESLIFValueResultp->representationp    = NULL;
              marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_ARRAY;
              marpaESLIFValueResultp->u.a.p              = parameters;
              marpaESLIFValueResultp->u.a.freeUserDatavp = NULL;
              marpaESLIFValueResultp->u.a.freeCallbackp  = NULL;
              marpaESLIFValueResultp->u.a.shallowb       = 1;
              marpaESLIFValueResultp->u.a.sizel          = strlen(parameters);

              GENERICSTACK_PUSH_PTR(flatStackp, marpaESLIFValueResultp);
              if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatStackp))) {
                MARPAESLIF_ERRORF(marpaESLIFp, "flatStackp push failure, %s", strerror(errno));
                goto err;
              }

              marpaESLIFValueResultp = NULL; /* marpaESLIFValueResultp is in flatStackp */

              GENERICSTACK_PUSH_PTR(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp, flatStackp);
              if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(prioritizedRhsAlternativep->u.rhsPrimaryp->u.parameterizedSymbolp->argumentStackp))) {
                MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryp->u.parameterizedSymbolp->argumentStackp push failure, %s", strerror(errno));
                goto err;
              }
              flatStackp = NULL; /* flatStackp is in rhsPrimaryp->u.parameterizedSymbolp->argumentStackp */
            }
          } else {
            prioritizedRhsAlternativep->u.rhsPrimaryp->symbolShallowp  = NULL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->type            = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
            if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp == NULL)) {
              MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto err;
            }
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->type      = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL;
            prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp = (marpaESLIF_bootstrap_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_t));
            if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp == NULL)) {
              MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto err;
            }

            if (left_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp->symbols = (arityixi == 0)            ? strdup(currentasciis) : strdup(nextasciis);
            } else if (right_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp->symbols = (arityixi == (arityi - 1)) ? strdup(currentasciis) : strdup(nextasciis);
            } else if (group_associationb) {
              prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp->symbols = strdup(topasciis);
            } else {
              /* Should never happen */
              MARPAESLIF_ERROR(marpaESLIFp, "No association !?");
              goto err;
            }
            if (MARPAESLIF_UNLIKELY(prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp->symbols == NULL)) {
              MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
              goto err;
            }
          }

          /* All is well, we can replace this rhs alternative with the new one */
          GENERICSTACK_SET_PTR(rhsAlternativeStackp, prioritizedRhsAlternativep, rhsi);
          if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(rhsAlternativeStackp))) {
            MARPAESLIF_ERRORF(marpaESLIFp, "rhsAlternativeStackp set failure, %s", strerror(errno));
            goto err;
          }
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "alternativesStackp[%d] alternativeStackp[%d] ... LHS is %s, RHS[%d] is now %s", alternativesi, alternativei, currentasciis, rhsi, prioritizedRhsAlternativep->u.rhsPrimaryp->u.singleSymbolp->u.symbolp->symbols);
          prioritizedRhsAlternativep = NULL; /* prioritizedRhsAlternativep is in rhsAlternativeStackp */
          /* We can forget the old one */
          _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep);
        }
      }

      GENERICSTACK_PUSH_PTR(flatAlternativeStackp, alternativep);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(flatAlternativeStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "flatAlternativeStackp push failure, %s", strerror(errno));
        goto err;
      }

      /* We force the LHS for EVERY alternative */
      alternativep->forcedLhsp = prioritizedLhsp;
    }
  }

  /* Create the prioritized alternatives */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_G1_action_priority_flat_ruleb(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, NULL /* lhsp */, flatAlternativesStackp, "prioritized alternatives", parameterStackp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (arityip != NULL) {
    free(arityip);
  }
  if (currentasciis != NULL) {
    free(currentasciis);
  }
  if (nextasciis != NULL) {
    free(nextasciis);
  }
  if (topasciis != NULL) {
    free(topasciis);
  }
  _marpaESLIF_bootstrap_rhs_alternative_freev(prioritizedRhsAlternativep);
  _marpaESLIF_bootstrap_flatStack_freev(flatStackp);
  if (marpaESLIFValueResultp != NULL) {
    free(marpaESLIFValueResultp);
  }
  GENERICSTACK_FREE(flatAlternativesStackp);
  GENERICSTACK_FREE(flatAlternativeStackp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_priority_flat_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_grammar_t *grammarp, marpaESLIF_symbol_t *lhsp, genericStack_t *alternativesStackp, char *contexts, genericStack_t *parameterStackp)
/*****************************************************************************/
{
  /* <priority rule> ::= lhs <op declare> priorities */
  /* This method is called when there is no more than one priority. It is ignoring the notion of priority. */
  static const char                       *funcs             = "_marpaESLIF_bootstrap_G1_action_priority_flat_ruleb";
  marpaESLIF_t                            *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_rule_t                       *rulep             = NULL;
  int                                     *rhsip             = NULL;
  short                                   *skipbp            = NULL;
  marpaESLIF_symbol_t                    **rhsp              = NULL;
  short                                    have_skipb;
  int                                      nrhsi;
  genericStack_t                          *alternativeStackp;
  genericStack_t                          *rhsAlternativeStackp;
  genericStack_t                          *adverbListItemStackp;
  int                                      alternativesi;
  int                                      alternativei;
  int                                      rhsAlternativei;
  marpaESLIF_bootstrap_alternative_t      *alternativep;
  marpaESLIF_bootstrap_rhs_alternative_t  *rhsAlternativep;
  short                                    rcb;
  short                                    left_associationb;
  short                                    right_associationb;
  short                                    group_associationb;
  int                                      ranki;
  short                                    nullRanksHighb;
  marpaESLIF_bootstrap_utf_string_t       *namingp;
  marpaESLIF_action_t                     *actionp;

  /* Priorities (things separated by the || operator) are IGNORED in this method */
  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Processing %d '|' rules (context: %s)", funcs, GENERICSTACK_USED(alternativesStackp), contexts);
  for (alternativesi = 0; alternativesi < GENERICSTACK_USED(alternativesStackp); alternativesi++) {
    MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Processing <alternatives> No %d", funcs, alternativesi);
#ifndef MARPAESLIF_NTRACE
    /* Should never happen */
    if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(alternativesStackp, alternativesi))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp at indice %d is not PTR (got %s, value %d)", alternativesi, _marpaESLIF_genericStack_i_types(alternativesStackp, alternativesi), GENERICSTACKITEMTYPE(alternativesStackp, alternativesi));
      goto err;
    }
#endif
    /* Alternatives (things separator by the | operator) is a stack of alternative */
    alternativeStackp = GENERICSTACK_GET_PTR(alternativesStackp, alternativesi);
    for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
      MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Processing <alternatives> No %d: Processing <alternative> No %d", funcs, alternativesi, alternativei);
#ifndef MARPAESLIF_NTRACE
      /* Should never happen */
      if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(alternativeStackp, alternativei))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", alternativei, _marpaESLIF_genericStack_i_types(alternativeStackp, alternativei), GENERICSTACKITEMTYPE(alternativeStackp, alternativei));
        goto err;
      }
#endif
      alternativep = (marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);
      /* Alternatives is a stack of RHS followed by adverb items */
      rhsAlternativeStackp = alternativep->rhsAlternativeStackp;
      adverbListItemStackp = alternativep->adverbListItemStackp;

      nrhsi = GENERICSTACK_USED(rhsAlternativeStackp);
      rhsip = (int *) malloc(nrhsi * sizeof(int));
      if (MARPAESLIF_UNLIKELY(rhsip == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      rhsp = (marpaESLIF_symbol_t **) malloc(nrhsi * sizeof(marpaESLIF_symbol_t *));
      if (MARPAESLIF_UNLIKELY(rhsp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      skipbp = (short *) malloc(nrhsi * sizeof(short));
      if (MARPAESLIF_UNLIKELY(skipbp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      /* Analyse alternative list */
      MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Processing <alternatives> No %d: Processing <alternative> No %d: Processing %d RHSs", funcs, alternativesi, alternativei, nrhsi);
      have_skipb = 0;
      for (rhsAlternativei = 0; rhsAlternativei < nrhsi; rhsAlternativei++) {
#ifndef MARPAESLIF_NTRACE
        /* Should never happen */
        if (MARPAESLIF_UNLIKELY(! GENERICSTACK_IS_PTR(rhsAlternativeStackp, rhsAlternativei))) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", rhsAlternativei, _marpaESLIF_genericStack_i_types(rhsAlternativeStackp, rhsAlternativei), GENERICSTACKITEMTYPE(rhsAlternativeStackp, rhsAlternativei));
          goto err;
        }
#endif
        rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) GENERICSTACK_GET_PTR(rhsAlternativeStackp, rhsAlternativei);
        if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsAlternativep(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsAlternativep, 1 /* createb */, lhsp, parameterStackp, &(rhsp[rhsAlternativei])) == NULL)) {
          goto err;
        }
        rhsip[rhsAlternativei] = rhsp[rhsAlternativei]->idi;
        switch (rhsAlternativep->type) {
        case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_PRIORITIES:
          if ((skipbp[rhsAlternativei] = rhsAlternativep->u.priorities.skipb) != 0) {
            have_skipb = 1;
          }
          break;
        case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_EXCEPTION:
          if ((skipbp[rhsAlternativei] = rhsAlternativep->u.exception.skipb) != 0) {
            have_skipb = 1;
          }
          break;
        case MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_QUANTIFIED:
          if ((skipbp[rhsAlternativei] = rhsAlternativep->u.quantified.skipb) != 0) {
            have_skipb = 1;
          }
          break;
        default:
          skipbp[rhsAlternativei] = 0;
          break;
        }
      }

      /* Analyse adverb list items - take care this is nullable and we propagate NULL if it is the case */
      /* Same arguments than in the loose version, except that we will ignore association adverbs */
      left_associationb  = 0;
      right_associationb = 0;
      group_associationb = 0;
      ranki              = 0;
      nullRanksHighb     = 0;
      namingp            = NULL;
      actionp            = NULL;
      if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                                  contexts,
                                                                                  adverbListItemStackp,
                                                                                  &actionp,
                                                                                  &left_associationb,
                                                                                  &right_associationb,
                                                                                  &group_associationb,
                                                                                  NULL, /* separatorRhsPrimarypp */
                                                                                  NULL, /* properbp */
                                                                                  NULL, /* hideseparatorbp */
                                                                                  &ranki,
                                                                                  &nullRanksHighb,
                                                                                  NULL, /* priorityipp */
                                                                                  NULL, /* pauseip */
                                                                                  NULL, /* latmbp */
                                                                                  &namingp,
                                                                                  NULL, /* symbolactionpp */
                                                                                  NULL, /* eventInitializationpp */
                                                                                  NULL, /* ifactionpp */
                                                                                  NULL, /* regexactionpp */
                                                                                  NULL, /* eventactionpp */
                                                                                  NULL, /* defaultEncodingsp */
                                                                                  NULL /* fallbackEncodingsp */
                                                                                  ))) {
        goto err;
      }
/* #ifndef MARPAESLIF_NTRACE */
      MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Creating rule %s at grammar level %d", funcs, (alternativep->forcedLhsp != NULL) ? _marpaESLIF_symbol_shows(alternativep->forcedLhsp) : _marpaESLIF_symbol_shows(lhsp), grammarp->leveli);
      MARPAESLIF_NOTICEF(marpaESLIFp, "%s: ... LHS     : %d %s", funcs, (alternativep->forcedLhsp != NULL) ? alternativep->forcedLhsp->idi : lhsp->idi, (alternativep->forcedLhsp != NULL) ? _marpaESLIF_symbol_shows(alternativep->forcedLhsp) : _marpaESLIF_symbol_shows(lhsp));
      for (rhsAlternativei = 0; rhsAlternativei < nrhsi; rhsAlternativei++) {
        MARPAESLIF_NOTICEF(marpaESLIFp, "%s: ... RHS[%3d]: %d %s", funcs, rhsAlternativei, rhsp[rhsAlternativei]->idi, _marpaESLIF_symbol_shows(rhsp[rhsAlternativei]));
      }
/* #endif */
      /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
      rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                    grammarp,
                                    (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                    (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                    (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                    (alternativep->forcedLhsp != NULL) ? alternativep->forcedLhsp->idi : lhsp->idi,
                                    (size_t) nrhsi,
                                    rhsip,
                                    -1, /* exceptioni */
                                    ranki,
                                    nullRanksHighb,
                                    0, /* sequenceb */
                                    -1, /* minimumi */
                                    -1, /* separatori */
                                    0, /* properb */
                                    actionp,
                                    0, /* passthroughb */
                                    0 /* hideseparatorb */,
                                    have_skipb ? skipbp : NULL,
                                    parameterStackp);
      if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
        goto err;
      }
      free(rhsip);
      rhsip = NULL;
      free(rhsp);
      rhsp = NULL;
      free(skipbp);
      skipbp = NULL;
      GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
        goto err;
      }
      /* Push is ok: rulep is in grammarp->ruleStackp */
      rulep = NULL;
    }
  }
  /* We set nothing in the stack, our parent will return ::undef up to the top-level */
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (rhsip != NULL) {
    free(rhsip);
  }
  if (rhsp != NULL) {
    free(rhsp);
  }
  if (skipbp != NULL) {
    free(skipbp);
  }
  _marpaESLIF_rule_freev(rulep);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_priority_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <priority rule> ::= <lhs> <op declare> priorities */
  /* **** The result will be undef **** */
  /* **** We work on userDatavp, that is a marpaESLIFGrammarp **** */
  /* **** In case of failure, the caller that is marpaESLIFGrammar_newp() will call a free on this marpaESLIFGrammarp **** */
  static const char          *funcs              = "_marpaESLIF_bootstrap_G1_action_priority_ruleb";
  marpaESLIFGrammar_t        *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_lhs_t *bootstrapLhsp;
  int                         leveli;
  genericStack_t             *alternativesStackp;
  marpaESLIF_grammar_t       *grammarp;
  marpaESLIF_symbol_t        *lhsp;
  short                       rcb;

  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, bootstrapLhsp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, alternativesStackp);

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the lhs exist */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_G1_action_priority_loosen_ruleb(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, lhsp, alternativesStackp, bootstrapLhsp->parameterStackp))) {
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
static short _marpaESLIF_bootstrap_G1_action_single_symbol_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <single symbol> ::= <symbol> */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp = NULL;
  marpaESLIF_t                         *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_symbol_t        *symbolp       = NULL;
  short                                 rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, symbolp);
  /* It is a non-sense to have a null symbolp */
  if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolp at indice %d is NULL", argni);
    goto err;
  }

  singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
  if (MARPAESLIF_UNLIKELY(singleSymbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type      = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_SYMBOL;
  singleSymbolp->u.symbolp = symbolp;
  symbolp = NULL; /* symbolp is in singleSymbolp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL, singleSymbolp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_symbol_freev(symbolp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_single_symbol_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <single symbol> ::= <terminal> */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp = NULL;
  marpaESLIF_t                         *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_terminal_t      *terminalp     = NULL;
  short                                 rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, terminalp);
  /* It is a non-sense to have a null terminalp */
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "terminalp at indice %d is NULL", argni);
    goto err;
  }

  singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
  if (MARPAESLIF_UNLIKELY(singleSymbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type        = MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_TERMINAL;
  singleSymbolp->u.terminalp = terminalp;
  terminalp = NULL; /* terminalp is in singleSymbolp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL, singleSymbolp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_terminal_freev(terminalp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <terminal> ::= <character class> */
  /* <character class> is a lexeme. */
  marpaESLIF_bootstrap_terminal_t *terminalp   = NULL;
  marpaESLIF_t                    *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  void                            *bytep       = NULL;
  short                            shallowb    = 0;
  size_t                           bytel;
  short                            rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i, bytep, bytel, shallowb);

  terminalp = (marpaESLIF_bootstrap_terminal_t *) malloc(sizeof(marpaESLIF_bootstrap_terminal_t));
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  terminalp->type              = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_NA;
  terminalp->u.characterClassp = _marpaESLIF_bootstrap_characterClass_to_stringb(marpaESLIFValuep, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(terminalp->u.characterClassp == NULL)) {
    goto err;
  }
  terminalp->type              = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_CHARACTER_CLASS;

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_TERMINAL, terminalp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_terminal_freev(terminalp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
 return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <terminal> ::= <regular expression> */
  /* <regular expression> is a lexeme. */
  marpaESLIF_bootstrap_terminal_t *terminalp   = NULL;
  marpaESLIF_t                    *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  void                            *bytep       = NULL;
  short                            shallowb    = 0;
  size_t                           bytel;
  short                            rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i, bytep, bytel, shallowb);

  terminalp = (marpaESLIF_bootstrap_terminal_t *) malloc(sizeof(marpaESLIF_bootstrap_terminal_t));
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  terminalp->type                 = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_NA;
  terminalp->u.regularExpressionp = _marpaESLIF_bootstrap_regex_to_stringb(marpaESLIFValuep, bytep, bytel);
  if (MARPAESLIF_UNLIKELY(terminalp->u.characterClassp == NULL)) {
    goto err;
  }
  terminalp->type                 = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_REGULAR_EXPRESSION;

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_TERMINAL, terminalp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_terminal_freev(terminalp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
 return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <terminal> ::= <quoted string> */
  marpaESLIF_bootstrap_terminal_t  *terminalp             = NULL;
  marpaESLIF_t                     *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIFRecognizer_t           *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  char                             *modifiers             = NULL;
  void                             *bytep                 = NULL;
  size_t                            bytel;
  short                             shallowb              = 0;
  char                             *tmps;
  marpaESLIFGrammar_t               marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t           marpaESLIFValueResult;
  size_t                            sizel;
  marpaESLIF_matcher_value_t        rci;
  short                             rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* action is the result of ::transfer, i.e. a lexeme in any case  */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i, bytep, bytel, shallowb);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY((bytep == NULL) || (bytel <= 0))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned {%p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  /* Duplicate bytep if it is shallow */
  if (shallowb) {
    tmps = (char *) malloc(bytel + 1);
    if (MARPAESLIF_UNLIKELY(tmps == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(tmps, bytep, bytel);
    tmps[bytel] = '\0';
    bytep = tmps;
    shallowb = 0;
  }
  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar,
                                                     NULL, /* marpaESLIFRecognizerOptionp */
                                                     0, /* discardb - no effect anway because we are in fake mode */
                                                     1, /* noEventb - no effect anway because we are in fake mode */
                                                     0, /* silentb */
                                                     NULL, /* marpaESLIFRecognizerParentp */
                                                     1, /* fakeb */
                                                     0, /* wantedStartCompletionsi */
                                                     1, /* A grammar is always transformed to valid UTF-8 before being parsed */
                                                     1 /* grammmarIsOnStackb */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp == NULL)) {
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(! _marpaESLIFRecognizer_terminal_matcherb(marpaESLIFRecognizerp,
                                                                    marpaESLIFRecognizerp->marpaESLIF_streamp,
                                                                    marpaESLIFp->stringModifiersp,
                                                                    bytep,
                                                                    bytel,
                                                                    1, /* eofb */
                                                                    &rci,
                                                                    &marpaESLIFValueResult,
                                                                    NULL /* matchedLengthlp */))) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a character class it is something like ":xxxxx" */
#ifndef MARPAESLIF_NTRACE
    /* Paranoid test */
    if (MARPAESLIF_UNLIKELY(marpaESLIFValueResult.u.a.sizel <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
#endif
    /* We want to maintain marpaESLIFValueResult lifetime, so need to unshallow the result of the INTERNAL method _marpaESLIFRecognizer_terminal_matcherb() if necessary */
    if (marpaESLIFValueResult.u.a.shallowb) {
      modifiers = (char *) malloc(marpaESLIFValueResult.u.a.sizel + 1);
      if (MARPAESLIF_UNLIKELY(modifiers == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(modifiers, marpaESLIFValueResult.u.a.p, marpaESLIFValueResult.u.a.sizel);
      modifiers[marpaESLIFValueResult.u.a.sizel] = '\0';
    } else {
      modifiers = (char *) marpaESLIFValueResult.u.a.p;
    }
    sizel = marpaESLIFValueResult.u.a.sizel;
  } else {
    /* Because we use this value just below */
    sizel = 0;
  }

  /* We leave the quotes because terminal_newp(), in case of a STRING, removes the surrounding characters. */
  /* Remember that a quoted string is a regexp with enforced unicode mode. Therefore the match is guaranteed to */
  /* have been done on a buffer always pre-converted to UTF-8, regardless of the original encoding of the input. */

  /* Make that a single symbol structure */
  terminalp = (marpaESLIF_bootstrap_terminal_t *) malloc(sizeof(marpaESLIF_bootstrap_terminal_t));
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  terminalp->type = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_NA;
  terminalp->u.stringp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (MARPAESLIF_UNLIKELY(terminalp->u.stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  terminalp->type                 = MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE_QUOTED_STRING;
  terminalp->u.stringp->modifiers = modifiers;
  terminalp->u.stringp->bytep     = bytep;
  terminalp->u.stringp->bytel     = bytel;
  modifiers = NULL; /* modifiers is in singleSymbolp */
  bytep = NULL; /* bytep is in singleSymbolp */
  if (sizel > 0) {
    terminalp->u.stringp->bytel -= (sizel + 1);  /* ":xxxx" */
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_TERMINAL, terminalp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_terminal_freev(terminalp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  if (modifiers != NULL) {
    free(modifiers);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_4b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_terminal_pseudob(userDatavp, marpaESLIFValuep, MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOF, resulti);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_terminal_pseudob(userDatavp, marpaESLIFValuep, MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__EOL, resulti);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_6b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_terminal_pseudob(userDatavp, marpaESLIFValuep, MARPAESLIF_BOOTSTRAP_TERMINAL_TYPE__SOL, resulti);
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_terminal_pseudob(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_bootstrap_terminal_type_t type, int resulti)
/*****************************************************************************/
{
  marpaESLIF_bootstrap_terminal_t  *terminalp   = NULL;
  marpaESLIF_t                     *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short                             rcb;

  terminalp = (marpaESLIF_bootstrap_terminal_t *) malloc(sizeof(marpaESLIF_bootstrap_terminal_t));
  if (MARPAESLIF_UNLIKELY(terminalp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  terminalp->type = type;

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_TERMINAL, terminalp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_terminal_freev(terminalp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <symbol> ::= <symbol name> */
  /* <symbol name> is a lexeme. */
  marpaESLIF_bootstrap_symbol_t *symbolp = NULL;
  marpaESLIF_t                  *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                          *symbols = NULL;
  short                          rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* symbols is an ASCII string that we pushed to a PTR that we own */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i, symbols);

  symbolp = (marpaESLIF_bootstrap_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_t));
  if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  symbolp->symbols = symbols;
  symbols = NULL; /* symbols is in symbolp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL, symbolp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_symbol_freev(symbolp);
  rcb = 0;

 done:
  if (symbols != NULL) {
    free(symbols);
  }
 return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_grammar_reference_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <grammar reference> ::= <quoted string literal> */
  marpaESLIF_t                             *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  void                                     *bytep             = NULL;
  size_t                                    bytel;
  short                                     shallowb          = 0;
  marpaESLIF_bootstrap_utf_string_t        *quotedStringp     = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i, bytep, bytel, shallowb);
  quotedStringp = _marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValuep, bytep, bytel, 0 /* allowEmptyStringb */);
  if (MARPAESLIF_UNLIKELY(quotedStringp == NULL)) {
    goto err;
  }

  grammarReferencep = (marpaESLIF_bootstrap_grammar_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_grammar_reference_t));
  if (MARPAESLIF_UNLIKELY(grammarReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type            = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING;
  grammarReferencep->u.quotedStringp = quotedStringp;
  quotedStringp = NULL; /* it is now in grammarReferencep */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE, grammarReferencep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  _marpaESLIF_bootstrap_utf_string_freev(quotedStringp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_grammar_reference_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <grammar reference> ::= <signed integer> */
  marpaESLIF_t                             *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  char                                     *signedIntegers;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, arg0i, signedIntegers);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(signedIntegers == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "signedIntegers at indice %d is NULL", arg0i);
    goto err;
  }

  grammarReferencep = (marpaESLIF_bootstrap_grammar_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_grammar_reference_t));
  if (MARPAESLIF_UNLIKELY(grammarReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type             = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER;
  grammarReferencep->u.signedIntegeri = atoi(signedIntegers);

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE, grammarReferencep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_grammar_reference_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <grammar reference> ::= '=' <unsigned integer> */
  marpaESLIF_t                             *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  char                                     *unsignedIntegers;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, unsignedIntegers);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(unsignedIntegers == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "unsignedIntegers at indice %d is NULL", arg0i);
    goto err;
  }

  grammarReferencep = (marpaESLIF_bootstrap_grammar_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_grammar_reference_t));
  if (MARPAESLIF_UNLIKELY(grammarReferencep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type               = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_UNSIGNED_INTEGER;
  grammarReferencep->u.unsignedIntegeri = (unsigned int) atoi(unsignedIntegers);

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE, grammarReferencep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <inaccessible treatment> ::= 'warn' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_INACESSIBLE_TREATMENT, MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_WARN);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <inaccessible treatment> ::= 'ok' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_INACESSIBLE_TREATMENT, MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_OK);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_inaccessible_treatment_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <inaccessible treatment> ::= 'fatal' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_INACESSIBLE_TREATMENT, MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_FATAL);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_inaccessible_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <inaccessible statement> ::= 'inaccessible' 'is' <inaccessible treatment> 'by' 'default' */
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t        *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short                inaccessibleTreatmentb;
  short                rcb;

  MARPAESLIF_BOOTSTRAP_GET_SHORT(marpaESLIFValuep, arg0i+2, inaccessibleTreatmentb);

  switch (inaccessibleTreatmentb) {
  case MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_WARN:
    marpaESLIFGrammarp->warningIsErrorb = 0;
    marpaESLIFGrammarp->warningIsIgnoredb = 0;
    break;
  case MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_OK:
    marpaESLIFGrammarp->warningIsErrorb = 0;
    marpaESLIFGrammarp->warningIsIgnoredb = 1;
    break;
  case MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_FATAL:
    marpaESLIFGrammarp->warningIsErrorb = 1;
    marpaESLIFGrammarp->warningIsIgnoredb = 0;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported inaccessible treatment value %d", (int) inaccessibleTreatmentb);
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
static short _marpaESLIF_bootstrap_G1_action_on_or_off_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <on or off>  ::= 'on' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ON_OR_OFF, MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_ON);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_on_or_off_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <on or off>  ::= 'off' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ON_OR_OFF, MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_OFF);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_autorank_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <autorank statement> ::= 'autorank' 'is' <on or off> 'by' 'default' */
  marpaESLIFGrammar_t                   *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                          *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_on_or_off_type_t  onOrOffb;
  short                                  rcb;

  MARPAESLIF_BOOTSTRAP_GET_SHORT(marpaESLIFValuep, arg0i+2, onOrOffb);

  switch (onOrOffb) {
  case MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_ON:
    marpaESLIFGrammarp->autorankb = 1;
    break;
  case MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_OFF:
    marpaESLIFGrammarp->autorankb = 0;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported on or off value %d", (int) onOrOffb);
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
static short _marpaESLIF_bootstrap_G1_action_quantifier_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* quantifier ::= '*' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_QUANTIFIER, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_quantifier_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* quantifier ::= '+' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_QUANTIFIER, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_quantified_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <quantified rule> ::= <lhs> <op declare> <rhs primary> quantifier <adverb list> */
  static const char                    *funcs                = "_marpaESLIF_bootstrap_G1_action_quantified_ruleb";
  marpaESLIFGrammar_t                  *marpaESLIFGrammarp   = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                         *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_rule_t                    *rulep                = NULL;
  genericStack_t                       *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_lhs_t           *bootstrapLhsp;
  int                                   leveli;
  marpaESLIF_bootstrap_rhs_primary_t   *rhsPrimaryp;
  int                                   minimumi;
  short                                 undefb;
  marpaESLIF_symbol_t                  *lhsp;
  marpaESLIF_bootstrap_rhs_primary_t   *separatorRhsPrimaryp;
  marpaESLIF_symbol_t                  *rhsp;
  marpaESLIF_symbol_t                  *separatorp;
  marpaESLIF_grammar_t                 *grammarp;
  short                                 rcb;
  marpaESLIF_action_t                  *actionp = NULL;
  int                                   ranki = 0;
  short                                 nullRanksHighb = 0;
  short                                 properb = 0;
  short                                 hideseparatorb = 0;
  marpaESLIF_bootstrap_utf_string_t    *namingp;

  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, bootstrapLhsp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, rhsPrimaryp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+3, minimumi);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }
 
  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the lhs */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* Check the rhs primary */
  if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, &rhsp) == NULL)) {
    goto err;
  }

  /* Check the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              "quantified rule",
                                                                              adverbListItemStackp,
                                                                              &actionp,
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              &separatorRhsPrimaryp,
                                                                              &properb,
                                                                              &hideseparatorb,
                                                                              &ranki,
                                                                              &nullRanksHighb,
                                                                              NULL, /* priorityipp */
                                                                              NULL, /* pauseip */
                                                                              NULL, /* latmbp */
                                                                              &namingp,
                                                                              NULL, /* symbolactionpp */
                                                                              NULL, /* eventInitializationpp */
                                                                              NULL, /* ifactionpp */
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  if (separatorRhsPrimaryp != NULL) {
    /* Check the separator */
    separatorp = _marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, separatorRhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, NULL /* lhsAsRhspp */);
    if (MARPAESLIF_UNLIKELY(separatorp == NULL)) {
      goto err;
    }
  } else {
    separatorp = NULL;
  }

/* #ifndef MARPAESLIF_NTRACE */
  if (separatorp != NULL) {
    MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Creating rule %s ::= %s%s ranki=>%d separator=>%s proper=>%d hide-separator=>%d null-ranking=>%s at grammar level %d", funcs, _marpaESLIF_symbol_shows(lhsp), _marpaESLIF_symbol_shows(rhsp), minimumi ? "+" : "*", ranki, _marpaESLIF_symbol_shows(separatorp), (int) properb, (int) hideseparatorb, nullRanksHighb ? "high" : "low", grammarp->leveli);
  } else {
    MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Creating rule %s ::= %s%s ranki=>%d null-ranking=>%s at grammar level %d", funcs, _marpaESLIF_symbol_shows(lhsp), _marpaESLIF_symbol_shows(rhsp), minimumi ? "+" : "*", ranki, nullRanksHighb ? "high" : "low", grammarp->leveli);
  }
/* #endif */
  /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
  rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                grammarp,
                                (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                lhsp->idi,
                                1, /* nrhsl */
                                &(rhsp->idi), /* rhsip */
                                -1, /* exceptioni */
                                ranki,
                                nullRanksHighb,
                                1, /* sequenceb */
                                minimumi,
                                (separatorp != NULL) ? separatorp->idi : -1, /* separatori */
                                properb,
                                actionp,
                                0, /* passthroughb */
                                hideseparatorb,
                                NULL, /* skipbp */
                                bootstrapLhsp->parameterStackp);
  if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
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
static short _marpaESLIF_bootstrap_G1_action_start_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <start rule>  ::= ':start' <op declare> <rhs primary> */
  static const char                  *funcs              = "_marpaESLIF_bootstrap_G1_action_start_ruleb";
  marpaESLIFGrammar_t                *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                       *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  int                                 leveli;
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp;
  marpaESLIF_grammar_t               *grammarp;
  marpaESLIF_symbol_t                *symbolp;
  short                               rcb;

  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, rhsPrimaryp);

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the symbol - note that :start is never parameterized */
  symbolp = _marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, NULL /* lhsAhsRhspp */);
  if (MARPAESLIF_UNLIKELY(symbolp == NULL)) {
    goto err;
  }

  /* Make it the start symbol */
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Marking meta symbol %s in grammar level %d as start symbol", _marpaESLIF_symbol_shows(symbolp), grammarp->leveli);
  symbolp->startb = 1;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_desc_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <desc rule> ::= ':desc' <op declare> <quoted string literal> */
  static const char                 *funcs              = "_marpaESLIF_bootstrap_G1_action_desc_ruleb";
  marpaESLIFGrammar_t               *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                      *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  int                                leveli;
  void                              *bytep             = NULL;
  size_t                             bytel;
  short                              shallowb          = 0;
  marpaESLIF_bootstrap_utf_string_t *quotedStringp     = NULL;
  short                 rcb;
  marpaESLIF_grammar_t *grammarp;

  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i+2, bytep, bytel, shallowb);
  quotedStringp = _marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValuep, bytep, bytel, 0 /* allowEmptyStringb */);
  if (MARPAESLIF_UNLIKELY(quotedStringp == NULL)) {
    goto err;
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  _marpaESLIF_string_freev(grammarp->descp, 0 /* onStackb */);
  /* Why hardcoded to UTF-8 ? Because a quote string is implemented as a regexp in unicode mode. */
  /* Therefore it is guaranteed that the match was done on UTF-8 bytes; regardless of the encoding */
  /* of the original input. */
  grammarp->descp = _marpaESLIF_string_newp(marpaESLIFp, (char *) MARPAESLIF_UTF8_STRING, quotedStringp->bytep, quotedStringp->bytel);
  if (MARPAESLIF_UNLIKELY(grammarp->descp == NULL)) {
    goto err;
  }
  grammarp->descautob = 0;

  /* Overwrite grammar start */
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Grammar level %d description set to %s", grammarp->leveli, grammarp->descp->asciis);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  _marpaESLIF_bootstrap_utf_string_freev(quotedStringp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_empty_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <empty rule> ::= <lhs> <op declare> <adverb list> */
  static const char                 *funcs              = "_marpaESLIF_bootstrap_G1_action_empty_ruleb";
  marpaESLIFGrammar_t               *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                      *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_rule_t                 *rulep              = NULL;
  marpaESLIF_bootstrap_lhs_t        *bootstrapLhsp;
  int                                leveli;
  genericStack_t                    *adverbListItemStackp = NULL;
  marpaESLIF_grammar_t              *grammarp;
  marpaESLIF_symbol_t               *lhsp;
  short                              undefb;
  marpaESLIF_action_t               *actionp;
  int                                ranki;
  short                              nullRanksHighb;
  marpaESLIF_bootstrap_utf_string_t *namingp;
  short                              rcb;

  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, bootstrapLhsp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the lhs exist */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* Unpack the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              "empty rule",
                                                                              adverbListItemStackp,
                                                                              &actionp,
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              &ranki,
                                                                              &nullRanksHighb,
                                                                              NULL, /* priorityipp */
                                                                              NULL, /* pauseip */
                                                                              NULL, /* latmbp */
                                                                              &namingp,
                                                                              NULL, /* symbolactionpp */
                                                                              NULL, /* eventInitializationpp */
                                                                              NULL, /* ifactionpp */
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  /* Create the rule */
  /* If there is a name description, then it is UTF-8 compatible (<standard name> or <quoted name>) */
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Creating empty rule %s at grammar level %d", _marpaESLIF_symbol_shows(lhsp), grammarp->leveli);
  rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                grammarp,
                                (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                lhsp->idi,
                                0, /* nrhsl */
                                NULL, /* rhsip */
                                -1, /* exceptioni */
                                ranki,
                                nullRanksHighb,
                                0, /* sequenceb */
                                -1, /* minimumi */
                                -1, /* separatori */
                                0, /* properb */
                                actionp,
                                0, /* passthroughb */
                                0 /* hideseparatorb */,
                                NULL, /* skipbp */
                                bootstrapLhsp->parameterStackp);
  if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
    goto err;
  }
  /* Push is ok, rulep is in grammarp->ruleStackp */
  rulep = NULL;

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  _marpaESLIF_rule_freev(rulep);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_default_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <default rule> ::= ':default' <op declare> <adverb list> */
  marpaESLIFGrammar_t               *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                      *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  int                                leveli;
  genericStack_t                    *adverbListItemStackp = NULL;
  marpaESLIF_grammar_t              *grammarp;
  short                              undefb;
  marpaESLIF_action_t               *actionp;
  short                              latmb;
  marpaESLIF_action_t               *symbolactionp;
  marpaESLIF_action_t               *eventactionp;
  marpaESLIF_action_t               *regexactionp;
  char                              *defaultEncodings;
  char                              *fallbackEncodings;
  short                              rcb;

  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* We restrict :default for a grammar to appear once */
  if (MARPAESLIF_UNLIKELY(grammarp->nbupdatei > 0)) {
    if (grammarp->descautob) {
      MARPAESLIF_ERRORF(marpaESLIFp, "The :default rule should appear once for grammar level %d", grammarp->leveli);
    } else {
      MARPAESLIF_ERRORF(marpaESLIFp, "The :default rule should appear once for grammar level %d (%s)", grammarp->leveli, grammarp->descp->asciis);
    }
    goto err;
  }
  
  /* Unpack the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              ":default rule",
                                                                              adverbListItemStackp,
                                                                              &actionp,
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              NULL, /* rankip */
                                                                              NULL, /* nullRanksHighbp */
                                                                              NULL, /* priorityipp */
                                                                              NULL, /* pauseip */
                                                                              &latmb,
                                                                              NULL, /* namingpp */
                                                                              &symbolactionp,
                                                                              NULL, /* eventInitializationpp */
                                                                              NULL, /* ifactionpp */
                                                                              &regexactionp,
                                                                              &eventactionp,
                                                                              &defaultEncodings,
                                                                              &fallbackEncodings
                                                                              ))) {
    goto err;
  }

  grammarp->nbupdatei++;

  /* Overwrite grammar default settings */
  _marpaESLIF_action_freev(grammarp->defaultRuleActionp);
  grammarp->defaultRuleActionp = NULL;
  if (actionp != NULL) {
    grammarp->defaultRuleActionp = _marpaESLIF_action_clonep(marpaESLIFp, actionp);
    if (MARPAESLIF_UNLIKELY(grammarp->defaultRuleActionp == NULL)) {
      goto err;
    }
  }

  grammarp->latmb = latmb;

  _marpaESLIF_action_freev(grammarp->defaultSymbolActionp);
  grammarp->defaultSymbolActionp = NULL;
  if (symbolactionp != NULL) {
    grammarp->defaultSymbolActionp = _marpaESLIF_action_clonep(marpaESLIFp, symbolactionp);
    if (MARPAESLIF_UNLIKELY(grammarp->defaultSymbolActionp == NULL)) {
      goto err;
    }
  }

  _marpaESLIF_action_freev(grammarp->defaultEventActionp);
  grammarp->defaultEventActionp = NULL;
  if (eventactionp != NULL) {
    grammarp->defaultEventActionp = _marpaESLIF_action_clonep(marpaESLIFp, eventactionp);
    if (MARPAESLIF_UNLIKELY(grammarp->defaultEventActionp == NULL)) {
      goto err;
    }
  }

  _marpaESLIF_action_freev(grammarp->defaultRegexActionp);
  grammarp->defaultRegexActionp = NULL;
  if (regexactionp != NULL) {
    grammarp->defaultRegexActionp = _marpaESLIF_action_clonep(marpaESLIFp, regexactionp);
    if (MARPAESLIF_UNLIKELY(grammarp->defaultRegexActionp == NULL)) {
      goto err;
    }
  }

  if (grammarp->defaultEncodings != NULL) {
    free(grammarp->defaultEncodings);
  }
  if (defaultEncodings != NULL) {
    grammarp->defaultEncodings = strdup(defaultEncodings);
    if (MARPAESLIF_UNLIKELY(grammarp->defaultEncodings == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  if (grammarp->fallbackEncodings != NULL) {
    free(grammarp->fallbackEncodings);
  }
  if (fallbackEncodings != NULL) {
    grammarp->fallbackEncodings = strdup(fallbackEncodings);
    if (MARPAESLIF_UNLIKELY(grammarp->fallbackEncodings == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_latm_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <latm specification> ::= 'latm' '=>' false */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LATM, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_latm_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <latm specification> ::= 'latm' '=>' true */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LATM, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_proper_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <proper specification> ::= 'proper' '=>' false */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PROPER, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_proper_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <proper specification> ::= 'proper' '=>' true */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PROPER, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_hideseparator_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <hide separator specification> ::= 'hide-separator' '=>' false */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_HIDESEPARATOR, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_hideseparator_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <hide separator specification> ::= 'hide-separator' '=>' true */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_HIDESEPARATOR, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rank_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rank specification> ::= 'rank' '=>' <signed integer> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char         *signedIntegers;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, signedIntegers);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(signedIntegers == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "signedIntegers indice %d is NULL", arg0i);
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RANK, atoi(signedIntegers));

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_null_ranking_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <null ranking specification> ::= 'null-ranking' '=>' <null ranking constant> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short         nullRanksHighb;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, argni, nullRanksHighb);

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING, nullRanksHighb);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_null_ranking_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <null ranking specification> ::= 'null' 'rank' '=>' <null ranking constant> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  short         nullRanksHighb;
  short         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_BOOL(marpaESLIFValuep, argni, nullRanksHighb);

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING, nullRanksHighb);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_null_ranking_constant_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <null ranking constant> ::= 'low' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_null_ranking_constant_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <null ranking constant> ::= 'high' */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_BOOL(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_pause_specification_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <pause specification> ::= 'pause' '=>' 'before' > */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PAUSE, MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_BEFORE);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_pause_specification_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <pause specification> ::= 'pause' '=>' 'before' > */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PAUSE, MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_AFTER);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_priority_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <priority specification> ::= 'priority' '=>' <signed integer> */
  marpaESLIF_t                             *marpaESLIFp    = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                                     *signedIntegers = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, arg0i+2, signedIntegers);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(signedIntegers == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "signedIntegers at indice %d is NULL", arg0i+2);
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_INT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PRIORITY, atoi(signedIntegers));

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_event_initializer_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <event initializer> ::= '=' <on or off> */
  marpaESLIF_t                                  *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_on_or_off_type_t          onOrOffb;
  marpaESLIF_bootstrap_event_initializer_type_t  eventInitializerb;
  short                                          rcb;

  MARPAESLIF_BOOTSTRAP_GET_SHORT(marpaESLIFValuep, argni, onOrOffb);

  switch (onOrOffb) {
  case MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_ON:
    eventInitializerb = MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON;
    break;
  case MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_OFF:
    eventInitializerb = MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported on or off value %d", (int) onOrOffb);
    goto err;
  }
  
  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_EVENT_INITIALIZER, eventInitializerb);
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_event_initializer_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <event initializer> ::= # empty */
  /* Per def this is a nullable - default event state is on */
  short rcb;

  MARPAESLIF_BOOTSTRAP_SET_SHORT(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_EVENT_INITIALIZER, MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_event_initializationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <event initialization> ::= <event name> <event initializer> */
  /* <event name> is an ASCII string */
  /* <event initializer> is a boolean */
  marpaESLIF_t                                 *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_event_initialization_t  *eventInitializationp  = NULL;
  char                                         *eventNames            = NULL;
  marpaESLIF_bootstrap_event_initializer_type_t eventInitializerb;
  short                                         rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, arg0i, eventNames);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(eventNames == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "eventNames at indice %d is NULL", argni);
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_SHORT(marpaESLIFValuep, argni, eventInitializerb);

  /* Make that an rhs primary structure */
  eventInitializationp = (marpaESLIF_bootstrap_event_initialization_t *) malloc(sizeof(marpaESLIF_bootstrap_event_initialization_t));
  if (MARPAESLIF_UNLIKELY(eventInitializationp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  eventInitializationp->eventNames  = strdup(eventNames);
  if (MARPAESLIF_UNLIKELY(eventInitializationp->eventNames == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }
  eventInitializationp->initializerb = eventInitializerb;

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_EVENT_INITIALIZATION, eventInitializationp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_event_initialization_freev(eventInitializationp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_event_specificationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <event specification> ::= 'event' '=>' <event initialization> */
  marpaESLIF_t                                *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp = NULL;
  short                                        rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+2, eventInitializationp);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(eventInitializationp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned NULL", arg0i+2);
    goto err;
  }

  /* Take care, eventInitializationp contain a pointer to an ASCII string, and it will be stored later in a marpaESLIF_bootstrap_adverb_list_item_t: */
  /* IF we were doing stack_get() followed by stack_set(..., 1 for the shallowb) this would work if eventInitializationp would not be stored again. */
  /* And unfortunately marpaESLIF_bootstrap_adverb_list_item_t structure has no notion of shallow pointer: it owns them totally. */
  /* This mean that we have to make sure that eventInitializationp does not remain in the stack at arg0. */
  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENT_INITIALIZATION, eventInitializationp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_event_initialization_freev(eventInitializationp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_lexeme_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <lexeme rule> ::= ':lexeme' <op declare> <lhs> <adverb list> */
  marpaESLIFGrammar_t                         *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                                *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                              *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_lhs_t                  *bootstrapLhsp;
  marpaESLIF_symbol_t                         *lhsp;
  int                                          leveli;
  marpaESLIF_grammar_t                        *grammarp;
  int                                         *priorityip;
  marpaESLIF_bootstrap_pause_type_t            pausei;
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp;
  short                                        undefb;
  marpaESLIF_action_t                         *symbolactionp;
  marpaESLIF_action_t                         *ifactionp;
  short                                        rcb;

  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, bootstrapLhsp);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the symbol exist */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* Unpack the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              ":lexeme rule",
                                                                              adverbListItemStackp,
                                                                              NULL, /* actionpp */
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              NULL, /* rankip */
                                                                              NULL, /* nullRanksHighbp */
                                                                              &priorityip,
                                                                              &pausei,
                                                                              NULL, /* latmbp */
                                                                              NULL, /* namingpp */
                                                                              &symbolactionp,
                                                                              &eventInitializationp,
                                                                              &ifactionp,
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  /* Update the symbol */
  if (priorityip != NULL) {
    lhsp->priorityi = *priorityip;
  }

  if (symbolactionp != NULL) {
    _marpaESLIF_action_freev(lhsp->symbolActionp);
    lhsp->symbolActionp = _marpaESLIF_action_clonep(marpaESLIFp, symbolactionp);
    if (MARPAESLIF_UNLIKELY(lhsp->symbolActionp == NULL)) {
      goto err;
    }
  }

  if (ifactionp != NULL) {
    _marpaESLIF_action_freev(lhsp->ifActionp);
    lhsp->ifActionp = _marpaESLIF_action_clonep(marpaESLIFp, ifactionp);
    if (MARPAESLIF_UNLIKELY(lhsp->ifActionp == NULL)) {
      goto err;
    }
  }

  if (eventInitializationp != NULL) {
    /* It is a non-sense to have an event initialization without pause information */
    if (MARPAESLIF_UNLIKELY(eventInitializationp->eventNames == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, event name is NULL", bootstrapLhsp->symbols);
      goto err;
    }
    switch (pausei) {
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_BEFORE:
      if (lhsp->eventBefores != NULL) {
        free(lhsp->eventBefores);
      }
      lhsp->eventBefores = strdup(eventInitializationp->eventNames);
      if (MARPAESLIF_UNLIKELY(lhsp->eventBefores == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      switch (eventInitializationp->initializerb) {
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
        lhsp->eventBeforeb = 1;
        break;
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
        lhsp->eventBeforeb = 0;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, unsupported event initializer type %d", bootstrapLhsp->symbols, (int) eventInitializationp->initializerb);
        goto err;
        break;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_AFTER:
      if (lhsp->eventAfters != NULL) {
        free(lhsp->eventAfters);
      }
      lhsp->eventAfters = strdup(eventInitializationp->eventNames);
      if (MARPAESLIF_UNLIKELY(lhsp->eventAfters == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      switch (eventInitializationp->initializerb) {
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
        lhsp->eventAfterb = 1;
        break;
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
        lhsp->eventAfterb = 0;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, unsupported event initializer type %d", bootstrapLhsp->symbols, (int) eventInitializationp->initializerb);
        goto err;
        break;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_NA:
      MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, you must supply pause => before, or pause => after, when giving an event name", bootstrapLhsp->symbols);
      goto err;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, Unsupported pause type %d", bootstrapLhsp->symbols, pausei);
      goto err;
    }
  } else {
    /* It is a non-sense to have pause information without an event initialization */
    switch (pausei) {
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_BEFORE:
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_AFTER:
      MARPAESLIF_ERRORF(marpaESLIFp, "In :lexeme rule for symbol <%s>, you must supply event => <event initializer> when giving a pause speficiation", bootstrapLhsp->symbols);
      goto err;
    default:
      break;
    }
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_terminal_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <terminal rule> ::= ':lexeme' <op declare> terminal <adverb list> */
  marpaESLIFGrammar_t                         *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                                *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                              *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_terminal_t             *terminalp = NULL;
  marpaESLIF_symbol_t                         *lhsp;
  int                                          leveli;
  marpaESLIF_grammar_t                        *grammarp;
  int                                         *priorityip;
  marpaESLIF_bootstrap_pause_type_t            pausei;
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp;
  short                                        undefb;
  marpaESLIF_action_t                         *symbolactionp;
  marpaESLIF_action_t                         *ifactionp;
  short                                        rcb;

  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, terminalp);

  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the terminal exist */
  lhsp = _marpaESLIF_bootstrap_check_terminalp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, terminalp, 1 /* createb */, 0 /* forcecreateb */, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* Unpack the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              ":terminal rule",
                                                                              adverbListItemStackp,
                                                                              NULL, /* actionpp */
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              NULL, /* rankip */
                                                                              NULL, /* nullRanksHighbp */
                                                                              &priorityip,
                                                                              &pausei,
                                                                              NULL, /* latmbp */
                                                                              NULL, /* namingpp */
                                                                              &symbolactionp,
                                                                              &eventInitializationp,
                                                                              &ifactionp,
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  /* Update the symbol */
  if (priorityip != NULL) {
    lhsp->priorityi = *priorityip;
  }

  if (symbolactionp != NULL) {
    _marpaESLIF_action_freev(lhsp->symbolActionp);
    lhsp->symbolActionp = _marpaESLIF_action_clonep(marpaESLIFp, symbolactionp);
    if (MARPAESLIF_UNLIKELY(lhsp->symbolActionp == NULL)) {
      goto err;
    }
  }

  if (ifactionp != NULL) {
    _marpaESLIF_action_freev(lhsp->ifActionp);
    lhsp->ifActionp = _marpaESLIF_action_clonep(marpaESLIFp, ifactionp);
    if (MARPAESLIF_UNLIKELY(lhsp->ifActionp == NULL)) {
      goto err;
    }
  }

  if (eventInitializationp != NULL) {
    /* It is a non-sense to have an event initialization without pause information */
    if (MARPAESLIF_UNLIKELY(eventInitializationp->eventNames == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "In :terminal rule for terminal <%s>, event name is NULL", _marpaESLIF_symbol_shows(lhsp));
      goto err;
    }
    switch (pausei) {
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_BEFORE:
      if (lhsp->eventBefores != NULL) {
        free(lhsp->eventBefores);
      }
      lhsp->eventBefores = strdup(eventInitializationp->eventNames);
      if (MARPAESLIF_UNLIKELY(lhsp->eventBefores == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      switch (eventInitializationp->initializerb) {
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
        lhsp->eventBeforeb = 1;
        break;
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
        lhsp->eventBeforeb = 0;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "In :terminal rule, unsupported event initializer type %d", (int) eventInitializationp->initializerb);
        goto err;
        break;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_AFTER:
      if (lhsp->eventAfters != NULL) {
        free(lhsp->eventAfters);
      }
      lhsp->eventAfters = strdup(eventInitializationp->eventNames);
      if (MARPAESLIF_UNLIKELY(lhsp->eventAfters == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      switch (eventInitializationp->initializerb) {
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
        lhsp->eventAfterb = 1;
        break;
      case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
        lhsp->eventAfterb = 0;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "In :terminal rule, unsupported event initializer type %d", (int) eventInitializationp->initializerb);
        goto err;
        break;
      }
      break;
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_NA:
      MARPAESLIF_ERROR(marpaESLIFp, "In :terminal rule, you must supply pause => before, or pause => after, when giving an event name");
      goto err;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "In :terminal rule, Unsupported pause type %d", pausei);
      goto err;
    }
  } else {
    /* It is a non-sense to have pause information without an event initialization */
    switch (pausei) {
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_BEFORE:
    case MARPAESLIF_BOOTSTRAP_PAUSE_TYPE_AFTER:
      MARPAESLIF_ERROR(marpaESLIFp, "In :terminal rule, you must supply event => <event initializer> when giving a pause speficiation");
      goto err;
    default:
      break;
    }
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_discard_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <discard rule> ::= ':discard' <op declare> <rhs primary> <adverb list> */
  static const char                           *funcs              = "_marpaESLIF_bootstrap_G1_action_discard_ruleb";
  marpaESLIFGrammar_t                         *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                                *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_rule_t                           *rulep = NULL;
  genericStack_t                              *adverbListItemStackp = NULL;
  int                                          leveli;
  marpaESLIF_bootstrap_rhs_primary_t          *rhsPrimaryp;
  short                                        undefb;
  marpaESLIF_symbol_t                         *discardp;
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp;
  marpaESLIF_symbol_t                         *rhsp;
  marpaESLIF_grammar_t                        *grammarp;
  short                                        rcb;
  short                                        forcecreateb;
  
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+2, rhsPrimaryp);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }
 
  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the :discard - note that it is never parameterized */
  discardp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, ":discard", 1 /* createb */, 0 /* forcecreateb */, NULL /* parameterStackp */, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(discardp == NULL)) {
    goto err;
  }
  /* Make sure it has the internal discard flag */
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Marking meta symbol %s in grammar level %d as :discard symbol", _marpaESLIF_symbol_shows(discardp), grammarp->leveli);
  discardp->discardb = 1;

  /* Check the rhs primary - Note that :discard is never parameterized */
  /* Note that we always want to explicitely create the symbol if this is a terminal, so that if there is an event for it, it is unique */
  if ((rhsPrimaryp->type == MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL) &&
      (rhsPrimaryp->u.singleSymbolp->type == MARPAESLIF_BOOTSTRAP_SINGLE_SYMBOL_TYPE_TERMINAL)) {
    forcecreateb = 1;
  } else {
    forcecreateb = 0;
  }
  if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp, 1 /* createb */, forcecreateb, &rhsp) == NULL)) {
    goto err;
  }

  /* Check the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              ":discard rule",
                                                                              adverbListItemStackp,
                                                                              NULL, /* actionpp */
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              NULL, /* ranki */
                                                                              NULL, /* nullRanksHighb */
                                                                              NULL, /* priorityipp */
                                                                              NULL, /* pauseip */
                                                                              NULL, /* latmbp */
                                                                              NULL, /* namingpp */
                                                                              NULL, /* symbolactionpp */
                                                                              &eventInitializationp,
                                                                              NULL, /* ifactionpp */
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  MARPAESLIF_NOTICEF(marpaESLIFp, "%s: Creating rule %s ::= %s at grammar level %d", funcs, _marpaESLIF_symbol_shows(discardp), _marpaESLIF_symbol_shows(rhsp), grammarp->leveli);
  /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
  rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                grammarp,
                                NULL, /* descEncodings */
                                NULL, /* descs */
                                0, /* descl */
                                discardp->idi,
                                1, /* nrhsl */
                                &(rhsp->idi), /* rhsip */
                                -1, /* exceptioni */
                                0, /* ranki */
                                0, /* nullRanksHighb */
                                0, /* sequenceb */
                                -1, /* minimumi */
                                -1, /* separatori */
                                0, /* properb */
                                NULL, /* actionp */
                                0, /* passthroughb */
                                0 /* hideseparatorb */,
                                NULL, /* skipbp */
                                NULL /* :discard is not contextual */);
  if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
    goto err;
  }

  if (eventInitializationp != NULL) {
    if (MARPAESLIF_UNLIKELY(eventInitializationp->eventNames == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "In :discard rule, event name is NULL");
      goto err;
    }
    /* Take care, we set the discard event on the RULE - not on the symbol */
    if (rulep->discardEvents != NULL) {
      free(rulep->discardEvents);
    }
    rulep->discardEvents = strdup(eventInitializationp->eventNames);
    if (MARPAESLIF_UNLIKELY(rulep->discardEvents == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    switch (eventInitializationp->initializerb) {
    case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
      rulep->discardEventb = 1;
      break;
    case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
      rulep->discardEventb = 0;
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "In :discard rule, unsupported event initializer type %d", (int) eventInitializationp->initializerb);
      goto err;
      break;
    }
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_event_declarationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, marpaESLIF_bootstrap_event_declaration_type_t type)
/*****************************************************************************/
{
  /* <TYPE event declaration> ::= 'event' <event initialization> {'=' OR <op_declare>} 'TYPE' <lhs> */
  static const char                           *funcs              = "_marpaESLIF_bootstrap_G1_action_event_declarationb";
  marpaESLIFGrammar_t                         *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                                *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_event_initialization_t *eventInitializationp;
  marpaESLIF_bootstrap_lhs_t                  *bootstrapLhsp;
  marpaESLIF_grammar_t                        *grammarp;
  marpaESLIF_symbol_t                         *lhsp;
  char                                       **eventsp = NULL;
  short                                       *eventbp = NULL;
#ifndef MARPAESLIF_NTRACE
  char                                        *types = NULL;
#endif
  short                                        intb = 0;
  int                                          leveli = 0;
  short                                        rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+1, eventInitializationp);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY(eventInitializationp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned NULL", arg0i+2);
    goto err;
  }
  MARPAESLIF_BOOTSTRAP_IS_INT(marpaESLIFValuep, arg0i+2, intb);
  if (intb) {
    MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+2, leveli);
  }
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i+4, bootstrapLhsp);

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the symbol */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* It is a non-sense to have an event initialization without a name */
  if (MARPAESLIF_UNLIKELY(eventInitializationp->eventNames == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "In event declaration for symbol <%s>, event name is NULL", bootstrapLhsp->symbols);
    goto err;
  }

  /* Update symbol */
  switch (type) {
  case MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_PREDICTED:
    eventsp = &(lhsp->eventPredicteds);
    eventbp = &(lhsp->eventPredictedb);
#ifndef MARPAESLIF_NTRACE
    types   = "predicted";
#endif
    break;
  case MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_NULLED:
    eventsp = &(lhsp->eventNulleds);
    eventbp = &(lhsp->eventNulledb);
#ifndef MARPAESLIF_NTRACE
    types   = "nulled";
#endif
    break;
  case MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_COMPLETED:
    eventsp = &(lhsp->eventCompleteds);
    eventbp = &(lhsp->eventCompletedb);
#ifndef MARPAESLIF_NTRACE
    types   = "completion";
#endif
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "In event declaration for symbol <%s>, unsupported event type %d", bootstrapLhsp->symbols, type);
    goto err;
    break;
  }
  
  if (*eventsp != NULL) {
    free(*eventsp);
  }
  *eventsp = strdup(eventInitializationp->eventNames);
  if (MARPAESLIF_UNLIKELY(*eventsp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }
  switch (eventInitializationp->initializerb) {
  case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_ON:
    *eventbp = 1;
    break;
  case MARPAESLIF_BOOTSTRAP_EVENT_INITIALIZER_TYPE_OFF:
    *eventbp = 0;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "In completion event declaration for symbol <%s>, unsupported event initializer type %d", bootstrapLhsp->symbols, (int) eventInitializationp->initializerb);
    goto err;
    break;
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

#ifndef MARPAESLIF_NTRACE
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Setted %s event %s=%s for symbol <%s> at grammar level %d", types, *eventsp, *eventbp ? "on" : "off", bootstrapLhsp->symbols, leveli);
#endif

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_completion_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_COMPLETED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_completion_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_COMPLETED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_NULLED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_nulled_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_NULLED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_PREDICTED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_predicted_event_declaration_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_action_event_declarationb(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb, MARPAESLIF_BOOTSTRAP_EVENT_DECLARATION_TYPE_PREDICTED);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_alternative_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <alternative name> ::= <quoted string literal> */
  marpaESLIF_t                      *marpaESLIFp   = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  void                              *bytep         = NULL;
  size_t                             bytel;
  short                              shallowb      = 0;
  marpaESLIF_bootstrap_utf_string_t *quotedStringp = NULL;
  short                              rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i, bytep, bytel, shallowb);
  quotedStringp = _marpaESLIF_bootstrap_unquote_string_literalp(marpaESLIFValuep, bytep, bytel, 0 /* allowEmptyStringb */);
  if (MARPAESLIF_UNLIKELY(quotedStringp == NULL)) {
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_ARRAY(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE_NAME, quotedStringp->bytep, quotedStringp->bytel);
  quotedStringp->bytep = NULL; /* We want to keep this pointer */

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  _marpaESLIF_bootstrap_utf_string_freev(quotedStringp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_namingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* naming ::= 'name' '=>' <alternative name> */
  /* <alternative name> is always an array */
  marpaESLIF_t                      *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_utf_string_t *namingp     = NULL;
  void                              *bytep       = NULL;
  size_t                             bytel;
  short                              shallowb    = 0;
  short                              rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, argni, bytep, bytel, shallowb);
  /* It is a non-sense to have a null information */
  if (MARPAESLIF_UNLIKELY((bytep == NULL) || (bytel <= 0))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned {p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  namingp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (MARPAESLIF_UNLIKELY(namingp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  namingp->bytep     = NULL;
  namingp->bytel     = bytel;
  namingp->modifiers = NULL;

  /* Duplicate bytep if it is shallow */
  if (shallowb) {
    namingp->bytep     = malloc(bytel + 1);
    if (MARPAESLIF_UNLIKELY(namingp->bytep == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(namingp->bytep, bytep, bytel);
    namingp->bytep[bytel] = '\0';
  } else {
    namingp->bytep     = bytep;
    bytep = NULL; /* Prevent free() */
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NAMING, namingp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_utf_string_freev(namingp);
  rcb = 0;

 done:
  if ((! shallowb) && (bytep != NULL)) {
    free(bytep);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_exception_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <exception statement> ::= <lhs> <op declare> <rhs primary> '-' <rhs primary> <adverb list> */
  static const char                    *funcs                = "_marpaESLIF_bootstrap_G1_action_exception_statementb";
  marpaESLIFGrammar_t                  *marpaESLIFGrammarp   = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                         *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_rule_t                    *rulep                = NULL;
  genericStack_t                       *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_rhs_primary_t   *rhsPrimaryp          = NULL;
  marpaESLIF_bootstrap_rhs_primary_t   *rhsPrimaryExceptionp = NULL;
  short                                 undefb;
  marpaESLIF_bootstrap_lhs_t           *bootstrapLhsp;
  int                                   leveli;
  marpaESLIF_symbol_t                  *lhsp;
  marpaESLIF_symbol_t                  *rhsp;
  marpaESLIF_symbol_t                  *rhsExceptionp;
  marpaESLIF_grammar_t                 *grammarp;
  marpaESLIF_action_t                  *actionp = NULL;
  int                                   ranki = 0;
  short                                 nullRanksHighb = 0;
  marpaESLIF_bootstrap_utf_string_t    *namingp;
  short                                 rcb;
  
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, bootstrapLhsp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+1, leveli);
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+2, rhsPrimaryp);
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+4, rhsPrimaryExceptionp);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, argni, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, argni, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }
 
  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFValuep, marpaESLIFGrammarp, leveli, NULL);
  if (MARPAESLIF_UNLIKELY(grammarp == NULL)) {
    goto err;
  }

  /* Check the lhs */
  lhsp = _marpaESLIF_bootstrap_check_lhsp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, bootstrapLhsp->symbols, 1 /* createb */, 0 /* forcecreateb */, bootstrapLhsp->parameterStackp, NULL /* lhsAsRhspp */);
  if (MARPAESLIF_UNLIKELY(lhsp == NULL)) {
    goto err;
  }

  /* Check the rhs primary */
  if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryp, 1 /* createb */, 0 /* forcecreateb */, &rhsp) == NULL)) {
    goto err;
  }

  /* Check the rhs primary exception */
  if (MARPAESLIF_UNLIKELY(_marpaESLIF_bootstrap_check_rhsPrimaryp(marpaESLIFValuep, marpaESLIFGrammarp, grammarp, rhsPrimaryExceptionp, 1 /* createb */, 0 /* forcecreateb */, &rhsExceptionp) == NULL)) {
    goto err;
  }

  /* Check the adverb list */
  if (MARPAESLIF_UNLIKELY(! _marpaESLIF_bootstrap_unpack_adverbListItemStackb(marpaESLIFp,
                                                                              "exception rule",
                                                                              adverbListItemStackp,
                                                                              &actionp,
                                                                              NULL, /* left_associationbp */
                                                                              NULL, /* right_associationbp */
                                                                              NULL, /* group_associationbp */
                                                                              NULL, /* separatorRhsPrimarypp */
                                                                              NULL, /* properbp */
                                                                              NULL, /* hideseparatorbp */
                                                                              &ranki,
                                                                              &nullRanksHighb,
                                                                              NULL, /* priorityipp */
                                                                              NULL, /* pauseip */
                                                                              NULL, /* latmbp */
                                                                              &namingp,
                                                                              NULL, /* symbolactionpp */
                                                                              NULL, /* eventInitializationpp */
                                                                              NULL, /* ifactionpp */
                                                                              NULL, /* regexactionpp */
                                                                              NULL, /* eventactionpp */
                                                                              NULL, /* defaultEncodingsp */
                                                                              NULL /* fallbackEncodingsp */
                                                                              ))) {
    goto err;
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Creating exception rule %s ::= %s - %s", _marpaESLIF_symbol_shows(lhsp), _marpaESLIF_symbol_shows(rhsp), _marpaESLIF_symbol_shows(rhsExceptionp));
  /* If naming is not NULL, it is guaranteed to be an UTF-8 thingy */
  rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                grammarp,
                                (namingp != NULL) ? (char *) MARPAESLIF_UTF8_STRING : NULL, /* descEncodings */
                                (namingp != NULL) ? namingp->bytep : NULL, /* descs */
                                (namingp != NULL) ? namingp->bytel : 0, /* descl */
                                lhsp->idi,
                                1, /* nrhsl */
                                &(rhsp->idi), /* rhsip */
                                rhsExceptionp->idi,
                                ranki,
                                0, /*nullRanksHighb */
                                0, /* sequenceb */
                                0, /* minimumi */
                                -1, /* separatori */
                                0, /* properb */
                                actionp,
                                0, /* passthroughb */
                                0 /* hideseparatorb */,
                                NULL, /* skipbp */
                                bootstrapLhsp->parameterStackp);
  if (MARPAESLIF_UNLIKELY(rulep == NULL)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(grammarp->ruleStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryExceptionp);
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_regex_to_stringb(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel)
/*****************************************************************************/
{
  marpaESLIF_t                      *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_bootstrap_utf_string_t *stringp               = NULL;
  char                              *modifiers             = NULL;
  void                              *newbytep              = NULL;
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  size_t                             newbytel;
  marpaESLIFGrammar_t                marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t            marpaESLIFValueResult;
  size_t                             sizel;
  marpaESLIF_matcher_value_t         rci;

  /* It is a non-sense to have a null lexeme */
  if (MARPAESLIF_UNLIKELY((bytep == NULL) || (bytel <= 0))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIF_bootstrap_regex_to_stringb called with {bytep,bytel}={%p,%ld}", bytep, (unsigned long) bytel);
    goto err;
  }

  /* Extract opti from the array */
  /* Thre are several methods...: */
  /* - Re-execute the sub-grammar as if it was a top grammar */
  /* - apply a regexp to extract the modifiers. */
  /* - revisit our own top grammar to have two separate lexemes (which I do not like because modifers can then be separated from regex by a discard symbol) */
  /* ... Since we are internal anyway I choose (what I think is) the costless method: the regexp */

  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar,
                                                     NULL, /* marpaESLIFRecognizerOptionp */
                                                     0, /* discardb - no effect anway because we are in fake mode */
                                                     1, /* noEventb - no effect anway because we are in fake mode */
                                                     0, /* silentb */
                                                     NULL, /* marpaESLIFRecognizerParentp */
                                                     1, /* fakeb */
                                                     0, /* wantedStartCompletionsi */
                                                     1, /* A grammar is always transformed to valid UTF-8 before being parsed */
                                                     1 /* grammmarIsOnStackb */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp == NULL)) {
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(! _marpaESLIFRecognizer_terminal_matcherb(marpaESLIFRecognizerp,
                                                                    marpaESLIFRecognizerp->marpaESLIF_streamp,
                                                                    marpaESLIFp->regexModifiersp,
                                                                    bytep,
                                                                    bytel,
                                                                    1, /* eofb */
                                                                    &rci,
                                                                    &marpaESLIFValueResult,
                                                                    NULL /* matchedLengthlp */))) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a regular expression it is something like "xxxxx" */
#ifndef MARPAESLIF_NTRACE
    /* Paranoid mode */
    if (MARPAESLIF_UNLIKELY(marpaESLIFValueResult.u.a.sizel <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
#endif
    /* We want to maintain marpaESLIFValueResult lifetime, so need to unshallow the result of the INTERNAL method _marpaESLIFRecognizer_terminal_matcherb() if necessary */
    if (marpaESLIFValueResult.u.a.shallowb) {
      modifiers = (char *) malloc(marpaESLIFValueResult.u.a.sizel + 1);
      if (MARPAESLIF_UNLIKELY(modifiers == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(modifiers, marpaESLIFValueResult.u.a.p, marpaESLIFValueResult.u.a.sizel);
      modifiers[marpaESLIFValueResult.u.a.sizel] = '\0';
    } else {
      modifiers = (char *) marpaESLIFValueResult.u.a.p;
    }
    sizel = marpaESLIFValueResult.u.a.sizel;
  } else {
    /* Because we use this value just below */
    sizel = 0;
  }

  stringp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  /* By definition a regular expression is a lexeme in this form: /xxxx/modifiers */
  /* we have already catched the modifiers. But we have to shift the UTF-8 buffer: */
  /* - We know per def that it is starting with the "/" ASCII character (one byte) */
  /* - We know per def that it is ending with "/modifiers", all of them being ASCII characters (one byte each) */
  newbytel = bytel - 2; /* First "/" and last "/" */
  if (MARPAESLIF_UNLIKELY(newbytel <= 0)) {
    /* Empty regex !? */
    MARPAESLIF_ERROR(marpaESLIFp, "Empty regex");
    goto err;
  }
  if (sizel > 0) {
    newbytel -= sizel;  /* "xxxx" */
  }
  if (MARPAESLIF_UNLIKELY(newbytel <= 0)) {
    /* Still Empty regex !? */
    MARPAESLIF_ERROR(marpaESLIFp, "Empty regex");
    goto err;
  }
  newbytep = malloc(newbytel);
  if (MARPAESLIF_UNLIKELY(newbytep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(newbytep, (void *) (((char *) bytep) + 1), newbytel);
  stringp->modifiers = modifiers;
  stringp->bytep     = newbytep;
  stringp->bytel     = newbytel;
  modifiers = NULL; /* modifiers is in singleSymbolp */
  newbytep = NULL; /* newbytep is in singleSymbolp */

  goto done;

 err:
  _marpaESLIF_bootstrap_utf_string_freev(stringp);
  stringp = NULL;

 done:
  if (newbytep != NULL) {
    free(newbytep);
  }
  if (modifiers != NULL) {
    free(modifiers);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
 return stringp;
}

/*****************************************************************************/
static inline marpaESLIF_bootstrap_utf_string_t *_marpaESLIF_bootstrap_characterClass_to_stringb(marpaESLIFValue_t *marpaESLIFValuep, void *bytep, size_t bytel)
/*****************************************************************************/
{
  marpaESLIF_t                      *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIF_bootstrap_utf_string_t *stringp               = NULL;
  char                              *modifiers             = NULL;
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  void                              *dupp                  = NULL;
  size_t                             dupl;
  marpaESLIFGrammar_t                marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t            marpaESLIFValueResult;
  size_t                             sizel;
  marpaESLIF_matcher_value_t         rci;
  char                              *tmps;

  /* It is a non-sense to have a null lexeme */
  if (MARPAESLIF_UNLIKELY((bytep == NULL) || (bytel <= 0))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIF_bootstrap_characterClass_to_stringb called with {bytep,bytel}={%p,%ld}", bytep, (unsigned long) bytel);
    goto err;
  }

  /* Duplicate bytep in any case */
  tmps = (char *) malloc(bytel + 1);
  if (MARPAESLIF_UNLIKELY(tmps == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(tmps, bytep, bytel);
  tmps[bytel] = '\0';
  dupp = tmps;
  dupl = bytel;

  /* Extract options from it */
  /* Thre are several methods...: */
  /* - Re-execute the sub-grammar as if it was a top grammar */
  /* - apply a regexp to extract the modifiers. */
  /* - revisit our own top grammar to have two separate lexemes (which I do not like because modifers can then be separated from regex by a discard symbol) */
  /* ... Since we are internal anyway I choose (what I think is) the costless method: the regexp */

  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar,
                                                     NULL, /* marpaESLIFRecognizerOptionp */
                                                     0, /* discardb */
                                                     1, /* noEventb - no effect anway because we are in fake mode */
                                                     0, /* silentb */
                                                     NULL, /* marpaESLIFRecognizerParentp */
                                                     1, /* fakeb */
                                                     0, /* wantedStartCompletionsi */
                                                     1, /* A grammar is always transformed to valid UTF-8 before being parsed */
                                                     1 /* grammmarIsOnStackb */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp == NULL)) {
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(! _marpaESLIFRecognizer_terminal_matcherb(marpaESLIFRecognizerp,
                                                                    marpaESLIFRecognizerp->marpaESLIF_streamp,
                                                                    marpaESLIFp->characterClassModifiersp,
                                                                    dupp,
                                                                    dupl,
                                                                    1, /* eofb */
                                                                    &rci,
                                                                    &marpaESLIFValueResult,
                                                                    NULL /* matchedLengthlp */))) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a character class it is something like ":xxxxx" */
#ifndef MARPAESLIF_NTRACE
    /* Paranoid mode */
    if (MARPAESLIF_UNLIKELY(marpaESLIFValueResult.u.a.sizel <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
#endif
    /* We want to maintain marpaESLIFValueResult lifetime, so need to unshallow the result of the INTERNAL method _marpaESLIFRecognizer_terminal_matcherb() if necessary */
    if (marpaESLIFValueResult.u.a.shallowb) {
      modifiers = (char *) malloc(marpaESLIFValueResult.u.a.sizel + 1);
      if (MARPAESLIF_UNLIKELY(modifiers == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(modifiers, marpaESLIFValueResult.u.a.p, marpaESLIFValueResult.u.a.sizel);
      modifiers[marpaESLIFValueResult.u.a.sizel] = '\0';
    } else {
      modifiers = (char *) marpaESLIFValueResult.u.a.p;
    }
    sizel = marpaESLIFValueResult.u.a.sizel;
  } else {
    /* Because we use this value just below */
    sizel = 0;
  }

  stringp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (MARPAESLIF_UNLIKELY(stringp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  stringp->modifiers = modifiers;
  stringp->bytep     = dupp;
  stringp->bytel     = dupl;

  modifiers = NULL; /* modifiers is in singleSymbolp */
  dupp      = NULL; /* dupp is in singleSymbolp */

  if (sizel > 0) {
    stringp->bytel -= (sizel + 1);  /* ":xxxx" */
  }

  goto done;

 err:
  _marpaESLIF_bootstrap_utf_string_freev(stringp);
  stringp = NULL;

 done:
  if (dupp != NULL) {
    free(dupp);
  }
  if (modifiers != NULL) {
    free(modifiers);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
 return stringp;
}

/*****************************************************************************/
static inline int _marpaESLIF_bootstrap_ord2utfb(marpaESLIF_uint32_t uint32, PCRE2_UCHAR *bufferp)
/*****************************************************************************/
{
  int i;
  int j;

  for (i = 0; i < utf8_table1_size; i++) {
    if ((int)uint32 <= utf8_table1[i]) {
      break;
    }
  }
  bufferp += i;
  for (j = i; j > 0; j--) {
    *bufferp-- = 0x80 | (uint32 & 0x3f);
    uint32 >>= 6;
  }
  *bufferp = utf8_table2[i] | uint32;
  return i + 1;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_2_and_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb)
/*****************************************************************************/
{
  /*  skipb: <rhs alternative> ::= '(-' <priorities> '-)' */
  /* !skipb: <rhs alternative> ::= '('  <priorities> ')'  */
  marpaESLIF_t                             *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  genericStack_t                           *alternativesStackp = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t   *rhsAlternativep    = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i + 1, alternativesStackp);
  /* It is a non-sense to not have valid information */
  if (MARPAESLIF_UNLIKELY(alternativesStackp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp at indice %d is NULL", arg0i + 1);
    goto err;
  }

  rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_alternative_t));
  if (MARPAESLIF_UNLIKELY(rhsAlternativep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsAlternativep->symbolShallowp                  = NULL;
  rhsAlternativep->type                            = MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_PRIORITIES;
  rhsAlternativep->u.priorities.skipb              = skipb;
  rhsAlternativep->u.priorities.alternativesStackp = alternativesStackp;
  alternativesStackp = NULL; /* alternativesStackp is now in rhsAlternativep */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_ALTERNATIVE, rhsAlternativep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_priorities_freev(alternativesStackp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_4_and_5b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb)
/*****************************************************************************/
{
  /*  skipb: <rhs alternative> ::= '(-' <rhs primary> '-' <rhs primary> <adverb list> '-)' */
  /* !skipb: <rhs alternative> ::= '('  <rhs primary> '-' <rhs primary> <adverb list> ')'  */
  marpaESLIF_t                             *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp          = NULL;
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryExceptionp = NULL;
  genericStack_t                           *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t   *rhsAlternativep      = NULL;
  short                                     undefb;
  short                                     rcb;

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+1, rhsPrimaryp);
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+3, rhsPrimaryExceptionp);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i+4, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+4, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_alternative_t));
  if (MARPAESLIF_UNLIKELY(rhsAlternativep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsAlternativep->symbolShallowp                   = NULL;
  rhsAlternativep->type                             = MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_EXCEPTION;
  rhsAlternativep->u.exception.skipb                = skipb;
  rhsAlternativep->u.exception.rhsPrimaryp          = rhsPrimaryp;
  rhsAlternativep->u.exception.rhsPrimaryExceptionp = rhsPrimaryExceptionp;
  rhsAlternativep->u.exception.adverbListItemStackp = adverbListItemStackp;

  rhsPrimaryp = NULL; /* rhsPrimaryp is now in rhsAlternativep */
  rhsPrimaryExceptionp = NULL; /* rhsPrimaryExceptionp is now in rhsAlternativep */
  adverbListItemStackp = NULL; /* adverbListItemStackp is now in rhsAlternativep */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_ALTERNATIVE, rhsAlternativep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryExceptionp);
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_action_rhs_alternative_6_and_7b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb, short skipb)
/*****************************************************************************/
{
  /*  skipb: <rhs alternative> ::= '(-' <rhs primary> <quantifier> <adverb list> '-)' */
  /* !skipb: <rhs alternative> ::= '('  <rhs primary> <quantifier> <adverb list> ')'  */
  marpaESLIF_t                             *marpaESLIFp          = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp          = NULL;
  int                                       minimumi;
  genericStack_t                           *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_rhs_alternative_t   *rhsAlternativep      = NULL;
  short                                     undefb;
  short                                     rcb;

  MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+1, rhsPrimaryp);
  MARPAESLIF_BOOTSTRAP_GET_INT(marpaESLIFValuep, arg0i+2, minimumi);
  /* adverb list may be undef */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i+3, undefb);
  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_PTR(marpaESLIFValuep, arg0i+3, adverbListItemStackp);
    /* Non-sense to have a NULL stack in this case */
    if (MARPAESLIF_UNLIKELY(adverbListItemStackp == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  rhsAlternativep = (marpaESLIF_bootstrap_rhs_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_alternative_t));
  if (MARPAESLIF_UNLIKELY(rhsAlternativep == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsAlternativep->symbolShallowp                    = NULL;
  rhsAlternativep->type                              = MARPAESLIF_BOOTSTRAP_RHS_ALTERNATIVE_TYPE_QUANTIFIED;
  rhsAlternativep->u.quantified.skipb                = skipb;
  rhsAlternativep->u.quantified.rhsPrimaryp          = rhsPrimaryp;
  rhsAlternativep->u.quantified.minimumi             = minimumi;
  rhsAlternativep->u.quantified.adverbListItemStackp = adverbListItemStackp;

  rhsPrimaryp = NULL; /* rhsPrimaryp is now in rhsAlternativep */
  adverbListItemStackp = NULL; /* adverbListItemStackp is now in rhsPrimaryp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_ALTERNATIVE, rhsAlternativep);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_alternative_freev(rhsAlternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_luascript_statementb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <external script statement> ::= <external script tag start> <discard off> <external script source> <external script tag end> <discard on>  */
  static const char   *funcs              = "_marpaESLIF_bootstrap_G1_action_luascript_statementb";
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t        *marpaESLIFp        = marpaESLIFValuep->marpaESLIFp; /* marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *luabytep           = NULL;
  size_t               luabytel;
  short                shallowb           = 0;
  short                rcb                = 0;
  char                *tmps;
  size_t               tmpl;
  short                undefb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* But source can be nullable */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i+2, undefb);

  if (! undefb) {
    MARPAESLIF_BOOTSTRAP_GETANDFORGET_ARRAY(marpaESLIFValuep, arg0i+2, luabytep, luabytel, shallowb);
    /* It is a non-sense to not have valid information */
    if (MARPAESLIF_UNLIKELY((luabytep == NULL) || (luabytel <= 0))) {
      MARPAESLIF_ERRORF(marpaESLIFp, "_marpaESLIFValue_stack_getAndForgetb at indice %d returned {%p,%ld}", arg0i+2, luabytep, (unsigned long) luabytel);
      goto err;
    }

    if (marpaESLIFGrammarp->luabytep == NULL) {
      /* First time: duplicate luabytep if it is shallow */
      if (shallowb) {
        marpaESLIFGrammarp->luabytep = (char *) malloc(luabytel + 1);
        if (MARPAESLIF_UNLIKELY(marpaESLIFGrammarp->luabytep == NULL)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        memcpy(marpaESLIFGrammarp->luabytep, luabytep, luabytel);
        marpaESLIFGrammarp->luabytep[luabytel] = '\0';
      } else {
        marpaESLIFGrammarp->luabytep = luabytep;
        marpaESLIFGrammarp->luabytel = luabytel;
        shallowb = 1;
      }
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Setted lua script of size %ld luabytes", (unsigned long) luabytel);
    } else {
      /* Append bytes - they are guaranteed to be full character bytes, i.e. a raw concat of the buffer is ok */
      tmpl = marpaESLIFGrammarp->luabytel + luabytel;
      /* Turnaround ? */
      if (MARPAESLIF_UNLIKELY(tmpl < marpaESLIFGrammarp->luabytel)) {
        MARPAESLIF_ERROR(marpaESLIFp, "size_t turnaround detected");
        goto err;
      }
      tmps = (char *) realloc(marpaESLIFGrammarp->luabytep, tmpl + 1); /* Hiden NUL byte */
      if (MARPAESLIF_UNLIKELY(tmps == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }
      tmps[tmpl] = '\0';
      marpaESLIFGrammarp->luabytep = tmps;
      memcpy(marpaESLIFGrammarp->luabytep + marpaESLIFGrammarp->luabytel, luabytep, luabytel);
      marpaESLIFGrammarp->luabytel = tmpl;
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Appended lua script of size %ld bytes, full script size is now %ld", (unsigned long) luabytel, (unsigned long) marpaESLIFGrammarp->luabytel);
    }
  }

  MARPAESLIF_BOOTSTRAP_SET_UNDEF(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA /* context not used */);

  rcb = 1;
  goto done;

 err:
  if ((! shallowb) && (luabytep != NULL)) {
    free(luabytep);
  }

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_ifactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <if action> ::= 'if-action' '=>' <if action name> */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *names       = NULL;
  marpaESLIF_action_t *ifactionp   = NULL;
  char                *actions     = NULL;
  char                *tmps;
  short                luab;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, names);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(names == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "if-action at indice %d returned NULL", argni);
    goto err;
  }
  if (strstr(names, LUA_ACTION_PREFIX) == names) {
    /* lua action ? */
    tmps = names + strlen(LUA_ACTION_PREFIX);
    if (MARPAESLIF_UNLIKELY(*tmps == '\0')) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No lua identifier after %s", LUA_ACTION_PREFIX);
      goto err;
    }
    actions = strdup(tmps);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 1;
  } else {
    actions = strdup(names);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 0;
  }

  ifactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(ifactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (luab) {
    ifactionp->type = MARPAESLIF_ACTION_TYPE_LUA;
    ifactionp->u.luas = actions;
  } else {
    ifactionp->type = MARPAESLIF_ACTION_TYPE_NAME;
    ifactionp->u.names = actions;
  }
  actions = NULL; /* actions is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_IFACTION, ifactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(ifactionp);
  rcb = 0;

 done:
  if (actions != NULL) {
    free(actions);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_regexactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <regex action> ::= 'regex-action' '=>' <regex action name> */
  marpaESLIF_t        *marpaESLIFp    = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *names          = NULL;
  char                *actions        = NULL;
  marpaESLIF_action_t *regexactionp   = NULL;
  char                *tmps;
  short                luab;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, names);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(names == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "regex-action at indice %d returned NULL", argni);
    goto err;
  }
  if (strstr(names, LUA_ACTION_PREFIX) == names) {
    /* lua action ? */
    tmps = names + strlen(LUA_ACTION_PREFIX);
    if (MARPAESLIF_UNLIKELY(*tmps == '\0')) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No lua identifier after %s", LUA_ACTION_PREFIX);
      goto err;
    }
    actions = strdup(tmps);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 1;
  } else {
    actions = strdup(names);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 0;
  }

  regexactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(regexactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (luab) {
    regexactionp->type = MARPAESLIF_ACTION_TYPE_LUA;
    regexactionp->u.luas = actions;
  } else {
    regexactionp->type = MARPAESLIF_ACTION_TYPE_NAME;
    regexactionp->u.names = actions;
  }
  actions = NULL; /* actions is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_REGEXACTION, regexactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(regexactionp);
  rcb = 0;

 done:
  if (actions != NULL) {
    free(actions);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_eventactionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <event action> ::= 'event-action' '=>' <event action name> */
  marpaESLIF_t        *marpaESLIFp    = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *names          = NULL;
  marpaESLIF_action_t *eventactionp   = NULL;
  char                *actions        = NULL;
  char                *tmps;
  short                luab;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GET_ASCII(marpaESLIFValuep, argni, names);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(names == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "if-action at indice %d returned NULL", argni);
    goto err;
  }
  if (strstr(names, LUA_ACTION_PREFIX) == names) {
    /* lua action ? */
    tmps = names + strlen(LUA_ACTION_PREFIX);
    if (MARPAESLIF_UNLIKELY(*tmps == '\0')) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No lua identifier after %s", LUA_ACTION_PREFIX);
      goto err;
    }
    actions = strdup(tmps);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 1;
  } else {
    actions = strdup(names);
    if (MARPAESLIF_UNLIKELY(actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    luab = 0;
  }

  eventactionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
  if (MARPAESLIF_UNLIKELY(eventactionp == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  if (luab) {
    eventactionp->type = MARPAESLIF_ACTION_TYPE_LUA;
    eventactionp->u.luas = actions;
  } else {
    eventactionp->type = MARPAESLIF_ACTION_TYPE_NAME;
    eventactionp->u.names = actions;
  }
  actions = NULL; /* actions is now in actionp */

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_EVENTACTION, eventactionp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_action_freev(eventactionp);
  rcb = 0;

 done:
  if (actions != NULL) {
    free(actions);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_defaultencodingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <default encoding> ::= 'default-encoding' '=>' <default encoding name> */
  marpaESLIF_t        *marpaESLIFp      = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *defaultEncodings = NULL;
  short                shallowb         = 0;
  char                *tmps;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <default encoding name> is an ASCII string, guaranteed to NUL terminated as per the doc  */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ASCII(marpaESLIFValuep, argni, defaultEncodings, shallowb);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(defaultEncodings == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "default-encoding at indice %d returned NULL", argni);
    goto err;
  }
  /* Unshallow it if necessary */
  if (shallowb) {
    tmps = strdup(defaultEncodings);
    if (MARPAESLIF_UNLIKELY(tmps == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    defaultEncodings = tmps;
    shallowb = 0;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_DEFAULTENCODING, defaultEncodings);
  defaultEncodings = NULL;

  rcb = 1;
  goto done;

 err:
  if ((! shallowb) && (defaultEncodings != NULL)) {
    /* This is not possible in theory */
    free(defaultEncodings);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_fallbackencodingb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <fallback encoding> ::= 'fallback-encoding' '=>' <fallback encoding name> */
  marpaESLIF_t        *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                *fallbackEncodings = NULL;
  short                shallowb          = 0;
  char                *tmps;
  short                rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <fallback encoding name> is an ASCII string  */
  MARPAESLIF_BOOTSTRAP_GETANDFORGET_ASCII(marpaESLIFValuep, argni, fallbackEncodings, shallowb);
  /* It is a non-sense to not have no action in this case */
  if (MARPAESLIF_UNLIKELY(fallbackEncodings == NULL)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "fallback-encoding at indice %d returned NULL", argni);
    goto err;
  }
  /* Unshallow it if necessary */
  if (shallowb) {
    tmps = strdup(fallbackEncodings);
    if (MARPAESLIF_UNLIKELY(tmps == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    fallbackEncodings = tmps;
    shallowb = 0;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_FALLBACKENCODING, fallbackEncodings);
  fallbackEncodings = NULL;

  rcb = 1;
  goto done;

 err:
  if ((! shallowb) && (fallbackEncodings != NULL)) {
    /* This is not possible in theory */
    free(fallbackEncodings);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_lhs_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <lhs> ::= <symbol name> */
  marpaESLIF_t               *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                       *symbols     = NULL;
  marpaESLIF_bootstrap_lhs_t *lhsp        = NULL;
  short                       rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* symbols is an ASCII string that we pushed into a PTR that we own */
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, symbols);

  lhsp = (marpaESLIF_bootstrap_lhs_t *) malloc(sizeof(marpaESLIF_bootstrap_lhs_t));
  if (lhsp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  lhsp->parameterStackp = NULL;
  lhsp->symbols         = NULL;

  lhsp->symbols = strdup(symbols);
  if (lhsp->symbols == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS, lhsp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_lhs_freev(lhsp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_lhs_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <lhs> ::= <symbol name> '(' <identifiers> ')' */
  marpaESLIF_t               *marpaESLIFp = marpaESLIFValuep->marpaESLIFp; /*marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep))); */
  char                       *symbols     = NULL;
  marpaESLIF_bootstrap_lhs_t *lhsp        = NULL;
  marpaESLIFValueResultRow_t  row;
  size_t                      rowl;
  size_t                      row2l;
  char                       *tmps;
  short                       noparameterb;
  short                       rcb;

  /* Cannot be nullable */
  if (MARPAESLIF_UNLIKELY(nullableb)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* symbols is an ASCII string that we pushed into a PTR that we own */
  MARPAESLIF_BOOTSTRAP_GET_PTR(marpaESLIFValuep, arg0i, symbols);

  /* There may be no identifier */
  MARPAESLIF_BOOTSTRAP_IS_UNDEF(marpaESLIFValuep, arg0i + 2, noparameterb);
  if (! noparameterb) {
    MARPAESLIF_BOOTSTRAP_GET_ROW(marpaESLIFValuep, arg0i + 2, row);
    /* Every item in the row is an ASCII string */
    for (rowl = 0; rowl < row.sizel; rowl++) {
      if (MARPAESLIF_UNLIKELY(row.p[rowl].type != MARPAESLIF_VALUE_TYPE_STRING)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "In rule %s, argument No %ld is not STRING (got %d, %s)", symbols, (unsigned long) rowl, row.p[rowl].type, _marpaESLIF_value_types(row.p[rowl].type));
        goto err;
      }
    }
    /* Every parameter must unique */
    for (rowl = 0; rowl < row.sizel; rowl++) {
      for (row2l = 0; row2l < row.sizel; row2l++) {
        if (rowl == row2l) {
          continue;
        }
        if (strcmp(row.p[rowl].u.s.p, row.p[row2l].u.s.p) == 0) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "In rule %s, argument %s is not unique", symbols, row.p[rowl].u.s.p);
          goto err;
        }
      }
    }
  }

  lhsp = (marpaESLIF_bootstrap_lhs_t *) malloc(sizeof(marpaESLIF_bootstrap_lhs_t));
  if (lhsp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  lhsp->parameterStackp = NULL;
  lhsp->symbols         = NULL;

  GENERICSTACK_NEW(lhsp->parameterStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(lhsp->parameterStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFp, "lhsp->parameterStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  if (! noparameterb) {
    for (rowl = 0; rowl < row.sizel; rowl++) {
      tmps = strdup(row.p[rowl].u.s.p);
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      GENERICSTACK_PUSH_PTR(lhsp->parameterStackp, tmps);
      if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(lhsp->parameterStackp))) {
        MARPAESLIF_ERRORF(marpaESLIFp, "lhsp->parameterStackp push failure, %s", strerror(errno));
        goto err;
      }
    }
  }

  lhsp->symbols = strdup(symbols);
  if (lhsp->symbols == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }

  MARPAESLIF_BOOTSTRAP_SET_PTR(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS, lhsp);

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_lhs_freev(lhsp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_flatifyerb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char       *funcs                       = "_marpaESLIF_bootstrap_flatifyerb";
  marpaESLIFGrammar_t     *marpaESLIFGrammarp          = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIFValueResult_t *marpaESLIFValueResultClonep = NULL;
  short                   *cloneshallowbp;
  short                   *origshallowbp;
  short                    rcb;

  /* Take care: flatStackp will only contain allocated marpaESLIFValueResult's */
  /* It will never own the content.                                            */
  if (marpaESLIFGrammarp->flatStackp == NULL) {
    /* Make sure that the stack of lua expressions exist - Take care this is a stack inside Grammar structure */
    GENERICSTACK_NEW(marpaESLIFGrammarp->flatStackp);
    if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(marpaESLIFGrammarp->flatStackp))) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "marpaESLIFGrammarp->flatStackp initialization failure, %s", strerror(errno));
      goto err;
    }
  }

  switch (marpaESLIFValueResultp->type) {
    /* If you look to the actions, it is impossible to have something other than: */
    /* UNDEF                ignored          */
    /* STRING               ASCII identifier */
    /* ARRAY                bytes            */
    /* PTR                  Rrhs primary     */
    /* ROW                  ignored          */
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    /* Ignored */
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
  case MARPAESLIF_VALUE_TYPE_SHORT:
  case MARPAESLIF_VALUE_TYPE_INT:
  case MARPAESLIF_VALUE_TYPE_LONG:
  case MARPAESLIF_VALUE_TYPE_FLOAT:
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
  case MARPAESLIF_VALUE_TYPE_BOOL:
  case MARPAESLIF_VALUE_TYPE_LONG_DOUBLE:
#ifdef MARPAESLIF_HAVE_LONG_LONG
  case MARPAESLIF_VALUE_TYPE_LONG_LONG:
#endif
    MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "Unsupported type %d in argument stack", marpaESLIFValueResultp->type);
    goto err;

  case MARPAESLIF_VALUE_TYPE_PTR:
    marpaESLIFValueResultClonep = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
    if (marpaESLIFValueResultClonep == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    *marpaESLIFValueResultClonep = *marpaESLIFValueResultp;
    cloneshallowbp               = &(marpaESLIFValueResultClonep->u.p.shallowb);
    origshallowbp                = &(marpaESLIFValueResultp->u.p.shallowb);
    break;

  case MARPAESLIF_VALUE_TYPE_ARRAY:
    marpaESLIFValueResultClonep = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
    if (marpaESLIFValueResultClonep == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    *marpaESLIFValueResultClonep = *marpaESLIFValueResultp;
    cloneshallowbp               = &(marpaESLIFValueResultClonep->u.a.shallowb);
    origshallowbp                = &(marpaESLIFValueResultp->u.a.shallowb);
    break;

  case MARPAESLIF_VALUE_TYPE_STRING:
    marpaESLIFValueResultClonep = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
    if (marpaESLIFValueResultClonep == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    *marpaESLIFValueResultClonep = *marpaESLIFValueResultp;
    cloneshallowbp               = &(marpaESLIFValueResultClonep->u.s.shallowb);
    origshallowbp                = &(marpaESLIFValueResultp->u.s.shallowb);
    break;

  case MARPAESLIF_VALUE_TYPE_ROW:
    /* Ignored */
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    /* Ignored */
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "Unknown type %d in argument stack", marpaESLIFValueResultp->type);
    goto err;
  }

  if (marpaESLIFValueResultClonep != NULL) {
    GENERICSTACK_PUSH_PTR(marpaESLIFGrammarp->flatStackp, marpaESLIFValueResultClonep);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->flatStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "flatStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* We take the ownership in any case */
    *cloneshallowbp = 0;
    *origshallowbp  = 1;
    marpaESLIFValueResultClonep = NULL; /* marpaESLIFValueResultClonep is in flatStackp */
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (marpaESLIFValueResultClonep != NULL) {
    free(marpaESLIFValueResultClonep);
  }
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_bootstrap_flatStack_freev(genericStack_t *flatStackp)
/*****************************************************************************/
{
  int                      i;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;

  if (flatStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(flatStackp); i++) {
      /* Every item is a marpaESLIFValueResult allocated on the heap */
      if (GENERICSTACK_IS_PTR(flatStackp, i)) {
        marpaESLIFValueResultp = (marpaESLIFValueResult_t *) GENERICSTACK_GET_PTR(flatStackp, i);
        if (marpaESLIFValueResultp != NULL) {
          switch (marpaESLIFValueResultp->type) {
          case MARPAESLIF_VALUE_TYPE_PTR:
            if (! marpaESLIFValueResultp->u.p.shallowb) {
              /* We own this rhs primary */
              _marpaESLIF_bootstrap_rhs_primary_freev((marpaESLIF_bootstrap_rhs_primary_t *) marpaESLIFValueResultp->u.p.p);
            }
            break;
          case MARPAESLIF_VALUE_TYPE_STRING:
            if (! marpaESLIFValueResultp->u.s.shallowb) {
              _marpaESLIF_generic_freeCallbackv(NULL, marpaESLIFValueResultp);
            }
            break;
          case MARPAESLIF_VALUE_TYPE_ARRAY:
            if (! marpaESLIFValueResultp->u.a.shallowb) {
              _marpaESLIF_generic_freeCallbackv(NULL, marpaESLIFValueResultp);
            }
            break;
          default:
            /* Should never happen */
            break;
          }
          free(marpaESLIFValueResultp);
        }
      }
    }
    GENERICSTACK_FREE(flatStackp);
  }
}

