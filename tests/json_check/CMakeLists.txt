cmake_minimum_required(VERSION 3.15)

#test for JsonUtils.cpp
add_executable(json_check main.cpp)
target_link_libraries(json_check
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

add_test(
    NAME json_check
    COMMAND $<TARGET_FILE:json_check> "${CMAKE_CURRENT_SOURCE_DIR}/broken.json" "${PROJECT_SOURCE_DIR}/examples/6_all_keys/gui_definition.json"
    )
