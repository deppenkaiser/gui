#pragma once

#include <gtk/gtk.h>

void notebook_create(GtkWidget* parent);
uint32_t notebook_append_page(GtkWidget* notebook_parent, GtkWidget* widget, const char* label);
GtkWidget* notebook_get_page(GtkWidget* notebook_parent, uint32_t index);
