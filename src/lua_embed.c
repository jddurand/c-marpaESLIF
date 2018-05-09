#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>

#undef  FILENAMES
#define FILENAMES "lua_embed.c" /* For logging */

/* Panic stuff inspired from https://github.com/ceph/ceph/blob/master/src/cls/lua/cls_lua.cc */
static jmp_buf marpaESLIF_lua_panic_jump;

/* Exception macros inspired from http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html */
#define MARPAESLIF_LUA_TRY do { jmp_buf ex_buf__; switch( setjmp(ex_buf__) ) { case 0: while(1) {
#define MARPAESLIF_LUA_CATCH(x) break; case x:
#define MARPAESLIF_LUA_FINALLY break; } default: {
#define MARPAESLIF_LUA_ETRY break; } } }while(0)
#define MARPAESLIF_LUA_THROW(x) longjmp(ex_buf__, x)

/*****************************************************************************/
static int _marpaESLIF_lua_atpanic(lua_State *lua)
/*****************************************************************************/
{
  /* No log here, we do not want to print to stderr bindly - just recover */
  /* Tant pis for the lua stack */
  /* MARPAESLIF_ERRORF(marpaESLIFp, "Lua panic: %s", lua_tostring(lua, -1)); */
  longjmp(marpaESLIF_lua_panic_jump, 1);
  return 0;
}

/*****************************************************************************/
static lua_State *_marpaESLIF_lua_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  lua_State *L;

  L = luaL_newstate();
  if (L == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "luaL_newstate failure, %s", strerror(errno));
  }

  return L;
}

/*****************************************************************************/
static short _marpaESLIF_lua_compileb(marpaESLIF_t *marpaESLIFp, lua_State *L, const char *p, size_t l, char *names)
/*****************************************************************************/
{
  short rcb;

  lua_atpanic(L, &_marpaESLIF_lua_atpanic);
  if (setjmp(marpaESLIF_lua_panic_jump) == 0) {
    if (luaL_loadbuffer(L, p, l, names) != LUA_OK) {
      goto err;
    }
  } else {
    MARPAESLIF_ERROR(marpaESLIFp, "Lua panic");
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
