cmake_minimum_required(VERSION 3.15)

#test for MainFrame.cpp
add_executable(main_frame main.cpp)
target_link_libraries(main_frame
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

set(TEST_JSON1 "${PROJECT_SOURCE_DIR}/examples/9_all_keys/gui_definition.json")
set(TEST_JSON2 "${CMAKE_CURRENT_SOURCE_DIR}/config_ascii.json")
set(TEST_JSON3 "${CMAKE_CURRENT_SOURCE_DIR}/config_utf.json")

add_test(
    NAME main_frame
    COMMAND $<TARGET_FILE:main_frame> ${TEST_JSON1} ${TEST_JSON2} ${TEST_JSON3}
    )
