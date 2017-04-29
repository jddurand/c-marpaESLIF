#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

#define DO_ALTERNATIVE_UNDEF(names) do {                                \
    marpaESLIFAlternative_t  marpaESLIFAlternative;                     \
                                                                        \
    marpaESLIFAlternative.lexemes        = names;                       \
    marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_UNDEF; \
    marpaESLIFAlternative.value.contexti = 0;                           \
    marpaESLIFAlternative.value.sizel    = 0;                           \
    marpaESLIFAlternative.grammarLengthl = 1;                           \
                                                                        \
    GENERICLOGGER_DEBUGF(genericLoggerp, "Injecting <%s>", names);      \
    if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 0)) { \
      GENERICLOGGER_ERRORF(genericLoggerp, "Failure to set <%s> alternative", names); \
      marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp, -1, -1, GENERICLOGGER_LOGLEVEL_ERROR); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define DO_ALTERNATIVE_CHAR(names, boolval) do {                        \
    marpaESLIFAlternative_t  marpaESLIFAlternative;                     \
                                                                        \
    marpaESLIFAlternative.lexemes        = names;                       \
    marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_CHAR;  \
    marpaESLIFAlternative.value.u.c      = boolval;                     \
    marpaESLIFAlternative.value.contexti = 0;                           \
    marpaESLIFAlternative.value.sizel    = 0;                           \
    marpaESLIFAlternative.grammarLengthl = 1;                           \
                                                                        \
    GENERICLOGGER_DEBUGF(genericLoggerp, "Injecting <%s> with value '%c'", names, boolval); \
    if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 0)) { \
      GENERICLOGGER_ERRORF(genericLoggerp, "Failure to set <%s> alternative", names); \
      marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp, -1, -1, GENERICLOGGER_LOGLEVEL_ERROR); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

const static char *grammars =
  "\n"
  "    :default ::= action => ::shift\n"
  "\n"
  "    Sentence        ::= SYMBOL\n"
  "                      | LPAREN Sentence RPAREN              assoc => group action => ::copy[1]\n"
  "                     || NOT Sentence                                       action => action_not\n"
  "                     ||        Sentence        AND Sentence                action => action_and\n"
  "                     ||        Sentence         OR Sentence                action => action_or\n"
  "                     ||        Sentence    IMPLIES Sentence                action => action_implies\n"
  "                      |        Sentence EQUIVALENT Sentence                action => action_equivalent\n"
  "\n"
  "    _DUMMY      ~ [^\\s\\S]\n"
  "    LPAREN      ~ _DUMMY\n"
  "    RPAREN      ~ _DUMMY\n"
  "    NOT         ~ _DUMMY\n"
  "    AND         ~ _DUMMY\n"
  "    OR          ~ _DUMMY\n"
  "    IMPLIES     ~ _DUMMY\n"
  "    EQUIVALENT  ~ _DUMMY\n"
  "    SYMBOL      ~ _DUMMY\n"
  ;

