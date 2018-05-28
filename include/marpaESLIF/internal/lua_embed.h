#ifndef MARPAESLIF_INTERNAL_LUA_EMBED_H
#define MARPAESLIF_INTERNAL_LUA_EMBED_H

#include <marpaESLIF.h>
#include <lua/luaunpanic.h>

static short _marpaESLIFValue_lua_newb(marpaESLIFValue_t *marpaESLIFValuep);
static void  _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep);
static short _marpaESLIFValue_lua_callb(marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);

#endif /* MARPAESLIF_INTERNAL_LUA_EMBED_H */

