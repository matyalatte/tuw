cmake_minimum_required(VERSION 3.15)

#test for MainFrame.cpp
add_executable(main_frame main.cpp)
target_link_libraries(main_frame
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

add_test(
    NAME main_frame
    COMMAND $<TARGET_FILE:main_frame> "${PROJECT_SOURCE_DIR}/examples/6_all_keys/gui_definition.json" "${CMAKE_CURRENT_SOURCE_DIR}/config.json"
    )
