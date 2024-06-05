#pragma once

#include <gtk/gtk.h>

typedef struct _button
{
    GtkWidget* button;
    uint32_t id;
    void* user_data;
} *button_t;

GtkWidget* button_create(uint32_t id, void* user_data);
GtkWidget* button_with_label_create(uint32_t id, const char* label, void* user_data);