static short                         failReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                         action_not(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_and(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_or(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_implies(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_equivalent(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         get_valueb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int argi, short *valuebp);

typedef struct valueContext {
  genericLogger_t *genericLoggerp;
  short            p;
  short            q;
  short            r;
} valueContext_t;

int main() {
  genericLogger_t             *genericLoggerp        = NULL;
  marpaESLIF_t                *marpaESLIFp           = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp    = NULL;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t           *marpaESLIFValuep = NULL;
  int                          rci;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFValueResult_t      marpaESLIFValueResult;
  char                        *examples = "(P AND R IMPLIES Q) AND P AND R AND NOT Q";
  valueContext_t               valueContext;

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

  GENERICLOGGER_INFOF(genericLoggerp, "Generating grammar:\n%s", grammars);
  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  marpaESLIFRecognizerOption.userDatavp                = NULL;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = failReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb                = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb                  = 1;  /* Do newline counting */
  marpaESLIFRecognizerOption.trackb                    = 0;  /* Do absolute position tracking */
  marpaESLIFRecognizerOption.bufsizl                   = 0;  /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    exit(1);
  }

  GENERICLOGGER_INFOF(genericLoggerp, "Injecting lexemes for: %s", examples);
  DO_ALTERNATIVE_UNDEF("LPAREN");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'P');
  DO_ALTERNATIVE_UNDEF("AND");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'R');
  DO_ALTERNATIVE_UNDEF("IMPLIES");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'Q');
  DO_ALTERNATIVE_UNDEF("RPAREN");
  DO_ALTERNATIVE_UNDEF("AND");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'P');
  DO_ALTERNATIVE_UNDEF("AND");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'R');
  DO_ALTERNATIVE_UNDEF("AND");
  DO_ALTERNATIVE_UNDEF("NOT");
  DO_ALTERNATIVE_CHAR ("SYMBOL", 'Q');

  /* Do valuation - we expect 0 in any case */

  valueContext.genericLoggerp = genericLoggerp;
  for (valueContext.p = 0; valueContext.p < 2; valueContext.p++) {
    for (valueContext.q = 0; valueContext.q < 2; valueContext.q++) {
      for (valueContext.r = 0; valueContext.r < 2; valueContext.r++) {

        marpaESLIFValueOption.userDatavp             = &valueContext;
        marpaESLIFValueOption.ruleActionResolverp    = ruleActionResolverp;
        marpaESLIFValueOption.symbolActionResolverp  = NULL; /* No symbol action resolver... Okay we use the default */
        marpaESLIFValueOption.freeActionResolverp    = NULL; /* No free action resolver... Okay if we generate no pointer */
        marpaESLIFValueOption.highRankOnlyb          = 1;    /* Recommended value */
        marpaESLIFValueOption.orderByRankb           = 1;    /* Recommended value */
        marpaESLIFValueOption.ambiguousb             = 0;    /* our BNF is not ambiguous thanks to loosen operator and group association */
        marpaESLIFValueOption.maxParsesi             = 0;    /* Meaningless here since we say it is not ambiguous */
        marpaESLIFValueOption.nullb                  = 0;    /* Recommended value */

        GENERICLOGGER_INFOF(genericLoggerp, "Valuation with {P, Q, R} = {%d, %d, %d} of %s ", (int) valueContext.p, (int) valueContext.q, (int) valueContext.r, examples);
        marpaESLIFValue_freev(marpaESLIFValuep); /* This is NULL protected */
        marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
        if (marpaESLIFValuep == NULL) {
          goto err;
        }

        if (marpaESLIFValue_valueb(marpaESLIFValuep, &marpaESLIFValueResult) <= 0) {
          goto err;
        }

        if (marpaESLIFValueResult.type != MARPAESLIF_VALUE_TYPE_SHORT) {
          GENERICLOGGER_ERROR(genericLoggerp, "Valuation result is not a boolean !?");
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d ", (int) valueContext.p, (int) valueContext.q, (int) valueContext.r, (int) marpaESLIFValueResult.u.b);
        if (marpaESLIFValueResult.u.b != 0) {
          GENERICLOGGER_ERROR(genericLoggerp, "Valuation result is not zero !?");
          goto err;
          }
      }
    }
  }

  rci = 0;
  goto done;

 err:
  rci = 1;

 done:
  /* All these calls are NULL protected */
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);

  exit(rci);
}

/*****************************************************************************/
static short failReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  return 0;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  valueContext_t *valueContextp = (valueContext_t *) userDatavp;

  if (strcmp(actions, "action_not") == 0) {
    return action_not;
  }
  else if (strcmp(actions, "action_and") == 0) {
    return action_and;
  }
  else if (strcmp(actions, "action_or") == 0) {
    return action_or;
  }
  else if (strcmp(actions, "action_implies") == 0) {
    return action_implies;
  }
  else if (strcmp(actions, "action_equivalent") == 0) {
    return action_equivalent;
  }
  else {
    GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Unknown action %s", actions);
    return NULL;
  }
}

