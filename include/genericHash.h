#ifndef GENERICHASH_H
#define GENERICHASH_H

#include <stdio.h>
#include <errno.h>
#include <genericStack.h>

/* A hash is nothing else but a generic stack of generic stacks */

typedef size_t (*genericHashKeyIndFunction_t)(void *userDatavp, genericStackItemType_t itemType, void **pp);
typedef short  (*genericHashKeyCmpFunction_t)(void *userDatavp, void *p1, void *p2);
typedef void  *(*genericHashKeyCopyFunction_t)(void *userDatavp, void *p);
typedef void   (*genericHashKeyFreeFunction_t)(void *userDatavp, void *p);
typedef void  *(*genericHashValCopyFunction_t)(void *userDatavp, void *p);
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
/* For getters and setters                                                */
/* ====================================================================== */
#define GENERICHASH_KEYCMPFUNCTION(hashName) hashName->keyCmpFunctionp
#define GENERICHASH_KEYCOPYFUNCTION(hashName) hashName->keyCopyFunctionp
#define GENERICHASH_KEYFREEFUNCTION(hashName) hashName->keyFreeFunctionp
#define GENERICHASH_VALCOPYFUNCTION(hashName) hashName->valCopyFunctionp
#define GENERICHASH_VALFREEFUNCTION(hashName) hashName->valFreeFunctionp

/* ====================================================================== */
/* Initialization                                                         */
/*                                                                        */
/* wantedSize is an estimated number of rows in the hash                  */
/* index and comparison function pointers are mandatory.                  */
/* thisWantedSize is an estimated number of rows in every hash's row      */
/* ====================================================================== */
#define GENERICHASH_NEW_ALL(hashName, thisKeyIndFunctionp, thisKeyCmpFunctionp, thisKeyCopyFunctionp, thisKeyFreeFunctionp, thisValCopyFunctionp, thisValFreeFunctionp, wantedSize, thisWantedSubSize) do { \
    genericHashKeyIndFunction_t  _keyIndFunctionp  = (genericHashKeyIndFunction_t)  (thisKeyIndFunctionp); \
    genericHashKeyCmpFunction_t  _keyCmpFunctionp  = (genericHashKeyCmpFunction_t)  (thisKeyCmpFunctionp); \
    genericHashKeyCopyFunction_t _keyCopyFunctionp = (genericHashKeyCopyFunction_t) (thisKeyCopyFunctionp); \
    genericHashKeyFreeFunction_t _keyFreeFunctionp = (genericHashKeyFreeFunction_t) (thisKeyFreeFunctionp); \
    genericHashValCopyFunction_t _valCopyFunctionp = (genericHashValCopyFunction_t) (thisValCopyFunctionp); \
    genericHashValFreeFunction_t _valFreeFunctionp = (genericHashValFreeFunction_t) (thisValFreeFunctionp); \
    									\
    if (_keyIndFunctionp == NULL) {                                     \
      hashName = NULL;                                                  \
    } else {                                                            \
      hashName = malloc(sizeof(genericHash_t));				\
      if (hashName != NULL) {						\
        hashName->wantedSubSize    = (thisWantedSubSize);               \
        hashName->keyIndFunctionp  = _keyIndFunctionp;			\
        hashName->keyCmpFunctionp  = _keyCmpFunctionp;			\
        hashName->keyCopyFunctionp = _keyCopyFunctionp;			\
        hashName->keyFreeFunctionp = _keyFreeFunctionp;			\
        hashName->valCopyFunctionp = _valCopyFunctionp;			\
        hashName->valFreeFunctionp = _valFreeFunctionp;			\
        hashName->error = 0;						\
        GENERICSTACK_NEW_SIZED(hashName->keyStackp, hashName->wantedSubSize); \
        if (GENERICSTACK_ERROR(hashName->keyStackp)) {			\
          free(hashName);                                               \
          hashName = NULL;						\
        }                                                               \
        GENERICSTACK_NEW_SIZED(hashName->valStackp, hashName->wantedSubSize); \
        if (GENERICSTACK_ERROR(hashName->valStackp)) {			\
          GENERICSTACK_FREE(hashName->keyStackp);                       \
          free(hashName);                                               \
          hashName = NULL;						\
        }                                                               \
      }									\
    }                                                                   \
  } while (0)

#define GENERICHASH_NEW(hashName, thisKeyIndFunctionp) GENERICHASH_NEW_ALL(hashName, thisKeyIndFunctionp, NULL, NULL, NULL, NULL, NULL, 0, 0)

