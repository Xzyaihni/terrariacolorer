cmake_minimum_required(VERSION 3.13)
project(terraria_colorer)
set(CMAKE_CXX_STANDARD 20)

option(Y_DEBUG "build in debug mode" "OFF")
option(Y_SANITIZE "build with address sanitizer" "OFF")


set(SOURCE_FILES main.cpp
colorer.cpp)

if(${Y_DEBUG})
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(${PROJECT_SOURCE_DIR})

find_package(wxWidgets REQUIRED COMPONENTS richtext xml html net core base)
find_package(Lua REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(${LUA_INCLUDE_DIR})

add_custom_target(folder_files
${CMAKE_COMMAND}
-D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
-D DESTINATION_DIR=${CMAKE_CURRENT_BINARY_DIR}
-P ${PROJECT_SOURCE_DIR}/CopyFolders.cmake
COMMENT "copying asset files" VERBATIM
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} folder_files)


if(${Y_DEBUG})
	target_link_libraries(${PROJECT_NAME} -O1 -pg -Wall)
	
	if(${Y_SANITIZE})
		target_link_libraries(${PROJECT_NAME} -fsanitize=address)
	endif()
else()
	target_link_libraries(${PROJECT_NAME} -O3)
endif()

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
