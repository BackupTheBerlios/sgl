CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (SimpleGL)
SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

IF (WIN32)
    # setup 3rd party directory as search path
    SET ( SIMPLE_GL_3RD_PARTY_FOUND FALSE )
    IF ( MSVC AND MSVC_VERSION EQUAL 1600 )
        SET ( CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/3rdParty/msvs_2010;${CMAKE_PREFIX_PATH}" )
        SET ( 3RD_PARTY_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdParty/msvs_2010 )
        SET ( SIMPLE_GL_3RD_PARTY_FOUND TRUE )
    ENDIF ( MSVC AND MSVC_VERSION EQUAL 1600 )
    
    IF (SIMPLE_GL_3RD_PARTY_FOUND)
        LIST ( APPEND CMAKE_INCLUDE_PATH 
            ${3RD_PARTY_DIRECTORY}/DevIL/include
            ${3RD_PARTY_DIRECTORY}/GLEW/include
        )

        LIST ( APPEND CMAKE_LIBRARY_PATH 
            ${3RD_PARTY_DIRECTORY}/DevIL/lib
            ${3RD_PARTY_DIRECTORY}/GLEW/lib
        )

        # copy dlls into target binary dir
        ADD_CUSTOM_TARGET (SimpleGLCopyDLL)

        MACRO (COPY_DLLS SOURCE DESTINATION COPY_FILES)
            FOREACH(FILENAME ${${COPY_FILES}})
                ADD_CUSTOM_COMMAND( TARGET CopyDLL
                                    COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE}/${FILENAME}" "${DESTINATION}/${FILENAME}")
            ENDFOREACH(FILENAME)
        ENDMACRO (COPY_DLLS)

        SET (DEVIL_DLLS
            DevIL.dll
            ILU.dll
            ILUT.dll
            jpeg.dll
            libpng12-0.dll
            libtiff-3.dll
            zlib1.dll
        )

        SET (GLEW_DLLS
            glew32.dll
            glew32mx.dll
        )

        SET (RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
        SET (3RD_PARTY_TARGET_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
        COPY_DLLS(${3RD_PARTY_DIRECTORY}/DevIL/lib  ${3RD_PARTY_TARGET_DIRECTORY}   DEVIL_DLLS)
        COPY_DLLS(${3RD_PARTY_DIRECTORY}/GLEW/bin   ${3RD_PARTY_TARGET_DIRECTORY}   GLEW_DLLS)
            
        SET (3RD_PARTY_TARGET CopyDLL)
    ENDIF (SIMPLE_GL_3RD_PARTY_FOUND)
ENDIF (WIN32)

ADD_SUBDIRECTORY(SimpleGL)
      