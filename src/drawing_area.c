#include "gui/drawing_area.h"

extern void drawing_area_callback(cairo_t* cr, int width, int height, drawing_area_t data) __attribute__((weak));

drawing_area_t _drawing_area_get_core(GtkDrawingArea* drawing_area);

void _drawing_area_draw(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
	if (drawing_area_callback != NULL)
	{
		drawing_area_callback(cr, width, height, _drawing_area_get_core(drawing_area));
	}
}

GtkWidget* drawing_area_create(uint32_t id, uint32_t width, uint32_t height, void* user_data)
{
    GtkWidget* drawing_area = gtk_drawing_area_new();
	g_object_set_data(G_OBJECT(drawing_area), "core", malloc(sizeof(struct _drawing_area)));
	drawing_area_t core = _drawing_area_get_core(GTK_DRAWING_AREA(drawing_area));
	core->drawing_area = drawing_area;
	core->id = id;
	core->user_data = user_data;
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), _drawing_area_draw, core, NULL);
    gtk_widget_set_size_request(drawing_area, width, height);
    return drawing_area;
}

void drawing_area_destroy(GtkDrawingArea* drawing_area)
{
	free(_drawing_area_get_core(drawing_area));
	g_object_set_data(G_OBJECT(drawing_area), "core", NULL);
	g_object_unref(G_OBJECT(drawing_area));
}

drawing_area_t _drawing_area_get_core(GtkDrawingArea* drawing_area)
{
	return (drawing_area_t) g_object_get_data(G_OBJECT(drawing_area), "core");
}
