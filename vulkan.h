#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct gui_window
{
    GLFWwindow* handle;
    int width;
    int height;
    const char* title;
    bool should_close;
}* gui_window_t;

gui_window_t gui_window_create(int width, int height, const char* title);
void gui_window_destroy(gui_window_t window);
bool gui_window_poll_events(gui_window_t window);
VkSurfaceKHR gui_window_create_surface(VkInstance instance, gui_window_t window);
