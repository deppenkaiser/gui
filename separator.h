#pragma once

#include <gtk/gtk.h>
#include <stdint.h>

typedef struct _gui_separator
{
    GtkWidget* separator;
    uint32_t id;
    void* user_data;
} *gui_separator_t;

GtkWidget* gui_separator_create(uint32_t id, bool horizontal, void* user_data);
