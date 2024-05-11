cmake_minimum_required(VERSION 3.10)

project(gui LANGUAGES C)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_library(gui STATIC)

target_sources(gui PRIVATE
    src/gui_application.c
    src/gui_main_window.c
    #src/gui.c
    #src/widget.c
    #src/text.c
    #src/notebook.c
    #src/frame.c
    #src/box.c
    #src/button.c
    #src/drawing_area.c
)

target_include_directories(gui PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../logging/include
    ${CMAKE_CURRENT_LIST_DIR}/../string/include
    PkgConfig::GTK4
)

target_link_libraries(gui
    logging
    string
    PkgConfig::GTK4
)
