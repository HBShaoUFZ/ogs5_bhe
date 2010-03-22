message (STATUS "Setting the g++_gems configuration")

if (UNIX)
	add_definitions(
		-pedantic
		-fno-nonansi-builtins
		-DGCC
	)
	
	set (_POSIX_SOURCE ON)
	set (NO_ERROR_CONTROL ON)
	set (NEW_EQS ON)
endif (UNIX)

set (PROBLEM_CLASS ON)
set (RANDOM_WALK ON)
set (GEM_REACT ON)

add_definitions(
	-DIPMGEMPLUGIN
)

if (MSVC)
	SET(CMAKE_CXX_FLAGS_DEBUG "/MTd")
	SET(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif (MSVC)