/*
** $Id: lundump.c,v 2.44 2015/11/02 16:09:30 roberto Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#define lundump_c
#define LUA_CORE

#include "lprefix.h"


#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lundump.h"
#include "lzio.h"


#if !defined(luai_verifycode)
#define luai_verifycode(L,b,f)  /* empty */
#endif


typedef struct {
  lua_State *L;
  ZIO *Z;
  const char *name;
} LoadState;


static l_noret error(LoadState *S, const char *why) {
  luaO_pushfstring(S->L, "%s: %s precompiled chunk", S->name, why);
  luaD_throw(S->L, LUA_ERRSYNTAX);
}


/*
** All high-level loads go through lundump_LoadVector; you can change it to
** adapt to the endianness of the input
*/
#define lundump_LoadVector(S,b,n)	lundump_LoadBlock(S,b,(n)*sizeof((b)[0]))

static void lundump_LoadBlock (LoadState *S, void *b, size_t size) {
  if (luaZ_read(S->Z, b, size) != 0)
    error(S, "truncated");
}


#define lundump_LoadVar(S,x)		lundump_LoadVector(S,&x,1)


static lu_byte lundump_LoadByte (LoadState *S) {
  lu_byte x;
  lundump_LoadVar(S, x);
  return x;
}


static int lundump_LoadInt (LoadState *S) {
  int x;
  lundump_LoadVar(S, x);
  return x;
}


static lua_Number lundump_LoadNumber (LoadState *S) {
  lua_Number x;
  lundump_LoadVar(S, x);
  return x;
}


static lua_Integer lundump_LoadInteger (LoadState *S) {
  lua_Integer x;
  lundump_LoadVar(S, x);
  return x;
}


static TString *lundump_LoadString (LoadState *S) {
  size_t size = lundump_LoadByte(S);
  if (size == 0xFF)
    lundump_LoadVar(S, size);
  if (size == 0)
    return NULL;
  else if (--size <= LUAI_MAXSHORTLEN) {  /* short string? */
    char buff[LUAI_MAXSHORTLEN];
    lundump_LoadVector(S, buff, size);
    return luaS_newlstr(S->L, buff, size);
  }
  else {  /* long string */
    TString *ts = luaS_createlngstrobj(S->L, size);
    lundump_LoadVector(S, getstr(ts), size);  /* load directly in final place */
    return ts;
  }
}


static void lundump_LoadCode (LoadState *S, Proto *f) {
  int n = lundump_LoadInt(S);
  f->code = luaM_newvector(S->L, n, Instruction);
  f->sizecode = n;
  lundump_LoadVector(S, f->code, n);
}


static void lundump_LoadFunction(LoadState *S, Proto *f, TString *psource);


static void lundump_LoadConstants (LoadState *S, Proto *f) {
  int i;
  int n = lundump_LoadInt(S);
  f->k = luaM_newvector(S->L, n, TValue);
  f->sizek = n;
  for (i = 0; i < n; i++)
    setnilvalue(&f->k[i]);
  for (i = 0; i < n; i++) {
    TValue *o = &f->k[i];
    int t = lundump_LoadByte(S);
    switch (t) {
    case LUA_TNIL:
      setnilvalue(o);
      break;
    case LUA_TBOOLEAN:
      setbvalue(o, lundump_LoadByte(S));
      break;
    case LUA_TNUMFLT:
      setfltvalue(o, lundump_LoadNumber(S));
      break;
    case LUA_TNUMINT:
      setivalue(o, lundump_LoadInteger(S));
      break;
    case LUA_TSHRSTR:
    case LUA_TLNGSTR:
      setsvalue2n(S->L, o, lundump_LoadString(S));
      break;
    default:
      lua_assert(0);
    }
  }
}


static void lundump_LoadProtos (LoadState *S, Proto *f) {
  int i;
  int n = lundump_LoadInt(S);
  f->p = luaM_newvector(S->L, n, Proto *);
  f->sizep = n;
  for (i = 0; i < n; i++)
    f->p[i] = NULL;
  for (i = 0; i < n; i++) {
    f->p[i] = luaF_newproto(S->L);
    lundump_LoadFunction(S, f->p[i], f->source);
  }
}


