#pragma once

#include <gtk/gtk.h>

typedef enum gui_application_events
{
	AE_STARTUP,
	AE_ACTIVATE,
	AE_SHUTDOWN
} gui_application_events_t;

typedef struct gui_application
{
	GtkApplication* app;
	void* user_data;
} *gui_application_t;

int32_t gui_application_run(const char* name, int argc, char **argv, void* user_data);
