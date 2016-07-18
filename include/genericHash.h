#ifndef GENERICHASH_H
#define GENERICHASH_H

#include <stdio.h>
#include <errno.h>
#include <genericStack.h>

/* A hash is nothing else but a generic stack of generic stacks */

typedef size_t (*genericHashKeyIndFunction_t)(void *userDatavp, genericStackItemType_t itemType, void *p);
typedef short  (*genericHashKeyCmpFunction_t)(void *userDatavp, void *dst, void *src);
typedef void  *(*genericHashKeyCopyFunction_t)(void *userDatavp, void *dst, void *src);
typedef void   (*genericHashKeyFreeFunction_t)(void *userDatavp, void *p);
typedef void  *(*genericHashValCopyFunction_t)(void *userDatavp, void *dst, void *src);
typedef void   (*genericHashValFreeFunction_t)(void *userDatavp, void *p);

typedef struct genericHash {
  size_t                       wantedSubSize;
  genericHashKeyIndFunction_t  keyIndFunctionp;
  genericHashKeyCmpFunction_t  keyCmpFunctionp;
  genericHashKeyCopyFunction_t keyCopyFunctionp;
  genericHashKeyFreeFunction_t keyFreeFunctionp;
  genericHashValCopyFunction_t valCopyFunctionp;
  genericHashValFreeFunction_t valFreeFunctionp;
  /* keyStackp and valStackp leave in parallel */
  genericStack_t              *keyStackp;
  genericStack_t              *valStackp;
  short                        error;
} genericHash_t;


/* ====================================================================== */
/* Error detection                                                        */
/* ====================================================================== */
#define GENERICHASH_ERROR(hashName) ((hashName == NULL) || (hashName->error != 0))

