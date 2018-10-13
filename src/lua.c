#include "marpaESLIF/internal/lua.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "../src/bindings/lua/src/marpaESLIFLua.c"

/* Note that a lua integer is:
 * - an int  if LUA_INT_TYPE = LUA_INT_INT
 * - a  long if LUA_INT_TYPE = LUA_INT_LONG
 * a lua number is:
 * - a float  if LUA_FLOAT_TYPE = LUA_FLOAT_FLOAT
 * - a double if LUA_FLOAT_TYPE = LUA_FLOAT_DOUBLE
 *
 * This is because we are forcing LUA_USE_C89 in our embedded lua. Then
 * remains these two possiblities for lua integer and number.
 */

#undef  FILENAMES
#define FILENAMES "lua.c" /* For logging */

/* Internal initialization */
static const char *MARPAESLIF_LUA_WRAPPER = "__marpaESLIFLuaWrapper";
static const char *MARPAESLIF_LUA_TABLE = "__marpaESLIFLuaTable";
static const char *MARPAESLIF_LUA_INIT =
  "--------------------------------------------------------\n"
  "-- Internal table used to communicate with outside world\n"
  "--------------------------------------------------------\n"
  "__marpaESLIFLuaTable = {}\n"
  "\n"
  "--------------------------------------------------------\n"
  "-- Function call wrapper\n"
  "--------------------------------------------------------\n"
  "function __marpaESLIFLuaWrapper(indice, funcname, ...)\n"
  "  local value = funcname(...)\n"
  "  __marpaESLIFLuaTable[indice + 1] = value\n"
  "  return value\n"
  "end\n";

static short       _marpaESLIF_lua_newb(marpaESLIFValue_t *marpaESLIFValuep);
static short       _marpaESLIF_lua_push_argb(marpaESLIFValue_t *marpaESLIFValuep, int i);
static short       _marpaESLIF_lua_pop_argb(marpaESLIFValue_t *marpaESLIFValuep, int resulti);
static void        _marpaESLIF_lua_freeInternalActionv(void *userDatavp, int resulti);
static const char *_marpaESLIF_lua_types(int typei);
static int         _marpaESLIF_lua_grammarWriteri(lua_State *L, const void* p, size_t sz, void* ud);

/* Value result transformer helper */
short _marpaESLIF_lua_transformUndefb(void *userDatavp, int contexti);
short _marpaESLIF_lua_transformCharb(void *userDatavp, int contexti, char c);
short _marpaESLIF_lua_transformShortb(void *userDatavp, int contexti, short b);
short _marpaESLIF_lua_transformIntb(void *userDatavp, int contexti, int i);
short _marpaESLIF_lua_transformLongb(void *userDatavp, int contexti, long l);
short _marpaESLIF_lua_transformFloatb(void *userDatavp, int contexti, float f);
short _marpaESLIF_lua_transformDoubleb(void *userDatavp, int contexti, double d);
short _marpaESLIF_lua_transformPtrb(void *userDatavp, int contexti, void *p);
short _marpaESLIF_lua_transformArrayb(void *userDatavp, int contexti, void *p, size_t sizel);
short _marpaESLIF_lua_transformBoolb(void *userDatavp, int contexti, short b);
static const marpaESLIFValueResultTransform_t marpaESLIF_lua_transform = {
  _marpaESLIF_lua_transformUndefb,
  _marpaESLIF_lua_transformCharb,
  _marpaESLIF_lua_transformShortb,
  _marpaESLIF_lua_transformIntb,
  _marpaESLIF_lua_transformLongb,
  _marpaESLIF_lua_transformFloatb,
  _marpaESLIF_lua_transformDoubleb,
  _marpaESLIF_lua_transformPtrb,
  _marpaESLIF_lua_transformArrayb,
  _marpaESLIF_lua_transformBoolb
};

static const char *LUATYPE_TNIL_STRING = "LUA_TNIL";
static const char *LUATYPE_TNUMBER_STRING = "LUA_TNUMBER";
static const char *LUATYPE_TBOOLEAN_STRING = "LUA_TBOOLEAN";
static const char *LUATYPE_TSTRING_STRING = "LUA_TSTRING";
static const char *LUATYPE_TTABLE_STRING = "LUA_TTABLE";
static const char *LUATYPE_TFUNCTION_STRING = "LUA_TFUNCTION";
static const char *LUATYPE_TUSERDATA_STRING = "LUA_TUSERDATA";
static const char *LUATYPE_TTHREAD_STRING = "LUA_TTHREAD";
static const char *LUATYPE_TLIGHTUSERDATA_STRING = "LUA_TLIGHTUSERDATA";
static const char *LUATYPE_TUNKNOWN_STRING = "UNKNOWN";

