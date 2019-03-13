cmake_minimum_required (VERSION 3.0)

set (CXX_STANDARD "c++17")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CXX_STANDARD}")

project (xar VERSION 1.0.0)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("/usr/local/include")

find_package(OpenCV REQUIRED)
#####################################
add_executable(xar-test src/xar.cxx )
target_link_libraries(xar-test  ${OpenCV_LIBS})

add_library(xar SHARED src/xar.cxx )
target_link_libraries(xar  ${OpenCV_LIBS})
