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
#undef  FILENAMES
#define FILENAMES "marpaESLIFLua.c"

/* Global table for the multiton pattern */
#define MARPAESLIFMULTITONS "__marpaESLIFMultiton"
/* Every key   is a marpaESLIFp light userdata */
/* Every value is a reference to the logger (reference to nil if there is none) */

/* Logger proxy: context and callback function */
typedef struct genericLoggerContext {
  lua_State *L; /* Lua state */
  int logger_r; /* Logger reference */
} genericLoggerContext_t;
static short marpaESLIFLua_paramIsLoggerInterfaceOrNilb(lua_State *L, int stacki);
static void  marpaESLIFLua_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static int   marpaESLIFLua_create_refi(lua_State *L, short weakb, lua_CFunction gcp);
static int   marpaESLIFLua_installi(lua_State *L);
static int   marpaESLIFLua_versioni(lua_State *L);
static int   marpaESLIFLua_marpaESLIF_newi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFMultiton_freevi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_newi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_freei(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ngrammari(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ruleShowi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_showi(lua_State *L);
static int   marpaESLIFLua_marpaESLIFGrammar_showByLeveli(lua_State *L);

#define MARPAESLIFLUA_GETORCREATEGLOBAL(L, name, gcp) do {              \
    if (lua_getglobal(L, name) != LUA_TTABLE) {                         \
      lua_pop(L, 1);                                                    \
      lua_newtable(L);                                                  \
      if (gcp != NULL) {                                                \
        lua_newtable(L);                                                \
        lua_pushcfunction(L, gcp);                                      \
        lua_setfield(L, -2, "__gc");                                    \
        lua_setmetatable(L, -2);                                        \
        lua_setglobal(L, name);                                         \
        lua_getglobal(L, name);                                         \
      }                                                                 \
    }                                                                   \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_FUNCTION(L, key, functionp) do {            \
    if (functionp != NULL) {                                            \
      lua_pushcfunction(L, functionp);                                  \
    } else {                                                            \
      lua_pushnil(L);                                                   \
    }                                                                   \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_USERDATA(L, key, p) do {                    \
    if (p != NULL) {                                                    \
      lua_pushlightuserdata(L, p);                                      \
    } else {                                                            \
      lua_pushnil(L);                                                   \
    }                                                                   \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_STRING(L, key, stringp) do {                \
    if (stringp != NULL) {                                              \
      lua_pushlstring(L, (const char *) stringp->bytep, stringp->bytel); \
    } else {                                                            \
      lua_pushnil(L);                                                   \
    }                                                                   \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_ASCIISTRING(L, key, asciis) do {            \
    if (asciis != NULL) {                                               \
      lua_pushstring(L, asciis);                                        \
    } else {                                                            \
      lua_pushnil(L);                                                   \
    }                                                                   \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_INTEGER(L, key, i) do {                     \
    lua_pushinteger(L, (lua_Integer) i);                                \
    lua_setfield(L, -2, key);                                           \
  } while (0)

#define MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, integerl, integerp) do {  \
    size_t _iteratorl;                                                  \
                                                                        \
    lua_createtable(L, (int) integerl, 0);                              \
    for (_iteratorl = 0; _iteratorl < integerl; _iteratorl++) {         \
      lua_pushinteger(L, (lua_Integer) integerp[_iteratorl]);           \
      lua_rawseti(L, -2, (lua_Integer) _iteratorl);                     \
    }                                                                   \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, key, integerl, integerp) do { \
    MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, integerl, integerp);            \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_BOOLEAN(L, key, b) do {                     \
    lua_pushboolean(L, (int) b);                                        \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/* Destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_ACTION(L, key, actionp) do {                \
    if (actionp != NULL) {                                              \
      switch (actionp->type) {                                          \
      case MARPAESLIF_ACTION_TYPE_NAME:                                 \
        lua_pushstring(L, actionp->u.names);                            \
        break;                                                          \
      case MARPAESLIF_ACTION_TYPE_STRING:                               \
        lua_pushlstring(L, (const char *) actionp->u.stringp->bytep, actionp->u.stringp->bytel); \
        break;                                                          \
      default:                                                          \
        return luaL_error(L, "Unsupported action type %d", actionp->type); \
      }                                                                 \
    } else {                                                            \
      lua_pushnil(L);                                                   \
    }                                                                   \
    lua_setfield(L, -2, key);                                           \
  } while (0)

/****************************************************************************/
int luaopen_marpaESLIFLua(lua_State* L)
/****************************************************************************/
{
  static const char *funcs = "luaopen_marpaESLIFLua";

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);
  luaL_requiref(L, "marpaESLIFLua", marpaESLIFLua_installi, 1 /* global */);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_create_refi(lua_State *L, short weakb, lua_CFunction gcp)
/****************************************************************************/
/* C.f. https://stackoverflow.com/questions/19340662/lua-c-api-weak-references-with-lual-ref */
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_create_refi";
  static const char *weak_refs   = "weak_ref";
  static const char *strong_refs = "strong_ref";
  int          rci;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p, weakb=%d, gcp=%p) at %s:%d", funcs, L, (int) weakb, gcp, FILENAMES, __LINE__);

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

  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", gcp);

  rci = luaL_ref(L, LUA_REGISTRYINDEX); /* stack: value */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p, weakb=%d, gcp=%p) return %d at %s:%d", funcs, L, (int) weakb, gcp, rci, FILENAMES, __LINE__);

  return rci;                           /* returns a reference to new_table */
}

/****************************************************************************/
static int marpaESLIFLua_installi(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_installi";
  static const luaL_Reg marpaESLIFLuaTable[] = {
    {"version", marpaESLIFLua_versioni},
    {"marpaESLIF_new", marpaESLIFLua_marpaESLIF_newi},
    {"marpaESLIFGrammar_new", marpaESLIFLua_marpaESLIFGrammar_newi},
    {"marpaESLIFGrammar_ngrammar", marpaESLIFLua_marpaESLIFGrammar_ngrammari},
    {"marpaESLIFGrammar_currentLevel", marpaESLIFLua_marpaESLIFGrammar_currentLeveli},
    {"marpaESLIFGrammar_currentDescription", marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni},
    {"marpaESLIFGrammar_descriptionByLevel", marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli},
    {"marpaESLIFGrammar_currentRuleIds", marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi},
    {"marpaESLIFGrammar_ruleIdsByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli},
    {"marpaESLIFGrammar_currentSymbolIds", marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi},
    {"marpaESLIFGrammar_symbolIdsByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli},
    {"marpaESLIFGrammar_currentProperties", marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi},
    {"marpaESLIFGrammar_propertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli},
    {"marpaESLIFGrammar_currentRuleProperties", marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi},
    {"marpaESLIFGrammar_rulePropertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli},
    {"marpaESLIFGrammar_currentSymbolProperties", marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi},
    {"marpaESLIFGrammar_symbolPropertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli},
    {"marpaESLIFGrammar_ruleDisplay", marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi},
    {"marpaESLIFGrammar_symbolDisplay", marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi},
    {"marpaESLIFGrammar_ruleShow", marpaESLIFLua_marpaESLIFGrammar_ruleShowi},
    {"marpaESLIFGrammar_ruleDisplayByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli},
    {"marpaESLIFGrammar_symbolDisplayByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli},
    {"marpaESLIFGrammar_ruleShowByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli},
    {"marpaESLIFGrammar_show", marpaESLIFLua_marpaESLIFGrammar_showi},
    {"marpaESLIFGrammar_showByLevel", marpaESLIFLua_marpaESLIFGrammar_showByLeveli},
    {NULL, NULL}
  };

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);
  luaL_newlib(L, marpaESLIFLuaTable);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_versioni(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_versioni";
  const char        *versions;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);
  versions = marpaESLIF_versions();
  lua_pushstring(L, versions);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (versions=\"%s\") at %s:%d", funcs, L, versions, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIF_newi(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIF_newi";
  marpaESLIF_t           *marpaESLIFp = NULL;
  short                   loggerb;
  genericLoggerContext_t *genericLoggerContextp;
  genericLogger_t        *genericLoggerp;
  marpaESLIFOption_t      marpaESLIFOption;
  int                     logger_r;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  switch (lua_gettop(L)) {
  case 0:
    loggerb = 0;
    break;
  case 1:
    loggerb = marpaESLIFLua_paramIsLoggerInterfaceOrNilb(L, 1);
    lua_pop(L, 1);
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIF_new([logger]");
  }

  MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFMULTITONS, marpaESLIFLua_marpaESLIFMultiton_freevi);

  /* Look if MARPAESLIFMULTITONS already contains a reference to logger */
  lua_pushnil(L);                                                     /* stack: MARPAESLIFMULTITONS, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: MARPAESLIFMULTITONS, marpaESLIFp, r */
    logger_r = (int) lua_tointeger(L, -1);
    if (logger_r == LUA_NOREF) {
      lua_pushnil(L);                                                 /* stack: MARPAESLIFMULTITONS, marpaESLIFp, r, nil */
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, logger_r);                    /* stack: MARPAESLIFMULTITONS, marpaESLIFp, r, loggerp_from_registry */
    }
    if (((! loggerb) && lua_isnil(L, -1))
        ||
        ((  loggerb) && lua_compare(L, 1, -1, LUA_OPEQ))) {
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, -3);
      lua_pop(L, 3);                                                  /* stack: MARPAESLIFMULTITONS */
      break;
    }
    lua_pop(L, 2);                                                    /* stack: MARPAESLIFMULTITONS, marpaESLIFp */
  }

  if (marpaESLIFp == NULL) {
    if (loggerb) {
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        return luaL_error(L, "malloc failure, %s\n", strerror(errno));
      }

      genericLoggerContextp->L = NULL;
      genericLoggerContextp->logger_r =  LUA_NOREF;

      /* Get logger reference */
      lua_pushnil(L);                                                 /* stack:  loggerp,  MARPAESLIFMULTITONS, nil */
      lua_copy(L, 1, -1);                                             /* stack:  loggerp,  MARPAESLIFMULTITONS, loggerp */
      logger_r = luaL_ref(L, LUA_REGISTRYINDEX);                      /* stack:  loggerp,  MARPAESLIFMULTITONS */
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got logger_r=%d from registry at %s:%d", funcs, L, (int) logger_r, FILENAMES, __LINE__);

      /* Fill genericLogger context */
      genericLoggerContextp->L = L;
      genericLoggerContextp->logger_r = logger_r;
      genericLoggerp = genericLogger_newp(marpaESLIFLua_genericLoggerCallbackv, genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, (int) logger_r, FILENAMES, __LINE__);
        luaL_unref(L, LUA_REGISTRYINDEX, (int) logger_r);
        free(genericLoggerContextp);
        return luaL_error(L, "genericLogger_newp failure, %s\n", strerror(errno));
      }
    } else {
      logger_r = LUA_NOREF;
      genericLoggerp = NULL;
    }

    marpaESLIFOption.genericLoggerp = genericLoggerp;
    marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
    if (marpaESLIFp == NULL) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, (int) logger_r, FILENAMES, __LINE__);
      luaL_unref(L, LUA_REGISTRYINDEX, (int) logger_r); /* No effect if it is LUA_NOREF */
      free(genericLoggerContextp);
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    /* Link marpaESLIFp to logger_r */
    lua_pushinteger(L, (lua_Integer) logger_r);                       /* stack: MARPAESLIFMULTITONS, logger_r */
    lua_rawsetp(L, -2, (const void *) marpaESLIFp);                   /* stack: MARPAESLIFMULTITONS */
    lua_pop(L, 1);                                                    /* stack: */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
  } else {
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p (reuse) at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
  }

  lua_pushlightuserdata(L, marpaESLIFp);                              /* stack: marpaESLIFp */

  /* Create a metable */
  lua_newtable(L);
  lua_newtable(L);                                                    /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFGrammar_new", marpaESLIFLua_marpaESLIFGrammar_newi);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                            /* stack: marpaESLIFp meta={...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (marpaESLIFp=%p) at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static short marpaESLIFLua_paramIsLoggerInterfaceOrNilb(lua_State *L, int stacki)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_paramIsLoggerInterfaceOrNilb";
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
  int                i;
  short              loggerb;

  if (lua_type(L, 1) == LUA_TNIL) {
    loggerb = 0;
  } else {
    /* Verify that the logger can do all wanted methods */
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "logger must be a table");
    }
    for (i = 0; i < sizeof(loggerFunctions)/sizeof(loggerFunctions[0]); i++) {
      if (lua_getfield(L, -1, loggerFunctions[i]) != LUA_TFUNCTION) {
        return luaL_error(L, "logger table must have a field named '%s' that is a function", loggerFunctions[i]);
      } else {
        lua_pop(L, 1);
      }
    }
    loggerb = 1;
  }

  return loggerb;
}

/****************************************************************************/
static void marpaESLIFLua_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  genericLoggerContext_t *genericLoggerContext = (genericLoggerContext_t *) userDatavp;
  int                     logger_r             = genericLoggerContext->logger_r;
  lua_State              *L                    = genericLoggerContext->L;
  const char             *funcs;
  int                     topi;

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
    topi = lua_gettop(L);                          /* stack: xxx */
    lua_rawgeti(L, LUA_REGISTRYINDEX, logger_r);   /* stack: xxx, logger */
    lua_getfield(L, -1, funcs);                    /* stack: xxx, logger, logger[funcs] */
    lua_pushstring(L, msgs);                       /* stack: xxx, logger, logger[funcs], msgs */
    lua_call(L, 1, LUA_MULTRET);                   /* stack: xxx, logger, logger[funcs], ?? */
    lua_settop(L, topi);                           /* stack: xxx */
  }
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFMultiton_freevi(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIFMultiton_freevi";
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLoggerContext_t *genericLoggerContextp = NULL;
  lua_Integer             logger_r              = LUA_NOREF;
  genericLogger_t        *genericLoggerp        = NULL;
  marpaESLIFOption_t     *marpaESLIFOptionp;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

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
          GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, (int) genericLoggerContextp->logger_r, FILENAMES, __LINE__);
          luaL_unref(L, LUA_REGISTRYINDEX, (int) genericLoggerContextp->logger_r); /* By construction genericLoggerContextp->logger_r == logger_r */
        }
      }

      /* Free marpaESLIF, genericLogger, genericLogger context (in this order) */
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
      marpaESLIF_freev(marpaESLIFp);
      if (genericLoggerContextp != NULL) {
        free(genericLoggerContextp);
      }
      genericLogger_freev(&genericLoggerp);
    } else {
      /* Free marpaESLIF */
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
      marpaESLIF_freev(marpaESLIFp);
    }

    lua_pop(L, 1);                                                    /* stack: MARPAESLIFMULTITONS, marpaESLIFp */
  }

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);

  return 0;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_newi(lua_State *L)
