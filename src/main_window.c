#include "gui/main_window.h"

#include <string/string.h>

extern void main_window_callback(mwe_types_t type, main_window_t data, main_window_event_t e) __attribute__((weak));
extern void main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, main_window_t data) __attribute__((weak));

GMenu* _main_window_create_menu_bar(GtkApplication* app, GtkApplicationWindow* window);
void _main_window_add_action(GtkApplication* app, const char* action_name, main_window_t data);

gboolean _main_window_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
    gboolean handled = FALSE;
	if (main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		e.key_pressed.keyval = keyval;
		main_window_callback(MWE_KEY_PRESSED, (main_window_t) user_data, &e);
		handled = e.key_pressed.handled;
	}
    return handled;
}

void _main_window_key_released(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
	if (main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		e.key_released.keyval = keyval;
		main_window_callback(MWE_KEY_RELEASED, (main_window_t) user_data, &e);
	}
}

gboolean _main_window_close_request(GtkWindow* self, gpointer user_data)
{
	gboolean close = FALSE;
	if (main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		main_window_callback(MWE_CLOSE_REQUEST, (main_window_t) user_data, &e);
		close = e.close_request.close;
	}
    return close;
}

void _main_window_action_callback(GSimpleAction* simple_action, GVariant* parameter, gpointer user_data)
{
	if (main_window_action_callback != NULL)
	{
		main_window_action_callback(simple_action, parameter, (main_window_t) user_data);
	}
}

void main_window_create(GtkApplication* app, void* user_data)
{
	static struct _main_window core = {0};
	core.user_data = user_data;
	core.app = app;
	core.main_window = gtk_application_window_new(app);
	core.keyboard_controller = gtk_event_controller_key_new();
	core.menu_bar = _main_window_create_menu_bar(app, GTK_APPLICATION_WINDOW(core.main_window));
    core.file_menu = main_window_create_sub_menu(core.menu_bar, "File");
	
	main_window_add_sub_menu_item(core.file_menu, "Exit", "exit", &core);

    gtk_widget_add_controller(core.main_window, core.keyboard_controller);
    g_signal_connect(core.keyboard_controller, "key-pressed", G_CALLBACK(_main_window_key_pressed), &core);
    g_signal_connect(core.keyboard_controller, "key-released", G_CALLBACK(_main_window_key_released), &core);
    g_signal_connect(core.main_window, "close-request", G_CALLBACK(_main_window_close_request), &core);

	if (main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		main_window_callback(MWE_BEFORE_PRESENT, &core, &e);
	}

    gtk_window_present(GTK_WINDOW(core.main_window));

	if (main_window_callback != NULL)
	{
		union _main_window_event e = {0};
		main_window_callback(MWE_AFTER_PRESENT, &core, &e);
	}
}

GMenu* main_window_create_sub_menu(GMenu* menu_bar, const char* sub_menu_name)
{
    GMenu* sub_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, sub_menu_name, G_MENU_MODEL(sub_menu));
    return sub_menu;
}

void main_window_add_sub_menu_item(GMenu* sub_menu, const char* item_name, const char* action, main_window_t data)
{
	string_t action_name = {0};
	snprintf(action_name, sizeof(string_t), "app.%s", action);

    GMenuItem* menu_item_exit = g_menu_item_new(item_name, action_name);
    g_menu_append_item(sub_menu, menu_item_exit);

	_main_window_add_action(data->app, action, data);
}

void _main_window_add_action(GtkApplication* app, const char* action_name, main_window_t data)
{
    GSimpleAction* action = g_simple_action_new(action_name, NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(_main_window_action_callback), data);
}

GMenu* _main_window_create_menu_bar(GtkApplication* app, GtkApplicationWindow* window)
{
    GMenu* menu_bar = g_menu_new();
    gtk_application_set_menubar(app, G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(window, TRUE);
    return menu_bar;
}
