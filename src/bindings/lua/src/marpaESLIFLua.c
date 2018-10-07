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

#ifdef MARPAESLIFLUA_EMBEDDED
#define MARPAESLIFLUA_METHOD(methodName) marpaESLIFLuaEmbedded_ ## methodName
#else
#define MARPAESLIFLUA_METHOD(methodName) marpaESLIFLua_ ## methodName
#endif

/* To ease me debugging -; */
static void MARPAESLIFLUA_METHOD(stackdumpv)(lua_State* L, int forcelookupi);
#undef  FILENAMES
#define FILENAMES "marpaESLIFLua.c"

/* Global table for the multiton pattern */
#define MARPAESLIFMULTITONSTABLE "__marpaESLIFLuaMultitonsTable"
/* Every key   is a marpaESLIFp light userdata */
/* Every value is a reference to the logger (reference to nil if there is none) */

/* marpaESLIFLua correctly unreferences everything from the valuation, but it you */
/* prefer it not to use the global registry, then uncomment the next line -; */
/* #define MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
/* Global table for our references */
#define MARPAESLIFLUAREGISTRYINDEX "__marpaESLIFLuaRegistryindex"
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
} recognizerContext_t;

/* Value proxy context */
typedef struct valueContext {
  lua_State         *L;                /* Lua state */
  int                value_r;          /* Lua value reference */
  int                recognizer_r;     /* Lua recognizer reference - can be LUA_NOREF */
  int                grammar_r;        /* Lua grammar reference - can be LUA_NOREF */
  char              *actions;          /* Shallow copy of last resolved name */
  void              *previous_strings; /* Latest stringification result */
  marpaESLIFValue_t *marpaESLIFValuep;
  char              *symbols;
  int                symboli;
  char              *rules;
  int                rulei;
  int                result_r;        /* Reference to last result */
} valueContext_t;

static short                           MARPAESLIFLUA_METHOD(paramIsLoggerInterfaceOrNilb)(lua_State *L, int stacki);
static short                           MARPAESLIFLUA_METHOD(paramIsRecognizerInterfacev)(lua_State *L, int stacki);
static short                           MARPAESLIFLUA_METHOD(paramIsValueInterfacev)(lua_State *L, int stacki);
static void                            MARPAESLIFLUA_METHOD(recognizerContextInitv)(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int recognizerOrigStacki, recognizerContext_t *recognizerContextp);
static void                            MARPAESLIFLUA_METHOD(valueContextInitv)(lua_State *L, int grammarStacki, int recognizerStacki, int valueInterfaceStacki, valueContext_t *valueContextp);
static void                            MARPAESLIFLUA_METHOD(recognizerContextCleanupv)(recognizerContext_t *recognizerContextp);
static void                            MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContext_t *recognizerContextp, short onStackb);
static void                            MARPAESLIFLUA_METHOD(valueContextCleanupv)(valueContext_t *valueContextp);
static void                            MARPAESLIFLUA_METHOD(valueContextFreev)(valueContext_t *valueContextp, short onStackb);
static void                            MARPAESLIFLUA_METHOD(genericLoggerCallbackv)(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static int                             MARPAESLIFLUA_METHOD(installi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(versioni)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIF_newi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFMultitonsTable_freevi)(lua_State *L);
#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRegistryindex_freevi)(lua_State *L);
#endif
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_newi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_freei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ngrammari)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentDescriptioni)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_descriptionByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRuleIdsi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleIdsByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolIdsi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolIdsByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentPropertiesi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_propertiesByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRulePropertiesi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_rulePropertiesByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolPropertiesi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolPropertiesByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showByLeveli)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFGrammar_parsei)(lua_State *L);
static short                           MARPAESLIFLUA_METHOD(readerCallbackb)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static marpaESLIFValueRuleCallback_t   MARPAESLIFLUA_METHOD(valueRuleActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t MARPAESLIFLUA_METHOD(valueSymbolActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueFreeCallback_t   MARPAESLIFLUA_METHOD(valueFreeActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                           MARPAESLIFLUA_METHOD(valueRuleCallbackb)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           MARPAESLIFLUA_METHOD(valueSymbolCallbackb)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void                            MARPAESLIFLUA_METHOD(valueFreeCallbackv)(void *userDatavp, int contexti, void *p, size_t sizel);
static short                           MARPAESLIFLUA_METHOD(transformUndefb)(void *userDatavp, int contexti);
static short                           MARPAESLIFLUA_METHOD(transformCharb)(void *userDatavp, int contexti, char c);
static short                           MARPAESLIFLUA_METHOD(transformShortb)(void *userDatavp, int contexti, short b);
static short                           MARPAESLIFLUA_METHOD(transformIntb)(void *userDatavp, int contexti, int i);
static short                           MARPAESLIFLUA_METHOD(transformLongb)(void *userDatavp, int contexti, long l);
static short                           MARPAESLIFLUA_METHOD(transformFloatb)(void *userDatavp, int contexti, float f);
static short                           MARPAESLIFLUA_METHOD(transformDoubleb)(void *userDatavp, int contexti, double d);
static short                           MARPAESLIFLUA_METHOD(transformPtrb)(void *userDatavp, int contexti, void *p);
static short                           MARPAESLIFLUA_METHOD(transformArrayb)(void *userDatavp, int contexti, void *p, size_t sizel);
static short                           MARPAESLIFLUA_METHOD(transformBoolb)(void *userDatavp, int contexti, short b);
static void                            MARPAESLIFLUA_METHOD(pushValuev)(valueContext_t *valueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, char *bytep, size_t bytel);
static short                           MARPAESLIFLUA_METHOD(representationb)(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp);
static void                            MARPAESLIFLUA_METHOD(iterate_and_print)(lua_State *L, int index);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_freei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newFromi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_set_exhausted_flagi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_sharei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isCanContinuei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isExhaustedi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_scani)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_resumei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventsi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventOnOffi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeAlternativei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeCompletei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeReadi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeTryi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardTryi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeExpectedi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastPausei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastTryi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardLastTryi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isEofi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_readi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_inputi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_progressLogi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedOffseti)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLenghti)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLocationi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_linei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_columni)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_locationi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_hookDiscardi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFValue_newi)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFValue_freei)(lua_State *L);
static int                             MARPAESLIFLUA_METHOD(marpaESLIFValue_valuei)(lua_State *L);

/* Transformers */
static marpaESLIFValueResultTransform_t marpaESLIFValueResultTransformDefault = {
  MARPAESLIFLUA_METHOD(transformUndefb),
  MARPAESLIFLUA_METHOD(transformCharb),
  MARPAESLIFLUA_METHOD(transformShortb),
  MARPAESLIFLUA_METHOD(transformIntb),
  MARPAESLIFLUA_METHOD(transformLongb),
  MARPAESLIFLUA_METHOD(transformFloatb),
  MARPAESLIFLUA_METHOD(transformDoubleb),
  MARPAESLIFLUA_METHOD(transformPtrb),
  MARPAESLIFLUA_METHOD(transformArrayb),
  MARPAESLIFLUA_METHOD(transformBoolb)
};

#define MARPAESLIFLUA_NOOP

#define MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, i) do {  \
    lua_pushinteger(L, (lua_Integer) i);                \
    lua_setglobal(L, #i);                               \
  } while (0)

#define MARPAESLIFLUA_GETORCREATEGLOBAL(L, name, gcp) do {              \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) getting global table name=%s gcp=%p at %s:%d", funcs, L, name, gcp, FILENAMES, __LINE__); */  \
    lua_getglobal(L, name);                                             \
    if (lua_type(L, -1) != LUA_TTABLE) {                                \
      lua_pop(L, 1);                                                    \
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) creating global table name=%s gcp=%p at %s:%d", funcs, L, name, gcp, FILENAMES, __LINE__); */  \
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
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFLUAREGISTRYINDEX, MARPAESLIFLUA_METHOD(marpaESLIFRegistryindex_freevi)); \
    lua_insert(L, -2);                                                  \
    refi = luaL_ref(L, -2);                                             \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) set global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */  \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, MARPAESLIFLUAREGISTRYINDEX, -1); */ \
    lua_pop(L, 1);                                                      \
  } while (0);

#define MARPAESLIFLUA_UNREF(L, refi) do {                               \
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFLUAREGISTRYINDEX, MARPAESLIFLUA_METHOD(marpaESLIFRegistryindex_freevi)); \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) del global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */  \
    luaL_unref(L, -1, refi);                                            \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, MARPAESLIFLUAREGISTRYINDEX, -1); */ \
    lua_pop(L, 1);                                                      \
  } while (0);

#define MARPAESLIFLUA_DEREF(L, refi) do {                               \
    MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFLUAREGISTRYINDEX, MARPAESLIFLUA_METHOD(marpaESLIFRegistryindex_freevi)); \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) use global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */  \
    lua_rawgeti(L, -1, refi);                                           \
    lua_remove(L, -2);                                                  \
  } while (0);

#else /* MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

#define MARPAESLIFLUA_REF(L, refi) do {                                 \
    refi = luaL_ref(L, LUA_REGISTRYINDEX);                              \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) set global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */  \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, "LUA_REGISTRYINDEX", LUA_REGISTRYINDEX); */  \
  } while (0);

#define MARPAESLIFLUA_UNREF(L, refi) do {                               \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) del global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */  \
    luaL_unref(L, LUA_REGISTRYINDEX, refi);                             \
    /* MARPAESLIFLUA_TRAVERSE_TABLE(L, "LUA_REGISTRYINDEX", LUA_REGISTRYINDEX); */  \
  } while (0);

#define MARPAESLIFLUA_DEREF(L, refi) do {                               \
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) use global reference refi=%d%s at %s:%d", funcs, L, refi, (refi == LUA_NOREF) ? " (LUA_NOREF)" : (refi == LUA_REFNIL) ? " (LUA_REFNIL)" : "", FILENAMES, __LINE__); */ \
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

