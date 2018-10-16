#ifndef MARPAESLIF_INTERNAL_LUA_H
#define MARPAESLIF_INTERNAL_LUA_H

#include <marpaESLIF.h>
#include <luaunpanic.h>

static char _MARPAESLIF_EMBEDDED_CONTEXT_LUA;

#define MARPAESLIF_EMBEDDED_CONTEXT_LUA &_MARPAESLIF_EMBEDDED_CONTEXT_LUA

static short _marpaESLIFGrammar_lua_precompileb(marpaESLIFGrammar_t *marpaESLIFGrammarp);
static void  _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep);
static short _marpaESLIFValue_lua_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short _marpaESLIFValue_lua_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static void  _marpaESLIF_lua_freeDefaultActionv(void *userDatavp, void *contextp, void *p, size_t sizel);

#endif /* MARPAESLIF_INTERNAL_LUA_H */

