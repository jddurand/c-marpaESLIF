#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

#define UTF_8_STRING "UTF-8"
#define ASCII_STRING "ASCII"

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *selfs = "\n"
  "  /*\n"
  "   * **********************\n"
  "   * Meta-grammar settings:\n"
  "   * **********************\n"
  "   */\n"
  "  :discard                       ::= whitespace\n"
  "  :discard                       ::= <perl comment> \n"
  "  :discard                       ::= <cplusplus comment> \n"
  "\n"
  "  /*\n"
  "   * ***************\n"
  "   * Event settings:\n"
  "   * ***************\n"
  "   */\n"
  "  event :discard[off]=on = nulled <discard off>\n"
  "  event :discard[on]=on = nulled <discard on>\n"
  "\n"
  "  /*\n"
  "   * ******\n"
  "   * Rules:\n"
  "   * ******\n"
  "   */\n"
  "  <statements>                   ::= statement*\n"
  "\n"
  "  <statement>                    ::= <start rule>\n"
  "                                   | <desc rule>\n"
  "                                   | <empty rule>\n"
  "                                   | <null statement>\n"
  "                                   | <statement group>\n"
  "                                   | <priority rule>\n"
  "                                   | <quantified rule>\n"
  "                                   | <discard rule>\n"
  "                                   | <default rule>\n"
  "                                   | <lexeme rule>\n"
  "                                   | <completion event declaration>\n"
  "                                   | <nulled event declaration>\n"
  "                                   | <prediction event declaration>\n"
  "                                   | <inaccessible statement>\n"
  "                                   | <exception statement>\n"
  "                                   | <autorank statement>\n"
  "\n"
  "  <start rule>                   ::= ':start' <op declare> symbol\n"
  "  <desc rule>                    ::= ':desc' <op declare> <quoted name>\n"
  "  <empty rule>                   ::= <lhs> <op declare> <adverb list>\n"
  "  <null statement>               ::= ';'\n"
  "  <statement group>              ::= '{' statements '}'\n"
  "  <priority rule>                ::= lhs <op declare> <priorities>\n"
  "  <quantified rule>              ::= lhs <op declare> <rhs primary> <quantifier> <adverb list>\n"
  "  <discard rule>                 ::= ':discard' <op declare> <rhs primary> <adverb list>\n"
  "  <default rule>                 ::= ':default' <op declare> <adverb list>\n"
  "  <lexeme rule>                  ::= ':lexeme' <op declare> symbol <adverb list>\n"
  "  <completion event declaration> ::= 'event' <event initialization> '=' 'completed' <symbol name>\n"
  "                                   | 'event' <event initialization> <op declare> 'completed' <symbol name>\n"
  "  <nulled event declaration>     ::= 'event' <event initialization> '=' 'nulled' <symbol name>\n"
  "                                   | 'event' <event initialization> <op declare> 'nulled' <symbol name>\n"
  "  <prediction event declaration> ::= 'event' <event initialization> '=' 'predicted' <symbol name>\n"
  "                                   | 'event' <event initialization> <op declare> 'predicted' <symbol name>\n"
  "  <inaccessible statement>       ::= 'inaccessible' 'is' <inaccessible treatment> 'by' 'default'\n"
  "  <inaccessible treatment>       ::= 'warn'\n"
  "  <inaccessible treatment>       ::= 'ok'\n"
  "  <inaccessible treatment>       ::= 'fatal'\n"
  "  <exception statement>          ::= lhs <op declare> <rhs primary> '-' <rhs primary> <adverb list>\n"
  "  <autorank statement>           ::= 'autorank' 'is' <on or off> 'by' 'default'\n"
  "  <op declare>                   ::= <op declare top grammar>\n"
  "                                   | <op declare lex grammar>\n"
  "                                   | <op declare any grammar>\n"
  "  <priorities>                   ::= <alternatives>+ separator => <op loosen> proper => 1\n"
  "  <alternatives>                 ::= <alternative>+ separator => <op equal priority> proper => 1\n"
  "  <alternative>                  ::= rhs <adverb list>\n"
  "  <adverb list>                  ::= <adverb list items>\n"
  "  <adverb list items>            ::= <adverb item>*\n"
  "  <adverb item>                  ::= <action>\n"
  "                                   | <left association>\n"
  "                                   | <right association>\n"
  "                                   | <group association>\n"
  "                                   | <separator specification>\n"
  "                                   | <proper specification>\n"
  "                                   | <rank specification>\n"
  "                                   | <null ranking specification>\n"
  "                                   | <priority specification>\n"
  "                                   | <pause specification>\n"
  "                                   | <latm specification>\n"
  "                                   | naming\n"
  "                                   | <null adverb>\n"
  "                                   | <symbol action>\n"
  "                                   | <free action>\n"
  "                                   | <event specification>\n"
  "                                   | <hide separator specification>\n"
  "  <action>                       ::= 'action' '=>' <action name>\n"
  "  <action>                       ::= 'action' '=>' <string literal>\n"
  "  <left association>             ::= 'assoc' '=>' 'left'\n"
  "  <right association>            ::= 'assoc' '=>' 'right'\n"
  "  <group association>            ::= 'assoc' '=>' 'group'\n"
  "  <separator specification>      ::= 'separator' '=>' <single symbol>\n"
  "  <proper specification>         ::= 'proper' '=>' false\n"
  "                                   | 'proper' '=>' true\n"
  "  <hide separator specification> ::= 'hide-separator' '=>' false\n"
  "                                   | 'hide-separator' '=>' true\n"
  "  <rank specification>           ::= 'rank' '=>' <signed integer>\n"
  "  <null ranking specification>   ::= 'null-ranking' '=>' <null ranking constant>\n"
  "                                   | 'null' 'rank' '=>' <null ranking constant>\n"
  "  <null ranking constant>        ::= 'low'\n"
  "                                   | 'high'\n"
  "  <priority specification>       ::= 'priority' '=>' <signed integer>\n"
  "  <pause specification>          ::= 'pause' '=>' 'before'\n"
  "                                   | 'pause' '=>' 'after'\n"
  "  <event specification>          ::= 'event' '=>' <event initialization>\n"
  "  <event initialization>         ::= <event name> <event initializer>\n"
  "  <event initializer>            ::= '=' <on or off>\n"
  "  <event initializer>            ::=\n"
  "  <on or off>                    ::= 'on'\n"
  "                                   | 'off'\n"
  "  <latm specification>           ::= 'latm' '=>' <false>\n"
  "                                   | 'latm' '=>' <true>\n"
  "  naming                         ::= 'name' '=>' <alternative name>\n"
  "  <null adverb>                  ::= ','\n"
  "  <symbol action>                ::= 'symbol-action' '=>' <symbol action name>\n"
  "  <symbol action>                ::= 'symbol-action' '=>' <string literal>\n"
  "  <free action>                  ::= 'free-action' '=>' <free name>\n"
  "  <alternative name>             ::= <standard name>\n"
  "  <alternative name>             ::= <quoted name>\n"
  "  <event name>                   ::= <restricted ascii graph name>\n"
  "                                   | ':symbol'\n"
  "                                   | ':discard[on]'\n"
  "                                   | ':discard[off]'\n"
  "  lhs                            ::= <symbol name>\n"
  "  rhs                            ::= <rhs primary>+\n"
  "  <rhs primary>                  ::= <single symbol>\n"
  "                                   | <symbol name> '@' <grammar reference>\n"
  "  <single symbol>                ::= symbol\n"
  "                                   | <character class>\n"
  "                                   | <regular expression>\n"
  "                                   | <quoted string>\n"
  "  symbol                         ::= <symbol name>\n"
  "  <symbol name>                  ::= <bare name>\n"
  "                                   | <bracketed name>\n"
  "  <action name>                  ::= <restricted ascii graph name>\n"
  "                                   | '::shift'\n"
  "                                   | '::undef'\n"
  "                                   | '::ascii'\n"
  "                                   | /::convert\\[[^\\]]+\\]/\n"
  "                                   | '::concat'\n"
  "                                   | /::copy\\[\\d+\\]/\n"
  "  <free name>                    ::= <restricted ascii graph name>\n"
  "  <symbol action name>           ::= <restricted ascii graph name>\n"
  "                                   | '::transfer'\n"
  "                                   | '::undef'\n"
  "                                   | '::ascii'\n"
  "                                   | /::convert\\[[^\\]]+\\]/\n"
  "                                   | '::concat'\n"
  "  <quantifier>                   ::= '*'\n"
  "                                   | '+'\n"
  "  <signed integer>               ::= /[+-]?\\d+/\n"
  "  <unsigned integer>             ::= /\\d+/\n"
  "  <grammar reference>            ::= <quoted string>\n"
  "                                   | <signed integer>\n"
  "                                   | '=' <unsigned integer>\n"
  "<string literal>                 ::= <string literal unit>+\n"
  "<string literal unit>            ::= '::u8\"' <discard off> <string literal inside any> '\"' <discard on>\n"
  "<discard off>                    ::=\n"
  "<discard on>                     ::=\n"
  "<string literal inside any>      ::= <string literal inside>*\n"
  "<string literal inside>          ::= /[^\"\\\\\\n]/\n"
  "                                   | '\\\\' /[\"'?\\\\abfnrtve]/\n"
  "                                   | '\\\\' /x\\{[a-fA-F0-9]{2}\\}/\n"
  "                                   | '\\\\' /u\\{[a-fA-F0-9]{4}\\}/\n"
  "                                   | '\\\\' /U\\{[a-fA-F0-9]{8}\\}/\n"
  "  <jdd> ::= <op declare any grammar>@=1\n"
  "\n"
  "  #\n"
  "  # The following is giving information on grammar components: resolved lexemes, rules and symbols properties\n"
  "  #\n"
  "  # -----------------\n"
  "  # Resolved lexemes:\n"
  "  # -----------------\n"
  "  #\n"
  "  # <op declare any grammar> ::= <op declare any grammar>@+1\n"
  "  # <op declare top grammar> ::= <op declare top grammar>@+1\n"
  "  # <op declare lex grammar> ::= <op declare lex grammar>@+1\n"
  "  # <op loosen> ::= <op loosen>@+1\n"
  "  # <op equal priority> ::= <op equal priority>@+1\n"
  "  # <false> ::= <false>@+1\n"
  "  # <true> ::= <true>@+1\n"
  "  # <standard name> ::= <standard name>@+1\n"
  "  # <quoted name> ::= <quoted name>@+1\n"
  "  # <quoted string> ::= <quoted string>@+1\n"
  "  # <character class> ::= <character class>@+1\n"
  "  # <regular expression> ::= <regular expression>@+1\n"
  "  # <bare name> ::= <bare name>@+1\n"
  "  # <bracketed name> ::= <bracketed name>@+1\n"
  "  # <restricted ascii graph name> ::= <restricted ascii graph name>@+1\n"
  "  # <whitespace> ::= <whitespace>@+1\n"
  "  # <perl comment> ::= <perl comment>@+1\n"
  "  # <cplusplus comment> ::= <cplusplus comment>@+1\n"
  "  #\n"
  "\n"
  "  /*\n"
  "   * *************\n"
  "   * Lexeme rules:\n"
  "   * *************\n"
  "   */\n"
  "  whitespace                       ~ /[\\s]+/\n"
  "  <perl comment>                   ~ /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u\n"
  "  <cplusplus comment>              ~ /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u\n"
  "  <op declare any grammar>         ~ /:\\[(\\d+)\\]:=/\n"
  "  <op declare top grammar>         ~ '::='\n"
  "  <op declare lex grammar>         ~ '~'\n"
  "  <op loosen>                      ~ '||'\n"
  "  <op equal priority>              ~ '|'\n"
  "  <true>                           ~ '1'\n"
  "  <false>                          ~ '0'\n"
  "  <word character>                 ~ [\\w]\n"
  "  <one or more word characters>    ~ <word character>+ proper => 1\n"
  "  <zero or more word characters>   ~ <word character>* proper => 1\n"
  "  <restricted ascii graph name>    ~ /[-!#$%&()*+.\\/;<>?@\\[\\\\\\]^_`|~A-Za-z0-9][-!#$%&()*+.\\/:;<>?@\\[\\\\\\]^_`|~A-Za-z0-9]*/\n"
  "  <bare name>                      ~ <word character>+ proper => 1\n"
  "  <standard name>                  ~ [a-zA-Z] <zero or more word characters>\n"
  "  <bracketed name>                 ~ '<' <bracketed name string> '>'\n"
  "  <bracketed name string>          ~ /[\\s\\w]+/\n"
  "  <quoted string>                  ~ /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")))/su\n"
  "                                   | /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")))/su ':' /ic?/\n"
  "  <quoted name>                    ~ /(?:(?|(?:')(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)(?:')|(?:\")(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)(?:\")))/su\n"
  "  <character class>                ~ /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/\n"
  "                                   | /((?:\\[(?:(?>[^\\[\\]]+)|(?-1))*\\]))/ ':' /[eijmnsxDJUuaNbcA]+/\n"
  "  <regular expression>             ~ /(?:(?|(?:\\/)(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)(?:\\/)))/su\n"
  "                                   | /(?:(?|(?:\\/)(?:[^\\\\\\/]*(?:\\\\.[^\\\\\\/]*)*)(?:\\/)))/su /[eijmnsxDJUuaNbcA]+/\n"
  "#\n"
  "# The following are unused but just added as examples...\n"
  "#\n"
  " <STR1>                            ~ 'Example':i\n"
  " <STR2>                            ~ '{Example}'\n"
  " <STR3>                            ~ \"{Example}\"\n"
  " <STR5>                            ~ '{Example[]\\\\}'\n"
  " <STR6>                            ~ 'Black Heart Suite Character: ♥'\n"
  " <STR7>                            ~ 'Black Heart Suite Character: ♥':i\n"
  " <CLS1>                            ~ [a-z]\n"
  " <CLS2>                            ~ [a-z♥]\n"
  " <CLS3>                            ~ [a-z\\x{2665}]:u\n"
  " <REG1>                            ~ /[a-z]/u\n"
  " <REG2>                            ~ /[a-z]+/u\n"
  " <REG3>                            ~ /(*LIMIT_MATCH=15)[a-z]+/\n"
  "\n";

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
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFGrammarDefaults_t  marpaESLIFGrammarDefaults;
  marpaESLIFAction_t           defaultFreeAction;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }
  marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
  if (marpaESLIFOptionp == NULL) {
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF was generated with these options:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... genericLoggerp: %p", marpaESLIFOptionp->genericLoggerp);
  if (marpaESLIFOptionp->genericLoggerp != marpaESLIFOption.genericLoggerp) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "... genericLoggerp != %p", marpaESLIFOption.genericLoggerp);
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------\n");

  /* Inspect options used to create ESLIF's grammar */
  marpaESLIFGrammarOptionp = marpaESLIFGrammar_optionp(marpaESLIF_grammarp(marpaESLIFp));
  if (marpaESLIFGrammarOptionp == NULL) {
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "ESLIF's grammar was generated with these options:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... bytep    : %p", marpaESLIFGrammarOptionp->bytep);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... bytel    : %ld", (unsigned long) marpaESLIFGrammarOptionp->bytel);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... encodings: %p", marpaESLIFGrammarOptionp->encodings);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... encodingl: %ld", (unsigned long) marpaESLIFGrammarOptionp->encodingl);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... encodings: %p", marpaESLIFGrammarOptionp->encodingOfEncodings);
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------\n");

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIF_grammarp(marpaESLIFp), &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIF_grammarp(marpaESLIFp), &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
      }
    }
  }

  marpaESLIFGrammarOption.bytep               = (void *) selfs;
  marpaESLIFGrammarOption.bytel               = strlen(selfs);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
  marpaESLIFGrammarOption.encodingOfEncodings = ASCII_STRING;
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
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* So in theory we must be able to reparse ESLIF using this generated grammar -; */
  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = (char *) selfs;
  marpaESLIFTester_context.inputl         = strlen(selfs);

  marpaESLIFRecognizerOption.userDatavp                   = &marpaESLIFTester_context; /* User specific context */
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp    = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb            = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb                   = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb                     = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb                       = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl                      = 0; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci              = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci                  = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
    goto err;
  }

  /* Play with defaults */
  for (leveli = 0; leveli < ngrammari; leveli++) {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Getting defaults of grammar at level %d", leveli);
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, leveli, NULL /* descp */)) {
      goto err;
    }
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Overwriting defaultFreeActions of grammar at level %d", leveli);
    defaultFreeAction.type    = MARPAESLIF_ACTION_TYPE_NAME;
    defaultFreeAction.u.names = ":defaultFreeActions";
    marpaESLIFGrammarDefaults.defaultFreeActionp = &defaultFreeAction;
    if (! marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, leveli, NULL /* descp */)) {
      goto err;
    }
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Checking overwrite of defaultFreeActions for grammar at level %d", leveli);
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, leveli, NULL /* descp */)) {
      goto err;
    }
    if (marpaESLIFGrammarDefaults.defaultFreeActionp->type != MARPAESLIF_ACTION_TYPE_NAME) {
      GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "Wrong defaultFreeActions type: %d instead of %d", marpaESLIFGrammarDefaults.defaultFreeActionp->type, MARPAESLIF_ACTION_TYPE_NAME);
      goto err;
    }
    if (strncmp(marpaESLIFGrammarDefaults.defaultFreeActionp->u.names, ":defaultFreeActions", strlen(":defaultFreeActions")) != 0) {
      GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "Wrong defaultFreeActions: %s instead of %s", marpaESLIFGrammarDefaults.defaultFreeActionp->u.names, ":defaultFreeActions");
      goto err;
    }
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);
  return exiti;
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}
