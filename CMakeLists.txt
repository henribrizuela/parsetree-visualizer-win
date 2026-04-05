cmake_minimum_required(VERSION "3.19.2")

# static link for shared usage
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(Parsetree_visualizer)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXTERNAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(EXTERNAL_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/externals)


add_executable(Parsetree_visualizer)

target_sources(Parsetree_visualizer
	PRIVATE
		${SRC_DIR}/parsetree_visualizer.cpp
	PUBLIC
		FILE_SET cxx_modules TYPE CXX_MODULES FILES
			${MODULE_DIR}/parsetree_app.ixx
			${EXTERNAL_MODULE_DIR}/imgui_wrapper.ixx
)

target_include_directories(Parsetree_visualizer
	PRIVATE
		${EXTERNAL_INCLUDE_DIR}/imgui
		${EXTERNAL_INCLUDE_DIR}/imgui/backends
)
