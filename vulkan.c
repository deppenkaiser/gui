#include "vulkan.h"
#include "gui_vulkan_controls.h"

#include <api/api.h>
#include <stdlib.h>
#include <logging/logging.h>
#define MODULE_ID "GUI"

callback_declaration(void, gui_vulkan_render(gui_vulkan_resources_t resources, uint32_t image_index));
callback_declaration(bool, gui_vulkan_load(gui_vulkan_resources_t resources));
callback_declaration(void, gui_vulkan_setup(gui_vulkan_resources_t resources));
callback_declaration(void, gui_vulkan_custom_error(int error, const char* description));
callback_declaration(void, gui_vulkan_custom_key(GLFWwindow* window, int key, int scancode, int action, int mods));
callback_declaration(void, gui_vulkan_custom_mouse_button(GLFWwindow* window, int button, int action, int mods));
callback_declaration(void, gui_vulkan_custom_cursor_pos(GLFWwindow* window, double x, double y));
callback_declaration(void, gui_vulkan_custom_scroll(GLFWwindow* window, double xoffset, double yoffset));
callback_declaration(void, gui_vulkan_custom_char(GLFWwindow* window, unsigned int codepoint));
callback_declaration(void, gui_vulkan_custom_drop(GLFWwindow* window, int count, const char** paths));
callback_declaration(void, gui_vulkan_custom_joystick(int jid, int event));
callback_declaration(void, gui_vulkan_custom_framebuffer_resize(GLFWwindow* window, int width, int height));
callback_declaration(void, gui_vulkan_custom_window_resize(GLFWwindow* window, int width, int height));
callback_declaration(void, gui_vulkan_custom_window_close(GLFWwindow* window));
callback_declaration(void, gui_vulkan_custom_window_refresh(GLFWwindow* window));
callback_declaration(void, gui_vulkan_custom_window_focus(GLFWwindow* window, int focused));
callback_declaration(void, gui_vulkan_custom_window_iconify(GLFWwindow* window, int iconified));
callback_declaration(void, gui_vulkan_custom_window_maximize(GLFWwindow* window, int maximized));

// === Resize State ===
static bool _window_resized = false;
static int _new_width = 0;
static int _new_height = 0;

// === Fullscreen State ===
static bool _is_fullscreen = false;
static int _windowed_xpos = 0;
static int _windowed_ypos = 0;
static int _windowed_width = 800;
static int _windowed_height = 600;

static void _gui_vulkan_render(gui_vulkan_resources_t resources, uint32_t image_index)
{
    if (gui_vulkan_render)
    {
        gui_vulkan_render(resources, image_index);
    }
    else
    {
        vg_renderer_draw_triangle(&resources->renderer);
    }
}

// === GLFW Callbacks ===

static void _glfw_error_callback(int error, const char* description)
{
	LOG("GLFW", "error %d: %s", error, description);	
	if (gui_vulkan_custom_error)
	{
		gui_vulkan_custom_error(error, description);
	}
}

static void _glfw_key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
    LOG("GLFW", "key=%d scancode=%d action=%d mods=%d", key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        if (_is_fullscreen)
        {
            LOG("GLFW", "exiting fullscreen via ESC");
            _is_fullscreen = false;
            glfwSetWindowMonitor(handle, NULL, _windowed_xpos, _windowed_ypos, _windowed_width, _windowed_height, 0);
        }
        else
        {
            glfwSetWindowShouldClose(handle, GLFW_TRUE);
        }
    }
    
    // Internal controls processing
    gui_controls_t controls = (gui_controls_t)glfwGetWindowUserPointer(handle);
    if (controls)
    {
        gui_controls_process_key(controls, key, scancode, action, mods);
    }
    
    // App callback for custom logic
    if (gui_vulkan_custom_key)
    {
        gui_vulkan_custom_key(handle, key, scancode, action, mods);
    }
}

static void _glfw_mouse_button_callback(GLFWwindow* handle, int button, int action, int mods)
{
    LOG("GLFW", "mouse_button=%d action=%d mods=%d", button, action, mods);
    
    // Internal controls processing
    gui_controls_t controls = (gui_controls_t)glfwGetWindowUserPointer(handle);
    if (controls)
    {
        gui_controls_process_mouse_button(controls, button, action);
    }
    
    // App callback for custom logic
    if (gui_vulkan_custom_mouse_button)
    {
        gui_vulkan_custom_mouse_button(handle, button, action, mods);
    }
}

