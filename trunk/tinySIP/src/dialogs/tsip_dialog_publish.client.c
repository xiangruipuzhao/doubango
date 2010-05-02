/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as publishd by
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

/**@file tsip_dialog_publish.client.c
 * @brief SIP dialog PUBLISH as per RFC 3903.
 *
 * @author Mamadou Diop <diopmamadou(at)doubango.org>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#include "tinySIP/dialogs/tsip_dialog_publish.h"

#include "tinySIP/headers/tsip_header_Dummy.h"
#include "tinySIP/headers/tsip_header_Min_Expires.h"
#include "tinySIP/headers/tsip_header_SIP_ETag.h"
#include "tinySIP/headers/tsip_header_SIP_If_Match.h"

#include "tinySIP/transactions/tsip_transac_layer.h"

#include "tinySIP/tsip_message.h"

#include "tinySIP/api/tsip_api_publish.h"

#include "tsk_debug.h"
#include "tsk_time.h"

/**@defgroup tsip_dialog_publish_group SIP dialog PUBLISH (Client side) as per RFC 3265.
*/

#define DEBUG_STATE_MACHINE											1
#define TSIP_DIALOG_PUBLISH_TIMER_SCHEDULE(TX)						TSIP_DIALOG_TIMER_SCHEDULE(publish, TX)
#define TSIP_DIALOG_PUBLISH_SIGNAL(self, type, code, phrase, message)	\
	tsip_publish_event_signal(type, TSIP_DIALOG_GET_STACK(self), TSIP_DIALOG(self)->ss, code, phrase, message)

/* ======================== internal functions ======================== */
int send_PUBLISH(tsip_dialog_publish_t *self);
int tsip_dialog_publish_OnTerminated(tsip_dialog_publish_t *self);

/* ======================== transitions ======================== */
int tsip_dialog_publish_Started_2_Trying_X_publish(va_list *app);
int tsip_dialog_publish_Trying_2_Trying_X_1xx(va_list *app);
int tsip_dialog_publish_Trying_2_Terminated_X_2xx(va_list *app);
int tsip_dialog_publish_Trying_2_Connected_X_2xx(va_list *app);
int tsip_dialog_publish_Trying_2_Trying_X_401_407_421_494(va_list *app);
int tsip_dialog_publish_Trying_2_Trying_X_423(va_list *app);
int tsip_dialog_publish_Trying_2_Terminated_X_300_to_699(va_list *app);
int tsip_dialog_publish_Trying_2_Terminated_X_cancel(va_list *app);
int tsip_dialog_publish_Connected_2_Trying_X_publish(va_list *app);
int tsip_dialog_publish_Any_2_Trying_X_hangup(va_list *app);
int tsip_dialog_publish_Any_2_Terminated_X_transportError(va_list *app);
int tsip_dialog_publish_Any_2_Terminated_X_Error(va_list *app);

/* ======================== conds ======================== */
int _fsm_cond_unpublishing(tsip_dialog_publish_t* dialog, tsip_message_t* message)
{
	return dialog->unpublishing ? 1 : 0;
}
int _fsm_cond_publishing(tsip_dialog_publish_t* dialog, tsip_message_t* message)
{
	return !_fsm_cond_unpublishing(dialog, message);
}


/* ======================== actions ======================== */
typedef enum _fsm_action_e
{
	_fsm_action_publish = atype_publish,
	_fsm_action_cancel = atype_cancel,
	_fsm_action_hangup = atype_unpublish,
	_fsm_action_shutdown = atype_shutdown,

	_fsm_action_1xx = 0xFF,
	_fsm_action_2xx,
	_fsm_action_401_407_421_494,
	_fsm_action_423,
	_fsm_action_300_to_699,	
	_fsm_action_transporterror,
	_fsm_action_error,
}
_fsm_action_t;

/* ======================== states ======================== */
typedef enum _fsm_state_e
{
	_fsm_state_Started,
	_fsm_state_Trying,
	_fsm_state_Connected,
	_fsm_state_Terminated
}
_fsm_state_t;


