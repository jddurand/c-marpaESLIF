/* Only 5.2 and upwards is supported */

#include "lua.h"        /* As per CMake doc */
#include "lauxlib.h"    /* As per CMake doc */
#include "lualib.h"     /* As per CMake doc */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIFLua.h>
#include <marpaESLIF.h>
#include <genericStack.h>

/* To ease me debugging -; */
static void marpaESLIFLua_stackdumpv(lua_State* L, int forcelookupi);
#undef  FILENAMES
#define FILENAMES "marpaESLIFLua.c"

/* Global table for the multiton pattern */
#define MARPAESLIFMULTITONS "__marpaESLIFMultiton"
/* Every key   is a marpaESLIFp light userdata */
/* Every value is a reference to the logger (reference to nil if there is none) */

/* marpaESLIFLua correctly unreferences everything from the valuation, but it you */
/* prefer it not to use the global registry, then uncomment the next line -; */
/* #define MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
/* Global table for our references */
#define MARPAESLIFREGISTRYINDEX "__marpaESLIFRegistryindex"
#endif

#define ESLIF_LUA_CONTEXT 2 /* Any value > 0 */

/* Logger proxy context */
typedef struct genericLoggerContext {
  lua_State *L; /* Lua state */
  int logger_r; /* Lua logger reference */
} genericLoggerContext_t;

/* Recognizer proxy context */
typedef struct recognizerContext {
  lua_State              *L;                      /* Lua state */
  int                     grammar_r;              /* Lua grammar reference */
  int                     recognizer_r;           /* Lua recognizer reference */
  int                     recognizer_orig_r;      /* Lua original recognizer reference in case of newFrom() */
  char                   *previousInputs;         /* Because we want to maintain lifetime of inputs lua string outside of stack */
  char                   *previousEncodings;      /* Because we want to maintain lifetime of encodings lua string outside of stack */
  genericStack_t         *lexemeStackp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   canContinueb;
} recognizerContext_t;

/* Value proxy context */
typedef struct valueContext {
  lua_State         *L;                /* Lua state */
  int                value_r;          /* Lua value reference */
  int                recognizer_r;     /* Lua recognizer reference - can be LUA_NOREF */
  int                grammar_r;        /* Lua grammar reference */
  char              *actions;          /* Shallow copy of last resolved name */
  void              *previous_strings; /* Latest stringification result */
  marpaESLIFValue_t *marpaESLIFValuep;
  char              *symbols;
  int                symboli;
  char              *rules;
  int                rulei;
  int                result_r;        /* Reference to last result */
} valueContext_t;

static short                           marpaESLIFLua_paramIsLoggerInterfaceOrNilb(lua_State *L, int stacki);
static short                           marpaESLIFLua_paramIsRecognizerInterfacev(lua_State *L, int stacki);
static short                           marpaESLIFLua_paramIsValueInterfacev(lua_State *L, int stacki);
static void                            marpaESLIFLua_recognizerContextInitv(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int recognizerOrigStacki, recognizerContext_t *recognizerContextp);
static void                            marpaESLIFLua_valueContextInitv(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int valuestacki, valueContext_t *valueContextp);
static void                            marpaESLIFLua_recognizerContextCleanupv(recognizerContext_t *recognizerContextp);
static void                            marpaESLIFLua_recognizerContextFreev(recognizerContext_t *recognizerContextp, short onStackb);
static void                            marpaESLIFLua_valueContextCleanupv(valueContext_t *valueContextp);
static void                            marpaESLIFLua_valueContextFreev(valueContext_t *valueContextp, short onStackb);
static void                            marpaESLIFLua_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static int                             marpaESLIFLua_installi(lua_State *L);
static int                             marpaESLIFLua_versioni(lua_State *L);
static int                             marpaESLIFLua_marpaESLIF_newi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFMultiton_freevi(lua_State *L);
#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
static int                             marpaESLIFLua_marpaESLIFRegistryindex_freevi(lua_State *L);
#endif
static int                             marpaESLIFLua_marpaESLIFGrammar_newi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_freei(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ngrammari(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ruleShowi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_showi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_showByLeveli(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFGrammar_parsei(lua_State *L);
static short                           marpaESLIFLua_readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t   marpaESLIFLua_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t marpaESLIFLua_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueFreeCallback_t   marpaESLIFLua_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                           marpaESLIFLua_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           marpaESLIFLua_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void                            marpaESLIFLua_valueFreeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel);
static short                           marpaESLIFLua_transformUndefb(void *userDatavp, int contexti);
static short                           marpaESLIFLua_transformCharb(void *userDatavp, int contexti, char c);
static short                           marpaESLIFLua_transformShortb(void *userDatavp, int contexti, short b);
static short                           marpaESLIFLua_transformIntb(void *userDatavp, int contexti, int i);
static short                           marpaESLIFLua_transformLongb(void *userDatavp, int contexti, long l);
static short                           marpaESLIFLua_transformFloatb(void *userDatavp, int contexti, float f);
static short                           marpaESLIFLua_transformDoubleb(void *userDatavp, int contexti, double d);
static short                           marpaESLIFLua_transformPtrb(void *userDatavp, int contexti, void *p);
static short                           marpaESLIFLua_transformArrayb(void *userDatavp, int contexti, void *p, size_t sizel);
static short                           marpaESLIFLua_transformBoolb(void *userDatavp, int contexti, short b);
static void                            marpaESLIFLua_pushValuev(valueContext_t *valueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei);
static short                           marpaESLIFLua_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp);
static void                            marpaESLIFLua_iterate_and_print(lua_State *L, int index);
static int                             marpaESLIFLua_marpaESLIFRecognizer_newi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFRecognizer_freei(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFRecognizer_newFromi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFRecognizer_sharei(lua_State *L);
static int                             marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei(lua_State *L);

/* Transformers */
static marpaESLIFValueResultTransform_t marpaESLIFValueResultTransformDefault = {
  marpaESLIFLua_transformUndefb,
  marpaESLIFLua_transformCharb,
  marpaESLIFLua_transformShortb,
  marpaESLIFLua_transformIntb,
  marpaESLIFLua_transformLongb,
  marpaESLIFLua_transformFloatb,
  marpaESLIFLua_transformDoubleb,
  marpaESLIFLua_transformPtrb,
  marpaESLIFLua_transformArrayb,
  marpaESLIFLua_transformBoolb
};

#define MARPAESLIFLUA_NOOP

#define MARPAESLIFLUA_GETORCREATEGLOBAL(L, name, gcp) do {              \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) getting global table name=%s gcp=%p at %s:%d", funcs, L, name, gcp, FILENAMES, __LINE__); \
    lua_getglobal(L, name);                                             \
    if (lua_type(L, -1) != LUA_TTABLE) {                                \
      lua_pop(L, 1);                                                    \
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) creating global table name=%s gcp=%p at %s:%d", funcs, L, name, gcp, FILENAMES, __LINE__); \
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

#define MARPAESLIFLUA_TRAVERSE_TABLE(L, name, t) do {                   \
    int _t = lua_absindex(L, t);                                        \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s at indice %d at %s:%d", funcs, L, name, _t, FILENAMES, __LINE__); \
    lua_pushnil(L);                                                     \
    while (lua_next(L, _t) != 0) {                                      \
      switch (lua_type(L, -2)) {                                        \
      case LUA_TNIL:                                                    \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: nil - %s at %s:%d", funcs, L, name, lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      case LUA_TNUMBER:                                                 \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: number %g - %s at %s:%d", funcs, L, name, lua_tonumber(L, -2), lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      case LUA_TBOOLEAN:                                                \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: boolean %s - %s at %s:%d", funcs, L, name, lua_toboolean(L, -2) ? "true" : "false", lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      case LUA_TUSERDATA:                                               \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: userdata %p - %s at %s:%d", funcs, L, name, lua_touserdata(L, -2), lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      case LUA_TLIGHTUSERDATA:                                          \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: light userdata %p - %s at %s:%d", funcs, L, name, lua_touserdata(L, -2), lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      case LUA_TSTRING:                                                 \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: string %s - %s at %s:%d", funcs, L, name, lua_tostring(L, -2), lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      default:                                                          \
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) TABLE_DUMP of %s: %s - %s at %s:%d", funcs, L, name, lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)), FILENAMES, __LINE__); \
        break;                                                          \
      }                                                                 \
      lua_pop(L, 1);                                                    \
    }                                                                   \
  } while (0)

