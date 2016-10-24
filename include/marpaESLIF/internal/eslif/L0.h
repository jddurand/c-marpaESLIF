#ifndef MARPAESLIF_INTERNAL_ESLIF_L0_H
#define MARPAESLIF_INTERNAL_ESLIF_L0_H

#include "marpaESLIF/internal/eslif/L0_join_G1.h"

/* Description of internal L0 grammar */

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
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR,
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
  L0_TERMINAL_START, /* Grammatically support for Marpa::R2 compatibility - no effect */
  L0_TERMINAL_LENGTH, /* Grammatically support for Marpa::R2 compatibility - no effect */
  L0_TERMINAL_G1START, /* Grammatically support for Marpa::R2 compatibility - no effect */
  L0_TERMINAL_G1LENGTH, /* Grammatically support for Marpa::R2 compatibility - no effect */
  L0_TERMINAL_NAME,
  L0_TERMINAL_LHS, /* Grammatically support for Marpa::R2 compatibility - no effect */
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
  L0_META_RESERVED_BLESSING_NAME,
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
  L0_META_SINGLE_QUOTED_NAME,
  L0_META_DOUBLE_QUOTED_NAME,
  L0_META_QUOTED_NAME,
  L0_META_CHARACTER_CLASS_REGEXP,
  L0_META_CHARACTER_CLASS,
  L0_META_CHARACTER_CLASS_MODIFIER,
  L0_META_CHARACTER_CLASS_MODIFIERS,
  L0_META_REGULAR_EXPRESSION,
  L0_META_REGULAR_EXPRESSION_MODIFIERS,
  L0_META_SUBSTITUTION_EXPRESSION,
  L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS
} bootstrap_grammar_L0_enum_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_L0_metas[] = {
  { L0_META_RESERVED_EVENT_NAME,                L0_JOIN_G1_META_RESERVED_EVENT_NAME, 0, 0 },
  { L0_META_WHITESPACE,                         L0_JOIN_G1_META_WHITESPACE, 0, 0 },
  { L0_META_PERL_COMMENT,                       L0_JOIN_G1_META_PERL_COMMENT, 0, 0 },
  { L0_META_CPLUSPLUS_COMMENT,                  L0_JOIN_G1_META_CPLUSPLUS_COMMENT, 0, 0 },
  { L0_META_C_COMMENT,                          L0_JOIN_G1_META_C_COMMENT, 0, 0 },
  { L0_META_OP_DECLARE_ANY_GRAMMAR,             L0_JOIN_G1_META_OP_DECLARE_ANY_GRAMMAR, 0, 0 },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,             L0_JOIN_G1_META_OP_DECLARE_TOP_GRAMMAR, 0, 0 },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,             L0_JOIN_G1_META_OP_DECLARE_LEX_GRAMMAR, 0, 0 },
  { L0_META_OP_LOOSEN,                          L0_JOIN_G1_META_OP_LOOSEN, 0, 0 },
  { L0_META_OP_EQUAL_PRIORITY,                  L0_JOIN_G1_META_OP_EQUAL_PRIORITY, 0, 0 },
  { L0_META_BEFORE_OR_AFTER,                    L0_JOIN_G1_META_BEFORE_OR_AFTER, 0, 0 },
  { L0_META_SIGNED_INTEGER,                     L0_JOIN_G1_META_SIGNED_INTEGER, 0, 0 },
  { L0_META_SIGN,                               "<L0 meta sign>", 0, 0 },
  { L0_META_INTEGER,                            L0_JOIN_G1_META_INTEGER, 0, 0 },
  { L0_META_TRUE,                               L0_JOIN_G1_META_TRUE, 0, 0 },
  { L0_META_BOOLEAN,                            L0_JOIN_G1_META_BOOLEAN, 0, 0 },
  { L0_META_WORD_CHARACTER,                     "<L0 meta word character>", 0, 0 },
  { L0_META_RESERVED_ACTION_NAME,               L0_JOIN_G1_META_RESERVED_ACTION_NAME, 0, 0 },
  { L0_META_RESERVED_BLESSING_NAME,             L0_JOIN_G1_META_RESERVED_BLESSING_NAME, 0, 0 },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,        "<L0 meta one or more word characters>", 0, 0 },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,       "<L0 meta zero or more word characters>", 0, 0 },
  { L0_META_PERL_IDENTIFIER,                    "<L0 meta perl identifier>", 0, 0 },
  { L0_META_DOUBLE_COLON,                       "<L0 meta double colon>", 0, 0 },
  { L0_META_PERL_NAME,                          L0_JOIN_G1_META_PERL_NAME, 0, 0 },
  { L0_META_BARE_NAME,                          L0_JOIN_G1_META_BARE_NAME, 0, 0 },
  { L0_META_STANDARD_NAME,                      L0_JOIN_G1_META_STANDARD_NAME, 0, 0 },
  { L0_META_BRACKETED_NAME,                     L0_JOIN_G1_META_BRACKETED_NAME, 0, 0 },
  { L0_META_BRACKETED_NAME_STRING,              "<L0 meta bracketed name string>", 0, 0 },
  { L0_META_ARRAY_DESCRIPTOR,                   L0_JOIN_G1_META_ARRAY_DESCRIPTOR, 0, 0 },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,      "<L0 meta array descriptor left bracket>", 0, 0 },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,     "<L0 meta array descriptor right bracket>", 0, 0 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_LIST,        "<L0 meta result item descriptor list>", 0, 0 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR,   "<L0 meta result item descriptor separator>", 0, 0 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,             "<L0 meta result item descriptor>", 0, 0 },
  { L0_META_SINGLE_QUOTED_STRING,               "<L0 meta single quoted string>", 0, 0 },
  { L0_META_DOUBLE_QUOTED_STRING,               "<L0 meta double quoted string>", 0, 0 },
  { L0_META_QUOTED_STRING,                      L0_JOIN_G1_META_QUOTED_STRING, 0, 0 },
  { L0_META_SINGLE_QUOTED_NAME,                 "<L0 meta single quoted name>", 0, 0 },
  { L0_META_DOUBLE_QUOTED_NAME,                 "<L0 meta double quoted name>", 0, 0 },
  { L0_META_QUOTED_NAME,                        L0_JOIN_G1_META_QUOTED_NAME, 0, 0 },
  { L0_META_CHARACTER_CLASS_REGEXP,             "<L0 meta character class regexp>", 0, 0 },
  { L0_META_CHARACTER_CLASS,                    L0_JOIN_G1_META_CHARACTER_CLASS, 0, 0 },
  { L0_META_CHARACTER_CLASS_MODIFIER,           "<L0 meta character class modifier>", 0, 0 },
  { L0_META_CHARACTER_CLASS_MODIFIERS,          "<L0 meta character class modifiers>", 0, 0 },
  { L0_META_REGULAR_EXPRESSION,                 L0_JOIN_G1_META_REGULAR_EXPRESSION, 0, 0 },
  { L0_META_REGULAR_EXPRESSION_MODIFIERS,       "<L0 meta regular expression modifiers>", 0, 0 },
  { L0_META_SUBSTITUTION_EXPRESSION,            L0_JOIN_G1_META_SUBSTITUTION_EXPRESSION, 0, 0 },
  { L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS,  "<L0 meta substitution expression modifiers>", 0, 0 }
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
    "<L0 terminal reserved event name>",               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":symbol",
