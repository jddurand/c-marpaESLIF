#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

/* It is very important here to list all the terminals first, and in order compatible */
/* with bootstrap_grammar_L0_terminals[] and bootstrap_grammar_L0_rules[] */
typedef enum bootstrap_grammar_L0_enum {
  L0_TERMINAL_RESERVED_EVENT_NAME = 0,
  L0_TERMINAL_WHITESPACE,
  L0_TERMINAL_PERL_COMMENT,
  L0_TERMINAL_CPLUSCPLUS_COMMENT,
  L0_TERMINAL_C_COMMENT,
  L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_MATCH,
  L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_MATCH_ALIAS,
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_MATCH,
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_MATCH_ALIAS,
  L0_TERMINAL_OP_LOOSEN,
  L0_TERMINAL_OP_EQUAL_PRIORITY,
  L0_TERMINAL_BEFORE,
  L0_TERMINAL_AFTER,
  L0_TERMINAL_SIGN,
  L0_TERMINAL_INTEGER,
  L0_TERMINAL_BOOLEAN,
  L0_TERMINAL_WORD_CHARACTER,
  L0_TERMINAL_DOUBLE_COLON,
  L0_TERMINAL_LATIN_ALPHABET_LETTER,
  L0_TERMINAL_LEFT_CURLY,
  L0_TERMINAL_RIGHT_CURLY,
  L0_TERMINAL_BRACKETED_NAME_STRING,
  L0_TERMINAL_LEFT_BRACKET,
  L0_TERMINAL_RIGHT_BRACKET,
  L0_TERMINAL_COMMA,
  L0_TERMINAL_START,
  L0_TERMINAL_LENGTH,
  L0_TERMINAL_G1START,
  L0_TERMINAL_G1LENGTH,
  L0_TERMINAL_NAME,
  L0_TERMINAL_LHS,
  L0_TERMINAL_SYMBOL,
  L0_TERMINAL_RULE,
  L0_TERMINAL_VALUE,
  L0_TERMINAL_VALUES,
  L0_TERMINAL_SINGLE_QUOTED_STRING,
  L0_TERMINAL_DOUBLE_QUOTED_STRING,
  L0_TERMINAL_REGULAR_EXPRESSION,
  L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER,
  L0_TERMINAL_SUBSTITUTION_EXPRESSION,
  L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER,
  L0_TERMINAL_CHARACTER_CLASS_REGEXP,
  L0_TERMINAL_CHARACTER_CLASS_MODIFIER,
  /* ----- Non terminals ------ */
  L0_META_RESERVED_EVENT_NAME,
  L0_META_WHITESPACE,
  L0_META_OP_DECLARE_TOP_GRAMMAR,
  L0_META_OP_DECLARE_LEX_GRAMMAR,
  L0_META_BEFORE_OR_AFTER,
  L0_META_SIGNED_INTEGER,
  L0_META_RESERVED_ACTION_NAME,
  L0_META_ONE_OR_MORE_WORD_CHARACTERS,
  L0_META_ZERO_OR_MORE_WORD_CHARACTERS,
  L0_META_PERL_NAME,
  L0_META_BARE_NAME,
  L0_META_STANDARD_NAME,
  L0_META_BRACKETED_NAME,
  L0_META_ARRAY_DESCRIPTOR
} bootstrap_grammar_L0_enum_t;

typedef struct bootstrap_grammar_L0_terminal {
  bootstrap_grammar_L0_enum_t idi;                /* Identifier */
  char                      *descs;               /* Description */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  marpaESLIF_uint32_t        optioni;             /* Terminal option */
  PCRE2_SPTR                 originp;             /* UTF-8 encoding */
  PCRE2_SPTR                 substitutionp;       /* UTF-8 encoding */
  char                      *testFullMatchs;
  char                      *testPartialMatchs;
} bootstrap_grammar_L0_terminal_t;

