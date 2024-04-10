#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static const char *ASCIIs = "ASCII";

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short parameterizedRhsb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
static marpaESLIFRecognizerGeneratorCallback_t generatorActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static void  stringFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short valueImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);

typedef struct marpaESLIF_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
  int              nbCalli;
} marpaESLIF_context_t;

const static char *grammars = "\n"
  "/*\n"
  " * Example of parameterized rules\n"
  "*/\n"
  ":default ::= action => ::shift\n"
  "top ::= . => parameterizedRhs->(1)\n"
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
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  char                        *grammarscripts;
  marpaESLIFSymbol_t          *stringSymbolp = NULL;
  marpaESLIFSymbol_t          *stringSymbol2p = NULL;
  marpaESLIFSymbol_t          *regexSymbolp = NULL;
  marpaESLIFString_t           string;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t           *marpaESLIFValuep = NULL;
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
  marpaESLIF_context.nbCalli        = 0;

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

  /* Get eventual value */
  marpaESLIFValueOption.userDatavp             = genericLoggerp;
  marpaESLIFValueOption.ruleActionResolverp    = NULL;
  marpaESLIFValueOption.symbolActionResolverp  = NULL;
  marpaESLIFValueOption.importerp              = &valueImportb;
  marpaESLIFValueOption.highRankOnlyb          = 1;
  marpaESLIFValueOption.orderByRankb           = 1;
  marpaESLIFValueOption.ambiguousb             = 0;
  marpaESLIFValueOption.nullb                  = 0;
  marpaESLIFValueOption.maxParsesi             = 0;

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
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    goto err;
  }
  while (1) {
    short rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep);
    if (rcValueb < 0) {
      goto err;
    } else if (rcValueb == 0) {
      break;
    }
  }

  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFValuep = NULL;
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

  GENERICLOGGER_NOTICE(genericLoggerp, "Testing parse");
  marpaESLIF_context.inputs         = (char *) inputs;
  marpaESLIF_context.inputl         = strlen(inputs);
  marpaESLIF_context.nbCalli        = 0;
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL /* exhaustedbp */)) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  if (marpaESLIFValuep != NULL) {
    marpaESLIFValue_freev(marpaESLIFValuep);
  }
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
static short parameterizedRhsb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  marpaESLIF_context_t *marpaESLIF_contextp = (marpaESLIF_context_t *) userDatavp;
  int                   parameteri;
  char                 *outputs;

  /* ESLIF guarantees that contextp is not null and is a ROW */
  /* ESLIF always tries to put a number in the lowest storage that it fits into */
  if (contextp->type != MARPAESLIF_VALUE_TYPE_ROW) {
    fprintf(stderr, "Unsupported type %d for first context argument\n", contextp[0].type);
    return 0;
  }
  if (contextp->u.r.sizel != 1) {
    fprintf(stderr, "Invalid number of items in context: got %ld, expected 1\n", (unsigned long) contextp->u.r.sizel);
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

  outputs = (char *) malloc(1000);
  if (outputs == NULL) {
    fprintf(stderr, "malloc failure, %s\n", strerror(errno));
    return 0;
  }

  /* Put \0 just to a nice log, we replace it with a newline just after logging */
  marpaESLIF_contextp->nbCalli++;
  if (marpaESLIF_contextp->nbCalli == 5) {
    sprintf(outputs, "'5'\0");
  } else if (marpaESLIF_contextp->nbCalli > 5) {
    sprintf(outputs, "'no match'\0");
  } else {
    sprintf(outputs, ". => parameterizedRhs->(%d)\0", parameteri + 1);
  }

  marpaESLIFValueResultOutp->p              = (unsigned char *) outputs;
  marpaESLIFValueResultOutp->freeUserDatavp = (void *) marpaESLIF_contextp;
  marpaESLIFValueResultOutp->freeCallbackp  = stringFreeCallbackv;
  marpaESLIFValueResultOutp->shallowb       = 0;
  marpaESLIFValueResultOutp->sizel          = strlen(outputs);
  marpaESLIFValueResultOutp->encodingasciis = (char *) ASCIIs;

  fprintf(stdout, "parameterizedRhs(%d) returns: %s\n", parameteri, outputs);
  outputs[strlen(outputs)] = '\n';

  return 1;
}

/*****************************************************************************/
static marpaESLIFRecognizerGeneratorCallback_t generatorActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  if (strcmp(actions, "parameterizedRhs") == 0) {
    return parameterizedRhsb;
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

/*****************************************************************************/
static short valueImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb)
/*****************************************************************************/
{
  genericLogger_t *genericLoggerp = (genericLogger_t *) userDatavp;
  size_t           i;
  short            rcb = 0;

  /* Result MUST be array with value '5' */

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    GENERICLOGGER_ERROR(genericLoggerp, "Result type is undef");
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is char: %c", marpaESLIFValueResultp->u.c);
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is short: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is int: %d", marpaESLIFValueResultp->u.i);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is long: %ld", marpaESLIFValueResultp->u.l);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is float: %f", (double) marpaESLIFValueResultp->u.f);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is double: %f", marpaESLIFValueResultp->u.d);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is ptr: %p", marpaESLIFValueResultp->u.p.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    if (marpaESLIFValueResultp->u.a.sizel > 0) {
      GENERICLOGGER_NOTICEF(genericLoggerp, "Result type is array: {%p,%ld}", marpaESLIFValueResultp->u.a.p, (unsigned long) marpaESLIFValueResultp->u.a.sizel);
    } else {
      GENERICLOGGER_ERRORF(genericLoggerp, "Result type is array: {%p,%ld}", marpaESLIFValueResultp->u.a.p, (unsigned long) marpaESLIFValueResultp->u.a.sizel);
    }
    for (i = 0; i < marpaESLIFValueResultp->u.a.sizel; i++) {
      if (i == 0) {
        if (marpaESLIFValueResultp->u.a.p[i] == '5') {
          GENERICLOGGER_NOTICEF(genericLoggerp, "  array[%ld] = '%c'", (unsigned long) i, marpaESLIFValueResultp->u.a.p[i]);
          rcb = 1;
        } else {
          GENERICLOGGER_ERRORF(genericLoggerp, "  array[%ld] = '%c'", (unsigned long) i, marpaESLIFValueResultp->u.a.p[i]);
        }
      } else {
        GENERICLOGGER_ERRORF(genericLoggerp, "  array[%ld] = '%c'", (unsigned long) i, marpaESLIFValueResultp->u.a.p[i]);
      }
    }
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is bool: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is string: %s, encoding: %s", marpaESLIFValueResultp->u.s.p, marpaESLIFValueResultp->u.s.encodingasciis);
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is row: {%p,%ld}", marpaESLIFValueResultp->u.r.p, (unsigned long) marpaESLIFValueResultp->u.r.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    GENERICLOGGER_ERRORF(genericLoggerp, "Result type is table: {%p,%ld}", marpaESLIFValueResultp->u.t.p, (unsigned long) marpaESLIFValueResultp->u.t.sizel);
    break;
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Unsupported result type %d", marpaESLIFValueResultp->type);
    break;
  }

  return rcb;
}