#define MARPAESLIFLUA_PUSH_ASCIISTRING_ARRAY(L, stringl, stringp) do {  \
    size_t _iteratorl;                                                  \
                                                                        \
    lua_createtable(L, (int) stringl, 0);                               \
    for (_iteratorl = 0; _iteratorl < stringl; _iteratorl++) {          \
      lua_pushstring(L, stringp[_iteratorl]);                           \
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
    /* GENERICLOGGER_NOTICEF(NULL, "... Storing pointer %p to global reference %d to stack indice %d at %s:%d", _p, *_p, indicei, FILENAMES, __LINE__); */  \
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

#ifdef MARPAESLIFLUA_EMBEDDED
/* luaopen_marpaESLIFLua is to be called explicitely by the program that embeds marpaESLIFLua */
static
#endif
/****************************************************************************/
int luaopen_marpaESLIFLua(lua_State* L)
/****************************************************************************/
{
  static const char *funcs = "luaopen_marpaESLIFLua";

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */
  luaL_requiref(L, "marpaESLIFLua", MARPAESLIFLUA_METHOD(installi), 1 /* global */);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(installi)(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_installi";
  static const luaL_Reg MARPAESLIFLUA_METHOD(installTable)[] = {
    {"version", MARPAESLIFLUA_METHOD(versioni)},
    {"marpaESLIF_new", MARPAESLIFLUA_METHOD(marpaESLIF_newi)},
    {"marpaESLIFGrammar_new", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_newi)},
    {"marpaESLIFGrammar_ngrammar", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ngrammari)},
    {"marpaESLIFGrammar_currentLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentLeveli)},
    {"marpaESLIFGrammar_currentDescription", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentDescriptioni)},
    {"marpaESLIFGrammar_descriptionByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_descriptionByLeveli)},
    {"marpaESLIFGrammar_currentRuleIds", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRuleIdsi)},
    {"marpaESLIFGrammar_ruleIdsByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleIdsByLeveli)},
    {"marpaESLIFGrammar_currentSymbolIds", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolIdsi)},
    {"marpaESLIFGrammar_symbolIdsByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolIdsByLeveli)},
    {"marpaESLIFGrammar_currentProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentPropertiesi)},
    {"marpaESLIFGrammar_propertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_propertiesByLeveli)},
    {"marpaESLIFGrammar_currentRuleProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRulePropertiesi)},
    {"marpaESLIFGrammar_rulePropertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_rulePropertiesByLeveli)},
    {"marpaESLIFGrammar_currentSymbolProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolPropertiesi)},
    {"marpaESLIFGrammar_symbolPropertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolPropertiesByLeveli)},
    {"marpaESLIFGrammar_ruleDisplay", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayi)},
    {"marpaESLIFGrammar_symbolDisplay", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayi)},
    {"marpaESLIFGrammar_ruleShow", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowi)},
    {"marpaESLIFGrammar_ruleDisplayByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayByLeveli)},
    {"marpaESLIFGrammar_symbolDisplayByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayByLeveli)},
    {"marpaESLIFGrammar_ruleShowByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowByLeveli)},
    {"marpaESLIFGrammar_show", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showi)},
    {"marpaESLIFGrammar_showByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showByLeveli)},
    {"marpaESLIFGrammar_parse", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_parsei)},
    {"marpaESLIFRecognizer_new", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newi)},
    {"marpaESLIFRecognizer_newFrom", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newFromi)},
    {"marpaESLIFRecognizer_sharei", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_sharei)},
    {"marpaESLIFRecognizer_isCanContinue", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isCanContinuei)},
    {"marpaESLIFRecognizer_isExhausted", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isExhaustedi)},
    {"marpaESLIFRecognizer_scan", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_scani)},
    {"marpaESLIFRecognizer_resume", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_resumei)},
    {"marpaESLIFRecognizer_events", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventsi)},
    {"marpaESLIFRecognizer_eventOnOff", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventOnOffi)},
    {"marpaESLIFRecognizer_lexemeAlternative", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeAlternativei)},
    {"marpaESLIFRecognizer_lexemeComplete", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeCompletei)},
    {"marpaESLIFRecognizer_lexemeRead", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeReadi)},
    {"marpaESLIFRecognizer_lexemeTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeTryi)},
    {"marpaESLIFRecognizer_discardTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardTryi)},
    {"marpaESLIFRecognizer_lexemeExpected", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeExpectedi)},
    {"marpaESLIFRecognizer_lexemeLastPause", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastPausei)},
    {"marpaESLIFRecognizer_lexemeLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastTryi)},
    {"marpaESLIFRecognizer_discardLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardLastTryi)},
    {"marpaESLIFRecognizer_isEof", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isEofi)},
    {"marpaESLIFRecognizer_read", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_readi)},
    {"marpaESLIFRecognizer_input", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_inputi)},
    {"marpaESLIFRecognizer_progressLog", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_progressLogi)},
    {"marpaESLIFRecognizer_lastCompletedOffset", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedOffseti)},
    {"marpaESLIFRecognizer_lastCompletedLength", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLenghti)},
    {"marpaESLIFRecognizer_lastCompletedLength", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLocationi)},
    {"marpaESLIFRecognizer_line", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_linei)},
    {"marpaESLIFRecognizer_column", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_columni)},
    {"marpaESLIFRecognizer_location", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_locationi)},
    {"marpaESLIFRecognizer_hookDiscard", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_hookDiscardi)},
    {"marpaESLIFValue_new", MARPAESLIFLUA_METHOD(marpaESLIFValue_newi)},
    {NULL, NULL}
  };

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */
  luaL_newlib(L, MARPAESLIFLUA_METHOD(installTable));

  /* Create constants */
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_NONE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_COMPLETED);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_NULLED);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_PREDICTED);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_BEFORE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_AFTER);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_EXHAUSTED);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_EVENTTYPE_DISCARD);
  
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_UNDEF);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_CHAR);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_SHORT);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_INT);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_LONG);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_FLOAT);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_DOUBLE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_PTR);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_ARRAY);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_VALUE_TYPE_BOOL);
  
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_RULE_IS_ACCESSIBLE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_RULE_IS_NULLABLE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_RULE_IS_NULLING);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_RULE_IS_LOOP);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_RULE_IS_PRODUCTIVE);
  
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_ACCESSIBLE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_NULLABLE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_NULLING);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_PRODUCTIVE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_START);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOL_IS_TERMINAL);
  
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOLTYPE_TERMINAL);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, MARPAESLIF_SYMBOLTYPE_META);

  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_TRACE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_DEBUG);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_INFO);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_NOTICE);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_WARNING);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_ERROR);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_CRITICAL);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_ALERT);
  MARPAESLIFLUA_CREATEINTEGERCONSTANT(L, GENERICLOGGER_LOGLEVEL_EMERGENCY);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(versioni)(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_versioni";
  const char        *versions;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */
  versions = marpaESLIF_versions();
  lua_pushstring(L, versions);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (versions=\"%s\") at %s:%d", funcs, L, versions, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIF_newi)(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIF_newi";
  marpaESLIF_t           *marpaESLIFp = NULL;
  short                   loggerb;
  genericLoggerContext_t *genericLoggerContextp;
  genericLogger_t        *genericLoggerp;
  marpaESLIFOption_t      marpaESLIFOption;
  int                     logger_r;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  switch (lua_gettop(L)) {
  case 0:
    loggerb = 0;
    break;
  case 1:
    loggerb = MARPAESLIFLUA_METHOD(paramIsLoggerInterfaceOrNilb)(L, 1);
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIF_new([logger]");
  }

  MARPAESLIFLUA_GETORCREATEGLOBAL(L, MARPAESLIFMULTITONSTABLE, MARPAESLIFLUA_METHOD(marpaESLIFMultitonsTable_freevi)); /* stack: logger?, MARPAESLIFMULTITONSTABLE */

  /* Look if MARPAESLIFMULTITONSTABLE already contains a reference to logger */
  lua_pushnil(L);                                                                                   /* stack: logger?, MARPAESLIFMULTITONSTABLE, nil */
  while (lua_next(L, -2) != 0) {                                                                    /* stack: logger?, MARPAESLIFMULTITONSTABLE, marpaESLIFp, r */
    logger_r = (int) lua_tointeger(L, -1);
    if (logger_r == LUA_NOREF) {
      lua_pushnil(L);                                                                               /* stack: logger?, MARPAESLIFMULTITONSTABLE, marpaESLIFp, r, nil */
    } else {
      MARPAESLIFLUA_DEREF(L, logger_r);                                                             /* stack: logger?, MARPAESLIFMULTITONSTABLE, marpaESLIFp, r, loggerp_from_registry */
    }
    if (((! loggerb) && lua_isnil(L, -1))
        ||
        ((  loggerb) && lua_compare(L, 1, -1, LUA_OPEQ))) {
      marpaESLIFp = (marpaESLIF_t *) lua_touserdata(L, -3);
      lua_pop(L, 3);                                                                                /* stack: logger?, MARPAESLIFMULTITONSTABLE */
      break;
    }
    lua_pop(L, 2);                                                                                  /* stack: logger?, MARPAESLIFMULTITONSTABLE, marpaESLIFp */
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
      lua_pushnil(L);                                                                               /* stack: logger, MARPAESLIFMULTITONSTABLE, nil */
      lua_copy(L, 1, -1);                                                                           /* stack: logger, MARPAESLIFMULTITONSTABLE, logger */
      MARPAESLIFLUA_REF(L, logger_r);                                                               /* stack: logger, MARPAESLIFMULTITONSTABLE */
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__); */

      /* Fill genericLogger context */
      genericLoggerContextp->L = L;
      genericLoggerContextp->logger_r = logger_r;
      genericLoggerp = genericLogger_newp(MARPAESLIFLUA_METHOD(genericLoggerCallbackv), genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__); */
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
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, logger_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, logger_r); /* No effect if it is LUA_NOREF */
      free(genericLoggerContextp);
      return luaL_error(L, "marpaESLIF_newp failure, %s\n", strerror(errno));
    }

    /* Link marpaESLIFp to logger_r */
    lua_pushinteger(L, (lua_Integer) logger_r);                                                     /* stack: logger?, MARPAESLIFMULTITONSTABLE, logger_r */
    lua_rawsetp(L, -2, (const void *) marpaESLIFp);                                                 /* stack: logger?, MARPAESLIFMULTITONSTABLE */
    lua_pop(L, 1);                                                                                  /* stack: logger? */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__); */
  } else {
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) marpaESLIFp=%p (reuse) at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__); */
  }

  if (loggerb) {
    lua_pop(L, 1);                                                                                  /* stack */
  }

  lua_pushlightuserdata(L, marpaESLIFp);                                                            /* stack: marpaESLIFp */

  /* Create a metable */
  lua_newtable(L);
  lua_newtable(L);                                                                                  /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFGrammar_new", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_newi));
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                                          /* stack: marpaESLIFp meta={...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (marpaESLIFp=%p) at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static short MARPAESLIFLUA_METHOD(paramIsLoggerInterfaceOrNilb)(lua_State *L, int stacki)
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
static short MARPAESLIFLUA_METHOD(paramIsRecognizerInterfacev)(lua_State *L, int stacki)
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
static short MARPAESLIFLUA_METHOD(paramIsValueInterfacev)(lua_State *L, int stacki)
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
static void  MARPAESLIFLUA_METHOD(recognizerContextInitv)(lua_State *L, int grammarStacki, int recognizerInterfaceStacki, int recognizerOrigStacki, recognizerContext_t *recognizerContextp)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_recognizerContextInitv";

  recognizerContextp->L = L;
  /* Get grammar reference - required */
  if (grammarStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, grammarStacki, -1);                                 /* stack: xxx, grammarTable */
    MARPAESLIFLUA_REF(L, recognizerContextp->grammar_r);            /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got grammar_r=%d from registry at %s:%d", funcs, L, recognizerContextp->grammar_r, FILENAMES, __LINE__); */
  } else {
    luaL_error(L, "grammarStacki must be != 0");
  }
  /* Get recognizer reference - optional */
  if (recognizerInterfaceStacki != 0) {
    lua_pushnil(L);                                                    /* stack: xxx, nil */
    lua_copy(L, recognizerInterfaceStacki, -1);                        /* stack: xxx, recognizerInterface */
    MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_r);            /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_r, FILENAMES, __LINE__); */
  } else {
    recognizerContextp->recognizer_r = LUA_NOREF;
  }
  /* Get original recognizer reference (in case of newFrom()) - optional */
  if (recognizerOrigStacki != 0) {
    lua_pushnil(L);                                                         /* stack: xxx, nil */
    lua_copy(L, recognizerOrigStacki, -1);                                  /* stack: xxx, recognizerOrigInterface */
    MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_orig_r);            /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__); */
  } else {
    recognizerContextp->recognizer_orig_r = LUA_NOREF;
  }
  recognizerContextp->previousInputs        = NULL;
  recognizerContextp->previousEncodings     = NULL;
  recognizerContextp->lexemeStackp          = NULL;
  recognizerContextp->marpaESLIFRecognizerp = NULL;
}