#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
#define MARPAESLIFLUA_REF(L, refi) do {                                 \
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFREGISTRYINDEX, marpaESLIFLua_marpaESLIFRegistryindex_freevi); \
    lua_insert(L, -2);                                                  \
    refi = luaL_ref(L, -2);                                             \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) set global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, MARPAESLIFREGISTRYINDEX, -1); */ \
    lua_pop(L, 1);                                                      \
  } while (0);

#define MARPAESLIFLUA_UNREF(L, refi) do {                               \
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFREGISTRYINDEX, marpaESLIFLua_marpaESLIFRegistryindex_freevi); \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) del global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    luaL_unref(L, -1, refi);                                            \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, MARPAESLIFREGISTRYINDEX, -1); */ \
    lua_pop(L, 1);                                                      \
  } while (0);

#define MARPAESLIFLUA_DEREF(L, refi) do {                               \
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFREGISTRYINDEX, marpaESLIFLua_marpaESLIFRegistryindex_freevi); \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) use global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    lua_rawgeti(L, -1, refi);                                           \
    lua_remove(L, -2);                                                  \
  } while (0);

#else /* MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

#define MARPAESLIFLUA_REF(L, refi) do {                                 \
    refi = luaL_ref(L, LUA_REGISTRYINDEX);                              \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) set global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, "LUA_REGISTRYINDEX", LUA_REGISTRYINDEX); */  \
  } while (0);

#define MARPAESLIFLUA_UNREF(L, refi) do {                               \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) del global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    luaL_unref(L, LUA_REGISTRYINDEX, refi);                             \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, "LUA_REGISTRYINDEX", LUA_REGISTRYINDEX); */  \
  } while (0);

#define MARPAESLIFLUA_DEREF(L, refi) do {                               \
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) use global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); \
    lua_rawgeti(L, LUA_REGISTRYINDEX, refi);                            \
  } while (0);

#endif /* MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

/* For every MARPAESLIFLUA_STORE_xxx macro, destination table is assumed to be at the top of the stack */
#define MARPAESLIFLUA_STORE_BY_KEY(L, key, valueproducer) do {          \
    valueproducer                                                       \
    lua_setfield(L, -2, key);                                           \
  } while (0)

#define MARPAESLIFLUA_STORE_FUNCTION(L, key, functionp)                 \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, if (functionp == NULL) { lua_pushnil(L); } else { lua_pushcfunction(L, functionp); })

#define MARPAESLIFLUA_STORE_USERDATA(L, key, p)                         \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, if (p == NULL)         { lua_pushnil(L); } else { lua_pushlightuserdata(L, p); })

#define MARPAESLIFLUA_STORE_STRING(L, key, stringp)                     \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, if (stringp == NULL)   { lua_pushnil(L); } else { lua_pushlstring(L, (const char *) stringp->bytep, stringp->bytel); })

#define MARPAESLIFLUA_STORE_ASCIISTRING(L, key, asciis)                 \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, if (asciis == NULL)    { lua_pushnil(L); } else { lua_pushstring(L, asciis); })

#define MARPAESLIFLUA_STORE_INTEGER(L, key, i)                          \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, lua_pushinteger(L, (lua_Integer) i);)

#define MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, integerl, integerp) do {    \
    size_t _iteratorl;                                                  \
                                                                        \
    lua_createtable(L, (int) integerl, 0);                              \
    for (_iteratorl = 0; _iteratorl < integerl; _iteratorl++) {         \
      lua_pushinteger(L, (lua_Integer) integerp[_iteratorl]);           \
      lua_rawseti(L, -2, (lua_Integer) _iteratorl);                     \
    }                                                                   \
  } while (0)

#define MARPAESLIFLUA_STORE_INTEGER_ARRAY(L, key, integerl, integerp)   \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, MARPAESLIFLUA_PUSH_INTEGER_ARRAY(L, integerl, integerp);)

#define MARPAESLIFLUA_STORE_BOOLEAN(L, key, b)                          \
  MARPAESLIFLUA_STORE_BY_KEY(L, key, lua_pushboolean(L, (int) b);)

#define MARPAESLIFLUA_STORE_ACTION(L, key, actionp)                     \
  MARPAESLIFLUA_STORE_BY_KEY(L, key,                                    \
                             if (actionp != NULL) {                     \
                               switch (actionp->type) {                 \
                               case MARPAESLIF_ACTION_TYPE_NAME:        \
                                 lua_pushstring(L, actionp->u.names);   \
                                 break;                                 \
                               case MARPAESLIF_ACTION_TYPE_STRING:      \
                                 lua_pushlstring(L, (const char *) actionp->u.stringp->bytep, actionp->u.stringp->bytel); \
                                 break;                                 \
                               default:                                 \
                                 return luaL_error(L, "Unsupported action type %d", actionp->type); \
                               }                                        \
                             } else {                                   \
                               lua_pushnil(L);                          \
                             }                                          \
                             )

/* This is vicious, but here it is: we assume that EVERY callback function refers to an object */
/* i.e. a function that has "self" as the first parameter. So nargs is always nargs+1 in reality */
/* This is the reason for lua_insert(L, -2) instead of lua_remove(L, -2) */
#define MARPAESLIFLUA_CALLBACK(L, interface_r, funcs, nargs, parameters) do { \
    MARPAESLIFLUA_DEREF(L, interface_r);                                \
    lua_getfield(L, -1, funcs);                                         \
    if (lua_type(L, -1) != LUA_TFUNCTION) {                             \
      luaL_error(L, "No such function %s", funcs);                      \
    }                                                                   \
    lua_insert(L, -2);                                                  \
    parameters                                                          \
    lua_call(L, nargs + 1, LUA_MULTRET);                                \
} while (0)
    
#define MARPAESLIFLUA_CALLBACKV(L, interface_r, funcs, nargs, parameters) do { \
  int _topi;                                                            \
                                                                        \
  _topi = lua_gettop(L);                                                \
  MARPAESLIFLUA_CALLBACK(L, interface_r, funcs, nargs, parameters);     \
  lua_settop(L, _topi);                                                 \
} while (0)
    
#define MARPAESLIFLUA_CALLBACKB(L, interface_r, funcs, nargs, parameters, bp) do { \
    int _topi;                                                          \
    int _newtopi;                                                       \
    int _typei;                                                         \
                                                                        \
    _topi = lua_gettop(L);                                              \
    MARPAESLIFLUA_CALLBACK(L, interface_r, funcs, nargs, parameters);   \
    _newtopi = lua_gettop(L);                                           \
    if (_newtopi != (_topi + 1)) {                                      \
      return luaL_error(L, "Function %s must return exactly one value", funcs); \
    }                                                                   \
    if ((_typei = lua_type(L, -1)) != LUA_TBOOLEAN) {                   \
      return luaL_error(L, "Function %s must return a boolean value, got %s", funcs, lua_typename(L, _typei)); \
    }                                                                   \
    if (bp != NULL) {                                                   \
      if (lua_toboolean(L, -1) != 0) {                                  \
        *bp = 1;                                                        \
      } else {                                                          \
        *bp = 0;                                                        \
      }                                                                 \
    }                                                                   \
    lua_settop(L, _topi);                                               \
} while (0)

