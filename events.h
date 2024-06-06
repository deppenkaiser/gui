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
	GE_B_TOGGLED
} gui_event_type_t;

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
