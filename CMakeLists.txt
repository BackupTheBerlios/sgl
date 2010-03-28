CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (SimpleGL)

# standart
INCLUDE(Configure.cmake)

# vars
SET (TARGET_NAME 		SimpleGL)
SET (TARGET_UNIX_NAME 	sgl)

# global dir
INCLUDE_DIRECTORIES ( ${PROJECT_SOURCE_DIR}/include )
LINK_DIRECTORIES ( ${PROJECT_BINARY_DIR}/lib )

# sources
ADD_SUBDIRECTORY (src)

# examples 
ADD_SUBDIRECTORY (examples)

IF (BUILD_DOCUMENTATION)
    INCLUDE(Doxy)
ENDIF (BUILD_DOCUMENTATION)

# uninstall rule
INCLUDE(Uninstall OPTIONAL)

