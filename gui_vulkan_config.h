#pragma once

#include <api/api.h>
#include <string/string.h>
#include <json/json.h>
#include <stdbool.h>
#include <stdint.h>

// === 3D Vector ===
typedef struct vec3
{
    float x;
    float y;
    float z;
} vec3_t;

// === Transform ===
typedef struct transform
{
    vec3_t position;
    vec3_t rotation;
    vec3_t scale;
} transform_t;

// === Color ===
typedef struct color
{
    float r;
    float g;
    float b;
    float a;
} color_t;

// === Model ===
typedef struct model_config
{
    string_t path;
    transform_t transform;
} model_config_t;

// === Window ===
typedef struct window_config
{
    string_t title;
    uint32_t width;
    uint32_t height;
    color_t background;
} window_config_t;

// === App Config ===
typedef struct gui_vulkan_config
{
    string_t config_path;
    window_config_t window;
    model_config_t* models;
    uint32_t model_count;
} gui_vulkan_config_t;

// === JSON Callback ===
callback bool gui_vulkan_config_callback(json_object* parent, void* user_data);

// === Config Functions ===
void gui_vulkan_config_init(gui_vulkan_config_t* config);
void gui_vulkan_config_shutdown(gui_vulkan_config_t* config);

// === Default Values ===
#define GUI_VULKAN_DEFAULT_BG_R 0.0f
#define GUI_VULKAN_DEFAULT_BG_G 0.0f
#define GUI_VULKAN_DEFAULT_BG_B 0.0f
#define GUI_VULKAN_DEFAULT_BG_A 1.0f
#define GUI_VULKAN_DEFAULT_WINDOW_WIDTH 800
#define GUI_VULKAN_DEFAULT_WINDOW_HEIGHT 600
