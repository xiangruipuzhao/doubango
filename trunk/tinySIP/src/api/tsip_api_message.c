/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou@yahoo.fr>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tsip_api_message.c
 * @brief Public short messaging (MESSAGE) functions.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#include "tinySIP/api/tsip_api_message.h"

#include "tinySIP/dialogs/tsip_dialog_layer.h"
#include "tinySIP/dialogs/tsip_dialog_message.h"

#include "tsip.h"

#include "tsk_runnable.h"
#include "tsk_debug.h"

#define TSIP_MESSAGE_EVENT_CREATE( type)		tsk_object_new(tsip_message_event_def_t, type)

int tsip_message_event_signal(tsip_message_event_type_t type, struct tsip_stack_s *stack, tsip_operation_handle_t* operation, short status_code, const char *phrase, const tsip_message_t* sipmessage)
{
	tsip_message_event_t* sipevent = TSIP_MESSAGE_EVENT_CREATE(type);
	tsip_event_init(TSIP_EVENT(sipevent), stack, operation, status_code, phrase, sipmessage, tsip_event_message);

	TSK_RUNNABLE_ENQUEUE_OBJECT(TSK_RUNNABLE(stack), sipevent);

	return 0;
}

int tsip_message(tsip_stack_handle_t *_stack, const tsip_operation_handle_t *operation)
{
	int ret = -1;

	if(_stack && operation)
	{
		tsip_stack_t *stack = _stack;
		tsip_dialog_message_t *dialog;
		
		dialog = (tsip_dialog_message_t*)tsip_dialog_layer_find_by_op(stack->layer_dialog, operation);
		if(dialog){
			TSK_DEBUG_WARN("Dialog with same opid already exist.");
			ret = -2;
			goto bail;
		}
		else{
			dialog = TSIP_DIALOG_MESSAGE_CREATE(stack, operation);
			ret = tsip_dialog_message_start(dialog);
			tsk_list_push_back_data(stack->layer_dialog->dialogs, (void**)&dialog);
		}
	}

bail:
	return ret;
}













//========================================================
//	SIP MESSAGE event object definition
//
static void* tsip_message_event_create(void * self, va_list * app)
{
	tsip_message_event_t *sipevent = self;
	if(sipevent)
	{
		sipevent->type = va_arg(*app, tsip_message_event_type_t);
	}
	return self;
}

static void* tsip_message_event_destroy(void * self)
{ 
	tsip_message_event_t *sipevent = self;
	if(sipevent)
	{
		tsip_event_deinit(TSIP_EVENT(sipevent));
	}
	return self;
}

static int tsip_message_event_cmp(const void *obj1, const void *obj2)
{
	return -1;
}

static const tsk_object_def_t tsip_message_event_def_s = 
{
	sizeof(tsip_message_event_t),
	tsip_message_event_create, 
	tsip_message_event_destroy,
	tsip_message_event_cmp, 
};
const void *tsip_message_event_def_t = &tsip_message_event_def_s;
