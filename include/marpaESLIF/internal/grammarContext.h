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
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENT,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTTYPE,
  MARPAESLIF_GRAMMARITEMTYPE_ADVERB_ITEM_EVENTINIT,
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
  MARPAESLIF_GRAMMARITEMTYPE_RHS_PRIMARY
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
  int             priorityi;   /* Used only when dealing with prioritized rules */
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
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_PAUSE,        adverb_item_pause)        /* C: int */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_EVENT,        adverb_item_event)        /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(INT,   ADVERB_ITEM_EVENTTYPE,    adverb_item_eventtype)    /* C: int */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_EVENTINIT,    adverb_item_eventinit)    /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_LATM,         adverb_item_latm)         /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_ITEM_NAMING,       adverb_item_naming)       /* C: void* (NUL terminated for convenience) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, ADVERB_ITEM_NULL,         adverb_item_null)         /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST_ITEMS,        adverb_list_items)        /* C: genericStack_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ADVERB_LIST,              adverb_list)              /* C: genericStack_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, LATM,                     latm)                     /* C: short */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL_NAME,              symbol_name)              /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SYMBOL,                   symbol)                   /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   LHS,                      lhs)                      /* C: void* (ASCII NUL terminated string) */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   SINGLE_SYMBOL,            single_symbol)            /* C: marpaESLIF_rhsItem_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(SHORT, QUANTIFIER,               quantifier)               /* 0 == '*', 1 == '+' */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   PRIORITIES,               priorities)               /* C: genericStack_t* of alternatives stacks */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ALTERNATIVES,             alternatives)             /* C: genericStack_t* of alternative stacks */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   ALTERNATIVE,              alternative)              /* C: genericStack_t* of marpaESLIF_alternativeItem_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   RHS,                      rhs)                      /* C: genericStack_t* of marpaESLIF_rhsItem_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   GRAMMAR_REFERENCE,        grammar_reference)        /* C: genericStack_t* of marpaESLIF_grammarReference_t* */
MARPAESLIF_INTERNAL_GRAMMARCONTEXT_DEFINE_ACCESSORS(PTR,   RHS_PRIMARY,              rhs_primary)              /* C: marpaESLIF_rhsItem_t* */

/* Getters and setters on the stack are hand-writen */
#define CALLBACKGRAMMAR_COMMON_HEADER(name)                             \
  static const char           *funcs                 = #name ;          \
  marpaESLIF_t                *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp; \
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp; \
  genericStack_t              *outputStackp          = marpaESLIF_grammarContextp->outputStackp; \
  genericStack_t              *itemTypeStackp        = marpaESLIF_grammarContextp->itemTypeStackp; \
  short                        rcb;                                     \
  marpaESLIFRecognizerp->callstackCounteri++;                           \
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start")

#define CALLBACKGRAMMAR_COMMON_TRAILER                                  \
  rcb = 1;                                                              \
  goto done;                                                            \
err:                                                                    \
 rcb = 0;                                                               \
done:                                                                   \
 MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb); \
 marpaESLIFRecognizerp->callstackCounteri--;                            \
 return rcb

#ifndef MARPAESLIF_NTRACE
#define CALLBACKGRAMMAR_HEXDUMPV(marpaESLIFRecognizerp, headers, asciidescs, p, lengthl, traceb) \
  MARPAESLIF_HEXDUMPV(marpaESLIFRecognizerp, headers, asciidescs, p, lengthl, traceb)
#else
#define CALLBACKGRAMMAR_HEXDUMPV(marpaESLIFRecognizerp, headers, asciidescs, p, lengthl, traceb)
#endif

