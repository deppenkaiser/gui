#pragma once

#include <gtk/gtk.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _gui_switch
{
    GtkWidget* switch_widget;
    uint32_t id;
    void* user_data;
} *gui_switch_t;

GtkWidget* gui_switch_create(uint32_t id, bool active, void* user_data);
bool gui_switch_get_active(GtkWidget* switch_widget);
void gui_switch_set_active(GtkWidget* switch_widget, bool active);
