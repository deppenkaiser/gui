#include "dialog.h"

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

GtkWidget* gui_dialog_create(uint32_t id, const char* title, uint32_t width, uint32_t height, void* user_data)
{
    GtkWidget* dialog = gtk_window_new();
    g_object_set_data(G_OBJECT(dialog), "core", malloc(sizeof(struct _gui_dialog)));
    gui_dialog_t core = _gui_get_core(dialog);
    core->dialog = dialog;
    core->id = id;
    core->user_data = user_data;
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    _gui_add_widget_to_internal_list(dialog);
    return dialog;
}

void gui_dialog_set_title(GtkWidget* dialog, const char* title)
{
    gtk_window_set_title(GTK_WINDOW(dialog), title);
}

void gui_dialog_close(GtkWidget* dialog)
{
    gtk_window_close(GTK_WINDOW(dialog));
}

void gui_dialog_set_modal(GtkWidget* dialog, bool modal)
{
    gtk_window_set_modal(GTK_WINDOW(dialog), modal);
}

void gui_dialog_set_transient_for(GtkWidget* dialog, GtkWidget* parent)
{
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
}

void gui_dialog_set_child(GtkWidget* dialog, GtkWidget* child)
{
    gtk_window_set_child(GTK_WINDOW(dialog), child);
}

void gui_dialog_present(GtkWidget* dialog)
{
    gtk_window_present(GTK_WINDOW(dialog));
}

void gui_dialog_set_resizable(GtkWidget* dialog, bool resizable)
{
    gtk_window_set_resizable(GTK_WINDOW(dialog), resizable);
}

void gui_dialog_set_default_size(GtkWidget* dialog, uint32_t width, uint32_t height)
{
    gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);
}