/**
 * 	Callback function called to alert the dialog for new events from the transaction/transport layers.
 *
 * @param [in,out]	self	A reference to the dialog.
 * @param	type		The event type. 
 * @param [in,out]	msg	The incoming SIP/IMS message. 
 *
 * @return	Zero if succeed and non-zero error code otherwise. 
**/
int tsip_dialog_publish_event_callback(const tsip_dialog_publish_t *self, tsip_dialog_event_type_t type, const tsip_message_t *msg)
{
	int ret = -1;

	switch(type)
	{
	case tsip_dialog_i_msg:
		{
			if(msg && TSIP_MESSAGE_IS_RESPONSE(msg))
			{
				//
				//	RESPONSE
				//
				if(TSIP_RESPONSE_IS_1XX(msg)){
					ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_1xx, msg, tsk_null);
				}
				else if(TSIP_RESPONSE_IS_2XX(msg)){
					ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_2xx, msg, tsk_null);
				}
				else if(TSIP_RESPONSE_IS(msg,401) || TSIP_RESPONSE_IS(msg,407) || TSIP_RESPONSE_IS(msg,421) || TSIP_RESPONSE_IS(msg,494)){
					ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_401_407_421_494, msg, tsk_null);
				}
				else if(TSIP_RESPONSE_IS(msg,423)){
					ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_423, msg, tsk_null);
				}
				else{
					// Alert User
					ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_error, msg, tsk_null);
					/* TSK_DEBUG_WARN("Not supported status code: %d", TSIP_RESPONSE_CODE(msg)); */
				}
			}
			else
			{
				//
				//	REQUEST
				//
			}
			break;
		}

	case tsip_dialog_canceled:
		{
			ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_cancel, msg, tsk_null);
			break;
		}

	case tsip_dialog_terminated:
	case tsip_dialog_timedout:
	case tsip_dialog_error:
	case tsip_dialog_transport_error:
		{
			ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_transporterror, msg, tsk_null);
			break;
		}
	}
	
	return ret;
}

/**
 * Timer manager callback.
 *
 * @param [in,out]	self	The owner of the signaled timer. 
 * @param	timer_id		The identifier of the signaled timer.
 *
 * @return	Zero if succeed and non-zero error code otherwise.  
**/
int tsip_dialog_publish_timer_callback(const tsip_dialog_publish_t* self, tsk_timer_id_t timer_id)
{
	int ret = -1;

	if(self)
	{
		if(timer_id == self->timerrefresh.id){
			tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_publish, tsk_null, tsk_null);
			ret = 0;
		}
		else if(timer_id == self->timershutdown.id){
			ret = tsip_dialog_fsm_act(TSIP_DIALOG(self), _fsm_action_error, tsk_null, tsk_null);
		}
	}
	return ret;
}


tsip_dialog_publish_t* tsip_dialog_publish_create(const tsip_ssession_handle_t* ss)
{
	return tsk_object_new(tsip_dialog_publish_def_t, ss);
}

