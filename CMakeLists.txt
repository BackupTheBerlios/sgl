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
IF (BUILD_EXAMPLES)
    IF (NOT SDL_FOUND)
        MESSAGE (FATAL_ERROR "Examples can't be built without SDL")
    ELSE (NOT SDL_FOUND)
        ADD_SUBDIRECTORY (examples)
    ENDIF (NOT SDL_FOUND)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_DOCUMENTATION)
    INCLUDE(Doxy)
ENDIF (BUILD_DOCUMENTATION)

# uninstall rule
INCLUDE(Uninstall OPTIONAL)

