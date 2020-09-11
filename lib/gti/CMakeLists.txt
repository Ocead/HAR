cmake_minimum_required(VERSION 3.10)
project("HAR Supplement for GTI"
        LANGUAGES CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3 -fno-exceptions")

find_library(hargui
        NAMES hargui harguilib
        HINTS "${CMAKE_BINARY_DIR}/hargui")
set(GUI_LIBRARY_NAME hargui)

#region GTI

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

set(GTI_LIBRARY_NAME hargti)

add_library(${GTI_LIBRARY_NAME}
        src/gti.cpp)

if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set_property(TARGET ${GTI_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
else ()
    set_property(TARGET ${GTI_LIBRARY_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION False)
endif ()

target_link_libraries(${GTI_LIBRARY_NAME}
        ${LIBRARY_NAME}
        ${GUI_LIBRARY_NAME}
        ${GTKMM_LIBRARIES}
        ${GLIBMM_LIBRARIES})

install(TARGETS ${GTI_LIBRARY_NAME}
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        CONFIGURATIONS Release
        OPTIONAL)

#endregion
