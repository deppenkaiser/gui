#include "vulkan.h"
#include "events.h"

#include <api/api.h>
#include <gtk/gtk.h>
#include <logging/logging.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <vulkan/vulkan_xcb.h>

// === Protected Imports ===
protected_import(void, _gui_add_widget_to_internal_list(GtkWidget* widget));
protected_import(void*, _gui_get_core(GtkWidget* widget));

// === Externe Deklarationen für GTK4-X11-Funktionen ===
extern Display* gdk_x11_display_get_xdisplay(GdkDisplay* display);
extern Window gdk_x11_surface_get_xid(GdkSurface* surface);

// === Crash Handler ===
static void _gui_vulkan_signal_handler(int sig)
{
    void *array[50];
    int size;
    
    logging_log_formatted("SIGNAL %d received!", sig);
    
    size = backtrace(array, 50);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    _exit(1);
}

// === Callback-Deklaration ===
callback_declaration(void, gui_vulkan(gui_vulkan_t core, gui_event_t e));

// === Hilfsfunktionen ===
static gui_vulkan_t _gui_vulkan_get_core(GtkWidget* widget)
{
    if (!widget)
    {
        return NULL;
    }
    return (gui_vulkan_t) g_object_get_data(G_OBJECT(widget), "core");
}

static void _gui_vulkan_set_core(GtkWidget* widget, gui_vulkan_t core)
{
    if (!widget)
    {
        return;
    }
    g_object_set_data(G_OBJECT(widget), "core", core);
}

// === X11-Hilfsfunktionen ===
static Display* _gui_get_x11_display(void)
{
    GdkDisplay* display = gdk_display_get_default();
    if (!display)
    {
        logging_log_message("_gui_get_x11_display: failed to get default display");
        return NULL;
    }
    return gdk_x11_display_get_xdisplay(display);
}

static Window _gui_get_x11_window(GtkWidget* widget)
{
    if (!widget)
    {
        logging_log_message("_gui_get_x11_window: widget is NULL");
        return 0;
    }

    GtkNative* native = gtk_widget_get_native(widget);
    if (!native)
    {
        logging_log_formatted("_gui_get_x11_window: widget %p has no native", widget);
        return 0;
    }

    GdkSurface* surface = gtk_native_get_surface(native);
    if (!surface)
    {
        logging_log_formatted("_gui_get_x11_window: native has no surface for widget %p", widget);
        return 0;
    }

    return gdk_x11_surface_get_xid(surface);
}

// === Realize ===
static void _gui_vulkan_realize_callback(GtkWidget* widget, gpointer user_data)
{
    logging_log_formatted("_gui_vulkan_realize_callback: widget %p", widget);

    gui_vulkan_t core = _gui_vulkan_get_core(widget);
    if (!core)
    {
        logging_log_message("_gui_vulkan_realize_callback: core is NULL");
        return;
    }

    logging_log_formatted("_gui_vulkan_realize_callback: instance = %p", core->instance);

    if (core->instance == VK_NULL_HANDLE)
    {
        logging_log_message("_gui_vulkan_realize_callback: ERROR - VkInstance is NULL");
        return;
    }

    Display* display = _gui_get_x11_display();
    Window window = _gui_get_x11_window(widget);

    if (!display)
    {
        logging_log_message("_gui_vulkan_realize_callback: failed to get X11 display");
        return;
    }
    if (window == 0)
    {
        logging_log_message("_gui_vulkan_realize_callback: failed to get X11 window");
        return;
    }

    logging_log_formatted("_gui_vulkan_realize_callback: display=%p, window=%lu", (void*)display, window);

    xcb_connection_t* connection = XGetXCBConnection(display);
    if (!connection)
    {
        logging_log_message("_gui_vulkan_realize_callback: failed to get XCB connection");
        return;
    }

    VkXcbSurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .connection = connection,
        .window = window
    };

    VkResult result = vkCreateXcbSurfaceKHR(core->instance, &createInfo, NULL, &core->surface);
    if (result != VK_SUCCESS)
    {
        logging_log_formatted("_gui_vulkan_realize_callback: failed to create XCB surface: %d", result);
        return;
    }

    logging_log_formatted("_gui_vulkan_realize_callback: Vulkan XCB surface created: %p", core->surface);
    core->initialized = true;

    if (gui_vulkan != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_VULKAN_REALIZE;
        e.data.vulkan_realize.vulkan_area = widget;
        e.data.vulkan_realize.surface = core->surface;
        gui_vulkan(core, &e);
        logging_log_message("_gui_vulkan_realize_callback: GE_VULKAN_REALIZE callback invoked");
    }
}

