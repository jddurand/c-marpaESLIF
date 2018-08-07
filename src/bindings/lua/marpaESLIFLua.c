/* Only 5.3 and upwards is supported */

#define LUA_LIB /* this sets LUA_API correctly */

#include "lua.h"        /* As per CMake doc */
#include "lauxlib.h"    /* As per CMake doc */
#include "lualib.h"     /* As per CMake doc */
#include <marpaESLIFLua.h>
#include <marpaESLIF.h>

static int installmarpaESLIFLua(lua_State *L);
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L);

/****************************************************************************/
int luaopen_marpaESLIFLua(lua_State* L)
/****************************************************************************/
{
  luaL_requiref(L, "marpaESLIFLua", installmarpaESLIFLua, 1 /* global */);
  return 1;
}

/****************************************************************************/
static int installmarpaESLIFLua(lua_State *L)
/****************************************************************************/
{
   static const luaL_Reg marpaESLIFLuaTable[] = {
      {"versions", marpaESLIFLua_marpaESLIF_versions},
      {NULL, NULL}
   };
   luaL_newlib(L, marpaESLIFLuaTable);

   return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L)
/****************************************************************************/
{
  lua_pushstring(L, marpaESLIF_versions());
  return 1;
}
