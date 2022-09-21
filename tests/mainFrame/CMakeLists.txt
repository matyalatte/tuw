cmake_minimum_required(VERSION 3.15)

#test for MainFrame.cpp
add_executable(mainFrame main.cpp)
target_link_libraries(mainFrame
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

add_test(
    NAME mainFrame
    COMMAND $<TARGET_FILE:mainFrame> "${PROJECT_SOURCE_DIR}/examples/6_all_keys/gui_definition.json" "${CMAKE_CURRENT_SOURCE_DIR}/config.json"
    )
