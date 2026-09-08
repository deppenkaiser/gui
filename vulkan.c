#include "vulkan.h"
#include "events.h"

#include <api/api.h>
#include <gdk/gdk.h>
#include <logging/logging.h>
#include <stdlib.h>

// === Protected Imports (interne Funktionen aus gui.c) ===
protected_import(void, _gui_add_widget_to_internal_list(GtkWidget* widget));
protected_import(void*, _gui_get_core(GtkWidget* widget));

// === Interne Struktur ===
struct _gui_vulkan_core
{
    GtkWidget* vulkan_area;
    VkSurfaceKHR surface;
    void* user_data;
    int width;
    int height;
    bool initialized;
    bool render_pending;
};

// === Callback-Deklaration (weak, überschreibbar) ===
callback_declaration(void, gui_vulkan(gui_vulkan_t core, gui_event_t e));

// === Hilfsfunktionen ===
static struct _gui_vulkan_core* _gui_vulkan_get_core(GtkWidget* widget)
{
    return (struct _gui_vulkan_core*) g_object_get_data(G_OBJECT(widget), "vulkan_core");
}

static void _gui_vulkan_set_core(GtkWidget* widget, struct _gui_vulkan_core* core)
{
    g_object_set_data(G_OBJECT(widget), "vulkan_core", core);
}

// === Realize ===
static void _gui_vulkan_realize_callback(GtkWidget* widget, gpointer user_data)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(widget);
    if (!core) return;

    GdkSurface* gdk_surface = gtk_native_get_surface(gtk_widget_get_native(widget));
    if (!gdk_surface)
    {
        logging_log_message("Failed to get GdkSurface for Vulkan widget");
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
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(widget);
    if (!core) return;

    if (core->surface != VK_NULL_HANDLE)
    {
        core->surface = VK_NULL_HANDLE;
    }
    
    core->initialized = false;
}

// === Größenänderung (size-allocate) ===
static void _gui_vulkan_size_allocate_callback(GtkWidget* widget, GtkAllocation* allocation, gpointer user_data)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(widget);
    if (!core) return;

    int old_width = core->width;
    int old_height = core->height;
    
    core->width = allocation->width;
    core->height = allocation->height;
    
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
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(GTK_WIDGET(drawing_area));
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

GtkWidget* gui_vulkan_create(void* user_data)
{
    GtkWidget* drawing_area = gtk_drawing_area_new();
    
    struct _gui_vulkan_core* core = calloc(1, sizeof(struct _gui_vulkan_core));
    if (!core)
    {
        logging_log_message("Failed to allocate Vulkan core structure");
        return NULL;
    }
    
    core->vulkan_area = drawing_area;
    core->user_data = user_data;
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
    g_signal_connect(drawing_area, "size-allocate", G_CALLBACK(_gui_vulkan_size_allocate_callback), NULL);
    
    _gui_add_widget_to_internal_list(drawing_area);
    
    return drawing_area;
}

VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return VK_NULL_HANDLE;
    return core->surface;
}

GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return NULL;
    return core->vulkan_area;
}

bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return false;
    return core->initialized;
}

void gui_vulkan_queue_render(GtkWidget* vulkan_widget)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) return;
    
    core->render_pending = true;
    gtk_widget_queue_draw(vulkan_widget);
}

void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height)
{
    struct _gui_vulkan_core* core = _gui_vulkan_get_core(vulkan_widget);
    if (!core) 
    {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    
    if (width) *width = core->width;
    if (height) *height = core->height;
}