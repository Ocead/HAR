cmake_minimum_required(VERSION 3.10)
project("HAR Arduino Extension"
        LANGUAGES C CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3 -fno-exceptions")

find_library(hargui
        NAMES hargui harguilib
        HINTS "${CMAKE_BINARY_DIR}/hargui")
set(GUI_LIBRARY_NAME hargui)

#region HARduino

compile_resource(DUINO_RESOURCES xxd "res/*")

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

set(DUINO_LIBRARY_NAME harduino)

add_library(${DUINO_LIBRARY_NAME}
        ${DUINO_RESOURCES}
        src/parts.cpp

        src/parts/empty.cpp

        src/parts/push_button.cpp
        src/parts/switch_button.cpp

        src/parts/lamp.cpp
        src/parts/rgb_led.cpp
        src/parts/seven_segment.cpp

        src/parts/proximity_sensor.cpp
        src/parts/color_sensor.cpp
        src/parts/movement_sensor.cpp

        src/parts/motor.cpp
        src/parts/conveyor_belt.cpp
        src/parts/thread_rod.cpp

        src/parts/producer.cpp
        src/parts/destructor.cpp

        src/parts/box_cargo.cpp

        src/parts/analog_pin.cpp
        src/parts/digital_pin.cpp
        src/parts/constant_pin.cpp
        src/parts/pwm_pin.cpp
        src/parts/serial_pin.cpp

        src/parts/smd_button.cpp
        src/parts/smd_led.cpp
        src/parts/timer.cpp

        src/parts/dummy_pin.cpp
        src/parts/keying_pin.cpp

        src/arduino.cpp
        src/duino.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${DUINO_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${DUINO_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${DUINO_LIBRARY_NAME}
        ${LIBRARY_NAME}
        ${GUI_LIBRARY_NAME}
        ${GTKMM_LIBRARIES}
        ${GLIBMM_LIBRARIES})

install(TARGETS ${DUINO_LIBRARY_NAME}
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        CONFIGURATIONS Release
        OPTIONAL)

#endregion

#region Catch2

#region Catch2

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3")

set(DUINO_TEST_NAME "${DUINO_LIBRARY_NAME}_test")

include_directories(
        ${PROJECT_INCLUDE_DIR}
        test/include)

add_executable(${DUINO_TEST_NAME}
        test/src/catch.cpp

        test/src/parts.cpp

        test/src/push_button.cpp
        test/src/switch_button.cpp

        test/src/digital_pin.cpp
        test/src/pwm_pin.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${DUINO_TEST_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${DUINO_TEST_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${DUINO_TEST_NAME}
        ${DUINO_LIBRARY_NAME})

#endregion

#endregion

#region Sketch

set(DUINO_SKETCH_NAME "${DUINO_LIBRARY_NAME}_sketch")

add_executable(${DUINO_SKETCH_NAME}
        test/sketch.cpp)

add_dependencies(${DUINO_SKETCH_NAME} ${LIBRARY_NAME} ${GUI_LIBRARY_NAME})

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${DUINO_SKETCH_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${DUINO_SKETCH_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${DUINO_SKETCH_NAME}
        ${DUINO_LIBRARY_NAME})

#endregion
