# shared includes
INCLUDE_DIRECTORIES ( ${SDL_INCLUDE_DIR} )

# vars
SET ( EXAMPLE_INSTALL_BINDIR    ${INSTALL_BINDIR}/examples )
SET ( EXAMPLE_DIR               ${PROJECT_SOURCE_DIR}/examples )

# list here all example dirs
ADD_SUBDIRECTORY( Fractal )
#ADD_SUBDIRECTORY( MarchingCubes )
#ADD_SUBDIRECTORY( RenderToVBO )
#ADD_SUBDIRECTORY( SkeletalAnimation)

# copy data on install
INSTALL ( DIRECTORY data
    DESTINATION     ${EXAMPLE_INSTALL_BINDIR}
    PATTERN         ".*"        EXCLUDE
)
