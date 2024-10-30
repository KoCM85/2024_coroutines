cmake_minimum_required(VERSION 3.30)

project(proj_coroutine)

set(coroutine_VERSION_MAJOR 1)
set(coroutine_VERSION_MINOR 2)
set(coroutine_VERSION_PATCH 1)

set(PROJ_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sources)
set(PROJ_SOURCES ${PROJ_SOURCE_DIR}/main.cpp)
set(PROJ_BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(PROJ_BINARY_DIR ${CMAKE_SOURCE_DIR}/binary)

configure_file(${PROJ_SOURCE_DIR}/VERSION.h_cmake_in ${PROJ_BUILD_DIR}/VERSION.h)

include_directories(${PROJ_BUILD_DIR})

add_executable(coroutine ${PROJ_SOURCES})

set_target_properties(coroutine PROPERTIES CXX_STANDARD 23)

install(TARGETS coroutine DESTINATION ${PROJ_BINARY_DIR})

option(TESTT "HELLO FROM TEST" ON)

message("TEST = "${TESTT})