typedef struct bootstrap_grammar_L0_meta {
  bootstrap_grammar_L0_enum_t idi;                /* Identifier */
  char                      *descs;               /* Description */
} bootstrap_grammar_L0_meta_t;

/* All non-terminals are listed here */
bootstrap_grammar_L0_meta_t bootstrap_grammar_L0_metas[] = {
  { L0_META_RESERVED_EVENT_NAME,          "<meta reserved event name>" },
  { L0_META_WHITESPACE,                   "<meta whitespace>" },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,       "<op declare top grammar>" },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,       "<op declare lex grammar>" },
  { L0_META_BEFORE_OR_AFTER,              "<before or after>" },
  { L0_META_SIGNED_INTEGER,               "<signed integer>" },
  { L0_META_RESERVED_ACTION_NAME,         "<reserved action name>" },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,  "<one or more word characters>" },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS, "<zero or more word characters>" },
  { L0_META_PERL_NAME,                    "<Perl name>" },
  { L0_META_BARE_NAME,                    "<Bare name>" },
  { L0_META_STANDARD_NAME,                "<standard name>" },
  { L0_META_BRACKETED_NAME,               "<bracketed name>" },
  { L0_META_ARRAY_DESCRIPTOR,             "<array descriptor>" }
};

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_L0_terminal_t bootstrap_grammar_L0_terminals[] = {
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
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /*                                                             TERMINALS                                                             */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RESERVED_EVENT_NAME,
    "<terminal reserved event name>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":symbol", NULL,
#ifndef MARPAESLIF_NTRACE
    ":symbol", ":symb"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WHITESPACE,
    "<terminal whitespace>",                        MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s]+", NULL,
