#include "drawing_area.h"
#include "gui.h"

extern void gui_drawing_area_callback(gui_drawing_area_t core, gui_event_t e) __attribute__((weak));

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

void _gui_drawing_area_draw(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
	if (gui_drawing_area_callback != NULL)
	{
		gui_drawing_area_t core = (gui_drawing_area_t) user_data;
		struct gui_event e = {0};
		e.type = GE_DA_DRAW;
		e.data.da_draw.drawing_area = drawing_area;
		e.data.da_draw.width = width;
		e.data.da_draw.height = height;
		e.data.da_draw.cr = cr;
		gui_drawing_area_callback(core, &e);
	}
}

void _gui_drawing_area_mouse_button_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	if (gui_drawing_area_callback != NULL)
	{
		gui_drawing_area_t core = (gui_drawing_area_t) user_data;
		struct gui_event e = {0};
		e.type = GE_DA_MOUSE_CLICK_LEFT;
		e.data.da_mouse_left_click.drawing_area = GTK_DRAWING_AREA(core->drawing_area);
		e.data.da_mouse_left_click.x = x;
		e.data.da_mouse_left_click.y = y;
		e.data.da_mouse_left_click.n = n_press;
		gui_drawing_area_callback(core, &e);
	}
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

GtkWidget* gui_drawing_area_create(uint32_t id, uint32_t width, uint32_t height, void* user_data)
{
    GtkWidget* drawing_area = gtk_drawing_area_new();
	g_object_set_data(G_OBJECT(drawing_area), "core", malloc(sizeof(struct gui_drawing_area)));
	gui_drawing_area_t core = gui_get_core(drawing_area);
	core->drawing_area = drawing_area;
	core->id = id;
	core->user_data = user_data;
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), _gui_drawing_area_draw, core, NULL);
    gtk_widget_set_size_request(drawing_area, width, height);
    GtkGesture* mouse_click = gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(drawing_area), GTK_EVENT_CONTROLLER(mouse_click));
    g_signal_connect(GTK_GESTURE(mouse_click), "pressed", G_CALLBACK(_gui_drawing_area_mouse_button_pressed), core);
    return drawing_area;
}
