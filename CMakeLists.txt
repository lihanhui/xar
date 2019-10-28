cmake_minimum_required (VERSION 3.0)

set (CXX_STANDARD "c++17")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g  -std=${CXX_STANDARD}")

project (xar VERSION 1.0.0)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("/usr/local/include")

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	 MESSAGE(STATUS "current platform: Linux ")
	set(EXTRA_LIBS ${EXTRA_LIBS} pthread)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        MESSAGE(STATUS "current platform: FreeBSD")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "APPLE")
	MESSAGE(STATUS "current platform: APPLE")    
ELSE ()
        MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

find_package(OpenCV REQUIRED)
find_package(doraemon REQUIRED)
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)
#####################################
add_executable(xar-test src/xar.cxx
                        src/recognition/feature.cxx )
target_link_libraries(xar-test  ${OpenCV_LIBS})
target_link_libraries(xar-test  ${doraemon_LIBRARIES})
target_link_libraries(xar-test  ${EXTRA_LIBS})
target_link_libraries(xar-test  ${Boost_LIBRARIES})
