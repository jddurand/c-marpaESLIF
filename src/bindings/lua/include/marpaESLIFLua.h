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
/* All those methods will return a value != 0 in case of failure. If the          */
/* argument is not NULL, then:                                                    */
/* - no argument on the lua stack is allowed                                      */
/* - pointer will be injected in lua interpreter as if lua created it             */
/* - Result is put on lua stack as if lua created it                              */
/* else:                                                                          */
/* - all those methods reacts like their "new" equivalents, e.g.:                 */
/*   _marpaESLIFLua_marpaESLIF_newi(L, NULL)                                      */
/*   is equivalent to                                                             */
/*   marpaESLIFLua_marpaESLIF_newi(L)                                             */
/*   expecting arguments on the lua stack exactly like when it is called by the   */
/*   interpreter.                                                                 */
/*                                                                                */
/* ------------------------------------------------------------------------------ */
#endif /* MARPAESLIFLUA_EMBEDDED */

#endif /* MARPAESLIFLUA_H*/
