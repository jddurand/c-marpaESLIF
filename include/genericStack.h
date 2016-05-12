#ifndef GENERICSTACK_H
#define GENERICSTACK_H

#include <stdlib.h>       /* For malloc, free */
#include <string.h>       /* For memcpy */
#include <stddef.h>       /* For size_t */

/* =============== */
/* C generic stack */
/* =============== */

/* Most of the generic stack implementations either assume that every  */
/* element is of the same size, or bypass type checking going through  */
/* a C layer. This version does not have these two constraints.        */
/* By default it is restricted to ANSI-C data type, nevertheless       */
/* adding others is as trivial as looking into e.g. long long below.   */
/* Please note that, IF the data typecast to the stack, no warning.    */

/* Define GENERICSTACK_C99 to have C99 data type */

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

typedef void *(*genericStackMalloc_t)(size_t size);
typedef void *(*genericStackMemcpy_t)(void *dst, void *src, size_t size);
typedef void *(*genericStackFree_t)(void *p);

typedef struct genericStackItemAny {
  void *p;
  size_t size;
  genericStackMalloc_t malloc;
  genericStackMemcpy_t memcpy;
  genericStackFree_t free;
} genericStackItemAny_t;

typedef enum genericStackItemType {
  GENERICSTACKITEMTYPE_CHAR,
  GENERICSTACKITEMTYPE_SHORT,
  GENERICSTACKITEMTYPE_INT,
  GENERICSTACKITEMTYPE_LONG,
  GENERICSTACKITEMTYPE_FLOAT,
  GENERICSTACKITEMTYPE_DOUBLE,
  GENERICSTACKITEMTYPE_PTR,
#if GENERICSTACK_HAVE_LONG_LONG > 0
  GENERICSTACKITEMTYPE_LONG_LONG,
#endif
#if GENERICSTACK_HAVE__BOOL > 0
  GENERICSTACKITEMTYPE__BOOL,
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
  GENERICSTACKITEMTYPE_FLOAT__COMPLEX,
  GENERICSTACKITEMTYPE_DOUBLE__COMPLEX,
  GENERICSTACKITEMTYPE_LONG_DOUBLE__COMPLEX,
#endif
  GENERICSTACKITEMTYPE_ANY,
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
    genericStackItemAny_t any;
  } u;
} genericStackItem_t;

typedef struct genericStack {
  size_t size;
  size_t used;
  genericStackItem_t *items;
} genericStack_t;

/* ====================================================================== */
/* Declaration                                                            */
/* ====================================================================== */
#define GENERICSTACK_DECL(stackName) genericStack_t * stackName

/* ====================================================================== */
/* Initialization                                                         */
/* ====================================================================== */
#define GENERICSTACK_NEW(stackName) do {				\
    stackName = malloc(sizeof(genericStack_t));                         \
    stackName->size = 0;                                                \
    stackName->used = 0;                                                \
    stackName->items = NULL;                                            \
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, size) do {			\
    size_t _size = (size);						\
    GENERICSTACK_NEW(stackName);					\
    stackName->items = malloc(sizeof(genericStackItem_t) * (stackName->size = _size)); \
  } while (0)

/* ====================================================================== */
/* Size management, internal macro                                        */
/* ====================================================================== */
#define _GENERICSTACK_EXTEND(stackName) do {                            \
    if (stackName->used > stackName->size) {                            \
      size_t _i_for_extend;						\
      stackName->items = (stackName->items != NULL) ? realloc(stackName->items, sizeof(genericStackItem_t) * stackName->used) : malloc(sizeof(genericStackItem_t) * stackName->used); \
      for (_i_for_extend = stackName->size; _i_for_extend < stackName->used; _i_for_extend++) {	\
	stackName->items[_i_for_extend].type = _GENERICSTACKITEMTYPE_NA; \
      }									\
      stackName->size = stackName->used;                                \
    }									\
  } while (0)

/* ====================================================================== */
/* SET interface                                                          */
/* ====================================================================== */
/* stackName is expected to an identifier                                 */
/* index is used more than once, so it has to be cached                   */
#define GENERICSTACK_SET_BASIC_TYPE(stackName, varType, var, itemType, dst, index) do { \
    size_t _index_for_set = index;                                      \
    if (_index_for_set >= stackName->used) {                            \
      stackName->used = _index_for_set + 1;                             \
      _GENERICSTACK_EXTEND(stackName);                                  \
    }                                                                   \
    stackName->items[_index_for_set].type = (itemType);                 \
    stackName->items[_index_for_set].u.dst = (varType) (var);           \
  } while (0)