/* Value is at the top of the stack and is popped by this macro */
/* We store at "indice" a pointer to an integer that contains a reference to the value */
#define MARPAESLIFLUA_SET_VALUE(valueContextp, marpaESLIFValuep, indicei, stringificationp) do { \
    marpaESLIFValueResult_t _marpaESLIFValueResult;                     \
    int *_p;                                                            \
                                                                        \
    _p = malloc(sizeof(int));                                           \
    if (_p == NULL) {                                                   \
      return luaL_error(L, "malloc failure, %s", strerror(errno));      \
    }                                                                   \
                                                                        \
    lua_pushnil(valueContextp->L);                                      \
    lua_copy(valueContextp->L, -2, -1);                                 \
    MARPAESLIFLUA_REF(valueContextp->L, *_p);                           \
                                                                        \
    GENERICLOGGER_NOTICEF(NULL, "... Storing pointer %p to global reference %d to stack indice %d at %s:%d", _p, *_p, indicei, FILENAMES, __LINE__); \
                                                                        \
    _marpaESLIFValueResult.contexti        = ESLIF_LUA_CONTEXT;         \
    _marpaESLIFValueResult.sizel           = sizeof(int);               \
    _marpaESLIFValueResult.representationp = stringificationp;          \
    _marpaESLIFValueResult.shallowb        = 0;                         \
    _marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_PTR; \
    _marpaESLIFValueResult.u.p             = _p;                        \
                                                                        \
    if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, indicei, &_marpaESLIFValueResult)) { \
      return luaL_error(L, "marpaESLIFValue_stack_setb failure, %s", strerror(errno)); \
    }                                                                   \
                                                                        \
  } while (0)

#define MARPAESLIFLUA_CALLBACKI(L, interface_r, funcs, nargs, parameters, ip) do { \
    int _topi;                                                          \
    int _newtopi;                                                       \
    int _isnum;                                                         \
    int _typei;                                                         \
    lua_Integer _i;                                                     \
                                                                        \
    _topi = lua_gettop(L);                                              \
    MARPAESLIFLUA_CALLBACK(L, interface_r, funcs, nargs, parameters);   \
    _newtopi = lua_gettop(L);                                           \
    if (_newtopi != (_topi + 1)) {                                      \
      return luaL_error(L, "Function %s must return exactly one value", funcs); \
    }                                                                   \
    if ((_typei = lua_type(L, -1)) != LUA_TNUMBER) {                    \
      return luaL_error(L, "Function %s must return a number value, got %s", funcs, lua_typename(L, _typei)); \
    }                                                                   \
    if (ip != NULL) {                                                   \
      _i = lua_tointegerx(L, -1, &_isnum);                              \
      if (! _isnum) {                                                   \
        return luaL_error(L, "Convertion to a number failed");          \
      }                                                                 \
      *ip = (int) _i;                                                   \
    }                                                                   \
    lua_settop(L, _topi);                                               \
} while (0)

/* Take care: if *sp is != NULL outside of the scope, caller's responsibility to free it */
#define MARPAESLIFLUA_CALLBACKS(L, interface_r, funcs, nargs, parameters, sp, lp) do { \
    int _topi;                                                          \
    int _newtopi;                                                       \
    char *_s;                                                           \
    size_t _l;                                                          \
    int _typei;                                                         \
                                                                        \
    _topi = lua_gettop(L);                                              \
    MARPAESLIFLUA_CALLBACK(L, interface_r, funcs, nargs, parameters);   \
    _newtopi = lua_gettop(L);                                           \
    if (_newtopi != (_topi + 1)) {                                      \
      return luaL_error(L, "Function %s must return exactly one value", funcs); \
    }                                                                   \
    switch (_typei = lua_type(L, -1)) {                                 \
    case LUA_TNIL:                                                      \
      *sp = NULL;                                                       \
      *lp = 0;                                                          \
      break;                                                            \
    case LUA_TSTRING:                                                   \
      _s = (char *) lua_tolstring(L, -1, &_l);                          \
      if (_s != NULL) {                                                 \
        *sp = malloc(_l);                                               \
        if (*sp == NULL) {                                              \
          return luaL_error(L, "malloc failure, %s", strerror(errno));  \
        }                                                               \
        memcpy(*sp, _s, _l);                                            \
        *lp = _l;                                                       \
      } else {                                                          \
        *sp = NULL;                                                     \
        *lp = 0;                                                        \
      }                                                                 \
      break;                                                            \
    default:                                                            \
      return luaL_error(L, "Function %s must return a string value or nil, got %s", funcs, lua_typename(L, _typei)); \
      break;                                                            \
    }                                                                   \
    lua_settop(L, _topi);                                               \
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
    {"marpaESLIFGrammar_parse", marpaESLIFLua_marpaESLIFGrammar_parsei},
    {"marpaESLIFRecognizer_new", marpaESLIFLua_marpaESLIFRecognizer_newi},
    {"marpaESLIFRecognizer_newFrom", marpaESLIFLua_marpaESLIFRecognizer_newFromi},
    {"marpaESLIFRecognizer_sharei", marpaESLIFLua_marpaESLIFRecognizer_sharei},
    {"marpaESLIFRecognizer_isCanContinue", marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei},
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
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIF_new([logger]");
  }

  MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFMULTITONS, marpaESLIFLua_marpaESLIFMultiton_freevi); /* stack: logger?, MARPAESLIFMULTITONS */

  /* Look if MARPAESLIFMULTITONS already contains a reference to logger */
  lua_pushnil(L);                                                                                   /* stack: logger?, MARPAESLIFMULTITONS, nil */
  while (lua_next(L, -2) != 0) {                                                                    /* stack: logger?, MARPAESLIFMULTITONS, marpaESLIFp, r */
    logger_r = (int) lua_tointeger(L, -1);
    if (logger_r == LUA_NOREF) {
      lua_pushnil(L);                                                                               /* stack: logger?, MARPAESLIFMULTITONS, marpaESLIFp, r, nil */
    } else {
      MARPAESLIFLUA_DEREF(L, logger_r);                                                             /* stack: logger?, MARPAESLIFMULTITONS, marpaESLIFp, r, loggerp_from_registry */
    }
    if (((! loggerb) && lua_isnil(L, -1))
        ||
        ((  loggerb) && lua_compare(L, 1, -1, LUA_OPEQ))) {
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, -3);
      lua_pop(L, 3);                                                                                /* stack: logger?, MARPAESLIFMULTITONS */
      break;
    }
    lua_pop(L, 2);                                                                                  /* stack: logger?, MARPAESLIFMULTITONS, marpaESLIFp */
  }

  if (marpaESLIFp == NULL) {
    if (loggerb) {
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        return luaL_error(L, "malloc failure, %s\n", strerror(errno));
      }

      genericLoggerContextp->L = NULL;
      genericLoggerContextp->logger_r = LUA_NOREF;

      /* Get logger reference */
      lua_pushnil(L);                                                                               /* stack: logger, MARPAESLIFMULTITONS, nil */
      lua_copy(L, 1, -1);                                                                           /* stack: logger, MARPAESLIFMULTITONS, logger */
      MARPAESLIFLUA_REF(L, logger_r);                                                               /* stack: logger, MARPAESLIFMULTITONS */
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__);

      /* Fill genericLogger context */
      genericLoggerContextp->L = L;
      genericLoggerContextp->logger_r = logger_r;
      genericLoggerp = genericLogger_newp(marpaESLIFLua_genericLoggerCallbackv, genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__);
        MARPAESLIFLUA_UNREF(L, logger_r);
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
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, logger_r); /* No effect if it is LUA_NOREF */
      free(genericLoggerContextp);
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    /* Link marpaESLIFp to logger_r */
    lua_pushinteger(L, (lua_Integer) logger_r);                                                     /* stack: logger?, MARPAESLIFMULTITONS, logger_r */
    lua_rawsetp(L, -2, (const void *) marpaESLIFp);                                                 /* stack: logger?, MARPAESLIFMULTITONS */
    lua_pop(L, 1);                                                                                  /* stack: logger? */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
  } else {
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p (reuse) at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__);
  }

  if (loggerb) {
    lua_pop(L, 1);                                                                                  /* stack */
  }

  lua_pushlightuserdata(L, marpaESLIFp);                                                            /* stack: marpaESLIFp */

  /* Create a metable */
  lua_newtable(L);
  lua_newtable(L);                                                                                  /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFGrammar_new", marpaESLIFLua_marpaESLIFGrammar_newi);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                                          /* stack: marpaESLIFp meta={...} */

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

  if (lua_type(L, stacki) == LUA_TNIL) {
    loggerb = 0;
  } else {
    /* Verify that the logger can do all wanted methods */
    if (lua_type(L, stacki) != LUA_TTABLE) {
      return luaL_error(L, "logger interface must be a table");
    }
    for (i = 0; i < sizeof(loggerFunctions)/sizeof(loggerFunctions[0]); i++) {
      lua_getfield(L, stacki, loggerFunctions[i]);                             /* stack: stack1, ..., stacki, field */
      if (lua_type(L, -1) != LUA_TFUNCTION) {
        lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
        return luaL_error(L, "logger table must have a field named '%s' that is a function", loggerFunctions[i]);
      } else {
        lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
      }
    }
    loggerb = 1;
  }

  return loggerb;
}