/* On the stack, value type is either an alternative (our input), either a SHORT (a result) */
/*****************************************************************************/
static short get_valueb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int argi, short *valuebp)
/*****************************************************************************/
{
  valueContext_t          *valueContextp = (valueContext_t *) userDatavp;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  short                    valueb;
  char                     c;

  marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, argi);
  if (marpaESLIFValueResultp == NULL) {
    return 0;
  }

  if ((marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_SHORT) &&
      (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_ARRAY)) {
    GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Wrong type %d", marpaESLIFValueResultp->type);
    return 0;
  }

  if (marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_SHORT) {
    valueb = marpaESLIFValueResultp->u.b;
  } else {
    if (marpaESLIFValueResultp->sizel > 0) {
      GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Got array with size %ld", (unsigned long) marpaESLIFValueResultp->sizel);
      return 0;
    }
    marpaESLIFValueResultp = (marpaESLIFValueResult_t *) marpaESLIFValueResultp->u.p;
    if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_CHAR) {
      GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Wrong alternative type %d", marpaESLIFValueResultp->type);
      return 0;
    }

    c = marpaESLIFValueResultp->u.c;
    switch (c) {
    case 'P':
      valueb = valueContextp->p;
      break;
    case 'Q':
      valueb = valueContextp->q;
      break;
    case 'R':
      valueb = valueContextp->r;
      break;
    default:
      GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Unknown char '%c'", c);
      return 0;
    }
  }

  if (valuebp != NULL) {
    *valuebp = valueb;
  }

  return 1;
}

/*****************************************************************************/
static short action_not(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t         *valueContextp = (valueContext_t *) userDatavp;
  short                   valb;
  short                   resultb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! get_valueb(userDatavp, marpaESLIFValuep, argni, &valb)) {
    return 0;
  }

  resultb = (valb ? 0 : 1);
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... NOT %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) valb, (int) resultb);

  marpaESLIFValueResult.contexti = 1;           /* It has to be != 0 */
  marpaESLIFValueResult.sizel = 0;              /* Not meaninful here */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.shallowb = 0;           /* Not meaninful here */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static short action_and(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t         *valueContextp = (valueContext_t *) userDatavp;
  short                   leftb;
  short                   rightb;
  short                   resultb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! get_valueb(userDatavp, marpaESLIFValuep, arg0i, &leftb)) {
    return 0;
  }
  if (! get_valueb(userDatavp, marpaESLIFValuep, argni, &rightb)) {
    return 0;
  }

  resultb = (leftb && rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d AND %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contexti = 1;           /* It has to be != 0 */
  marpaESLIFValueResult.sizel = 0;              /* Not meaninful here */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.shallowb = 0;           /* Not meaninful here */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static short action_or(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t         *valueContextp = (valueContext_t *) userDatavp;
  short                   leftb;
  short                   rightb;
  short                   resultb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! get_valueb(userDatavp, marpaESLIFValuep, arg0i, &leftb)) {
    return 0;
  }
  if (! get_valueb(userDatavp, marpaESLIFValuep, argni, &rightb)) {
    return 0;
  }

  resultb = (leftb || rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d OR %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contexti = 1;           /* It has to be != 0 */
  marpaESLIFValueResult.sizel = 0;              /* Not meaninful here */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.shallowb = 0;           /* Not meaninful here */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static short action_implies(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t         *valueContextp = (valueContext_t *) userDatavp;
  short                   leftb;
  short                   rightb;
  short                   resultb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! get_valueb(userDatavp, marpaESLIFValuep, arg0i, &leftb)) {
    return 0;
  }
  if (! get_valueb(userDatavp, marpaESLIFValuep, argni, &rightb)) {
    return 0;
  }

  resultb = leftb ? rightb : 1;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d IMPLIES %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contexti = 1;           /* It has to be != 0 */
  marpaESLIFValueResult.sizel = 0;              /* Not meaninful here */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.shallowb = 0;           /* Not meaninful here */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
static short action_equivalent(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t         *valueContextp = (valueContext_t *) userDatavp;
  short                   leftb;
  short                   rightb;
  short                   resultb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! get_valueb(userDatavp, marpaESLIFValuep, arg0i, &leftb)) {
    return 0;
  }
  if (! get_valueb(userDatavp, marpaESLIFValuep, argni, &rightb)) {
    return 0;
  }

  resultb = (leftb == rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d EQUIVALENT %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contexti = 1;           /* It has to be != 0 */
  marpaESLIFValueResult.sizel = 0;              /* Not meaninful here */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.shallowb = 0;           /* Not meaninful here */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}
