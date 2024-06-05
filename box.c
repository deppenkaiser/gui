#include "box.h"

GtkWidget* gui_box_vertical_create(uint32_t spacing)
{
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
    return box;
}

GtkWidget* gui_box_horizontal_create(uint32_t spacing)
{
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
	return box;
}

void gui_box_append_widget(GtkBox* box, GtkWidget* widget)
{
    gtk_box_append(box, widget);
}

void gui_box_append_widgets(GtkBox* box, widget_array_t widgets, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        gtk_box_append(box, widgets[i]);
    }
}
