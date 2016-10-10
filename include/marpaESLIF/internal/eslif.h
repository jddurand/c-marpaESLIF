#ifndef MARPAESLIF_INTERNAL_ESLIF_H
#define MARPAESLIF_INTERNAL_ESLIF_H

/* This is a bit painful, but C89 does not allow union initializers but      */
/* the first element -; so this will be reworked on-the-fly in the bootstrap */
typedef struct bootstrap_grammar_L0_terminals {
  char                      *descs;               /* Description */
  marpaESLIF_terminal_type_t terminalType;        /* Terminal type */
  marpaESLIF_uint32_t        optioni;             /* Terminal option */
  PCRE2_SPTR                 originp;             /* UTF-8 encoding */
  PCRE2_SPTR                 substitutionp;       /* UTF-8 encoding */
  char                      *testFullMatchs;
  char                      *testPartialMatchs;
} bootstrap_grammar_L0_terminals_t;

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_L0_terminals_t bootstrap_grammar_L0_terminals[] = {
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
  { "<reserved event name>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":symbol", NULL,
#ifndef MARPAESLIF_NTRACE
    ":symbol", ":symb"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "whitespace",                          MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
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
  { "<perl comment>",                      MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "#[^\\n]*(?:\\n|\\z)", NULL,
#ifndef MARPAESLIF_NTRACE
    "# Comment up to the end of the buffer", "# Again a comment"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* <perl comment> adapted for C++ (//) only */
  { "<cplusplus comment>",                 MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
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
  { "<c comment>",                         MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "/\\*(?:[^\\*]+|\\*(?!/))*\\*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/* Comment including a newline \n here (too late) */", "/* Comment is starting"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op declare any grammar>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":\\[\\d+\\]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[0123]:=", ":[0"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op declare top grammar>",            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[0]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[0]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op declare top grammar alias>",      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::=", NULL,
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op declare lex grammar>",            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":[1]:=", NULL,
#ifndef MARPAESLIF_NTRACE
    ":[1]:=", ":["
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op declare lex grammar alias>",      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "~", NULL,
#ifndef MARPAESLIF_NTRACE
    "~", NULL
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op loosen>",                         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "||", NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "||", "|"
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<op equal priority>",                 MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "|", NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "|", NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "before",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "before", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "after",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "after", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<sign>",                              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[+-]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<integer>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\d]+", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<boolean>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[01]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<word character>",                    MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\w]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<double colon>",                      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<latin alphabet letter>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[a-zA-Z]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<left curly>",                        MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "<", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<right curly>",                       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ">", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<bracketed name string>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s\\w]+", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<left bracket>",                      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "[", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<right bracket>",                     MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "]", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "comma",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "start",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "start", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "length",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "length", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "g1start",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1start", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "g1length",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1length", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "name",                                MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "lhs",                                 MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lhs", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "symbol",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "symbol", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "rule",                                MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rule", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "value",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "value", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "values",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "values", NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { "<single quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
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
  { "<double quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
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
  { "<regular expression>",                MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/", "/a("
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<regular expression modifier>",       MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUuaN]", NULL,
    NULL, NULL
  },
  /* Copy paste of the inner match of previous regexp */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<substitution expression>",           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/[^\\/]*(?:\\.[^\\/]*)*/", NULL,
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/d$1f/", "/a(b)c/d$"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<substitution expression modifier>",  MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
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
  { "<character class regexp>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "\\[[^\\\\\\]]*(?:\\\\.[^\\\\\\]]*)*\\]", NULL,
#ifndef MARPAESLIF_NTRACE
    "[a-z]","[a-z"
#else
    NULL, NULL
#endif
    },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { "<character class modifier>",          MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":[eijmnsxDJUuaNc]*", NULL,
    NULL, NULL
  },
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_H */
