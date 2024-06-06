#pragma once

#include "application.h"
#include "main_window.h"
#include "drawing_area.h"
#include "frame.h"
#include "box.h"
#include "text.h"
#include "spin_button.h"
#include "events.h"

void gui_destroy_widget_core(GtkWidget* widget);
void* gui_get_core(GtkWidget* widget);
