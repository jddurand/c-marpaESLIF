#ifndef GENERICSTACK_H
#define GENERICSTACK_H

#include <stdlib.h>       /* For malloc, free */
#include <string.h>       /* For memcpy */
#include <stddef.h>       /* For size_t */

/* =============== */
/* C generic stack */
/* =============== */

/* ------------------------------------------------------------------- */
/* Most of the generic stack implementations either assume that every  */
/* element is of the same size, or bypass type checking going through  */
/* a C layer. This version does not have these two constraints.        */
/* By default it is restricted to ANSI-C data type, nevertheless       */
/* adding others is as trivial as looking into e.g. long long below.   */
/* Please note that, IF the data typecast to the stack, no warning.    */
/*                                                                     */
/* Purists will notice this is an array-based implementation. This     */
/* choice was made because it is fits all my applications.             */
/*                                                                     */
/* Define GENERICSTACK_C99 to have C99 data type                       */
/* ------------------------------------------------------------------- */

#ifndef GENERICSTACK_INCREASE_BUCKET
#define GENERICSTACK_INCREASE_BUCKET 1000 /* Subjective number */
#endif

/* ------------------------------------------------------------------- */
/* Memory increases on demand using the following algorithm:           */
/* - current size is multiplied by 2                                   */
/* - the increase cannot be greater than GENERICSTACK_INCREASE_BUCKET  */
/*   unless: + the stack is originally empty                           */
/*           + user ask for a greater indice                           */
/* Memory always decreases by a factor 2 if the usage is <= size/2     */
/* ------------------------------------------------------------------- */

#ifdef GENERICSTACK_C99
#  undef GENERICSTACK_HAVE_LONG_LONG
#  define GENERICSTACK_HAVE_LONG_LONG 1
#  undef GENERICSTACK_HAVE__BOOL
#  define GENERICSTACK_HAVE__BOOL     1
#  undef GENERICSTACK_HAVE__COMPLEX
#  define GENERICSTACK_HAVE__COMPLEX  1
#else
#  ifndef GENERICSTACK_HAVE_LONG_LONG
#    define GENERICSTACK_HAVE_LONG_LONG 0
#  endif
#  ifndef GENERICSTACK_HAVE__BOOL
#    define GENERICSTACK_HAVE__BOOL     0
#  endif
#  ifndef GENERICSTACK_HAVE__COMPLEX
#    define GENERICSTACK_HAVE__COMPLEX  0
#  endif
#endif

typedef void *(*genericStackClone_t)(void *p);
typedef void  (*genericStackFree_t)(void *p);
typedef struct genericStackItemTypeArray {
  void *p;
  size_t lengthl;
} genericStackItemTypeArray_t;

typedef enum genericStackItemType {
  _GENERICSTACKITEMTYPE_NA = 0,    /* Not a hasard it is explicitely 0 */
  GENERICSTACKITEMTYPE_CHAR,
  GENERICSTACKITEMTYPE_SHORT,
  GENERICSTACKITEMTYPE_INT,
  GENERICSTACKITEMTYPE_LONG,
  GENERICSTACKITEMTYPE_FLOAT,
  GENERICSTACKITEMTYPE_DOUBLE,
  GENERICSTACKITEMTYPE_PTR,
  GENERICSTACKITEMTYPE_ARRAY,
#if GENERICSTACK_HAVE_LONG_LONG
  GENERICSTACKITEMTYPE_LONG_LONG,
#endif
#if GENERICSTACK_HAVE__BOOL
  GENERICSTACKITEMTYPE__BOOL,
#endif
#if GENERICSTACK_HAVE__COMPLEX
  GENERICSTACKITEMTYPE_FLOAT__COMPLEX,
  GENERICSTACKITEMTYPE_DOUBLE__COMPLEX,
  GENERICSTACKITEMTYPE_LONG_DOUBLE__COMPLEX,
#endif
  _GENERICSTACKITEMTYPE_MAX
} genericStackItemType_t;

static genericStackItemType_t genericStackItemTypeNA = _GENERICSTACKITEMTYPE_NA;

typedef struct genericStackItem {
  genericStackItemType_t type;
  union {
    char c;
    short s;
    int i;
    long l;
    float f;
    double d;
    void *p;
    genericStackItemTypeArray_t a;
#if GENERICSTACK_HAVE_LONG_LONG > 0
    long long ll;
#endif
#if GENERICSTACK_HAVE__BOOL > 0
    _Bool b;
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
    float _Complex fc;
    double _Complex dc;
    long double _Complex ldc;
#endif
  } u;
} genericStackItem_t;

