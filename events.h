#pragma once

#include <gtk/gtk.h>

typedef enum
{
	GE_DA_DRAW,
	GE_DA_MOUSE_CLICK_LEFT
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

typedef union _gui_event_data
{
	struct _gui_da_draw_event da_draw;
	struct _gui_da_mouse_left_click_event da_mouse_left_click_event;
} *gui_event_data_t;

typedef struct gui_event
{
	gui_event_type_t type;
	union _gui_event_data data;
} *gui_event_t;
