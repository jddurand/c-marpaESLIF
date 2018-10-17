/* Copyright (C) 2009-2015 Francesco Nidito 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 */

#ifndef _TRY_THROW_CATCH_H_
#define _TRY_THROW_CATCH_H_

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

/* For the full documentation and explanation of the code below, please refer to
 * http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html
 */

#undef TRY
#if defined(LUA_USE_POSIX)
#  define TRY(LW) do { jmp_buf env; (LW)->envp = &env; switch( _setjmp(env) ) { case 0: while(1) {
#else
#  define TRY(LW) do { jmp_buf env; (LW)->envp = &env; switch( setjmp(env) ) { case 0: while(1) {
#endif

#undef CATCH
#define CATCH(LW, x) break; case x:

#undef FINALLY
#define FINALLY(LW) break; } default: {

#undef ETRY
#define ETRY(LW) break; } } if ((LW) != NULL) (LW)->envp = NULL; } while(0)

#undef THROW
#if defined(LUA_USE_POSIX)
#  define THROW(LW, x) if (((LW) != NULL) && ((LW)->envp != NULL)) _longjmp(*((LW)->envp), x)
#else
#  define THROW(LW, x) if (((LW) != NULL) && ((LW)->envp != NULL)) longjmp(*((LW)->envp), x)
#endif

#endif /*!_TRY_THROW_CATCH_H_*/