/**
 * Initializes the dialog.
 *
 * @param [in,out]	self	The dialog to initialize. 
**/
int tsip_dialog_publish_init(tsip_dialog_publish_t *self)
{	
	/* Initialize the State Machine. */
	tsk_fsm_set(TSIP_DIALOG_GET_FSM(self),
			
			/*=======================
			* === Started === 
			*/
			// Started -> (PUBLISH) -> Trying
			TSK_FSM_ADD_ALWAYS(_fsm_state_Started, _fsm_action_publish, _fsm_state_Trying, tsip_dialog_publish_Started_2_Trying_X_publish, "tsip_dialog_publish_Started_2_Trying_X_publish"),
			// Started -> (Any) -> Started
			TSK_FSM_ADD_ALWAYS_NOTHING(_fsm_state_Started, "tsip_dialog_publish_Started_2_Started_X_any"),
			

			/*=======================
			* === Trying === 
			*/
			// Trying -> (1xx) -> Trying
			TSK_FSM_ADD_ALWAYS(_fsm_state_Trying, _fsm_action_1xx, _fsm_state_Trying, tsip_dialog_publish_Trying_2_Trying_X_1xx, "tsip_dialog_publish_Trying_2_Trying_X_1xx"),
			// Trying -> (2xx) -> Terminated
			TSK_FSM_ADD(_fsm_state_Trying, _fsm_action_2xx, _fsm_cond_unpublishing, _fsm_state_Terminated, tsip_dialog_publish_Trying_2_Terminated_X_2xx, "tsip_dialog_publish_Trying_2_Terminated_X_2xx"),
			// Trying -> (2xx) -> Connected
			TSK_FSM_ADD(_fsm_state_Trying, _fsm_action_2xx, _fsm_cond_publishing, _fsm_state_Connected, tsip_dialog_publish_Trying_2_Connected_X_2xx, "tsip_dialog_publish_Trying_2_Connected_X_2xx"),
			// Trying -> (401/407/421/494) -> Trying
			TSK_FSM_ADD_ALWAYS(_fsm_state_Trying, _fsm_action_401_407_421_494, _fsm_state_Trying, tsip_dialog_publish_Trying_2_Trying_X_401_407_421_494, "tsip_dialog_publish_Trying_2_Trying_X_401_407_421_494"),
			// Trying -> (423) -> Trying
			TSK_FSM_ADD_ALWAYS(_fsm_state_Trying, _fsm_action_423, _fsm_state_Trying, tsip_dialog_publish_Trying_2_Trying_X_423, "tsip_dialog_publish_Trying_2_Trying_X_423"),
			// Trying -> (300_to_699) -> Terminated
			TSK_FSM_ADD_ALWAYS(_fsm_state_Trying, _fsm_action_300_to_699, _fsm_state_Terminated, tsip_dialog_publish_Trying_2_Terminated_X_300_to_699, "tsip_dialog_publish_Trying_2_Terminated_X_300_to_699"),
			// Trying -> (cancel) -> Terminated
			TSK_FSM_ADD_ALWAYS(_fsm_state_Trying, _fsm_action_cancel, _fsm_state_Terminated, tsip_dialog_publish_Trying_2_Terminated_X_cancel, "tsip_dialog_publish_Trying_2_Terminated_X_cancel"),
			// Trying -> (Any) -> Trying
			// TSK_FSM_ADD_ALWAYS_NOTHING(_fsm_state_Trying, "tsip_dialog_publish_Trying_2_Trying_X_any"),


			/*=======================
			* === Connected === 
			*/
			// Connected -> (PUBLISH) -> Trying
			TSK_FSM_ADD_ALWAYS(_fsm_state_Connected, _fsm_action_publish, _fsm_state_Trying, tsip_dialog_publish_Connected_2_Trying_X_publish, "tsip_dialog_publish_Connected_2_Trying_X_publish"),
			
			/*=======================
			* === Any === 
			*/
			// Any -> (transport error) -> Terminated
			TSK_FSM_ADD_ALWAYS(tsk_fsm_state_any, _fsm_action_transporterror, _fsm_state_Terminated, tsip_dialog_publish_Any_2_Terminated_X_transportError, "tsip_dialog_publish_Any_2_Terminated_X_transportError"),
			// Any -> (transport error) -> Terminated
			TSK_FSM_ADD_ALWAYS(tsk_fsm_state_any, _fsm_action_error, _fsm_state_Terminated, tsip_dialog_publish_Any_2_Terminated_X_Error, "tsip_dialog_publish_Any_2_Terminated_X_Error"),
			// Any -> (hangup) -> Trying
			TSK_FSM_ADD_ALWAYS(tsk_fsm_state_any, _fsm_action_hangup, _fsm_state_Trying, tsip_dialog_publish_Any_2_Trying_X_hangup, "tsip_dialog_publish_Any_2_Trying_X_hangup"),

			TSK_FSM_ADD_NULL());

	/* Sets callback function */
	TSIP_DIALOG(self)->callback = TSIP_DIALOG_EVENT_CALLBACK_F(tsip_dialog_publish_event_callback);

	/* Timers */
	self->timerrefresh.id = TSK_INVALID_TIMER_ID;
	self->timerrefresh.timeout = TSIP_DIALOG(self)->expires;
	self->timershutdown.id = TSK_INVALID_TIMER_ID;
	self->timershutdown.timeout = TSIP_DIALOG_SHUTDOWN_TIMEOUT;

	return 0;
}

