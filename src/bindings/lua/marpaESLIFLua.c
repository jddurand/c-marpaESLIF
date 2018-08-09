/* Only 5.3 and upwards is supported */

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
/* Every value is a reference to the logger (reference to nil if there is none) */

/* Logger proxy: context and callback function */
typedef struct genericLoggerContext {
  lua_State *L; /* Logger has its own state */
  int logger_r; /* Logger reference */
  int L_r;      /* State reference */
} genericLoggerContext_t;
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

static int installmarpaESLIFLua(lua_State *L);
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L);
static int marpaESLIFLua_marpaESLIF_newp(lua_State *L);
static int marpaESLIFLua_marpaESLIF_freev(lua_State *L);
static int marpaESLIFLua_marpaESLIFGrammar_newp(lua_State *L);

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
      {"marpaESLIF_freev", marpaESLIFLua_marpaESLIF_freev},
      {"marpaESLIFGrammar_newp", marpaESLIFLua_marpaESLIFGrammar_newp},
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
  marpaESLIF_t           *marpaESLIFp = NULL;
  short                   loggerb;
  genericLoggerContext_t *genericLoggerContextp;
  genericLogger_t        *genericLoggerp;
  marpaESLIFOption_t      marpaESLIFOption;
  lua_Integer             logger_r;
  lua_Integer             L_r;

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
    loggerb = 0;
    break;
  case 1:
    if (lua_type(L, 1) == LUA_TNIL) {
      loggerb = 0;
    } else {
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
      loggerb = 1;
    }
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

  /* Look if MARPAESLIF_GLOBAL_TABLE_NAME already contains a reference to logger */
  lua_pushnil(L);                                                     /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp, r */
    logger_r = lua_tointeger(L, -1);
    if (logger_r == LUA_NOREF) {
      lua_pushnil(L);                                                 /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp, r, nil */
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, logger_r);                    /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp, r, loggerp_from_registry */
    }
    if (((! loggerb) && lua_isnil(L, -1))
        ||
        ((  loggerb) && lua_compare(L, 1, -1, LUA_OPEQ))) {
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, -3);
      lua_pop(L, 3);                                                  /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME */
      break;
    }
    lua_pop(L, 2);                                                    /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp */
  }                                                                   /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME */

  if (marpaESLIFp == NULL) {
    if (loggerb) {
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        return luaL_error(L, "malloc failure, %s\n", strerror(errno));
      }

      /* Get logger reference */
      lua_pushnil(L);                                                 /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, nil */
      lua_copy(L, 1, -1);                                             /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, loggerp */
      logger_r = luaL_ref(L, LUA_REGISTRYINDEX);                      /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME */

      /* Create a dedicated lua state and get its reference */
      genericLoggerContextp->L = lua_newthread(L);                    /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME, L */
      L_r = luaL_ref(L, LUA_REGISTRYINDEX);                           /* stack:  loggerp,  MARPAESLIF_GLOBAL_TABLE_NAME */

      genericLoggerContextp->L_r = L_r;
      genericLoggerContextp->logger_r = logger_r;
      genericLoggerp = genericLogger_newp(genericLoggerCallback, genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        luaL_unref(L, LUA_REGISTRYINDEX, L_r);
        luaL_unref(L, LUA_REGISTRYINDEX, logger_r);
        free(genericLoggerContextp);
        return luaL_error(L, "genericLogger_newp failure, %s\n", strerror(errno));
      }
    } else {
      L_r = LUA_NOREF;
      logger_r = LUA_NOREF;
      genericLoggerp = NULL;
    }

    marpaESLIFOption.genericLoggerp = genericLoggerp;
    marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
    if (marpaESLIFp == NULL) {
      luaL_unref(L, LUA_REGISTRYINDEX, L_r);
      luaL_unref(L, LUA_REGISTRYINDEX, logger_r);
      free(genericLoggerContextp);
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    /* Link marpaESLIFp to logger_r */
    lua_pushinteger(L, (lua_Integer) logger_r);                       /* stack: [loggerp,] MARPAESLIF_GLOBAL_TABLE_NAME, logger_r */
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
  int                     logger_r             = genericLoggerContext->logger_r;
  lua_State              *L                    = genericLoggerContext->L;
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
    lua_rawgeti(L, LUA_REGISTRYINDEX, logger_r); /* stack: logger */
    lua_getfield(L, -1, funcs);                  /* stack: logger, logger[funcs] */
    lua_pushstring(L, msgs);                     /* stack: logger, logger[funcs], msgs */
    lua_call(L, 1, LUA_MULTRET);                 /* stack: logger, logger[funcs], ?? */
    lua_settop(L, 0);                            /* stack: */
  }
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIF_freev(lua_State *L)
/****************************************************************************/
{
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLoggerContext_t *genericLoggerContextp = NULL;
  lua_Integer             logger_r              = LUA_NOREF;
  lua_Integer             L_r                   = LUA_NOREF;
  genericLogger_t        *genericLoggerp        = NULL;
  marpaESLIFOption_t     *marpaESLIFOptionp;

  if (lua_gettop(L) == 1) {
    if (lua_islightuserdata(L, 1)) {                           /* stack: marpaESLIFp? */
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, 1);
      /* Is that in the registry ? */
      if (lua_getglobal(L, MARPAESLIF_GLOBAL_TABLE_NAME) == LUA_TTABLE) { /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME? */
        lua_pushnil(L);                                                   /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME, nil */
        while (lua_next(L, -2) != 0) {                                    /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp, r */
          if (lua_touserdata(L, -2) == marpaESLIFp) {
            logger_r = lua_tointeger(L, -1);
            /* Remove the entry from MARPAESLIF_GLOBAL_TABLE_NAME */
            lua_pop(L, 2);                                                /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME */
            lua_pushnil(L);                                               /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME, nil */
            lua_rawsetp(L, -2, marpaESLIFp);                              /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME */
            break;
          }
          lua_pop(L, 1);                                                  /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME, marpaESLIFp */
        }                                                                 /* stack: marpaESLIFp, MARPAESLIF_GLOBAL_TABLE_NAME */
      }
      lua_pop(L, 1);                                                      /* stack: marpaESLIFp */
    }
  }

  if (logger_r != LUA_NOREF) {

    marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
    if (marpaESLIFOptionp != NULL) {
      genericLoggerp = marpaESLIFOptionp->genericLoggerp;
      if (genericLoggerp != NULL) {
        genericLoggerContextp = (genericLoggerContext_t *) genericLogger_userDatavp_getp(genericLoggerp);
        luaL_unref(L, LUA_REGISTRYINDEX, genericLoggerContextp->L_r);
        luaL_unref(L, LUA_REGISTRYINDEX, genericLoggerContextp->logger_r);
      }
    }

    /* Free marpaESLIF, genericLogger, genericLogger context (in this order) */
    if (marpaESLIFp != NULL) {
      marpaESLIF_freev(marpaESLIFp);
    }
    if (genericLoggerContextp != NULL) {
      free(genericLoggerContextp);
    }
    if (genericLoggerp != NULL) {
      genericLogger_freev(&genericLoggerp);
    }
  }
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_newp(lua_State *L)
/****************************************************************************/
{
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammar_t       *marpaESLIFGrammarp;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption = {
    NULL, /* bytep */
    0,    /* bytel */
    NULL, /* encodings */
    0     /* encodingl */
  };
 
  switch (lua_gettop(L)) {
  case 3:
    marpaESLIFGrammarOption.encodings = (char *) luaL_checklstring(L, 3, &(marpaESLIFGrammarOption.encodingl));
    /* Intentionnaly no break */
  case 2:
    marpaESLIFGrammarOption.bytep = (char *) luaL_checklstring(L, 2, &(marpaESLIFGrammarOption.bytel));
    /* Verify that the 1st argument is a light user data */
    if (lua_type(L, 1) != LUA_TLIGHTUSERDATA) {
      return luaL_error(L, "Usage: marpaESLIFp must be a light user data)");
    }
    marpaESLIFp = lua_touserdata(L, 1);
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIFGrammar_newp(marpaESLIFp, string[, encoding])");
  }

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    return luaL_error(L, "marpaESLIFGrammarp failure, %s", strerror(errno));
  }

  lua_pushlightuserdata(L, marpaESLIFGrammarp);
  return 1;
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
    case LUA_TNIL:
      printf("  nil\n");
      break;
    case LUA_TNUMBER:
      printf("  number: %g\n", lua_tonumber(L, i));
      break;
    case LUA_TBOOLEAN:
      printf("  boolean %s\n",lua_toboolean(L, i) ? "true" : "false");
      break;
    case LUA_TUSERDATA:
      printf("  userdata: %p\n", lua_touserdata(L, i));
      break;
    case LUA_TLIGHTUSERDATA:
      printf("  light userdata: %p\n", lua_touserdata(L, i));
      break;
    case LUA_TSTRING:
      printf("  string: '%s'\n", lua_tostring(L, i));
      break;
    default:  /* other values */
      printf("  %s\n", lua_typename(L, t));
      break;
    }
  }
  fflush(stdout);
}
