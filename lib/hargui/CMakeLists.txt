cmake_minimum_required(VERSION 3.10)
project("HAR Cross Platform GUI"
        LANGUAGES CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3")

find_library(har
        NAMES har harlib
        HINTS "${CMAKE_BINARY_DIR}/har")
set(LIBRARY_NAME har)

#region GUI

find_package(PkgConfig)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(GLIBMM REQUIRED glibmm-2.4)
pkg_check_modules(SIGCPP REQUIRED sigc++-2.0)

include_directories(
        ${PROJECT_INCLUDE_DIR}
        include
        ${GTKMM_INCLUDE_DIRS}
        ${SIGCPP_INCLUDE_DIRS}
        ${GLIBMM_INCLUDE_DIRS})

link_directories(
        ${GTKMM_LIBRARY_DIRS}
        ${GLIBMM_LIBRARY_DIRS})

set(GUI_LIBRARY_NAME hargui)

add_library(${GUI_LIBRARY_NAME}
        src/about.cpp
        src/gui.cpp
        src/action_bar.cpp
        src/cell.cpp
        src/cargo_list.cpp
        src/connection_list.cpp
        src/connection_popover.cpp
        src/grid.cpp
        src/headerbar.cpp
        src/main_win.cpp
        src/part.cpp
        src/properties.cpp
        src/property_list.cpp
        src/terminal.cpp
        src/timing_control.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${GUI_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${GUI_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${GUI_LIBRARY_NAME}
        ${LIBRARY_NAME}
        ${GTKMM_LIBRARIES}
        ${GLIBMM_LIBRARIES}
        ${SIGCPP_LIBRARIES})

install(TARGETS ${GUI_LIBRARY_NAME}
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        CONFIGURATIONS Release
        OPTIONAL)

#endregion

#region Sketch

set(GUI_SKETCH_NAME "${GUI_LIBRARY_NAME}_sketch")

add_executable(${GUI_SKETCH_NAME}
        test/src/main.cpp)

target_link_libraries(${GUI_SKETCH_NAME}
        ${GUI_LIBRARY_NAME})

#endregion
