#include "marpaESLIF/internal/lua_embed.h"

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

/*****************************************************************************/
static lua_State *_marpaESLIF_luaL_newstate(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L;

  L = luaL_newstate();
  if (L == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "luaL_newstate failure, %s", strerror(errno));
  }

  return L;
}

