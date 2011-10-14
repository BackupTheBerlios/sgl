# flags
SET (BUILD_SHARED_LIBS ON)

# standart
INCLUDE (StandartIncludes)
INCLUDE (FindNecessaryLibraries.cmake)
INCLUDE (OptionDependentOnPackage)

IF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)
	# examples
	OPTION (BUILD_EXAMPLES "Set to ON to build examples" ON)
	MESSAGE ("Build examples: " ${BUILD_EXAMPLES})
	
	# documentation
	OPTION_DEPENDENT_ON_PACKAGE (SIMPLE_GL_BUILD_DOCUMENTATION "Set to ON to build doxygen reference documentation" DOXYGEN_FOUND)
	MESSAGE ("Build documentation: " ${BUILD_DOCUMENTATION})
ENDIF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)
	
IF (SIMPLE_GL_ANDROID)
	# mklink process doesn't work by default
	EXECUTE_PROCESS(COMMAND ${PROJECT_SOURCE_DIR}/SimpleGL/copy_data.bat ${PROJECT_SOURCE_DIR}/Android/Fractal/assets/data ${PROJECT_SOURCE_DIR}/bin/data RESULT_VARIABLE RES)
ELSE (SIMPLE_GL_ANDROID)
	# image library
	OPTION (SIMPLE_GL_USE_SDL_IMAGE "Set to ON to build SimpleGL using SDL_image library." OFF)
	MESSAGE ("Use SDL_image: " ${SIMPLE_GL_USE_SDL_IMAGE})

	# image library #2
	OPTION_DEPENDENT_ON_PACKAGE (SIMPLE_GL_USE_DEVIL "Set to ON to build SimpleGL using DevIL library." DEVIL_FOUND)
	MESSAGE ("Use DevIL: " ${SIMPLE_GL_USE_DEVIL})

	# check settings
	IF (NOT SDL_FOUND AND BUILD_EXAMPLES)
		MESSAGE (FATAL_ERROR "Can't build examples without SDL.") 
	ENDIF (NOT SDL_FOUND AND BUILD_EXAMPLES)

	IF (NOT BUILD_EXAMPLES AND INSTALL_EXAMPLES)
		MESSAGE (FATAL_ERROR "You must check in BUILD_EXAMPLES option in order to install them.") 
	ENDIF (NOT BUILD_EXAMPLES AND INSTALL_EXAMPLES)

	IF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)
		OPTION (BUILD_QT_EXAMPLES "Set to ON to build examples wich use QT" OFF)
		MESSAGE ("Build QT examples: " ${BUILD_QT_EXAMPLES})
	ENDIF (NOT SIMPLE_GL_CONFIGURE_INTRUSIVE)

	# check settings
	IF (BUILD_QT_EXAMPLES AND NOT QT_FOUND)
		MESSAGE (FATAL_ERROR "QT examples can't be built without QT library.") 
	ENDIF (BUILD_QT_EXAMPLES AND NOT QT_FOUND)

	OPTION (INSTALL_EXAMPLES "Set to ON to install examples" NO)
	MESSAGE ("Install examples: " ${INSTALL_EXAMPLES})

	# android port
	MESSAGE ("Android: " ${SIMPLE_GL_ANDROID})

	# sse optimization
	OPTION (SIMPLE_GL_USE_SSE "Set to ON to enable sse optimizations for math lib" ON)
	MESSAGE ("Use SSE: " ${SIMPLE_GL_USE_SSE})

	# this option moves creates sgl::math namespace, otherwise it will be simply math namespace
	OPTION (SIMPLEGL_MATH_IN_SGL_NAMESPACE "Set to ON to move math namespace to the sgl namespace" OFF)
	MESSAGE ("math in sgl namespace: " ${SIMPLE_GL_USE_SSE})

ENDIF (SIMPLE_GL_ANDROID)

# check settings
IF (BUILD_EXAMPLES AND NOT SDL_FOUND AND NOT SIMPLE_GL_ANDROID)
	MESSAGE (FATAL_ERROR "Examples can't be built without SDL library on PC.") 
ENDIF (BUILD_EXAMPLES AND NOT SDL_FOUND AND NOT SIMPLE_GL_ANDROID)
	
IF (INSTALL_EXAMPLES AND NOT BUILD_EXAMPLES)
	MESSAGE (FATAL_ERROR "Examples can't be installed if you don't build them.") 
ENDIF (INSTALL_EXAMPLES AND NOT BUILD_EXAMPLES)

IF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)
	MESSAGE (FATAL_ERROR "SimpleGL can use only one images library. Switch one of the SIMPLE_GL_USE_DEVIL, SIMPLE_GL_USE_SDL_IMAGE to ON, other to OFF.") 
ENDIF (SIMPLE_GL_USE_DEVIL AND SIMPLE_GL_USE_SDL_IMAGE)

IF (BUILD_EXAMPLES AND NOT SIMPLE_GL_USE_SDL_IMAGE AND NOT SIMPLE_GL_USE_DEVIL AND NOT SIMPLE_GL_ANDROID)
	MESSAGE (FATAL_ERROR "Examples can't be built without images library. Check SIMPLE_GL_USE_DEVIL or SIMPLE_GL_USE_SDL_IMAGE option is ON") 
ENDIF (BUILD_EXAMPLES AND NOT SIMPLE_GL_USE_SDL_IMAGE AND NOT SIMPLE_GL_USE_DEVIL AND NOT SIMPLE_GL_ANDROID)

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

# qt config
IF (QT_FOUND AND BUILD_QT_EXAMPLES)
	SET(QT_USE_QTOPENGL 1)
	INCLUDE(${QT_USE_FILE})
ENDIF (QT_FOUND AND BUILD_QT_EXAMPLES)

# config
CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/sgl/Config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/sgl/Config.h)

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

# global dir
IF (SIMPLE_GL_ANDROID)
	INCLUDE_DIRECTORIES (
		${CMAKE_CURRENT_SOURCE_DIR}/sgl
	)
ELSE (SIMPLE_GL_ANDROID)
	INCLUDE_DIRECTORIES (
		${CMAKE_CURRENT_SOURCE_DIR}/sgl
		${OPENGL_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
	)
ENDIF (SIMPLE_GL_ANDROID)
