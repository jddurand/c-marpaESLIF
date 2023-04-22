MACRO (MYPACKAGECONFIG config_in config_out)

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-CONFIG-DEBUG] Creating ${config_out} using ${config_in}")
  ENDIF ()
  CONFIGURE_FILE(${config_in} ${config_out})

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-CONFIG-DEBUG] Creating target ${PROJECT_NAME}_config")
  ENDIF ()
  ADD_CUSTOM_TARGET(${PROJECT_NAME}_config SOURCES ${config_in})

ENDMACRO()
