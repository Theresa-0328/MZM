cmake_minimum_required (VERSION 3.8)

project ("MZM")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/Zi /sdl /utf-8)
add_link_options(/MANIFESTUAC:level='highestAvailable')
#add_link_options(/MANIFESTUAC:uiAccess='true')

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/backends) 
add_library(imgui STATIC 
	3rdparty/imgui/imgui.cpp
	3rdparty/imgui/imgui_demo.cpp
	3rdparty/imgui/imgui_draw.cpp
	3rdparty/imgui/imgui_tables.cpp
	3rdparty/imgui/imgui_widgets.cpp
	3rdparty/imgui/backends/imgui_impl_dx11.cpp
	3rdparty/imgui/backends/imgui_impl_win32.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cs2_dump) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src) 
add_executable ("MZM"
	src/main.cpp  
	src/myimgui.cpp)
target_link_libraries("MZM" imgui d3d11 dwmapi)