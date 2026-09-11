#include "application.h"
#include "events.h"

#include <api/api.h>
#include <logging/logging.h>

#define MODULE_ID "GUI"

callback_declaration(bool, gui_application(gui_event_type_t event, gui_application_t core));

private void _gui_application_startup(GApplication* app, gpointer user_data)
{
	if (gui_application != NULL)
	{
		LOG(MODULE_ID, "application startup begin.");
		if (gui_application(GE_A_STARTUP, (gui_application_t) user_data) == false)
		{
			exit(1);
		}
		LOG(MODULE_ID, "application startup end.");
	}
}

private void _gui_application_activate(GApplication* app, gpointer user_data)
{
	if (gui_application != NULL)
	{
		LOG(MODULE_ID, "application activation begin.");
		if (gui_application(GE_A_ACTIVATE, (gui_application_t) user_data) == false)
		{
			exit(1);
		}
		LOG(MODULE_ID, "application activation end.");
	}
}

private void _gui_application_shutdown(GApplication* app, gpointer user_data)
{
	if (gui_application != NULL)
	{
		LOG(MODULE_ID, "application shutdown begin.");
		if (gui_application(GE_A_SHUTDOWN, (gui_application_t) user_data) == false)
		{
			exit(1);
		}
		LOG(MODULE_ID, "application shutdown end.");
	}
}

int32_t gui_application_run(const char* name, int argc, char **argv, void* user_data)
{
    LOG(MODULE_ID, "Hello World!");
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
    LOG(MODULE_ID, "Good bye World!");
	return status;
}
