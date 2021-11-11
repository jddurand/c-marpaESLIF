#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static const char *ASCIIs = "ASCII";

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short rhsb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
static marpaESLIFRecognizerGeneratorCallback_t generatorActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static void  stringFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

typedef struct marpaESLIF_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIF_context_t;

const static char *grammars = "# Parameterized grammar\n"
  ":discard ::= /[\\s]+/\n"
  "\n"
  "top  ::= rhs1\n"
  "rhs1 ::= . => rhs-->(1, nil, String('Input should be \"1\"'))\n"
  "       | . => rhs-->(2, nil, String('Input should be \"2\"'))\n"
  "       | . => rhs-->(3, nil, String('Input should be \"3\"'))\n"
  "       | . => rhs-->(4, nil, String('Input should be \"4\"'))\n"
  "\n"
  "<luascript>\n"
  "function String(p_string, p_encoding)\n"
  "  local output = p_string\n"
  "  p_string:encoding(p_encoding or 'ASCII')\n"
  "  return output\n"
  "end\n"
  "</luascript>\n"
  ;

const static char *inputs = "5";

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
  marpaESLIF_context_t         marpaESLIF_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  char                        *grammarscripts;
  marpaESLIFSymbol_t          *stringSymbolp = NULL;
  marpaESLIFSymbol_t          *stringSymbol2p = NULL;
  marpaESLIFSymbol_t          *regexSymbolp = NULL;
  marpaESLIFString_t           string;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        continueb;
  short                        exhaustedb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
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
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "----------------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Parameterized grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "----------------------------------\n%s", grammarshows);
    }
  }

  marpaESLIF_context.genericLoggerp = genericLoggerp;
  marpaESLIF_context.inputs         = (char *) inputs;
  marpaESLIF_context.inputl         = strlen(inputs);

  marpaESLIFRecognizerOption.userDatavp        = &marpaESLIF_context; /* User specific context */
  marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb        = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb          = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb            = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl           = 10; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
  marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
  marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
  marpaESLIFRecognizerOption.generatorActionResolverp = generatorActionResolverp;
  marpaESLIFRecognizerOption.importerp                = NULL;

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);
  GENERICLOGGER_NOTICE(genericLoggerp, "Testing interactive recognizer");
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  GENERICLOGGER_INFOF(genericLoggerp, "After scan: continueb=%d, exhaustedb=%d", (int) continueb, (int) exhaustedb);
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
      goto err;
    }
    GENERICLOGGER_INFOF(genericLoggerp, "After resume: continueb=%d, exhaustedb=%d", (int) continueb, (int) exhaustedb);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

  GENERICLOGGER_NOTICE(genericLoggerp, "Testing parse");
  marpaESLIF_context.inputs         = (char *) inputs;
  marpaESLIF_context.inputl         = strlen(inputs);
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */)) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  }
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_LEVEL_SET(marpaESLIFOption.genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Exiting with status %d", exiti);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIF_context_t *marpaESLIF_contextp = (marpaESLIF_context_t *) userDatavp;

  *inputsp              = marpaESLIF_contextp->inputs;
  *inputlp              = marpaESLIF_contextp->inputl;
  *characterStreambp    = 0;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  *eofbp = 1;

  return 1;
}

/*****************************************************************************/
static short rhsb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  marpaESLIF_context_t *marpaESLIF_contextp = (marpaESLIF_context_t *) userDatavp;
  int                   parameteri;
  char                 *outputs;
  char                 *explanations;
  char                 *explanation_encodingasciis;

  /* ESLIF guarantees that contextp is not null and is a ROW */
  /* ESLIF always tries to put a number in the lowest storage that it fits into */
  if (contextp->type != MARPAESLIF_VALUE_TYPE_ROW) {
    fprintf(stderr, "Unsupported type %d for first context argument\n", contextp[0].type);
    return 0;
  }
  if (contextp->u.r.sizel != 3) {
    fprintf(stderr, "Invalid number of items in context: got %ld, expected 3\n", (unsigned long) contextp->u.r.sizel);
    return 0;
  }

  switch (contextp->u.r.p[0].type) {
  case MARPAESLIF_VALUE_TYPE_SHORT:
    parameteri = contextp->u.r.p[0].u.b;
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    parameteri = contextp->u.r.p[0].u.i;
    break;
  default:
    fprintf(stderr, "Unsupported type %d for first argument in context\n", contextp->u.r.p[0].type);
    return 0;
  }

  switch (contextp->u.r.p[2].type) {
  case MARPAESLIF_VALUE_TYPE_STRING:
    explanations = (char *) contextp->u.r.p[2].u.s.p;
    if (explanations == NULL) {
      fprintf(stderr, "Explanation is NULL for third argument in context\n");
    }
    explanation_encodingasciis = contextp->u.r.p[2].u.s.encodingasciis;
    if (explanation_encodingasciis == NULL) {
      fprintf(stderr, "Explanation encoding is NULL for third argument in context\n");
    }
    break;
  default:
    fprintf(stderr, "Unsupported type %d for third argument in context\n", contextp->u.r.p[2].type);
    return 0;
  }

  outputs = (char *) malloc(1000);
  if (outputs == NULL) {
    fprintf(stderr, "malloc failure, %s\n", strerror(errno));
    return 0;
  }

  if (parameteri > 4) {
    sprintf(outputs, "start ::= '%d'", parameteri);
  } else {
    sprintf(outputs, "start ::= . => rhs->(5, { x = 'Value of x', y = 'Value of y' }, String('Input should be 5'))");
  }

  marpaESLIFValueResultOutp->p              = (unsigned char *) outputs;
  marpaESLIFValueResultOutp->freeUserDatavp = (void *) marpaESLIF_contextp;
  marpaESLIFValueResultOutp->freeCallbackp  = stringFreeCallbackv;
  marpaESLIFValueResultOutp->shallowb       = 0;
  marpaESLIFValueResultOutp->sizel          = strlen(outputs);
  marpaESLIFValueResultOutp->encodingasciis = (char *) ASCIIs;

  fprintf(stdout, "[C] parameter=%d, explanation=%s [%s] => %s\n", parameteri, explanations, explanation_encodingasciis, outputs);

  return 1;
}

/*****************************************************************************/
static marpaESLIFRecognizerGeneratorCallback_t generatorActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  if (strcmp(actions, "rhs") == 0) {
    return rhsb;
  } else {
    fprintf(stderr, "Unsupported generator action \"%s\"\n", actions);
    return NULL;
  }
}

/*****************************************************************************/
static void stringFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  free(marpaESLIFValueResultp->u.s.p);
  if ((marpaESLIFValueResultp->u.s.encodingasciis != NULL) && (marpaESLIFValueResultp->u.s.encodingasciis != ASCIIs)) {
    free(marpaESLIFValueResultp->u.s.encodingasciis);
  }
}
