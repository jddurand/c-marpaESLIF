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
  char      *panicstring;

  if (luaunpanicL_newstate(&L)) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_newstate failure");
    errno = EINVAL;
    goto err;
  }
  if (L == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
    errno = EINVAL;
    goto err;
  }

  /* Register our functions */
  if (luaunpanicL_openlib(L, "marpaESLIF", marpaESLIF_lualib, 0)) {
    LOG_PANIC_STRING(marpaESLIFp, L, luaunpanicL_openlib);
    errno = EINVAL;
    goto err;
  }

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
