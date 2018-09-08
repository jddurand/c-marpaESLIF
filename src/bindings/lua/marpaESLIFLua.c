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
#define MARPAESLIFMULTITONS "__marpaESLIFMultiton"
/* Every key   is a marpaESLIFp light userdata */
/* Every value is a reference to the logger (reference to nil if there is none) */

/* Logger proxy: context and callback function */
typedef struct genericLoggerContext {
  lua_State *L; /* Logger has its own state */
  int logger_r; /* Logger reference */
  int L_r;      /* State reference */
} genericLoggerContext_t;
static void marpaESLIFLua_genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static int marpaESLIFLua_create_refi(lua_State *L, short weakb, lua_CFunction gcp);
static int marpaESLIFLua_install(lua_State *L);
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L);
static int marpaESLIFLua_marpaESLIF_newp(lua_State *L);
static int marpaESLIFLua_marpaESLIFMultiton_freev(lua_State *L);
static int marpaESLIFLua_marpaESLIFGrammar_newp(lua_State *L);
static int marpaESLIFLua_marpaESLIFGrammar_freev(lua_State *L);

/****************************************************************************/
int luaopen_marpaESLIFLua(lua_State* L)
/****************************************************************************/
{
  GENERICLOGGER_NOTICEF(NULL, "luaopen_marpaESLIFLua(L=%p)", L);
  luaL_requiref(L, "marpaESLIFLua", marpaESLIFLua_install, 1 /* global */);

  GENERICLOGGER_NOTICEF(NULL, "luaopen_marpaESLIFLua(L=%p) return 1", L);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_create_refi(lua_State *L, short weakb, lua_CFunction gcp)
/****************************************************************************/
/* C.f. https://stackoverflow.com/questions/19340662/lua-c-api-weak-references-with-lual-ref */
/****************************************************************************/
{
  static const char *weak_refs   = "weak_ref";
  static const char *strong_refs = "strong_ref";
  int          rci;

  lua_newtable(L);                      /* stack: value, new_table={} */

  if (weakb) {
    lua_newtable(L);                    /* stack: value, new_table={}, metatable={} */

    lua_pushliteral(L, "__mode");       /* stack: value, new_table={}, metatable={}, key="__mode" */
    lua_pushliteral(L, "v");            /* stack: value, new_table={}, metatable={}, key="__mode", value="v" */
    lua_rawset(L, -3);                  /* stack: value, new_table={}, metatable={"__mode" = "v"} */

    lua_setmetatable(L, -2);            /* stack: value, new_table={} with metatable={"__mode" = "v"} */
    lua_pushstring(L, weak_refs);       /* stack: value, new_table={} with metatable={"__mode" = "v"}, "weak_refs" */
    lua_pushvalue(L, -3);               /* stack: value, new_table={} with metatable={"__mode" = "v"}, "weak_refs", value */
    lua_rawset(L, -3);                  /* stack: value, new_table={"weak_refs" = value} with metatable={"__mode" = "v"} */
  } else {
    lua_pushstring(L, strong_refs);     /* stack: value, new_table={}, "strong_ref" */
    lua_pushvalue(L, -3);               /* stack: value, new_table={}, "strong_ref", value */
    lua_rawset(L, -3);                  /* stack: value, new_table={"strong_ref" = value} */
  }

  if (gcp != NULL) {
    lua_pushcfunction(L, gcp);          /* stack: value, new_table={ref_type = value}, gcp */
    lua_setfield(L, -2, "__gc");        /* stack: value, new_table={ref_type = value, "__gc" = gcp} */
  }

  rci = luaL_ref(L, LUA_REGISTRYINDEX); /* stack: value */

  return rci;                           /* returns a reference to new_table */
}

/****************************************************************************/
static int marpaESLIFLua_install(lua_State *L)
/****************************************************************************/
{
   static const luaL_Reg marpaESLIFLuaTable[] = {
      {"marpaESLIF_versions", marpaESLIFLua_marpaESLIF_versions},
      {"marpaESLIF_newp", marpaESLIFLua_marpaESLIF_newp},
      {"marpaESLIFGrammar_newp", marpaESLIFLua_marpaESLIFGrammar_newp},
      {NULL, NULL}
   };

   GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_install(L=%p)", L);
   luaL_newlib(L, marpaESLIFLuaTable);

   /* Install MARPAESLIFMULTITONS */
   lua_newtable(L);                                                  /* stack: {} */
   lua_newtable(L);                                                  /* stack: {}, {} */
   lua_pushcfunction(L, marpaESLIFLua_marpaESLIFMultiton_freev);       /* stack: {}, {}, marpaESLIFLua_marpaESLIFMultiton_freev */
   lua_setfield(L, -2, "__gc");                                      /* stack: {}, {__gc = marpaESLIFLua_marpaESLIFMultiton_freev} */
   lua_setmetatable(L, -2);                                          /* stack: {} meta={__gc = marpaESLIFLua_marpaESLIFMultiton_freev} */
   lua_setglobal(L, MARPAESLIFMULTITONS);                   /* stack: */

    /* Associate a __gc to MARPAESLIFMULTITONS */
   GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_install(L=%p) return 1", L);
   return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIF_versions(lua_State *L)
/****************************************************************************/
{
  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIF_versions(L=%p)", L);
  lua_pushstring(L, marpaESLIF_versions());

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIF_versions(L=%p) return 1", L);
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

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIF_newp(L=%p)", L);

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

  if (lua_getglobal(L, MARPAESLIFMULTITONS) != LUA_TTABLE) { /* stack: [loggerp,] MARPAESLIFMULTITONS? */
    return luaL_error(L, "%s is not a table", MARPAESLIFMULTITONS);
  }

  /* Look if MARPAESLIFMULTITONS already contains a reference to logger */
  lua_pushnil(L);                                                     /* stack: [loggerp,] MARPAESLIFMULTITONS, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: [loggerp,] MARPAESLIFMULTITONS, marpaESLIFp, r */
    logger_r = lua_tointeger(L, -1);
    if (logger_r == LUA_NOREF) {
      lua_pushnil(L);                                                 /* stack: [loggerp,] MARPAESLIFMULTITONS, marpaESLIFp, r, nil */
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, logger_r);                    /* stack: [loggerp,] MARPAESLIFMULTITONS, marpaESLIFp, r, loggerp_from_registry */
    }
    if (((! loggerb) && lua_isnil(L, -1))
        ||
        ((  loggerb) && lua_compare(L, 1, -1, LUA_OPEQ))) {
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, -3);
      lua_pop(L, 3);                                                  /* stack: [loggerp,] MARPAESLIFMULTITONS */
      break;
    }
    lua_pop(L, 2);                                                    /* stack: [loggerp,] MARPAESLIFMULTITONS, marpaESLIFp */
  }

  if (marpaESLIFp == NULL) {
    if (loggerb) {
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        return luaL_error(L, "malloc failure, %s\n", strerror(errno));
      }

      /* Get logger reference */
      lua_pushnil(L);                                                 /* stack:  loggerp,  MARPAESLIFMULTITONS, nil */
      lua_copy(L, 1, -1);                                             /* stack:  loggerp,  MARPAESLIFMULTITONS, loggerp */
      logger_r = luaL_ref(L, LUA_REGISTRYINDEX);                      /* stack:  loggerp,  MARPAESLIFMULTITONS */

      /* Create a dedicated lua state and get its reference */
      genericLoggerContextp->L = lua_newthread(L);                    /* stack:  loggerp,  MARPAESLIFMULTITONS, L */
      L_r = luaL_ref(L, LUA_REGISTRYINDEX);                           /* stack:  loggerp,  MARPAESLIFMULTITONS */

      genericLoggerContextp->L_r = L_r;
      genericLoggerContextp->logger_r = logger_r;
      genericLoggerp = genericLogger_newp(marpaESLIFLua_genericLoggerCallback, genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
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
      luaL_unref(L, LUA_REGISTRYINDEX, L_r); /* No effect if it is LUA_NOREF */
      luaL_unref(L, LUA_REGISTRYINDEX, logger_r); /* No effect if it is LUA_NOREF */
      free(genericLoggerContextp);
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    /* Link marpaESLIFp to logger_r */
    lua_pushinteger(L, (lua_Integer) logger_r);                       /* stack: [loggerp,] MARPAESLIFMULTITONS, logger_r */
    lua_rawsetp(L, -2, (const void *) marpaESLIFp);                   /* stack: [loggerp,] MARPAESLIFMULTITONS */
    lua_pop(L, 1);                                                    /* stack: [loggerp ] */
  }

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIF_newp(L=%p) marpaESLIFp=%p", L, marpaESLIFp);
  lua_pushlightuserdata(L, marpaESLIFp);                              /* stack: [loggerp], marpaESLIFp */

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIF_newp(L=%p) return 1", L);
  return 1;
}

