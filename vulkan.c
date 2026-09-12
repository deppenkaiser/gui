#include "vulkan.h"

#include <stdlib.h>
#include <logging/logging.h>
#define MODULE_ID "GUI"

// === Resize State ===
static bool _window_resized = false;
static int _new_width = 0;
static int _new_height = 0;

// === GLFW Callbacks ===

static void _glfw_error_callback(int error, const char* description)
{
    LOG("GLFW", "error %d: %s", error, description);
}

static void _glfw_key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
    LOG("GLFW", "key=%d scancode=%d action=%d mods=%d", key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(handle, GLFW_TRUE);
    }
}

static void _glfw_mouse_button_callback(GLFWwindow* handle, int button, int action, int mods)
{
    LOG("GLFW", "mouse_button=%d action=%d mods=%d", button, action, mods);
}

static void _glfw_cursor_pos_callback(GLFWwindow* handle, double x, double y)
{
    LOG("GLFW", "cursor_pos=%.2f,%.2f", x, y);
}

static void _glfw_scroll_callback(GLFWwindow* handle, double xoffset, double yoffset)
{
    LOG("GLFW", "scroll=%.2f,%.2f", xoffset, yoffset);
}

static void _glfw_framebuffer_size_callback(GLFWwindow* handle, int width, int height)
{
    LOG("GLFW", "framebuffer_size=%dx%d", width, height);
    _window_resized = true;
    _new_width = width;
    _new_height = height;
}

static void _glfw_window_size_callback(GLFWwindow* handle, int width, int height)
{
    LOG("GLFW", "window_size=%dx%d", width, height);
}

static void _glfw_window_close_callback(GLFWwindow* handle)
{
    LOG("GLFW", "window_close");
}

static void _glfw_window_refresh_callback(GLFWwindow* handle)
{
    LOG("GLFW", "window_refresh");
}

static void _glfw_window_focus_callback(GLFWwindow* handle, int focused)
{
    LOG("GLFW", "window_focus=%d", focused);
}

static void _glfw_window_iconify_callback(GLFWwindow* handle, int iconified)
{
    LOG("GLFW", "window_iconify=%d", iconified);
}

static void _glfw_window_maximize_callback(GLFWwindow* handle, int maximized)
{
    LOG("GLFW", "window_maximize=%d", maximized);
}

static void _glfw_char_callback(GLFWwindow* handle, unsigned int codepoint)
{
    LOG("GLFW", "char=%u", codepoint);
}

static void _glfw_drop_callback(GLFWwindow* handle, int count, const char** paths)
{
    LOG("GLFW", "drop count=%d", count);
    for (int i = 0; i < count; i++)
    {
        LOG("GLFW", "  path[%d]=%s", i, paths[i]);
    }
}

static void _glfw_joystick_callback(int jid, int event)
{
    LOG("GLFW", "joystick jid=%d event=%d", jid, event);
}

// === Window Creation ===

gui_window_t gui_window_create(int width, int height, const char* title)
{
    gui_window_t result = NULL;
    struct gui_window* window = NULL;

    glfwSetErrorCallback(_glfw_error_callback);
    glfwSetJoystickCallback(_glfw_joystick_callback);

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

                // Register all callbacks
                glfwSetKeyCallback(handle, _glfw_key_callback);
                glfwSetMouseButtonCallback(handle, _glfw_mouse_button_callback);
                glfwSetCursorPosCallback(handle, _glfw_cursor_pos_callback);
                glfwSetScrollCallback(handle, _glfw_scroll_callback);
                glfwSetFramebufferSizeCallback(handle, _glfw_framebuffer_size_callback);
                glfwSetWindowSizeCallback(handle, _glfw_window_size_callback);
                glfwSetWindowCloseCallback(handle, _glfw_window_close_callback);
                glfwSetWindowRefreshCallback(handle, _glfw_window_refresh_callback);
                glfwSetWindowFocusCallback(handle, _glfw_window_focus_callback);
                glfwSetWindowIconifyCallback(handle, _glfw_window_iconify_callback);
                glfwSetWindowMaximizeCallback(handle, _glfw_window_maximize_callback);
                glfwSetCharCallback(handle, _glfw_char_callback);
                glfwSetDropCallback(handle, _glfw_drop_callback);

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

bool gui_window_was_resized(gui_window_t window, int* width, int* height)
{
    bool result = false;

    if (_window_resized)
    {
        _window_resized = false;
        if (window)
        {
            window->width = _new_width;
            window->height = _new_height;
        }
        if (width) *width = _new_width;
        if (height) *height = _new_height;
        result = true;
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

bool gui_create_surface_device_and_swapchain(gui_window_t window, vb_instance_t instance, gui_glwf_resources_t resources)
{
	bool is_ok = false;
	
	resources->surface = gui_window_create_surface(instance->instance, window);
	if (resources->surface != VK_NULL_HANDLE)
	{
		glfwWaitEventsTimeout(0.1);

		const char* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		struct vg_device_config device_config =
		{
			.features = {0},
			.extension_names = device_extensions,
			.extension_count = 1,
			.enable_anisotropy = true,
			.enable_msaa = true
		};

		if (vg_device_create(instance, resources->surface, &device_config, &resources->device))
		{
			int width = 0, height = 0;
			
			glfwGetWindowSize(window->handle, &width, &height);
			struct vg_swapchain_config swap_config =
			{
				.width = (uint32_t)width,
				.height = (uint32_t)height,
				.present_mode = VK_PRESENT_MODE_FIFO_KHR,
				.image_count = 3,
				.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
			};

			is_ok = vg_swapchain_create(&resources->device, resources->surface, &swap_config, &resources->swapchain);
		}
	}

	return is_ok;
}

void gui_destroy_surface_device_and_swapchain(vb_instance_t instance, gui_glwf_resources_t resources)
{
	vg_swapchain_destroy(&resources->swapchain);
	vg_device_destroy(&resources->device);
	vkDestroySurfaceKHR(instance->instance, resources->surface, NULL);
	resources->surface = NULL;
}
