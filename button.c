#include "button.h"
#include "events.h"

#include <api/api.h>

callback_declaration(void, gui_button(gui_button_t core, gui_event_t e));

extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
extern void* _gui_get_core(GtkWidget* widget);

private void _gui_button_clicked(GtkButton* self, gpointer user_data)
{
    gui_button_t core = (gui_button_t) user_data;
    if (gui_button != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_B_CLICKED;
        gui_button(core, &e);
    }
}

private void _gui_button_toggled(GtkToggleButton* self, gpointer user_data)
{
    gui_button_t core = (gui_button_t) user_data;
    if (gui_button != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_B_TOGGLED;
        e.data.b_toggled.button = self;
        e.data.b_toggled.active = gtk_toggle_button_get_active(self);
        gui_button(core, &e);
    }
}

private void _gui_button_spin_value_changed(GtkSpinButton* self, gpointer user_data)
{
    gui_button_t core = (gui_button_t) user_data;

    if (gui_button != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_B_SELECTED;
        gui_button(core, &e);
    }
}

private gboolean _gui_button_drop_down_callback(GtkEventControllerLegacy* self, GdkEvent* event, gpointer user_data)
{
    gboolean handled = FALSE;
    gui_button_t core = (gui_button_t) user_data;

	switch (gdk_event_get_event_type(event))
	{
		case GDK_GRAB_BROKEN:
            if (gui_button != NULL)
            {
                struct gui_event e = {0};
                gui_button(core, &e);
                handled = TRUE;
            }
			break;
	}

    return handled;
}

GtkWidget* gui_button_create(uint32_t id, gui_button_configuration_t configuration, void* user_data)
{
    GtkWidget* button = NULL;
    GCallback callback_function = NULL;

    if (configuration->toggle)
    {
        button = configuration->label != NULL ? gtk_toggle_button_new_with_label(configuration->label) : gtk_toggle_button_new();
        callback_function = G_CALLBACK(_gui_button_toggled);        
    }
    else
    {
        button = configuration->label != NULL ? gtk_button_new_with_label(configuration->label) : gtk_button_new();
        callback_function = G_CALLBACK(_gui_button_clicked);
    }

    g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _gui_button)));
    gui_button_t core = _gui_get_core(button);
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, configuration->toggle ? "toggled" : "clicked", callback_function, core);
    _gui_add_widget_to_internal_list(button);

    return button;
}

GtkWidget* gui_button_spin_create(uint32_t id, gui_spin_button_configuration_t configuration, void* user_data)
{
    GtkAdjustment* adjustment = gtk_adjustment_new(configuration->value, configuration->min, configuration->max,
        configuration->increment, 0.0, 0.0);
    GtkWidget* button = gtk_spin_button_new(adjustment, 1.0, configuration->digits);
    gtk_editable_set_alignment(GTK_EDITABLE(button), MIN(MAX(configuration->alignment, 0.0f), 1.0));
	g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _gui_button)));
    gui_button_t core = _gui_get_core(button);
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, "value-changed", G_CALLBACK(_gui_button_spin_value_changed), core);
    _gui_add_widget_to_internal_list(button);
    return button;
}

double gui_button_spin_get_double(GtkWidget* spin_button)
{
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button));
}

void gui_button_spin_set_double(GtkWidget* spin_button, double value)
{
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), value);
}

void gui_button_spin_set_configuration(GtkWidget* spin_button, gui_spin_button_configuration_t configuration)
{
    GtkAdjustment* adjustment = gtk_adjustment_new(configuration->value, configuration->min, configuration->max,
        configuration->increment, 0.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spin_button), adjustment);
}

GtkWidget* gui_button_drop_down_create(uint32_t id, const char* strings[], void* user_data)
{
    GtkWidget* button = gtk_drop_down_new_from_strings(strings);
    GtkEventController* legacy_controller = gtk_event_controller_legacy_new();
    gtk_widget_add_controller(button, legacy_controller);
	g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _gui_button)));
    gui_button_t core = _gui_get_core(button);
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(legacy_controller, "event", G_CALLBACK(_gui_button_drop_down_callback), core);
    _gui_add_widget_to_internal_list(button);
    return button;
}

int32_t gui_button_drop_down_get_selection(GtkWidget* drop_down_button)
{
    return gtk_drop_down_get_selected(GTK_DROP_DOWN(drop_down_button));
}
