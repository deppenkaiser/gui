#include "gui_main_window.h"

#include <string/string.h>
#include <logging/logging.h>

extern void gui_main_window_callback(mwe_types_t type, gui_main_window_t data, main_window_event_t e) __attribute__((weak));
extern void gui_main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, gui_main_window_t data) __attribute__((weak));

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

GMenu* _gui_main_window_create_menu_bar(GtkApplication* app, GtkApplicationWindow* window);
void _gui_main_window_add_action(GtkApplication* app, const char* action_name, gui_main_window_t data);

gboolean _gui_main_window_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
    gboolean handled = FALSE;
	if (gui_main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		e.key_pressed.keyval = keyval;
		logging_log_message("key pressed event begin...", true);
		gui_main_window_callback(MWE_KEY_PRESSED, (gui_main_window_t) user_data, &e);
		logging_log_message("key pressed event end...", true);
		handled = e.key_pressed.handled;
	}
    return handled;
}

void _gui_main_window_key_released(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
	if (gui_main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		e.key_released.keyval = keyval;
		logging_log_message("key released event begin...", true);
		gui_main_window_callback(MWE_KEY_RELEASED, (gui_main_window_t) user_data, &e);
		logging_log_message("key released event end...", true);
	}
}

gboolean _gui_main_window_close_request(GtkWindow* self, gpointer user_data)
{
	gboolean close = FALSE;
	if (gui_main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		gui_main_window_callback(MWE_CLOSE_REQUEST, (gui_main_window_t) user_data, &e);
		close = e.close_request.close;
	}
    return close;
}

void _gui_main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, gpointer user_data)
{
	if (gui_main_window_action_callback != NULL)
	{
		logging_log_message("action event begin...", true);
		gui_main_window_action_callback(simple_action, parameter, (gui_main_window_t) user_data);
		logging_log_message("action event end...", true);
	}
}

void _gui_main_window_add_action(GtkApplication* app, const char* action_name, gui_main_window_t data)
{
    GSimpleAction* action = g_simple_action_new(action_name, NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(_gui_main_window_action_callback), data);
}

GMenu* _gui_main_window_create_menu_bar(GtkApplication* app, GtkApplicationWindow* window)
{
    GMenu* menu_bar = g_menu_new();
    gtk_application_set_menubar(app, G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(window, TRUE);
    return menu_bar;
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

void gui_main_window_create(GtkApplication* app, void* user_data, bool show_menu)
{
	static struct gui_main_window core = {0};
	core.user_data = user_data;
	core.app = app;
	core.main_window = gtk_application_window_new(app);
	core.keyboard_controller = gtk_event_controller_key_new(core.main_window);
	core.menu_bar = _gui_main_window_create_menu_bar(app, GTK_APPLICATION_WINDOW(core.main_window));
    core.file_menu = gui_main_window_create_sub_menu(core.menu_bar, "File");
	
	gui_main_window_add_sub_menu_item(core.file_menu, "Exit", "exit", &core);
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(core.main_window), show_menu ? TRUE : FALSE);

    //gtk_widget_add_controller(core.main_window, core.keyboard_controller);
    g_signal_connect(core.keyboard_controller, "key-pressed", G_CALLBACK(_gui_main_window_key_pressed), &core);
    g_signal_connect(core.keyboard_controller, "key-released", G_CALLBACK(_gui_main_window_key_released), &core);
	//Gtk4 only
    //g_signal_connect(core.main_window, "close-request", G_CALLBACK(_gui_main_window_close_request), &core);

	if (gui_main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		logging_log_message("main window design phase begin.", true);
		gui_main_window_callback(MWE_BEFORE_PRESENT, &core, &e);
		logging_log_message("main window design phase end.", true);
	}

	gtk_widget_show_all(core.main_window);
    gtk_window_present(GTK_WINDOW(core.main_window));

	if (gui_main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		logging_log_message("main window initializing phase begin.", true);
		gui_main_window_callback(MWE_AFTER_PRESENT, &core, &e);
		logging_log_message("main window initializing phase end.", true);
	}
}

GMenu* gui_main_window_create_sub_menu(GMenu* menu_bar, const char* sub_menu_name)
{
    GMenu* sub_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, sub_menu_name, G_MENU_MODEL(sub_menu));
    return sub_menu;
}

void gui_main_window_add_sub_menu_item(GMenu* sub_menu, const char* item_name, const char* action, gui_main_window_t data)
{
	string_t action_name = {0};
	snprintf(action_name, sizeof(string_t), "app.%s", action);

    GMenuItem* menu_item_exit = g_menu_item_new(item_name, action_name);
    g_menu_append_item(sub_menu, menu_item_exit);

	_gui_main_window_add_action(data->app, action, data);
}