/* ====================================================================== */
/* Copy key/value to internal variables                                   */
/* ====================================================================== */
#define _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, keyValCopy, valType, valVal, valValCopy) do { \
									\
    if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyCopyFunctionp != NULL)) { \
      void *_p = hashName->keyCopyFunctionp((void *) userDatavp, (void *) &keyVal); \
      if (((void *) keyVal != NULL) && (_p == NULL)) {                  \
	hashName->error = 1;						\
      } else {                                                          \
        keyValCopy = (GENERICSTACKITEMTYPE2TYPE_##keyType) _p;          \
      }                                                                 \
    } else {								\
      keyValCopy = keyVal;						\
    }									\
    if ((GENERICSTACKITEMTYPE_##valType == GENERICSTACKITEMTYPE_PTR) && (hashName->valCopyFunctionp != NULL)) { \
      void *_p = hashName->valCopyFunctionp((void *) userDatavp, (void *) &valVal); \
      if (((void *) valVal != NULL) && (_p == NULL)) {                  \
	hashName->error = 1;						\
      } else {                                                          \
        valValCopy = (GENERICSTACKITEMTYPE2TYPE_##valType) _p;          \
      }                                                                 \
    } else {								\
      valValCopy = valVal;						\
    }									\
  } while (0)

/* ====================================================================== */
/* Push internal variables in the hash                                    */
/* ====================================================================== */
#define _GENERICHASH_PUSH(hashName, userDatavp, keyType, keyVal, valType, valVal, subKeyStackp, subValStackp) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyValCopy;			\
    GENERICSTACKITEMTYPE2TYPE_##valType _valValCopy;			\
									\
    _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, _keyValCopy, valType, valVal, _valValCopy); \
    if (hashName->error == 0) {						\
      GENERICSTACK_PUSH_##keyType(subKeyStackp, _keyValCopy);		\
      GENERICSTACK_PUSH_##valType(subValStackp, _valValCopy);		\
      if (GENERICSTACK_ERROR(subKeyStackp) || GENERICSTACK_ERROR(subValStackp)) { \
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Set internal variables in the hash                                     */
/* ====================================================================== */
#define _GENERICHASH_SET(hashName, userDatavp, keyType, keyVal, valType, valVal, subKeyStackp, subValStackp, index) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyValCopy;			\
    GENERICSTACKITEMTYPE2TYPE_##valType _valValCopy;			\
									\
    _GENERICHASH_COPY(hashName, userDatavp, keyType, keyVal, _keyValCopy, valType, valVal, _valValCopy); \
    if (hashName->error == 0) {						\
      GENERICSTACK_SET_##keyType(subKeyStackp, _keyValCopy, index);	\
      GENERICSTACK_SET_##valType(subValStackp, _valValCopy, index);	\
      if (GENERICSTACK_ERROR(subKeyStackp) || GENERICSTACK_ERROR(subValStackp)) { \
	hashName->error = 1;						\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Set external variables in the hash                                     */
/* ====================================================================== */
#define GENERICHASH_SET(hashName, userDatavp, keyType, keyVal, valType, valVal) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyVal = (GENERICSTACKITEMTYPE2TYPE_##keyType) (keyVal); \
    size_t _subStackIndex = hashName->keyIndFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void **) &_keyVal); \
									\
    GENERICHASH_SET_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valVal, _subStackIndex); \
  } while (0)

#define GENERICHASH_SET_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valVal, subStackIndex) do { \
    GENERICSTACKITEMTYPE2TYPE_##keyType _keyVal = (GENERICSTACKITEMTYPE2TYPE_##keyType) (keyVal); \
    hashName->error = 0;						\
									\
    if (subStackIndex < 0 ) {                                           \
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
	if (GENERICSTACK_ERROR(_subKeyStackp) || GENERICSTACK_ERROR(_subValStackp)) { \
	  GENERICSTACK_FREE(_subKeyStackp);				\
	  GENERICSTACK_FREE(_subValStackp);				\
	  hashName->error = 1;						\
	} else {							\
	  GENERICSTACK_SET_PTR(hashName->keyStackp, _subKeyStackp, subStackIndex); \
	  GENERICSTACK_SET_PTR(hashName->valStackp, _subValStackp, subStackIndex); \
	  if (GENERICSTACK_ERROR(hashName->keyStackp) || GENERICSTACK_ERROR(hashName->valStackp)) { \
	    GENERICSTACK_FREE(_subKeyStackp);				\
	    GENERICSTACK_FREE(_subValStackp);				\
	    GENERICSTACK_SET_NA(hashName->keyStackp, subStackIndex);	\
	    GENERICSTACK_SET_NA(hashName->valStackp, subStackIndex);	\
	    hashName->error = 1;					\
	  } else {							\
	    _GENERICHASH_PUSH(hashName, userDatavp, keyType, _keyVal, valType, _valVal, _subKeyStackp, _subValStackp); \
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
	    if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyCmpFunctionp != NULL)) {	\
	      if (! hashName->keyCmpFunctionp((void *) userDatavp, (void *) _keyVal, (void *) _gotKeyVal)) { \
		continue;						\
	      }								\
	    } else {							\
	      if (_keyVal != _gotKeyVal) {				\
		continue;						\
	      }								\
	    }								\
									\
	    if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && ((void *) _gotKeyVal != NULL) && (hashName->keyFreeFunctionp != NULL)) { \
	      hashName->keyFreeFunctionp((void *) userDatavp, (void *) _gotKeyVal); \
	    }								\
	    if ((GENERICSTACKITEMTYPE(_subValStackp, _i) == GENERICSTACKITEMTYPE_PTR)) { \
	      GENERICSTACKITEMTYPE2TYPE_PTR _gotValVal = GENERICSTACK_GET_PTR(_subValStackp, _i); \
	      if ((_gotValVal != NULL) && (hashName->valFreeFunctionp != NULL)) { \
		hashName->valFreeFunctionp((void *) userDatavp, _gotValVal); \
	      }								\
	    }								\
		    							\
	    _GENERICHASH_SET(hashName, userDatavp, keyType, _keyVal, valType, _valVal, _subKeyStackp, _subValStackp, _i); \
	    break;							\
	  }								\
	  if (_i >= _subStackused) {					\
	    _GENERICHASH_PUSH(hashName, userDatavp, keyType, _keyVal, valType, _valVal, _subKeyStackp, _subValStackp); \
	  }								\
	}								\
      }									\
    }									\
  } while (0)

