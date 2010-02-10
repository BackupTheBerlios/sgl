# try to find 3rdParty directory
Include(Find3rdParty)
IF (3RD_PARTY_FOUND)
    SET ( ${3RD_PARTY_PATHS}
        ${3RD_PARTY_DIRECTORY}/SDL
        ${3RD_PARTY_DIRECTORY}/GLEW
    )
ENDIF (3RD_PARTY_FOUND)

# packages
FIND_PACKAGE (OpenGL    REQUIRED)
FIND_PACKAGE (SDL)
FIND_PACKAGE (SDL_image)
FIND_PACKAGE (DevIL)

SET(GLEW_USE_STATIC_LIB OFF)
FIND_PACKAGE (Glew      REQUIRED)

# Documentation
FIND_PACKAGE (Doxygen)
