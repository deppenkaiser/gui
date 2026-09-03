#pragma once

#include <gtk/gtk.h>
#include <stdint.h>

typedef struct _gui_label
{
    GtkWidget* label;
    uint32_t id;
    void* user_data;
} *gui_label_t;

GtkWidget* gui_label_create(uint32_t id, const char* text, void* user_data);
const char* gui_label_get_text(GtkWidget* label);
void gui_label_set_text(GtkWidget* label, const char* text);
