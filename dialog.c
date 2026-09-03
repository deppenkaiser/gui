#include "dialog.h"
#include "events.h"

#include <api/api.h>

callback_declaration(void, gui_dialog(gui_dialog_t core, gui_event_t e));

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void _gui_remove_widget_from_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

private void _gui_dialog_destroy(GtkWidget* widget, gpointer user_data)
{
    gui_dialog_t core = (gui_dialog_t) user_data;
    if (gui_dialog != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_DIALOG_DESTROY;
        e.data.dialog_destroy.dialog = widget;
        gui_dialog(core, &e);
    }
    _gui_remove_widget_from_internal_list(widget);
}

private gboolean _gui_dialog_close_request(GtkWindow* self, gpointer user_data)
{
    gboolean close = FALSE;
    gui_dialog_t core = (gui_dialog_t) user_data;
	if (gui_dialog != NULL)
    {
		struct gui_event e = {0};
		e.type = GE_CLOSE_REQUEST;
		gui_dialog(core, &e);
		close = e.data.close_request.close;
	}
    return close;
}

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
    g_signal_connect(dialog, "destroy", G_CALLBACK(_gui_dialog_destroy), core);
    g_signal_connect(dialog, "close-request", G_CALLBACK(_gui_dialog_close_request), core);
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
