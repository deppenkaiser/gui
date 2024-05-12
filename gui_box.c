#include "box.h"

GtkWidget* box_vertical_create(uint32_t spacing)
{
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
    return box;
}

GtkWidget* box_horizontal_create(uint32_t spacing)
{
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
	return box;
}

void box_append_widget(GtkBox* box, GtkWidget* widget)
{
    gtk_box_append(box, widget);
}

void box_append_widgets(GtkBox* box, widget_array_t widgets, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        gtk_box_append(box, widgets[i]);
    }
}
