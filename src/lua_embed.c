#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

#define LOG_PANIC_STRING(marpaESLIFp, L, f) do {                        \
    char *panicstring;							\
    if (luaunpanic_panicstring(&panicstring, L)) {                      \
      MARPAESLIF_ERRORF(marpaESLIFp, "%s panic\n", #f);                 \
    } else {								\
      MARPAESLIF_ERRORF(marpaESLIFp, "%s panic: %s\n", #f, panicstring); \
    }									\
  } while (0)

#define LOG_ERROR_STRING(marpaESLIFp, L, f) do {                        \
    const char *errorstring;                                            \
    if (luaunpanic_tostring(&errorstring, L, -1)) {                     \
      LOG_PANIC_STRING(marpaESLIFp, L, luaunpanic_tostring);            \
      MARPAESLIF_ERRORF(marpaESLIFp, "%s failure\n", #f);               \
    } else {                                                            \
      if (errorstring == NULL) {                                        \
        MARPAESLIF_ERRORF(marpaESLIFp, "%s failure\n", #f);             \
      } else {								\
        MARPAESLIF_ERRORF(marpaESLIFp, "%s failure: %s\n", #f, errorstring); \
      }									\
    }                                                                   \
  } while (0)

static int l_marpaESLIF_versions(lua_State *L) {
  const char *versions = marpaESLIF_versions();

  lua_pushlstring(L, versions, strlen(versions));
  return 1;
}

static const struct luaL_Reg marpaESLIF_lualib [] = {
  {"versions", l_marpaESLIF_versions},
  {NULL, NULL}  /* sentinel */
};

#define LUAL_CHECKVERSION(L) do {                               \
    if (luaunpanicL_checkversion(L)) {                          \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_checkversion);      \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUAL_OPENLIBS(L) do {                                   \
    if (luaunpanicL_openlibs(L)) {                              \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_openlibs);          \
      errno = ENOSYS;                                           \
      goto err;                                                 \
    }                                                           \
  } while (0)

#define LUAL_NEWMETATABLE(L, tablename) do {                            \
    if (luaunpanicL_newmetatable(NULL, L, tablename)) {                 \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_newmetatable);              \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHSTRING(L, string) do {                                  \
    if (luaunpanic_pushstring(NULL, L, string)) {                       \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_pushstring);                 \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHVALUE(L, index) do {                                    \
    if (luaunpanic_pushvalue(L, index)) {                               \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_pushvalue);                  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETTABLE(L, index) do {                                     \
    if (luaunpanic_settable(L, index)) {                                \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_settable);                   \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_SETFUNCS(L, l, nup) do {                                   \
    if (luaunpanicL_setfuncs(L, l, nup)) {                              \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_setfuncs);                  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_SETGLOBAL(L, name) do {                                     \
    if (luaunpanic_setglobal(L, name)) {                                \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_setfuncs);                  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)
    
#define LUA_POP(L, numberofelements) do {                               \
    if (luaunpanic_pop(L, numberofelements)) {                          \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_pop);                        \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUA_PUSHCFUNCTION(L, f) do {                                    \
    if (luaunpanic_pushcfunction(L, f)) {                               \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_pushcfunction);              \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0);
    
/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L;

  if (luaunpanicL_newstate(&L)) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_newstate failure");
    errno = ENOSYS;
    goto err;
  }
  if (L == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
    errno = ENOSYS;
    goto err;
  }

  /* Register our functions */
  LUAL_OPENLIBS(L);
  LUA_PUSHCFUNCTION(L, l_marpaESLIF_versions);
  LUA_SETGLOBAL(L, "marpaESLIF_versions");

#ifndef MARPAESLIF_LUA_TEST
  {
    int rc;
    if (luaunpanicL_dostring(&rc, L, "print(\"marpaESLIF version is \" .. marpaESLIF_versions(1, 2))")) {
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_dostring);
    }
    if (rc) {
      LOG_ERROR_STRING(marpaESLIFp, L, luaL_dostring);
    }
  }
#endif

  goto done;

 err:
  if (L != NULL) {
    if (luaunpanic_close(L)) {
      LOG_PANIC_STRING(marpaESLIFp, L, lua_close);
    }
    L = NULL;
  }

 done:
  return L;
}

/*****************************************************************************/
static void _marpaESLIF_lua_freev(marpaESLIF_t *marpaESLIFp, lua_State *L)
/*****************************************************************************/
{
  if (L != NULL) {
    if (luaunpanic_close(L)) {
      LOG_PANIC_STRING(marpaESLIFp, L, luaunpanic_close);
    }
  }
}

