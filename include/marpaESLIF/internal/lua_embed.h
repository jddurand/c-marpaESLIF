#ifndef MARPAESLIF_INTERNAL_LUA_EMBED_H
#define MARPAESLIF_INTERNAL_LUA_EMBED_H

#include <marpaESLIF.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int        _marpaESLIF_lua_atpanic(lua_State *lua);
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp);
static short      _marpaESLIF_lua_compileb(marpaESLIF_t *marpaESLIF, lua_State *L, const char *p, size_t l, char *names);

#endif /* MARPAESLIF_INTERNAL_LUA_EMBED_H */

