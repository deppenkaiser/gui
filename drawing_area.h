#pragma once

#include <gtk/gtk.h>

typedef struct gui_drawing_area
{
	GtkWidget* drawing_area;
	void* user_data;
	uint32_t id;
} *gui_drawing_area_t;

GtkWidget* gui_drawing_area_create(uint32_t id, uint32_t width, uint32_t height, void* user_data);
