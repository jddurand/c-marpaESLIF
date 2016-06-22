#ifndef GENERICHASH_H
#define GENERICHASH_H

#include <stdio.h>
#include <genericStack.h>

/* A hash is nothing else but a generic stack of generic stacks */

typedef size_t (*genericHashIndFunction_t)(void *userDatavp, genericStackItemType_t itemType, void *p);
typedef short  (*genericHashCmpFunction_t)(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);

typedef struct genericHash {
  size_t                   wantedSubSize;
  genericHashIndFunction_t indFunctionp;
  genericHashCmpFunction_t cmpFunctionp;
  genericStack_t          *stackp;
  short                    error;
  short                    unique;
} genericHash_t;


/* ====================================================================== */
/* Error detection                                                        */
/* ====================================================================== */
#define GENERICHASH_ERROR(hashName) ((hashName == NULL) || (hashName->error))

/* ====================================================================== */
/* Initialization                                                         */
/*                                                                        */
/* thisIndFunctionp and thisCmpFunctionp must be valid C identifiers      */
/* wantedSize is an estimated number of rows in the hash                  */
/* thisWantedSize is an estimated number of rows in the hash's rows       */
/* ====================================================================== */
/* hashName must be a valid C identifier */
#define _GENERICHASH_NEW_SIZED(hashName, thisIndFunctionp, thisCmpFunctionp, thisUnique, wantedSize, thisWantedSubSize) do { \
    genericHashIndFunction_t _indFunctionp = (genericHashIndFunction_t) (thisIndFunctionp); \
    genericHashCmpFunction_t _cmpFunctionp = (genericHashCmpFunction_t) (thisCmpFunctionp); \
                                                                        \
    if ((_indFunctionp == NULL) || (_cmpFunctionp == NULL)) {           \
      hashName = NULL;							\
    } else {								\
      hashName = malloc(sizeof(genericHash_t));				\
      if (hashName != NULL) {						\
	hashName->wantedSubSize = (thisWantedSubSize);			\
	hashName->indFunctionp = _indFunctionp;                         \
	hashName->cmpFunctionp = _cmpFunctionp;                         \
	hashName->error = 0;						\
	hashName->unique = thisUnique;                                  \
	GENERICSTACK_NEW_SIZED(hashName->stackp, (wantedSize));		\
	if (GENERICSTACK_ERROR(hashName->stackp)) {			\
	  free(hashName);						\
	  hashName = NULL;						\
	}								\
      }									\
    }									\
  } while (0)

#define GENERICHASH_NEW(hashName, indFunctionp, cmpFunctionp, thisUnique) _GENERICHASH_NEW_SIZED(hashName, indFunctionp, cmpFunctionp, thisUnique, 0, 0)
#define GENERICHASH_NEW_SIZED(hashName, indFunctionp, cmpFunctionp, thisUnique, wantedSize, wantedSubSize) _GENERICHASH_NEW_SIZED(hashName, indFunctionp, cmpFunctionp, thisUnique, (wantedSize), (wantedSubSize)) 

