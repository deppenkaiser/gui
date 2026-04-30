#pragma once

#include <gtk/gtk.h>

typedef enum
{
	GE_A_STARTUP,
	GE_A_ACTIVATE,
	GE_A_SHUTDOWN,
	GE_BEFORE_PRESENT,
	GE_AFTER_PRESENT,
	GE_CLOSE_REQUEST,
	GE_KEY_PRESSED,
	GE_KEY_RELEASED,
	GE_DA_DRAW,
	GE_DA_MOUSE_CLICK_LEFT,
	GE_B_CLICKED,
	GE_B_TOGGLED,
	GE_B_SELECTED,
	GE_GL_RENDER,
	GE_GL_REALIZE
} gui_event_type_t;

typedef struct _gui_before_present
{
	uint32_t dummy;
} *gui_before_present_t;

typedef struct _gui_after_present
{
	uint32_t dummy;
} *gui_after_present_t;

typedef struct _gui_close_request
{
	bool close;
} *gui_close_request_t;

typedef struct _gui_key_pressed
{
	uint32_t keyval;
	bool handled;
} *gui_key_pressed_t;

typedef struct _gui_key_released
{
	uint32_t keyval;
} *gui_key_released_t;

typedef struct _gui_da_draw_event
{
	GtkDrawingArea* drawing_area;
	cairo_t* cr;
	uint32_t width;
	uint32_t height;
} *gui_da_draw_event_t;

typedef struct _gui_da_mouse_left_click_event
{
	GtkDrawingArea* drawing_area;
	double x;
	double y;
	uint32_t n;
} *gui_da_mouse_left_click_event_t;

typedef struct _gui_b_clicked_event
{
	GtkButton* button;
} *gui_b_clicked_event_t;

typedef struct _gui_b_toggled_event
{
	GtkToggleButton* button;
	bool active;
} *gui_b_toggled_event_t;

typedef union _gui_event_data
{
	struct _gui_before_present before_present;
	struct _gui_after_present after_present;
	struct _gui_close_request close_request;
	struct _gui_key_pressed key_pressed;
	struct _gui_key_released key_released;
	struct _gui_da_draw_event da_draw;
	struct _gui_da_mouse_left_click_event da_mouse_left_click;
	struct _gui_b_clicked_event b_clicked;
	struct _gui_b_toggled_event b_toggled;
} *gui_event_data_t;

typedef struct gui_event
{
	gui_event_type_t type;
	union _gui_event_data data;
} *gui_event_t;
