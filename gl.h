#pragma once

#include <gtk/gtk.h>

typedef struct _gui_gl
{
	GtkWidget* gl_area;
	gpointer user_data;
} *gui_gl_t;

GtkWidget* gui_gl_create(GtkWidget* parent, gpointer user_data);
