find_package(Qt5Core)
set( CMAKE_AUTOMOC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON)

set( common_SRCS
	SystemMeasurement.cpp )

add_library(common ${tr_common_SRCS} )
target_link_libraries( common
	Qt5::Core
	)
set_property( TARGET common PROPERTY CXX_STANDARD 11)
set_property( TARGET common APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