/****************************************************************************/
static void  MARPAESLIFLUA_METHOD(recognizerContextCleanupv)(recognizerContext_t *recognizerContextp)
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
static void  MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContext_t *recognizerContextp, short onStackb)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_recognizerContextFreev";
  lua_State         *L;
  genericStack_t    *lexemeStackp;
  int                i;
  int                refi;

  if (recognizerContextp != NULL) {
    L = recognizerContextp->L;

    MARPAESLIFLUA_METHOD(recognizerContextCleanupv)(recognizerContextp);

    if (recognizerContextp->grammar_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing grammar_r=%d from registry at %s:%d", funcs, L, recognizerContextp->grammar_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, recognizerContextp->grammar_r);
    }

    if (recognizerContextp->recognizer_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, recognizerContextp->recognizer_r);
    }

    if (recognizerContextp->recognizer_orig_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__); */
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
          /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing lexeme reference=%d from registry at %s:%d", funcs, L, refi, FILENAMES, __LINE__); */
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
static void  MARPAESLIFLUA_METHOD(valueContextCleanupv)(valueContext_t *valueContextp)
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
static void  MARPAESLIFLUA_METHOD(valueContextFreev)(valueContext_t *valueContextp, short onStackb)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_valueContextFreev";
  lua_State         *L;

  if (valueContextp != NULL) {
    L = valueContextp->L;

    MARPAESLIFLUA_METHOD(valueContextCleanupv)(valueContextp);

    if (valueContextp->marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(valueContextp->marpaESLIFValuep);
    }

    /* Decrement dependencies */
    if (valueContextp->value_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing value_r=%d from registry at %s:%d", funcs, L, valueContextp->value_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, valueContextp->value_r);
    }
    if (valueContextp->recognizer_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_r=%d from registry at %s:%d", funcs, L, valueContextp->recognizer_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, valueContextp->recognizer_r);
    }
    if (valueContextp->grammar_r != LUA_NOREF) {
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing grammar_r=%d from registry at %s:%d", funcs, L, valueContextp->grammar_r, FILENAMES, __LINE__); */
      MARPAESLIFLUA_UNREF(L, valueContextp->grammar_r);
    }

    if (! onStackb) {
      free(valueContextp);
    }
  }
}

/****************************************************************************/
static void  MARPAESLIFLUA_METHOD(valueContextInitv)(lua_State *L, int grammarStacki, int recognizerStacki, int valueInterfaceStacki, valueContext_t *valueContextp)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_valueContextInitv";

  valueContextp->L                = L;
  /* Get value reference - required */
  if (valueInterfaceStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, valueInterfaceStacki, -1);                          /* stack: xxx, valueInterface */
    MARPAESLIFLUA_REF(L, valueContextp->value_r);                   /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got value_r=%d from registry at %s:%d", funcs, L, valueContextp->value_r, FILENAMES, __LINE__); */
  } else {
    luaL_error(L, "valueInterfaceStacki must be != 0");
  }
  /* Get recognizer reference - optional */
  if (recognizerStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, recognizerStacki, -1);                              /* stack: xxx, recognizer */
    MARPAESLIFLUA_REF(L, valueContextp->recognizer_r);              /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_r=%d from registry at %s:%d", funcs, L, valueContextp->recognizer_r, FILENAMES, __LINE__); */
  } else {
    valueContextp->recognizer_r = LUA_NOREF;
  }
  /* Get grammar reference - optional */
  if (grammarStacki != 0) {
    lua_pushnil(L);                                                 /* stack: xxx, nil */
    lua_copy(L, grammarStacki, -1);                                 /* stack: xxx, grammar */
    MARPAESLIFLUA_REF(L, valueContextp->grammar_r);                 /* stack: xxx */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got grammar_r=%d from registry at %s:%d", funcs, L, valueContextp->grammar_r, FILENAMES, __LINE__); */
  } else {
    valueContextp->grammar_r = LUA_NOREF;
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
static void MARPAESLIFLUA_METHOD(genericLoggerCallbackv)(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
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
static int MARPAESLIFLUA_METHOD(marpaESLIFMultitonsTable_freevi)(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIFMultitonsTable_freevi";
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLoggerContext_t *genericLoggerContextp = NULL;
  lua_Integer             logger_r              = LUA_NOREF;
  genericLogger_t        *genericLoggerp        = NULL;
  marpaESLIFOption_t     *marpaESLIFOptionp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  /* Loop on MARPAESLIFMULTITONSTABLE */
  lua_pushnil(L);                                                     /* stack: MARPAESLIFMULTITONSTABLE, nil */
  while (lua_next(L, -2) != 0) {                                      /* stack: MARPAESLIFMULTITONSTABLE, marpaESLIFp, r */
    marpaESLIFp = lua_touserdata(L, -2);
    logger_r = lua_tointeger(L, -1);

    if (logger_r != LUA_NOREF) {

      marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
      if (marpaESLIFOptionp != NULL) {
        genericLoggerp = marpaESLIFOptionp->genericLoggerp;
        if (genericLoggerp != NULL) {
          genericLoggerContextp = (genericLoggerContext_t *) genericLogger_userDatavp_getp(genericLoggerp);
          /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing logger_r=%d from registry at %s:%d", funcs, L, genericLoggerContextp->logger_r, FILENAMES, __LINE__); */
          MARPAESLIFLUA_UNREF(L, genericLoggerContextp->logger_r); /* By construction genericLoggerContextp->logger_r == logger_r */
        }
      }

      /* Free marpaESLIF, genericLogger, genericLogger context (in this order) */
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__); */
      marpaESLIF_freev(marpaESLIFp);
      if (genericLoggerContextp != NULL) {
        free(genericLoggerContextp);
      }
      genericLogger_freev(&genericLoggerp);
    } else {
      /* Free marpaESLIF */
      /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFp=%p at %s:%d", funcs, L, marpaESLIFp, FILENAMES, __LINE__); */
      marpaESLIF_freev(marpaESLIFp);
    }

    lua_pop(L, 1);                                                    /* stack: MARPAESLIFMULTITONSTABLE, marpaESLIFp */
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

#ifdef MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX
/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRegistryindex_freevi)(lua_State *L)
/****************************************************************************/
{
  static const char      *funcs = "marpaESLIFLua_marpaESLIFRegistryindex_freevi";

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}
#endif /* MARPAESLIFLUA_USE_INTERNALREGISTRYINDEX */

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFGrammar_newi)(lua_State *L)
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
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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
  MARPAESLIFLUA_STORE_BY_KEY(L, MARPAESLIFMULTITONSTABLE, lua_getglobal(L, MARPAESLIFMULTITONSTABLE);); /* stack: {["MARPAESLIFMULTITONSTABLE"] => MARPAESLIFMULTITONSTABLE} */
  MARPAESLIFLUA_STORE_USERDATA(L, "marpaESLIFGrammarp", marpaESLIFGrammarp);                  /* stack: {["marpaESLIFGrammarp"] =>marpaESLIFGrammarp, ["MARPAESLIFMULTITONSTABLE"] => MARPAESLIFMULTITONSTABLE} */

  /* Create a metable */
  lua_newtable(L);
  MARPAESLIFLUA_STORE_ASCIISTRING(L, "__mode", "v");                              /* ... Say the values are weak */
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_freei)); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "ngrammar", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ngrammari));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentDescription", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentDescriptioni));
  MARPAESLIFLUA_STORE_FUNCTION(L, "descriptionByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_descriptionByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentRuleIds", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRuleIdsi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleIdsByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleIdsByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentSymbolIds", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolIdsi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolIdsByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolIdsByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentPropertiesi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "propertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_propertiesByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentRuleProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRulePropertiesi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "rulePropertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_rulePropertiesByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "currentSymbolProperties", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolPropertiesi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolPropertiesByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolPropertiesByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleDisplay", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolDisplay", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleShow", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleDisplayByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "symbolDisplayByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "ruleShowByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "show", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "showByLevel", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showByLeveli));
  MARPAESLIFLUA_STORE_FUNCTION(L, "parse", MARPAESLIFLUA_METHOD(marpaESLIFGrammar_parsei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFRecognizer_new", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newi));
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                                    /* stack: {["marpaESLIFGrammarp"] =>marpaESLIFGrammarp, ["MARPAESLIFMULTITONSTABLE"] => MARPAESLIFMULTITONSTABLE} meta=>{...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({marpaESLIFGrammarp=%p}) at %s:%d", funcs, L, marpaESLIFGrammarp, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFGrammar_freei)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_freei";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  lua_getfield(L, -1, "marpaESLIFGrammarp"); /* stack: {...}, marpaESLIFGrammarp */
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing marpaESLIFGrammarp=%p at %s:%d", funcs, L, marpaESLIFGrammarp, FILENAMES, __LINE__); */
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);

  lua_pop(L, 1);                             /* stack: */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/****************************************************************************/
static void MARPAESLIFLUA_METHOD(stackdumpv)(lua_State* L, int forcelookupi)
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
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ngrammari)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ngrammari";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  ngrammari;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ngrammari=%d) at %s:%d", funcs, L, ngrammari, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (leveli=%d) at %s:%d", funcs, L, leveli, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentDescriptioni)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentDescriptioni";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (bytep=%p, bytel=%ld) at %s:%d", funcs, L, descp->bytep, (unsigned long) descp->bytel, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_descriptionByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_descriptionByLeveli";
  lua_Integer          leveli;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (bytep=%p, bytel=%ld) at %s:%d", funcs, L, descp->bytep, (unsigned long) descp->bytel, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRuleIdsi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentRuleIdsi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *ruleip;
  size_t               rulel;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ruleip=%p, rulel=%ld) at %s:%d", funcs, L, ruleip, (unsigned long) rulel, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleIdsByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleIdsByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  int                 *ruleip;
  size_t               rulel;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (ruleip=%p, rulel=%ld) at %s:%d", funcs, L, ruleip, (unsigned long) rulel, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolIdsi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentSymbolIdsi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *symbolip;
  size_t               symboll;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (symbolip=%p, symboll=%ld) at %s:%d", funcs, L, symbolip, (unsigned long) symboll, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolIdsByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolIdsByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  int                 *symbolip;
  size_t               symboll;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (symbolip=%p, symboll=%ld) at %s:%d", funcs, L, symbolip, (unsigned long) symboll, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentPropertiesi)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentPropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFGrammarProperty_t  grammarProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_propertiesByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_propertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  marpaESLIFGrammarProperty_t  grammarProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentRulePropertiesi)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentRulePropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  rulei;
  marpaESLIFRuleProperty_t     ruleProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_rulePropertiesByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_rulePropertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  lua_Integer                  rulei;
  marpaESLIFRuleProperty_t     ruleProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_currentSymbolPropertiesi)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_currentSymbolPropertiesi";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  symboli;
  marpaESLIFSymbolProperty_t   symbolProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolPropertiesByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char           *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolPropertiesByLeveli";
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  lua_Integer                  leveli;
  lua_Integer                  symboli;
  marpaESLIFSymbolProperty_t     symbolProperty;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleDisplayi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          rulei;
  char                 *ruleDisplays;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolDisplayi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          symboli;
  char                 *symbolDisplays;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleShowi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          rulei;
  char                 *ruleShows;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleDisplayByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleDisplayByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          rulei;
  char                 *ruleDisplays;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_symbolDisplayByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_symbolDisplayByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          symboli;
  char                 *symbolDisplays;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_ruleShowByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_ruleShowByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  lua_Integer          rulei;
  char                 *ruleShows;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showi)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_showi";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  char                *shows;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (shows=%p) at %s:%d", funcs, L, shows, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_showByLeveli)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFGrammar_showByLeveli";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  lua_Integer          leveli;
  char                *shows;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (shows=%p) at %s:%d", funcs, L, shows, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int  MARPAESLIFLUA_METHOD(marpaESLIFGrammar_parsei)(lua_State *L)