/* ====================================================================== */
/* Find and eventually remove in the hash                                 */
/* ====================================================================== */
#define _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, remove) do { \
									\
    if (hashName->keyIndFunctionp == NULL) {				\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      size_t _subStackIndex = hashName->keyIndFunctionp((void *) userDatavp, GENERICSTACKITEMTYPE_##keyType, (void **) &keyVal); \
      _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, _subStackIndex, remove); \
    }									\
  } while (0)

#define _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, _subStackIndex, remove) do { \
									\
    if (_subStackIndex == (size_t) -1) {				\
      errno = EINVAL;							\
      hashName->error = 1;						\
    } else {								\
      findResult = 0;                                                   \
									\
      if ((_subStackIndex < GENERICSTACK_USED(hashName->keyStackp))	\
	  &&								\
	  (GENERICSTACKITEMTYPE(hashName->keyStackp, _subStackIndex) == GENERICSTACKITEMTYPE_PTR)) { \
	genericStack_t *_subKeyStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->keyStackp, _subStackIndex); \
	genericStack_t *_subValStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->valStackp, _subStackIndex); \
									\
	if (GENERICSTACK_ERROR(hashName->keyStackp) || GENERICSTACK_ERROR(hashName->valStackp)) { \
	  hashName->error = 1;						\
	} else {							\
	  size_t _subStackused = GENERICSTACK_USED(_subKeyStackp);	\
	  size_t _i;							\
									\
	  for (_i = 0; _i < _subStackused; _i++) {			\
            GENERICSTACKITEMTYPE2TYPE_##keyType _gotKeyVal;		\
									\
            if ((GENERICSTACKITEMTYPE(_subKeyStackp, _i) != GENERICSTACKITEMTYPE_##keyType)) { \
	      continue;							\
	    }								\
									\
	    _gotKeyVal = GENERICSTACK_GET_##keyType(_subKeyStackp, _i); \
            if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && (hashName->keyCmpFunctionp != NULL)) { \
              if (! hashName->keyCmpFunctionp((void *) userDatavp, (void *) keyVal, (void *) _gotKeyVal)) { \
		continue;						\
	      }								\
	    } else {							\
	      if (keyVal != _gotKeyVal) {				\
		continue;						\
	      }								\
	    }								\
            findResult = 1;                                             \
                                                                        \
            if (valValp != NULL) {                                      \
              *valValp = GENERICSTACK_GET_##valType(_subValStackp, _i); \
            }                                                           \
            if (remove) {                                               \
              if ((GENERICSTACKITEMTYPE_##keyType == GENERICSTACKITEMTYPE_PTR) && ((void *) _gotKeyVal != NULL) && (hashName->keyFreeFunctionp != NULL)) { \
                hashName->keyFreeFunctionp((void *) userDatavp, (void *) _gotKeyVal); \
              }								\
              GENERICSTACK_SET_NA(_subKeyStackp, _i);			\
              GENERICSTACK_SWITCH(_subKeyStackp, _i, -1);		\
              GENERICSTACK_POP_NA(_subKeyStackp);			\
									\
              if (valValp == NULL) {                                    \
                if ((GENERICSTACKITEMTYPE(_subValStackp, _i) == GENERICSTACKITEMTYPE_PTR) && (hashName->valFreeFunctionp == NULL)) { \
                  GENERICSTACKITEMTYPE2TYPE_PTR _valVal = GENERICSTACK_GET_PTR(_subValStackp, _i); \
                  if (_valVal != NULL) {                                \
                    hashName->valFreeFunctionp((void *) userDatavp, _valVal); \
                  }                                                     \
                }                                                       \
              }                                                         \
              GENERICSTACK_SET_NA(_subValStackp, _i);			\
              GENERICSTACK_SWITCH(_subValStackp, _i, -1);		\
              GENERICSTACK_POP_NA(_subValStackp);			\
            }                                                           \
            break;							\
          }								\
        }								\
      }									\
    }									\
} while (0)

