project(QLINKBOT)
cmake_minimum_required(VERSION 2.8.12)

set(target qlinkbot)
if(TARGET ${target})
  # FIXME make sure that the current target's version or configuration matches?
  # this feels rather dangerous. find_package would solve this conundrum :/
  message(STATUS "qlinkbot already configured")
  return()
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

option(BUILD_SHARED_LIBS "build shared libraries" ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.54.0 REQUIRED COMPONENTS filesystem log thread)

if(NOT DEFINED DEPS_ROOT)
    if(DEFINED ENV{DEPS_ROOT})
        set(DEPS_ROOT "$ENV{DEPS_ROOT}")
    else()
        message(FATAL_ERROR "Please set DEPS_ROOT to the location of your dependencies (libsfp, ribbon-bridge, etc.).")
    endif()
endif()

function(add_dependency _name)
    add_subdirectory(${DEPS_ROOT}/${_name} ${_name})
endfunction()

# qlinkbot project has one library target

set(sources
  src/qlinkbot.cpp
  include/barobo/qlinkbot.hpp
)

add_library(${target} ${sources} )

# requermintes (aka requirements, at the end of the day)

add_dependency(cxx-util)
add_dependency(libsfp)
add_dependency(ribbon-bridge)
add_dependency(ribbon-bridge-interfaces)
add_dependency(baromesh)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)


# This line needed for the automoc-generated file.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# properties

set_target_properties(${target} PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  DEFINE_SYMBOL QLINKBOT_LIBRARY
  COMPILE_FLAGS_DEBUG "-DDEBUG"
  AUTOMOC ON
)

# qlinkbot and its consumers must define QLINKBOT_DYN_LINK if qlinkbot's type is
# SHARED_LIBRARY.
target_compile_definitions(${target}
  PUBLIC $<$<STREQUAL:$<TARGET_PROPERTY:${target},TYPE>,SHARED_LIBRARY>:QLINKBOT_DYN_LINK>
  )

if(APPLE)
  set_target_properties(${target} PROPERTIES
    INSTALL_NAME_DIR "@executable_path")
endif()
target_include_directories(${target} PUBLIC include)

# Linker requirements

target_link_libraries(${target}
    Qt5::Core
    Qt5::Network
    baromesh
    ${Boost_LIBRARIES}
)

#
# STAGE
#

if(WIN32)
  set(INSTALL_LIB_DIR .)
  set(INSTALL_BIN_DIR .)
elseif(APPLE)
  set(INSTALL_LIB_DIR Contents/MacOS)
  set(INSTALL_BIN_DIR Contents/MacOS)
else()
  set(INSTALL_LIB_DIR lib)
  set(INSTALL_BIN_DIR bin)
endif()

install(TARGETS ${target}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR})
