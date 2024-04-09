include(CMakeLists.orig.txt)
#
# We append to PCRE's CMakeLists a rule that create object libraries.
#
foreach(target IN LISTS targets)
  foreach(bits 8 16 32)
    if((target STREQUAL "pcre2-${bits}-static") OR (target STREQUAL "pcre2-${bits}-shared"))
      set(objs_target objs_${target})
      message(STATUS "Creating ${objs_target} OBJECT LIBRARY out of ${target}")
      add_library(${objs_target} OBJECT EXCLUDE_FROM_ALL ${PCRE2_HEADERS} ${PCRE2_SOURCES} ${PROJECT_BINARY_DIR}/config.h)
      #
      # targe explicit commands
      #
      foreach(property
          COMPILE_DEFINITIONS COMPILE_FEATURES COMPILE_FLAGS COMPILE_OPTIONS COMPILE_WARNING_AS_ERROR
          INCLUDE_DIRECTORIES
          LINK_DIRECTORIES LINK_LIBRARIES LINK_OPTIONS)
        get_target_property(target_property ${target} ${property})
        if(target_property)
          message(STATUS "cmake_language(CALL TARGET_${property} ${objs_target} PUBLIC ${target_property})")
          cmake_language(CALL TARGET_${property} ${objs_target} PUBLIC ${target_property})
        endif()
      endforeach()
      #
      # Other properties
      #
      foreach(property
          POSITION_INDEPENDENT_CODE)
        get_target_property(target_property ${target} ${property})
        if(target_property)
          message(STATUS "set_target_properties(${objs_target} PROPERTIES ${property} ${target_property})")
          set_target_properties(${objs_target} PROPERTIES ${property} ${target_property})
        endif()
      endforeach()
    endif()
  endforeach()
endforeach()