/****************************************************************************/
static short marpaESLIFLua_paramIsRecognizerInterfacev(lua_State *L, int stacki)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_paramIsRecognizerInterfacev";
  static const char *recognizerFunctions[] = {
    "read",
    "isEof",
    "isCharacterStream",
    "encoding",
    "data",
    "isWithDisableThreshold",
    "isWithExhaustion",
    "isWithNewline",
    "isWithTrack"
  };
  int                i;

  /* Verify that the recognizer can do all wanted methods */
  if (lua_type(L, stacki) != LUA_TTABLE) {
    return luaL_error(L, "recognizer interface must be a table");
  }
  for (i = 0; i < sizeof(recognizerFunctions)/sizeof(recognizerFunctions[0]); i++) {
    lua_getfield(L, stacki, recognizerFunctions[i]);                             /* stack: stack1, ..., stacki, field */
    if (lua_type(L, -1) != LUA_TFUNCTION) {
      lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
      return luaL_error(L, "recognizer table must have a field named '%s' that is a function", recognizerFunctions[i]);
    } else {
      lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
    }
  }

  return 1;
}

/****************************************************************************/
static short marpaESLIFLua_paramIsValueInterfacev(lua_State *L, int stacki)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_paramIsValueInterfacev";
  static const char *valueFunctions[] = {
    "isWithHighRankOnly",
    "isWithOrderByRank",
    "isWithAmbiguous",
    "isWithNull",
    "maxParses",
    "setResult",
    "getResult"
  };
  int                i;

  /* Verify that the recognizer can do all wanted methods */
  if (lua_type(L, stacki) != LUA_TTABLE) {
    return luaL_error(L, "value interface must be a table");
  }
  for (i = 0; i < sizeof(valueFunctions)/sizeof(valueFunctions[0]); i++) {
    lua_getfield(L, stacki, valueFunctions[i]);                             /* stack: stack1, ..., stacki, field */
    if (lua_type(L, -1) != LUA_TFUNCTION) {
      lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
      return luaL_error(L, "value table must have a field named '%s' that is a function", valueFunctions[i]);
    } else {
      lua_pop(L, 1);                                                         /* stack: stack1, ..., stacki */
    }
  }

  return 1;
}

/****************************************************************************/
static void  marpaESLIFLua_recognizerContextInitv(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int recognizerOrigStacki, recognizerContext_t *recognizerContextp)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_recognizerContextInitv";

  recognizerContextp->L = L;
  /* Get grammar reference */
  if (grammarStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, grammarStacki, -1);                                 /* stack: xxx, grammarTable */
    MARPAESLIFLUA_REF(L, recognizerContextp->grammar_r);            /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got grammar_r=%d from registry at %s:%d", funcs, L, recognizerContextp->grammar_r, FILENAMES, __LINE__);
  } else {
    luaL_error(L, "grammarStacki must be != 0");
  }
  /* Get recognizer reference */
  if (recognizerInterfaceStacki != 0) {
    lua_pushnil(L);                                                    /* stack: xxx, nil */
    lua_copy(L, recognizerInterfaceStacki, -1);                        /* stack: xxx, recognizerInterface */
    MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_r);            /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_r, FILENAMES, __LINE__);
  }
  /* Get original recognizer reference (in case of newFrom()) */
  if (recognizerOrigStacki != 0) {
    lua_pushnil(L);                                                         /* stack: xxx, nil */
    lua_copy(L, recognizerOrigStacki, -1);                                  /* stack: xxx, recognizerOrigInterface */
    MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_orig_r);            /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__);
  } else {
    recognizerContextp->recognizer_orig_r = LUA_NOREF;
  }
  recognizerContextp->previousInputs        = NULL;
  recognizerContextp->previousEncodings     = NULL;
  recognizerContextp->lexemeStackp          = NULL;
  recognizerContextp->marpaESLIFRecognizerp = NULL;
  recognizerContextp->canContinueb          = 0;
}

/****************************************************************************/
static void  marpaESLIFLua_recognizerContextCleanupv(recognizerContext_t *recognizerContextp)
/****************************************************************************/
{
  if (recognizerContextp != NULL) {
    if (recognizerContextp->previousInputs != NULL) {
      free(recognizerContextp->previousInputs);
      recognizerContextp->previousInputs = NULL;
    }
    if (recognizerContextp->previousEncodings != NULL) {
      free(recognizerContextp->previousEncodings);
      recognizerContextp->previousEncodings = NULL;
    }
  }
}

/****************************************************************************/
static void  marpaESLIFLua_recognizerContextFreev(recognizerContext_t *recognizerContextp, short onStackb)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_recognizerContextFreev";
  lua_State         *L;
  genericStack_t    *lexemeStackp;
  int                i;
  int                refi;

  if (recognizerContextp != NULL) {
    L = recognizerContextp->L;

    marpaESLIFLua_recognizerContextCleanupv(recognizerContextp);

    if (recognizerContextp->grammar_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing grammar_r=%d from registry at %s:%d", funcs, L, recognizerContextp->grammar_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, recognizerContextp->grammar_r);
    }

    if (recognizerContextp->recognizer_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, recognizerContextp->recognizer_r);
    }

    if (recognizerContextp->recognizer_orig_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, recognizerContextp->recognizer_orig_r);
    }

    lexemeStackp = recognizerContextp->lexemeStackp;
    if (lexemeStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (GENERICSTACK_USED(lexemeStackp) > 0) {
        /* Last indice ... */
        i = GENERICSTACK_USED(lexemeStackp) - 1;
        /* ... is cleared ... */
        if (GENERICSTACK_IS_INT(lexemeStackp, i)) {
          refi = GENERICSTACK_GET_INT(lexemeStackp, i);
          GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing lexeme reference=%d from registry at %s:%d", funcs, L, refi, FILENAMES, __LINE__);
          MARPAESLIFLUA_UNREF(L, refi);
        }
        /* ... and becomes current used size */
        GENERICSTACK_USED(lexemeStackp) = i;
      }
      GENERICSTACK_FREE(recognizerContextp->lexemeStackp);
    }
    if (recognizerContextp->marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizer_freev(recognizerContextp->marpaESLIFRecognizerp);
    }

    if (! onStackb) {
      free(recognizerContextp);
    }
  }
}

/****************************************************************************/
static void  marpaESLIFLua_valueContextCleanupv(valueContext_t *valueContextp)
/****************************************************************************/
{
  if (valueContextp != NULL) {
    if (valueContextp->previous_strings != NULL) {
      free(valueContextp->previous_strings);
      valueContextp->previous_strings = NULL;
    }
  }
}

/****************************************************************************/
static void  marpaESLIFLua_valueContextFreev(valueContext_t *valueContextp, short onStackb)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_valueContextFreev";
  lua_State         *L;
  int                i;
  int                refi;

  if (valueContextp != NULL) {
    L = valueContextp->L;

    marpaESLIFLua_valueContextCleanupv(valueContextp);

    if (valueContextp->marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(valueContextp->marpaESLIFValuep);
    }

    /* Decrement dependencies */
    if (valueContextp->value_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing value_r=%d from registry at %s:%d", funcs, L, valueContextp->value_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, valueContextp->value_r);
    }
    if (valueContextp->recognizer_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_r=%d from registry at %s:%d", funcs, L, valueContextp->recognizer_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, valueContextp->recognizer_r);
    }
    if (valueContextp->grammar_r != LUA_NOREF) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing grammar_r=%d from registry at %s:%d", funcs, L, valueContextp->grammar_r, FILENAMES, __LINE__);
      MARPAESLIFLUA_UNREF(L, valueContextp->grammar_r);
    }

    if (! onStackb) {
      free(valueContextp);
    }
  }
}

