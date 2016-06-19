#ifndef GENERICHASH_H
#define GENERICHASH_H

#include <stdio.h>
#include <genericStack.h>

/* A hash is nothing else but a generic stack of generic stacks */

typedef size_t (*genericHashFunction_t)(void *userDatavp, genericStackItemType_t itemType, void *p);
typedef short  (*genericHashCmpAnyFunction_t)(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);

typedef struct genericHash {
  short                 error;
  size_t                wantedSubSize;
  genericHashFunction_t hashFunctionp;
  GENERICSTACK_DECL(stackp);
} genericHash_t;


/* ====================================================================== */
/* Declaration                                                            */
/* ====================================================================== */
#define GENERICHASH_DECL(hashName) genericHash_t * hashName

/* ====================================================================== */
/* Error detection                                                        */
/* ====================================================================== */
#define GENERICHASH_ERROR(hashName) ((hashName == NULL) || (hashName->error))

/* ====================================================================== */
/* Initialization                                                         */
/*                                                                        */
/* hashFunctionp must be a valid C (function pointer) identifier          */
/* wantedSize is an estimated number of rows in the hash                  */
/* thisWantedSize is an estimated number of rows in the hash's rows       */
/* ====================================================================== */
/* hashName must be a valid C identifier */
#define _GENERICHASH_NEW_SIZED(hashName, thisHashFunctionp, wantedSize, thisWantedSubSize) do { \
    genericHashFunction_t _hashFunctionp = (genericHashFunction_t) (thisHashFunctionp); \
									\
    if (_hashFunctionp == NULL) {					\
      hashName = NULL;							\
    } else {								\
      hashName = malloc(sizeof(genericHash_t));				\
      if (hashName != NULL) {						\
	hashName->error = 0;						\
	hashName->wantedSubSize = (thisWantedSubSize);			\
	hashName->hashFunctionp = _hashFunctionp;			\
	GENERICSTACK_NEW_SIZED(hashName->stackp, (wantedSize));		\
	if (GENERICSTACK_ERROR(hashName->stackp)) {			\
	  free(hashName);						\
	  hashName = NULL;						\
	}								\
      }									\
    }									\
  } while (0)

#define GENERICHASH_NEW(hashName, hashFunctionp) _GENERICHASH_NEW_SIZED(hashName, hashFunctionp, 0, 0)
#define GENERICHASH_NEW_SIZED(hashName, hashFunctionp, wantedSize, wantedSubSize) _GENERICHASH_NEW_SIZED(hashName, hashFunctionp, (wantedSize), (wantedSubSize)) 

