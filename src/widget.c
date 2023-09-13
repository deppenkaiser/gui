#include "gui/widget.h"

void widget_set_name(GtkWidget* widget, const char* label)
{
    gtk_widget_set_name(widget, label);
}

const char* widget_get_name(GtkWidget* widget)
{
	return gtk_widget_get_name(widget);
}
