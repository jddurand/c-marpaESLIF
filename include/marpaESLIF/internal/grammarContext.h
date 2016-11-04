#ifndef MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H
#define MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H

/* Management of grammarContext is veyr generic, almost all code is generated with macros */

typedef struct  marpaESLIF_grammarContext  marpaESLIF_grammarContext_t;
typedef enum    marpaESLIF_grammarItemType marpaESLIF_grammarItemType_t;
typedef struct  marpaESLIF_adverbItem      marpaESLIF_adverbItem_t;

/* Internal structure to have value context information */
/* This is used in the grammar generation context */
/* We maintain in parallel thress stacks:
   - the outputStack as per Marpa,
   - a description of what is at every indice of this outputStack
   - grammars
/* Grammar themselves are in grammarStackp */
struct marpaESLIF_grammarContext {
  genericStack_t              outputStack; /* This stack is temporary: GENERICSTACK_INIT() */
  genericStack_t             *outputStackp;
  genericStack_t              itemTypeStack; /* This stack is temporary: GENERICSTACK_INIT() */
  genericStack_t             *itemTypeStackp;
  genericStack_t             *grammarStackp; /* This stack will have to survive if success: GENERICSTACK_NEW() */
  marpaESLIF_grammar_t       *current_grammarp;
};

enum marpaESLIF_grammarItemType {
  MARPAESLIF_GRAMMARITEMTYPE_NA = 0,
  MARPAESLIF_GRAMMARITEMTYPE_LEXEME,
  MARPAESLIF_GRAMMARITEMTYPE_OP_DECLARE,
  MARPAESLIF_GRAMMARITEMTYPE_ACTION_NAME,
  MARPAESLIF_GRAMMARITEMTYPE_ACTION,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_ACTION,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_AUTORANK,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LEFT,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RIGHT,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_GROUP,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_SEPARATOR,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PROPER,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_RANK,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL_RANKING,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PRIORITY,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_PAUSE,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_LATM,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NAMING,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_NULL,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST_ITEMS,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_LIST,
  MARPAESLIF_GRAMMARITEMTYPE_LATM,
  MARPAESLIF_GRAMMARITEMTYPE_SYMBOL_NAME,
  MARPAESLIF_GRAMMARITEMTYPE_SYMBOL,
  MARPAESLIF_GRAMMARITEMTYPE_LHS,
  MARPAESLIF_GRAMMARITEMTYPE_SINGLE_SYMBOL,
  MARPAESLIF_GRAMMARITEMTYPE_QUANTIFIER
};

struct marpaESLIF_adverbItem {
  marpaESLIF_grammarItemType_t type;
  union {
    /* Four raw types possible */
    char                *asciis;
    marpaESLIF_string_t *stringp;
    int                  i;
    short                b;
  } u;
};

static inline void        _marpaESLIF_grammarContext_resetv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp);
static inline short       _marpaESLIF_grammarContext_i_resetb(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i);
static inline const char *_marpaESLIF_grammarContext_i_types(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i);
static inline short       _marpaESLIF_grammarContext_get_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t *typep);
static inline short       _marpaESLIF_grammarContext_set_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t type);

