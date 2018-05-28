#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/* Note that we emebed c-lua543 distribution, in which lua numbers */
/* are forced to be C89 compliant, i.e. a lua integer is a C long, */
/* and a lua number is a C double. */

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

static int   lua_marpaESLIFValue_stack_set_undef(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_char(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_short(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_int(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_long(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_double(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_ptr(lua_State *L);
static int   lua_marpaESLIFValue_stack_set_array(lua_State *L);
static short lua_marpaESLIFValue_push_arg(marpaESLIFValue_t *marpaESLIFValuep, int i);
static short lua_marpaESLIFValue_pop_arg(marpaESLIFValue_t *marpaESLIFValuep, int resulti);

#define LOG_PANIC_STRING(marpaESLIFValuep, f) do {                      \
    char *panicstring;							\
    if (luaunpanic_panicstring(&panicstring, marpaESLIFValuep->L)) {    \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s panic", #f); \
    } else {								\
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s panic: %s", #f, panicstring); \
    }									\
  } while (0)

#define LOG_ERROR_STRING(marpaESLIFValuep, f) do {                      \
    const char *errorstring;                                            \
    if (luaunpanic_tostring(&errorstring, marpaESLIFValuep->L, -1)) {   \
      LOG_PANIC_STRING(marpaESLIFValuep, luaunpanic_tostring);          \
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s failure", #f); \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s failure", #f); \
      } else {								\
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s failure: %s", #f, errorstring); \
      }									\
    }                                                                   \
  } while (0)

#define LUAL_CHECKVERSION(marpaESLIFValuep) do {                        \
    if (luaunpanicL_checkversion(marpaESLIFValuep->L)) {                \
      LOG_PANIC_STRING(marpaESLIFValuep, luaL_checkversion);            \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_OPENLIBS(marpaESLIFValuep) do {                           \
    if (luaunpanicL_openlibs(marpaESLIFValuep->L)) {                   \
      LOG_PANIC_STRING(marpaESLIFValuep, luaL_openlibs);               \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_REGISTER(marpaESLIFValuep, n ,f) do {               \
    if (luaunpanic_register(marpaESLIFValuep->L, n, f)) {       \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_register);         \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_PUSHNIL(marpaESLIFValuep) do {                             \
    if (luaunpanic_pushnil(marpaESLIFValuep->L)) {                     \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushnil);                 \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHLSTRING(marpaESLIFValuep, s, l) do {                    \
    if (luaunpanic_pushlstring(NULL, marpaESLIFValuep->L, s, l)) {      \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushlstring);              \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, p) do {                \
    if (luaunpanic_pushlightuserdata(marpaESLIFValuep->L, p)) {        \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushlightuserdata);       \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHINTEGER(marpaESLIFValuep, i) do {                      \
    if (luaunpanic_pushinteger(marpaESLIFValuep->L, i)) {              \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushinteger);             \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHNUMBER(marpaESLIFValuep, x) do {                       \
    if (luaunpanic_pushnumber(marpaESLIFValuep->L, x)) {               \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushnumber);              \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHBOOLEAN(marpaESLIFValuep, b) do {                      \
    if (luaunpanic_pushboolean(marpaESLIFValuep->L, b)) {              \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pushboolean);             \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_GETGLOBAL(marpaESLIFValuep, name) do {                      \
    if (luaunpanic_getglobal(NULL, marpaESLIFValuep->L, name)) {        \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_getglobal);                \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_LOADBUFFER(marpaESLIFValuep, s, sz, n) do {                \
    int _rci = -1;                                                      \
    if (luaunpanicL_loadbuffer(&_rci, marpaESLIFValuep->L, s, sz, n)) { \
      LOG_PANIC_STRING(marpaESLIFValuep, luaL_loadbuffer);              \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (_rci != 0) {                                                    \
      LOG_ERROR_STRING(marpaESLIFValuep, luaL_loadbuffer);              \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_CALL(marpaESLIFValuep, n, r) do {                           \
    int _rci;                                                           \
    if (luaunpanic_call(marpaESLIFValuep->L, n, r)) {                   \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_call);                     \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETTOP(marpaESLIFValuep, idx) do {                       \
    if (luaunpanic_settop(marpaESLIFValuep->L, idx)) {               \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_SETTOP);                \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TYPE(marpaESLIFValuep, rcp, idx) do {                    \
    if (luaunpanic_type(rcp, marpaESLIFValuep->L, idx)) {            \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_type);                  \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TOBOOLEAN(marpaESLIFValuep, rcp, idx) do {               \
    if (luaunpanic_toboolean(rcp, marpaESLIFValuep->L, idx)) {       \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_toboolean);             \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TONUMBER(marpaESLIFValuep, rcp, idx) do {                   \
    int isnum;                                                          \
    if (luaunpanic_tonumberx(rcp, marpaESLIFValuep->L, idx, &isnum)) {  \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_tonumberx);                \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (! isnum) {                                                      \
      MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "lua_tonumberx failure"); \
    }                                                                   \
  } while (0)

#define LUA_TOLSTRING(marpaESLIFValuep, rcpp, idx, lenp) do {            \
    if (luaunpanic_tolstring(rcpp, marpaESLIFValuep->L, idx, lenp)) {   \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_tolstring);                \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_TOPOINTER(marpaESLIFValuep, rcpp, idx) do {              \
    if (luaunpanic_topointer(rcpp, marpaESLIFValuep->L, idx)) {      \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_topointer);             \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
  } while (0)

#define LUA_TOUSERDATA(marpaESLIFValuep, rcpp, idx) do {              \
    if (luaunpanic_touserdata(rcpp, marpaESLIFValuep->L, idx)) {      \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_touserdata);             \
      errno = ENOSYS;                                                 \
      goto err;                                                       \
    }                                                                 \
  } while (0)

#define LUA_POP(marpaESLIFValuep, n) do {                       \
    if (luaunpanic_pop(marpaESLIFValuep->L, n)) {               \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pop);              \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_PCALL(marpaESLIFValuep, n, r, f) do {                       \
    int _rci;                                                           \
    if (luaunpanic_pcall(&_rci, marpaESLIFValuep->L, n, r, f)) {        \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_pcall);                    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
    if (_rci != 0) {                                                    \
      LOG_ERROR_STRING(marpaESLIFValuep, lua_pcall);                    \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)


/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_undef(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  
  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_UNDEF;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_char(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  const char             *stringp;
  size_t                  len = 0;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TSTRING);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  stringp          = lua_tolstring(L, 3, &len);
  if (stringp == NULL) {
    return luaL_error(L, "marpaESLIFValue_stack_set_char second argument must be a string, first character will then be taken");
  } else if (len != 1) {
    return luaL_error(L, "marpaESLIFValue_stack_set_char second argument must be a string of length equal to one byte, while we found %ld bytes", (unsigned long) len);
  }

  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_CHAR;
  marpaESLIFValueResult.u.c = *stringp;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_short(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  int                     shorti;
  short                   shortb;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  shorti           = lua_tointegerx(L, 3, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert third argument to an integer");
  }
#ifdef SHRT_MIN
  if (shorti < SHRT_MIN) {
    return luaL_error(L, "Third argument must not be < %d", (int) SHRT_MIN);
  }
#endif
#ifdef SHRT_MAX
  if (shorti > SHRT_MAX) {
    return luaL_error(L, "Third argument must not be > %d", (int) SHRT_MAX);
  }
#endif
  shortb = (short) shorti;
  
  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_SHORT;
  marpaESLIFValueResult.u.b = shortb;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_int(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  int                     integeri;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  integeri         = lua_tointegerx(L, 3, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert third argument to an integer");
  }
#ifdef INT_MIN
  if (integeri < INT_MIN) {
    return luaL_error(L, "Third argument must not be < %d", (int) INT_MIN);
  }
#endif
#ifdef INT_MAX
  if (integeri > INT_MAX) {
    return luaL_error(L, "Third argument must not be > %d", (int) INT_MAX);
  }
#endif
  
  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_INT;
  marpaESLIFValueResult.u.i = integeri;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_long(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  lua_Number              number;
  long                    longl;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  number           = lua_tonumberx(L, 3, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert third argument to an number");
  }
#ifdef LONG_MIN
  if (number < LONG_MIN) {
    return luaL_error(L, "Third argument must not be < %ld", (long) LONG_MIN);
  }
#endif
#ifdef LONG_MAX
  if (number > LONG_MAX) {
    return luaL_error(L, "Third argument must not be > %ld", (long) LONG_MAX);
  }
#endif
  longl = (long) number;
  
  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_LONG;
  marpaESLIFValueResult.u.l = longl;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_double(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  double                  doubled; /* Note that lua_Number == double in our embeded lua */
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TNUMBER);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  doubled          = lua_tonumberx(L, 3, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert third argument to a double");
  }
  
  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_DOUBLE;
  marpaESLIFValueResult.u.d = doubled;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_ptr(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  void                   *p;
  int                     isnum = 0;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TLIGHTUSERDATA);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  p                = (marpaESLIFValue_t *) lua_touserdata(L, 3);
  
  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = 0;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFValueResult.u.p = p;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static int lua_marpaESLIFValue_stack_set_array(lua_State *L)
/*****************************************************************************/
{
  marpaESLIFValue_t      *marpaESLIFValuep;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     indicei;
  const char             *stringp;
  size_t                  len = 0;
  int                     isnum = 0;
  void                   *bytep;
  size_t                  bytel;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, 2, LUA_TNUMBER);
  luaL_checktype(L, 3, LUA_TSTRING);

  marpaESLIFValuep = (marpaESLIFValue_t *) lua_touserdata(L, 1);

  indicei          = lua_tointegerx(L, 2, &isnum);
  if (! isnum) {
    return luaL_error(L, "Cannot convert second argument to an integer");
  }

  stringp          = lua_tolstring(L, 3, &len);
  if (stringp == NULL) {
    return luaL_error(L, "marpaESLIFValue_stack_set_array second argument must be a string");
  } else if (len <= 0) {
    return luaL_error(L, "marpaESLIFValue_stack_set_char second argument must be a string of length > 0");
  }

  /* Duplicate lua's string */
  bytep = malloc(len);
  if (bytep == NULL) {
    return luaL_error(L, "malloc failure, %s", strerror(errno));
  }
  memcpy(bytep, stringp, len);
  bytel = len;

  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = bytel;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_ARRAY;
  marpaESLIFValueResult.u.p = bytep;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static short _marpaESLIFValue_lua_newb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  marpaESLIFGrammar_t    *marpaESLIFGrammarp;
  int                     i;
  short                   rcb;
  const static luaL_Reg   lua_lib[] = {
    { "marpaESLIFValue_stack_set_undef",  lua_marpaESLIFValue_stack_set_undef },
    { "marpaESLIFValue_stack_set_char",   lua_marpaESLIFValue_stack_set_char },
    { "marpaESLIFValue_stack_set_short",  lua_marpaESLIFValue_stack_set_short },
    { "marpaESLIFValue_stack_set_int",    lua_marpaESLIFValue_stack_set_int },
    { "marpaESLIFValue_stack_set_long",   lua_marpaESLIFValue_stack_set_long },
    { "marpaESLIFValue_stack_set_double", lua_marpaESLIFValue_stack_set_double },
    { "marpaESLIFValue_stack_set_ptr",    lua_marpaESLIFValue_stack_set_ptr },
    { "marpaESLIFValue_stack_set_array",  lua_marpaESLIFValue_stack_set_array }
  };

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

  /* Register all stack functions into the main namespace */
  for (i = 0; i < sizeof(lua_lib) / sizeof(lua_lib[0]); i++) {
    LUA_REGISTER(marpaESLIFValuep, lua_lib[i].name, lua_lib[i].func);
  }

  /* Execute lua scripts present in the grammar */
  if ((marpaESLIFGrammarp->luabytep != NULL) && (marpaESLIFGrammarp->luabytel > 0)) {
    LUAL_LOADBUFFER(marpaESLIFValuep, marpaESLIFGrammarp->luabytep, marpaESLIFGrammarp->luabytel, "=(marpaESLIFGrammar)");
    LUA_PCALL(marpaESLIFValuep, 0, LUA_MULTRET, 0);
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
static short _marpaESLIFValue_lua_callb(marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  int   i;
  int   rci;
  short rcb;
  int   nargi;

  LUA_GETGLOBAL(marpaESLIFValuep, marpaESLIFValuep->actions);

  if (! nullableb) {
    nargi = argni - arg0i + 1;
    for (i = arg0i; i <= argni; i++) {
      if (! lua_marpaESLIFValue_push_arg(marpaESLIFValuep, i)) {
        goto err;
      }
    }
  } else {
    nargi = 0;
  }

  /* Lua will make sure there is a room for at least one argument on the stack at return */
  LUA_CALL(marpaESLIFValuep, nargi, 1);

  if (! lua_marpaESLIFValue_pop_arg(marpaESLIFValuep, resulti)) {
    goto err;
  }

  /* Clear the stack */
  LUA_SETTOP(marpaESLIFValuep, 0);
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short lua_marpaESLIFValue_push_arg(marpaESLIFValue_t *marpaESLIFValuep, int i)
/*****************************************************************************/
{
  short                   rcb;
  marpaESLIFValueResult_t marpaESLIFValueResult;

  if (! _marpaESLIFValue_stack_getb(marpaESLIFValuep, i, &marpaESLIFValueResult)) {
    goto err;
  }

  switch (marpaESLIFValueResult.type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    LUA_PUSHNIL(marpaESLIFValuep);
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    LUA_PUSHLSTRING(marpaESLIFValuep, &marpaESLIFValueResult.u.c, 1);
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) marpaESLIFValueResult.u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) marpaESLIFValueResult.u.i);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    LUA_PUSHINTEGER(marpaESLIFValuep, (lua_Integer) marpaESLIFValueResult.u.l);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) marpaESLIFValueResult.u.f);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    LUA_PUSHNUMBER(marpaESLIFValuep, (lua_Number) marpaESLIFValueResult.u.d);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, marpaESLIFValueResult.u.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    LUA_PUSHLSTRING(marpaESLIFValuep, marpaESLIFValueResult.u.p, marpaESLIFValueResult.sizel);
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unsupported value type %d", marpaESLIFValueResult.type);
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short lua_marpaESLIFValue_pop_arg(marpaESLIFValue_t *marpaESLIFValuep, int resulti)
/*****************************************************************************/
{
  short                   rcb;
  int                     type;
  marpaESLIFValueResult_t marpaESLIFValueResult;
  int                     rci;
  const void             *rcp;
  const char             *bytep;
  size_t                  sizel;
  lua_Number              rcl;
  char                   *p;

  LUA_TYPE(marpaESLIFValuep, &type, -1);

  switch (type) {
  case LUA_TNONE:
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
    break;
  case LUA_TNIL:
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
    break;
  case LUA_TBOOLEAN:
    LUA_TOBOOLEAN(marpaESLIFValuep, &rci, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_SHORT;
    marpaESLIFValueResult.u.b             = (rci != 0) ? 1 : 0;
    break;
  case LUA_TLIGHTUSERDATA:
    LUA_TOUSERDATA(marpaESLIFValuep, (void *) &rcp, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) rcp;
    break;
  case LUA_TNUMBER:
    LUA_TONUMBER(marpaESLIFValuep, &rcl, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG;
    marpaESLIFValueResult.u.l             = rcl;
    break;
  case LUA_TSTRING:
    LUA_TOLSTRING(marpaESLIFValuep, &bytep, -1, &sizel);
    if ((bytep == NULL) || (sizel <= 0)) {
      /* Empty string, equivalent to undef for us */
      marpaESLIFValueResult.contexti        = 0;
      marpaESLIFValueResult.sizel           = 0;
      marpaESLIFValueResult.representationp = NULL;
      marpaESLIFValueResult.shallowb        = 0;
      marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
    } else {
      /* Duplicate the memory */
      p = (char *) malloc(sizel + 1); /* Hiden NUL byte */
      if (p == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(p, bytep, sizel);
      p[sizel] = '\0';
      marpaESLIFValueResult.contexti        = 0;
      marpaESLIFValueResult.sizel           = 0;
      marpaESLIFValueResult.representationp = NULL;
      marpaESLIFValueResult.shallowb        = 0;
      marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.p             = p;
      marpaESLIFValueResult.sizel           = sizel;
    }
  break;
  case LUA_TTABLE:
    LUA_TOPOINTER(marpaESLIFValuep, &rcp, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) rcp;
    break;
  case LUA_TFUNCTION:
    LUA_TOPOINTER(marpaESLIFValuep, &rcp, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) rcp;
    break;
  case LUA_TUSERDATA:
    LUA_TOPOINTER(marpaESLIFValuep, &rcp, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) rcp;
    break;
  case LUA_TTHREAD:
    LUA_TOPOINTER(marpaESLIFValuep, &rcp, -1);
    marpaESLIFValueResult.contexti        = 0;
    marpaESLIFValueResult.sizel           = 0;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.shallowb        = 0;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR;
    marpaESLIFValueResult.u.p             = (void *) rcp;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unsupported lua type %d", type);
    goto err;
  }
  
  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult); /* allow context == 0 */
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