/* ====================================================================== */
/* Initialization                                                         */
/*                                                                        */
/* wantedSize is an estimated number of rows in the hash                  */
/* index and comparison function pointers are mandatory.                  */
/* thisWantedSize is an estimated number of rows in every hash's row      */
/* ====================================================================== */
#define GENERICHASH_NEW(hashName, thisKeyIndFunctionp, thisKeyCmpFunctionp, thisKeyCopyFunctionp, thisKeyFreeFunctionp, thisValCopyFunctionp, thisValFreeFunctionp, wantedSize, thisWantedSubSize) do { \
    genericHashKeyIndFunction_t  _keyIndFunctionp  = (genericHashKeyIndFunction_t)  (thisKeyIndFunctionp); \
    genericHashKeyCmpFunction_t  _keyCmpFunctionp  = (genericHashKeyCmpFunction_t)  (thisKeyCmpFunctionp); \
    genericHashKeyCopyFunction_t _keyCopyFunctionp = (genericHashKeyCopyFunction_t) (thisKeyCopyFunctionp); \
    genericHashKeyFreeFunction_t _keyFreeFunctionp = (genericHashKeyFreeFunction_t) (thisKeyFreeFunctionp); \
    genericHashValCopyFunction_t _valCopyFunctionp = (genericHashValCopyFunction_t) (thisValCopyFunctionp); \
    genericHashValFreeFunction_t _valFreeFunctionp = (genericHashValFreeFunction_t) (thisValFreeFunctionp); \
    									\
    if (_keyIndFunctionp == NULL) {					\
      hashName = NULL;							\
    } else {								\
      hashName = malloc(sizeof(genericHash_t));				\
      if (hashName != NULL) {						\
	hashName->wantedSubSize    = (thisWantedSubSize);		\
	hashName->keyIndFunctionp  = _keyIndFunctionp;			\
	hashName->keyCmpFunctionp  = _keyCmpFunctionp;			\
	hashName->keyCopyFunctionp = _keyCopyFunctionp;			\
	hashName->keyFreeFunctionp = _keyFreeFunctionp;			\
	hashName->valCopyFunctionp = _valCopyFunctionp;			\
	hashName->valFreeFunctionp = _valFreeFunctionp;			\
	hashName->error = 0;						\
	GENERICSTACK_NEW_SIZED(hashName->keyStackp, hashName->wantedSubSize); \
	if (GENERICSTACK_ERROR(hashName->keyStackp)) {			\
	  free(hashName);						\
	  hashName = NULL;						\
	}								\
	GENERICSTACK_NEW_SIZED(hashName->valStackp, hashName->wantedSubSize); \
	if (GENERICSTACK_ERROR(hashName->valStackp)) {			\
	  GENERICSTACK_FREE(hashName->keyStackp);			\
	  free(hashName);						\
	  hashName = NULL;						\
	}								\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Copy key/value to internal variables                                   */
/* ====================================================================== */
#define _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, keyValCopy, valType, valVal, valValCopy) do { \
									\
    if ((GENERICSTACKITEMTYPE2TYPE_##keyType == GENERICSTACKITEMTYPE2TYPE_PTR) && (hashName->keyCopyFunctionp != NULL)) { \
      if (hashName->keyCopyFunctionp((void *) userDatavp, , (void *) &_keyValCopy, (void *) &_keyVal) == NULL) { \
	hashName->error = 1;						\
      }									\
    } else {								\
      _keyValCopy = _keyVal;						\
    }									\
    if ((GENERICSTACKITEMTYPE2TYPE_##valType == GENERICSTACKITEMTYPE2TYPE_PTR) &&  (hashName->valCopyFunctionp != NULL)) { \
      if (hashName->valCopyFunctionp((void *) userDatavp, (void *) &_valValCopy, (void *) &_valVal) == NULL) { \
	hashName->error = 1;						\
      }									\
    } else {								\
      _valValCopy = _valVal;						\
    }									\
  } while (0)

/* ====================================================================== */
/* Push internal variables in the hash                                    */
/* ====================================================================== */
#define _GENERICHASH_PUSH(hashName, userDatavp, keyType, keyVal, valType, valVal) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyValCopy;			\
    GENERICSTACKITEMTYPE2TYPE_##valType _valValCopy;			\
									\
    _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, _keyValCopy, valType, valVal, _valValCopy); \
    if (hashName->error == 0) {						\
      GENERICSTACK_PUSH_##keyType(_subKeyStackp, _keyValCopy);		\
      GENERICSTACK_PUSH_##valType(_subValStackp, _valValCopy);		\
      if (GENERICSTACK_ERROR(_subKeyStackp) || GENERICSTACK_ERROR(_subValStackp)) { \
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Set internal variables in the hash                                     */
/* ====================================================================== */
#define _GENERICHASH_SET(hashName, userDatavp, keyType, keyVal, valType, valVal, index) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyValCopy;			\
    GENERICSTACKITEMTYPE2TYPE_##valType _valValCopy;			\
									\
    _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, _keyValCopy, valType, valVal, _valValCopy); \
    if (hashName->error == 0) {						\
      GENERICSTACK_SET_##keyType(_subKeyStackp, _keyValCopy, index);	\
      GENERICSTACK_SET_##valType(_subValStackp, _valValCopy, index);	\
      if (GENERICSTACK_ERROR(_subKeyStackp) || GENERICSTACK_ERROR(_subValStackp)) { \
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Set external variables in the hash                                     */
/* ====================================================================== */
#define GENERICHASH_SET(hashName, userDatavp, keyType, keyVal, valType, valVal) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyVal = (GENERICSTACKITEMTYPE2TYPE_##keyType) (keyVal); \
    hashName->error = 0;						\
									\
    if (hashName->keyIndFunctionp == NULL) {				\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      size_t _subStackIndex = hashName->keyIndFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_keyVal); \
      GENERICHASH_SET_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valVal, _subStackIndex); \
    }									\
  } while (0)

#define GENERICHASH_SET_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valVal, subStackIndex) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyVal = (GENERICSTACKITEMTYPE2TYPE_##keyType) (keyVal); \
    hashName->error = 0;						\
									\
    if (subStackIndex == (size_t) -1) {					\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      GENERICSTACKITEMTYPE2TYPE_##valType _valVal = (GENERICSTACKITEMTYPE2TYPE_##valType) (valVal); \
      if ((subStackIndex >= GENERICSTACK_USED(hashName->keyStackp)) || \
	  (subStackIndex >= GENERICSTACK_USED(hashName->valStackp)) ||	\
	  (GENERICSTACKITEMTYPE(hashName->keyStackp, subStackIndex) != GENERICSTACKITEMTYPE_PTR) || \
	  (GENERICSTACKITEMTYPE(hashName->valStackp, subStackIndex) != GENERICSTACKITEMTYPE_PTR)) { \
	genericStack_t *_subKeyStackp;					\
	genericStack_t *_subValStackp;					\
									\
	GENERICSTACK_NEW_SIZED(_subKeyStackp, hashName->wantedSubSize); \
	GENERICSTACK_NEW_SIZED(_subValStackp, hashName->wantedSubSize); \
	if (GENERICSTACK_ERROR(_subKeyStack) || GENERICSTACK_ERROR(_subValStackp)) { \
	  GENERICSTACK_FREE(_subKeyStack);				\
	  GENERICSTACK_FREE(_subValStack);				\
	  hashName->error = 1;						\
	} else {							\
	  GENERICSTACK_SET_PTR(hashName->keyStackp, _subKeyStackp, subStackIndex); \
	  GENERICSTACK_SET_PTR(hashName->valStackp, _subValStackp, subStackIndex); \
	  if (GENERICSTACK_ERROR(hashName->keyStackp) || GENERICSTACK_ERROR(hashName->valStackp)) { \
	    GENERICSTACK_FREE(_subKeyStack);				\
	    GENERICSTACK_FREE(_subValStack);				\
	    GENERICSTACK_SET_NA(hashName->keyStackp, subStackIndex);	\
	    GENERICSTACK_SET_NA(hashName->valStackp, subStackIndex);	\
	    hashName->error = 1;					\
	  } else {							\
	    _GENERICHASH_PUSH(hashName, userDatavp, keyType, _keyVal, valType, _valVal); \
	  }								\
	}								\
      } else {								\
	genericStack_t *_subKeyStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->keyStackp, subStackIndex); \
	genericStack_t *_subValStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->valStackp, subStackIndex); \
									\
	if (GENERICSTACK_ERROR(hashName->keyStackp) || GENERICSTACK_ERROR(hashName->valStackp)) { \
	  hashName->error = 1;						\
	} else {							\
	  size_t _subStackused = GENERICSTACK_USED(_subKeyStackp);	\
	  size_t _i;							\
									\
	  for (_i = 0; _i < _subStackused; _i++) {			\
	    GENERICSTACKITEMTYPE2TYPE_##keyType _gotKeyVal;		\
	    if ((GENERICSTACKITEMTYPE(_subKeyStackp, _i) != GENERICSTACKITEMTYPE_##keyType)) { \
	      continue;							\
	    }								\
									\
	    _gotKeyVal = GENERICSTACK_GET_##keyType(_subKeyStackp, _i); \
	    _if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyCmpFunctionp != NULL)) {	\
	      if (! hashName->keyCmpFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_keyVal, (void *) &_gotKeyVal)) { \
		continue;						\
	      }								\
	    } else {							\
	      if (_keyVal != _gotKeyVal) {				\
		continue;						\
	      }								\
	    }								\
									\
	    if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyFreeFunctionp != NULL)) { \
	      hashName->keyFreeFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_gotKeyVal); \
	    }								\
	    if ((GENERICSTACKITEMTYPE(_subValStackp, _i) == GENERICSTACKITEMTYPE_PTR)) { \
	      GENERICSTACKITEMTYPE2TYPE_PTR _gotValVal = GENERICSTACK_GET_PTR(_subValStackp, _i); \
	      if (hashName->valFreeFunctionp != NULL) {			\
		hashName->valFreeFunctionp((void *) userDatavp, (void *) &_gotValVal); \
	      }								\
	    }								\
		    							\
	    _GENERICHASH_SET(hashName, userDatavp, keyType, _keyVal, valType, _valVal, _i); \
	    break;							\
	  }								\
	  if (_i >= _subStackused) {					\
	    _GENERICHASH_PUSH(hashName, userDatavp, keyType, _keyVal, valType, _valVal); \
	  }								\
	}								\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Find and eventually remove in the hash                                 */