/****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFGrammar_parsei";
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
  recognizerContext_t           recognizerContext;
  valueContext_t                valueContext;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  int                           rci;
  int                           resultStacki;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 3) {
    return luaL_error(L, "Usage: marpaESLIFGrammar_parse(marpaESLIFGrammarp, recognizerInterface, valueInterface)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface, marpaESLIFGrammarp */
  lua_pop(L, 1);                                 /* stack: marpaESLIFGrammarTable, recognizerInterface, valueInterface */

  MARPAESLIFLUA_METHOD(paramIsRecognizerInterfacev)(L, 2);
  MARPAESLIFLUA_METHOD(paramIsValueInterfacev)(L, 3);

  MARPAESLIFLUA_METHOD(recognizerContextInitv)(L, 1 /* grammarStacki */, 2 /* recognizerInterfaceStacki */, 0 /* recognizerOrigStacki */, &recognizerContext);
  MARPAESLIFLUA_METHOD(valueContextInitv)(L, 1 /* grammarStacki */, 0 /* recognizerStacki */, 3 /* valueInterfaceStacki */, &valueContext);
  
  lua_pop(L, 3);                                 /* stack */

  marpaESLIFRecognizerOption.userDatavp        = &recognizerContext;
  marpaESLIFRecognizerOption.readerCallbackp   = MARPAESLIFLUA_METHOD(readerCallbackb);
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithDisableThreshold", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.disableThresholdb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithExhaustion",       0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.exhaustedb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithNewline",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.newlineb));
  MARPAESLIFLUA_CALLBACKB(L, recognizerContext.recognizer_r, "isWithTrack",            0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFRecognizerOption.trackb));
  marpaESLIFRecognizerOption.bufsizl           = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Recommended value */

  marpaESLIFValueOption.userDatavp             = &valueContext;
  marpaESLIFValueOption.ruleActionResolverp    = MARPAESLIFLUA_METHOD(valueRuleActionResolver);
  marpaESLIFValueOption.symbolActionResolverp  = MARPAESLIFLUA_METHOD(valueSymbolActionResolver);
  marpaESLIFValueOption.freeActionResolverp    = MARPAESLIFLUA_METHOD(valueFreeActionResolver);
  marpaESLIFValueOption.transformerp           = &marpaESLIFValueResultTransformDefault;
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithHighRankOnly", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.highRankOnlyb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithOrderByRank",  0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.orderByRankb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithAmbiguous",    0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.ambiguousb));
  MARPAESLIFLUA_CALLBACKB(L, valueContext.value_r, "isWithNull",         0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.nullb));
  MARPAESLIFLUA_CALLBACKI(L, valueContext.value_r, "maxParses",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.maxParsesi));

  if ((rci = marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL)) != 0) {
    resultStacki = lua_gettop(L);
    /* marpaESLIFGrammar_parseb called the transformers that pushed the final value to the stack */
    MARPAESLIFLUA_CALLBACKV(L, valueContext.value_r, "setResult", 1 /* nargs */, lua_pushnil(L); lua_copy(L, resultStacki, -1););
    lua_pop(L, 1);
  }

  MARPAESLIFLUA_METHOD(valueContextFreev)(&valueContext, 1 /* onStackb */);
  MARPAESLIFLUA_METHOD(recognizerContextFreev)(&recognizerContext, 1 /* onStackb */);

  lua_pushboolean(L, rci);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(readerCallbackb)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  MARPAESLIFLUA_METHOD(recognizerContextCleanupv)(recognizerContextp);

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (*inputcpp=%p, *inputlp=%ld, *eofbp=%d, *characterStreambp=%d, *encodingsp=%p, *encodinglp=%ld) at %s:%d", funcs, L, *inputcpp, (unsigned long) *inputlp, (int) *eofbp, (int) *characterStreambp, *encodingsp, (unsigned long) *encodinglp, FILENAMES, __LINE__); */

  return 1;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t MARPAESLIFLUA_METHOD(valueRuleActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueRuleActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__); */

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return MARPAESLIFLUA_METHOD(valueRuleCallbackb);
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t MARPAESLIFLUA_METHOD(valueSymbolActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueSymbolActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__); */

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return MARPAESLIFLUA_METHOD(valueSymbolCallbackb);
}

