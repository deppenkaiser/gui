#pragma once

#include <gtk-4.0/gtk/gtk.h>

typedef enum application_events
{
	AE_STARTUP,
	AE_ACTIVATE,
	AE_SHUTDOWN
} application_events_t;

typedef struct _application
{
	GtkApplication* app;
	void* user_data;
} *application_t;

int32_t application_run(const char* name, int argc, char **argv, void* user_data);
