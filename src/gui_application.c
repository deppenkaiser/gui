#include "gui_application.h"

#include <logging/logging.h>

extern void application_callback(application_events_t event, application_t data) __attribute__((weak));

void _gui_application_startup(GApplication* app, gpointer user_data)
{
	if (application_callback != NULL)
	{
		application_callback(AE_STARTUP, (application_t) user_data);
	}
}

void _gui_application_activate(GApplication* app, gpointer user_data)
{
	if (application_callback != NULL)
	{
		application_callback(AE_ACTIVATE, (application_t) user_data);
	}
}

void _gui_application_shutdown(GApplication* app, gpointer user_data)
{
	if (application_callback != NULL)
	{
		application_callback(AE_SHUTDOWN, (application_t) user_data);
	}
}

int32_t gui_application_run(const char* name, int argc, char **argv, void* user_data)
{
	static struct _application core;
	core.app = gtk_application_new(name, G_APPLICATION_FLAGS_NONE);
	core.user_data = user_data;
    g_signal_connect(core.app, "activate", G_CALLBACK(_gui_application_activate), &core);
    g_signal_connect(core.app, "startup", G_CALLBACK(_gui_application_startup), &core);
    g_signal_connect(core.app, "shutdown", G_CALLBACK(_gui_application_shutdown), &core);
    int status = g_application_run(G_APPLICATION(core.app), argc, argv);
    g_object_unref(core.app);
    logging_log_message("Good bye!", true);
	return status;
}
