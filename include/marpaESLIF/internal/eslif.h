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
  marpaESLIF_symbol_t        symbol;
  marpaESLIF_terminal_t      terminal;
  marpaESLIF_rule_t          rule;
} bootstrap_grammar_L0_symbols_t;

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_L0_symbols_t bootstrap_grammar_L0_symbols[] = {
  { "<reserved event name>",          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, ":symbol" },
  { "whitespace",                     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\s]+" },
  { "hash",                           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "#" },
  { "<vertical space char>",          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]" },
  { "<hash comment char>",            MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[^\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]" },
  { "<op declare bnf>",               MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "::=" },
  { "<op declare match>",             MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "~" },
  { "<op loosen>",                    MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "||" },
  { "<op equal priority>",            MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "|" },
  { "before",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "before" },
  { "after",                          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "after" },
  { "<sign>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[+-]" },
  { "<integer>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\d]+" },
  { "<boolean>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[01]" },
  { "<double colon>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "::" },
  { "<one or more word characters>",  MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\w]+" },
  { "<zero or more word characters>", MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\w]*" },
  { "<latin alphabet letter>",        MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[a-zA-Z]" },
  { "<left bracket>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "<" },
  { "<right bracket>",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, ">" },
  { "<bracketed name string>",        MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "[\\s\\w]+" },
  { "<left bracket>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "[" },
  { "<right bracket>",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "]" },
  { "comma",                          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA, "," },
  { "<regular expression>",           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "/[^\\/]*(?:\\.[^\\/]*)*/" },
  { "<single quoted string>",         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "'[^\\']*(?:\\.[^\\']*)*'" },
  { "<double quoted string>",         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA, "\"[^\\\"]*(?:\\.[^\\\"]*)*\"" }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