static void lundump_LoadUpvalues (LoadState *S, Proto *f) {
  int i, n;
  n = lundump_LoadInt(S);
  f->upvalues = luaM_newvector(S->L, n, Upvaldesc);
  f->sizeupvalues = n;
  for (i = 0; i < n; i++)
    f->upvalues[i].name = NULL;
  for (i = 0; i < n; i++) {
    f->upvalues[i].instack = lundump_LoadByte(S);
    f->upvalues[i].idx = lundump_LoadByte(S);
  }
}


static void lundump_LoadDebug (LoadState *S, Proto *f) {
  int i, n;
  n = lundump_LoadInt(S);
  f->lineinfo = luaM_newvector(S->L, n, int);
  f->sizelineinfo = n;
  lundump_LoadVector(S, f->lineinfo, n);
  n = lundump_LoadInt(S);
  f->locvars = luaM_newvector(S->L, n, LocVar);
  f->sizelocvars = n;
  for (i = 0; i < n; i++)
    f->locvars[i].varname = NULL;
  for (i = 0; i < n; i++) {
    f->locvars[i].varname = lundump_LoadString(S);
    f->locvars[i].startpc = lundump_LoadInt(S);
    f->locvars[i].endpc = lundump_LoadInt(S);
  }
  n = lundump_LoadInt(S);
  for (i = 0; i < n; i++)
    f->upvalues[i].name = lundump_LoadString(S);
}


static void lundump_LoadFunction (LoadState *S, Proto *f, TString *psource) {
  f->source = lundump_LoadString(S);
  if (f->source == NULL)  /* no source in dump? */
    f->source = psource;  /* reuse parent's source */
  f->linedefined = lundump_LoadInt(S);
  f->lastlinedefined = lundump_LoadInt(S);
  f->numparams = lundump_LoadByte(S);
  f->is_vararg = lundump_LoadByte(S);
  f->maxstacksize = lundump_LoadByte(S);
  lundump_LoadCode(S, f);
  lundump_LoadConstants(S, f);
  lundump_LoadUpvalues(S, f);
  lundump_LoadProtos(S, f);
  lundump_LoadDebug(S, f);
}


static void checkliteral (LoadState *S, const char *s, const char *msg) {
  char buff[sizeof(LUA_SIGNATURE) + sizeof(LUAC_DATA)]; /* larger than both */
  size_t len = strlen(s);
  lundump_LoadVector(S, buff, len);
  if (memcmp(s, buff, len) != 0)
    error(S, msg);
}


static void fchecksize (LoadState *S, size_t size, const char *tname) {
  if (lundump_LoadByte(S) != size)
    error(S, luaO_pushfstring(S->L, "%s size mismatch in", tname));
}


#define checksize(S,t)	fchecksize(S,sizeof(t),#t)

static void checkHeader (LoadState *S) {
  checkliteral(S, LUA_SIGNATURE + 1, "not a");  /* 1st char already checked */
  if (lundump_LoadByte(S) != LUAC_VERSION)
    error(S, "version mismatch in");
  if (lundump_LoadByte(S) != LUAC_FORMAT)
    error(S, "format mismatch in");
  checkliteral(S, LUAC_DATA, "corrupted");
  checksize(S, int);
  checksize(S, size_t);
  checksize(S, Instruction);
  checksize(S, lua_Integer);
  checksize(S, lua_Number);
  if (lundump_LoadInteger(S) != LUAC_INT)
    error(S, "endianness mismatch in");
  if (lundump_LoadNumber(S) != LUAC_NUM)
    error(S, "float format mismatch in");
}


/*
** load precompiled chunk
*/
LClosure *luaU_undump(lua_State *L, ZIO *Z, const char *name) {
  LoadState S;
  LClosure *cl;
  if (*name == '@' || *name == '=')
    S.name = name + 1;
  else if (*name == LUA_SIGNATURE[0])
    S.name = "binary string";
  else
    S.name = name;
  S.L = L;
  S.Z = Z;
  checkHeader(&S);
  cl = luaF_newLclosure(L, lundump_LoadByte(&S));
  setclLvalue(L, L->top, cl);
  luaD_inctop(L);
  cl->p = luaF_newproto(L);
  lundump_LoadFunction(&S, cl->p, NULL);
  lua_assert(cl->nupvalues == cl->p->sizeupvalues);
  luai_verifycode(L, buff, cl->p);
  return cl;
}

