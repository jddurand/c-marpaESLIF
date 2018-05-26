#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

static int lua_marpaESLIF_versions(lua_State *L);
static int luaL_requiref_marpaESLIF(lua_State *L);

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

#define LUA_POP(L, numberofelements) do {                               \
    if (luaunpanic_pop(L, numberofelements)) {                          \
      LOG_PANIC_STRING(marpaESLIFp, L, lua_pop);                        \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define LUAL_REQUIREF(L, modname, openf, glb) do {                      \
    if (luaunpanicL_requiref(L, modname, openf, glb)) {                 \
      LOG_PANIC_STRING(marpaESLIFp, L, luaL_requiref);                  \
      errno = ENOSYS;                                                   \
      goto err;                                                         \
    }                                                                   \
  } while (0)


/*****************************************************************************/
static int lua_marpaESLIF_versions(lua_State *L)
/*****************************************************************************/
{
  const char *versions = marpaESLIF_versions();

  lua_pushlstring(L, versions, strlen(versions));
  return 1;
}

/*****************************************************************************/
static int luaL_requiref_marpaESLIF(lua_State *L)
/*****************************************************************************/
{
  static const luaL_Reg lua_marpaESLIF_lib[] = {
    { "versions", &lua_marpaESLIF_versions },
    { NULL, NULL }
  };
	
  /* stack: 0 | "marpaESLIF" | -1 */
  luaL_newlib(L, lua_marpaESLIF_lib);

  /* stack: 0 | "marpaESLIF" | -1 */
  luaL_newmetatable(L, "marpaESLIF");

  /* stack: 1 | meta         | -1 */
  /* stack: 0 | "marpaESLIF" | -2 */
  lua_pop(L, 1);

  /* stack: 0 | "marpaESLIF" | -1 */
  return 1;
}

/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L = NULL;
  int        i;

  /* Create Lua state */
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

  /* Open all available libraries */
  /* stack: none                  */
  LUAL_OPENLIBS(L);

  /* Check Lua version */
  /* stack: none                  */
  LUAL_CHECKVERSION(L);

  /* Register marpaESLIF functions */
  /* stack: none                  */
  LUAL_REQUIREF(L, "marpaESLIF", &luaL_requiref_marpaESLIF, 1);

  /* stack: 0 | "marpaESLIF" | -1 */
  LUA_POP(L, 1);

#ifndef MARPAESLIF_LUA_TEST
  {
    int rc;
    /* stack: none                  */
    if (luaunpanicL_dostring(&rc, L, "print(\"marpaESLIF version is \" .. marpaESLIF.versions())")) {
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

