cmake_minimum_required(VERSION 3.11)

#project
set(PROJECT_NAME SimpleCommandRunner)
project(${PROJECT_NAME})

if (CMAKE_COMPILER_IS_GNUCXX)
	#settings for unix
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
endif()

#fetch json
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

if (WIN32)
	#you may need to edit here
	file(STRINGS WX_VERSION.txt WX_VERSION)
	set(wxWidgets_ROOT_DIR C:/wxWidgets-${WX_VERSION})
endif()

#find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

#add main app
add_executable(${PROJECT_NAME}
	"src/Main.cpp"
	"src/MainFrame.h"
	"src/MainFrame.cpp"
	"src/Exec.h"
	"src/Exec.cpp"
	"src/Component.h"
	"src/Component.cpp"
	"src/JsonUtils.h"
	"src/JsonUtils.cpp"
)

#link libraries
target_link_libraries(${PROJECT_NAME}
	nlohmann_json::nlohmann_json
	${wxWidgets_LIBRARIES}
)
