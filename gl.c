#include "gl.h"
#include <api/api.h>
#include <epoxy/gl.h>
#include "events.h"

callback_declaration(void, gui_gl(gui_gl_t core, gui_event_t e));

protected_import(void*, _gui_get_core(GtkWidget*));
protected_import(void, _gui_add_widget_to_internal_list(GtkWidget*));

private gboolean _gl_tick_cb(GtkWidget* widget, GdkFrameClock* clock, gpointer user_data)
{
    gui_gl_t core = user_data;
    gtk_gl_area_queue_render(GTK_GL_AREA(widget));
    return G_SOURCE_CONTINUE; // Loop weiterlaufen lassen
}

private gboolean _gui_gl_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data)
{
	gui_gl_t core = user_data;

	gtk_gl_area_make_current(area);

	if (gui_gl != NULL)
	{
		if (core->render_tick == false)
		{
			gtk_widget_add_tick_callback(GTK_WIDGET(core->gl_area), _gl_tick_cb, core, NULL);
			core->render_tick = true;
		}

		struct gui_event e = {0};
        e.type = GE_GL_RENDER;
		gui_gl(core, &e);
	}
	else
	{
		glClearColor(0.0, 0.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	return TRUE;
}

private void _gui_gl_realize(GtkGLArea *area, gpointer user_data)
{
	gui_gl_t core = user_data;

	gtk_gl_area_make_current(area);

	if (gui_gl != NULL)
	{
        struct gui_event e = {0};
        e.type = GE_GL_REALIZE;
		gui_gl(core, &e);
	}
}

GtkWidget* gui_gl_create(gpointer user_data)
{
	GtkWidget* gl =  gtk_gl_area_new();
    g_object_set_data(G_OBJECT(gl), "core", malloc(sizeof(struct _gui_gl)));
    gui_gl_t core = _gui_get_core(gl);
	core->gl_area = gl;
	core->user_data = user_data;
	core->render_tick = false;
	gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(gl), FALSE);
	gtk_widget_set_vexpand(gl, TRUE);
	gtk_widget_set_hexpand(gl, TRUE);
	g_signal_connect(gl, "render", G_CALLBACK(_gui_gl_render), core);
	g_signal_connect(gl, "realize", G_CALLBACK(_gui_gl_realize), core);
    _gui_add_widget_to_internal_list(gl);
	return gl;
}
