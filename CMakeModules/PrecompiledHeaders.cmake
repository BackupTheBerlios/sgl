MACRO(ADD_PRECOMPILED_HEADER TARGET_NAME PRECOMPILED_HEADER PRECOMPILED_SOURCE)
	IF (MSVC)
        GET_FILENAME_COMPONENT (PRECOMPILED_NAME ${PRECOMPILED_HEADER} NAME)
		GET_FILENAME_COMPONENT (PRECOMPILED_BASE_NAME ${PRECOMPILED_HEADER} NAME_WE)
		SET (PRECOMPILED_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${PRECOMPILED_BASE_NAME}.pch")

		SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/Yu\"${PRECOMPILED_NAME}\"")
		SET_SOURCE_FILES_PROPERTIES(${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc\"${PRECOMPILED_NAME}\"")  
	ELSEIF (CMAKE_COMPILER_IS_GNUCXX)
	    IF (NOT CMAKE_BUILD_TYPE)
		    MESSAGE(FATAL_ERROR 
			    "This is the ADD_PRECOMPILED_HEADER macro. " 
			    "You must set CMAKE_BUILD_TYPE!"
		    )
	    ENDIF(NOT CMAKE_BUILD_TYPE)

        GET_FILENAME_COMPONENT(_name ${PRECOMPILED_HEADER} NAME)
	    GET_FILENAME_COMPONENT(_path ${PRECOMPILED_HEADER} PATH)
        SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
	    SET(_output "${_outdir}/${CMAKE_BUILD_TYPE}.c++")

        ADD_CUSTOM_COMMAND(
		    OUTPUT ${_outdir}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${_outdir}
	    )
	    #MAKE_DIRECTORY(${_outdir})
        
	    STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
        SET(_compile_FLAGS ${${_flags_var_name}})
        
        GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)

	    SET(_CMAKE_CURRENT_BINARY_DIR_included_before_path FALSE)
	    FOREACH(item ${_directory_flags})
            IF (NOT _CMAKE_CURRENT_BINARY_DIR_included_before_path)
		        IF(${item} STREQUAL ${_path})
			        MESSAGE(FATAL_ERROR 
				        "This is the ADD_PRECOMPILED_HEADER macro. "
				        "CMAKE_CURRENT_BINARY_DIR has to mentioned at INCLUDE_DIRECTORIES's argument list before ${_path}, where ${_name} is located"
			        )	
		        ENDIF(${item} STREQUAL ${_path})

		        IF(${item} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
			        SET(_CMAKE_CURRENT_BINARY_DIR_included_before_path TRUE)
		        ENDIF(${item} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
	        ENDIF (NOT _CMAKE_CURRENT_BINARY_DIR_included_before_path)
		    LIST(APPEND _compile_FLAGS "-I${item}")
        ENDFOREACH(item)

	    GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
        LIST(APPEND _compile_FLAGS ${_directory_flags})
	    LIST(APPEND _compile_FLAGS ${CMAKE_CXX_FLAGS} )

        SEPARATE_ARGUMENTS(_compile_FLAGS)
        #MESSAGE("_compiler_FLAGS: ${_compiler_FLAGS}")
	    #message(${_compile_FLAGS} )
        #message("COMMAND ${CMAKE_CXX_COMPILER}	${_compile_FLAGS} -x c++-header -o ${_output} ${_input}")
	
	    ADD_CUSTOM_COMMAND(
		    OUTPUT	${CMAKE_CURRENT_BINARY_DIR}/${_name} 
		    COMMAND ${CMAKE_COMMAND} -E copy ${PRECOMPILED_HEADER} ${CMAKE_CURRENT_BINARY_DIR}/${_name} # ensure same directory! Required by gcc
	    )

        ADD_CUSTOM_COMMAND(
            OUTPUT ${_output} 	
		    COMMAND ${CMAKE_CXX_COMPILER}
				    ${_compile_FLAGS}
				    -x c++-header
				    -o ${_output} 
				    ${PRECOMPILED_HEADER}
            DEPENDS ${PRECOMPILED_HEADER} ${_outdir} ${CMAKE_CURRENT_BINARY_DIR}/${_name}	
	    )
       	ADD_CUSTOM_TARGET(${TARGET_NAME}_gch 
		    DEPENDS	${_output} 	
	    )
        ADD_DEPENDENCIES(${TARGET_NAME} ${TARGET_NAME}_gch )
        SET_TARGET_PROPERTIES(${TARGET_NAME} 
		    PROPERTIES	
			    COMPILE_FLAGS "-include ${_name} -Winvalid-pch"
        )
    ENDIF (MSVC)
ENDMACRO(ADD_PRECOMPILED_HEADER TARGET_NAME PRECOMPILED_HEADER PRECOMPILED_SOURCE)