/****************************************************************************/
{
  static const char        *funcs = "marpaESLIFLua_marpaESLIFGrammar_newi";
  int                        multiton_refi;
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammar_t       *marpaESLIFGrammarp;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption = {
    NULL, /* bytep */
    0,    /* bytel */
    NULL, /* encodings */
    0     /* encodingl */
  };
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

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
    return luaL_error(L, "Usage: marpaESLIFGrammar_new(marpaESLIFp, string[, encoding])");
  }

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    return luaL_error(L, "marpaESLIFGrammarp failure, %s", strerror(errno));
  }

  /* Get a strong reference to multiton table */
  lua_getglobal(L, MARPAESLIFMULTITONS);                                       /* stack: MARPAESLIFMULTITONS */
  multiton_refi = marpaESLIFLua_create_refi(L, 0 /* weakb */, NULL /* gcp */); /* stack: MARPAESLIFMULTITONS */
  lua_pop(L, 1);                                                               /* stack: */

  /* We return a table of weak values { "marpaESLIFGrammarp" => marpaESLIFGrammarp } with an associated __gc */
  lua_newtable(L);                                                             /* stack: {} */
  MARPAESLIFLUA_STORE_USERDATA(L, "marpaESLIFGrammarp", marpaESLIFGrammarp);
  MARPAESLIFLUA_STORE_INTEGER(L, "multiton_refi", multiton_refi);

  /* Create a metable */
  lua_newtable(L);
  MARPAESLIFLUA_STORE_ASCIISTRING(L, "__mode", "v");                              /* ... Say the values are weak */
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", marpaESLIFLua_marpaESLIFGrammar_freei); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "ngrammar", marpaESLIFLua_marpaESLIFGrammar_ngrammari);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentLevel", marpaESLIFLua_marpaESLIFGrammar_currentLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentDescription", marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni);
  MARPAESLIFLUA_STORE_FUNCTION(L, "descriptionByLevel", marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentRuleIds", marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleIdsByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentSymbolIds", marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolIdsByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentProperties", marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "propertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentRuleProperties", marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "rulePropertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentSymbolProperties", marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolPropertiesByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleDisplay", marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolDisplay", marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleShow", marpaESLIFLua_marpaESLIFGrammar_ruleShowi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleDisplayByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolDisplayByLevel", marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleShowByLevel", marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli);
  MARPAESLIFLUA_STORE_FUNCTION(L, "show", marpaESLIFLua_marpaESLIFGrammar_showi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "showByLevel", marpaESLIFLua_marpaESLIFGrammar_showByLeveli);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                     /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp, "multiton_refi" = multiton_refi} meta={...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({marpaESLIFGrammarp=%p, multiton_refi=%d}) at %s:%d", funcs, L, marpaESLIFGrammarp, multiton_refi, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_freei(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_freei";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          multiton_refi;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  lua_getfield(L, -1, "marpaESLIFGrammarp"); /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp, "multiton_refi" = multiton_refi}, marpaESLIFGrammarp */
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp, "multiton_refi" = multiton_refi} */

  lua_getfield(L, -1, "multiton_refi");      /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp, "multiton_refi" = multiton_refi}, multiton_refi */
  multiton_refi = lua_tointeger(L, -1);
  lua_pop(L, 1);                             /* stack: {"marpaESLIFGrammarp" = marpaESLIFGrammarp, "multiton_refi" = multiton_refi} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing multiton_refi=%d from registry at %s:%d", funcs, L, (int) multiton_refi, FILENAMES, __LINE__);
  luaL_unref(L, LUA_REGISTRYINDEX, (int) multiton_refi);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFGrammarp=%p at %s:%d", funcs, L, marpaESLIFGrammarp, FILENAMES, __LINE__);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);

  lua_pop(L, 1);                             /* stack: */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);
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

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ngrammari(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ngrammari";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  ngrammari;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ngrammar(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    return luaL_error(L, "marpaESLIFGrammar_ngrammarib failure");
  }
  lua_pushinteger(L, (lua_Integer) ngrammari);   /* stack: ngrammari */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ngrammari=%d) at %s:%d", funcs, L, ngrammari, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentLevel(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_grammar_currentb failure");
  }
  lua_pushinteger(L, (lua_Integer) leveli);   /* stack: ngrammari */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (leveli=%d) at %s:%d", funcs, L, leveli, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentDescription(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, NULL, &descp)) {
    return luaL_error(L, "marpaESLIFGrammar_grammar_currentb failure");
  }
  lua_pushlstring(L, (const char *) descp->bytep, descp->bytel);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (bytep=%p, bytel=%ld) at %s:%d", funcs, L, descp->bytep, (unsigned long) descp->bytel, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli";
  lua_Integer          leveli;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_descriptionByLevel(marpaESLIFGrammarp, leveli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  
  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammarp, (int) leveli, NULL, NULL, &descp)) {
    return luaL_error(L, "marpaESLIFGrammar_grammar_by_levelb failure");
  }
  lua_pushlstring(L, (const char *) descp->bytep, descp->bytel);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (bytep=%p, bytel=%ld) at %s:%d", funcs, L, descp->bytep, (unsigned long) descp->bytel, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *ruleip;
  size_t               rulel;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentRuleIds(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_rulearray_currentb(marpaESLIFGrammarp, &ruleip, &rulel)) {
    return luaL_error(L, "marpaESLIFGrammar_rulearray_currentb failure");
  }
  if (rulel <= 0) {
    return luaL_error(L, "marpaESLIFGrammar_rulearray_currentb returned no rule");
  }

  MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, rulel, ruleip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ruleip=%p, rulel=%ld) at %s:%d", funcs, L, ruleip, (unsigned long) rulel, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  int                 *ruleip;
  size_t               rulel;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ruleIdsByLevel(marpaESLIFGrammarp, leveli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammarp, &ruleip, &rulel, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_rulearray_by_levelb failure");
  }
  if (rulel <= 0) {
    return luaL_error(L, "marpaESLIFGrammar_rulearray_by_levelb returned no rule");
  }

  MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, rulel, ruleip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ruleip=%p, rulel=%ld) at %s:%d", funcs, L, ruleip, (unsigned long) rulel, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *symbolip;
  size_t               symboll;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentSymbolIds(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_symbolarray_currentb(marpaESLIFGrammarp, &symbolip, &symboll)) {
    return luaL_error(L, "marpaESLIFGrammar_symbolarray_currentb failure");
  }
  if (symboll <= 0) {
    return luaL_error(L, "marpaESLIFGrammar_symbolarray_currentb returned no symbol");
  }

  MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, symboll, symbolip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (symbolip=%p, symboll=%ld) at %s:%d", funcs, L, symbolip, (unsigned long) symboll, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  int                 *symbolip;
  size_t               symboll;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, leveli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_symbolarray_by_levelb(marpaESLIFGrammarp, &symbolip, &symboll, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_symbolarray_by_levelb failure");
  }
  if (symboll <= 0) {
    return luaL_error(L, "marpaESLIFGrammar_symbolarray_by_levelb returned no symbol");
  }

  MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, symboll, symbolip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (symbolip=%p, symboll=%ld) at %s:%d", funcs, L, symbolip, (unsigned long) symboll, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFGrammarProperty_t  grammarProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentProperties(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_grammarproperty_currentb(marpaESLIFGrammarp, &grammarProperty)) {
    return luaL_error(L, "marpaESLIFGrammar_grammarproperty_currentb failure");
  }

  lua_createtable(L, 11, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "level",               grammarProperty.leveli);
  MARPAESLIFLUA_STORE_INTEGER      (L, "maxlevel",            grammarProperty.maxLeveli);
  MARPAESLIFLUA_STORE_STRING       (L, "description",         grammarProperty.descp);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "latm",                grammarProperty.latmb);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultSymbolAction", grammarProperty.defaultSymbolActionp);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultRuleAction",   grammarProperty.defaultRuleActionp);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultFreeAction",   grammarProperty.defaultFreeActionp);
  MARPAESLIFLUA_STORE_INTEGER      (L, "startId",             grammarProperty.starti);
  MARPAESLIFLUA_STORE_INTEGER      (L, "discardId",           grammarProperty.discardi);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "symbolIds",           grammarProperty.nsymboll, grammarProperty.symbolip);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "ruleIds",             grammarProperty.nrulel, grammarProperty.ruleip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  marpaESLIFGrammarProperty_t  grammarProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_symbolIdsByLevel(marpaESLIFGrammarp, leveli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_grammarproperty_by_levelb(marpaESLIFGrammarp, &grammarProperty, (int) leveli, NULL /* descp */)) {
    return luaL_error(L, "marpaESLIFGrammar_grammarproperty_by_levelb failure");
  }

  lua_createtable(L, 11, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "level",               grammarProperty.leveli);
  MARPAESLIFLUA_STORE_INTEGER      (L, "maxlevel",            grammarProperty.maxLeveli);
  MARPAESLIFLUA_STORE_STRING       (L, "description",         grammarProperty.descp);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "latm",                grammarProperty.latmb);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultSymbolAction", grammarProperty.defaultSymbolActionp);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultRuleAction",   grammarProperty.defaultRuleActionp);
  MARPAESLIFLUA_STORE_ACTION       (L, "defaultFreeAction",   grammarProperty.defaultFreeActionp);
  MARPAESLIFLUA_STORE_INTEGER      (L, "startId",             grammarProperty.starti);
  MARPAESLIFLUA_STORE_INTEGER      (L, "discardId",           grammarProperty.discardi);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "symbolIds",           grammarProperty.nsymboll, grammarProperty.symbolip);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "ruleIds",             grammarProperty.nrulel, grammarProperty.ruleip);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  rulei;
  marpaESLIFRuleProperty_t     ruleProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentRuleProperties(marpaESLIFGrammarp, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  rulei = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_ruleproperty_currentb(marpaESLIFGrammarp, (int) rulei, &ruleProperty)) {
    return luaL_error(L, "marpaESLIFGrammar_ruleproperty_currentb failure");
  }

  lua_createtable(L, 18, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "id",                       ruleProperty.idi);
  MARPAESLIFLUA_STORE_STRING       (L, "description",              ruleProperty.descp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "show",                     ruleProperty.asciishows);
  MARPAESLIFLUA_STORE_INTEGER      (L, "lhsId",                    ruleProperty.lhsi);
  MARPAESLIFLUA_STORE_INTEGER      (L, "separatorId",              ruleProperty.separatori);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "rhsIds",                   ruleProperty.nrhsl, ruleProperty.rhsip);
  MARPAESLIFLUA_STORE_INTEGER      (L, "exceptionId",              ruleProperty.exceptioni);
  MARPAESLIFLUA_STORE_ACTION       (L, "action",                   ruleProperty.actionp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "discardEvent",             ruleProperty.discardEvents);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardEventInitialState", ruleProperty.discardEventb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "rank",                     ruleProperty.ranki);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "nullRanksHigh",            ruleProperty.nullRanksHighb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "sequence",                 ruleProperty.sequenceb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "proper",                   ruleProperty.properb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "minimum",                  ruleProperty.minimumi);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "internal",                 ruleProperty.internalb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "propertyBitSet",           ruleProperty.propertyBitSet);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "hideseparator",            ruleProperty.hideseparatorb);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  lua_Integer                  rulei;
  marpaESLIFRuleProperty_t     ruleProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_rulePropertiesByLevel(marpaESLIFGrammarp, leveli, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  rulei = luaL_checkinteger(L, 3);
  lua_pop(L, 4);                                 /* stack: */

  if (! marpaESLIFGrammar_ruleproperty_by_levelb(marpaESLIFGrammarp, (int) rulei, &ruleProperty, (int) leveli, NULL /* descp */)) {
    return luaL_error(L, "marpaESLIFGrammar_ruleproperty_by_levelb failure");
  }

  lua_createtable(L, 18, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "id",                       ruleProperty.idi);
  MARPAESLIFLUA_STORE_STRING       (L, "description",              ruleProperty.descp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "show",                     ruleProperty.asciishows);
  MARPAESLIFLUA_STORE_INTEGER      (L, "lhsId",                    ruleProperty.lhsi);
  MARPAESLIFLUA_STORE_INTEGER      (L, "separatorId",              ruleProperty.separatori);
  MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, "rhsIds",                   ruleProperty.nrhsl, ruleProperty.rhsip);
  MARPAESLIFLUA_STORE_INTEGER      (L, "exceptionId",              ruleProperty.exceptioni);
  MARPAESLIFLUA_STORE_ACTION       (L, "action",                   ruleProperty.actionp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "discardEvent",             ruleProperty.discardEvents);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardEventInitialState", ruleProperty.discardEventb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "rank",                     ruleProperty.ranki);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "nullRanksHigh",            ruleProperty.nullRanksHighb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "sequence",                 ruleProperty.sequenceb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "proper",                   ruleProperty.properb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "minimum",                  ruleProperty.minimumi);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "internal",                 ruleProperty.internalb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "propertyBitSet",           ruleProperty.propertyBitSet);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "hideseparator",            ruleProperty.hideseparatorb);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  symboli;
  marpaESLIFSymbolProperty_t   symbolProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_currentSymbolProperties(marpaESLIFGrammarp, symboli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  symboli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_symbolproperty_currentb(marpaESLIFGrammarp, (int) symboli, &symbolProperty)) {
    return luaL_error(L, "marpaESLIFGrammar_symbolproperty_currentb failure");
  }

  lua_createtable(L, 24, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "type",                       symbolProperty.type);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "start",                      symbolProperty.startb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discard",                    symbolProperty.discardb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardRhs",                 symbolProperty.discardRhsb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "lhs",                        symbolProperty.lhsb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "top",                        symbolProperty.topb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "id",                         symbolProperty.idi);
  MARPAESLIFLUA_STORE_STRING       (L, "description",                symbolProperty.descp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventBefore",                symbolProperty.eventBefores);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventBeforeInitialState",    symbolProperty.eventBeforeb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventAfter",                 symbolProperty.eventAfters);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventAfterInitialState",     symbolProperty.eventAfterb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventPredicted",             symbolProperty.eventPredicteds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventPredictedInitialState", symbolProperty.eventPredictedb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventNulled",                symbolProperty.eventNulleds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventNulledInitialState",    symbolProperty.eventNulledb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventCompleted",             symbolProperty.eventCompleteds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventCompletedInitialState", symbolProperty.eventCompletedb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "discardEvent",               symbolProperty.discardEvents);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardEventInitialState",   symbolProperty.discardEventb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "lookupResolvedLeveli",       symbolProperty.lookupResolvedLeveli);
  MARPAESLIFLUA_STORE_INTEGER      (L, "priority",                   symbolProperty.priorityi);
  MARPAESLIFLUA_STORE_ACTION       (L, "nullableAction",             symbolProperty.nullableActionp);
  MARPAESLIFLUA_STORE_INTEGER      (L, "propertyBitSet",             symbolProperty.propertyBitSet);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  lua_Integer                  symboli;
  marpaESLIFSymbolProperty_t     symbolProperty;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_symbolPropertiesByLevel(marpaESLIFGrammarp, leveli, symboli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, symboli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, symboli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  symboli = luaL_checkinteger(L, 3);
  lua_pop(L, 4);                                 /* stack: */

  if (! marpaESLIFGrammar_symbolproperty_by_levelb(marpaESLIFGrammarp, (int) symboli, &symbolProperty, (int) leveli, NULL /* descp */)) {
    return luaL_error(L, "marpaESLIFGrammar_symbolproperty_by_levelb failure");
  }

  lua_createtable(L, 24, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER      (L, "type",                       symbolProperty.type);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "start",                      symbolProperty.startb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discard",                    symbolProperty.discardb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardRhs",                 symbolProperty.discardRhsb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "lhs",                        symbolProperty.lhsb);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "top",                        symbolProperty.topb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "id",                         symbolProperty.idi);
  MARPAESLIFLUA_STORE_STRING       (L, "description",                symbolProperty.descp);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventBefore",                symbolProperty.eventBefores);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventBeforeInitialState",    symbolProperty.eventBeforeb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventAfter",                 symbolProperty.eventAfters);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventAfterInitialState",     symbolProperty.eventAfterb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventPredicted",             symbolProperty.eventPredicteds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventPredictedInitialState", symbolProperty.eventPredictedb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventNulled",                symbolProperty.eventNulleds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventNulledInitialState",    symbolProperty.eventNulledb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "eventCompleted",             symbolProperty.eventCompleteds);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "eventCompletedInitialState", symbolProperty.eventCompletedb);
  MARPAESLIFLUA_STORE_ASCIISTRING  (L, "discardEvent",               symbolProperty.discardEvents);
  MARPAESLIFLUA_STORE_BOOLEAN      (L, "discardEventInitialState",   symbolProperty.discardEventb);
  MARPAESLIFLUA_STORE_INTEGER      (L, "lookupResolvedLeveli",       symbolProperty.lookupResolvedLeveli);
  MARPAESLIFLUA_STORE_INTEGER      (L, "priority",                   symbolProperty.priorityi);
  MARPAESLIFLUA_STORE_ACTION       (L, "nullableAction",             symbolProperty.nullableActionp);
  MARPAESLIFLUA_STORE_INTEGER      (L, "propertyBitSet",             symbolProperty.propertyBitSet);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          rulei;
  char                 *ruleDisplays;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ruleDisplay(marpaESLIFGrammarp, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, rulei, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, rulei, marpaESLIFGrammarp */
  rulei = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_ruledisplayform_currentb(marpaESLIFGrammarp, (int) rulei, &ruleDisplays)) {
    return luaL_error(L, "marpaESLIFGrammar_ruledisplayform_currentb failure");
  }

  lua_pushstring(L, (const char *) ruleDisplays);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          symboli;
  char                 *symbolDisplays;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_symbolDisplay(marpaESLIFGrammarp, symboli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, symboli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, symboli, marpaESLIFGrammarp */
  symboli = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_symboldisplayform_currentb(marpaESLIFGrammarp, (int) symboli, &symbolDisplays)) {
    return luaL_error(L, "marpaESLIFGrammar_symboldisplayform_currentb failure");
  }

  lua_pushstring(L, (const char *) symbolDisplays);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ruleShowi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleShowi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          rulei;
  char                 *ruleShows;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ruleShow(marpaESLIFGrammarp, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, rulei, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, rulei, marpaESLIFGrammarp */
  rulei = luaL_checkinteger(L, 2);
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_ruleshowform_currentb(marpaESLIFGrammarp, (int) rulei, &ruleShows)) {
    return luaL_error(L, "marpaESLIFGrammar_ruleshowform_currentb failure");
  }

  lua_pushstring(L, (const char *) ruleShows);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          rulei;
  char                 *ruleDisplays;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ruleShow(marpaESLIFGrammarp, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  rulei = luaL_checkinteger(L, 3);
  lua_pop(L, 4);                                 /* stack: */

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(marpaESLIFGrammarp, (int) rulei, &ruleDisplays, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_ruledisplayform_by_levelb");
  }

  lua_pushstring(L, (const char *) ruleDisplays);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          symboli;
  char                 *symbolDisplays;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_symbolShow(marpaESLIFGrammarp, symboli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, symboli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, symboli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  symboli = luaL_checkinteger(L, 3);
  lua_pop(L, 4);                                 /* stack: */

  if (! marpaESLIFGrammar_symboldisplayform_by_levelb(marpaESLIFGrammarp, (int) symboli, &symbolDisplays, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_symboldisplayform_by_levelb");
  }

  lua_pushstring(L, (const char *) symbolDisplays);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          rulei;
  char                 *ruleShows;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_ruleShow(marpaESLIFGrammarp, rulei)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, rulei, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);
  rulei = luaL_checkinteger(L, 3);
  lua_pop(L, 4);                                 /* stack: */

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(marpaESLIFGrammarp, (int) rulei, &ruleShows, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_ruleshowform_by_levelb");
  }

  lua_pushstring(L, (const char *) ruleShows);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_showi(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_show";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  char                *shows;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_show(marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 2);                                 /* stack: */

  if (! marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammarp, &shows)) {
    return luaL_error(L, "marpaESLIFGrammar_grammarshowform_currentb failure");
  }
  lua_pushstring(L, (const char *) shows);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (shows=%p) at %s:%d", funcs, L, shows, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_showByLeveli(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_showByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  char                *shows;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_showByLevel(marpaESLIFGrammarp, leveli)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  leveli = luaL_checkinteger(L, 2);              /* stack: marpaESLIFGrammarTable, leveli, marpaESLIFGrammarp */
  lua_pop(L, 3);                                 /* stack: */

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &shows, (int) leveli, NULL)) {
    return luaL_error(L, "marpaESLIFGrammar_grammarshowform_by_levelb failure");
  }
  lua_pushstring(L, (const char *) shows);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (shows=%p) at %s:%d", funcs, L, shows, FILENAMES, __LINE__);
  return 1;
}

