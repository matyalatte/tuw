cmake_minimum_required(VERSION 3.15)

#set(CMAKE_BUILD_TYPE DEBUG)

#project
set(PROJECT_NAME SimpleCommandRunner)
project(${PROJECT_NAME})


#fetch json
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
)
FetchContent_MakeAvailable(json)

if (WIN32)
    #you may need to edit here
	set(wxWidgets_ROOT_DIR C:/wxWidgets-3.1.5)
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
)

#link libraries
target_link_libraries(${PROJECT_NAME}
	nlohmann_json::nlohmann_json
	${wxWidgets_LIBRARIES}
)
