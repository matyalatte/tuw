cmake_minimum_required(VERSION 3.15)

# options
option(BUILD_TESTS "Build all tests." OFF)
option(BUILD_EXE "Build executable." ON)

# project
set(PROJECT_NAME SimpleCommandRunner)
project(${PROJECT_NAME})

if (CMAKE_COMPILER_IS_GNUCXX)
	#settings for GCC
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
	if (BUILD_TESTS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
		set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	endif()
endif()

# find wxWidgets
# set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# fetch json
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

# make a lib
include_directories(include)
add_library(${PROJECT_NAME}_lib STATIC
	"src/MainFrame.cpp"
	"src/Exec.cpp"
	"src/Component.cpp"
	"src/JsonUtils.cpp"
	"src/CustomWxObj.cpp"
)
target_link_libraries(${PROJECT_NAME}_lib
	nlohmann_json::nlohmann_json
	${wxWidgets_LIBRARIES}
)

# add main app
if (BUILD_EXE)
	add_executable(${PROJECT_NAME} "src/Main.cpp")
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
