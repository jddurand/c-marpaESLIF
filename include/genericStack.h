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

typedef enum genericStackItemType {
  GENERICSTACKITEMTYPE_CHAR,
  GENERICSTACKITEMTYPE_SHORT,
  GENERICSTACKITEMTYPE_INT,
  GENERICSTACKITEMTYPE_LONG,
  GENERICSTACKITEMTYPE_FLOAT,
  GENERICSTACKITEMTYPE_DOUBLE,
  GENERICSTACKITEMTYPE_PTR,
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
  _GENERICSTACKITEMTYPE_NA
} genericStackItemType_t;

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
} genericStack_t;

/* ====================================================================== */
/* Error detection                                                        */
/* ====================================================================== */
#define GENERICSTACK_ERROR(stackName) ((stackName == NULL) || (stackName->error != 0))

/* ====================================================================== */
/* Size management, internal macro                                        */
/* We check for size_t turnaround heuristically - should work IMHO        */
/* ====================================================================== */
#define _GENERICSTACK_EXTEND(stackName, wantedSize) do {                \
    size_t _wantedSize = (size_t) (wantedSize);                         \
    size_t _currentSize = (size_t) stackName->size;                     \
    if (_wantedSize > _currentSize) {                                   \
      size_t _newSize;                                                  \
      size_t _i_for_extend;						\
      genericStackItem_t *_items = stackName->items;			\
                                                                        \
      if (_currentSize <= 0) {                                          \
        _newSize = _wantedSize;                                         \
      } else {                                                          \
        _newSize = _currentSize * 2;                                    \
        if ((_newSize < _currentSize) || (_newSize < _wantedSize)) {    \
          _newSize = _wantedSize;                                       \
        }                                                               \
      }                                                                 \
      _items = (_items == NULL) ?					\
	malloc(sizeof(genericStackItem_t) * _newSize)                   \
	:								\
	realloc(_items, sizeof(genericStackItem_t) * _newSize);         \
      if (_items == NULL) {						\
	stackName->error = 1;						\
      } else {								\
	for (_i_for_extend = stackName->size;				\
	     _i_for_extend < _newSize;                                  \
	     _i_for_extend++) {						\
	  _items[_i_for_extend].type = _GENERICSTACKITEMTYPE_NA;	\
	}								\
	stackName->items = _items;					\
	stackName->size = _newSize;					\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Initialization                                                         */
/* ====================================================================== */
#define GENERICSTACK_INIT(stackName) do {                               \
  if (stackName != NULL) {						\
    stackName->size = 0;						\
    stackName->used = 0;						\
    stackName->items = NULL;						\
    stackName->error = 0;						\
  }                                                                     \
  } while (0)

#define GENERICSTACK_NEW(stackName) do {				\
    stackName = malloc(sizeof(genericStack_t));                         \
    GENERICSTACK_INIT(stackName);                                       \
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, wantedSize) do {              \
    GENERICSTACK_NEW(stackName);                                        \
    if (! GENERICSTACK_ERROR(stackName)) {				\
      _GENERICSTACK_EXTEND(stackName, wantedSize);			\
    }									\
  } while (0)

/* ====================================================================== */
/* Allocated size                                                         */
/* ====================================================================== */
#define GENERICSTACK_SIZE(stackName) stackName->size

/* ====================================================================== */
/* Used size                                                              */
/* ====================================================================== */
#define GENERICSTACK_USED(stackName) stackName->used

/* ====================================================================== */
/* SET interface: Stack is extended on demand                             */
/* ====================================================================== */
/* stackName is expected to an identifier                                 */
/* index is used more than once, so it has to be cached                   */
#define _GENERICSTACK_SET_BY_TYPE(stackName, varType, var, itemType, dst, index) do { \
    size_t _index_for_set = index;                                      \
    if (_index_for_set >= stackName->used) {                            \
      stackName->used = _index_for_set + 1;                             \
      _GENERICSTACK_EXTEND(stackName, stackName->used);                 \
    }									\
    if (! GENERICSTACK_ERROR(stackName)) {				\
      stackName->items[_index_for_set].type = (itemType);		\
      stackName->items[_index_for_set].u.dst = (varType) (var);		\
    }									\
  } while (0)