/* ====================================================================== */
#define _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, _keyVal, valType, valVal, findResult, got, remove) do { \
									\
    if (hashName->keyIndFunctionp == NULL) {				\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      size_t _subStackIndex = hashName->keyIndFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_keyVal); \
      _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, _subStackIndex, findResult, got, remove); \
    }									\
  } while (0)

#define _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, _subStackIndex, findResult, remove) do { \
									\
    if (_subStackIndex == (size_t) -1) {				\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      findResult = 0;							\
									\
      if ((_subStackIndex < GENERICSTACK_USED(hashName->keyStackp))	\
	  &&								\
	  (GENERICSTACKITEMTYPE(hashName->keyStackp, _subStackIndex) == GENERICSTACKITEMTYPE_PTR)) { \
	genericStack_t *_subKeyStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->keyStackp, subStackIndex); \
	genericStack_t *_subValStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->valStackp, subStackIndex); \
									\
	if (GENERICSTACK_ERROR(hashName->keyStackp) || GENERICSTACK_ERROR(hashName->valStackp)) { \
	  hashName->error = 1;						\
	} else {							\
	  size_t _subStackused = GENERICSTACK_USED(_subKeyStackp);	\
	  size_t _i;							\
									\
	  for (_i = 0; _i < _subStackused; _i++) {			\
	    GENERICSTACKITEMTYPE2TYPE_##valType _valVal;		\
	    GENERICSTACKITEMTYPE2TYPE_##keyType _gotKeyVal;		\
									\
	    if ((GENERICSTACKITEMTYPE(_subKeyStackp, _i) != GENERICSTACKITEMTYPE_##keyType)) { \
	      continue;							\
	    }								\
									\
	    _gotKeyVal = GENERICSTACK_GET_##keyType(_subKeyStackp, _i); \
	    _if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyCmpFunctionp != NULL)) {	\
	      if (! hashName->keyCmpFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_keyVal, (void *) &_gotKeyVal)) { \
		continue;						\
	      }								\
	    } else {							\
	      if (_keyVal != _gotKeyVal) {				\
		continue;						\
	      }								\
	    }								\
	    _valVal = GENERICSTACK_GET_##valType(_subValStackp, _i);	\
	    if ((hashName->valCopyFunctionp != NULL) && (hashName->valCopyFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##valType, (void *) &valVal, (void *) &_valVal) == NULL)) { \
	      hashName->error = 1;					\
	      break;							\
	    } else {							\
	      valVal = _valVal;						\
	    }								\
	    findResult = 1;						\
            if (remove) {						\
	      if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyFreeFunctionp != NULL)) { \
		hashName->keyFreeFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void *) &_gotKeyVal); \
	      }								\
              GENERICSTACK_SET_NA(_subKeyStackp, _i);			\
              GENERICSTACK_SWITCH(_subKeyStackp, _i, -1);		\
              GENERICSTACK_POP_NA(_subKeyStackp);			\
									\
	      if ((GENERICSTACKITEMTYPE_##valType == GENERICSTACKITEMTYPE_PTR)) { \
		if (hashName->valFreeFunctionp != NULL) {		\
		  hashName->valFreeFunctionp((void *) userDatavp, (void *) &valVal); \
		}							\
	      }								\
              GENERICSTACK_SET_NA(_subValStackp, _i);			\
              GENERICSTACK_SWITCH(_subValStackp, _i, -1);		\
              GENERICSTACK_POP_NA(_subValStackp);			\
	    }								\
            break;							\
          }								\
        }								\
      }									\
    }									\
} while (0)

