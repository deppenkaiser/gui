#pragma once

#include <gtk/gtk.h>

GtkWidget* frame_create(const char* label, GtkWidget* widget);
GtkWidget* frame_get_child(GtkFrame* frame);
