cmake_minimum_required(VERSION 3.16)
project(imgui)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/GLFW/lib/cmake")
endif()
find_package(glfw3 REQUIRED)

set(CORE_FILES
		imconfig.h
		imgui.cpp
		imgui.h
		imgui_demo.cpp
		imgui_draw.cpp
		imgui_internal.h
		imgui_tables.cpp
		imgui_widgets.cpp
		imstb_rectpack.h
		imstb_textedit.h
		imstb_truetype.h
)
set(BACKEND_FILES
		backends/imgui_impl_glfw.h
		backends/imgui_impl_glfw.cpp
		backends/imgui_impl_opengl3.h
		backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${CORE_FILES} ${BACKEND_FILES})
target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR})
