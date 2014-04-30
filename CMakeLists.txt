project(QLINKBOT)
cmake_minimum_required(VERSION 2.8.11)

set(TARGET qlinkbot)
if(TARGET ${TARGET})
  # FIXME make sure that the current target's version or configuration matches?
  # this feels rather dangerous. find_package would solve this conundrum :/
  message(STATUS "libbarobo already configured")
  return()
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

##############################################################################
# Headers

set(PUBLIC_INCLUDE_DIR include)
include_directories(${PUBLIC_INCLUDE_DIR})

##
## Utilities
##

##############################################################################
# Setup

add_subdirectory(../libbarobo libbarobo)

include_directories(${LIBBAROBO_INCLUDE_DIRS})

add_definitions(-DQBAROBO_LIBRARY)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
##
## Package outputs
##

set(SOURCES
    src/QLinkbot.cpp
    include/QLinkbot.h
   )

option(BUILD_SHARED_LIBS "build shared libraries" ON)
add_library(${TARGET} ${SOURCES} )

set_target_properties(${TARGET} PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR})


##
## Linker requirements
##

target_link_libraries(${TARGET}
    Qt5::Core
    ${LIBBAROBO_LIBRARIES}
)

##############################################################################
# Installation

if(UNIX)
  set(INSTALL_LIB_DIR lib)
  set(INSTALL_BIN_DIR bin)
else()
  set(INSTALL_LIB_DIR .)
  set(INSTALL_BIN_DIR .)
endif()

install(TARGETS ${TARGET}
  #ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR})

# TODO figure out what to do with the headers on install?
#install(DIRECTORY ${PUBLIC_INCLUDE_DIR} DESTINATION .)

##############################################################################
# Configuration for dependent projects

get_filename_component(${PROJECT_NAME}_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIR} ABSOLUTE CACHE)
set(${PROJECT_NAME}_LIBRARIES ${TARGET} CACHE INTERNAL "")
set(${PROJECT_NAME}_VERSION ${VERSION} CACHE INTERNAL "")
