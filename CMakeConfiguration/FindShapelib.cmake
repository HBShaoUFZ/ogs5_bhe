# - Try to find Shapelib
# Once done, this will define
#
#  Shapelib_FOUND
#  Shapelib_INCLUDE_DIRS
#  Shapelib_LIBRARIES

if (NOT Shapelib_FOUND)

	include(LibFindMacros)
	
	find_path( Shapelib_INCLUDE_DIR
		NAMES shapefil.h
		PATHS /usr/include/libshp /usr/include ${CMAKE_SOURCE_DIR}/../Libs/shapelib )

	if ( UNIX )
		find_library(Shapelib_LIBRARIES
			NAMES shp
			PATHS /usr/lib64 /usr/lib ${CMAKE_SOURCE_DIR}/../Libs/shapelib )
		find_library(Shapelib_LIBRARIES
			NAMES shapelib
			PATHS /usr/lib64 /usr/lib ${CMAKE_SOURCE_DIR}/../Libs/shapelib )
	else ( UNIX )			
		find_library(Shapelib_LIBRARIES
			NAMES shapelib
			PATHS ${CMAKE_SOURCE_DIR}/../Libs/shapelib )
	endif ( UNIX )


	# Set the include dir variables and the libraries and let libfind_process do the rest.
	# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
	if (NOT Shapelib_LIBRARIES STREQUAL "" AND NOT Shapelib_INCLUDE_DIR STREQUAL "")
		set(Shapelib_PROCESS_INCLUDES Shapelib_INCLUDE_DIR)
		set(Shapelib_PROCESS_LIBS Shapelib_LIBRARIES)
		libfind_process(Shapelib)
	else (NOT Shapelib_LIBRARIES STREQUAL "" AND NOT Shapelib_INCLUDE_DIR STREQUAL "")
		message (STATUS "Error: shapelib not found!")
	endif (NOT Shapelib_LIBRARIES STREQUAL "" AND NOT Shapelib_INCLUDE_DIR STREQUAL "")
	
endif (NOT Shapelib_FOUND)
