#ifndef MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H
#define MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H

/* Management of grammarContext is veyr generic, almost all code is generated with macros */

typedef struct  marpaESLIF_grammarContext   marpaESLIF_grammarContext_t;
typedef enum    marpaESLIF_grammarItemType  marpaESLIF_grammarItemType_t;
typedef struct  marpaESLIF_adverbItem       marpaESLIF_adverbItem_t;
typedef struct  marpaESLIF_alternativeItem  marpaESLIF_alternativeItem_t;
typedef struct  marpaESLIF_rhsItem          marpaESLIF_rhsItem_t;
typedef struct  marpaESLIF_grammarReference marpaESLIF_grammarReference_t;

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
  MARPAESLIF_GRAMMARITEMTYPE_QUANTIFIER,
  MARPAESLIF_GRAMMARITEMTYPE_PRIORITIES,
  MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVES,
  MARPAESLIF_GRAMMARITEMTYPE_ALTERNATIVE,
  MARPAESLIF_GRAMMARITEMTYPE_RHS,
  MARPAESLIF_GRAMMARITEMTYPE_GRAMMAR_REFERENCE,
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

struct marpaESLIF_alternativeItem {
  genericStack_t *rhsItemStackp;
  genericStack_t *adverbItemStackp;
};

struct marpaESLIF_rhsItem {
  char *singleSymbols;
  marpaESLIF_grammarReference_t *grammarReferencep;
};

struct marpaESLIF_grammarReference {
  marpaESLIF_string_t *stringp;
  int                  leveli;
};

#define GENERATE_MARPAESLIF_GRAMMARCONTEXT_GETTER_BODY(genericStackType, itemType, name) \
  static const char                  *funcs = "_marpaESLIF_grammarContext_get_" #name "b"; \
  marpaESLIF_grammarItemType_t        type;                             \
  short                               rcb;                              \
  marpaESLIF_grammarContext_##name##_t value;                            \
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
    MARPAESLIF_ERRORF(marpaESLIFp, "Type is not %s (enum value %d) in itemTypeStackp->[%d], but %s (enum value %d)", #itemType, MARPAESLIF_GRAMMARITEMTYPE_##itemType, i, _marpaESLIF_grammarContext_i_types(marpaESLIFp, itemTypeStackp, i), type); \
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

static const char *marpaESLIF_grammarContext_NA_types = "NA";
static const char *marpaESLIF_grammarContext_UNKNOWN_types = "UNKNOWN";
#define MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(genericStackType, itemType, name) \
  typedef GENERICSTACKITEMTYPE2TYPE_##genericStackType marpaESLIF_grammarContext_##name##_t; \
  static const char *marpaESLIF_grammarContext_##itemType##_types = #itemType; \
  static inline short _marpaESLIF_grammarContext_get_##name##b(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarContext_##name##_t *valuep) \
  {                                                                     \
    GENERATE_MARPAESLIF_GRAMMARCONTEXT_GETTER_BODY(genericStackType, itemType, name); \
  }                                                                     \
  static inline short _marpaESLIF_grammarContext_set_##name##b(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarContext_##name##_t value) \
  {                                                                     \
    GENERATE_MARPAESLIF_GRAMMARCONTEXT_SETTER_BODY(genericStackType, itemType, name); \
  }

/* The macros generating getters and setters must know about these methods: */
static inline void        _marpaESLIF_grammarContext_resetv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp);
static inline short       _marpaESLIF_grammarContext_i_resetb(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i);
static inline const char *_marpaESLIF_grammarContext_i_types(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i);
static inline short       _marpaESLIF_grammarContext_get_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t *typep);
static inline short       _marpaESLIF_grammarContext_set_typeb(marpaESLIF_t *marpaESLIFp, genericStack_t *itemTypeStackp, int i, marpaESLIF_grammarItemType_t type);

/* Special version that is putting N/A : it has nothing in input - and there is no getter counterpart */
static inline short       _marpaESLIF_grammarContext_set_NAb(marpaESLIF_t *marpaESLIFp, genericStack_t *outputStackp, genericStack_t *itemTypeStackp, int i)
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

MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(ARRAY, LEXEME,                   lexeme)                   /* C: { void*, size_t } */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   OP_DECLARE,               op_declare)               /* C: int */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ACTION_NAME,              action_name)              /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ACTION,                   action)                   /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_ACTION,       adverb_item_action)       /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_AUTORANK,     adverb_item_autorank)     /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_LEFT,         adverb_item_left)         /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_RIGHT,        adverb_item_right)        /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_GROUP,        adverb_item_group)        /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_SEPARATOR,    adverb_item_separator)    /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_PROPER,       adverb_item_proper)       /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_RANK,         adverb_item_rank)         /* C: int */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_NULL_RANKING, adverb_item_null_ranking) /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_PRIORITY,     adverb_item_priority)     /* C: int */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_PAUSE,        adverb_item_pause)        /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_LATM,         adverb_item_latm)         /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_NAMING,       adverb_item_naming)       /* C: void* (NUL terminated for convenience) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_NULL,         adverb_item_null)         /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST_ITEMS,        adverb_list_items)        /* C: genericStack_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST,              adverb_list)              /* C: genericStack_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, LATM,                     latm)                     /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL_NAME,              symbol_name)              /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL,                   symbol)                   /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   LHS,                      lhs)                      /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SINGLE_SYMBOL,            single_symbol)            /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, QUANTIFIER,               quantifier)               /* 0 == '*', 1 == '+' */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   PRIORITIES,               priorities)               /* C: genericStack_t* of alternatives */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ALTERNATIVES,             alternatives)             /* C: genericStack_t* of alternative */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ALTERNATIVE,              alternative)              /* C: genericStack_t* of marpaESLIF_alternativeItem_t */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   RHS,                      rhs)                      /* C: genericStack_t* of marpaESLIF_rhsItem_t */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   GRAMMAR_REFERENCE,        grammar_reference)        /* C: genericStack_t* of marpaESLIF_grammarReference_t */

/* Special methods that must know about the types generated by the macros: */
static inline short _marpaESLIFValueRuleCallbackGrammar_op_declareb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, short createb, marpaESLIF_grammar_t **out_grammarpp);
static inline short _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, char *ansis, marpaESLIF_symbol_t **out_symbolpp);
static inline short _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIF_t *marpaESLIFp, genericStack_t *adverbListStackp, char *contexts, char **actionsp, short *autorankbp, short *leftbp, short *rightbp, short *groupbp, char **separatorsp, short *properbp, int *rankip, short *nullRanksHighbp, int *priorityip, char **pausesp, short *latmbp, marpaESLIF_string_t **namingpp);
static inline void  _marpaESLIF_adverbItemStack_freev(genericStack_t *stackp);
static inline void  _marpaESLIF_alternativeItem_freev(marpaESLIF_alternativeItem_t *alternativeItemp);
static inline void  _marpaESLIF_alternativeItemStack_freev(genericStack_t *stackp);
static inline void  _marpaESLIF_rhsItem_freev(marpaESLIF_rhsItem_t *rhsItemp);
static inline void  _marpaESLIF_rhsItemStack_freev(genericStack_t *stackp);
static inline void  _marpaESLIF_grammarReference_freev(marpaESLIF_grammarReference_t *grammarReferencep);

/* For every rule we write a specific action. Every action is isually very short, and just pop/push the stack */
/* For convenience action names mimic the rule names */
static inline short _G1_RULE_STATEMENTS       (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_01     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_02     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_03     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_04     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_05     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_06     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_07     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_08     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_09     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_10     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_11     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_12     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_13     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_14     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_15     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_16     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_17     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_START_RULE       (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DESC_RULE        (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_EMPTY_RULE       (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_NULL_STATEMENT   (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_GROUP  (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_PRIORITY_RULE    (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);

static inline short _G1_RULE_OP_DECLARE_3     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_OP_DECLARE_1     (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ACTION_NAME      (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ACTION           (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_ITEM_01   (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_ITEM_12   (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_LIST_ITEMS(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);

#endif /* MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H */
