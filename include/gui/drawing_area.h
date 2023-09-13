#pragma once

#include <gtk-4.0/gtk/gtk.h>

typedef struct _drawing_area
{
	GtkWidget* drawing_area;
	void* user_data;
	uint32_t id;
} *drawing_area_t;

GtkWidget* drawing_area_create(uint32_t id, uint32_t width, uint32_t height, void* user_data);
void drawing_area_destroy(GtkDrawingArea* drawing_area);