//--------------------------------------------------------
//				== STATE MACHINE BEGIN ==
//--------------------------------------------------------


/* Started -> (send) -> Trying
*/
int tsip_dialog_publish_Started_2_Trying_X_publish(va_list *app)
{
	tsip_dialog_publish_t *self;
	const tsip_action_t* action;

	self = va_arg(*app, tsip_dialog_publish_t *);
	va_arg(*app, const tsip_message_t *);
	action = va_arg(*app, const tsip_action_t *);

	TSIP_DIALOG(self)->running = tsk_true;
	tsip_dialog_set_curr_action(TSIP_DIALOG(self), action);

	return send_PUBLISH(self);
}

/* Trying -> (1xx) -> Trying
*/
int tsip_dialog_publish_Trying_2_Trying_X_1xx(va_list *app)
{
	/*tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);*/
	/*const tsip_response_t *response = va_arg(*app, const tsip_response_t *);*/

	return 0;
}

/* Trying -> (2xx) -> Terminated
*/
int tsip_dialog_publish_Trying_2_Terminated_X_2xx(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);

	/* Alert the user. */
	TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
		TSIP_RESPONSE_CODE(response), TSIP_RESPONSE_PHRASE(response), response);

	return 0;
}

/* Trying -> (2xx) -> Connected
*/
int tsip_dialog_publish_Trying_2_Connected_X_2xx(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);
	int ret;

	/*	RFC 3903 - 4.1.  Identification of Published Event State
		For each successful PUBLISH request, the ESC will generate and assign
		an entity-tag and return it in the SIP-ETag header field of the 2xx
		response.
	*/
	const tsip_header_SIP_ETag_t *SIP_ETag;
	if((SIP_ETag = (const tsip_header_SIP_ETag_t*)tsip_message_get_header(response, tsip_htype_SIP_ETag))){
		tsk_strupdate(&self->etag, SIP_ETag->value);
	}

	/* Alert the user. */
	TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
		TSIP_RESPONSE_CODE(response), TSIP_RESPONSE_PHRASE(response), response);

	/* Update the dialog state. */
	if((ret = tsip_dialog_update(TSIP_DIALOG(self), response))){
		return ret;
	}

	/* Reset current action */
	tsip_dialog_set_curr_action(TSIP_DIALOG(self), tsk_null);

	/* Request timeout for dialog refresh (re-publish). */
	self->timerrefresh.timeout = tsip_dialog_get_newdelay(TSIP_DIALOG(self), response);
	TSIP_DIALOG_PUBLISH_TIMER_SCHEDULE(refresh);

	return 0;
}

/* Trying -> (401/407/421/494) -> Trying
*/
int tsip_dialog_publish_Trying_2_Trying_X_401_407_421_494(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);
	int ret;

	if((ret = tsip_dialog_update(TSIP_DIALOG(self), response))){
		/* Alert the user. */
		TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
			TSIP_RESPONSE_CODE(response), TSIP_RESPONSE_PHRASE(response), response);
		
		return ret;
	}
	
	return send_PUBLISH(self);
}

/* Trying -> (423) -> Trying
*/
int tsip_dialog_publish_Trying_2_Trying_X_423(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);

	tsip_header_Min_Expires_t *hdr;

	/*
	RFC 3261 - 10.2.8 Error Responses

	If a UA receives a 423 (Interval Too Brief) response, it MAY retry
	the registration after making the expiration interval of all contact
	addresses in the PUBLISH request equal to or greater than the
	expiration interval within the Min-Expires header field of the 423
	(Interval Too Brief) response.
	*/
	hdr = (tsip_header_Min_Expires_t*)tsip_message_get_header(response, tsip_htype_Min_Expires);
	if(hdr){
		TSIP_DIALOG(self)->expires = TSK_TIME_S_2_MS(hdr->value);
		send_PUBLISH(self);
	}
	else{
		TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
			715, "Invalid SIP response.", response);

		return -1;
	}

	return 0;
}

