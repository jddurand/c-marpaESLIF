#ifndef MARPAESLIF_INTERNAL_ESLIF_G1_H
#define MARPAESLIF_INTERNAL_ESLIF_G1_H

#include "marpaESLIF/internal/eslif/L0_join_G1.h"

/* Description of internal G1 grammar */

/* It is very important here to list all the terminals first, and in order compatible */
/* with bootstrap_grammar_G1_terminals[] and bootstrap_grammar_G1_rules[] */
typedef enum bootstrap_grammar_G1_enum {
  G1_TERMINAL__START = 0,
  G1_TERMINAL_SEMICOLON,
  G1_TERMINAL_LEFT_BRACKET,
  G1_TERMINAL_RIGHT_BRACKET,
  G1_TERMINAL__DISCARD,
  G1_TERMINAL__DEFAULT,
  G1_TERMINAL_LEXEME,
  G1_TERMINAL_DEFAULT,
  G1_TERMINAL_EQUAL,
  G1_TERMINAL_DISCARD,
  G1_TERMINAL__LEXEME,
  G1_TERMINAL_EVENT,
  G1_TERMINAL_COMPLETED,
  G1_TERMINAL_NULLED,
  G1_TERMINAL_PREDICTED,
  G1_TERMINAL_CURRENT,
  G1_TERMINAL_LEXER,
  G1_TERMINAL_IS,
  G1_TERMINAL_INACCESSIBLE,
  G1_TERMINAL_BY,
  G1_TERMINAL_WARN,
  G1_TERMINAL_OK,
  G1_TERMINAL_FATAL,
  G1_TERMINAL_MINUS,
  G1_TERMINAL_ACTION,
  G1_TERMINAL_THEN,
  G1_TERMINAL_AUTORANK,
  G1_TERMINAL_ASSOC,
  G1_TERMINAL_LEFT,
  G1_TERMINAL_RIGHT,
  G1_TERMINAL_GROUP,
  G1_TERMINAL_SEPARATOR,
  G1_TERMINAL_PROPER,
  G1_TERMINAL_RANK,
  G1_TERMINAL_NULL_RANKING,
  G1_TERMINAL_NULL,
  G1_TERMINAL_LOW,
  G1_TERMINAL_HIGH,
  G1_TERMINAL_PRIORITY,
  G1_TERMINAL_PAUSE,
  G1_TERMINAL_ON,
  G1_TERMINAL_OFF,
  G1_TERMINAL_FORGIVING,
  G1_TERMINAL_LATM,
  G1_TERMINAL_BLESS,
  G1_TERMINAL_NAME,
  G1_TERMINAL_COMMA,
  G1_TERMINAL_LPAREN,
  G1_TERMINAL_RPAREN,
  G1_TERMINAL_STAR,
  G1_TERMINAL_PLUS,
  /* ----- Non terminals ------ */
  G1_META_STATEMENTS,
  G1_META_STATEMENT,
  G1_META_START_RULE,
  G1_META_EMPTY_RULE,
  G1_META_NULL_STATEMENT,
  G1_META_STATEMENT_GROUP,
  G1_META_PRIORITY_RULE,
  G1_META_QUANTIFIED_RULE,
  G1_META_DISCARD_RULE,
  G1_META_DEFAULT_RULE,
  G1_META_LEXEME_DEFAULT_STATEMENT,
  G1_META_DISCARD_DEFAULT_STATEMENT,
  G1_META_LEXEME_RULE,
  G1_META_COMPLETION_EVENT_DECLARATION,
  G1_META_NULLED_EVENT_DECLARATION,
  G1_META_PREDICTION_EVENT_DECLARATION,
  G1_META_CURRENT_LEXER_STATEMENT,
  G1_META_INACCESSIBLE_STATEMENT,
  G1_META_INACCESSIBLE_TREATMENT,
  G1_META_EXCEPTION_STATEMENT,
  G1_META_OP_DECLARE,
  G1_META_OP_DECLARE_ANY_GRAMMAR,
  G1_META_OP_DECLARE_TOP_GRAMMAR,
  G1_META_OP_DECLARE_LEX_GRAMMAR,
  G1_META_OP_LOOSEN,
  G1_META_OP_EQUAL_PRIORITY,
  G1_META_PRIORITIES,
  G1_META_ALTERNATIVES,
  G1_META_ALTERNATIVE,
  G1_META_ADVERB_LIST,
  G1_META_ADVERB_LIST_ITEMS,
  G1_META_ADVERB_ITEM,
  G1_META_ACTION,
  G1_META_AUTORANK,
  G1_META_LEFT_ASSOCIATION,
  G1_META_RIGHT_ASSOCIATION,
  G1_META_GROUP_ASSOCIATION,
  G1_META_SEPARATOR_ASSOCIATION,
  G1_META_PROPER_ASSOCIATION,
  G1_META_RANK_SPECIFICATION,
  G1_META_NULL_RANKING_SPECIFICATION,
  G1_META_NULL_RANKING_CONSTANT,
  G1_META_PRIORITY_SPECIFICATION,
  G1_META_PAUSE_SPECIFICATION,
  G1_META_EVENT_SPECIFICATION,
  G1_META_EVENT_INITIALIZATION,
  G1_META_EVENT_INITIALIZER,
  G1_META_ON_OR_OFF,
  G1_META_LATM_SPECIFICATION,
  G1_META_BLESSING,
  G1_META_BLESSING_NAME,
  G1_META_NAMING,
  G1_META_NULL_ADVERB,
  G1_META_ALTERNATIVE_NAME,
  G1_META_LEXER_NAME,
  G1_META_EVENT_NAME,
  G1_META_LHS,
  G1_META_RHS,
  G1_META_RHS_PRIMARY,
  G1_META_PARENTHESIZED_RHS_PRIMARY_LIST,
  G1_META_RHS_PRIMARY_LIST,
  G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST,
  G1_META_RHS_EXCEPTION_LIST,
  G1_META_RHS_EXCEPTION,
  G1_META_SINGLE_SYMBOL,
  G1_META_SYMBOL,
  G1_META_SYMBOL_NAME,
  G1_META_ACTION_NAME,
  G1_META_QUANTIFIER,
  G1_META_DISCARD,
  /* These meta identifiers are handled by L0 */
  G1_META_BOOLEAN,
  G1_META_SIGNED_INTEGER,
  G1_META_BEFORE_OR_AFTER,
  G1_META_TRUE,
  G1_META_STANDARD_NAME,
  G1_META_RESERVED_BLESSING_NAME,
  G1_META_QUOTED_NAME,
  G1_META_RESERVED_EVENT_NAME,
  G1_META_QUOTED_STRING,
  G1_META_CHARACTER_CLASS,
  G1_META_REGULAR_EXPRESSION,
  G1_META_SUBSTITUTION_EXPRESSION,
  G1_META_BARE_NAME,
  G1_META_BRACKETED_NAME,
  G1_META_PERL_NAME,
  G1_META_RESERVED_ACTION_NAME,
  G1_META_ARRAY_DESCRIPTOR,
  G1_META_WHITESPACE,
  G1_META_PERL_COMMENT,
  G1_META_CPLUSPLUS_COMMENT,
  G1_META_C_COMMENT
} bootstrap_grammar_G1_enum_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_G1_metas[] = {
  { G1_META_STATEMENTS,                       "<meta statements>", 1 },
  { G1_META_STATEMENT,                        "<meta statement>", 0 },
  { G1_META_START_RULE,                       "<meta start rule>", 0 },
  { G1_META_EMPTY_RULE,                       "<meta empty rule>", 0 },
  { G1_META_NULL_STATEMENT,                   "<meta null statement>", 0 },
  { G1_META_STATEMENT_GROUP,                  "<meta statement group>", 0 },
  { G1_META_PRIORITY_RULE,                    "<meta priority rule>", 0 },
  { G1_META_QUANTIFIED_RULE,                  "<meta quantified rule>", 0 },
  { G1_META_DISCARD_RULE,                     "<meta discard rule>", 0 },
  { G1_META_DEFAULT_RULE,                     "<meta default rule>", 0 },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         "<meta lexeme default statement>", 0 },
  { G1_META_DISCARD_DEFAULT_STATEMENT,        "<meta discard default statement>", 0 },
  { G1_META_LEXEME_RULE,                      "<meta lexeme rule>", 0 },
  { G1_META_COMPLETION_EVENT_DECLARATION,     "<meta completion event declaration>", 0 },
  { G1_META_NULLED_EVENT_DECLARATION,         "<meta nulled event declaration>", 0 },
  { G1_META_PREDICTION_EVENT_DECLARATION,     "<meta prediction event declaration>", 0 },
  { G1_META_CURRENT_LEXER_STATEMENT,          "<meta current lexer statement>", 0 },
  { G1_META_INACCESSIBLE_STATEMENT,           "<meta inaccessible statement>", 0 },
  { G1_META_INACCESSIBLE_TREATMENT,           "<meta inaccessible treatment>", 0 },
  { G1_META_EXCEPTION_STATEMENT,              "<meta exception statement>", 0 },
  { G1_META_OP_DECLARE,                       "<meta op declare>", 0 },
  { G1_META_OP_DECLARE_ANY_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_ANY_GRAMMAR, 0 },
  { G1_META_OP_DECLARE_TOP_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_TOP_GRAMMAR, 0 },
  { G1_META_OP_DECLARE_LEX_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_LEX_GRAMMAR, 0 },
  { G1_META_OP_LOOSEN,                        L0_JOIN_G1_META_OP_LOOSEN, 0 },
  { G1_META_OP_EQUAL_PRIORITY,                L0_JOIN_G1_META_OP_EQUAL_PRIORITY, 0 },
  { G1_META_PRIORITIES,                       "<meta priorities>", 0 },
  { G1_META_ALTERNATIVES,                     "<meta alternatives>", 0 },
  { G1_META_ALTERNATIVE,                      "<meta alternative>", 0 },
  { G1_META_ADVERB_LIST,                      "<meta adverb list>", 0 },
  { G1_META_ADVERB_LIST_ITEMS,                "<meta adverb list items>", 0 },
  { G1_META_ADVERB_ITEM,                      "<meta adverb item>", 0 },
  { G1_META_ACTION,                           "<meta action>", 0 },
  { G1_META_AUTORANK,                         "<meta autorank>", 0 },
  { G1_META_LEFT_ASSOCIATION,                 "<meta left association>", 0 },
  { G1_META_RIGHT_ASSOCIATION,                "<meta right association>", 0 },
  { G1_META_GROUP_ASSOCIATION,                "<meta group association>", 0 },
  { G1_META_SEPARATOR_ASSOCIATION,            "<meta separator association>", 0 },
  { G1_META_PROPER_ASSOCIATION,               "<meta proper association>", 0 },
  { G1_META_RANK_SPECIFICATION,               "<meta rank specification>", 0 },
  { G1_META_NULL_RANKING_SPECIFICATION,       "<meta null ranking specification>", 0 },
  { G1_META_NULL_RANKING_CONSTANT,            "<meta null ranking constant>", 0 },
  { G1_META_PRIORITY_SPECIFICATION,           "<meta priority specification>", 0 },
  { G1_META_PAUSE_SPECIFICATION,              "<meta pause specification>", 0 },
  { G1_META_EVENT_SPECIFICATION,              "<meta event specification>", 0 },
  { G1_META_EVENT_INITIALIZATION,             "<meta event initialization>", 0 },
  { G1_META_EVENT_INITIALIZER,                "<meta event initializer>", 0 },
  { G1_META_ON_OR_OFF,                        "<meta on or off>", 0 },
  { G1_META_LATM_SPECIFICATION,               "<meta latm specification>", 0 },
  { G1_META_BLESSING,                         "<meta blessing>", 0 },
  { G1_META_BLESSING_NAME,                    "<meta blessing name>", 0 },
  { G1_META_NAMING,                           "<meta naming>", 0 },
  { G1_META_NULL_ADVERB,                      "<meta null adverb>", 0 },
  { G1_META_ALTERNATIVE_NAME,                 "<meta alternative name>", 0 },
  { G1_META_LEXER_NAME,                       "<meta lexer name>", 0 },
  { G1_META_EVENT_NAME,                       "<meta event name>", 0 },
  { G1_META_LHS,                              "<meta lhs>", 0 },
  { G1_META_RHS,                              "<meta rhs>", 0 },
  { G1_META_RHS_PRIMARY,                      "<meta rhs primary>", 0 },
  { G1_META_PARENTHESIZED_RHS_PRIMARY_LIST,   "<meta parenthesized rhs primary list>", 0 },
  { G1_META_RHS_PRIMARY_LIST,                 "<meta rhs primary list>", 0 },
  { G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST, "<meta parenthesized rhs exception list>", 0 },
  { G1_META_RHS_EXCEPTION_LIST,               "<meta rhs exception list>", 0 },
  { G1_META_RHS_EXCEPTION,                    "<meta rhs exception>", 0 },
  { G1_META_SINGLE_SYMBOL,                    "<meta single symbol>", 0 },
  { G1_META_SYMBOL,                           "<meta symbol>", 0 },
  { G1_META_SYMBOL_NAME,                      "<meta symbol name>", 0 },
  { G1_META_ACTION_NAME,                      "<meta action name>", 0 },
  { G1_META_QUANTIFIER,                       "<meta quantifier>", 0 },
  { G1_META_DISCARD,                          "<meta discard>", 0 },
  /* L0 join */
  { G1_META_BOOLEAN,                          L0_JOIN_G1_META_BOOLEAN, 0 },
  { G1_META_SIGNED_INTEGER,                   L0_JOIN_G1_META_SIGNED_INTEGER, 0 },
  { G1_META_BEFORE_OR_AFTER,                  L0_JOIN_G1_META_BEFORE_OR_AFTER, 0 },
  { G1_META_TRUE,                             L0_JOIN_G1_META_TRUE, 0 },
  { G1_META_STANDARD_NAME,                    L0_JOIN_G1_META_STANDARD_NAME, 0 },
  { G1_META_RESERVED_BLESSING_NAME,           L0_JOIN_G1_META_RESERVED_BLESSING_NAME, 0 },
  { G1_META_QUOTED_NAME,                      L0_JOIN_G1_META_QUOTED_NAME, 0 },
  { G1_META_RESERVED_EVENT_NAME,              L0_JOIN_G1_META_RESERVED_EVENT_NAME, 0 },
  { G1_META_QUOTED_STRING,                    L0_JOIN_G1_META_QUOTED_STRING, 0 },
  { G1_META_CHARACTER_CLASS,                  L0_JOIN_G1_META_CHARACTER_CLASS, 0 },
  { G1_META_REGULAR_EXPRESSION,               L0_JOIN_G1_META_REGULAR_EXPRESSION, 0 },
  { G1_META_SUBSTITUTION_EXPRESSION,          L0_JOIN_G1_META_SUBSTITUTION_EXPRESSION, 0 },
  { G1_META_BARE_NAME,                        L0_JOIN_G1_META_BARE_NAME, 0 },
  { G1_META_BRACKETED_NAME,                   L0_JOIN_G1_META_BRACKETED_NAME, 0 },
  { G1_META_PERL_NAME,                        L0_JOIN_G1_META_PERL_NAME, 0 },
  { G1_META_RESERVED_ACTION_NAME,             L0_JOIN_G1_META_RESERVED_ACTION_NAME, 0 },
  { G1_META_ARRAY_DESCRIPTOR,                 L0_JOIN_G1_META_ARRAY_DESCRIPTOR, 0 },
  { G1_META_WHITESPACE,                       L0_JOIN_G1_META_WHITESPACE, 0 },
  { G1_META_PERL_COMMENT,                     L0_JOIN_G1_META_PERL_COMMENT, 0 },
  { G1_META_CPLUSPLUS_COMMENT,                L0_JOIN_G1_META_CPLUSPLUS_COMMENT, 0 },
  { G1_META_C_COMMENT,                        L0_JOIN_G1_META_C_COMMENT, 0 }
};

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_terminal_t bootstrap_grammar_G1_terminals[] = {
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /*                                                             TERMINALS                                                             */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { G1_TERMINAL__START,
    "<terminal :start>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":start", NULL,
#ifndef MARPAESLIF_NTRACE
    ":start", ":sta"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_SEMICOLON,
    "<terminal ;>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ";", NULL,
#ifndef MARPAESLIF_NTRACE
    ";", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEFT_BRACKET,
    "<terminal {>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "{", NULL,
#ifndef MARPAESLIF_NTRACE
    "{", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RIGHT_BRACKET,
    "<terminal }>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "}", NULL,
#ifndef MARPAESLIF_NTRACE
    "}", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__DISCARD,
    "<terminal :discard>",        MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":discard", NULL,
#ifndef MARPAESLIF_NTRACE
    ":discard", ":dis"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__DEFAULT,
    "<terminal :default>",        MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":default", NULL,
#ifndef MARPAESLIF_NTRACE
    ":default", ":def"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEXEME,
    "<terminal lexeme>",          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lexeme", NULL,
#ifndef MARPAESLIF_NTRACE
    "lexeme", "lexe"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_DEFAULT,
    "<terminal default>",         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "default", NULL,
#ifndef MARPAESLIF_NTRACE
    "default", "def"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_EQUAL,
    "<terminal equal>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "=", NULL,
#ifndef MARPAESLIF_NTRACE
    "=", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_DISCARD,
    "<terminal discard>",         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "discard", NULL,
#ifndef MARPAESLIF_NTRACE
    "discard", "dis"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__LEXEME,
    "<terminal :lexeme>",         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":lexeme", NULL,
#ifndef MARPAESLIF_NTRACE
    ":lexeme", ":lexe"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_EVENT,
    "<terminal event>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "event", NULL,
#ifndef MARPAESLIF_NTRACE
    "event", "eve"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_COMPLETED,
    "<terminal completed>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "completed", NULL,
#ifndef MARPAESLIF_NTRACE
    "completed", "comp"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULLED,
    "<terminal nulled>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "nulled", NULL,
#ifndef MARPAESLIF_NTRACE
    "nulled", "nul"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PREDICTED,
    "<terminal predicted>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "predicted", NULL,
#ifndef MARPAESLIF_NTRACE
    "predicted", "pre"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_CURRENT,
    "<terminal current>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "current", NULL,
#ifndef MARPAESLIF_NTRACE
    "current", "cur"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEXER,
    "<terminal lexer>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lexer", NULL,
#ifndef MARPAESLIF_NTRACE
    "lexer", "lex"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_IS,
    "<terminal is>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "is", NULL,
#ifndef MARPAESLIF_NTRACE
    "is", "i"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_INACCESSIBLE,
    "<terminal inaccessible>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "inaccessible", NULL,
#ifndef MARPAESLIF_NTRACE
    "inaccessible", "inac"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_BY,
    "<terminal by>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "by", NULL,
#ifndef MARPAESLIF_NTRACE
    "by", "b"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_WARN,
    "<terminal warn>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "warn", NULL,
#ifndef MARPAESLIF_NTRACE
    "warn", "wa"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_OK,
    "<terminal ok>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "ok", NULL,
#ifndef MARPAESLIF_NTRACE
    "ok", "o"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_FATAL,
    "<terminal fatal>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "fatal", NULL,
#ifndef MARPAESLIF_NTRACE
    "fatal", "fata"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_MINUS,
    "<terminal ->",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "-", NULL,
#ifndef MARPAESLIF_NTRACE
    "-", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ACTION,
    "<terminal action>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "action", NULL,
#ifndef MARPAESLIF_NTRACE
    "action", "act"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_THEN,
    "<terminal =>>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "=>", NULL,
#ifndef MARPAESLIF_NTRACE
    "=>", "="
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_AUTORANK,
    "<terminal autorank>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "autorank", NULL,
#ifndef MARPAESLIF_NTRACE
    "autorank", "auto"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ASSOC,
    "<terminal assoc>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "assoc", NULL,
#ifndef MARPAESLIF_NTRACE
    "assoc", "asso"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEFT,
    "<terminal left>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "left", NULL,
#ifndef MARPAESLIF_NTRACE
    "left", "lef"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RIGHT,
    "<terminal right>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "right", NULL,
#ifndef MARPAESLIF_NTRACE
    "right", "r"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_GROUP,
    "<terminal group>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "group", NULL,
#ifndef MARPAESLIF_NTRACE
    "group", "gr"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_SEPARATOR,
    "<terminal separator>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "separator", NULL,
#ifndef MARPAESLIF_NTRACE
    "separator", "sep"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PROPER,
    "<terminal proper>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "proper", NULL,
#ifndef MARPAESLIF_NTRACE
    "proper", "pro"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RANK,
    "<terminal rank>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rank", NULL,
#ifndef MARPAESLIF_NTRACE
    "rank", "ra"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULL_RANKING,
    "<terminal null-ranking>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "null-ranking", NULL,
#ifndef MARPAESLIF_NTRACE
    "null-ranking", "null-"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULL,
    "<terminal null>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "null", NULL,
#ifndef MARPAESLIF_NTRACE
    "null", "nul"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LOW,
    "<terminal low>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "low", NULL,
#ifndef MARPAESLIF_NTRACE
    "low", "lo"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_HIGH,
    "<terminal high>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "high", NULL,
#ifndef MARPAESLIF_NTRACE
    "high", "hi"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PRIORITY,
    "<terminal priority>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "priority", NULL,
#ifndef MARPAESLIF_NTRACE
    "priority", "prio"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PAUSE,
    "<terminal pause>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "pause", NULL,
#ifndef MARPAESLIF_NTRACE
    "pause", "pa"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ON,
    "<terminal on>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "on", NULL,
#ifndef MARPAESLIF_NTRACE
    "on", "o"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_OFF,
    "<terminal off>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "off", NULL,
#ifndef MARPAESLIF_NTRACE
    "off", "of"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_FORGIVING,
    "<terminal forgiving>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "forgiving", NULL,
#ifndef MARPAESLIF_NTRACE
    "forgiving", "for"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LATM,
    "<terminal latm>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "latm", NULL,
#ifndef MARPAESLIF_NTRACE
    "latm", "lat"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_BLESS,
    "<terminal bless>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "bless", NULL,
#ifndef MARPAESLIF_NTRACE
    "bless", "bl"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NAME,
    "<terminal name>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name", NULL,
#ifndef MARPAESLIF_NTRACE
    "name", "nam"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_COMMA,
    "<terminal ,>",          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",", NULL,
#ifndef MARPAESLIF_NTRACE
    ",", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LPAREN,
    "<terminal (>",          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "(", NULL,
#ifndef MARPAESLIF_NTRACE
    "(", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RPAREN,
    "<terminal )>",          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ")", NULL,
#ifndef MARPAESLIF_NTRACE
    ")", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_STAR,
    "<terminal *>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "*", NULL,
#ifndef MARPAESLIF_NTRACE
    "*", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PLUS,
    "<terminal +>",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "+", NULL,
#ifndef MARPAESLIF_NTRACE
    "+", ""
#else
    NULL, NULL
#endif
  }
};

