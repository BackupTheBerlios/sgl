
IF (WIN32)
	FIND_PATH( 3RD_PARTY_DIRECTORY 
		   3rdParty/README.txt
		   ${PROJECT_SOURCE_DIR} 
		   NO_CMAKE_SYSTEM_PATH
		   NO_SYSTEM_ENVIRONMENT_PATH )

	IF (3RD_PARTY_DIRECTORY)
		SET ( 3RD_PARTY_DIRECTORY "${3RD_PARTY_DIRECTORY}/3rdParty" )	
		SET ( 3RD_PARTY_FOUND TRUE )
	ENDIF (3RD_PARTY_DIRECTORY)
ELSE (WIN32)
	SET (3RD_PARTY_FOUND FALSE)
ENDIF (WIN32)

IF (3RD_PARTY_FOUND)
	SET ( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${3RD_PARTY_DIRECTORY}/include/ )
	SET ( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${3RD_PARTY_DIRECTORY}/lib/ )
	MESSAGE( STATUS "Found 3rdParty directory: ${3RD_PARTY_DIRECTORY}" )
ENDIF (3RD_PARTY_FOUND)
