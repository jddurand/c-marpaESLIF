#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

typedef struct reader_context {
  char            *inputs;
  size_t           inputl;
} reader_context_t;

typedef enum stack_context {
  INTEGER_CONTEXT = 1,
  DOUBLE_CONTEXT
} stack_context_t;

static short                         inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                         do_int(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         do_op(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

const static char *grammars =
  ":default ::= action => do_op\n"
  ":discard ::= whitespace\n"
  ":discard ::= comment\n"
  "Expression ::=\n"
  "    /[\\d]+/                          action => do_int\n"
  "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
  "   ||     Expression '**' Expression  assoc => right\n"
  "   ||     Expression  '*' Expression\n"
  "    |     Expression  '/' Expression\n"
  "   ||     Expression  '+' Expression\n"
  "    |     Expression  '-' Expression\n"
  "whitespace :[1]:= [\\s]\n"
  "comment ~ /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u\n"
  "\n";

int main() {
  genericLogger_t             *genericLoggerp;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  int                          leveli;
  int                          ngrammari;
  char                        *grammarshows;
  reader_context_t             reader_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp;
  char                         inputs[4096];
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFValue_t           *marpaESLIFValuep;
  marpaESLIFValueResult_t      marpaESLIFValueResult;
  char                        *offsetp;
  size_t                       lengthl;
  char                        *expressions;
  
  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    exit(1);
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    exit(1);
  }

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    exit(1);
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

  marpaESLIFRecognizerOption.userDatavp                = &reader_context;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb                = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb                  = 1;  /* Do newline counting */
  marpaESLIFRecognizerOption.trackb                    = 1;  /* Do absolute position tracking */
  marpaESLIFRecognizerOption.bufsizl                   = 0;  /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    exit(1);
  }

  if (fgets(inputs, sizeof(inputs), stdin) == NULL) {
    perror("fgets");
    exit(1);
  }
  reader_context.inputs = inputs;
  reader_context.inputl = strlen(inputs);
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp,
                                   0 /* initialEventsb */,
                                   NULL /* continuebp*/,
                                   NULL /* exhaustedbp */)) {
    exit(1);
  }

  marpaESLIFValueOption.userDatavp             = NULL; /* No context... why not */
  marpaESLIFValueOption.ruleActionResolverp    = ruleActionResolverp;
  marpaESLIFValueOption.symbolActionResolverp  = NULL; /* No symbol action resolver... Okay we use the default */
  marpaESLIFValueOption.freeActionResolverp    = NULL; /* No free action resolver... Okay if we generate no pointer */
  marpaESLIFValueOption.highRankOnlyb          = 1;    /* Recommended value */
  marpaESLIFValueOption.orderByRankb           = 1;    /* Recommended value */
  marpaESLIFValueOption.ambiguousb             = 0;    /* Recommended value */
  marpaESLIFValueOption.nullb                  = 0;    /* Recommended value */
  marpaESLIFValueOption.maxParsesi             = 0;    /* Get all parse tree values - meaningless because ambiguousb is false */
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    exit(1);
  }
  if (! marpaESLIFValue_valueb(marpaESLIFValuep, &marpaESLIFValueResult)) {
    exit(1);
  }
  if (! marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizerp, "Expression", &offsetp, &lengthl)) {
    fprintf(stderr, "marpaESLIFRecognizer_last_completedb failure!\n");
    exit(1);
  }

  expressions = (char *) malloc(lengthl + 1);
  memcpy(expressions, inputs + (size_t) offsetp, lengthl);
  expressions[lengthl] = '\0';

  switch (marpaESLIFValueResult.contexti) {
  case INTEGER_CONTEXT:
    fprintf(stdout, "%s = %d\n", expressions, marpaESLIFValueResult.u.i);
    break;
  case DOUBLE_CONTEXT:
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    fprintf(stdout, "%s = %f\n", expressions, marpaESLIFValueResult.u.d);
    break;
  default:
    fprintf(stdout, "Result is of unexpected type %d!\n", marpaESLIFValueResult.type);
    exit(1);
  }
  free(expressions);

  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);

  exit(0);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  reader_context_t *reader_contextp = (reader_context_t *) userDatavp;
  
  *inputsp              = reader_contextp->inputs;
  *inputlp              = reader_contextp->inputl;
  *eofbp                = 1;    /* One chunk */
  *characterStreambp    = 1;    /* We say this is a stream of characters */
  *encodingOfEncodingsp = NULL; /* let marpaESLIF deal with encoding */
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}

