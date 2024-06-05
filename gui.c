#include "gui/gui.h"

void gui_destroy_widget_core(GtkWidget* widget)
{
	free(gui_get_core(widget));
	g_object_set_data(G_OBJECT(widget), "core", NULL);
}

void* gui_get_core(GtkWidget* widget)
{
	return g_object_get_data(G_OBJECT(widget), "core");
}
