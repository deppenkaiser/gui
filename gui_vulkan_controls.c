#include "gui_vulkan_controls.h"

#include <logging/logging.h>
#define MODULE_ID "GUI_CONTROLS"

#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

// === Animation System ===

typedef struct gui_anim
{
    float r, g, b;
    float target_r, target_g, target_b;
} gui_anim_t;

#define MAX_ANIM 32
static gui_anim_t _anims[MAX_ANIM];

static gui_anim_t* _anim_for(gui_control_t c)
{
    return &_anims[(uintptr_t)c % MAX_ANIM];
}

static void _anim_set(gui_anim_t* a, float r, float g, float b)
{
    a->target_r = r;
    a->target_g = g;
    a->target_b = b;
}

static void _anim_tick(gui_anim_t* a)
{
    float s = 0.15f;
    a->r += (a->target_r - a->r) * s;
    a->g += (a->target_g - a->g) * s;
    a->b += (a->target_b - a->b) * s;
}

// === Pending Buffer Destruction ===

typedef struct gui_pending_buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    struct gui_pending_buffer* next;
} gui_pending_buffer_t;

static gui_pending_buffer_t* _pending_buffers = NULL;

void gui_controls_cleanup_pending(VkDevice device)
{
    if (!_pending_buffers) return;
    
    // Wait for all GPU work to complete before destroying buffers
    vkDeviceWaitIdle(device);
    
    gui_pending_buffer_t* current = _pending_buffers;
    while (current)
    {
        gui_pending_buffer_t* next = current->next;
        if (current->buffer)
        {
            vkDestroyBuffer(device, current->buffer, NULL);
        }
        if (current->memory)
        {
            vkFreeMemory(device, current->memory, NULL);
        }
        free(current);
        current = next;
    }
    _pending_buffers = NULL;
}

// === Internal Data ===

typedef struct gui_draw_context
{
    float* vertices;
    uint32_t vertex_count;
    uint32_t vertex_capacity;
    uint32_t screen_width;
    uint32_t screen_height;
} gui_draw_context_t;

// === Coordinate Conversion ===

static float _gui_pixel_to_clip_x(int px, uint32_t screen_w)
{
    return (2.0f * px / screen_w) - 1.0f;
}

static float _gui_pixel_to_clip_y(int py, uint32_t screen_h)
{
    return 1.0f - (2.0f * py / screen_h);
}

// === Helpers ===

static gui_control_t _gui_control_alloc(gui_control_type_t type)
{
    gui_control_t control = (gui_control_t)malloc(sizeof(struct gui_control));
    if (control)
    {
        memset(control, 0, sizeof(struct gui_control));
        control->type = type;
        control->state.visible = true;
        control->state.enabled = true;
    }
    return control;
}

static void _gui_control_link(gui_controls_t controls, gui_control_t control)
{
    if (controls->tail)
    {
        controls->tail->next = control;
    }
    else
    {
        controls->head = control;
    }
    controls->tail = control;
    controls->count++;
}

// === Controls Collection ===

gui_controls_t gui_controls_create(void)
{
    gui_controls_t controls = (gui_controls_t)malloc(sizeof(struct gui_controls));
    if (controls)
    {
        memset(controls, 0, sizeof(struct gui_controls));
    }
    return controls;
}

void gui_controls_destroy(gui_controls_t controls)
{
    if (!controls) return;
    
    gui_control_t current = controls->head;
    while (current)
    {
        gui_control_t next = current->next;
        if (current->type == GUI_CONTROL_LIST && current->data.list.items)
        {
            free(current->data.list.items);
        }
        free(current);
        current = next;
    }
    free(controls);
}

// === Control Creation ===

