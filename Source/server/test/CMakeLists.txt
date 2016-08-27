cmake_minimum_required(VERSION 3.0)

find_package(Qt5Core)
find_package(Qt5Test)
find_package(CURL)
if (NOT CURL_FOUND)
  message( FATAL_ERROR "Package Curl is required, but not found!")
endif (NOT CURL_FOUND)

set( CMAKE_AUTOMOC ON)
set( CMAKE_AUTORCC ON)


# SMTP Unit test
set( SMTPAgentUT_SRCS SMTPAgentUT.cpp
	${CMAKE_SOURCE_DIR}/server/src/SMTPAgent.cpp )

add_executable( SMTPAgentUT ${SMTPAgentUT_SRCS})
target_link_libraries( SMTPAgentUT Qt5::Core Qt5::Test ${CURL_LIBRARIES})
target_include_directories( SMTPAgentUT PUBLIC ${CMAKE_SOURCE_DIR}/server/src ${CURL_INCLUDE_DIR})
set_property( TARGET SMTPAgentUT PROPERTY CXX_STANDARD 11)
set_property( TARGET SMTPAgentUT APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -std=c++11")

enable_testing()
add_test( SMTPAgentUT_t SMTPAgentUT)
