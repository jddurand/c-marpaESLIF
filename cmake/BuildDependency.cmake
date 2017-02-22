MACRO (BuildDependency arg_name arg_srcdir arg_workdir arg_cpworkdir)
  #
  # arg1: name
  # arg2: source dir
  # arg3: working dir
  # arg4: copy working dir
  #
  SET (GENERICLOGGER_BUILD_DIR "${3RDPARTY_OUTPUT_PATH}/genericLogger-remote")
  MESSAGE (STATUS "Copying ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${arg_srcdir}" "${arg_workdir}"
    WORKING_DIRECTORY "${arg_cpworkdir}"
    )
  MESSAGE (STATUS "Setting up ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . -DALL_IN_ONE=ON
    WORKING_DIRECTORY "${arg_workdir}"
    )
  MESSAGE (STATUS "Buildind ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${arg_workdir}"
    )
ENDMACRO ()
