cmake_minimum_required (VERSION 2.6)
PROJECT (OGMP)

OPTION(FORCE32 "Force a 32 bit built" OFF)
# Used to specify the directory for the compiled binaries of libboost on windows.
# set(BOOSTDIRECTORY "C:/boost/32/boost_1_63_0" CACHE STRING "Where to find boost")

# Version
SET(OGMP_MAJOR_VERSION0)
SET(OGMP_MINOR_VERSION 0)
SET(OGMP_PATCH_VERSION 5)
SET(OGMP_VERSION ${OGMP_MAJOR_VERSION}.${OGMP_MINOR_VERSION}.${OGMP_PATCH_VERSION})

# Dependencies
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

# Boost location
MESSAGE( STATUS "Using the boost location " ${BOOSTDIRECTORY} )

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(LINUX TRUE)
ENDIF()

SET(BOOST_ROOT ${BOOSTDIRECTORY})

IF(WIN32)
	IF( ${ARCHITECTURE} STREQUAL x86_64 AND (NOT FORCE32) )
		MESSAGE( STATUS "Compiling a 64 bit binary." )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static -static-libgcc -static-libstdc++")
		link_libraries(ws2_32 wsock32)
	ELSEIF( ${ARCHITECTURE} STREQUAL x86 OR FORCE32 )
		MESSAGE( STATUS "Compiling a 32 bit binary." )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D _WIN32_WINNT=0x0501 -static -static-libgcc -static-libstdc++")
		link_libraries(ws2_32 wsock32)
	ENDIF()
ELSEIF(LINUX)
	IF( ${ARCHITECTURE} STREQUAL x86_64 AND (NOT FORCE32) )
		MESSAGE( STATUS "Compiling a 64 bit binary." )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -m64 -static-libgcc -static-libstdc++")
	ELSEIF( ${ARCHITECTURE} STREQUAL x86 OR FORCE32 )
		MESSAGE( STATUS "Compiling a 32 bit binary." )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -m32 -static-libgcc -static-libstdc++")
	ELSEIF( ${ARCHITECTURE} STREQUAL arm )
		MESSAGE( STATUS "Compiling an ARM binary." )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++")
	ENDIF()
ELSEIF(APPLE)
	IF(${CMAKE_SYSTEM_NAME} STREQUAL Darwin)
		MESSAGE( STATUS "Compiling a binary." )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ENDIF()
ENDIF()

find_package(Boost COMPONENTS REQUIRED program_options system)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${PROJECT_BINARY_DIR})

# Add source
add_subdirectory(src)

# Add misc
add_subdirectory(misc)
