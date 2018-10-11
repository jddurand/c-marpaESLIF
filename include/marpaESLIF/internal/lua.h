#ifndef MARPAESLIF_INTERNAL_LUA_H
#define MARPAESLIF_INTERNAL_LUA_H

#include <marpaESLIF.h>
#include <luaunpanic.h>

typedef enum marpaESLIFValue_lua_context {
  MARPAESLIFVALUE_LUA_CONTEXT = -1000
} marpaESLIFValue_lua_context_t;

static void  _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep);
static void  _marpaESLIFGrammar_lua_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
static void  _marpaESLIFValue_lua_free_callbackv(void *userDatavp, int contexti, void *p, size_t sizel);
static short _marpaESLIFValue_lua_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short _marpaESLIFValue_lua_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void  _marpaESLIF_lua_freeDefaultActionv(void *userDatavp, int contexti, void *p, size_t sizel);
static short _marpaESLIFGrammar_lua_precompileb(marpaESLIFGrammar_t *marpaESLIFGrammarp);

#endif /* MARPAESLIF_INTERNAL_LUA_H */