/*****************************************************************************/
static marpaESLIFValueFreeCallback_t MARPAESLIFLUA_METHOD(valueFreeActionResolver)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueFreeActionResolver";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (actions=%s) at %s:%d", funcs, L, actions, FILENAMES, __LINE__); */

  /* It HAS to be ":defaultFreeActions" */
  if (strcmp(actions, ":defaultFreeActions") != 0) {
    return NULL;
  }

  /* Just remember the action name - lua will croak if calling this method fails */
  valueContextp->actions = actions;

  return MARPAESLIFLUA_METHOD(valueFreeCallbackv);
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(valueRuleCallbackb)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char        *funcs         = "marpaESLIFLua_valueRuleCallbackb";
  valueContext_t           *valueContextp = (valueContext_t *) userDatavp;
  lua_State                *L             = valueContextp->L;
  int                       topi;
  int                       newtopi;
  int                       i;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", FILENAMES, __LINE__); */

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(valueContextp->symbols), &(valueContextp->symboli), &(valueContextp->rules), &(valueContextp->rulei))) {
    return luaL_error(L, "marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  topi = lua_gettop(L);
  MARPAESLIFLUA_CALLBACK(L, valueContextp->value_r, valueContextp->actions, nullableb ? 0 : (argni - arg0i + 1) /* nargs */, 
                         if (! nullableb) {
                           for (i = arg0i; i <= argni; i++) {
                             /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) i=%d at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", i, FILENAMES, __LINE__); */
                             MARPAESLIFLUA_METHOD(pushValuev)(valueContextp, marpaESLIFValuep, i, NULL /* bytep */, 0 /* bytel */);
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

  MARPAESLIFLUA_SET_VALUE(valueContextp, marpaESLIFValuep, resulti, MARPAESLIFLUA_METHOD(representationb));
  lua_pop(L, 1);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (arg0i=%d, argni=%d, resulti=%d, nullableb=%s) return 1 at %s:%d", funcs, L, arg0i, argni, resulti, nullableb ? "true" : "false", FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(valueSymbolCallbackb)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/*****************************************************************************/
{
  static const char        *funcs         = "marpaESLIFLua_valueSymbolCallbackb";
  valueContext_t           *valueContextp = (valueContext_t *) userDatavp;
  lua_State                *L             = valueContextp->L;
  int                       topi;
  int                       newtopi;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (bytep=%p, bytel=%ld, resulti=%d) at %s:%d", funcs, L, bytep, (unsigned long) bytel, resulti, FILENAMES, __LINE__); */

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &(valueContextp->symbols), &(valueContextp->symboli), &(valueContextp->rules), &(valueContextp->rulei))) {
    return luaL_error(L, "marpaESLIFValue_contextb failure, %s", strerror(errno));
  }

  topi = lua_gettop(L);
  MARPAESLIFLUA_CALLBACK(L, valueContextp->value_r, valueContextp->actions, 1 /* nargs */, MARPAESLIFLUA_METHOD(pushValuev)(valueContextp, marpaESLIFValuep, -1 /* stackindicei */, bytep, bytel););
  newtopi = lua_gettop(L);
  if (newtopi == topi) {
    lua_pushnil(L);
  } else {
    if (newtopi != (topi + 1)) {
      return luaL_error(L, "Function %s must return exactly one or zero value", valueContextp->actions);
    }
  }

  MARPAESLIFLUA_SET_VALUE(valueContextp, marpaESLIFValuep, resulti, MARPAESLIFLUA_METHOD(representationb));
  lua_pop(L, 1);                                \

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (bytep=%p, bytel=%ld, resulti=%d) return 1 at %s:%d", funcs, L, bytep, (unsigned long) bytel, resulti, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static void MARPAESLIFLUA_METHOD(valueFreeCallbackv)(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_valueFreeCallbackv";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, FILENAMES, __LINE__); */

  if (contexti == ESLIF_LUA_CONTEXT) {
    /* This is a pointer to an integer that contains a global reference to the value */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) releasing global reference %d at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, * (int *) p, FILENAMES, __LINE__); */
    MARPAESLIFLUA_UNREF(L, * (int *) p);
    free(p);
  }
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformUndefb)(void *userDatavp, int contexti)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformUndefb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d) at %s:%d", funcs, L, contexti, FILENAMES, __LINE__); */

  lua_pushnil(valueContextp->L);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformCharb)(void *userDatavp, int contexti, char c)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformCharb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, c=%c) at %s:%d", funcs, L, contexti, c, FILENAMES, __LINE__); */

  lua_pushlstring(valueContextp->L, &c, 1);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformShortb)(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformShortb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, b=%d) at %s:%d", funcs, L, contexti, (int) b, FILENAMES, __LINE__); */

  lua_pushinteger(valueContextp->L, (lua_Integer) b);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformIntb)(void *userDatavp, int contexti, int i)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformIntb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, i=%d) at %s:%d", funcs, L, contexti, i, FILENAMES, __LINE__); */

  lua_pushinteger(valueContextp->L, (lua_Integer) i);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformLongb)(void *userDatavp, int contexti, long l)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformLongb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, l=%ld) at %s:%d", funcs, L, contexti, l, FILENAMES, __LINE__); */

  lua_pushinteger(valueContextp->L, (lua_Integer) l);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformFloatb)(void *userDatavp, int contexti, float f)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformFloatb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, f=%f) at %s:%d", funcs, L, contexti, (double) f, FILENAMES, __LINE__); */

  lua_pushnumber(valueContextp->L, (lua_Number) f);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformDoubleb)(void *userDatavp, int contexti, double d)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformDoubleb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, d=%f) at %s:%d", funcs, L, contexti, d, FILENAMES, __LINE__); */

  lua_pushnumber(valueContextp->L, (lua_Number) d);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformPtrb)(void *userDatavp, int contexti, void *p)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformPtrb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p) at %s:%d", funcs, L, contexti, p, FILENAMES, __LINE__); */

  if (contexti == ESLIF_LUA_CONTEXT) {
    /* This is a pointer to an integer value that is a global reference to the real value */
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p) pushing value with global reference %d at %s:%d", funcs, L, contexti, p, * (int *) p, FILENAMES, __LINE__); */
    MARPAESLIFLUA_DEREF(L, * (int *) p);
  } else {
    lua_pushlightuserdata(valueContextp->L, p);
  }

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformArrayb)(void *userDatavp, int contexti, void *p, size_t sizel)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformArrayb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, p=%p, sizel=%ld) at %s:%d", funcs, L, contexti, p, (unsigned long) sizel, FILENAMES, __LINE__); */

  lua_pushlstring(valueContextp->L, p, sizel);

  return 1;
}

/*****************************************************************************/
static short MARPAESLIFLUA_METHOD(transformBoolb)(void *userDatavp, int contexti, short b)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_transformBoolb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (contexti=%d, b=%s) at %s:%d", funcs, L, contexti, b ? "true" : "false", FILENAMES, __LINE__); */

  lua_pushboolean(valueContextp->L, (int) b);

  return 1;
}

/*****************************************************************************/
static void MARPAESLIFLUA_METHOD(pushValuev)(valueContext_t *valueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, char *bytep, size_t bytel)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_pushValuev";
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  lua_State               *L = valueContextp->L;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) at %s:%d", funcs, L, stackindicei, FILENAMES, __LINE__); */

  if (bytep != NULL) {
    /* Fake a marpaESLIFValueResult */
    marpaESLIFValueResult.type     = MARPAESLIF_VALUE_TYPE_ARRAY;
    marpaESLIFValueResult.contexti = 0;
    marpaESLIFValueResult.sizel    = bytel;
    marpaESLIFValueResult.u.p      = bytep;
    marpaESLIFValueResultp         = &marpaESLIFValueResult;
  } else {
    marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, stackindicei);
    if (marpaESLIFValueResultp == NULL) {
      luaL_error(L, "marpaESLIFValueResultp is NULL at stack indice %d", stackindicei);
    }
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is of type %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->type, FILENAMES, __LINE__); */
  /*
  if (marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_INT) {
    if (marpaESLIFValueResultp->contexti == ESLIF_LUA_CONTEXT) {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is an INT with context == ESLIF_LUA_CONTEXT, value is %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->u.i, FILENAMES, __LINE__);
    } else {
      GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) (stackindice=%d) marpaESLIFValueResultp is an INT with context != ESLIF_LUA_CONTEXT, value is %d at %s:%d", funcs, L, stackindicei, marpaESLIFValueResultp->u.i, FILENAMES, __LINE__);
    }
  }
  */
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
static short MARPAESLIFLUA_METHOD(representationb)(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp)
/*****************************************************************************/
{
  static const char *funcs         = "marpaESLIFLua_representationb";
  valueContext_t    *valueContextp = (valueContext_t *) userDatavp;
  lua_State         *L             = valueContextp->L;
  const char        *s;
  size_t             l;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  MARPAESLIFLUA_METHOD(valueContextCleanupv)(valueContextp);

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) *inputcpp=\"%s\" at %s:%d", funcs, L, (*inputcpp != NULL) ? *inputcpp : "<undef>", FILENAMES, __LINE__); */
  /* Always return a true value, else ::concat will abort */
  return 1;
}

