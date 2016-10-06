#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

/* This is a bit painful, but C89 does not allow union initializers but      */
/* the first element -; so this will be reworked on-the-fly in the bootstrap */
typedef struct bootstrap_grammar_L0_symbols {
  char                      *descs;               /* Description */
  marpaESLIF_symbol_type_t   symbolType;          /* Symbol type */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  marpaESLIF_uint32_t        optioni;             /* Terminal option */
  PCRE2_SPTR                 originp;             /* UTF-8 encoding */
  PCRE2_SPTR                 substitutionp;       /* UTF-8 encoding */
  marpaESLIF_symbol_t        symbol;              /* Filled on the fly at bootstrap */
  marpaESLIF_terminal_t      terminal;            /* Ditto */
  marpaESLIF_rule_t          rule;                /* Ditto */
} bootstrap_grammar_L0_symbols_t;

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_L0_symbols_t bootstrap_grammar_L0_symbols[] = {
  { "<reserved event name>",          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, ":symbol", NULL },
  { "whitespace",                     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\s]+", NULL },
  { "hash",                           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "#", NULL },
  { "<vertical space char>",          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]", NULL },
  { "<hash comment char>",            MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[^\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]", NULL },
  { "<op declare bnf>",               MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "::=", NULL },
  { "<op declare match>",             MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "~", NULL },
  { "<op loosen>",                    MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "||", NULL },
  { "<op equal priority>",            MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "|", NULL },
  { "before",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "before", NULL },
  { "after",                          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "after", NULL },
  { "<sign>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[+-]", NULL },
  { "<integer>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\d]+", NULL },
  { "<boolean>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[01]", NULL },
  { "<double colon>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "::", NULL },
  { "<one or more word characters>",  MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\w]+", NULL },
  { "<zero or more word characters>", MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\w]*", NULL },
  { "<latin alphabet letter>",        MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[a-zA-Z]", NULL },
  { "<left bracket>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "<", NULL },
  { "<right bracket>",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, ">", NULL },
  { "<bracketed name string>",        MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\s\\w]+", NULL },
  { "<left bracket>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "[", NULL },
  { "<right bracket>",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "]", NULL },
  { "comma",                          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, ",", NULL },
  { "<regular expression>",           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "/([^\\/]*(?:\\.[^\\/]*)*)/", NULL },
  { "<substitution expression>",      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "/([^\\/]*(?:\\.[^\\/]*)*)/([^\\/]*(?:\\.[^\\/]*)*)/", NULL },
  { "<single quoted string>",         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "'([^\\']*(?:\\.[^\\']*)*)'", NULL },
  { "<double quoted string>",         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "\"([^\\\"]*(?:\\.[^\\\"]*)*)\"", NULL }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
