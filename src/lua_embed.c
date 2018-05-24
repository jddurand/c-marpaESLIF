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
  {"marpaESLIF_versions", l_marpaESLIF_versions},
  {NULL, NULL}  /* sentinel */
};

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
  if (luaunpanicL_checkversion_(L, LUA_VERSION_NUM, LUAL_NUMSIZES)) {
    LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_checkversion);
    errno = ENOSYS;
    goto err;
  }
  if (luaunpanicL_checkversion(L)) {
    LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_checkversion);
    errno = ENOSYS;
    goto err;
  }
  if (luaunpanicL_newlibtable(L, marpaESLIF_lualib)) {
    LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_newlibtable);
    errno = ENOSYS;
    goto err;
  }
  if (luaunpanicL_setfuncs(L, marpaESLIF_lualib, 0)) {
    LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_setfuncs);
    errno = ENOSYS;
    goto err;
  }

#ifdef MARPAESLIF_LUA_TEST
  {
    int rc;
    if (luaunpanicL_dostring(&rc, L, "print(mysin(2))")) {
      LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_dostring);
    }
    if (rc) {
      LOG_ERROR_STRING(marpaESLIFp, L, luaunpanicL_dostring);
    }
  }
#endif

  goto done;

 err:
  if (L != NULL) {
    if (luaunpanic_close(L)) {
      LOG_PANIC_STRING(marpaESLIFp, L, luaunpanic_close);
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

