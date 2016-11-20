#ifndef MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H
#define MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H

typedef enum marpaESLIF_bootstrap_stack_context {
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PROPER,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RANK,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NULL_RANKING,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PRIORITY,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_PAUSE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LATM,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_NAMING,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SYMBOLACTION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_FREEACTION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVES,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_PRIORITIES,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_INACESSIBLE_TREATMENT,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ON_OR_OFF,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_QUANTIFIER
} marpaESLIF_bootstrap_stack_context_t;

typedef enum marpaESLIF_bootstrap_adverb_list_item_type {
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SYMBOLACTION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_FREEACTION
} marpaESLIF_bootstrap_adverb_list_item_type_t;

typedef enum marpaESLIF_bootstrap_pause {
  MARPAESLIF_BOOTSTRAP_PAUSE_NA = 0,
  MARPAESLIF_BOOTSTRAP_PAUSE_BEFORE,
  MARPAESLIF_BOOTSTRAP_PAUSE_AFTER
} marpaESLIF_bootstrap_null_pause_t;

typedef struct marpaESLIF_bootstrap_utf_string {
  char  *bytep;
  size_t bytel;
  char  *modifiers;
} marpaESLIF_bootstrap_utf_string_t;

typedef enum marpaESLIF_bootstrap_single_symbol_type {
  MARPAESLIF_SINGLE_SYMBOL_TYPE_NA = 0,
  MARPAESLIF_SINGLE_SYMBOL_TYPE_SYMBOL,
  MARPAESLIF_SINGLE_SYMBOL_TYPE_CHARACTER_CLASS,
  MARPAESLIF_SINGLE_SYMBOL_TYPE_REGULAR_EXPRESSION
} marpaESLIF_bootstrap_single_symbol_type_t;

typedef struct marpaESLIF_bootstrap_single_symbol {
  marpaESLIF_bootstrap_single_symbol_type_t type;
  union {
    char *symbols;
    marpaESLIF_bootstrap_utf_string_t *characterClassp;
    marpaESLIF_bootstrap_utf_string_t *regularExpressionp;
  } u;
} marpaESLIF_bootstrap_single_symbol_t;

typedef struct marpaESLIF_bootstrap_adverb_list_item {
  marpaESLIF_bootstrap_adverb_list_item_type_t type;
  union {
    char                                 *actions;
    short                                 left_associationb;
    short                                 right_associationb;
    short                                 group_associationb;
    marpaESLIF_bootstrap_single_symbol_t *separatorSingleSymbolp;
    short                                 properb;
    int                                   ranki;
    short                                 nullRanksHighb;
    int                                   priorityi;
    marpaESLIF_bootstrap_null_pause_t     pausei;
    short                                 latmb;
    marpaESLIF_bootstrap_utf_string_t    *namingp;
    char                                 *symbolactions;
    char                                 *freeactions;
  } u;
} marpaESLIF_bootstrap_adverb_list_item_t;

typedef enum marpaESLIF_bootstrap_rhs_primary_type {
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL,
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_QUOTED_STRING,
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_NAME_AND_REFERENCE
} marpaESLIF_bootstrap_rhs_primary_type_t;

typedef enum marpaESLIF_bootstrap_grammar_reference_type {
  MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING,
  MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER
} marpaESLIF_bootstrap_grammar_reference_type_t;

typedef struct marpaESLIF_bootstrap_grammar_reference {
  marpaESLIF_bootstrap_grammar_reference_type_t type;
  union {
    marpaESLIF_bootstrap_utf_string_t *quotedStringp;
    int                                signedIntegeri;
  } u;
} marpaESLIF_bootstrap_grammar_reference_t;

typedef struct marpaESLIF_bootstrap_symbol_name_and_reference {
  char                                     *symbols;
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep;
} marpaESLIF_bootstrap_symbol_name_and_reference_t;

typedef struct marpaESLIF_bootstrap_rhs_primary {
  marpaESLIF_bootstrap_rhs_primary_type_t type;
  union {
    marpaESLIF_bootstrap_single_symbol_t             *singleSymbolp;
    marpaESLIF_bootstrap_utf_string_t                *quotedStringp;
    marpaESLIF_bootstrap_symbol_name_and_reference_t *symbolNameAndReferencep;
  } u;
} marpaESLIF_bootstrap_rhs_primary_t;

typedef struct marpaESLIF_bootstrap_alternative {
  genericStack_t      *rhsPrimaryStackp;
  genericStack_t      *adverbListItemStackp;
  int                  priorityi;         /* Used when there is the loosen "||" operator */
  marpaESLIF_symbol_t *forcedLhsp;        /* ditto */
} marpaESLIF_bootstrap_alternative_t;

typedef enum marpaESLIF_bootstrap_inaccessible_treatment_type {
  MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_WARN = 0,
  MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_OK,
  MARPAESLIF_BOOTSTRAP_INACCESSIBLE_TREATMENT_TYPE_FATAL
} marpaESLIF_bootstrap_inaccessible_treatment_type_t;

typedef enum marpaESLIF_bootstrap_on_or_off_type {
  MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_ON = 0,
  MARPAESLIF_BOOTSTRAP_ON_OR_OFF_TYPE_OFF
} marpaESLIF_bootstrap_on_or_off_type_t;

#endif /* MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H */

