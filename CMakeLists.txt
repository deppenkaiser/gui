cmake_minimum_required(VERSION 3.10)

project(gui LANGUAGES C)

pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(EPOXY REQUIRED epoxy)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(gui STATIC)

target_sources(gui PRIVATE
    gui.c
    button.c
    application.c
    main_window.c
    drawing_area.c
    frame.c
    notebook.c
    text.c
    widget.c
    box.c
	gl.c
)

target_include_directories(gui PUBLIC
    ${GTK4_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
)

target_include_directories(gui PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../threading
    ${CMAKE_CURRENT_LIST_DIR}/../string
    ${CMAKE_CURRENT_LIST_DIR}/../api
    ${CMAKE_CURRENT_LIST_DIR}/../logging
)

target_link_libraries(gui
    ${GTK4_LIBRARIES}
	${EPOXY_LIBRARIES}
	logging
)

link_directories(
	${EPOXY_LIBRARY_DIRS}
)
