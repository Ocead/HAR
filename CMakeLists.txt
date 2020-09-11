cmake_minimum_required(VERSION 3.13)
project("HAR"
        LANGUAGES C CXX
        VERSION 2.0)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
endif ()

set(CMAKE_CXX_PROTO_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_PROTO_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_PROTO_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

add_subdirectory(lib/har)

add_subdirectory(lib/hargui)

add_subdirectory(lib/harduino)

#region package

#TODO: Figure out CPack

#endregion

#region Examples

include_directories(include)

find_library(harduino
        NAMES harduino harduino
        HINTS "${CMAKE_BINARY_DIR}/harduino")
set(DUINO_LIBRARY_NAME harduino)

link_directories(${CMAKE_BINARY_DIR}/harduino)

#To allow Arduino sketches without explicit includes of <Arduino.h>
if (MSVC)
    add_definitions(/FI"include/Arduino.h")
else ()
    add_definitions(-include Arduino.h)
endif ()

add_executable(example_blink include/Arduino.h src/example/blink.c)

target_link_libraries(example_blink
        ${DUINO_LIBRARY_NAME})

#endregion

#region Documentation

find_package(Doxygen)

add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM)

#endregion
