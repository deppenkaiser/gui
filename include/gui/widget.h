#pragma once

#include <gtk-4.0/gtk/gtk.h>

typedef GtkWidget* widget_array_t[];

void widget_set_name(GtkWidget* widget, const char* label);
const char* widget_get_name(GtkWidget* widget);