/****************************************************************************/
static void marpaESLIFLua_genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  genericLoggerContext_t *genericLoggerContext = (genericLoggerContext_t *) userDatavp;
  int                     logger_r             = genericLoggerContext->logger_r;
  lua_State              *L                    = genericLoggerContext->L;
  const char             *funcs;
  int                     n;

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_genericLoggerCallback(userDatavp=%p, logLeveli=%d, msgs=%s)", userDatavp, logLeveli, msgs);
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
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, funcs);                  /* stack: logger, logger[funcs] */
      lua_pushstring(L, msgs);                     /* stack: logger, logger[funcs], msgs */
      lua_call(L, 1, LUA_MULTRET);                 /* stack: logger, logger[funcs], ?? */
    }
    lua_settop(L, 0);                            /* stack: */
  }
  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_genericLoggerCallback(userDatavp=%p, logLeveli=%d, msgs=%s) return", userDatavp, logLeveli, msgs);
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFMultiton_freev(lua_State *L)
/****************************************************************************/
{
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLoggerContext_t *genericLoggerContextp = NULL;
  lua_Integer             logger_r              = LUA_NOREF;
  lua_Integer             L_r                   = LUA_NOREF;
  genericLogger_t        *genericLoggerp        = NULL;
  marpaESLIFOption_t     *marpaESLIFOptionp;

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFMultiton_freev(L=%p)", L);

  /* Loop on MARPAESLIFMULTITONS */
  lua_pushnil(L);                                                     /* stack: MARPAESLIFMULTITONS, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: MARPAESLIFMULTITONS, marpaESLIFp, r */
    marpaESLIFp = lua_touserdata(L, -2);
    logger_r = lua_tointeger(L, -1);

    if (logger_r != LUA_NOREF) {

      marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
      if (marpaESLIFOptionp != NULL) {
        genericLoggerp = marpaESLIFOptionp->genericLoggerp;
        if (genericLoggerp != NULL) {
          genericLoggerContextp = (genericLoggerContext_t *) genericLogger_userDatavp_getp(genericLoggerp);
          luaL_unref(L, LUA_REGISTRYINDEX, genericLoggerContextp->L_r);
          luaL_unref(L, LUA_REGISTRYINDEX, genericLoggerContextp->logger_r); /* By construction genericLoggerContextp->logger_r == logger_r */
        }
      }

      /* Free marpaESLIF, genericLogger, genericLogger context (in this order) */
      GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFMultiton_freev(L=%p) marpaESLIFp=%p", L, marpaESLIFp);
      marpaESLIF_freev(marpaESLIFp);
      if (genericLoggerContextp != NULL) {
        free(genericLoggerContextp);
      }
      genericLogger_freev(&genericLoggerp);
    } else {
      /* Free marpaESLIF */
      GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFMultiton_freev(L=%p) marpaESLIFp=%p", L, marpaESLIFp);
      marpaESLIF_freev(marpaESLIFp);
    }

    lua_pop(L, 1);                                                    /* stack: MARPAESLIFMULTITONS, marpaESLIFp */
  }

  return 0;
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
 
  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFGrammar_newp(L=%p)", L);

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

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFGrammar_newp(L=%p) marpaESLIFGrammarp=%p", marpaESLIFGrammarp);

  /* We return a table of weak values { "marpaESLIFGrammarp" => marpaESLIFGrammarp } with an associated __gc */
  lua_newtable(L);                                                  /* stack: {} */
  lua_pushlightuserdata(L, marpaESLIFGrammarp);                     /* stack: {}, marpaESLIFGrammarp */
  lua_setfield(L, -2, "marpaESLIFGrammarp");                        /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp} */
  lua_newtable(L);                                                  /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, {} */
  lua_pushstring(L, "v");                                           /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, {}, "v" */
  lua_setfield(L, -2, "__mode");                                    /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, {__mode = "v"} */
  lua_pushcfunction(L, marpaESLIFLua_marpaESLIFGrammar_freev);      /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, {__mode = "v"}, marpaESLIFLua_marpaESLIFGrammar_freev */
  lua_setfield(L, -2, "__gc");                                      /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, {__mode = "v"__gc = marpaESLIFLua_marpaESLIFGrammar_freev} */
  lua_setmetatable(L, -2);                                          /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp} meta={__mode = "v", __gc = marpaESLIFLua_marpaESLIFGrammar_freev} */

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFGrammar_newp(L=%p) return 1", L);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_freev(lua_State *L)
/****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFGrammar_freev(L=%p)", L);

  lua_getfield(L, -1, "marpaESLIFGrammarp"); /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp}, marpaESLIFGrammarp */
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) lua_touserdata(L, -1);

  GENERICLOGGER_NOTICEF(NULL, "marpaESLIFLua_marpaESLIFGrammar_freev(L=%p) marpaESLIFGrammarp=%p", L, marpaESLIFGrammarp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);

  lua_pop(L, 2); /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp} */

  return 0;
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