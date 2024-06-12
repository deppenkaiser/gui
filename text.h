#pragma once

#include <gtk/gtk.h>

typedef struct _gui_text
{
    GtkWidget* text;
    uint32_t id;
    const char* white_list;
    void* user_data;
} *gui_text_t;

GtkWidget* gui_text_create(uint32_t id, float alignment, const char* white_list, const char* value, void* user_data);
