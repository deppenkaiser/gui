#pragma once

#include <gtk/gtk.h>

GtkWidget* gui_frame_create(const char* label, GtkWidget* widget);
GtkWidget* gui_frame_get_child(GtkFrame* frame);
