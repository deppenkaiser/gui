#include "vulkan.h"
#include "events.h"

#include <api/api.h>
#include <gtk/gtk.h>
#include <logging/logging.h>
#define MODULE_ID "GUI"
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
    
    LOG(MODULE_ID, "SIGNAL %d received!", sig);
    
    size = backtrace(array, 50);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    _exit(1);
}

// === Callback-Deklaration ===
callback_declaration(void, gui_vulkan(gui_vulkan_t core, gui_event_t e));

// === Hilfsfunktionen ===
static gui_vulkan_t _gui_vulkan_get_core(GtkWidget* widget)
{
    gui_vulkan_t result = NULL;
    if (widget)
    {
        result = (gui_vulkan_t) g_object_get_data(G_OBJECT(widget), "core");
    }
    return result;
}

static void _gui_vulkan_set_core(GtkWidget* widget, gui_vulkan_t core)
{
    if (widget)
    {
        g_object_set_data(G_OBJECT(widget), "core", core);
    }
}

// === X11-Hilfsfunktionen ===
static Display* _gui_get_x11_display(void)
{
    Display* result = NULL;
    GdkDisplay* display = gdk_display_get_default();
    if (display)
    {
        result = gdk_x11_display_get_xdisplay(display);
    }
    else
    {
        LOG(MODULE_ID, "_gui_get_x11_display: failed to get default display");
    }
    return result;
}

static Window _gui_get_x11_window(GtkWidget* widget)
{
    Window result = 0;
    if (widget)
    {
        GtkNative* native = gtk_widget_get_native(widget);
        if (native)
        {
            GdkSurface* surface = gtk_native_get_surface(native);
            if (surface)
            {
                result = gdk_x11_surface_get_xid(surface);
            }
            else
            {
                LOG(MODULE_ID, "_gui_vulkan_get_x11_window: native has no surface for widget %p", widget);
            }
        }
        else
        {
            LOG(MODULE_ID, "_gui_vulkan_get_x11_window: widget %p has no native", widget);
        }
    }
    else
    {
        LOG(MODULE_ID, "_gui_vulkan_get_x11_window: widget is NULL");
    }
    return result;
}

// === Realize ===
static void _gui_vulkan_realize_callback(GtkWidget* widget, gpointer user_data)
{
    LOG(MODULE_ID, "_gui_vulkan_realize_callback: widget %p", widget);

    gui_vulkan_t core = _gui_vulkan_get_core(widget);
    if (core)
    {
        LOG(MODULE_ID, "_gui_vulkan_realize_callback: instance = %p", core->instance);

        if (core->instance != VK_NULL_HANDLE)
        {
            Display* display = _gui_get_x11_display();
            Window window = _gui_get_x11_window(widget);

            if (display && window != 0)
            {
                LOG(MODULE_ID, "_gui_vulkan_realize_callback: display=%p, window=%lu", (void*)display, window);

                xcb_connection_t* connection = XGetXCBConnection(display);
                if (connection)
                {
                    VkXcbSurfaceCreateInfoKHR createInfo = {
                        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
                        .connection = connection,
                        .window = window
                    };

                    VkResult result = vkCreateXcbSurfaceKHR(core->instance, &createInfo, NULL, &core->surface);
                    if (result == VK_SUCCESS)
                    {
                        LOG(MODULE_ID, "_gui_vulkan_realize_callback: Vulkan XCB surface created: %p", core->surface);
                        core->initialized = true;

                        if (gui_vulkan != NULL)
                        {
                            struct gui_event e = {0};
                            e.type = GE_VULKAN_REALIZE;
                            e.data.vulkan_realize.vulkan_area = widget;
                            e.data.vulkan_realize.surface = core->surface;
                            gui_vulkan(core, &e);
                            LOG(MODULE_ID, "_gui_vulkan_realize_callback: GE_VULKAN_REALIZE callback invoked");
                        }
                    }
                    else
                    {
                        LOG(MODULE_ID, "_gui_vulkan_realize_callback: failed to create XCB surface: %d", result);
                    }
                }
                else
                {
                    LOG(MODULE_ID, "_gui_vulkan_realize_callback: failed to get XCB connection");
                }
            }
            else
            {
                if (!display)
                {
                    LOG(MODULE_ID, "_gui_vulkan_realize_callback: failed to get X11 display");
                }
                if (window == 0)
                {
                    LOG(MODULE_ID, "_gui_vulkan_realize_callback: failed to get X11 window");
                }
            }
        }
        else
        {
            LOG(MODULE_ID, "_gui_vulkan_realize_callback: ERROR - VkInstance is NULL");
        }
    }
    else
    {
        LOG(MODULE_ID, "_gui_vulkan_realize_callback: core is NULL");
    }
}