bootstrap_grammar_rule_t bootstrap_grammar_G1_rules[] = {
  { G1_META_STATEMENTS,                       "<rule statements>",                         MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_STATEMENT                            }, { 1 },        0,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 01>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_START_RULE                           }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 02>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_EMPTY_RULE                           }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 03>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_STATEMENT                       }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 04>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STATEMENT_GROUP                      }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 05>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PRIORITY_RULE                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 06>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUANTIFIED_RULE                      }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 07>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DISCARD_RULE                         }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 08>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DEFAULT_RULE                         }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 09>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEXEME_DEFAULT_STATEMENT             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 10>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DISCARD_DEFAULT_STATEMENT            }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 11>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEXEME_RULE                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 12>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_COMPLETION_EVENT_DECLARATION         }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 13>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULLED_EVENT_DECLARATION             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 14>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PREDICTION_EVENT_DECLARATION         }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 15>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CURRENT_LEXER_STATEMENT              }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 16>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_INACCESSIBLE_STATEMENT               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT,                        "<rule statement 17>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_EXCEPTION_STATEMENT                  }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_START_RULE,                       "<rule start rule>",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL__START,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_SYMBOL                               }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_EMPTY_RULE,                       "<rule empty rule>",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_META_LHS,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_ADVERB_LIST                          }, { 1, 1, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_NULL_STATEMENT,                   "<rule null statement>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_SEMICOLON                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_STATEMENT_GROUP,                  "<rule statement group>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LEFT_BRACKET,
                                                                                                                                  G1_META_STATEMENTS,
                                                                                                                                  G1_TERMINAL_RIGHT_BRACKET                    }, { 0, 1, 0 }, -1,                   -1,      -1, 0 },
  { G1_META_PRIORITY_RULE,                    "<rule priority rule>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_META_LHS,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_PRIORITIES                           }, { 1, 1, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_QUANTIFIED_RULE,                  "<rule quantified rule>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_META_LHS,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_SINGLE_SYMBOL,
                                                                                                                                  G1_META_QUANTIFIER,
                                                                                                                                  G1_META_ADVERB_LIST                          }, {1,1,1,1,1 },-1,                   -1,      -1, 0 },
  { G1_META_DISCARD_RULE,                     "<rule discard rule>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL__DISCARD,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_SINGLE_SYMBOL,
                                                                                                                                  G1_META_ADVERB_LIST                          }, {0,0,1,1 },  -1,                   -1,      -1, 0 },
  { G1_META_DEFAULT_RULE,                     "<rule default rule>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL__DEFAULT,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_ADVERB_LIST                          }, { 1, 1, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         "<rule lexeme default statement>",           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_LEXEME,
                                                                                                                                  G1_TERMINAL_DEFAULT,
                                                                                                                                  G1_TERMINAL_EQUAL,
                                                                                                                                  G1_META_ADVERB_LIST                          }, {0,0,0,1 },  -1,                   -1,      -1, 0 },
  { G1_META_DISCARD_DEFAULT_STATEMENT,        "<rule discard default statement>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_DISCARD,
                                                                                                                                  G1_TERMINAL_DEFAULT,
                                                                                                                                  G1_TERMINAL_EQUAL,
                                                                                                                                  G1_META_ADVERB_LIST                          }, {0,0,0,1 },  -1,                   -1,      -1, 0 },
  { G1_META_LEXEME_RULE,                      "<rule lexeme rule>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL__LEXEME,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_SYMBOL,
                                                                                                                                  G1_META_ADVERB_LIST                          }, {0,0,1,1 },  -1,                   -1,      -1, 0 },
  { G1_META_COMPLETION_EVENT_DECLARATION,     "<rule completion event declaration>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                  G1_META_EVENT_INITIALIZATION,
                                                                                                                                  G1_TERMINAL_EQUAL,
                                                                                                                                  G1_TERMINAL_COMPLETED,
                                                                                                                                  G1_META_SYMBOL_NAME                          }, {0,1,0,0,1}, -1,                   -1,      -1, 0 },
  { G1_META_NULLED_EVENT_DECLARATION,         "<rule nulled event declaration>",           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                  G1_META_EVENT_INITIALIZATION,
                                                                                                                                  G1_TERMINAL_EQUAL,
                                                                                                                                  G1_TERMINAL_NULLED,
                                                                                                                                  G1_META_SYMBOL_NAME                          }, {0,1,0,0,1}, -1,                   -1,      -1, 0 },
  { G1_META_PREDICTION_EVENT_DECLARATION,     "<rule nulled predicted declaration>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                  G1_META_EVENT_INITIALIZATION,
                                                                                                                                  G1_TERMINAL_EQUAL,
                                                                                                                                  G1_TERMINAL_PREDICTED,
                                                                                                                                  G1_META_SYMBOL_NAME                          }, {0,1,0,0,1}, -1,                   -1,      -1, 0 },
  { G1_META_CURRENT_LEXER_STATEMENT,          "<rule current lexer statement>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_CURRENT,
                                                                                                                                  G1_TERMINAL_LEXER,
                                                                                                                                  G1_TERMINAL_IS,
                                                                                                                                  G1_META_LEXER_NAME                           }, {0,0,0,1},   -1,                   -1,      -1, 0 },
  { G1_META_INACCESSIBLE_STATEMENT,           "<rule inaccessible statement>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_INACCESSIBLE,
                                                                                                                                  G1_TERMINAL_IS,
                                                                                                                                  G1_META_INACCESSIBLE_TREATMENT,
                                                                                                                                  G1_TERMINAL_BY,
                                                                                                                                  G1_TERMINAL_DEFAULT                          }, {0,0,1,0,0}, -1,                   -1,      -1, 0 },
  { G1_META_INACCESSIBLE_TREATMENT,           "<rule inaccessible treatment 1>",           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_WARN                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_INACCESSIBLE_TREATMENT,           "<rule inaccessible treatment 2>",           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_OK                               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_INACCESSIBLE_TREATMENT,           "<rule inaccessible treatment 3>",           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_FATAL                            }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_EXCEPTION_STATEMENT,              "<rule exception statement>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_META_LHS,
                                                                                                                                  G1_META_OP_DECLARE,
                                                                                                                                  G1_META_RHS_PRIMARY,
                                                                                                                                  G1_TERMINAL_MINUS,
                                                                                                                                  G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST     }, {1,1,1,1,1}, -1,                   -1,      -1, 0 },
  { G1_META_OP_DECLARE,                       "<rule op declare 1>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_TOP_GRAMMAR               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_OP_DECLARE,                       "<rule op declare 2>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_LEX_GRAMMAR               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_OP_DECLARE,                       "<rule op declare 3>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_ANY_GRAMMAR               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_PRIORITIES,                       "<rule priorities>",                         MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ALTERNATIVES                         }, { 1 },        1,    G1_META_OP_LOOSEN,       1, 0 },
  { G1_META_ALTERNATIVES,                     "<rule alternatives>",                       MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ALTERNATIVE                          }, { 1 },        1,G1_META_OP_EQUAL_PRIORITY,   1, 0 },
  { G1_META_ALTERNATIVE,                      "<rule alternative>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_META_RHS,
                                                                                                                                  G1_META_ADVERB_LIST                          }, { 1, 1},     -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_LIST,                      "<rule adverb list>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ADVERB_LIST_ITEMS                    }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_LIST_ITEMS,                "<rule adverb list items>",                  MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ADVERB_ITEM                          }, { 1 },        0,                   -1,       1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 01>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ACTION                               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 02>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_AUTORANK                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 03>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEFT_ASSOCIATION                     }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 04>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RIGHT_ASSOCIATION                    }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 05>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_GROUP_ASSOCIATION                    }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 06>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SEPARATOR_ASSOCIATION                }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 07>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PROPER_ASSOCIATION                   }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 08>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RANK_SPECIFICATION                   }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 09>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_RANKING_CONSTANT                }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 10>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PRIORITY_SPECIFICATION               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 11>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PAUSE_SPECIFICATION                  }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 12>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LATM_SPECIFICATION                   }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 13>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_BLESSING                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 14>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NAMING                               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ADVERB_ITEM,                      "<rule adverb item 15>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_ADVERB                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ACTION,                           "<rule action>",                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ACTION,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_ACTION_NAME                          }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_AUTORANK,                         "<rule autorank>",                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_AUTORANK,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_BOOLEAN                              }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_LEFT_ASSOCIATION,                 "<rule left association>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_TERMINAL_LEFT                             }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_RIGHT_ASSOCIATION,                "<rule right association>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_TERMINAL_RIGHT                            }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_GROUP_ASSOCIATION,                "<rule group association>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_TERMINAL_GROUP                            }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_SEPARATOR_ASSOCIATION,            "<rule separator association>",              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_SEPARATOR,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_SINGLE_SYMBOL                        }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_PROPER_ASSOCIATION,               "<rule proper association>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PROPER,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_BOOLEAN                              }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_RANK_SPECIFICATION,               "<rule rank specification>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_RANK,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_SIGNED_INTEGER                       }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_NULL_RANKING_SPECIFICATION,       "<rule null ranking specification 1>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_NULL_RANKING,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_NULL_RANKING_CONSTANT                }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_NULL_RANKING_SPECIFICATION,       "<rule null ranking specification 2>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_NULL,
                                                                                                                                  G1_TERMINAL_RANK,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_NULL_RANKING_CONSTANT                }, { 0,0,0,1 }, -1,                   -1,      -1, 0 },
  { G1_META_NULL_RANKING_CONSTANT,            "<rule null ranking constant 1>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_LOW                              }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_NULL_RANKING_CONSTANT,            "<rule null ranking constant 2>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_HIGH                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_PRIORITY_SPECIFICATION,           "<rule priority specification>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PRIORITY,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_SIGNED_INTEGER                       }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_PAUSE_SPECIFICATION,              "<rule pause specification>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PAUSE,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_BEFORE_OR_AFTER                      }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_EVENT_SPECIFICATION,              "<rule event specification>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_EVENT,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_EVENT_INITIALIZATION                 }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_EVENT_INITIALIZATION,             "<rule event initialization>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_META_EVENT_NAME,
                                                                                                                                  G1_META_EVENT_INITIALIZER                    }, { 1, 1 },    -1,                   -1,      -1, 0 },
  { G1_META_EVENT_INITIALIZER,                "<rule event initializer 1>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_TERMINAL_EQUAL,
                                                                                                                                  G1_META_ON_OR_OFF                            }, { 0, 1 },    -1,                   -1,      -1, 0 },
  { G1_META_ON_OR_OFF,                        "<rule on or off 1>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_ON                               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ON_OR_OFF,                        "<rule on or off 2>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_OFF                              }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_EVENT_INITIALIZER,                "<rule event initializer 2>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 0, { -1                                           }, { -1 },      -1,                   -1,      -1, 0 },
  { G1_META_LATM_SPECIFICATION,               "<rule latm specification 1>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_FORGIVING,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_TRUE                                 }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_LATM_SPECIFICATION,               "<rule latm specification 1>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LATM,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_TRUE                                 }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_BLESSING,                         "<rule blessing>",                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_BLESS,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_BLESSING_NAME                        }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_NAMING,                           "<rule naming>",                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_NAME,
                                                                                                                                  G1_TERMINAL_THEN,
                                                                                                                                  G1_META_ALTERNATIVE_NAME                     }, { 0, 0, 1 }, -1,                   -1,      -1, 0 },
  { G1_META_NULL_ADVERB,                      "<rule null adverb>",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_COMMA                            }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ALTERNATIVE_NAME,                 "<rule alternative name 1>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ALTERNATIVE_NAME,                 "<rule alternative name 2>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_LEXER_NAME,                       "<rule lexer name 1>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_LEXER_NAME,                       "<rule lexer name 2>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_EVENT_NAME,                       "<rule event name 1>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_EVENT_NAME,                       "<rule event name 2>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_EVENT_NAME,                       "<rule event name 3>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RESERVED_EVENT_NAME                  }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_BLESSING_NAME,                    "<rule blessing name 1>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_BLESSING_NAME,                    "<rule blessing name 2>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RESERVED_BLESSING_NAME               }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_LHS,                              "<rule lhs>",                                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_RHS,                              "<rule rhs>",                                MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_PRIMARY                          }, { 1 },        1,                   -1,       0, 0 },
  { G1_META_RHS_PRIMARY,                      "<rule rhs primary 1>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SINGLE_SYMBOL                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_RHS_PRIMARY,                      "<rule rhs primary 2>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_STRING                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_RHS_PRIMARY,                      "<rule rhs primary 3>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PARENTHESIZED_RHS_PRIMARY_LIST       }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_PARENTHESIZED_RHS_PRIMARY_LIST,   "<rule parenthesized rhs primary list>",     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LPAREN,
                                                                                                                                  G1_META_RHS_PRIMARY_LIST,
                                                                                                                                  G1_TERMINAL_RPAREN                           }, { 0, 1, 0 }, -1,                   -1,      -1, 0 },
  { G1_META_RHS_PRIMARY_LIST,                 "<rule rhs primary list>",                   MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_PRIMARY                          }, { 1 },        1,                   -1,       0, 0 },
  { G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST, "<rule parenthesized rhs exception list>",   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LPAREN,
                                                                                                                                  G1_META_RHS_EXCEPTION_LIST,
                                                                                                                                  G1_TERMINAL_RPAREN                           }, { 0, 1, 0 }, -1,                   -1,      -1, 0 },
  { G1_META_RHS_EXCEPTION_LIST,               "<rule rhs exception list>",                 MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_EXCEPTION                        }, { 1 },        1,                   -1,       0, 0 },
  { G1_META_RHS_EXCEPTION,                    "<rule rhs exception 1>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_STRING                        }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_RHS_EXCEPTION,                    "<rule rhs exception 2>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CHARACTER_CLASS                      }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_RHS_EXCEPTION,                    "<rule rhs exception 3>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_REGULAR_EXPRESSION                   }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SINGLE_SYMBOL,                    "<rule single symbol 1>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SINGLE_SYMBOL,                    "<rule single symbol 2>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CHARACTER_CLASS                      }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SINGLE_SYMBOL,                    "<rule single symbol 3>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_REGULAR_EXPRESSION                   }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SINGLE_SYMBOL,                    "<rule single symbol 4>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SUBSTITUTION_EXPRESSION              }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SYMBOL,                           "<rule symbol>",                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL_NAME                          }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SYMBOL_NAME,                      "<rule symbol name 1>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_BARE_NAME                            }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_SYMBOL_NAME,                      "<rule symbol name 2>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_BRACKETED_NAME                       }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ACTION_NAME,                      "<rule action name 1>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PERL_NAME                            }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ACTION_NAME,                      "<rule action name 2>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RESERVED_ACTION_NAME                 }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_ACTION_NAME,                      "<rule action name 3>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ARRAY_DESCRIPTOR                     }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_QUANTIFIER,                       "<rule quantifier 1>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_STAR                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_QUANTIFIER,                       "<rule quantifier 2>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_PLUS                             }, { 1 },       -1,                   -1,      -1, 0 },
  { G1_META_DISCARD,                          "<rule discard 1>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_WHITESPACE                           }, { 1 },       -1,                   -1,      -1, 1 },
  { G1_META_DISCARD,                          "<rule discard 2>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PERL_COMMENT                         }, { 1 },       -1,                   -1,      -1, 1 },
  { G1_META_DISCARD,                          "<rule discard 3>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CPLUSPLUS_COMMENT                    }, { 1 },       -1,                   -1,      -1, 1 },
  { G1_META_DISCARD,                          "<rule discard 4>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_C_COMMENT                            }, { 1 },       -1,                   -1,      -1, 1 }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_G1_H */
