#ifndef MARPAESLIF_INTERNAL_ESLIF_G1_H
#define MARPAESLIF_INTERNAL_ESLIF_G1_H

#include "marpaESLIF/internal/eslif/L0_join_G1.h"

/* Description of internal G1 grammar */

/* It is very important here to list all the terminals first, and in order compatible */
/* with bootstrap_grammar_G1_terminals[] and bootstrap_grammar_G1_rules[] */
typedef enum bootstrap_grammar_G1_enum {
  G1_TERMINAL__START = 0,
  G1_TERMINAL__DESC,
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
  G1_TERMINAL_LOWER,
  G1_TERMINAL_HIGHER,
  /* ----- Non terminals ------ */
  G1_META_STATEMENTS,
  G1_META_STATEMENT,
  G1_META_START_RULE,
  G1_META_DESC_RULE,
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
  G1_META_NAMING,
  G1_META_NULL_ADVERB,
  G1_META_ALTERNATIVE_NAME,
  G1_META_EVENT_NAME,
  G1_META_LHS,
  G1_META_RHS,
  G1_META_RHS_PRIMARY,
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
  G1_META_INTEGER,
  G1_META_BEFORE_OR_AFTER,
  G1_META_TRUE,
  G1_META_STANDARD_NAME,
  G1_META_QUOTED_NAME,
  G1_META_RESERVED_EVENT_NAME,
  G1_META_QUOTED_STRING,
  G1_META_CHARACTER_CLASS,
  G1_META_REGULAR_EXPRESSION,
  G1_META_BARE_NAME,
  G1_META_BRACKETED_NAME,
  G1_META_ASCII_GRAPH_NAME,
  G1_META_WHITESPACE,
  G1_META_PERL_COMMENT,
  G1_META_CPLUSPLUS_COMMENT
} bootstrap_grammar_G1_enum_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_G1_metas[] = {
  { G1_META_STATEMENTS,                       "<G1 meta statements>", 1, 0 },
  { G1_META_STATEMENT,                        "<G1 meta statement>", 0, 0 },
  { G1_META_START_RULE,                       "<G1 meta start rule>", 0, 0 },
  { G1_META_DESC_RULE,                        "<G1 meta desc rule>", 0, 0 },
  { G1_META_EMPTY_RULE,                       "<G1 meta empty rule>", 0, 0 },
  { G1_META_NULL_STATEMENT,                   "<G1 meta null statement>", 0, 0 },
  { G1_META_STATEMENT_GROUP,                  "<G1 meta statement group>", 0, 0 },
  { G1_META_PRIORITY_RULE,                    "<G1 meta priority rule>", 0, 0 },
  { G1_META_QUANTIFIED_RULE,                  "<G1 meta quantified rule>", 0, 0 },
  { G1_META_DISCARD_RULE,                     "<G1 meta discard rule>", 0, 0 },
  { G1_META_DEFAULT_RULE,                     "<G1 meta default rule>", 0, 0 },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         "<G1 meta lexeme default statement>", 0, 0 },
  { G1_META_DISCARD_DEFAULT_STATEMENT,        "<G1 meta discard default statement>", 0, 0 },
  { G1_META_LEXEME_RULE,                      "<G1 meta lexeme rule>", 0, 0 },
  { G1_META_COMPLETION_EVENT_DECLARATION,     "<G1 meta completion event declaration>", 0, 0 },
  { G1_META_NULLED_EVENT_DECLARATION,         "<G1 meta nulled event declaration>", 0, 0 },
  { G1_META_PREDICTION_EVENT_DECLARATION,     "<G1 meta prediction event declaration>", 0, 0 },
  { G1_META_INACCESSIBLE_STATEMENT,           "<G1 meta inaccessible statement>", 0, 0 },
  { G1_META_INACCESSIBLE_TREATMENT,           "<G1 meta inaccessible treatment>", 0, 0 },
  { G1_META_EXCEPTION_STATEMENT,              "<G1 meta exception statement>", 0, 0 },
  { G1_META_OP_DECLARE,                       "<G1 meta op declare>", 0, 0 },
  { G1_META_OP_DECLARE_ANY_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_ANY_GRAMMAR, 0, 0 },
  { G1_META_OP_DECLARE_TOP_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_TOP_GRAMMAR, 0, 0 },
  { G1_META_OP_DECLARE_LEX_GRAMMAR,           L0_JOIN_G1_META_OP_DECLARE_LEX_GRAMMAR, 0, 0 },
  { G1_META_OP_LOOSEN,                        L0_JOIN_G1_META_OP_LOOSEN, 0, 0 },
  { G1_META_OP_EQUAL_PRIORITY,                L0_JOIN_G1_META_OP_EQUAL_PRIORITY, 0, 0 },
  { G1_META_PRIORITIES,                       "<G1 meta priorities>", 0, 0 },
  { G1_META_ALTERNATIVES,                     "<G1 meta alternatives>", 0, 0 },
  { G1_META_ALTERNATIVE,                      "<G1 meta alternative>", 0, 0 },
  { G1_META_ADVERB_LIST,                      "<G1 meta adverb list>", 0, 0 },
  { G1_META_ADVERB_LIST_ITEMS,                "<G1 meta adverb list items>", 0, 0 },
  { G1_META_ADVERB_ITEM,                      "<G1 meta adverb item>", 0, 0 },
  { G1_META_ACTION,                           "<G1 meta action>", 0, 0 },
  { G1_META_AUTORANK,                         "<G1 meta autorank>", 0, 0 },
  { G1_META_LEFT_ASSOCIATION,                 "<G1 meta left association>", 0, 0 },
  { G1_META_RIGHT_ASSOCIATION,                "<G1 meta right association>", 0, 0 },
  { G1_META_GROUP_ASSOCIATION,                "<G1 meta group association>", 0, 0 },
  { G1_META_SEPARATOR_ASSOCIATION,            "<G1 meta separator association>", 0, 0 },
  { G1_META_PROPER_ASSOCIATION,               "<G1 meta proper association>", 0, 0 },
  { G1_META_RANK_SPECIFICATION,               "<G1 meta rank specification>", 0, 0 },
  { G1_META_NULL_RANKING_SPECIFICATION,       "<G1 meta null ranking specification>", 0, 0 },
  { G1_META_NULL_RANKING_CONSTANT,            "<G1 meta null ranking constant>", 0, 0 },
  { G1_META_PRIORITY_SPECIFICATION,           "<G1 meta priority specification>", 0, 0 },
  { G1_META_PAUSE_SPECIFICATION,              "<G1 meta pause specification>", 0, 0 },
  { G1_META_EVENT_SPECIFICATION,              "<G1 meta event specification>", 0, 0 },
  { G1_META_EVENT_INITIALIZATION,             "<G1 meta event initialization>", 0, 0 },
  { G1_META_EVENT_INITIALIZER,                "<G1 meta event initializer>", 0, 0 },
  { G1_META_ON_OR_OFF,                        "<G1 meta on or off>", 0, 0 },
  { G1_META_LATM_SPECIFICATION,               "<G1 meta latm specification>", 0, 0 },
  { G1_META_NAMING,                           "<G1 meta naming>", 0, 0 },
  { G1_META_NULL_ADVERB,                      "<G1 meta null adverb>", 0, 0 },
  { G1_META_ALTERNATIVE_NAME,                 "<G1 meta alternative name>", 0, 0 },
  { G1_META_EVENT_NAME,                       "<G1 meta event name>", 0, 0 },
  { G1_META_LHS,                              "<G1 meta lhs>", 0, 0 },
  { G1_META_RHS,                              "<G1 meta rhs>", 0, 0 },
  { G1_META_RHS_PRIMARY,                      "<G1 meta rhs primary>", 0, 0 },
  { G1_META_RHS_PRIMARY_LIST,                 "<G1 meta rhs primary list>", 0, 0 },
  { G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST, "<G1 meta parenthesized rhs exception list>", 0, 0 },
  { G1_META_RHS_EXCEPTION_LIST,               "<G1 meta rhs exception list>", 0, 0 },
  { G1_META_RHS_EXCEPTION,                    "<G1 meta rhs exception>", 0, 0 },
  { G1_META_SINGLE_SYMBOL,                    "<G1 meta single symbol>", 0, 0 },
  { G1_META_SYMBOL,                           "<G1 meta symbol>", 0, 0 },
  { G1_META_SYMBOL_NAME,                      "<G1 meta symbol name>", 0, 0 },
  { G1_META_ACTION_NAME,                      "<G1 meta action name>", 0, 0 },
  { G1_META_QUANTIFIER,                       "<G1 meta quantifier>", 0, 0 },
  { G1_META_DISCARD,                          "<G1 meta discard>", 0, 1 },
  /* L0 join */
  { G1_META_BOOLEAN,                          L0_JOIN_G1_META_BOOLEAN, 0, 0 },
  { G1_META_SIGNED_INTEGER,                   L0_JOIN_G1_META_SIGNED_INTEGER, 0, 0 },
  { G1_META_INTEGER,                          L0_JOIN_G1_META_INTEGER, 0, 0 },
  { G1_META_BEFORE_OR_AFTER,                  L0_JOIN_G1_META_BEFORE_OR_AFTER, 0, 0 },
  { G1_META_TRUE,                             L0_JOIN_G1_META_TRUE, 0, 0 },
  { G1_META_STANDARD_NAME,                    L0_JOIN_G1_META_STANDARD_NAME, 0, 0 },
  { G1_META_QUOTED_NAME,                      L0_JOIN_G1_META_QUOTED_NAME, 0, 0 },
  { G1_META_RESERVED_EVENT_NAME,              L0_JOIN_G1_META_RESERVED_EVENT_NAME, 0, 0 },
  { G1_META_QUOTED_STRING,                    L0_JOIN_G1_META_QUOTED_STRING, 0, 0 },
  { G1_META_CHARACTER_CLASS,                  L0_JOIN_G1_META_CHARACTER_CLASS, 0, 0 },
  { G1_META_REGULAR_EXPRESSION,               L0_JOIN_G1_META_REGULAR_EXPRESSION, 0, 0 },
  { G1_META_BARE_NAME,                        L0_JOIN_G1_META_BARE_NAME, 0, 0 },
  { G1_META_BRACKETED_NAME,                   L0_JOIN_G1_META_BRACKETED_NAME, 0, 0 },
  { G1_META_ASCII_GRAPH_NAME,                 L0_JOIN_G1_META_ASCII_GRAPH_NAME, 0, 0 },
  { G1_META_WHITESPACE,                       L0_JOIN_G1_META_WHITESPACE, 0, 0 },
  { G1_META_PERL_COMMENT,                     L0_JOIN_G1_META_PERL_COMMENT, 0, 0 },
  { G1_META_CPLUSPLUS_COMMENT,                L0_JOIN_G1_META_CPLUSPLUS_COMMENT, 0, 0 }
};

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_terminal_t bootstrap_grammar_G1_terminals[] = {
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /*                                                             TERMINALS                                                             */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { G1_TERMINAL__START, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":start",
#ifndef MARPAESLIF_NTRACE
    ":start", ":sta",
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__DESC, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":desc",
#ifndef MARPAESLIF_NTRACE
    ":desc", ":de"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_SEMICOLON, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ";",
#ifndef MARPAESLIF_NTRACE
    ";", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEFT_BRACKET, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "{",
#ifndef MARPAESLIF_NTRACE
    "{", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RIGHT_BRACKET, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "}",
#ifndef MARPAESLIF_NTRACE
    "}", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__DISCARD, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":discard",
#ifndef MARPAESLIF_NTRACE
    ":discard", ":dis"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__DEFAULT, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":default",
#ifndef MARPAESLIF_NTRACE
    ":default", ":def"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEXEME, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lexeme",
#ifndef MARPAESLIF_NTRACE
    "lexeme", "lexe"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_DEFAULT, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "default",
#ifndef MARPAESLIF_NTRACE
    "default", "def"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_EQUAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "=",
#ifndef MARPAESLIF_NTRACE
    "=", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_DISCARD, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "discard",
#ifndef MARPAESLIF_NTRACE
    "discard", "dis"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL__LEXEME, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":lexeme",
#ifndef MARPAESLIF_NTRACE
    ":lexeme", ":lexe"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_EVENT, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "event",
#ifndef MARPAESLIF_NTRACE
    "event", "eve"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_COMPLETED, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "completed",
#ifndef MARPAESLIF_NTRACE
    "completed", "comp"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULLED, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "nulled",
#ifndef MARPAESLIF_NTRACE
    "nulled", "nul"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PREDICTED, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "predicted",
#ifndef MARPAESLIF_NTRACE
    "predicted", "pre"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_IS, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "is",
#ifndef MARPAESLIF_NTRACE
    "is", "i"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_INACCESSIBLE, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "inaccessible",
#ifndef MARPAESLIF_NTRACE
    "inaccessible", "inac"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_BY, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "by",
#ifndef MARPAESLIF_NTRACE
    "by", "b"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_WARN, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "warn",
#ifndef MARPAESLIF_NTRACE
    "warn", "wa"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_OK,  MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "ok",
#ifndef MARPAESLIF_NTRACE
    "ok", "o"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_FATAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "fatal",
#ifndef MARPAESLIF_NTRACE
    "fatal", "fata"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_MINUS, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "-",
#ifndef MARPAESLIF_NTRACE
    "-", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ACTION, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "action",
#ifndef MARPAESLIF_NTRACE
    "action", "act"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_THEN, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "=>",
#ifndef MARPAESLIF_NTRACE
    "=>", "="
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_AUTORANK, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "autorank",
#ifndef MARPAESLIF_NTRACE
    "autorank", "auto"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ASSOC, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "assoc",
#ifndef MARPAESLIF_NTRACE
    "assoc", "asso"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LEFT, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "left",
#ifndef MARPAESLIF_NTRACE
    "left", "lef"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RIGHT, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "right",
#ifndef MARPAESLIF_NTRACE
    "right", "r"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_GROUP, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "group",
#ifndef MARPAESLIF_NTRACE
    "group", "gr"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_SEPARATOR, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "separator",
#ifndef MARPAESLIF_NTRACE
    "separator", "sep"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PROPER, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "proper",
#ifndef MARPAESLIF_NTRACE
    "proper", "pro"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RANK, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rank",
#ifndef MARPAESLIF_NTRACE
    "rank", "ra"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULL_RANKING, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "null-ranking",
#ifndef MARPAESLIF_NTRACE
    "null-ranking", "null-"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NULL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "null",
#ifndef MARPAESLIF_NTRACE
    "null", "nul"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LOW, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "low",
#ifndef MARPAESLIF_NTRACE
    "low", "lo"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_HIGH, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "high",
#ifndef MARPAESLIF_NTRACE
    "high", "hi"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PRIORITY, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "priority",
#ifndef MARPAESLIF_NTRACE
    "priority", "prio"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PAUSE, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "pause",
#ifndef MARPAESLIF_NTRACE
    "pause", "pa"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_ON, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "on",
#ifndef MARPAESLIF_NTRACE
    "on", "o"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_OFF, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "off",
#ifndef MARPAESLIF_NTRACE
    "off", "of"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_FORGIVING, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "forgiving",
#ifndef MARPAESLIF_NTRACE
    "forgiving", "for"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LATM, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "latm",
#ifndef MARPAESLIF_NTRACE
    "latm", "lat"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_BLESS, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "bless",
#ifndef MARPAESLIF_NTRACE
    "bless", "bl"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_NAME, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name",
#ifndef MARPAESLIF_NTRACE
    "name", "nam"
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_COMMA, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",",
#ifndef MARPAESLIF_NTRACE
    ",", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LPAREN, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "(",
#ifndef MARPAESLIF_NTRACE
    "(", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RPAREN, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ")",
#ifndef MARPAESLIF_NTRACE
    ")", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_STAR, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "*",
#ifndef MARPAESLIF_NTRACE
    "*", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_PLUS, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "+",
#ifndef MARPAESLIF_NTRACE
    "+", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LOWER, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "<",
#ifndef MARPAESLIF_NTRACE
    "<", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_HIGHER, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ">",
#ifndef MARPAESLIF_NTRACE
    ">", ""
#else
    NULL, NULL
#endif
  }
};

