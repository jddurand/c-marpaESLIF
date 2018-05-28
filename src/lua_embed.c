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

static int lua_marpaESLIFValue_stack_set_undef(lua_State *L);
static int lua_marpaESLIFValue_stack_set_char(lua_State *L);
static int lua_marpaESLIFValue_stack_set_short(lua_State *L);
static int lua_marpaESLIFValue_stack_set_int(lua_State *L);
static int lua_marpaESLIFValue_stack_set_long(lua_State *L);
static int lua_marpaESLIFValue_stack_set_double(lua_State *L);
static int lua_marpaESLIFValue_stack_set_ptr(lua_State *L);
static int lua_marpaESLIFValue_stack_set_array(lua_State *L);

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

#define LUA_TOBOOLEAN(marpaESLIFValuep, rcp, idx) do {               \
    if (luaunpanic_toboolean(rcp, marpaESLIFValuep->L, idx)) {       \
      LOG_PANIC_STRING(marpaESLIFValuep, lua_toboolean);             \
      errno = ENOSYS;                                                \
      goto err;                                                      \
    }                                                                \
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
  int   rci;
  short rcb;

  LUA_GETGLOBAL(marpaESLIFValuep, marpaESLIFValuep->actions);

  LUA_PUSHLIGHTUSERDATA(marpaESLIFValuep, marpaESLIFValuep);
  LUA_PUSHINTEGER(marpaESLIFValuep, arg0i);
  LUA_PUSHINTEGER(marpaESLIFValuep, argni);
  LUA_PUSHINTEGER(marpaESLIFValuep, resulti);
  LUA_PUSHBOOLEAN(marpaESLIFValuep, nullableb);

  LUA_CALL(marpaESLIFValuep, 5, 1);

  LUA_TOBOOLEAN(marpaESLIFValuep, &rci, -1);
  LUA_POP(marpaESLIFValuep, -1);

  rcb = (rci != 0) ? 1 : 0;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
