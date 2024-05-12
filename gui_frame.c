#include "gui_frame.h"

GtkWidget* frame_create(const char* label, GtkWidget* widget)
{
	GtkWidget* frame = gtk_frame_new(label);
	gtk_frame_set_child(GTK_FRAME(frame), widget);
	return frame;
}

GtkWidget* frame_get_child(GtkFrame* frame)
{
	return gtk_frame_get_child(frame);
}
