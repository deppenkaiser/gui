#include "application.h"
#include "gui.h"

#include <logging/logging.h>

extern void gui_application_callback(gui_event_type_t event, gui_application_t core) __attribute__((weak));

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

static void _gui_application_startup(GApplication* app, gpointer user_data)
{
	if (gui_application_callback != NULL)
	{
		logging_log_message("application startup begin.", true);
		gui_application_callback(GE_A_STARTUP, (gui_application_t) user_data);
		logging_log_message("application startup end.", true);
	}
}

static void _gui_application_activate(GApplication* app, gpointer user_data)
{
	if (gui_application_callback != NULL)
	{
		logging_log_message("application activation begin.", true);
		gui_application_callback(GE_A_ACTIVATE, (gui_application_t) user_data);
		logging_log_message("application activation end.", true);
	}
}

static void _gui_application_shutdown(GApplication* app, gpointer user_data)
{
	if (gui_application_callback != NULL)
	{
		logging_log_message("application shutdown begin.", true);
		gui_application_callback(GE_A_SHUTDOWN, (gui_application_t) user_data);
		logging_log_message("application shutdown end.", true);
	}
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

int32_t gui_application_run(const char* name, int argc, char **argv, void* user_data)
{
    logging_log_message("Hello World!", true);
	static struct gui_application core = {0};
	#ifdef USE_GTK3
	gtk_init(&argc, &argv);
	#else
	gtk_init();
	#endif
	core.app = gtk_application_new(name, G_APPLICATION_DEFAULT_FLAGS);
	core.user_data = user_data;
    g_signal_connect(core.app, "activate", G_CALLBACK(_gui_application_activate), &core);
    g_signal_connect(core.app, "startup", G_CALLBACK(_gui_application_startup), &core);
    g_signal_connect(core.app, "shutdown", G_CALLBACK(_gui_application_shutdown), &core);
    int status = g_application_run(G_APPLICATION(core.app), argc, argv);
    g_object_unref(core.app);
    logging_log_message("Good bye World!", true);
	return status;
}
