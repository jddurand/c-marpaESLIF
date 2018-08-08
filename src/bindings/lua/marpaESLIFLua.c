/* Only 5.3 and upwards is supported */

#define LUA_LIB /* this sets LUA_API correctly */

#include "lua.h"        /* As per CMake doc */
#include "lauxlib.h"    /* As per CMake doc */
#include "lualib.h"     /* As per CMake doc */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIFLua.h>
#include <marpaESLIF.h>

/* To ease me debugging -; */
static void stackdump_g(lua_State* L);

/* Global table for the multiton pattern */
#define MARPAESLIF_GLOBAL_TABLE_NAME "__marpaESLIFGlobal"
/* Every key   is a marpaESLIFp light userdata */
/* Every value is a table hosting loggerp pointer, or the false boolean */

/* Logger proxy: context and callback function */
typedef struct genericLoggerContext {
  lua_State  *L;
  int         r;
} genericLoggerContext_t;
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

static int installmarpaESLIFLua(lua_State *L);
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L);
static int marpaESLIFLua_marpaESLIF_newp(lua_State *L);

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
      {"marpaESLIF_versions", marpaESLIFLua_marpaESLIF_versions},
      {"marpaESLIF_newp", marpaESLIFLua_marpaESLIF_newp},
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

/****************************************************************************/
static int marpaESLIFLua_marpaESLIF_newp(lua_State *L)
/****************************************************************************/
{
  const void             *loggerp;
  marpaESLIF_t           *marpaESLIFp = NULL;
  genericLoggerContext_t *genericLoggerContextp;
  genericLogger_t        *genericLoggerp;
  marpaESLIFOption_t      marpaESLIFOption;

  static const char *loggerFunctions[] = {
    "trace",
    "debug",
    "info",
    "notice",
    "warning",
    "error",
    "critical",
    "emergency"
  };
  int i;

  switch (lua_gettop(L)) {
  case 0:
    loggerp = NULL;
    break;
  case 1:
    /* Verify that the logger can do all wanted methods */
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "Usage: logger must be a table");
    }
    for (i = 0; i < sizeof(loggerFunctions)/sizeof(loggerFunctions[0]); i++) {
      if (lua_getfield(L, -1, loggerFunctions[i]) != LUA_TFUNCTION) {
        return luaL_error(L, "Usage: logger table must have a field named '%s' that is a function", loggerFunctions[i]);
      } else {
        lua_pop(L, 1);
      }
    }
    loggerp = lua_topointer(L, 1);
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIF_newp([logger]");
  }

  /* Prepare MARPAESLIF_GLOBAL_TABLE_NAME as global table */
  if (lua_getglobal(L, MARPAESLIF_GLOBAL_TABLE_NAME) != LUA_TTABLE) { /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME? */
    lua_pop(L, 1);                                                    /* stack:  loggerp */
    lua_newtable(L);                                                  /* stack:  loggerp, {} */
    lua_setglobal(L, MARPAESLIF_GLOBAL_TABLE_NAME);                   /* stack:  loggerp */
    lua_getglobal(L, MARPAESLIF_GLOBAL_TABLE_NAME);                   /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME */
  }

  /* Look if MARPAESLIF_GLOBAL_TABLE_NAME already contains a reference to loggerp */
  lua_pushnil(L);                                                     /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp, loggerp? */
    if (((loggerp == NULL) && lua_isboolean(L, -1))
        ||
        ((loggerp != NULL) && (lua_topointer(L, -1) == loggerp))) {
      marpaESLIFp = (marpaESLIF_t *) lua_topointer(L, -2);
      lua_pop(L, 1);                                                  /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, key */
      break;
    } else {
      lua_pop(L, 1);                                                  /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, key */
    }
  }                                                                   /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME */

  if (marpaESLIFp == NULL) {
    if (loggerp != NULL) {
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        return luaL_error(L, "malloc failure, %s\n", strerror(errno));
      }
      lua_pushnil(L);                                                 /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, nil */
      lua_copy(L, 1, -1);                                             /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, loggerp */
      genericLoggerContextp->L = L;
      genericLoggerContextp->r = luaL_ref(L, LUA_REGISTRYINDEX);      /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME */
      genericLoggerp = genericLogger_newp(genericLoggerCallback, genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        return luaL_error(L, "genericLogger_newp failure, %s\n", strerror(errno));
      }
    } else {
      genericLoggerp = NULL;
    }
    marpaESLIFOption.genericLoggerp = genericLoggerp;
    marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
    if (marpaESLIFp == NULL) {
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    lua_pushboolean(L, 0);                                            /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, false */
    if (loggerp != NULL) {
      lua_copy(L, 1, -1);                                             /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, loggerp */
    }
    lua_rawsetp(L, -2, (const void *) marpaESLIFp);                   /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME */
    lua_pop(L, 1);                                                    /* stack: [loggerp ] */
  }

  lua_pushlightuserdata(L, marpaESLIFp);
  return 1;
}

/****************************************************************************/
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  genericLoggerContext_t *genericLoggerContext = (genericLoggerContext_t *) userDatavp;
  lua_State              *L                    = genericLoggerContext->L;
  int                     r                    = genericLoggerContext->r;
  const char             *funcs;
  int                     n;

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    funcs = "trace";
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    funcs = "debug";
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    funcs = "info";
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    funcs = "notice";
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    funcs = "warning";
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
    funcs = "error";
    break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
    funcs = "critical";
    break;
  case GENERICLOGGER_LOGLEVEL_ALERT:
    funcs = "alert";
    break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    funcs = "emergency";
    break;
  default:
    funcs = NULL;
    break;
  }

  if (funcs != NULL) {
    n = lua_gettop(L);                         /* stack: ?? */
    lua_rawgeti(L, LUA_REGISTRYINDEX, r);      /* stack: ??, logger */
    lua_getfield(L, -1, funcs);                /* stack: ??, logger, logger[funcs] */
    lua_pushstring(L, msgs);                   /* stack: ??, logger, logger[funcs], msgs */
    lua_call(L, 1, LUA_MULTRET);               /* stack: ??, logger, logger[funcs], ?? */
    lua_settop(L, n);                          /* stack: ?? */
  }
}

/****************************************************************************/
static void stackdump_g(lua_State* L)  
/****************************************************************************/
/* Reference: https://groups.google.com/forum/#!topic/lua5/gc3Ghjo6ipg      */
/****************************************************************************/
{
  int i;
  int top = lua_gettop(L);

  printf("total in stack %d\n",top);
  
  for (i = 1; i <= top; i++) {
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING:  /* strings */
      printf("  string: '%s'\n", lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN:  /* booleans */
      printf("  boolean %s\n",lua_toboolean(L, i) ? "true" : "false");
      break;
    case LUA_TNUMBER:  /* numbers */
      printf("  number: %g\n", lua_tonumber(L, i));
      break;
    default:  /* other values */
      printf("  %s\n", lua_typename(L, t));
      break;
    }
  }
  fflush(stdout);
}
