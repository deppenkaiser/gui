#include "button.h"

extern void button_callback(button_t data) __attribute__((weak));

button_t _button_get_core(GtkButton* button);

void _button_clicked(GtkButton* self, gpointer user_data)
{
    button_t data = (button_t) user_data;
    if (button_callback != NULL)
    {
        button_callback(data);
    }
}

GtkWidget* button_create(uint32_t id, void* user_data)
{
    GtkWidget* button = gtk_button_new();
	g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _button)));
    button_t core = _button_get_core(GTK_BUTTON(button));
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, "clicked", G_CALLBACK(_button_clicked), core);
    return button;
}

GtkWidget* button_with_label_create(uint32_t id, const char* label, void* user_data)
{
    GtkWidget* button = gtk_button_new_with_label(label);
	g_object_set_data(G_OBJECT(button), "core", malloc(sizeof(struct _button)));
    button_t core = _button_get_core(GTK_BUTTON(button));
    core->button = button;
    core->id = id;
    core->user_data = user_data;
    g_signal_connect(button, "clicked", G_CALLBACK(_button_clicked), core);
    return button;
}

void button_destroy(GtkButton* button)
{
	free(_button_get_core(button));
	g_object_set_data(G_OBJECT(button), "core", NULL);
	g_object_unref(G_OBJECT(button));
}

button_t _button_get_core(GtkButton* button)
{
	return (button_t) g_object_get_data(G_OBJECT(button), "core");
}
