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
    const char* tooltip;
} *gui_button_configuration_t;

typedef struct gui_spin_button_configuration
{
    float alignment;
    double value;
    double min;
    double max;
    double increment;
    uint32_t digits;
    const char* tooltip;
} *gui_spin_button_configuration_t;

GtkWidget* gui_button_create(uint32_t id, gui_button_configuration_t configuration, void* user_data);
GtkWidget* gui_button_drop_down_create(uint32_t id, const char* strings[], void* user_data);
int32_t gui_button_drop_down_get_selection(GtkWidget* drop_down_button);
GtkWidget* gui_button_spin_create(uint32_t id, gui_spin_button_configuration_t configuration, void* user_data);
void gui_button_spin_set_configuration(GtkWidget* spin_button, gui_spin_button_configuration_t configuration);
double gui_button_spin_get_double(GtkWidget* spin_button);
void gui_button_spin_set_double(GtkWidget* spin_button, double value);
bool gui_button_toggle_is_active(GtkWidget* button_toggle);