/* Trying -> (300 to 699) -> Terminated
*/
int tsip_dialog_publish_Trying_2_Terminated_X_300_to_699(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);

	/* Alert the user. */
	TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish,  
		TSIP_RESPONSE_CODE(response), TSIP_RESPONSE_PHRASE(response), response);

	return 0;
}

/* Trying -> (cancel) -> Terminated
*/
int tsip_dialog_publish_Trying_2_Terminated_X_cancel(va_list *app)
{
	int ret;
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	/* const tsip_message_t *message = va_arg(*app, const tsip_message_t *); */

	/* Cancel all transactions associated to this dialog (will also be one when the dialog is destroyed (worth nothing)) */
	ret = tsip_transac_layer_cancel_by_dialog(TSIP_DIALOG_GET_STACK(self)->layer_transac, TSIP_DIALOG(self));

	/* Alert the user. */
	TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
		701, "Subscription cancelled", tsk_null);

	return ret;
}

/* Connected -> (PUBLISH) -> Trying
*/
int tsip_dialog_publish_Connected_2_Trying_X_publish(va_list *app)
{
	tsip_dialog_publish_t *self;
	const tsip_action_t* action;

	self = va_arg(*app, tsip_dialog_publish_t *);
	va_arg(*app, const tsip_message_t *);
	action = va_arg(*app, const tsip_action_t *);
	
	/* Set  current action */
	tsip_dialog_set_curr_action(TSIP_DIALOG(self), action);

	return send_PUBLISH(self);
}

/* Connected -> (hangup) -> Trying
*/
int tsip_dialog_publish_Any_2_Trying_X_hangup(va_list *app)
{
	tsip_dialog_publish_t *self;
	const tsip_action_t* action;

	self = va_arg(*app, tsip_dialog_publish_t *);
	va_arg(*app, const tsip_message_t *);
	action = va_arg(*app, const tsip_action_t *);
	
	///* Schedule timeout (shutdown). */
	//if(shuttingdown){
	//	TSIP_DIALOG_PUBLISH_TIMER_SCHEDULE(shutdown);
	//}
	
	//self->unpublishing = tsk_true;
	//return send_PUBLISH(self, rt_remove);
	
	/* Set  current action */
	tsip_dialog_set_curr_action(TSIP_DIALOG(self), action);
	
	self->unpublishing = tsk_true;
	return send_PUBLISH(self);
}

/* Any -> (transport error) -> Terminated
*/
int tsip_dialog_publish_Any_2_Terminated_X_transportError(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	/*const tsip_message_t *message = va_arg(*app, const tsip_message_t *);*/

	/* Alert the user. */
	TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
		702, "Transport error.", tsk_null);

	return 0;
}

