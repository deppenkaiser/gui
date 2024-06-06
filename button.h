#pragma once

#include <gtk/gtk.h>

typedef struct _gui_button
{
    GtkWidget* button;
    uint32_t id;
    void* user_data;
} *gui_button_t;

typedef struct gui_button_configuration
{
    const char* label;
    bool toggle;
} *gui_button_configuration_t;

GtkWidget* gui_button_create(uint32_t id, gui_button_configuration_t configuration, void* user_data);
