project(QLinkbot)
cmake_minimum_required(VERSION 2.8.11)

##
## Utilities
##

##
## Package dependencies/configuration
##

add_subdirectory(../libbarobo libbarobo)
find_package(LIBBAROBO)

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

include_directories(${LIBBAROBO_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
set(SOURCES
    src/QLinkbot.cpp
   )

add_library(qlinkbot ${SOURCES} )

##
## Linker requirements
##

target_link_libraries(qlinkbot
    Qt5::Core
    ${LIBBAROBO_LIBRARIES}
)