/* Any -> (Error) -> Terminated
*/
int tsip_dialog_publish_Any_2_Terminated_X_Error(va_list *app)
{
	tsip_dialog_publish_t *self = va_arg(*app, tsip_dialog_publish_t *);
	const tsip_response_t *response = va_arg(*app, const tsip_response_t *);

	/* Alert the user. */
	if(response){
		TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
				TSIP_RESPONSE_CODE(response), TSIP_RESPONSE_PHRASE(response), response);
	}
	else{
		TSIP_DIALOG_PUBLISH_SIGNAL(self, self->unpublishing ? tsip_ao_unpublish : tsip_ao_publish, 
			703, "Global error.", tsk_null);
	}

	return 0;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//				== STATE MACHINE END ==
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/**
 * Sends a PUBLISH request. 
**/
int send_PUBLISH(tsip_dialog_publish_t *self)
{
	tsip_request_t *request = tsk_null;
	int ret = -1;
	const tsip_action_t* action;
	
	if(!self){
		return -1;
	}
	
	// action
	action = TSIP_DIALOG(self)->curr_action;

	if(self->unpublishing){
		TSIP_DIALOG(self)->expires = 0;
	}
	
	/*	RFC 3903 - 4.1.  Identification of Published Event State
		The presence of a body and the SIP-If-Match header field determine
		the specific SSESSION that the request is performing, as described in Table 1.
		+-----------+-------+---------------+---------------+
		| SSESSION | Body? | SIP-If-Match? | Expires Value |
		+-----------+-------+---------------+---------------+
		| Initial   | yes   | no            | > 0           |
		| Refresh   | no    | yes           | > 0           |
		| Modify    | yes   | yes           | > 0           |
		| Remove    | no    | yes           | 0             |
		+-----------+-------+---------------+---------------+
		Table 1: Publication ssessions
	*/	
	if((request = tsip_dialog_request_new(TSIP_DIALOG(self), "PUBLISH"))){
		/*Etag. If initial then etag is null. */
		if(self->etag){
			TSIP_MESSAGE_ADD_HEADER(request, TSIP_HEADER_SIP_IF_MATCH_VA_ARGS(self->etag));
		}
		/*Body*/
		if(action && action->payload && !self->unpublishing){
			const tsk_list_item_t* item;
			tsk_list_foreach(item, action->headers){
				TSIP_MESSAGE_ADD_HEADER(request, TSIP_HEADER_DUMMY_VA_ARGS(TSK_PARAM(item->data)->name, TSK_PARAM(item->data)->value));
			}
			if(action->payload){
				tsip_message_add_content(request, tsk_null, TSK_BUFFER_DATA(action->payload), TSK_BUFFER_SIZE(action->payload));
			}
		}

		ret = tsip_dialog_request_send(TSIP_DIALOG(self), request);
		TSK_OBJECT_SAFE_FREE(request);
	}

	return ret;
}

/**
 * Callback function called by the state machine manager to signal that the final state has been reached.
 *
 * @param [in,out]	self	The state machine owner.
**/
int tsip_dialog_publish_OnTerminated(tsip_dialog_publish_t *self)
{
	TSK_DEBUG_INFO("=== PUBLISH Dialog terminated ===");

	/* Remove from the dialog layer. */
	return tsip_dialog_remove(TSIP_DIALOG(self));
}













//========================================================
//	SIP dialog PUBLISH object definition
//
static tsk_object_t* tsip_dialog_publish_ctor(tsk_object_t * self, va_list * app)
{
	tsip_dialog_publish_t *dialog = self;
	if(dialog){
		tsip_ssession_handle_t *ss = va_arg(*app, tsip_ssession_handle_t *);

		/* init base class */
		tsip_dialog_init(TSIP_DIALOG(self), tsip_dialog_PUBLISH, tsk_null, ss, _fsm_state_Started, _fsm_state_Terminated);

		/* FSM */
		TSIP_DIALOG_GET_FSM(self)->debug = DEBUG_STATE_MACHINE;
		tsk_fsm_set_callback_terminated(TSIP_DIALOG_GET_FSM(self), TSK_FSM_ONTERMINATED_F(tsip_dialog_publish_OnTerminated), (const void*)dialog);

		/* init the class itself */
		tsip_dialog_publish_init(self);
	}
	return self;
}

static tsk_object_t* tsip_dialog_publish_dtor(tsk_object_t * _self)
{ 
	tsip_dialog_publish_t *self = _self;
	if(self){
		TSK_DEBUG_INFO("*** PUBLISH Dialog destroyed ***");

		/* Cancel all timers */
		DIALOG_TIMER_CANCEL(refresh);
		DIALOG_TIMER_CANCEL(shutdown);
		
		/* deinit base class (will cancel all transactions) */
		tsip_dialog_deinit(TSIP_DIALOG(self));

		/* deinit self*/
		TSK_FREE(self->etag);
	}
	return self;
}

static int tsip_dialog_publish_cmp(const tsk_object_t *obj1, const tsk_object_t *obj2)
{
	return tsip_dialog_cmp(obj1, obj2);
}

static const tsk_object_def_t tsip_dialog_publish_def_s = 
{
	sizeof(tsip_dialog_publish_t),
	tsip_dialog_publish_ctor, 
	tsip_dialog_publish_dtor,
	tsip_dialog_publish_cmp, 
};
const tsk_object_def_t *tsip_dialog_publish_def_t = &tsip_dialog_publish_def_s;
