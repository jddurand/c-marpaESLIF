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
  L0_TERMINAL_LATIN_ALPHABET_LETTER,
  L0_TERMINAL_LEFT_CURLY,
  L0_TERMINAL_RIGHT_CURLY,
  L0_TERMINAL_BRACKETED_NAME_STRING,
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
  L0_TERMINAL_ASCII_GRAPH_CHARACTERS,
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
  L0_META_ONE_OR_MORE_WORD_CHARACTERS,
  L0_META_ZERO_OR_MORE_WORD_CHARACTERS,
  L0_META_PERL_IDENTIFIER,
  L0_META_ASCII_GRAPH_NAME,
  L0_META_BARE_NAME,
  L0_META_STANDARD_NAME,
  L0_META_BRACKETED_NAME,
  L0_META_BRACKETED_NAME_STRING,
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
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,        "<L0 meta one or more word characters>", 0, 0 },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,       "<L0 meta zero or more word characters>", 0, 0 },
  { L0_META_PERL_IDENTIFIER,                    "<L0 meta perl identifier>", 0, 0 },
  { L0_META_ASCII_GRAPH_NAME,                   L0_JOIN_G1_META_ASCII_GRAPH_NAME, 0, 0 },
  { L0_META_BARE_NAME,                          L0_JOIN_G1_META_BARE_NAME, 0, 0 },
  { L0_META_STANDARD_NAME,                      L0_JOIN_G1_META_STANDARD_NAME, 0, 0 },
  { L0_META_BRACKETED_NAME,                     L0_JOIN_G1_META_BRACKETED_NAME, 0, 0 },
  { L0_META_BRACKETED_NAME_STRING,              "<L0 meta bracketed name string>", 0, 0 },
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
  { L0_TERMINAL_RESERVED_EVENT_NAME, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ":symbol",
#ifndef MARPAESLIF_NTRACE
    ":symbol", ":symb"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WHITESPACE, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
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
  { L0_TERMINAL_PERL_COMMENT, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "#[^\\n]*(?:\\n|\\z)",
#ifndef MARPAESLIF_NTRACE
    "# Comment up to the end of the buffer", "# Again a comment"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* <perl comment> adapted for C++ (//) only */
  { L0_TERMINAL_CPLUSPLUS_COMMENT, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
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
  { L0_TERMINAL_C_COMMENT, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "/\\*(?:[^\\*]+|\\*(?!/))*\\*/",
#ifndef MARPAESLIF_NTRACE
    "/* Comment including a newline \n here (too late) */", "/* Comment is starting"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":\\[(\\d+)\\]:=",
#ifndef MARPAESLIF_NTRACE
    ":[0123]:=", ":[0"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "::=",
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "~",
#ifndef MARPAESLIF_NTRACE
    "~", NULL
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_LOOSEN, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "||",
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "||", "|"
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_EQUAL_PRIORITY, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "|",
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "|", NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BEFORE, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "before",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_AFTER, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "after",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SIGN, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[+-]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_INTEGER, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\d]+",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_TRUE, MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    "1",
#ifndef MARPAESLIF_NTRACE
    "1", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BOOLEAN, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[01]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WORD_CHARACTER, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\w]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LATIN_ALPHABET_LETTER, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[a-zA-Z]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LEFT_CURLY, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "<",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RIGHT_CURLY, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ">",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BRACKETED_NAME_STRING, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[\\s\\w]+",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_COMMA, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    ",",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_START, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "start",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LENGTH, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "length",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1START, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1start",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1LENGTH, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "g1length",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_NAME, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "name",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LHS, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "lhs",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SYMBOL, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "symbol",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RULE, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "rule",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUE, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "value",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUES, MARPAESLIF_TERMINAL_TYPE_STRING,  MARPAESLIF_REGEX_OPTION_NA,
    "values",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"'"}{-cdelim=>"'"} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))) */
  /* Simplified to (without the [/"'] escapes) "[^\\"]*(?:\\.[^\\"]*)*" */
  { L0_TERMINAL_SINGLE_QUOTED_STRING, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
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
  { L0_TERMINAL_DOUBLE_QUOTED_STRING, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
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
  { L0_TERMINAL_REGULAR_EXPRESSION, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/",
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/", "/a("
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUuaN]",
    NULL, NULL
  },
  /* Copy paste of the inner match of previous regexp */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "/[^\\/]*(?:\\.[^\\/]*)*/[^\\/]*(?:\\.[^\\/]*)*/",
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/d$1f/", "/a(b)c/d$"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[eijmnsxDJUugzZaN]",
#ifndef MARPAESLIF_NTRACE
    "e", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::balanced, $RE{balanced}{-parens=>'[]'} */
  /* Perl stringified version is: ((?:\[(?:(?>[^\[\]]+)|(?-1))*\])) */
  { L0_TERMINAL_CHARACTER_CLASS_REGEXP, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_DOTALL,
    "((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))",
#ifndef MARPAESLIF_NTRACE
    "[[:alnum]]","[a-z"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    ":[eijmnsxDJUuaN]",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT, MARPAESLIF_TERMINAL_TYPE_STRING,   MARPAESLIF_REGEX_OPTION_NA,
    ":ic",
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_ASCII_GRAPH_CHARACTERS, MARPAESLIF_TERMINAL_TYPE_REGEX,   MARPAESLIF_REGEX_OPTION_NA,
    "[[:ascii:]]+",
    NULL, NULL
  }
};

bootstrap_grammar_rule_t bootstrap_grammar_L0_rules[] = {
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_RESERVED_EVENT_NAME,              "<L0 rule reserved event name>",                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RESERVED_EVENT_NAME              }, { 0 },            -1,                                       -1, -1 },
  { L0_META_WHITESPACE,                       "<L0 rule whitespace>",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WHITESPACE                       }, { 0 },            -1,                                       -1, -1 },
  { L0_META_PERL_COMMENT,                     "<L0 rule perl comment>",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_PERL_COMMENT                     }, { 0 },            -1,                                       -1, -1 },
  { L0_META_CPLUSPLUS_COMMENT,                "<L0 rule cplusplus comment>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CPLUSPLUS_COMMENT                }, { 0 },            -1,                                       -1, -1 },
  { L0_META_C_COMMENT,                        "<L0 rule c comment>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_C_COMMENT                        }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_OP_DECLARE_ANY_GRAMMAR,           "<L0 rule op declare any grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR           }, { 0 },            -1,                                       -1, -1 },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,           "<L0 rule op declare top grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR           }, { 0 },            -1,                                       -1, -1 },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,           "<L0 rule op declare lex grammar>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR           }, { 0 },            -1,                                       -1, -1 },
  { L0_META_OP_LOOSEN,                        "<L0 rule op loosen>",                          MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_LOOSEN                        }, { 0 },            -1,                                       -1, -1 },
  { L0_META_OP_EQUAL_PRIORITY,                "<L0 rule op equal priority>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_EQUAL_PRIORITY                }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_BEFORE_OR_AFTER,                  "<L0 rule before or after 1>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BEFORE                           }, { 0 },            -1,                                       -1, -1 },
  { L0_META_BEFORE_OR_AFTER,                  "<L0 rule before or after 2>",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_AFTER                            }, { 0 },            -1,                                       -1, -1 },
  { L0_META_SIGNED_INTEGER,                   "<L0 rule signed integer 1>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_INTEGER                              }, { 0 },            -1,                                       -1, -1 },
  { L0_META_SIGNED_INTEGER,                   "<L0 rule signed integer 2>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_SIGN,
                                                                                                                                  L0_META_INTEGER                              }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_SIGN,                             "<L0 rule sign>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SIGN                             }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_INTEGER,                          "<L0 rule integer>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_INTEGER                          }, { 0 },            -1,                                       -1, -1 },
  { L0_META_TRUE,                             "<L0 rule true>",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_TRUE                             }, { 0 },            -1,                                       -1, -1 },
  { L0_META_BOOLEAN,                          "<L0 rule boolean>",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BOOLEAN                          }, { 0 },            -1,                                       -1, -1 },
  { L0_META_WORD_CHARACTER     ,              "<L0 rule word character>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WORD_CHARACTER                   }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,      "<L0 rule one more word characters>",           MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 0 },             1,                                       -1, -1 },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,     "<L0 rule zero more word characters>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 0 },             0,                                       -1, -1 },
  { L0_META_PERL_IDENTIFIER,                  "<L0 rule perl identifier>",                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       }, { 0 },             1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_ASCII_GRAPH_NAME,                 "<L0 rule ascii graph name>",                   MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_ASCII_GRAPH_CHARACTERS        }, { 0 },            -1                                        -1, -1 },
  { L0_META_BARE_NAME,                        "<L0 rule bare name>",                          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                    }, { 0 },             1,                                       -1, -1 },
  { L0_META_STANDARD_NAME,                    "<L0 rule standard name>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_LATIN_ALPHABET_LETTER,
                                                                                                                                  L0_META_ZERO_OR_MORE_WORD_CHARACTERS         }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_BRACKETED_NAME,                   "<L0 rule bracketed name>",                     MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_LEFT_CURLY,
                                                                                                                                  L0_META_BRACKETED_NAME_STRING,
                                                                                                                                  L0_TERMINAL_RIGHT_CURLY                      }, { 0, 0, 0},       -1,                                       -1, -1 },
  { L0_META_BRACKETED_NAME_STRING,            "<L0 rule bracketed name string>",              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BRACKETED_NAME_STRING            }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_SINGLE_QUOTED_STRING,             "<L0 rule single quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_SINGLE_QUOTED_STRING,
                                                                                                                                  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_DOUBLE_QUOTED_STRING,             "<L0 rule double quoted string>",               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_DOUBLE_QUOTED_STRING,
                                                                                                                                  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 0, 0 },         -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_QUOTED_STRING,                    "<L0 rule quoted string 1>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_SINGLE_QUOTED_STRING                 }, { 0 },            -1,                                       -1, -1 },
  { L0_META_QUOTED_STRING,                    "<L0 rule quoted string 2>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_DOUBLE_QUOTED_STRING                 }, { 0 },            -1,                                       -1, -1 },
  { L0_META_SINGLE_QUOTED_NAME,               "<L0 rule single quoted name>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SINGLE_QUOTED_STRING             }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_DOUBLE_QUOTED_NAME,               "<L0 rule double quoted name>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_DOUBLE_QUOTED_STRING             }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_QUOTED_NAME,                      "<L0 rule quoted name 1>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_SINGLE_QUOTED_NAME                   }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_QUOTED_NAME,                      "<L0 rule quoted name 2>",                      MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_META_DOUBLE_QUOTED_NAME                   }, { 0 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_REGEXP,           "<L0 rule character class regexp>",             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_REGEXP           }, { 0 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS,                  "<L0 rule character class>",                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_META_CHARACTER_CLASS_REGEXP,
																  L0_META_CHARACTER_CLASS_MODIFIERS            }, { 0, 0 },         -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<L0 rule character class modifier standard>",  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_STANDARD}, { 0 },            -1,                                       -1, -1 },
  { L0_META_CHARACTER_CLASS_MODIFIER,         "<L0 rule character class modifier compat>",    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_MODIFIER_COMPAT  }, { 0 },            -1,                                       -1, -1 },
  /*
    lhsi                                      descs                                        type                          nrhsl  { rhsi }                                       }  { mask }    minimumi                                separatori properb
  */
  { L0_META_CHARACTER_CLASS_MODIFIERS,        "<L0 rule character class modifiers>",          MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_CHARACTER_CLASS_MODIFIER             }, { 0 },             0,                                       -1,  0 },
  { L0_META_REGULAR_EXPRESSION,               "<L0 rule regular expression>",                 MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_REGULAR_EXPRESSION               }, { 0 },            -1,                                       -1, -1 },
  { L0_META_REGULAR_EXPRESSION_MODIFIERS,     "<L0 rule regular expression modifiers>",       MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIER      }, { 0 },             0,                                       -1,  0 },
  { L0_META_SUBSTITUTION_EXPRESSION,          "<L0 rule substitution expression>",            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION          }, { 0 },            -1,                                       -1, -1 },
  { L0_META_SUBSTITUTION_EXPRESSION_MODIFIERS,"<L0 rule substitution expression modifiers>",  MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_TERMINAL_SUBSTITUTION_EXPRESSION_MODIFIER }, { 0 },             0,                                       -1,  0 }
};

#endif /* MARPAESLIF_INTERNAL_ESLIF_L0_H */
