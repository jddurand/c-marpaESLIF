#include "marpaESLIF/internal/lua.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif

/* Revisit lua context that refers to values inside the lua interpreter */
#undef MARPAESLIFLUA_CONTEXT
#define MARPAESLIFLUA_CONTEXT MARPAESLIF_EMBEDDED_CONTEXT_LUA

/* Revisit some lua macros */
#undef marpaESLIFLua_luaL_error
#define marpaESLIFLua_luaL_error(L, string) luaunpanicL_error(NULL, L, "%s", string)
#undef marpaESLIFLua_luaL_errorf
#define marpaESLIFLua_luaL_errorf(L, formatstring, ...) luaunpanicL_error(NULL, L, formatstring, __VA_ARGS__)
#undef marpaESLIFLua_luaL_newlib
#define marpaESLIFLua_luaL_newlib(L, l) (! luaunpanicL_newlib(L, l))
#include "../src/bindings/lua/src/marpaESLIFLua.c"

#undef  FILENAMES
#define FILENAMES "lua.c" /* For logging */


static int   _marpaESLIF_lua_writeri(marpaESLIF_t *marpaESLIFp, char **luaprecompiledpp, size_t *luaprecompiledlp, const void* p, size_t sz);
static int   _marpaESLIFGrammar_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud);
static int   _marpaESLIFRecognizer_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud);
static int   _marpaESLIFValue_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud);

static lua_State *_marpaESLIFValue_lua_newp(marpaESLIFValue_t *marpaESLIFValuep);
static lua_State *_marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static short _marpaESLIFValue_lua_function_loadb(marpaESLIFValue_t *marpaESLIFValuep);
static short _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static short _marpaESLIFRecognizer_lua_function_precompileb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *luabytep, size_t luabytel, short stripb, int popi);

#define MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, f) do {          \
    const char *errorstring;                                            \
                                                                        \
    if ((L == NULL) || luaunpanic_tostring(&errorstring, L, -1)) {      \
      MARPAESLIF_ERROR(marpaESLIFp, "Lua failure");                     \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERRORF(marpaESLIFp, "%s failure", #f);               \
      } else {								\
        MARPAESLIF_ERRORF(marpaESLIFp, "%s failure: %s", #f, errorstring); \
      }									\
    }                                                                   \
  } while (0)

#define MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L) do {             \
    const char *errorstring;                                            \
                                                                        \
    if ((L == NULL) || luaunpanic_tostring(&errorstring, L, -1)) {      \
      MARPAESLIF_ERROR(marpaESLIFp, "Lua failure");                     \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERROR(marpaESLIFp, "Unknown lua failure");           \
      } else {								\
        MARPAESLIF_ERROR(marpaESLIFp, errorstring);                     \
      }									\
    }                                                                   \
  } while (0)

#define LUAL_CHECKVERSION(marpaESLIFp, L) do {                          \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_checkversion(L))) {             \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, luaL_checkversion); \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_OPENLIBS(marpaESLIFp, L) do {                             \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_openlibs(L))) {                \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, luaL_openlibs);   \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_DUMP(marpaESLIFp, L, writer, data, strip) do {              \
    int _rci = -1;                                                      \
    if (MARPAESLIF_UNLIKELY(luaunpanic_dump(&_rci, L, writer, data, strip) || _rci)) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_dump);         \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_GETFIELD(rcp, marpaESLIFp, L, idx, k) do {                  \
    if (MARPAESLIF_UNLIKELY(luaunpanic_getfield(rcp, L, idx, k))) {     \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_getfield);     \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_REMOVE(marpaESLIFp, L, idx) do {                            \
    if (MARPAESLIF_UNLIKELY(luaunpanic_remove(L, idx))) {               \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_remove);       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_GETGLOBAL(rcp, marpaESLIFp, L, name) do {                   \
    if (MARPAESLIF_UNLIKELY(luaunpanic_getglobal(rcp, L, name))) {      \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_getglobal);    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETGLOBAL(marpaESLIFp, L, name) do {                        \
    if (MARPAESLIF_UNLIKELY(luaunpanic_setglobal(L, name))) {           \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_setglobal);    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_LOADBUFFER(marpaESLIFp, L, s, sz, n) do {                  \
    int _rci = -1;                                                      \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_loadbuffer(&_rci, L, s, sz, n) || _rci)) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, luaL_loadbuffer);  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_LOADSTRING(marpaESLIFp, L, s) do {                         \
    int _rci = -1;                                                      \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_loadstring(&_rci, L, s) || _rci)) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, luaL_loadstring);  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_CHECKSTACK(marpaESLIFp, L, extra, msg) do {                \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_checkstack(L, extra, msg))) {   \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, luaL_checkstack);  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_GETTOP(marpaESLIFp, L, rcip) do {                           \
    if (MARPAESLIF_UNLIKELY(luaunpanic_gettop(rcip, L))) {              \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_gettop);       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETTOP(marpaESLIFp, L, idx) do {                            \
    if (MARPAESLIF_UNLIKELY(luaunpanic_settop(L, idx))) {               \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_settop);       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_INSERT(marpaESLIFp, L, idx) do {                            \
    if (MARPAESLIF_UNLIKELY(luaunpanic_insert(L, idx))) {               \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_insert);       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_TOUSERDATA(marpaESLIFp, L, rcpp, idx) do {                  \
    if (MARPAESLIF_UNLIKELY(luaunpanic_touserdata((void **) rcpp, L, idx))) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_touserdata);   \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_REQUIREF(marpaESLIFp, L, modname, openf, glb) do {         \
    if (MARPAESLIF_UNLIKELY(luaunpanicL_requiref(L, modname, openf, glb))) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lual_requiref);    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_POP(marpaESLIFp, L, n) do {                                 \
    if (MARPAESLIF_UNLIKELY(luaunpanic_pop(L, n))) {                    \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_pop);          \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PCALL(marpaESLIFp, L, n, r, f) do {                         \
    int _rci;                                                           \
    if (MARPAESLIF_UNLIKELY(luaunpanic_pcall(&_rci, L, n, r, f) || _rci)) { \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_pcall);        \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHSTRING(sp, marpaESLIFp, L, s) do {                      \
    if (MARPAESLIF_UNLIKELY(luaunpanic_pushstring(sp, L, s))) {         \
      MARPAESLIFLUA_LOG_ERROR_STRING(marpaESLIFp, L, lua_pushstring);   \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

/*****************************************************************************/
static lua_State *_marpaESLIFValue_lua_newp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  lua_State    *L;

  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFValuep->marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  /* Get a shallow copy */
  L = marpaESLIFValuep->L = marpaESLIFValuep->marpaESLIFRecognizerp->L;

  /* Inject current valuator */
  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_marpaESLIFValue_newFromUnmanagedi(L, marpaESLIFValuep))) goto err; /* stack: marpaESLIFValue */
  LUA_SETGLOBAL(marpaESLIFp, L, "marpaESLIFValue");                                                          /* stack: */

  goto done;

 err:
  _marpaESLIFValue_lua_freev(marpaESLIFValuep);
  L = NULL;

 done:
  return L;
}

/*****************************************************************************/
static lua_State *_marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t    *marpaESLIFGrammarp;
  marpaESLIF_t           *marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerTopp;
  lua_State              *L = NULL;

  /* Lua state is owned by the top-level recognizer */
  marpaESLIFRecognizerTopp = marpaESLIFRecognizerp->marpaESLIFRecognizerTopp;

  if (marpaESLIFRecognizerTopp->L != NULL) {
    /* lua_State already created: get a shallow copy */
    L = marpaESLIFRecognizerp->L = marpaESLIFRecognizerTopp->L;
    goto inject_current_recognizer;
  }

  marpaESLIFGrammarp = marpaESLIFRecognizerTopp->marpaESLIFGrammarp;
  marpaESLIFp        = marpaESLIFRecognizerTopp->marpaESLIFp;

  /* Create a lua "thread" */
  if (MARPAESLIF_UNLIKELY(luaunpanic_newthread(&L, marpaESLIFGrammarp->L))) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerTopp->marpaESLIFp, "luaunpanic_newthread failure");
    errno = ENOSYS;
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerTopp->marpaESLIFp, "luaunpanic_newthread success but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  /* Top level recognizer owns lua state, we do not */
  marpaESLIFRecognizerTopp->L = L;
  marpaESLIFRecognizerp->L    = L;

  /* We store the thread in a global that is in the lua execution stack of the parent thread */
  sprintf(marpaESLIFRecognizerTopp->Lids, "marpaESLIFRecognizerLuathread%p", marpaESLIFRecognizerTopp);
  LUA_SETGLOBAL(marpaESLIFp, marpaESLIFGrammarp->L, marpaESLIFRecognizerTopp->Lids);

  /* We are embedded: instantiate the marpaESLIFContextStack object */
  LUA_GETGLOBAL(NULL, marpaESLIFp, L, "marpaESLIFContextStack");                      /* stack: ..., marpaESLIFContextStack */
  LUAL_CHECKSTACK(marpaESLIFp, L, 1, 0);                                              /* stack: ..., marpaESLIFContextStack */
  LUA_GETFIELD(NULL, marpaESLIFp, L, -1, "new");                                      /* stack: ..., marpaESLIFContextStack, marpaESLIFContextStack.new() */
  LUA_REMOVE(marpaESLIFp, L, -2);                                                     /* stack: ..., marpaESLIFContextStack.new() */
  LUA_PCALL(marpaESLIFp, L, 0, 1, 0);                                                 /* stack: ..., marpaESLIFContextStackp = marpaESLIFContextStack.new() */
  LUA_SETGLOBAL(marpaESLIFp, L, "marpaESLIFContextStackp");                           /* stack: ...  */


 inject_current_recognizer:
  /* No needed to reinject the same marpaESLIFRecognizerp twice */
  if (marpaESLIFRecognizerp->marpaESLIFRecognizerLastInjectedp != marpaESLIFRecognizerp) {
    /* Inject current recognizer */
    if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_marpaESLIFRecognizer_newFromUnmanagedi(L, marpaESLIFRecognizerp))) goto err; /* stack: marpaESLIFRecognizer */
    marpaESLIFRecognizerp->marpaESLIFRecognizerLastInjectedp = marpaESLIFRecognizerp;
    LUA_SETGLOBAL(marpaESLIFp, L, "marpaESLIFRecognizer");                                                               /* stack: */
  }

  goto done;

 err:
  L = NULL;

 done:
  return L;
}

