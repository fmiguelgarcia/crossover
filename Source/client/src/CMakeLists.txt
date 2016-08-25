cmake_minimum_required(VERSION 3.0)
find_package(Qt5Core)
find_package(Qt5Network)
set( CMAKE_AUTOMOC ON)

set( client_SRCS agent.cpp client.cpp HttpSession.cpp)

add_executable( client ${client_SRCS})
target_link_libraries( client common Qt5::Core Qt5::Network)
target_include_directories( client PUBLIC ${CMAKE_SOURCE_DIR}/common/src)

set_property( TARGET client PROPERTY CXX_STANDARD 11)
set_property( TARGET client APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -std=c++11")

install( TARGETS client RUNTIME DESTINATION bin)
