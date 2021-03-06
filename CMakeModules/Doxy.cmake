# Optional variables:
# USER_DOX_VERSION
# USER_DOX_CREATE_SUBDIRS
# USER_DOX_EXCLUDE_PATTERN
# USER_DOX_FILE_PATTERNS
# USER_DOX_LANGUAGE
# USER_DOX_PREDEFINED
# USER_DOX_IMAGE_PATH
# USER_DOX_PROJECT_FOLDER

SET (BUILD_DOCUMENTATION ON)
INCLUDE(${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)
IF (DOXYGEN)
# Checking DOT
	IF(DOT)
		SET(HAVE_DOT YES)
	ELSE(DOT)
		SET(HAVE_DOT NO)
	ENDIF(DOT)

# Checking language
    IF (USER_DOX_LANGUAGE) 
        SET(DOX_LANGUAGE USER_DOX_LANGUAGE)
        SET(DOX_DOCS_INPUT USER_DOX_LANGUAGE)
    ELSE (USER_DOX_LANGUAGE)
		SET (DOX_LANGUAGE English)
		SET (DOX_DOCS_INPUT English)
    ENDIF (USER_DOX_LANGUAGE)   
	MESSAGE ("Documentation output language is ${DOX_LANGUAGE}")
    
# Checking OS
	IF (UNIX)
		SET (DOX_EXCLUDE_PATTERN Win.)
		SET (DOX_PREDEFINED linux)
	ELSE (UNIX)
		SET (DOX_EXCLUDE_PATTERN Lin.)
		SET (DOX_PREDEFINED WIN32)
	ENDIF (UNIX)
	SET (DOX_EXCLUDE_PATTERN ${DOX_EXCLUDE_PATTERN}*)
    
# Checking version
	IF (DEFINED USER_DOX_VERSION)
		SET (DOX_VERSION ${USER_DOX_VERSION})
	ELSE (DEFINED USER_DOX_VERSION)
		SET (DOX_VERSION  "Not recognized")
		EXECUTE_PROCESS (COMMAND svn info ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE SVN_INFO)
		IF (SVN_INFO)
			STRING (REGEX MATCH "(Revision|Ревизия): [0-9]+" SVN_REVISION ${SVN_INFO})
			IF (SVN_REVISION)
				STRING (REGEX MATCH "[0-9]+" DOX_VERSION ${SVN_REVISION})
			ENDIF (SVN_REVISION)
		ENDIF (SVN_INFO)
	ENDIF (DEFINED USER_DOX_VERSION)
    
# Setting some other variables, which user can change
	SET (DOX_FILE_PATTERNS "*.h *.cpp *.dox ${USER_DOX_FILE_PATTERNS}")
	SET (DOX_EXCLUDE_PATTERN "${DOX_EXCLUDE_PATTERN} */.svn*/ ${USER_DOX_EXCLUDE_PATTERN}")
	SET (DOX_PREDEFINED "${DOX_PREDEFINED} ${USER_DOX_PREDEFINED}")
	IF (DEFINED USER_DOX_CREATE_SUBDIRS)
		SET (DOX_CREATE_SUBDIRS ${USER_DOX_CREATE_SUBDIRS})
	ELSE (DEFINED USER_DOX_CREATE_SUBDIRS)
		SET (DOX_CREATE_SUBDIRS YES)
	ENDIF (DEFINED USER_DOX_CREATE_SUBDIRS)
	SET (DOX_IMAGE_PATH ${USER_DOX_IMAGE_PATH})
    
# Configuring Doxyfile
	SET (DOXYFILE_NAME ${PROJECT_SOURCE_DIR}/../CMakeModules/Doxyfile.cmake.in)
	IF(NOT EXISTS DOXYFILE_NAME)
		SET (DOXYFILE_NAME ${PROJECT_SOURCE_DIR}/CMakeModules/Doxyfile.cmake.in)
	ENDIF (NOT EXISTS DOXYFILE_NAME)
	
	CONFIGURE_FILE(${DOXYFILE_NAME}
		${PROJECT_BINARY_DIR}/Doxyfile
	)
	ADD_CUSTOM_TARGET(DoxygenDoc ${DOXYGEN_EXECUTABLE}
		${PROJECT_BINARY_DIR}/Doxyfile
	)
    IF (USER_DOX_PROJECT_FOLDER)
        SET_TARGET_PROPERTIES(DoxygenDoc PROPERTIES
            FOLDER ${USER_DOX_PROJECT_FOLDER}
        )
    ENDIF (USER_DOX_PROJECT_FOLDER)
    IF (WIN32)
        INSTALL (DIRECTORY ${PROJECT_BINARY_DIR}/docs/html DESTINATION docs)
    ELSE (WIN32)
        INSTALL (DIRECTORY ${PROJECT_BINARY_DIR}/docs/html DESTINATION share/doc/${PROJECT_NAME})
    ENDIF (WIN32)
ELSE (DOXYGEN)
	MESSAGE ("Doxygen is not found, documentation will NOT be built")
ENDIF (DOXYGEN)
