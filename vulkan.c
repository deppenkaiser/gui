#include "vulkan.h"

#include <stdlib.h>
#include <logging/logging.h>
#define MODULE_ID "GUI"

gui_window_t gui_window_create(int width, int height, const char* title)
{
    gui_window_t result = NULL;
    struct gui_window* window = NULL;

    if (!glfwInit())
    {
        LOG(MODULE_ID, "failed to initialize GLFW");
    }
    else
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* handle = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!handle)
        {
            LOG(MODULE_ID, "failed to create GLFW window");
            glfwTerminate();
        }
        else
        {
            window = (struct gui_window*)malloc(sizeof(struct gui_window));
            if (!window)
            {
                LOG(MODULE_ID, "failed to allocate window structure");
                glfwDestroyWindow(handle);
                glfwTerminate();
            }
            else
            {
                window->handle = handle;
                window->width = width;
                window->height = height;
                window->title = title;
                window->should_close = false;
                result = window;
            }
        }
    }

    return result;
}

void gui_window_destroy(gui_window_t window)
{
    if (window)
    {
        if (window->handle)
        {
            glfwDestroyWindow(window->handle);
        }
        free(window);
        glfwTerminate();
    }
}

bool gui_window_poll_events(gui_window_t window)
{
    bool result = false;

    if (window && window->handle)
    {
        glfwPollEvents();
        window->should_close = glfwWindowShouldClose(window->handle);
        result = !window->should_close;
    }

    return result;
}

VkSurfaceKHR gui_window_create_surface(VkInstance instance, gui_window_t window)
{
    VkSurfaceKHR result = VK_NULL_HANDLE;

    if (window && window->handle)
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkResult vk_result = glfwCreateWindowSurface(instance, window->handle, NULL, &surface);
        if (vk_result != VK_SUCCESS)
        {
            LOG(MODULE_ID, "failed to create Vulkan surface: %d", vk_result);
        }
        else
        {
            result = surface;
        }
    }
    else
    {
        LOG(MODULE_ID, "window or handle is NULL");
    }

    return result;
}

void gui_initialize_instance_config(vb_instance_config_t config)
{
	uint32_t ext_count = 0;
	const char** ext = glfwGetRequiredInstanceExtensions(&ext_count);
	config->extension_count = ext_count;
	config->extension_names = ext;
	config->layer_count = 0;
	config->layer_names = NULL;
}
