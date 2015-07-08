cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=c++98")
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")
file(DOWNLOAD
 "https://raw.githubusercontent.com/octoblu/cmake-external-project-utils/master/external-project-utils.cmake"
 "${CMAKE_MODULE_PATH}/external-project-utils.cmake"
)

include(external-project-utils)

project (tentacle-pseudopod)

file(GLOB SRCS *.cpp *.hpp *.c *.h)

add_library(tentacle-pseudopod STATIC ${SRCS})

getExternalProject("gtest" "https://googletest.googlecode.com/files/gtest-1.7.0.zip")
getExternalProject("gmock" "https://googlemock.googlecode.com/files/gmock-1.7.0.zip")
getExternalProject("arduino-mock" "https://github.com/octoblu/arduino-mock/archive/master.zip")
getExternalProject("arduino-nanopb" "https://github.com/octoblu/arduino-nanopb/archive/master.zip")
getExternalProject("tentacle" "https://github.com/octoblu/the-tentacle/archive/master.zip")


include_directories(tentacle-pseudopod
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

set(TENTACLE-PSEUDOPOD_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)

set_target_properties(tentacle-pseudopod PROPERTIES ENABLE_EXPORTS true)

linkExternalProject(tentacle-pseudopod "gtest")
linkExternalProject(tentacle-pseudopod "gmock")
linkExternalProject(tentacle-pseudopod "arduino-mock")
linkExternalProject(tentacle-pseudopod "arduino-nanopb")
linkExternalProject(tentacle-pseudopod "tentacle")

option(test "Build all tests." OFF)

if (test)
  enable_testing()
  add_subdirectory(test)
endif()
