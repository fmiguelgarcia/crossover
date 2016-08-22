cmake_minimum_required(VERSION 2.8)
find_package(Qt5Core)
set( CMAKE_AUTOMOC ON)

set( client_SRCS agent.cpp client.cpp )

add_executable( client ${client_SRCS})
target_link_libraries( client
	common
	Qt5::Core)
set_property( TARGET client PROPERTY CXX_STANDARD 11)
set_property( TARGET client APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)

install( TARGETS client RUNTIME DESTINATION bin)
