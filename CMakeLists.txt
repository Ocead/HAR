cmake_minimum_required(VERSION 3.13)
project("HAR"
        LANGUAGES C CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
endif ()

set(CMAKE_CXX_PROTO_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_PROTO_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_PROTO_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

include(cmake/RessourceCompiler.cmake)

add_subdirectory(lib/har)
add_subdirectory(lib/hargui)
add_subdirectory(lib/harduino)
add_subdirectory(lib/gti)

#region package

#TODO: Figure out CPack

#endregion

#region Examples

include_directories(include)

#region Blink

find_library(harduino
        NAMES harduino harduino
        HINTS "${CMAKE_BINARY_DIR}/harduino")
set(DUINO_LIBRARY_NAME harduino)

link_directories(${CMAKE_BINARY_DIR}/harduino)

#To allow Arduino sketches without explicit includes of <Arduino.h>
if (MSVC)
    set_source_files_properties(src/example/blink.c PROPERTIES COMPILE_FLAGS /FI"include/Arduino.h")
else ()
    set_source_files_properties(src/example/blink.c PROPERTIES COMPILE_FLAGS "-include Arduino.h")
endif ()

add_executable(example_blink include/Arduino.h src/example/blink.c)

target_link_libraries(example_blink
        ${DUINO_LIBRARY_NAME})

#endregion

#region Game of Life

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3 -fno-exceptions")

find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(GLIBMM REQUIRED glibmm-2.4)

include_directories(
        ${PROJECT_INCLUDE_DIR}
        include
        ../hargui/include
        ${GTKMM_INCLUDE_DIRS}
        ${GLIBMM_INCLUDE_DIRS})

link_directories(
        ${GTKMM_LIBRARY_DIRS}
        ${GLIBMM_LIBRARY_DIRS})

add_executable(example_gol src/example/game_of_life.cpp)

target_link_libraries(example_gol
        ${DUINO_LIBRARY_NAME}
        ${GTKMM_LIBRARIES}
        ${GLIBMM_LIBRARIES})

#endregion

#endregion

#region Documentation

find_package(Doxygen)

add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM)

#endregion