#define GENERICSTACK_SET_CHAR(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, char,   var, GENERICSTACKITEMTYPE_CHAR, c, index)
#define GENERICSTACK_SET_SHORT(stackName, var, index)  GENERICSTACK_SET_BASIC_TYPE(stackName, short,  var, GENERICSTACKITEMTYPE_SHORT, s, index)
#define GENERICSTACK_SET_INT(stackName, var, index)    GENERICSTACK_SET_BASIC_TYPE(stackName, int,    var, GENERICSTACKITEMTYPE_INT, i, index)
#define GENERICSTACK_SET_LONG(stackName, var, index)   GENERICSTACK_SET_BASIC_TYPE(stackName, long,   var, GENERICSTACKITEMTYPE_LONG, l, index)
#define GENERICSTACK_SET_FLOAT(stackName, var, index)  GENERICSTACK_SET_BASIC_TYPE(stackName, float,  var, GENERICSTACKITEMTYPE_FLOAT, f, index)
#define GENERICSTACK_SET_DOUBLE(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, double, var, GENERICSTACKITEMTYPE_DOUBLE, d, index)
#define GENERICSTACK_SET_PTR(stackName, var, index)    GENERICSTACK_SET_BASIC_TYPE(stackName, void *, var, GENERICSTACKITEMTYPE_PTR, p, index)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_SET_LONG_LONG(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, long long, var, GENERICSTACKITEMTYPE_LONG_LONG, ll, index)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_SET__BOOL(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, _Bool, var, GENERICSTACKITEMTYPE_LONG_LONG, b, index)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_SET_FLOAT__COMPLEX(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, float _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, fc, index)
#define GENERICSTACK_SET_DOUBLE__COMPLEX(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, dc, index)
#define GENERICSTACK_SET_LONG_DOUBLE__COMPLEX(stackName, var, index) GENERICSTACK_SET_BASIC_TYPE(stackName, long double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, ldc, index)
#endif

/* ====================================================================== */
/* PUSH interface                                                         */
/* ====================================================================== */
#define GENERICSTACK_PUSH_BASIC_TYPE(stackName, varType, var, itemType, dst) do { \
    size_t _index_for_push = stackName->used;                           \
    GENERICSTACK_SET_BASIC_TYPE(stackName, varType, var, itemType, dst, _index_for_push); \
  } while (0)

#define GENERICSTACK_PUSH_CHAR(stackName, var)   GENERICSTACK_PUSH_BASIC_TYPE(stackName, char,   var, GENERICSTACKITEMTYPE_CHAR, c)
#define GENERICSTACK_PUSH_SHORT(stackName, var)  GENERICSTACK_PUSH_BASIC_TYPE(stackName, short,  var, GENERICSTACKITEMTYPE_SHORT, s)
#define GENERICSTACK_PUSH_INT(stackName, var)    GENERICSTACK_PUSH_BASIC_TYPE(stackName, int,    var, GENERICSTACKITEMTYPE_INT, i)
#define GENERICSTACK_PUSH_LONG(stackName, var)   GENERICSTACK_PUSH_BASIC_TYPE(stackName, long,   var, GENERICSTACKITEMTYPE_LONG, l)
#define GENERICSTACK_PUSH_FLOAT(stackName, var)  GENERICSTACK_PUSH_BASIC_TYPE(stackName, float,  var, GENERICSTACKITEMTYPE_FLOAT, f)
#define GENERICSTACK_PUSH_DOUBLE(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, double, var, GENERICSTACKITEMTYPE_DOUBLE, d)
#define GENERICSTACK_PUSH_PTR(stackName, var)    GENERICSTACK_PUSH_BASIC_TYPE(stackName, void *, var, GENERICSTACKITEMTYPE_PTR, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_PUSH_LONG_LONG(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, long long, var, GENERICSTACKITEMTYPE_LONG_LONG, ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_PUSH__BOOL(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, _Bool, var, GENERICSTACKITEMTYPE_LONG_LONG, b)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_PUSH_FLOAT__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, float _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, fc)
#define GENERICSTACK_PUSH_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, dc)
#define GENERICSTACK_PUSH_LONG_DOUBLE__COMPLEX(stackName, var) GENERICSTACK_PUSH_BASIC_TYPE(stackName, long double _Complex, var, GENERICSTACKITEMTYPE_LONG_LONG, ldc)
#endif

#define GENERICSTACK_PUSH_ANY(stackName, varType, var, mallocp, memcpyp, freep) do { \
    size_t _i = stackName->used++;                                      \
    size_t _size = sizeof(varType);					\
    void *_p;								\
    genericStackMalloc_t _malloc = (genericStackMalloc_t) (mallocp);	\
    genericStackMemcpy_t _memcpy = (genericStackMemcpy_t) (memcpyp);	\
    genericStackFree_t _free = (genericStackFree_t) (freep);		\
    									\
    _GENERICSTACK_EXTEND(stackName);                                    \
    _p = (_malloc == NULL) ? malloc(_size) : _malloc(_size);		\
    if (_memcpy == NULL) {                                              \
      memcpy(_p, var, _size);						\
    } else {                                                            \
      _memcpy(_p, var, _size);						\
    }                                                                   \
    stackName->items[_i].type = GENERICSTACKITEMTYPE_ANY;               \
    stackName->items[_i].u.any.p = _p;					\
    stackName->items[_i].u.any.size = _size;                            \
    stackName->items[_i].u.any.malloc = _malloc;                        \
    stackName->items[_i].u.any.memcpy = _memcpy;                        \
    stackName->items[_i].u.any.free = _free;                            \
									\
  } while (0)