gui_control_t gui_control_label(gui_controls_t controls, int x, int y, int width, int height, const char* text)
{
    gui_control_t control = _gui_control_alloc(GUI_CONTROL_LABEL);
    if (control)
    {
        control->rect.x = x;
        control->rect.y = y;
        control->rect.width = width;
        control->rect.height = height;
        if (text)
        {
            strncpy(control->data.label.text, text, sizeof(control->data.label.text) - 1);
            control->data.label.text[sizeof(control->data.label.text) - 1] = '\0';
        }
        _gui_control_link(controls, control);
    }
    return control;
}

gui_control_t gui_control_button(gui_controls_t controls, int x, int y, int width, int height, const char* text, gui_button_callback callback)
{
    gui_control_t control = _gui_control_alloc(GUI_CONTROL_BUTTON);
    if (control)
    {
        control->rect.x = x;
        control->rect.y = y;
        control->rect.width = width;
        control->rect.height = height;
        if (text)
        {
            strncpy(control->data.button.text, text, sizeof(control->data.button.text) - 1);
            control->data.button.text[sizeof(control->data.button.text) - 1] = '\0';
        }
        _gui_control_link(controls, control);
    }
    return control;
}

gui_control_t gui_control_list(gui_controls_t controls, int x, int y, int width, int height, gui_list_callback callback)
{
    gui_control_t control = _gui_control_alloc(GUI_CONTROL_LIST);
    if (control)
    {
        control->rect.x = x;
        control->rect.y = y;
        control->rect.width = width;
        control->rect.height = height;
        control->data.list.item_height = 24;
        control->data.list.selected_index = -1;
        _gui_control_link(controls, control);
    }
    return control;
}

gui_control_t gui_control_entry(gui_controls_t controls, int x, int y, int width, int height, gui_entry_callback callback)
{
    gui_control_t control = _gui_control_alloc(GUI_CONTROL_ENTRY);
    if (control)
    {
        control->rect.x = x;
        control->rect.y = y;
        control->rect.width = width;
        control->rect.height = height;
        control->data.entry.max_length = sizeof(control->data.entry.text) - 1;
        _gui_control_link(controls, control);
    }
    return control;
}

// === Event Processing ===

gui_control_t gui_controls_hit_test(gui_controls_t controls, int x, int y)
{
    gui_control_t found = NULL;
    gui_control_t current = controls->head;
    while (current)
    {
        if (current->state.visible &&
            x >= current->rect.x && x < current->rect.x + current->rect.width &&
            y >= current->rect.y && y < current->rect.y + current->rect.height)
        {
            found = current;
        }
        current = current->next;
    }
    return found;
}

void gui_controls_process_mouse_move(gui_controls_t controls, int x, int y)
{
    gui_control_t current = controls->head;
    while (current)
    {
        bool hit = (x >= current->rect.x && x < current->rect.x + current->rect.width &&
                    y >= current->rect.y && y < current->rect.y + current->rect.height);
        current->state.hovered = hit && current->state.enabled;
        current = current->next;
    }
}

void gui_controls_process_mouse_button(gui_controls_t controls, int button, int action)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    
    gui_control_t current = controls->head;
    while (current)
    {
        if (current->state.hovered && current->state.enabled)
        {
            if (action == GLFW_PRESS)
            {
                current->state.pressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                current->state.pressed = false;
                if (current->type == GUI_CONTROL_BUTTON)
                {
                    current->data.button.clicked = true;
                }
            }
        }
        current = current->next;
    }
}

void gui_controls_process_char(gui_controls_t controls, unsigned int codepoint)
{
    gui_control_t current = controls->head;
    while (current)
    {
        if (current->state.focused && current->state.enabled && current->type == GUI_CONTROL_ENTRY)
        {
            gui_entry_t* entry = &current->data.entry;
            if (entry->cursor_pos < entry->max_length && codepoint >= 32 && codepoint < 127)
            {
                entry->text[entry->cursor_pos++] = (char)codepoint;
                entry->text[entry->cursor_pos] = '\0';
            }
            else if (codepoint == 8 && entry->cursor_pos > 0)
            {
                entry->text[--entry->cursor_pos] = '\0';
            }
        }
        current = current->next;
    }
}

