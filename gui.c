#include "gui/gui.h"

#include <logging/logging.h>
#include <sys/queue.h>

void* _gui_get_core(GtkWidget* widget);

/*------------------------------------------------- PRIVATE ------------------------------------------------------*/

typedef struct _gui_widgets_list_element
{
	GtkWidget* widget;
	LIST_ENTRY(_gui_widgets_list_element) elements;
} *_gui_widgets_list_element_t;

LIST_HEAD(_gui_widgets_list_head, _gui_widgets_list_element);

static struct _gui_widgets_list_head _widgets_list_head = {0};

void _gui_destroy_widget_core(GtkWidget* widget)
{
	free(_gui_get_core(widget));
	g_object_set_data(G_OBJECT(widget), "core", NULL);
}

/*----------------------------------------------- PROTECTED -----------------------------------------------------*/

// extern void _gui_destroy_all_widget_cores();
void _gui_destroy_all_widget_cores()
{
	_gui_widgets_list_element_t entry = NULL;
	LIST_FOREACH(entry, &_widgets_list_head, elements)
	{
		if (entry->widget != NULL)
		{
			_gui_destroy_widget_core(entry->widget);
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
}

// extern void _gui_add_widget_to_internal_list(GtkWidget* widget);
void _gui_add_widget_to_internal_list(GtkWidget* widget)
{
	_gui_widgets_list_element_t entry = malloc(sizeof(struct _gui_widgets_list_element));
	entry->widget = widget;
	LIST_INSERT_HEAD(&_widgets_list_head, entry, elements);
}

// extern void* _gui_get_core(GtkWidget* widget);
void* _gui_get_core(GtkWidget* widget)
{
	return g_object_get_data(G_OBJECT(widget), "core");
}

// extern gboolean _gui_debug_event_callback(GtkEventControllerLegacy* self, GdkEvent* event, gpointer user_data);
gboolean _gui_debug_event_callback(GtkEventControllerLegacy* self, GdkEvent* event, gpointer user_data)
{
    gboolean handled = FALSE;
    GdkEventType type = gdk_event_get_event_type(event);

	switch (type)
	{
		case GDK_DELETE:
			logging_log_message("GDK_DELETE", true);
			break;

		case GDK_MOTION_NOTIFY:
			logging_log_message("GDK_MOTION_NOTIFY", true);
			break;

		case GDK_BUTTON_PRESS:
			logging_log_message("GDK_BUTTON_PRESS", true);
			break;

		case GDK_BUTTON_RELEASE:
			logging_log_message("GDK_BUTTON_RELEASE", true);
			break;

		case GDK_KEY_PRESS:
			logging_log_message("GDK_KEY_PRESS", true);
			break;

		case GDK_KEY_RELEASE:
			logging_log_message("GDK_KEY_RELEASE", true);
			break;

		case GDK_ENTER_NOTIFY:
			logging_log_message("GDK_ENTER_NOTIFY", true);
			break;

		case GDK_LEAVE_NOTIFY:
			logging_log_message("GDK_LEAVE_NOTIFY", true);
			break;

		case GDK_FOCUS_CHANGE:
			logging_log_message("GDK_FOCUS_CHANGE", true);
			break;

		case GDK_PROXIMITY_IN:
			logging_log_message("GDK_PROXIMITY_IN", true);
			break;

		case GDK_PROXIMITY_OUT:
			logging_log_message("GDK_PROXIMITY_OUT", true);
			break;

		case GDK_DRAG_ENTER:
			logging_log_message("GDK_DRAG_ENTER", true);
			break;

		case GDK_DRAG_LEAVE:
			logging_log_message("GDK_DRAG_LEAVE", true);
			break;

		case GDK_DRAG_MOTION:
			logging_log_message("GDK_DRAG_MOTION", true);
			break;

		case GDK_DROP_START:
			logging_log_message("GDK_DROP_START", true);
			break;

		case GDK_SCROLL:
			logging_log_message("GDK_SCROLL", true);
			break;

		case GDK_GRAB_BROKEN:
			logging_log_message("GDK_GRAB_BROKEN", true);
			break;

		case GDK_TOUCH_BEGIN:
			logging_log_message("GDK_TOUCH_BEGIN", true);
			break;

		case GDK_TOUCH_UPDATE:
			logging_log_message("GDK_TOUCH_UPDATE", true);
			break;

		case GDK_TOUCH_END:
			logging_log_message("GDK_TOUCH_END", true);
			break;

		case GDK_TOUCH_CANCEL:
			logging_log_message("GDK_TOUCH_CANCEL", true);
			break;

		case GDK_TOUCHPAD_SWIPE:
			logging_log_message("GDK_TOUCHPAD_SWIPE", true);
			break;

		case GDK_TOUCHPAD_PINCH:
			logging_log_message("GDK_TOUCHPAD_PINCH", true);
			break;

		case GDK_PAD_BUTTON_PRESS:
			logging_log_message("GDK_PAD_BUTTON_PRESS", true);
			break;

		case GDK_PAD_BUTTON_RELEASE:
			logging_log_message("GDK_PAD_BUTTON_RELEASE", true);
			break;

		case GDK_PAD_RING:
			logging_log_message("GDK_PAD_RING", true);
			break;

		case GDK_PAD_STRIP:
			logging_log_message("GDK_PAD_STRIP", true);
			break;

		case GDK_PAD_GROUP_MODE:
			logging_log_message("GDK_PAD_GROUP_MODE", true);
			break;

		case GDK_TOUCHPAD_HOLD:
			logging_log_message("GDK_TOUCHPAD_HOLD", true);
			break;

		case GDK_EVENT_LAST:
			logging_log_message("GDK_EVENT_LAST", true);
			break;
	}

    return handled;
}

/*------------------------------------------------- PUBLIC ------------------------------------------------------*/
