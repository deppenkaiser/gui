#include "vulkan.h"
#include "events.h"

#include <api/api.h>

struct _gui_vulkan_core
{
    GtkWidget* vulkan_area;
    VkSurfaceKHR surface;
    void* user_data;
    int width;
    int height;
    bool initialized;
};

// In der gui-Bibliothek (z.B. in vulkan.c)
callback_declaration(void, gui_vulkan(gui_vulkan_t core, gui_event_t e));