/*****************************************************************************/
static void MARPAESLIFLUA_METHOD(iterate_and_print)(lua_State *L, int index)
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
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newi)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_newi";
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  recognizerContext_t          *recognizerContextp;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_new(marpaESLIFGrammarp, recognizerInterface)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFGrammarp must be a table");
  }

  lua_getfield(L, 1, "marpaESLIFGrammarp");      /* stack: marpaESLIFGrammarTable, recognizerInterface, marpaESLIFGrammarp */
  marpaESLIFGrammarp = lua_touserdata(L, -1);    /* stack: marpaESLIFGrammarTable, recognizerInterface, marpaESLIFGrammarp */
  lua_pop(L, 1);                                 /* stack: marpaESLIFGrammarTable, recognizerInterface */

  MARPAESLIFLUA_METHOD(paramIsRecognizerInterfacev)(L, 2);

  recognizerContextp = (recognizerContext_t *) malloc(sizeof(recognizerContext_t));
  if (recognizerContextp == NULL) {
    return luaL_error(L, "malloc failure, %s", strerror(errno));
  }

  MARPAESLIFLUA_METHOD(recognizerContextInitv)(L, 1 /* grammarStacki */, 2 /* recognizerInterfaceStacki */, 0 /* recognizerOrigStacki */, recognizerContextp);

  /* We need a lexeme stack in this mode (in contrary to the parse() method that never calls back) */
  GENERICSTACK_NEW(recognizerContextp->lexemeStackp);
  if (recognizerContextp->lexemeStackp == NULL) {
    int save_errno = errno;
    MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "GENERICSTACK_NEW failure, %s", strerror(save_errno));
  }

  marpaESLIFRecognizerOption.userDatavp        = recognizerContextp;
  marpaESLIFRecognizerOption.readerCallbackp   = MARPAESLIFLUA_METHOD(readerCallbackb);
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
    MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContextp, 0 /* onStackb */);
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
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_freei)); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                   /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "newFrom", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newFromi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "set_exhausted_flag", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_set_exhausted_flagi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "share", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_sharei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isCanContinue", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isCanContinuei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isExhausted", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isExhaustedi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "scan", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_scani));
  MARPAESLIFLUA_STORE_FUNCTION(L, "resume", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_resumei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "events", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventsi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "eventOnOff", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventOnOffi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeAlternative", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeAlternativei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeComplete", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeCompletei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeRead", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeReadi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "discardTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeExpected", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeExpectedi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeLastPause", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastPausei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "discardLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardLastTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isEof", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isEofi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "read", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_readi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "input", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_inputi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "progressLog", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_progressLogi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedOffset", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedOffseti));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedLength", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLenghti));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedLocation", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLocationi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "line", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_linei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "column", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_columni));
  MARPAESLIFLUA_STORE_FUNCTION(L, "location", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_locationi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "hookDiscard", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_hookDiscardi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFValue_new", MARPAESLIFLUA_METHOD(marpaESLIFValue_newi));
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                           /* stack: {["recognizerContextp"] =>recognizerContextp, meta=>{...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({recognizerContextp->marpaESLIFRecognizerp=%p}) at %s:%d", funcs, L, recognizerContextp->marpaESLIFRecognizerp, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_freei)(lua_State *L)
/****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFRecognizer_freei";
  recognizerContext_t *recognizerContextp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  lua_getfield(L, -1, "recognizerContextp"); /* stack: {...}, recognizerContextp */
  recognizerContextp = (recognizerContext_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizerContext=%p at %s:%d", funcs, L, recognizerContextp, FILENAMES, __LINE__); */
  MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContextp, 0 /* onStackb */);

  lua_pop(L, 1);                             /* stack: */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newFromi)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_newFromi";
  recognizerContext_t          *recognizerContextFromp;
  recognizerContext_t          *recognizerContextp;
  marpaESLIFGrammar_t          *marpaESLIFGrammarp;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  MARPAESLIFLUA_METHOD(recognizerContextInitv)(L, 2 /* grammarStacki */, 0 /* recognizerInterfaceStacki */, 1 /* recognizerOrigStacki */, recognizerContextp);

  /* We need a lexeme stack in this mode (in contrary to the parse() method that never calls back) */
  GENERICSTACK_NEW(recognizerContextp->lexemeStackp);
  if (recognizerContextp->lexemeStackp == NULL) {
    int save_errno = errno;
    MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContextp, 0 /* onStackb */);
    return luaL_error(L, "GENERICSTACK_NEW failure, %s", strerror(save_errno));
  }

  recognizerContextp->marpaESLIFRecognizerp = marpaESLIFRecognizer_newFromp(marpaESLIFGrammarp, recognizerContextFromp->marpaESLIFRecognizerp);
  if (recognizerContextp->marpaESLIFRecognizerp == NULL) {
    int save_errno = errno;
    MARPAESLIFLUA_METHOD(recognizerContextFreev)(recognizerContextp, 0 /* onStackb */);
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
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_freei)); /* ... Associate a garbage collector */
  lua_newtable(L);                                                                   /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "newFrom", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_newFromi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "set_exhausted_flag", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_set_exhausted_flagi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "share", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_sharei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isCanContinue", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isCanContinuei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isExhausted", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isExhaustedi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "scan", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_scani));
  MARPAESLIFLUA_STORE_FUNCTION(L, "resume", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_resumei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "events", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventsi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "eventOnOff", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventOnOffi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeAlternative", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeAlternativei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeComplete", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeCompletei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeRead", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeReadi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "discardTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeExpected", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeExpectedi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeLastPause", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastPausei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lexemeLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "discardLastTry", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardLastTryi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "isEof", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isEofi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "read", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_readi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "input", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_inputi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "progressLog", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_progressLogi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedOffset", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedOffseti));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedLength", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLenghti));
  MARPAESLIFLUA_STORE_FUNCTION(L, "lastCompletedLocation", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLocationi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "line", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_linei));
  MARPAESLIFLUA_STORE_FUNCTION(L, "column", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_columni));
  MARPAESLIFLUA_STORE_FUNCTION(L, "location", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_locationi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "hookDiscard", MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_hookDiscardi));
  MARPAESLIFLUA_STORE_FUNCTION(L, "marpaESLIFValue_new", MARPAESLIFLUA_METHOD(marpaESLIFValue_newi));
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                           /* stack: {["recognizerContextp"] =>recognizerContextp, meta=>{...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({recognizerContextp->marpaESLIFRecognizerp=%p}) at %s:%d", funcs, L, recognizerContextp->marpaESLIFRecognizerp, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_set_exhausted_flagi)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_set_exhausted_flagi";
  recognizerContext_t          *recognizerContextp;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_sharei)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_sharei";
  recognizerContext_t          *recognizerContextp;
  recognizerContext_t          *recognizerContextSharedp;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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
    /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__); */
    MARPAESLIFLUA_UNREF(L, recognizerContextp->recognizer_orig_r);
  }

  lua_pushnil(L);                                              /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, nil */
  lua_copy(L, 2, -1);                                          /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, marpaESLIFRecognizerSharedTable */
  MARPAESLIFLUA_REF(L, recognizerContextp->recognizer_orig_r); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) got recognizer_orig_r=%d from registry at %s:%d", funcs, L, recognizerContextp->recognizer_orig_r, FILENAMES, __LINE__); */

  if (! marpaESLIFRecognizer_shareb(recognizerContextp->marpaESLIFRecognizerp, recognizerContextSharedp->marpaESLIFRecognizerp)) {
    return luaL_error(L, "marpaESLIFRecognizer_shareb failure, %s", strerror(errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isCanContinuei)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_isCanContinuei";
  recognizerContext_t          *recognizerContextp;
  short                         isCanContinueb;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

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

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (isCanContinueb=%s) at %s:%d", funcs, L, isCanContinueb ? "true" : "false", FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isExhaustedi)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_isExhaustedi";
  recognizerContext_t          *recognizerContextp;
  short                         exhaustedb;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_isexhausted(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }

  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, marpaESLIFRecognizerSharedTable */

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_isExhaustedb(recognizerContextp->marpaESLIFRecognizerp, &exhaustedb)) {
    return luaL_error(L, "marpaESLIFRecognizer_isExhaustedb failure, %s", strerror(errno));
  }

  lua_pushboolean(L, exhaustedb);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 (exhaustedb=%s) at %s:%d", funcs, L, exhaustedb ? "true" : "false", FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_scani)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_scani";
  recognizerContext_t          *recognizerContextp;
  short                         initialEventsb = 0;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  switch (lua_gettop(L)) {
  case 2:
    if (lua_type(L, 2) != LUA_TBOOLEAN) {
      return luaL_error(L, "Usage: marpaESLIFRecognizer_scan(marpaESLIFRecognizerp, initialEvents)");
    }
    initialEventsb = lua_toboolean(L, 2) ? 1 : 0;
    /* Intentionnaly no break here */
  case 1:
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "marpaESLIFRecognizerp must be a table");
    }
    lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, initialEventsb?, recognizerContextFromp */
    recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, initialEventsb?, recognizerContextFromp */
    lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, initialEventsb? */
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIFRecognizer_scan(marpaESLIFRecognizerp[, initialEvents])");
  }

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_scanb(recognizerContextp->marpaESLIFRecognizerp, initialEventsb, NULL /* continuebp */, NULL /* exhaustedbp */));

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_resumei)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_resumei";
  recognizerContext_t          *recognizerContextp;
  int                           isNumi;
  int                           deltaLengthi = 0;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  switch (lua_gettop(L)) {
  case 2:
    if (lua_type(L, 2) != LUA_TNUMBER) {
      return luaL_error(L, "Usage: marpaESLIFRecognizer_scan(marpaESLIFRecognizerp, deltaLength)");
    }
    deltaLengthi = (int) lua_tointegerx(L, 2, &isNumi);
    if (! isNumi) {
      return luaL_error(L, "Failed to convert deltaLength argument to a number");
    }
    if (deltaLengthi < 0) {
      return luaL_error(L, "deltaLength argument cannot be negative");
    }
    /* Intentionnaly no break here */
  case 1:
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "marpaESLIFRecognizerp must be a table");
    }
    lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, deltaLength?, recognizerContextFromp */
    recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, deltaLength?, recognizerContextFromp */
    lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, deltaLength? */
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIFRecognizer_resume(marpaESLIFRecognizerp[, deltaLength])");
  }

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_resumeb(recognizerContextp->marpaESLIFRecognizerp, (size_t) deltaLengthi, NULL /* continuebp */, NULL /* exhaustedbp */));

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventsi)(lua_State *L)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFLua_marpaESLIFRecognizer_eventsi";
  recognizerContext_t          *recognizerContextp;
  size_t                        i;
  size_t                        eventArrayl;
  marpaESLIFEvent_t            *eventArrayp;
 
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_events(marpaESLIFRecognizerp)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, initialEventsb?, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, initialEventsb?, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, initialEventsb? */

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_eventb(recognizerContextp->marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    return luaL_error(L, "marpaESLIFRecognizer_eventb failure, %s", strerror(errno));
  }

  lua_newtable(L);                                                        /* stack: {} */
  for (i = 0; i < eventArrayl; i++) {
    lua_newtable(L);                                                      /* stack: {}, {} */
    MARPAESLIFLUA_STORE_INTEGER(L, "type", eventArrayp[i].type);          /* stack: {}, {"type" => type} */
    MARPAESLIFLUA_STORE_ASCIISTRING(L, "symbol", eventArrayp[i].symbols); /* stack: {}, {"type" => type, "symbol" => symbol} */
    MARPAESLIFLUA_STORE_ASCIISTRING(L, "event", eventArrayp[i].events);   /* stack: {}, {"type" => type, "symbol" => symbol, "event" => event} */
    lua_rawseti(L, 1, (int) i);                                           /* stack: {i => {"type" => type, "symbol" => symbol, "event" => event}} */
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_eventOnOffi)(lua_State *L)
/*****************************************************************************/
{
  static const char     *funcs = "marpaESLIFLua_marpaESLIFRecognizer_eventOnOffi";
  recognizerContext_t   *recognizerContextp;
  const char            *symbols;
  int                    isNumi;
  int                    codei;
  marpaESLIFEventType_t  eventSeti = MARPAESLIF_EVENTTYPE_NONE;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 4) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_eventOnOff(marpaESLIFRecognizerp, symbol, eventTypes, onOff)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");   /* stack: marpaESLIFRecognizerTable, symbol, eventTypes, onOff, recognizerContextFromp */
  recognizerContextp = lua_touserdata(L, -1); /* stack: marpaESLIFRecognizerTable, symbol, eventTypes, onOff, recognizerContextFromp */
  lua_pop(L, 1);                              /* stack: marpaESLIFRecognizerTable, symbol, eventTypes, onOff */

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "symbol must be a string");
  }
  symbols = lua_tostring(L, 2);

  if (lua_type(L, 3) != LUA_TTABLE) {
    return luaL_error(L, "eventTypes must be a table");
  }
  lua_pushnil(L);
  while (lua_next(L, 3) != 0) {
    codei = (int) lua_tonumberx(L, -1, &isNumi);
    if (! isNumi) {
      return luaL_error(L, "Failed to convert event type %s to a number", lua_tostring(L, -1));
    }
    switch (codei) {
    case MARPAESLIF_EVENTTYPE_NONE:
      break;
    case MARPAESLIF_EVENTTYPE_COMPLETED:
    case MARPAESLIF_EVENTTYPE_NULLED:
    case MARPAESLIF_EVENTTYPE_PREDICTED:
    case MARPAESLIF_EVENTTYPE_BEFORE:
    case MARPAESLIF_EVENTTYPE_AFTER:
    case MARPAESLIF_EVENTTYPE_EXHAUSTED:
    case MARPAESLIF_EVENTTYPE_DISCARD:
      eventSeti |= codei;
      break;
    default:
      return luaL_error(L, "Unknown code %d", (int) codei);
    }
    lua_pop(L, 1);
  }

  if (lua_type(L, 4) != LUA_TBOOLEAN) {
    return luaL_error(L, "onOff must be a boolean");
  }

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_event_onoffb(recognizerContextp->marpaESLIFRecognizerp, (char *) symbols, eventSeti, lua_toboolean(L, 4) ? 1 : 0)) {
    return luaL_error(L, "marpaESLIFRecognizer_event_onoffb failure, %s", strerror(errno));
  }


  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeAlternativei)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs          = "marpaESLIFLua_marpaESLIFRecognizer_lexemeAlternativei";
  size_t                   grammarLengthl = 1;
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  int                      isNumi;
  int                      refi;
  marpaESLIFAlternative_t  marpaESLIFAlternative;
  int                     *p;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  switch (lua_gettop(L)) {
  case 4:
    if (lua_type(L, 4) != LUA_TNUMBER) {
      return luaL_error(L, "grammarLength must be a number");
    }
    grammarLengthl = (size_t) lua_tonumberx(L, 4, &isNumi);
    if (! isNumi) {
      return luaL_error(L, "Failed to convert grammarLength %s to a number", lua_tostring(L, 4));
    }
    /* Intentionnaly no break */
  case 3:
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "marpaESLIFRecognizerp must be a table");
    }
    lua_getfield(L, 1, "recognizerContextp");
    recognizerContextp = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (lua_type(L, 2) != LUA_TSTRING) {
      return luaL_error(L, "name must be a string");
    }
    names = lua_tostring(L, 2);

    /* We make a reference to the value and remember that in an (int *) pointer */
    /* in order to re-use the marpaESLIFLua_representationb method */
    lua_pushnil(L);
    lua_copy(L, 3, -1);
    MARPAESLIFLUA_REF(L, refi);
    
    p = malloc(sizeof(int));
    if (p == NULL) {
      return luaL_error(L, "malloc failure, %s", strerror(errno));
    }
    *p = refi;

    /* And remember that reference */
    GENERICSTACK_PUSH_INT(recognizerContextp->lexemeStackp, refi);
    if (GENERICSTACK_ERROR(recognizerContextp->lexemeStackp)) {
      return luaL_error(L, "lexemeStackp push failure, %s", strerror(errno));
    }
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeAlternative(marpaESLIFRecognizerp, name, value[, grammarLength])");
  }

  marpaESLIFAlternative.lexemes               = (char *) names;
  marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.u.p             = p;
  marpaESLIFAlternative.value.contexti        = ESLIF_LUA_CONTEXT;
  marpaESLIFAlternative.value.sizel           = 0; /* Not used */
  marpaESLIFAlternative.value.representationp = MARPAESLIFLUA_METHOD(representationb);
  marpaESLIFAlternative.value.shallowb        = 0; /* C.f. marpaESLIF_valueFreeCallbackv */
  marpaESLIFAlternative.grammarLengthl        = grammarLengthl;

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_lexeme_alternativeb(recognizerContextp->marpaESLIFRecognizerp, &marpaESLIFAlternative));

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeCompletei)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs          = "marpaESLIFLua_marpaESLIFRecognizer_lexemeCompletei";
  recognizerContext_t     *recognizerContextp;
  size_t                   lengthl;
  int                      isNumi;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeComplete(marpaESLIFRecognizerp, length)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TNUMBER) {
    return luaL_error(L, "length must be a number");
  }
  lengthl = (size_t) lua_tonumberx(L, 2, &isNumi);
  if (! isNumi) {
    return luaL_error(L, "Failed to convert length %s to a number", lua_tostring(L, 2));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_lexeme_completeb(recognizerContextp->marpaESLIFRecognizerp, lengthl));

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeReadi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs          = "marpaESLIFLua_marpaESLIFRecognizer_lexemeReadi";
  size_t                   grammarLengthl = 1;
  size_t                   lengthl;
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  int                      isNumi;
  int                      refi;
  marpaESLIFAlternative_t  marpaESLIFAlternative;
  int                     *p;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  switch (lua_gettop(L)) {
  case 5:
    if (lua_type(L, 5) != LUA_TNUMBER) {
      return luaL_error(L, "grammarLength must be a number");
    }
    grammarLengthl = (size_t) lua_tonumberx(L, 5, &isNumi);
    if (! isNumi) {
      return luaL_error(L, "Failed to convert grammarLength %s to a number", lua_tostring(L, 5));
    }
    /* Intentionnaly no break */
  case 4:
    if (lua_type(L, 1) != LUA_TTABLE) {
      return luaL_error(L, "marpaESLIFRecognizerp must be a table");
    }
    lua_getfield(L, 1, "recognizerContextp");
    recognizerContextp = lua_touserdata(L, -1);
    lua_pop(L, 1);

    if (lua_type(L, 2) != LUA_TSTRING) {
      return luaL_error(L, "name must be a string");
    }
    names = lua_tostring(L, 2);

    /* We make a reference to the value and remember that in an (int *) pointer */
    /* in order to re-use the marpaESLIFLua_representationb method */
    lua_pushnil(L);
    lua_copy(L, 3, -1);
    MARPAESLIFLUA_REF(L, refi);
    
    p = malloc(sizeof(int));
    if (p == NULL) {
      return luaL_error(L, "malloc failure, %s", strerror(errno));
    }
    *p = refi;

    /* And remember that reference */
    GENERICSTACK_PUSH_INT(recognizerContextp->lexemeStackp, refi);
    if (GENERICSTACK_ERROR(recognizerContextp->lexemeStackp)) {
      return luaL_error(L, "lexemeStackp push failure, %s", strerror(errno));
    }

    if (lua_type(L, 4) != LUA_TNUMBER) {
      return luaL_error(L, "length must be a number");
    }
    lengthl = (size_t) lua_tonumberx(L, 4, &isNumi);
    if (! isNumi) {
      return luaL_error(L, "Failed to convert length %s to a number", lua_tostring(L, 4));
    }
    break;
  default:
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeRead(marpaESLIFRecognizerp, name, value, length[, grammarLength])");
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) setting p=%p that points to refi=%d at %s:%d", funcs, L, p, refi, FILENAMES, __LINE__); */

  marpaESLIFAlternative.lexemes               = (char *) names;
  marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFAlternative.value.u.p             = p;
  marpaESLIFAlternative.value.contexti        = ESLIF_LUA_CONTEXT;
  marpaESLIFAlternative.value.sizel           = 0; /* Not used */
  marpaESLIFAlternative.value.representationp = MARPAESLIFLUA_METHOD(representationb);
  marpaESLIFAlternative.value.shallowb        = 0; /* C.f. marpaESLIFLua_valueFreeCallbackv */
  marpaESLIFAlternative.grammarLengthl        = grammarLengthl;

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_lexeme_readb(recognizerContextp->marpaESLIFRecognizerp, &marpaESLIFAlternative, lengthl));

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeTryi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lexemeTryi";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  short                    rcb;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeTry(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_lexeme_tryb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, &rcb)) {
    return luaL_error(L, "marpaESLIFRecognizer_lexeme_tryb failure, %s", strerror(errno));
  }
  
  lua_pushboolean(L, rcb);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardTryi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_discardTryi";
  recognizerContext_t     *recognizerContextp;
  short                    rcb;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_discardTry(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_discard_tryb(recognizerContextp->marpaESLIFRecognizerp, &rcb)) {
    return luaL_error(L, "marpaESLIFRecognizer_discard_tryb failure, %s", strerror(errno));
  }
  
  lua_pushboolean(L, rcb);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeExpectedi)(lua_State *L)