typedef struct genericStack {
  size_t size;
  size_t used;
  genericStackItem_t *items;
  short  error;
  size_t tmpsize;
  genericStackItem_t *tmpitems;
  short NA_is_0_bytes;
} genericStack_t;

/* ====================================================================== */
/* Error detection and reset                                              */
/* ====================================================================== */
#define GENERICSTACK_ERROR(stackName) (((stackName) == NULL) || ((stackName)->error != 0))
#define GENERICSTACK_ERROR_RESET(stackName) do {			\
    if ((stackName) != NULL) {						\
      (stackName)->error = 0;						\
    }									\
  } while (0)

/* ====================================================================== */
/* Size management, internal macro                                        */
/* We check for size_t turnaround heuristically - should work IMHO        */
/* ====================================================================== */
#define _GENERICSTACK_EXTEND(stackName, wantedSize) do {                \
    size_t _genericStackExtend_wantedSize = (size_t) (wantedSize);      \
    size_t _genericStackExtend_currentSize = (size_t) (stackName)->size; \
    if (_genericStackExtend_wantedSize > _genericStackExtend_currentSize) { \
      size_t _genericStackExtend_newSize;                               \
      genericStackItem_t *_genericStackExtend_items = (stackName)->items; \
      short _genericStackExtend_memsetb = 0;                            \
                                                                        \
      if (_genericStackExtend_currentSize <= 0) {                       \
        _genericStackExtend_newSize = _genericStackExtend_wantedSize;   \
      } else {                                                          \
        _genericStackExtend_newSize = _genericStackExtend_currentSize * 2; \
        if ((_genericStackExtend_newSize < _genericStackExtend_currentSize) || (_genericStackExtend_newSize < _genericStackExtend_wantedSize)) { \
          _genericStackExtend_newSize = _genericStackExtend_wantedSize; \
        }                                                               \
      }                                                                 \
      if (_genericStackExtend_items == NULL) {                          \
	_genericStackExtend_items = calloc(_genericStackExtend_newSize, sizeof(genericStackItem_t)); \
      } else {								\
	_genericStackExtend_memsetb = 1;							\
	_genericStackExtend_items = realloc(_genericStackExtend_items, sizeof(genericStackItem_t) * _genericStackExtend_newSize); \
      }									\
      if (_genericStackExtend_items == NULL) {                          \
	(stackName)->error = 1;						\
      } else {								\
	if (_genericStackExtend_memsetb != 0) {                         \
	  if ((stackName)->NA_is_0_bytes != 0) {				\
	    memset(&(_genericStackExtend_items[(stackName)->size]), 0, _genericStackExtend_newSize - (stackName)->size); \
	  } else {							\
	    size_t _i_for_extend;					\
	    for (_i_for_extend = (stackName)->size;			\
		 _i_for_extend < _genericStackExtend_newSize;           \
		 _i_for_extend++) {					\
	      _genericStackExtend_items[_i_for_extend].type = _GENERICSTACKITEMTYPE_NA;	\
	    }								\
	  }								\
	}								\
	(stackName)->items = _genericStackExtend_items;			\
	(stackName)->size = _genericStackExtend_newSize;		\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Initialization                                                         */
/* ====================================================================== */
#define GENERICSTACK_INIT(stackName) do {                               \
    if ((stackName) != NULL) {						\
      (stackName)->size = 0;						\
      (stackName)->used = 0;						\
      (stackName)->items = NULL;					\
      (stackName)->error = 0;						\
      {									\
	genericStackItemType_t *_genericStackInit_t = (genericStackItemType_t *) calloc(1, sizeof(genericStackItemType_t)); \
	if (_genericStackInit_t == NULL) {                              \
	  (stackName)->error = 1;					\
	} else {							\
	  (stackName)->NA_is_0_bytes = ((*_genericStackInit_t) == _GENERICSTACKITEMTYPE_NA) ? 1 : 0; \
	  free(_genericStackInit_t);                                    \
	}								\
      }									\
    }									\
  } while (0)

#define GENERICSTACK_NEW(stackName) do {				\
    (stackName) = malloc(sizeof(genericStack_t));			\
    GENERICSTACK_INIT((stackName));					\
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, wantedSize) do {              \
    GENERICSTACK_NEW((stackName));					\
    if (! GENERICSTACK_ERROR((stackName))) {				\
      _GENERICSTACK_EXTEND((stackName), (wantedSize));			\
    }									\
  } while (0)