// === Unrealize ===
static void _gui_vulkan_unrealize_callback(GtkWidget* widget, gpointer user_data)
{
    LOG(MODULE_ID, "_gui_vulkan_unrealize_callback: widget %p", widget);

    gui_vulkan_t core = _gui_vulkan_get_core(widget);
    if (core)
    {
        if (core->surface != VK_NULL_HANDLE)
        {
            LOG(MODULE_ID, "_gui_vulkan_unrealize_callback: destroying surface %p", core->surface);
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
            LOG(MODULE_ID, "_gui_vulkan_unrealize_callback: GE_VULKAN_UNREALIZE callback invoked");
        }
    }
    else
    {
        LOG(MODULE_ID, "_gui_vulkan_unrealize_callback: core is NULL");
    }
}

// === Idle mit Flag-Logik ===

static gboolean _gui_vulkan_idle_callback(gpointer user_data)
{
    GtkWidget* vulkan_area = GTK_WIDGET(user_data);
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_area);
    gboolean result = G_SOURCE_REMOVE;
    
    if (core)
    {
        if (core->need_close)
        {
            core->need_close = false;
            core->need_render = false;
            core->need_resize = false;
            if (core->timer_id != 0)
            {
                g_source_remove(core->timer_id);
                core->timer_id = 0;
            }
            gtk_widget_set_visible(vulkan_area, FALSE);
            result = G_SOURCE_REMOVE;
        }
        else if (core->need_resize)
        {
            core->need_resize = false;
            struct gui_event e = {0};
            e.type = GE_VULKAN_RESIZE;
            e.data.vulkan_resize.vulkan_area = vulkan_area;
            e.data.vulkan_resize.surface = core->surface;
            e.data.vulkan_resize.width = core->width;
            e.data.vulkan_resize.height = core->height;
            gui_vulkan(core, &e);
            gtk_widget_queue_draw(vulkan_area);
            result = G_SOURCE_CONTINUE;
        }
        else if (core->need_render)
        {
            core->need_render = false;
            gtk_widget_queue_draw(vulkan_area);
            result = G_SOURCE_CONTINUE;
        }
        else
        {
            result = G_SOURCE_CONTINUE;
        }
    }
    else
    {
        result = G_SOURCE_REMOVE;
    }

    return result;
}

