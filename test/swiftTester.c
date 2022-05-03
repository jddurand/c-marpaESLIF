#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

const static char *swifts = "\n"
  "#\n"
  "# Ref: https://docs.swift.org/swift-book/ReferenceManual/AboutTheLanguageReference.html\n"
  "#\n"
  ":start   ::= <start>\n"
  ":discard ::= <whitespace>\n"
  "\n"
  "# Grammar of whitespace\n"
  "# ---------------------\n"
  "<whitespace>                  ::= <whitespace item>+\n"
  "<whitespace item>             ::= <line break>\n"
  "                                | <inline space>\n"
  "                                | <comment>\n"
  "                                | <multiline comment>\n"
  "<whitespace item>             ::= [\\x{0000}\\x{000B}\\x{000C}]\n"
  "<line break>                  ::= /(?:\\x{000D}\\x{000A})|\\x{000A}|\\x{000D}/\n"
  "<inline spaces>               ::= <inline space>+\n"
  "<inline space>                ::= [\\x{0009}\\x{0020}]\n"
  "<comment>                     ::= '//' <comment text> <line break>\n"
  "<multiline comment>           ::= '/*' <multiline comment text> '*/'\n"
  "<comment text>                ::= <comment text item>\n"
  "<comment text item>           ::= [^\\x{000A}\\x{000D}]\n"
  "<multiline comment text>      ::= <multiline comment text item>+\n"
  "<multiline comment text item> ::= <multiline comment>\n"
  "<multiline comment text item> ::= <comment text item>\n"
  "<multiline comment text item> ::= '/' (?!'*')\n"
  "                                | '*' (?!'/')\n"
  "\n"
  "#\n"
  "# Top grammar start here\n"
  "# ----------------------\n"
  "<start> ::= <identifier>\n"
  "<start> ::= :eof\n"
  "\n"
  "# Grammar of identifier\n"
  "# ---------------------\n"
  "<identifier>                  ::= <identifier head> <identifier characters opt>\n"
  "                                | '`' <identifier head> <identifier characters opt> '`'\n"
  "                                | <implicit parameter name>\n"
  "                                | <property wrapper projection>\n"
  "<identifier list>             ::= <identifier>+ separator => ',' proper => 1\n"
  "<identifier head>             ::= [a-zA-Z]\n"
  "                                | '_'\n"
  "                                | [\\x{00A8}\\x{00AA}\\x{00AD}\\x{00AF}\\x{00B2}-\\x{00B5}\\x{00B7}-\\x{00BA}]\n"
  "                                | [\\x{00BC}-\\x{00BE}\\x{00C0}-\\x{00D6}\\x{00D8}-\\x{00F6}\\x{00F8}-\\x{00FF}]\n"
  "                                | [\\x{0100}-\\x{02FF}\\x{0370}-\\x{167F}\\x{1681}-\\x{180D}\\x{180F}-\\x{1DBF}]:u\n"
  "                                | [\\x{1E00}-\\x{1FFF}]:u\n"
  "                                | [\\x{200B}-\\x{200D}\\x{202A}-\\x{202E}\\x{203F}-\\x{2040}\\x{2054}\\x{2060}-\\x{206F}]:u\n"
  "                                | [\\x{2070}-\\x{20CF}\\x{2100}-\\x{218F}\\x{2460}-\\x{24FF}\\x{2776}-\\x{2793}]:u\n"
  "                                | [\\x{2C00}-\\x{2DFF}\\x{2E80}-\\x{2FFF}]:u\n"
  "                                | [\\x{3004}-\\x{3007}\\x{3021}-\\x{302F}\\x{3031}-\\x{303F}\\x{3040}-\\x{D7FF}]:u\n"
  "                                | [\\x{F900}-\\x{FD3D}\\x{FD40}-\\x{FDCF}\\x{FDF0}-\\x{FE1F}\\x{FE30}-\\x{FE44}]:u\n"
  "                                | [\\x{FE47}-\\x{FFFD}]:u\n"
  "                                | [\\x{10000}-\\x{1FFFD}\\x{20000}-\\x{2FFFD}\\x{30000}-\\x{3FFFD}\\x{40000}-\\x{4FFFD}]:u\n"
  "                                | [\\x{50000}-\\x{5FFFD}\\x{60000}-\\x{6FFFD}\\x{70000}-\\x{7FFFD}\\x{80000}-\\x{8FFFD}]:u\n"
  "                                | [\\x{90000}-\\x{9FFFD}\\x{A0000}-\\x{AFFFD}\\x{B0000}-\\x{BFFFD}\\x{C0000}-\\x{CFFFD}]:u\n"
  "                                | [\\x{D0000}-\\x{DFFFD}\\x{E0000}-\\x{EFFFD}]:u\n"
  "\n"
  "<identifier character>        ::= [0-9]\n"
  "                                | [\\x{0300}-\\x{036F}\\x{1DC0}-\\x{1DFF}\\x{20D0}-\\x{20FF}\\x{FE20}-\\x{FE2F}]:u\n"
  "<identifier character>        ::= <identifier head>\n"
  "<identifier characters>       ::= <identifier character> <identifier characters opt>\n"
  "<implicit parameter name>     ::= '$' <decimal digits>\n"
  "<property wrapper projection> ::= '$' <identifier characters>\n"
  "\n"
  "<identifier characters opt>   ::= <identifier characters>\n"
  "<identifier characters opt>   ::=\n"
  "\n"
  "# Grammar of an integer literal\n"
  "# -----------------------------\n"
  "<decimal digit> ::= [0-9]\n"
  "<decimal digits> ::= <decimal digit>+\n";

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short doparseb(genericLogger_t *genericLoggerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIFGrammar_t *marpaESLIFGrammarObjectp, char *inputs, int recursionleveli);

