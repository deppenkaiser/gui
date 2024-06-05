#pragma once

#include "widget.h"

GtkWidget* gui_box_vertical_create(uint32_t spacing);
GtkWidget* gui_box_horizontal_create(uint32_t spacing);
void gui_box_append_widget(GtkWidget* box, GtkWidget* widget);
void gui_box_append_widgets(GtkWidget* box, widget_array_t widgets, uint32_t count);
