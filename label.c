#include "label.h"

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

GtkWidget* gui_label_create(uint32_t id, const char* text, void* user_data)
{
    GtkWidget* label = gtk_label_new(text);
    g_object_set_data(G_OBJECT(label), "core", malloc(sizeof(struct _gui_label)));
    gui_label_t core = _gui_get_core(label);
    core->label = label;
    core->id = id;
    core->user_data = user_data;
    _gui_add_widget_to_internal_list(label);
    return label;
}

const char* gui_label_get_text(GtkWidget* label)
{
    return gtk_label_get_text(GTK_LABEL(label));
}

void gui_label_set_text(GtkWidget* label, const char* text)
{
    gtk_label_set_text(GTK_LABEL(label), text);
}
