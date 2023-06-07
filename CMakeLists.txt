# find_package requires 3.25 for wxWidgets3.2.x
cmake_minimum_required(VERSION 3.25)

# options
option(BUILD_TESTS "Build all tests." OFF)
option(BUILD_EXE "Build executable." ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

# project
set(PROJECT_NAME SimpleCommandRunner)
project(${PROJECT_NAME})

# set flags for compilers
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
	if (BUILD_TESTS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-dead_strip")
endif()


# find wxWidgets
# set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})


# fetch json
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP:
	cmake_policy(SET CMP0135 NEW)
endif()
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

# make a lib
include_directories(include)
add_library(${PROJECT_NAME}_lib STATIC
	"src/main_frame.cpp"
	"src/exec.cpp"
	"src/component.cpp"
	"src/json_utils.cpp"
	"src/custom_wx_obj.cpp"
	"src/exe_container.cpp"
)
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
