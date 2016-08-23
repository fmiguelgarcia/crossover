cmake_minimum_required(VERSION 3.0)
find_package(Qt5Core)
find_package(Qt5Network)
set( CMAKE_AUTOMOC ON)

set( server_SRCS server.cpp )

add_executable( server ${server_SRCS})
target_link_libraries( server common Qt5::Core Qt5::Network)
target_include_directories( server PUBLIC ${CMAKE_SOURCE_DIR}/common/src)

set_property( TARGET server PROPERTY CXX_STANDARD 11)
set_property( TARGET server APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -std=c++11")

install( TARGETS server RUNTIME DESTINATION bin)
