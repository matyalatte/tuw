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
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
	string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	string(REPLACE "-O3" "-Os" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
	if (BUILD_TESTS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-dead_strip")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	string(REPLACE "/O2" "/O1" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
else()
    message(WARNING "${PROJECT_NAME} does NOT support your compiler. You might get some problems.")
endif()

# check OS
if (NOT ((${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		OR (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		OR (${CMAKE_SYSTEM_NAME} MATCHES "Linux")))
	message(WARNING "${PROJECT_NAME} does NOT support your OS. You might get some problems.")
endif()

# Show settings
message(VERBOSE "SYSTEM                  : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
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

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP:
cmake_policy(SET CMP0135 NEW)

# fetch json
set(JSON_ImplicitConversions OFF)
set(JSON_DisableEnumSerialization ON)
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

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
	nlohmann_json::nlohmann_json
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
