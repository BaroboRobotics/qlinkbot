project(QLINKBOT)
cmake_minimum_required(VERSION 2.8.11)

set(target qlinkbot)
if(TARGET ${target})
  # FIXME make sure that the current target's version or configuration matches?
  # this feels rather dangerous. find_package would solve this conundrum :/
  message(STATUS "qlinkbot already configured")
  return()
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

option(BUILD_SHARED_LIBS "build shared libraries" ON)

# qlinkbot project has one library target

set(sources
  src/QLinkbot.cpp
  include/QLinkbot.h
)

add_library(${target} ${sources} )

# requermintes (aka requirements, at the end of the day)

add_subdirectory(../libbarobo libbarobo)

find_package(Qt5Core)
find_package(Qt5Widgets)




set(CMAKE_INCLUDE_CURRENT_DIR ON)


# properties

set_target_properties(${target} PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  COMPILE_FLAGS "-DQBAROBO_LIBRARY"
  COMPILE_FLAGS_DEBUG "-DDEBUG"
  AUTOMOC ON
)
if(APPLE)
  set_target_properties(${target} PROPERTIES
    INSTALL_NAME_DIR "@executable_path")
endif()
target_include_directories(${target} PUBLIC ${LIBBAROBO_INCLUDE_DIRS})

# Linker requirements

target_link_libraries(${target}
    Qt5::Core
    barobo
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
  #ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR})


#
# Configuration for dependent projects.
#

set(PUBLIC_INCLUDE_DIR include)
include_directories(${PUBLIC_INCLUDE_DIR})

get_filename_component(${PROJECT_NAME}_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIR} ABSOLUTE CACHE)
set(${PROJECT_NAME}_LIBRARIES ${target} CACHE INTERNAL "")
set(${PROJECT_NAME}_VERSION ${VERSION} CACHE INTERNAL "")
