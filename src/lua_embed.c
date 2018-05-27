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

static int lua_marpaESLIF_versions(lua_State *L);
static int lua_marpaESLIF_grammarp(lua_State *L);
static int lua_marpaESLIFGrammar_eslifp(lua_State *L);
static int lua_marpaESLIFValue_stack_set_undef(lua_State *L);
static int lua_marpaESLIFValue_stack_set_char(lua_State *L);
static int lua_marpaESLIFValue_stack_set_short(lua_State *L);
static int lua_marpaESLIFValue_stack_set_int(lua_State *L);
static int lua_marpaESLIFValue_stack_set_long(lua_State *L);
static int lua_marpaESLIFValue_stack_set_double(lua_State *L);
static int lua_marpaESLIFValue_stack_set_ptr(lua_State *L);
static int lua_marpaESLIFValue_stack_set_array(lua_State *L);

#define LOG_PANIC_STRING(marpaESLIFGrammarp, L, f) do {                 \
    char *panicstring;							\
    if (luaunpanic_panicstring(&panicstring, L)) {                      \
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "%s panic\n", #f); \
    } else {								\
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "%s panic: %s\n", #f, panicstring); \
    }									\
  } while (0)

#define LOG_ERROR_STRING(marpaESLIFGrammarp, L, f) do {                 \
    const char *errorstring;                                            \
    if (luaunpanic_tostring(&errorstring, L, -1)) {                     \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, luaunpanic_tostring);     \
      MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "%s failure\n", #f); \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "%s failure\n", #f); \
      } else {								\
        MARPAESLIF_ERRORF(marpaESLIFGrammarp->marpaESLIFp, "%s failure: %s\n", #f, errorstring); \
      }									\
    }                                                                   \
  } while (0)

#define LUAL_CHECKVERSION(marpaESLIFGrammarp, L) do {                   \
    if (luaunpanicL_checkversion(L)) {                                  \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, luaL_checkversion);       \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_OPENLIBS(marpaESLIFGrammarp, L) do {                      \
    if (luaunpanicL_openlibs(L)) {                                     \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, luaL_openlibs);          \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_REGISTER(marpaESLIFGrammarp, L, n ,f) do {          \
    if (luaunpanic_register(L, n, f)) {                         \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_register);    \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_PUSHSTRING(marpaESLIFGrammarp, rcpp, L, s) do {     \
    if (luaunpanic_pushstring(rcpp, L, s)) {                    \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_pushstring);  \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_PUSHLIGHTUSERDATA(marpaESLIFGrammarp, L, p) do {           \
    if (luaunpanic_pushlightuserdata(L, p)) {                          \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_pushlightuserdata);  \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_PUSHINTEGER(marpaESLIFGrammarp, L, i) do {                 \
    if (luaunpanic_pushinteger(L, i)) {                                \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_pushinteger);        \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_RAWSET(marpaESLIFGrammarp, L, idx) do {                    \
    if (luaunpanic_rawset(L, idx)) {                                   \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_rawset);             \
      errno = ENOSYS;                                                  \
      goto err;                                                        \
    }                                                                  \
  } while (0)

#define LUA_NEWTABLE(marpaESLIFGrammarp, L) do {                \
    if (luaunpanic_newtable(L)) {                               \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_newtable);    \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUA_SETGLOBAL(marpaESLIFGrammarp, L, name) do {         \
    if (luaunpanic_setglobal(L, name)) {                        \
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_setglobal);   \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)


/*****************************************************************************/
static int lua_marpaESLIF_versions(lua_State *L)
/*****************************************************************************/
/* This function is executed inside lua, so no need of luaunpanic */
{
  const char *versions = marpaESLIF_versions();

  lua_pushlstring(L, versions, strlen(versions));
  return 1;
}

/*****************************************************************************/
static int lua_marpaESLIF_grammarp(lua_State *L)
/*****************************************************************************/
/* This function is executed inside lua, so no need of luaunpanic */
{
  marpaESLIF_t        *marpaESLIFp;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);

  marpaESLIFp        = (marpaESLIF_t *) lua_touserdata(L, 1);
  marpaESLIFGrammarp = marpaESLIF_grammarp(marpaESLIFp);

  lua_pushlightuserdata(L, (void *) marpaESLIFGrammarp);
  return 1;
}

/*****************************************************************************/
static int lua_marpaESLIFGrammar_eslifp(lua_State *L)
/*****************************************************************************/
/* This function is executed inside lua, so no need of luaunpanic */
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIF_t        *marpaESLIFp;

  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);

  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) lua_touserdata(L, 1);
  marpaESLIFp        = marpaESLIFGrammar_eslifp(marpaESLIFGrammarp);

  lua_pushlightuserdata(L, (void *) marpaESLIFp);
  return 1;
}

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

  marpaESLIFValueResult.contexti = 0;
  marpaESLIFValueResult.sizel = len;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.shallowb = 0;
  marpaESLIFValueResult.type = MARPAESLIF_VALUE_TYPE_ARRAY;
  marpaESLIFValueResult.u.p = (void *) stringp;

  if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &marpaESLIFValueResult)) {
    return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
  }

  return 0;
}