/****************************************************************************/
static void  marpaESLIFLua_valueContextInitv(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int valuestacki, valueContext_t *valueContextp)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_valueContextInitv";

  valueContextp->L                = L;
  /* Get value reference */
  if (valuestacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, valuestacki, -1);                                   /* stack: xxx, valueInterface */
    MARPAESLIFLUA_REF(L, valueContextp->value_r);                   /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got value_r=%d from registry at %s:%d", funcs, L, valueContextp->value_r, FILENAMES, __LINE__);
  } else {
    luaL_error(L, "valuestacki must be != 0");
  }
  /* Get recognizer reference */
  if (recognizerInterfaceStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, recognizerInterfaceStacki, -1);                     /* stack: xxx, recognizerInterface */
    MARPAESLIFLUA_REF(L, valueContextp->recognizer_r);              /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_r=%d from registry at %s:%d", funcs, L, valueContextp->recognizer_r, FILENAMES, __LINE__);
  } else {
    luaL_error(L, "recognizerInterfaceStacki must be != 0");
  }
  /* Get grammar reference */
  if (grammarStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, grammarStacki, -1);                                 /* stack: xxx, grammarInterface */
    MARPAESLIFLUA_REF(L, valueContextp->grammar_r);                 /* stack: xxx */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got grammar_r=%d from registry at %s:%d", funcs, L, valueContextp->grammar_r, FILENAMES, __LINE__);
  } else {
    luaL_error(L, "grammarStacki must be != 0");
  }
  valueContextp->actions          = NULL;
  valueContextp->previous_strings = NULL;
  valueContextp->marpaESLIFValuep = NULL;
  valueContextp->symbols          = NULL;
  valueContextp->symboli          = -1;
  valueContextp->rules            = NULL;
  valueContextp->rulei            = -1;
  valueContextp->result_r         = LUA_NOREF;
}

/****************************************************************************/
static void marpaESLIFLua_genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_genericLoggerCallbackv";
  genericLoggerContext_t *genericLoggerContext = (genericLoggerContext_t *) userDatavp;
  int                     logger_r             = genericLoggerContext->logger_r;
  lua_State              *L                    = genericLoggerContext->L;
  const char             *loggerfuncs;

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    loggerfuncs = "trace";
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    loggerfuncs = "debug";
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    loggerfuncs = "info";
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    loggerfuncs = "notice";
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    loggerfuncs = "warning";
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
    loggerfuncs = "error";
    break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
    loggerfuncs = "critical";
    break;
  case GENERICLOGGER_LOGLEVEL_ALERT:
    loggerfuncs = "alert";
    break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    loggerfuncs = "emergency";
    break;
  default:
    loggerfuncs = NULL;
    break;
  }

  if (loggerfuncs != NULL) {
    MARPAESLIFLUA_CALLBACKV(L, logger_r, loggerfuncs, 1 /* nargs */, lua_pushstring(L, msgs););
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
          GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, genericLoggerContextp->logger_r, FILENAMES, __LINE__);
          MARPAESLIFLUA_UNREF(L, genericLoggerContextp->logger_r); /* By construction genericLoggerContextp->logger_r == logger_r */
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

#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
/****************************************************************************/
static int marpaESLIFLua_marpaESLIFRegistryindex_freevi(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIFRegistryindex_freevi";

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);

  return 0;
}
#endif /* MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_newi(lua_State *L)
/****************************************************************************/
{
  static const char          *funcs = "marpaESLIFLua_marpaESLIFGrammar_newi";
  marpaESLIF_t               *marpaESLIFp;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp;
  int                         ngrammari;
  int                         i;
  marpaESLIFAction_t          defaultFreeAction;
  marpaESLIFGrammarDefaults_t marpaESLIFGrammarDefaults;
  marpaESLIFGrammarOption_t   marpaESLIFGrammarOption = {
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

  /* We want to take control over the free default action, and put something that is illegal via normal parse */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    return luaL_error(L, "marpaESLIFGrammar_ngrammarib failure, %s", strerror(errno));
  }
  for (i = 0; i < ngrammari; i++) {
    if (! marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      return luaL_error(L, "marpaESLIFGrammar_defaults_by_levelb failure, %s", strerror(errno));
    }
    defaultFreeAction.type    = MARPAESLIF_ACTION_TYPE_NAME;
    defaultFreeAction.u.names = ":defaultFreeActions";
    marpaESLIFGrammarDefaults.defaultFreeActionp = &defaultFreeAction;
    if (! marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammarp, &marpaESLIFGrammarDefaults, i, NULL /* descp */)) {
      return luaL_error(L, "marpaESLIFGrammar_defaults_by_level_setb failure, %s", strerror(errno));
    }
  }

  /* Clear the stack */
  lua_settop(L, 0);

  /* We will return a table */
  lua_newtable(L);                                                                            /* stack: {} */
  MARPAESLIFLUA_STORE_BY_KEY(L, MARPAESLIFMULTITONS, lua_getglobal(L, MARPAESLIFMULTITONS);); /* stack: {["MARPAESLIFMULTITONS"] => MARPAESLIFMULTITONS} */
  MARPAESLIFLUA_STORE_USERDATA(L, "marpaESLIFGrammarp", marpaESLIFGrammarp);                  /* stack: {["marpaESLIFGrammarp"] =>marpaESLIFGrammarp, ["MARPAESLIFMULTITONS"] => MARPAESLIFMULTITONS} */

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
  MARPAESLIFLUA_STORE_FUNCTION(L, "parse", marpaESLIFLua_marpaESLIFGrammar_parsei);
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFRecognizer_new", marpaESLIFLua_marpaESLIFRecognizer_newi);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                                    /* stack: {["marpaESLIFGrammarp"] =>marpaESLIFGrammarp, ["MARPAESLIFMULTITONS"] => MARPAESLIFMULTITONS} meta=>{...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({marpaESLIFGrammarp=%p}) at %s:%d", funcs, L, marpaESLIFGrammarp, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFGrammar_freei(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_freei";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  lua_getfield(L, -1, "marpaESLIFGrammarp"); /* stack: {...}, marpaESLIFGrammarp */
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFGrammarp=%p at %s:%d", funcs, L, marpaESLIFGrammarp, FILENAMES, __LINE__);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);

  lua_pop(L, 1);                             /* stack: */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 0;
}

