#pragma once

#include <gtk/gtk.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct _gui_vulkan
{
    GtkWidget* vulkan_area;
    VkSurfaceKHR surface;
    VkInstance instance;
    void* user_data;
    bool initialized;
    int width;
    int height;
    guint timer_id;
    bool need_render;
    bool need_close;
    bool need_resize;
} *gui_vulkan_t;

GtkWidget* gui_vulkan_create(VkInstance instance, void* user_data);
void gui_vulkan_set_instance(GtkWidget* vulkan_widget, VkInstance instance);
VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget);
VkInstance gui_vulkan_get_instance(GtkWidget* vulkan_widget);
GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget);
bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget);
void gui_vulkan_queue_render(GtkWidget* vulkan_widget);
void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height);
void _gui_vulkan_request_close(GtkWidget* vulkan_widget);