/* ====================================================================== */
/* Allocated size                                                         */
/* ====================================================================== */
#define GENERICSTACK_SIZE(stackName) (stackName)->size

/* ====================================================================== */
/* Used size                                                              */
/* ====================================================================== */
#define GENERICSTACK_USED(stackName) (stackName)->used

/* ====================================================================== */
/* SET interface: Stack is extended on demand                             */
/* ====================================================================== */
/* stackName is expected to an identifier                                 */
/* index is used more than once, so it has to be cached                   */
#define _GENERICSTACK_SET_BY_TYPE(stackName, varType, var, itemType, dst, index) do { \
    size_t _genericStackSetByType_indexForSet = (index);                  \
    if (_genericStackSetByType_indexForSet >= (stackName)->used) {        \
      size_t _genericStackSetByType_wantedSize = _genericStackSetByType_indexForSet + 1; \
      size_t _genericStackSetByType_isForGap;                           \
                                                                        \
      _GENERICSTACK_EXTEND((stackName), _genericStackSetByType_wantedSize); \
      for (_genericStackSetByType_isForGap = (stackName)->used; _genericStackSetByType_isForGap < _genericStackSetByType_indexForSet; _genericStackSetByType_isForGap++) { \
        (stackName)->items[_genericStackSetByType_isForGap].type = _GENERICSTACKITEMTYPE_NA;   \
      }                                                                 \
      (stackName)->used = _genericStackSetByType_wantedSize;		\
    }									\
    if (! GENERICSTACK_ERROR((stackName))) {				\
      genericStackItem_t *_item = (stackName)->items + _genericStackSetByType_indexForSet; \
      _item->type = (itemType);						\
      _item->u.dst = (varType) (var);					\
    }									\
  } while (0)

/* It appears that come compilers (like cl) does not like some casts */
#define _GENERICSTACK_SET_BY_TYPE_NOCAST(stackName, var, itemType, dst, index) do { \
    size_t _genericStackSetByType_indexForSet = (index);                  \
    if (_genericStackSetByType_indexForSet >= (stackName)->used) {        \
      size_t _genericStackSetByType_wantedSize = _genericStackSetByType_indexForSet + 1; \
      size_t _genericStackSetByType_isForGap;                           \
                                                                        \
      _GENERICSTACK_EXTEND((stackName), _genericStackSetByType_wantedSize); \
      for (_genericStackSetByType_isForGap = (stackName)->used; _genericStackSetByType_isForGap < _genericStackSetByType_indexForSet; _genericStackSetByType_isForGap++) { \
        (stackName)->items[_genericStackSetByType_isForGap].type = _GENERICSTACKITEMTYPE_NA;   \
      }                                                                 \
      (stackName)->used = _genericStackSetByType_wantedSize;		\
    }									\
    if (! GENERICSTACK_ERROR((stackName))) {				\
      genericStackItem_t *_item = (stackName)->items + _genericStackSetByType_indexForSet; \
      _item->type = (itemType);						\
      _item->u.dst = (var);						\
    }									\
  } while (0)

#define GENERICSTACK_SET_CHAR(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), char,   var, GENERICSTACKITEMTYPE_CHAR, c, index)
#define GENERICSTACK_SET_SHORT(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE((stackName), short,  var, GENERICSTACKITEMTYPE_SHORT, s, index)
#define GENERICSTACK_SET_INT(stackName, var, index)    _GENERICSTACK_SET_BY_TYPE((stackName), int,    var, GENERICSTACKITEMTYPE_INT, i, index)
#define GENERICSTACK_SET_LONG(stackName, var, index)   _GENERICSTACK_SET_BY_TYPE((stackName), long,   var, GENERICSTACKITEMTYPE_LONG, l, index)
#define GENERICSTACK_SET_FLOAT(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE((stackName), float,  var, GENERICSTACKITEMTYPE_FLOAT, f, index)
#define GENERICSTACK_SET_DOUBLE(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), double, var, GENERICSTACKITEMTYPE_DOUBLE, d, index)
#define GENERICSTACK_SET_PTR(stackName, var, index)    _GENERICSTACK_SET_BY_TYPE((stackName), void *, var, GENERICSTACKITEMTYPE_PTR, p, index)
#define GENERICSTACK_SET_ARRAY(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE_NOCAST((stackName), var, GENERICSTACKITEMTYPE_ARRAY, a, index)
#define GENERICSTACK_SET_ARRAYP(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE_NOCAST((stackName), *(var), GENERICSTACKITEMTYPE_ARRAY, a, index)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_SET_LONG_LONG(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), long long, var, GENERICSTACKITEMTYPE_LONG_LONG, ll, index)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_SET__BOOL(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), _Bool, var, GENERICSTACKITEMTYPE_LONG_LONG, b, index)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_SET_FLOAT__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), float _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, fc, index)
#define GENERICSTACK_SET_DOUBLE__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, dc, index)
#define GENERICSTACK_SET_LONG_DOUBLE__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE((stackName), long double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, ldc, index)
#endif