/****************************************************************************/
static void marpaESLIFLua_stackdumpv(lua_State* L, int forcelookupi)
/****************************************************************************/
/* Reference: https://groups.google.com/forum/#!topic/lua5/gc3Ghjo6ipg      */
/****************************************************************************/
{
  int topi   = lua_gettop(L);
  int starti = forcelookupi ? forcelookupi : 1;
  int endi   = forcelookupi ? forcelookupi : topi;
  int i;

  if (! forcelookupi) {
    printf("total in stack %d\n", topi);
  }
  
  for (i = starti; i <= endi; i++) {
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TNIL:
      printf("  [%d] nil\n", i);
      break;
    case LUA_TNUMBER:
      printf("  [%d] number: %g\n", i, lua_tonumber(L, i));
      break;
    case LUA_TBOOLEAN:
      printf("  [%d] boolean %s\n", i, lua_toboolean(L, i) ? "true" : "false");
      break;
    case LUA_TUSERDATA:
      printf("  [%d] userdata: %p\n", i, lua_touserdata(L, i));
      break;
    case LUA_TLIGHTUSERDATA:
      printf("  [%d] light userdata: %p\n", i, lua_touserdata(L, i));
      break;
      /*
    case LUA_TSTRING:
      printf("  [%d] string: '%s'\n", i, lua_tostring(L, i));
      break;
      */
    default:  /* other values */
      printf("  [%d] %s\n", i, lua_typename(L, t));
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
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_showi";
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
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_showByLeveli";
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

/****************************************************************************/
static int  marpaESLIFLua_marpaESLIFGrammar_parsei(lua_State *L)
/****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFGrammar_parsei";
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
  recognizerContext_t           recognizerContext;
  valueContext_t                valueContext;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  int                           rci;
  int                           resultstacki;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_parse(marpaESLIFGrammarp, recognizerInterface, valueInterface)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface, marpaESLIFGrammarp */
  lua_pop(L, 1);                                 /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface */

  marpaESLIFLua_paramIsRecognizerInterfacev(L, 2);
  marpaESLIFLua_paramIsValueInterfacev(L, 3);

  marpaESLIFLua_recognizerContextInitv(L, 1 /* grammarStacki */, 2 /* recognizerInterfaceStacki */, 0 /* recognizerOrigStacki */, &recognizerContext);
  marpaESLIFLua_valueContextInitv(L, 1 /* grammarStacki */, 2 /* recognizerInterfaceStacki */, 3 /* valuestacki */, &valueContext);
  
  lua_pop(L, 3);                                 /* stack */

  marpaESLIFRecognizerOption.userDatavp        = &recognizerContext;
  marpaESLIFRecognizerOption.readerCallbackp   = marpaESLIFLua_readerCallbackb;
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithDisableThreshold", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.disableThresholdb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithExhaustion",       0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.exhaustedb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithNewline",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.newlineb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithTrack",            0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.trackb));
  marpaESLIFRecognizerOption.bufsizl           = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */

  marpaESLIFValueOption.userDatavp             = &valueContext;
  marpaESLIFValueOption.ruleActionResolverp    = marpaESLIFLua_valueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp  = marpaESLIFLua_valueSymbolActionResolver;
  marpaESLIFValueOption.freeActionResolverp    = marpaESLIFLua_valueFreeActionResolver;
  marpaESLIFValueOption.transformerp           = &marpaESLIFValueResultTransformDefault;
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithHighRankOnly", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.highRankOnlyb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithOrderByRank",  0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.orderByRankb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithAmbiguous",    0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.ambiguousb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithNull",         0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.nullb));
  MARPAESLIFLUA_CALLBACKI(L, valueContext.value_r, "maxParses",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.maxParsesi));

  if ((rci = marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL)) != 0) {
    resultstacki = lua_gettop(L);
    /* marpaESLIFGrammar_parseb called the transformers that pushed the final value to the stack */
    MARPAESLIFLUA_CALLBACKV(L, valueContext.value_r, "setResult", 1 /* nargs */, lua_pushnil(L); lua_copy(L, resultstacki, -1););
    lua_pop(L, 1);
  }

  marpaESLIFLua_valueContextFreev(&valueContext, 1 /* onStackb */);
  marpaESLIFLua_recognizerContextFreev(&recognizerContext, 1 /* onStackb */);

  lua_pushboolean(L, rci);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  static const char         *funcs              = "marpaESLIFLua_readerCallbackb";
  recognizerContext_t       *recognizerContextp = (recognizerContext_t *) userDatavp;
  lua_State                 *L                  = recognizerContextp->L;
  int                        recognizer_r       = recognizerContextp->recognizer_r;
  char                      *inputs             = NULL;
  char                      *encodings          = NULL;
  size_t                     inputl;
  size_t                     encodingl;
  short                      callbackb;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  marpaESLIFLua_recognizerContextCleanupv(recognizerContextp);

  /* Call the read interface */
  MARPAESLIFLUA_CALLBACKB(L, recognizer_r, "read", 0 /* nargs */, MARPAESLIFLUA_NOOP, &callbackb);
  if (! callbackb) {
    return luaL_error(L, "Recognizer read method failure");
  }

  /* Call the data interface */
  MARPAESLIFLUA_CALLBACKS(L, recognizer_r, "data", 0 /* nargs */, MARPAESLIFLUA_NOOP, &inputs, &inputl);

  /* Call the encoding interface */
  MARPAESLIFLUA_CALLBACKS(L, recognizer_r, "encoding", 0 /* nargs */, MARPAESLIFLUA_NOOP, &encodings, &encodingl);

  *inputcpp             = inputs;
  *inputlp              = (size_t) inputl;
  MARPAESLIFLUA_CALLBACKB(L, recognizer_r, "isEof",             0 /* nargs */, MARPAESLIFLUA_NOOP, eofbp);
  MARPAESLIFLUA_CALLBACKB(L, recognizer_r, "isCharacterStream", 0 /* nargs */, MARPAESLIFLUA_NOOP, characterStreambp);
  *encodingsp           = encodings;
  *encodinglp           = encodingl;

  recognizerContextp->previousInputs    = (char *) inputs;
  recognizerContextp->previousEncodings = (char *) encodings;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (*inputcpp=%p, *inputlp=%ld, *eofbp=%d, *characterStreambp=%d, *encodingsp=%p, *encodinglp=%ld) at %s:%d", funcs, L, *inputcpp, (unsigned long) *inputlp, (int) *eofbp, (int) *characterStreambp, *encodingsp, (unsigned long) *encodinglp, FILENAMES, __LINE__);

  return 1;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t marpaESLIFLua_valueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueRuleActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__);

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return marpaESLIFLua_valueRuleCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t marpaESLIFLua_valueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueSymbolActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__);

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return marpaESLIFLua_valueSymbolCallbackb;
}

/*****************************************************************************/
static marpaESLIFValueFreeCallback_t marpaESLIFLua_valueFreeActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueFreeActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__);

  /* It HAS to be ":defaultFreeActions" */
  if (strcmp(actions, ":defaultFreeActions") != 0) {
    return NULL;
  }

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return marpaESLIFLua_valueFreeCallbackv;
}

/*****************************************************************************/
static short marpaESLIFLua_valueRuleCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char        *funcs         = "marpaESLIFLua_valueRuleCallbackb";
  valueContext_t           *valueContextp = (valueContext_t *) userDatavp;
  lua_State                *L             = valueContextp->L;
  int                       topi;
  int                       newtopi;
  int                       i;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", FILENAMES, __LINE__);

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(valueContextp->symbols), &(valueContextp->symboli), &(valueContextp->rules), &(valueContextp->rulei))) {
    return luaL_error(L, "marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  topi = lua_gettop(L);
  MARPAESLIFLUA_CALLBACK(L, valueContextp->value_r, valueContextp->actions, nullableb ? 0 : (argni - arg0i + 1) /* nargs */, 
                         if (! nullableb) {
                           for (i = arg0i; i <= argni; i++) {
                             GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) i=%d at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", i, FILENAMES, __LINE__);
                             marpaESLIFLua_pushValuev(valueContextp, marpaESLIFValuep, i);
                           }
                         }
                         );
  newtopi = lua_gettop(L);
  if (newtopi == topi) {
    lua_pushnil(L);
  } else {
    if (newtopi != (topi + 1)) {
      return luaL_error(L, "Function %s must return exactly one or zero value", valueContextp->actions);
    }
  }

  MARPAESLIFLUA_SET_VALUE(valueContextp, marpaESLIFValuep, resulti, marpaESLIFLua_representationb);
  lua_pop(L, 1);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) return 1 at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", FILENAMES, __LINE__);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  static const char        *funcs         = "marpaESLIFLua_valueSymbolCallbackb";
  valueContext_t           *valueContextp = (valueContext_t *) userDatavp;
  lua_State                *L             = valueContextp->L;
  int                       topi;
  int                       newtopi;
  int                       i;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (bytep=%p, bytel=%ld, resulti=%d) at %s:%d", funcs, L, bytep, (unsigned long) bytel, resulti, FILENAMES, __LINE__);

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(valueContextp->symbols), &(valueContextp->symboli), &(valueContextp->rules), &(valueContextp->rulei))) {
    return luaL_error(L, "marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  topi = lua_gettop(L);
  MARPAESLIFLUA_CALLBACK(L, valueContextp->value_r, valueContextp->actions, 1 /* nargs */, lua_pushlstring(L, (const char *) bytep, bytel););
  newtopi = lua_gettop(L);
  if (newtopi == topi) {
    lua_pushnil(L);
  } else {
    if (newtopi != (topi + 1)) {
      return luaL_error(L, "Function %s must return exactly one or zero value", valueContextp->actions);
    }
  }

  MARPAESLIFLUA_SET_VALUE(valueContextp, marpaESLIFValuep, resulti, marpaESLIFLua_representationb);
  lua_pop(L, 1);                                \

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (bytep=%p, bytel=%ld, resulti=%d) return 1 at %s:%d", funcs, L, bytep, (unsigned long) bytel, resulti, FILENAMES, __LINE__);

  return 1;
}

/*****************************************************************************/
static void marpaESLIFLua_valueFreeCallbackv(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueFreeCallbackv";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, FILENAMES, __LINE__);

  if (contexti == ESLIF_LUA_CONTEXT) {
    /* This is a pointer to an integer that contains a global reference to the value */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) releasing global reference %d at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, * (int *) p, FILENAMES, __LINE__);
    MARPAESLIFLUA_UNREF(L, * (int *) p);
    free(p);
  }
}

