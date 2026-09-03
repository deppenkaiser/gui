#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>

typedef GtkWidget* widget_array_t[];

void gui_widget_set_name(GtkWidget* widget, const char* label);
const char* gui_widget_get_name(GtkWidget* widget);

void gui_widget_set_margins(GtkWidget* widget, int start, int end, int top, int bottom);
void gui_widget_set_hexpand(GtkWidget* widget, bool expand);
void gui_widget_set_halign(GtkWidget* widget, GtkAlign alignment);
