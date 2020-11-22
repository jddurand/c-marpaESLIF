MACRO (MYPACKAGECHECKCOMMONSIZES)
  INCLUDE (CheckIncludeFile)
  INCLUDE (CheckTypeSize)

  SET (CMAKE_EXTRA_INCLUDE_FILES_BACKUP ${CMAKE_EXTRA_INCLUDE_FILES}) # backup
  SET (CMAKE_EXTRA_INCLUDE_FILES) # reset
  CHECK_INCLUDE_FILE ("stdint.h"       HAVE_STDINT_H)
  IF (HAVE_STDINT_H)
    LIST (APPEND CMAKE_EXTRA_INCLUDE_FILES "stdint.h")
  ENDIF ()
  CHECK_INCLUDE_FILE ("inttypes.h"     HAVE_INTTYPES_H)
  IF (HAVE_INTTYPES_H)
    LIST (APPEND CMAKE_EXTRA_INCLUDE_FILES "inttypes.h")
  ENDIF ()
  CHECK_INCLUDE_FILE ("sys/inttypes.h" HAVE_SYS_INTTYPES_H)
  IF (HAVE_SYS_INTTYPES_H)
    LIST (APPEND CMAKE_EXTRA_INCLUDE_FILES "sys/inttypes.h")
  ENDIF ()
  CHECK_INCLUDE_FILE ("stddef.h" HAVE_STDDEF_H)
  IF (HAVE_STDDEF_H)
    LIST (APPEND CMAKE_EXTRA_INCLUDE_FILES "stddef.h")
  ENDIF ()

  CHECK_TYPE_SIZE("char" SIZEOF_CHAR)
  CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
  CHECK_TYPE_SIZE("int" SIZEOF_INT)
  CHECK_TYPE_SIZE("long" SIZEOF_LONG)
  CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
  CHECK_TYPE_SIZE("float" SIZEOF_FLOAT)
  CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
  CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)
  CHECK_TYPE_SIZE("unsigned char" SIZEOF_UNSIGNED_CHAR)
  CHECK_TYPE_SIZE("unsigned short" SIZEOF_UNSIGNED_SHORT)
  CHECK_TYPE_SIZE("unsigned int" SIZEOF_UNSIGNED_INT)
  CHECK_TYPE_SIZE("unsigned long" SIZEOF_UNSIGNED_LONG)
  CHECK_TYPE_SIZE("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
  CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
  CHECK_TYPE_SIZE("void *" SIZEOF_VOID_STAR)
  CHECK_TYPE_SIZE("ptrdiff_t" SIZEOF_PTRDIFF_T)
  #
  # Integer types
  #
  FOREACH (_sign "" "u")
    #
    # Remember that CHAR_BIT minimum value is 8 -;
    #
    FOREACH (_size 8 16 32 64)
      MATH(EXPR _sizeof "${_size} / ${C_CHAR_BIT}")

      #
      # Speciying a MIN for unsigned case is meaningless (it is always zero) and not in the standard.
      # We neverthless set it, well, to zero.
      #
      SET (_mytypemin    MYPACKAGE_${_sign}int${_size}_MIN)
      STRING (TOUPPER ${_mytypemin} _MYTYPEMIN)

      SET (_mytypemax    MYPACKAGE_${_sign}int${_size}_MAX)
      STRING (TOUPPER ${_mytypemax} _MYTYPEMAX)
      #
      # Always define the MYPACKAGE_XXX_MIN and MYPACKAGE_XXX_MAX
      #
      FOREACH (_c "char" "short" "int" "long" "long long")
        #
        # Without an extension, integer literal is always int,
        # so we have to handle the case of "long" and "long long"
        #
        IF (_c STREQUAL "char")
          SET (_extension "")
        ELSEIF (_c STREQUAL "short")
          SET (_extension "")
        ELSEIF (_c STREQUAL "int")
          SET (_extension "")
        ELSEIF (_c STREQUAL "long")
          IF (_sign STREQUAL "")
            SET (_extension "L")
          ELSEIF (_sign STREQUAL "u")
            SET (_extension "UL")
          ELSE ()
            MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
          ENDIF ()
        ELSEIF (_c STREQUAL "long long")
          #
          # By definition, this C supports "long long", so it must support the "LL" suffix
          IF (_sign STREQUAL "")
            SET (_extension "LL")
          ELSEIF (_sign STREQUAL "u")
            SET (_extension "ULL")
          ELSE ()
            MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
          ENDIF ()
        ELSE ()
          MESSAGE(FATAL_ERROR "Unsupported c ${_c}")
        ENDIF ()
        STRING (TOUPPER ${_c} _C)
        STRING (REPLACE " " "_" _C "${_C}")
        IF (HAVE_SIZEOF_${_C})
          IF (${SIZEOF_${_C}} EQUAL ${_sizeof})
            #
            # In C language, a decimal constant without a u/U is always signed,
            # but an hexadecimal constant is signed or unsigned, depending on value and integer type range
            IF (_size EQUAL 8)
              IF (_sign STREQUAL "")
                SET (${_MYTYPEMIN} "(-127${_extension} - 1${_extension})")
                SET (${_MYTYPEMAX} "127${_extension}")
              ELSEIF (_sign STREQUAL "u")
                SET (${_MYTYPEMIN} "0x00${_extension}")
                SET (${_MYTYPEMAX} "0xFF${_extension}")
              ELSE ()
                MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
              ENDIF ()
            ELSEIF (_size EQUAL 16)
              IF (_sign STREQUAL "")
                SET (${_MYTYPEMIN} "(-32767${_extension} - 1${_extension})")
                SET (${_MYTYPEMAX} "32767${_extension}")
              ELSEIF (_sign STREQUAL "u")
                SET (${_MYTYPEMIN} "0x0000${_extension}")
                SET (${_MYTYPEMAX} "0xFFFF${_extension}")
              ELSE ()
                MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
              ENDIF ()
            ELSEIF (_size EQUAL 32)
              IF (_sign STREQUAL "")
                SET (${_MYTYPEMIN} "(-2147483647${_extension} - 1${_extension})")
                SET (${_MYTYPEMAX} "2147483647${_extension}")
              ELSEIF (_sign STREQUAL "u")
                SET (${_MYTYPEMIN} "0x00000000${_extension}")
                SET (${_MYTYPEMAX} "0xFFFFFFFF${_extension}")
              ELSE ()
                MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
              ENDIF ()
            ELSEIF (_size EQUAL 64)
              IF (_sign STREQUAL "")
                SET (${_MYTYPEMIN} "(-9223372036854775807${_extension} - 1${_extension})")
                SET (${_MYTYPEMAX} "9223372036854775807${_extension}")
              ELSEIF (_sign STREQUAL "u")
                SET (${_MYTYPEMIN} "0x0000000000000000${_extension}")
                SET (${_MYTYPEMAX} "0xFFFFFFFFFFFFFFFF${_extension}")
              ELSE ()
                MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
              ENDIF ()
            ELSE ()
              MESSAGE(FATAL_ERROR "Unsupported size ${_size}")
            ENDIF ()
            BREAK ()
          ENDIF ()
        ENDIF ()
      ENDFOREACH ()
      #
      # We handle the _least and _fast variations
      #
      FOREACH (_variation "" "_least" "_fast")

        SET (_ctype    ${_sign}int${_variation}${_size}_t)
        STRING (TOUPPER ${_ctype} _CTYPE)

        SET (_mytype    MYPACKAGE_${_sign}int${_variation}${_size})
        STRING (TOUPPER ${_mytype} _MYTYPE)

        SET (_MYTYPEDEF ${_MYTYPE}_TYPEDEF)

        SET (HAVE_${_MYTYPE} FALSE)
        SET (${_MYTYPE} "")
        SET (${_MYTYPEDEF} "")

        SET (_found_type FALSE)
        FOREACH (_underscore "" "_" "__")
          SET (_type ${_underscore}${_sign}int${_variation}${_size}_t)
          STRING (TOUPPER ${_type} _TYPE)
          CHECK_TYPE_SIZE (${_type} ${_TYPE})
          IF (HAVE_${_TYPE})
            SET (HAVE_${_MYTYPE} TRUE)
            SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
            SET (${_MYTYPEDEF} ${_type})
            IF (${_type} STREQUAL ${_ctype})
              SET (HAVE_${_CTYPE} TRUE)
            ELSE ()
              SET (HAVE_${_CTYPE} FALSE)
            ENDIF ()
            BREAK ()
          ENDIF ()
        ENDFOREACH ()
        IF (NOT HAVE_${_MYTYPE})
          #
          # Try with C types
          #
          FOREACH (_c "char" "short" "int" "long" "long long")
            IF ("${_sign}" STREQUAL "u")
              SET (_c "unsigned ${_c}")
            ENDIF ()
            STRING (TOUPPER ${_c} _C)
            STRING (REPLACE " " "_" _C "${_C}")
            IF (HAVE_SIZEOF_${_C})
              IF (_variation STREQUAL "")
                IF (${SIZEOF_${_C}} EQUAL ${_sizeof})
                  SET (HAVE_${_MYTYPE} TRUE)
                  SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
                  SET (${_MYTYPEDEF} ${_c})
                  BREAK ()
                ENDIF ()
              ELSEIF (_variation STREQUAL "_least")
                IF (NOT (${SIZEOF_${_C}} LESS ${_sizeof}))
                  SET (HAVE_${_MYTYPE} TRUE)
                  SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
                  SET (${_MYTYPEDEF} ${_c})
                  BREAK ()
                ENDIF ()
              ELSEIF (_variation STREQUAL "_fast")
                #
                # We give the same result as _least
                #
                IF (NOT (${SIZEOF_${_C}} LESS ${_sizeof}))
                  SET (HAVE_${_MYTYPE} TRUE)
                  SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
                  SET (${_MYTYPEDEF} ${_c})
                  BREAK ()
                ENDIF ()
              ELSE ()
                MESSAGE(FATAL_ERROR "Unsupported variation ${_variation}")
              ENDIF ()
            ENDIF ()
          ENDFOREACH ()
        ENDIF ()
        MARK_AS_ADVANCED (
          HAVE_${_MYTYPE}
          SIZEOF_${_MYTYPE}
          HAVE_${_CTYPE}
          ${_MYTYPEDEF}
          ${_MYTYPEMIN}
          ${_MYTYPEMAX})
      ENDFOREACH ()
    ENDFOREACH ()
  ENDFOREACH ()
  #
  # Integer type capable of holding object pointers
  #
  FOREACH (_sign "" "u")
    SET (_sizeof ${SIZEOF_VOID_STAR})
    SET (_ctype    ${_sign}intptr_t)
    STRING (TOUPPER ${_ctype} _CTYPE)
    SET (_mytype    MYPACKAGE_${_sign}intptr)
    STRING (TOUPPER ${_mytype} _MYTYPE)
    SET (_MYTYPEDEF ${_MYTYPE}_TYPEDEF)

    SET (HAVE_${_MYTYPE} FALSE)
    SET (${_MYTYPE} "")
    SET (${_MYTYPEDEF} "")

    SET (_type ${_sign}intptr_t)
    STRING (TOUPPER ${_type} _TYPE)
    CHECK_TYPE_SIZE (${_type} ${_TYPE})
    IF (HAVE_${_TYPE})
      SET (HAVE_${_MYTYPE} TRUE)
      SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
      SET (${_MYTYPEDEF} ${_type})
      IF (${_type} STREQUAL ${_ctype})
        SET (HAVE_${_CTYPE} TRUE)
      ELSE ()
        SET (HAVE_${_CTYPE} FALSE)
      ENDIF ()
    ENDIF ()
    IF (NOT HAVE_${_MYTYPE})
      #
      # Try with C types
      #
      FOREACH (_c "char" "short" "int" "long" "long long")
        IF ("${_sign}" STREQUAL "u")
          SET (_c "unsigned ${_c}")
        ENDIF ()
        STRING (TOUPPER ${_c} _C)
        STRING (REPLACE " " "_" _C "${_C}")
        IF (HAVE_SIZEOF_${_C})
          IF (${SIZEOF_${_C}} EQUAL ${_sizeof})
            SET (HAVE_${_MYTYPE} TRUE)
            SET (SIZEOF_${_MYTYPE} ${${_TYPE}})
            SET (${_MYTYPEDEF} ${_c})
            BREAK ()
          ENDIF ()
        ENDIF ()
        MARK_AS_ADVANCED (
          HAVE_${_MYTYPE}
          SIZEOF_${_MYTYPE}
          HAVE_${_CTYPE}
          ${_MYTYPEDEF})
      ENDFOREACH ()
    ENDIF ()
  ENDFOREACH ()
  
  SET (CMAKE_EXTRA_INCLUDE_FILES ${CMAKE_EXTRA_INCLUDE_FILES_BACKUP}) # restore
ENDMACRO()