static void _gui_vulkan_render_callback(
    GtkDrawingArea* drawing_area,
    cairo_t* cr,
    int width,
    int height,
    gpointer user_data)
{
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    gui_vulkan_t core = _gui_vulkan_get_core(GTK_WIDGET(drawing_area));
    if (core && core->initialized)
    {
        if (core->width != width || core->height != height)
        {
            core->width = width;
            core->height = height;
            core->need_resize = true;
        }

        struct gui_event e = {0};
        e.type = GE_VULKAN_RENDER;
        e.data.vulkan_render.vulkan_area = GTK_WIDGET(drawing_area);
        e.data.vulkan_render.surface = core->surface;
        e.data.vulkan_render.width = width;
        e.data.vulkan_render.height = height;
        gui_vulkan(core, &e);
    }
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

    LOG(MODULE_ID, "gui_vulkan_create: instance=%p, user_data=%p", instance, user_data);

    GtkWidget* drawing_area = gtk_drawing_area_new();
    GtkWidget* result = NULL;
    
    if (drawing_area)
    {
        gui_vulkan_t core = (gui_vulkan_t) g_malloc(sizeof(struct _gui_vulkan));
        if (core)
        {
            memset(core, 0, sizeof(struct _gui_vulkan));

            core->vulkan_area = drawing_area;
            core->user_data = user_data;
            core->instance = instance;
            core->surface = VK_NULL_HANDLE;
            core->initialized = false;
            core->width = 0;
            core->height = 0;
            core->timer_id = 0;
            core->need_render = false;
            core->need_close = false;
            core->need_resize = false;

            _gui_vulkan_set_core(drawing_area, core);

            gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area),
                _gui_vulkan_render_callback, NULL, NULL);

            g_signal_connect(drawing_area, "realize", G_CALLBACK(_gui_vulkan_realize_callback), NULL);
            g_signal_connect(drawing_area, "unrealize", G_CALLBACK(_gui_vulkan_unrealize_callback), NULL);

            _gui_add_widget_to_internal_list(drawing_area);
            
            core->timer_id = g_idle_add(_gui_vulkan_idle_callback, drawing_area);
            core->need_render = true;

            LOG(MODULE_ID, "gui_vulkan_create: SUCCESS - widget=%p, core=%p", drawing_area, core);
            result = drawing_area;
        }
        else
        {
            LOG(MODULE_ID, "gui_vulkan_create: failed to allocate Vulkan core structure");
        }
    }
    else
    {
        LOG(MODULE_ID, "gui_vulkan_create: failed to create drawing area");
    }
    
    return result;
}

void gui_vulkan_set_instance(GtkWidget* vulkan_widget, VkInstance instance)
{
    LOG(MODULE_ID, "gui_vulkan_set_instance: widget=%p, instance=%p", vulkan_widget, instance);

    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core && instance != VK_NULL_HANDLE)
    {
        core->instance = instance;
        LOG(MODULE_ID, "gui_vulkan_set_instance: instance set successfully");
    }
    else
    {
        if (!core)
        {
            LOG(MODULE_ID, "gui_vulkan_set_instance: core is NULL");
        }
        if (instance == VK_NULL_HANDLE)
        {
            LOG(MODULE_ID, "gui_vulkan_set_instance: instance is VK_NULL_HANDLE");
        }
    }
}

VkSurfaceKHR gui_vulkan_get_surface(GtkWidget* vulkan_widget)
{
    VkSurfaceKHR result = VK_NULL_HANDLE;
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        result = core->surface;
    }
    return result;
}

VkInstance gui_vulkan_get_instance(GtkWidget* vulkan_widget)
{
    VkInstance result = VK_NULL_HANDLE;
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        result = core->instance;
    }
    return result;
}

GtkWidget* gui_vulkan_get_drawing_area(GtkWidget* vulkan_widget)
{
    GtkWidget* result = NULL;
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        result = core->vulkan_area;
    }
    return result;
}

bool gui_vulkan_is_initialized(GtkWidget* vulkan_widget)
{
    bool result = false;
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        result = core->initialized;
    }
    return result;
}

void gui_vulkan_queue_render(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        core->need_render = true;
        LOG(MODULE_ID, "gui_vulkan_queue_render: render queued for widget %p", vulkan_widget);
    }
    else
    {
        LOG(MODULE_ID, "gui_vulkan_queue_render: core is NULL");
    }
}

void gui_vulkan_get_size(GtkWidget* vulkan_widget, int* width, int* height)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        if (width) *width = core->width;
        if (height) *height = core->height;
    }
    else
    {
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

void _gui_vulkan_request_close(GtkWidget* vulkan_widget)
{
    gui_vulkan_t core = _gui_vulkan_get_core(vulkan_widget);
    if (core)
    {
        core->need_close = true;
        core->need_render = false;
        core->need_resize = false;
    }
}
