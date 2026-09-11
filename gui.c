#include "gui/gui.h"

#include <api/api.h>
#include <logging/logging.h>
#define MODULE_ID "GUI"
#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>

void* _gui_get_core(GtkWidget* widget);

typedef struct _gui_widgets_list_element
{
	GtkWidget* widget;
	LIST_ENTRY(_gui_widgets_list_element) elements;
} *_gui_widgets_list_element_t;

LIST_HEAD(_gui_widgets_list_head, _gui_widgets_list_element);

static struct _gui_widgets_list_head _widgets_list_head = {0};

// ============================================================================
// Widget-Core Verwaltung
// ============================================================================

static void _gui_destroy_widget_core(GtkWidget* widget)
{
	if (!widget)
	{
		LOG(MODULE_ID, "_gui_destroy_widget_core: widget is NULL");
		return;
	}

	void* core = _gui_get_core(widget);
	if (core)
	{
		LOG(MODULE_ID, "_gui_destroy_widget_core: freeing core %p for widget %p", core, widget);
		free(core);
		g_object_set_data(G_OBJECT(widget), "core", NULL);
	}
}

// ============================================================================
// Widget-Liste verwalten
// ============================================================================

void _gui_add_widget_to_internal_list(GtkWidget* widget)
{
	if (!widget)
	{
		LOG(MODULE_ID, "_gui_add_widget_to_internal_list: widget is NULL");
		return;
	}

	_gui_widgets_list_element_t entry = malloc(sizeof(struct _gui_widgets_list_element));
	if (!entry)
	{
		LOG(MODULE_ID, "_gui_add_widget_to_internal_list: failed to allocate list element");
		return;
	}

	entry->widget = widget;
	LIST_INSERT_HEAD(&_widgets_list_head, entry, elements);
	LOG(MODULE_ID, "_gui_add_widget_to_internal_list: added widget %p", widget);
}

void _gui_remove_widget_from_internal_list(GtkWidget* widget)
{
	if (!widget)
	{
		LOG(MODULE_ID, "_gui_remove_widget_from_internal_list: widget is NULL");
		return;
	}

	_gui_widgets_list_element_t entry = NULL;
	LIST_FOREACH(entry, &_widgets_list_head, elements)
	{
		if (entry->widget == widget)
		{
			LIST_REMOVE(entry, elements);
			_gui_destroy_widget_core(widget);
			free(entry);
			LOG(MODULE_ID, "_gui_remove_widget_from_internal_list: removed widget %p", widget);
			return;
		}
	}

	LOG(MODULE_ID, "_gui_remove_widget_from_internal_list: widget %p not found", widget);
}

void* _gui_get_core(GtkWidget* widget)
{
	if (!widget)
	{
		return NULL;
	}
	return g_object_get_data(G_OBJECT(widget), "core");
}

void _gui_destroy_all_widget_cores()
{
	LOG(MODULE_ID, "_gui_destroy_all_widget_cores: ENTER");

	_gui_widgets_list_element_t entry = NULL;
	LIST_FOREACH(entry, &_widgets_list_head, elements)
	{
		if (entry->widget != NULL)
		{
			if (G_IS_OBJECT(entry->widget))
			{
				_gui_destroy_widget_core(entry->widget);
			}
			entry->widget = NULL;
		}
	}

	entry = LIST_FIRST(&_widgets_list_head);
	while (entry != NULL)
	{
		_gui_widgets_list_element_t next = LIST_NEXT(entry, elements);
		free(entry);
		entry = next;
	}
	LIST_INIT(&_widgets_list_head);

	LOG(MODULE_ID, "_gui_destroy_all_widget_cores: SUCCESS");
}
