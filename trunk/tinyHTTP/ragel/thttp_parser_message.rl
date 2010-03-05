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

/**@file thttp_parser_message.c
 * @brief HTTP parser.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#include "tinyhttp/parsers/thttp_parser_message.h"
#include "tinyhttp/parsers/thttp_parser_header.h"

#include "tinyhttp/parsers/thttp_parser_url.h"

#include "tsk_debug.h"
#include "tsk_memory.h"

static void thttp_message_parser_execute(tsk_ragel_state_t *state, thttp_message_t *message);
static void thttp_message_parser_init(tsk_ragel_state_t *state);
static void thttp_message_parser_eoh(tsk_ragel_state_t *state, thttp_message_t *message);

/***********************************
*	Ragel state machine.
*/
%%{
	machine thttp_machine_parser_message;

	#/* Tag the buffer (start point). */
	action tag
	{
		state->tag_start = p;
	}

	#/* HTTP method */
	action parse_method
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);

		if(message->type == thttp_unknown)
		{
			message->type = thttp_request;
			if(!message->method)
			{
				message->method = tsk_calloc(1, len+1);
				memcpy(message->method, state->tag_start, len);
			}
		}
		else
		{
			state->cs = thttp_machine_parser_message_error;
		}
	}

	#/* Request URI parsing */
	action parse_requesturl
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);
		
		if(!message->url)
		{
			message->url = thttp_url_parse(state->tag_start, (size_t)len);
		}
	}

	#/* Sip Version */
	action parse_httpversion
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);

		if(!message->http_version)
		{
			message->http_version = tsk_calloc(1, len+1);
			memcpy(message->http_version, state->tag_start, len);
		}
	}

	#/* Status Code */
	action parse_status_code
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);
		
		if(message->type == thttp_unknown)
		{
			message->type = thttp_response;
			message->status_code = atoi(state->tag_start);
		}
		else
		{
			state->cs = thttp_machine_parser_message_error;
		}
	}

	#/* Reason Phrase */
	action parse_reason_phrase
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);

		if(!message->reason_phrase)
		{
			message->reason_phrase = tsk_calloc(1, len+1);
			memcpy(message->reason_phrase, state->tag_start, len);
		}
	}

	#/* Parse http header */
	action parse_header
	{
		int len;
		state->tag_end = p;
		len = (int)(state->tag_end  - state->tag_start);
		
		if(thttp_header_parse(state, message))
		{
			//TSK_DEBUG_INFO("THTTP_MESSAGE_PARSER::PARSE_HEADER len=%d state=%d", len, state->cs);
		}
		else
		{
			TSK_DEBUG_ERROR("Failed to parse header - %s", state->tag_start);
		}
	}

	#/* Parse http content/body. */
	#action parse_body
	#{
	#	int len;
	#	state->tag_end = p;
	#	len = (int)(state->tag_end  - state->tag_start);
	#	TSK_DEBUG_ERROR("==THTTP_MESSAGE_PARSER::PARSE_BODY==");
	#}

	#/* End-Of-Headers */
	action eoh
	{
		state->cs = cs;
		state->p = p;
		state->pe = pe;
		state->eof = eof;

		thttp_message_parser_eoh(state, message);

		cs = state->cs;
		p = state->p;
		pe = state->pe;
		eof = state->eof;
	}

	# Includes
	include thttp_machine_utils "./thttp_machine_utils.rl";
	include thttp_machine_message "./thttp_machine_message.rl";
	
	# Entry point
	main := HTTP_message;
}%%


/* Regel data */
%%write data;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	int thttp_message_parse(tsk_ragel_state_t *state, thttp_message_t *result)
///
/// @brief	Parse a HTTP message. Both requests and reponses messages. 
///
/// @author	Mamadou
/// @date	12/4/2009
///
/// @param [in,out]	state	The ragel state to use. 
/// @param [out]	result	Non-null http message created using @ref THTTP_MESSAGE_CREATE. You must use @ref TSK_OBJECT_SAFE_FREE to
///							free the result.
///
/// @return	@ref zero if succeed and non-zero error code otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
int thttp_message_parse(tsk_ragel_state_t *state, thttp_message_t **result)
{
	if(!state || state->pe <= state->p)
	{
		return -1;
	}

	if(!*result)
	{
		*result = THTTP_MESSAGE_CREATE();
	}

	/* Ragel init */
	thttp_message_parser_init(state);

	/*
	*	State mechine execution.
	*/
	thttp_message_parser_execute(state, *result);

	/* Check result */

	if( state->cs < %%{ write first_final; }%% )
	{
		TSK_OBJECT_SAFE_FREE(*result);
		return -2;
	}
	return 0;
}


static void thttp_message_parser_init(tsk_ragel_state_t *state)
{
	int cs = 0;

	/* Regel machine initialization. */
	%% write init;
	
	state->cs = cs;
}

static void thttp_message_parser_execute(tsk_ragel_state_t *state, thttp_message_t *message)
{
	int cs = state->cs;
	const char *p = state->p;
	const char *pe = state->pe;
	const char *eof = state->eof;

	%% write exec;

	state->cs = cs;
	state->p = p;
	state->pe = pe;
	state->eof = eof;
}

static void thttp_message_parser_eoh(tsk_ragel_state_t *state, thttp_message_t *message)
{
	int cs = state->cs;
	const char *p = state->p;
	const char *pe = state->pe;
	const char *eof = state->eof;

	if(message)
	{
		uint32_t clen = THTTP_MESSAGE_CONTENT_LENGTH(message);
		if((p+clen) <pe && !message->Content)
		{
			message->Content = TSK_BUFFER_CREATE((p+1), clen);
			p = (p+clen);
		}
		else
		{
			p = (pe-1);
		}
	}
	//%%write eof;

	state->cs = cs;
	state->p = p;
	state->pe = pe;
	state->eof = eof;
}