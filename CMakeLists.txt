cmake_minimum_required (VERSION 2.6)
project (OGMP)

# Version
set(OGMP_MAJOR_VERSION 0)
set(OGMP_MINOR_VERSION 0)
set(OGMP_PATCH_VERSION 1)
set(OGMP_VERSION ${OGMP_MAJOR_VERSION}.${OGMP_MINOR_VERSION}.${OGMP_PATCH_VERSION})

# Dependencies
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS REQUIRED program_options system) 
include_directories(${Boost_INCLUDE_DIRS})

# Config
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${PROJECT_BINARY_DIR})

# Add source
add_subdirectory(src)

# Add misc
add_subdirectory(misc)