/* When bootstrapping we decide for convenience that the description is also the action name */
bootstrap_grammar_rule_t bootstrap_grammar_G1_rules[] = {
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_STATEMENTS,                       G1_RULE_STATEMENTS,                             MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_STATEMENT                            },  0,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_01,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_START_RULE                           }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_02,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DESC_RULE                            }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_03,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_EMPTY_RULE                           }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_04,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_STATEMENT                       }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_05,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STATEMENT_GROUP                      }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_06,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PRIORITY_RULE                        }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_07,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUANTIFIED_RULE                      }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_08,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DISCARD_RULE                         }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_09,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DEFAULT_RULE                         }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_10,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEXEME_DEFAULT_STATEMENT             }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_11,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_DISCARD_DEFAULT_STATEMENT            }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_12,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEXEME_RULE                          }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_13,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_COMPLETION_EVENT_DECLARATION         }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_14,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULLED_EVENT_DECLARATION             }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_15,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PREDICTION_EVENT_DECLARATION         }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_16,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_INACCESSIBLE_STATEMENT               }, -1,                        -1, -1 },
  { G1_META_STATEMENT,                        G1_RULE_STATEMENT_17,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_EXCEPTION_STATEMENT                  }, -1,                        -1, -1 },
  { G1_META_START_RULE,                       G1_RULE_START_RULE,                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL__START,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_SYMBOL                               }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_DESC_RULE,                        G1_RULE_DESC_RULE,                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL__DESC,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_QUOTED_NAME                          }, -1,                        -1, -1 },
  { G1_META_EMPTY_RULE,                       G1_RULE_EMPTY_RULE,                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_META_LHS,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_NULL_STATEMENT,                   G1_RULE_NULL_STATEMENT,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_SEMICOLON                        }, -1,                        -1, -1 },
  { G1_META_STATEMENT_GROUP,                  G1_RULE_STATEMENT_GROUP,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LEFT_BRACKET,
                                                                                                                                     G1_META_STATEMENTS,
                                                                                                                                     G1_TERMINAL_RIGHT_BRACKET                    }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_PRIORITY_RULE,                    G1_RULE_PRIORITY_RULE,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_META_LHS,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_PRIORITIES                           }, -1,                        -1, -1 },
  { G1_META_QUANTIFIED_RULE,                  G1_RULE_QUANTIFIED_RULE,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_META_LHS,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_SINGLE_SYMBOL,
                                                                                                                                     G1_META_QUANTIFIER,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_DISCARD_RULE,                     G1_RULE_DISCARD_RULE,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL__DISCARD,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_SINGLE_SYMBOL,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_DEFAULT_RULE,                     G1_RULE_DEFAULT_RULE,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL__DEFAULT,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         G1_RULE_LEXEME_DEFAULT_STATEMENT_1,            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_LEXEME,
                                                                                                                                     G1_TERMINAL_DEFAULT,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         G1_RULE_LEXEME_DEFAULT_STATEMENT_2,            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_LEXEME,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_DISCARD_DEFAULT_STATEMENT,        G1_RULE_DISCARD_DEFAULT_STATEMENT,              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_DISCARD,
                                                                                                                                     G1_TERMINAL_DEFAULT,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_LEXEME_RULE,                      G1_RULE_LEXEME_RULE,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL__LEXEME,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_SYMBOL,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_COMPLETION_EVENT_DECLARATION,     G1_RULE_COMPLETION_EVENT_DECLARATION,           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                     G1_META_EVENT_INITIALIZATION,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_TERMINAL_COMPLETED,
                                                                                                                                     G1_META_SYMBOL_NAME                          }, -1,                        -1, -1 },
  { G1_META_NULLED_EVENT_DECLARATION,         G1_RULE_NULLED_EVENT_DECLARATION,               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                     G1_META_EVENT_INITIALIZATION,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_TERMINAL_NULLED,
                                                                                                                                     G1_META_SYMBOL_NAME                          }, -1,                        -1, -1 },
  { G1_META_PREDICTION_EVENT_DECLARATION,     G1_RULE_NULLED_PREDICTED_DECLARATION,           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_EVENT,
                                                                                                                                     G1_META_EVENT_INITIALIZATION,
                                                                                                                                     G1_TERMINAL_EQUAL,
                                                                                                                                     G1_TERMINAL_PREDICTED,
                                                                                                                                     G1_META_SYMBOL_NAME                          }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_INACCESSIBLE_STATEMENT,           G1_RULE_INACCESSIBLE_STATEMENT,                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_TERMINAL_INACCESSIBLE,
                                                                                                                                     G1_TERMINAL_IS,
                                                                                                                                     G1_META_INACCESSIBLE_TREATMENT,
                                                                                                                                     G1_TERMINAL_BY,
                                                                                                                                     G1_TERMINAL_DEFAULT                          }, -1,                        -1, -1 },
  { G1_META_INACCESSIBLE_TREATMENT,           G1_RULE_INACCESSIBLE_TREATMENT_1,               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_WARN                             }, -1,                        -1, -1 },
  { G1_META_INACCESSIBLE_TREATMENT,           G1_RULE_INACCESSIBLE_TREATMENT_2,               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_OK                               }, -1,                        -1, -1 },
  { G1_META_INACCESSIBLE_TREATMENT,           G1_RULE_INACCESSIBLE_TREATMENT_3,               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_FATAL                            }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_EXCEPTION_STATEMENT,              G1_RULE_EXCEPTION_STATEMENT,                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 5, { G1_META_LHS,
                                                                                                                                     G1_META_OP_DECLARE,
                                                                                                                                     G1_META_RHS_PRIMARY,
                                                                                                                                     G1_TERMINAL_MINUS,
                                                                                                                                     G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST     }, -1,                        -1, -1 },
  { G1_META_OP_DECLARE,                       G1_RULE_OP_DECLARE_1,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_TOP_GRAMMAR               }, -1,                        -1, -1 },
  { G1_META_OP_DECLARE,                       G1_RULE_OP_DECLARE_2,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_LEX_GRAMMAR               }, -1,                        -1, -1 },
  { G1_META_OP_DECLARE,                       G1_RULE_OP_DECLARE_3,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_OP_DECLARE_ANY_GRAMMAR               }, -1,                        -1, -1 },
  { G1_META_PRIORITIES,                       G1_RULE_PRIORITIES,                             MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ALTERNATIVES                         }, 1,          G1_META_OP_LOOSEN,  1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_ALTERNATIVES,                     G1_RULE_ALTERNATIVES,                           MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ALTERNATIVE                          },  1, G1_META_OP_EQUAL_PRIORITY,  1 },
  { G1_META_ALTERNATIVE,                      G1_RULE_ALTERNATIVE,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_META_RHS,
                                                                                                                                     G1_META_ADVERB_LIST                          }, -1,                        -1, -1 },
  { G1_META_ADVERB_LIST,                      G1_RULE_ADVERB_LIST,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ADVERB_LIST_ITEMS                    }, -1,                        -1, -1 },
  { G1_META_ADVERB_LIST_ITEMS,                G1_RULE_ADVERB_LIST_ITEMS,                      MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_ADVERB_ITEM                          },  0,                        -1,  1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_01,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ACTION                               }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_02,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_AUTORANK                             }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_03,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LEFT_ASSOCIATION                     }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_04,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RIGHT_ASSOCIATION                    }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_05,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_GROUP_ASSOCIATION                    }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_06,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SEPARATOR_ASSOCIATION                }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_07,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PROPER_ASSOCIATION                   }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_08,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RANK_SPECIFICATION                   }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_09,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_RANKING_CONSTANT                }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_10,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PRIORITY_SPECIFICATION               }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_11,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PAUSE_SPECIFICATION                  }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_12,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_LATM_SPECIFICATION                   }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_13,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NAMING                               }, -1,                        -1, -1 },
  { G1_META_ADVERB_ITEM,                      G1_RULE_ADVERB_ITEM_14,                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_NULL_ADVERB                          }, -1,                        -1, -1 },
  { G1_META_ACTION,                           G1_RULE_ACTION,                                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ACTION,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_ACTION_NAME                          }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_AUTORANK,                         G1_RULE_AUTORANK,                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_AUTORANK,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_BOOLEAN                              }, -1,                        -1, -1 },
  { G1_META_LEFT_ASSOCIATION,                 G1_RULE_LEFT_ASSOCIATION,                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_TERMINAL_LEFT                             }, -1,                        -1, -1 },
  { G1_META_RIGHT_ASSOCIATION,                G1_RULE_RIGHT_ASSOCIATION,                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_TERMINAL_RIGHT                            }, -1,                        -1, -1 },
  { G1_META_GROUP_ASSOCIATION,                G1_RULE_GROUP_ASSOCIATION,                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_ASSOC,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_TERMINAL_GROUP                            }, -1,                        -1, -1 },
  { G1_META_SEPARATOR_ASSOCIATION,            G1_RULE_SEPARATOR_ASSOCIATION,                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_SEPARATOR,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_SINGLE_SYMBOL                        }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_PROPER_ASSOCIATION,               G1_RULE_PROPER_ASSOCIATION,                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PROPER,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_BOOLEAN                              }, -1,                        -1, -1 },
  { G1_META_RANK_SPECIFICATION,               G1_RULE_RANK_SPECIFICATION,                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_RANK,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_SIGNED_INTEGER                       }, -1,                        -1, -1 },
  { G1_META_NULL_RANKING_SPECIFICATION,       G1_RULE_NULL_RANKING_SPECIFICATION_1,           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_NULL_RANKING,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_NULL_RANKING_CONSTANT                }, -1,                        -1, -1 },
  { G1_META_NULL_RANKING_SPECIFICATION,       G1_RULE_NULL_RANKING_SPECIFICATION_2,           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 4, { G1_TERMINAL_NULL,
                                                                                                                                     G1_TERMINAL_RANK,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_NULL_RANKING_CONSTANT                }, -1,                        -1, -1 },
  { G1_META_NULL_RANKING_CONSTANT,            G1_RULE_NULL_RANKING_CONSTANT_1,                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_LOW                              }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_NULL_RANKING_CONSTANT,            G1_RULE_NULL_RANKING_CONSTANT_2,                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_HIGH                             }, -1,                        -1, -1 },
  { G1_META_PRIORITY_SPECIFICATION,           G1_RULE_PRIORITY_SPECIFICATION,                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PRIORITY,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_SIGNED_INTEGER                       }, -1,                        -1, -1 },
  { G1_META_PAUSE_SPECIFICATION,              G1_RULE_PAUSE_SPECIFICATION,                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_PAUSE,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_BEFORE_OR_AFTER                      }, -1,                        -1, -1 },
  { G1_META_EVENT_SPECIFICATION,              G1_RULE_EVENT_SPECIFICATION,                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_EVENT,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_EVENT_INITIALIZATION                 }, -1,                        -1, -1 },
  { G1_META_EVENT_INITIALIZATION,             G1_RULE_EVENT_INITIALIZATION,                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_META_EVENT_NAME,
                                                                                                                                     G1_META_EVENT_INITIALIZER                    }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_EVENT_INITIALIZER,                G1_RULE_EVENT_INITIALIZER_1,                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { G1_TERMINAL_EQUAL,
                                                                                                                                     G1_META_ON_OR_OFF                            }, -1,                        -1, -1 },
  { G1_META_ON_OR_OFF,                        G1_RULE_ON_OR_OFF_1,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_ON                               }, -1,                        -1, -1 },
  { G1_META_ON_OR_OFF,                        G1_RULE_ON_OR_OFF_2,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_OFF                              }, -1,                        -1, -1 },
  { G1_META_EVENT_INITIALIZER,                G1_RULE_EVENT_INITIALIZER_2,                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 0, { -1                                           }, -1,                        -1, -1 },
  { G1_META_LATM_SPECIFICATION,               G1_RULE_LATM_SPECIFICATION_1,                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_FORGIVING,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_BOOLEAN                              }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_LATM_SPECIFICATION,               G1_RULE_LATM_SPECIFICATION_2,                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LATM,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_BOOLEAN                              }, -1,                        -1, -1 },
  { G1_META_NAMING,                           G1_RULE_NAMING,                                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_NAME,
                                                                                                                                     G1_TERMINAL_THEN,
                                                                                                                                     G1_META_ALTERNATIVE_NAME                     }, -1,                        -1, -1 },
  { G1_META_NULL_ADVERB,                      G1_RULE_NULL_ADVERB,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_COMMA                            }, -1,                        -1, -1 },
  { G1_META_ALTERNATIVE_NAME,                 G1_RULE_ALTERNATIVE_NAME_1,                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_ALTERNATIVE_NAME,                 G1_RULE_ALTERNATIVE_NAME_2,                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_NAME                          }, -1,                        -1, -1 },
  { G1_META_EVENT_NAME,                       G1_RULE_EVENT_NAME_1,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_STANDARD_NAME                        }, -1,                        -1, -1 },
  { G1_META_EVENT_NAME,                       G1_RULE_EVENT_NAME_2,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_NAME                          }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_EVENT_NAME,                       G1_RULE_EVENT_NAME_3,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_RESERVED_EVENT_NAME                  }, -1,                        -1, -1 },
  { G1_META_LHS,                              G1_RULE_LHS,                                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL_NAME                          }, -1,                        -1, -1 },
  { G1_META_RHS,                              G1_RULE_RHS,                                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_PRIMARY                          },  1,                        -1,  0 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_RHS_PRIMARY,                      G1_RULE_RHS_PRIMARY_1,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SINGLE_SYMBOL                        }, -1,                        -1, -1 },
  { G1_META_RHS_PRIMARY,                      G1_RULE_RHS_PRIMARY_2,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_STRING                        }, -1,                        -1, -1 },
  { G1_META_RHS_PRIMARY_LIST,                 G1_RULE_RHS_PRIMARY_LIST,                       MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_PRIMARY                          },  1,                        -1,  0 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST, G1_RULE_PARENTHESIZED_RHS_EXCEPTION_LIST,       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { G1_TERMINAL_LPAREN,
                                                                                                                                     G1_META_RHS_EXCEPTION_LIST,
                                                                                                                                     G1_TERMINAL_RPAREN                           }, -1,                        -1, -1 },
  { G1_META_RHS_EXCEPTION_LIST,               G1_RULE_RHS_EXCEPTION_LIST,                     MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_RHS_EXCEPTION                        },  1,                        -1,  0 },
  { G1_META_RHS_EXCEPTION,                    G1_RULE_RHS_EXCEPTION_1,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_QUOTED_STRING                        }, -1,                        -1, -1 },
  { G1_META_RHS_EXCEPTION,                    G1_RULE_RHS_EXCEPTION_2,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CHARACTER_CLASS                      }, -1,                        -1, -1 },
  { G1_META_RHS_EXCEPTION,                    G1_RULE_RHS_EXCEPTION_3,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_REGULAR_EXPRESSION                   }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_SINGLE_SYMBOL,                    G1_RULE_SINGLE_SYMBOL_1,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL                               }, -1,                        -1, -1 },
  { G1_META_SINGLE_SYMBOL,                    G1_RULE_SINGLE_SYMBOL_2,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CHARACTER_CLASS                      }, -1,                        -1, -1 },
  { G1_META_SINGLE_SYMBOL,                    G1_RULE_SINGLE_SYMBOL_3,                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_REGULAR_EXPRESSION                   }, -1,                        -1, -1 },
  { G1_META_SYMBOL,                           G1_RULE_SYMBOL,                                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_SYMBOL_NAME                          }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_SYMBOL_NAME,                      G1_RULE_SYMBOL_NAME_1,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_BARE_NAME                            }, -1,                        -1, -1 },
  { G1_META_SYMBOL_NAME,                      G1_RULE_SYMBOL_NAME_2,                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_BRACKETED_NAME                       }, -1,                        -1, -1 },
  { G1_META_ACTION_NAME,                      G1_RULE_ACTION_NAME,                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_ASCII_GRAPH_NAME                     }, -1,                        -1, -1 },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb
  */
  { G1_META_QUANTIFIER,                       G1_RULE_QUANTIFIER_1,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_STAR                             }, -1,                        -1, -1 },
  { G1_META_QUANTIFIER,                       G1_RULE_QUANTIFIER_2,                           MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_TERMINAL_PLUS                             }, -1,                        -1, -1 },
  { G1_META_DISCARD,                          G1_RULE_DISCARD_1,                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_WHITESPACE                           }, -1,                        -1, -1 },
  { G1_META_DISCARD,                          G1_RULE_DISCARD_2,                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_PERL_COMMENT                         }, -1,                        -1, -1 },
  { G1_META_DISCARD,                          G1_RULE_DISCARD_3,                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { G1_META_CPLUSPLUS_COMMENT                    }, -1,                        -1, -1 }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_G1_H */
