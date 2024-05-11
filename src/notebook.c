#include "notebook.h"

void notebook_create(GtkWidget* parent)
{
	GtkWidget* notebook = gtk_notebook_new();
    gtk_window_set_child(GTK_WINDOW(parent), notebook);
}

uint32_t notebook_append_page(GtkWidget* notebook_parent, GtkWidget* widget, const char* label)
{
	return gtk_notebook_append_page(GTK_NOTEBOOK(gtk_window_get_child(GTK_WINDOW(notebook_parent))), widget, gtk_label_new(label));
}

GtkWidget* notebook_get_page(GtkWidget* notebook_parent, uint32_t index)
{
	return gtk_notebook_get_nth_page(GTK_NOTEBOOK(gtk_window_get_child(GTK_WINDOW(notebook_parent))), index);
}