static void _glfw_cursor_pos_callback(GLFWwindow* handle, double x, double y)
{
    LOG("GLFW", "cursor_pos=%.2f,%.2f", x, y);
    
    // Internal controls processing
    gui_controls_t controls = (gui_controls_t)glfwGetWindowUserPointer(handle);
    if (controls)
    {
        gui_controls_process_mouse_move(controls, (int)x, (int)y);
    }
    
    // App callback for custom logic
    if (gui_vulkan_custom_cursor_pos)
    {
        gui_vulkan_custom_cursor_pos(handle, x, y);
    }
}

static void _glfw_char_callback(GLFWwindow* handle, unsigned int codepoint)
{
    LOG("GLFW", "char=%u", codepoint);
    
    // Internal controls processing
    gui_controls_t controls = (gui_controls_t)glfwGetWindowUserPointer(handle);
    if (controls)
    {
        gui_controls_process_char(controls, codepoint);
    }
    
    // App callback for custom logic
    if (gui_vulkan_custom_char)
    {
        gui_vulkan_custom_char(handle, codepoint);
    }
}

static void _glfw_framebuffer_size_callback(GLFWwindow* handle, int width, int height)
{
    LOG("GLFW", "framebuffer_size=%dx%d", width, height);
    _window_resized = true;
    _new_width = width;
    _new_height = height;
    if (gui_vulkan_custom_framebuffer_resize)
    {
        gui_vulkan_custom_framebuffer_resize(handle, width, height);
    }
}

static void _glfw_window_size_callback(GLFWwindow* handle, int width, int height)
{
    LOG("GLFW", "window_size=%dx%d", width, height);
    if (gui_vulkan_custom_window_resize)
    {
        gui_vulkan_custom_window_resize(handle, width, height);
    }
}

static void _glfw_window_close_callback(GLFWwindow* handle)
{
    LOG("GLFW", "window_close");
    if (gui_vulkan_custom_window_close)
    {
        gui_vulkan_custom_window_close(handle);
    }
}

static void _glfw_window_refresh_callback(GLFWwindow* handle)
{
    LOG("GLFW", "window_refresh");
    if (gui_vulkan_custom_window_refresh)
    {
        gui_vulkan_custom_window_refresh(handle);
    }
}

static void _glfw_window_focus_callback(GLFWwindow* handle, int focused)
{
    LOG("GLFW", "window_focus=%d", focused);
    if (gui_vulkan_custom_window_focus)
    {
        gui_vulkan_custom_window_focus(handle, focused);
    }
}

static void _glfw_window_iconify_callback(GLFWwindow* handle, int iconified)
{
    LOG("GLFW", "window_iconify=%d", iconified);
    if (gui_vulkan_custom_window_iconify)
    {
        gui_vulkan_custom_window_iconify(handle, iconified);
    }
}

static void _glfw_window_maximize_callback(GLFWwindow* handle, int maximized)
{
    if (maximized)
    {
        LOG("GLFW", "entering fullscreen");
        _is_fullscreen = true;
        glfwGetWindowPos(handle, &_windowed_xpos, &_windowed_ypos);
        glfwGetWindowSize(handle, &_windowed_width, &_windowed_height);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        LOG("GLFW", "exiting fullscreen");
        _is_fullscreen = false;
        glfwSetWindowMonitor(handle, NULL, _windowed_xpos, _windowed_ypos, _windowed_width, _windowed_height, 0);
    }
    if (gui_vulkan_custom_window_maximize)
    {
        gui_vulkan_custom_window_maximize(handle, maximized);
    }
}

static void _glfw_scroll_callback(GLFWwindow* handle, double xoffset, double yoffset)
{
    LOG("GLFW", "scroll=%.2f,%.2f", xoffset, yoffset);
    if (gui_vulkan_custom_scroll)
    {
        gui_vulkan_custom_scroll(handle, xoffset, yoffset);
    }
}

static void _glfw_drop_callback(GLFWwindow* handle, int count, const char** paths)
{
    LOG("GLFW", "drop count=%d", count);
    for (int i = 0; i < count; i++)
    {
        LOG("GLFW", "  path[%d]=%s", i, paths[i]);
    }
    if (gui_vulkan_custom_drop)
    {
        gui_vulkan_custom_drop(handle, count, paths);
    }
}

static void _glfw_joystick_callback(int jid, int event)
{
    LOG("GLFW", "joystick jid=%d event=%d", jid, event);
    if (gui_vulkan_custom_joystick)
    {
        gui_vulkan_custom_joystick(jid, event);
    }
}

