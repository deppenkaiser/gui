#pragma once

#include <gtk/gtk.h>

typedef struct _drawing_area
{
	GtkWidget* drawing_area;
	void* user_data;
	uint32_t id;
} *drawing_area_t;

typedef enum
{
	DAE_DRAW,
	DAE_MOUSE_CLICK_LEFT
} drawing_area_event_type_t;

typedef struct _draw_event
{
	GtkDrawingArea* drawing_area;
	cairo_t* cr;
	uint32_t width;
	uint32_t height;
} draw_event_t;

typedef struct _mouse_left_click_event
{
	GtkDrawingArea* drawing_area;
	double x;
	double y;
	uint32_t n;
} mouse_left_click_event_t;

typedef union _drawing_area_event
{
	draw_event_t draw;
	mouse_left_click_event_t mouse_left_click_event;
} *drawing_area_event_t;

GtkWidget* gui_drawing_area_create(uint32_t id, uint32_t width, uint32_t height, void* user_data);
void gui_drawing_area_destroy(GtkDrawingArea* drawing_area);
