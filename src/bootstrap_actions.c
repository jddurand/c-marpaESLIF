#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <genericStack.h>
#include "config.h" /* For inline */
#include "marpaESLIF/internal/bootstrap_actions.h"
#include "marpaESLIF/internal/bootstrap_types.h"

/* This file contain the definition of all bootstrap actions, i.e. the ESLIF grammar itself */
/* This is an example of how to use the API */

static inline void  _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsStackp);
static inline void  _marpaESLIF_bootstrap_adverb_list_freev(genericStack_t *adverbListStackp);
static inline void  _marpaESLIF_bootstrap_adverb_list_item_freev(marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp);

static        void  _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel);

static        short _marpaESLIF_bootstrap_G1_rule_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_rule_rhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_rule_adverb_list(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_rule_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

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

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_adverb_list_freev(genericStack_t *adverbListStackp)
/*****************************************************************************/
{
  int                                      i;
  marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp;

  if (adverbListStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(adverbListStackp); i++) {
      if (GENERICSTACK_IS_PTR(adverbListStackp, i)) {
        adverbListItemp = (marpaESLIF_bootstrap_adverb_list_item_t *) GENERICSTACK_GET_PTR(adverbListStackp, i);
        _marpaESLIF_bootstrap_adverb_list_item_freev(adverbListItemp);
      }
    }
    GENERICSTACK_FREE(adverbListStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_adverb_list_item_freev(marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp)
/*****************************************************************************/
{
  if (adverbListItemp != NULL) {
    switch (adverbListItemp->type) {
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION:
      if (adverbListItemp->u.actions != NULL) {
        free(adverbListItemp->u.actions);
      }
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_AUTORANK:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR:
      if (adverbListItemp->u.separators != NULL) {
        free(adverbListItemp->u.separators);
      }
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PROPER:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RANK:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NULL_RANKING:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PRIORITY:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_PAUSE:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LATM:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NAMING:
      if (adverbListItemp->u.naming.bytep != NULL) {
        free(adverbListItemp->u.naming.bytep);
      }
      break;
    default:
      break;
    }
    free(adverbListItemp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  switch (contexti) {
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE:
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS:
    _marpaESLIF_bootstrap_rhs_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST:
    _marpaESLIF_bootstrap_adverb_list_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ACTION:
    _marpaESLIF_bootstrap_adverb_list_item_freev((marpaESLIF_bootstrap_adverb_list_item_t *) p);
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
       if (strcmp(actions, "G1_rule_op_declare_1")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_op_declare_1b;  }
  else if (strcmp(actions, "G1_rule_rhs")            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_rhs;            }
  else if (strcmp(actions, "G1_rule_adverb_list")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_adverb_list;    }
  else if (strcmp(actions, "G1_rule_action")         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_rule_action;         }
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
static short _marpaESLIF_bootstrap_G1_rule_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  /* <op declare> ::= <op declare top grammar> */

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    return 0;
  }

  return marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 0);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_rhs(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs> ::= <rhs primary>+ */
  marpaESLIF_t        *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t      *rhsStackp   = NULL;
  int                  i;
  char                *p;
  void                *bytep;
  size_t               bytel;
  short                rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

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
static short _marpaESLIF_bootstrap_G1_rule_adverb_list(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <adverb list> ::= <adverb list items> */
  marpaESLIF_t                            *marpaESLIFp      = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t                          *adverbListStackp = NULL;
  marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp  = NULL;
  int                                      i;
  short                                    rcb;

  GENERICSTACK_NEW(adverbListStackp);
  if (GENERICSTACK_ERROR(adverbListStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "adverbListStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  if (! nullableb) {
    for (i = arg0i; i <= argni; i++) {
      if (! marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, i, NULL, (void **) &adverbListItemp, NULL)) {
        goto err;
      }
      if (! marpaESLIFValue_stack_forgetb(marpaESLIFValuep, i)) {
        goto err;
      }
      if (adverbListItemp != NULL) {
        /* NULL is the eventual null adverb - not needed for further processing */
        GENERICSTACK_PUSH_PTR(adverbListStackp, (void *) adverbListItemp);
        if (GENERICSTACK_ERROR(adverbListStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "adverbListStackp push failure, %s", strerror(errno));
          goto err;
        }
        adverbListItemp = NULL; /* adverbListItemp is now in adverbListStackp */
      }
    }
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST, adverbListStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_adverb_list_item_freev(adverbListItemp);
  _marpaESLIF_bootstrap_adverb_list_freev(adverbListStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'action' '=>' <action name> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char         *actions = NULL;
  void         *p;
  size_t        sizel;
  short         rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* action is the result of ::shift, i.e. a lexeme in any case  */
  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, argni, NULL /* contextip */, &p, &sizel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have valid information */
  if ((p == NULL) || (sizel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {%p,%ld}", argni, p, (unsigned long) sizel);
    goto err;
  }
  /* This is an ASCII thingy per-def */
  actions = (char *) malloc(sizel+1);
  if (actions == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(actions, p, sizel);
  actions[sizel] = '\0';
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ACTION, actions, 0 /* shallowb */)) {
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  if (actions != NULL) {
    free(actions);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_G1_adverb_item(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /*
    Normally this is a no-op
    <adverb item>                  ::= action
                                     | autorank
                                     | <left association>
                                     | <right association>
                                     | <group association>
                                     | <separator specification>
                                     | <proper specification>
                                     | <rank specification>
                                     | <null ranking specification>
                                     | <priority specification>
                                     | <pause specification>
                                     | <latm specification>
                                     | naming
                                     | <null adverb>
  */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  void         *p;
  short         rcb;
  int           contexti;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* adverb item is just a shallow copy of its single RHS */
  if (! marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, arg0i, &contexti, &p, NULL)) {
    goto err;
  }
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, contexti, p, 1 /* shallowb */)) {
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_01(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_02(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_03(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_04(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_05(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_06(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_07(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_08(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_09(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_10(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_11(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_12(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_13(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_rule_adverb_item_14(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_G1_adverb_item(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb);
}

