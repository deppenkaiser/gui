#include "widget.h"

void gui_widget_set_name(GtkWidget* widget, const char* label)
{
    gtk_widget_set_name(widget, label);
}

const char* gui_widget_get_name(GtkWidget* widget)
{
	return gtk_widget_get_name(widget);
}

void gui_widget_set_margins(GtkWidget* widget, int start, int end, int top, int bottom)
{
    gtk_widget_set_margin_start(widget, start);
    gtk_widget_set_margin_end(widget, end);
    gtk_widget_set_margin_top(widget, top);
    gtk_widget_set_margin_bottom(widget, bottom);
}

void gui_widget_set_hexpand(GtkWidget* widget, bool expand)
{
    gtk_widget_set_hexpand(widget, expand);
}

void gui_widget_set_halign(GtkWidget* widget, GtkAlign alignment)
{
    gtk_widget_set_halign(widget, alignment);
}
