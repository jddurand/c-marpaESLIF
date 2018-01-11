#
# This is the convrtrs.txt as of ICU SVN revision number 40756
#
# It is used to detect as best as possible the "encoding" family.
#
# Despite it comes from ICU it is NOT used by the ICU plugin, because
# ICU converters does not suffer from iconv bugs: on some platforms
# (in particular solaris) iconv is nothing else but trash, at best
# returning an error when converting to/from the same encoding, at worst
# consuming all the CPU.
#
# Therefore, in the iconv plugin, and in particular when iconv is
# in an external library, we try to detect the "family" of the source
# and destination encodings.
#
# When they are the same, we enforce an internal conversion via
# UTF-8, or UTF-32 if UTF-8 is the source/destination family.
#
# The script convrtrs.txt creates a header file that maps an encoder
# family to all known aliases.
#
