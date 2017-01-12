cmake_minimum_required (VERSION 2.6)
project (OGMP)

# Version
set(OGMP_MAJOR_VERSION 0)
set(OGMP_MINOR_VERSION 0)
set(OGMP_PATCH_VERSION 3)
set(OGMP_VERSION ${OGMP_MAJOR_VERSION}.${OGMP_MINOR_VERSION}.${OGMP_PATCH_VERSION})

# Dependencies
set(Boost_USE_STATIC_LIBS OFF)
# set(Boost_USE_STATIC_LIBS ON) //Windows compile
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Boost location for compiling on Windows
# set(BOOST_ROOT "E:/boost/64/boost_1_63_0") #64bit compiled
# set(BOOST_ROOT "E:/boost/32/boost_1_63_0") #32bit compiled
find_package(Boost COMPONENTS REQUIRED program_options system)
include_directories(${Boost_INCLUDE_DIRS})

# Config
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_WIN32_WINNT=0x0501 -static -static-libgcc -static-libstdc++") #Windows flags
include_directories(${PROJECT_BINARY_DIR})

# Add source
add_subdirectory(src)

# Add misc
add_subdirectory(misc)
