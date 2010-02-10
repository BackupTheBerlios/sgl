# vars
SET ( EXAMPLE_NAME			Fractal )
SET ( EXAMPLE_INSTALL_DIR	${EXAMPLE_INSTALL_DIR}/${EXAMPLE_NAME} )
SET ( EXAMPLE_SOURCE_DIR 	${EXAMPLE_DIR}/Fractal )

# global parameters
INCLUDE_DIRECTORIES(include)

# list here all example dirs
ADD_SUBDIRECTORY(src)

