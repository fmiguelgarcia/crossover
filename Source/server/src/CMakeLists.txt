cmake_minimum_required(VERSION 3.0)

find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Sql)
find_package(Qt5Xml)
find_package(CURL)
if (NOT CURL_FOUND)
  message( FATAL_ERROR "Package Curl is required, but not found!")
endif (NOT CURL_FOUND)

set( CMAKE_AUTOMOC ON)
set( CMAKE_AUTORCC ON)

set( server_SRCS
  server.cpp
  SimpleHttpServer.cpp
  ClientConfiguration.cpp
  DBBackEnd.cpp
  SMTPAgent.cpp
  server.qrc )

add_executable( server ${server_SRCS})
target_link_libraries( server
  common
  Qt5::Core
  Qt5::Network
  Qt5::Sql
  Qt5::Xml
  ${CURL_LIBRARIES})
target_include_directories( server
  PUBLIC ${CMAKE_SOURCE_DIR}/common/src
  ${CURL_INCLUDE_DIR})

set_property( TARGET server PROPERTY CXX_STANDARD 11)
set_property( TARGET server APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -std=c++11")

install( TARGETS server RUNTIME DESTINATION bin)
install( FILES ../resource/clientDefaultConf.xml DESTINATION bin)
