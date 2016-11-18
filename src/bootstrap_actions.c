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

static inline void _marpaESLIF_bootstrap_rhs_primary_freev(marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp);
static inline void _marpaESLIF_bootstrap_symbol_name_and_reference_freev(marpaESLIF_bootstrap_symbol_name_and_reference_t *symbolNameAndReferencep);
static inline void _marpaESLIF_bootstrap_utf_string_freev(marpaESLIF_bootstrap_utf_string_t *stringp);
static inline void _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsPrimaryStackp);
static inline void _marpaESLIF_bootstrap_adverb_list_item_freev(marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp);
static inline void _marpaESLIF_bootstrap_adverb_list_items_freev(genericStack_t *adverbListItemStackp);
static inline void _marpaESLIF_bootstrap_alternative_freev(marpaESLIF_bootstrap_alternative_t *alternativep);
static inline void _marpaESLIF_bootstrap_alternatives_freev(genericStack_t *alternativeStackp);
static inline void _marpaESLIF_bootstrap_priorities_freev(genericStack_t *alternativesStackp);
static inline void _marpaESLIF_bootstrap_single_symbol_freev(marpaESLIF_bootstrap_single_symbol_t *singleSymbolp);
static inline void _marpaESLIF_bootstrap_grammar_reference_freev(marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep);

static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_check_grammarp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, marpaESLIF_bootstrap_utf_string_t *stringp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_meta_by_namep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *asciinames);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_search_terminal_by_descriptionp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *stringp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_quotedStringp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *quotedStringp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_regexp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *regexp);
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_single_symbol_t *singleSymbolp);

static        void  _marpaESLIF_bootstrap_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel);

