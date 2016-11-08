
/*****************************************************************************/
static inline void _marpaESLIF_grammarContext_resetv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp)
/*****************************************************************************/
{
  int                   i;
  int                   usedi;
  genericStack_t       *outputStackp;
  genericStack_t       *itemTypeStackp;
  genericStack_t       *grammarStackp;
  marpaESLIF_grammar_t *grammarp;

  /* In the grammar context, outputStack contain items that are typed using itemTypeStackp */
  if (marpaESLIF_grammarContextp != NULL) {
    outputStackp   = marpaESLIF_grammarContextp->outputStackp;
    itemTypeStackp = marpaESLIF_grammarContextp->itemTypeStackp;
    grammarStackp  = marpaESLIF_grammarContextp->grammarStackp;

    /* If one of them is NULL, this cannot be done */
    if ((outputStackp != NULL) && (itemTypeStackp != NULL)) {
      usedi = (int) GENERICSTACK_USED(outputStackp);
      for (i = usedi - 1; i >= 0; i--) {
        _marpaESLIF_grammarContext_i_resetb(marpaESLIFp, outputStackp, itemTypeStackp, i);
      }
      /* We know there are "on the stack" */
      GENERICSTACK_RESET(outputStackp);
      GENERICSTACK_RESET(itemTypeStackp);
    }

    if (grammarStackp != NULL) {
      usedi = (int) GENERICSTACK_USED(grammarStackp);
      for (i = usedi - 1; i >= 0; i--) {
        if (! GENERICSTACK_IS_PTR(grammarStackp, i)) {
          continue;
        }
        grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, i);
        _marpaESLIF_grammar_freev(grammarp);
      }
      /* We know it is on the "heap" */
      GENERICSTACK_FREE(grammarStackp);
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIF_grammarContext_i_resetb(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i)
/*****************************************************************************/
{
  static const char            *funcs = "_marpaESLIF_grammarContext_i_resetb";
  marpaESLIF_grammarItemType_t  itemType;
  char                         *asciis;
  genericStack_t               *alternativesStackp;
  genericStack_t               *alternativeStackp;
  int                           j;
  int                           k;
  short                         rcb;

  if ((outputStackp != NULL) && (itemTypeStackp != NULL)) {
    if (GENERICSTACK_IS_INT(itemTypeStackp, i)) {
      itemType = (marpaESLIF_grammarItemType_t) GENERICSTACK_GET_INT(itemTypeStackp, i);

      switch (itemType) {
      case MARPAESLIF_GRAMMARITEMTYPE_LEXEME:        /* ARRAY */
        if (GENERICSTACK_IS_ARRAY(outputStackp, i)) {
          GENERICSTACKITEMTYPE2TYPE_ARRAY array =  GENERICSTACK_GET_ARRAY(outputStackp, i);
          if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
            free(GENERICSTACK_ARRAY_PTR(array));
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_OP_DECLARE:         /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ACTION_NAME:        /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ACTION:             /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION: /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR: /* String */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          marpaESLIF_string_t *stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(outputStackp, i);
          _marpaESLIF_string_freev(stringp);
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK: /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY: /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE: /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT: /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE: /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING: /* String */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          marpaESLIF_string_t *stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(outputStackp, i);
          _marpaESLIF_string_freev(stringp);
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL: /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST_ITEMS:  /* Stack of marpaESLIF_adverbItem_t */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_adverbItemStack_freev((genericStack_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST:        /* Stack of marpaESLIF_adverbItem_t as well */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_adverbItemStack_freev((genericStack_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_LATM:               /* SHORT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SYMBOL_NAME:        /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SYMBOL:             /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_LHS:                /* ASCII string */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          asciis = (char *) GENERICSTACK_GET_PTR(outputStackp, i);
          if (asciis != NULL) {
            free(asciis);
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SINGLE_SYMBOL:       /* marpaESLIF_rhsItem_t * */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_rhsItem_freev((marpaESLIF_rhsItem_t *) GENERICSTACK_IS_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_QUANTIFIER:          /* INT */
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_PRIORITIES:        /* Stack of alternatives */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          alternativesStackp = (genericStack_t *) GENERICSTACK_GET_PTR(outputStackp, i);
          for (j = 0; j < GENERICSTACK_USED(alternativesStackp); j++) { /* Stack of alternatives -> Stack of alternative */
            if (GENERICSTACK_IS_PTR(alternativesStackp, j)) {
              alternativeStackp = (genericStack_t *) GENERICSTACK_GET_PTR(alternativesStackp, j);
              for (k = 0; k < GENERICSTACK_USED(alternativeStackp); k++) { /* Stack of alternatives -> Stack of alternative -> Stack of marpaESLIF_alternativeItem_t */
                if (GENERICSTACK_IS_PTR(alternativeStackp, k)) {
                  _marpaESLIF_alternativeItemStack_freev((genericStack_t *) GENERICSTACK_GET_PTR(alternativeStackp, k));
                }
              }
            }
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVES:        /* Stack of alternative */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          alternativeStackp = (genericStack_t *) GENERICSTACK_GET_PTR(outputStackp, i);
          for (j = 0; j < GENERICSTACK_USED(alternativeStackp); j++) { /* Stack of alternative -> Stack of marpaESLIF_alternativeItem_t */
            if (GENERICSTACK_IS_PTR(alternativeStackp, j)) {
              _marpaESLIF_alternativeItemStack_freev((genericStack_t *) GENERICSTACK_GET_PTR(alternativeStackp, j));
            }
          }
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVE:        /* marpaESLIF_alternativeItem_t* */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_alternativeItem_freev((marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_RHS:        /* marpaESLIF_rhsItem_t* */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_rhsItem_freev((marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_GRAMMAR_REFERENCE: /* marpaESLIF_grammarReference_t* */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_grammarReference_freev((marpaESLIF_grammarReference_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_RHS_PRIMARY: /* marpaESLIF_rhsItem_t* */
        if (GENERICSTACK_IS_PTR(outputStackp, i)) {
          _marpaESLIF_rhsItem_freev((marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(outputStackp, i));
        }
        break;
      default:
        break;
      }
    }

    GENERICSTACK_SET_NA(itemTypeStackp, i);
    if (GENERICSTACK_ERROR(itemTypeStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "itemTypeStackp set failure, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_SET_NA(outputStackp, i);
    if (GENERICSTACK_ERROR(outputStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp set failure, %s", strerror(errno));
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline const char *_marpaESLIF_grammarContext_i_types(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i)
/*****************************************************************************/
{
  static const char            *funcs = "_marpaESLIF_grammarContext_i_types";
  marpaESLIF_grammarItemType_t  itemType;
  const char                   *rcs = marpaESLIF_grammarContext_UNKNOWN_types;

  if (itemTypeStackp != NULL) {
    if (GENERICSTACK_IS_INT(itemTypeStackp, i)) {
      itemType = (marpaESLIF_grammarItemType_t) GENERICSTACK_GET_INT(itemTypeStackp, i);

      switch (itemType) {
      case MARPAESLIF_GRAMMARITEMTYPE_NA:        /* N/A */
        rcs = marpaESLIF_grammarContext_NA_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_LEXEME:        /* ARRAY */
        rcs = marpaESLIF_grammarContext_LEXEME_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_OP_DECLARE:         /* INT */
        rcs = marpaESLIF_grammarContext_OP_DECLARE_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ACTION_NAME:        /* ASCII string */
        rcs = marpaESLIF_grammarContext_ACTION_NAME_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ACTION:             /* ASCII string */
        rcs = marpaESLIF_grammarContext_ACTION_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION: /* ASCII string */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_ACTION_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_AUTORANK_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_LEFT_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_RIGHT_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_GROUP_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR: /* String */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_SEPARATOR_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_PROPER_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK: /* INT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_RANK_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_NULL_RANKING_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY: /* INT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_PRIORITY_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE: /* INT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_PAUSE_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT: /* ASCII string */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_EVENT_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE: /* INT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_EVENTTYPE_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_EVENTINIT_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_LATM_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING: /* String */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_NAMING_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL: /* SHORT */
        rcs = marpaESLIF_grammarContext_ADVERB_ITEM_NULL_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST_ITEMS:  /* Stack of marpaESLIF_adverbItem_t */
        rcs = marpaESLIF_grammarContext_ADVERB_LIST_ITEMS_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST:        /* Alias to MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST_ITEMS - the two are exclusives (they share the same indice in outputStackp) */
        rcs = marpaESLIF_grammarContext_ADVERB_LIST_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_LATM:               /* SHORT */
        rcs = marpaESLIF_grammarContext_LATM_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SYMBOL_NAME:        /* ASCII string */
        rcs = marpaESLIF_grammarContext_SYMBOL_NAME_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SYMBOL:             /* ASCII string */
        rcs = marpaESLIF_grammarContext_SYMBOL_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_LHS:                /* ASCII string */
        rcs = marpaESLIF_grammarContext_LHS_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_SINGLE_SYMBOL:       /* mrpaESLIF_rhsItem_t * */
        rcs = marpaESLIF_grammarContext_SINGLE_SYMBOL_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_QUANTIFIER:          /* INT */
        rcs = marpaESLIF_grammarContext_QUANTIFIER_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_PRIORITIES:          /* Stack of alternatives */
        rcs = marpaESLIF_grammarContext_PRIORITIES_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVES:        /* Stack of alternative */
        rcs = marpaESLIF_grammarContext_ALTERNATIVES_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVE:        /* Stack of marpaESLIF_alternativeItem_t */
        rcs = marpaESLIF_grammarContext_ALTERNATIVE_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_RHS:        /* Stack of marpaESLIF_rhsItem_t */
        rcs = marpaESLIF_grammarContext_RHS_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_GRAMMAR_REFERENCE:        /* Pointer to marpaESLIF_grammarReferenceItem_t */
        rcs = marpaESLIF_grammarContext_GRAMMAR_REFERENCE_types;
        break;
      case MARPAESLIF_GRAMMARITEMTYPE_RHS_PRIMARY:        /* Pointer to marpaESLIF_grammarReferenceItem_t */
        rcs = marpaESLIF_grammarContext_RHS_PRIMARY_types;
        break;
      default:
        break;
      }
    }
  }

  return rcs;
}

/*****************************************************************************/
static inline short _marpaESLIF_grammarContext_get_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t *typep)
/*****************************************************************************/
{
  static const char            *funcs = "_marpaESLIF_grammarContext_get_typeb";
  marpaESLIF_grammarItemType_t  type;
  short                         rcb;

  if (GENERICSTACK_IS_INT(itemTypeStackp, i)) {
    type = (marpaESLIF_grammarItemType_t) GENERICSTACK_GET_INT(itemTypeStackp, i);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFp, "Not an INT in itemTypeStackp at indice %d", i);
    goto err;
  }

  if (typep != NULL) {
    *typep = type;
  }
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_grammarContext_set_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t type)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_grammarContext_set_typeb";
  short              rcb;

  GENERICSTACK_SET_INT(itemTypeStackp, type, i);
  if (GENERICSTACK_ERROR(itemTypeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "itemTypeStackp set failure at indice %d, %s", i, strerror(errno));
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
static inline void _marpaESLIF_adverbItemStack_freev(genericStack_t *adverbItemStackp)
/*****************************************************************************/
{
  int i;

  if (adverbItemStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(adverbItemStackp); i++) {
      if (GENERICSTACK_IS_PTR(adverbItemStackp, i)) {
        _marpaESLIF_adverbItem_freev((marpaESLIF_adverbItem_t *) GENERICSTACK_GET_PTR(adverbItemStackp, i));
      }
    }
    GENERICSTACK_FREE(adverbItemStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_adverbItem_freev(marpaESLIF_adverbItem_t *adverbItemp)
/*****************************************************************************/
{
  if (adverbItemp != NULL) {
    switch (adverbItemp->type) {
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION: /* ASCII string */
      if (adverbItemp->u.asciis != NULL) {
        free(adverbItemp->u.asciis);
      }
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR: /* String */
      _marpaESLIF_string_freev(adverbItemp->u.stringp);
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK: /* INT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY: /* INT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE: /* INT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT: /* ASCII string */
      if (adverbItemp->u.asciis != NULL) {
        free(adverbItemp->u.asciis);
      }
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE: /* INT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM: /* SHORT */
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING: /* String */
      _marpaESLIF_string_freev(adverbItemp->u.stringp);
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL: /* NA */
      break;
    default:
      break;
    }
    free(adverbItemp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_adverbItem_t *_marpaESLIF_adverbItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_adverbItem_t *adverbItemOrigp)
/*****************************************************************************/
{
  marpaESLIF_adverbItem_t *adverbItemp = NULL;

  if (adverbItemOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "adverbItemOrigp is NULL");
    goto err;
  }

  adverbItemp = (marpaESLIF_adverbItem_t *) malloc(sizeof(marpaESLIF_adverbItem_t));
  if (adverbItemp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  adverbItemp->type = MARPAESLIF_GRAMMARITEMTYPE_NA;

  switch (adverbItemOrigp->type) {
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION: /* ASCII string */
      if (adverbItemOrigp->u.asciis != NULL) {
        adverbItemp->u.asciis = strdup(adverbItemOrigp->u.asciis);
        if (adverbItemp->u.asciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
          goto err;
        }
      }
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR: /* String */
      adverbItemp->u.stringp = _marpaESLIF_string_clonep(marpaESLIFp, adverbItemOrigp->u.stringp);
      if (adverbItemp->u.stringp != NULL) {
        goto err;
      }
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK: /* INT */
      adverbItemp->u.i = adverbItemOrigp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY: /* INT */
      adverbItemp->u.i = adverbItemOrigp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE: /* INT */
      adverbItemp->u.i = adverbItemOrigp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT: /* ASCII string */
      if (adverbItemOrigp->u.asciis != NULL) {
        adverbItemp->u.asciis = strdup(adverbItemOrigp->u.asciis);
        if (adverbItemp->u.asciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
          goto err;
        }
      }
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE: /* INT */
      adverbItemp->u.i = adverbItemOrigp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM: /* SHORT */
      adverbItemp->u.b = adverbItemOrigp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING: /* String */
      adverbItemp->u.stringp = _marpaESLIF_string_clonep(marpaESLIFp, adverbItemOrigp->u.stringp);
      if (adverbItemp->u.stringp != NULL) {
        goto err;
      }
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL: /* NA */
      break;
    default:
      break;
    }


  adverbItemp->type = adverbItemOrigp->type;
  goto done;

 err:
  _marpaESLIF_adverbItem_freev(adverbItemp);
  adverbItemp = NULL;

 done:
  return adverbItemp;  
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_adverbItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *adverbItemStackOrigp)
/*****************************************************************************/
{
  genericStack_t          *adverbItemStackp = NULL;
  marpaESLIF_adverbItem_t *adverbItemp;
  int                      i;

  if (adverbItemStackOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "adverbItemStackOrigp is NULL");
    goto err;
  }

  GENERICSTACK_INIT(adverbItemStackp);
  if (GENERICSTACK_ERROR(adverbItemStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "adverbItemStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  for (i = 0; i < GENERICSTACK_USED(adverbItemStackOrigp); i++) {
    if (! GENERICSTACK_IS_PTR(adverbItemStackOrigp, i)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "adverbItemStackOrigp->[%d] is not a PTR", i);
      goto err;
    }
    adverbItemp = _marpaESLIF_adverbItem_clonep(marpaESLIFp, (marpaESLIF_adverbItem_t *) GENERICSTACK_GET_PTR(adverbItemStackOrigp, i));
    if (adverbItemp == NULL) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(adverbItemStackp, adverbItemp);
    if (GENERICSTACK_ERROR(adverbItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "adverbItemStackp push failure, %s", strerror(errno));
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_adverbItemStack_freev(adverbItemStackp);
  adverbItemStackp = NULL;

 done:
  return adverbItemStackp;  
}

/*****************************************************************************/
static inline void _marpaESLIF_alternativeItem_freev(marpaESLIF_alternativeItem_t *alternativeItemp)
/*****************************************************************************/
{
  if (alternativeItemp != NULL) {
    _marpaESLIF_rhsItemStack_freev(alternativeItemp->rhsItemStackp);
    _marpaESLIF_adverbItemStack_freev(alternativeItemp->adverbItemStackp);
    free(alternativeItemp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_alternativeItem_t *_marpaESLIF_alternativeItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_alternativeItem_t *alternativeItemOrigp)
/*****************************************************************************/
{
  marpaESLIF_alternativeItem_t *alternativeItemp = NULL;

  if (alternativeItemOrigp == NULL) {
      MARPAESLIF_ERROR(marpaESLIFp, "alternativeItemOrigp is NULL");
      goto err;
  }

  alternativeItemp = (marpaESLIF_alternativeItem_t *) malloc(sizeof(marpaESLIF_alternativeItem_t));
  if (alternativeItemp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  alternativeItemp->rhsItemStackp     = NULL;
  alternativeItemp->adverbItemStackp  = NULL;
  alternativeItemp->priorityi         = alternativeItemOrigp->priorityi;

  if (alternativeItemOrigp->rhsItemStackp != NULL) {
    alternativeItemp->rhsItemStackp = _marpaESLIF_rhsItemStack_clonep(marpaESLIFp, alternativeItemOrigp->rhsItemStackp);
    if (alternativeItemp->rhsItemStackp == NULL) {
      goto err;
    }
  }

  if (alternativeItemOrigp->adverbItemStackp != NULL) {
    alternativeItemp->adverbItemStackp = _marpaESLIF_adverbItemStack_clonep(marpaESLIFp, alternativeItemOrigp->adverbItemStackp);
    if (alternativeItemp->adverbItemStackp == NULL) {
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_alternativeItem_freev(alternativeItemp);
  alternativeItemp = NULL;

 done:
  return alternativeItemp;  
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_alternativeItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *alternativeItemStackOrigp)
/*****************************************************************************/
{
  genericStack_t               *alternativeItemStackp = NULL;
  marpaESLIF_alternativeItem_t *alternativeItemp;
  int                           i;

  if (alternativeItemStackOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "alternativeItemStackOrigp is NULL");
    goto err;
  }

  GENERICSTACK_INIT(alternativeItemStackp);
  if (GENERICSTACK_ERROR(alternativeItemStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeItemStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  for (i = 0; i < GENERICSTACK_USED(alternativeItemStackOrigp); i++) {
    if (! GENERICSTACK_IS_PTR(alternativeItemStackOrigp, i)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativeItemStackOrigp->[%d] is not a PTR", i);
      goto err;
    }
    alternativeItemp = _marpaESLIF_alternativeItem_clonep(marpaESLIFp, (marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(alternativeItemStackOrigp, i));
    if (alternativeItemp == NULL) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(alternativeItemStackp, alternativeItemp);
    if (GENERICSTACK_ERROR(alternativeItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "alternativeItemStackp push failure, %s", strerror(errno));
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_alternativeItemStack_freev(alternativeItemStackp);
  alternativeItemStackp = NULL;

 done:
  return alternativeItemStackp;  
}

/*****************************************************************************/
static inline void _marpaESLIF_alternativeItemStack_freev(genericStack_t *stackp)
/*****************************************************************************/
{
  int i;

  if (stackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(stackp); i++) {
      if (GENERICSTACK_IS_PTR(stackp, i)) {
        _marpaESLIF_alternativeItem_freev((marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(stackp, i));
      }
    }
    GENERICSTACK_FREE(stackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_rhsItem_freev(marpaESLIF_rhsItem_t *rhsItemp)
/*****************************************************************************/
{
  if (rhsItemp != NULL) {
    if (rhsItemp->singleSymbols != NULL) {
      free(rhsItemp->singleSymbols);
    }
    _marpaESLIF_grammarReference_freev(rhsItemp->grammarReferencep);
    free(rhsItemp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_rhsItem_t *_marpaESLIF_rhsItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_rhsItem_t *rhsItemOrigp)
/*****************************************************************************/
{
  marpaESLIF_rhsItem_t *rhsItemp = NULL;

  if (rhsItemOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "rhsItemOrigp is NULL");
    goto err;
  }

  rhsItemp = (marpaESLIF_rhsItem_t *) malloc(sizeof(marpaESLIF_rhsItem_t));
  if (rhsItemp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rhsItemp->singleSymbols      = NULL;
  rhsItemp->grammarReferencep  = NULL;

  if (rhsItemOrigp->singleSymbols != NULL) {
    rhsItemp->singleSymbols = strdup(rhsItemOrigp->singleSymbols);
    if (rhsItemp->singleSymbols == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  if (rhsItemOrigp->grammarReferencep != NULL) {
    rhsItemp->grammarReferencep = _marpaESLIF_grammarReference_clonep(marpaESLIFp, rhsItemOrigp->grammarReferencep);
    if (rhsItemp->grammarReferencep == NULL) {
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_rhsItem_freev(rhsItemp);
  rhsItemp = NULL;

 done:
  return rhsItemp;  
}

/*****************************************************************************/
static inline void _marpaESLIF_rhsItemStack_freev(genericStack_t *stackp)
/*****************************************************************************/
{
  int i;

  if (stackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(stackp); i++) {
      if (GENERICSTACK_IS_PTR(stackp, i)) {
        _marpaESLIF_rhsItem_freev((marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(stackp, i));
      }
    }
    GENERICSTACK_FREE(stackp);
  }
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_rhsItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *rhsItemStackOrigp)
/*****************************************************************************/
{
  genericStack_t       *rhsItemStackp = NULL;
  marpaESLIF_rhsItem_t *rhsItemp;
  int                  i;

  if (rhsItemStackOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "rhsItemStackOrigp is NULL");
    goto err;
  }

  GENERICSTACK_INIT(rhsItemStackp);
  if (GENERICSTACK_ERROR(rhsItemStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  for (i = 0; i < GENERICSTACK_USED(rhsItemStackOrigp); i++) {
    if (! GENERICSTACK_IS_PTR(rhsItemStackOrigp, i)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackOrigp->[%d] is not a PTR", i);
      goto err;
    }
    rhsItemp = _marpaESLIF_rhsItem_clonep(marpaESLIFp, (marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(rhsItemStackOrigp, i));
    if (rhsItemp == NULL) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rhsItemStackp, rhsItemp);
    if (GENERICSTACK_ERROR(rhsItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp push failure, %s", strerror(errno));
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_rhsItemStack_freev(rhsItemStackp);
  rhsItemStackp = NULL;

 done:
  return rhsItemStackp;  
}

/*****************************************************************************/
static inline void _marpaESLIF_grammarReference_freev(marpaESLIF_grammarReference_t *grammarReferencep)
/*****************************************************************************/
{
  if (grammarReferencep != NULL) {
    _marpaESLIF_string_freev(grammarReferencep->stringp);
    free(grammarReferencep);
  }
}

/*****************************************************************************/
static inline marpaESLIF_grammarReference_t *_marpaESLIF_grammarReference_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammarReference_t *grammarReferenceOrigp)
/*****************************************************************************/
{
  marpaESLIF_grammarReference_t *grammarReferencep = NULL;

  if (grammarReferenceOrigp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "grammarReferenceOrigp is NULL");
    goto err;
  }

  grammarReferencep = (marpaESLIF_grammarReference_t *) malloc(sizeof(marpaESLIF_grammarReference_t));
  if (grammarReferencep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarReferencep->stringp = NULL;
  grammarReferencep->leveli  = grammarReferenceOrigp->leveli;

  if (grammarReferenceOrigp->stringp != NULL) {
    grammarReferencep->stringp = _marpaESLIF_string_clonep(marpaESLIFp, grammarReferenceOrigp->stringp);
    if (grammarReferencep->stringp == NULL) {
      goto err;
    }
  }

  goto done;

 err:
  _marpaESLIF_grammarReference_freev(grammarReferencep);
  grammarReferencep = NULL;

 done:
  return grammarReferencep;  
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, marpaESLIF_string_t *descp, marpaESLIF_string_t *new_descp, short createb, marpaESLIF_grammar_t **out_grammarpp)
/*****************************************************************************/
{
  static const char           *funcs = "_marpaESLIFValueRuleCallbackGrammar_grammarb";
  marpaESLIF_t                *marpaESLIFp                = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp      = marpaESLIFValuep->marpaESLIFRecognizerp;
  genericStack_t              *grammarStackp              = marpaESLIF_grammarContextp->grammarStackp;
  marpaESLIFGrammar_t          marpaESLIFGrammar;
  marpaESLIF_grammar_t        *out_grammarp               = NULL;
  short                        rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* Make sure grammar at this level exist - the method _marpaESLIFGrammar_grammar_findp want a marpaESLIFGrammar_t in input -; */
  marpaESLIFGrammar.marpaESLIFp   = marpaESLIFp;
  marpaESLIFGrammar.grammarStackp = grammarStackp;
  marpaESLIFGrammar.grammarp      = NULL;
  
  out_grammarp = _marpaESLIFGrammar_grammar_findp(&marpaESLIFGrammar, op_declare, descp);
  if (out_grammarp == NULL) {
    if (! createb) {
      MARPAESLIF_ERROR(marpaESLIFp, "No such grammar");
      goto err;
    }
    out_grammarp = _marpaESLIF_grammar_newp(marpaESLIFp,
                                            NULL /* marpaWrapperGrammarOptionp */,
                                            op_declare,
                                            "UTF-8", (descp != NULL) ? descp->bytep : NULL, (descp != NULL) ? descp->bytel : 0,
                                            0 /* latmb */,
                                            NULL /* defaultSymbolActions */,
                                            NULL /* defaultRuleActions */,
                                            NULL, /* defaultDiscardEvents */
                                            0 /* defaultDiscardEventb */);
    if (out_grammarp == NULL) {
      goto err;
    }
    GENERICSTACK_SET_PTR(grammarStackp, out_grammarp, op_declare);
    if (GENERICSTACK_ERROR(grammarStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "grammarStackp set failure, %s", strerror(errno));
      goto err;
    }
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Created grammar at level %d", op_declare);
  } else {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Grammar at level %d already exist", op_declare);
    out_grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, op_declare);
  }

  /* Check/Update attributes */
  if (new_descp != NULL) {
    _marpaESLIF_string_freev(out_grammarp->descp);
    out_grammarp->descp = _marpaESLIF_string_clonep(marpaESLIFValuep->marpaESLIFp, new_descp);
    if (out_grammarp->descp == NULL) {
      goto err;
    }
  }
  
  if (out_grammarpp != NULL) {
    *out_grammarpp = out_grammarp;
  }
  
  rcb = 1;
  goto done;

 err:
  _marpaESLIF_grammar_freev(out_grammarp);
  out_grammarp = NULL;
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, char *asciis, marpaESLIF_symbol_t **out_symbolpp, marpaESLIF_grammar_t **out_grammarpp)
/*****************************************************************************/
{
  static const char           *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_metab";
  marpaESLIF_t                *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  char                        *currentasciis         = NULL;
  marpaESLIF_meta_t           *out_metap             = NULL;
  marpaESLIF_symbol_t         *out_symbolp           = NULL;
  marpaESLIF_grammar_t        *out_grammarp;
  short                        rcb;
  int                          nbdigiti;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (asciis == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No symbol name given");
    goto err;
  }
  
  /* Make sure grammar at this level exist */
  if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, &out_grammarp)) {
    goto err;
  }

  /* The symbol is NEVER created with the ascii description given, but ALWAYS in the form asciis{leveli}. The "{}" are not allowed in primary grammar */
  nbdigiti = _marpaESLIF_count_revifi(out_grammarp->leveli);
  currentasciis = (char *) malloc(strlen(asciis) + 3 /* "{} and NUL byte */ + nbdigiti);
  if (currentasciis == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  strcpy(currentasciis, asciis);
  sprintf(currentasciis + strlen(currentasciis), "{%d}", out_grammarp->leveli);

  /* Make sure this symbol at this level exist */
  out_symbolp = _marpaESLIF_symbol_findp(marpaESLIFp, out_grammarp, currentasciis, -1 /* symboli */);
  if (out_symbolp == NULL) {
    /* This is a symbol out of a meta symbol */
    out_metap = _marpaESLIF_meta_newp(marpaESLIFp, out_grammarp, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE, currentasciis, NULL /* descEncodings */, NULL /* descs */, 0 /* descl */);
    if (out_metap == NULL) {
      goto err;
    }
    out_symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (out_symbolp == NULL) {
      goto err;
    }
    out_symbolp->type       = MARPAESLIF_SYMBOL_TYPE_META;
    out_symbolp->u.metap    = out_metap;
    out_symbolp->idi        = out_metap->idi;
    out_symbolp->descp      = out_metap->descp;
    /* Meta is now in symbol */
    out_metap = NULL;
    /* Push symbol */
    GENERICSTACK_SET_PTR(out_grammarp->symbolStackp, out_symbolp, out_symbolp->u.metap->idi);
    if (GENERICSTACK_ERROR(out_grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "out_grammarp->symbolStackp set failure, %s", strerror(errno));
      goto err;
    }
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Created symbol No %d (%s) at level %d", out_symbolp->idi, out_symbolp->descp->asciis, op_declare);
  } else {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Found symbol No %d (%s) at level %d", out_symbolp->idi, out_symbolp->descp->asciis, op_declare);
  }

  if (out_grammarpp != NULL) {
    *out_grammarpp = out_grammarp;
  }
  if (out_symbolpp != NULL) {
    *out_symbolpp = out_symbolp;
  }

  rcb = 1;
  goto done;

 err:
  _marpaESLIF_meta_freev(out_metap);
  if (out_symbolp != NULL) {
    /* First remove shallow reference - per def out_grammarp is not NULL if out_symbolp is not NULL */
    GENERICSTACK_SET_NA(out_grammarp->symbolStackp, out_symbolp->idi);
    if (GENERICSTACK_ERROR(out_grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "out_grammarp->symbolStackp set failure, %s", strerror(errno));
      /* Really - bad luck - let grammar cleaner handle the cleanup */
      /* goto err; */
    } else {
      _marpaESLIF_symbol_freev(out_symbolp);
    }
    out_symbolp = NULL;
  }
  rcb = 0;

 done:
  if (currentasciis != NULL) {
    free(currentasciis);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIF_t *marpaESLIFp, genericStack_t *adverbItemStackp, char **actionsp, short *autorankbp, short *leftbp, short *rightbp, short *groupbp, char **separatorsp, short *properbp, int *rankip, short *nullRanksHighbp, int *priorityip, int *pauseip, char **eventsp, int *eventtypeip, short *eventinitbp, short *latmbp, marpaESLIF_string_t **namingpp)
/*****************************************************************************/
/* Semantics are special here: if a NULL pointer is given to an adverb, then we do NOT want this adverb to exist */
{
  marpaESLIF_adverbItem_t *adverbItemp;
  /* These items are exclusive */
  short                    leftb           = 0;
  short                    rightb          = 0;
  short                    groupb          = 0;
  /* Every item must appear once */
  int                      actionsi        = 0;
  int                      autorankbi      = 0;
  int                      leftbi          = 0;
  int                      rightbi         = 0;
  int                      groupbi         = 0;
  int                      separatorsi     = 0;
  int                      properbi        = 0;
  int                      rankii          = 0;
  int                      nullRanksHighbi = 0;
  int                      priorityii      = 0;
  int                      pauseii         = 0;
  int                      eventsi         = 0;
  int                      eventtypeii     = 0;
  int                      eventinitbi     = 0;
  int                      latmbi          = 0;
  int                      namingpi        = 0;
  int                      i;
  short                    rcb;

  if (adverbItemStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "adverbItemStackp is NULL");
    goto err;
  }

  /* Put the default values */
  if (actionsp != NULL) {
    *actionsp = NULL;
  }
  if (autorankbp != NULL) {
    *autorankbp = 0;
  }
  if (leftbp != NULL) {
    *leftbp = 1; /* Default is always left associativity */
  }
  if (rightbp != NULL) {
    *rightbp = 0;
  }
  if (groupbp != NULL) {
    *groupbp = 0;
  }
  if (separatorsp != NULL) {
    *separatorsp = NULL;
  }
  if (properbp != NULL) {
    *properbp = 0;
  }
  if (rankip != NULL) {
    *rankip = 0;
  }
  if (nullRanksHighbp != NULL) {
    *nullRanksHighbp = 0;
  }
  if (priorityip != NULL) {
    *priorityip = 0;
  }
  if (pauseip != NULL) {
    *pauseip = MARPAESLIF_EVENTTYPE_NONE;
  }
  if (eventsp != NULL) {
    *eventsp = NULL;
  }
  if (eventtypeip != NULL) {
    *eventtypeip = MARPAESLIF_EVENTTYPE_NONE; /* This must appear in the adverb list */
  }
  if (eventinitbp != NULL) {
    *eventinitbp = 1; /* Default is on for an event */
  }
  if (latmbp != NULL) {
    *latmbp = 0;
  }
  if (namingpp != NULL) {
    *namingpp = NULL;
  }

  for (i = 0; i < GENERICSTACK_USED(adverbItemStackp); i++) {
    if (! GENERICSTACK_IS_PTR(adverbItemStackp, i)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Item No %d of adverbItemStackp is NULL", i);
      goto err;
    }

    adverbItemp = (marpaESLIF_adverbItem_t *) GENERICSTACK_GET_PTR(adverbItemStackp, i);
    switch (adverbItemp->type) {
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION:
      if (actionsp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "action adverb is not allowed");
        goto err;
      }
      if (++actionsi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "action adverb is not unique");
        goto err;
      }
      *actionsp = adverbItemp->u.asciis;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK:
      if (autorankbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "autorank adverb is not allowed");
        goto err;
      }
      if (++autorankbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "autorank adverb is not unique");
        goto err;
      }
      *autorankbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT:
      if (leftbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "left adverb is not allowed");
        goto err;
      }
      if (++leftbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "left adverb is not unique");
        goto err;
      }
      leftb = *leftbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT:
      if (rightbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "right adverb is not allowed");
        goto err;
      }
      if (++rightbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "right adverb is not unique");
        goto err;
      }
      rightb = *rightbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP:
      if (groupbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "group adverb is not allowed");
        goto err;
      }
      if (++groupbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "group adverb is not unique");
        goto err;
      }
      groupb = *groupbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR:
      if (separatorsp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "separator adverb is not allowed");
        goto err;
      }
      if (++separatorsi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "separator adverb is not unique");
        goto err;
      }
      *separatorsp = adverbItemp->u.asciis;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER:
      if (properbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "proper adverb is not allowed");
        goto err;
      }
      if (++properbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "proper adverb is not unique");
        goto err;
      }
      *properbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK:
      if (rankip == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "rank adverb is not allowed");
        goto err;
      }
      if (++rankii > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "rank adverb is not unique");
        goto err;
      }
      *rankip = adverbItemp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING:
      if (nullRanksHighbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "null-ranking adverb is not allowed");
        goto err;
      }
      if (++nullRanksHighbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "null-ranking adverb is not unique");
        goto err;
      }
      *nullRanksHighbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY:
      if (priorityip == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "priority adverb is not allowed");
        goto err;
      }
      if (++priorityii > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "priority adverb is not unique");
        goto err;
      }
      *priorityip = adverbItemp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE:
      if (pauseip == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "pause adverb is not allowed");
        goto err;
      }
      if (++pauseii > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "pause adverb is not unique");
        goto err;
      }
      /* It is a non-sense to have a pause different than the ones known */
      if ((adverbItemp->u.i != MARPAESLIF_EVENTTYPE_BEFORE) &&
          (adverbItemp->u.i != MARPAESLIF_EVENTTYPE_AFTER)) {
        MARPAESLIF_ERROR(marpaESLIFp, "pause adverb must be 'before'  or 'after'");
        goto err;
      }
      *pauseip = adverbItemp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT:
      if (eventsp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "event adverb is not allowed");
        goto err;
      }
      if (++eventsi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "event adverb is not unique");
        goto err;
      }
      *eventsp = adverbItemp->u.asciis;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE:
      if (eventtypeip == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "event type adverb is not allowed");
        goto err;
      }
      if (++eventtypeii > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "event type adverb is not unique");
        goto err;
      }
      /* It is a non-sense to have an event type different than those known */
      if ((adverbItemp->u.i != MARPAESLIF_EVENTTYPE_COMPLETED) &&
          (adverbItemp->u.i != MARPAESLIF_EVENTTYPE_NULLED) &&
          (adverbItemp->u.i != MARPAESLIF_EVENTTYPE_PREDICTED)) {
        MARPAESLIF_ERROR(marpaESLIFp, "event type must be one of 'completed', 'nulled' or 'predicted'");
        goto err;
      }
      *eventtypeip = adverbItemp->u.i;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT:
      if (eventinitbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "event initial state adverb is not allowed");
        goto err;
      }
      if (++eventinitbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "event initial state adverb is not unique");
        goto err;
      }
      *eventinitbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM:
      if (latmbp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "latm or forgiving adverb is not allowed");
        goto err;
      }
      if (++latmbi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "latm adverb is not unique");
        goto err;
      }
      *latmbp = adverbItemp->u.b;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING:
      if (namingpp == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "name adverb is not allowed");
        goto err;
      }
      if (++namingpi > 1) {
        MARPAESLIF_ERROR(marpaESLIFp, "name adverb is not unique");
        goto err;
      }
      *namingpp = adverbItemp->u.stringp;
      break;
    case MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL:
      /* Ignored */
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported type %s (enum value %d) in adverbItemStackp", _marpaESLIF_grammarContext_i_types(marpaESLIFp, adverbItemStackp, i), adverbItemp->type);
      goto err;
    }
  }

  /* Pos-processing: leftb, rightb and groupb are mutually exclusive */
  if (leftb + rightb + groupb > 1) {
    MARPAESLIF_ERROR(marpaESLIFp, "assoc => 'left', assoc => 'right' and assoc => 'group' are mutually exclusive");
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
static inline short _marpaESLIFValueRuleCallbackGrammar_rhsItemb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammar_t *current_grammarp, marpaESLIF_rhsItem_t *rhsItemp, marpaESLIF_symbol_t **out_symbolpp, marpaESLIF_symbol_t **out_symbol_referencedpp, marpaESLIF_grammar_t **out_grammar_referencedpp)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_rhsItemb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIF_grammar_t          *out_grammar_referencedp;
  marpaESLIF_symbol_t           *out_symbolp;
  marpaESLIF_symbol_t           *out_symbol_referencedp;
  short                          rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (current_grammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "current_grammarp is NULL");
    goto err;
  }
  if  (rhsItemp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "rhsItemp is NULL");
    goto err;
  }

  /* Symbol is ALWAYS reworked if necessary in a form that is not allowed in input, i.e.: symbol<leveli0> */

  /* Always check this symbol in current grammar) */
  if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep, marpaESLIF_grammarContextp, current_grammarp->leveli, rhsItemp->singleSymbols, &out_symbolp, NULL /* out_grammarpp */)) {
    goto err;
  }

  if  (rhsItemp->grammarReferencep != NULL) {
    /* Check referenced grammar */
    if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, rhsItemp->grammarReferencep->leveli, rhsItemp->grammarReferencep->stringp, NULL /* new_descp */, 1 /* createb */, &out_grammar_referencedp)) {
      goto err;
    }
    /* And the symbol in this referenced grammar */
    if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep, marpaESLIF_grammarContextp, out_grammar_referencedp->leveli, rhsItemp->singleSymbols, &out_symbol_referencedp, NULL /* out_grammarpp */)) {
      goto err;
    }
  } else {
    /* No referenced grammar */
    out_grammar_referencedp = current_grammarp;
    out_symbol_referencedp = out_symbolp;
  }

  if (out_symbolpp != NULL) {
    *out_symbolpp = out_symbolp;
  }
  if (out_symbol_referencedpp != NULL) {
    *out_symbol_referencedpp = out_symbol_referencedp;
  }
  if (out_grammar_referencedpp != NULL) {
    *out_grammar_referencedpp = out_grammar_referencedp;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_rhsItemStackb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammar_t *current_grammarp, genericStack_t *rhsItemStackp, size_t *nrhslp, int **rhsipp)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_rhsItemStackb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  size_t                         nrhsl                 = 0;
  int                           *rhsip                 = NULL;
  int                           *rhstmpip;
  marpaESLIF_rhsItem_t          *rhsItemp;
  marpaESLIF_symbol_t           *out_symbolp;
  int                            i;
  short                          rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  if (rhsItemStackp != NULL) {
  
    /* Check every RHS */
    if (rhsItemStackp != NULL) {
      for (i = 0; i < GENERICSTACK_USED(rhsItemStackp); i++) {
        if (! GENERICSTACK_IS_PTR(rhsItemStackp, i)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp->[%d] is not a PTR", i);
          goto err;
        }
        rhsItemp = (marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(rhsItemStackp, i);

        if (! _marpaESLIFValueRuleCallbackGrammar_rhsItemb(marpaESLIFValuep, marpaESLIF_grammarContextp, current_grammarp, rhsItemp, &out_symbolp, NULL /* out_symbol_referencedpp */, NULL /* out_grammar_referencedpp */)) {
          goto err;
        }

        /* Create/expand rhsip */
        if (nrhsl <= 0) {
          rhsip = (int *) malloc(sizeof(int));
          if (rhsip == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
            goto err;
          }
          nrhsl = 1;
          rhsip[0] = out_symbolp->idi;
        } else {
          rhstmpip = realloc(rhsip, (nrhsl + 1) * sizeof(int));
          if (rhstmpip == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
            goto err;
          }
          rhsip = rhstmpip;
          rhsip[nrhsl++] = out_symbolp->idi;
        }
      }
    }

  }
      
  if (nrhslp != NULL) {
    *nrhslp = nrhsl;
  }
  if (rhsipp != NULL) {
    *rhsipp = rhsip;
  } else if (rhsip != NULL) {
    free(rhsip);
  }

  rcb = 1;
  goto done;

 err:
  if (rhsip != NULL) {
    free(rhsip);
  }
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, marpaESLIF_string_t *descp, char *lhsasciis, genericStack_t *rhsItemStackp, genericStack_t *rhsItemExceptionStackp, int ranki, short nullRanksHighb, short sequenceb, int minimumi, char *separators, short properb, char *actions, short passthroughb, marpaESLIF_rule_t **out_rulepp)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_ruleb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIF_rule_t             *out_rulep             = NULL;
  size_t                         nrhsl                 = 0;
  int                           *rhsip                 = NULL;
  size_t                         nrhsExceptionl        = 0;
  int                           *rhsExceptionip        = NULL;
  int                            separatori            = -1;
  marpaESLIF_symbol_t           *out_symbolp;
  marpaESLIF_symbol_t           *out_symbolseparatorp;
  marpaESLIF_grammar_t          *out_grammarp;
  short                          rcb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* Check LHS symbol */
  if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, lhsasciis, &out_symbolp, &out_grammarp)) {
    goto err;
  }

  /* Check RHS stack */
  if (! _marpaESLIFValueRuleCallbackGrammar_rhsItemStackb(marpaESLIFValuep, marpaESLIF_grammarContextp, out_grammarp, rhsItemStackp, &nrhsl, &rhsip)) {
    goto err;
  }

  /* Check Exception stack */
  if (! _marpaESLIFValueRuleCallbackGrammar_rhsItemStackb(marpaESLIFValuep, marpaESLIF_grammarContextp, out_grammarp, rhsItemExceptionStackp, &nrhsExceptionl, &rhsExceptionip)) {
    goto err;
  }

  /* Check separator */
  if (separators != NULL) {
    if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, separators, &out_symbolseparatorp, NULL /* out_grammarpp */)) {
      goto err;
    }
    separatori = out_symbolseparatorp->idi;
  }

  out_rulep = _marpaESLIF_rule_newp(marpaESLIFp,
                                    out_grammarp,
                                    (descp != NULL) ? descp->encodingasciis : NULL,
                                    (descp != NULL) ? descp->bytep : NULL,
                                    (descp != NULL) ? descp->bytel : 0,
                                    out_symbolp->idi,
                                    nrhsl,
                                    rhsip,
                                    nrhsExceptionl,
                                    rhsExceptionip,
                                    ranki,
                                    nullRanksHighb,
                                    sequenceb,
                                    minimumi,
                                    separatori,
                                    properb,
                                    actions,
                                    passthroughb);
  if (out_rulep == NULL) {
    goto err;
  }

  GENERICSTACK_SET_PTR(out_grammarp->ruleStackp, out_rulep, out_rulep->idi);
  if (GENERICSTACK_ERROR(out_grammarp->ruleStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
    goto err;
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Created rule No %d (%s) at level %d", out_rulep->idi, out_rulep->descp->asciis, out_grammarp->leveli);

  if (out_rulepp != NULL) {
    *out_rulepp = out_rulep;
  }
  rcb = 1;
  goto done;

 err:
  _marpaESLIF_rule_freev(out_rulep);
  rcb = 0;

 done:
  if (rhsip != NULL) {
    free(rhsip);
  }
  if (rhsExceptionip != NULL) {
    free(rhsExceptionip);
  }
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/* http://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c */
/*****************************************************************************/
static inline int _marpaESLIF_count_revifi(int n)
/*****************************************************************************/
{
  /* I do not imagine a nano-second one would repeat the LHS more than 9999999999 times in the RHS right ? */
  if (n < 0) {
    n = (n == INT_MIN) ? INT_MAX : -n;
  }

  if (n > 999999999) return 10;
  if (n > 99999999) return 9;
  if (n > 9999999) return 8;
  if (n > 999999) return 7;
  if (n > 99999) return 6;
  if (n > 9999) return 5;
  if (n > 999) return 4;
  if (n > 99) return 3;
  if (n > 9) return 2;

  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_lexemeDefaultb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, genericStack_t *adverbListStackp)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_lexemeDefaultb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                          rcb;
  marpaESLIF_grammar_t          *out_grammarp;
  /* Adverb items */
  char                          *actions;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* Get the adverb items that are allowed in our context */
  if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                       adverbListStackp,
                                                       &actions,
                                                       NULL, /* autorankbp */
                                                       NULL, /* leftbp */
                                                       NULL, /* rightbp */
                                                       NULL, /* groupbp */
                                                       NULL, /* separatorsp */
                                                       NULL, /* properbp */
                                                       NULL, /* rankip */
                                                       NULL, /* nullRanksHighbp */
                                                       NULL, /* priorityip */
                                                       NULL, /* pauseip */
                                                       NULL, /* eventsp */
                                                       NULL, /* eventtypeip */
                                                       NULL, /* eventinitbp */
                                                       NULL, /* latmbp */
                                                       NULL /* namingpp */)) {
    goto err;
  }

  /* Check and update grammar */
  if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, &out_grammarp)) {
    goto err;
  }
  /* :default rule must be unique on this grammar */
  if (++out_grammarp->nbupdateviaLexemei > 1) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "leeme default for grammar level %d (%s) must be unique", out_grammarp->leveli, out_grammarp->descp->asciis);
    goto err;
  }
  if (out_grammarp->defaultSymbolActions != NULL) {
    free(out_grammarp->defaultSymbolActions);
    out_grammarp->defaultSymbolActions = NULL;
  }
  if (actions != NULL) {
    out_grammarp->defaultSymbolActions = strdup(actions);
    if (out_grammarp->defaultSymbolActions == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFValueRuleCallbackGrammar_discardDefaultb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, genericStack_t *adverbListStackp)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValueRuleCallbackGrammar_discardDefaultb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                          rcb;
  marpaESLIF_grammar_t          *out_grammarp;
  /* Adverb items */
  char                          *events;
  short                          eventinitb;

  marpaESLIFRecognizerp->callstackCounteri++;
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  /* Get the adverb items that are allowed in our context */
  /* There is no notion of event type for a discard event: this is faked as an event completion in the Lexeme callback wrappers */
  if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                       adverbListStackp,
                                                       NULL, /* actionsp */
                                                       NULL, /* autorankbp */
                                                       NULL, /* leftbp */
                                                       NULL, /* rightbp */
                                                       NULL, /* groupbp */
                                                       NULL, /* separatorsp */
                                                       NULL, /* properbp */
                                                       NULL, /* rankip */
                                                       NULL, /* nullRanksHighbp */
                                                       NULL, /* priorityip */
                                                       NULL, /* pauseip */
                                                       &events, /* eventsp */
                                                       NULL, /* eventtypeip */
                                                       &eventinitb,
                                                       NULL, /* latmbp */
                                                       NULL /* namingpp */)) {
    goto err;
  }

  /* Check and update grammar */
  if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, &out_grammarp)) {
    goto err;
  }
  /* :default rule must be unique on this grammar */
  if (++out_grammarp->nbupdateviaDiscardi > 1) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "leeme default for grammar level %d (%s) must be unique", out_grammarp->leveli, out_grammarp->descp->asciis);
    goto err;
  }
  if (out_grammarp->defaultDiscardEvents != NULL) {
    free(out_grammarp->defaultDiscardEvents);
    out_grammarp->defaultDiscardEvents = NULL;
  }
  if (events != NULL) {
    out_grammarp->defaultDiscardEvents = strdup(events);
    if (out_grammarp->defaultDiscardEvents == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }
    out_grammarp->defaultDiscardEventb = eventinitb;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  marpaESLIFRecognizerp->callstackCounteri--;
  return rcb;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENTS(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statements ::= statement+
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENTS);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_01(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <start rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_01);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_02(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <desc rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_02);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_03(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <empty rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_03);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_04(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <null statement>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_04);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_05(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <statement group>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_05);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_06(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <priority rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_06);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_07(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <quantified rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_07);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_08(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <discard rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_08);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_09(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <default rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_09);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_10(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <lexeme default statement>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_10);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_11(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <discard default statement>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_11);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_12(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <lexeme rule>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_12);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_13(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <completion event declaration>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_13);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_14(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <nulled event declaration>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_14);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_15(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <prediction event declaration>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_15);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_16(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <inaccessible statement>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_16);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_17(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * statement ::= <exception statement>
   *
   * No-op
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_17);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "No op");
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_START_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <start rule> ::= ':start' <op declare> symbol
   *
   * Stack types:
   * NA ::= LEXEME OP_DECLARE SYMBOL
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t int void*
   *
   * Note: We push NA because parents rule are No-opts
   *       symbol is an ASCII NUL terminated string
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_START_RULE);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_SYMBOL(symbol);
    marpaESLIF_symbol_t *out_symbolp;
    marpaESLIF_grammar_t *out_grammarp;

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_SYMBOL(arg0i+2, symbol);

    /* Check and update symbol */
    if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, (char *) symbol, &out_symbolp, &out_grammarp)) {
      goto err;
    }
    /* :start rule must be unique */
    if (++out_symbolp->nbupdatei > 1) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, ":start for grammar level %d (%s) must be unique", out_grammarp->leveli, out_grammarp->descp->asciis);
      goto err;
    }
    out_symbolp->startb = 1;

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_DESC_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <desc rule> ::= ':desc' <op declare> <quoted string>
   *
   * Stack types:
   * NA ::= LEXEME OP_DECLARE LEXEME
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t int genericStackItemTypeArray_t
   *
   * Note: We push NA because parents rule are No-opts
   *       symbol is an ASCII NUL terminated string
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_DESC_RULE);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_LEXEME(quoted_string);
    marpaESLIF_string_t  *out_descp;

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_LEXEME(arg0i+2, quoted_string);

    out_descp = _marpaESLIF_string_newp(marpaESLIFp, "UTF-8"/* Our lexeme imposed character stream */, GENERICSTACK_ARRAY_PTR(quoted_string), GENERICSTACK_ARRAY_LENGTH(quoted_string), 1 /* asciib */);
    if (out_descp == NULL) {
      goto err;
    }

    /* Change grammar description - we cannot search on description since we are changing it... */
    if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, NULL /* descp */, out_descp /* new_descp */, 0 /* createb */, NULL /* out_grammarpp */)) {
      _marpaESLIF_string_freev(out_descp);
      goto err;
    }

    _marpaESLIF_string_freev(out_descp);

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_EMPTY_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <empty rule> ::= lhs <op declare> <adverb list>
   *
   * Stack types:
   * NA ::= LHS OP_DECLARE ADVERB_LIST
   *
   * C types:
   * -- ::= void* int void*
   *
   * Note: We push NA because parents rule are No-opts
   *       lhs is an ASCII NUL terminated string
   *       <adverb list> is a genericStack_t*
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_EMPTY_RULE);
  {
    CALLBACKGRAMMAR_DECL_LHS(lhs);
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    /* Allowed adbvers */
    char                   *actions;
    int                     ranki;
    short                   nullRanksHighb;
    marpaESLIF_string_t    *namingp;
    
    CALLBACKGRAMMAR_GET_LHS(arg0i, lhs);
    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+2, adverb_list);

    /* Get the adverb items that are allowed in our context */
    if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                         (genericStack_t *) adverb_list,
                                                         &actions,
                                                         NULL, /* autorankbp */
                                                         NULL, /* leftbp */
                                                         NULL, /* rightbp */
                                                         NULL, /* groupb */
                                                         NULL, /* separatorsp */
                                                         NULL, /* properbp */
                                                         &ranki,
                                                         &nullRanksHighb,
                                                         NULL, /* priorityip */
                                                         NULL, /* pauseip */
                                                         NULL, /* eventsp */
                                                         NULL, /* eventtypeip */
                                                         NULL, /* eventinitbp */
                                                         NULL, /* latmbp */
                                                         &namingp)) {
      goto err;
    }

    /* Create the rule */
    if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                    marpaESLIF_grammarContextp,
                                                    op_declare,
                                                    namingp,
                                                    (char *) lhs,
                                                    NULL, /* rhsItemStackp */
                                                    NULL, /* rhsItemExceptionStackp */
                                                    ranki,
                                                    nullRanksHighb,
                                                    0, /* sequenceb */
                                                    -1, /* minimumi */
                                                    NULL, /* separators */
                                                    -1, /* properb */
                                                    actions,
                                                    0, /* passthroughb */
                                                    NULL /* out_rulepp */)) {
      goto err;
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_NULL_STATEMENT(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <null statement> ::= ';'
   *
   * Stack types:
   * NA ::= LEXEME
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_NULL_STATEMENT);
  {
    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_STATEMENT_GROUP(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <statement group>              ::= '{' statements '}'
   *
   * Stack types:
   * NA ::= NA
   *
   * C types:
   * -- ::= --
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_STATEMENT_GROUP);
  {
    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_PRIORITY_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <priority rule> ::= lhs <op declare> priorities
   *
   * Stack types:
   * NA ::= LHS OP_DECLARE PRIORITIES
   *
   * C types:
   * -- ::= void* int void*
   *
   * Note: We push NA because parents rule are No-opts
   *       lhs is an ASCII NUL terminated string
   *       priorities is a stack of alternatives separated by the loosen operator
   *       priorities can never be NULL because it is defined as alternatives+
   *
   *       Alternatives is a stack of alternative, cannot be NULL because it is defined as alternative+
   *       Alternative is a stack of rhs, cannot be NULL because it is defined as rhs, itself being defined a <rhs primary>+
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_PRIORITY_RULE);
  {
    CALLBACKGRAMMAR_DECL_LHS(lhs);
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_PRIORITIES(priorities);
    genericStack_t               *alternativesStackp;
    genericStack_t               *alternativeStackp;
    marpaESLIF_alternativeItem_t *alternativeItemp;
    int                           priorityCounti;
    
    CALLBACKGRAMMAR_GET_LHS(arg0i, lhs);
    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_PRIORITIES(arg0i+2, priorities);

    alternativesStackp = (genericStack_t *) priorities;
    priorityCounti = GENERICSTACK_USED(alternativesStackp);  /* Number of times the loosen operator '||' appears */

    if (priorityCounti <= 1) {
      int alternativei;

      /* There is only one priority */
      if (! GENERICSTACK_IS_PTR(alternativesStackp, 0)) {
        MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "alternativesStackp->[0] is not a PTR");
        goto err;
      }
      alternativeStackp = (genericStack_t *) GENERICSTACK_IS_PTR(alternativesStackp, 0);
      /* These are normal alternatives, separated by '|' */
      for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
        /* Adverb items */
        char                *actions;
        int                  ranki;
        short                nullRanksHighb;
        marpaESLIF_string_t *namingp;

        if (! GENERICSTACK_IS_PTR(alternativeStackp, alternativei)) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "alternativesStackp->[%d] is not a PTR", alternativei);
          goto err;
        }
        alternativeItemp = (marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);

        /* Get the adverb items that are allowed in our context */
        if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                             alternativeItemp->adverbItemStackp,
                                                             &actions,
                                                             NULL, /* autorankbp */
                                                             NULL, /* leftbp */
                                                             NULL, /* rightbp */
                                                             NULL, /* groupb */
                                                             NULL, /* separatorsp */
                                                             NULL, /* properbp */
                                                             &ranki,
                                                             &nullRanksHighb,
                                                             NULL, /* priorityip */
                                                             NULL, /* pauseip */
                                                             NULL, /* eventsp */
                                                             NULL, /* eventtypeip */
                                                             NULL, /* eventinitbp */
                                                             NULL, /* latmbp */
                                                             &namingp)) {
          goto err;
        }

        /* Create the rule */
        if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                        marpaESLIF_grammarContextp,
                                                        op_declare,
                                                        namingp,
                                                        (char *) lhs,
                                                        alternativeItemp->rhsItemStackp, /* rhsItemStackp */
                                                        NULL, /* rhsItemExceptionStackp */
                                                        ranki,
                                                        0, /* nullRanksHighb - Note this cannot be a nullable as per our grammar */
                                                        0, /* sequenceb */
                                                        -1, /* minimumi */
                                                        NULL, /* separators */
                                                        -1, /* properb */
                                                        actions,
                                                        0, /* passthroughb */
                                                        NULL /* out_rulepp */)) {
          goto err;
        }
      }
    } else {
      { /* WORKSTACK BLOCK */
        genericStack_t *workStackp = NULL;
        char           *topasciis  = NULL;
        int             priority_ixi;
        short           workStackb;

        GENERICSTACK_NEW(workStackp);
        if (GENERICSTACK_ERROR(workStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "workStackp initialization failure, %s", strerror(errno));
          goto workstack_block_err;
        }

        /* Create a fixed version of top prioritized LHS: lhs[0] */
        topasciis = (char *) malloc(strlen((char *) lhs) + 3 /* "[0]" */ + 1 /* NUL byte */);
        if (topasciis == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto workstack_block_err;
        }
        strcpy(topasciis, (char *) lhs);
        strcat(topasciis, "[0]");

        /* There is more than one set of alternatives, each of them separated by the loosen '||' operator */
        /* First we reassociate all rules with its priority */
        for (priority_ixi = 0; priority_ixi < priorityCounti; priority_ixi++) {
          int priorityi;
          int alternativei;

          priorityi = priorityCounti - (priority_ixi + 1);

          /* Get set of alternatives at indice priority_ixi */
          if (! GENERICSTACK_IS_PTR(alternativesStackp, priority_ixi)) {
            MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "alternativesStackp->[%d] is not a PTR", priority_ixi);
            goto workstack_block_err;
          }
          alternativeStackp = (genericStack_t *) GENERICSTACK_IS_PTR(alternativesStackp, priority_ixi);
          for (alternativei = 0; alternativei < GENERICSTACK_USED(alternativeStackp); alternativei++) {
            if (! GENERICSTACK_IS_PTR(alternativeStackp, alternativei)) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "alternativeStackp->[%d] is not a PTR", alternativei);
              goto workstack_block_err;
            }
            alternativeItemp = (marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(alternativeStackp, alternativei);

            /* Set the priority of this alternative and push to the work stack */
            alternativeItemp->priorityi = priorityi;
            GENERICSTACK_PUSH_PTR(workStackp, alternativeItemp);
            if (GENERICSTACK_ERROR(workStackp)) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "workStackp push failure, %s", strerror(errno));
              goto workstack_block_err;
            }
          }
        }

        /* ------------------------------------------------------------------ */
        /* Create the first rule: lhs ::= lhs[0]                              */
        /* ------------------------------------------------------------------ */
        { /* PRIORITIZED_RHS BLOCK */
          short prioritizedRhsb;

          { /* RHSITEMSTACKP BLOCK */
            genericStack_t       *rhsItemStackp = NULL;
            marpaESLIF_rhsItem_t *rhsItemp;
            short                 rhsItemStackb;

            GENERICSTACK_NEW(rhsItemStackp);
            if (GENERICSTACK_ERROR(rhsItemStackp)) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "rhsItemStackp initialization failure, %s", strerror(errno));
              goto rhsItemStack_block_err;
            }

            rhsItemp = (marpaESLIF_rhsItem_t *) malloc(sizeof(marpaESLIF_rhsItem_t));
            if (rhsItemp == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto rhsItemStack_block_err;
            }
            rhsItemp->singleSymbols      = NULL;
            rhsItemp->grammarReferencep  = NULL; /* i.e. current grammar */

            rhsItemp->singleSymbols = strdup(topasciis);
            if (rhsItemp->singleSymbols == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
              goto rhsItemStack_block_err;
            }

            GENERICSTACK_PUSH_PTR(rhsItemStackp, rhsItemp);
            if (GENERICSTACK_ERROR(rhsItemStackp)) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "rhsItemStackp push failure, %s", strerror(errno));
              goto rhsItemStack_block_err;
            }
            rhsItemp = NULL; /* rhsItemp is now in rhsItemStackp */
      
            if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                            marpaESLIF_grammarContextp,
                                                            op_declare,
                                                            NULL, /* descp */
                                                            (char *) lhs,
                                                            rhsItemStackp,
                                                            NULL, /* rhsItemExceptionStackp */
                                                            0, /* ranki */
                                                            0, /* nullRanksHighb - a prioritized rule is never a nullable anyway */
                                                            0, /* sequenceb */
                                                            -1, /* minimumi */
                                                            NULL, /* separators */
                                                            -1, /* properb */
                                                            NULL, /* actions - c.f. comment about passthrough in marpaESLIF.c */
                                                            1, /* passthroughb */
                                                            NULL /* out_rulepp */)) {
              goto rhsItemStack_block_err;
            }

            rhsItemStackb = 1;
            goto rhsItemStack_block_done;
          rhsItemStack_block_err:
            rhsItemStackb = 0;
          rhsItemStack_block_done:
            _marpaESLIF_rhsItem_freev(rhsItemp);
            _marpaESLIF_rhsItemStack_freev(rhsItemStackp);
            if (! rhsItemStackb) {
              goto prioritizedRhs_block_err;
            }
          } /* RHSITEMSTACK BLOCK */

          prioritizedRhsb = 1;
          goto prioritizedRhs_block_done;
        prioritizedRhs_block_err:
          prioritizedRhsb = 0;
        prioritizedRhs_block_done:
          if (! prioritizedRhsb) {
            goto workstack_block_err;
          }
        }  /* PRIORITIZED_RHS BLOCK */

        /* ------------------------------------------------------------------ */
        /* Create the prioritized alternatives                                */
        /* ------------------------------------------------------------------ */
        { /* WORKSTACK LOOP BLOCK */
          char                 *currentasciis      = NULL;
          char                 *nextasciis         = NULL;
          genericStack_t       *rhsItemStackClonep = NULL;
          int                  *arityip            = NULL;
          int                   workingRulei;
          short                 workStackLoopb;
          marpaESLIF_grammar_t *current_grammarp;
          int                   nbdigiti;

          /* We will need the current grammar */
          if (! GENERICSTACK_IS_PTR(marpaESLIF_grammarContextp->grammarStackp, op_declare)) {
            MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIF_grammarContextp->grammarStackp->[%d] is not a PTR", op_declare);
            goto workStackLoop_block_err;
          }
          current_grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIF_grammarContextp->grammarStackp, op_declare);
          if (current_grammarp == NULL) {
            MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "current_grammarp is NULL");
            goto workStackLoop_block_err;
          }
          
          for (workingRulei = 0; workingRulei < GENERICSTACK_USED(workStackp); workingRulei++) {
            int                   arityi;
            int                   priorityi;
            marpaESLIF_rhsItem_t *rhsItemp;
            marpaESLIF_grammar_t *resolved_grammarp;
            int                   rhsItemStacki;
            int                   rhsLengthi;
            int                   nextPriorityi;
            int                   rhsixi;
            int                   arityixi;
            /* Adverb items */
            char                 *actions;
            short                 leftb;
            short                 rightb;
            short                 groupb;
            int                   ranki;
            short                 nullRanksHighb;
            marpaESLIF_string_t  *namingp;
                
            if (! GENERICSTACK_IS_PTR(workStackp, workingRulei)) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "workStackp->[%d] is not a PTR", workingRulei);
              goto workStackLoop_block_err;
            }
            alternativeItemp = (marpaESLIF_alternativeItem_t *) GENERICSTACK_GET_PTR(workStackp, workingRulei);
            /* Arity is the number of times the LHS appear in the list of RHS */
            if (alternativeItemp->rhsItemStackp == NULL) {
              MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "alternativeItemp->rhsItemStackp is NULL");
              goto workStackLoop_block_err;
            }
            priorityi = alternativeItemp->priorityi;

            /* Clone rhsItem stack */
            _marpaESLIF_rhsItemStack_freev(rhsItemStackClonep);
            rhsItemStackClonep = _marpaESLIF_rhsItemStack_clonep(marpaESLIFp, alternativeItemp->rhsItemStackp);
            if (rhsItemStackClonep == NULL) {
              goto err;
            }

            /* Look for arity */
            arityi = 0;
            if (arityip != NULL) {
              free(arityip);
            }
            arityip = (int *) malloc(rhsLengthi * sizeof(int));
            if (arityip == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto workStackLoop_block_err;
            }
            rhsLengthi = GENERICSTACK_USED(alternativeItemp->rhsItemStackp);
            for (rhsItemStacki = 0; rhsItemStacki < rhsLengthi; rhsItemStacki++) {
              if (! GENERICSTACK_IS_PTR(alternativeItemp->rhsItemStackp, rhsItemStacki)) {
                MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "alternativeItemp->rhsItemStackp->[%d] is not a PTR", rhsItemStacki);
                goto workStackLoop_block_err;
              }
              rhsItemp = (marpaESLIF_rhsItem_t *) GENERICSTACK_GET_PTR(alternativeItemp->rhsItemStackp, rhsItemStacki);
              /* Make sure we are comparing with a symbol at the same grammar */
              if (_marpaESLIF_resolveGrammarb(marpaESLIFp,
                                              marpaESLIF_grammarContextp->grammarStackp,
                                              current_grammarp,
                                              rhsItemp->singleSymbols,
                                              0, /* lookupLevelDeltai */
                                              NULL, /* lookupGrammarStringp */
                                              &resolved_grammarp,
                                              NULL /* symbolpp */)) {
                arityip[arityi++] = 1;
              }
            }

            /* Rework LHS to be lhs[$priority] */
            nbdigiti = _marpaESLIF_count_revifi(priorityi);
            currentasciis = (char *) malloc(strlen((char *) lhs) + 3 /* "[] and NUL byte */ + nbdigiti);
            if (currentasciis == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto workStackLoop_block_err;
            }
            strcpy(currentasciis, (char *) lhs);
            sprintf(currentasciis + strlen(currentasciis), "[%d]", priorityi);

            /* Get the adverb items that are allowed in our context */
            if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                                 alternativeItemp->adverbItemStackp,
                                                                 &actions,
                                                                 NULL, /* autorankbp */
                                                                 &leftb,
                                                                 &rightb,
                                                                 &groupb,
                                                                 NULL, /* separatorsp */
                                                                 NULL, /* properb */
                                                                 &ranki,
                                                                 &nullRanksHighb,
                                                                 NULL, /* priorityip */
                                                                 NULL, /* pauseip */
                                                                 NULL, /* eventsp */
                                                                 NULL, /* eventtypeip */
                                                                 NULL, /* eventinitbp */
                                                                 NULL, /* latmbp */
                                                                 &namingp)) {
              goto workStackLoop_block_err;
            }

            /* Default assocativity is left */
            if ((! leftb) && (! rightb) && (! groupb)) {
              leftb = 1;
            }

            /* Get next priority */
            nextPriorityi = priorityi + 1;
            /* Original Marpa::R2 calculus is $next_priority = 0 if $next_priority >= $priority_count */
            /* And a comment says this is probably a misfeature that the author did not fix for backward */
            /* compatibility issues on a quite rare case. */
            if (nextPriorityi >= priorityCounti) {
              nextPriorityi = priorityi;
            }

            /* Generate next prioritized LHS */
            nbdigiti = _marpaESLIF_count_revifi(priorityi);
            nextasciis = (char *) malloc(strlen((char *) lhs) + 3 /* "[] and NUL byte */ + nbdigiti);
            if (nextasciis != NULL) {
              /* Because we reuse this variable */
              free(nextasciis);
            }
            if (nextasciis == NULL) {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
              goto workStackLoop_block_err;
            }
            strcpy(nextasciis, (char *) lhs);
            sprintf(nextasciis + strlen(nextasciis), "[%d]", priorityi);

            if (arityi <= 0) {
              /* No arity: the LHS does not appear in the RHS list */
              if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                              marpaESLIF_grammarContextp,
                                                              op_declare,
                                                              NULL, /* descp */
                                                              (char *) currentasciis,
                                                              rhsItemStackClonep,
                                                              NULL, /* rhsItemExceptionStackp */
                                                              ranki,
                                                              nullRanksHighb,
                                                              0, /* sequenceb */
                                                              -1, /* minimumi */
                                                              NULL, /* separators */
                                                              -1, /* properb */
                                                              actions,
                                                              0, /* passthroughb */
                                                              NULL /* out_rulepp */)) {
                goto rhsItemStack_block_err;
              }
              continue;
            }
            
            if (arityi == 1) {
              if (rhsLengthi == 1) {
                /* Something like Expression ::= Expression in a prioritized rule -; */
                MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unnecessary unit rule %s in priority rule", (char *) lhs);
                goto workStackLoop_block_err;
              }
            }

            /* Do the association by reworking RHS's matching the LHS */
            for (arityixi = 0; arityixi < arityi; arityixi++) {
              rhsixi = arityip[arityixi];
              if (! GENERICSTACK_IS_PTR(rhsItemStackClonep, rhsixi)) {
                MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "rhsItemStackClonep->[%d] is not a PTR", arityip[0]);
                goto workStackLoop_block_err;
              }
              rhsItemp = (marpaESLIF_rhsItem_t *) GENERICSTACK_IS_PTR(rhsItemStackClonep, rhsixi);
              
              if (leftb) {
                rhsItemp->singleSymbols = (arityixi == 0)            ? strdup(currentasciis) : strdup(nextasciis);
              } else if (rightb) {
                rhsItemp->singleSymbols = (arityixi == (arityi - 1)) ? strdup(currentasciis) : strdup(nextasciis);
              } else if (groupb) {
                rhsItemp->singleSymbols = strdup(topasciis);
              } else {
                /* Should never happen */
                MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "No association !?");
                goto workStackLoop_block_err;
              }
            }
            
            if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                            marpaESLIF_grammarContextp,
                                                            op_declare,
                                                            NULL, /* descp */
                                                            (char *) currentasciis,
                                                            rhsItemStackClonep,
                                                            NULL, /* rhsItemExceptionStackp */
                                                            ranki,
                                                            nullRanksHighb,
                                                            0, /* sequenceb */
                                                            -1, /* minimumi */
                                                            NULL, /* separators */
                                                            -1, /* properb */
                                                            actions,
                                                            0, /* passthroughb */
                                                            NULL /* out_rulepp */)) {
              goto rhsItemStack_block_err;
            }
          }

          workStackLoopb = 1;
          goto workStackLoop_block_done;
        workStackLoop_block_err:
          workStackLoopb = 0;
        workStackLoop_block_done:
          if (currentasciis != NULL) {
            free(currentasciis);
          }
          if (nextasciis != NULL) {
            free(nextasciis);
          }
          if (arityip != NULL) {
            free(arityip);
          }
          _marpaESLIF_rhsItemStack_freev(rhsItemStackClonep);
          if (! workStackLoopb) {
            goto workstack_block_err;
          }
        }  /* WORKSTACK LOOP BLOCK */

        workStackb = 1;
        goto workstack_block_done;
      workstack_block_err:
        workStackb = 0;
      workstack_block_done:
        if (topasciis != NULL) {
          free(topasciis);
        }
        if (workStackp != NULL) {
          /* workStackp is a stack of shallow pointers of marpaESLIF_alternativeItem_t * */
          /* in which we just overwrote priority. we must NOT free the content of this stack. */
          GENERICSTACK_FREE(workStackp);
        }
        if (! workStackb) {
          goto err;
        }
      } /* WORKSTACK BLOCK */
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_QUANTIFIED_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <quantified rule> ::= lhs <op declare> <rhs primary> quantifier <adverb list>
   *
   * Stack types:
   * NA ::= LHS OP_DECLARE RHS_PRIMARY QUANTIFIER ADVERB_LIST
   *
   * C types:
   * -- ::= void* int void* int genericStack_t*
   *
   * Note: We push NA because parents rule are No-opts
   *       lhs is an ASCII NUL terminated string
   *       rhs primary is a pointer to a marpaESLIF_rhsItem_t
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_QUANTIFIED_RULE);
  {
    CALLBACKGRAMMAR_DECL_LHS(lhs);
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_RHS_PRIMARY(rhs_primary);
    CALLBACKGRAMMAR_DECL_QUANTIFIER(quantifier);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    genericStack_t       rhsItemStack;
    genericStack_t      *rhsItemStackp = &rhsItemStack;
    /* Adverb items */
    char                 *actions;
    char                 *separators;
    short                 properb;
    int                   ranki;
    short                 nullRanksHighb;
    marpaESLIF_string_t  *namingp;

    CALLBACKGRAMMAR_GET_LHS(arg0i, lhs);
    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_RHS_PRIMARY(arg0i+2, rhs_primary);
    CALLBACKGRAMMAR_GET_QUANTIFIER(arg0i+3, quantifier);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+4, adverb_list);

    /* Get the adverb items that are allowed in our context */
    if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                         (genericStack_t *) adverb_list,
                                                         &actions,
                                                         NULL, /* autorankbp */
                                                         NULL, /* leftbp */
                                                         NULL, /* rightbp */
                                                         NULL, /* groupbp */
                                                         &separators,
                                                         &properb, /* properb */
                                                         &ranki,
                                                         &nullRanksHighb,
                                                         NULL, /* priorityip */
                                                         NULL, /* pauseip */
                                                         NULL, /* eventsp */
                                                         NULL, /* eventtypeip */
                                                         NULL, /* eventinitbp */
                                                         NULL, /* latmbp */
                                                         &namingp)) {
      goto err;
    }

    /* Create the rule */
    GENERICSTACK_INIT(rhsItemStackp);
    if (GENERICSTACK_ERROR(rhsItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp initialization failure, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rhsItemStackp, rhs_primary);
    if (GENERICSTACK_ERROR(rhsItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp push failure, %s", strerror(errno));
      GENERICSTACK_RESET(rhsItemStackp);
      goto err;
    }
    
    if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                    marpaESLIF_grammarContextp,
                                                    op_declare,
                                                    namingp,
                                                    (char *) lhs,
                                                    rhsItemStackp,
                                                    NULL, /* rhsItemExceptionStackp */
                                                    ranki,
                                                    nullRanksHighb,
                                                    1, /* sequenceb */
                                                    quantifier,
                                                    separators,
                                                    properb,
                                                    actions,
                                                    0, /* passthroughb */
                                                    NULL /* out_rulepp */)) {
      GENERICSTACK_RESET(rhsItemStackp);
      goto err;
    }
    
    GENERICSTACK_RESET(rhsItemStackp);

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_DISCARD_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <discard rule> ::= ':discard' <op declare> <rhs primary> <adverb list>
   *
   * Stack types:
   * NA ::= LEXEME OP_DECLARE RHS_PRIMARY ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t int marpaESLIF_rhsItem_t* genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   *       rhs primary is a pointer to a marpaESLIF_rhsItem_t
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_DISCARD_RULE);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_RHS_PRIMARY(rhs_primary);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    marpaESLIF_symbol_t *out_symbolp;
    marpaESLIF_rule_t   *out_rulep;
    genericStack_t       rhsItemStack;
    genericStack_t      *rhsItemStackp = &rhsItemStack;
    /* Adverb items */
    char *events;
    int   eventtypei;
    short eventinitb;

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_RHS_PRIMARY(arg0i+2, rhs_primary);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    /* Get the adverb items that are allowed in our context */
    if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                         (genericStack_t *) adverb_list,
                                                         NULL, /* actionsp */
                                                         NULL, /* autorankbp */
                                                         NULL, /* leftbp */
                                                         NULL, /* rightbp */
                                                         NULL, /* groupbp */
                                                         NULL, /* separatorsp */
                                                         NULL, /* properbp */
                                                         NULL, /* rankip */
                                                         NULL, /* nullRanksHighbp */
                                                         NULL, /* priorityip */
                                                         NULL, /* pauseip */
                                                         &events,
                                                         &eventtypei,
                                                         &eventinitb,
                                                         NULL, /* latmbp */
                                                         NULL /* namingpp */)) {
      goto err;
    }

    /* Create the rule */
    GENERICSTACK_INIT(rhsItemStackp);
    if (GENERICSTACK_ERROR(rhsItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp initialization failure, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rhsItemStackp, rhs_primary);
    if (GENERICSTACK_ERROR(rhsItemStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsItemStackp push failure, %s", strerror(errno));
      GENERICSTACK_RESET(rhsItemStackp);
      goto err;
    }

    /* There is a trick in the recognizer when a discard sub-grammar finishes ok. It looks to the */
    /* grammar of the parent recognizer and check if there is an action for the matched top symbol */
    /* that got a discard... therefore the discard "event" is in reality a completion "action" on the discard rule */
    /* It is not a hazard that action adverb is not allowed here -; */
    /* Please note that when we execute a sub-grammar, ALL events are disabled - this is why it is guaranteed */
    /* to happen only if the value of the sub-grammar is successful, and at return, within parent's scope. */
    if (! _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValuep,
                                                    marpaESLIF_grammarContextp,
                                                    op_declare,
                                                    NULL, /* namingp */
                                                    ":discard",
                                                    rhsItemStackp,
                                                    NULL, /* rhsItemExceptionStackp */
                                                    0, /* ranki */
                                                    0, /* nullRanksHighb */
                                                    0, /* sequenceb */
                                                    -1, /* quantifier */
                                                    NULL, /* separators */
                                                    -1, /* properb */
                                                    events, /* actions */
                                                    0, /* passthroughb */
                                                    &out_rulep)) {
      GENERICSTACK_RESET(rhsItemStackp);
      goto err;
    }
    
    GENERICSTACK_RESET(rhsItemStackp);

    /* Check and update symbol - grammar validator will make sure that only one symbol in a grammar can have the discard flag */
    if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep,
                                                    marpaESLIF_grammarContextp,
                                                    op_declare,
                                                    ":discard",
                                                    &out_symbolp,
                                                    NULL /* out_grammarpp */)) {
      goto err;
    }
    out_symbolp->discardb = 1;

    /* Update rule discard event */
    if (out_rulep->discardEvents != NULL) {
      free(out_rulep->discardEvents);
      out_rulep->discardEvents = NULL;
    }
    if (events != NULL) {
      out_rulep->discardEvents = strdup(events);
      if (out_rulep->discardEvents == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_DEFAULT_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <default rule> ::= ':default' <op declare> <adverb list>
   *
   * Stack types:
   * NA ::= LEXEME OP_DECLARE ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t int genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_DEFAULT_RULE);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    marpaESLIF_grammar_t *out_grammarp;
    /* Adverb items */
    char *actions;

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+2, adverb_list);

    /* Get the adverb items that are allowed in our context */
    if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                         (genericStack_t *) adverb_list,
                                                         &actions,
                                                         NULL, /* autorankbp */
                                                         NULL, /* leftbp */
                                                         NULL, /* rightbp */
                                                         NULL, /* groupbp */
                                                         NULL, /* separatorsp */
                                                         NULL, /* properbp */
                                                         NULL, /* rankip */
                                                         NULL, /* nullRanksHighbp */
                                                         NULL, /* priorityip */
                                                         NULL, /* pauseip */
                                                         NULL, /* eventsp */
                                                         NULL, /* eventtypeip */
                                                         NULL, /* eventinitbp */
                                                         NULL, /* latmbp */
                                                         NULL /* namingpp */)) {
      goto err;
    }

    /* Check and update grammar */
    if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, &out_grammarp)) {
      goto err;
    }
    /* :default rule must be unique on this grammar */
    if (++out_grammarp->nbupdateviaStarti > 1) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, ":default for grammar level %d (%s) must be unique", out_grammarp->leveli, out_grammarp->descp->asciis);
      goto err;
    }
    if (out_grammarp->defaultRuleActions != NULL) {
      free(out_grammarp->defaultRuleActions);
      out_grammarp->defaultRuleActions = NULL;
    }
    if (actions != NULL) {
      out_grammarp->defaultRuleActions = strdup(actions);
      if (out_grammarp->defaultRuleActions == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_LEXEME_DEFAULT_STATEMENT_1(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <lexeme default statement> ::= 'lexeme' 'default' '=' <adverb list>
   *
   * Stack types:
   * NA ::= LEXEME LEXEME LEXEME ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t genericStackItemTypeArray_t genericStackItemTypeArray_t genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_LEXEME_DEFAULT_STATEMENT_1);
  {
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    if (! _marpaESLIFValueRuleCallbackGrammar_lexemeDefaultb(marpaESLIFValuep, marpaESLIF_grammarContextp, 0 /* op_declare */, (genericStack_t *) adverb_list)) {
      goto err;
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_LEXEME_DEFAULT_STATEMENT_2(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <lexeme default statement> ::= 'lexeme' 'default' <op declare> <adverb list>
   *
   * Stack types:
   * NA ::= LEXEME LEXEME INT ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t genericStackItemTypeArray_t int genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_LEXEME_DEFAULT_STATEMENT_2);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+2, op_declare);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    if (! _marpaESLIFValueRuleCallbackGrammar_lexemeDefaultb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, (genericStack_t *) adverb_list)) {
      goto err;
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_DISCARD_DEFAULT_STATEMENT_1(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <discard default statement> ::= 'discard' 'default' '=' <adverb list>
   *
   * Stack types:
   * NA ::= DISCARD DISCARD DISCARD ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t genericStackItemTypeArray_t genericStackItemTypeArray_t genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_DISCARD_DEFAULT_STATEMENT_1);
  {
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    if (! _marpaESLIFValueRuleCallbackGrammar_discardDefaultb(marpaESLIFValuep, marpaESLIF_grammarContextp, 0 /* op_declare */, (genericStack_t *) adverb_list)) {
      goto err;
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_DISCARD_DEFAULT_STATEMENT_2(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <discard default statement> ::= 'discard' 'default' <op declare> <adverb list>
   *
   * Stack types:
   * NA ::= DISCARD DISCARD INT ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t genericStackItemTypeArray_t int genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_DISCARD_DEFAULT_STATEMENT_2);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+2, op_declare);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    if (! _marpaESLIFValueRuleCallbackGrammar_discardDefaultb(marpaESLIFValuep, marpaESLIF_grammarContextp, op_declare, (genericStack_t *) adverb_list)) {
      goto err;
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_LEXEME_RULE(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <lexeme rule>::= ':lexeme' <op declare> symbol <adverb list>
   *
   * Stack types:
   * NA ::= LEXEME OP_DECLARE SYMBOL ADVERB_LIST
   *
   * C types:
   * -- ::= genericStackItemTypeArray_t int char* genericStack_t *
   *
   * Note: We push NA because parents rule are No-opts
   *       symbol is an ASCII string
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_LEXEME_RULE);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_SYMBOL(symbol);
    CALLBACKGRAMMAR_DECL_ADVERB_LIST(adverb_list);
    marpaESLIF_grammar_t *out_grammarp;
    marpaESLIF_symbol_t *out_symbolp;
    /* Adverb items */
    int   priorityi;
    int  *pausei;
    char *events;
    short eventinitb;

    CALLBACKGRAMMAR_GET_OP_DECLARE(arg0i+1, op_declare);
    CALLBACKGRAMMAR_GET_SYMBOL(arg0i+2, symbol);
    CALLBACKGRAMMAR_GET_ADVERB_LIST(arg0i+3, adverb_list);

    /* Get the adverb items that are allowed in our context */
    /* There is no notion of event type for lexeme, this is transfered to the notion of pause "before" or "after" */
    if (! _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIFp,
                                                         (genericStack_t *) adverb_list,
                                                         NULL, /* actionsp */
                                                         NULL, /* autorankbp */
                                                         NULL, /* leftbp */
                                                         NULL, /* rightbp */
                                                         NULL, /* groupbp */
                                                         NULL, /* separatorsp */
                                                         NULL, /* properbp */
                                                         NULL, /* rankip */
                                                         NULL, /* nullRanksHighbp */
                                                         &priorityi,
                                                         &pausei,
                                                         &events,
                                                         NULL, /* eventtypeip */
                                                         &eventinitb,
                                                         NULL, /* latmbp */
                                                         NULL /* namingpp */)) {
      goto err;
    }

    /* Check and update symbol */
    if (! _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValuep,
                                                    marpaESLIF_grammarContextp,
                                                    op_declare,
                                                    (char *) symbol,
                                                    &out_symbolp,
                                                    &out_grammarp)) {
      goto err;
    }
    /* :lexeme rule must be unique on that symbol */
    if (++out_symbolp->nbupdatei > 1) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, ":lexeme on symbol %s for grammar level %d (%s) must be unique", (char *) symbol, out_grammarp->leveli, out_grammarp->descp->asciis);
      goto err;
    }
    out_symbolp->priorityi = 1;
    if (out_symbolp->pauses != NULL) {
      free(out_symbolp->pauses);
      out_symbolp->pauses = NULL;
    }
    if (pauses != NULL) {
      out_symbolp->pauses = strdup(pauses);
      if (out_symbolp->pauses == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
    }
    if (events != NULL) {
      out_symbolp->events = strdup(events);
      if (out_symbolp->events == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
    }

    /* Parents are no-op */
    CALLBACKGRAMMAR_SET_NA(resulti);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_OP_DECLARE_1(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <op declare> ::= <op declare top grammar>
   *
   * Stack types:
   * INT ::= LEXEME
   *
   * C types:
   * int ::= genericStackItemTypeArray_t
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_OP_DECLARE_1);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    op_declare = 0; /* No need to fetch the lexeme, we know it is for level 0 */
    CALLBACKGRAMMAR_SET_OP_DECLARE(resulti, op_declare);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_OP_DECLARE_3(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /* ---------------------------------------------------------------------------
   * <op declare> ::= <op declare any grammar>
   *
   * Stack types:
   * INT ::= LEXEME
   *
   * C types:
   * int ::= genericStackItemTypeArray_t
   * ------------------------------------------------------------------------- */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_OP_DECLARE_3);
  {
    CALLBACKGRAMMAR_DECL_OP_DECLARE(op_declare);
    CALLBACKGRAMMAR_DECL_LEXEME    (op_declare_any_grammar);
    char *p;

    CALLBACKGRAMMAR_GET_LEXEME(arg0i, op_declare_any_grammar);

    /* Get the grammar level */
    /* By construction this cannot be < 0 - we made sure the string is ASCII (via the grammar) and NUL terminated (via the symbol callback) */
    p = GENERICSTACK_ARRAY_PTR(op_declare_any_grammar);
    p += 2;    /* Skip ":[" */
    op_declare = atoi(p);

    CALLBACKGRAMMAR_SET_OP_DECLARE(resulti, op_declare);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_ACTION_NAME(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /*
   * ---------------------------------------------------------------------------------------------
   * <action name>  ::= <ascii name>
   *
   * Stack types:
   * PTR ::= LEXEME
   *
   * C types:
   * void* ::= genericStackItemTypeArray_t
   *
   * Note: <ascii name>  inner pointer is an ASCII NUL terminated string: ASCII as per the grammar, NUL terminated as per charconv
   *       <action name> is an ASCII NUL terminated string
   * ---------------------------------------------------------------------------------------------
   */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_ACTION_NAME);
  {
    CALLBACKGRAMMAR_DECL_ACTION_NAME(action_name);
    CALLBACKGRAMMAR_DECL_LEXEME     (ascii_name);

    CALLBACKGRAMMAR_GET_LEXEME(arg0i, ascii_name);

    action_name = strdup(GENERICSTACK_ARRAY_PTR(ascii_name));
    if (action_name == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    CALLBACKGRAMMAR_SET_ACTION_NAME(resulti, action_name);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_ACTION(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /*
   * ---------------------------------------------------------------------------------------------
   * action ::= 'action' '=>' <action name>
   *
   * Stack types:
   * PTR ::= LEXEME LEXEME PTR
   *
   * C types:
   * void* ::= genericStackItemTypeArray_t genericStackItemTypeArray_t void*
   *
   * Note: <action name> is an ASCII NUL terminated string
   *       <action> is an ASCII NUL terminated string
   * ---------------------------------------------------------------------------------------------
   */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_ACTION);
  {
    CALLBACKGRAMMAR_DECL_ACTION     (action);
    CALLBACKGRAMMAR_DECL_ACTION_NAME(action_name);

    CALLBACKGRAMMAR_GET_ACTION_NAME(arg0i+2, action_name);

    action = strdup(action_name);
    if (action == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    CALLBACKGRAMMAR_SET_ACTION(resulti, action);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_ADVERB_ITEM_01(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /*
   * ---------------------------------------------------------------------------------------------
   * <adverb item> ::= action
   *
   * Stack types:
   * PTR ::= PTR
   *
   * C types:
   * void* ::= void*
   *
   * Note: <action>      is an ASCII NUL terminated string
   *       <adverb item> is an ASCII NUL terminated string
   * ---------------------------------------------------------------------------------------------
   */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_ADVERB_ITEM_01);
  {
    CALLBACKGRAMMAR_DECL_ADVERB_ITEM_ACTION(adverb_item_action);
    CALLBACKGRAMMAR_DECL_ACTION            (action);

    CALLBACKGRAMMAR_GET_ACTION(arg0i, action);

    adverb_item_action = strdup(action);
    if (adverb_item_action == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    CALLBACKGRAMMAR_SET_ADVERB_ITEM_ACTION(resulti, adverb_item_action);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_ADVERB_ITEM_12(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /*
   * ---------------------------------------------------------------------------------------------
   * <adverb item> ::= <latm specification>
   *
   * Stack types:
   * SHORT ::= SHORT
   *
   * C types:
   * short ::= short
   * ---------------------------------------------------------------------------------------------
   */
  CALLBACKGRAMMAR_COMMON_HEADER(_G1_RULE_ADVERB_ITEM_12);
  {
    CALLBACKGRAMMAR_DECL_ADVERB_ITEM_LATM(adverb_item_latm);
    CALLBACKGRAMMAR_DECL_LATM            (latm);

    CALLBACKGRAMMAR_GET_LATM(arg0i, latm);

    adverb_item_latm = latm;

    CALLBACKGRAMMAR_SET_ADVERB_ITEM_LATM(resulti, adverb_item_latm);
  }
  CALLBACKGRAMMAR_COMMON_TRAILER;
}

/*****************************************************************************/
static inline short _G1_RULE_ADVERB_LIST_ITEMS(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  /*
   * ---------------------------------------------------------------------------------------------
   * <adverb list items> ::= <adverb item>*
   *
   * Stack types:
   * PTR ::= ~variable types see below~
   *
   * C types:
   * PTR ::= short
   *
   * Note: <adverb list items> is a genericStack pointer
   *       <adverb item> can be any of the supported adverb items
   * ---------------------------------------------------------------------------------------------
   */
}

