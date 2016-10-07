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
} bootstrap_grammar_L0_symbols_t;

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_L0_symbols_t bootstrap_grammar_L0_symbols[] = {
  /* From perl stringified version to C and // versions: */
  /*
   my $this = q{STRINGIFIED_VERSION_WITHOUT_THE_SLASH_AND_QUOTE_ESCAPES};
   my $copy = $this;

   $this =~ s/\\/\\\\/g;
   $this =~ s/"/\\"/g;
   print "FOR C: $this\n";

   $copy =~ s/\//\\\//g;
   print "FOR /: $copy\n";
  */
  { "<reserved event name>",               MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     ":symbol", NULL },
  { "whitespace",                          MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,    "[\\s]+", NULL },    /* Unicode notion of whitespace */
  /* Taken from Regexp::Common::comment, $RE{comment}{Perl} */
  /* Perl stringified version is: (?:(?:#)(?:[^\n]*)(?:\n)) */
  /* Simplified to (without the [/'"] escapes): #[^\n]*\n */
  /* I add \\z to match the end of buffer */
  { "<perl comment>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "#[^\\n]*(?:\\n|\\z)", NULL },
  /* Adapted for C++ (//) only */
  { "<cplusplus comment>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "//[^\\n]*(?:\\n|\\z)", NULL },
  /* Taken from Regexp::Common::comment, $RE{comment}{C} */
  /* Perl stringified version is: (?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)) */
  /* Simplified to (without the [/"'] escapes) /\*(?:[^\*]+|\*(?!/))*\!*!/ */   /* REMOVE THE ! character - this is C compilers -; */
  { "<c comment>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "/\\*(?:[^\\*]+|\\*(?!/))*\\*/", NULL },
  { "<op declare bnf>",                    MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "::=", NULL },
  { "<op declare match>",                  MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "~", NULL },
  { "<op loosen>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "||", NULL },
  { "<op equal priority>",                 MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "|", NULL },
  { "<star char>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "*", NULL },
  { "<plus char>",                         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "+", NULL },
  { "before",                              MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "before", NULL },
  { "after",                               MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "after", NULL },
  { "<sign>",                              MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[+-]", NULL },
  { "<integer>",                           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[\\d]+", NULL },
  { "<boolean>",                           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[01]", NULL },
  { "<double colon>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "::", NULL },
  { "<one or more word characters>",       MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[\\w]+", NULL },
  { "<zero or more word characters>",      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[\\w]*", NULL },
  { "<latin alphabet letter>",             MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[a-zA-Z]", NULL },
  { "<left bracket>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "<", NULL },
  { "<right bracket>",                     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     ">", NULL },
  { "<bracketed name string>",             MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     "[\\s\\w]+", NULL },
  { "<left bracket>",                      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "[", NULL },
  { "<right bracket>",                     MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     "]", NULL },
  { "comma",                               MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,     ",", NULL },
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"/"}{-cdelim=>"/"} */
  /* Perl stringified version is: (?:(?|(?:\/)(?:[^\\\/]*(?:\\.[^\\\/]*)*)(?:\/))) */
  /* Simplified to (without the [/"'] escapes) /[^\\/]*(?:\\.[^\\/]*)!*!/ */   /* REMOVE THE ! character - this is C compilers -; */
  { "<regular expression>",                MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "/[^\\/]*(?:\\.[^\\/]*)*/", NULL },
  { "<regular expression modifiers>",      MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "[eijmnsxADJU]*", NULL },
  /* Copy paste of the inner match of previous regexp */
  { "<substitution expression>",           MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "/[^\\/]*(?:\\.[^\\/]*)*/[^\\/]*(?:\\.[^\\/]*)*/", NULL },
  { "<substitution expression modifiers>", MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "[eijmnsxADJUgzZ]*", NULL },
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { "<single quoted string>",              MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "'[^\\']*(?:\\.[^\\']*)*'", NULL },
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\")(?:[^\\\"]*(?:\\.[^\\\"]*)*)(?:\"))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { "<double quoted string>",              MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "\"[^\\\"]*(?:\\.[^\\\"]*)*\"", NULL },
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"["}{-cdelim=>"]"} */
  /* Perl stringified version is: (?:(?|(?:\[)(?:[^\\\]]*(?:\\.[^\\\]]*)*)(?:\]))) */
  { "<character class regexp>",            MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL, "\\[(?:[^\\\\]]*(?:\\.[^\\\\]]*)*)\\]", NULL },
  { "<character class modifiers>",         MARPAESLIF_SYMBOL_TYPE_TERMINAL, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,     ":[eijmnsxADJUc]*", NULL },
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
