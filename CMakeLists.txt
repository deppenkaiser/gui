cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)

project(gui LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(EPOXY REQUIRED epoxy)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(gui STATIC)
add_subdirectory(../logging ${CMAKE_BINARY_DIR}/logging_from_gui)
add_subdirectory(../threading ${CMAKE_BINARY_DIR}/threading_from_gui)
add_subdirectory(../string ${CMAKE_BINARY_DIR}/string_from_gui)
add_subdirectory(../api ${CMAKE_BINARY_DIR}/api_from_gui)

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
)

target_include_directories(gui PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(gui
	PUBLIC
		PkgConfig::GTK4
		logging
		threading
		string
		api
	PRIVATE
		${EPOXY_LIBRARIES}
)