/*****************************************************************************/
static void _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  /* It is virtual: L is always owned by the top-level recognizer */
  marpaESLIFValuep->L = NULL;
}

/*****************************************************************************/
static void _marpaESLIFRecognizer_lua_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  lua_State *L = marpaESLIFRecognizerp->L;

  if (L != NULL) {
    /* It is owned by the top-level recognizer */
    if (marpaESLIFRecognizerp == marpaESLIFRecognizerp->marpaESLIFRecognizerTopp) {
      /* The thread is in the parent's global marpaESLIFRecognizerp->marpaESLIFRecognizerTopp->Lids - we push nil and overwrite this global - garbage collection will take over */
      if (marpaESLIFLua_lua_pushnil(marpaESLIFRecognizerp->marpaESLIFGrammarp->L)) {
        LUA_SETGLOBAL(marpaESLIFRecognizerp->marpaESLIFp, marpaESLIFRecognizerp->marpaESLIFGrammarp->L, marpaESLIFRecognizerp->marpaESLIFRecognizerTopp->Lids);
      }
    }
    marpaESLIFRecognizerp->L = NULL;
  }
 err:
  return;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char             *funcs                 = "_marpaESLIFValue_lua_actionb";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                            topi                  = -1;
  marpaESLIFValueRuleCallback_t  ruleCallbackp;
  lua_State                     *L;
  short                          rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  LUA_GETTOP(marpaESLIFp, L, &topi);

  ruleCallbackp = marpaESLIFLua_valueRuleActionResolver(userDatavp, marpaESLIFValuep, marpaESLIFValuep->actions);
  if (MARPAESLIF_UNLIKELY(ruleCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "Lua bindings returned no rule callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! ruleCallbackp(userDatavp, marpaESLIFValuep, arg0i, argni, resulti, nullableb))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti)
/*****************************************************************************/
{
  static const char               *funcs       = "_marpaESLIFValue_lua_symbolb";
  marpaESLIF_t                    *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  int                              topi        = -1;
  marpaESLIFValueSymbolCallback_t  symbolCallbackp;
  lua_State                        *L;
  short                            rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  symbolCallbackp = marpaESLIFLua_valueSymbolActionResolver(userDatavp, marpaESLIFValuep, marpaESLIFValuep->actions);
  if (MARPAESLIF_UNLIKELY(symbolCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "Lua bindings returned no symbol callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! symbolCallbackp(userDatavp, marpaESLIFValuep, marpaESLIFValueResultp, resulti))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_ifactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char                *funcs       = "_marpaESLIFRecognizer_lua_ifactionb";
  marpaESLIF_t                     *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                               topi        = -1;
  marpaESLIFRecognizerIfCallback_t  ifCallbackp;
  lua_State                        *L;
  short                             rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  ifCallbackp = marpaESLIFLua_recognizerIfActionResolver(userDatavp, marpaESLIFRecognizerp, marpaESLIFRecognizerp->actions);
  if (MARPAESLIF_UNLIKELY(ifCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "Lua bindings returned no if-action callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! ifCallbackp(userDatavp, marpaESLIFRecognizerp, marpaESLIFValueResultp, marpaESLIFValueResultBoolp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_regexactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char                   *funcs       = "_marpaESLIFRecognizer_lua_regexactionb";
  marpaESLIF_t                        *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                                  topi        = -1;
  marpaESLIFRecognizerRegexCallback_t  regexCallbackp;
  lua_State                           *L;
  short                                rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  regexCallbackp = marpaESLIFLua_recognizerRegexActionResolver(userDatavp, marpaESLIFRecognizerp, marpaESLIFRecognizerp->actions);
  if (MARPAESLIF_UNLIKELY(regexCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "Lua bindings returned no regex-action callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! regexCallbackp(userDatavp, marpaESLIFRecognizerp, marpaESLIFCalloutBlockp, marpaESLIFValueResultOutp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_generatoractionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char                       *funcs       = "_marpaESLIFRecognizer_lua_generatoractionb";
  marpaESLIF_t                            *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                                      topi        = -1;
  marpaESLIFRecognizerGeneratorCallback_t  generatorCallbackp;
  lua_State                               *L;
  short                                    rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  generatorCallbackp = marpaESLIFLua_recognizerGeneratorActionResolver(userDatavp, marpaESLIFRecognizerp, marpaESLIFRecognizerp->actions);
  if (MARPAESLIF_UNLIKELY(generatorCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "Lua bindings returned no symbol-generator callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! generatorCallbackp(userDatavp, marpaESLIFRecognizerp, contextp, marpaESLIFValueResultOutp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L;
  short      rcb;

  /* Create Lua state */
  if (MARPAESLIF_UNLIKELY(luaunpanicL_newstate(&L))) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_newstate failure");
    errno = ENOSYS;
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_newstate success but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  marpaESLIFp->L = L;

  /* Open all available libraries */
  LUAL_OPENLIBS(marpaESLIFp, L);

  /* Check Lua version */
  LUAL_CHECKVERSION(marpaESLIFp, L);

  /* Load the marpaESLIFLua library built-in */
  LUAL_REQUIREF(marpaESLIFp, L, "marpaESLIFLua", marpaESLIFLua_installi, 1);                                        /* stack: marpaESLIFLua */
  LUA_POP(marpaESLIFp, L, 1);                                                                                       /* stack: */

  /* Inject current eslif */
  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_marpaESLIF_newFromUnmanagedi(marpaESLIFp->L, marpaESLIFp))) goto err;     /* stack: marpaESLIF */
  LUA_SETGLOBAL(marpaESLIFp, L, "marpaESLIF");                                                                      /* stack: */

  goto done;

 err:
  L = NULL;

 done:
  return L;
}

/*****************************************************************************/
static void _marpaESLIF_lua_freev(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  if (marpaESLIFp->L != NULL) {
    if (luaunpanic_close(marpaESLIFp->L)) {
      MARPAESLIF_ERROR(marpaESLIFp, "luaunpanic_close failure");
    }
    marpaESLIFp->L = NULL;
  }
}

/*****************************************************************************/
static lua_State *_marpaESLIFGrammar_lua_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  lua_State    *L;

  /* Create Lua thread */
  if (MARPAESLIF_UNLIKELY(luaunpanic_newthread(&L, marpaESLIFGrammarp->marpaESLIFp->L))) {
    MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanic_newthread failure");
    errno = ENOSYS;
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanic_newthread but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  marpaESLIFGrammarp->L = L;

  /* We store the thread in a global that is in the lua execution stack of the parent thread */
  sprintf(marpaESLIFGrammarp->Lids, "marpaESLIFGrammarLuathread%p", marpaESLIFGrammarp);
  LUA_SETGLOBAL(marpaESLIFp, marpaESLIFGrammarp->marpaESLIFp->L, marpaESLIFGrammarp->Lids);

  /* Inject current grammar */
  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_marpaESLIFGrammar_newFromUnmanagedi(marpaESLIFGrammarp->L, marpaESLIFGrammarp))) goto err;
  LUA_SETGLOBAL(marpaESLIFp, L, "marpaESLIFGrammar");                                                                           /* stack: marpaESLIFLua */

  if ((marpaESLIFGrammarp->luabytep != NULL) && (marpaESLIFGrammarp->luabytel > 0)) {
    /* Compiles lua script present in the grammar */
    LUAL_LOADBUFFER(marpaESLIFp, L, marpaESLIFGrammarp->luabytep, marpaESLIFGrammarp->luabytel, "=<luascript/>");
    /* Result is a "function" at the top of the stack */
    LUA_PCALL(marpaESLIFp, L, 0, LUA_MULTRET, 0);
    /* Clear the stack */
    LUA_SETTOP(marpaESLIFp, L, 0);
  }

  goto done;

 err:
  L = NULL;

 done:
  return L;
}

/*****************************************************************************/
static void _marpaESLIFGrammar_lua_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp->L != NULL) {
    /* The thread is in the parent's global marpaESLIFGrammarp->Lids - we push nil and overwrite this global - garbage collection will take over */
    if (marpaESLIFLua_lua_pushnil(marpaESLIFGrammarp->marpaESLIFp->L)) {
      LUA_SETGLOBAL(marpaESLIFGrammarp->marpaESLIFp, marpaESLIFGrammarp->marpaESLIFp->L, marpaESLIFGrammarp->Lids);
    }
  }
 err:
  return;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_precompileb(marpaESLIFValue_t *marpaESLIFValuep, char *luabytep, size_t luabytel, short stripb, int popi)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  lua_State    *L;
  short         rcb;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  /* Compiles lua script */
  LUAL_LOADBUFFER(marpaESLIFp, L, luabytep, luabytel, "=<luafunction/>");

  /* Result is a "function" at the top of the stack - we now have to dump it so that lua knows about it  */
  LUA_DUMP(marpaESLIFp, L, _marpaESLIFValue_lua_writeri, marpaESLIFValuep, stripb);

  if (popi > 0) {
    LUA_POP(marpaESLIFp, L, popi);
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static int _marpaESLIF_lua_writeri(marpaESLIF_t *marpaESLIFp, char **luaprecompiledpp, size_t *luaprecompiledlp, const void* p, size_t sz)
/*****************************************************************************/
{
  char *q;
  int   rci;

  if (sz > 0) {
    if (*luaprecompiledpp == NULL) {
      *luaprecompiledpp = (char *) malloc(sz);
      if (MARPAESLIF_UNLIKELY(*luaprecompiledpp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      q = *luaprecompiledpp;
      *luaprecompiledlp = sz;
    } else {
      q = (char *) realloc(*luaprecompiledpp, *luaprecompiledlp + sz);
      if (MARPAESLIF_UNLIKELY(q == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      *luaprecompiledpp = q;
      q += *luaprecompiledlp;
      *luaprecompiledlp += sz;
    }

    memcpy(q, p, sz);
  }

  rci = 0;
  goto end;
  
 err:
  rci = 1;
  
 end:
  return rci;
}

/*****************************************************************************/
static int _marpaESLIFGrammar_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) ud;

  return _marpaESLIF_lua_writeri(marpaESLIFGrammarp->marpaESLIFp, &(marpaESLIFGrammarp->luaprecompiledp), &(marpaESLIFGrammarp->luaprecompiledl), p, sz);
}

/****************************************************************************/
static int _marpaESLIFValue_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud)
/****************************************************************************/
{
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) ud;

  return _marpaESLIF_lua_writeri(marpaESLIFValuep->marpaESLIFp, &(marpaESLIFValuep->luaprecompiledp), &(marpaESLIFValuep->luaprecompiledl), p, sz);
}

/****************************************************************************/
static int _marpaESLIFRecognizer_lua_writeri(lua_State *L, const void* p, size_t sz, void* ud)
/****************************************************************************/
{
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) ud;

  return _marpaESLIF_lua_writeri(marpaESLIFRecognizerp->marpaESLIFp, &(marpaESLIFRecognizerp->luaprecompiledp), &(marpaESLIFRecognizerp->luaprecompiledl), p, sz);
}

/****************************************************************************/
/* When MARPAESLIFLUA_EMBEDDED the file that includes this source must      */
/* provide the following implementations.                                   */
/****************************************************************************/

/****************************************************************************/
static short marpaESLIFLua_lua_pushinteger(lua_State *L, lua_Integer n)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushinteger(L, n));
}

/****************************************************************************/
static short marpaESLIFLua_lua_setglobal(lua_State *L, const char *name)
/****************************************************************************/
{
  return ! luaunpanic_setglobal(L, name);
}

/****************************************************************************/
static short marpaESLIFLua_lua_getglobal(int *luaip, lua_State *L, const char *name)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_getglobal(luaip, L, name));
}

/****************************************************************************/
static short marpaESLIFLua_lua_type(int *luaip, lua_State *L, int index)
/****************************************************************************/
{
  return ! luaunpanic_type(luaip, L, index);
}

/****************************************************************************/
static short marpaESLIFLua_lua_pop(lua_State *L, int n)
/****************************************************************************/
{
  return ! luaunpanic_pop(L, n);
}

/****************************************************************************/
static short marpaESLIFLua_lua_newtable(lua_State *L)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_newtable(L));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushcfunction(lua_State *L, lua_CFunction f)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushcfunction(L, f));
}

