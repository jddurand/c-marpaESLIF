#############################
# Global settings
#############################
CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (marpaESLIF C CXX)

INCLUDE (CMakeGlobalSetup.txt NO_POLICY_SCOPE)

INCLUDE (CMakeCompilerSetup.txt)
INCLUDE (src/CMakeLists.txt)
INCLUDE (test/CMakeLists.txt)
