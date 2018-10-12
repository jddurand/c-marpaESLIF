#ifndef MARPAESLIFLUA_H
#define MARPAESLIFLUA_H

#include "lua.h"         /* And not <lua.h> because it is not standardized as per CMake recommandation */

#ifndef MARPAESLIFLUA_EMBEDDED
#include <marpaESLIFLua/export.h>

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIFLua_EXPORT int luaopen_marpaESLIFLua(lua_State *L);
#ifdef __cplusplus
}
#endif
#else
/* ------------------------------------------------------------------------------ */
/* When MARPAESLIFLUA_EMBEDDED is on, the source file marpaESLIFLua.c should be   */
/* included as-is, no symbol is exported. The file that includes marpaESLIFLua.c  */
/* is expected to:                                                                */
/*                                                                                */
/* - Register marpaESLIFLua programmatically:                                     */
/*   ----------------------------------------                                     */
/*   luaopen_marpaESLIFLua(L);                                                    */
/*   lua_pop(1);                                                                  */
/*                                                                                */
/*   OR                                                                           */
/*                                                                                */
/*   luaL_requiref(L, "marpaESLIFLua", marpaESLIFLua_installi, 1);                */
/*   lua_pop(1);                                                                  */
/*                                                                                */
/* - marpaESLIF, marpaESLIFGrammar, marpaESLIFRecognizer marpaESLIFValue          */
/*   all have explicit methods to inject unmanaged values (from L point of view): */
/*   ---------------------------------------------------------------------------- */
/*                                                                                */
/*   _marpaESLIFLua_marpaESLIF_newi(L, marpaESLIFp);                              */
/*   _marpaESLIFLua_marpaESLIFGrammar_newi(L, marpaESLIFGrammarp);                */
/*   _marpaESLIFLua_marpaESLIFRecognizer_newi(L, marpaESLIFRecognizerp);          */
/*   _marpaESLIFLua_marpaESLIFValue_newi(L, marpaESLIFValuep);                    */
/*                                                                                */
/* All those methods behave EXACTLY like as if they were called by the Lua        */
/* interpreter:                                                                   */
/* - same expectations on the lua stack in input                                  */
/* - same output                                                                  */
/* - exception in case of error                                                   */
/* - the only difference is: lua will not call marpaESLIF to create the pointer   */
/*                                                                                */
/* If called with a NULL pointer, these methods behave like the version with no   */
/* underscore. They are unknown to Lua.                                           */
/* ------------------------------------------------------------------------------ */
#endif /* MARPAESLIFLUA_EMBEDDED */

#endif /* MARPAESLIFLUA_H*/