/* Special case for NA: there is not associated data */
#define GENERICSTACK_SET_NA(stackName, index) do {			\
    size_t _genericStackSetNA_indexForSet = (index);			\
    if (_genericStackSetNA_indexForSet >= (stackName)->used) {		\
      size_t _genericStackSetNA_wantedSize = _genericStackSetNA_indexForSet + 1; \
      size_t _genericStackSetNA_isForGap;                               \
                                                                        \
      _GENERICSTACK_EXTEND(stackName, _genericStackSetNA_wantedSize);   \
      for (_genericStackSetNA_isForGap = (stackName)->used; _genericStackSetNA_isForGap < _genericStackSetNA_indexForSet; _genericStackSetNA_isForGap++) { \
        (stackName)->items[_genericStackSetNA_isForGap].type = _GENERICSTACKITEMTYPE_NA; \
      }                                                                 \
      (stackName)->used = _genericStackSetNA_wantedSize;		\
    }									\
    if (! GENERICSTACK_ERROR(stackName)) {				\
      (stackName)->items[_genericStackSetNA_indexForSet].type = _GENERICSTACKITEMTYPE_NA; \
    }									\
  } while (0)

/* ====================================================================== */
/* Internal reduce of size before a GET that decrements stackName->use.   */
/* It is used with the GET interface, so have to fit in a single line     */
/* ====================================================================== */
#define _GENERICSTACK_REDUCE_SIZE(stackName)				\
  ((stackName)->used > 0) ?						\
  (									\
   ((stackName)->used <= ((stackName)->tmpsize = (size_t) ((stackName)->size / 2))) ? \
   (									\
    (((stackName)->tmpitems = (genericStackItem_t *) realloc((stackName)->items, (stackName)->tmpsize * sizeof(genericStackItem_t))) != NULL) ? \
    (									\
     (void)((stackName)->items = (stackName)->tmpitems, (stackName)->size = (stackName)->tmpsize) \
									) \
    :									\
    (void)0								\
									) \
   :									\
   (void)0								\
									) \
  :									\
  (void)(								\
   ((stackName)->items != NULL) ? free((stackName)->items) : (void)0, (stackName)->items = NULL, (stackName)->size = (stackName)->used = 0 \
   )

