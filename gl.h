#pragma once

#include <gtk/gtk.h>
#include <epoxy/gl.h>

typedef struct _gui_gl
{
	GtkWidget* gl_area;
	gpointer user_data;
	bool render_tick;
} *gui_gl_t;

GtkWidget* gui_gl_create(GtkWidget* parent, gpointer user_data);
