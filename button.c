#include "button.h"
#include "gui.h"

extern void gui_button_callback(gui_button_t data) __attribute__((weak));

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

void _gui_button_clicked(GtkButton* self, gpointer user_data)
{
    gui_button_t data = (gui_button_t) user_data;
    if (gui_button_callback != NULL)
    {
        gui_button_callback(data);
    }
}

void _gui_button_toggled(GtkButton* self, gpointer user_data)
{
    gui_button_t data = (gui_button_t) user_data;
    if (gui_button_callback != NULL)
    {
        gui_button_callback(data);
    }
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/

GtkWidget* button_create(uint32_t id, gui_button_configuration_t configuration, void* user_data)
{
    GtkWidget* button = NULL;
    GCallback callback = NULL;

    if (configuration->toggle)
    {
        button = configuration->label != NULL ? gtk_toggle_button_new_with_label(configuration->label) : gtk_toggle_button_new();
        callback = G_CALLBACK(_gui_button_toggled);        
    }
    else
    {
        button = configuration->label != NULL ? gtk_button_new_with_label(configuration->label) : gtk_button_new();
        callback = G_CALLBACK(_gui_button_clicked);
    }

    g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _gui_button)));
    gui_button_t core = gui_get_core(button);
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, configuration->toggle ? "toggled" : "clicked", callback, core);

    return button;
}
