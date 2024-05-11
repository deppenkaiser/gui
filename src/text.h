#pragma once

#include <gtk-4.0/gtk/gtk.h>

typedef struct _text
{
    GtkWidget* text;
    uint32_t id;
    void* user_data;
} *text_t;

GtkWidget* text_create(uint32_t id, float alignment, void* user_data);
void text_destroy(GtkText* text);