/* ====================================================================== */
/* Insert in the hash: there is a clear distinction if ANY or not         */
/* ====================================================================== */
#define GENERICHASH_INSERT(hashName, userDatavp, itemType, value) do {	\
    void                                *_userDatavp = (void *) userDatavp; \
    genericStackItemType_t               _varType = GENERICSTACKITEMTYPE_##itemType; \
    GENERICSTACKITEMTYPE2TYPE_##itemType _var = (GENERICSTACKITEMTYPE2TYPE_##itemType) (value);	\
    size_t                               _subStackIndex = hashName->hashFunctionp(_userDatavp, _varType, (void *) &_var); \
    GENERICSTACK_DECL(_subStackp);					\
									\
    if (_subStackIndex == (size_t) -1) {				\
      hashName->error = 1;						\
    } else if ((_subStackIndex >= GENERICSTACK_SIZE(hashName->stackp)) \
	       ||						      \
	       (GENERICSTACKITEMTYPE(hashName->stackp, _subStackIndex) != GENERICSTACKITEMTYPE_PTR)) { \
      GENERICSTACK_NEW_SIZED(_subStackp, hashName->wantedSubSize);	\
      if (GENERICSTACK_ERROR(_subStackp)) {				\
	hashName->error = 1;						\
      } else {								\
	GENERICSTACK_SET_PTR(hashName->stackp, _subStackp, _subStackIndex);	\
	if (GENERICSTACK_ERROR(hashName->stackp)) {			\
	  GENERICSTACK_FREE(_subStackp);					\
	  hashName->error = 1;						\
	} else {							\
	  GENERICSTACK_PUSH_##itemType(_subStackp, _var);		\
	  if (GENERICSTACK_ERROR(_subStackp)) {				\
	    hashName->error = 1;					\
	  }								\
	}								\
      }									\
    } else {								\
      _subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _subStackIndex); \
      GENERICSTACK_PUSH_##itemType(_subStackp, _var);			\
      if (GENERICSTACK_ERROR(_subStackp)) {				\
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

#define GENERICHASH_INSERT_ANY(hashName, userDatavp, varIdentifier, clonep, freep) do {	\
    void                                *_userDatavp = (void *) userDatavp; \
    genericStackItemType_t               _varType = GENERICSTACKITEMTYPE_##itemType; \
    GENERICSTACKITEMTYPE2TYPE_##itemType _var = (GENERICSTACKITEMTYPE2TYPE_##itemType) (value);	\
    size_t                               _subStackIndex = hashName->hashFunctionp(_userDatavp, _varType, (void *) &_var); \
    GENERICSTACK_DECL(subStackp);					\
									\
    if (_subStackIndex == (size_t) -1) {				\
      hashName->error = 1;						\
    } else if ((_subStackIndex >= GENERICSTACK_SIZE(hashName->stackp))	\
	       ||							\
	       (GENERICSTACKITEMTYPE(hashName->stackp, _subStackIndex) != GENERICSTACKITEMTYPE_PTR)) { \
      GENERICSTACK_NEW_SIZED(subStackp, hashName->wantedSubSize);	\
      if (GENERICSTACK_ERROR(subStackp)) {				\
	hashName->error = 1;						\
      } else {								\
	GENERICSTACK_PUSH_ANY(subStackp, _var, (clonep), (freep));	\
	if (GENERICSTACK_ERROR(subStackp)) {				\
	  hashName->error = 1;						\
	}								\
      }									\
    } else {								\
      subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _subStackIndex); \
      GENERICSTACK_PUSH_ANY(subStackp, _var, (clonep), (freep));	\
      if (GENERICSTACK_ERROR(subStackp)) {				\
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Find and eventually remove in the hash                                 */
/*                                                                        */
/* wanted variable, cmpAnyFunctionp and result must be C identifiers      */
/* ====================================================================== */
#define _GENERICHASH_FIND_REMOVE(hashName, userDatavp, wantedType, wantedValue, cmpAnyFunctionp, result, remove) do { \
    genericStackItemType_t                 _wantedType = GENERICSTACKITEMTYPE_##wantedType; \
    GENERICSTACKITEMTYPE2TYPE_##wantedType _wantedVar = (GENERICSTACKITEMTYPE2TYPE_##wantedType) (wantedValue); \
    genericHashCmpAnyFunction_t            _cmpAnyFunctionp = (genericHashCmpAnyFunction_t) (cmpAnyFunctionp); \
									\
    if (_cmpAnyFunctionp == NULL) { \
      (result) = 0;							\
    } else {								\
      void   *_userDatavp = (void *) userDatavp;			\
      size_t  _subStackIndex = hashName->hashFunctionp(_userDatavp, _wantedType, (void *) &_wantedVar); \
									\
      if (_subStackIndex == (size_t) -1) {				\
	hashName->error = 1;						\
	(result) = 0;							\
      } else if ((_subStackIndex >= GENERICSTACK_SIZE(hashName->stackp))	\
		 ||							\
		 (GENERICSTACKITEMTYPE(hashName->stackp, _subStackIndex) != GENERICSTACKITEMTYPE_PTR)) { \
	(result) = 0;							\
      } else {								\
	GENERICSTACK_DECL(_subStackp);					\
	size_t          _subStackused;					\
									\
	_subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _subStackIndex); \
	_subStackused = GENERICSTACK_SIZE(_subStackp);			\
	if (_subStackused <= 0) {					\
	  (result) = 0;							\
	} else {							\
	  int                 _i;					\
	  genericStackItem_t *_itemp;					\
	  short               _result;					\
									\
	  for (_i = 0, _itemp = _subStackp->items; _i < _subStackused; _i++, _itemp++) { \
	    if (_itemp->type != _wantedType) {				\
	      continue;							\
	    }								\
	    _result = cmpAnyFunctionp(_userDatavp, _wantedType, (void *) &_wantedVar, &(GENERICSTACK_GET_##wantedType(_subStackp, _i))); \
	    if (_result) {						\
	      if (remove) {						\
		switch (_wantedType) {					\
		case GENERICSTACKITEMTYPE_ANY:				\
		  GENERICSTACK_SET_ANY(_subStackp, NULL, NULL, NULL, _i); \
		  _itemp->type = _GENERICSTACKITEMTYPE_NA;		\
		  break;						\
		default:						\
		  _itemp->type = _GENERICSTACKITEMTYPE_NA;		\
		  break;						\
		}							\
	      }								\
	      break;							\
	    }								\
	  }								\
									\
	  (result) = _result;						\
	}								\
      }									\
    }									\
} while (0)

#define GENERICHASH_FIND(hashName, userDatavp, wantedType, wantedVar, cmpFunctionp, result) _GENERICHASH_FIND_REMOVE(hashName, (userDatavp), wantedType, (wantedVar), (cmpFunctionp), result, 0)
#define GENERICHASH_REMOVE(hashName, userDatavp, wantedType, wantedVar, cmpFunctionp, result) _GENERICHASH_FIND_REMOVE(hashName, (userDatavp), wantedType, (wantedVar), (cmpFunctionp), result, 1)

/* ====================================================================== */
/* Memory release                                                         */
/* ====================================================================== */
#define GENERICHASH_FREE(hashName) do {					\
    if (hashName != NULL) {						\
      size_t _usedl  = GENERICSTACK_SIZE(hashName->stackp);		\
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
