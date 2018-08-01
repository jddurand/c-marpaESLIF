#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

#define MY_CONTEXT 1 /* Any value > 0 */

#define DO_ALTERNATIVE_UNDEF(names) do {                                \
    marpaESLIFAlternative_t  marpaESLIFAlternative;                     \
                                                                        \
    marpaESLIFAlternative.lexemes        = names;                       \
    marpaESLIFAlternative.value.type     = MARPAESLIF_VALUE_TYPE_UNDEF; \
    marpaESLIFAlternative.value.contexti = MY_CONTEXT;                  \
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
    marpaESLIFAlternative.value.contexti = MY_CONTEXT;                  \
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

typedef struct valueContext {
  int p;
  int q;
  int r;
  int symboli;
  int resultb;
} valueContext_t;

static marpaESLIFValueRuleCallback_t ruleActionResolver(void*userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short action_symbol(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

const static char *grammars =
  "\n"
  "    :default ::= action => ::shift\n"
  "\n"
  "    Sentence        ::= SYMBOL                                             action => action_symbol\n"
  "                      | LPAREN Sentence RPAREN              assoc => group action => ::copy[1]\n"
  "                     || NOT Sentence                                       action => ::lua->action_not\n"
  "                     ||        Sentence        AND Sentence                action => ::lua->action_and\n"
  "                     ||        Sentence         OR Sentence                action => ::lua->action_or\n"
  "                     ||        Sentence    IMPLIES Sentence                action => ::lua->action_implies\n"
  "                      |        Sentence EQUIVALENT Sentence                action => ::lua->action_equivalent\n"
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
  "\n"
  "<luascript>\n"
  "  function returnme(x, rc)\n"
  "    print(tostring(x)..' => '..type(rc)..' '..tostring(rc))\n"
  "    io.flush()\n"
  "    return rc\n"
  "  end\n"
  "\n"
  "  function toboolean(x)\n"
  "  -- take care, in lua 0 is true\n"
  "    if (type(x) == 'boolean') then\n"
  "      rc = x\n"
  "    elseif (tonumber(x) == 0) then\n"
  "      rc = false\n"
  "    else\n"
  "      rc = true\n"
  "    end\n"
  "    return returnme('  '..type(x)..' '..tostring(x), rc)\n"
  "  end\n"
  "\n"
  "  function action_not(context, op, x)\n"
  "    return returnme('NOT '..tostring(x), not toboolean(x))\n"
  "  end\n"
  "\n"
  "  function action_and(context, x, op, y)\n"
  "    return returnme(tostring(x)..' AND '..tostring(y), toboolean(x) and toboolean(y))\n"
  "  end\n"
  "\n"
  "  function action_or(context, x, op, y)\n"
  "    return returnme(tostring(x)..' OR '..tostring(y), toboolean(x) or toboolean(y))\n"
  "  end\n"
  "\n"
  "  function action_implies(context, x, op, y)\n"
  "    if (toboolean(x)) then\n"
  "      rc = toboolean(y)\n"
  "    else\n"
  "      rc = true\n"
  "    end\n"
  "    return returnme(tostring(x)..' IMPLIES '..tostring(y), rc)\n"
  "  end\n"
  "\n"
  "  function action_equivalent(context, x, op, y)\n"
  "    if (toboolean(x) == toboolean(y)) then\n"
  "      rc = true\n"
  "    else\n"
  "      rc = false\n"
  "    end\n"
  "    return returnme(tostring(x)..' EQUIVALENT '..tostring(y), rc)\n"
  "  end\n"
  "</luascript>\n"
  ;

/* marpaESLIFValueResult transformers - in this tutorial we transform a lua boolean, that is mapped to an integer by marpaESLIF */
static short marpaESLIF_TransformChar(void *userDatavp, int contexti, char c);
static short marpaESLIF_TransformBool(void *userDatavp, int contexti, short b);

/* Transformers */
static marpaESLIFValueResultTransform_t marpaESLIFValueResultTransformDefault = {
  NULL, /* undefTransformerp */
  marpaESLIF_TransformChar,
  NULL, /* intTransformerp */
  NULL, /* longTransformerp */
  NULL, /* floatTransformerp */
  NULL, /* doubleTransformerp */
  NULL, /* ptrTransformerp */
  NULL, /* arrayTransformerp */
  NULL, /* boolTransformerp */
  marpaESLIF_TransformBool
};

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

  marpaESLIFRecognizerOption.userDatavp        = NULL;
  marpaESLIFRecognizerOption.readerCallbackp   = NULL; /* We completely externalize lexing */
  marpaESLIFRecognizerOption.disableThresholdb = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb        = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb          = 1;  /* Do newline counting */
  marpaESLIFRecognizerOption.trackb            = 0;  /* Do absolute position tracking */
  marpaESLIFRecognizerOption.bufsizl           = 0;  /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */
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

  for (valueContext.p = 0; valueContext.p < 2; valueContext.p++) {
    for (valueContext.q = 0; valueContext.q < 2; valueContext.q++) {
      for (valueContext.r = 0; valueContext.r < 2; valueContext.r++) {

        marpaESLIFValueOption.userDatavp             = &valueContext;
        marpaESLIFValueOption.ruleActionResolverp    = ruleActionResolver;
        marpaESLIFValueOption.symbolActionResolverp  = NULL; /* No symbol action resolver... Okay we use the default */
        marpaESLIFValueOption.freeActionResolverp    = NULL; /* No free action resolver... Okay if we generate no pointer */
        marpaESLIFValueOption.transformerp           = &marpaESLIFValueResultTransformDefault;
        marpaESLIFValueOption.highRankOnlyb          = 1;    /* Recommended value */
        marpaESLIFValueOption.orderByRankb           = 1;    /* Recommended value */
        marpaESLIFValueOption.ambiguousb             = 0;    /* our BNF is not ambiguous thanks to loosen operator and group association */
        marpaESLIFValueOption.maxParsesi             = 0;    /* Meaningless here since we say it is not ambiguous */
        marpaESLIFValueOption.nullb                  = 0;    /* Recommended value */

        GENERICLOGGER_INFOF(genericLoggerp, "Valuation with {P, Q, R} = {%d, %d, %d} of %s ", valueContext.p, valueContext.q, valueContext.r, examples);

        marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
        if (marpaESLIFValuep == NULL) {
          GENERICLOGGER_ERRORF(genericLoggerp, "marpaESLIFValue_newp failure, %s", strerror(errno));
          goto err;
        }

        if (marpaESLIFValue_valueb(marpaESLIFValuep) <= 0) {
          GENERICLOGGER_ERRORF(genericLoggerp, "marpaESLIFValue_valueb failure, %s", strerror(errno));
          goto err;
        }

        marpaESLIFValue_freev(marpaESLIFValuep);
        marpaESLIFValuep = NULL;

        GENERICLOGGER_INFOF(genericLoggerp, ".............. {P, Q, R} = {%d, %d, %d}... %d ", valueContext.p, valueContext.q, valueContext.r, (int) valueContext.resultb);
        if (valueContext.resultb != 0) {
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
static short marpaESLIF_TransformChar(void *userDatavp, int contexti, char c)
/*****************************************************************************/
{
  valueContext_t *valueContextp = (valueContext_t *) userDatavp;

  switch (c) {
  case 'P':
    valueContextp->symboli = valueContextp->p;
    break;
  case 'Q':
    valueContextp->symboli = valueContextp->q;
    break;
  case 'R':
    valueContextp->symboli = valueContextp->r;
    break;
  default:
    return 0;
  }

  return 1;
}

/*****************************************************************************/
static short marpaESLIF_TransformBool(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  valueContext_t *valueContextp = (valueContext_t *) userDatavp;

  valueContextp->resultb = b ? 1 : 0;

  return 1;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolver(void*userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  if (strcmp(actions, "action_symbol") == 0) {
    return action_symbol;
  }

  return NULL;
}

/*****************************************************************************/
static short action_symbol(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  valueContext_t          *valueContextp = (valueContext_t *) userDatavp;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;

  marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (marpaESLIFValueResultp == NULL) {
    return 0;
  }

  if (! marpaESLIFValue_transformb(marpaESLIFValuep, marpaESLIFValueResultp, NULL /* marpaESLIFValueResultResolvedp */)) {
    return 0;
  }

  marpaESLIFValueResult.contexti        = MY_CONTEXT;
  marpaESLIFValueResult.sizel           = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb        = 0;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT;
  marpaESLIFValueResult.u.i             = valueContextp->symboli;

  return marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
}
