#ifndef MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H
#define MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H

typedef enum marpaESLIF_bootstrap_stack_context {
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ACTION,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE
} marpaESLIF_bootstrap_stack_context_t;

typedef enum marpaESLIF_bootstrap_adverb_list_item_type {
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_AUTORANK,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM,
  MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING
} marpaESLIF_bootstrap_adverb_list_item_type_t;

typedef enum marpaESLIF_bootstrap_null_ranking {
  MARPAESLIF_BOOTSTRAP_NULL_RANKING_NA = 0,
  MARPAESLIF_BOOTSTRAP_NULL_RANKING_LOW,
  MARPAESLIF_BOOTSTRAP_NULL_RANKING_HIGHT
} marpaESLIF_bootstrap_null_ranking_t;

typedef enum marpaESLIF_bootstrap_pause {
  MARPAESLIF_BOOTSTRAP_PAUSE_NA = 0,
  MARPAESLIF_BOOTSTRAP_PAUSE_BEFORE,
  MARPAESLIF_BOOTSTRAP_PAUSE_AFTER
} marpaESLIF_bootstrap_null_pause_t;

typedef struct marpaESLIF_bootstrap_utf_string {
  char  *bytep;
  size_t bytel;
} marpaESLIF_bootstrap_utf_string_t;

typedef struct marpaESLIF_bootstrap_adverb_list_item {
  marpaESLIF_bootstrap_adverb_list_item_type_t type;
  union {
    char                               *actions;
    short                               autorankb;
    short                               leftb;
    short                               rightb;
    short                               groupb;
    char                               *separators;
    short                               properb;
    int                                 ranki;
    marpaESLIF_bootstrap_null_ranking_t nullrankingi;
    int                                 priorityi;
    marpaESLIF_bootstrap_null_pause_t   pausei;
    short                               latmb;
    marpaESLIF_bootstrap_utf_string_t   naming;
  } u;
} marpaESLIF_bootstrap_adverb_list_item_t;

typedef enum marpaESLIF_bootstrap_rhs_primary_type {
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA = 0,
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL,
  MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_QUOTED_STRING
} marpaESLIF_bootstrap_rhs_primary_type_t;

typedef struct marpaESLIF_bootstrap_rhs_primary_symbol {
  char                              *symbols;
  marpaESLIF_bootstrap_utf_string_t *lookupGrammarStringp;
  int                                lookupLevelDeltai;
} marpaESLIF_bootstrap_rhs_primary_symbol_t;

typedef struct marpaESLIF_bootstrap_rhs_primary {
  marpaESLIF_bootstrap_rhs_primary_type_t type;
  union {
    marpaESLIF_bootstrap_rhs_primary_symbol_t *symbolp;
    marpaESLIF_bootstrap_utf_string_t         *quotedStringp;
  } u;
} marpaESLIF_bootstrap_rhs_primary_t;

typedef struct marpaESLIF_bootstrap_alternative {
  genericStack_t *rhsPrimaryStackp;
  genericStack_t *adverbListItemStackp;
} marpaESLIF_bootstrap_alternative_t;

#endif /* MARPAESLIF_INTERNAL_BOOTSTRAP_TYPES_H */

