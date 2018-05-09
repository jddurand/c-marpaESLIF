#include "marpaESLIF/internal/lua_embed.h"
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

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
static int _marpaESLIF_lua_atpanic(lua_State *L)
/*****************************************************************************/
{
  /* No log here, we do not want to print to stderr bindly - just recover */
  /* We store the lua stack if a (char *) fits into an int. Value (int)-1 */
  /* is not usable. */
  int jmpvalue = -1;

  if (sizeof(int) == sizeof(char *)) {
    char *p = NULL;
    char *stackp = (char *) lua_tostring(L, -1);
    if (stackp != NULL) {
      p = strdup(stackp);
      if (p != NULL) {
        jmpvalue = (int)p;
        /* Can it be ? */
        if (jmpvalue == -1) {
          free(p);
        }
      }
    }
  }

  longjmp(marpaESLIF_lua_panic_jump, jmpvalue);
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
  int jmpvalue;

  lua_atpanic(L, &_marpaESLIF_lua_atpanic);

  jmpvalue = setjmp(marpaESLIF_lua_panic_jump);
  if (jmpvalue == 0) {
    if (luaL_loadbuffer(L, p, l, names) != LUA_OK) {
      goto err;
    }
  } else {
    if (jmpvalue != -1) {
      char *p = (char *) jmpvalue;
      MARPAESLIF_ERRORF(marpaESLIFp, "Lua panic:\n%s", p);
      free(p);
    } else {
      MARPAESLIF_ERROR(marpaESLIFp, "Lua panic (no stack information available)");
    }
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
