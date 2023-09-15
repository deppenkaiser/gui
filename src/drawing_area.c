#include "gui/drawing_area.h"

extern void drawing_area_callback(drawing_area_event_type_t type, drawing_area_t data, drawing_area_event_t e) __attribute__((weak));

drawing_area_t _drawing_area_get_core(GtkDrawingArea* drawing_area);

void _drawing_area_draw(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
	if (drawing_area_callback != NULL)
	{
		drawing_area_t data = (drawing_area_t) user_data;
		union _drawing_area_event e = {0};
		e.draw.drawing_area = drawing_area;
		e.draw.width = width;
		e.draw.height = height;
		e.draw.cr = cr;
		drawing_area_callback(DAE_DRAW, data, &e);
	}
}

void _drawing_area_mouse_button_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	if (drawing_area_callback != NULL)
	{
		drawing_area_t data = (drawing_area_t) user_data;
		union _drawing_area_event e = {0};
		e.mouse_left_click_event.drawing_area = GTK_DRAWING_AREA(data->drawing_area);
		e.mouse_left_click_event.x = x;
		e.mouse_left_click_event.y = y;
		e.mouse_left_click_event.n = n_press;
		drawing_area_callback(DAE_MOUSE_CLICK_LEFT, data, &e);
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
    GtkGesture* mouse_click = gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(drawing_area), GTK_EVENT_CONTROLLER(mouse_click));
    g_signal_connect(GTK_GESTURE(mouse_click), "pressed", G_CALLBACK(_drawing_area_mouse_button_pressed), core);
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