/* ====================================================================== */
/* GET interface                                                          */
/* Last executed statement in the () is its return value                  */
/* ====================================================================== */
#define GENERICSTACK_GET_CHAR(stackName, index)   (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.c)
#define GENERICSTACK_GET_SHORT(stackName, index)  (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.s)
#define GENERICSTACK_GET_INT(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.i)
#define GENERICSTACK_GET_LONG(stackName, index)   (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.l)
#define GENERICSTACK_GET_FLOAT(stackName, index)  (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.f)
#define GENERICSTACK_GET_DOUBLE(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.d)
#define GENERICSTACK_GET_PTR(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.p)
#define GENERICSTACK_GET_ARRAY(stackName, index)  (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.a)
#define GENERICSTACK_GET_ARRAYP(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), &((stackName)->items[index].u.a))
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_GET_LONG_LONG(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_GET__BOOL(stackName, index)  (stackName)->items[index].u.b
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_GET_FLOAT__COMPLEX(stackName, index)       (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.fc)
#define GENERICSTACK_GET_DOUBLE__COMPLEX(stackName, index)      (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.dc)
#define GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), (stackName)->items[index].u.ldc)
#endif
#define GENERICSTACK_GET_NA(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), index)

/* ====================================================================== */
/* PUSH interface: built on top of SET                                    */
/* ====================================================================== */
#define GENERICSTACK_PUSH_CHAR(stackName, var)   GENERICSTACK_SET_CHAR(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_SHORT(stackName, var)  GENERICSTACK_SET_SHORT(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_INT(stackName, var)    GENERICSTACK_SET_INT(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_LONG(stackName, var)   GENERICSTACK_SET_LONG(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_FLOAT(stackName, var)  GENERICSTACK_SET_FLOAT(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_DOUBLE(stackName, var) GENERICSTACK_SET_DOUBLE(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_PTR(stackName, var)    GENERICSTACK_SET_PTR(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_ARRAY(stackName, var)  GENERICSTACK_SET_ARRAY(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_ARRAYP(stackName, var) GENERICSTACK_SET_ARRAYP(stackName, var, (stackName)->used)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_PUSH_LONG_LONG(stackName, var) GENERICSTACK_SET_LONG_LONG(stackName, var, (stackName)->used)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_PUSH__BOOL(stackName, var) GENERICSTACK_SET__BOOL(stackName, var, (stackName)->used)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_PUSH_FLOAT__COMPLEX(stackName, var) GENERICSTACK_SET_FLOAT__COMPLEX(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_SET_DOUBLE__COMPLEX(stackName, var, (stackName)->used)
#define GENERICSTACK_PUSH_LONG_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_SET_LONG_DOUBLE__COMPLEX(stackName, var, (stackName)->used)
#endif
#define GENERICSTACK_PUSH_NA(stackName) GENERICSTACK_SET_NA(stackName, (stackName)->used)

/* ====================================================================== */
/* POP interface: built on top GET                                        */
/* ====================================================================== */
#define GENERICSTACK_POP_CHAR(stackName)   GENERICSTACK_GET_CHAR(stackName,   --(stackName)->used)
#define GENERICSTACK_POP_SHORT(stackName)  GENERICSTACK_GET_SHORT(stackName,  --(stackName)->used)
#define GENERICSTACK_POP_INT(stackName)    GENERICSTACK_GET_INT(stackName,    --(stackName)->used)
#define GENERICSTACK_POP_LONG(stackName)   GENERICSTACK_GET_LONG(stackName,   --(stackName)->used)
#define GENERICSTACK_POP_FLOAT(stackName)  GENERICSTACK_GET_FLOAT(stackName,  --(stackName)->used)
#define GENERICSTACK_POP_DOUBLE(stackName) GENERICSTACK_GET_DOUBLE(stackName, --(stackName)->used)
#define GENERICSTACK_POP_PTR(stackName)    GENERICSTACK_GET_PTR(stackName,    --(stackName)->used)
#define GENERICSTACK_POP_ARRAY(stackName)  GENERICSTACK_GET_ARRAY(stackName,  --(stackName)->used)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_POP_LONG_LONG(stackName)    GENERICSTACK_GET_LONG_LONG(stackName, --(stackName)->used)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_POP__BOOL(stackName)  GENERICSTACK_GET__BOOL(stackName, --(stackName)->used)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_POP_FLOAT__COMPLEX(stackName)       GENERICSTACK_GET_FLOAT__COMPLEX(stackName,       --(stackName)->used)
#define GENERICSTACK_POP_DOUBLE__COMPLEX(stackName)      GENERICSTACK_GET_DOUBLE__COMPLEX(stackName,      --(stackName)->used)
#define GENERICSTACK_POP_LONG_DOUBLE__COMPLEX(stackName) GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, --(stackName)->used)
#endif
#define GENERICSTACK_POP_NA(stackName) GENERICSTACK_GET_NA(stackName, --(stackName)->used)

/* ====================================================================== */
/* Memory release                                                         */
/* We intentionnaly loop on size and not used.                            */
/* ====================================================================== */
#define GENERICSTACK_FREE(stackName) do {				\
    if ((stackName) != NULL) {						\
      if ((stackName)->items != NULL) {					\
        free((stackName)->items);					\
      }									\
      free((stackName));						\
      (stackName) = NULL;						\
    }									\
  } while (0)

/* ====================================================================== */
/* In some rare occasions user might want to get the basic type           */
/* from an item type.                                                     */
/* ====================================================================== */
#define GENERICSTACKITEMTYPE(stackName, index) (stackName)->items[index].type

