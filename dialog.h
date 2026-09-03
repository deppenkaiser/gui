#pragma once

#include <gtk/gtk.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct _gui_dialog
{
    GtkWidget* dialog;
    uint32_t id;
    void* user_data;
} *gui_dialog_t;

GtkWidget* gui_dialog_create(uint32_t id, const char* title, uint32_t width, uint32_t height, void* user_data);
void gui_dialog_set_title(GtkWidget* dialog, const char* title);
void gui_dialog_close(GtkWidget* dialog);
void gui_dialog_set_modal(GtkWidget* dialog, bool modal);
void gui_dialog_set_transient_for(GtkWidget* dialog, GtkWidget* parent);
void gui_dialog_set_child(GtkWidget* dialog, GtkWidget* child);
void gui_dialog_present(GtkWidget* dialog);
void gui_dialog_set_resizable(GtkWidget* dialog, bool resizable);
void gui_dialog_set_default_size(GtkWidget* dialog, uint32_t width, uint32_t height);
