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
* GNU Lesser General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tsk_string.h
 * @brief Useful string functions to manipulate strings.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#ifndef _TINYSAK_STRING_H_
#define _TINYSAK_STRING_H_

#include "tinySAK_config.h"
#include "tsk_heap.h"

TINYSAK_API void tsk_free(tsk_heap_t *heap, void** ptr);
TINYSAK_API char* tsk_strdup(tsk_heap_t *heap, const char *s1);

#endif /* _TINYSAK_STRING_H_ */