/****************************************************************************/
static short marpaESLIFLua_lua_setfield(lua_State *L, int index, const char *k)
/****************************************************************************/
{
  return ! luaunpanic_setfield(L, index, k);
}

/****************************************************************************/
static short marpaESLIFLua_lua_setmetatable(lua_State *L, int index)
/****************************************************************************/
{
  return ! luaunpanic_setmetatable(NULL, L, index);
}

/****************************************************************************/
static short marpaESLIFLua_lua_insert(lua_State *L, int index)
/****************************************************************************/
{
  return ((index <= 0) || marpaESLIFLua_luaL_checkstack(L, index, "Cannot grow stack")) && (! luaunpanic_insert(L, index));
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawgeti(int *luaip, lua_State *L, int index, lua_Integer n)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_rawgeti(luaip, L, index, n));
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawget(int *luaip, lua_State *L, int index)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_rawget(luaip, L, index));
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawgetp(int *luaip, lua_State *L, int index, const void *p)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_rawgetp(luaip, L, index, p));
}

/****************************************************************************/
static short marpaESLIFLua_lua_remove(lua_State *L, int index)
/****************************************************************************/
{
  return ! luaunpanic_remove(L, index);
}

/****************************************************************************/
static short marpaESLIFLua_lua_createtable(lua_State *L, int narr, int nrec)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_createtable(L, narr, nrec));
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawseti(lua_State *L, int index, lua_Integer i)
/****************************************************************************/
{
  return ! luaunpanic_rawseti(L, index, i);
}

/****************************************************************************/
static short marpaESLIFLua_lua_seti(lua_State *L, int index, lua_Integer i)
/****************************************************************************/
{
  return ! luaunpanic_seti(L, index, i);
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushstring(const char **luasp, lua_State *L, const char *s)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushstring(luasp, L, s));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushlstring(const char **luasp, lua_State *L, const char *s, size_t len)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushlstring(luasp, L, s, len));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushnil(lua_State *L)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushnil(L));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checkstack(lua_State *L, int extra, const char *msg)
/****************************************************************************/
{
  return ! luaunpanicL_checkstack(L, extra, msg);
}

