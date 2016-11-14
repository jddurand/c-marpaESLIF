#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "marpaESLIF/internal/bootstrap_actions.h"

/* This file contain the definition of all bootstrap actions, i.e. the ESLIF grammar itself */
/* This is an example of how to use the API */

static short _marpaESLIF_bootstrap_transferLexemeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, int contexti);
static void  _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p);
static short _marpaESLIF_bootstrap_lexemeDefaultActionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static short _marpaESLIF_bootstrap_G1_rule_symbol_name_2(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static short _marpaESLIF_bootstrap_G1_rule_lhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static short _marpaESLIF_bootstrap_G1_rule_op_declare_1(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static short _marpaESLIF_bootstrap_G1_rule_rhs_primary_2(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);

typedef enum marpaESLIF_bootstrap_stack_type {
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_LEXEME = 0,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY
} marpaESLIF_bootstrap_stack_type_t;

/*****************************************************************************/
static short _marpaESLIF_bootstrap_transferLexemeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, int contexti)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp    = marpaESLIFValue_grammarp(marpaESLIFValuep);
  marpaESLIF_t        *marpaESLIFp           = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);
  void                *p                     = NULL;
  short                rcb;

  if ((bytep == NULL) || (bytel <= 0)) {
    /* This can never happen, though */
    MARPAESLIF_ERRORF(marpaESLIFp, "{bytep=%p,bytel=%ld} is not allowed", bytep, (unsigned long) bytel);
    goto err;
  }

  p = malloc(bytel);
  if (p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(p, bytep, bytel);
  if (! marpaESLIFValue_stack_set_arrayb(marpaESLIFValuep, resulti, contexti, p, bytel, 0)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  if (p != NULL) {
    free(p);
  }
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_lexemeDefaultActionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_transferLexemeb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_LEXEME);
}

/*****************************************************************************/
static void _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;

  switch (contexti) {
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_LEXEME:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY:
    free(p);
    break;
  default:
    break;
  }
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t _marpaESLIF_bootstrap_ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFGrammar_t           *marpaESLIFGrammarp    = marpaESLIFValue_grammarp(marpaESLIFValuep);
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);
  marpaESLIFValueRuleCallback_t  marpaESLIFValueRuleCallbackp;
  int                            grammari;
  int                            leveli;

  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarib failure");
    goto err;
  }
  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarp failure");
    goto err;
  }
  if (! marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL /* descp */)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFGrammar_leveli_by_grammarb failure");
    goto err;
  }
  /* We have only one level here */
  if (leveli != 0) {
    MARPAESLIF_ERRORF(marpaESLIFp, "leveli is %d", leveli);
    goto err;
  }
  /* TO DO */
       if (strcmp(actions, "G1_rule_symbol_name_2") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_symbol_name_2; }
  else if (strcmp(actions, "G1_rule_lhs")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_lhs;           }
  else if (strcmp(actions, "G1_rule_op_declare_1")  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_op_declare_1;  }
  else if (strcmp(actions, "G1_rule_rhs_primary_2") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_rhs_primary_2; }
  else
  {
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueRuleCallbackp = NULL;
 done:
  return marpaESLIFValueRuleCallbackp;
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t _marpaESLIF_bootstrap_symbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFValue_grammarp(marpaESLIFValuep);
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);
  marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolCallbackp;
  int                             grammari;
  int                             leveli;

  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarib failure");
    goto err;
  }
  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarp failure");
    goto err;
  }
  if (! marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL /* descp */)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFGrammar_leveli_by_grammarb failure");
    goto err;
  }
  /* We have only one level here */
  if (leveli != 0) {
    MARPAESLIF_ERRORF(marpaESLIFp, "leveli is %d", leveli);
    goto err;
  }

  if (strcmp(actions, "_marpaESLIF_bootstrap_lexemeDefaultActionb") == 0) {
    marpaESLIFValueSymbolCallbackp = _marpaESLIF_bootstrap_lexemeDefaultActionb;
  } else {
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueSymbolCallbackp = NULL;
 done:
  return marpaESLIFValueSymbolCallbackp;
}

/*****************************************************************************/
static marpaESLIFValueFreeCallback_t _marpaESLIF_bootstrap_freeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFValue_grammarp(marpaESLIFValuep);
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);
  marpaESLIFValueFreeCallback_t   marpaESLIFValueFreeCallbackp;
  int                             grammari;
  int                             leveli;

  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarib failure");
    goto err;
  }
  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFValue_grammarp failure");
    goto err;
  }
  if (! marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL /* descp */)) {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFGrammar_leveli_by_grammarb failure");
    goto err;
  }
  /* We have only one level here */
  if (leveli != 0) {
    MARPAESLIF_ERRORF(marpaESLIFp, "leveli is %d", leveli);
    goto err;
  }

  if (strcmp(actions, "_marpaESLIF_bootstrap_freeDefaultActionv") == 0) {
    marpaESLIFValueFreeCallbackp = _marpaESLIF_bootstrap_freeDefaultActionv;
  } else {
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueFreeCallbackp = NULL;
 done:
  return marpaESLIFValueFreeCallbackp;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_symbol_name_2(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <symbol name>   ::= <bracketed name> */
  void                *bytep;
  size_t               bytel;
  short                rcb;

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, &bytep, &bytel, NULL)) {
    goto err;
  }

  rcb = _marpaESLIF_bootstrap_transferLexemeb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME);
  goto done;

 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_lhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <symbol name>   ::= <bracketed name> */
  void                *bytep;
  size_t               bytel;
  short                rcb;

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, &bytep, &bytel, NULL)) {
    goto err;
  }

  rcb = _marpaESLIF_bootstrap_transferLexemeb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS);
  goto done;

 err:
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_op_declare_1(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <op declare> ::= <op declare top grammar> */
  return marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 0);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_rhs_primary_2(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <rhs primary> ::= <quoted string> */
  void                *bytep;
  size_t               bytel;
  short                rcb;

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, &bytep, &bytel, NULL)) {
    goto err;
  }

  rcb = _marpaESLIF_bootstrap_transferLexemeb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY);
  goto done;

 err:
  rcb = 0;
 done:
  return rcb;
}
