cmake_minimum_required(VERSION 3.15)

#test for JsonUtils.cpp
add_executable(exe_container main.cpp)
target_link_libraries(exe_container
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

add_test(
    NAME exe_container
    COMMAND $<TARGET_FILE:exe_container> "${PROJECT_SOURCE_DIR}/examples/all_keys/gui_definition.json"
    )
