#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <vulkan_base/vulkan_base.h>
#include <vulkan_graphics/vulkan_graphics.h>

typedef struct gui_window
{
    GLFWwindow* handle;
    int width;
    int height;
    const char* title;
    bool should_close;
}* gui_window_t;

typedef struct gui_glwf_resources
{
	VkSurfaceKHR surface;
    struct vg_device device;
    struct vg_swapchain swapchain;
}* gui_glwf_resources_t;

gui_window_t gui_window_create(int width, int height, const char* title);
void gui_window_destroy(gui_window_t window);
bool gui_window_poll_events(gui_window_t window);
bool gui_window_was_resized(gui_window_t window, int* width, int* height);
VkSurfaceKHR gui_window_create_surface(VkInstance instance, gui_window_t window);
void gui_initialize_instance_config(vb_instance_config_t config);
bool gui_create_surface_device_and_swapchain(gui_window_t window, vb_instance_t instance, gui_glwf_resources_t resources);
void gui_destroy_surface_device_and_swapchain(vb_instance_t instance, gui_glwf_resources_t resources);
