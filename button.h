#pragma once

#include <gtk/gtk.h>
#include <stdint.h>
#include <stdbool.h>

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

typedef struct gui_spin_button_configuration
{
    double value;
    double min;
    double max;
    double increment;
    uint32_t digits;
} *gui_spin_button_configuration_t;

GtkWidget* gui_button_create(uint32_t id, gui_button_configuration_t configuration, void* user_data);
GtkWidget* gui_spin_button_create(uint32_t id, gui_spin_button_configuration_t configuration, void* user_data);
double gui_spin_button_get_double(GtkWidget* spin_button);
