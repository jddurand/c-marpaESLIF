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
/*   luaopen_marpaESLIFLua(L)... OR:                                              */
/*   luaL_requiref(L, "marpaESLIFLua", MARPAESLIFLUA_METHOD(installi), 1);        */
/*   lua_pop(1);                                                                  */
/*                                                                                */
/* - marpaESLIF, marpaESLIFGrammar, marpaESLIFRecognizer marpaESLIFValue          */
/*   all have explicit methods to inject unmanaged values (from L point of view): */
/*   ---------------------------------------------------------------------------- */
/*                                                                                */
/*   marpaESLIFLua_marpaESLIF_new_unmanagedi(L, marpaESLIFp);                     */
/*   marpaESLIFLua_marpaESLIFGrammar_new_unmanagedi(L, marpaESLIFGrammarp);       */
/*   marpaESLIFLua_marpaESLIFRecognizer_new_unmanagedi(L, marpaESLIFRecognizerp); */
/*   marpaESLIFLua_marpaESLIFValue_new_unmanagedi(L, marpaESLIFValuep);           */
/* ------------------------------------------------------------------------------ */
#endif /* MARPAESLIFLUA_EMBEDDED */

#endif /* MARPAESLIFLUA_H*/
