#pragma once

#include "vulkan.h"

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// === Widget Types ===
typedef enum
{
    GUI_CONTROL_LABEL,
    GUI_CONTROL_BUTTON,
    GUI_CONTROL_LIST,
    GUI_CONTROL_ENTRY
} gui_control_type_t;

// === Forward Declarations ===
typedef struct gui_control* gui_control_t;
typedef struct gui_controls* gui_controls_t;

// === Control State ===
typedef struct gui_control_state
{
    bool visible;
    bool enabled;
    bool hovered;
    bool pressed;
    bool focused;
} gui_control_state_t;

// === Layout ===
typedef struct gui_control_rect
{
    int x;
    int y;
    int width;
    int height;
} gui_control_rect_t;

// === Label ===
typedef struct gui_label
{
    char text[256];
} gui_label_t;

// === Button ===
typedef struct gui_button
{
    char text[128];
    bool clicked;
} gui_button_t;

// === List Box ===
typedef struct gui_list_item
{
    char text[128];
} gui_list_item_t;

typedef struct gui_list
{
    gui_list_item_t* items;
    uint32_t item_count;
    int32_t selected_index;
    int32_t scroll_offset;
    uint32_t visible_items;
    uint32_t item_height;
} gui_list_t;

// === Text Entry ===
typedef struct gui_entry
{
    char text[256];
    uint32_t cursor_pos;
    uint32_t max_length;
    bool password_mode;
} gui_entry_t;

// === Widget Data Union ===
typedef union gui_control_data
{
    gui_label_t label;
    gui_button_t button;
    gui_list_t list;
    gui_entry_t entry;
} gui_control_data_t;

// === Main Control Structure ===
typedef struct gui_control
{
    gui_control_type_t type;
    gui_control_rect_t rect;
    gui_control_state_t state;
    gui_control_data_t data;
    struct gui_control* next;
} *gui_control_t;

// === Controls Collection ===
typedef struct gui_controls
{
    gui_control_t head;
    gui_control_t tail;
    uint32_t count;
} *gui_controls_t;

// === Event Callback Types ===
typedef void (*gui_button_callback)(gui_control_t button);
typedef void (*gui_list_callback)(gui_control_t list, int32_t selected_index);
typedef void (*gui_entry_callback)(gui_control_t entry);

// === API ===

// Controls Collection
gui_controls_t gui_controls_create(void);
void gui_controls_destroy(gui_controls_t controls);

// Control Creation
gui_control_t gui_control_label(gui_controls_t controls, int x, int y, int width, int height, const char* text);
gui_control_t gui_control_button(gui_controls_t controls, int x, int y, int width, int height, const char* text, gui_button_callback callback);
gui_control_t gui_control_list(gui_controls_t controls, int x, int y, int width, int height, gui_list_callback callback);
gui_control_t gui_control_entry(gui_controls_t controls, int x, int y, int width, int height, gui_entry_callback callback);

// Event Processing
gui_control_t gui_controls_hit_test(gui_controls_t controls, int x, int y);
void gui_controls_process_mouse_move(gui_controls_t controls, int x, int y);
void gui_controls_process_mouse_button(gui_controls_t controls, int button, int action);
void gui_controls_process_char(gui_controls_t controls, unsigned int codepoint);

// Internal (called from vulkan.c)
void gui_controls_draw_internal(gui_controls_t controls, VkCommandBuffer cmd, VkDevice device, VkPhysicalDevice physical_device, uint32_t screen_width, uint32_t screen_height);
void gui_controls_cleanup_pending(VkDevice device);
