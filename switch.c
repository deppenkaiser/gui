#include "switch.h"

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

GtkWidget* gui_switch_create(uint32_t id, bool active, void* user_data)
{
    GtkWidget* switch_widget = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(switch_widget), active);
    g_object_set_data(G_OBJECT(switch_widget), "core", malloc(sizeof(struct _gui_switch)));
    gui_switch_t core = _gui_get_core(switch_widget);
    core->switch_widget = switch_widget;
    core->id = id;
    core->user_data = user_data;
    _gui_add_widget_to_internal_list(switch_widget);
    return switch_widget;
}

bool gui_switch_get_active(GtkWidget* switch_widget)
{
    return gtk_switch_get_active(GTK_SWITCH(switch_widget));
}

void gui_switch_set_active(GtkWidget* switch_widget, bool active)
{
    gtk_switch_set_active(GTK_SWITCH(switch_widget), active);
}
