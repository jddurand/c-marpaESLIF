#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L;
  char *errstring;

  if (luaunpanicL_newstate(&L)) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_newstate failure");
    errno = EINVAL;
  }
  if (L == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "luaunpanicL_success but lua_State is NULL");
    errno = EINVAL;
  }

  return L;
}
