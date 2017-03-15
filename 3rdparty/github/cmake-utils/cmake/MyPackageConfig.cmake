MACRO (MYPACKAGECONFIG config_in config_out)

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-CONFIG-DEBUG] Creating ${config_out} using ${config_in}")
  ENDIF ()
  CONFIGURE_FILE(${config_in} ${config_out})

  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-CONFIG-DEBUG] Creating target ${PROJECT_NAME}Config")
  ENDIF ()
  ADD_CUSTOM_TARGET(${PROJECT_NAME}Config SOURCES ${config_in})
ENDMACRO()
