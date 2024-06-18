#include "text.h"

#include <api/api.h>
#include <regex.h>

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

private void _gui_text_changed(GtkEditable* self, gpointer user_data)
{
    gui_text_t data = (gui_text_t) user_data;
    GtkEntryBuffer* buffer = gtk_text_get_buffer(GTK_TEXT(self));
    const char* string = gtk_entry_buffer_get_text(buffer);
    size_t string_length = strlen(string);
    regex_t compiled_expression = {0};
    if (regcomp(&compiled_expression, data->regular_expression, REG_EXTENDED) == 0)
    {
        regmatch_t pmatch[1] = {0};
        regexec(&compiled_expression, string, 1, pmatch, 0);
        
        regoff_t regex_length = pmatch[0].rm_eo - pmatch[0].rm_so;
        if (string_length != regex_length)
        {
            gtk_entry_buffer_delete_text(buffer, regex_length, 1);
        }

        regfree(&compiled_expression);
    }
}

GtkWidget* gui_text_create(uint32_t id, float alignment, const char* regular_expression, const char* value, void* user_data)
{
    GtkWidget* text = gtk_text_new();
	g_object_set_data(G_OBJECT(text), "core", malloc(sizeof(struct _gui_text)));
    gtk_editable_set_alignment(GTK_EDITABLE(text), MIN(MAX(alignment, 0.0f), 1.0));
    gtk_editable_set_text(GTK_EDITABLE(text), value);
    gui_text_t core = _gui_get_core(text);
    core->text = text;
    core->regular_expression = regular_expression;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(GTK_EDITABLE(text), "changed", G_CALLBACK(_gui_text_changed), core);
    _gui_add_widget_to_internal_list(text);
    return text;
}

double gui_text_get_double(GtkWidget* text)
{
    GtkEntryBuffer* buffer = gtk_text_get_buffer(GTK_TEXT(text));
    const char* string = gtk_entry_buffer_get_text(buffer);
    return atof(string);
}