/*****************************************************************************/
{
  static const char    *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lexemeExpectedi";
  recognizerContext_t  *recognizerContextp;
  size_t                nLexeme;
  char                **lexemesArrayp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeExpected(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_lexeme_expectedb(recognizerContextp->marpaESLIFRecognizerp, &nLexeme, &lexemesArrayp)) {
    return luaL_error(L, "marpaESLIFRecognizer_lexeme_expectedb failure, %s", strerror(errno));
  }
  
  MARPAESLIFLUA_PUSH_ASCIISTRING_ARRAY(L, nLexeme, lexemesArrayp);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastPausei)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lexemeLastPausei";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  char                    *pauses;
  size_t                   pausel;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeLastPause(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_lexeme_last_pauseb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, &pauses, &pausel)) {
    return luaL_error(L, "marpaESLIFRecognizer_lexeme_last_pauseb failure, %s", strerror(errno));
  }
  
  if ((pauses != NULL) && (pausel > 0)) {
    lua_pushlstring(L, (const char *) pauses, pausel);
  } else {
    lua_pushnil(L);
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lexemeLastTryi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lexemeLastTryi";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  char                    *trys;
  size_t                   tryl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lexemeLastTry(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_lexeme_last_tryb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, &trys, &tryl)) {
    return luaL_error(L, "marpaESLIFRecognizer_lexeme_last_tryb failure, %s", strerror(errno));
  }
  
  if ((trys != NULL) && (tryl > 0)) {
    lua_pushlstring(L, (const char *) trys, tryl);
  } else {
    lua_pushnil(L);
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_discardLastTryi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_discardLastTryi";
  recognizerContext_t     *recognizerContextp;
  char                    *trys;
  size_t                   tryl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_discardLastTry(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_discard_last_tryb(recognizerContextp->marpaESLIFRecognizerp, &trys, &tryl)) {
    return luaL_error(L, "marpaESLIFRecognizer_discard_last_tryb failure, %s", strerror(errno));
  }
  
  if ((trys != NULL) && (tryl > 0)) {
    lua_pushlstring(L, (const char *) trys, tryl);
  } else {
    lua_pushnil(L);
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_isEofi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_isEofi";
  recognizerContext_t     *recognizerContextp;
  short                    eofb;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_isEof(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_isEofb(recognizerContextp->marpaESLIFRecognizerp, &eofb)) {
    return luaL_error(L, "marpaESLIFRecognizer_isEofb failure, %s", strerror(errno));
  }
  
  lua_pushboolean(L, eofb ? 1 : 0);
  
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_readi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_readi";
  recognizerContext_t     *recognizerContextp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_read(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  lua_pushboolean(L, marpaESLIFRecognizer_readb(recognizerContextp->marpaESLIFRecognizerp, NULL, NULL) ? 1 : 0);
  
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_inputi)(lua_State *L)
/*****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFRecognizer_inputi";
  recognizerContext_t *recognizerContextp;
  char                *inputs;
  size_t               inputl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_read(marpaESLIFRecognizerp)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_inputb(recognizerContextp->marpaESLIFRecognizerp, &inputs, &inputl)) {
    return luaL_error(L, "marpaESLIFRecognizer_inputb failure, %s", strerror(errno));
  }

  if ((inputs != NULL) && (inputl > 0)) {
    lua_pushlstring(L, (const char *) inputs, inputl);
  } else {
    lua_pushnil(L);
  }
  
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_progressLogi)(lua_State *L)
/*****************************************************************************/
{
  static const char   *funcs = "marpaESLIFLua_marpaESLIFRecognizer_progressLogi";
  recognizerContext_t *recognizerContextp;
  int                  starti;
  int                  endi;
  int                  leveli;
  int                  isNumi;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 4) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_read(marpaESLIFRecognizerp, start, end, level)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TNUMBER) {
    return luaL_error(L, "start must be a number");
  }
  starti = (int) lua_tointegerx(L, 2, &isNumi);
  if (! isNumi) {
    return luaL_error(L, "Failed to convert start %s to a number", lua_tostring(L, 2));
  }

  if (lua_type(L, 3) != LUA_TNUMBER) {
    return luaL_error(L, "end must be a number");
  }
  endi = (int) lua_tointegerx(L, 2, &isNumi);
  if (! isNumi) {
    return luaL_error(L, "Failed to convert end %s to a number", lua_tostring(L, 3));
  }

  if (lua_type(L, 4) != LUA_TNUMBER) {
    return luaL_error(L, "level must be a number");
  }
  leveli = (int) lua_tointegerx(L, 4, &isNumi);
  if (! isNumi) {
    return luaL_error(L, "Failed to convert level %s to a number", lua_tostring(L, 4));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  switch (leveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
  case GENERICLOGGER_LOGLEVEL_DEBUG:
  case GENERICLOGGER_LOGLEVEL_INFO:
  case GENERICLOGGER_LOGLEVEL_NOTICE:
  case GENERICLOGGER_LOGLEVEL_WARNING:
  case GENERICLOGGER_LOGLEVEL_ERROR:
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
  case GENERICLOGGER_LOGLEVEL_ALERT:
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    break;
  default:
    return luaL_error(L, "Unknown logger level %d", leveli);
    break;
  }

  if (! marpaESLIFRecognizer_progressLogb(recognizerContextp->marpaESLIFRecognizerp, starti, endi, leveli)) {
    return luaL_error(L, "marpaESLIFRecognizer_progressLogb failure, %s", strerror(errno));
  }
  
  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedOffseti)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lastCompletedOffseti";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  char                    *offsetp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lastCompletedOffset(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_last_completedb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, &offsetp, NULL /* lengthlp */)) {
    return luaL_error(L, "marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  
  lua_pushinteger(L, (lua_Integer) offsetp);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLenghti)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lastCompletedLenghti";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  size_t                   lengthl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lastCompletedLength(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_last_completedb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, NULL /* offsetpp */, &lengthl)) {
    return luaL_error(L, "marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }
  
  lua_pushinteger(L, (lua_Integer) lengthl);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_lastCompletedLocationi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_lastCompletedLocationi";
  recognizerContext_t     *recognizerContextp;
  const char              *names;
  char                    *offsetp;
  size_t                   lengthl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_lastCompletedLocation(marpaESLIFRecognizerp, name)");
  }
  
  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_error(L, "name must be a string");
  }
  names = lua_tostring(L, 2);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_last_completedb(recognizerContextp->marpaESLIFRecognizerp, (char *) names, &offsetp, &lengthl)) {
    return luaL_error(L, "marpaESLIFRecognizer_last_completedb failure, %s", strerror(errno));
  }

  lua_newtable(L);
  MARPAESLIFLUA_STORE_INTEGER(L, "offset", offsetp);
  MARPAESLIFLUA_STORE_INTEGER(L, "length", lengthl);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_linei)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_linei";
  recognizerContext_t     *recognizerContextp;
  size_t                   linel;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_line(marpaESLIFRecognizerp)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_locationb(recognizerContextp->marpaESLIFRecognizerp, &linel, NULL /* columnlp */)) {
    return luaL_error(L, "marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }

  lua_pushinteger(L, (lua_Integer) linel);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_columni)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_columni";
  recognizerContext_t     *recognizerContextp;
  size_t                   columnl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_column(marpaESLIFRecognizerp)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_locationb(recognizerContextp->marpaESLIFRecognizerp, NULL /* linelp */, &columnl)) {
    return luaL_error(L, "marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }

  lua_pushinteger(L, (lua_Integer) columnl);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_locationi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_locationi";
  recognizerContext_t     *recognizerContextp;
  size_t                   linel;
  size_t                   columnl;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 1) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_location(marpaESLIFRecognizerp)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_locationb(recognizerContextp->marpaESLIFRecognizerp, &linel, &columnl)) {
    return luaL_error(L, "marpaESLIFRecognizer_locationb failure, %s", strerror(errno));
  }

  lua_createtable(L, 2, 0);                                                 /* stack; {} */
  MARPAESLIFLUA_STORE_INTEGER(L, "line",   linel);
  MARPAESLIFLUA_STORE_INTEGER(L, "column", columnl);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFRecognizer_hookDiscardi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFRecognizer_hookDiscardi";
  recognizerContext_t     *recognizerContextp;
  short                    discardOnOffb;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFRecognizer_hookDiscard(marpaESLIFRecognizerp, discardOnOff)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  if (lua_type(L, 2) != LUA_TBOOLEAN) {
    return luaL_error(L, "discardOnOff must be a boolean");
  }
  discardOnOffb = lua_toboolean(L, 2) ? 1 : 0;

  /* Clear the stack */
  lua_settop(L, 0);

  if (! marpaESLIFRecognizer_hook_discardb(recognizerContextp->marpaESLIFRecognizerp, discardOnOffb)) {
    return luaL_error(L, "marpaESLIFRecognizer_hook_discardb failure, %s", strerror(errno));
  }

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/*****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFValue_newi)(lua_State *L)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFLua_marpaESLIFValue_newi";
  marpaESLIFValueOption_t  marpaESLIFValueOption;
  recognizerContext_t     *recognizerContextp;
  valueContext_t          *valueContextp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  if (lua_gettop(L) != 2) {
    return luaL_error(L, "Usage: marpaESLIFValue_new(marpaESLIFRecognizerp, valueInterface)");
  }

  if (lua_type(L, 1) != LUA_TTABLE) {
    return luaL_error(L, "marpaESLIFRecognizerp must be a table");
  }
  lua_getfield(L, 1, "recognizerContextp");
  recognizerContextp = lua_touserdata(L, -1);
  lua_pop(L, 1);

  MARPAESLIFLUA_METHOD(paramIsValueInterfacev)(L, 2);

  valueContextp = (valueContext_t *) malloc(sizeof(valueContext_t));
  if (valueContextp == NULL) {
    return luaL_error(L, "malloc failure, %s", strerror(errno));
  }

  MARPAESLIFLUA_METHOD(valueContextInitv)(L, 0 /* grammarStacki */, 1 /* recognizerStacki */, 2 /* valueInterfaceStacki */, valueContextp);

  marpaESLIFValueOption.userDatavp             = valueContextp;
  marpaESLIFValueOption.ruleActionResolverp    = MARPAESLIFLUA_METHOD(valueRuleActionResolver);
  marpaESLIFValueOption.symbolActionResolverp  = MARPAESLIFLUA_METHOD(valueSymbolActionResolver);
  marpaESLIFValueOption.freeActionResolverp    = MARPAESLIFLUA_METHOD(valueFreeActionResolver);
  marpaESLIFValueOption.transformerp           = &marpaESLIFValueResultTransformDefault;
  MARPAESLIFLUA_CALLBACKB(L, valueContextp->value_r, "isWithHighRankOnly", 0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.highRankOnlyb));
  MARPAESLIFLUA_CALLBACKB(L, valueContextp->value_r, "isWithOrderByRank",  0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.orderByRankb));
  MARPAESLIFLUA_CALLBACKB(L, valueContextp->value_r, "isWithAmbiguous",    0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.ambiguousb));
  MARPAESLIFLUA_CALLBACKB(L, valueContextp->value_r, "isWithNull",         0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.nullb));
  MARPAESLIFLUA_CALLBACKI(L, valueContextp->value_r, "maxParses",          0 /* nargs */, MARPAESLIFLUA_NOOP, &(marpaESLIFValueOption.maxParsesi));

  valueContextp->marpaESLIFValuep = marpaESLIFValue_newp(recognizerContextp->marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (valueContextp->marpaESLIFValuep == NULL) {
    int save_errno = errno;
    MARPAESLIFLUA_METHOD(valueContextFreev)(valueContextp, 0 /* onStackb */);
    return luaL_error(L, "marpaESLIFValue_newp failure, %s", strerror(save_errno));
  }

  /* Clear the stack */
  lua_settop(L, 0);

  /* We will return a table */
  lua_newtable(L);                                                                   /* stack: {} */
  MARPAESLIFLUA_STORE_USERDATA(L, "valueContextp", valueContextp);                   /* stack: {["valueContextp"] =>valueContextp */

  /* Create a metable */
  lua_newtable(L);
  MARPAESLIFLUA_STORE_ASCIISTRING(L, "__mode", "v");                                 /* ... Say the values are weak */
  MARPAESLIFLUA_STORE_FUNCTION(L, "__gc", MARPAESLIFLUA_METHOD(marpaESLIFValue_freei));      /* ... Associate a garbage collector */
  lua_newtable(L);                                                                   /* ... Associate methods */
  MARPAESLIFLUA_STORE_FUNCTION(L, "value", MARPAESLIFLUA_METHOD(marpaESLIFValue_valuei));
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);                                                           /* stack: {["recognizerContextp"] =>recognizerContextp, meta=>{...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 ({recognizerContextp->marpaESLIFRecognizerp=%p}) at %s:%d", funcs, L, recognizerContextp->marpaESLIFRecognizerp, FILENAMES, __LINE__); */
  return 1;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFValue_freei)(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_marpaESLIFValue_freei";
  valueContext_t    *valueContextp;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  lua_getfield(L, -1, "valueContextp"); /* stack: {...}, valueContextp */
  valueContextp = (valueContext_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) freeing valueContext=%p at %s:%d", funcs, L, valueContextp, FILENAMES, __LINE__); */
  MARPAESLIFLUA_METHOD(valueContextFreev)(valueContextp, 0 /* onStackb */);

  lua_pop(L, 1);                             /* stack: */

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 0 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 0;
}