/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  lua_State             *L = NULL;
  int                    i;
  static const luaL_Reg  lua_lib[] = {
    { "marpaESLIF_versions",              lua_marpaESLIF_versions },
    { "marpaESLIF_grammarp",              lua_marpaESLIF_grammarp },
    { "marpaESLIFGrammar_eslifp",         lua_marpaESLIFGrammar_eslifp },
    { "marpaESLIFValue_stack_set_undef",  lua_marpaESLIFValue_stack_set_undef },
    { "marpaESLIFValue_stack_set_char",   lua_marpaESLIFValue_stack_set_char },
    { "marpaESLIFValue_stack_set_short",  lua_marpaESLIFValue_stack_set_short },
    { "marpaESLIFValue_stack_set_int",    lua_marpaESLIFValue_stack_set_int },
    { "marpaESLIFValue_stack_set_long",   lua_marpaESLIFValue_stack_set_long },
    { "marpaESLIFValue_stack_set_double", lua_marpaESLIFValue_stack_set_double },
    { "marpaESLIFValue_stack_set_ptr",    lua_marpaESLIFValue_stack_set_ptr },
    { "marpaESLIFValue_stack_set_array",  lua_marpaESLIFValue_stack_set_array }
  };


  /* Create Lua state */
  if (luaunpanicL_newstate(&L)) {
    MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanicL_newstate failure");
    errno = ENOSYS;
    goto err;
  }
  if (L == NULL) {
    MARPAESLIF_ERROR(marpaESLIFGrammarp->marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  /* Open all available libraries */
  LUAL_OPENLIBS(marpaESLIFGrammarp, L);

  /* Check Lua version */
  LUAL_CHECKVERSION(marpaESLIFGrammarp, L);

  /* Register all functions into the main namespace - we explicitely do not export */
  /* constructor and destructors. */
  for (i = 0; i < sizeof(lua_lib) / sizeof(lua_lib[0]); i++) {
    LUA_REGISTER(marpaESLIFGrammarp, L, lua_lib[i].name, lua_lib[i].func);
  }

  /* Inject the "global" variable marpaESLIF.marpaESLIFGrammarp */
  LUA_NEWTABLE(marpaESLIFGrammarp, L);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "marpaESLIFGrammarp");
  LUA_PUSHLIGHTUSERDATA(marpaESLIFGrammarp, L, marpaESLIFGrammarp);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_SETGLOBAL(marpaESLIFGrammarp, L, "marpaESLIF");

  /* Inject the "global" variables marpaESLIFValueType.[UNDEF, CHAR, etc...] */
  LUA_NEWTABLE(marpaESLIFGrammarp, L);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "UNDEF");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_UNDEF);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "CHAR");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_CHAR);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "SHORT");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_SHORT);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "INT");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_INT);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "FLOAT");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_FLOAT);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "DOUBLE");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_DOUBLE);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "PTR");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_PTR);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_PUSHSTRING(marpaESLIFGrammarp, NULL, L, "ARRAY");
  LUA_PUSHINTEGER(marpaESLIFGrammarp, L, MARPAESLIF_VALUE_TYPE_ARRAY);
  LUA_RAWSET(marpaESLIFGrammarp, L, -3);
  LUA_SETGLOBAL(marpaESLIFGrammarp, L, "marpaESLIFValueType");

#ifndef MARPAESLIF_LUA_NTEST
  {
    int rc;
    /* stack: none                  */
    if (luaunpanicL_dostring(&rc, L,
                             "print(\"marpaESLIF version returns \" .. marpaESLIF_versions())\n"
                             "print(\"marpaESLIFGrammar_eslifp returns \" .. tostring(marpaESLIFGrammar_eslifp(marpaESLIF.marpaESLIFGrammarp)))"
                             "print(\"marpaESLIFValueType.PTR returns \" .. marpaESLIFValueType.PTR)"
                             )) {
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, luaL_dostring);
    }
    if (rc) {
      LOG_ERROR_STRING(marpaESLIFGrammarp, L, luaL_dostring);
    }
  }
#endif

  goto done;

 err:
  if (L != NULL) {
    if (luaunpanic_close(L)) {
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, lua_close);
    }
    L = NULL;
  }

 done:
  return L;
}

/*****************************************************************************/
static void _marpaESLIF_lua_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, lua_State *L)
/*****************************************************************************/
{
  if (L != NULL) {
    if (luaunpanic_close(L)) {
      LOG_PANIC_STRING(marpaESLIFGrammarp, L, luaunpanic_close);
    }
  }
}