#ifndef MARPAESLIF_NTRACE
    ":symbol", ":symb"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WHITESPACE,
    "<L0 terminal whitespace>",                        MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s]+",
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
    "<L0 terminal perl comment>",                      MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "#[^\\n]*(?:\\n|\\z)",
#ifndef MARPAESLIF_NTRACE
    "# Comment up to the end of the buffer", "# Again a comment"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* <perl comment> adapted for C++ (//) only */
  { L0_TERMINAL_CPLUSPLUS_COMMENT,
    "<L0 terminal cplusplus comment>",                 MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "//[^\\n]*(?:\\n|\\z)",
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
    "<L0 terminal c comment>",                         MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "/\\*(?:[^\\*]+|\\*(?!/))*\\*/",
#ifndef MARPAESLIF_NTRACE
    "/* Comment including a newline \n here (too late) */", "/* Comment is starting"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR,
    "<L0 terminal op declare any grammar>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":\\[(\\d+)\\]:=",
#ifndef MARPAESLIF_NTRACE
    ":[0123]:=", ":[0"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR,
    "<L0 terminal op declare top grammar>",       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::=",
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR,
    "<L0 terminal op declare lex grammar>",       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "~",
#ifndef MARPAESLIF_NTRACE
    "~", NULL
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_LOOSEN,
    "<L0 terminal op loosen>",                         MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "||",
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "||", "|"
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_EQUAL_PRIORITY,
    "<L0 terminal op equal priority>",                 MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "|",
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "|", NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BEFORE,
    "<L0 terminal before>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "before",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_AFTER,
    "<L0 terminal after>",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "after",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SIGN,
    "<L0 terminal sign>",                              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[+-]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_INTEGER,
    "<L0 terminal integer>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\d]+",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_TRUE,
    "<L0 terminal true>",                              MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    "1",
#ifndef MARPAESLIF_NTRACE
    "1", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BOOLEAN,
    "<L0 terminal boolean>",                           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[01]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WORD_CHARACTER,
    "<L0 terminal word character>",                    MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\w]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_DOUBLE_COLON,
    "<L0 terminal double colon>",                      MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LATIN_ALPHABET_LETTER,
    "<L0 terminal latin alphabet letter>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[a-zA-Z]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LEFT_CURLY,
    "<L0 terminal left curly>",                        MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "<",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RIGHT_CURLY,
    "<L0 terminal right curly>",                       MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ">",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BRACKETED_NAME_STRING,
    "<L0 terminal bracketed name string>",             MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s\\w]+",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET,
    "<L0 terminal array descriptor left bracket>",     MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "[",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET,
    "<L0 terminal array descriptor right bracket>",    MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_COMMA,
    "<L0 terminal comma>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_START,
    "<L0 terminal start>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "start",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LENGTH,
    "<L0 terminal length>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "length",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1START,
    "<L0 terminal g1start>",                           MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1start",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1LENGTH,
    "<L0 terminal g1length>",                          MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1length",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_NAME,
    "<L0 terminal name>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LHS,
    "<L0 terminal lhs>",                               MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lhs",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SYMBOL,
    "<L0 terminal symbol>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "symbol",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RULE,
    "<L0 terminal rule>",                              MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rule",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUE,
    "<L0 terminal value>",                             MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "value",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUES,
    "<L0 terminal values>",                            MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "values",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { L0_TERMINAL_SINGLE_QUOTED_STRING,
    "<L0 terminal single quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "'[^\\']*(?:\\.[^\\']*)*'",
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
    "<L0 terminal double quoted string>",              MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "\"[^\\\"]*(?:\\.[^\\\"]*)*\"",
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
    "<L0 terminal regular expression>",                MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/",
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/", "/a("
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER,
    "<L0 terminal regular expression modifier>",       MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUuaN]",
    NULL, NULL
  },
  /* Copy paste of the inner match of previous regexp */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION,
    "<L0 terminal substitution expression>",           MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/[^\\/]*(?:\\.[^\\/]*)*/",
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/d$1f/", "/a(b)c/d$"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER,
    "<L0 terminal substitution expression modifier>",  MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUugzZaN]",
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
    "<L0 terminal character class regexp>",            MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "\\[[^\\\\\\]]*(?:\\\\.[^\\\\\\]]*)*\\]",
