#pragma once

#include <gtk/gtk.h>

typedef struct gui_application
{
	GtkApplication* app;
	void* user_data;
} *gui_application_t;

int32_t gui_application_run(const char* name, int argc, char **argv, void* user_data);
