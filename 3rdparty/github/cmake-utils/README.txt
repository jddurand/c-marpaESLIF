# cmake-utils
CMake macros customized for /my/ packages

#
# All /my/ packages follow this convention:
# - a package Xx has a FindXx's cmake, that will set XX_FOUND, XX_INCLUDE_DIRS, XX_ROOT_DIR variables if success
#   In case of success XX_LIBRARIES is always XX_ROOT_DIR/lib
# - there is a target Xx that is the library
# - there is always an include directory
# - build always generate headers in output/include
# - Tests always overwrite PATH with value of variable TEST_PATH
# - Sourcedir of package Xx is always inside current package as 3rdparty/github/Xx
# - There all under github at https://github.com/jddurand/c-Xx.git
#

List of macros:

MACRO (MYPACKAGEDEPENDENCY dependTarget dependSourceDir dependIncludeDirsVarname)
