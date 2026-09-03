#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>

typedef GtkWidget* widget_array_t[];

void widget_set_name(GtkWidget* widget, const char* label);
const char* widget_get_name(GtkWidget* widget);

void widget_set_margins(GtkWidget* widget, int start, int end, int top, int bottom);
void widget_set_hexpand(GtkWidget* widget, bool expand);
void widget_set_halign(GtkWidget* widget, GtkAlign alignment);