#define GENERICHASH_FIND(hashName, userDatavp, keyType, _keyVal, valType, valVal, findResult) _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, _keyVal, valType, valVal, findResult, 0)
#define GENERICHASH_REMOVE(hashName, userDatavp, keyType, _keyVal, valType, valVal, findResult) _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, _keyVal, valType, valVal, findResult, 1)

#define GENERICHASH_FIND_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, index, findResult) _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, index, findResult, 0)
#define GENERICHASH_REMOVE_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, index, findResult) _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, _keyVal, valType, valVal, index, findResult, 1)

/* ====================================================================== */
/* Memory release                                                         */
/* ====================================================================== */
#define GENERICHASH_FREE(hashName) do {					\
    if (hashName != NULL) {						\
      size_t _usedl  = GENERICSTACK_USED(hashName->keyStackp);		\
      size_t _i;							\
      for (_i = 0; _i < _usedl; _i++) {					\
	if (GENERICSTACKITEMTYPE(hashName->stackp, _i) == GENERICSTACKITEMTYPE_PTR) { \
	  genericStack_t *_subStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->stackp, _i); \
	  GENERICSTACK_FREE(_subStackp);				\
	}								\
      }									\
      GENERICSTACK_FREE(hashName->stackp);				\
      free(hashName);							\
      hashName = NULL;							\
    }									\
  } while (0)

#endif /* GENERICHASH_H */