#define GENERICHASH_FIND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult)   _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, 0)
#define GENERICHASH_REMOVE(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult) _GENERICHASH_FIND_REMOVE(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, 1)

#define GENERICHASH_FIND_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, index) _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, index, 0)
#define GENERICHASH_REMOVE_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, index) _GENERICHASH_FIND_REMOVE_BY_IND(hashName, userDatavp, keyType, keyVal, valType, valValp, findResult, index, 1)

/* ====================================================================== */
/* Memory release                                                         */
/* ====================================================================== */
#define GENERICHASH_FREE(hashName, userDatavp) do {                     \
    if (hashName != NULL) {						\
      size_t _usedl  = GENERICSTACK_USED(hashName->keyStackp);		\
      size_t _i;							\
      for (_i = 0; _i < _usedl; _i++) {					\
	if (GENERICSTACKITEMTYPE(hashName->keyStackp, _i) == GENERICSTACKITEMTYPE_PTR) { \
          genericStack_t *_subKeyStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->keyStackp, _i); \
          genericStack_t *_subValStackp = (genericStack_t *) GENERICSTACK_GET_PTR(hashName->valStackp, _i); \
                                                                        \
          if (! GENERICSTACK_ERROR(hashName->keyStackp) && ! GENERICSTACK_ERROR(hashName->valStackp)) { \
            size_t _subStackused = GENERICSTACK_USED(_subKeyStackp);	\
            size_t _j;							\
                                                                        \
            for (_j = 0; _j < _subStackused; _j++) {			\
              if ((GENERICSTACKITEMTYPE(_subKeyStackp, _j) == GENERICSTACKITEMTYPE_PTR) && (hashName->keyFreeFunctionp != NULL)) { \
                GENERICSTACKITEMTYPE2TYPE_PTR _keyValp = GENERICSTACK_GET_PTR(_subKeyStackp, _j); \
                if (! GENERICSTACK_ERROR(_subKeyStackp) && (_keyValp != NULL)) { \
                  hashName->keyFreeFunctionp(userDatavp, _keyValp);     \
                }                                                       \
              }                                                         \
              if ((GENERICSTACKITEMTYPE(_subValStackp, _j) == GENERICSTACKITEMTYPE_PTR) && (hashName->valFreeFunctionp != NULL)) { \
                GENERICSTACKITEMTYPE2TYPE_PTR _valValp = GENERICSTACK_GET_PTR(_subValStackp, _j); \
                if (! GENERICSTACK_ERROR(_subValStackp) && (_valValp != NULL)) { \
                  hashName->valFreeFunctionp(userDatavp, _valValp);     \
                }                                                       \
              }                                                         \
            }                                                           \
          }                                                             \
          GENERICSTACK_FREE(_subKeyStackp);                             \
          GENERICSTACK_FREE(_subValStackp);                             \
        }                                                               \
      }                                                                 \
      GENERICSTACK_FREE(hashName->keyStackp);				\
      GENERICSTACK_FREE(hashName->valStackp);				\
      free(hashName);							\
      hashName = NULL;							\
    }									\
  } while (0)

#endif /* GENERICHASH_H */
