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

void gui_box_append_widget(GtkWidget* box, GtkWidget* widget)
{
    gtk_box_append(GTK_BOX(box), widget);
}

void gui_box_append_widgets(GtkWidget* box, widget_array_t widgets, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        gtk_box_append(GTK_BOX(box), widgets[i]);
    }
}