/*****************************************************************************/
static short marpaESLIFLua_transformUndefb(void *userDatavp, int contexti)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformUndefb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d) at %s:%d", funcs, L, contexti, FILENAMES, __LINE__);

  lua_pushnil(valueContextp->L);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformCharb(void *userDatavp, int contexti, char c)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformCharb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, c=%c) at %s:%d", funcs, L, contexti, c, FILENAMES, __LINE__);

  lua_pushlstring(valueContextp->L, &c, 1);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformShortb(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformShortb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, b=%d) at %s:%d", funcs, L, contexti, (int) b, FILENAMES, __LINE__);

  lua_pushinteger(valueContextp->L, (lua_Integer) b);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformIntb(void *userDatavp, int contexti, int i)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformIntb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, i=%d) at %s:%d", funcs, L, contexti, i, FILENAMES, __LINE__);

  lua_pushinteger(valueContextp->L, (lua_Integer) i);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformLongb(void *userDatavp, int contexti, long l)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformLongb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, l=%ld) at %s:%d", funcs, L, contexti, l, FILENAMES, __LINE__);

  lua_pushinteger(valueContextp->L, (lua_Integer) l);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformFloatb(void *userDatavp, int contexti, float f)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformFloatb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, f=%f) at %s:%d", funcs, L, contexti, (double) f, FILENAMES, __LINE__);

  lua_pushnumber(valueContextp->L, (lua_Number) f);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformDoubleb(void *userDatavp, int contexti, double d)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformDoubleb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, d=%f) at %s:%d", funcs, L, contexti, d, FILENAMES, __LINE__);

  lua_pushnumber(valueContextp->L, (lua_Number) d);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformPtrb(void *userDatavp, int contexti, void *p)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformPtrb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p) at %s:%d", funcs, L, contexti, p, FILENAMES, __LINE__);

  if (contexti == ESLIF_LUA_CONTEXT) {
    /* This is a pointer to an integer value that is a global reference to the real value */
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p) pushing value with global reference %d at %s:%d", funcs, L, contexti, p, * (int *) p, FILENAMES, __LINE__);
    MARPAESLIFLUA_DEREF(L, * (int *) p);
  } else {
    lua_pushlightuserdata(valueContextp->L, p);
  }

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformArrayb(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformArrayb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, FILENAMES, __LINE__);

  lua_pushlstring(valueContextp->L, p, sizel);

  return 1;
}

/*****************************************************************************/
static short marpaESLIFLua_transformBoolb(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformBoolb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, b=%s) at %s:%d", funcs, L, contexti, b ? "true" : "false", FILENAMES, __LINE__);

  lua_pushboolean(valueContextp->L, (int) b);

  return 1;
}

/*****************************************************************************/
static void marpaESLIFLua_pushValuev(valueContext_t *valueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_pushValuev";
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  lua_State               *L = valueContextp->L;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) at %s:%d", funcs, L, stackindicei, FILENAMES, __LINE__);

  marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, stackindicei);
  if (marpaESLIFValueResultp == NULL) {
    luaL_error(L, "marpaESLIFValueResultp is NULL at stack indice %d", stackindicei);
  }

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is of type %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->type, FILENAMES, __LINE__);
  if (marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_INT) {
    if (marpaESLIFValueResultp->contexti == ESLIF_LUA_CONTEXT) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is an INT with context == ESLIF_LUA_CONTEXT, value is %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->u.i, FILENAMES, __LINE__);
    } else {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is an INT with context != ESLIF_LUA_CONTEXT, value is %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->u.i, FILENAMES, __LINE__);
    }
  }
  if (! marpaESLIFValue_transformb(marpaESLIFValuep, marpaESLIFValueResultp, NULL /* marpaESLIFValueResultResolvedp */)) {
    luaL_error(L, "marpaESLIFValue_transformb failure, %s", strerror(errno));
  }

  /* Dereference eventual last result and keep a reference to the new one */
  MARPAESLIFLUA_UNREF(L, valueContextp->result_r);
  lua_pushnil(L);                                                                      /* stack: value, nil */
  lua_copy(L, -2, -1);                                                                 /* stack: value, value */
  MARPAESLIFLUA_REF(L, valueContextp->result_r);                                       /* stack: value */
}

/*****************************************************************************/
static short marpaESLIFLua_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_representationb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;
  const char        *s;
  size_t             l;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  marpaESLIFLua_valueContextCleanupv(valueContextp);

  /* We always push a PTR */
  if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
    return luaL_error(L, "User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
  }
  /* Our context is always ESLIF_LUA_CONTEXT */
  if (marpaESLIFValueResultp->contexti != ESLIF_LUA_CONTEXT) {
    return luaL_error(L, "User-defined value context is not ESLIF_PERL_CONTEXT but %d", marpaESLIFValueResultp->contexti);
  }

  MARPAESLIFLUA_DEREF(L, * (int *) marpaESLIFValueResultp->u.p);
  s = lua_tolstring(L, -1, &l);
  if ((s != NULL) && (l > 0)) {
    *inputcpp = (char *) malloc(l);
    if (*inputcpp == NULL) {
      return luaL_error(L, "malloc failure, %s", strerror(errno));
    }
    *inputlp = l;
  }
  lua_pop(L, 1);

  /* Always return a true value, else ::concat will abort */
  return 1;
}

/*****************************************************************************/
static void marpaESLIFLua_iterate_and_print(lua_State *L, int index)
/*****************************************************************************/
/* C.f. https://stackoverflow.com/questions/6137684/iterate-through-lua-table */
/*****************************************************************************/
{
  const char *key;
  const char *value;

  /* Push another reference to the table on top of the stack (so we know */
  /* where it is, and this function can work for negative, positive and */
  /* pseudo indices */
  lua_pushvalue(L, index);
  /* stack now contains: -1 => table */
  lua_pushnil(L);
  /* stack now contains: -1 => nil; -2 => table */
  while (lua_next(L, -2)) {
    /* stack now contains: -1 => value; -2 => key; -3 => table */
    /* copy the key so that lua_tostring does not modify the original */
    lua_pushvalue(L, -2);
    /* stack now contains: -1 => key; -2 => value; -3 => key; -4 => table */
    key = lua_tostring(L, -1);
    value = lua_tostring(L, -2);
    printf("%s => %s\n", key, value);
    /* pop value + copy of key, leaving original key */
    lua_pop(L, 2);
    /* stack now contains: -1 => key; -2 => table */
  }
  /* stack now contains: -1 => table (when lua_next returns 0 it pops the key */
  /* but does not push anything.) */
  /* Pop table */
  lua_pop(L, 1);
  /* Stack is now the same as it was on entry to this function */
}

