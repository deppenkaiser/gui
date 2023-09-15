#include "gui/text.h"

extern void text_callback(GtkEntryBuffer* buffer, text_t data) __attribute__((weak));

text_t _text_get_core(GtkText* text);

void _text_changed(GtkEditable* self, gpointer user_data)
{
    text_t data = (text_t) user_data;
    if (text_callback != NULL)
    {
        text_callback(gtk_text_get_buffer(GTK_TEXT(self)), data);
    }
}

GtkWidget* text_create(uint32_t id, float alignment, void* user_data)
{
    GtkWidget* text = gtk_text_new();
	g_object_set_data(G_OBJECT(text), "core", malloc(sizeof(struct _text)));
    gtk_editable_set_alignment(GTK_EDITABLE(text), alignment);
    text_t core = _text_get_core(GTK_TEXT(text));
    core->text = text;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(GTK_EDITABLE(text), "changed", G_CALLBACK(_text_changed), core);
    return text;
}

void text_destroy(GtkText* text)
{
	free(_text_get_core(text));
	g_object_set_data(G_OBJECT(text), "core", NULL);
	g_object_unref(G_OBJECT(text));
}

text_t _text_get_core(GtkText* text)
{
	return (text_t) g_object_get_data(G_OBJECT(text), "core");
}
