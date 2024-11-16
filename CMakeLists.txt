cmake_minimum_required(VERSION 3.28)

project(proj_coroutine LANGUAGES CXX VERSION 1.1.4)

set(coroutine_VERSION_MAJOR 1)
set(coroutine_VERSION_MINOR 3)
set(coroutine_VERSION_PATCH 0)

set(PROJ_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sources)
set(PROJ_SOURCES ${PROJ_SOURCE_DIR}/main.cpp)
set(PROJ_BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(PROJ_BINARY_DIR ${CMAKE_SOURCE_DIR}/binary)
set(PROJ_VERSION_H_CMAKE_IN ${PROJ_SOURCE_DIR}/VERSION.h_cmake_in)
set(PROJ_VERSION_H ${PROJ_BUILD_DIR}/VERSION.h)
set(PROJ_BUILD_DIR_CONFIG ${PROJ_BUILD_DIR}/$<CONFIG>)

option(PROJ_VERSION_PRINT "If selected, project version will be printed, otherwise nope" ON)

get_property(PROJ_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(PROJ_IS_MULTI_CONFIG)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

configure_file(${PROJ_VERSION_H_CMAKE_IN} ${PROJ_VERSION_H})

include_directories(${PROJ_BUILD_DIR})

add_executable(coroutine ${PROJ_SOURCES})

set_target_properties(coroutine PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
set_target_properties(coroutine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJ_BINARY_DIR})
set_target_properties(coroutine PROPERTIES OUTPUT_NAME "coroutine$<$<CONFIG:Debug>:_d>")

if(PROJ_VERSION_PRINT)
	message("Project version: ${coroutine_VERSION_MAJOR}.${coroutine_VERSION_MINOR}.${coroutine_VERSION_PATCH}")
endif()