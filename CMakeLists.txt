# find_package requires 3.25 for wxWidgets3.2.x
cmake_minimum_required(VERSION 3.25)

# options
option(BUILD_TESTS "Build all tests." OFF)
option(BUILD_EXE "Build executable." ON)
option(VERBOSE "Show optional messages." ON)

if (VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
endif()

# project
set(PROJECT_NAME SimpleCommandRunner)
project(${PROJECT_NAME})

# set flags for compilers
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections -flto -D wxDEBUG_LEVEL=0")
    string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "-O3" "-Os" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    if (NOT BUILD_TESTS)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-exceptions")
    endif()
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie -D wxNO_RTTI")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
    if (BUILD_TESTS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-dead_strip")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0")
        message(FATAL_ERROR "The version of Clang should be 4.0 or later.")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(REPLACE "/O2" "/O1" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DwxNO_RTTI")
    string(REPLACE "/EHsc " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /GR- /DwxDEBUG_LEVEL=0")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE
        "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG")
    if (BUILD_TESTS)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_HAS_EXCEPTIONS=0")
    endif()
else()
    message(WARNING "${PROJECT_NAME} does NOT support your compiler. You might get some problems.")
endif()

# check OS
if (NOT ((${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        OR (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        OR (${CMAKE_SYSTEM_NAME} MATCHES "Linux")))
    message(WARNING "${PROJECT_NAME} does NOT support your OS. You might get some problems.")
endif()

# check architecture
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_LOWER)
if ((${ARCH_LOWER} MATCHES "arm") OR (${ARCH_LOWER} MATCHES "aarch"))
    message(WARNING "${PROJECT_NAME} is NOT tested with ARM devices. You might get some problems.")
endif()

# Show settings
message(VERBOSE "SYSTEM                  : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(VERBOSE "ARCHITECTURE            : ${CMAKE_SYSTEM_PROCESSOR}")
message(VERBOSE "COMPILER                : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(VERBOSE "CXX_FLAGS               : ${CMAKE_CXX_FLAGS}")
message(VERBOSE "CXX_FLAGS_RELEASE       : ${CMAKE_CXX_FLAGS_RELEASE}")
message(VERBOSE "EXE_LINKER_FLAGS        : ${CMAKE_EXE_LINKER_FLAGS}")
message(VERBOSE "EXE_LINKER_FLAGS_RELEASE: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message(VERBOSE "BUILD_TESTS             : ${BUILD_TESTS}")
message(VERBOSE "BUILD_EXE               : ${BUILD_EXE}")

# find wxWidgets
# set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# fetch json lib
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)  # avoid to reset cmake_args
include(FetchContent)
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
set(RAPIDJSON_BUILD_CXX11 OFF)
set(RAPIDJSON_BUILD_CXX17 OFF)
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG 973dc9c06dcd3d035ebd039cfb9ea457721ec213 # May 17, 2023
)
FetchContent_MakeAvailable(rapidjson)
include_directories(${rapidjson_SOURCE_DIR}/include)

# make a lib
include_directories(include)
set(SOURCES
    src/main_frame.cpp
    src/exec.cpp
    src/component.cpp
    src/json_utils.cpp
    src/custom_wx_obj.cpp
    src/exe_container.cpp)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib
    ${wxWidgets_LIBRARIES}
)

# add main app
if (BUILD_EXE)
    add_executable(${PROJECT_NAME} "src/main.cpp")
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)
endif()

#------------------------------
# test with Google Test
if (BUILD_TESTS)
    enable_testing()
  
    # Google Test settings (reference: https://google.github.io/googletest/quickstart-cmake.html)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # include gtest
    FetchContent_MakeAvailable(googletest)

    # make test exe files
    add_subdirectory(tests)
endif()
