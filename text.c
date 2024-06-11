#include "text.h"
#include "gui.h"

extern void gui_text_callback(GtkEntryBuffer* buffer, gui_text_t data) __attribute__((weak));

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

static void _gui_text_changed(GtkEditable* self, gpointer user_data)
{
    gui_text_t data = (gui_text_t) user_data;
    if (gui_text_callback != NULL)
    {
        gui_text_callback(gtk_text_get_buffer(GTK_TEXT(self)), data);
    }
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

GtkWidget* gui_text_create(uint32_t id, float alignment, void* user_data)
{
    GtkWidget* text = gtk_text_new();
	g_object_set_data(G_OBJECT(text), "core", malloc(sizeof(struct _gui_text)));
    gtk_editable_set_alignment(GTK_EDITABLE(text), alignment);
    gui_text_t core = _gui_get_core(text);
    core->text = text;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(GTK_EDITABLE(text), "changed", G_CALLBACK(_gui_text_changed), core);
    _gui_add_widget_to_internal_list(text);
    return text;
}

/*
void gui_text_callback(GtkEntryBuffer* buffer, gui_text_t data)
{
    skyview_application_data_t ad = (skyview_application_data_t) data->user_data;
    const char* string = gtk_entry_buffer_get_text(buffer);
    int32_t index = strlen(string) - 1;

    if (index >= 0)
    {
        bool delete_char = (isdigit(string[index]) == 0) && (string[index] != ',');
        if (delete_char)
        {
            gtk_entry_buffer_delete_text(buffer, index, 1);
        }
    }
}
*/