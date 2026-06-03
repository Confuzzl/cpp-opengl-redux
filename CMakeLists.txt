cmake_minimum_required(VERSION 3.16)
project(test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(test)
add_subdirectory("src")

target_include_directories(test PRIVATE include)
target_compile_definitions(test PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(test PRIVATE GLM_FORCE_XYZW_ONLY) # constexpr friendly

if(MSVC)
	target_compile_options(test PRIVATE /W4)
else()
	target_compile_options(test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory("glad")
target_link_libraries(test glad)

target_link_libraries(test imgui::imgui)
target_link_libraries(test fmt::fmt)
target_link_libraries(test glfw)
target_link_libraries(test glm::glm)
