#ifndef MARPAESLIF_INTERNAL_ESLIF_L0_H
#define MARPAESLIF_INTERNAL_ESLIF_L0_H

#include "marpaESLIF/internal/eslif/L0_join_G1.h"

/* Description of internal L0 grammar */

/* It is very important here to list all the terminals first, and in order compatible */
/* with bootstrap_grammar_L0_terminals[] and bootstrap_grammar_L0_rules[] */
typedef enum bootstrap_grammar_L0_enum {
  L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR,
  L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR,
  L0_TERMINAL_OP_LOOSEN,
  L0_TERMINAL_OP_EQUAL_PRIORITY,
  L0_TERMINAL_TRUE,
  L0_TERMINAL_FALSE,
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
  L0_TERMINAL_QUOTED_STRING,
  L0_TERMINAL_REGULAR_EXPRESSION,
  L0_TERMINAL_CHARACTER_CLASS_REGEXP,
  L0_TERMINAL_REGULAR_EXPRESSION_MODIFIERS,
  L0_TERMINAL_STRING_MODIFIERS,
  L0_TERMINAL_RESTRICTED_ASCII_GRAPH_CHARACTERS,
  L0_TERMINAL_LUA_ACTION_NAME,
  L0_TERMINAL_SEMICOLON,
  L0_TERMINAL_GRAPH_ASCII_CHARACTERS,
  /* ----- Non terminals ------ */
  L0_META_OP_DECLARE_ANY_GRAMMAR,
  L0_META_OP_DECLARE_TOP_GRAMMAR,
  L0_META_OP_DECLARE_LEX_GRAMMAR,
  L0_META_OP_LOOSEN,
  L0_META_OP_EQUAL_PRIORITY,
  L0_META_TRUE,
  L0_META_FALSE,
  L0_META_WORD_CHARACTER,
  L0_META_ONE_OR_MORE_WORD_CHARACTERS,
  L0_META_ZERO_OR_MORE_WORD_CHARACTERS,
  L0_META_RESTRICTED_ASCII_GRAPH_NAME,
  L0_META_LUA_ACTION_NAME,
  L0_META_BARE_NAME,
  L0_META_STANDARD_NAME,
  L0_META_BRACKETED_NAME,
  L0_META_BRACKETED_NAME_STRING,
  L0_META_QUOTED_STRING,
  L0_META_QUOTED_STRING_LITERAL,
  L0_META_CHARACTER_CLASS,
  L0_META_REGULAR_EXPRESSION,
  L0_META_GRAPH_ASCII_NAME
} bootstrap_grammar_L0_enum_t;

/* All non-terminals are listed here */
bootstrap_grammar_meta_t bootstrap_grammar_L0_metas[] = {
  /* Identifier                           Description                              Start  Discard :discard[on] :discard[off] lazyb lookupLevelDeltai verboseb */
  { L0_META_OP_DECLARE_ANY_GRAMMAR,       L0_JOIN_G1_META_OP_DECLARE_ANY_GRAMMAR,      0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,       L0_JOIN_G1_META_OP_DECLARE_TOP_GRAMMAR,      0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,       L0_JOIN_G1_META_OP_DECLARE_LEX_GRAMMAR,      0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_OP_LOOSEN,                    L0_JOIN_G1_META_OP_LOOSEN,                   0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_OP_EQUAL_PRIORITY,            L0_JOIN_G1_META_OP_EQUAL_PRIORITY,           0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_TRUE,                         L0_JOIN_G1_META_TRUE,                        0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_FALSE,                        L0_JOIN_G1_META_FALSE,                       0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_WORD_CHARACTER,               "word character",                            0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,  "one or more word characters",               0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS, "zero or more word characters",              0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_RESTRICTED_ASCII_GRAPH_NAME,  L0_JOIN_G1_META_RESTRICTED_ASCII_GRAPH_NAME, 0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_LUA_ACTION_NAME,              L0_JOIN_G1_META_LUA_ACTION_NAME,             0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_BARE_NAME,                    L0_JOIN_G1_META_BARE_NAME,                   0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_STANDARD_NAME,                L0_JOIN_G1_META_STANDARD_NAME,               0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_BRACKETED_NAME,               L0_JOIN_G1_META_BRACKETED_NAME,              0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_BRACKETED_NAME_STRING,        "bracketed name string",                     0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_QUOTED_STRING,                L0_JOIN_G1_META_QUOTED_STRING,               0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_QUOTED_STRING_LITERAL,        L0_JOIN_G1_META_QUOTED_STRING_LITERAL,       0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_CHARACTER_CLASS,              L0_JOIN_G1_META_CHARACTER_CLASS,             0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_REGULAR_EXPRESSION,           L0_JOIN_G1_META_REGULAR_EXPRESSION,          0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL },
  { L0_META_GRAPH_ASCII_NAME,             L0_JOIN_G1_META_GRAPH_ASCII_NAME,            0,       0,           0,            0,    0,               -1,       0,        NULL,       NULL }
};

/* Here it is very important that all the string constants are UTF-8 compatible - this is the case */