#define GENERICSTACKITEMTYPE2TYPE_CHAR   char
#define GENERICSTACKITEMTYPE2TYPE_SHORT  short
#define GENERICSTACKITEMTYPE2TYPE_INT    int
#define GENERICSTACKITEMTYPE2TYPE_LONG   long
#define GENERICSTACKITEMTYPE2TYPE_FLOAT  float
#define GENERICSTACKITEMTYPE2TYPE_DOUBLE double
#define GENERICSTACKITEMTYPE2TYPE_PTR    void *
#define GENERICSTACKITEMTYPE2TYPE_ARRAY  genericStackItemTypeArray_t
#define GENERICSTACKITEMTYPE2TYPE_ARRAYP  genericStackItemTypeArray_t *
#define GENERICSTACK_ARRAY_PTR(a) (a).p
#define GENERICSTACK_ARRAYP_PTR(a) (a)->p
#define GENERICSTACK_ARRAY_LENGTH(a) (a).lengthl
#define GENERICSTACK_ARRAYP_LENGTH(a) (a)->lengthl
#if GENERICSTACK_HAVE_LONG_LONG
  #define GENERICSTACKITEMTYPE2TYPE_LONG_LONG long long
#endif
#if GENERICSTACK_HAVE__BOOL
  #define GENERICSTACKITEMTYPE2TYPE__BOOL _Bool
#endif
#if GENERICSTACK_HAVE__COMPLEX
  #define GENERICSTACKITEMTYPE2TYPE_FLOAT__COMPLEX       float _Complex
  #define GENERICSTACKITEMTYPE2TYPE_DOUBLE__COMPLEX      double _Complex
  #define GENERICSTACKITEMTYPE2TYPE_LONG_DOUBLE__COMPLEX long double _Complex
#endif

/* ====================================================================== */
/* Switches two entries                                                   */
/* We support a "negative index", which mean start by far from the end.   */
/* ====================================================================== */
#define GENERICSTACK_SWITCH(stackName, i1, i2) do {                     \
    int _genericStackSwitch_index1 = (int) (i1);                        \
    int _genericStackSwitch_index2 = (int) (i2);                        \
                                                                        \
    if (_genericStackSwitch_index1 < 0) {                               \
      _genericStackSwitch_index1 = (stackName)->used + _genericStackSwitch_index1; \
    }                                                                   \
    if (_genericStackSwitch_index2 < 0) {                               \
      _genericStackSwitch_index2 = (stackName)->used + _genericStackSwitch_index2; \
    }                                                                   \
                                                                        \
    if ((_genericStackSwitch_index1 < 0) || (((size_t) _genericStackSwitch_index1) >= (stackName)->used) || \
        (_genericStackSwitch_index2 < 0) || (((size_t) _genericStackSwitch_index2) >= (stackName)->used)) { \
      (stackName)->error = 1;                                             \
    } else if (_genericStackSwitch_index1 != _genericStackSwitch_index2) { \
      genericStackItem_t _item = (stackName)->items[_genericStackSwitch_index1]; \
      (stackName)->items[_genericStackSwitch_index1] = (stackName)->items[_genericStackSwitch_index2]; \
      (stackName)->items[_genericStackSwitch_index2] = _item;		\
    }                                                                   \
  } while (0)

/* ====================================================================== */
/* More easy macros                                                       */
/* ====================================================================== */
#define GENERICSTACK_EXISTS(stackName, i) (((stackName) != NULL) && ((stackName)->used > (i)))
#define GENERICSTACK_IS_NA(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == _GENERICSTACKITEMTYPE_NA))
#define GENERICSTACK_IS_CHAR(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_CHAR))
#define GENERICSTACK_IS_SHORT(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_SHORT))
#define GENERICSTACK_IS_INT(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_INT))
#define GENERICSTACK_IS_LONG(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_LONG))
#define GENERICSTACK_IS_FLOAT(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_FLOAT))
#define GENERICSTACK_IS_DOUBLE(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_DOUBLE))
#define GENERICSTACK_IS_PTR(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_PTR))
#define GENERICSTACK_IS_ARRAY(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_ARRAY))
#if GENERICSTACK_HAVE_LONG_LONG
#define GENERICSTACK_IS_LONG_LONG(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_LONG_LONG))
#endif
#if GENERICSTACK_HAVE__BOOL
#define GENERICSTACK_IS__BOOL(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE__BOOL))
#endif
#if GENERICSTACK_HAVE__COMPLEX
#define GENERICSTACK_IS_FLOAT__COMPLEX(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_FLOAT__COMPLEX))
#define GENERICSTACK_IS_DOUBLE__COMPLEX(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_DOUBLE__COMPLEX))
#define GENERICSTACK_IS_LONG_DOUBLE__COMPLEX(stackName, i) (GENERICSTACK_EXISTS(stackName, i) && ((stackName)->items[i].type == GENERICSTACKITEMTYPE_LONG_DOUBLE__COMPLEX))
#endif

#endif /* GENERICSTACK_H */