/*****************************************************************************/
static short do_int(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* /[\d]+/  action => do_int */
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  int                      i;

  if (nullableb) {
    fprintf(stderr, "Nullable is impossible here!\n");
    return 0;
  }

  marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (marpaESLIFValueResultp == NULL) {
    fprintf(stderr, "marpaESLIFValue_stack_getp failure!\n");
    return 0;
  }
  if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_ARRAY) {
    fprintf(stderr, "marpaESLIFValueResultp->type is not ARRAY but %d!\n", marpaESLIFValueResultp->type);
    return 0;
  }

  i = atoi((char *) marpaESLIFValueResultp->u.p);

  marpaESLIFValueResult.contexti        = INTEGER_CONTEXT;           /* Anything but 0 */
  marpaESLIFValueResult.sizel           = 0;                         /* Not relevant */
  marpaESLIFValueResult.representationp = NULL;                      /* No representation */
  marpaESLIFValueResult.shallowb        = 0;                         /* Not relevant */
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT; /* Integer type */
  marpaESLIFValueResult.u.i             = i;                         /* Integer value */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static short do_op(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* Expression OPERATOR Expression */
  marpaESLIFValueResult_t *marpaESLIFValueResultpp[3];
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int     j;
  int     i[2];
  double  d[2];
  short   intb[2];
  short   doubleb[2];
  char   *ops;
  int     valuei;
  double  valued;
  short   resultIsIntb;

  if (nullableb) {
    fprintf(stderr, "Nullable is impossible here!\n");
    return 0;
  }

  marpaESLIFValueResultpp[0] = marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (marpaESLIFValueResultpp[0] == NULL) {
    fprintf(stderr, "marpaESLIFValue_stack_getp failure!\n");
    return 0;
  }
  marpaESLIFValueResultpp[1] = marpaESLIFValue_stack_getp(marpaESLIFValuep, argni);
  if (marpaESLIFValueResultpp[0] == NULL) {
    fprintf(stderr, "marpaESLIFValue_stack_getp failure!\n");
    return 0;
  }

  intb[0] = (marpaESLIFValueResultpp[0]->contexti == INTEGER_CONTEXT);
  intb[1] = (marpaESLIFValueResultpp[1]->contexti == INTEGER_CONTEXT);

  doubleb[0] = (marpaESLIFValueResultpp[0]->contexti == DOUBLE_CONTEXT);
  doubleb[1] = (marpaESLIFValueResultpp[1]->contexti == DOUBLE_CONTEXT);

  for (j = 0; j < 2; j++) {
    if ((! intb[j]) && (! doubleb[j])) {
      fprintf(stderr, "Expression No %d is neither an int or a double!\n", j);
      return 0;
    }
    /* Get values */
    if (intb[j]) {
      i[j] = marpaESLIFValueResultpp[j]->u.i;
      /* fprintf(stderr, "<== %d\n", i[j]); */
    } else {
      d[j] = marpaESLIFValueResultpp[j]->u.d;
      /* fprintf(stderr, "<== %f\n", d[j]); */
    }
  }

  /* By definition OPERATOR is a lexeme, i.e. an ARRAY */
  marpaESLIFValueResultpp[2] = marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i + 1);
  if (marpaESLIFValueResultpp[2] == NULL) {
    fprintf(stderr, "marpaESLIFValue_stack_getp failure!\n");
    return 0;
  }
  ops = (char *) marpaESLIFValueResultpp[2]->u.p;
  /* For convenience, marpaESLIF is ALWAYS putting a NUL character after every lexeme buffer. */
  
  if (strcmp(ops, "**") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      valued = pow((double) i[0], (double) i[1]);
    } else {
      valued = pow((intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1]));
    }
    resultIsIntb = 0;
  } else if (strcmp(ops, "*") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      valuei = i[0] * i[1];
      resultIsIntb = 1;
    } else {
      valued = (intb[0] ? (double) i[0] : d[0]) * (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else if (strcmp(ops, "/") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      if (intb[1] == 0) {
        fprintf(stderr, "Division by zero!\n");
        return 0;
      }
      valued = (double) i[0] / (double) i[1];
    } else {
      if ((intb[1] ? (double) i[1] : d[1]) < 0.E-8) {
        fprintf(stderr, "Division by zero (well, value is < 10E-8) !\n");
        return 0;
      }
      valued = (intb[0] ? (double) i[0] : d[0]) / (intb[1] ? (double) i[1] : d[1]);
    }
    resultIsIntb = 0;
  } else if (strcmp(ops, "+") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      valuei = i[0] + i[1];
      resultIsIntb = 1;
    } else {
      valued = (intb[0] ? (double) i[0] : d[0]) + (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else if (strcmp(ops, "-") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      valuei = i[0] - i[1];
      resultIsIntb = 1;
    } else {
      valued = (intb[0] ? (double) i[0] : d[0]) - (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else {
    fprintf(stderr, "Unknown operator %s!\n", ops);
    return 0;
  }

  if (resultIsIntb) {
    marpaESLIFValueResult.contexti        = INTEGER_CONTEXT;              /* Anything but 0 */
    marpaESLIFValueResult.sizel           = 0;                            /* Not relevant */
    marpaESLIFValueResult.representationp = NULL;                         /* No representation */
    marpaESLIFValueResult.shallowb        = 0;                            /* Not relevant */
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT;    /* Integer type */
    marpaESLIFValueResult.u.i             = valuei;                       /* Integer value */
    /* fprintf(stderr, "==> %d\n", valuei); */
  } else {
    marpaESLIFValueResult.contexti        = DOUBLE_CONTEXT;               /* Anything but 0 */
    marpaESLIFValueResult.sizel           = 0;                            /* Not relevant */
    marpaESLIFValueResult.representationp = NULL;                         /* No representation */
    marpaESLIFValueResult.shallowb        = 0;                            /* Not relevant */
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_DOUBLE; /* Double type */
    marpaESLIFValueResult.u.d             = valued;                       /* Double value */
    /* fprintf(stderr, "==> %f\n", valued); */
  }
  
  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  if (strcmp(actions, "do_int") == 0) {
    return do_int;
  } else if (strcmp(actions, "do_op") == 0) {
    return do_op;
  } else {
    fprintf(stderr, "Unknown action %s!\n", actions);
    return NULL;
  }
}