bootstrap_grammar_terminal_t bootstrap_grammar_L0_terminals[] = {
  /* From perl stringified version to C and // versions: */
  /*
#!env perl
use strict;
use diagnostics;
use Regexp::Common 'RE_ALL';

goto pass2;
pass1:
my $r = $RE{delimited}{-delim=>"'"}{-cdelim=>"'"}; # $RE{balanced}{-parens=>'[]'};
print "==> $r\n";
print "... copy/paste that in \$this variable and switch to pass2\n";
exit;

pass2:
my $this = do { local $/; <DATA> };
$this =~ s/\s*$//;
my $copy = $this;

$this =~ s/\\/\\\\/g;
$this =~ s/"/\\"/g;
print "FOR C: $this\n";

$copy =~ s/\//\\\//g;
print "FOR /: $copy\n";

__DATA__
(?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\')))
  */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /*                                                             TERMINALS                                                             */
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    ":\\[[\\d]+\\]:=", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    ":[0123]:=", ":[0"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'::='", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "::=", "::"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'~'", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "~", NULL
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_LOOSEN, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'||'", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "||", "|"
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_OP_EQUAL_PRIORITY, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'|'", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    NULL, NULL
#else
    "|", NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_TRUE, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'1'", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "1", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_FALSE, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'0'", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "0", ""
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_WORD_CHARACTER, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[\\w]", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LATIN_ALPHABET_LETTER, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[a-zA-Z]", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LEFT_CURLY, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'<'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RIGHT_CURLY, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'>'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_BRACKETED_NAME_STRING, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[\\s\\w]+", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_COMMA, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "','", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_START, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'start'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LENGTH, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'length'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1START, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'g1start'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_G1LENGTH, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'g1length'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_NAME, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'name'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LHS, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'lhs'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SYMBOL, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'symbol'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RULE, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'rule'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUE, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'value'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_VALUES, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "'values'", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>q{'"}}{-cdelim=>q{'"}} */
  /* Perl stringified version is: (?:(?|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\')|(?:\")(?:[^\\\"]*(?:\\.[^\\\"]*)*)(?:\"))) */
  { L0_TERMINAL_QUOTED_STRING, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, "su",
    "'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\x{201C}(?:[^\\\\\\x{201D}]*(?:\\\\.[^\\\\\\x{201D}]*)*)\\x{201D}", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "'A string'", "'"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::delimited, $RE{delimited}{-delim=>"/"}{-cdelim=>"/"} */
  /* Perl stringified version is: (?:(?|(?:\/)(?:[^\\\/]*(?:\\.[^\\\/]*)*)(?:\/))) */
  /* We add a protection against so that it does not conflict with C++ comments. */
  /* And it appears that is ok because a regexp starting with C comment have no sense, as well */
  /* as an empty regexp starting with // */
  { L0_TERMINAL_REGULAR_EXPRESSION, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, "su",
    "/(?![*/])(?:[^\\\\/]*(?:\\\\.[^\\\\/]*)*)/", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "/a(b)c/", "/a("
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  /* Taken from Regexp::Common::balanced, $RE{balanced}{-parens=>'[]'} */
  /* Perl stringified version is: (?^:((?:\[(?:(?>[^\[\]]+)|(?-1))*\]))) */
  /* Perl stringified version is revisited without the (?^:XXX): ((?:\[(?:(?>[^\[\]]+)|(?-1))*\])) */
  { L0_TERMINAL_CHARACTER_CLASS_REGEXP, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))", NULL, NULL,
#ifndef MARPAESLIF_NTRACE
    "[[:alnum]]","[a-z"
#else
    NULL, NULL
#endif
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_REGULAR_EXPRESSION_MODIFIERS, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[eijmnsxDJUuaNbcA]+", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_STRING_MODIFIERS, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "ic?", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_RESTRICTED_ASCII_GRAPH_CHARACTERS, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[!#$%&*+./;?\\[\\\\\\]^_`~A-Za-z0-9]+", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_LUA_ACTION_NAME, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "::lua->[a-zA-Z_][a-zA-Z0-9_]*", NULL, NULL,
    NULL, NULL
  },
  /* --------------------------------------------------------------------------------------------------------------------------------- */
  { L0_TERMINAL_SEMICOLON, MARPAESLIF_TERMINAL_TYPE_STRING, 0 /* charclassb */, NULL,
    "':'", NULL, NULL,
    NULL, NULL
  },
  { L0_TERMINAL_GRAPH_ASCII_CHARACTERS, MARPAESLIF_TERMINAL_TYPE_REGEX, 0 /* charclassb */, NULL,
    "[[:graph:]]+", NULL, NULL,
    NULL, NULL
  },
};

bootstrap_grammar_rule_t bootstrap_grammar_L0_rules[] = {
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_OP_DECLARE_ANY_GRAMMAR,           "op declare any grammar",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_ANY_GRAMMAR           }, -1,                        -1,      -1,             0, NULL },
  { L0_META_OP_DECLARE_TOP_GRAMMAR,           "op declare top grammar",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_TOP_GRAMMAR           }, -1,                        -1,      -1,             0, NULL },
  { L0_META_OP_DECLARE_LEX_GRAMMAR,           "op declare lex grammar",                       MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_DECLARE_LEX_GRAMMAR           }, -1,                        -1,      -1,             0, NULL },
  { L0_META_OP_LOOSEN,                        "op loosen",                                    MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_LOOSEN                        }, -1,                        -1,      -1,             0, NULL },
  { L0_META_OP_EQUAL_PRIORITY,                "op equal priority",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_OP_EQUAL_PRIORITY                }, -1,                        -1,      -1,             0, NULL },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_TRUE,                             "true",                                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_TRUE                             }, -1,                        -1,      -1,             0, NULL },
  { L0_META_FALSE,                            "false",                                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_FALSE                            }, -1,                        -1,      -1,             0, NULL },
  { L0_META_WORD_CHARACTER     ,              "word character",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_WORD_CHARACTER                   }, -1,                        -1,      -1,             0, NULL },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_ONE_OR_MORE_WORD_CHARACTERS,      "one more word characters",                     MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },  1,                        -1,      -1,             0, NULL },
  { L0_META_ZERO_OR_MORE_WORD_CHARACTERS,     "zero more word characters",                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },  0,                        -1,      -1,             0, NULL },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_RESTRICTED_ASCII_GRAPH_NAME,      "restricted ascii graph name",                  MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_RESTRICTED_ASCII_GRAPH_CHARACTERS}, -1,                        -1,      -1,             0, NULL },
  { L0_META_LUA_ACTION_NAME,                  "lua action name",                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_LUA_ACTION_NAME                  }, -1,                        -1,      -1,             0, NULL },
  { L0_META_BARE_NAME,                        "bare name",                                    MARPAESLIF_RULE_TYPE_SEQUENCE,    1, { L0_META_WORD_CHARACTER                       },  1,                        -1,      -1,             0, NULL },
  { L0_META_STANDARD_NAME,                    "standard name",                                MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_LATIN_ALPHABET_LETTER,
                                                                                                                                     L0_META_ZERO_OR_MORE_WORD_CHARACTERS         }, -1,                        -1,      -1,             0, NULL },
  { L0_META_BRACKETED_NAME,                   "bracketed name",                               MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_LEFT_CURLY,
                                                                                                                                     L0_META_BRACKETED_NAME_STRING,
                                                                                                                                     L0_TERMINAL_RIGHT_CURLY                      }, -1,                        -1,      -1,             0, NULL },
  { L0_META_BRACKETED_NAME_STRING,            "bracketed name string",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_BRACKETED_NAME_STRING            }, -1,                        -1,      -1,             0, NULL },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_QUOTED_STRING,                    "quoted string 1",                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_QUOTED_STRING                    }, -1,                        -1,      -1,             0, NULL },
  { L0_META_QUOTED_STRING,                    "quoted string 2",                              MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_QUOTED_STRING,
                                                                                                                                     L0_TERMINAL_SEMICOLON,
                                                                                                                                     L0_TERMINAL_STRING_MODIFIERS                 }, -1,                        -1,      -1,             0, NULL },
  { L0_META_QUOTED_STRING_LITERAL,            "quoted string literal",                        MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_QUOTED_STRING                    }, -1,                        -1,      -1,             0, NULL },
  { L0_META_CHARACTER_CLASS,                  "character class 1",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_CHARACTER_CLASS_REGEXP           }, -1,                        -1,      -1,             0, NULL },
  { L0_META_CHARACTER_CLASS,                  "character class 2",                            MARPAESLIF_RULE_TYPE_ALTERNATIVE, 3, { L0_TERMINAL_CHARACTER_CLASS_REGEXP,
                                                                                                                                     L0_TERMINAL_SEMICOLON,
                                                                                                                                     L0_TERMINAL_REGULAR_EXPRESSION_MODIFIERS     }, -1,                        -1,      -1,             0, NULL },
  /*
    lhsi                                      descs                                           type                          nrhsl  { rhsi }                                       }  minimumi           separatori  properb hideseparatorb actions
  */
  { L0_META_REGULAR_EXPRESSION,               "regular expression 1",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_REGULAR_EXPRESSION               }, -1,                        -1,      -1,             0, NULL },
  { L0_META_REGULAR_EXPRESSION,               "regular expression 2",                         MARPAESLIF_RULE_TYPE_ALTERNATIVE, 2, { L0_TERMINAL_REGULAR_EXPRESSION,
                                                                                                                                     L0_TERMINAL_REGULAR_EXPRESSION_MODIFIERS     }, -1,                        -1,      -1,             0, NULL },
  { L0_META_GRAPH_ASCII_NAME,                 "graph ascii name",                             MARPAESLIF_RULE_TYPE_ALTERNATIVE, 1, { L0_TERMINAL_GRAPH_ASCII_CHARACTERS           }, -1,                        -1,      -1,             0, NULL }
};

/* No L0 lazy rule
bootstrap_grammar_rule_t bootstrap_grammar_L0_lazy_rules[] = {
};
*/

#endif /* MARPAESLIF_INTERNAL_ESLIF_L0_H */
