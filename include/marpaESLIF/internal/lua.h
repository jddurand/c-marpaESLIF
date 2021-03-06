#ifndef MARPAESLIF_INTERNAL_LUA_H
#define MARPAESLIF_INTERNAL_LUA_H

#include <marpaESLIF.h>
#include <luaunpanic.h>

static char _MARPAESLIF_EMBEDDED_CONTEXT_LUA;

#define MARPAESLIF_EMBEDDED_CONTEXT_LUA &_MARPAESLIF_EMBEDDED_CONTEXT_LUA

static short _marpaESLIFGrammar_lua_precompileb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int popi);
static short _marpaESLIFValue_lua_precompileb(marpaESLIFValue_t *marpaESLIFValuep, char *luabytep, size_t luabytel, short stripb, int popi);
static void  _marpaESLIFRecognizer_lua_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static short _marpaESLIFRecognizer_lua_ifactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short _marpaESLIFRecognizer_lua_regexactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
static void  _marpaESLIFValue_lua_freev(marpaESLIFValue_t *marpaESLIFValuep);
static short _marpaESLIFValue_lua_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short _marpaESLIFValue_lua_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti);
static short _marpaESLIFValue_lua_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);
static short _marpaESLIFRecognizer_lua_eventactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static marpaESLIFGrammar_t *_marpaESLIF_luaGrammarp(marpaESLIF_t *marpaESLIFp);
static short _marpaESLIFValue_lua_action_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short _marpaESLIFValue_lua_symbol_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti);
static short _marpaESLIFRecognizer_lua_ifaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short _marpaESLIFRecognizer_lua_regexaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
static short _marpaESLIFRecognizer_lua_eventaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);

#endif /* MARPAESLIF_INTERNAL_LUA_H */

