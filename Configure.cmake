# flags
SET(BUILD_SHARED_LIBS ON)

# standart
SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
INCLUDE (StandartIncludes)
INCLUDE (FindNecessaryLibraries.cmake)
INCLUDE (OptionDependentOnPackage)

# image library
OPTION_DEPENDENT_ON_ALL_PACKAGES (SIMPLE_GL_USE_SDL_IMAGE "Set to ON to build SimpleGL using SDL_image library." SDL_FOUND SDLIMAGE_FOUND)
MESSAGE ("Use SDL_image: " ${SIMPLE_GL_USE_SDL_IMAGE})

# image library #2
OPTION_DEPENDENT_ON_PACKAGE (SIMPLE_GL_USE_DEVIL "Set to ON to build SimpleGL using DevIL library." DEVIL_FOUND)
MESSAGE ("Use DevIL: " ${SIMPLE_GL_USE_DEVIL})

# examples
OPTION_DEPENDENT_ON_PACKAGE (BUILD_EXAMPLES "Set to ON to build examples" SDL_FOUND)
MESSAGE ("Build examples: " ${BUILD_EXAMPLES})

OPTION (INSTALL_EXAMPLES "Set to ON to install examples" NO)
MESSAGE ("Build examples: " ${BUILD_EXAMPLES})

# check settings
IF (NOT SDL_FOUND AND BUILD_EXAMPLES)
	MESSAGE (FATAL_ERROR "Can't build examples without SDL.") 
ENDIF (NOT SDL_FOUND AND BUILD_EXAMPLES)

IF (NOT BUILD_EXAMPLES AND INSTALL_EXAMPLES)
	MESSAGE (FATAL_ERROR "You must check in BUILD_EXAMPLES option in order to install them.") 
ENDIF (NOT BUILD_EXAMPLES AND INSTALL_EXAMPLES)

# documentation
OPTION_DEPENDENT_ON_PACKAGE (BUILD_DOCUMENTATION "Set to ON to build doxygen reference documentation" DOXYGEN_FOUND)
MESSAGE ("Build documentation: " ${BUILD_DOCUMENTATION})

# sse optimization
OPTION (SIMPLE_GL_USE_SSE "Set to ON to enable sse optimizations for math lib" ON)
MESSAGE ("Use SSE: " ${SIMPLE_GL_USE_SSE})

# this option moves creates sgl::math namespace, otherwise it will be simply math namespace
OPTION (SIMPLEGL_MATH_IN_SGL_NAMESPACE "Set to ON to move math namespace to the sgl namespace" OFF)
MESSAGE ("math in sgl namespace: " ${SIMPLE_GL_USE_SSE})

# check settings
IF (BUILD_EXAMPLES AND NOT SDL_FOUND)
	MESSAGE (FATAL_ERROR "Examples can't be built without SDL library.") 
ENDIF (BUILD_EXAMPLES AND NOT SDL_FOUND)

# Prefer DevIL
IF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)
	SET (SIMPLE_GL_USE_SDL_IMAGE OFF)
ENDIF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)

# check settings
IF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)
	MESSAGE (FATAL_ERROR "SimpleGL can use only one images library. Switch one of the SIMPLE_GL_USE_DEVIL, SIMPLE_GL_USE_SDL_IMAGE to ON, other to OFF.") 
ENDIF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)

# check settings
IF (BUILD_EXAMPLES AND NOT SIMPLE_GL_USE_SDL_IMAGE AND NOT SIMPLE_GL_USE_DEVIL)
	MESSAGE (FATAL_ERROR "Examples can't be built without images library. Check SIMPLE_GL_USE_DEVIL or SIMPLE_GL_USE_SDL_IMAGE option is ON") 
ENDIF (BUILD_EXAMPLES AND NOT SIMPLE_GL_USE_SDL_IMAGE AND NOT SIMPLE_GL_USE_DEVIL)

# check for sse
IF (SIMPLE_GL_USE_SSE)
    SET (SSE_VERSION 3) 

    # determine sse version
    IF (${SSE_VERSION} GREATER 3)
        SET (SIMPLE_GL_USE_SSE4 ON)
    ENDIF (${SSE_VERSION} GREATER 3)
    IF (${SSE_VERSION} GREATER 2)
        SET (SIMPLE_GL_USE_SSE3 ON)
    ENDIF (${SSE_VERSION} GREATER 2)
    IF (${SSE_VERSION} GREATER 1)
        SET (SIMPLE_GL_USE_SSE2 ON)
    ENDIF (${SSE_VERSION} GREATER 1)

    MESSAGE ("Used sse version: " ${SSE_VERSION})

ENDIF (SIMPLE_GL_USE_SSE)

# config
CONFIGURE_FILE (${PROJECT_SOURCE_DIR}/include/Config.h.cmake ${PROJECT_SOURCE_DIR}/include/Config.h)

# enable sse for g++
IF (CMAKE_COMPILER_IS_GNUCC AND SIMPLE_GL_USE_SSE)
	ADD_DEFINITIONS (-mmmx)
	ADD_DEFINITIONS (-msse)
	ADD_DEFINITIONS (-mfpmath=sse)

    IF (SIMPLE_GL_USE_SSE2)
	    ADD_DEFINITIONS (-msse2)
    ENDIF (SIMPLE_GL_USE_SSE2)

    IF (SIMPLE_GL_USE_SSE3)
	    ADD_DEFINITIONS (-msse3)
    ENDIF (SIMPLE_GL_USE_SSE3)

    IF (SIMPLE_GL_USE_SSE4)
	    ADD_DEFINITIONS (-msse4)
    ENDIF (SIMPLE_GL_USE_SSE4)
ENDIF (CMAKE_COMPILER_IS_GNUCC AND SIMPLE_GL_USE_SSE)

