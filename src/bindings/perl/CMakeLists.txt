PROJECT (marpaESLIFPerl)
#
# Inspired from https://github.com/dataseries/Lintel/blob/master/src/perl/CMakeLists.txt
#
# ------------------------------------------
# XSUBPP
# ------------------------------------------
IF (NOT PERL_XSUBPPDIR)
  EXECUTE_PROCESS(
    COMMAND perl -e "for (@INC) { next unless -r qq{\$_/ExtUtils/xsubpp} && -r qq{\$_/ExtUtils/typemap}; print; exit(0); }; print q{NOT-FOUND};"
    OUTPUT_VARIABLE PERL_TMP)
  IF (EXISTS "${PERL_TMP}/ExtUtils/xsubpp")
    SET (PERL_XSUBPPDIR "${PERL_TMP}/ExtUtils" CACHE INTERNAL "directory containing xsubpp")
  ELSE (EXISTS "${PERL_TMP}/ExtUtils/xsubpp")
    MESSAGE ("Can NOT build binary Perl modules: ExtUtils/{xsubpp,typemap} not found in perl @INC dirs")
  ENDIF (EXISTS "${PERL_TMP}/ExtUtils/xsubpp")
ENDIF (NOT PERL_XSUBPPDIR)

IF (PERL_XSUBPPDIR)
  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ESLIFPerl.c
    COMMAND perl ${PERL_XSUBPPDIR}/xsubpp -typemap ${PERL_XSUBPPDIR}/typemap -typemap ${CMAKE_CURRENT_SOURCE_DIR}/typemap ${CMAKE_CURRENT_SOURCE_DIR}/ESLIF.xs > ESLIFPerl.c
    # COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/xsubpp-cleanup < ESLIFPerl_1.c > ESLIFPerl_2.c
    # COMMAND ${CMAKE_COMMAND} -E remove -f ESLIFPerl_1.c
    # COMMAND ${CMAKE_COMMAND} -E remove -f ESLIFPerl.c
    # COMMAND ${CMAKE_COMMAND} -E rename ESLIFPerl_2.c ESLIFPerl.c
    )

    EXECUTE_PROCESS(COMMAND perl -e "use Config; print \$Config{ccflags};"
                    OUTPUT_VARIABLE PERL_CCFLAGS ERROR_QUIET)
    
    SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/ESLIFPerl.c
      PROPERTIES GENERATED true
      COMPILE_FLAGS "-I\"${PERL_INCLUDE_PATH}\" ${PERL_EXTRA_C_FLAGS}")
    
    ADD_LIBRARY(marpaESLIFPerl SHARED ${CMAKE_CURRENT_BINARY_DIR}/ESLIFPerl.c)
    TARGET_LINK_LIBRARIES(marpaESLIFPerl marpaESLIF)
    
    INSTALL(TARGETS marpaESLIFPerl DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/perl5)
    INSTALL(FILES ESLIF.pm DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/perl5)
    
    ADD_TEST(lintel-perl env PERL5LIB=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_CURRENT_SOURCE_DIR}:$ENV{PERL5LIB} perl ${CMAKE_CURRENT_SOURCE_DIR}/lintel.pl)
ENDIF (PERL_XSUBPPDIR)
