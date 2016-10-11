#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

/* It is very important here to list all the terminals first, and in order compatible */
/* with bootstrap_grammar_L0_terminals[] and bootstrap_grammar_L0_rules[] */
typedef enum bootstrap_grammar_L0_enum {
  L0_TERMINAL_RESERVED_EVENT_NAME = 0,
  L0_TERMINAL_WHITESPACE,
  L0_TERMINAL_PERL_COMMENT,
  L0_TERMINAL_CPLUSPLUS_COMMENT,
  L0_TERMINAL_C_COMMENT,
  L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_ALIAS,
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_ALIAS,
  L0_TERMINAL_OP_LOOSEN,
  L0_TERMINAL_OP_EQUAL_PRIORITY,
  L0_TERMINAL_BEFORE,
  L0_TERMINAL_AFTER,
  L0_TERMINAL_SIGN,
  L0_TERMINAL_INTEGER,
  L0_TERMINAL_TRUE,
  L0_TERMINAL_BOOLEAN,
  L0_TERMINAL_WORD_CHARACTER,
  L0_TERMINAL_DOUBLE_COLON,
  L0_TERMINAL_LATIN_ALPHABET_LETTER,
  L0_TERMINAL_LEFT_CURLY,
  L0_TERMINAL_RIGHT_CURLY,
  L0_TERMINAL_BRACKETED_NAME_STRING,
  L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET,
  L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET,
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
  L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD,
  L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT,
  /* ----- Non terminals ------ */
  L0_META_RESERVED_EVENT_NAME,
  L0_META_WHITESPACE,
  L0_META_PERL_COMMENT,
  L0_META_CPLUSPLUS_COMMENT,
  L0_META_C_COMMENT,
  L0_META_OP_DECLARE_ANY_GRAMMAR,
  L0_META_OP_DECLARE_TOP_GRAMMAR,
  L0_META_OP_DECLARE_LEX_GRAMMAR,
  L0_META_OP_LOOSEN,
  L0_META_OP_EQUAL_PRIORITY,
  L0_META_BEFORE_OR_AFTER,
  L0_META_SIGNED_INTEGER,
  L0_META_SIGN,
  L0_META_INTEGER,
  L0_META_TRUE,
  L0_META_BOOLEAN,
  L0_META_WORD_CHARACTER,
  L0_META_RESERVED_ACTION_NAME,
  L0_META_ONE_OR_MORE_WORD_CHARACTERS,
  L0_META_ZERO_OR_MORE_WORD_CHARACTERS,
  L0_META_PERL_IDENTIFIER,
  L0_META_DOUBLE_COLON,
  L0_META_PERL_NAME,
  L0_META_BARE_NAME,
  L0_META_STANDARD_NAME,
  L0_META_BRACKETED_NAME,
  L0_META_BRACKETED_NAME_STRING,
  L0_META_ARRAY_DESCRIPTOR,
  L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,
  L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,
  L0_META_RESULT_ITEM_DESCRIPTOR_LIST,
  L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR,
  L0_META_RESULT_ITEM_DESCRIPTOR,
  L0_META_SINGLE_QUOTED_STRING,
  L0_META_DOUBLE_QUOTED_STRING,
  L0_META_QUOTED_STRING,
  L0_META_CHARACTER_CLASS_REGEXP,
  L0_META_CHARACTER_CLASS,
  L0_META_CHARACTER_CLASS_MODIFIER,
  L0_META_CHARACTER_CLASS_MODIFIERS,
  L0_META_REGULAR_EXPRESSION,
  L0_META_REGULAR_EXPRESSION_MODIFIERS,
  L0_META_SUBSTITUTION_EXPRESSION,
  L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS
} bootstrap_grammar_L0_enum_t;

typedef struct bootstrap_grammar_terminal {
  bootstrap_grammar_L0_enum_t idi;                /* Identifier */
  char                      *descs;               /* Description */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  marpaESLIF_uint32_t        optioni;             /* Terminal option */
  PCRE2_SPTR                 originp;             /* UTF-8 encoding */
  PCRE2_SPTR                 substitutionp;       /* UTF-8 encoding */
  char                      *testFullMatchs;
  char                      *testPartialMatchs;
} bootstrap_grammar_terminal_t;