#define GENERATE_MARPAESLIF_GRAMMARCONTEXT_GETTER_BODY(genericStackType, itemType, name) \
  static const char                  *funcs = "_marpaESLIF_grammarContext_get_" #name "b"; \
  marpaESLIF_grammarItemType_t        type;                             \
  short                               rcb;                              \
  marpESLIF_grammarContext_##name##_t value;                            \
                                                                        \
  if (! _marpaESLIF_grammarContext_get_typeb(marpaESLIFp, itemTypeStackp, i, &type)) { \
    goto err;                                                           \
  }                                                                     \
                                                                        \
  if (type == MARPAESLIF_GRAMMARITEMTYPE_##itemType) {                  \
    if (GENERICSTACK_IS_##genericStackType(outputStackp, i)) {          \
      value = GENERICSTACK_GET_##genericStackType(outputStackp, i);     \
    } else {                                                            \
      MARPAESLIF_ERRORF(marpaESLIFp, "Not a %s in outputStackp at indice %d", #genericStackType, i); \
      goto err;                                                         \
    }                                                                   \
  } else {                                                              \
    MARPAESLIF_ERRORF(marpaESLIFp, "Not a MARPAESLIF_GRAMMARITEMTYPE_%s in itemTypeStackp at indice %d, got %d instead of %d", #itemType, i, type, MARPAESLIF_GRAMMARITEMTYPE_##itemType); \
    goto err;                                                           \
  }                                                                     \
                                                                        \
  if (valuep != NULL) {                                                 \
    *valuep = value;                                                    \
  }                                                                     \
  rcb = 1;                                                              \
  goto done;                                                            \
                                                                        \
err:                                                                    \
 rcb = 0;                                                               \
                                                                        \
done:                                                                   \
 return rcb

#define GENERATE_MARPAESLIF_GRAMMARCONTEXT_SETTER_BODY(genericStackType, itemType, name) \
  static const char *funcs = "_marpaESLIF_grammarContext_set_" #name "b"; \
  short              rcb;                                               \
                                                                        \
  if (! _marpaESLIF_grammarContext_i_resetb(marpaESLIFp, outputStackp, itemTypeStackp, i)) { \
    goto err;                                                           \
  }                                                                     \
                                                                        \
  GENERICSTACK_SET_INT(itemTypeStackp, MARPAESLIF_GRAMMARITEMTYPE_##itemType, i); \
  if (GENERICSTACK_ERROR(itemTypeStackp)) {                             \
    MARPAESLIF_ERRORF(marpaESLIFp, "itemTypeStackp set failure, %s", strerror(errno)); \
    goto err;                                                           \
  }                                                                     \
  GENERICSTACK_SET_##genericStackType(outputStackp, value, i);          \
  if (GENERICSTACK_ERROR(outputStackp)) {                               \
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp set failure, %s", strerror(errno)); \
    goto err;                                                           \
  }                                                                     \
                                                                        \
  rcb = 1;                                                              \
  goto done;                                                            \
                                                                        \
err:                                                                    \
 rcb = 0;                                                               \
                                                                        \
done:                                                                   \
 return rcb

static const char *marpESLIF_grammarContext_NA_types = "NA";
static const char *marpESLIF_grammarContext_UNKNOWN_types = "UNKNOWN";
#define MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(genericStackType, itemType, name) \
  typedef GENERICSTACKITEMTYPE2TYPE_##genericStackType marpESLIF_grammarContext_##name##_t; \
  static const char *marpESLIF_grammarContext_##itemType##_types = #itemType; \
  static inline short _marpaESLIF_grammarContext_get_##name##b(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i, marpESLIF_grammarContext_##name##_t *valuep) \
  {                                                                     \
    GENERATE_MARPAESLIF_GRAMMARCONTEXT_GETTER_BODY(genericStackType, itemType, name); \
  }                                                                     \
  static inline short _marpaESLIF_grammarContext_set_##name##b(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i, marpESLIF_grammarContext_##name##_t value) \
  {                                                                     \
    GENERATE_MARPAESLIF_GRAMMARCONTEXT_SETTER_BODY(genericStackType, itemType, name); \
  }

/* Special version that is putting N/A : it has nothing in input - and there is no getter counterpart */
static inline short _marpaESLIF_grammarContext_set_NAb(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i)
{
  static const char *funcs = "_marpaESLIF_grammarContext_set_NAb";
  short              rcb;

  if (! _marpaESLIF_grammarContext_i_resetb(marpaESLIFp, outputStackp, itemTypeStackp, i)) {
    goto err;
  }

  GENERICSTACK_SET_INT(itemTypeStackp, MARPAESLIF_GRAMMARITEMTYPE_NA, i);
  if (GENERICSTACK_ERROR(itemTypeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "itemTypeStackp set failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_SET_NA(outputStackp, i);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp set failure, %s", strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;

  err:
  rcb = 0;

  done:
  return rcb;
}

MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(ARRAY, LEXEME,                   lexeme)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   OP_DECLARE,               op_declare)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ACTION_NAME,              action_name)              /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ACTION,                   action)                   /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_ACTION,       adverb_item_action)       /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_AUTORANK,     adverb_item_autorank)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_LEFT,         adverb_item_left)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_RIGHT,        adverb_item_right)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_GROUP,        adverb_item_group)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_SEPARATOR,    adverb_item_separator)    /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_PROPER,       adverb_item_proper)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_RANK,         adverb_item_rank)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_NULL_RANKING, adverb_item_null_ranking)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_PRIORITY,     adverb_item_priority)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_PAUSE,        adverb_item_pause)        /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_LATM,         adverb_item_latm)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_NAMING,       adverb_item_naming)       /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_NULL,         adverb_item_null)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST_ITEMS,        adverb_list_items)        /* Stack */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST,              adverb_list)              /* Stack */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, LATM,                     latm)
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL_NAME,              symbol_name)              /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL,                   symbol)                   /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   LHS,                      lhs)                      /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SINGLE_SYMBOL,            single_symbol)            /* ASCII NUL terminated string */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   QUANTIFIER,               quantifier)               /* 0 == '*', 1 == '+' */

#endif /* MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H */
