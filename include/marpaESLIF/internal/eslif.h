#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

/* This is a bit painful, but C89 does not allow union initializers but      */
/* the first element -; so this will be reworked on-the-fly in the bootstrap */
typedef struct bootstrap_grammar_L0_symbols {
  char                      *descs;               /* Description */
  marpaESLIF_symbol_type_t   symbolType;          /* Symbol type */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  PCRE2_SPTR                 originp;             /* UTF-8 encoding */
  marpaESLIF_symbol_t        symbol;
  marpaESLIF_terminal_t      terminal;
  marpaESLIF_rule_t          rule;
} bootstrap_grammar_L0_symbols_t;
bootstrap_grammar_L0_symbols_t bootstrap_grammar_L0_symbols[] = {
  { "<reserved event name>",     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  ":symbol" },
  { "whitespace",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEXP, "[\\s]+" },
  { "hash",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  "#" },
  { "<vertical space char>",     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEXP, "[\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]" },
  { "<hash comment char>",       MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEXP, "[^\\x{A}\\x{B}\\x{C}\\x{D}\\x{2028}\\x{2029}]" }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