typedef struct bootstrap_grammar_meta {
  bootstrap_grammar_L0_enum_t idi;                /* Identifier */
  char                      *descs;               /* Description */
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
  int                           rhsip[10]; /* 10 is arbitrary - just to make C happy */
  int                           minimumi;
  int                           separatori;
  short                         properb;
} bootstrap_grammar_rule_t;

typedef enum bootstrap_level {
  BOOTSTRAP_LEVEL_G1,
  BOOTSTRAP_LEVEL_L0
} bootstrap_level_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_L0_metas[] = {
  { L0_META_RESERVED_EVENT_NAME,                "<meta reserved event name>" },
  { L0_META_WHITESPACE,                         "<meta whitespace>" },
  { L0_META_PERL_COMMENT,                       "<meta perl comment>" },
  { L0_META_CPLUSPLUS_COMMENT,                  "<meta cplusplus comment>" },
  { L0_META_C_COMMENT,                          "<meta c comment>" },
  { L0_META_OP_DECLARE_ANY_GRAMMAR,             "<meta op declare any grammar>" },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,             "<meta op declare top grammar>" },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,             "<meta op declare lex grammar>" },
  { L0_META_OP_LOOSEN,                          "<meta op loosen>" },
  { L0_META_OP_EQUAL_PRIORITY,                  "<meta op equal priority>" },
  { L0_META_BEFORE_OR_AFTER,                    "<meta before or after>" },
  { L0_META_SIGNED_INTEGER,                     "<meta signed integer>" },
  { L0_META_SIGN,                               "<meta sign>" },
  { L0_META_INTEGER,                            "<meta integer>" },
  { L0_META_TRUE,                               "<meta true>" },
  { L0_META_BOOLEAN,                            "<meta boolean>" },
  { L0_META_WORD_CHARACTER,                     "<meta word character>" },
  { L0_META_RESERVED_ACTION_NAME,               "<meta reserved action name>" },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,        "<meta one or more word characters>" },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,       "<meta zero or more word characters>" },
  { L0_META_PERL_IDENTIFIER,                    "<meta perl identifier>" },
  { L0_META_DOUBLE_COLON,                       "<meta double colon>" },
  { L0_META_PERL_NAME,                          "<meta perl name>" },
  { L0_META_BARE_NAME,                          "<meta bare name>" },
  { L0_META_STANDARD_NAME,                      "<meta standard name>" },
  { L0_META_BRACKETED_NAME,                     "<meta bracketed name>" },
  { L0_META_BRACKETED_NAME_STRING,              "<meta bracketed name string>" },
  { L0_META_ARRAY_DESCRIPTOR,                   "<meta array descriptor>" },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,      "<meta array descriptor left bracket>" },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,     "<meta array descriptor right bracket>" },
  { L0_META_RESULT_ITEM_DESCRIPTOR_LIST,        "<meta result item descriptor list>" },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR,   "<meta result item descriptor separator>" },
  { L0_META_RESULT_ITEM_DESCRIPTOR,             "<meta result item descriptor>" },
  { L0_META_SINGLE_QUOTED_STRING,               "<meta single quoted string>" },
  { L0_META_DOUBLE_QUOTED_STRING,               "<meta double quoted string>" },
  { L0_META_QUOTED_STRING,                      "<meta quoted string>" },
  { L0_META_CHARACTER_CLASS_REGEXP,             "<meta character class regexp>" },
  { L0_META_CHARACTER_CLASS,                    "<meta character class>" },
  { L0_META_CHARACTER_CLASS_MODIFIER,           "<meta character class modifier>" },
  { L0_META_CHARACTER_CLASS_MODIFIERS,          "<meta character class modifiers>" },
  { L0_META_REGULAR_EXPRESSION,                 "<meta regular expression>" },
  { L0_META_REGULAR_EXPRESSION_MODIFIERS,       "<meta regular expression modifiers>" },
  { L0_META_SUBSTITUTION_EXPRESSION,            "<meta substitution expression>" },
  { L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS,  "<meta substitution expression modifiers>" }
};

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_terminal_t bootstrap_grammar_L0_terminals[] = {
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
  { L0_TERMINAL_CPLUSPLUS_COMMENT,
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
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR,
    "<terminal op declare top grammar>",      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[0]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[0]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_ALIAS,
    "<terminal op declare top grammar alias>", MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::=", NULL,
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR,
    "<terminal op declare lex grammar>",       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[1]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[1]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_ALIAS,
    "<terminal op declare lex grammar alias>", MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
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
  { L0_TERMINAL_TRUE,
    "<terminal true>",                              MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    "1", NULL,
#ifndef MARPAESLIF_NTRACE
    "1", ""
#else
    NULL, NULL
#endif
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
  { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET,
    "<terminal array descriptor left bracket>",     MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "[", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET,
    "<terminal array descriptor right bracket>",    MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
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
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD,
    "<terminal character class modifier standard>", MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":[eijmnsxDJUuaN]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT,
    "<terminal character class modifier compat>",   MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    ":ic", NULL,
    NULL, NULL
  },
};

bootstrap_grammar_rule_t bootstrap_grammar_L0_rules[] = {
  { L0_META_RESERVED_EVENT_NAME,              "<rule reserved event name>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RESERVED_EVENT_NAME              },       -1,                   -1,      -1 },
  { L0_META_WHITESPACE,                       "<rule whitespace>",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WHITESPACE                       },       -1,                   -1,      -1 },
  { L0_META_PERL_COMMENT,                     "<rule perl comment>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_PERL_COMMENT                     },       -1,                   -1,      -1 },
  { L0_META_CPLUSPLUS_COMMENT,                "<rule cplusplus comment>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CPLUSPLUS_COMMENT                },       -1,                   -1,      -1 },
  { L0_META_C_COMMENT,                        "<rule c comment>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_C_COMMENT                        },       -1,                   -1,      -1 },
  { L0_META_OP_DECLARE_ANY_GRAMMAR,           "<rule op declare any grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR           },       -1,                   -1,      -1 },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,           "<rule op declare top grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR           },       -1,                   -1,      -1 },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,           "<rule op declare top grammar alias>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR_ALIAS     },       -1,                   -1,      -1 },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,           "<rule op declare lex grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR           },       -1,                   -1,      -1 },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,           "<rule op declare lex grammar alias>",       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR_ALIAS     },       -1,                   -1,      -1 },
  { L0_META_OP_LOOSEN,                        "<rule op loosen>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_LOOSEN                        },       -1,                   -1,      -1 },
  { L0_META_OP_EQUAL_PRIORITY,                "<rule op equal priority>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_EQUAL_PRIORITY                },       -1,                   -1,      -1 },
  { L0_META_BEFORE_OR_AFTER,                  "<rule before or after 1>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BEFORE                           },       -1,                   -1,      -1 },
  { L0_META_BEFORE_OR_AFTER,                  "<rule before or after 2>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_AFTER                            },       -1,                   -1,      -1 },
  { L0_META_SIGNED_INTEGER,                   "<rule signed integer 1>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_INTEGER                              },       -1,                   -1,      -1 },
  { L0_META_SIGNED_INTEGER,                   "<rule signed integer 2>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_SIGN,
                                                                                                                                  L0_META_INTEGER                              },       -1,                   -1,      -1 },
  { L0_META_SIGN,                             "<rule sign>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SIGN                             },       -1,                   -1,      -1 },
  { L0_META_INTEGER,                          "<rule integer>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_INTEGER                          },       -1,                   -1,      -1 },
  { L0_META_TRUE,                             "<rule true>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_TRUE                             },       -1,                   -1,      -1 },
  { L0_META_BOOLEAN,                          "<rule boolean>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BOOLEAN                          },       -1,                   -1,      -1 },
  { L0_META_WORD_CHARACTER     ,              "<rule word character>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WORD_CHARACTER                   },       -1,                   -1,      -1 },
  { L0_META_RESERVED_ACTION_NAME,             "<rule reserved action name>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_DOUBLE_COLON,
                                                                                                                                  L0_META_ONE_OR_MORE_WORD_CHARACTERS          },       -1,                   -1,      -1 },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,      "<rule one more word characters>",           MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },        1,                   -1,      -1 },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,     "<rule zero more word characters>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },        0,                   -1,      -1 },
  { L0_META_PERL_IDENTIFIER,                  "<rule perl identifier>",                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },        1,                   -1,      -1 },
  { L0_META_DOUBLE_COLON,                     "<rule double colon>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_DOUBLE_COLON                     },       -1,                   -1,      -1 },
  { L0_META_PERL_NAME,                        "<rule perl name>",                          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_PERL_IDENTIFIER                      },        1, L0_META_DOUBLE_COLON,       1 },
  { L0_META_BARE_NAME,                        "<rule bare name>",                          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },        1,                   -1,      -1 },
  { L0_META_STANDARD_NAME,                    "<rule standard name>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_LATIN_ALPHABET_LETTER,
                                                                                                                                  L0_META_ZERO_OR_MORE_WORD_CHARACTERS         },       -1,                   -1,      -1 },
  { L0_META_BRACKETED_NAME,                   "<rule bracketed name>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_LEFT_CURLY,
                                                                                                                                  L0_META_BRACKETED_NAME_STRING,
                                                                                                                                  L0_TERMINAL_RIGHT_CURLY                      },       -1,                   -1,      -1 },
  { L0_META_BRACKETED_NAME_STRING,            "<rule bracketed name string>",              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BRACKETED_NAME_STRING            },       -1,                   -1,      -1 },
  { L0_META_ARRAY_DESCRIPTOR,                 "<rule array descriptor>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,
                                                                                                                                  L0_META_RESULT_ITEM_DESCRIPTOR_LIST,
                                                                                                                                  L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET       },       -1,                   -1,      -1 },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,    "<rule array descriptor left bracket 1>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET    },       -1,                   -1,      -1 },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,    "<rule array descriptor left bracket 2>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET,
                                                                                                                                  L0_META_WHITESPACE                           },       -1,                   -1,      -1 },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,   "<rule array descriptor right bracket 1>",   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET   },       -1,                   -1,      -1 },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,   "<rule array descriptor right bracket 2>",   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_WHITESPACE,
                                                                                                                                  L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET   },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_LIST,      "<rule result item descriptor list>",        MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_RESULT_ITEM_DESCRIPTOR               },        0, L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR, 0 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR, "<rule result item descriptor separator 1>", MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_COMMA                            },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR, "<rule result item descriptor separator 2>", MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_COMMA,
                                                                                                                                  L0_META_WHITESPACE                           },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  1>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_START                            },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  2>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_LENGTH                           },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  3>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_G1START                          },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  4>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_G1LENGTH                         },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  5>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_NAME                             },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  6>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_LHS                              },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  7>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SYMBOL                           },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  8>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RULE                             },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor  9>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_VALUE                            },       -1,                   -1,      -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<rule result item descriptor 10>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_VALUES                           },       -1,                   -1,      -1 },
  { L0_META_SINGLE_QUOTED_STRING,             "<rule single quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SINGLE_QUOTED_STRING             },       -1,                   -1,      -1 },
  { L0_META_DOUBLE_QUOTED_STRING,             "<rule double quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_DOUBLE_QUOTED_STRING             },       -1,                   -1,      -1 },
  { L0_META_QUOTED_STRING,                    "<rule quoted string>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_SINGLE_QUOTED_STRING,
																  L0_META_DOUBLE_QUOTED_STRING                 },       -1,                   -1,      -1 },
  { L0_META_CHARACTER_CLASS_REGEXP,           "<rule character class regexp>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_REGEXP           },       -1,                   -1,      -1 },
  { L0_META_CHARACTER_CLASS,                  "<rule character class>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_CHARACTER_CLASS_REGEXP,
																  L0_META_CHARACTER_CLASS_MODIFIERS            },       -1,                   -1,      -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<rule character class modifier standard>",  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD},       -1,                   -1,      -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<rule character class modifier compat>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT  },       -1,                   -1,      -1 },
  { L0_META_CHARACTER_CLASS_MODIFIERS,        "<rule character class modifiers>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_CHARACTER_CLASS_MODIFIER             },        0,                   -1,       0 },
  { L0_META_REGULAR_EXPRESSION,               "<rule regular expression>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_REGULAR_EXPRESSION               },       -1,                   -1,      -1 },
  { L0_META_REGULAR_EXPRESSION_MODIFIERS,     "<rule regular expression modifiers>",       MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER      },        0,                   -1,       0 },
  { L0_META_SUBSTITUTION_EXPRESSION,          "<rule substitution expression>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION          },       -1,                   -1,      -1 },
  { L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS,"<rule substitution expression modifiers>",  MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER },        0,                   -1,       0 }
};
#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
