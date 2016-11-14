#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericStack.h>
#include "config.h" /* For inline */
#include "marpaESLIF/internal/bootstrap_actions.h"

/* This file contain the definition of all bootstrap actions, i.e. the ESLIF grammar itself */
/* This is an example of how to use the API */

static inline short _marpaESLIF_bootstrap_dup_and_set_arrayb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, int contexti);
static        void  _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel);
static        short _marpaESLIF_bootstrap_lexemeDefaultActionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static        short _marpaESLIF_bootstrap_G1_rule_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static        short _marpaESLIF_bootstrap_G1_rule_lhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static        short _marpaESLIF_bootstrap_G1_rule_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static        short _marpaESLIF_bootstrap_G1_rule_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static        short _marpaESLIF_bootstrap_G1_rule_rhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
static inline void  _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsStackp);

typedef enum marpaESLIF_bootstrap_stack_type {
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY = 0,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY,
  MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS
} marpaESLIF_bootstrap_stack_type_t;

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_dup_and_set_arrayb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti, int contexti)
/*****************************************************************************/
{
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  void                *p           = NULL;
  short                rcb;

  if ((bytep != NULL) && (bytel > 0)) {
    p = malloc(bytel);
    if (p == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(p, bytep, bytel);
  }
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
  return _marpaESLIF_bootstrap_dup_and_set_arrayb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY);
}

/*****************************************************************************/
static void _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  switch (contexti) {
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ARRAY:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS:
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY:
    /* We are guaranteed to be called only when p is not NULL */
    free(p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS:
    _marpaESLIF_bootstrap_rhs_freev((genericStack_t *) p);
    break;
  default:
    break;
  }
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t _marpaESLIF_bootstrap_ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIF_t                  *marpaESLIFp        = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFGrammar_t           *marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
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
       if (strcmp(actions, "G1_rule_symbol_name_2") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_symbol_name_2b; }
  else if (strcmp(actions, "G1_rule_lhs")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_lhsb;           }
  else if (strcmp(actions, "G1_rule_op_declare_1")  == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_op_declare_1b;  }
  else if (strcmp(actions, "G1_rule_rhs_primary_2") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_rhs_primary_2b; }
  else if (strcmp(actions, "G1_rule_rhs")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_rhs;            }
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
  marpaESLIF_t                   *marpaESLIFp        = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFGrammar_t            *marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
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
  marpaESLIF_t                   *marpaESLIFp        = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFGrammar_t            *marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
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
static short _marpaESLIF_bootstrap_G1_rule_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <symbol name> ::= <bracketed name> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char   *bracketedNames;
  size_t  bracketedNamel;

  /* By definition a bracketed name start with '<' (one UTF-8 byte) and end with '>' (one UTF-8 byte). */

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, (void *) &bracketedNames, &bracketedNamel, NULL)) {
    return 0;
  }

  if (bracketedNamel < 2) {
    MARPAESLIF_ERRORF(marpaESLIFp, "<bracketed name> is less than two bytes: %ld", (unsigned long) bracketedNamel);
    return 0;
  }

  /* The following would work even with mutil-byte characters: we just skip the very first and very last bytes */
  return _marpaESLIF_bootstrap_dup_and_set_arrayb(userDatavp, marpaESLIFValuep, (void *) (bracketedNames + 1), bracketedNamel - 2, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_lhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* lhs ::= <symbol name> */
  void                *bytep;
  size_t               bytel;

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, &bytep, &bytel, NULL)) {
    return 0;
  }

  return _marpaESLIF_bootstrap_dup_and_set_arrayb(userDatavp, marpaESLIFValuep, bytep, bytel, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_LHS);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <op declare> ::= <op declare top grammar> */
  return marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 0);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <rhs primary> ::= <quoted string> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char   *quotedStrings;
  size_t  quotedStringl;

  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL, (void *) &quotedStrings, &quotedStringl, NULL)) {
    return 0;
  }

  if (quotedStringl < 2) {
    MARPAESLIF_ERRORF(marpaESLIFp, "<quoted string> is less than two bytes: %ld", (unsigned long) quotedStringl);
    return 0;
  }

  return _marpaESLIF_bootstrap_dup_and_set_arrayb(userDatavp, marpaESLIFValuep, (void *) (quotedStrings + 1), quotedStringl - 2, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_rhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* <rhs> ::= <rhs primary>+ */
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = marpaESLIFValue_grammarp(marpaESLIFValuep);
  marpaESLIF_t        *marpaESLIFp        = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);
  genericStack_t      *rhsStackp          = NULL;
  int                  i;
  char                *p;
  void                *bytep;
  size_t               bytel;
  short                rcb;

  GENERICSTACK_NEW(rhsStackp);
  if (GENERICSTACK_ERROR(rhsStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i++) {
    if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, i, NULL, &bytep, &bytel, NULL)) {
      return 0;
    }
    if ((bytep == NULL) || (bytel <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "An RHS primary is not set");
      goto err;
    }

    p = (char *) malloc(bytel);
    if (p == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy((void *) p, bytep, bytel);
    GENERICSTACK_PUSH_PTR(rhsStackp, p);
    if (GENERICSTACK_ERROR(rhsStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS, rhsStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_freev(rhsStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsStackp)
/*****************************************************************************/
{
  int   i;
  char *p;

  if (rhsStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(rhsStackp); i++) {
      if (GENERICSTACK_IS_PTR(rhsStackp, i)) {
        p = (char *) GENERICSTACK_GET_PTR(rhsStackp, i);
        if (p != NULL) {
          free(p);
        }
      }
    }
    GENERICSTACK_FREE(rhsStackp);
  }
}
