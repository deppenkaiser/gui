#pragma once

#include "widget.h"

GtkWidget* box_vertical_create(uint32_t spacing);
GtkWidget* box_horizontal_create(uint32_t spacing);
void box_append_widget(GtkBox* box, GtkWidget* widget);
void box_append_widgets(GtkBox* box, widget_array_t widgets, uint32_t count);