#ifndef MARPAESLIF_NTRACE
    "\x09\x20xxx", "\x09\x20"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::comment, $RE{comment}{Perl} */
  /* Perl stringified version is: (?:(?:#)(?:[^\n]*)(?:\n)) */
  /* Simplified to (without the [/'"] escapes): #[^\n]*\n */
  /* I add \\z to match the end of buffer */
  { L0_TERMINAL_PERL_COMMENT,
    "<terminal perl comment>",                      MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "#[^\\n]*(?:\\n|\\z)", NULL,
#ifndef MARPAESLIF_NTRACE
    "# Comment up to the end of the buffer", "# Again a comment"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* <perl comment> adapted for C++ (//) only */
  { L0_TERMINAL_CPLUSCPLUS_COMMENT,
    "<terminal cplusplus comment>",                 MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "//[^\\n]*(?:\\n|\\z)", NULL,
#ifndef MARPAESLIF_NTRACE
    "// Comment up to the end of the buffer", "// Again a comment"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::comment, $RE{comment}{C} */
  /* Perl stringified version is: (?:(?:\/\*)(?:(?:[^\*]+|\*(?!\/))*)(?:\*\/)) */
  /* Simplified to (without the [/"'] escapes) /\*(?:[^\*]+|\*(?!/))*\!*!/ */   /* REMOVE THE ! character - this is C compilers -; */
  { L0_TERMINAL_C_COMMENT,
    "<terminal c comment>",                         MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "/\\*(?:[^\\*]+|\\*(?!/))*\\*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/* Comment including a newline \n here (too late) */", "/* Comment is starting"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR,
    "<terminal op declare any grammar>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":\\[(\\d+)\\]:=", "$1",
#ifndef MARPAESLIF_NTRACE
    ":[0123]:=", ":[0"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_MATCH,
    "<terminal op declare top grammar match>",      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[0]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[0]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_MATCH_ALIAS,
    "<terminal op declare top grammar match alias>", MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::=", NULL,
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_MATCH,
    "<terminal op declare lex grammar match>",       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[1]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[1]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_MATCH_ALIAS,
    "<terminal op declare lex grammar match alias>", MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "~", NULL,
#ifndef MARPAESLIF_NTRACE
    "~", NULL
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_LOOSEN,
    "<terminal op loosen>",                         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "||", NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "||", "|"
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_EQUAL_PRIORITY,
    "<terminal op equal priority>",                 MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "|", NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "|", NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BEFORE,
    "<terminal before>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "before", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_AFTER,
    "<terminal after>",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "after", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SIGN,
    "<terminal sign>",                              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[+-]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_INTEGER,
    "<terminal integer>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\d]+", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BOOLEAN,
    "<terminal boolean>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[01]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WORD_CHARACTER,
    "<terminal word character>",                    MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\w]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_DOUBLE_COLON,
    "<terminal double colon>",                      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LATIN_ALPHABET_LETTER,
    "<terminal latin alphabet letter>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[a-zA-Z]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LEFT_CURLY,
    "<terminal left curly>",                        MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "<", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RIGHT_CURLY,
    "<terminal right curly>",                       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ">", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BRACKETED_NAME_STRING,
    "<terminal bracketed name string>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s\\w]+", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LEFT_BRACKET,
    "<terminal left bracket>",                      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "[", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RIGHT_BRACKET,
    "<terminal right bracket>",                     MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_COMMA,
    "<terminal comma>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_START,
    "<terminal start>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "start", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LENGTH,
    "<terminal length>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "length", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1START,
    "<terminal g1start>",                           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1start", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1LENGTH,
    "<terminal g1length>",                          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1length", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_NAME,
    "<terminal name>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LHS,
    "<terminal lhs>",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lhs", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SYMBOL,
    "<terminal symbol>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "symbol", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RULE,
    "<terminal rule>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rule", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUE,
    "<terminal value>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "value", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUES,
    "<terminal values>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "values", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { L0_TERMINAL_SINGLE_QUOTED_STRING,
    "<terminal single quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "'[^\\']*(?:\\.[^\\']*)*'", NULL,
#ifndef MARPAESLIF_NTRACE
    "'A string'", "'"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\")(?:[^\\\"]*(?:\\.[^\\\"]*)*)(?:\"))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { L0_TERMINAL_DOUBLE_QUOTED_STRING,
    "<terminal double quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "\"[^\\\"]*(?:\\.[^\\\"]*)*\"", NULL,
#ifndef MARPAESLIF_NTRACE
    "\"A string\"", "\""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"/"}{-cdelim=>"/"} */
  /* Perl stringified version is: (?:(?|(?:\/)(?:[^\\\/]*(?:\\.[^\\\/]*)*)(?:\/))) */
  /* Simplified to (without the [/"'] escapes) /[^\\/]*(?:\\.[^\\/]*)!*!/ */   /* REMOVE THE ! character - this is C compilers -; */
  { L0_TERMINAL_REGULAR_EXPRESSION,
    "<terminal regular expression>",                MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/", "/a("
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER,
    "<terminal regular expression modifier>",       MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUuaN]", NULL,
    NULL, NULL
  },
  /* Copy paste of the inner match of previous regexp */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION,
    "<terminal substitution expression>",           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/[^\\/]*(?:\\.[^\\/]*)*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/d$1f/", "/a(b)c/d$"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER,
    "<terminal substitution expression modifier>",  MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUugzZaN]", NULL,
#ifndef MARPAESLIF_NTRACE
    "e", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"["}{-cdelim=>"]"} */
  /* Perl stringified version is: (?:(?|(?:\[)(?:[^\\\]]*(?:\\.[^\\\]]*)*)(?:\]))) */
  { L0_TERMINAL_CHARACTER_CLASS_REGEXP,
    "<terminal character class regexp>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "\\[[^\\\\\\]]*(?:\\\\.[^\\\\\\]]*)*\\]", NULL,
#ifndef MARPAESLIF_NTRACE
    "[a-z]","[a-z"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER,
    "<terminal character class modifier>",          MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":[eijmnsxDJUuaNc]*", NULL,
    NULL, NULL
  },
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