#define GENERICSTACK_SET_CHAR(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, char,   var, GENERICSTACKITEMTYPE_CHAR, c, index)
#define GENERICSTACK_SET_SHORT(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE(stackName, short,  var, GENERICSTACKITEMTYPE_SHORT, s, index)
#define GENERICSTACK_SET_INT(stackName, var, index)    _GENERICSTACK_SET_BY_TYPE(stackName, int,    var, GENERICSTACKITEMTYPE_INT, i, index)
#define GENERICSTACK_SET_LONG(stackName, var, index)   _GENERICSTACK_SET_BY_TYPE(stackName, long,   var, GENERICSTACKITEMTYPE_LONG, l, index)
#define GENERICSTACK_SET_FLOAT(stackName, var, index)  _GENERICSTACK_SET_BY_TYPE(stackName, float,  var, GENERICSTACKITEMTYPE_FLOAT, f, index)
#define GENERICSTACK_SET_DOUBLE(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, double, var, GENERICSTACKITEMTYPE_DOUBLE, d, index)
#define GENERICSTACK_SET_PTR(stackName, var, index)    _GENERICSTACK_SET_BY_TYPE(stackName, void *, var, GENERICSTACKITEMTYPE_PTR, p, index)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_SET_LONG_LONG(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, long long, var, GENERICSTACKITEMTYPE_LONG_LONG, ll, index)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_SET__BOOL(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, _Bool, var, GENERICSTACKITEMTYPE_LONG_LONG, b, index)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_SET_FLOAT__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, float _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, fc, index)
#define GENERICSTACK_SET_DOUBLE__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, dc, index)
#define GENERICSTACK_SET_LONG_DOUBLE__COMPLEX(stackName, var, index) _GENERICSTACK_SET_BY_TYPE(stackName, long double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, ldc, index)
#endif

/* Special case for NA: there is not associated data */
#define GENERICSTACK_SET_NA(stackName, index) do {                      \
    size_t _index_for_set = index;                                      \
    if (_index_for_set >= stackName->used) {                            \
      stackName->used = _index_for_set + 1;                             \
      _GENERICSTACK_EXTEND(stackName, stackName->used);                 \
    }									\
    if (! GENERICSTACK_ERROR(stackName)) {				\
      stackName->items[_index_for_set].type = _GENERICSTACKITEMTYPE_NA; \
    }									\
  } while (0)

/* ====================================================================== */
/* Internal reduce of size before a GET that decrements stackName->use.   */
/* It is used with the GET interface, so have to fit in a single line     */
/* ====================================================================== */
#define _GENERICSTACK_REDUCE_SIZE(stackName)				\
  (stackName->used > 0) ?						\
  (									\
   (stackName->used <= (stackName->tmpsize = (size_t) (stackName->size / 2))) ? \
   (									\
    ((stackName->tmpitems = (genericStackItem_t *) realloc(stackName->items, stackName->tmpsize * sizeof(genericStackItem_t))) != NULL) ? \
    (									\
     stackName->items = stackName->tmpitems, stackName->size = stackName->tmpsize \
									) \
    :									\
    0									\
									) \
   :									\
   0									\
									) \
  :									\
  (\
   (stackName->items != NULL) ? free(stackName->items) : 0, stackName->items = NULL, stackName->size = stackName->used = 0 \
   )

/* ====================================================================== */
/* GET interface                                                          */
/* Last executed statement in the {} is its return value                  */
/* ====================================================================== */
#define GENERICSTACK_GET_CHAR(stackName, index)   (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.c)
#define GENERICSTACK_GET_SHORT(stackName, index)  (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.s)
#define GENERICSTACK_GET_INT(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.i)
#define GENERICSTACK_GET_LONG(stackName, index)   (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.l)
#define GENERICSTACK_GET_FLOAT(stackName, index)  (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.f)
#define GENERICSTACK_GET_DOUBLE(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.d)
#define GENERICSTACK_GET_PTR(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_GET_LONG_LONG(stackName, index)    (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_GET__BOOL(stackName, index)  stackName->items[index].u.b
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_GET_FLOAT__COMPLEX(stackName, index)       (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.fc)
#define GENERICSTACK_GET_DOUBLE__COMPLEX(stackName, index)      (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.dc)
#define GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, index) (_GENERICSTACK_REDUCE_SIZE(stackName), stackName->items[index].u.ldc)
#endif

