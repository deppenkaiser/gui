#pragma once

#include <gdk/gdkkeysyms.h>
#include "gui_application.h"

typedef enum mwe_types
{
	MWE_BEFORE_PRESENT,
	MWE_AFTER_PRESENT,
	MWE_CLOSE_REQUEST,
	MWE_KEY_PRESSED,
	MWE_KEY_RELEASED
} mwe_types_t;

typedef struct _main_window
{
	GtkApplication* app;
	GtkEventController* keyboard_controller;
	GtkWidget* main_window;
	GMenu* menu_bar;
	GMenu* file_menu;
	void* user_data;
} *main_window_t;

typedef struct _mwe_before_present
{
	uint32_t dummy;
} mwe_before_present_t;

typedef struct _mwe_after_present
{
	uint32_t dummy;
} mwe_after_present_t;

typedef struct _mwe_close_request
{
	bool close;
} mwe_close_request_t;

typedef struct _mwe_key_pressed
{
	uint32_t keyval;
	bool handled;
} mwe_key_pressed_t;

typedef struct _mwe_key_released
{
	uint32_t keyval;
} mwe_key_released_t;

typedef union _main_window_event
{
	mwe_before_present_t before_present;
	mwe_after_present_t after_present;
	mwe_close_request_t close_request;
	mwe_key_pressed_t key_pressed;
	mwe_key_released_t key_released;
} *main_window_event_t;

void main_window_create(GtkApplication* app, void* user_data, bool show_menu);
GMenu* main_window_create_sub_menu(GMenu* menu_bar, const char* sub_menu_name);
void main_window_add_sub_menu_item(GMenu* sub_menu, const char* item_name, const char* action_name, main_window_t data);
