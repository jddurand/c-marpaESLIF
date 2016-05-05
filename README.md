[![Build Status](https://travis-ci.org/jddurand/c-tconv.svg?branch=master)](https://travis-ci.org/jddurand/c-tconv)

# NAME

tconv - iconv-like interface with automatic charset detection

# DESCRIPTION

tconv is a generic interface on charset detection and character conversion implementations. It is not necessary anymore to know in advance the charset: if you do NOT specify, tconv will guess.

Built-in charset detection engines are: cchardet, ICU.
Built-in character conversion engines are: iconv (even on Windows, via win-iconv), ICU.

# SEE ALSO

[iconv(3)](http://man.he.net/man3/iconv), [cchardet](https://pypi.python.org/pypi/cchardet/), [win-iconv](https://github.com/win-iconv/win-iconv), [ICU](http://icu-project.org/)
