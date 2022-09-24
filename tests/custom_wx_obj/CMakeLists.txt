cmake_minimum_required(VERSION 3.15)

#test for custom_wx_obj.cpp
add_executable(custom_wx_obj main.cpp)
target_link_libraries(custom_wx_obj
    gtest
    gtest_main
    ${PROJECT_NAME}_lib
    )

add_test(custom_wx_obj custom_wx_obj)