// === Drawing Helpers ===

static void _gui_draw_rect(gui_draw_context_t* ctx, int x, int y, int w, int h, float r, float g, float b)
{
    if (ctx->vertex_count + 4 > ctx->vertex_capacity)
    {
        ctx->vertex_capacity *= 2;
        ctx->vertices = realloc(ctx->vertices, ctx->vertex_capacity * 5 * sizeof(float));
    }
    
    // Convert pixel coords to clip space (-1 to 1)
    float left   = _gui_pixel_to_clip_x(x, ctx->screen_width);
    float right  = _gui_pixel_to_clip_x(x + w, ctx->screen_width);
    float top    = _gui_pixel_to_clip_y(y, ctx->screen_height);
    float bottom = _gui_pixel_to_clip_y(y + h, ctx->screen_height);
    
    uint32_t base = ctx->vertex_count;
    float* v = &ctx->vertices[base * 5];
    
    // 6 vertices for 2 triangles
    // Triangle 1: bottom-left, bottom-right, top-right
    // Triangle 2: bottom-right, top-right, top-left
    
    // Vertex 0: bottom-left
    v[0] = left;  v[1] = bottom; v[2] = r; v[3] = g; v[4] = b;
    // Vertex 1: bottom-right
    v[5] = right; v[6] = bottom; v[7] = r; v[8] = g; v[9] = b;
    // Vertex 2: top-right
    v[10] = right; v[11] = top; v[12] = r; v[13] = g; v[14] = b;
    // Vertex 3: bottom-left (again for triangle 2)
    v[15] = left; v[16] = bottom; v[17] = r; v[18] = g; v[19] = b;
    // Vertex 4: top-right (again for triangle 2)
    v[20] = right; v[21] = top; v[22] = r; v[23] = g; v[24] = b;
    // Vertex 5: top-left
    v[25] = left; v[26] = top; v[27] = r; v[28] = g; v[29] = b;
    
    ctx->vertex_count += 6;
}

// === Control Drawing ===

static void _gui_draw_label(gui_draw_context_t* ctx, gui_control_t control)
{
    gui_control_rect_t* r = &control->rect;
    
    // Background
    _gui_draw_rect(ctx, r->x, r->y, r->width, r->height, 0.15f, 0.15f, 0.15f);
    
    // Border
    _gui_draw_rect(ctx, r->x, r->y, r->width, 1, 0.3f, 0.3f, 0.3f);
    _gui_draw_rect(ctx, r->x, r->y + r->height - 1, r->width, 1, 0.3f, 0.3f, 0.3f);
    _gui_draw_rect(ctx, r->x, r->y, 1, r->height, 0.3f, 0.3f, 0.3f);
    _gui_draw_rect(ctx, r->x + r->width - 1, r->y, 1, r->height, 0.3f, 0.3f, 0.3f);
    
    // Text placeholder
    int text_len = (int)strlen(control->data.label.text);
    if (text_len > 0)
    {
        int text_w = text_len * 6;
        int text_h = r->height / 2;
        if (text_h < 22) text_h = 22;
        int text_x = r->x + 5;
        int text_y = r->y + (r->height - text_h) / 2;
        _gui_draw_rect(ctx, text_x, text_y, text_w, text_h, 0.9f, 0.9f, 0.9f);
    }
}

