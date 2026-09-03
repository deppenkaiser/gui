#include "separator.h"

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

GtkWidget* gui_separator_create(uint32_t id, bool horizontal, void* user_data)
{
    GtkWidget* separator = gtk_separator_new(horizontal ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL);
    g_object_set_data(G_OBJECT(separator), "core", malloc(sizeof(struct _gui_separator)));
    gui_separator_t core = _gui_get_core(separator);
    core->separator = separator;
    core->id = id;
    core->user_data = user_data;
    _gui_add_widget_to_internal_list(separator);
    return separator;
}
