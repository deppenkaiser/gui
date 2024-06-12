#include "text.h"

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

static void _gui_text_changed(GtkEditable* self, gpointer user_data)
{
    gui_text_t data = (gui_text_t) user_data;
    GtkEntryBuffer* buffer = gtk_text_get_buffer(GTK_TEXT(self));
    const char* string = gtk_entry_buffer_get_text(buffer);
    uint32_t string_length = strlen(string);
    for (uint32_t i = 0; i < string_length; ++i)
    {
        if (strchr(data->white_list, string[i]) == NULL)
        {
            gtk_entry_buffer_delete_text(buffer, i, 1);
        }
    }
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

GtkWidget* gui_text_create(uint32_t id, float alignment, const char* white_list, const char* value, void* user_data)
{
    GtkWidget* text = gtk_text_new();
	g_object_set_data(G_OBJECT(text), "core", malloc(sizeof(struct _gui_text)));
    gtk_editable_set_alignment(GTK_EDITABLE(text), MIN(MAX(alignment, 0.0f), 1.0));
    gtk_editable_set_text(GTK_EDITABLE(text), value);
    gui_text_t core = _gui_get_core(text);
    core->text = text;
    core->white_list = white_list;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(GTK_EDITABLE(text), "changed", G_CALLBACK(_gui_text_changed), core);
    _gui_add_widget_to_internal_list(text);
    return text;
}
