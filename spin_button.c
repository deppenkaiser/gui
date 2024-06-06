#include "spin_button.h"
#include "gui.h"

extern void gui_spin_button_callback(gui_spin_button_t data) __attribute__((weak));

void _gui_spin_button_value_changed(GtkSpinButton* self, gpointer user_data)
{
    gui_spin_button_t data = (gui_spin_button_t) user_data;

    if (gui_spin_button_callback != NULL)
    {
        gui_spin_button_callback(data);
    }
}

double gui_spin_button_get_double(GtkWidget* spin_button)
{
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button));
}

GtkWidget* gui_spin_button_create(uint32_t id, gui_spin_button_configuration_t configuration, void* user_data)
{
    GtkAdjustment* adjustment = gtk_adjustment_new(configuration->value, configuration->min, configuration->max,
        configuration->increment, 0.0, 0.0);
    GtkWidget* button = gtk_spin_button_new(adjustment, 1.0, configuration->digits);
	g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _gui_spin_button)));
    gui_spin_button_t core = gui_get_core(button);
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, "value-changed", G_CALLBACK(_gui_spin_button_value_changed), core);
    return button;
}
