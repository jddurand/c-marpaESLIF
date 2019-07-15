#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

static char myContext;
#define MY_CONTEXT &myContext /* Any value != NULL 0 */

#define DO_ALTERNATIVE_UNDEF(names) do {                                \
    marpaESLIFAlternative_t  marpaESLIFAlternative;                     \
                                                                        \
    marpaESLIFAlternative.lexemes        = names;                       \
    marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_UNDEF; \
    marpaESLIFAlternative.value.contextp = MY_CONTEXT;                  \
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
    marpaESLIFAlternative.value.contextp = MY_CONTEXT;                  \
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
  "                     || NOT    Sentence                                    action => action_not\n"
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

static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                         action_not(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_and(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_or(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_implies(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                         action_equivalent(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

short                                importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short                         getFromstack(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int indicei);

typedef struct valueContext {
  genericLogger_t *genericLoggerp;
  short            p;
  short            q;
  short            r;
  short            result;
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
  marpaESLIFGrammarOption.bytep     = (void *) grammars;
  marpaESLIFGrammarOption.bytel     = strlen(grammars);
  marpaESLIFGrammarOption.encodings = NULL;
  marpaESLIFGrammarOption.encodingl = 0;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  marpaESLIFRecognizerOption.userDatavp                = NULL;
  marpaESLIFRecognizerOption.readerCallbackp           = NULL; /* We completely externalize lexing */
  marpaESLIFRecognizerOption.disableThresholdb         = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb                = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb                  = 1;  /* Do newline counting */
  marpaESLIFRecognizerOption.trackb                    = 0;  /* Do absolute position tracking */
  marpaESLIFRecognizerOption.bufsizl                   = 0;  /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerOption.ifActionResolverp         = NULL; /* symbol if-action resolver */
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
        marpaESLIFValueOption.importerp              = importb;
        marpaESLIFValueOption.highRankOnlyb          = 1;    /* Recommended value */
        marpaESLIFValueOption.orderByRankb           = 1;    /* Recommended value */
        marpaESLIFValueOption.ambiguousb             = 0;    /* our BNF is not ambiguous thanks to loosen operator and group association */
        marpaESLIFValueOption.maxParsesi             = 0;    /* Meaningless here since we say it is not ambiguous */
        marpaESLIFValueOption.nullb                  = 0;    /* Recommended value */

        GENERICLOGGER_INFOF(genericLoggerp, "Valuation with {P, Q, R} = {%d, %d, %d} of %s ", (int) valueContext.p, (int) valueContext.q, (int) valueContext.r, examples);

        marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
        if (marpaESLIFValuep == NULL) {
          goto err;
        }

        if (marpaESLIFValue_valueb(marpaESLIFValuep) <= 0) {
          goto err;
        }

        marpaESLIFValue_freev(marpaESLIFValuep);
        marpaESLIFValuep = NULL;

        GENERICLOGGER_INFOF(genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d ", (int) valueContext.p, (int) valueContext.q, (int) valueContext.r, (int) valueContext.result);
        if (valueContext.result != 0) {
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
static short failReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
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

/*****************************************************************************/
static short action_not(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t          *valueContextp = (valueContext_t *) userDatavp;
  short                    valb;
  short                    resultb;
  marpaESLIFValueResult_t  marpaESLIFValueResult;

  if (! getFromstack(userDatavp, marpaESLIFValuep, argni)) return 0;

  resultb = (valueContextp->result ? 0 : 1);
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... NOT %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) valb, (int) resultb);

  marpaESLIFValueResult.contextp = MY_CONTEXT;  /* It has to be > 0 */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
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

  if (! getFromstack(userDatavp, marpaESLIFValuep, arg0i)) return 0;
  leftb = valueContextp->result;

  if (! getFromstack(userDatavp, marpaESLIFValuep, argni)) return 0;
  rightb = valueContextp->result;

  resultb = (leftb && rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d AND %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contextp = MY_CONTEXT;  /* It has to be > 0 */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
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

  if (! getFromstack(userDatavp, marpaESLIFValuep, arg0i)) return 0;
  leftb = valueContextp->result;

  if (! getFromstack(userDatavp, marpaESLIFValuep, argni)) return 0;
  rightb = valueContextp->result;

  resultb = (leftb || rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d OR %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contextp = MY_CONTEXT;  /* It has to be > 0 */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
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

  if (! getFromstack(userDatavp, marpaESLIFValuep, arg0i)) return 0;
  leftb = valueContextp->result;

  if (! getFromstack(userDatavp, marpaESLIFValuep, argni)) return 0;
  rightb = valueContextp->result;

  resultb = leftb ? rightb : 1;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d IMPLIES %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contextp = MY_CONTEXT;  /* It has to be > 0 */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
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

  if (! getFromstack(userDatavp, marpaESLIFValuep, arg0i)) return 0;
  leftb = valueContextp->result;

  if (! getFromstack(userDatavp, marpaESLIFValuep, argni)) return 0;
  rightb = valueContextp->result;

  resultb = (leftb == rightb) ? 1 : 0;
  GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d EQUIVALENT %d : %d", (int) valueContextp->p, (int) valueContextp->q, (int) valueContextp->r, (int) leftb, (int) rightb, (int) resultb);

  marpaESLIFValueResult.contextp = MY_CONTEXT;  /* It has to be > 0 */
  marpaESLIFValueResult.representationp = NULL; /* No special representation */
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = resultb;          /* Value is a short */

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}

/*****************************************************************************/
short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  valueContext_t *valueContextp = (valueContext_t *) userDatavp;
  short           rcb;

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_CHAR:
    switch (marpaESLIFValueResultp->u.c) {
    case 'P':
      valueContextp->result = valueContextp->p;
      rcb = 1;
      break;
    case 'Q':
      valueContextp->result = valueContextp->q;
      rcb = 1;
      break;
    case 'R':
      valueContextp->result = valueContextp->r;
      rcb = 1;
      break;
    default:
      GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Unknown char '%c'", marpaESLIFValueResultp->u.c);
      rcb = 0;
    }
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    valueContextp->result = marpaESLIFValueResultp->u.b;
    rcb = 1;
    break;
  default:
    GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Unsupported type %d\n", marpaESLIFValueResultp->type);
    rcb = 0;
    break;
  }

  return rcb;
}

/*****************************************************************************/
static short getFromstack(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int indicei)
/*****************************************************************************/
{
  valueContext_t          *valueContextp = (valueContext_t *) userDatavp;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;

  marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, indicei);
  if (marpaESLIFValueResultp == NULL) {
    GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, "marpaESLIFValueResultp is NULL at stack indice %d", indicei);
    return 0;
  }

  if (! marpaESLIFValue_importb(marpaESLIFValuep, marpaESLIFValueResultp, NULL /* marpaESLIFValueResultResolvedp */)) {
    GENERICLOGGER_DEBUGF(valueContextp->genericLoggerp, "marpaESLIFValue_importb failure, %s", strerror(errno));
    return 0;
  }

  return 1;
}

