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
  DOUBLE_CONTEXT = 1
} stack_context_t;

static short                         inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                         do_int(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         do_double(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

const static char *grammars =
  ":default ::= action => do_double\n"
  ":discard ::= [\\s]\n"
  "Expression ::=\n"
  "    /[\\d]+/                          action => do_int\n"
  "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
  "   ||     Expression '**' Expression  assoc => right\n"
  "   ||     Expression  '*' Expression\n"
  "    |     Expression  '/' Expression\n"
  "   ||     Expression  '+' Expression\n"
  "    |     Expression  '-' Expression\n"
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
  int                          rcValueb;
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
  /*
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }
  */

  marpaESLIFRecognizerOption.userDatavp                = &reader_context;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb                = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb                  = 1;  /* Do newline counting */
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

  switch (marpaESLIFValueResult.type) {
  case MARPAESLIF_STACK_TYPE_INT:
    fprintf(stdout, "%s = %d\n", expressions, marpaESLIFValueResult.u.i);
    break;
  case MARPAESLIF_STACK_TYPE_DOUBLE:
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
  void   *bytep;
  size_t  bytel;
  int     i;

  if (nullableb) {
    fprintf(stderr, "Nullable is impossible here!\n");
    return 0;
  }

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    fprintf(stderr, "marpaESLIFValue_stack_get_arrayb failure!\n");
    return 0;
  }

  i = atoi((char *) bytep);

  if (! marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, INTEGER_CONTEXT /* Anything but 0 */, i)) {
    return 0;
  }

  return 1; /* Ok */
}