#ifndef MARPAESLIF_NTRACE
    "[a-z]","[a-z"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD,
    "<L0 terminal character class modifier standard>", MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":[eijmnsxDJUuaN]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT,
    "<L0 terminal character class modifier compat>",   MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    ":ic",
    NULL, NULL
  },
};

bootstrap_grammar_rule_t bootstrap_grammar_L0_rules[] = {
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori propertb
  */
  { L0_META_RESERVED_EVENT_NAME,              "<L0 rule reserved event name>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RESERVED_EVENT_NAME              }, { 1 },            -1,                                       -1, -1 },
  { L0_META_WHITESPACE,                       "<L0 rule whitespace>",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WHITESPACE                       }, { 1 },            -1,                                       -1, -1 },
  { L0_META_PERL_COMMENT,                     "<L0 rule perl comment>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_PERL_COMMENT                     }, { 1 },            -1,                                       -1, -1 },
  { L0_META_CPLUSPLUS_COMMENT,                "<L0 rule cplusplus comment>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CPLUSPLUS_COMMENT                }, { 1 },            -1,                                       -1, -1 },
  { L0_META_C_COMMENT,                        "<L0 rule c comment>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_C_COMMENT                        }, { 1 },            -1,                                       -1, -1 },
  { L0_META_OP_DECLARE_ANY_GRAMMAR,           "<L0 rule op declare any grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,           "<L0 rule op declare top grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,           "<L0 rule op declare lex grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_OP_LOOSEN,                        "<L0 rule op loosen>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_LOOSEN                        }, { 1 },            -1,                                       -1, -1 },
  { L0_META_OP_EQUAL_PRIORITY,                "<L0 rule op equal priority>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_EQUAL_PRIORITY                }, { 1 },            -1,                                       -1, -1 },
  { L0_META_BEFORE_OR_AFTER,                  "<L0 rule before or after 1>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BEFORE                           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_BEFORE_OR_AFTER,                  "<L0 rule before or after 2>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_AFTER                            }, { 1 },            -1,                                       -1, -1 },
  { L0_META_SIGNED_INTEGER,                   "<L0 rule signed integer 1>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_INTEGER                              }, { 1 },            -1,                                       -1, -1 },
  { L0_META_SIGNED_INTEGER,                   "<L0 rule signed integer 2>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_SIGN,
                                                                                                                                  L0_META_INTEGER                              }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_SIGN,                             "<L0 rule sign>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SIGN                             }, { 1 },            -1,                                       -1, -1 },
  { L0_META_INTEGER,                          "<L0 rule integer>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_INTEGER                          }, { 1 },            -1,                                       -1, -1 },
  { L0_META_TRUE,                             "<L0 rule true>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_TRUE                             }, { 1 },            -1,                                       -1, -1 },
  { L0_META_BOOLEAN,                          "<L0 rule boolean>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BOOLEAN                          }, { 1 },            -1,                                       -1, -1 },
  { L0_META_WORD_CHARACTER     ,              "<L0 rule word character>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WORD_CHARACTER                   }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESERVED_ACTION_NAME,             "<L0 rule reserved action name>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_DOUBLE_COLON,
                                                                                                                                  L0_META_ONE_OR_MORE_WORD_CHARACTERS          }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_RESERVED_BLESSING_NAME,           "<L0 rule reserved blessing name>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_DOUBLE_COLON,
                                                                                                                                  L0_META_ONE_OR_MORE_WORD_CHARACTERS          }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,      "<L0 rule one more word characters>",           MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 1 },             1,                                       -1, -1 },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,     "<L0 rule zero more word characters>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 1 },             0,                                       -1, -1 },
  { L0_META_PERL_IDENTIFIER,                  "<L0 rule perl identifier>",                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 1 },             1,                                       -1, -1 },
  { L0_META_DOUBLE_COLON,                     "<L0 rule double colon>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_DOUBLE_COLON                     }, { 1 },            -1                                        -1, -1 },
  { L0_META_PERL_NAME,                        "<L0 rule perl name>",                          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_PERL_IDENTIFIER                      }, { 1 },             1,                     L0_META_DOUBLE_COLON,  1 },
  { L0_META_BARE_NAME,                        "<L0 rule bare name>",                          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 1 },             1,                                       -1, -1 },
  { L0_META_STANDARD_NAME,                    "<L0 rule standard name>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_LATIN_ALPHABET_LETTER,
                                                                                                                                  L0_META_ZERO_OR_MORE_WORD_CHARACTERS         }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_BRACKETED_NAME,                   "<L0 rule bracketed name>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_LEFT_CURLY,
                                                                                                                                  L0_META_BRACKETED_NAME_STRING,
                                                                                                                                  L0_TERMINAL_RIGHT_CURLY                      }, { 1, 1, 1},       -1,                                       -1, -1 },
  { L0_META_BRACKETED_NAME_STRING,            "<L0 rule bracketed name string>",              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BRACKETED_NAME_STRING            }, { 1 },            -1,                                       -1, -1 },
  { L0_META_ARRAY_DESCRIPTOR,                 "<L0 rule array descriptor>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,
                                                                                                                                  L0_META_RESULT_ITEM_DESCRIPTOR_LIST,
                                                                                                                                  L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET       }, { 1, 1, 1},       -1,                                       -1, -1 },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,    "<L0 rule array descriptor left bracket 1>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET    }, { 1 },            -1,                                       -1, -1 },
  { L0_META_ARRAY_DESCRIPTOR_LEFT_BRACKET,    "<L0 rule array descriptor left bracket 2>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_ARRAY_DESCRIPTOR_LEFT_BRACKET,
                                                                                                                                  L0_META_WHITESPACE                           }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,   "<L0 rule array descriptor right bracket 1>",   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET   }, { 1 },            -1,                                       -1, -1 },
  { L0_META_ARRAY_DESCRIPTOR_RIGHT_BRACKET,   "<L0 rule array descriptor right bracket 2>",   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_WHITESPACE,
                                                                                                                                  L0_TERMINAL_ARRAY_DESCRIPTOR_RIGHT_BRACKET   }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_LIST,      "<L0 rule result item descriptor list>",        MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_RESULT_ITEM_DESCRIPTOR               }, { 1 },             0, L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR,  0 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR, "<L0 rule result item descriptor separator 1>", MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_COMMA                            }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR_SEPARATOR, "<L0 rule result item descriptor separator 2>", MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_COMMA,
                                                                                                                                  L0_META_WHITESPACE                           }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  1>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_START                            }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  2>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_LENGTH                           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  3>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_G1START                          }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  4>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_G1LENGTH                         }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  5>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_NAME                             }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  6>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_LHS                              }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  7>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SYMBOL                           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  8>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RULE                             }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor  9>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_VALUE                            }, { 1 },            -1,                                       -1, -1 },
  { L0_META_RESULT_ITEM_DESCRIPTOR,           "<L0 rule result item descriptor 10>",          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_VALUES                           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_SINGLE_QUOTED_STRING,             "<L0 rule single quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_SINGLE_QUOTED_STRING,
                                                                                                                                  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_DOUBLE_QUOTED_STRING,             "<L0 rule double quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_DOUBLE_QUOTED_STRING,
                                                                                                                                  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_QUOTED_STRING,                    "<L0 rule quoted string 1>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_SINGLE_QUOTED_STRING                 }, { 1 },            -1,                                       -1, -1 },
  { L0_META_QUOTED_STRING,                    "<L0 rule quoted string 2>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_DOUBLE_QUOTED_STRING                 }, { 1 },            -1,                                       -1, -1 },
  { L0_META_SINGLE_QUOTED_NAME,               "<L0 rule single quoted name>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SINGLE_QUOTED_STRING             }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_DOUBLE_QUOTED_NAME,               "<L0 rule double quoted name>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_DOUBLE_QUOTED_STRING             }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_QUOTED_NAME,                      "<L0 rule quoted name 1>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_SINGLE_QUOTED_NAME                   }, { 1 },            -1,                                       -1, -1 },
  { L0_META_QUOTED_NAME,                      "<L0 rule quoted name 2>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_DOUBLE_QUOTED_NAME                   }, { 1 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_REGEXP,           "<L0 rule character class regexp>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_REGEXP           }, { 1 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS,                  "<L0 rule character class>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_CHARACTER_CLASS_REGEXP,
																  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 1, 1 },         -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<L0 rule character class modifier standard>",  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD}, { 1 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<L0 rule character class modifier compat>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT  }, { 1 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_MODIFIERS,        "<L0 rule character class modifiers>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_CHARACTER_CLASS_MODIFIER             }, { 1 },             0,                                       -1,  0 },
  { L0_META_REGULAR_EXPRESSION,               "<L0 rule regular expression>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_REGULAR_EXPRESSION               }, { 1 },            -1,                                       -1, -1 },
  { L0_META_REGULAR_EXPRESSION_MODIFIERS,     "<L0 rule regular expression modifiers>",       MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER      }, { 1 },             0,                                       -1,  0 },
  { L0_META_SUBSTITUTION_EXPRESSION,          "<L0 rule substitution expression>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION          }, { 1 },            -1,                                       -1, -1 },
  { L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS,"<L0 rule substitution expression modifiers>",  MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER }, { 1 },             0,                                       -1,  0 }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_L0_H */
