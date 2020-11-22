MACRO (MYPACKAGECHECKCOMMONINCLUDEFILES)
  INCLUDE (CheckIncludeFile)
  CHECK_INCLUDE_FILE ("stdio.h"        HAVE_STDIO_H)
  CHECK_INCLUDE_FILE ("stddef.h"       HAVE_STDDEF_H)
  CHECK_INCLUDE_FILE ("stdlib.h"       HAVE_STDLIB_H)
  CHECK_INCLUDE_FILE ("stdarg.h"       HAVE_STDARG_H)
  CHECK_INCLUDE_FILE ("stdint.h"       HAVE_STDINT_H)
  CHECK_INCLUDE_FILE ("sys/stdint.h"   HAVE_SYS_STDINT_H)
  CHECK_INCLUDE_FILE ("inttypes.h"     HAVE_INTTYPES_H)
  CHECK_INCLUDE_FILE ("sys/inttypes.h" HAVE_SYS_INTTYPES_H)
  CHECK_INCLUDE_FILE ("sys/time.h"     HAVE_SYS_TIME_H)
  CHECK_INCLUDE_FILE ("sys/types.h"    HAVE_SYS_TYPES_H)
  CHECK_INCLUDE_FILE ("sys/stat.h"     HAVE_SYS_STAT_H)
  CHECK_INCLUDE_FILE ("errno.h"        HAVE_ERRNO_H)
  CHECK_INCLUDE_FILE ("string.h"       HAVE_STRING_H)
  CHECK_INCLUDE_FILE ("unistd.h"       HAVE_UNISTD_H)
  CHECK_INCLUDE_FILE ("io.h"           HAVE_IO_H)
  CHECK_INCLUDE_FILE ("time.h"         HAVE_TIME_H)
  CHECK_INCLUDE_FILE ("fcntl.h"        HAVE_FCNTL_H)
  CHECK_INCLUDE_FILE ("math.h"         HAVE_MATH_H)
  CHECK_INCLUDE_FILE ("float.h"        HAVE_FLOAT_H)
  CHECK_INCLUDE_FILE ("locale.h"       HAVE_LOCALE_H)
ENDMACRO()
