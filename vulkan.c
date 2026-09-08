#include "vulkan.h"
#include "events.h"

#include <api/api.h>
#include <gdk/gdk.h>
#include <logging/logging.h>
#include <stdlib.h>

// === Protected Imports ===
protected_import(void, _gui_add_widget_to_internal_list(GtkWidget* widget));
protected_import(void*, _gui_get_core(GtkWidget* widget));

// === Interne Struktur ===
typedef struct _gui_vulkan_core
{
    GtkWidget* vulkan_area;
    VkSurfaceKHR surface;
    VkInstance instance;      // <-- NEU
    void* user_data;
    int width;
    int height;
    bool initialized;
    bool render_pending;
} *_gui_vulkan_core_t;

// === Callback-Deklaration ===
callback_declaration(void, gui_vulkan(gui_vulkan_t core, gui_event_t e));

// === Hilfsfunktionen ===
static _gui_vulkan_core_t _gui_vulkan_get_core(GtkWidget* widget)
{
    return (_gui_vulkan_core_t) g_object_get_data(G_OBJECT(widget), "vulkan_core");
}

static void _gui_vulkan_set_core(GtkWidget* widget, _gui_vulkan_core_t core)
{
    g_object_set_data(G_OBJECT(widget), "vulkan_core", core);
}

// === Realize ===
static void _gui_vulkan_realize_callback(GtkWidget* widget, gpointer user_data)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(widget);
    if (!core) return;

    GdkSurface* gdk_surface = gtk_native_get_surface(gtk_widget_get_native(widget));
    if (!gdk_surface)
    {
        logging_log_message("Failed to get GdkSurface for Vulkan widget");
        return;
    }

    // GTK4: Vulkan-Surface mit der gespeicherten Instance erstellen
    if (core->instance != VK_NULL_HANDLE)
    {
        GError* error = NULL;
        if (!gdk_vulkan_surface_create(gdk_surface, core->instance, NULL, &core->surface, &error))
        {
            logging_log_formatted("Failed to create Vulkan surface: %s", error->message);
            g_error_free(error);
            return;
        }
        logging_log_message("Vulkan surface created successfully");
    }
    else
    {
        logging_log_message("No VkInstance available for surface creation");
        return;
    }
    
    core->initialized = true;
    
    if (gui_vulkan != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_VULKAN_REALIZE;
        e.data.vulkan_realize.vulkan_area = widget;
        e.data.vulkan_realize.surface = core->surface;
        gui_vulkan((gui_vulkan_t) core, &e);
    }
}

// === Unrealize ===
static void _gui_vulkan_unrealize_callback(GtkWidget* widget, gpointer user_data)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(widget);
    if (!core) return;

    if (core->surface != VK_NULL_HANDLE)
    {
        // Surface wird von der App zerstört
        core->surface = VK_NULL_HANDLE;
    }
    
    core->initialized = false;
}

// === Resize ===
static void _gui_vulkan_resize_callback(GtkWidget* widget, gpointer user_data)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(widget);
    if (!core) return;

    int old_width = core->width;
    int old_height = core->height;
    
    core->width = gtk_widget_get_width(widget);
    core->height = gtk_widget_get_height(widget);
    
    if (gui_vulkan != NULL && (core->width != old_width || core->height != old_height))
    {
        struct gui_event e = {0};
        e.type = GE_VULKAN_RESIZE;
        e.data.vulkan_resize.vulkan_area = widget;
        e.data.vulkan_resize.surface = core->surface;
        e.data.vulkan_resize.width = core->width;
        e.data.vulkan_resize.height = core->height;
        e.data.vulkan_resize.old_width = old_width;
        e.data.vulkan_resize.old_height = old_height;
        gui_vulkan((gui_vulkan_t) core, &e);
    }
}

// === Render ===
static void _gui_vulkan_render_callback(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(GTK_WIDGET(drawing_area));
    if (!core) return;

    if (!core->render_pending)
    {
        return;
    }
    
    core->render_pending = false;
    
    if (gui_vulkan != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_VULKAN_RENDER;
        e.data.vulkan_render.vulkan_area = GTK_WIDGET(drawing_area);
        e.data.vulkan_render.surface = core->surface;
        e.data.vulkan_render.width = width;
        e.data.vulkan_render.height = height;
        gui_vulkan((gui_vulkan_t) core, &e);
    }
}

// === Öffentliche Funktionen ===

GtkWidget* gui_vulkan_create(VkInstance instance, void* user_data)
{
    GtkWidget* drawing_area = gtk_drawing_area_new();
    
    _gui_vulkan_core_t core = calloc(1, sizeof(struct _gui_vulkan_core));
    if (!core)
    {
        logging_log_message("Failed to allocate Vulkan core structure");
        return NULL;
    }
    
    core->vulkan_area = drawing_area;
    core->user_data = user_data;
    core->instance = instance;   // <-- NEU: Instance speichern
    core->surface = VK_NULL_HANDLE;
    core->initialized = false;
    core->render_pending = false;
    core->width = 0;
    core->height = 0;
    
    _gui_vulkan_set_core(drawing_area, core);
    
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area),
        _gui_vulkan_render_callback, NULL, NULL);
    
    g_signal_connect(drawing_area, "realize", G_CALLBACK(_gui_vulkan_realize_callback), NULL);
    g_signal_connect(drawing_area, "unrealize", G_CALLBACK(_gui_vulkan_unrealize_callback), NULL);
    g_signal_connect(drawing_area, "resize", G_CALLBACK(_gui_vulkan_resize_callback), drawing_area);
    
    _gui_add_widget_to_internal_list(drawing_area);
    
    return drawing_area;
}

VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return VK_NULL_HANDLE;
    return core->surface;
}

VkInstance gui_vulkan_get_instance(GtkWidget* vulkan_widget)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return VK_NULL_HANDLE;
    return core->instance;
}

GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return NULL;
    return core->vulkan_area;
}

bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return false;
    return core->initialized;
}

void gui_vulkan_queue_render(GtkWidget* vulkan_widget)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return;
    
    core->render_pending = true;
    gtk_widget_queue_draw(vulkan_widget);
}

void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height)
{
    _gui_vulkan_core_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) 
    {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    
    if (width) *width = core->width;
    if (height) *height = core->height;
}