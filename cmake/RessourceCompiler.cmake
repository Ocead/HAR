function(compile_resource TARGET RESOURCE_COMPILER PATH)
    file(GLOB RESOURCES ${PATH})
    foreach (INPUT_FILE ${RESOURCES})
        if (NOT ${INPUT_FILE} MATCHES ".*.cpp" AND NOT IS_DIRECTORY ${INPUT_FILE})
            get_filename_component(RES_PATH ${INPUT_FILE} DIRECTORY)
            get_filename_component(INPUT_FILE_NAME ${INPUT_FILE} NAME)
            set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/binary/${INPUT_FILE_NAME}.cpp)
            add_custom_command(
                    OUTPUT ${OUTPUT_FILE}
                    COMMAND ${RESOURCE_COMPILER} -i ${INPUT_FILE_NAME} ${OUTPUT_FILE}
                    WORKING_DIRECTORY ${RES_PATH}
                    COMMENT "Creating CXX source file ${INPUT_FILE_NAME}.cpp"
                    DEPENDS ${INPUT_FILE})
            list(APPEND RESOURCES ${OUTPUT_FILE})
        endif ()
    endforeach ()
    set(${TARGET} ${RESOURCES} PARENT_SCOPE)
endfunction()