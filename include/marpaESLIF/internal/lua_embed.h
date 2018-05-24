#ifndef MARPAESLIF_INTERNAL_LUA_EMBED_H
#define MARPAESLIF_INTERNAL_LUA_EMBED_H

#include <marpaESLIF.h>
#include <lua/luaunpanic.h>

static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp);
static void       _marpaESLIF_lua_freev(marpaESLIF_t *marpaESLIFp, lua_State *L);

#endif /* MARPAESLIF_INTERNAL_LUA_EMBED_H */

