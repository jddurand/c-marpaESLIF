#ifndef GENERICSTACK_H
#define GENERICSTACK_H

#include <stdlib.h>       /* For malloc, memcpy, free */
#include <stddef.h>       /* For size_t */

/* =============== */
/* C generic stack */
/* =============== */

#define GENERICSTACK_HAVE_LONG_LONG 0

typedef struct genericStackItemAny {
  void *p;
  size_t _size;
  void *(*malloc)(size_t size);
  void *(*memcpy)(void *dst, void *src, size_t size);
  void *(*free)(void *p);
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
  GENERICSTACKITEMTYPE_ANY
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
    long ll;
#endif
    genericStackItemAny_t any;
  } u;
} genericStackItem_t;

typedef struct genericStack {
  size_t size;
  size_t used;
  genericStackItem_t *items;
} genericStack_t;

#define GENERICSTACK_NAME(stackName) _ ## GENERICSTACK_ ## stackName
#define GENERICSTACK_SIZE(stackName)  GENERICSTACK_NAME(stackName)->size
#define GENERICSTACK_USED(stackName)  GENERICSTACK_NAME(stackName)->used
#define GENERICSTACK_ITEMS(stackName) GENERICSTACK_NAME(stackName)->items

#define GENERICSTACK_DECL(stackName) genericStack_t * GENERICSTACK_NAME(stackName)

#define GENERICSTACK_NEW(stackName) do {				\
    GENERICSTACK_NAME(stackName) = malloc(sizeof(genericStack_t));	\
    GENERICSTACK_SIZE(stackName) = 0;					\
    GENERICSTACK_USED(stackName) = 0;					\
    GENERICSTACK_ITEMS(stackName) = NULL;				\
  } while (0)

#define GENERICSTACK_NEW_SIZED(stackName, size) do {			\
    size_t _size = (size);						\
    GENERICSTACK_NEW(stackName);					\
    GENERICSTACK_ITEMS(stackName) = malloc(sizeof(genericStackItem_t) * (GENERICSTACK_SIZE(stackName) = _size)); \
    GENERICSTACK_SIZE(stackName) = _size;				\
  } while (0)

#define GENERICSTACK_EXTEND(stackName, wantedSize) do {			\
    size_t _wantedSize = (wantedSize);					\
    if (_wantedSize > GENERICSTACK_USED(stackName)) { 			\
      GENERICSTACK_ITEMS(stackName) = realloc(GENERICSTACK_ITEMS(stackName), sizeof(genericStackItem_t) * _wantedSize);	\
      GENERICSTACK_SIZE(stackName) = _wantedSize;			\
    }									\
  } while (0)

#define GENERICSTACK_PUSH_BASIC_TYPE(stackName, type, var, itemType, dst) do { \
    size_t _i = GENERICSTACK_USED(stackName)++;				\
									\
    GENERICSTACK_EXTEND(stackName, GENERICSTACK_USED(stackName));	\
    GENERICSTACK_ITEMS(stackName)[_i].type = (itemType);		\
    GENERICSTACK_ITEMS(stackName)[_i].u.dst = (type) (var);		\
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

#define GENERICSTACK_PUSH_ANY(stackName, type, var, mallocp, memcpyp, freep) do { \
    size_t _i = GENERICSTACK_USED(stackName)++;				\
    size_t _size = sizeof(type);					\
    void *_p;								\
    void *_malloc = (void (*)()) (mallocp);				\
    void *_memcpy = (void (*)()) (memcpyp);				\
    void *_free = (void (*)()) (freefp);				\
    									\
    GENERICSTACK_EXTEND(stackName, GENERICSTACK_USED(stackName));	\
    p = (mallocp == NULL) ? malloc(_size) : mallocp(_size);		\
    if (memcpyp == NULL) { memcpy(p, var, _size); } else { memcpyp(p, var, _size); } \
    GENERICSTACK_ITEMS(stackName)[_i].type = GENERICSTACKITEMTYPE_ANY;	\
    GENERICSTACK_ITEMS(stackName)[_i].u.any.p = p;			\
    GENERICSTACK_ITEMS(stackName)[_i].u.any.size = _size;		\
    GENERICSTACK_ITEMS(stackName)[_i].u.any.malloc = mallocp;		\
    GENERICSTACK_ITEMS(stackName)[_i].u.any.memcpy = memcpyp;		\
    GENERICSTACK_ITEMS(stackName)[_i].u.any.free = freep;		\
									\
  } while (0)

#define GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, dst) var = (type) GENERICSTACK_ITEMS(stackName)[--GENERICSTACK_USED(stackName)].u.dst

#define GENERICSTACK_POP_CHAR(stackName, var, type)   GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, c)
#define GENERICSTACK_POP_SHORT(stackName, var, type)  GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, s)
#define GENERICSTACK_POP_INT(stackName, var, type)    GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, i)
#define GENERICSTACK_POP_LONG(stackName, var, type)   GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, l)
#define GENERICSTACK_POP_FLOAT(stackName, var, type)  GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, f)
#define GENERICSTACK_POP_DOUBLE(stackName, var, type) GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, d)
#define GENERICSTACK_POP_PTR(stackName, var, type)    GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, p)
#if GENERICSTACK_HAVE_LONG_LONG > 0
#define GENERICSTACK_POP_LONG_LONG(stackName, var, type)    GENERICSTACK_POP_BASIC_TYPE(stackName, type, var, ll)
#endif

#define GENERICSTACK_POP_ANY(stackName, var, type) do {	\
    void *p;								\
    genericStackItemAny_t any = GENERICSTACK_ITEMS(stackName)[--GENERICSTACK_USED(stackName)].u.any; \
									\
    p = (any.malloc == NULL) ? malloc(any.size) : any.malloc(any.size); \
    if (any.memcpy == NULL) { memcpy(p, any.p, any.size); } else { any.memcpy(p, any.p, any.size); } \
    if (any.free == NULL) { free(any.p); } else { any.free(any.p); }	\
    (var) = (type) p;							\
  } while (0)

#define GENERICSTACK_FREE(stackName) do {				\
    size_t _i;								\
    size_void *_p;							\
    while ((i = --GENERICSTACK_USED(stackName)) > 0) {			\
      if (GENERICSTACK_ITEMS(stackName)[--GENERICSTACK_USED(stackName)].type == GENERICSTACKITEMTYPE_ANY) { \
	GENERICSTACK_POP_ANY(stackName, p, void *)			\
      }									\
    }									\
    free(GENERICSTACK_NAME(stackName));					\
  } while (0);

#endif /* GENERICSTACK_H */
