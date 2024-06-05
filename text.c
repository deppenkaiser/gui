#include "text.h"

extern void gui_text_callback(GtkEntryBuffer* buffer, gui_text_t data) __attribute__((weak));

gui_text_t _gui_text_get_core(GtkText* text);

void _text_changed(GtkEditable* self, gpointer user_data)
{
    gui_text_t data = (gui_text_t) user_data;
    if (gui_text_callback != NULL)
    {
        gui_text_callback(gtk_text_get_buffer(GTK_TEXT(self)), data);
    }
}

GtkWidget* gui_text_create(uint32_t id, float alignment, void* user_data)
{
    GtkWidget* text = gtk_text_new();
	g_object_set_data(G_OBJECT(text), "core", malloc(sizeof(struct _gui_text)));
    gtk_editable_set_alignment(GTK_EDITABLE(text), alignment);
    gui_text_t core = _gui_text_get_core(GTK_TEXT(text));
    core->text = text;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(GTK_EDITABLE(text), "changed", G_CALLBACK(_text_changed), core);
    return text;
}

void gui_text_destroy(GtkText* text)
{
	free(_gui_text_get_core(text));
	g_object_set_data(G_OBJECT(text), "core", NULL);
}

gui_text_t _gui_text_get_core(GtkText* text)
{
	return (gui_text_t) g_object_get_data(G_OBJECT(text), "core");
}