/****************************************************************************/
static short marpaESLIFLua_lua_getfield(int *luaip, lua_State *L, int index, const char *k)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_getfield(luaip, L, index, k));
}

/****************************************************************************/
static short marpaESLIFLua_lua_call(lua_State *L, int nargs, int nresults)
/****************************************************************************/
{
  return ! luaunpanic_call(L, nargs, nresults);
}

/****************************************************************************/
static short marpaESLIFLua_lua_settop(lua_State *L, int index)
/****************************************************************************/
{
  return (index <= 0 || marpaESLIFLua_luaL_checkstack(L, index, "Cannot grow stack")) && (! luaunpanic_settop(L, index));
}

/****************************************************************************/
static short marpaESLIFLua_lua_copy(lua_State *L, int fromidx, int toidx)
/****************************************************************************/
{
  return (toidx <= 0 || marpaESLIFLua_luaL_checkstack(L, toidx, "Cannot grow stack")) && (! luaunpanic_copy(L, fromidx, toidx));
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawsetp(lua_State *L, int index, const void *p)
/****************************************************************************/
{
  return ! luaunpanic_rawsetp(L, index, p);
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawset(lua_State *L, int index)
/****************************************************************************/
{
  return ! luaunpanic_rawset(L, index);
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushboolean(lua_State *L, int b)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushboolean(L, b));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushnumber(lua_State *L, lua_Number n)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushnumber(L, n));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushlightuserdata(lua_State *L, void *p)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushlightuserdata(L, p));
}

/****************************************************************************/
static short marpaESLIFLua_lua_newuserdata(void **rcpp, lua_State *L, size_t sz)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_newuserdata(rcpp, L, sz));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushvalue(lua_State *L, int index)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushvalue(L, index));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_ref(int *rcip, lua_State *L, int t)
/****************************************************************************/
{
  return ! luaunpanicL_ref(rcip, L, t);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_unref(lua_State *L, int t, int ref)
/****************************************************************************/
{
  return ! luaunpanicL_unref(L, t, ref);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanicL_requiref(L, modname, openf, glb));
}

