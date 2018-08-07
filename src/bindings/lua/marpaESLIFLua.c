/* Only 5.3 and upwards is supported */

#define LUA_LIB /* this sets LUA_API correctly */

#include "lua.h"        /* As per CMake doc */
#include "lauxlib.h"    /* As per CMake doc */
#include "lualib.h"     /* As per CMake doc */
#include <marpaESLIFLua.h>
#include <marpaESLIF.h>

static int installmarpaESLIFLua(lua_State *L);
static int marpaESLIFLua_open(lua_State *L);

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
      {"open", marpaESLIFLua_open},
      {NULL, NULL}
   };
   luaL_newlib(L, marpaESLIFLuaTable);

   return 1;
}

/****************************************************************************/
static int marpaESLIFLua_open(lua_State *L)
/****************************************************************************/
{
  return 0;
}