/* ====================================================================== */
/* Set in the hash                                                        */
/* ====================================================================== */
#define GENERICHASH_SET(hashName, userDatavp, itemType, value) do {	\
    GENERICSTACKITEMTYPE2TYPE_##itemType _var = (GENERICSTACKITEMTYPE2TYPE_##itemType) (value);	\
    size_t                               _subStackIndex = hashName->indFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##itemType, (void *) &_var); \
									\
    if (_subStackIndex == (size_t) -1) {				\
      hashName->error = 1;						\
    } else if ((_subStackIndex >= GENERICSTACK_USED(hashName->stackp))	\
	       ||							\
	       (GENERICSTACKITEMTYPE(hashName->stackp, _subStackIndex) != GENERICSTACKITEMTYPE_PTR)) { \
      genericStack_t *_subStackp;					\
      GENERICSTACK_NEW_SIZED(_subStackp, hashName->wantedSubSize);	\
      if (GENERICSTACK_ERROR(_subStackp)) {				\
	hashName->error = 1;						\
      } else {								\
	GENERICSTACK_SET_PTR(hashName->stackp, _subStackp, _subStackIndex); \
	if (GENERICSTACK_ERROR(hashName->stackp)) {			\
	  GENERICSTACK_FREE(_subStackp);				\
	  hashName->error = 1;						\
	} else {							\
	  GENERICSTACK_PUSH_##itemType(_subStackp, _var);		\
	  if (GENERICSTACK_ERROR(_subStackp)) {				\
	    hashName->error = 1;					\
	  }								\
	}								\
      }									\
    } else {								\
      genericStack_t *_subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _subStackIndex); \
                                                                        \
      if (! hashName->unique) {                                         \
        GENERICSTACK_PUSH_##itemType(_subStackp, _var);                 \
      } else {                                                          \
        size_t _subStackused = GENERICSTACK_USED(_subStackp);           \
                                                                        \
        if (_subStackused > 0) {                                        \
          size_t _i;                                                    \
                                                                        \
          for (_i = 0; _i < _subStackused; _i++) {                      \
            GENERICSTACKITEMTYPE2TYPE_##itemType _gotVar;               \
                                                                        \
              if (GENERICSTACKITEMTYPE(_subStackp, _i) != GENERICSTACKITEMTYPE_##itemType) { \
                continue;                                               \
              }								\
              _gotVar = GENERICSTACK_GET_##itemType(_subStackp, _i);    \
                if (hashName->cmpFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##itemType, (void *) &_var, (void *) &_gotVar)) { \
                  GENERICSTACK_SET_##itemType(_subStackp, _var, _i);    \
                    break;                                              \
                }                                                       \
          }								\
          if (_i >= _subStackused) {                                    \
            GENERICSTACK_PUSH_##itemType(_subStackp, _var);             \
          }                                                             \
        } else {                                                        \
          GENERICSTACK_PUSH_##itemType(_subStackp, _var);               \
        }                                                               \
      }                                                                 \
      if (GENERICSTACK_ERROR(_subStackp)) {				\
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Find and eventually remove in the hash                                 */
/*                                                                        */
/* wanted variable and result must be C identifiers                       */
/* Removal takes care to not let a sparse entry in the stack              */
/* ====================================================================== */
#define _GENERICHASH_FIND_REMOVE(hashName, userDatavp, wantedType, wantedValue, findResult, got, remove) do { \
    GENERICSTACKITEMTYPE2TYPE_##wantedType _wantedVar = (GENERICSTACKITEMTYPE2TYPE_##wantedType) (wantedValue); \
    size_t  _subStackIndex = hashName->indFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##wantedType, (void *) &_wantedVar); \
    findResult = 0;							\
									\
    if (_subStackIndex == (size_t) -1) {				\
      hashName->error = 1;						\
    } else if ((_subStackIndex < GENERICSTACK_USED(hashName->stackp))	\
               &&							\
               (GENERICSTACKITEMTYPE(hashName->stackp, _subStackIndex) == GENERICSTACKITEMTYPE_PTR)) { \
      genericStack_t *_subStackp;					\
      size_t          _subStackused;					\
									\
      _subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _subStackIndex); \
      _subStackused = GENERICSTACK_USED(_subStackp);			\
      if (_subStackused > 0) {						\
        size_t _i;                                                      \
	                                                                \
        for (_i = 0; _i < _subStackused; _i++) {                        \
          GENERICSTACKITEMTYPE2TYPE_##wantedType _gotVar;               \
          if (GENERICSTACKITEMTYPE(_subStackp, _i) != GENERICSTACKITEMTYPE_##wantedType) {	\
            continue;                                                   \
          }								\
          _gotVar = GENERICSTACK_GET_##wantedType(_subStackp, _i);      \
          if (hashName->cmpFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##wantedType, (void *) &_wantedVar, (void *) &_gotVar)) { \
	    findResult = 1;						\
	    got = GENERICSTACK_GET_##wantedType(_subStackp, _i);	\
            if (remove) {						\
              GENERICSTACK_SET_NA(_subStackp, _i);                      \
              GENERICSTACK_SWITCH(_subStackp, _i, -1);                  \
              GENERICSTACK_POP_NA(_subStackp);                          \
	    }								\
            break;							\
          }								\
        }								\
      }									\
    }									\
} while (0)

#define GENERICHASH_FIND(hashName, userDatavp, wantedType, wantedVar, findResult, got) _GENERICHASH_FIND_REMOVE(hashName, (userDatavp), wantedType, wantedVar, findResult, got, 0)
#define GENERICHASH_REMOVE(hashName, userDatavp, wantedType, wantedVar, findResult, got) _GENERICHASH_FIND_REMOVE(hashName, userDatavp, wantedType, wantedVar, findResult, got, 1)

/* ====================================================================== */
/* Memory release                                                         */
/* ====================================================================== */
#define GENERICHASH_FREE(hashName) do {					\
    if (hashName != NULL) {						\
      size_t _usedl  = GENERICSTACK_USED(hashName->stackp);		\
      if (_usedl > 0) {							\
	size_t _i;							\
	for (_i = 0; _i < _usedl; _i++) {				\
	  if (GENERICSTACKITEMTYPE(hashName->stackp, _i) == GENERICSTACKITEMTYPE_PTR) {	\
	    genericStack_t *_subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _i); \
	    GENERICSTACK_FREE(_subStackp);				\
	  }								\
	}								\
      }									\
      GENERICSTACK_FREE(hashName->stackp);				\
      free(hashName);							\
      hashName = NULL;							\
    }									\
  } while (0)

#endif /* GENERICHASH_H */
