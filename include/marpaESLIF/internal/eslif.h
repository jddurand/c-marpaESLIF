#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

#define INTERNAL_DEFAULT_SYMBOL_LEXEME_ACTION "transfert_to_outputStack"
#define INTERNAL_RULE_OP_DECLARE_ANY_GRAMMAR  "op_declare_any_grammar"
#define INTERNAL_RULE_ASCII_GRAPH_NAME        "ascii_graph_name"

typedef struct bootstrap_grammar_terminal {
  int                        idi;                 /* Identifier */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  marpaESLIF_uint32_t        optioni;             /* Terminal option */
  char                      *utf8s;               /* ASCII UTF-8 compatible encoding */
  char                      *testFullMatchs;
  char                      *testPartialMatchs;
} bootstrap_grammar_terminal_t;

typedef struct bootstrap_grammar_meta {
  int    idi;                  /* Identifier */
  char  *descs;                /* Description */
  short  startb;               /* Start symbol ? */
  short  discardb;             /* Discard symbol ? */
  char  *defaultSymbolactions; /* Default symbol action */
  char  *defaultRuleactions;   /* Default rule action */
} bootstrap_grammar_meta_t;

typedef enum bootstrap_grammar_rule_type {
  MARPAESLIF_RULE_TYPE_ALTERNATIVE,
  MARPAESLIF_RULE_TYPE_SEQUENCE
} bootstrap_grammar_rule_type_t;

typedef struct bootstrap_grammar_rule {
  int                           lhsi;
  char                         *descs;
  bootstrap_grammar_rule_type_t type;
  size_t                        nrhsl;
  int                           rhsip[10];  /* 10 is arbitrary - just to make C happy */
  short                         maskbp[10]; /* Ditto */
  int                           minimumi;
  int                           separatori;
  short                         properb;
  char                         *actions;
} bootstrap_grammar_rule_t;

#include "marpaESLIF/internal/eslif/L0.h"
#include "marpaESLIF/internal/eslif/G1.h"

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