#define LOG_PANIC_STRING(containerp, f) do {                            \
    char *panicstring;							\
    if (luaunpanic_panicstring(&panicstring, containerp->L)) {          \
      MARPAESLIF_ERRORF(containerp->marpaESLIFp, "%s panic", #f);       \
    } else {								\
      MARPAESLIF_ERRORF(containerp->marpaESLIFp, "%s panic: %s", #f, panicstring); \
    }									\
  } while (0)

#define LOG_ERROR_STRING(containerp, f) do {                            \
    const char *errorstring;                                            \
    if (luaunpanic_tostring(&errorstring, containerp->L, -1)) {         \
      LOG_PANIC_STRING(containerp, luaunpanic_tostring);                \
      MARPAESLIF_ERRORF(containerp->marpaESLIFp, "%s failure", #f);     \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERRORF(containerp->marpaESLIFp, "%s failure", #f);   \
      } else {								\
        MARPAESLIF_ERRORF(containerp->marpaESLIFp, "%s failure: %s", #f, errorstring); \
      }									\
    }                                                                   \
  } while (0)

#define LUAL_CHECKVERSION(containerp) do {                              \
    if (luaunpanicL_checkversion(containerp->L)) {                      \
      LOG_PANIC_STRING(containerp, luaL_checkversion);                  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_OPENLIBS(containerp) do {                                 \
    if (luaunpanicL_openlibs(containerp->L)) {                         \
      LOG_PANIC_STRING(containerp, luaL_openlibs);                     \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHNIL(containerp) do {                                   \
    if (luaunpanic_pushnil(containerp->L)) {                           \
      LOG_PANIC_STRING(containerp, lua_pushnil);                       \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHLSTRING(containerp, s, l) do {                          \
    if (luaunpanic_pushlstring(NULL, containerp->L, s, l)) {            \
      LOG_PANIC_STRING(containerp, lua_pushlstring);                    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_DOSTRING(containerp, string) do {                          \
    int rc;                                                             \
    if (luaunpanicL_dostring(&rc, containerp->L, string)) {             \
      LOG_PANIC_STRING(containerp, luaL_dostring);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (rc) {                                                           \
      LOG_ERROR_STRING(containerp, luaL_dostring);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHLIGHTUSERDATA(containerp, p) do {                      \
    if (luaunpanic_pushlightuserdata(containerp->L, p)) {              \
      LOG_PANIC_STRING(containerp, lua_pushlightuserdata);             \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_NEWTABLE(containerp) do {                                  \
    if (luaunpanic_newtable(containerp->L)) {                          \
      LOG_PANIC_STRING(containerp, lua_newtable);                      \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHINTEGER(containerp, i) do {                            \
    if (luaunpanic_pushinteger(containerp->L, i)) {                    \
      LOG_PANIC_STRING(containerp, lua_pushinteger);                   \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHNUMBER(containerp, x) do {                             \
    if (luaunpanic_pushnumber(containerp->L, x)) {                     \
      LOG_PANIC_STRING(containerp, lua_pushnumber);                    \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHBOOLEAN(containerp, b) do {                            \
    if (luaunpanic_pushboolean(containerp->L, b)) {                    \
      LOG_PANIC_STRING(containerp, lua_pushboolean);                   \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_DUMP(containerp, writer, data, strip) do {                  \
    int _rci = -1;                                                      \
    if (luaunpanic_dump(&_rci, containerp->L, writer, data, strip)) {   \
      LOG_PANIC_STRING(containerp, lua_dump);                           \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (_rci != 0) {                                                    \
      LOG_ERROR_STRING(containerp, lua_dump);                           \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_RAWGETI(rcp, containerp, idx, n) do {                       \
    if (luaunpanic_rawgeti(rcp, containerp->L, idx, n)) {               \
      LOG_PANIC_STRING(containerp, lua_rawgeti);                        \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_GETFIELDI(rcp, containerp, idx, k) do {                     \
    if (luaunpanic_getfield(rcp, containerp->L, idx, k)) {              \
      LOG_PANIC_STRING(containerp, lua_getfield);                       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_RAWSETI(containerp, idx, n) do {                            \
    if (luaunpanic_rawseti(containerp->L, idx, n)) {                    \
      LOG_PANIC_STRING(containerp, lua_rawseti);                        \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_REMOVE(containerp, idx) do {                                \
    if (luaunpanic_remove(containerp->L, idx)) {                        \
      LOG_PANIC_STRING(containerp, lua_remove);                         \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_GETGLOBAL(rcp, containerp, name) do {                       \
    if (luaunpanic_getglobal(rcp, containerp->L, name)) {               \
      LOG_PANIC_STRING(containerp, lua_getglobal);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETGLOBAL(containerp, name) do {                            \
    if (luaunpanic_setglobal(containerp->L, name)) {                    \
      LOG_PANIC_STRING(containerp, lua_setglobal);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_LOADBUFFER(containerp, s, sz, n) do {                      \
    int _rci = -1;                                                      \
    if (luaunpanicL_loadbuffer(&_rci, containerp->L, s, sz, n)) {       \
      LOG_PANIC_STRING(containerp, luaL_loadbuffer);                    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (_rci != 0) {                                                    \
      LOG_ERROR_STRING(containerp, luaL_loadbuffer);                    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_CALL(containerp, n, r) do {                                 \
    if (luaunpanic_call(containerp->L, n, r)) {                         \
      LOG_PANIC_STRING(containerp, lua_call);                           \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETTOP(containerp, idx) do {                             \
    if (luaunpanic_settop(containerp->L, idx)) {                     \
      LOG_PANIC_STRING(containerp, lua_SETTOP);                      \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TYPE(containerp, rcp, idx) do {                          \
    if (luaunpanic_type(rcp, containerp->L, idx)) {                  \
      LOG_PANIC_STRING(containerp, lua_type);                        \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TOBOOLEAN(containerp, rcp, idx) do {                     \
    if (luaunpanic_toboolean(rcp, containerp->L, idx)) {             \
      LOG_PANIC_STRING(containerp, lua_toboolean);                   \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TONUMBER(containerp, rcp, idx) do {                         \
    int isnum;                                                          \
    if (luaunpanic_tonumberx(rcp, containerp->L, idx, &isnum)) {        \
      LOG_PANIC_STRING(containerp, lua_tonumberx);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (! isnum) {                                                      \
      MARPAESLIF_ERROR(containerp->marpaESLIFp, "lua_tonumberx failure"); \
    }                                                                   \
  } while (0)

#define LUA_TOLSTRING(containerp, rcpp, idx, lenp) do {                 \
    if (luaunpanic_tolstring(rcpp, containerp->L, idx, lenp)) {         \
      LOG_PANIC_STRING(containerp, lua_tolstring);                      \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_TOPOINTER(containerp, rcpp, idx) do {                    \
    if (luaunpanic_topointer(rcpp, containerp->L, idx)) {            \
      LOG_PANIC_STRING(containerp, lua_topointer);                   \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TOUSERDATA(containerp, rcpp, idx) do {                    \
    if (luaunpanic_touserdata(rcpp, containerp->L, idx)) {            \
      LOG_PANIC_STRING(containerp, lua_touserdata);                   \
      errno = ENOSYS;                                                 \
      goto err;                                                       \
    }                                                                 \
  } while (0)

#define LUAL_REQUIREF(containerp, modname, openf, glb) do {	      \
    if (luaunpanicL_requiref(containerp->L, modname, openf, glb)) {   \
      LOG_PANIC_STRING(containerp, lual_requiref);		      \
      errno = ENOSYS;                                                 \
      goto err;                                                       \
    }                                                                 \
  } while (0)

#define LUA_POP(containerp, n) do {                             \
    if (luaunpanic_pop(containerp->L, n)) {                     \
      LOG_PANIC_STRING(containerp, lua_pop);                    \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_PCALL(containerp, n, r, f) do {                             \
    int _rci;                                                           \
    if (luaunpanic_pcall(&_rci, containerp->L, n, r, f)) {              \
      LOG_PANIC_STRING(containerp, lua_pcall);                          \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (_rci != 0) {                                                    \
      LOG_ERROR_STRING(containerp, lua_pcall);                          \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_SETFUNCS(containerp, l, nup) do {                          \
    if (luaunpanicL_setfuncs(containerp->L, l, nup)) {                  \
      LOG_PANIC_STRING(containerp, lua_pcall);                          \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

/*****************************************************************************/
static short _marpaESLIF_lua_newb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
/* This function is called only if there is at least one <luascript/>        */
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  short                rcb;
  int                  typei;

  if (marpaESLIFValuep->L != NULL) {
    /* Already done */
    rcb = 1;
    goto done;
  }

  marpaESLIFGrammarp = marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFGrammarp;

  /* Create Lua state */
  if (luaunpanicL_newstate(&(marpaESLIFValuep->L))) {
    MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "luaunpanicL_newstate failure");
    errno = ENOSYS;
    goto err;
  }
  if (marpaESLIFValuep->L == NULL) {
    MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  /* Open all available libraries */
  LUAL_OPENLIBS(marpaESLIFValuep);

  /* Check Lua version */
  LUAL_CHECKVERSION(marpaESLIFValuep);

  /* Load the marpaESLIFLua library built-in */
  LUAL_REQUIREF(marpaESLIFValuep, "marpaESLIFLua", marpaESLIFLua_installi, 1);
  LUA_POP(marpaESLIFValuep, 1);

  /* Load the marpaESLIFLua library built-in and injects "marpaESLIF" global variable */
  LUAL_REQUIREF(marpaESLIFValuep, "marpaESLIFLua", marpaESLIFLua_installi, 1); /* stack: marpaESLIFLua */
  LUA_GETFIELDI(&typei, marpaESLIFValuep, -1, "marpaESLIF_newFromUnmanaged");  /* stack: marpaESLIFLua, marpaESLIF_newFromUnmanaged */
  if (typei != LUA_TFUNCTION) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a function", "marpaESLIF_newFromUnmanaged");
    goto err;
  }
  LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, marpaESLIFValuep->marpaESLIFp);      /* stack: marpaESLIFLua, marpaESLIF_newFromUnmanaged, marpaESLIFp */
  LUA_PCALL(marpaESLIFValuep, 1, LUA_MULTRET, 0);                              /* stack: marpaESLIFLua, marpaESLIF */
  LUA_SETGLOBAL(marpaESLIFValuep, "marpaESLIF");                               /* stack: marpaESLIFLua */
  LUA_POP(marpaESLIFValuep, 1);                                                /* stack: */

  /* We load byte code generated during grammar validation */
  if ((marpaESLIFGrammarp->luabytep != NULL) && (marpaESLIFGrammarp->luabytel > 0)) {
    LUAL_LOADBUFFER(marpaESLIFValuep, marpaESLIFGrammarp->luaprecompiledp, marpaESLIFGrammarp->luaprecompiledl, "=<luaScript/>");
    LUA_PCALL(marpaESLIFValuep, 0, LUA_MULTRET, 0);
    /* Clear the stack */
    LUA_SETTOP(marpaESLIFValuep, 0);
  }
  
  rcb = 1;
  goto done;

 err:
  if (marpaESLIFValuep->L != NULL) {
    if (luaunpanic_close(marpaESLIFValuep->L)) {
      LOG_PANIC_STRING(marpaESLIFValuep, lua_close);
    }
    marpaESLIFValuep->L = NULL;
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  if (marpaESLIFValuep->L != NULL) {
    if (luaunpanic_close(marpaESLIFValuep->L)) {
      LOG_PANIC_STRING(marpaESLIFValuep, luaunpanic_close);
    }
    marpaESLIFValuep->L = NULL;
  }
}

/*****************************************************************************/
static void  _marpaESLIFGrammar_lua_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp->L != NULL) {
    if (luaunpanic_close(marpaESLIFGrammarp->L)) {
      LOG_PANIC_STRING(marpaESLIFGrammarp, luaunpanic_close);
    }
    marpaESLIFGrammarp->L = NULL;
  }
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_lua_actionb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                     i;
  short                   rcb;
  int                     nargi;
  int                     typei;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC;

  /* Create the lua state if needed */
  if (! _marpaESLIF_lua_newb(marpaESLIFValuep)) {
    goto err;
  }

  /* We going through our function wrapper __marpaESLIFLuaWrapper(indice, funcname, ...) */
  /* 1: __marpaESLIFLuaWrapper */
  LUA_GETGLOBAL(&typei, marpaESLIFValuep, MARPAESLIF_LUA_WRAPPER);
  if (typei != LUA_TFUNCTION) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a function", MARPAESLIF_LUA_WRAPPER);
    goto err;
  }
  /* 2: indice */
  LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) resulti);
  /* 3: user action */
  LUA_GETGLOBAL(&typei, marpaESLIFValuep, marpaESLIFValuep->actions);
  if (typei != LUA_TFUNCTION) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Lua action %s is not a function", marpaESLIFValuep->actions);
    goto err;
  }
  /* 4...: user context */
  LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, userDatavp);
  /* 5...: arguments */
  if (! nullableb) {
    nargi = argni - arg0i + 1;
    for (i = arg0i; i <= argni; i++) {
      if (! _marpaESLIF_lua_push_argb(marpaESLIFValuep, i)) {
        goto err;
      }
    }
  } else {
    nargi = 0;
  }

  /* Lua will make sure there is a room for at least one argument on the stack at return */
  LUA_CALL(marpaESLIFValuep, nargi + 3 /* +3 is for (indice, function, userDatavp) */, 1);

  if (! _marpaESLIF_lua_pop_argb(marpaESLIFValuep, resulti)) {
    goto err;
  }

  /* Clear the stack */
  LUA_SETTOP(marpaESLIFValuep, 0);
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  static const char      *funcs                 = "_marpaESLIFValue_lua_symbolb";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  short                   rcb;
  int                     typei;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC;

  /* Create the lua state if needed */
  if (! _marpaESLIF_lua_newb(marpaESLIFValuep)) {
    goto err;
  }

  /* We going through our function wrapper __marpaESLIFLuaWrapper(indice, funcname, ...) */
  /* 1: __marpaESLIFLuaWrapper */
  LUA_GETGLOBAL(&typei, marpaESLIFValuep, MARPAESLIF_LUA_WRAPPER);
  if (typei != LUA_TFUNCTION) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a function", MARPAESLIF_LUA_WRAPPER);
    goto err;
  }
  /* 2: indice */
  LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) resulti);
  /* 3: user action */
  LUA_GETGLOBAL(&typei, marpaESLIFValuep, marpaESLIFValuep->actions);
  if (typei != LUA_TFUNCTION) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Lua action %s is not a function", marpaESLIFValuep->actions);
    goto err;
  }
  /* 4: context */
  LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, userDatavp);
  /* 5: lexeme */
  LUA_PUSHLSTRING(marpaESLIFValuep, bytep, bytel);

  /* Lua will make sure there is a room for at least one argument on the stack at return */
  LUA_CALL(marpaESLIFValuep, 4 /* (indice, funcname, context, lexeme) */, 1);

  if (! _marpaESLIF_lua_pop_argb(marpaESLIFValuep, resulti)) {
    goto err;
  }

  /* Clear the stack */
  LUA_SETTOP(marpaESLIFValuep, 0);
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC;
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_lua_push_argb(marpaESLIFValue_t *marpaESLIFValuep, int i)
/*****************************************************************************/
{
  static const char       *funcs = "_marpaESLIF_lua_push_argb";
  short                    rcb;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  int                      typei;
  int                     *indiceip;

  marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, i);
  if (marpaESLIFValueResultp == NULL) {
    goto err;
  }

  /* Special values that comes from lua and have a meaning only for lua have the */
  /* context MARPAESLIFVALUE_LUA_CONTEXT. Then per def, this is a PTR that hosts */
  /* a malloced integer. This integer is the indice in an internal lua table. */
  if (marpaESLIFValueResultp->contexti == MARPAESLIFVALUE_LUA_CONTEXT) {
    LUA_GETGLOBAL(&typei, marpaESLIFValuep, MARPAESLIF_LUA_TABLE);                                    /* stack: ..., table */
    if (typei != LUA_TTABLE) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a table", MARPAESLIF_LUA_TABLE);
      goto err;
    }
    indiceip = (int *) marpaESLIFValueResultp->u.p;
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing %s[%d+1]", MARPAESLIF_LUA_TABLE, *indiceip);
    LUA_RAWGETI(NULL, marpaESLIFValuep, -1, (lua_Integer) (*indiceip + 1));                           /* stack: ..., table, table[(*indiceip)+1] */
    LUA_REMOVE(marpaESLIFValuep, -2);                                                                 /* stack: ..., table[(*indiceip)+1] */
  } else {
    /* We have the control here, and we know our lua transformers needs a marpaESLIFValue as user context */
    if (! _marpaESLIFValue_transformb(marpaESLIFValuep, marpaESLIFValueResultp, NULL /* marpaESLIFValueResultResolvedp */, marpaESLIFValuep, (marpaESLIFValueResultTransform_t *) &marpaESLIF_lua_transform)) {
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIF_lua_pop_argb(marpaESLIFValue_t *marpaESLIFValuep, int resulti)
/*****************************************************************************/
{
  static const char       *funcs    = "_marpaESLIF_lua_pop_argb";
  int                     *resultip = NULL;
  int                      typei;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  short                    rcb;
  const char              *bytep;
  size_t                   bytel;
  int                      booli;

  LUA_TYPE(marpaESLIFValuep, &typei, -1);

  switch (typei) {
  case LUA_TNIL:
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
    /* No need to keep that in our internal lua table */
    _marpaESLIF_lua_freeInternalActionv(marpaESLIFValuep /* userDatavp */, resulti);
    MARPAESLIF_TRACE(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TNIL => MARPAESLIF_VALUE_TYPE_UNDEF");
  break;
  case LUA_TNUMBER:
    /* The native type of a number in lua depends on LUA_FLOAT_TYPE */
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_FLOAT;
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TNUMBER => MARPAESLIF_VALUE_TYPE_FLOAT %f", (double) marpaESLIFValueResult.u.f);
    LUA_TONUMBER(marpaESLIFValuep, &(marpaESLIFValueResult.u.f), -1);
#else
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_DOUBLE;
    LUA_TONUMBER(marpaESLIFValuep, &(marpaESLIFValueResult.u.d), -1);
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TNUMBER => MARPAESLIF_VALUE_TYPE_DOUBLE %f", marpaESLIFValueResult.u.d);
#endif
    /* No need to keep that in our internal lua table */
    _marpaESLIF_lua_freeInternalActionv(marpaESLIFValuep /* userDatavp */, resulti);
    break;
  case LUA_TBOOLEAN:
    /* A boolean in lua maps to an int */
    LUA_TOBOOLEAN(marpaESLIFValuep, &booli, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_BOOL;
    marpaESLIFValueResult.u.b             = (booli != 0) ? 1 : 0;
    /* No need to keep that in our internal lua table */
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TBOOLEAN => MARPAESLIF_VALUE_TYPE_BOOL %d", (int) marpaESLIFValueResult.u.b);
    _marpaESLIF_lua_freeInternalActionv(marpaESLIFValuep /* userDatavp */, resulti);
    break;
  case LUA_TSTRING:
    /* A lua string is a sequence of 8-bits chars - so it can contain anything, and may not represent a valid string whatever the encoding */
    /* This first well with our ARRAY notion */
    LUA_TOLSTRING(marpaESLIFValuep, &bytep, -1, &bytel);
    if ((bytep == NULL) || (bytel <= 0)) {
      /* In reality this is a null string */
      marpaESLIFValueResult.contexti        = 0;
      marpaESLIFValueResult.sizel           = 0;
      marpaESLIFValueResult.representationp = NULL;
      marpaESLIFValueResult.shallowb        = 0;
      marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
      MARPAESLIF_TRACE(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TSTRING empty => MARPAESLIF_VALUE_TYPE_UNDEF");
    } else {
      marpaESLIFValueResult.u.p = (char *) malloc(bytel);
      if (marpaESLIFValueResult.u.p == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      marpaESLIFValueResult.contexti        = 0;
      marpaESLIFValueResult.sizel           = bytel;
      marpaESLIFValueResult.representationp = NULL;
      marpaESLIFValueResult.shallowb        = 0;
      marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_ARRAY;
      memcpy(marpaESLIFValueResult.u.p, bytep, bytel);
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TSTRING => MARPAESLIF_VALUE_TYPE_ARRAY at %p of size %ld", bytep, (unsigned long) bytel);
    }
    /* No need to keep that in our internal lua table */
    _marpaESLIF_lua_freeInternalActionv(marpaESLIFValuep /* userDatavp */, resulti);
    break;
    /* The following four cases are meaningul to lua only, and cannot be exported outside */
    /* We store the indice in our internal lua table - in order to trigger the free we */
    /* must allocate something - we allocate an int and store resulti in it. */
  case LUA_TTABLE:
  case LUA_TFUNCTION:
  case LUA_TUSERDATA:
  case LUA_TTHREAD:
    resultip = (int *) malloc(sizeof(int));
    if (resultip == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    *resultip = resulti;
    marpaESLIFValueResult.contexti        = MARPAESLIFVALUE_LUA_CONTEXT;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) resultip;
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Keeping lua type %s (%d) in internal table at indice %d", (char *) _marpaESLIF_lua_types(typei), typei, resulti);
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s => MARPAESLIF_VALUE_TYPE_PTR at %p (pointer to integer, *p: %d)", _marpaESLIF_lua_types(typei), resultip, resulti);
    break;
  case LUA_TLIGHTUSERDATA:
    /* External pointer */
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    LUA_TOUSERDATA(marpaESLIFValuep, &(marpaESLIFValueResult.u.p), -1);
    /* No need to keep that in our internal lua table */
    _marpaESLIF_lua_freeInternalActionv(marpaESLIFValuep /* userDatavp */, resulti);
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "LUA_TLIGHTUSERDATA => MARPAESLIF_VALUE_TYPE_PTR at %p", marpaESLIFValueResult.u.p);
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unsupported lua type %d", typei);
    goto err;
  }

  if (! _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  if (resultip != NULL) {
    free(resultip);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_lua_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_lua_freeDefaultActionv";
  marpaESLIFValue_t       *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                      typei;
  int                     *indiceip;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC;

  /* When called with no pointer, this is an internal call done by the pop of values */
  if (p != NULL) {
    indiceip = (int *) p;

    /* The value is always in  MARPAESLIF_LUA_TABLE */
    LUA_GETGLOBAL(&typei, marpaESLIFValuep, MARPAESLIF_LUA_TABLE);                                      /* stack: table */
    if (typei != LUA_TTABLE) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a table", MARPAESLIF_LUA_TABLE);
    } else {
      LUA_PUSHNIL(marpaESLIFValuep);                                                                    /* stack: table, nil */
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Resetting %s[%d+1]", MARPAESLIF_LUA_TABLE, *indiceip);
      LUA_RAWSETI(marpaESLIFValuep, -2, (lua_Integer) (*indiceip + 1));                                 /* stack: table */
    }

    /* Remove table from the stack */
    LUA_POP(marpaESLIFValuep, -1);

    /* Free this malloced integer */
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Freeing %p that was hosting internal indice %d", p, *indiceip);
    free(p);
  }

  /* This function returns void, though we need the err label */
 err:
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC;
}

/*****************************************************************************/
static void _marpaESLIF_lua_freeInternalActionv(void *userDatavp, int resulti)
/*****************************************************************************/
{
  static const char       *funcs                 = "_marpaESLIF_lua_freeInternalActionv";
  marpaESLIFValue_t       *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIFRecognizer_t  *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  int                      typei;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC;

  /* The value is always in  MARPAESLIF_LUA_TABLE */
  LUA_GETGLOBAL(&typei, marpaESLIFValuep, MARPAESLIF_LUA_TABLE);                                    /* stack: table */
  if (typei != LUA_TTABLE) {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s is not a table", MARPAESLIF_LUA_TABLE);
  } else {
    LUA_PUSHNIL(marpaESLIFValuep);                                                                  /* stack: table, nil */
    LUA_RAWSETI(marpaESLIFValuep, -2, (lua_Integer) (resulti + 1));                                 /* stack: table */
  }

  /* Remove table from the stack */
  LUA_POP(marpaESLIFValuep, -1);

  /* This function returns void, though we need the err label */
 err:
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC;
}

/*****************************************************************************/
static const char *_marpaESLIF_lua_types(int typei)
/*****************************************************************************/
{
  switch (typei) {
  case LUA_TNIL:
    return LUATYPE_TNIL_STRING;
  case LUA_TNUMBER:
    return LUATYPE_TNUMBER_STRING;
  case LUA_TBOOLEAN:
    return LUATYPE_TBOOLEAN_STRING;
  case LUA_TSTRING:
    return LUATYPE_TSTRING_STRING;
  case LUA_TTABLE:
    return LUATYPE_TTABLE_STRING;
  case LUA_TFUNCTION:
    return LUATYPE_TFUNCTION_STRING;
  case LUA_TUSERDATA:
    return LUATYPE_TUSERDATA_STRING;
  case LUA_TTHREAD:
    return LUATYPE_TTHREAD_STRING;
  case LUA_TLIGHTUSERDATA:
    return LUATYPE_TLIGHTUSERDATA_STRING;
  default:
    return LUATYPE_TUNKNOWN_STRING;
  }   
}

/*****************************************************************************/
static short _marpaESLIFGrammar_lua_precompileb(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  char      *luabytep             = NULL;
  size_t     marpaeslif_lua_initl = strlen(MARPAESLIF_LUA_INIT);
  char      *p;
  size_t     luabytel = 0;
  short      rcb;

  if ((marpaESLIFGrammarp->luabytep != NULL) && (marpaESLIFGrammarp->luabytel > 0)) {
    /* We append our own string to user script */
    luabytel = marpaESLIFGrammarp->luabytel + 1 /* \n */ + marpaeslif_lua_initl;
    luabytep = (char *) malloc(luabytel + 1); /* +1 for hiden NUL byte */
    if (luabytep == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }

    p = luabytep;
    memcpy(luabytep, marpaESLIFGrammarp->luabytep, marpaESLIFGrammarp->luabytel);
    p += marpaESLIFGrammarp->luabytel;
    *p++ = '\n';
    memcpy(p, MARPAESLIF_LUA_INIT, marpaeslif_lua_initl);
    p += marpaeslif_lua_initl;
    *p = '\0';

    /* Create Lua state */
    if (luaunpanicL_newstate(&(marpaESLIFGrammarp->L))) {
      MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanicL_newstate failure");
      errno = ENOSYS;
      goto err;
    }
    if (marpaESLIFGrammarp->L == NULL) {
      MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
      errno = ENOSYS;
      goto err;
    }

    /* Open all available libraries */
    LUAL_OPENLIBS(marpaESLIFGrammarp);

    /* Check Lua version */
    LUAL_CHECKVERSION(marpaESLIFGrammarp);

    /* Execute lua script present in the grammar */
    LUAL_LOADBUFFER(marpaESLIFGrammarp, luabytep, luabytel, "=<luaScript/>");
    /* Result is a "function" at the top of the stack - we now have to dump it */
    LUA_DUMP(marpaESLIFGrammarp, _marpaESLIF_lua_grammarWriteri, marpaESLIFGrammarp, 0 /* strip */);
    /* Clear the stack */
    LUA_SETTOP(marpaESLIFGrammarp, 0);
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  /* In any case, free the lua_State, that we temporary created */
  _marpaESLIFGrammar_lua_freev(marpaESLIFGrammarp);
  if (luabytep != NULL) {
    free(luabytep);
  }
  return rcb;
}

/*****************************************************************************/
static int _marpaESLIF_lua_grammarWriteri(lua_State *L, const void* p, size_t sz, void* ud)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) ud;
  char                *q;
  int                  rci;

  if (sz > 0) {
    if (marpaESLIFGrammarp->luaprecompiledp == NULL) {
      marpaESLIFGrammarp->luaprecompiledp = (char *) malloc(sz);
      if (marpaESLIFGrammarp->luaprecompiledp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      q = marpaESLIFGrammarp->luaprecompiledp;
    } else {
      q = (char *) realloc(marpaESLIFGrammarp->luaprecompiledp, marpaESLIFGrammarp->luaprecompiledl + sz);
      if (q == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      marpaESLIFGrammarp->luaprecompiledp = q;
      q += marpaESLIFGrammarp->luaprecompiledl;
    }

    memcpy(q, p, sz);
    marpaESLIFGrammarp->luaprecompiledl += sz;
  }

  rci = 0;
  goto end;
  
 err:
  rci = 1;
  
 end:
  return rci;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformUndefb(void *userDatavp, int contexti)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACE(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing nil");
  LUA_PUSHNIL(marpaESLIFValuep);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformCharb(void *userDatavp, int contexti, char c)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformCharb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing string \"%c\" (0x%2x) of length 1", ((c >= 128) || (! isprint(c & 0xFF))) ? ' ' : c, (int) c);
  LUA_PUSHLSTRING(marpaESLIFValuep, &c, 1);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformShortb(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformShortb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing integer %d", (int) b);
  LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) b);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformIntb(void *userDatavp, int contexti, int i)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing integer %d", i);
  LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) i);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformLongb(void *userDatavp, int contexti, long l)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing integer %ld", l);
  LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) l);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformFloatb(void *userDatavp, int contexti, float f)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing number %f", (double) f);
  LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) f);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformDoubleb(void *userDatavp, int contexti, double d)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing number %f", d);
  LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) d);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformPtrb(void *userDatavp, int contexti, void *p)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing light userdata %p", p);
  LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, p);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformArrayb(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformUndefb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing string {p=%p,sizel=%ld}", p, (unsigned long) sizel);
  LUA_PUSHLSTRING(marpaESLIFValuep, p, sizel);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
short _marpaESLIF_lua_transformBoolb(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIF_lua_transformBoolb";
  /* userDatavp is forced to be marpaESLIFValuep */
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  short              rcb;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Pushing boolean %s", b ? "true" : "false");
  LUA_PUSHBOOLEAN(marpaESLIFValuep, (int) b);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

