#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *dsl = "\n"
  "/*\n"
  " * Example of a calulator with ESLIF BNF:\n"
  " *\n"
  " * Automatic discard of whitespaces\n"
  " * Correct association for expressions\n"
  " * Embedded action using anonymous lua functions\n"
  " *\n"
  "*/\n"
  ":discard ::= /[\\s]+/\n"
  ":default ::= event-action => ::luac->function()\n"
  "                                       print('In event-action')\n"
  "                                       error('JDD')\n"
  "                                     end\n"
  "             symbol-action => ::luac->function(input)\n"
  "                                        print('In symbol-action for: '..tostring(input))\n"
  "                                        return input\n"
  "                                      end\n"
  "event ^exp = predicted exp\n"
  "exp ::=\n"
  "    /[\\d]+/                            action => ::luac->function(exp)\n"
  "                                                            print('In action for: '..tostring(exp))\n"
  "                                                            return tonumber(exp)\n"
  "                                                          end\n"
  "    |    '('  exp ')'    assoc => group action => ::luac->function(lparen, exp, rparen)\n"
  "                                                            print('In action for: '..lparen..tostring(exp)..rparen)\n"
  "                                                            return exp\n"
  "                                                          end\n"
  "   || exp (- '**' -) exp assoc => right action => ::lua->function(x,y)\n"
  "                                                            print('In action for: '..tostring(x)..' ** '..tostring(y))\n"
  "                                                           return x^y\n"
  "                                                         end\n"
  "   || exp (-  '*' -) exp                action => ::luac->function(x,y)\n"
  "                                                            print('In action for: '..tostring(x)..' * '..tostring(y))\n"
  "                                                           return x*y\n"
  "                                                         end\n"
  "    | exp (-  '/' -) exp                action => ::lua->function(x,y)\n"
  "                                                            print('In action for: '..tostring(x)..' / '..tostring(y))\n"
  "                                                           return x/y\n"
  "                                                         end\n"
  "   || exp (-  '+' -) exp                action => ::luac->function(x,y)\n"
  "                                                            print('In action for: '..tostring(x)..' + '..tostring(y))\n"
  "                                                           return x+y\n"
  "                                                         end\n"
  "    | exp (-  '-' -) exp                action => ::lua->function(x,y)\n"
  "                                                            print('In action for: '..tostring(x)..' - '..tostring(y))\n"
  "                                                           return x-y\n"
  "                                                         end\n"
  ;


int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  int                          i;
  marpaESLIFValueOption_t      marpaESLIFValueOption;

  const static char           *inputs[] = {
    "(((3 * 4) + 2 * 7) / 2 - 1) ** 3",
    "5 / (2 * 3)",
    "5 / 2 * 3",
    "(5 ** 2) ** 3",
    "5 * (2 * 3)",
    "5 ** (2 ** 3)",
    "5 ** (2 / 3)",
    "1 + ( 2 + ( 3 + ( 4 + 5) )",
    "1 + ( 2 + ( 3 + ( 4 + 50) ) )",
    " 100"
  };

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) dsl;
  marpaESLIFGrammarOption.bytel               = strlen(dsl);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  for (i = 0; i < (sizeof(inputs)/sizeof(inputs[0])); i++) {
    marpaESLIFTester_context.genericLoggerp = genericLoggerp;
    marpaESLIFTester_context.inputs         = (char *) inputs[i];
    marpaESLIFTester_context.inputl         = (inputs[i] != NULL) ? strlen(inputs[i]) : 0;

    if (marpaESLIFTester_context.inputs == NULL) {
      break;
    }

    marpaESLIFRecognizerOption.userDatavp               = &marpaESLIFTester_context;
    marpaESLIFRecognizerOption.readerCallbackp          = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb        = 0;
    marpaESLIFRecognizerOption.exhaustedb               = 0;
    marpaESLIFRecognizerOption.newlineb                 = 1;
    marpaESLIFRecognizerOption.trackb                   = 1;
    marpaESLIFRecognizerOption.bufsizl                  = 0;
    marpaESLIFRecognizerOption.buftriggerperci          = 50;
    marpaESLIFRecognizerOption.bufaddperci              = 50;
    marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
    marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
    marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
    marpaESLIFRecognizerOption.generatorActionResolverp = NULL;

    marpaESLIFValueOption.userDatavp            = genericLoggerp; /* User specific context */
    marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
    marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
    marpaESLIFValueOption.importerp             = importb;
    marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
    marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
    marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
    marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
    marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */

    GENERICLOGGER_INFO(genericLoggerp, "Parsing Expression");
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    GENERICLOGGER_INFOF(genericLoggerp, "%s", inputs[i]);
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    /* Scan the input */
    /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
    marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL /* isExhaustedbp */);
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);

  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  return 1;
}

/*****************************************************************************/
static short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  genericLogger_t *genericLoggerp = (genericLogger_t *) userDatavp;

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    GENERICLOGGER_NOTICE(genericLoggerp, "Result type is undef");
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is char: %c", marpaESLIFValueResultp->u.c);
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is short: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is int: %d", marpaESLIFValueResultp->u.i);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is long: %ld", marpaESLIFValueResultp->u.l);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is float: %f", (double) marpaESLIFValueResultp->u.f);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is double: %f", marpaESLIFValueResultp->u.d);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is ptr: %p", marpaESLIFValueResultp->u.p.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is array: {%p,%ld}", marpaESLIFValueResultp->u.a.p, (unsigned long) marpaESLIFValueResultp->u.a.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is bool: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is string: %s, encoding: %s", marpaESLIFValueResultp->u.s.p, marpaESLIFValueResultp->u.s.encodingasciis);
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is row: {%p,%ld}", marpaESLIFValueResultp->u.r.p, (unsigned long) marpaESLIFValueResultp->u.r.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is table: {%p,%ld}", marpaESLIFValueResultp->u.t.p, (unsigned long) marpaESLIFValueResultp->u.t.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG_DOUBLE:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is long double: %Lf", marpaESLIFValueResultp->u.ld);
    break;
#ifdef MARPAESLIF_HAVE_LONG_LONG
  case MARPAESLIF_VALUE_TYPE_LONG_LONG:
    GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is long long double: " MARPAESLIF_LONG_LONG_FMT, marpaESLIFValueResultp->u.ll);
    break;
#endif
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Unsupported result type %d", marpaESLIFValueResultp->type);
    break;
  }

  return 1;
}
