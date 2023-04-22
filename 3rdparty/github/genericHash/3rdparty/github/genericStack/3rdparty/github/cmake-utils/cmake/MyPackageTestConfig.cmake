MACRO (MYPACKAGETESTCONFIG config_in config_out)

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-TESTCONFIG-DEBUG] Creating ${config_out} using ${config_in}")
  ENDIF ()
  CONFIGURE_FILE(${config_in} ${config_out})

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-TESTCONFIG-DEBUG] Creating target ${PROJECT_NAME}_config_test")
  ENDIF ()
  ADD_CUSTOM_TARGET(${PROJECT_NAME}_config_test SOURCES ${config_in})
ENDMACRO()
