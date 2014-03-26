project(QLINKBOT)
cmake_minimum_required(VERSION 2.8.11)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

##############################################################################
# Headers

set(PUBLIC_INCLUDE_DIR include)
install(DIRECTORY ${PUBLIC_INCLUDE_DIR} DESTINATION .)
include_directories(${PUBLIC_INCLUDE_DIR})

##
## Utilities
##

##############################################################################
# Setup

set(required_libbarobo_version 0.1.0)
add_subdirectory(../libbarobo libbarobo)
if(NOT LIBBAROBO_VERSION VERSION_EQUAL ${required_libbarobo_version})
  message(FATAL_ERROR "qlinkbot requires libbarobo version ${required_libbarobo_version}")
endif()

include_directories(${LIBBAROBO_INCLUDE_DIRS})

add_definitions(-DQBAROBO_LIBRARY)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(DEFINED ENV{QTDIR})
  set(CMAKE_PREFIX_PATH 
      ${CMAKE_PREFIX_PATH}
      $ENV{QTDIR}lib/cmake/Qt5
     )
endif()
find_package(Qt5Core)
find_package(Qt5Widgets)
##
## Package outputs
##

set(TARGET qlinkbot)
set(SOURCES
    src/QLinkbot.cpp
    include/QLinkbot.h
   )

option(BUILD_SHARED_LIBS "build shared libraries" ON)
add_library(${TARGET} ${SOURCES} )

set_target_properties(${TARGET} PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR})

install(TARGETS ${TARGET}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)


##
## Linker requirements
##

target_link_libraries(qlinkbot
    Qt5::Core
    ${LIBBAROBO_LIBRARIES}
)

##############################################################################
# Configuration for dependent projects

get_filename_component(${PROJECT_NAME}_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIR} ABSOLUTE CACHE)
set(${PROJECT_NAME}_LIBRARIES ${TARGET} CACHE INTERNAL "")
set(${PROJECT_NAME}_VERSION ${VERSION} CACHE INTERNAL "")
