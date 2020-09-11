cmake_minimum_required(VERSION 3.10)
project("Headered Automaton Renderer"
        LANGUAGES CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3 -fno-exceptions")

#region Library

set(LIBRARY_NAME har)

include_directories(
        ${PROJECT_INCLUDE_DIR}
        include)

set(RESOURCE_COMPILER xxd)
file(GLOB HAR_RESOURCES "lib/har/include/res/*")
foreach (INPUT_FILE ${HAR_RESOURCES})
    if (NOT ${INPUT_FILE} MATCHES ".*.cpp")
        set(OUTPUT_FILE ${INPUT_FILE}.cpp)
        get_filename_component(RES_PATH ${INPUT_FILE} DIRECTORY)
        get_filename_component(INPUT_FILE_NAME ${INPUT_FILE} NAME)
        add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND ${RESOURCE_COMPILER} -i ${INPUT_FILE_NAME} ${OUTPUT_FILE}
                WORKING_DIRECTORY ${RES_PATH}
                COMMENT "Compiling ${INPUT_FILE_NAME} to binary"
                DEPENDS ${INPUT_FILE})
        list(APPEND HAR_RESOURCES ${OUTPUT_FILE})
    endif ()
endforeach ()

add_library(${LIBRARY_NAME}
        src/cargo_cell.cpp
        src/coords.cpp
        src/cell.cpp
        src/cell_base.cpp
        src/full_cell.cpp
        src/grid_cell.cpp
        src/part.cpp
        src/participant.cpp
        src/program.cpp
        src/property.cpp
        src/sketch_cell.cpp
        src/simulation.cpp
        src/value.cpp

        src/logic/automaton.cpp
        src/logic/context.cpp
        src/logic/guard.cpp
        src/logic/inner_participant.cpp
        src/logic/inner_simulation.cpp

        src/world/artifact.cpp
        src/world/cargo_cell_base.cpp
        src/world/grid.cpp
        src/world/grid_cell_base.cpp
        src/world/model.cpp
        src/world/world.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

#endregion

#region Catch2

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3")

set(TEST_NAME "${LIBRARY_NAME}_test")

include_directories(
        ${PROJECT_INCLUDE_DIR}
        test/include)

add_executable(${TEST_NAME}
        test/src/catch.cpp

        test/src/automaton.cpp
        test/src/cell.cpp
        test/src/cell_base.cpp
        test/src/parts.cpp
        test/src/simulation.cpp
        test/src/types.cpp
        test/src/value.cpp
        test/src/world.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${TEST_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${TEST_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${TEST_NAME}
        ${LIBRARY_NAME})

install(TARGETS ${LIBRARY_NAME}
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        CONFIGURATIONS Release
        OPTIONAL)

#endregion

#region Sketch

set(SKETCH_NAME "${LIBRARY_NAME}_sketch")

add_executable(${SKETCH_NAME} test/sketch.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${SKETCH_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${SKETCH_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${SKETCH_NAME}
        ${LIBRARY_NAME})

#endregion