// === Unrealize ===
static void _gui_vulkan_unrealize_callback(GtkWidget* widget, gpointer user_data)
{
    logging_log_formatted("_gui_vulkan_unrealize_callback: widget %p", widget);

    gui_vulkan_t core = _gui_vulkan_get_core(widget);
    if (!core)
    {
        logging_log_message("_gui_vulkan_unrealize_callback: core is NULL");
        return;
    }

    if (core->surface != VK_NULL_HANDLE)
    {
        logging_log_formatted("_gui_vulkan_unrealize_callback: destroying surface %p", core->surface);
        vkDestroySurfaceKHR(core->instance, core->surface, NULL);
        core->surface = VK_NULL_HANDLE;
    }
    core->initialized = false;

    if (gui_vulkan != NULL)
    {
        struct gui_event e = {0};
        e.type = GE_VULKAN_UNREALIZE;
        e.data.vulkan_unrealize.vulkan_area = widget;
        gui_vulkan(core, &e);
        logging_log_message("_gui_vulkan_unrealize_callback: GE_VULKAN_UNREALIZE callback invoked");
    }
}

// === Render ===
static void _gui_vulkan_render_callback(
    GtkDrawingArea* drawing_area,
    cairo_t* cr,
    int width,
    int height,
    gpointer user_data)
{
    return;
}

// === Öffentliche Funktionen ===

GtkWidget* gui_vulkan_create(VkInstance instance, void* user_data)
{
    static bool handler_installed = false;
    if (!handler_installed)
    {
        signal(SIGSEGV, _gui_vulkan_signal_handler);
        signal(SIGABRT, _gui_vulkan_signal_handler);
        handler_installed = true;
    }

    logging_log_formatted("gui_vulkan_create: instance=%p, user_data=%p", instance, user_data);

    GtkWidget* drawing_area = gtk_drawing_area_new();
    if (!drawing_area)
    {
        logging_log_message("gui_vulkan_create: failed to create drawing area");
        return NULL;
    }

    gui_vulkan_t core = (gui_vulkan_t) g_malloc(sizeof(struct _gui_vulkan));
    if (!core)
    {
        logging_log_message("gui_vulkan_create: failed to allocate Vulkan core structure");
        return NULL;
    }

    memset(core, 0, sizeof(struct _gui_vulkan));

    core->vulkan_area = drawing_area;
    core->user_data = user_data;
    core->instance = instance;
    core->surface = VK_NULL_HANDLE;
    core->initialized = false;
    core->width = 0;
    core->height = 0;

    _gui_vulkan_set_core(drawing_area, core);

    g_signal_connect(drawing_area, "realize", G_CALLBACK(_gui_vulkan_realize_callback), NULL);
    // g_signal_connect(drawing_area, "unrealize", G_CALLBACK(_gui_vulkan_unrealize_callback), NULL);
    // gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), _gui_vulkan_render_callback, NULL, NULL);

    _gui_add_widget_to_internal_list(drawing_area);

    logging_log_formatted("gui_vulkan_create: SUCCESS - widget=%p, core=%p", drawing_area, core);
    return drawing_area;
}

void gui_vulkan_set_instance(GtkWidget* vulkan_widget, VkInstance instance)
{
    logging_log_formatted("gui_vulkan_set_instance: widget=%p, instance=%p", vulkan_widget, instance);

    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        logging_log_message("gui_vulkan_set_instance: core is NULL");
        return;
    }
    if (instance == VK_NULL_HANDLE)
    {
        logging_log_message("gui_vulkan_set_instance: instance is VK_NULL_HANDLE");
        return;
    }
    core->instance = instance;
    logging_log_formatted("gui_vulkan_set_instance: instance set successfully");
}

VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        return VK_NULL_HANDLE;
    }
    return core->surface;
}

VkInstance gui_vulkan_get_instance(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        return VK_NULL_HANDLE;
    }
    return core->instance;
}

GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        return NULL;
    }
    return core->vulkan_area;
}

bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        return false;
    }
    return core->initialized;
}

void gui_vulkan_queue_render(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        logging_log_message("gui_vulkan_queue_render: core is NULL");
        return;
    }

    gtk_widget_queue_draw(vulkan_widget);
    logging_log_formatted("gui_vulkan_queue_render: render queued for widget %p", vulkan_widget);
}

void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (!core)
    {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (width) *width = core->width;
    if (height) *height = core->height;
}
