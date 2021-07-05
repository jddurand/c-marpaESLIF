#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

#define UTF_8_STRING "UTF-8"
const static char *yamls = "\n"
  "<_C_PRINTABLE>            ~ [\\x{9}] | [\\x{A}] | [\\x{D}] | [\\x{20}-\\x{7E}]          /* 8 bit */\n"
  "                          | [\\x{85}] | [\\x{A0}-\\x{D7FF}]:u | [\\x{E000}-\\x{FFFD}]:u /* 16 bit */\n"
  "                          | [\\x{10000}-\\x{10FFFF}]:u                                  /* 32 bit */\n"
  "\n"
  "<C_PRINTABLE>             ~ <_C_PRINTABLE>\n"
  "<NB_CHAR>                 ~ <_C_PRINTABLE>\n"
  "\n"
  "<c_printable>           ::= <C_PRINTABLE>\n"
  "<nb_json>               ::= [\\x{9}] | [\\x{20}-\\x{10FFFF}]:u\n"
  "<c_byte_order_mark>     ::= [\\x{FEFF}]:u\n"
  "<c_sequence_entry>      ::= “-”\n"
  "<c_mapping_key>         ::= “?”\n"
  "<c_mapping_value>       ::= “:”\n"
  "<c_collect_entry>       ::= “,”\n"
  "<c_sequence_start>      ::= “[”\n"
  "<c_sequence_end>        ::= “]”\n"
  "<c_mapping_start>       ::= “{”\n"
  "<c_mapping_end>         ::= “}”\n"
  "<c_comment>             ::= “#”\n"
  "<c_anchor>              ::= “&”\n"
  "<c_alias>               ::= “*”\n"
  "<c_tag>                 ::= “!”\n"
  "<c_literal>             ::= “|”\n"
  "<c_folded>              ::= “>”\n"
  "<c_single_quote>        ::= “'”\n"
  "<c_double_quote>        ::= “\"”\n"
  "<c_directive>           ::= “%”\n"
  "<c_reserved>            ::= “@” | “`”\n"
  "<c_indicator>           ::= “-” | “?” | “:” | “,” | “[” | “]” | “{” | “}”\n"
  "                          | “#” | “&” | “*” | “!” | “|” | “>” | “'” | “\"”\n"
  "                          | “%” | “@” | “`”\n"
  "<c_flow_indicator>      ::= “,” | “[” | “]” | “{” | “}”\n"
  "<b_line_feed>           ::= [\\x{A}]   /* LF */\n"
  "<b_carriage_return>     ::= [\\x{D}]   /* CR */\n"
  "<b_char>                ::= <b_line_feed> | <b_carriage_return>\n"
  "<nb_char>               ::= <NB_CHAR> - b-char - c-byte-order-mark\n"
  ;

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFOption_t          *marpaESLIFOptionp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammarOption_t   *marpaESLIFGrammarOptionp;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFGrammarDefaults_t  marpaESLIFGrammarDefaults;
  char                        *grammarscripts;
  marpaESLIFSymbol_t          *stringSymbolp = NULL;
  marpaESLIFSymbol_t          *stringSymbol2p = NULL;
  marpaESLIFSymbol_t          *regexSymbolp = NULL;
  marpaESLIFSymbol_t          *metaSymbolp = NULL;
  marpaESLIFString_t           string;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        matchb;
  marpaESLIFSymbolOption_t     marpaESLIFSymbolOption;
  size_t                       discardl;
  char                        *discardLasts;
  size_t                       discardLastl;
  short                        continueb;
  short                        exhaustedb;

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

  /* Compile YAML grammar */
  marpaESLIFGrammarOption.bytep               = (void *) yamls;
  marpaESLIFGrammarOption.bytel               = strlen(yamls);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
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
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "YAML grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarp, &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "--------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "YAML grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "--------------------\n%s", grammarscripts);

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
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
  size_t                      sendl;

  *inputsp              = (char *) yamls;
  *inputlp              = strlen(yamls);
  *characterStreambp    = 1;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;
  *eofbp                = 1;

  return 1;
}
