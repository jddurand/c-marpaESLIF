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
/* ------------------------------------------------------------------------------------------------- */
/* When MARPAESLIFLUA_EMBEDDED is on, the source file marpaESLIFLua.c should be included as-is, no   */
/* symbol is exported. The file that includes marpaESLIFLua.c is expected to:                        */
/*                                                                                                   */
/* - Register marpaESLIFLua programmatically:                                                        */
/*   ----------------------------------------                                                        */
/*   luaopen_marpaESLIFLua(L);                                                                       */
/*   lua_pop(1);                                                                                     */
/*                                                                                                   */
/*   OR                                                                                              */
/*                                                                                                   */
/*   luaL_requiref(L, "marpaESLIFLua", marpaESLIFLua_installi, 1);                                   */
/*   lua_pop(1);                                                                                     */
/*                                                                                                   */
/* - Inject external contexts programmatically if required:                                          */
/*   ------------------------------------------------------                                          */
/* - marpaESLIF, marpaESLIFGrammar, marpaESLIFRecognizer marpaESLIFValue                             */
/*   all have explicit methods to inject unmanaged values:                                           */
/*                                                                                                   */
/*   marpaESLIFp           = marpaESLIF_newFromUnmanaged(lightuserdata)                              */
/*   marpaESLIFGrammarp    = marpaESLIFGrammar_newFromUnmanaged(lightuserdata)                       */
/*   marpaESLIFRecognizerp = marpaESLIFRecognizer_newFromUnmanaged(lightuserdata)                    */
/*   marpaESLIFValuep      = marpaESLIFValue_newFromUnmanaged(lightuserdata)                         */
/*                                                                                                   */
/* All those methods:                                                                                */
/* - requires one argument on the stack that is a non-null light userdata                            */
/* - push on lua stack the same lua object as their "new" counterparts but with NO reference thus    */
/*   no guaranteed lifetime.                                                                         */
/*                                                                                                   */
/* The caller is responsible to manage these objects lifetime buy storing these objects. The easiest */
/* way to do that is to store them as global variables.                                              */
/* ------------------------------------------------------------------------------------------------- */
#endif /* MARPAESLIFLUA_EMBEDDED */

#endif /* MARPAESLIFLUA_H*/