/*****************************************************************************/
static short do_double(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* Expression OPERATOR Expression */
  int     i[2];
  double  d[2];
  short   intb[2];
  short   doubleb[2];
  void   *bytep;
  size_t  bytel;
  int     valuei;
  double  valued;
  short   resultIsIntb;

  if (nullableb) {
    fprintf(stderr, "Nullable is impossible here!\n");
    return 0;
  }

  /* First expression is an int or a double */
  if (! marpaESLIFValue_stack_is_intb(marpaESLIFValuep, arg0i, &(intb[0]))) {
    fprintf(stderr, "marpaESLIFValue_stack_is_intb failure!\n");
    return 0;
  }
  if (! intb[0]) {
    /* Then it must be a double */
    if (! marpaESLIFValue_stack_is_doubleb(marpaESLIFValuep, arg0i, &(doubleb[0]))) {
      fprintf(stderr, "marpaESLIFValue_stack_is_doubleb failure!\n");
      return 0;
    }
    if (! doubleb[0]) {
      fprintf(stderr, "First expression is neither an int or a double!\n");
      return 0;
    }
  }

  /* Idem for second expression */
  if (! marpaESLIFValue_stack_is_intb(marpaESLIFValuep, argni, &(intb[1]))) {
    fprintf(stderr, "marpaESLIFValue_stack_is_intb failure!\n");
    return 0;
  }
  if (! intb[1]) {
    /* Then it must be a double */
    if (! marpaESLIFValue_stack_is_doubleb(marpaESLIFValuep, argni, &(doubleb[1]))) {
      fprintf(stderr, "marpaESLIFValue_stack_is_doubleb failure!\n");
      return 0;
    }
    if (! doubleb[1]) {
      fprintf(stderr, "Second expression is neither an int or a double!\n");
      return 0;
    }
  }

  /* Get values */
  if (intb[0]) {
    if (! marpaESLIFValue_stack_get_intb(marpaESLIFValuep, arg0i, NULL /* contextip */, &(i[0]))) {
      fprintf(stderr, "marpaESLIFValue_stack_get_intb failure!\n");
      return 0;
    }
  } else {
    if (! marpaESLIFValue_stack_get_doubleb(marpaESLIFValuep, arg0i, NULL /* contextip */, &(d[0]))) {
      fprintf(stderr, "marpaESLIFValue_stack_get_double failure!\n");
      return 0;
    }
  }
  if (intb[1]) {
    if (! marpaESLIFValue_stack_get_intb(marpaESLIFValuep, argni, NULL /* contextip */, &(i[1]))) {
      fprintf(stderr, "marpaESLIFValue_stack_get_intb failure!\n");
      return 0;
    }
  } else {
    if (! marpaESLIFValue_stack_get_doubleb(marpaESLIFValuep, argni, NULL /* contextip */, &(d[1]))) {
      fprintf(stderr, "marpaESLIFValue_stack_get_double failure!\n");
      return 0;
    }
  }
  
  /* Operator is a lexeme */
  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i + 1, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    fprintf(stderr, "marpaESLIFValue_stack_get_arrayb failure!\n");
    return 0;
  }
  /* For convenience, marpaESLIF is ALWAYS putting a NUL character after every lexeme buffer. */
  if (strcmp((char *) bytep, "**") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      /* fprintf(stdout, "%d ** %d\n", i[0], i[1]); */
      valued = pow((double) i[0], (double) i[1]);
    } else {
      /* fprintf(stdout, "%f ** %f\n", (intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1])); */
      valued = pow((intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1]));
    }
    resultIsIntb = 0;
  } else if (strcmp((char *) bytep, "*") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      /* fprintf(stdout, "%d * %d\n", i[0], i[1]); */
      valuei = i[0] * i[1];
      resultIsIntb = 1;
    } else {
      /* fprintf(stdout, "%f * %f\n", (intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1])); */
      valued = (intb[0] ? (double) i[0] : d[0]) * (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else if (strcmp((char *) bytep, "/") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      if (intb[1] == 0) {
        fprintf(stderr, "Division by zero!\n");
        return 0;
      }
      /* fprintf(stdout, "%f / %f\n", (double) i[0], (double) i[1]); */
      valued = (double) i[0] / (double) i[1];
    } else {
      if ((intb[1] ? (double) i[1] : d[1]) < 0.E-8) {
        fprintf(stderr, "Division by zero (well, value is < 10E-8) !\n");
        return 0;
      }
      /* fprintf(stdout, "%f / %f\n", (intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1])); */
      valued = (intb[0] ? (double) i[0] : d[0]) / (intb[1] ? (double) i[1] : d[1]);
    }
    resultIsIntb = 0;
  } else if (strcmp((char *) bytep, "+") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      /* fprintf(stdout, "%d + %d\n", i[0], i[1]); */
      valuei = i[0] + i[1];
      resultIsIntb = 1;
    } else {
      /* fprintf(stdout, "%f + %f\n", (intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1])); */
      valued = (intb[0] ? (double) i[0] : d[0]) + (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else if (strcmp((char *) bytep, "-") == 0) {
    /* Both are int ? */
    if (intb[0] && intb[1]) {
      /* fprintf(stdout, "%d - %d\n", i[0], i[1]); */
      valuei = i[0] - i[1];
      resultIsIntb = 1;
    } else {
      /* fprintf(stdout, "%f - %f\n", (intb[0] ? (double) i[0] : d[0]), (intb[1] ? (double) i[1] : d[1])); */
      valued = (intb[0] ? (double) i[0] : d[0]) - (intb[1] ? (double) i[1] : d[1]);
      resultIsIntb = 0;
    }
  } else {
    fprintf(stderr, "Unknown operator %s!\n", (char *) bytep);
    return 0;
  }

  return resultIsIntb ? marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, INTEGER_CONTEXT, valuei) : marpaESLIFValue_stack_set_doubleb(marpaESLIFValuep, resulti, DOUBLE_CONTEXT, valued);
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  if (strcmp(actions, "do_int") == 0) {
    return do_int;
  } else if (strcmp(actions, "do_double") == 0) {
    return do_double;
  } else {
    fprintf(stderr, "Unknown action %s!\n", actions);
    return NULL;
  }
}