/****************************************************************************/
static int MARPAESLIFLUA_METHOD(marpaESLIFValue_valuei)(lua_State *L)
/****************************************************************************/
{
  static const char *funcs = "marpaESLIFLua_marpaESLIFValue_valuei";
  valueContext_t    *valueContextp;
  short              valueb;
  int                rci;
  int                resultStacki;

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) at %s:%d", funcs, L, FILENAMES, __LINE__); */

  lua_getfield(L, 1, "valueContextp"); /* stack: {...}, valueContextp */
  valueContextp = (valueContext_t *) lua_touserdata(L, -1);
  lua_pop(L, 1);                             /* stack: {...} */

  /* Clear the stack */
  lua_settop(L, 0);

  valueb = marpaESLIFValue_valueb(valueContextp->marpaESLIFValuep);
  if (valueb < 0) {
    return luaL_error(L, "marpaESLIFValue_valueb failure, %s", strerror(errno));
  }

  /* MARPAESLIFLUA_METHOD(stackdumpv)(L, 0); */
  if (valueb > 0) {
    resultStacki = lua_gettop(L);
    /* marpaESLIFValue_valueb called the transformers that pushed the final value to the stack */
    MARPAESLIFLUA_CALLBACKV(L, valueContextp->value_r, "setResult", 1 /* nargs */, lua_pushnil(L); lua_copy(L, resultStacki, -1););
    lua_pop(L, 1);
    rci = 1;
  } else {
    rci = 0;
  }

  lua_pushboolean(L, rci);

  /* GENERICLOGGER_NOTICEF(NULL, "%s(L=%p) return 1 at %s:%d", funcs, L, FILENAMES, __LINE__); */
  return 1;
}

