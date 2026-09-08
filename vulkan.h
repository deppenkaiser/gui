#pragma once

#include <gtk/gtk.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct _gui_vulkan
{
    GtkWidget* vulkan_area;   // Das DrawingArea-Widget
    VkSurfaceKHR surface;     // Die Vulkan-Surface
    void* user_data;          // App-Daten
    bool initialized;         // Status
} *gui_vulkan_t;

// Widget erstellen
GtkWidget* gui_vulkan_create(void* user_data);

// Zugriff auf die Vulkan-Surface
VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget);

// Zugriff auf das DrawingArea (falls benötigt)
GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget);

// Status abfragen
bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget);

// Manuelles Rendern anfordern (weil "nur bei Bedarf")
void gui_vulkan_queue_render(GtkWidget* vulkan_widget);

// Auflösung abfragen
void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height);