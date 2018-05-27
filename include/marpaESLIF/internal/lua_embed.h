#ifndef MARPAESLIF_INTERNAL_LUA_EMBED_H
#define MARPAESLIF_INTERNAL_LUA_EMBED_H

#include <marpaESLIF.h>
#include <lua/luaunpanic.h>

static lua_State *_marpaESLIF_lua_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
static void       _marpaESLIF_lua_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, lua_State *L);

#endif /* MARPAESLIF_INTERNAL_LUA_EMBED_H */