typedef struct swiftTester_context {
  char            *inputs;
  size_t           inputl;
} swiftTester_context_t;

typedef struct swiftTester_test {
  short  valueb;
  size_t forcedInputl;
  char  *names;
  char  *inputs;
} swiftTester_test_t;

static swiftTester_test_t swiftTester_testArrayp[] = {
  { 1, 0, "Whitespace - one blank",                      " "           },
  { 1, 0, "Whitespace - many blanks",                    "    "        },
  { 1, 1, "Whitespace - only \\x{0000} character",       "\0"          },
  { 0, 0, "Whitespace - // no text after",               "//"          },
  { 0, 0, "Whitespace - // with text but no line break", "// "         },
  { 1, 0, "Whitespace - // with text and line break",    "// \n"       },
  { 1, 0, "Whitespace - multiline comment",              "/* /* */ */" },
  { 1, 0, "Whitespace - nested multiline comment",       "/* /* */ */" },
  { 0, 0, "Whitespace - badly nested multiline comment", "/* */ */"    },
  { 0, 0, "Whitespace - Uncomplete discard",             "/* */ /*"    },
  { 0, 0, "Incomplete multi-line comment",               "/* */ /* x"  }
};

int main(int argc, char **argv) {
  marpaESLIF_t                *marpaESLIFp           = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp    = NULL;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t           *marpaESLIFValuep      = NULL;
  int                          exiti                 = 0;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFOption_t          *marpaESLIFOptionp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  char                        *grammarscripts;
  swiftTester_context_t        swiftTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  int                          i;
  short                        continueb;
  short                        exhaustedb;
  short                        valueb;
  swiftTester_test_t           swiftTester_test;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }
  marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
  if (marpaESLIFOptionp == NULL) {
    goto err;
  }

  /* Compile SWIFT grammar */
  marpaESLIFGrammarOption.bytep               = (void *) swifts;
  marpaESLIFGrammarOption.bytel               = strlen(swifts);
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = strlen("UTF-8");
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    goto err;
  }
  for (leveli = 0; leveli < ngrammari; leveli++) {
    if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
      GENERICLOGGER_INFO (genericLoggerp, "-------------------------");
      GENERICLOGGER_INFOF(genericLoggerp, "SWIFT grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarp, &grammarscripts)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (genericLoggerp, "--------------------");
  GENERICLOGGER_INFO (genericLoggerp, "SWIFT grammar script:");
  GENERICLOGGER_INFOF(genericLoggerp, "--------------------\n%s", grammarscripts);

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);

  if (argc == 2) {
    char ins[1024];
    size_t inl;
    FILE *fd;
    

    if (strcmp(argv[1], "-") == 0) {
      /* stdin */
      fd = stdin;
    } else {
      fd = fopen(argv[1], "r");
      if (fd == NULL) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        goto err;
      }
    }

    swiftTester_context.inputs = NULL;
    swiftTester_context.inputl = 0;
    while (1) {
      clearerr(fd);
      inl = fread(ins, 1, sizeof(ins), fd);
      if (inl > sizeof(ins)) {
        fprintf(stderr, "fread returned more bytes than expected\n");
        goto err;
      } else {
        if (ferror(fd)) {
          fprintf(stderr, "read error, %s\n", strerror(errno));
          goto err;
        }
        if (inl > 0) {
          if (swiftTester_context.inputs == NULL) {
            swiftTester_context.inputs = (char *) malloc(inl);
            if (swiftTester_context.inputs == NULL) {
              GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
              goto err;
            }
          } else {
            /* Tant pis for the memory leak if realloc failed */
            swiftTester_context.inputs = (char *) realloc(swiftTester_context.inputs, swiftTester_context.inputl + inl);
            if (swiftTester_context.inputs == NULL) {
              GENERICLOGGER_ERRORF(genericLoggerp, "realloc failure, %s", strerror(errno));
              goto err;
            }
          }
          memcpy(swiftTester_context.inputs + swiftTester_context.inputl, ins, inl);
          swiftTester_context.inputl += inl;
        }
      }
      if (feof(fd)) {
        break;
      }
    }

    swiftTester_test.valueb = 1; /* Assuming user want success */
    swiftTester_test.names  = argv[1];
    swiftTester_test.inputs = swiftTester_context.inputs;
    goto go;

  } else {
    for (i = 0; i < (sizeof(swiftTester_testArrayp)/sizeof(swiftTester_testArrayp[0])); i++) {
      swiftTester_test = swiftTester_testArrayp[i];

      swiftTester_context.inputs = swiftTester_test.inputs;
      swiftTester_context.inputl = swiftTester_test.inputs != NULL ? (swiftTester_test.forcedInputl > 0 ? swiftTester_test.forcedInputl : strlen(swiftTester_test.inputs)) : 0;

    go:
      marpaESLIFRecognizerOption.userDatavp               = &swiftTester_context;
      marpaESLIFRecognizerOption.readerCallbackp          = inputReaderb;
      marpaESLIFRecognizerOption.disableThresholdb        = 0;
      marpaESLIFRecognizerOption.exhaustedb               = 0;
      marpaESLIFRecognizerOption.newlineb                 = 1;
      marpaESLIFRecognizerOption.trackb                   = 0;
      marpaESLIFRecognizerOption.bufsizl                  = 0;
      marpaESLIFRecognizerOption.buftriggerperci          = 50;
      marpaESLIFRecognizerOption.bufaddperci              = 50;
      marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
      marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
      marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
      marpaESLIFRecognizerOption.generatorActionResolverp = NULL;

      if (marpaESLIFRecognizerp != NULL) {
        marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
      }
      marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
      if (marpaESLIFRecognizerp == NULL) {
        goto err;
      }
      if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
        valueb = 0;
        goto result;
      }
      while (continueb) {
        if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
          valueb = 0;
          goto result;
        }
      }

      marpaESLIFValueOption.userDatavp            = NULL; /* User specific context */
      marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
      marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
      marpaESLIFValueOption.importerp             = NULL;
      marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
      marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
      marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
      marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
      marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */

      if (marpaESLIFValuep != NULL) {
        marpaESLIFValue_freev(marpaESLIFValuep);
      }
      marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
      if (marpaESLIFValuep == NULL) {
        valueb = 0;
        goto result;
      }

      valueb = marpaESLIFValue_valueb(marpaESLIFValuep);

    result:
      if (swiftTester_test.valueb) {
        if (valueb) {
          GENERICLOGGER_NOTICEF(genericLoggerp, "OK: %s: Success when success expected", swiftTester_test.names);
        } else {
          GENERICLOGGER_NOTICEF(genericLoggerp, "KO: %s: Success when failure expected", swiftTester_test.names);
          exiti = 1;
        }
      } else {
        if (valueb) {
          GENERICLOGGER_NOTICEF(genericLoggerp, "KO: %s: Failure when success expected", swiftTester_test.names);
          exiti = 1;
        } else {
          GENERICLOGGER_NOTICEF(genericLoggerp, "OK: %s: Failure when failure expected", swiftTester_test.names);
        }
      }

      if (argc == 2) {
        /* Because of the go ;) */
        break;
      }
    }
  }

  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Exiting with status %d", exiti);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  swiftTester_context_t *swiftTester_contextp = (swiftTester_context_t *) userDatavp;

  *inputsp              = swiftTester_contextp->inputs;
  *inputlp              = swiftTester_contextp->inputl;
  *characterStreambp    = 1;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;
  *eofbp                = 1;

  return 1;
}
