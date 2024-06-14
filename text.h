#pragma once

#include <gtk/gtk.h>

#define REG_EXP_UNSIGNED_INTEGER    "[0]{0,1}|[1-9]{1}[0-9]{0,}"
#define REG_EXP_SIGNED_INTEGER      "[0-]{0,1}|[-]{0,1}[1-9][0-9]{0,}"
#define REG_EXP_FLOAT               "[0]{0,1}|[0][,]{0,1}|[0][,][0-9]{1,}|"\
                                    "[-]{0,1}|[-][0]{0,1}|[-][0][,]{0,1}|[-][0][,][0-9]{1,}|"\
                                    "[1-9]|[1-9][0-9]{1,}|[1-9][0-9]{1,}[,]{0,1}|[1-9][0-9]{1,}[,][0-9]{1,}|"\
                                    "[-][1-9]{1,}|[-][1-9][0-9]{1,}|[-][1-9][0-9]{1,}[,]{0,1}|[-][1-9][0-9]{1,}[,][0-9]{1,}|"\
                                    "[1-9][,]{0,1}|[1-9][,][0-9]{1,}|"\
                                    "[-][1-9][,]{0,1}|[-][1-9][,][0-9]{1,}"

typedef struct _gui_text
{
    GtkWidget* text;
    uint32_t id;
    const char* regular_expression;
    void* user_data;
} *gui_text_t;

GtkWidget* gui_text_create(uint32_t id, float alignment, const char* white_list, const char* value, void* user_data);
