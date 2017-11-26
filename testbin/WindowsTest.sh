#!/bin/sh
#
# We are using wine, and we expect that the wine default
# have not changed, i.e. host's root is mapped to Z:
#
WINEPATH=`echo "${PATH}:/usr/lib/gcc/i686-w64-mingw32/6.3-win32" | sed -e 's/:/;Z:/g' -e 's#/#\\\\#g' -e 's/^/Z:/g'`
WINEPATH=${WINEPATH} wine $*
