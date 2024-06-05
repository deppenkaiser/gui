#pragma once

#include <gtk/gtk.h>

typedef struct _gui_spin_button
{
    GtkWidget* button;
    uint32_t id;
    void* user_data;
} *gui_spin_button_t;

typedef struct gui_spin_button_configuration
{
    double value;
    double min;
    double max;
    double increment;
    uint32_t digits;
} *gui_spin_button_configuration_t;

GtkWidget* gui_spin_button_create(uint32_t id, gui_spin_button_configuration_t configuration, void* user_data);
