cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

project(gui LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(EPOXY REQUIRED epoxy)
pkg_check_modules(VULKAN REQUIRED IMPORTED_TARGET vulkan)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
pkg_check_modules(X11_XCB REQUIRED IMPORTED_TARGET x11-xcb)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(gui STATIC)
add_subdirectory(../logging ${CMAKE_BINARY_DIR}/logging_from_gui)
add_subdirectory(../threading ${CMAKE_BINARY_DIR}/threading_from_gui)
add_subdirectory(../string ${CMAKE_BINARY_DIR}/string_from_gui)
add_subdirectory(../api ${CMAKE_BINARY_DIR}/api_from_gui)
add_subdirectory(../vulkan_base ${CMAKE_BINARY_DIR}/vulkan_base_from_gui)
add_subdirectory(../vulkan_graphics ${CMAKE_BINARY_DIR}/vulkan_graphics_from_gui)
add_subdirectory(../json ${CMAKE_BINARY_DIR}/json_from_gui)

target_sources(gui PRIVATE
	gui.c
	button.c
	application.c
	main_window.c
	drawing_area.c
	frame.c
	notebook.c
	text.c
	label.c
	dialog.c
	switch.c
	separator.c
	widget.c
	box.c
	gl.c
	vulkan.c
	gui_vulkan_controls.c
)

target_include_directories(gui PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gui
	PUBLIC
		PkgConfig::GTK4
		PkgConfig::VULKAN
		PkgConfig::GLFW
		PkgConfig::JSONC
		logging
		threading
		string
		api
		vulkan_base
		vulkan_graphics
		json
	PRIVATE
		${EPOXY_LIBRARIES}
		${X11_LIBRARIES}
		${XCB_LIBRARIES}
		${X11_XCB_LIBRARIES}
)