// === Window Creation ===

gui_vulkan_window_t gui_vulkan_window_create(int width, int height, const char* title)
{
    gui_vulkan_window_t result = NULL;
    struct gui_vulkan_window* window = NULL;

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
            window = (struct gui_vulkan_window*)malloc(sizeof(struct gui_vulkan_window));
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

void gui_vulkan_window_destroy(gui_vulkan_window_t window)
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

bool gui_vulkan_window_poll_events(gui_vulkan_window_t window, gui_vulkan_resources_t resources)
{
    bool result = false;

    if (window && window->handle)
    {
        glfwPollEvents();
        window->should_close = glfwWindowShouldClose(window->handle);
        result = !window->should_close;

        if (result && resources)
        {
            int new_width, new_height;
            if (gui_vulkan_window_was_resized(window, &new_width, &new_height))
            {
                vg_renderer_wait_idle(&resources->renderer);
                vg_renderer_destroy(&resources->renderer);
                vg_swapchain_recreate(&resources->swapchain, (uint32_t)new_width, (uint32_t)new_height);
                vg_renderer_create(&resources->swapchain, &resources->renderer);
            }

            uint32_t image_index = 0;
            if (vg_renderer_begin_frame(&resources->renderer, &image_index))
            {
                _gui_vulkan_render(resources, image_index);
                
                // Draw controls
                if (resources->controls)
                {
                    int width, height;
                    glfwGetWindowSize(window->handle, &width, &height);
                    gui_controls_cleanup_pending(resources->device.device);
                    gui_controls_draw_internal(resources->controls, resources->renderer.current_command_buffer, resources->device.device, resources->device.physical_device, (uint32_t)width, (uint32_t)height);
                }
                
                vg_renderer_end_frame(&resources->renderer, image_index);
            }
            else
            {
                vg_result_t err = vg_get_last_error();
                if (err == VG_ERROR_SWAPCHAIN_ERROR)
                {
                    glfwGetWindowSize(window->handle, &new_width, &new_height);
                    vg_renderer_wait_idle(&resources->renderer);
                    vg_renderer_destroy(&resources->renderer);
                    vg_swapchain_recreate(&resources->swapchain, (uint32_t)new_width, (uint32_t)new_height);
                    vg_renderer_create(&resources->swapchain, &resources->renderer);
                }
            }
        }
    }

    return result;
}

bool gui_vulkan_window_was_resized(gui_vulkan_window_t window, int* width, int* height)
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

VkSurfaceKHR gui_vulkan_window_create_surface(VkInstance instance, gui_vulkan_window_t window)
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

static void _gui_initialize_instance_config(vb_instance_config_t config)
{
	uint32_t ext_count = 0;
	const char** ext = glfwGetRequiredInstanceExtensions(&ext_count);
	config->extension_count = ext_count;
	config->extension_names = ext;
	config->layer_count = 0;
	config->layer_names = NULL;
}

bool gui_vulkan_create_resources(gui_vulkan_window_t window, gui_vulkan_resources_t resources)
{
	bool is_ok = false;

	struct vb_instance_config config = {0};
	_gui_initialize_instance_config(&config);

	if (vb_initialize(&config, &resources->instance))
	{
		resources->surface = gui_vulkan_window_create_surface(resources->instance.instance, window);
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

			if (vg_device_create(&resources->instance, resources->surface, &device_config, &resources->device))
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
				if (is_ok)
				{
					is_ok = vg_renderer_create(&resources->swapchain, &resources->renderer);
					if (is_ok)
					{
						// Create controls collection
						resources->controls = gui_controls_create();
						if (resources->controls)
						{
							// Store pointer for callbacks
							glfwSetWindowUserPointer(window->handle, resources->controls);
							
							// Call user setup to populate controls
							if (gui_vulkan_setup)
							{
								gui_vulkan_setup(resources);
							}
						}
					}
				}
			}
		}
	}

	return is_ok;
}

void gui_vulkan_destroy_resources(gui_vulkan_resources_t resources)
{
	if (resources->controls)
	{
		gui_controls_cleanup_pending(resources->device.device);
		gui_controls_destroy(resources->controls);
		resources->controls = NULL;
	}
    vg_renderer_destroy(&resources->renderer);
	vg_swapchain_destroy(&resources->swapchain);
	vg_device_destroy(&resources->device);
	vkDestroySurfaceKHR(resources->instance.instance, resources->surface, NULL);
	resources->surface = NULL;
}
