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
/*                                                                     */
/* Purists will notice this is an array-based implementation. This     */
/* choice was made because it is fits all my applications.             */
/*                                                                     */
/* Define GENERICSTACK_C99 to have C99 data type                       */

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

typedef struct genericStackItemAny {
  void *p;
  genericStackClone_t clone;
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
/* Size management, internal macro                                        */
/* ====================================================================== */
#define _GENERICSTACK_EXTEND(stackName, wantedSize) do {                \
    if (wantedSize > stackName->size) {                                 \
      size_t _i_for_extend;						\
      stackName->items = (stackName->items != NULL) ? realloc(stackName->items, sizeof(genericStackItem_t) * wantedSize) : malloc(sizeof(genericStackItem_t) * wantedSize); \
      for (_i_for_extend = stackName->size; _i_for_extend < wantedSize; _i_for_extend++) {	\
	stackName->items[_i_for_extend].type = _GENERICSTACKITEMTYPE_NA; \
      }									\
      stackName->size = wantedSize;                                     \
    }									\
  } while (0)

/* ====================================================================== */
/* Initialization                                                         */
/* ====================================================================== */
#define GENERICSTACK_NEW(stackName) do {				\
    stackName = malloc(sizeof(genericStack_t));                         \
    if (stackName != NULL) {						\
      stackName->size = 0;						\
      stackName->used = 0;						\
      stackName->items = NULL;						\
    }									\
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, wantedSize) do {              \
    GENERICSTACK_NEW(stackName);                                        \
    _GENERICSTACK_EXTEND(stackName, wantedSize);                        \
  } while (0)

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
    }                                                                   \
    stackName->items[_index_for_set].type = (itemType);                 \
    stackName->items[_index_for_set].u.dst = (varType) (var);           \
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
/* It is illegal to call this macro with a NULL clonep */
#define GENERICSTACK_SET_ANY(stackName, var, clonep, freep, index) do { \
    genericStackClone_t _clone = (genericStackClone_t) clonep;		\
    size_t _index_for_set = index;                                      \
    if (_index_for_set >= stackName->used) {                            \
      stackName->used = _index_for_set + 1;                             \
      _GENERICSTACK_EXTEND(stackName, stackName->used);                 \
    }                                                                   \
    stackName->items[_index_for_set].type = GENERICSTACKITEMTYPE_ANY;   \
    if (_clone != NULL) {						\
      stackName->items[_index_for_set].u.any.clone = _clone;		\
      stackName->items[_index_for_set].u.any.free = freep;		\
      stackName->items[_index_for_set].u.any.p = stackName->items[_index_for_set].u.any.clone(var); \
    } else {								\
      stackName->items[_index_for_set].u.any.clone = NULL;		\
      stackName->items[_index_for_set].u.any.free = NULL;		\
      stackName->items[_index_for_set].u.any.p = (void *) (var);	\
    }									\
                                                                        \
  } while (0)

/* ====================================================================== */
/* GET interface                                                          */
/* ====================================================================== */
#define GENERICSTACK_GET_CHAR(stackName, index)   stackName->items[index].u.c
#define GENERICSTACK_GET_SHORT(stackName, index)  stackName->items[index].u.s
#define GENERICSTACK_GET_INT(stackName, index)    stackName->items[index].u.i
#define GENERICSTACK_GET_LONG(stackName, index)   stackName->items[index].u.l
#define GENERICSTACK_GET_FLOAT(stackName, index)  stackName->items[index].u.f
#define GENERICSTACK_GET_DOUBLE(stackName, index) stackName->items[index].u.d
#define GENERICSTACK_GET_PTR(stackName, index)    stackName->items[index].u.p
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_GET_LONG_LONG(stackName, index)    stackName->items[index].u.ll
#endif
#if GENERICSTACK_HAVE__BOOL > 0
#define GENERICSTACK_GET__BOOL(stackName, index)  stackName->items[index].u.b
#endif
#if GENERICSTACK_HAVE__COMPLEX > 0
#define GENERICSTACK_GET_FLOAT__COMPLEX(stackName, index)       stackName->items[index].u.fc
#define GENERICSTACK_GET_DOUBLE__COMPLEX(stackName, index)      stackName->items[index].u.dc
#define GENERICSTACK_GET_LONG_DOUBLE__COMPLEX(stackName, index) stackName->items[index].u.ldc
#endif
#define GENERICSTACK_GET_ANY(stackName, index) stackName->items[index].u.any.p

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
#define GENERICSTACK_PUSH_ANY(stackName, var, clonep, freep) GENERICSTACK_SET_ANY(stackName, var, clonep, freep, stackName->used)

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
#define GENERICSTACK_POP_ANY(stackName) GENERICSTACK_GET_ANY(stackName, --stackName->used)

/* ====================================================================== */
/* Memory release                                                         */
/* We intentionnaly loop on size and not used.                            */
/* ====================================================================== */
#define GENERICSTACK_FREE(stackName) do {				\
    if (stackName != NULL) {						\
      if (stackName->size > 0) {					\
	while (stackName->size > 0) {					\
	  size_t _index = stackName->size-- - 1;			\
	  if (stackName->items[_index].type == GENERICSTACKITEMTYPE_ANY) { \
	    genericStackItemAny_t _any = stackName->items[_index].u.any; \
	    if (_any.free != NULL) {					\
	      _any.free(_any.p);					\
	    }								\
	  }								\
	}								\
	free(stackName->items);						\
      }									\
      free(stackName);							\
      stackName = NULL;							\
    }									\
  } while (0)

#endif /* GENERICSTACK_H */
