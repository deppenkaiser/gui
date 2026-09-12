#include "gui_vulkan_config.h"
#include <logging/logging.h>
#include <string.h>

#define MODULE_ID "GUI_CONFIG"

// === JSON Callback Implementation ===

bool gui_vulkan_config_callback(json_object* parent, void* user_data)
{
    bool modified = false;
    gui_vulkan_config_t* config = (gui_vulkan_config_t*)user_data;
    
    if (!config) return false;
    
    if (parent != NULL)
    {
        // Read from JSON
        json_object* window = NULL;
        if (json_object_object_get_ex(parent, "window", &window))
        {
            json_object* obj = NULL;
            if (json_object_object_get_ex(window, "title", &obj))
            {
                string_copy(config->window.title, sizeof(string_t), json_object_get_string(obj));
            }
            if (json_object_object_get_ex(window, "width", &obj))
            {
                config->window.width = (uint32_t)json_object_get_int(obj);
            }
            if (json_object_object_get_ex(window, "height", &obj))
            {
                config->window.height = (uint32_t)json_object_get_int(obj);
            }
            if (json_object_object_get_ex(window, "background", &obj))
            {
                json_object* color = obj;
                if (json_object_is_type(color, json_type_array) && json_object_array_length(color) >= 4)
                {
                    config->window.background.r = (float)json_object_get_double(json_object_array_get_idx(color, 0));
                    config->window.background.g = (float)json_object_get_double(json_object_array_get_idx(color, 1));
                    config->window.background.b = (float)json_object_get_double(json_object_array_get_idx(color, 2));
                    config->window.background.a = (float)json_object_get_double(json_object_array_get_idx(color, 3));
                }
            }
        }
        
        // Read models array
        json_object* models = NULL;
        if (json_object_object_get_ex(parent, "models", &models))
        {
            if (json_object_is_type(models, json_type_array))
            {
                config->model_count = (uint32_t)json_object_array_length(models);
                if (config->model_count > 0)
                {
                    config->models = malloc(config->model_count * sizeof(model_config_t));
                    memset(config->models, 0, config->model_count * sizeof(model_config_t));
                    
                    for (uint32_t i = 0; i < config->model_count; i++)
                    {
                        json_object* model = json_object_array_get_idx(models, i);
                        json_object* obj = NULL;
                        if (json_object_object_get_ex(model, "path", &obj))
                        {
                            string_copy(config->models[i].path, sizeof(string_t), json_object_get_string(obj));
                        }
                        if (json_object_object_get_ex(model, "position", &obj))
                        {
                            if (json_object_is_type(obj, json_type_array) && json_object_array_length(obj) >= 3)
                            {
                                config->models[i].transform.position.x = (float)json_object_get_double(json_object_array_get_idx(obj, 0));
                                config->models[i].transform.position.y = (float)json_object_get_double(json_object_array_get_idx(obj, 1));
                                config->models[i].transform.position.z = (float)json_object_get_double(json_object_array_get_idx(obj, 2));
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Create defaults
        string_copy(config->window.title, sizeof(string_t), "vulcan_demo");
        config->window.width = GUI_VULKAN_DEFAULT_WINDOW_WIDTH;
        config->window.height = GUI_VULKAN_DEFAULT_WINDOW_HEIGHT;
        config->window.background.r = GUI_VULKAN_DEFAULT_BG_R;
        config->window.background.g = GUI_VULKAN_DEFAULT_BG_G;
        config->window.background.b = GUI_VULKAN_DEFAULT_BG_B;
        config->window.background.a = GUI_VULKAN_DEFAULT_BG_A;
        config->models = NULL;
        config->model_count = 0;
        modified = true;
    }
    
    return modified;
}

// === Config Functions ===

bool gui_vulkan_config_load(const char* filepath, gui_vulkan_config_t* config)
{
    if (!filepath || !config) return false;
    
    memset(config, 0, sizeof(gui_vulkan_config_t));
    json_load(filepath, config);
    return true;
}

bool gui_vulkan_config_save(const char* filepath, gui_vulkan_config_t* config)
{
    if (!filepath || !config) return false;
    
    json_object* root = json_object_new_object();
    
    // Window
    json_object* window = json_object_new_object();
    json_object_object_add(window, "title", json_object_new_string(config->window.title));
    json_object_object_add(window, "width", json_object_new_int(config->window.width));
    json_object_object_add(window, "height", json_object_new_int(config->window.height));
    
    json_object* bg = json_object_new_array();
    json_object_array_add(bg, json_object_new_double(config->window.background.r));
    json_object_array_add(bg, json_object_new_double(config->window.background.g));
    json_object_array_add(bg, json_object_new_double(config->window.background.b));
    json_object_array_add(bg, json_object_new_double(config->window.background.a));
    json_object_object_add(window, "background", bg);
    
    json_object_object_add(root, "window", window);
    
    // Models
    json_object* models = json_object_new_array();
    for (uint32_t i = 0; i < config->model_count; i++)
    {
        json_object* model = json_object_new_object();
        json_object_object_add(model, "path", json_object_new_string(config->models[i].path));
        
        json_object* pos = json_object_new_array();
        json_object_array_add(pos, json_object_new_double(config->models[i].transform.position.x));
        json_object_array_add(pos, json_object_new_double(config->models[i].transform.position.y));
        json_object_array_add(pos, json_object_new_double(config->models[i].transform.position.z));
        json_object_object_add(model, "position", pos);
        
        json_object_array_add(models, model);
    }
    json_object_object_add(root, "models", models);
    
    json_object_to_file_ext(filepath, root, JSON_C_TO_STRING_PRETTY);
    json_object_put(root);
    
    return true;
}
