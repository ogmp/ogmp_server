include_directories(${OGMP_SOURCE_DIR}/include)
link_directories(${OGMP_BINARY_DIR}/src)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(ogmp main.cpp server.cpp connection.cpp connection_manager.cpp
	reply.cpp request_handler.cpp request_parser.cpp mime_types.cpp client.cpp
	client_manager.cpp config.cpp)

target_link_libraries(ogmp pthread ${Boost_LIBRARIES})
