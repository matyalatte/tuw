cmake_minimum_required(VERSION 3.15)

#test for jsonUtils.hpp
add_executable(jsonCheck main.cpp)
target_link_libraries(jsonCheck
    gtest
    gtest_main
	${PROJECT_NAME}_lib
    )

add_test(
    NAME jsonCheck
    COMMAND $<TARGET_FILE:jsonCheck> "${CMAKE_CURRENT_SOURCE_DIR}/broken.json" "${CMAKE_CURRENT_SOURCE_DIR}/test.json"
    )