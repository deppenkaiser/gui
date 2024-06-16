#include "main_window.h"
#include "events.h"

#include <api/api.h>
#include <string/string.h>
#include <logging/logging.h>

extern void gui_main_window_callback(gui_main_window_t core, gui_event_t e) __attribute__((weak));
extern void gui_main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, gui_main_window_t core) __attribute__((weak));

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);
extern void _gui_destroy_all_widget_cores();

private gboolean _gui_main_window_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
    gboolean handled = FALSE;
	if (gui_main_window_callback != NULL)
	{
		struct gui_event e = {0};
		e.type = GE_KEY_PRESSED;
		e.data.key_pressed.keyval = keyval;
		logging_log_message("key pressed event begin...", true);
		gui_main_window_callback((gui_main_window_t) user_data, &e);
		logging_log_message("key pressed event end...", true);
		handled = e.data.key_pressed.handled;
	}
    return handled;
}

private void _gui_main_window_key_released(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
	if (gui_main_window_callback != NULL)
	{
		struct gui_event e = {0};
		e.type = GE_KEY_RELEASED;
		e.data.key_released.keyval = keyval;
		logging_log_message("key released event begin...", true);
		gui_main_window_callback((gui_main_window_t) user_data, &e);
		logging_log_message("key released event end...", true);
	}
}

private gboolean _gui_main_window_close_request(GtkWindow* self, gpointer user_data)
{
	gboolean close = FALSE;
	if (gui_main_window_callback != NULL)
	{
		struct gui_event e = {0};
		e.type = GE_CLOSE_REQUEST;
		gui_main_window_callback((gui_main_window_t) user_data, &e);
		close = e.data.close_request.close;
	}

	if (close == FALSE)
	{
		_gui_destroy_all_widget_cores();
	}

    return close;
}

private void _gui_main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, gpointer user_data)
{
	if (gui_main_window_action_callback != NULL)
	{
		logging_log_message("action event begin...", true);
		gui_main_window_action_callback(simple_action, parameter, (gui_main_window_t) user_data);
		logging_log_message("action event end...", true);
	}
}

private void _gui_main_window_add_action(GtkApplication* app, const char* action_name, gui_main_window_t core)
{
    GSimpleAction* action = g_simple_action_new(action_name, NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(_gui_main_window_action_callback), core);
}

private GMenu* _gui_main_window_create_menu_bar(GtkApplication* app, GtkApplicationWindow* window)
{
    GMenu* menu_bar = g_menu_new();
    gtk_application_set_menubar(app, G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(window, TRUE);
    return menu_bar;
}

GtkWidget* gui_main_window_create(GtkApplication* app, uint32_t width_pix, uint32_t height_pix, void* user_data,
	bool show_menu, bool resizeable)
{
	GtkWidget* main_window = gtk_application_window_new(app);
	g_object_set_data(G_OBJECT(main_window), "core", malloc(sizeof(struct gui_main_window)));
	gui_main_window_t core = _gui_get_core(main_window);
	core->main_window = main_window;
	core->user_data = user_data;
	core->app = app;
	#ifdef USE_GTK3
	core.keyboard_controller = gtk_event_controller_key_new(core.main_window);
	#else
	core->keyboard_controller = gtk_event_controller_key_new();
    gtk_widget_add_controller(core->main_window, core->keyboard_controller);
	#endif
	core->menu_bar = _gui_main_window_create_menu_bar(app, GTK_APPLICATION_WINDOW(core->main_window));
    core->file_menu = gui_main_window_create_sub_menu(core->menu_bar, "File");
	
	gui_main_window_add_sub_menu_item(core->file_menu, "Exit", "exit", core);
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(core->main_window), show_menu ? TRUE : FALSE);
	gtk_window_set_default_size(GTK_WINDOW(core->main_window), width_pix, height_pix);
	gtk_window_set_resizable(GTK_WINDOW(core->main_window), resizeable ?  TRUE : FALSE);

    g_signal_connect(core->keyboard_controller, "key-pressed", G_CALLBACK(_gui_main_window_key_pressed), core);
    g_signal_connect(core->keyboard_controller, "key-released", G_CALLBACK(_gui_main_window_key_released), core);
	#ifdef USE_GTK3
	g_signal_connect(G_OBJECT(core.main_window), "delete-event", G_CALLBACK(_gui_main_window_close_request), &core);
	#else
    g_signal_connect(core->main_window, "close-request", G_CALLBACK(_gui_main_window_close_request), core);
	#endif

	if (gui_main_window_callback != NULL)
	{
		struct gui_event e = {0};
		e.type = GE_BEFORE_PRESENT;
		logging_log_message("main window design phase begin.", true);
		gui_main_window_callback(core, &e);
		logging_log_message("main window design phase end.", true);
	}

	#ifdef USE_GTK3
	gtk_widget_show_all(core.main_window);
	#endif
    gtk_window_present(GTK_WINDOW(core->main_window));

	if (gui_main_window_callback != NULL)
	{
		struct gui_event e = {0};
		e.type = GE_AFTER_PRESENT;
		logging_log_message("main window initializing phase begin.", true);
		gui_main_window_callback(core, &e);
		logging_log_message("main window initializing phase end.", true);
	}
	_gui_add_widget_to_internal_list(main_window);
	return main_window;
}

GMenu* gui_main_window_create_sub_menu(GMenu* menu_bar, const char* sub_menu_name)
{
    GMenu* sub_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, sub_menu_name, G_MENU_MODEL(sub_menu));
    return sub_menu;
}

void gui_main_window_add_sub_menu_item(GMenu* sub_menu, const char* item_name, const char* action, gui_main_window_t core)
{
	string_t action_name = {0};
	snprintf(action_name, sizeof(string_t), "app.%s", action);

    GMenuItem* menu_item_exit = g_menu_item_new(item_name, action_name);
    g_menu_append_item(sub_menu, menu_item_exit);

	_gui_main_window_add_action(core->app, action, core);
}

/*
    ad->pipeline = cam_tis_create_pipeline_with_uri("playbin",
        "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm", ad);
    if (ad->pipeline != NULL)
    {
        logging_log_message("pipeline created.", true);

        #if USE_GTK3
        GstElement* videosink = gst_element_factory_make("glsinkbin", "glsinkbin");
        GstElement* gtkglsink = gst_element_factory_make("gtkglsink", "gtkglsink");
        if ((videosink != NULL) && (gtkglsink != NULL))
        {
            g_object_set(videosink, "sink", gtkglsink, NULL);
            g_object_get(gtkglsink, "widget", &ad->video_sink_widget, NULL);
            g_object_set(ad->pipeline, "video-sink", videosink, NULL);
            gtk_container_add(GTK_CONTAINER(data->main_window), ad->video_sink_widget);
            logging_log_message("video sink widget created and added.", true);
        }
        #endif
    }

	GstClockTime timestamp = GST_BUFFER_PTS(buffer);

	g_print("Captured frame %d, Pixel Value=%03d Timestamp=%" GST_TIME_FORMAT "            \n",
			framecount, pixel_data, GST_TIME_ARGS(timestamp));

*/