static        short _marpaESLIF_bootstrap_G1_action_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_autorankb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_left_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_right_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_group_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_separatorb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_rhs_primary_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_alternativeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_alternativesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_prioritiesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_priority_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_single_symbol_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_single_symbol_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_single_symbol_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_grammar_reference_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static        short _marpaESLIF_bootstrap_G1_action_grammar_reference_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_rhs_primary_freev(marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp)
/*****************************************************************************/
{
  if (rhsPrimaryp != NULL) {
    switch (rhsPrimaryp->type) {
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL:
      _marpaESLIF_bootstrap_single_symbol_freev(rhsPrimaryp->u.singleSymbolp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_QUOTED_STRING:
      _marpaESLIF_bootstrap_utf_string_freev(rhsPrimaryp->u.quotedStringp);
      break;
    case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_NAME_AND_REFERENCE:
      _marpaESLIF_bootstrap_symbol_name_and_reference_freev(rhsPrimaryp->u.symbolNameAndReferencep);
      break;
    default:
      break;
    }
    free(rhsPrimaryp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_symbol_name_and_reference_freev(marpaESLIF_bootstrap_symbol_name_and_reference_t *symbolNameAndReferencep)
/*****************************************************************************/
{
  if (symbolNameAndReferencep != NULL) {
    if (symbolNameAndReferencep->symbols != NULL) {
      free(symbolNameAndReferencep->symbols);
    }
    _marpaESLIF_bootstrap_grammar_reference_freev(symbolNameAndReferencep->grammarReferencep);
    free(symbolNameAndReferencep);
  }
}

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_utf_string_freev(marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  if (stringp != NULL) {
    if (stringp->bytep != NULL) {
      free(stringp->bytep);
    }
    if (stringp->modifiers != NULL) {
      free(stringp->modifiers);
    }
    free(stringp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_rhs_freev(genericStack_t *rhsPrimaryStackp)
/*****************************************************************************/
{
  int i;

  if (rhsPrimaryStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(rhsPrimaryStackp); i++) {
      if (GENERICSTACK_IS_PTR(rhsPrimaryStackp, i)) {
        _marpaESLIF_bootstrap_rhs_primary_freev((marpaESLIF_bootstrap_rhs_primary_t *) GENERICSTACK_GET_PTR(rhsPrimaryStackp, i));
      }
    }
    GENERICSTACK_FREE(rhsPrimaryStackp);
  }
}

/*****************************************************************************/
static inline void  _marpaESLIF_bootstrap_adverb_list_items_freev(genericStack_t *adverbListItemStackp)
/*****************************************************************************/
{
  int i;

  if (adverbListItemStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(adverbListItemStackp); i++) {
      if (GENERICSTACK_IS_PTR(adverbListItemStackp, i)) {
        _marpaESLIF_bootstrap_adverb_list_item_freev((marpaESLIF_bootstrap_adverb_list_item_t *) GENERICSTACK_GET_PTR(adverbListItemStackp, i));
      }
    }
    GENERICSTACK_FREE(adverbListItemStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_alternative_freev(marpaESLIF_bootstrap_alternative_t *alternativep)
/*****************************************************************************/
{
  if (alternativep != NULL) {
    _marpaESLIF_bootstrap_rhs_freev(alternativep->rhsPrimaryStackp);
    _marpaESLIF_bootstrap_adverb_list_items_freev(alternativep->adverbListItemStackp);
    free(alternativep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_alternatives_freev(genericStack_t *alternativeStackp)
/*****************************************************************************/
{
  int i;

  if (alternativeStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(alternativeStackp); i++) {
      if (GENERICSTACK_IS_PTR(alternativeStackp, i)) {
        _marpaESLIF_bootstrap_alternative_freev((marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, i));
      }
    }
    GENERICSTACK_FREE(alternativeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_priorities_freev(genericStack_t *alternativesStackp)
/*****************************************************************************/
{
  int i;

  if (alternativesStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(alternativesStackp); i++) {
      if (GENERICSTACK_IS_PTR(alternativesStackp, i)) {
        _marpaESLIF_bootstrap_alternatives_freev((genericStack_t *) GENERICSTACK_GET_PTR(alternativesStackp, i));
      }
    }
    GENERICSTACK_FREE(alternativesStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_single_symbol_freev(marpaESLIF_bootstrap_single_symbol_t *singleSymbolp)
/*****************************************************************************/
{
  if (singleSymbolp != NULL) {
    switch (singleSymbolp->type) {
    case MARPAESLIF_SINGLE_SYMBOL_TYPE_SYMBOL:
      if (singleSymbolp->u.symbols != NULL) {
        free(singleSymbolp->u.symbols);
      }
      break;
    case MARPAESLIF_SINGLE_SYMBOL_TYPE_CHARACTER_CLASS:
      _marpaESLIF_bootstrap_utf_string_freev(singleSymbolp->u.characterClassp);
      break;
    case MARPAESLIF_SINGLE_SYMBOL_TYPE_REGULAR_EXPRESSION:
      _marpaESLIF_bootstrap_utf_string_freev(singleSymbolp->u.regularExpressionp);
      break;
    default:
      break;
    }
    free(singleSymbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_bootstrap_grammar_reference_freev(marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep)
/*****************************************************************************/
{
  if (grammarReferencep != NULL) {
    switch (grammarReferencep->type) {
    case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING:
      _marpaESLIF_bootstrap_utf_string_freev(grammarReferencep->u.quotedStringp);
      break;
    default:
      break;
    }
    free(grammarReferencep);
  }
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_check_grammarp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_grammar_t *grammarp = NULL;
  marpaESLIF_string_t   desc;
  marpaESLIF_string_t  *descp = NULL;

  if (marpaESLIFGrammarp->grammarStackp == NULL) {
    /* Make sure that the stack of grammars exist */
    GENERICSTACK_NEW(marpaESLIFGrammarp->grammarStackp);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
      goto err;
    }
  }

  if (stringp != NULL) {
    desc.bytep          = stringp->bytep;
    desc.bytel          = stringp->bytel;
    desc.encodingasciis = NULL;
    desc.asciis         = NULL;
    descp = &desc;
  }
  grammarp = _marpaESLIFGrammar_grammar_findp(marpaESLIFGrammarp, leveli, descp);
  if (grammarp == NULL) {
    /* Create it */
    grammarp = _marpaESLIF_grammar_newp(marpaESLIFp,
                                        NULL /* marpaWrapperGrammarOptionp */,
                                        leveli,
                                        NULL /* descEncodings */,
                                        NULL /* descs */,
                                        0 /* descl */,
                                        0 /* latmb */,
                                        NULL /* defaultSymbolActions */,
                                        NULL /* defaultRuleActions */,
                                        NULL /* defaultFreeActions */,
                                        NULL /* defaultDiscardEvents */,
                                        0 /* defaultDiscardEventb */);
    if (grammarp == NULL) {
      goto err;
    }
    GENERICSTACK_SET_PTR(marpaESLIFGrammarp->grammarStackp, grammarp, leveli);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp)) {
      _marpaESLIF_grammar_freev(grammarp);
      grammarp = NULL;
    }
  }

  /* Note: grammarp may be NULL here */
  goto done;
 err:
  grammarp = NULL;
 done:
  return grammarp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_meta_by_namep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *asciinames)
/*****************************************************************************/
{
  marpaESLIF_symbol_t *symbolp = NULL;
  marpaESLIF_meta_t   *metap   = NULL;
  marpaESLIF_symbol_t *symbol_i_p;
  int                  i;

  for (i = 0; i < GENERICSTACK_USED(grammarp->symbolStackp); i++) {
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, i)) {
      continue;
    }
    symbol_i_p = GENERICSTACK_GET_PTR(grammarp->symbolStackp, i);
    if (symbol_i_p->type != MARPAESLIF_SYMBOL_TYPE_META) {
      continue;
    }
    if (strcmp(symbol_i_p->u.metap->asciinames, asciinames) == 0) {
      symbolp = symbol_i_p;
      break;
    }
  }

  if (symbolp == NULL) {
    metap = _marpaESLIF_meta_newp(marpaESLIFp, grammarp, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE, asciinames, NULL /* descEncodings */, NULL /* descs */, 0 /* descl */);
    if (metap == NULL) {
      goto err;
    }
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }
    symbolp->type              = MARPAESLIF_SYMBOL_TYPE_META;
    symbolp->startb            = 0; /* TO DO */
    symbolp->discardb          = 0; /* TO DO */
    symbolp->u.metap           = metap;
    symbolp->idi               = metap->idi;
    symbolp->descp             = metap->descp;
    symbolp->lookupLevelDeltai = 0; /* This method is inserting symbols in the same grammar */
    metap = NULL; /* metap is now in symbolp */

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
  }
  goto done;
  
 err:
  _marpaESLIF_meta_freev(metap);
  _marpaESLIF_symbol_freev(symbolp);
  symbolp = NULL;
 done:
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_search_terminal_by_descriptionp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_symbol_t *symbolp = NULL;
  marpaESLIF_symbol_t *symbol_i_p;
  int                  i;
  marpaESLIF_string_t  desc;
  char                *modifiers;

  desc.bytep          = stringp->bytep;
  desc.bytel          = stringp->bytel;
  desc.encodingasciis = NULL;
  desc.asciis         = NULL;

  modifiers = stringp->modifiers;

  for (i = 0; i < GENERICSTACK_USED(grammarp->symbolStackp); i++) {
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, i)) {
      continue;
    }
    symbol_i_p = GENERICSTACK_GET_PTR(grammarp->symbolStackp, i);
    if (symbol_i_p->type != MARPAESLIF_SYMBOL_TYPE_TERMINAL) {
      continue;
    }
    if (! _marpaESLIF_string_eqb(symbol_i_p->u.terminalp->descp, &desc)) {
      continue;
    }
    /* Modifiers ? */
    if ((modifiers != NULL) && (symbolp->u.terminalp->modifiers == NULL)) {
      continue;
    }
    if ((modifiers == NULL) && (symbolp->u.terminalp->modifiers != NULL)) {
      continue;
    }
    /* Either both are NULL, or both are not NULL */
    /* We do try to re-evaluate the modifiers. Even if at the end this would give the same */
    /* PCRE2 option, at mos this will generate a new terminal technically equivalent */
    if (modifiers != NULL) {
      if (strcmp(modifiers, symbolp->u.terminalp->modifiers) != 0) {
        continue;
      }
    }
    /* Got it */
    symbolp = symbol_i_p;
    break;
  }

  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t  *_marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_terminal_type_t terminalType, marpaESLIF_bootstrap_utf_string_t *stringp)
/*****************************************************************************/
{
  marpaESLIF_symbol_t   *symbolp   = NULL;
  marpaESLIF_terminal_t *terminalp = NULL;

  symbolp = _marpaESLIF_bootstrap_search_terminal_by_descriptionp(marpaESLIFp, grammarp, stringp);

  if (symbolp == NULL) {
    terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
                                          grammarp,
                                          MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
                                          NULL /* descEncodings */,
                                          NULL /* descs */,
                                          0 /* descl */,
                                          terminalType,
                                          stringp->modifiers,
                                          stringp->bytep,
                                          stringp->bytel,
                                          NULL /* testFullMatchs */,
                                          NULL /* testPartialMatchs */);
    if (terminalp == NULL) {
      goto err;
    }
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }
    symbolp->type        = MARPAESLIF_SYMBOL_TYPE_TERMINAL;
    symbolp->u.terminalp = terminalp;
    symbolp->idi         = terminalp->idi;
    symbolp->descp       = terminalp->descp;
    terminalp = NULL; /* terminalp is now in symbolp */
      
    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  _marpaESLIF_symbol_freev(symbolp);
  symbolp = NULL;
 done:
  return symbolp;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_quotedStringp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *quotedStringp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFp, grammarp, MARPAESLIF_TERMINAL_TYPE_STRING, quotedStringp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_regexp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_utf_string_t *regexp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_check_terminal_by_typep(marpaESLIFp, grammarp, MARPAESLIF_TERMINAL_TYPE_REGEX, regexp);
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, marpaESLIF_bootstrap_single_symbol_t *singleSymbolp)
/*****************************************************************************/
{
  marpaESLIF_symbol_t *symbolp = NULL;

  switch (singleSymbolp->type) {
  case MARPAESLIF_SINGLE_SYMBOL_TYPE_SYMBOL:
    symbolp = _marpaESLIF_bootstrap_check_meta_by_namep(marpaESLIFp, grammarp, singleSymbolp->u.symbols);
    break;
  case MARPAESLIF_SINGLE_SYMBOL_TYPE_CHARACTER_CLASS:
    symbolp = _marpaESLIF_bootstrap_check_regexp(marpaESLIFp, grammarp, singleSymbolp->u.characterClassp);
    break;
  case MARPAESLIF_SINGLE_SYMBOL_TYPE_REGULAR_EXPRESSION:
    symbolp = _marpaESLIF_bootstrap_check_regexp(marpaESLIFp, grammarp, singleSymbolp->u.regularExpressionp);
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported singleSymbolp->Type = %d", singleSymbolp->type);
    goto err;
  }

  goto done;
  
 err:
  _marpaESLIF_symbol_freev(symbolp);
  symbolp = NULL;

 done:
  return symbolp;
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
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION:
      break;
    case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION:
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
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME:
    free(p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY:
    _marpaESLIF_bootstrap_rhs_primary_freev((marpaESLIF_bootstrap_rhs_primary_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS:
    _marpaESLIF_bootstrap_rhs_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION:
    free(p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_AUTORANK:
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION:
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION:
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION:
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR:
    free(p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS:
    _marpaESLIF_bootstrap_adverb_list_items_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE:
    _marpaESLIF_bootstrap_alternative_freev((marpaESLIF_bootstrap_alternative_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVES:
    _marpaESLIF_bootstrap_alternatives_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_PRIORITIES:
    _marpaESLIF_bootstrap_priorities_freev((genericStack_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL:
    _marpaESLIF_bootstrap_single_symbol_freev((marpaESLIF_bootstrap_single_symbol_t *) p);
    break;
  case MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE:
    _marpaESLIF_bootstrap_grammar_reference_freev((marpaESLIF_bootstrap_grammar_reference_t *) p);
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
       if (strcmp(actions, "G1_action_op_declare_1")        == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_op_declare_1b;        }
  else if (strcmp(actions, "G1_action_rhs")                 == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhsb;                 }
  else if (strcmp(actions, "G1_action_adverb_list_items")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb;   }
  else if (strcmp(actions, "G1_action_action")              == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_actionb;              }
  else if (strcmp(actions, "G1_action_autorank")            == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_autorankb;            }
  else if (strcmp(actions, "G1_action_left_association")    == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_left_associationb;    }
  else if (strcmp(actions, "G1_action_right_association")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_right_associationb;   }
  else if (strcmp(actions, "G1_action_group_association")   == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_group_associationb;   }
  else if (strcmp(actions, "G1_action_separator")           == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_separatorb;           }
  else if (strcmp(actions, "G1_action_symbol_name_2")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_symbol_name_2b;       }
  else if (strcmp(actions, "G1_action_rhs_primary_1")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_primary_1b;       }
  else if (strcmp(actions, "G1_action_rhs_primary_2")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_primary_2b;       }
  else if (strcmp(actions, "G1_action_rhs_primary_3")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_rhs_primary_3b;       }
  else if (strcmp(actions, "G1_action_alternative")         == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_alternativeb;         }
  else if (strcmp(actions, "G1_action_alternatives")        == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_alternativesb;        }
  else if (strcmp(actions, "G1_action_priorities")          == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_prioritiesb;          }
  else if (strcmp(actions, "G1_action_priority_rule")       == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_priority_ruleb;       }
  else if (strcmp(actions, "G1_action_single_symbol_1")     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_single_symbol_1b;     }
  else if (strcmp(actions, "G1_action_single_symbol_2")     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_single_symbol_2b;     }
  else if (strcmp(actions, "G1_action_single_symbol_3")     == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_single_symbol_3b;     }
  else if (strcmp(actions, "G1_action_grammar_reference_1") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_grammar_reference_1b; }
  else if (strcmp(actions, "G1_action_grammar_reference_2") == 0) { marpaESLIFValueRuleCallbackp = _marpaESLIF_bootstrap_G1_action_grammar_reference_2b; }
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
static short _marpaESLIF_bootstrap_G1_action_symbol_name_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <symbol name>  ::= <bracketed name> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char         *barenames   = NULL;
  short         arrayb;
  char         *asciis; /* bare name is only ASCII letters as per the grammar */
  size_t        asciil;
  short         rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* Per def, because of the ::shift default action, <bracketed name> is of type array since this is a lexeme */
  if (! _marpaESLIFValue_stack_is_arrayb(marpaESLIFValuep, arg0i, &arrayb)) {
    goto err;
  }
  if (! arrayb) {
    MARPAESLIF_ERRORF(marpaESLIFp, "RHS No %d is of type ARRAY", arg0i);
    goto err;
  }
  if (! _marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &asciis, &asciil, NULL /* shallowbp */)) {
    goto err;
  }
  if ((asciis == NULL) || (asciil <= 0)) {
    /* Should never happen as per the grammar */
    MARPAESLIF_ERROR(marpaESLIFp, "Null bare name");
    goto err;
  }
  if (asciil < 2) {
    /* Should never happen neither as per the grammar */
    MARPAESLIF_ERRORF(marpaESLIFp, "Length of bare name is %ld", (unsigned long) asciil);
    goto err;
  }
  /* We just remove the '<' and '>' around... */
  barenames = (char *) malloc(asciil - 2 + 1);
  if (barenames == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  strncpy(barenames, asciis + 1, asciil - 2);
  barenames[asciil - 2] = '\0';

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SYMBOL_NAME, barenames, 0 /* shallowb */)) {
    goto err;
  }

  /* You will note that we are coherent will ALL the other <symbol name> rules: the outcome is an ASCII NUL terminated string pointer */
  rcb = 1;
  goto done;
 err:
  if (barenames != NULL) {
    free(barenames);
  }
  rcb = 0;
 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_op_declare_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  /* <op declare> ::= <op declare top grammar> */

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    return 0;
  }

  return marpaESLIFValue_stack_set_intb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_OP_DECLARE, 0 /* ::= is level No 0 */);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs> ::= <rhs primary>+ */
  marpaESLIF_t                       *marpaESLIFp      = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t                     *rhsPrimaryStackp = NULL;
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp      = NULL;
  int                                 i;
  short                rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  GENERICSTACK_NEW(rhsPrimaryStackp);
  if (GENERICSTACK_ERROR(rhsPrimaryStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsPrimaryStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i++) {
    if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &rhsPrimaryp, NULL /* shallowbp */)) {
      return 0;
    }
    if (rhsPrimaryp == NULL) {
      MARPAESLIF_ERROR(marpaESLIFp, "An RHS primary is not set");
      goto err;
    }

    GENERICSTACK_PUSH_PTR(rhsPrimaryStackp, rhsPrimaryp);
    if (GENERICSTACK_ERROR(rhsPrimaryStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp push failure, %s", strerror(errno));
      goto err;
    }
    rhsPrimaryp = NULL; /* rhsPrimaryp is now in rhsPrimaryStackp */
  }
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS, rhsPrimaryStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  _marpaESLIF_bootstrap_rhs_freev(rhsPrimaryStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_adverb_list_itemsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <adverb list items> ::= <adverb item>* */
  marpaESLIF_t                            *marpaESLIFp          = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t                          *adverbListItemStackp = NULL;
  marpaESLIF_bootstrap_adverb_list_item_t *adverbListItemp      = NULL;
  char                                    *actions              = NULL;
  short                                    left_associationb    = 0;
  short                                    right_associationb   = 0;
  short                                    group_associationb   = 0;
  char                                    *separators           = NULL;
  short                                    autorankb;
  int                                      contexti;
  int                                      i;
  short                                    rcb;

  GENERICSTACK_NEW(adverbListItemStackp);
  if (GENERICSTACK_ERROR(adverbListItemStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* In theory, if we are called, this is because there is something on the stack */
  /* In any case, this is okay to have an empty stack -; */
  if (! nullableb) {
    for (i = arg0i; i <= argni; i++) {
      if (! marpaESLIFValue_stack_get_contextb(marpaESLIFValuep, i, &contexti)) {
        goto err;
      }

      adverbListItemp = (marpaESLIF_bootstrap_adverb_list_item_t *) malloc(sizeof(marpaESLIF_bootstrap_adverb_list_item_t));
      if (adverbListItemp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      adverbListItemp->type = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_NA;

      switch (contexti) {
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION:
        if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &actions, NULL /* shallowbp */)) {
          goto err;
        }
        if (actions == NULL) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item action is NULL");
          goto err;
        }
        adverbListItemp->type      = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION;
        adverbListItemp->u.actions = actions;
        actions = NULL; /* actions is now in adverbListItemp */
        break;
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_AUTORANK:
        if (! marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, i, NULL /* contextip */, &autorankb)) {
          goto err;
        }
        adverbListItemp->type        = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_AUTORANK;
        adverbListItemp->u.autorankb = autorankb;
        break;
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION:
        if (! marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, i, NULL /* contextip */, &left_associationb)) {
          goto err;
        }
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION;
        adverbListItemp->u.left_associationb = left_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION:
        if (! marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, i, NULL /* contextip */, &right_associationb)) {
          goto err;
        }
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION;
        adverbListItemp->u.right_associationb = right_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION:
        if (! marpaESLIFValue_stack_get_shortb(marpaESLIFValuep, i, NULL /* contextip */, &group_associationb)) {
          goto err;
        }
        adverbListItemp->type                = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION;
        adverbListItemp->u.group_associationb = group_associationb;
        break;
      case MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR:
        if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &separators, NULL /* shallowbp */)) {
          goto err;
        }
        if (separators == NULL) { /* Not possible */
          MARPAESLIF_ERROR(marpaESLIFp, "Adverb list item separator is NULL");
          goto err;
        }
        adverbListItemp->type         = MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR;
        adverbListItemp->u.separators = separators;
        separators = NULL; /* separators is now in adverbListItemp */
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported adverb list item type %d", contexti);
        goto err;
      }

      GENERICSTACK_PUSH_PTR(adverbListItemStackp, (void *) adverbListItemp);
      if (GENERICSTACK_ERROR(adverbListItemStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp push failure, %s", strerror(errno));
        goto err;
      }
      adverbListItemp = NULL; /* adverbListItemp is now in adverbListItemStackp */
    }
  }

  /* It is possible to do a sanity check here */
  if (left_associationb +  right_associationb + group_associationb > 1) {
    MARPAESLIF_ERROR(marpaESLIFp, "assoc => left, assoc => right and assoc => group are mutually exclusive");
    goto err;
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_LIST_ITEMS, adverbListItemStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  if (actions != NULL) {
    free(actions);
  }
  if (separators != NULL) {
    free(separators);
  }
  _marpaESLIF_bootstrap_adverb_list_item_freev(adverbListItemp);
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* action ::= 'action' '=>' <action name> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char         *actions     = NULL;
  short         rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <action name> is the result of ::ascii, i.e. a ptr in any case  */
  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, argni, NULL /* contextip */, (void **) &actions, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have no action in this case */
  if (actions == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "action at indice %d returned NULL", argni);
    goto err;
  }
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_ACTION, actions, 0 /* shallowb */)) {
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
static short _marpaESLIF_bootstrap_G1_action_separatorb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* 'separator' '=>' <single symbol>> */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char         *separators  = NULL;
  short         rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <single symbol> is the result of ::ascii, i.e. a ptr in any case  */
  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, argni, NULL /* contextip */, (void **) &separators, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have no action in this case */
  if (separators == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "separator at indice %d returned NULL", argni);
    goto err;
  }
  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_SEPARATOR, separators, 0 /* shallowb */)) {
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  if (separators != NULL) {
    free(separators);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_autorankb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* autorank ::= 'autorank' '=>' boolean */
  marpaESLIF_t *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  short         autorankb;
  short         rcb;
  char         *bytep;
  size_t        bytel;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* <boolean> can be either '0' or '1' - in UTF-8 - ASCII compatible  */
  if (! marpaESLIFValue_stack_get_arrayb(marpaESLIFValuep, argni, NULL /* contextip */, (void **) &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have no action in this case */
  if (bytep == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "boolean lexeme is NULL");
    goto err;
  }
  autorankb = (bytep[0] == '1');

  if (! marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_AUTORANK, autorankb)) {
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
static short _marpaESLIF_bootstrap_G1_action_left_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <left association> ::= 'assoc' '=>' 'left' */
  return marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_LEFT_ASSOCIATION, 1);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_right_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <right association> ::= 'assoc' '=>' 'right' */
  return marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_RIGHT_ASSOCIATION, 1);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_group_associationb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <group association> ::= 'assoc' '=>' 'group' */
  return marpaESLIFValue_stack_set_shortb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ADVERB_ITEM_GROUP_ASSOCIATION, 1);
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_primary_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs primary> ::= <single symbol> */
  /* <single symbol> is on the stack, typed MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL */
  marpaESLIF_bootstrap_rhs_primary_t   *rhsPrimaryp   = NULL;
  marpaESLIF_t                         *marpaESLIFp   = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp = NULL;
  short                                 rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &singleSymbolp, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have valid information */
  if (singleSymbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_ptrb at indice %d returned NULL", argni);
    goto err;
  }

  /* Make that an rhs primary structure */
  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (rhsPrimaryp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type            = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL;
  rhsPrimaryp->u.singleSymbolp = singleSymbolp;
  singleSymbolp = NULL; /* singleSymbolp is now in rhsPrimaryp */

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  if (singleSymbolp != NULL) {
    _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_primary_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs primary> ::= <quoted string> */
  marpaESLIF_bootstrap_rhs_primary_t *rhsPrimaryp = NULL;
  marpaESLIF_t                       *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFRecognizer_t             *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  char                               *modifiers             = NULL;
  marpaESLIFGrammar_t                 marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t             marpaESLIFValueResult;
  marpaESLIF_matcher_value_t          rci;
  void                               *bytep = NULL;
  size_t                              bytel;
  short                               rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* action is the result of ::shift, i.e. a lexeme in any case  */
  if (! marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have valid information */
  if ((bytep == NULL) || (bytel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {%p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, marpaESLIFp->stringModifiersp, bytep, bytel, 1 /* eofb */, &rci, &marpaESLIFValueResult)) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a character class it is something like ":xxxxx" */
    if (marpaESLIFValueResult.sizel <= 0) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
    modifiers = (char *) malloc(marpaESLIFValueResult.sizel + 1);
    if (modifiers == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(modifiers, marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
    modifiers[marpaESLIFValueResult.sizel] = '\0';
    free(marpaESLIFValueResult.u.p);
  } else {
    /* Because we use this value just below */
    marpaESLIFValueResult.sizel = 0;
  }

  /* Make that an rhs primary structure */
  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (rhsPrimaryp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA;

  rhsPrimaryp->u.quotedStringp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (rhsPrimaryp->u.quotedStringp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rhsPrimaryp->type                       = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_QUOTED_STRING;
  rhsPrimaryp->u.quotedStringp->modifiers = modifiers;
  /* We leave the quotes because terminal_newp(), in case of a STRING, remove itself the surrounding characters */
  /* In contrary to regexps, because [] in a character class are not to be removed, while // in an explicit regexp */
  /* are to be removed. Therefore, terminal_newp() to be called with regexp *content* in regexp mode, while it can */
  /* handle totally the string case. */
  rhsPrimaryp->u.quotedStringp->bytep     = bytep;
  rhsPrimaryp->u.quotedStringp->bytel     = bytel;
  modifiers = NULL; /* modifiers is in rhsPrimaryp */
  bytep = NULL; /* bytep is in rhsPrimaryp */
  if (marpaESLIFValueResult.sizel > 0) {
    rhsPrimaryp->u.quotedStringp->bytel -= (marpaESLIFValueResult.sizel + 1);  /* ":xxxx" */
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  if (bytep != NULL) {
    free(bytep);
  }
  if (modifiers != NULL) {
    free(modifiers);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_rhs_primary_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <rhs primary> ::= <symbol name> '@' <grammar reference> */
  marpaESLIF_bootstrap_rhs_primary_t       *rhsPrimaryp = NULL;
  marpaESLIF_t                             *marpaESLIFp = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char                                     *symbolNames = NULL;
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  short                               rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  /* symbolNames is of type ::ascii, even after going through <bracketed name>  */
  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &symbolNames, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have valid information */
  if (symbolNames == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_ptrb at indice %d returned NULL", arg0i);
    goto err;
  }

  /* <grammar reference> is a pointer */
  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, arg0i+2, NULL /* contextip */, (void *) &grammarReferencep, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to not have valid information */
  if (grammarReferencep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_ptrb at indice %d returned NULL", arg0i+2);
    goto err;
  }

  /* Make that an rhs primary structure */
  rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) malloc(sizeof(marpaESLIF_bootstrap_rhs_primary_t));
  if (rhsPrimaryp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_NA;

  rhsPrimaryp->u.symbolNameAndReferencep = (marpaESLIF_bootstrap_symbol_name_and_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_symbol_name_and_reference_t));
  if (rhsPrimaryp->u.symbolNameAndReferencep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  rhsPrimaryp->type = MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_NAME_AND_REFERENCE;
  rhsPrimaryp->u.symbolNameAndReferencep->symbols           = symbolNames;
  rhsPrimaryp->u.symbolNameAndReferencep->grammarReferencep = grammarReferencep;
  symbolNames = NULL; /* symbolNames is in symbolNameAndReferencep */
  grammarReferencep = NULL; /* grammarReferencep  is in symbolNameAndReferencep */

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_RHS_PRIMARY, rhsPrimaryp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_rhs_primary_freev(rhsPrimaryp);
  rcb = 0;

 done:
  if (symbolNames != NULL) {
    free(symbolNames);
  }
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_alternativeb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* alternative ::= rhs <adverb list> */
  marpaESLIF_t                       *marpaESLIFp          = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIF_bootstrap_alternative_t *alternativep         = NULL;
  genericStack_t                     *adverbListItemStackp = NULL;
  genericStack_t                     *rhsPrimaryStackp     = NULL;
  short                               undefb;
  short                               rcb;

  /* rhs must be a non-NULL generic stack of the primary */
  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &rhsPrimaryStackp, NULL /* shallowbp */)) {
    goto err;
  }
  if (rhsPrimaryStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "rhsPrimaryStackp is NULL");
    goto err;
  }
  
  /* adverb list may be undef */
  if (! marpaESLIFValue_stack_is_undefb(marpaESLIFValuep, argni, &undefb)) {
    goto err;
  }
  if (! undefb) {
    if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, argni, NULL /* contextip */, (void **) &adverbListItemStackp, NULL /* shallowbp */)) {
      goto err;
    }
    /* Non-sense to have a NULL stack in this case */
    if (adverbListItemStackp == NULL) {
      MARPAESLIF_ERROR(marpaESLIFp, "adverbListItemStackp is NULL");
      goto err;
    }
  }

  alternativep = (marpaESLIF_bootstrap_alternative_t *) malloc(sizeof(marpaESLIF_bootstrap_alternative_t));
  if (alternativep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  alternativep->rhsPrimaryStackp     = rhsPrimaryStackp;
  alternativep->adverbListItemStackp = adverbListItemStackp;
  alternativep->priorityi            = 0;  /* Used when there is the loosen "||" operator */

  rhsPrimaryStackp     = NULL;
  adverbListItemStackp = NULL;

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVE, alternativep, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternative_freev(alternativep);
  rcb = 0;

 done:
  _marpaESLIF_bootstrap_adverb_list_items_freev(adverbListItemStackp);
  _marpaESLIF_bootstrap_rhs_freev(rhsPrimaryStackp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_alternativesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* alternatives ::= alternative+  separator => <op equal priority> proper => 1 */
  marpaESLIF_t                       *marpaESLIFp       = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t                     *alternativeStackp = NULL;
  marpaESLIF_bootstrap_alternative_t *alternativep      = NULL;
  int                                i;
  short                              rcb;

  GENERICSTACK_NEW(alternativeStackp);
  if (GENERICSTACK_ERROR(alternativeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i += 2) { /* The separator is NOT skipped from the list of arguments */
    if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &alternativep, NULL /* shallowbp */)) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(alternativeStackp, (void *) alternativep);
    if (GENERICSTACK_ERROR(alternativeStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
      goto err;
    }
    alternativep = NULL; /* alternativep is now in alternativeStackp */
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_ALTERNATIVES, alternativeStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternative_freev(alternativep);
  _marpaESLIF_bootstrap_alternatives_freev(alternativeStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_prioritiesb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* priorities ::= alternatives+ separator => <op loosen>         proper => 1 */
  marpaESLIF_t   *marpaESLIFp        = marpaESLIFValue_eslifp(marpaESLIFValuep);
  genericStack_t *alternativesStackp = NULL;
  genericStack_t *alternativeStackp  = NULL;
  int             i;
  short           rcb;

  GENERICSTACK_NEW(alternativesStackp);
  if (GENERICSTACK_ERROR(alternativesStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  for (i = arg0i; i <= argni; i += 2) { /* The separator is NOT skipped from the list of arguments */
    if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, i, NULL /* contextip */, (void **) &alternativeStackp, NULL /* shallowbp */)) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(alternativesStackp, (void *) alternativeStackp);
    if (GENERICSTACK_ERROR(alternativesStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp push failure, %s", strerror(errno));
      goto err;
    }
    alternativeStackp = NULL; /* alternativeStackp is now in alternativesStackp */
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_PRIORITIES, alternativesStackp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_alternatives_freev(alternativeStackp);
  _marpaESLIF_bootstrap_priorities_freev(alternativesStackp);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_priority_ruleb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <priority rule> ::= lhs <op declare> priorities */
  /* **** The result will be undef **** */
  /* **** We work on userDatavp, that is a marpaESLIFGrammarp **** */
  /* **** In case of failure, the caller that is marpaESLIFGrammar_newp() will call a free on this marpaESLIFGrammarp **** */
  
  marpaESLIFGrammar_t                              *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_t                                     *marpaESLIFp        = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIF_rule_t                                *rulep              = NULL;
  int                                              *rhsip              = NULL;
  size_t                                            nrhsl;
  char                                             *symbolNames;
  genericStack_t                                   *alternativesStackp;
  genericStack_t                                   *alternativeStackp;
  genericStack_t                                   *rhsPrimaryStackp;
  genericStack_t                                   *adverbListItemStackp;
  int                                               adverbListItemi;
  int                                               alternativesi;
  int                                               alternativei;
  int                                               rhsPrimaryi;
  marpaESLIF_bootstrap_alternative_t               *alternativep;
  marpaESLIF_bootstrap_rhs_primary_t               *rhsPrimaryp;
  marpaESLIF_symbol_t                              *lhsp;
  marpaESLIF_symbol_t                              *rhsp;
  marpaESLIF_bootstrap_single_symbol_t             *singleSymbolp;
  marpaESLIF_bootstrap_single_symbol_t              singleSymbol; /* Fake single symbol in case of a "referenced-in-any-grammar" symbol */
  marpaESLIF_bootstrap_utf_string_t                *quotedStringp;
  marpaESLIF_bootstrap_symbol_name_and_reference_t *symbolNameAndReferencep;
  marpaESLIF_bootstrap_adverb_list_item_t          *adverbListItemp;
  marpaESLIF_grammar_t                             *grammarp;
  marpaESLIF_grammar_t                             *referencedGrammarp;
  int                                               leveli;
  short                                             rcb;
  short                                             undefb;
  char                                             *actions;
  short                                             left_associationb;
  short                                             right_associationb;
  short                                             group_associationb;
  int                                               priorityCounti;

  if (! marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &symbolNames, NULL /* shallowbp */)) {
    goto err;
  }
  if (! marpaESLIFValue_stack_get_intb(marpaESLIFValuep, arg0i+1, NULL /* contextip */, &leveli)) {
    goto err;
  }
  if (! marpaESLIFValue_stack_get_ptrb(marpaESLIFValuep, arg0i+2, NULL /* contextip */, (void **) &alternativesStackp, NULL /* shallowbp */)) {
    goto err;
  }

  /* Check grammar at that level exist */
  grammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFp, marpaESLIFGrammarp, leveli, NULL);
  if (grammarp == NULL) {
    goto err;
  }

  /* Check the lhs exist */
  lhsp = _marpaESLIF_bootstrap_check_meta_by_namep(marpaESLIFp, grammarp, symbolNames);
  if (lhsp == NULL) {
    goto err;
  }

  priorityCounti = GENERICSTACK_USED(alternativesStackp);
  if (priorityCounti > 1) {
    /* There is at least one '||' involved in the parsing      */
    /* We revisit everything up the RHS of every individual alternative */
    genericStack_t *workStackp             = NULL;
    char           *topasciis              = NULL;
    int             priority_ixi;
    short           localrcb;

    /* Create a fixed version of top prioritized LHS: lhs[0] */
    topasciis = (char *) malloc(strlen(symbolNames) + 3 /* "[0]" */ + 1 /* NUL byte */);
    if (topasciis == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto localerr;
    }
    strcpy(topasciis, symbolNames);
    strcat(topasciis, "[0]");

    GENERICSTACK_NEW(workStackp);
    if (GENERICSTACK_ERROR(workStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "workStackp initialization failure, %s", strerror(errno));
      goto localerr;
    }

    /* Fill priorityi of every alternative */
    for (priority_ixi = 0; priority_ixi < priorityCounti; priority_ixi++) {
      int priorityi;
      int alternativei;

      priorityi = priorityCounti - (priority_ixi + 1);
      /* Get the alternatives at indice priority_ixi */
      if (! GENERICSTACK_IS_PTR(alternativesStackp, priority_ixi)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp at indice %d is not PTR (got %s, value %d)", alternativesi, _marpaESLIF_genericStack_i_types(alternativesStackp, alternativesi), GENERICSTACKITEMTYPE(alternativesStackp, alternativesi));
        goto localerr;
      }
      alternativeStackp = (genericStack_t *) GENERICSTACK_IS_PTR(alternativesStackp, priority_ixi);
      for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
        if (! GENERICSTACK_IS_PTR(alternativeStackp, alternativei)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", alternativei, _marpaESLIF_genericStack_i_types(alternativeStackp, alternativei), GENERICSTACKITEMTYPE(alternativeStackp, alternativei));
          goto localerr;
        }
        alternativep = (marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);
        /* Set the priority of this alternative and push to the work stack */
        alternativep->priorityi = priorityi;
        GENERICSTACK_PUSH_PTR(workStackp, alternativep);
        if (GENERICSTACK_ERROR(workStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "workStackp push failure, %s", strerror(errno));
          goto localerr;
        }
      }
    }

    localrcb = 1;
    goto localdone;
 
  localerr:
    localrcb = 0;

  localdone:
    GENERICSTACK_FREE(workStackp);
    if (topasciis != NULL) {
      free(topasciis);
    }
    if (! localrcb) {
      goto err;
    } else {
      rcb = 1;
      goto done;
    }
  }

  /* Priorities (things separated by the || operator) is a stack of alternatives */
  for (alternativesi = 0; alternativesi < GENERICSTACK_USED(alternativesStackp); alternativesi++) {
    if (! GENERICSTACK_IS_PTR(alternativesStackp, alternativesi)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativesStackp at indice %d is not PTR (got %s, value %d)", alternativesi, _marpaESLIF_genericStack_i_types(alternativesStackp, alternativesi), GENERICSTACKITEMTYPE(alternativesStackp, alternativesi));
      goto err;
    }
    /* Alternatives (things separator by the | operator) is a stack of alternative */
    alternativeStackp = GENERICSTACK_GET_PTR(alternativesStackp, alternativesi);
    for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
      if (! GENERICSTACK_IS_PTR(alternativeStackp, alternativei)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", alternativei, _marpaESLIF_genericStack_i_types(alternativeStackp, alternativei), GENERICSTACKITEMTYPE(alternativeStackp, alternativei));
        goto err;
      }
      alternativep = (marpaESLIF_bootstrap_alternative_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);
      /* Alternatives is a stack of RHS followed by adverb items */
      rhsPrimaryStackp     = alternativep->rhsPrimaryStackp;
      adverbListItemStackp = alternativep->adverbListItemStackp;

      /* Prepare arguments to create the rule - note that RHS cannot be empty, this is the purpose of <empty rule> */
      rhsip = (int *) malloc(GENERICSTACK_USED(rhsPrimaryStackp) * sizeof(int));
      if (rhsip == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      nrhsl = GENERICSTACK_USED(rhsPrimaryStackp);

      /* Analyse RHS list */
      for (rhsPrimaryi = 0; rhsPrimaryi < (int) nrhsl; rhsPrimaryi++) {
        if (! GENERICSTACK_IS_PTR(rhsPrimaryStackp, rhsPrimaryi)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp at indice %d is not PTR (got %s, value %d)", rhsPrimaryi, _marpaESLIF_genericStack_i_types(rhsPrimaryStackp, rhsPrimaryi), GENERICSTACKITEMTYPE(rhsPrimaryStackp, rhsPrimaryi));
          goto err;
        }
        rhsPrimaryp = (marpaESLIF_bootstrap_rhs_primary_t *) GENERICSTACK_GET_PTR(rhsPrimaryStackp, rhsPrimaryi);
        switch (rhsPrimaryp->type) {
        case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SINGLE_SYMBOL:
          rhsp = _marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFp, grammarp, rhsPrimaryp->u.singleSymbolp);
          if (rhsp == NULL) {
            goto err;
          }
          break;
        case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_QUOTED_STRING:
          rhsp = _marpaESLIF_bootstrap_check_quotedStringp(marpaESLIFp, grammarp, rhsPrimaryp->u.quotedStringp);
          if (rhsp == NULL) {
            goto err;
          }
          break;
        case MARPAESLIF_BOOTSTRAP_RHS_PRIMARY_TYPE_SYMBOL_NAME_AND_REFERENCE:
          /* We want to do two things: */
          /* 1. symbol must exist in the current grammar */
          singleSymbol.type = MARPAESLIF_SINGLE_SYMBOL_TYPE_SYMBOL;
          singleSymbol.u.symbols = rhsPrimaryp->u.symbolNameAndReferencep->symbols;
          rhsp = _marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFp, grammarp, &singleSymbol);
          if (rhsp == NULL) {
            goto err;
          }
          /* 2. symbol must exist in the referenced grammar */
          switch (rhsPrimaryp->u.symbolNameAndReferencep->grammarReferencep->type) {
          case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING:
            referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFp, marpaESLIFGrammarp, -1, rhsPrimaryp->u.symbolNameAndReferencep->grammarReferencep->u.quotedStringp);
            if (referencedGrammarp == NULL) {
              goto err;
            }
            break;
          case MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER:
            referencedGrammarp = _marpaESLIF_bootstrap_check_grammarp(marpaESLIFp, marpaESLIFGrammarp, grammarp->leveli + rhsPrimaryp->u.symbolNameAndReferencep->grammarReferencep->u.signedIntegeri, NULL);
            if (referencedGrammarp == NULL) {
              goto err;
            }
            break;
          default:
            MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported grammar reference type (%d)", rhsPrimaryp->u.symbolNameAndReferencep->grammarReferencep->type);
            goto err;
          }
          if (_marpaESLIF_bootstrap_check_singleSymbolp(marpaESLIFp, referencedGrammarp, &singleSymbol) == NULL) {
            goto err;
          }
          break;
        default:
          MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported RHS primary type (%d)", rhsPrimaryp->type);
          goto err;
        }
        rhsip[rhsPrimaryi] = rhsp->idi;
      }

      /* Analyse adverb list items - take care this is nullable and we propage NULL if it is the case */
      actions = NULL;
      if (adverbListItemStackp != NULL) {
        for (adverbListItemi = 0; adverbListItemi < GENERICSTACK_USED(adverbListItemStackp); adverbListItemi++) {
          if (! GENERICSTACK_IS_PTR(adverbListItemStackp, adverbListItemi)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp at indice %d is not PTR (got %s, value %d)", adverbListItemi, _marpaESLIF_genericStack_i_types(adverbListItemStackp, adverbListItemi), GENERICSTACKITEMTYPE(adverbListItemStackp, adverbListItemi));
            goto err;
          }
          adverbListItemp = (marpaESLIF_bootstrap_adverb_list_item_t *) GENERICSTACK_GET_PTR(adverbListItemStackp, adverbListItemi);
          switch (adverbListItemp->type) {
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_ACTION:
            actions = adverbListItemp->u.actions;
            break;
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_AUTORANK:
            MARPAESLIF_ERROR(marpaESLIFp, "autorank adverb is not allowed in a rule context");
            goto err;
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_LEFT_ASSOCIATION:
            left_associationb = adverbListItemp->u.left_associationb;
            break;
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_RIGHT_ASSOCIATION:
            right_associationb = adverbListItemp->u.right_associationb;
            break;
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_GROUP_ASSOCIATION:
            group_associationb = adverbListItemp->u.group_associationb;
            break;
          case MARPAESLIF_BOOTSTRAP_ADVERB_LIST_ITEM_TYPE_SEPARATOR:
            MARPAESLIF_ERROR(marpaESLIFp, "separator adverb is not allowed in a priotitized rule context");
            break;
          default:
            MARPAESLIF_ERRORF(marpaESLIFp, "adverbListItemStackp type at indice %d is not supported (value %d)", adverbListItemi, adverbListItemp->type);
            goto err;
          }
        }
      }
      rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                    grammarp,
                                    NULL, /* descEncodings */
                                    NULL, /* descs */
                                    0, /* descl */
                                    lhsp->idi,
                                    nrhsl,
                                    rhsip,
                                    0, /* nexceptionl */
                                    NULL, /* exceptionip */
                                    0, /* ranki */
                                    0, /* nullRanksHighb */
                                    0, /* sequenceb */
                                    -1, /* minimumi */
                                    -1, /* separatori */
                                    0, /* properb */
                                    actions,
                                    0 /* passthroughb */);
      if (rulep == NULL) {
        goto err;
      }
      free(rhsip);
      rhsip = NULL;
      GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
      if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
        goto err;
      }
      /* Push is ok: rulep is in grammarp->ruleStackp */
      rulep = NULL;
    }
  }
  /* We set nothing in the stack, our parent will return ::undef up to the top-level */
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (rhsip != NULL) {
    free(rhsip);
  }
  _marpaESLIF_rule_freev(rulep);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_single_symbol_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <single symbol> ::= symbol */
  /* symbol is guaranteed to be an ::ascii compatible thingy */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp = NULL;
  marpaESLIF_t                         *marpaESLIFp     = marpaESLIFValue_eslifp(marpaESLIFValuep);
  char                                 *asciis;
  short                                 rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValuep, arg0i, NULL /* contextip */, (void **) &asciis, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to have a null asciis */
  if (asciis == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_ptrb at indice %d returned NULL", argni);
    goto err;
  }

  singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
  if (singleSymbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type      = MARPAESLIF_SINGLE_SYMBOL_TYPE_SYMBOL;
  singleSymbolp->u.symbols = asciis;
  asciis = NULL; /* asciis is in singleSymbolp */

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL, singleSymbolp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  rcb = 0;

 done:
  if (asciis != NULL) {
    free(asciis);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_single_symbol_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <single symbol> ::= <character class> */
  /* <character class> is a lexeme. */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp         = NULL;
  marpaESLIF_t                         *marpaESLIFp           = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFRecognizer_t               *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  char                                 *modifiers             = NULL;
  marpaESLIFGrammar_t                   marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t               marpaESLIFValueResult;
  marpaESLIF_matcher_value_t            rci;
  void                                 *bytep;
  size_t                                bytel;
  short                                 rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to have a null lexeme */
  if ((bytep == NULL) || (bytel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {%p,%ld}", argni, bytep, (unsigned long) bytel);
    goto err;
  }

  /* Extract opti from it */
  /* Thre are several methods...: */
  /* - Re-execute the sub-grammar as if it was a top grammar */
  /* - apply a regexp to extract the modifiers. */
  /* - revisit our own top grammar to have two separate lexemes (which I do not like because modifers can then be separated from regex by a discard symbol) */
  /* ... Since we are internal anyway I choose (what I think is) the costless method: the regexp */

  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, marpaESLIFp->characterClassModifiersp, bytep, bytel, 1 /* eofb */, &rci, &marpaESLIFValueResult)) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a character class it is something like ":xxxxx" */
    if (marpaESLIFValueResult.sizel <= 0) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
    modifiers = (char *) malloc(marpaESLIFValueResult.sizel + 1);
    if (modifiers == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(modifiers, marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
    modifiers[marpaESLIFValueResult.sizel] = '\0';
    free(marpaESLIFValueResult.u.p);
  } else {
    /* Because we use this value just below */
    marpaESLIFValueResult.sizel = 0;
  }

  singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
  if (singleSymbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type              = MARPAESLIF_SINGLE_SYMBOL_TYPE_NA;
  singleSymbolp->u.characterClassp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (singleSymbolp->u.characterClassp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type                         = MARPAESLIF_SINGLE_SYMBOL_TYPE_CHARACTER_CLASS;
  singleSymbolp->u.characterClassp->modifiers = modifiers;
  singleSymbolp->u.characterClassp->bytep     = bytep;
  singleSymbolp->u.characterClassp->bytel     = bytel;
  modifiers = NULL; /* modifiers is in singleSymbolp */
  bytep = NULL; /* bytep is in singleSymbolp */
  if (marpaESLIFValueResult.sizel > 0) {
    singleSymbolp->u.characterClassp->bytel -= (marpaESLIFValueResult.sizel + 1);  /* ":xxxx" */
  }

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL, singleSymbolp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  rcb = 0;

 done:
  if (modifiers != NULL) {
    free(modifiers);
  }
  if (bytep != NULL) {
    free(bytep);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
 return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_single_symbol_3b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <single symbol> ::= <regular expression> */
  /* <regular expression> is a lexeme. */
  marpaESLIF_bootstrap_single_symbol_t *singleSymbolp         = NULL;
  marpaESLIF_t                         *marpaESLIFp           = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIFRecognizer_t               *marpaESLIFRecognizerp = NULL; /* Fake recognizer to use the internal regex */
  char                                 *modifiers             = NULL;
  void                                 *bytep                 = NULL;
  size_t                                bytel;
  void                                 *newbytep              = NULL;
  size_t                                newbytel;
  marpaESLIFGrammar_t                   marpaESLIFGrammar; /* Fake grammar for the same reason */
  marpaESLIFValueResult_t               marpaESLIFValueResult;
  marpaESLIF_matcher_value_t            rci;
  short                                 rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to have a null lexeme */
  if ((bytep == NULL) || (bytel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {%p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  /* Extract opti from it */
  /* Thre are several methods...: */
  /* - Re-execute the sub-grammar as if it was a top grammar */
  /* - apply a regexp to extract the modifiers. */
  /* - revisit our own top grammar to have two separate lexemes (which I do not like because modifers can then be separated from regex by a discard symbol) */
  /* ... Since we are internal anyway I choose (what I think is) the costless method: the regexp */

  /* Fake a recognizer. EOF flag will be set automatically in fake mode */
  marpaESLIFGrammar.marpaESLIFp = marpaESLIFp;
  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(&marpaESLIFGrammar, NULL /* marpaESLIFRecognizerOptionp */, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, 1 /* fakeb */);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_regex_matcherb(marpaESLIFRecognizerp, marpaESLIFp->regexModifiersp, bytep, bytel, 1 /* eofb */, &rci, &marpaESLIFValueResult)) {
    goto err;
  }
  if (rci == MARPAESLIF_MATCH_OK) {
    /* Got modifiers. Per def this is an sequence of ASCII characters. */
    /* For a regular expression it is something like "xxxxx" */
    if (marpaESLIFValueResult.sizel <= 0) {
      MARPAESLIF_ERROR(marpaESLIFp, "Match of character class modifiers returned empty size");
      goto err;
    }
    modifiers = (char *) malloc(marpaESLIFValueResult.sizel + 1);
    if (modifiers == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(modifiers, marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
    modifiers[marpaESLIFValueResult.sizel] = '\0';
    free(marpaESLIFValueResult.u.p);
  } else {
    /* Because we use this value just below */
    marpaESLIFValueResult.sizel = 0;
  }

  singleSymbolp = (marpaESLIF_bootstrap_single_symbol_t *) malloc(sizeof(marpaESLIF_bootstrap_single_symbol_t));
  if (singleSymbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  singleSymbolp->type                 = MARPAESLIF_SINGLE_SYMBOL_TYPE_NA;
  singleSymbolp->u.regularExpressionp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (singleSymbolp->u.characterClassp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  /* By definition a regular expression is a lexeme in this form: /xxxx/modifiers */
  /* we have already catched the modifiers. But we have to shift the UTF-8 buffer: */
  /* - We know per def that it is starting with the "/" ASCII character (one byte) */
  /* - We know per def that it is endiing with "/modifiers", all of them being ASCII characters (one byte each) */
  newbytel = bytel - 2; /* First "/" and last "/" */
  if (newbytel <= 0) {
    /* Empty regex !? */
    MARPAESLIF_ERROR(marpaESLIFp, "Empty regex");
    goto err;
  }
  if (marpaESLIFValueResult.sizel > 0) {
    newbytel -= marpaESLIFValueResult.sizel;  /* "xxxx" */
  }
  if (newbytel <= 0) {
    /* Still Empty regex !? */
    MARPAESLIF_ERROR(marpaESLIFp, "Empty regex");
    goto err;
  }
  newbytep = malloc(newbytel);
  if (newbytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(newbytep, (void *) (((char *) bytep) + 1), newbytel);
  singleSymbolp->type                         = MARPAESLIF_SINGLE_SYMBOL_TYPE_REGULAR_EXPRESSION;
  singleSymbolp->u.characterClassp->modifiers = modifiers;
  singleSymbolp->u.characterClassp->bytep     = newbytep;
  singleSymbolp->u.characterClassp->bytel     = newbytel;
  modifiers = NULL; /* modifiers is in singleSymbolp */
  newbytep = NULL; /* newbytep is in singleSymbolp */

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_SINGLE_SYMBOL, singleSymbolp, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_single_symbol_freev(singleSymbolp);
  rcb = 0;

 done:
  if (bytep != NULL) {
    free(bytep);
  }
  if (newbytep != NULL) {
    free(newbytep);
  }
  if (modifiers != NULL) {
    free(modifiers);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
 return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_grammar_reference_1b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <grammar reference> ::= <quoted string> */
  marpaESLIF_t                             *marpaESLIFp       = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  void                                     *bytep             = NULL;
  size_t                                    bytel;
  void                                     *newbytep          = NULL;
  size_t                                    newbytel;
  short                                     rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to have a null information */
  if ((bytep == NULL) || (bytel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  /* We are not going to use this quoted string as a terminal, therefore we have to remove the surrounding characters ourself */
  if (bytel <= 2) {
    /* Empty string ? */
    MARPAESLIF_ERROR(marpaESLIFp, "An empty string as grammar reference is not allowed");
    goto err;
  }
  newbytel = bytel - 2;
  newbytep = malloc(newbytel);
  if (newbytep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  /* Per def, the surrounding characters are always ASCII taking one byte ("", '', {}) */
  memcpy(newbytep, (void *)(((char *) bytep) - 2), newbytel);
  free(bytep);
  bytep = NULL; /* No need of bytep anymore */

  grammarReferencep = (marpaESLIF_bootstrap_grammar_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_grammar_reference_t));
  if (grammarReferencep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type            = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_NA;
  grammarReferencep->u.quotedStringp = (marpaESLIF_bootstrap_utf_string_t *) malloc(sizeof(marpaESLIF_bootstrap_utf_string_t));
  if (grammarReferencep->u.quotedStringp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type          = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_STRING;
  grammarReferencep->u.quotedStringp->bytep     = newbytep;
  grammarReferencep->u.quotedStringp->bytel     = newbytel;
  grammarReferencep->u.quotedStringp->modifiers = NULL;
  newbytep = NULL; /* newbytep is in quotedStringp */

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE, grammarReferencep, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  rcb = 0;

 done:
  if (bytep != NULL) {
    free(bytep);
  }
  if (newbytep != NULL) {
    free(newbytep);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_bootstrap_G1_action_grammar_reference_2b(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* <grammar reference> ::= <signed integer> */
  marpaESLIF_t                             *marpaESLIFp       = marpaESLIFValue_eslifp(marpaESLIFValuep);
  marpaESLIF_bootstrap_grammar_reference_t *grammarReferencep = NULL;
  void                                     *bytep             = NULL;
  size_t                                    bytel;
  char                                     *signedIntegers    = NULL;
  short                                     rcb;

  /* Cannot be nullable */
  if (nullableb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Nullable mode is not supported");
    goto err;
  }

  if (! marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValuep, arg0i, NULL /* contextip */, &bytep, &bytel, NULL /* shallowbp */)) {
    goto err;
  }
  /* It is a non-sense to have a null information */
  if ((bytep == NULL) || (bytel <= 0)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFValue_stack_get_arrayb at indice %d returned {p,%ld}", arg0i, bytep, (unsigned long) bytel);
    goto err;
  }

  /* Per def via the grammar it is not-NULL terminated ASCII string */
  signedIntegers = malloc(bytel + 1);
  if (signedIntegers == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  memcpy(signedIntegers, bytep, bytel);
  signedIntegers[bytel] = '\0';
  
  grammarReferencep = (marpaESLIF_bootstrap_grammar_reference_t *) malloc(sizeof(marpaESLIF_bootstrap_grammar_reference_t));
  if (grammarReferencep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  grammarReferencep->type             = MARPAESLIF_BOOTSTRAP_GRAMMAR_REFERENCE_TYPE_SIGNED_INTEGER;
  grammarReferencep->u.signedIntegeri = atoi(signedIntegers);

  if (! marpaESLIFValue_stack_set_ptrb(marpaESLIFValuep, resulti, MARPAESLIF_BOOTSTRAP_STACK_TYPE_GRAMMAR_REFERENCE, grammarReferencep, 0 /* shallowb */)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_bootstrap_grammar_reference_freev(grammarReferencep);
  rcb = 0;

 done:
  if (bytep != NULL) {
    free(bytep);
  }
  if (signedIntegers != NULL) {
    free(signedIntegers);
  }
  return rcb;
}
