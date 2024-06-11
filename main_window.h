#pragma once

#include <gdk/gdkkeysyms.h>
#include <stdbool.h>
#include <stdint.h>

#include "application.h"

typedef struct gui_main_window
{
	GtkApplication* app;
	GtkEventController* keyboard_controller;
	GtkWidget* main_window;
	GMenu* menu_bar;
	GMenu* file_menu;
	void* user_data;
} *gui_main_window_t;

GtkWidget* gui_main_window_create(GtkApplication* app, uint32_t width_pix, uint32_t height_pix, void* user_data,
	bool show_menu, bool resizeable);
GMenu* gui_main_window_create_sub_menu(GMenu* menu_bar, const char* sub_menu_name);
void gui_main_window_add_sub_menu_item(GMenu* sub_menu, const char* item_name, const char* action_name, gui_main_window_t core);
