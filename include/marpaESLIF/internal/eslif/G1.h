#ifndef MARPAESLIF_INTERNAL_ESLIF_G1_H
#define MARPAESLIF_INTERNAL_ESLIF_G1_H

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
  G1_META_PRIORITIES,
  G1_META_ALTERNATIVES,
  G1_META_ALTERNATIVE_,
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
  G1_META_NAMING,
  G1_META_NULL_ADVERB,
  G1_META_ALTERNATIVE_NAME,
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
  G1_META_ACTION_NAME
} bootstrap_grammar_G1_enum_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_G1_metas[] = {
  { G1_META_STATEMENTS,                       "<meta statements>" },
  { G1_META_STATEMENT,                        "<meta statement>" },
  { G1_META_START_RULE,                       "<meta start rule>" },
  { G1_META_EMPTY_RULE,                       "<meta empty rule>" },
  { G1_META_NULL_STATEMENT,                   "<meta null statement>" },
  { G1_META_STATEMENT_GROUP,                  "<meta statement group>" },
  { G1_META_PRIORITY_RULE,                    "<meta priority rule>" },
  { G1_META_QUANTIFIED_RULE,                  "<meta quantified rule>" },
  { G1_META_DISCARD_RULE,                     "<meta discard rule>" },
  { G1_META_DEFAULT_RULE,                     "<meta default rule>" },
  { G1_META_LEXEME_DEFAULT_STATEMENT,         "<meta lexeme default statement>" },
  { G1_META_DISCARD_DEFAULT_STATEMENT,        "<meta discard default statement>" },
  { G1_META_LEXEME_RULE,                      "<meta lexeme rule>" },
  { G1_META_COMPLETION_EVENT_DECLARATION,     "<meta completion event declaration>" },
  { G1_META_NULLED_EVENT_DECLARATION,         "<meta nulled event declaration>" },
  { G1_META_PREDICTION_EVENT_DECLARATION,     "<meta prediction event declaration>" },
  { G1_META_CURRENT_LEXER_STATEMENT,          "<meta current lexer statement>" },
  { G1_META_INACCESSIBLE_STATEMENT,           "<meta inaccessible statement>" },
  { G1_META_INACCESSIBLE_TREATMENT,           "<meta inaccessible treatment>" },
  { G1_META_EXCEPTION_STATEMENT,              "<meta exception statement>" },
  { G1_META_OP_DECLARE,                       "<meta op declare>" },
  { G1_META_PRIORITIES,                       "<meta priorities>" },
  { G1_META_ALTERNATIVES,                     "<meta alternatives>" },
  { G1_META_ALTERNATIVE_,                     "<meta alternative >" },
  { G1_META_ADVERB_LIST,                      "<meta adverb list>" },
  { G1_META_ADVERB_LIST_ITEMS,                "<meta adverb list items>" },
  { G1_META_ADVERB_ITEM,                      "<meta adverb item>" },
  { G1_META_ACTION,                           "<meta action>" },
  { G1_META_AUTORANK,                         "<meta autorank>" },
  { G1_META_LEFT_ASSOCIATION,                 "<meta left association>" },
  { G1_META_RIGHT_ASSOCIATION,                "<meta right association>" },
  { G1_META_GROUP_ASSOCIATION,                "<meta group association>" },
  { G1_META_SEPARATOR_ASSOCIATION,            "<meta separator association>" },
  { G1_META_PROPER_ASSOCIATION,               "<meta proper association>" },
  { G1_META_RANK_SPECIFICATION,               "<meta rank specification>" },
  { G1_META_NULL_RANKING_SPECIFICATION,       "<meta null ranking specification>" },
  { G1_META_NULL_RANKING_CONSTANT,            "<meta null ranking constant>" },
  { G1_META_PRIORITY_SPECIFICATION,           "<meta priority specification>" },
  { G1_META_PAUSE_SPECIFICATION,              "<meta pause specification>" },
  { G1_META_EVENT_SPECIFICATION,              "<meta event specification>" },
  { G1_META_EVENT_INITIALIZATION,             "<meta event initialization>" },
  { G1_META_EVENT_INITIALIZER,                "<meta event initializer>" },
  { G1_META_ON_OR_OFF,                        "<meta on or off>" },
  { G1_META_LATM_SPECIFICATION,               "<meta latm specification>" },
  { G1_META_BLESSING,                         "<meta blessing>" },
  { G1_META_NAMING,                           "<meta naming>" },
  { G1_META_NULL_ADVERB,                      "<meta null adverb>" },
  { G1_META_ALTERNATIVE_NAME,                 "<meta alternative name>" },
  { G1_META_EVENT_NAME,                       "<meta event name>" },
  { G1_META_LHS,                              "<meta lhs>" },
  { G1_META_RHS,                              "<meta rhs>" },
  { G1_META_RHS_PRIMARY,                      "<meta rhs primary>" },
  { G1_META_PARENTHESIZED_RHS_PRIMARY_LIST,   "<meta parenthesized rhs primary list>" },
  { G1_META_RHS_PRIMARY_LIST,                 "<meta rhs primary list>" },
  { G1_META_PARENTHESIZED_RHS_EXCEPTION_LIST, "<meta parenthesized rhs exception list>" },
  { G1_META_RHS_EXCEPTION_LIST,               "<meta rhs exception list>" },
  { G1_META_RHS_EXCEPTION,                    "<meta rhs exception>" },
  { G1_META_SINGLE_SYMBOL,                    "<meta single symbol>" },
  { G1_META_SYMBOL,                           "<meta symbol>" },
  { G1_META_SYMBOL_NAME,                      "<meta symbol name>" },
  { G1_META_ACTION_NAME,                      "<meta action name>" },
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
    "<terminal ,",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",", NULL,
#ifndef MARPAESLIF_NTRACE
    ",", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_LPAREN,
    "<terminal (",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "(", NULL,
#ifndef MARPAESLIF_NTRACE
    "(", ""
#else
    NULL, NULL
#endif
  },
  { G1_TERMINAL_RPAREN,
    "<terminal )",           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ")", NULL,
#ifndef MARPAESLIF_NTRACE
    ")", ""
#else
    NULL, NULL
#endif
  }
};

bootstrap_grammar_rule_t bootstrap_grammar_G1_rules[] = {
  { G1_META_STATEMENTS,                       "<rule statements>",                         MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { G1_META_STATEMENT                            },        0,                   -1,      -1 },
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_G1_H */