static void _gui_draw_button(gui_draw_context_t* ctx, gui_control_t control)
{
    gui_control_rect_t* r = &control->rect;
    
    gui_anim_t* anim = _anim_for(control);
    
    // Determine target based on state
    float target_r = 0.25f, target_g = 0.45f, target_b = 0.75f;
    if (control->state.hovered)
    {
        target_r = 0.35f; target_g = 0.55f; target_b = 0.85f;
    }
    if (control->state.pressed)
    {
        target_r = 0.15f; target_g = 0.35f; target_b = 0.65f;
    }
    
    _anim_set(anim, target_r, target_g, target_b);
    _anim_tick(anim);
    
    // Background with animated color
    _gui_draw_rect(ctx, r->x, r->y, r->width, r->height, anim->r, anim->g, anim->b);
    
    // Border (3D effect)
    _gui_draw_rect(ctx, r->x, r->y, r->width, 1, 0.6f, 0.6f, 0.6f);
    _gui_draw_rect(ctx, r->x, r->y, 1, r->height, 0.6f, 0.6f, 0.6f);
    _gui_draw_rect(ctx, r->x, r->y + r->height - 1, r->width, 1, 0.2f, 0.2f, 0.2f);
    _gui_draw_rect(ctx, r->x + r->width - 1, r->y, 1, r->height, 0.2f, 0.2f, 0.2f);
    
    // Text placeholder
    int text_len = (int)strlen(control->data.button.text);
    if (text_len > 0)
    {
        int text_w = text_len * 6;
        int text_h = r->height / 2;
        if (text_h < 22) text_h = 22;
        int text_x = r->x + (r->width - text_w) / 2;
        int text_y = r->y + (r->height - text_h) / 2;
        _gui_draw_rect(ctx, text_x, text_y, text_w, text_h, 1.0f, 1.0f, 1.0f);
    }
}

static void _gui_draw_list(gui_draw_context_t* ctx, gui_control_t control)
{
    gui_control_rect_t* r = &control->rect;
    
    // Background
    _gui_draw_rect(ctx, r->x, r->y, r->width, r->height, 0.1f, 0.1f, 0.1f);
    
    // Items
    gui_list_t* list = &control->data.list;
    
    // Distribute items evenly across available height
    uint32_t visible_count = list->item_count;
    if (visible_count == 0) visible_count = 1;
    int item_h = r->height / (int)visible_count;
    if (item_h < 28) item_h = 28;
    
    int y_offset = 0;
    for (uint32_t i = list->scroll_offset; i < list->item_count && y_offset + item_h <= r->height; i++)
    {
        int item_y = r->y + y_offset;
        bool selected = (i == (uint32_t)list->selected_index);
        
        if (selected)
        {
            _gui_draw_rect(ctx, r->x + 1, item_y, r->width - 2, item_h, 0.3f, 0.5f, 0.8f);
        }
        
        int text_len = (int)strlen(list->items[i].text);
        if (text_len > 0)
        {
            int text_w = text_len * 6;
            int text_h = item_h / 2;
            if (text_h < 22) text_h = 22;
            int text_x = r->x + 5;
            int text_y = item_y + (item_h - text_h) / 2;
            _gui_draw_rect(ctx, text_x, text_y, text_w, text_h, 0.9f, 0.9f, 0.9f);
        }
        
        y_offset += item_h;
    }
    
    // Border
    _gui_draw_rect(ctx, r->x, r->y, r->width, 1, 0.4f, 0.4f, 0.4f);
    _gui_draw_rect(ctx, r->x, r->y + r->height - 1, r->width, 1, 0.4f, 0.4f, 0.4f);
    _gui_draw_rect(ctx, r->x, r->y, 1, r->height, 0.4f, 0.4f, 0.4f);
    _gui_draw_rect(ctx, r->x + r->width - 1, r->y, 1, r->height, 0.4f, 0.4f, 0.4f);
}