/****************************************************************************/
static short marpaESLIFLua_lua_touserdata(void **rcpp, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_touserdata(rcpp, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tointeger(lua_Integer *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_tointeger(rcip, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tointegerx(lua_Integer *rcip, lua_State *L, int idx, int *isnum)
/****************************************************************************/
{
  return ! luaunpanic_tointegerx(rcip, L, idx, isnum);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tonumber(lua_Number *rcdp, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_tonumber(rcdp, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tonumberx(lua_Number *rcdp, lua_State *L, int idx, int *isnum)
/****************************************************************************/
{
  return ! luaunpanic_tonumberx(rcdp, L, idx, isnum);
}

/****************************************************************************/
static short marpaESLIFLua_lua_toboolean(int *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_toboolean(rcip, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_tolstring(const char **rcp, lua_State *L, int idx, size_t *len)
/****************************************************************************/
{
  return ! luaunpanicL_tolstring(rcp, L, idx, len);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tolstring(const char **rcpp, lua_State *L, int idx, size_t *len)
/****************************************************************************/
{
  return ! luaunpanic_tolstring(rcpp, L, idx, len);
}

/****************************************************************************/
static short marpaESLIFLua_lua_tostring(const char **rcpp, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_tostring(rcpp, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_compare(int *rcip, lua_State *L, int idx1, int idx2, int op)
/****************************************************************************/
{
  return ! luaunpanic_compare(rcip, L, idx1, idx2, op);
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawequal(int *rcip, lua_State *L, int idx1, int idx2)
/****************************************************************************/
{
  return ! luaunpanic_rawequal(rcip, L, idx1, idx2);
}

/****************************************************************************/
static short marpaESLIFLua_lua_isnil(int *rcip, lua_State *L, int n)
/****************************************************************************/
{
  return ! luaunpanic_isnil(rcip, L, n);
}

/****************************************************************************/
static short marpaESLIFLua_lua_gettop(int *rcip, lua_State *L)
/****************************************************************************/
{
  return ! luaunpanic_gettop(rcip, L);
}

/****************************************************************************/
static short marpaESLIFLua_lua_absindex(int *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_absindex(rcip, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_next(int *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 2, "Cannot grow stack by 2") && (! luaunpanic_next(rcip, L, idx));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checklstring(const char **rcp, lua_State *L, int arg, size_t *l)
/****************************************************************************/
{
  return ! luaunpanicL_checklstring(rcp, L, arg, l);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checkstring(const char **rcp, lua_State *L, int arg)
/****************************************************************************/
{
  return ! luaunpanicL_checkstring(rcp, L, arg);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checkinteger(lua_Integer *rcp, lua_State *L, int arg)
/****************************************************************************/
{
  return ! luaunpanicL_checkinteger(rcp, L, arg);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_optinteger(lua_Integer *rcp, lua_State *L, int arg, lua_Integer def)
/****************************************************************************/
{
  return ! luaunpanicL_optinteger(rcp, L, arg, def);
}

/****************************************************************************/
static short marpaESLIFLua_lua_getmetatable(int *rcip, lua_State *L, int index)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_getmetatable(rcip, L, index));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_callmeta(int *rcip, lua_State *L, int obj, const char *e)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanicL_callmeta(rcip, L, obj, e));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_getmetafield(int *rcip, lua_State *L, int obj, const char *e)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanicL_getmetafield(rcip, L, obj, e));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checktype(lua_State *L, int arg, int t)
/****************************************************************************/
{
  return ! luaunpanicL_checktype(L, arg, t);
}

/****************************************************************************/
static short marpaESLIFLua_lua_topointer(const void **rcpp, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_topointer(rcpp, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_rawlen(size_t *rcp, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_rawlen(rcp, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_dostring(int *rcip, lua_State *L, const char *fn)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanicL_dostring(rcip, L, fn));
}

/****************************************************************************/
static short marpaESLIFLua_luaL_loadstring(int *rcip, lua_State *L, const char *fn)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanicL_loadstring(rcip, L, fn));
}

/****************************************************************************/
static short marpaESLIFLua_lua_pushglobaltable(lua_State *L)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_pushglobaltable(L));
}

/****************************************************************************/
static short marpaESLIFLua_lua_settable(lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_settable(L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_lua_gettable(int *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return marpaESLIFLua_luaL_checkstack(L, 1, "Cannot grow stack by 1") && (! luaunpanic_gettable(rcip, L, idx));
}

/****************************************************************************/
static short marpaESLIFLua_lua_isinteger(int *rcip, lua_State *L, int idx)
/****************************************************************************/
{
  return ! luaunpanic_isinteger(rcip, L, idx);
}

/****************************************************************************/
static short marpaESLIFLua_luaL_checkudata(void **rcpp, lua_State *L, int ud, const char *tname)
/****************************************************************************/
{
  return ! luaunpanicL_checkudata(rcpp, L, ud, tname);
}

/****************************************************************************/
static short _marpaESLIFValue_lua_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp)
/****************************************************************************/
{
  static const char                *funcs                 = "_marpaESLIFValue_lua_representationb";
  /* Internal function: we force userDatavp to be marpaESLIFValuep */
  marpaESLIFValue_t                *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                     *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t           *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFLuaValueContext_t      *marpaESLIFLuaValueContextp;
  int                               typei;
  lua_State                         *L;
  short                             rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  /* Remember that we pushed the "marpaESLIFValue" global ? */
  LUA_GETGLOBAL(&typei, marpaESLIFp, L, "marpaESLIFValue");                    /* stack: ..., marpaESLIFValueTable */
  if (MARPAESLIF_UNLIKELY(typei != LUA_TTABLE)) {
    MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "Lua marpaESLIFValue global is not a table");
    goto err; /* Lua will shutdown anyway */
  }
  /* And this marpaESLIFValue is a table with a key "marpaESLIFValueContextp" */
  LUA_GETFIELD(&typei, marpaESLIFp, L, -1, "marpaESLIFLuaValueContextp");     /* stack: ..., marpaESLIFValueTable, marpaESLIFLuaValueContextp */
  if (MARPAESLIF_UNLIKELY(typei != LUA_TLIGHTUSERDATA)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Lua marpaESLIFLuaValueContextp is not a light userdata");
    goto err; /* Lua will shutdown anyway */
  }
  LUA_TOUSERDATA(marpaESLIFp, L, &marpaESLIFLuaValueContextp, -1);
  LUA_POP(marpaESLIFp, L, 2);                                                  /* stack: ... */

  /* Proxy to the lua representation callback action - then userDatavp has to be marpaESLIFLuaValueContextp */
  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_representationb((void *) marpaESLIFLuaValueContextp /* userDatavp */, marpaESLIFValueResultp, inputcpp, inputlp, encodingasciisp, disposeCallbackpp, stringbp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_eventactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char                   *funcs       = "_marpaESLIFRecognizer_lua_eventactionb";
  marpaESLIF_t                        *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFRecognizerEventCallback_t  eventCallbackp;
  lua_State                           *L;
  short                                rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  eventCallbackp = marpaESLIFLua_recognizerEventActionResolver(userDatavp, marpaESLIFRecognizerp, marpaESLIFRecognizerp->actions);
  if (MARPAESLIF_UNLIKELY(eventCallbackp == NULL)) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "Lua bindings returned no event-action callback");
    goto err; /* Lua will shutdown anyway */
  }

  if (MARPAESLIF_UNLIKELY(! eventCallbackp(userDatavp, marpaESLIFRecognizerp, eventArrayp, eventArrayl, marpaESLIFValueResultBoolp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static marpaESLIFGrammar_t *_marpaESLIF_luaGrammarp(marpaESLIF_t *marpaESLIFp, char *starts)
/*****************************************************************************/
{
  static const char *luas =
    "#\n"
    "# Special entry just to make ESLIF happy\n"
    "#\n"
    "dummy ::= 'not used'\n"
    "#\n"
    "# Special entries used to hook the lua grammar in ESLIF\n"
    "#\n"
    "<lua funcbody after lparen>            :[2]:= <lua optional parlist> ')' <lua block> <lua keyword end>\n"
    "<lua args after lparen>                :[2]:= <lua optional explist> ')' action => ::lua->listSize\n"
    "<lua optional parlist after lparen>    :[2]:= <lua optional parlist> ')' action => ::lua->listSize\n"
    "\n"
    "<luascript>\n"
    "  function addDotDotDotToTable(namelist, comma, dotdotdot)\n"
    "     namelist[#namelist + 1] = dotdotdot\n"
    "     return namelist\n"
    "  end\n"
    "  \n"
    "  function listSize(list, rparen)\n"
    "    return #list\n"
    "  end\n"
    "  \n"
    "</luascript>\n"
    "\n"
    "#\n"
    "# -----------------------------------------------------------------------\n"
    "# Lua 5.3.4 grammar. Based on perl package MarpaX::Languages::Lua::Parser\n"
    "# -----------------------------------------------------------------------\n"
    "#\n"
    ":desc                                  :[2]:= 'Lua 5.3'\n"
    "\n"
    ":discard                               :[2]:= /\\s+/\n"
    ":discard                               :[2]:= /--(?:\\[(?!=*\\[)|(?!\\[))[^\\n]*/\n"
    ":discard                               :[2]:= /--\\[(=*)\\[.*?\\]\\1\\]/s\n"
    "\n"
    "<lua chunk>                            :[2]:=\n"
    "<lua chunk>                            :[2]:= <lua stat list>\n"
    "                                            | <lua stat list> <lua laststat>\n"
    "                                            | <lua stat list> <lua laststat> ';'\n"
    "                                            | <lua laststat> ';'\n"
    "                                            | <lua laststat>\n"
    "<lua stat list>                        :[2]:= <lua stat>\n"
    "                                            | <lua stat> ';'\n"
    "                                            | <lua stat list> <lua stat> rank => -1\n"
    "                                            | <lua stat list> <lua stat> ';'\n"
    "<lua block>                            :[2]:= <lua chunk>\n"
    "<lua stat>                             :[2]:= <lua varlist> '=' <lua explist>\n"
    "                                            | <lua functioncall> rank => -1\n"
    "                                            | <lua label>\n"
    "                                            | <lua keyword goto> <lua Name>\n"
    "                                            | <lua keyword do> <lua block> <lua keyword end>\n"
    "                                            | <lua keyword while> <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
    "                                            | <lua keyword repeat> <lua block> <lua keyword until> <lua exp>\n"
    "                                            | <lua keyword if> <lua exp> <lua keyword then> <lua block> <lua elseif sequence> <lua optional else block> <lua keyword end>\n"
    "                                            | <lua keyword for> <lua Name> '=' <lua exp> ',' <lua exp> ',' <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
    "                                            | <lua keyword for> <lua Name> '=' <lua exp> ',' <lua exp> <lua keyword do> <lua block> <lua keyword end>\n"
    "                                            | <lua keyword for> <lua namelist> <lua keyword in> <lua explist> <lua keyword do> <lua block> <lua keyword end>\n"
    "                                            | <lua keyword function> <lua funcname> <lua funcbody>\n"
    "                                            | <lua keyword local> <lua keyword function> <lua Name> <lua funcbody>\n"
    "                                            | <lua keyword local> <lua namelist> <lua optional namelist initialization>\n"
    "                                            | ';'\n"
    "<lua elseif sequence>                  :[2]:=\n"
    "<lua elseif sequence>                  :[2]:= <lua elseif sequence> <lua elseif block>\n"
    "<lua elseif block>                     :[2]:= <lua keyword elseif> <lua exp> <lua keyword then> <lua block>\n"
    "<lua optional else block>              :[2]:=\n"
    "<lua optional else block>              :[2]:= <lua keyword else> <lua block>\n"
    "<lua optional namelist initialization> :[2]:=\n"
    "<lua optional namelist initialization> :[2]:= '=' <lua explist>\n"
    "<lua laststat>                         :[2]:= <lua keyword return> <lua optional explist>\n"
    "                                            | <lua keyword break>\n"
    "<lua optional explist>                 :[2]:=                                        action => ::row #Empty table\n"
    "<lua optional explist>                 :[2]:= <lua explist>                          action => ::shift\n"
    "<lua funcname>                         :[2]:= <lua dotted name> <lua optional colon name element>\n"
    "<lua dotted name>                      :[2]:= <lua Name>+ separator => '.' proper => 1\n"
    "<lua optional colon name element>      :[2]:=\n"
    "<lua optional colon name element>      :[2]:= ':' <lua Name>\n"
    "<lua varlist>                          :[2]:= <lua var>+ separator => ',' proper => 1\n"
    "<lua var>                              :[2]:= <lua Name>\n"
    "                                            | <lua prefixexp> '[' <lua exp> ']'\n"
    "                                            | <lua prefixexp> '.' <lua Name>\n"
    "<lua namelist>                         :[2]:= <lua Name>+ separator => ',' proper => 1 hide-separator => 1           action => ::row # Table of arguments\n"
    "<lua explist>                          :[2]:= <lua exp>+  separator => ',' proper => 1 hide-separator => 1           action => ::row # Table of expressions\n"
    "<lua exp>                              :[2]:= <lua var>\n"
    "                                            | '(' <lua exp> ')' assoc => group\n"
    "                                           || <lua exp> <lua args> assoc => right\n"
    "                                           || <lua exp> ':' <lua Name> <lua args> assoc => right\n"
    "                                            | <lua keyword nil>\n"
    "                                            | <lua keyword false>\n"
    "                                            | <lua keyword true>\n"
    "                                            | <lua Number>\n"
    "                                            | <lua String>\n"
    "                                            | '...'\n"
    "                                            | <lua tableconstructor>\n"
    "                                            | <lua function>\n"
    "                                           || <lua exp> '^' <exponent> assoc => right\n"
    "                                           || '-' <lua exp>\n"
    "                                            | <lua keyword not> <lua exp>\n"
    "                                            | '#' <lua exp>\n"
    "                                            | '~' <lua exp>\n"
    "                                           || <lua exp> '*' <lua exp>\n"
    "                                            | <lua exp> '/' <lua exp>\n"
    "                                            | <lua exp> '//' <lua exp>\n"
    "                                            | <lua exp> '%' <lua exp>\n"
    "                                           || <lua exp> '+' <lua exp>\n"
    "                                            | <lua exp> '-' <lua exp>\n"
    "                                           || <lua exp> '..' <lua exp> assoc => right\n"
    "                                           || <lua exp> '<<' <lua exp>\n"
    "                                            | <lua exp> '>>' <lua exp>\n"
    "                                           || <lua exp> '&' <lua exp>\n"
    "                                           || <lua exp> '~' <lua exp>\n"
    "                                           || <lua exp> '|' <lua exp>\n"
    "                                           || <lua exp> '<' <lua exp>\n"
    "                                            | <lua exp> '<=' <lua exp>\n"
    "                                            | <lua exp> '>' <lua exp>\n"
    "                                            | <lua exp> '>=' <lua exp>\n"
    "                                            | <lua exp> '==' <lua exp> rank => 1\n"
    "                                            | <lua exp> '~=' <lua exp>\n"
    "                                           || <lua exp> <lua keyword and> <lua exp> rank => 1\n"
    "                                           || <lua exp> <lua keyword or> <lua exp>\n"
    "<exponent>                             :[2]:= <lua var>\n"
    "                                            | '(' <lua exp> ')'\n"
    "                                           || <exponent> <lua args>\n"
    "                                           || <exponent> ':' <lua Name> <lua args>\n"
    "                                            | <lua keyword nil>\n"
    "                                            | <lua keyword false>\n"
    "                                            | <lua keyword true>\n"
    "                                            | <lua Number>\n"
    "                                            | <lua String>\n"
    "                                            | '...'\n"
    "                                            | <lua tableconstructor>\n"
    "                                            | <lua function>\n"
    "                                           || <lua keyword not> <exponent>\n"
    "                                            | '#' <exponent>\n"
    "                                            | '-' <exponent>\n"
    "<lua prefixexp>                        :[2]:= <lua var>\n"
    "                                            | <lua functioncall>\n"
    "                                            | '(' <lua exp> ')'\n"
    "<lua functioncall>                     :[2]:= <lua prefixexp> <lua args>\n"
    "                                            | <lua prefixexp> ':' <lua Name> <lua args>\n"
    "<lua args>                             :[2]:= '(' <lua optional explist> ')'\n"
    "                                            | <lua tableconstructor>\n"
    "                                            | <lua String>\n"
    "<lua function>                         :[2]:= <lua keyword function> <lua funcbody>\n"
    "<lua funcbody>                         :[2]:= '(' <lua optional parlist> ')' <lua block> <lua keyword end>\n"
    "<lua optional parlist>                 :[2]:=                             action => ::row # Empty table\n"
    "<lua optional parlist>                 :[2]:= <lua namelist>              action => ::shift\n"
    "                                            | <lua namelist> ',' '...'    action => ::lua->addDotDotDotToTable\n"
    "                                            | '...'                       action => ::row # Table with one entry\n"
    " \n"
    "# A lone comma is not allowed in an empty fieldlist,\n"
    "# apparently. This is why I use a dedicated rule\n"
    "# for an empty table and a '+' sequence,\n"
    "# instead of a '*' sequence.\n"
    " \n"
    "<lua tableconstructor>                 :[2]:= '{' '}'\n"
    "                                            | '{' <lua fieldlist> '}'\n"
    "<lua fieldlist>                        :[2]:= <lua field>+ separator => [,;]\n"
    "<lua field>                            :[2]:= '[' <lua exp> ']' '=' <lua exp>\n"
    "                                            | <lua Name> '=' <lua exp>\n"
    "                                            | <lua exp>\n"
    "<lua label>                            :[2]:= '::' <lua Name> '::'\n"
    "<lua Name>                             :[2]:= <LUA NAME> - <LUA RESERVED KEYWORDS>\n"
    "<lua String>                           :[2]:= /'(?:[^\\\\']*(?:\\\\.[^\\\\']*)*)'|\"(?:[^\\\\\"]*(?:\\\\.[^\\\\\"]*)*)\"|\\[(=*)\\[.*?\\]\\1\\]/su\n"
    "\n"
    "# A lua number can start with '.' if the later is followed by at least one (hex) digit\n"
    "<lua Number>                           :[2]:= /(?:\\.[0-9]+|[0-9]+(?:\\.[0-9]*)?)(?:[eE][+-]?[0-9]+)?/ \n"
    "                                            | /0[xX](?:\\.[a-fA-F0-9]+|[a-fA-F0-9]+(?:\\.[a-fA-F0-9]*)?)(?:\\.[a-fA-F0-9]*)?(?:[pP][+-]?[0-9]+)?/ \n"
    "\n"
    "\n"
    "<lua keyword and>                      :[3]:= 'and'\n"
    "<lua keyword break>                    :[3]:= 'break'\n"
    "<lua keyword do>                       :[3]:= 'do'\n"
    "<lua keyword else>                     :[3]:= 'else'\n"
    "<lua keyword elseif>                   :[3]:= 'elseif'\n"
    "<lua keyword end>                      :[3]:= 'end'\n"
    "<lua keyword false>                    :[3]:= 'false'\n"
    "<lua keyword for>                      :[3]:= 'for'\n"
    "<lua keyword function>                 :[3]:= 'function'\n"
    "<lua keyword if>                       :[3]:= 'if'\n"
    "<lua keyword in>                       :[3]:= 'in'\n"
    "<lua keyword local>                    :[3]:= 'local'\n"
    "<lua keyword nil>                      :[3]:= 'nil'\n"
    "<lua keyword not>                      :[3]:= 'not'\n"
    "<lua keyword or>                       :[3]:= 'or'\n"
    "<lua keyword repeat>                   :[3]:= 'repeat'\n"
    "<lua keyword return>                   :[3]:= 'return'\n"
    "<lua keyword then>                     :[3]:= 'then'\n"
    "<lua keyword true>                     :[3]:= 'true'\n"
    "<lua keyword until>                    :[3]:= 'until'\n"
    "<lua keyword while>                    :[3]:= 'while'\n"
    "<lua keyword goto>                     :[3]:= 'goto'\n"
    " \n"
    "<LUA NAME>                             :[3]:= /[a-zA-Z_][a-zA-Z_0-9]*/\n"
    "<LUA RESERVED KEYWORDS>                :[3]:= 'and'\n"
    "                                            | 'break'\n"
    "                                            | 'do'\n"
    "                                            | 'else'\n"
    "                                            | 'elseif'\n"
    "                                            | 'end'\n"
    "                                            | 'false'\n"
    "                                            | 'for'\n"
    "                                            | 'function'\n"
    "                                            | 'if'\n"
    "                                            | 'in'\n"
    "                                            | 'local'\n"
    "                                            | 'nil'\n"
    "                                            | 'not'\n"
    "                                            | 'or'\n"
    "                                            | 'repeat'\n"
    "                                            | 'return'\n"
    "                                            | 'then'\n"
    "                                            | 'true'\n"
    "                                            | 'until'\n"
    "                                            | 'while'\n"
    "                                            | 'goto'\n"
    "\n"
    ;
  char                      *grammars;
  size_t                     grammarl;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;
  marpaESLIFGrammar_t       *rcp;

  if (starts == NULL) {
    grammars = (char *) luas;
    grammarl = strlen(grammars);
  } else {
    grammarl = strlen(luas)
      + strlen("\n:start :[2]:= <")
      + strlen(starts)
      + strlen(">\n");
    grammars = (char *) malloc(grammarl + 1);
    if (grammars == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    strcpy(grammars, luas);
    strcat(grammars, "\n:start :[2]:= <");
    strcat(grammars, starts);
    strcat(grammars, ">\n");
  }
  marpaESLIFGrammarOption.bytep     = grammars;
  marpaESLIFGrammarOption.bytel     = grammarl;
  marpaESLIFGrammarOption.encodings = "ASCII";
  marpaESLIFGrammarOption.encodingl = 5;

  rcp = _marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption, 0 /* startGrammarIsLexemeb */, 0 /* luaNewThreadb */);
  goto done;

 err:
  rcp = NULL;

 done:
  if ((grammars != NULL) && (grammars != (char *) luas)) {
    free(grammars);
  }
  return rcp;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_action_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFValue_lua_action_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFValue_lua_function_loadb(marpaESLIFValuep)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_valueCallbackb(userDatavp, marpaESLIFValuep, arg0i, argni, NULL /* marpaESLIFValueResultLexemep */, resulti, nullableb, 0 /* symbolb */, 1 /* precompiledb */))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_symbol_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFValue_lua_symbol_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFValue_lua_newp(marpaESLIFValuep);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFValue_lua_function_loadb(marpaESLIFValuep)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_valueCallbackb(userDatavp, marpaESLIFValuep, -1 /* arg0i */, -1 /* argni */, marpaESLIFValueResultp, resulti, 0 /* nullableb */, 1 /* symbolb */, 1 /* precompiledb */))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_ifaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_ifaction_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_ifCallbackb(userDatavp, marpaESLIFRecognizerp, marpaESLIFValueResultp, marpaESLIFValueResultBoolp, 1 /* precompiledb */))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_regexaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_regexaction_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_regexCallbackb(userDatavp, marpaESLIFRecognizerp, marpaESLIFCalloutBlockp, marpaESLIFValueResultOutp, 1 /* precompiledb */))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_generatoraction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_generatoraction_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_generatorCallbackb(userDatavp, marpaESLIFRecognizerp, contextp, 1 /* precompiledb */, marpaESLIFValueResultOutp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_eventaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_eventaction_functionb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  short              rcb;

  /* Create the lua state if needed */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_eventCallbackb(userDatavp, marpaESLIFRecognizerp, eventArrayp, eventArrayl, marpaESLIFValueResultBoolp, 1 /* precompiledb */))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_function_loadb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  char         *actions     = marpaESLIFValuep->actionp->u.luaFunction.actions;
  marpaESLIF_t *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  lua_State    *L           = marpaESLIFValuep->L;
  short rcb;

  if (marpaESLIFValuep->actionp->u.luaFunction.luacb) {
    if (marpaESLIFValuep->actionp->u.luaFunction.luacp == NULL) {
      /* We precompile the unstripped version if not already done */
      if (! _marpaESLIFValue_lua_precompileb(marpaESLIFValuep, actions, strlen(actions), 0 /* stripb */, 0 /* popi */)) {
        goto err;
      }
      marpaESLIFValuep->actionp->u.luaFunction.luacp = marpaESLIFValuep->luaprecompiledp;
      marpaESLIFValuep->actionp->u.luaFunction.luacl = marpaESLIFValuep->luaprecompiledl;

      marpaESLIFValuep->luaprecompiledp = NULL;
      marpaESLIFValuep->luaprecompiledl = 0;
    } else {
      /* We inject it */
      LUAL_LOADBUFFER(marpaESLIFp, L, marpaESLIFValuep->actionp->u.luaFunction.luacp, marpaESLIFValuep->actionp->u.luaFunction.luacl, "=<luafunction/>");
    }
  } else {
    LUAL_LOADSTRING(marpaESLIFp, L, actions);
  }

  /* We injected a function that returns a function */
  LUA_PCALL(marpaESLIFp, L, 0, 1, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  char         *actions     = marpaESLIFRecognizerp->actionp->u.luaFunction.actions;
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  lua_State    *L           = marpaESLIFRecognizerp->L;
  short rcb;

  if (marpaESLIFRecognizerp->actionp->u.luaFunction.luacb) {
    if (marpaESLIFRecognizerp->actionp->u.luaFunction.luacp == NULL) {
      /* We precompile the unstripped version if not already done */
      if (! _marpaESLIFRecognizer_lua_function_precompileb(marpaESLIFRecognizerp, actions, strlen(actions), 0 /* stripb */, 0 /* popi */)) {
        goto err;
      }
      marpaESLIFRecognizerp->actionp->u.luaFunction.luacp = marpaESLIFRecognizerp->luaprecompiledp;
      marpaESLIFRecognizerp->actionp->u.luaFunction.luacl = marpaESLIFRecognizerp->luaprecompiledl;

      marpaESLIFRecognizerp->luaprecompiledp = NULL;
      marpaESLIFRecognizerp->luaprecompiledl = 0;
    } else {
      /* We inject it */
      LUAL_LOADBUFFER(marpaESLIFp, L, marpaESLIFRecognizerp->actionp->u.luaFunction.luacp, marpaESLIFRecognizerp->actionp->u.luaFunction.luacl, "=<luafunction/>");
    }
  } else {
    LUAL_LOADSTRING(marpaESLIFp, L, actions);
  }

  /* We injected a function that returns a function */
  LUA_PCALL(marpaESLIFp, L, 0, 1, 0);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_function_precompileb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *luabytep, size_t luabytel, short stripb, int popi)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  lua_State    *L;
  short         rcb;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }

  /* Compiles lua script */
  LUAL_LOADBUFFER(marpaESLIFp, L, luabytep, luabytel, "=<luafunction/>");

  /* Result is a "function" at the top of the stack - we now have to dump it so that lua knows about it  */
  LUA_DUMP(marpaESLIFp, L, _marpaESLIFRecognizer_lua_writeri, marpaESLIFRecognizerp, stripb);

  if (popi > 0) {
    LUA_POP(marpaESLIFp, L, popi);
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_push_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
/* Note: by def, symbolp->callp is != NULL, symbolp->declp may be NULL,      */
/* symbolp->parameterizedRhsLhsp is != NULL                                  */
/*****************************************************************************/
{
  static const char            *funcs                = "_marpaESLIFRecognizer_lua_push_contextb";
  marpaESLIF_t                 *marpaESLIFp          = marpaESLIFRecognizerp->marpaESLIFp;
  genericLogger_t              *genericLoggerp       = NULL;
  char                         *parlistWithoutParens = NULL;
  int                           topi                 = -1;
  lua_State                    *L;
#ifndef MARPAESLIF_NTRACE
  int                           i;
  char                         *p;
  char                         *p2;
  char                          c;
#endif
  marpaESLIF_stringGenerator_t  marpaESLIF_stringGenerator;
  short                         rcb;

  marpaESLIF_stringGenerator.s = NULL;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  /* --------------------------------------------------------------------------------------------- */
  /* return function()                                                                             */
  /*   local PARLIST = table.unpack(marpaESLIFContextStackp:get())                                 */
  /*   marpaESLIFContextStackp:push(table.pack(EXPLIST))                                           */
  /* end                                                                                           */
  /* --------------------------------------------------------------------------------------------- */
  if (symbolp->pushContextActionp == NULL) {
    /* We initialize the correct action content. */
    symbolp->pushContextActionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
    if (symbolp->pushContextActionp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    symbolp->pushContextActionp->type                     = MARPAESLIF_ACTION_TYPE_LUA_FUNCTION;
    symbolp->pushContextActionp->u.luaFunction.luas       = NULL; /* Original action as per the grammar - not used */
    symbolp->pushContextActionp->u.luaFunction.actions    = NULL; /* The action injected into lua */
    symbolp->pushContextActionp->u.luaFunction.luacb      = 0;    /* True if action is precompiled */
    symbolp->pushContextActionp->u.luaFunction.luacp      = NULL; /* Precompiled chunk. Not NULL only when luacb is true and action as been used at least once */
    symbolp->pushContextActionp->u.luaFunction.luacl      = 0;    /* Precompiled chunk length */
    symbolp->pushContextActionp->u.luaFunction.luacstripp = NULL; /* Precompiled stripped chunk - not used */
    symbolp->pushContextActionp->u.luaFunction.luacstripl = 0;    /* Precompiled stripped chunk length */

    marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
    marpaESLIF_stringGenerator.s           = NULL;
    marpaESLIF_stringGenerator.l           = 0;
    marpaESLIF_stringGenerator.okb         = 0;
    marpaESLIF_stringGenerator.allocl      = 0;

    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      goto err;
    }

    GENERICLOGGER_TRACE(genericLoggerp,
                        "return function()\n"
                        );
    if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGenerator.okb)) {
      goto err;
    }
    if ((symbolp->declp != NULL) && (symbolp->declp->sizei > 0)) {
      parlistWithoutParens = strdup(symbolp->declp->luaparlists);
      if (parlistWithoutParens == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "strdup failure, %s", strerror(errno));
        goto err;
      }
      parlistWithoutParens[0] = ' '; /* First character will be a space, this is why we say local%s just below */
      parlistWithoutParens[strlen(parlistWithoutParens) - 1] = '\0';
      GENERICLOGGER_TRACEF(genericLoggerp, "  local%s = table.unpack(marpaESLIFContextStackp:get())\n", parlistWithoutParens);
      if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGenerator.okb)) {
        goto err;
      }
    }
#ifndef MARPAESLIF_NTRACE
    if (parlistWithoutParens != NULL) {
      /* Enclose parlist members with '' */
      p = parlistWithoutParens;
      for (i = 0; i < symbolp->declp->sizei; i++) {
        /* Get start of symbol */
        c = *p;
        while (1) {
          if ((c == '_') || ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            break;
          }
          c = *++p;
        }
        /* Put NUL at end of symbol */
        p2 = p;
        c = *++p2;
        while (1) {
          if ((c == '_') || ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
            c = *++p2;
            continue;
          }
          break;
        }
        *p2 = '\0';
        GENERICLOGGER_TRACEF(genericLoggerp, "  print('[%d][lua] Parameter %s = '..tostring(%s))\n", marpaESLIFRecognizerp->leveli, p, p);
        if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGenerator.okb)) {
          goto err;
        }
        *p2 = c;
        p = ++p2;
      }
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "  print('[%d][lua] Pushed: %s')\n", marpaESLIFRecognizerp->leveli, symbolp->callp->luaexplists);
#endif
    GENERICLOGGER_TRACEF(genericLoggerp, "  marpaESLIFContextStackp:push(table.pack%s)\n", symbolp->callp->luaexplists);
    if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGenerator.okb)) {
      goto err;
    }
    GENERICLOGGER_TRACE(genericLoggerp, "end\n");
    if (MARPAESLIF_UNLIKELY(! marpaESLIF_stringGenerator.okb)) {
      goto err;
    }

    /* Action is always precompiled unless declp or callp says it should not */
    if ((! symbolp->callp->luaexplistcb) || ((symbolp->declp != NULL) && (! symbolp->declp->luaparlistcb))) {
      symbolp->pushContextActionp->u.luaFunction.luacb = 0;
    } else {
      symbolp->pushContextActionp->u.luaFunction.luacb = 1;
    }

    symbolp->pushContextActionp->u.luaFunction.actions = marpaESLIF_stringGenerator.s;
    marpaESLIF_stringGenerator.s = NULL;

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Generated action:\n%s", symbolp->pushContextActionp->u.luaFunction.actions);
  }

  /* Call the context action */
  marpaESLIFRecognizerp->actionp = symbolp->pushContextActionp;
  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_pushContextb(marpaESLIFRecognizerp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (marpaESLIF_stringGenerator.s != NULL) {
    free(marpaESLIF_stringGenerator.s);
  }
  GENERICLOGGER_FREE(genericLoggerp);
  if (parlistWithoutParens != NULL) {
    free(parlistWithoutParens);
  }
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_pop_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_pop_contextb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  const char        *pops  =
    "return function()\n"
    "  marpaESLIFContextStackp:pop()\n"
    "end\n";
  short       rcb;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  /* ---------------------------------------------------------------------------------------------------------------- */
  /* return function()                                                                                                */
  /*   marpaESLIFContextStackp:pop()                                                                                  */
  /* end                                                                                                              */
  /* ---------------------------------------------------------------------------------------------------------------- */
  if (marpaESLIFRecognizerp->popContextActionp == NULL) {
    /* We initialize the correct action content. */
    marpaESLIFRecognizerp->popContextActionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
    if (marpaESLIFRecognizerp->popContextActionp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->popContextActionp->type                     = MARPAESLIF_ACTION_TYPE_LUA_FUNCTION;
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luas       = NULL; /* Original action as per the grammar - not used */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.actions    = NULL; /* The action injected into lua */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luacb      = 1;    /* True if action is precompiled */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luacp      = NULL; /* Precompiled chunk. Not NULL only when luacb is true and action as been used at least once */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luacl      = 0;    /* Precompiled chunk length */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luacstripp = NULL; /* Precompiled stripped chunk - not used */
    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.luacstripl = 0;    /* Precompiled stripped chunk length */

    marpaESLIFRecognizerp->popContextActionp->u.luaFunction.actions = strdup(pops);
    if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp->popContextActionp->u.luaFunction.actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Generated action:\n%s", marpaESLIFRecognizerp->popContextActionp->u.luaFunction.actions);
  }

  marpaESLIFRecognizerp->actionp = marpaESLIFRecognizerp->popContextActionp;
  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_popContextb(marpaESLIFRecognizerp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_get_contextp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_get_contextp";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  const char        *gets  =
    "return function()\n"
    "  return marpaESLIFContextStackp:get()\n"
    "end\n";
  short              rcb;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  /* ---------------------------------------------------------------------------------------------------------------- */
  /* return function()                                                                                                */
  /*   return marpaESLIFContextStackp:get()                                                                           */
  /* end                                                                                                              */
  /* ---------------------------------------------------------------------------------------------------------------- */
  if (marpaESLIFRecognizerp->getContextActionp == NULL) {
    /* We initialize the correct action content. */
    marpaESLIFRecognizerp->getContextActionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
    if (marpaESLIFRecognizerp->getContextActionp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->getContextActionp->type                     = MARPAESLIF_ACTION_TYPE_LUA_FUNCTION;
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luas       = NULL; /* Original action as per the grammar - not used */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.actions    = NULL; /* The action injected into lua */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luacb      = 1;    /* True if action is precompiled */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luacp      = NULL; /* Precompiled chunk. Not NULL only when luacb is true and action as been used at least once */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luacl      = 0;    /* Precompiled chunk length */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luacstripp = NULL; /* Precompiled stripped chunk - not used */
    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.luacstripl = 0;    /* Precompiled stripped chunk length */

    marpaESLIFRecognizerp->getContextActionp->u.luaFunction.actions = strdup(gets);
    if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp->getContextActionp->u.luaFunction.actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Generated action:\n%s", marpaESLIFRecognizerp->getContextActionp->u.luaFunction.actions);
  }

  marpaESLIFRecognizerp->actionp = marpaESLIFRecognizerp->getContextActionp;
  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_getContextb(marpaESLIFRecognizerp, contextp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFRecognizer_lua_set_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFRecognizer_lua_set_contextb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  int                topi        = -1;
  lua_State         *L;
  const char        *sets  =
    "return function(context)\n"
    "  marpaESLIFContextStackp:set(context)\n"
    "end\n";
  short              rcb;

  /* Create the lua state if needed - this is a lua state using ESLIF internal's, i.e. there is no </luascript> in it. */
  L = _marpaESLIFRecognizer_lua_newp(marpaESLIFRecognizerp);
  if (MARPAESLIF_UNLIKELY(L == NULL)) {
    goto err;
  }
  LUA_GETTOP(marpaESLIFp, L, &topi);

  /* ---------------------------------------------------------------------------------------------------------------- */
  /* return function()                                                                                                */
  /*   return marpaESLIFContextStackp:set()                                                                           */
  /* end                                                                                                              */
  /* ---------------------------------------------------------------------------------------------------------------- */
  if (marpaESLIFRecognizerp->setContextActionp == NULL) {
    /* We initialize the correct action content. */
    marpaESLIFRecognizerp->setContextActionp = (marpaESLIF_action_t *) malloc(sizeof(marpaESLIF_action_t));
    if (marpaESLIFRecognizerp->setContextActionp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->setContextActionp->type                     = MARPAESLIF_ACTION_TYPE_LUA_FUNCTION;
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luas       = NULL; /* Original action as per the grammar - not used */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.actions    = NULL; /* The action injected into lua */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luacb      = 1;    /* True if action is precompiled */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luacp      = NULL; /* Precompiled chunk. Not NULL only when luacb is true and action as been used at least once */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luacl      = 0;    /* Precompiled chunk length */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luacstripp = NULL; /* Precompiled stripped chunk - not used */
    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.luacstripl = 0;    /* Precompiled stripped chunk length */

    marpaESLIFRecognizerp->setContextActionp->u.luaFunction.actions = strdup(sets);
    if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp->setContextActionp->u.luaFunction.actions == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "strdup failure, %s", strerror(errno));
      goto err;
    }

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Generated action:\n%s", marpaESLIFRecognizerp->setContextActionp->u.luaFunction.actions);
  }

  marpaESLIFRecognizerp->actionp = marpaESLIFRecognizerp->setContextActionp;
  if (! _marpaESLIFRecognizer_lua_function_loadb(marpaESLIFRecognizerp)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFLua_setContextb(marpaESLIFRecognizerp, contextp))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  MARPAESLIFLUA_LOG_LATEST_ERROR(marpaESLIFp, L);
  rcb = 0;

 done:
  if (topi >= 0) {
    LUA_SETTOP(marpaESLIFp, L, topi);
  }
  return rcb;
}
