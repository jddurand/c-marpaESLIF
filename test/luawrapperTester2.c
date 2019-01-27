/* Reference: http://lua-users.org/wiki/SimpleLuaApiExample */
/*
 * test.c
 * Example of a C program that interfaces with Lua.
 * Based on Lua 5.0 code by Pedro Martelletto in November, 2003.
 * Updated to Lua 5.1. David Manura, January 2007.
 */

#include <stdlib.h>
#include <stdio.h>
#include <luaunpanic.h>

#define PRINT_PANIC_STRING(L, f) do {					\
    char *panicstring;							\
    if (luaunpanic_panicstring(&panicstring, L)) {                      \
      fprintf(stderr, "%s panic\n", #f);				\
    } else {								\
      fprintf(stderr, "%s panic: %s\n", #f, panicstring);               \
    }									\
  } while (0)

#define PRINT_ERROR_STRING(L, f) do {					\
    char *errorstring = (char *) lua_tostring(L, -1);;			\
    if (errorstring == NULL) {                                          \
      fprintf(stderr, "%s error\n", #f);				\
    } else {								\
      fprintf(stderr, "%s error: %s\n", #f, errorstring);               \
    }									\
  } while (0)

int main(int argc, char *argv[])
{
  int status, result, i;
  double sum;
  lua_Number luaSum;
  lua_State *L;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s path/to/add.lua\n", argv[0]);
    exit(1);
  }

  /*
   * All Lua contexts are held in this structure. We work with it almost
   * all the time.
   */
  if (luaunpanicL_newstate(&L)) {
    fprintf(stderr, "luaunpanicL_newstate failure\n");
    exit(1);
  }
  if (L == NULL) {
    fprintf(stderr, "luaunpanicL_newstate success but L is NULL\n");
    exit(1);
  }

  /* load Lua base libraries */
  if (luaunpanicL_openlibs(L)) {
    PRINT_PANIC_STRING(L, luaunpanicL_openlibs);
    exit(1);
  }

  /* Load the file containing the script we are going to run */
  if (luaunpanicL_loadfile(&status, L, argv[1])) {
    PRINT_PANIC_STRING(L, luaunpanicL_loadfile);
    exit(1);
  }
  if (status) {
    PRINT_ERROR_STRING(L, luaunpanicL_loadfile);
    exit(1);
  }

  /*
   * Ok, now here we go: We pass data to the lua script on the stack.
   * That is, we first have to prepare Lua's virtual stack the way we
   * want the script to receive it, then ask Lua to run it.
   */
  if (luaunpanic_newtable(L)) {    /* We will pass a table */
    PRINT_PANIC_STRING(L, luaunpanic_newtable);
    exit(1);
  }

  /*
   * To put values into the table, we first push the index, then the
   * value, and then call lua_rawset() with the index of the table in the
   * stack. Let's see why it's -3: In Lua, the value -1 always refers to
   * the top of the stack. When you create the table with lua_newtable(),
   * the table gets pushed into the top of the stack. When you push the
   * index and then the cell value, the stack looks like:
   *
   * <- [stack bottom] -- table, index, value [top]
   *
   * So the -1 will refer to the cell value, thus -3 is used to refer to
   * the table itself. Note that lua_rawset() pops the two last elements
   * of the stack, so that after it has been called, the table is at the
   * top of the stack.
   */
  for (i = 1; i <= 5; i++) {
    if (luaunpanic_pushnumber(L, (lua_Number) i)) {   /* Push the table index */
      PRINT_PANIC_STRING(L, luaunpanic_pushnumber);
      exit(1);
    }
    if (luaunpanic_pushnumber(L, (lua_Number) i*2)) { /* Push the cell value */
      PRINT_PANIC_STRING(L, luaunpanic_pushnumber);
      exit(1);
    }
    if (luaunpanic_rawset(L, -3)) {      /* Stores the pair in the table */
      PRINT_PANIC_STRING(L, luaunpanic_rawset);
      exit(1);
    }
  }

  /* By what name is the script going to reference our table? */
  if (luaunpanic_setglobal(L, "foo")) {
    PRINT_PANIC_STRING(L, luaunpanic_setglobal);
    exit(1);
  }

  /* Ask Lua to run our little script */
  if (luaunpanic_pcall(&result, L, 0, LUA_MULTRET, 0)) {
    PRINT_PANIC_STRING(L, luaunpanic_pcall);
    exit(1);
  }
  if (result) {
    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
    exit(1);
  }

  /* Get the returned value at the top of the stack (index -1) */
  if (luaunpanic_tonumber(&luaSum, L, -1)) {
    PRINT_PANIC_STRING(L, luaunpanic_tonumber);
    exit(1);
  }
  sum = (double) luaSum;

  printf("Script returned: %.0f\n", sum);

  if (luaunpanic_pop(L, 1)) {  /* Take the returned value out of the stack */
    PRINT_PANIC_STRING(L, luaunpanic_pop);
    exit(1);
  }
  if (luaunpanic_close(L)) {   /* Cya, Lua */
    PRINT_PANIC_STRING(L, luaunpanic_close);
    exit(1);
  }

  exit(0);
}