static void _gui_draw_entry(gui_draw_context_t* ctx, gui_control_t control)
{
    gui_control_rect_t* r = &control->rect;
    
    // Background
    float bg = control->state.focused ? 0.25f : 0.15f;
    _gui_draw_rect(ctx, r->x, r->y, r->width, r->height, bg, bg, bg);
    
    // Border
    float border = control->state.focused ? 0.6f : 0.3f;
    _gui_draw_rect(ctx, r->x, r->y, r->width, 1, border, border, border);
    _gui_draw_rect(ctx, r->x, r->y + r->height - 1, r->width, 1, border, border, border);
    _gui_draw_rect(ctx, r->x, r->y, 1, r->height, border, border, border);
    _gui_draw_rect(ctx, r->x + r->width - 1, r->y, 1, r->height, border, border, border);
    
    // Text placeholder
    int text_len = (int)strlen(control->data.entry.text);
    if (text_len > 0)
    {
        int text_w = text_len * 6;
        _gui_draw_rect(ctx, r->x + 5, r->y + (r->height - 8) / 2, text_w, 8, 0.9f, 0.9f, 0.9f);
    }
    
    // Cursor
    if (control->state.focused)
    {
        int cursor_x = r->x + 5 + text_len * 6;
        _gui_draw_rect(ctx, cursor_x, r->y + 3, 1, r->height - 6, 1.0f, 1.0f, 1.0f);
    }
}

// === Main Draw Function ===

void gui_controls_draw_internal(gui_controls_t controls, VkCommandBuffer cmd, VkDevice device, VkPhysicalDevice physical_device, uint32_t screen_width, uint32_t screen_height)
{
    if (!controls) return;
    
    gui_draw_context_t ctx;
    ctx.vertex_count = 0;
    ctx.vertex_capacity = 128;
    ctx.screen_width = screen_width;
    ctx.screen_height = screen_height;
    ctx.vertices = malloc(ctx.vertex_capacity * 5 * sizeof(float));
    
    if (!ctx.vertices) return;
    
    gui_control_t current = controls->head;
    while (current)
    {
        if (current->state.visible)
        {
            switch (current->type)
            {
                case GUI_CONTROL_LABEL:
                    _gui_draw_label(&ctx, current);
                    break;
                case GUI_CONTROL_BUTTON:
                    _gui_draw_button(&ctx, current);
                    break;
                case GUI_CONTROL_LIST:
                    _gui_draw_list(&ctx, current);
                    break;
                case GUI_CONTROL_ENTRY:
                    _gui_draw_entry(&ctx, current);
                    break;
            }
        }
        current = current->next;
    }
    
    // Flush
    if (ctx.vertex_count > 0)
    {
        VkDeviceSize size = ctx.vertex_count * 5 * sizeof(float);
        
        VkBufferCreateInfo buffer_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };
        
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_memory;
        vkCreateBuffer(device, &buffer_info, NULL, &vertex_buffer);
        
        VkMemoryRequirements mem_req;
        vkGetBufferMemoryRequirements(device, vertex_buffer, &mem_req);
        
        VkMemoryAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = mem_req.size,
            .memoryTypeIndex = 0
        };
        
        VkPhysicalDeviceMemoryProperties mem_props;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);
        for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++)
        {
            if ((mem_req.memoryTypeBits & (1 << i)) && (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            {
                alloc_info.memoryTypeIndex = i;
                break;
            }
        }
        
        vkAllocateMemory(device, &alloc_info, NULL, &vertex_memory);
        vkBindBufferMemory(device, vertex_buffer, vertex_memory, 0);
        
        void* data;
        vkMapMemory(device, vertex_memory, 0, size, 0, &data);
        memcpy(data, ctx.vertices, (size_t)size);
        vkUnmapMemory(device, vertex_memory);
        
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertex_buffer, &offset);
        vkCmdDraw(cmd, ctx.vertex_count, 1, 0, 0);
        
        // Defer destruction - buffer is still needed by async GPU execution
        gui_pending_buffer_t* pending = (gui_pending_buffer_t*)malloc(sizeof(gui_pending_buffer_t));
        if (pending)
        {
            pending->buffer = vertex_buffer;
            pending->memory = vertex_memory;
            pending->next = _pending_buffers;
            _pending_buffers = pending;
        }
        else
        {
            // Fallback if malloc fails
            vkDestroyBuffer(device, vertex_buffer, NULL);
            vkFreeMemory(device, vertex_memory, NULL);
        }
    }
    
    free(ctx.vertices);
}
