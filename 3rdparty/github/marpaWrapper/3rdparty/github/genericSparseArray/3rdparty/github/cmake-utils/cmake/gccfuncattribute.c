#include <stdlib.h>

#ifdef C_GCC_FUNC_ATTRIBUTE_ALIAS
  int foo( void ) { return 0; }
  int bar( void ) __attribute__((alias("foo")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_ALIGNED
  int foo( void ) __attribute__((aligned(32)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_ALLOC_SIZE
  void *foo(int a) __attribute__((alloc_size(1)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_ALWAYS_INLINE
inline __attribute__((always_inline)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_ARTIFICIAL
inline __attribute__((artificial)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_COLD
  int foo( void ) __attribute__((cold));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_CONST
  int foo( void ) __attribute__((const));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_CONSTRUCTOR_PRIORITY
  int foo( void ) __attribute__((__constructor__(65535/2)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_CONSTRUCTOR
  int foo( void ) __attribute__((constructor));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_DEPRECATED
  int foo( void ) __attribute__((deprecated("")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_DESTRUCTOR
  int foo( void ) __attribute__((destructor));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_DLLEXPORT
__attribute__((dllexport)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_DLLIMPORT
  int foo( void ) __attribute__((dllimport));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_ERROR
  int foo( void ) __attribute__((error("")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_EXTERNALLY_VISIBLE
  int foo( void ) __attribute__((externally_visible));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_FALLTHROUGH
  int foo( void ) {switch (0) { case 1: __attribute__((fallthrough)); case 2: break ; }};
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_FLATTEN
  int foo( void ) __attribute__((flatten));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_FORMAT
  int foo(const char *p, ...) __attribute__((format(printf, 1, 2)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_GNU_FORMAT
  int foo(const char *p, ...) __attribute__((format(gnu_printf, 1, 2)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_FORMAT_ARG
char *foo(const char *p) __attribute__((format_arg(1)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_GNU_INLINE
inline __attribute__((gnu_inline)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_HOT
  int foo( void ) __attribute__((hot));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_IFUNC
  int my_foo( void ) { return 0; }
  static int (*resolve_foo(void))(void) { return my_foo; }
  int foo( void ) __attribute__((ifunc("resolve_foo")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_LEAF
__attribute__((leaf)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_MALLOC
  void *foo( void ) __attribute__((malloc));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_NOCLONE
  int foo( void ) __attribute__((noclone1));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_NOINLINE
__attribute__((noinline1)) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_NONNULL
  int foo(char *p) __attribute__((nonnull(1)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_NORETURN
  void foo( void ) __attribute__((noreturn));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_NOTHROW
  int foo( void ) __attribute__((nothrow1));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_OPTIMIZE
__attribute__((optimize(3))) int foo( void ) { return 0; }
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_PURE
  int foo( void ) __attribute__((pure));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_SENTINEL
  int foo(void *p, ...) __attribute__((sentinel));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_SENTINEL_POSITION
  int foo(void *p, ...) __attribute__((sentinel_position(1)));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_RETURNS_NONNULL
  void *foo( void ) __attribute__((returns_nonnull));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_UNUSED
  int foo( void ) __attribute__((unused));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_USED
  int foo( void ) __attribute__((used));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_VISIBILITY
  int foo_def( void ) __attribute__((visibility("default")));
  int foo_hid( void ) __attribute__((visibility("hidden")));
  int foo_int( void ) __attribute__((visibility("internal")));
  int foo_pro( void ) __attribute__((visibility("protected")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_WARNING
  int foo( void ) __attribute__((warning1("")));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_WARN_UNUSED_RESULT
  int foo( void ) __attribute__((warn_unused_result));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_WEAK
  int foo( void ) __attribute__((weak));
  int main() { exit(0); }
#endif

#ifdef C_GCC_FUNC_ATTRIBUTE_WEAKREF
  static int foo( void ) { return 0; }
  static int bar( void ) __attribute__((weakref("foo")));
  int main() { exit(0); }
#endif