/* -------------------------------------------------------------------- */
/*                              LEXEME                                  */
/* genericStack type: ARRAY                                             */
/*            C type: genericStackItemTypeArray_t                       */
/*                                                                      */
/* There is only a getter: the G1 level does never produce a lexeme.    */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_LEXEME(identifier) marpaESLIF_grammarContext_lexeme_t identifier
#define CALLBACKGRAMMAR_GET_LEXEME(indice, identifier) do {              \
    if (! _marpaESLIF_grammarContext_get_lexemeb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (GENERICSTACK_ARRAY_PTR(identifier) == NULL) {                   \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, #identifier " is a null lexeme" MARPAESLIF_LOC_FMT, MARPAESLIF_LOC_VAR); \
      goto err;                                                         \
    }                                                                   \
    CALLBACKGRAMMAR_HEXDUMPV(marpaESLIFValuep->marpaESLIFRecognizerp, "Dump of ", #identifier, GENERICSTACK_ARRAY_PTR(identifier), GENERICSTACK_ARRAY_LENGTH(identifier), 1 /* traceb */); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                                NA                                    */
/* genericStack type: NA                                                */
/*            C type: --                                                */
/*                                                                      */
/* There is only a setter: NA is a no-op knowing parent rules are noop. */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_SET_NA(indice) do {                             \
    if (! _marpaESLIF_grammarContext_set_NAb(marpaESLIFp, outputStackp, itemTypeStackp, indice)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to NA", indice); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           OP_DECLARE                                 */
/* genericStack type: INT                                               */
/*            C type: int                                               */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_OP_DECLARE(identifier) marpaESLIF_grammarContext_op_declare_t identifier
#define CALLBACKGRAMMAR_GET_OP_DECLARE(indice, identifier) do { \
    if (! _marpaESLIF_grammarContext_get_op_declareb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier < 0) {                                               \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, #identifier "value is %d", identifier); \
      goto err;                                                         \
    }                                                                   \
    if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, identifier, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, NULL /* out_grammarpp */)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_OP_DECLARE(indice, identifier) do {        \
    if (! _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValuep, marpaESLIF_grammarContextp, identifier, NULL /* descp */, NULL /* new_descp */, 1 /* createb */, NULL /* out_grammarpp */)) { \
      goto err;                                                         \
    }                                                                   \
    if (! _marpaESLIF_grammarContext_set_op_declareb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ACTION_NAME                                */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ACTION_NAME(identifier) marpaESLIF_grammarContext_action_name_t identifier
#define CALLBACKGRAMMAR_GET_ACTION_NAME(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_action_nameb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ACTION_NAME(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_action_nameb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                              ACTION                                  */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ACTION(identifier) marpaESLIF_grammarContext_action_t identifier
#define CALLBACKGRAMMAR_GET_ACTION(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_actionb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ACTION(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_actionb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                          ADVERB_ITEM_ACTION                          */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_ACTION(identifier) marpaESLIF_grammarContext_adverb_item_action_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_ACTION(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_actionb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_ACTION(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_actionb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                         ADVERB_ITEM_AUTORANK                         */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_AUTORANK(identifier) marpaESLIF_grammarContext_adverb_item_autorank_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_AUTORANK(indice, identifier) do { \
    if (! _marpaESLIF_grammarContext_get_adverb_item_autorankb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_AUTORANK(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_autorankb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ADVERB_ITEM_LEFT                           */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_LEFT(identifier) marpaESLIF_grammarContext_adverb_item_left_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_LEFT(indice, identifier) do {   \
    if (! _marpaESLIF_grammarContext_get_adverb_item_leftb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_LEFT(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_leftb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ADVERB_ITEM_RIGHT                          */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_RIGHT(identifier) marpaESLIF_grammarContext_adverb_item_right_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_RIGHT(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_rightb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_RIGHT(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_rightb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ADVERB_ITEM_GROUP                          */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_GROUP(identifier) marpaESLIF_grammarContext_adverb_item_group_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_GROUP(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_groupb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_GROUP(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_groupb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        ADVERB_ITEM_SEPARATOR                         */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_SEPARATOR(identifier) marpaESLIF_grammarContext_adverb_item_separator_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_SEPARATOR(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_separatorb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_SEPARATOR(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_separatorb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ADVERB_ITEM_PROPER                         */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_PROPER(identifier) marpaESLIF_grammarContext_adverb_item_proper_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_PROPER(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_properb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_PROPER(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_properb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_ITEM_RANK                          */
/* genericStack type: INT                                               */
/*            C type: int                                               */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_RANK(identifier) marpaESLIF_grammarContext_adverb_item_rank_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_RANK(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_rankb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_RANK(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_rankb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        ADVERB_ITEM_NULL_RANKING                      */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_NULL_RANKING(identifier) marpaESLIF_grammarContext_adverb_item_null_ranking_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_NULL_RANKING(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_null_rankingb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_NULL_RANKING(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_null_rankingb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                          ADVERB_ITEM_PRIORITY                        */
/* genericStack type: INT                                               */
/*            C type: int                                               */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_PRIORITY(identifier) marpaESLIF_grammarContext_adverb_item_priority_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_PRIORITY(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_priorityb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_PRIORITY(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_priorityb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                          ADVERB_ITEM_PAUSE                           */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_PAUSE(identifier) marpaESLIF_grammarContext_adverb_item_pause_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_PAUSE(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_pauseb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is 0x%x", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_PAUSE(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_pauseb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to 0x%x", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                          ADVERB_ITEM_EVENT                           */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_EVENT(identifier) marpaESLIF_grammarContext_adverb_item_event_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_EVENT(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_eventb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_EVENT(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_eventb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        ADVERB_ITEM_EVENTTYPE                         */
/* genericStack type: INT                                               */
/*            C type: int                                               */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_EVENTTYPE(identifier) marpaESLIF_grammarContext_adverb_item_eventtype_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_EVENTTYPE(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_eventtypeb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is 0x%x", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_EVENTTYPE(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_eventtypeb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to 0x%x", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        ADVERB_ITEM_EVENTINIT                         */
/* genericStack type: INT                                               */
/*            C type: int                                               */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_EVENTINIT(identifier) marpaESLIF_grammarContext_adverb_item_eventinit_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_EVENTINIT(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_adverb_item_eventinitb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_EVENTINIT(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_adverb_item_eventinitb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_ITEM_LATM                          */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_LATM(identifier) marpaESLIF_grammarContext_adverb_item_latm_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_LATM(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_latmb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_LATM(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_latmb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_ITEM_NAMING                        */
/* genericStack type: PTR                                               */
/*            C type: void * (UTF-8 string)                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_NAMING(identifier) marpaESLIF_grammarContext_adverb_item_naming_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_NAMING(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_namingb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_NAMING(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_namingb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_ITEM_NULL                          */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_ITEM_NULL(identifier) marpaESLIF_grammarContext_adverb_item_null_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_ITEM_NULL(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_item_nullb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_ITEM_NULL(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_item_nullb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_LIST_ITEMS                         */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_LIST_ITEMS(identifier) marpaESLIF_grammarContext_adverb_list_items_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_LIST_ITEMS(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_list_itemsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_LIST_ITEMS(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_list_itemsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ADVERB_LIST                               */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ADVERB_LIST(identifier) marpaESLIF_grammarContext_adverb_list_t identifier
#define CALLBACKGRAMMAR_GET_ADVERB_LIST(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_adverb_listb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ADVERB_LIST(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_adverb_listb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                               LATM                                   */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_LATM(identifier) marpaESLIF_grammarContext_latm_t identifier
#define CALLBACKGRAMMAR_GET_LATM(indice, identifier) do {               \
    if (! _marpaESLIF_grammarContext_get_latmb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_LATM(indice, identifier) do {               \
    if (! _marpaESLIF_grammarContext_set_latmb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                             SYMBOL_NAME                              */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_SYMBOL_NAME(identifier) marpaESLIF_grammarContext_symbol_name_t identifier
#define CALLBACKGRAMMAR_GET_SYMBOL_NAME(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_symbol_nameb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_SYMBOL_NAME(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_symbol_nameb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                                SYMBOL                                */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_SYMBOL(identifier) marpaESLIF_grammarContext_symbol_t identifier
#define CALLBACKGRAMMAR_GET_SYMBOL(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_symbolb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_SYMBOL(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_symbolb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                                 LHS                                  */
/* genericStack type: PTR                                               */
/*            C type: void * (ASCII NUL terminated string)              */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_LHS(identifier) marpaESLIF_grammarContext_lhs_t identifier
#define CALLBACKGRAMMAR_GET_LHS(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_lhsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is \"%s\"", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_LHS(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_lhsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to \"%s\"", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            SINGLE_SYMBOL                             */
/* genericStack type: PTR                                               */
/*            C type: marpaESLIF_rhsItem_t*                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_SINGLE_SYMBOL(identifier) marpaESLIF_grammarContext_single_symbol_t identifier
#define CALLBACKGRAMMAR_GET_SINGLE_SYMBOL(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_get_single_symbolb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_SINGLE_SYMBOL(indice, identifier) do {        \
    if (! _marpaESLIF_grammarContext_set_single_symbolb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                               QUANTIFIER                             */
/* genericStack type: SHORT                                             */
/*            C type: short                                             */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_QUANTIFIER(identifier) marpaESLIF_grammarContext_quantifier_t identifier
#define CALLBACKGRAMMAR_GET_QUANTIFIER(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_quantifierb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %d", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_QUANTIFIER(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_quantifierb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %d", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            PRIORITIES                                */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_PRIORITIES(identifier) marpaESLIF_grammarContext_priorities_t identifier
#define CALLBACKGRAMMAR_GET_PRIORITIES(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_prioritiesb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_PRIORITIES(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_prioritiesb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                           ALTERNATIVES                               */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ALTERNATIVES(identifier) marpaESLIF_grammarContext_alternatives_t identifier
#define CALLBACKGRAMMAR_GET_ALTERNATIVES(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_alternativesb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ALTERNATIVES(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_alternativesb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                            ALTERNATIVE                               */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_ALTERNATIVE(identifier) marpaESLIF_grammarContext_alternative_t identifier
#define CALLBACKGRAMMAR_GET_ALTERNATIVE(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_alternativeb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_ALTERNATIVE(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_alternativeb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                                RHS                                   */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_RHS(identifier) marpaESLIF_grammarContext_rhs_t identifier
#define CALLBACKGRAMMAR_GET_RHS(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_rhsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_RHS(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_rhsb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        GRAMMAR_REFERENCE                             */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_GRAMMAR_REFERENCE(identifier) marpaESLIF_grammarContext_grammar_reference_t identifier
#define CALLBACKGRAMMAR_GET_GRAMMAR_REFERENCE(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_grammar_referenceb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_GRAMMAR_REFERENCE(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_grammar_referenceb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* -------------------------------------------------------------------- */
/*                        RHS_PRIMARY                             */
/* genericStack type: PTR                                               */
/*            C type: void * (genericStack_t *)                         */
/* -------------------------------------------------------------------- */
#define CALLBACKGRAMMAR_DECL_RHS_PRIMARY(identifier) marpaESLIF_grammarContext_rhs_primary_t identifier
#define CALLBACKGRAMMAR_GET_RHS_PRIMARY(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_get_rhs_primaryb(marpaESLIFp, outputStackp, itemTypeStackp, indice, &identifier)) { \
      goto err;                                                         \
    }                                                                   \
    if (identifier == NULL) {                                           \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, #identifier "value is NULL"); \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] val  is %p", indice, (int) identifier); \
  } while (0)

#define CALLBACKGRAMMAR_SET_RHS_PRIMARY(indice, identifier) do {  \
    if (! _marpaESLIF_grammarContext_set_rhs_primaryb(marpaESLIFp, outputStackp, itemTypeStackp, indice, identifier)) { \
      goto err;                                                         \
    }                                                                   \
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "outputStackp->[%d] set  to %p", indice, (int) identifier); \
  } while (0)

/* Special methods that must know about the types generated by the macros: */
static inline void                     _marpaESLIF_adverbItem_freev(marpaESLIF_adverbItem_t *adverbItemp);
static inline marpaESLIF_adverbItem_t *_marpaESLIF_adverbItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_adverbItem_t *adverbItemOrigp);

static inline void            _marpaESLIF_adverbItemStack_freev(genericStack_t *adverbItemStackp);
static inline genericStack_t *_marpaESLIF_adverbItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *adverbItemStackOrigp);

static inline void                          _marpaESLIF_alternativeItem_freev(marpaESLIF_alternativeItem_t *alternativeItemp);
static inline marpaESLIF_alternativeItem_t *_marpaESLIF_alternativeItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_alternativeItem_t *alternativeItemOrigp);

static inline void            _marpaESLIF_alternativeItemStack_freev(genericStack_t *stackp);
static inline genericStack_t *_marpaESLIF_alternativeItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *alternativeItemStackOrigp);

static inline void                  _marpaESLIF_rhsItem_freev(marpaESLIF_rhsItem_t *rhsItemp);
static inline marpaESLIF_rhsItem_t *_marpaESLIF_rhsItem_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_rhsItem_t *rhsItemOrigp);

static inline void            _marpaESLIF_rhsItemStack_freev(genericStack_t *stackp);
static inline genericStack_t *_marpaESLIF_rhsItemStack_clonep(marpaESLIF_t *marpaESLIFp, genericStack_t *rhsItemStackOrigp);

static inline void                           _marpaESLIF_grammarReference_freev(marpaESLIF_grammarReference_t *grammarReferencep);
static inline marpaESLIF_grammarReference_t *_marpaESLIF_grammarReference_clonep(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammarReference_t *grammarReferenceOrigp);

static inline short _marpaESLIFValueRuleCallbackGrammar_grammarb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, marpaESLIF_string_t *descp, marpaESLIF_string_t *new_descp, short createb, marpaESLIF_grammar_t **out_grammarpp);
static inline short _marpaESLIFValueRuleCallbackGrammar_metab(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, char *ansis, marpaESLIF_symbol_t **out_symbolpp, marpaESLIF_grammar_t **out_grammarpp);
static inline short _marpaESLIF_grammarContext_adverbList_unstackb(marpaESLIF_t *marpaESLIFp, genericStack_t *adverbListStackp, char **actionsp, short *autorankbp, short *leftbp, short *rightbp, short *groupbp, char **separatorsp, short *properbp, int *rankip, short *nullRanksHighbp, int *priorityip, int *pauseip, char **eventsp, int *eventtypeip, short *eventinitbp, short *latmbp, marpaESLIF_string_t **namingpp);
static inline short _marpaESLIFValueRuleCallbackGrammar_rhsItemb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammar_t *current_grammarp, marpaESLIF_rhsItem_t *rhsItemp, marpaESLIF_symbol_t **out_symbolpp, marpaESLIF_symbol_t **out_symbol_referencepp, marpaESLIF_grammar_t **out_grammar_referencepp);
static inline short _marpaESLIFValueRuleCallbackGrammar_rhsItemStackb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammar_t *current_grammarp, genericStack_t *rhsItemStackp, size_t *nrhslp, int **rhsipp);
static inline short _marpaESLIFValueRuleCallbackGrammar_ruleb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, marpaESLIF_string_t *descp, char *lhsasciis, genericStack_t *rhsItemStackp, genericStack_t *rhsItemExceptionStackp, int ranki, short nullRanksHighb, short sequenceb, int minimumi, char *separators, short properb, char *actions, short passthroughb, marpaESLIF_rule_t **out_rulepp);
/* http://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c */
static inline int   _marpaESLIF_count_revifi(int n);
static inline short _marpaESLIFValueRuleCallbackGrammar_lexemeDefaultb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, genericStack_t *adverbListStackp);
static inline short _marpaESLIFValueRuleCallbackGrammar_discardDefaultb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, marpaESLIF_grammarContext_op_declare_t op_declare, genericStack_t *adverbListStackp);
  
/* For every rule we write a specific action. Every action is isually very short, and just pop/push the stack */
/* For convenience action names mimic the rule names */
static inline short _G1_RULE_STATEMENTS                 (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_01               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_02               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_03               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_04               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_05               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_06               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_07               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_08               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_09               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_10               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_11               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_12               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_13               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_14               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_15               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_16               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_17               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_START_RULE                 (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DESC_RULE                  (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_EMPTY_RULE                 (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_NULL_STATEMENT             (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_STATEMENT_GROUP            (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_PRIORITY_RULE              (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_QUANTIFIED_RULE            (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DISCARD_RULE               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DEFAULT_RULE               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_LEXEME_DEFAULT_STATEMENT_1 (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_LEXEME_DEFAULT_STATEMENT_2 (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DISCARD_DEFAULT_STATEMENT_1(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_DISCARD_DEFAULT_STATEMENT_2(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);

static inline short _G1_RULE_LEXEME_RULE               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);

static inline short _G1_RULE_OP_DECLARE_3              (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_OP_DECLARE_1              (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ACTION_NAME               (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ACTION                    (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_ITEM_01            (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_ITEM_12            (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);
static inline short _G1_RULE_ADVERB_LIST_ITEMS         (marpaESLIFValue_t *marpaESLIFValuep, marpaESLIF_grammarContext_t *marpaESLIF_grammarContextp, int rulei, int arg0i, int argni, int resulti);

#endif /* MARPAESLIF_INTERNAL_GRAMMARCONTEXT_H */