/*****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_newi(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_newi";
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  recognizerContext_t          *recognizerContextp;
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_new(marpaESLIFGrammarp, recognizerInterface)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, recognizerInterface, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, recognizerInterface, marpaESLIFGrammarp */
  lua_pop(L, 1);                                 /* stack: marpaESLIFGrammarTable, recognizerInterface */

  marpaESLIFLua_paramIsRecognizerInterfacev(L, 2);

  recognizerContextp = (recognizerContext_t *) malloc(sizeof(recognizerContext_t));
  if (recognizerContextp == NULL) {
    return luaL_error(L, "malloc failure, %s", strerror(errno));
  }

  marpaESLIFLua_recognizerContextInitv(L, 1 /* grammarStacki */, 2 /* recognizerInterfaceStacki */, 0 /* recognizerOrigStacki */, recognizerContextp);

  /* We need a lexeme stack in this mode (in contrary to the parse() method that never calls back) */
  GENERICSTACK_NEW(recognizerContextp->lexemeStackp);
  if (recognizerContextp->lexemeStackp == NULL) {
    int save_errno = errno;
    marpaESLIFLua_recognizerContextFreev(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "GENERICSTACK_NEW failure, %s", strerror(save_errno));
  }

  marpaESLIFRecognizerOption.userDatavp        = recognizerContextp;
  marpaESLIFRecognizerOption.readerCallbackp   = marpaESLIFLua_readerCallbackb;
  MARPAESLIFLUA_CALLBACKB(L, recognizerContextp->recognizer_r, "isWithDisableThreshold", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.disableThresholdb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContextp->recognizer_r, "isWithExhaustion",       0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.exhaustedb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContextp->recognizer_r, "isWithNewline",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.newlineb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContextp->recognizer_r, "isWithTrack",            0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.trackb));
  marpaESLIFRecognizerOption.bufsizl           = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */

  recognizerContextp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (recognizerContextp->marpaESLIFRecognizerp == NULL) {
    int save_errno = errno;
    marpaESLIFLua_recognizerContextFreev(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "marpaESLIFRecognizer_newp failure, %s", strerror(save_errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  /* We will return a table */
  lua_newtable(L);                                                                   /* stack: {} */
  MARPAESLIFLUA_STORE_USERDATA(L, "recognizerContextp", recognizerContextp);         /* stack: {["recognizerContextp"] =>recognizerContextp */

  /* Create a metable */
  lua_newtable(L);
  MARPAESLIFLUA_STORE_ASCIISTRING(L, "__mode", "v");                                 /* ... Say the values are weak */
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", marpaESLIFLua_marpaESLIFRecognizer_freei); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                   /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "newFrom", marpaESLIFLua_marpaESLIFRecognizer_newFromi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "set_exhausted_flag", marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "share", marpaESLIFLua_marpaESLIFRecognizer_sharei);
  MARPAESLIFLUA_STORE_FUNCTION(L, "isCanContinue", marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                           /* stack: {["recognizerContextp"] =>recognizerContextp, meta=>{...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({recognizerContextp->marpaESLIFRecognizerp=%p}) at %s:%d", funcs, L, recognizerContextp->marpaESLIFRecognizerp, FILENAMES, __LINE__);
  return 1;
}

/****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_freei(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFRecognizer_freei";
  recognizerContext_t *recognizerContextp;

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  lua_getfield(L, -1, "recognizerContextp"); /* stack: {...}, recognizerContextp */
  recognizerContextp = (recognizerContext_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizerContext=%p at %s:%d", funcs, L, recognizerContextp, FILENAMES, __LINE__);
  marpaESLIFLua_recognizerContextFreev(recognizerContextp, 0 /* onStackb */);

  lua_pop(L, 1);                             /* stack: */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 0;
}

/*****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_newFromi(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_newFromi";
  recognizerContext_t          *recognizerContextFromp;
  recognizerContext_t          *recognizerContextp;
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_newFrom(marpaESLIFRecognizerp, marpaESLIFGrammarp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }

  if (lua_type(L, 2) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "recognizerContextp");       /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, recognizerContextFromp */
  recognizerContextFromp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, recognizerContextFromp */
  lua_pop(L, 1);                                  /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable */

  lua_getfield(L, 2, "marpaESLIFGrammarp");       /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);     /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, marpaESLIFGrammarp */
  lua_pop(L, 1);                                  /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable */

  recognizerContextp = (recognizerContext_t *) malloc(sizeof(recognizerContext_t));
  if (recognizerContextp == NULL) {
    return luaL_error(L, "malloc failure, %s", strerror(errno));
  }

  marpaESLIFLua_recognizerContextInitv(L, 2 /* grammarStacki */, 0 /* recognizerInterfaceStacki */, 1 /* recognizerOrigStacki */, recognizerContextp);

  /* We need a lexeme stack in this mode (in contrary to the parse() method that never calls back) */
  GENERICSTACK_NEW(recognizerContextp->lexemeStackp);
  if (recognizerContextp->lexemeStackp == NULL) {
    int save_errno = errno;
    marpaESLIFLua_recognizerContextFreev(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "GENERICSTACK_NEW failure, %s", strerror(save_errno));
  }

  recognizerContextp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newFromp(marpaESLIFGrammarp, recognizerContextFromp->marpaESLIFRecognizerp);
  if (recognizerContextp->marpaESLIFRecognizerp == NULL) {
    int save_errno = errno;
    marpaESLIFLua_recognizerContextFreev(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "marpaESLIFRecognizer_newFromp failure, %s", strerror(save_errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  /* We will return a table */
  lua_newtable(L);                                                                   /* stack: {} */
  MARPAESLIFLUA_STORE_USERDATA(L, "recognizerContextp", recognizerContextp);         /* stack: {["recognizerContextp"] =>recognizerContextp */

  /* Create a metable */
  lua_newtable(L);
  MARPAESLIFLUA_STORE_ASCIISTRING(L, "__mode", "v");                                 /* ... Say the values are weak */
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", marpaESLIFLua_marpaESLIFRecognizer_freei); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                   /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "newFrom", marpaESLIFLua_marpaESLIFRecognizer_newFromi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "set_exhausted_flag", marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi);
  MARPAESLIFLUA_STORE_FUNCTION(L, "share", marpaESLIFLua_marpaESLIFRecognizer_sharei);
  MARPAESLIFLUA_STORE_FUNCTION(L, "isCanContinue", marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                           /* stack: {["recognizerContextp"] =>recognizerContextp, meta=>{...} */

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({recognizerContextp->marpaESLIFRecognizerp=%p}) at %s:%d", funcs, L, recognizerContextp->marpaESLIFRecognizerp, FILENAMES, __LINE__);
  return 1;
}

/*****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi";
  recognizerContext_t          *recognizerContextp;
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_set_exhausted_flag(marpaESLIFRecognizerp, flag)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }

  if (lua_type(L, 2) != LUA_TBOOLEAN) {
    return luaL_error(L, "flag must be a boolean");
  }

  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, marpaESLIFGrammarTable */

  if (! marpaESLIFRecognizer_set_exhausted_flagb(recognizerContextp->marpaESLIFRecognizerp, lua_toboolean(L, 2) ? 1 : 0)) {
    return luaL_error(L, "marpaESLIFRecognizer_set_exhausted_flagb failure, %s", strerror(errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 0;
}

/*****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_sharei(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_sharei";
  recognizerContext_t          *recognizerContextp;
  recognizerContext_t          *recognizerContextSharedp;
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_sharei(marpaESLIFRecognizerp, marpaESLIFRecognizerSharedp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }

  if (lua_type(L, 2) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerSharedp must be a table");
  }

  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */

  lua_getfield(L, 2, "recognizerContextp");         /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  recognizerContextSharedp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  lua_pop(L, 1);                                    /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */

  /*
   * The eventual previous reference on another shared recognizer has its refcount decreased.
   */
  if (recognizerContextp->recognizer_orig_r != LUA_NOREF) {
    GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__);
    MARPAESLIFLUA_UNREF(L, recognizerContextp->recognizer_orig_r);
  }

  lua_pushnil(L);                                              /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, nil */
  lua_copy(L, 2, -1);                                          /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, marpaESLIFRecognizerSharedTable */
  MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_orig_r); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__);

  if (! marpaESLIFRecognizer_shareb(recognizerContextp->marpaESLIFRecognizerp, recognizerContextSharedp->marpaESLIFRecognizerp)) {
    return luaL_error(L, "marpaESLIFRecognizer_shareb failure, %s", strerror(errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__);
  return 0;
}

/*****************************************************************************/
static int marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei";
  recognizerContext_t          *recognizerContextp;
  short                         isCanContinueb;
 
  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__);

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_isCanContinue(marpaESLIFRecognizerp, marpaESLIFRecognizerSharedp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }

  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_isCanContinueb(recognizerContextp->marpaESLIFRecognizerp, &isCanContinueb)) {
    return luaL_error(L, "marpaESLIFRecognizer_isCanContinueb failure, %s", strerror(errno));
  }

  lua_pushboolean(L, isCanContinueb);

  GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (isCanContinueb=%s) at %s:%d", funcs, L, isCanContinueb ? "true" : "false", FILENAMES, __LINE__);
  return 1;
}

