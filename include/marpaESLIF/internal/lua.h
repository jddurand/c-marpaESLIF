#ifndef MARPAESLIF_INTERNAL_LUA_H
#define MARPAESLIF_INTERNAL_LUA_H

/* Pretend luaunpanic/export.h has already been included and force all lua to be static */
#undef luaunpanic_EXPORT_H
#define luaunpanic_EXPORT_H
#define luaunpanic_EXPORT static
#define LUAUNPANIC_NO_EXPORT static
#define LUAI_FUNC static
#define LUAI_DDEC static
#define LUAI_DDEF static

/* When linking against a lua that would not be compile with relocatable code, there will be */
/* an error, so we also make sure that all know exported symbols of the lua that we embed    */
/* have their own symbol.                                                                    */
#define luaL_checkversion_    marpaESLIFLuaEmbedded_luaL_checkversion_
#define luaL_getmetafield     marpaESLIFLuaEmbedded_luaL_getmetafield
#define luaL_callmeta         marpaESLIFLuaEmbedded_luaL_callmeta
#define luaL_tolstring        marpaESLIFLuaEmbedded_luaL_tolstring
#define luaL_argerror         marpaESLIFLuaEmbedded_luaL_argerror
#define luaL_checklstring     marpaESLIFLuaEmbedded_luaL_checklstring
#define luaL_optlstring       marpaESLIFLuaEmbedded_luaL_optlstring
#define luaL_checknumber      marpaESLIFLuaEmbedded_luaL_checknumber
#define luaL_optnumber        marpaESLIFLuaEmbedded_luaL_optnumber
#define luaL_checkinteger     marpaESLIFLuaEmbedded_luaL_checkinteger
#define luaL_optinteger       marpaESLIFLuaEmbedded_luaL_optinteger
#define luaL_checkstack       marpaESLIFLuaEmbedded_luaL_checkstack
#define luaL_checktype        marpaESLIFLuaEmbedded_luaL_checktype
#define luaL_checkany         marpaESLIFLuaEmbedded_luaL_checkany
#define luaL_newmetatable     marpaESLIFLuaEmbedded_luaL_newmetatable
#define luaL_setmetatable     marpaESLIFLuaEmbedded_luaL_setmetatable
#define luaL_testudata        marpaESLIFLuaEmbedded_luaL_testudata
#define luaL_checkudata       marpaESLIFLuaEmbedded_luaL_checkudata
#define luaL_where            marpaESLIFLuaEmbedded_luaL_where
#define luaL_error            marpaESLIFLuaEmbedded_luaL_error
#define luaL_checkoption      marpaESLIFLuaEmbedded_luaL_checkoption
#define luaL_fileresult       marpaESLIFLuaEmbedded_luaL_fileresult
#define luaL_execresult       marpaESLIFLuaEmbedded_luaL_execresult
#define luaL_ref              marpaESLIFLuaEmbedded_luaL_ref
#define luaL_unref            marpaESLIFLuaEmbedded_luaL_unref
#define luaL_loadfilex        marpaESLIFLuaEmbedded_luaL_loadfilex
#define luaL_loadbufferx      marpaESLIFLuaEmbedded_luaL_loadbufferx
#define luaL_loadstring       marpaESLIFLuaEmbedded_luaL_loadstring
#define luaL_newstate         marpaESLIFLuaEmbedded_luaL_newstate
#define luaL_len              marpaESLIFLuaEmbedded_luaL_len
#define luaL_gsub             marpaESLIFLuaEmbedded_luaL_gsub
#define luaL_setfuncs         marpaESLIFLuaEmbedded_luaL_setfuncs
#define luaL_getsubtable      marpaESLIFLuaEmbedded_luaL_getsubtable
#define luaL_traceback        marpaESLIFLuaEmbedded_luaL_traceback
#define luaL_requiref         marpaESLIFLuaEmbedded_luaL_requiref
#define luaL_buffinit         marpaESLIFLuaEmbedded_luaL_buffinit
#define luaL_prepbuffsize     marpaESLIFLuaEmbedded_luaL_prepbuffsize
#define luaL_addlstring       marpaESLIFLuaEmbedded_luaL_addlstring
#define luaL_addstring        marpaESLIFLuaEmbedded_luaL_addstring
#define luaL_addvalue         marpaESLIFLuaEmbedded_luaL_addvalue
#define luaL_pushresult       marpaESLIFLuaEmbedded_luaL_pushresult
#define luaL_pushresultsize   marpaESLIFLuaEmbedded_luaL_pushresultsize
#define luaL_buffinitsize     marpaESLIFLuaEmbedded_luaL_buffinitsize
#define luaL_pushmodule       marpaESLIFLuaEmbedded_luaL_pushmodule
#define luaL_openlib          marpaESLIFLuaEmbedded_luaL_openlib
#define lua_setuserdata       marpaESLIFLuaEmbedded_lua_setuserdata
#define lua_getuserdata       marpaESLIFLuaEmbedded_lua_getuserdata
#define lua_newstate          marpaESLIFLuaEmbedded_lua_newstate
#define lua_close             marpaESLIFLuaEmbedded_lua_close
#define lua_newthread         marpaESLIFLuaEmbedded_lua_newthread
#define lua_atpanic           marpaESLIFLuaEmbedded_lua_atpanic
#define lua_version           marpaESLIFLuaEmbedded_lua_version
#define lua_absindex          marpaESLIFLuaEmbedded_lua_absindex
#define lua_gettop            marpaESLIFLuaEmbedded_lua_gettop
#define lua_settop            marpaESLIFLuaEmbedded_lua_settop
#define lua_pushvalue         marpaESLIFLuaEmbedded_lua_pushvalue
#define lua_rotate            marpaESLIFLuaEmbedded_lua_rotate
#define lua_copy              marpaESLIFLuaEmbedded_lua_copy
#define lua_checkstack        marpaESLIFLuaEmbedded_lua_checkstack
#define lua_xmove             marpaESLIFLuaEmbedded_lua_xmove
#define lua_isnumber          marpaESLIFLuaEmbedded_lua_isnumber
#define lua_isstring          marpaESLIFLuaEmbedded_lua_isstring
#define lua_iscfunction       marpaESLIFLuaEmbedded_lua_iscfunction
#define lua_isinteger         marpaESLIFLuaEmbedded_lua_isinteger
#define lua_isuserdata        marpaESLIFLuaEmbedded_lua_isuserdata
#define lua_type              marpaESLIFLuaEmbedded_lua_type
#define lua_typename          marpaESLIFLuaEmbedded_lua_typename
#define lua_tonumberx         marpaESLIFLuaEmbedded_lua_tonumberx
#define lua_tointegerx        marpaESLIFLuaEmbedded_lua_tointegerx
#define lua_toboolean         marpaESLIFLuaEmbedded_lua_toboolean
#define lua_tolstring         marpaESLIFLuaEmbedded_lua_tolstring
#define lua_rawlen            marpaESLIFLuaEmbedded_lua_rawlen
#define lua_tocfunction       marpaESLIFLuaEmbedded_lua_tocfunction
#define lua_touserdata        marpaESLIFLuaEmbedded_lua_touserdata
#define lua_tothread          marpaESLIFLuaEmbedded_lua_tothread
#define lua_topointer         marpaESLIFLuaEmbedded_lua_topointer
#define lua_arith             marpaESLIFLuaEmbedded_lua_arith
#define lua_rawequal          marpaESLIFLuaEmbedded_lua_rawequal
#define lua_compare           marpaESLIFLuaEmbedded_lua_compare
#define lua_pushnil           marpaESLIFLuaEmbedded_lua_pushnil
#define lua_pushnumber        marpaESLIFLuaEmbedded_lua_pushnumber
#define lua_pushinteger       marpaESLIFLuaEmbedded_lua_pushinteger
#define lua_pushlstring       marpaESLIFLuaEmbedded_lua_pushlstring
#define lua_pushstring        marpaESLIFLuaEmbedded_lua_pushstring
#define lua_pushvfstring      marpaESLIFLuaEmbedded_lua_pushvfstring
#define lua_pushfstring       marpaESLIFLuaEmbedded_lua_pushfstring
#define lua_pushcclosure      marpaESLIFLuaEmbedded_lua_pushcclosure
#define lua_pushboolean       marpaESLIFLuaEmbedded_lua_pushboolean
#define lua_pushlightuserdata marpaESLIFLuaEmbedded_lua_pushlightuserdata
#define lua_pushthread        marpaESLIFLuaEmbedded_lua_pushthread
#define lua_getglobal         marpaESLIFLuaEmbedded_lua_getglobal
#define lua_gettable          marpaESLIFLuaEmbedded_lua_gettable
#define lua_getfield          marpaESLIFLuaEmbedded_lua_getfield
#define lua_geti              marpaESLIFLuaEmbedded_lua_geti
#define lua_rawget            marpaESLIFLuaEmbedded_lua_rawget
#define lua_rawgeti           marpaESLIFLuaEmbedded_lua_rawgeti
#define lua_rawgetp           marpaESLIFLuaEmbedded_lua_rawgetp
#define lua_createtable       marpaESLIFLuaEmbedded_lua_createtable
#define lua_newuserdata       marpaESLIFLuaEmbedded_lua_newuserdata
#define lua_getmetatable      marpaESLIFLuaEmbedded_lua_getmetatable
#define lua_getuservalue      marpaESLIFLuaEmbedded_lua_getuservalue
#define lua_setglobal         marpaESLIFLuaEmbedded_lua_setglobal
#define lua_settable          marpaESLIFLuaEmbedded_lua_settable
#define lua_setfield          marpaESLIFLuaEmbedded_lua_setfield
#define lua_seti              marpaESLIFLuaEmbedded_lua_seti
#define lua_rawset            marpaESLIFLuaEmbedded_lua_rawset
#define lua_rawseti           marpaESLIFLuaEmbedded_lua_rawseti
#define lua_rawsetp           marpaESLIFLuaEmbedded_lua_rawsetp
#define lua_setmetatable      marpaESLIFLuaEmbedded_lua_setmetatable
#define lua_setuservalue      marpaESLIFLuaEmbedded_lua_setuservalue
#define lua_callk             marpaESLIFLuaEmbedded_lua_callk
#define lua_pcallk            marpaESLIFLuaEmbedded_lua_pcallk
#define lua_load              marpaESLIFLuaEmbedded_lua_load
#define lua_dump              marpaESLIFLuaEmbedded_lua_dump
#define lua_yieldk            marpaESLIFLuaEmbedded_lua_yieldk
#define lua_resume            marpaESLIFLuaEmbedded_lua_resume
#define lua_status            marpaESLIFLuaEmbedded_lua_status
#define lua_isyieldable       marpaESLIFLuaEmbedded_lua_isyieldable
#define lua_gc                marpaESLIFLuaEmbedded_lua_gc
#define lua_error             marpaESLIFLuaEmbedded_lua_error
#define lua_next              marpaESLIFLuaEmbedded_lua_next
#define lua_concat            marpaESLIFLuaEmbedded_lua_concat
#define lua_len               marpaESLIFLuaEmbedded_lua_len
#define lua_stringtonumber    marpaESLIFLuaEmbedded_lua_stringtonumber
#define lua_getallocf         marpaESLIFLuaEmbedded_lua_getallocf
#define lua_setallocf         marpaESLIFLuaEmbedded_lua_setallocf
#define lua_getstack          marpaESLIFLuaEmbedded_lua_getstack
#define lua_getinfo           marpaESLIFLuaEmbedded_lua_getinfo
#define lua_getlocal          marpaESLIFLuaEmbedded_lua_getlocal
#define lua_setlocal          marpaESLIFLuaEmbedded_lua_setlocal
#define lua_getupvalue        marpaESLIFLuaEmbedded_lua_getupvalue
#define lua_setupvalue        marpaESLIFLuaEmbedded_lua_setupvalue
#define lua_upvalueid         marpaESLIFLuaEmbedded_lua_upvalueid
#define lua_upvaluejoin       marpaESLIFLuaEmbedded_lua_upvaluejoin
#define lua_sethook           marpaESLIFLuaEmbedded_lua_sethook
#define lua_gethook           marpaESLIFLuaEmbedded_lua_gethook
#define lua_gethookmask       marpaESLIFLuaEmbedded_lua_gethookmask
#define lua_gethookcount      marpaESLIFLuaEmbedded_lua_gethookcount
#define luaopen_base          marpaESLIFLuaEmbedded_luaopen_base
#define luaopen_coroutine     marpaESLIFLuaEmbedded_luaopen_coroutine
#define luaopen_table         marpaESLIFLuaEmbedded_luaopen_table
#define luaopen_io            marpaESLIFLuaEmbedded_luaopen_io
#define luaopen_os            marpaESLIFLuaEmbedded_luaopen_os
#define luaopen_string        marpaESLIFLuaEmbedded_luaopen_string
#define luaopen_utf8          marpaESLIFLuaEmbedded_luaopen_utf8
#define luaopen_bit32         marpaESLIFLuaEmbedded_luaopen_bit32
#define luaopen_math          marpaESLIFLuaEmbedded_luaopen_math
#define luaopen_debug         marpaESLIFLuaEmbedded_luaopen_debug
#define luaopen_package       marpaESLIFLuaEmbedded_luaopen_package
#define luaL_openlibs         marpaESLIFLuaEmbedded_luaL_openlibs

