MACRO (BuildDependency arg_name arg_srcdir arg_workdir arg_installprefix arg_cpworkdir)
  MESSAGE (STATUS "Copying ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${arg_srcdir}" "${arg_workdir}"
    WORKING_DIRECTORY "${arg_cpworkdir}"
    )
  MESSAGE (STATUS "Setting up ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${arg_installprefix} -DALL_IN_ONE=ON
    WORKING_DIRECTORY "${arg_workdir}"
    )
  MESSAGE (STATUS "Building and installing ${arg_name}")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_MAKE_PROGRAM}" install
    WORKING_DIRECTORY "${arg_workdir}"
    )
ENDMACRO ()