/* ====================================================================== */
/* PUSH interface: built on top of SET                                    */
/* ====================================================================== */
#define GENERICSTACK_PUSH_CHAR(stackName, var)   GENERICSTACK_SET_CHAR(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_SHORT(stackName, var)  GENERICSTACK_SET_SHORT(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_INT(stackName, var)    GENERICSTACK_SET_INT(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_LONG(stackName, var)   GENERICSTACK_SET_LONG(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_FLOAT(stackName, var)  GENERICSTACK_SET_FLOAT(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_DOUBLE(stackName, var) GENERICSTACK_SET_DOUBLE(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_PTR(stackName, var)    GENERICSTACK_SET_PTR(stackName, var, stackName->used)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_PUSH_LONG_LONG(stackName, var) GENERICSTACK_SET_LONG_LONG(stackName, var, stackName->used)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_PUSH__BOOL(stackName, var) GENERICSTACK_SET__BOOL(stackName, var, stackName->used)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_PUSH_FLOAT__COMPLEX(stackName, var) GENERICSTACK_SET_FLOAT__COMPLEX(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_SET_DOUBLE__COMPLEX(stackName, var, stackName->used)
#define GENERICSTACK_PUSH_LONG_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_SET_LONG_DOUBLE__COMPLEX(stackName, var, stackName->used)
#endif

/* ====================================================================== */
/* POP interface: built on top GET                                        */
/* ====================================================================== */
#define GENERICSTACK_POP_CHAR(stackName)   GENERICSTACK_GET_CHAR(stackName,   --stackName->used)
#define GENERICSTACK_POP_SHORT(stackName)  GENERICSTACK_GET_SHORT(stackName,  --stackName->used)
#define GENERICSTACK_POP_INT(stackName)    GENERICSTACK_GET_INT(stackName,    --stackName->used)
#define GENERICSTACK_POP_LONG(stackName)   GENERICSTACK_GET_LONG(stackName,   --stackName->used)
#define GENERICSTACK_POP_FLOAT(stackName)  GENERICSTACK_GET_FLOAT(stackName,  --stackName->used)
#define GENERICSTACK_POP_DOUBLE(stackName) GENERICSTACK_GET_DOUBLE(stackName, --stackName->used)
#define GENERICSTACK_POP_PTR(stackName)    GENERICSTACK_GET_PTR(stackName,    --stackName->used)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_POP_LONG_LONG(stackName)    GENERICSTACK_GET_LONG_LONG(stackName, --stackName->used)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_POP__BOOL(stackName)  GENERICSTACK_GET__BOOL(stackName, --stackName->used)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_POP_FLOAT__COMPLEX(stackName)       GENERICSTACK_GET_FLOAT__COMPLEX(stackName,       --stackName->used)
#define GENERICSTACK_POP_DOUBLE__COMPLEX(stackName)      GENERICSTACK_GET_DOUBLE__COMPLEX(stackName,      --stackName->used)
#define GENERICSTACK_POP_LONG_DOUBLE__COMPLEX(stackName) GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, --stackName->used)
#endif

/* ====================================================================== */
/* Memory release                                                         */
/* We intentionnaly loop on size and not used.                            */
/* ====================================================================== */
#define GENERICSTACK_FREE(stackName) do {				\
    if (stackName != NULL) {						\
      if (stackName->items != NULL) {                                   \
        free(stackName->items);						\
      }									\
      free(stackName);							\
      stackName = NULL;							\
    }									\
  } while (0)

/* ====================================================================== */
/* In some rare occasions user might want to get the basic type           */
/* from an item type.                                                     */
/* ====================================================================== */
#define GENERICSTACKITEMTYPE(stackName, index) stackName->items[index].type

#define GENERICSTACKITEMTYPE2TYPE_CHAR   char
#define GENERICSTACKITEMTYPE2TYPE_SHORT  short
#define GENERICSTACKITEMTYPE2TYPE_INT    int
#define GENERICSTACKITEMTYPE2TYPE_LONG   long
#define GENERICSTACKITEMTYPE2TYPE_FLOAT  float
#define GENERICSTACKITEMTYPE2TYPE_DOUBLE double
#define GENERICSTACKITEMTYPE2TYPE_PTR    void *
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

#endif /* GENERICSTACK_H */