#include "luaunpanic.h"

static char _MARPAESLIF_EMBEDDED_CONTEXT_LUA;

#define MARPAESLIF_EMBEDDED_CONTEXT_LUA &_MARPAESLIF_EMBEDDED_CONTEXT_LUA

static inline lua_State    *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp);
static inline void          _marpaESLIF_lua_freev(marpaESLIF_t *marpaESLIFp);
static inline void          _marpaESLIF_lua_grammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
static inline short         _marpaESLIF_lua_grammar_precompileb(marpaESLIFGrammar_t *marpaESLIFGrammarp);
static inline short         _marpaESLIF_lua_value_precompileb(marpaESLIFValue_t *marpaESLIFValuep, char *luabytep, size_t luabytel, short stripb, int popi);
static inline void          _marpaESLIF_lua_value_freev(marpaESLIFValue_t *marpaESLIFValuep);
static inline void          _marpaESLIF_lua_recognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static short                _marpaESLIF_lua_recognizer_ifactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short                _marpaESLIF_lua_recognizer_regexactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
static short                _marpaESLIF_lua_recognizer_generatoractionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
static short                _marpaESLIF_lua_value_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                _marpaESLIF_lua_value_symbolb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti);
static short                _marpaESLIF_lua_value_representationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);
static short                _marpaESLIF_lua_recognizer_eventactionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static inline marpaESLIFGrammar_t *_marpaESLIF_lua_grammarp(marpaESLIF_t *marpaESLIFp, char *starts);
static short                _marpaESLIF_lua_value_action_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                _marpaESLIF_lua_value_symbol_functionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, int resulti);
static short                _marpaESLIF_lua_recognizer_ifaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short                _marpaESLIF_lua_recognizer_regexaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
static short                _marpaESLIF_lua_recognizer_generatoraction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
static short                _marpaESLIF_lua_recognizer_eventaction_functionb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static inline short         _marpaESLIF_lua_recognizer_push_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp);
static inline short         _marpaESLIF_lua_recognizer_pop_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short         _marpaESLIF_lua_recognizer_get_contextp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp);
static inline short         _marpaESLIF_lua_recognizer_set_contextb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp);

#endif /* MARPAESLIF_INTERNAL_LUA_H */

