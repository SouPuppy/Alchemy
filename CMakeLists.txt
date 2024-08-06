cmake_minimum_required(VERSION 3.20)

# Project name and version
project(Alchemy VERSION 1.0)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Build options
option( USE_SYSTEM_GLFW
"Use system glfw rather than the included glfw submodule if available" OFF )
option(BUILD_EXAMPLE "Build the demo program" ON)
option(BUILD_INSTALL "Build the install target" ON)
option(USE_FFAST_MATH "Use -ffast-math" OFF)

# Set C++ standard and stack size
set( CMAKE_CXX_STACK_SIZE "10000000" )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STACK_SIZE "10000000")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set directories
set( INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" )
set( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )
set( VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" )
set( GLFW_DIR "${VENDOR_DIR}/glfw" )
set( GLEW_DIR "${VENDOR_DIR}/glew" )

# Lib name
set( PROJ_LIB_NAME "alchemy" )

# Compiler settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-deprecated-declarations -O3 -funroll-loops -g")
if(USE_FFAST_MATH)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
endif()

# Git submodule auto update
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Running git submodule update --init --recursive")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
        message(STATUS "Submodule update done")
    endif()
endif()

if(NOT EXISTS "${GLFW_DIR}/CMakeLists.txt")
   message(FATAL_ERROR "A submodule as not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_definitions(-DGLEW_STATIC)

# Gather sources and headers
file(GLOB SOURCES ${SRC_DIR}/*.cpp)
file(GLOB PUBLIC_HEADERS ${INCLUDE_DIR}/alchemy/*.hpp)
file(GLOB PRIVATE_HEADERS ${INCLUDE_DIR}/alchemy/internal/*.hpp)

# Create common.hpp
set(COMMON_HPP_PATH "${CMAKE_BINARY_DIR}/include/alchemy/common.hpp")
configure_file(common.hpp.in "${COMMON_HPP_PATH}")
list(APPEND PUBLIC_HEADERS "${COMMON_HPP_PATH}")

set(HEADERS ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

set( VENDOR_SOURCES
    ${GLEW_DIR}/src/glew.c
)

# Source groups
source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

include_directories(
    ${GLEW_DIR}/include
)

set( DEPENDENCIES )

# OpenGL
find_package(OpenGL REQUIRED)
set( DEPENDENCIES ${DEPENDENCIES} OpenGL::GL )

# GLFW
set ( WILL_USE_SYSTEM_GLFW ${USE_SYSTEM_GLFW} )

find_package(PkgConfig)
if ( NOT PkgConfig_FOUND )
    set ( WILL_USE_SYSTEM_GLFW OFF )
else()
    pkg_check_modules(GLFW glfw3)
    if ( NOT GLFW_FOUND )
        set ( WILL_USE_SYSTEM_GLFW OFF )
    endif ()
endif ()

if ( NOT WILL_USE_SYSTEM_GLFW )
    message ( STATUS "Using included glfw3 (in 3rdparty/)" )
    # Build GLFW
    SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW example" FORCE)
    SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW tests" FORCE)
    SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW docs" FORCE)
    SET(GLFW_INSTALL ${BUILD_INSTALL} CACHE BOOL "GLFW install" FORCE)
    add_subdirectory(${GLFW_DIR})
    set( DEPENDENCIES ${DEPENDENCIES} glfw )
    include_directories( "${GLFW_DIR}/include" )
else()
    message ( STATUS "Using system glfw3" )
    set( DEPENDENCIES ${DEPENDENCIES} glfw3 )
endif()

# Library target
add_library(${PROJ_LIB_NAME} STATIC ${SOURCES} ${VENDOR_SOURCES})

target_include_directories(
    ${PROJ_LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include/alchemy>
)
target_link_libraries( ${PROJ_LIB_NAME} PUBLIC ${DEPENDENCIES})

# Installation
if(BUILD_INSTALL)
    if ( NOT USE_SYSTEM_GLFW )
    # Ugly hack
    export(TARGETS glfw
        NAMESPACE
        FILE "${CMAKE_CURRENT_BINARY_DIR}/external/glfwConfig.cmake")
    endif()
    set_target_properties(${PROJ_LIB_NAME} PROPERTIES
        PUBLIC_HEADER "${PUBLIC_HEADERS}"
    )
    install(TARGETS ${PROJ_LIB_NAME}
        EXPORT ${PROJ_LIB_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/alchemy
    )
    export(TARGETS ${PROJ_LIB_NAME}
        NAMESPACE alchemy::
        FILE "${CMAKE_BINARY_DIR}/${PROJ_LIB_NAME}Config.cmake"
    )
endif()

# Example/demo target
if(BUILD_EXAMPLE)
    add_executable(demo demo.cpp)
    target_link_libraries(demo PRIVATE ${PROJ_LIB_NAME})
    set_target_properties(demo PROPERTIES OUTPUT_NAME "demo")
endif()