/* ====================================================================== */
/* GET interface                                                          */
/* ====================================================================== */
#define GENERICSTACK_GET_BASIC_TYPE(stackName, index, src) stackName->items[index].u.src

#define GENERICSTACK_GET_CHAR(stackName, index)   GENERICSTACK_GET_BASIC_TYPE(stackName, index, c)
#define GENERICSTACK_GET_SHORT(stackName, index)  GENERICSTACK_GET_BASIC_TYPE(stackName, index, s)
#define GENERICSTACK_GET_INT(stackName, index)    GENERICSTACK_GET_BASIC_TYPE(stackName, index, i)
#define GENERICSTACK_GET_LONG(stackName, index)   GENERICSTACK_GET_BASIC_TYPE(stackName, index, l)
#define GENERICSTACK_GET_FLOAT(stackName, index)  GENERICSTACK_GET_BASIC_TYPE(stackName, index, f)
#define GENERICSTACK_GET_DOUBLE(stackName, index) GENERICSTACK_GET_BASIC_TYPE(stackName, index, d)
#define GENERICSTACK_GET_PTR(stackName, index)    GENERICSTACK_GET_BASIC_TYPE(stackName, index, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_GET_LONG_LONG(stackName, index)    GENERICSTACK_GET_BASIC_TYPE(stackName, index, ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_GET__BOOL(stackName, index)  GENERICSTACK_GET_BASIC_TYPE(stackName, index, b)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_GET_FLOAT__COMPLEX(stackName, index)       GENERICSTACK_GET_BASIC_TYPE(stackName, index, fc)
#define GENERICSTACK_GET_DOUBLE__COMPLEX(stackName, index)      GENERICSTACK_GET_BASIC_TYPE(stackName, index, dc)
#define GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, index) GENERICSTACK_GET_BASIC_TYPE(stackName, index, ldc)
#endif
#define GENERICSTACK_GET_ANY(stackName, index) GENERICSTACK_GET_BASIC_TYPE(stackName, index, any.p)

/* ====================================================================== */
/* POP interface                                                          */
/* ====================================================================== */
#define GENERICSTACK_POP_BASIC_TYPE(stackName, src) stackName->items[--stackName->used].u.src

#define GENERICSTACK_POP_CHAR(stackName)   GENERICSTACK_POP_BASIC_TYPE(stackName, c)
#define GENERICSTACK_POP_SHORT(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, s)
#define GENERICSTACK_POP_INT(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, i)
#define GENERICSTACK_POP_LONG(stackName)   GENERICSTACK_POP_BASIC_TYPE(stackName, l)
#define GENERICSTACK_POP_FLOAT(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, f)
#define GENERICSTACK_POP_DOUBLE(stackName) GENERICSTACK_POP_BASIC_TYPE(stackName, d)
#define GENERICSTACK_POP_PTR(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_POP_LONG_LONG(stackName)    GENERICSTACK_POP_BASIC_TYPE(stackName, ll)
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_POP__BOOL(stackName)  GENERICSTACK_POP_BASIC_TYPE(stackName, b)
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_POP_FLOAT__COMPLEX(stackName)       GENERICSTACK_POP_BASIC_TYPE(stackName, fc)
#define GENERICSTACK_POP_DOUBLE__COMPLEX(stackName)      GENERICSTACK_POP_BASIC_TYPE(stackName, dc)
#define GENERICSTACK_POP_LONG_DOUBLE__COMPLEX(stackName) GENERICSTACK_POP_BASIC_TYPE(stackName, ldc)
#endif
#define GENERICSTACK_POP_ANY(stackName) GENERICSTACK_POP_BASIC_TYPE(stackName, any.p)

/* ====================================================================== */
/* Memory release                                                         */
/* ====================================================================== */
#define GENERICSTACK_FREE(stackName) do {				\
    if (stackName->size > 0) {                                          \
      while (stackName->used > 0) {                                     \
	size_t _index = stackName->used-- - 1;				\
        if (stackName->items[_index].type == GENERICSTACKITEMTYPE_ANY) { \
	  genericStackItemAny_t _any = stackName->items[_index].u.any;	\
	  if (_any.free == NULL) {					\
	    free(_any.p);						\
	  } else {							\
	    _any.free(_any.p);						\
	  }								\
        }								\
      }									\
      free(stackName->items);                                           \
    }                                                                   \
    free(stackName);                                                    \
  } while (0);

#endif /* GENERICSTACK_H */
