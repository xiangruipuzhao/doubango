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

/**@file tipsec_common.h
 * @brief IPSec common functions.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#ifndef TINYIPSEC_IPSEC_COMMON_H
#define TINYIPSEC_IPSEC_COMMON_H

#include "tinyIPSEC_config.h"

#include "tsk_string.h"

TSK_BEGIN_DECLS

#define TIPSEC_CONTEXT_CREATE(ipproto, use_ipv6, mode, ealg, alg, protocol, ik, ck)\
	tsk_object_new(tipsec_context_def_t, (tipsec_ipproto_t)ipproto, (int)use_ipv6, (tipsec_mode_t)mode, (tipsec_ealgorithm_t)ealg, (tipsec_algorithm_t)alg, (tipsec_protocol_t)protocol, (const tipsec_key_t*)ik, (const tipsec_key_t*)ck)

#define TIPSEC_CONTEXT(ctx) ((tipsec_context_t*)(ctx))

#define TIPSEC_IPPROTO_FROM_STR(ipproto) (tsk_strequals(ipproto, "tcp") ? ipproto_tcp : ipproto_udp)
#define TIPSEC_IPPROTO_TO_STR(ipproto)	(ipproto == ipproto_tcp ? "tcp" : "udp")

#define TIPSEC_MODE_FROM_STR(mode) (tsk_strequals(mode, "trans") ? mode_trans : mode_tun)
#define TIPSEC_MODE_TO_STR(mode)	(mode == mode_trans ? "trans" : "tun")

#define TIPSEC_EALG_FROM_STR(ealg) (tsk_strequals(ealg, "des-ede3-cbc") ? ealg_des_ede3_cbc : (tsk_strequals(ealg, "aes") ? ealg_aes : ealg_null))
#define TIPSEC_EALG_TO_STR(ealg)	(ealg == ealg_des_ede3_cbc ? "des-ede3-cbc" : (ealg == ealg_aes ? "aes" : "null"))

#define TIPSEC_ALG_FROM_STR(alg) (tsk_strequals(alg, "hmac-sha-1-96") ? algo_hmac_sha_1_96 : algo_hmac_md5_96)
#define TIPSEC_ALG_TO_STR(alg)	(alg == algo_hmac_sha_1_96 ? "hmac-sha-1-96" : "hmac-md5-96")

#define TIPSEC_PROTOCOL_FROM_STR(protocol) (tsk_strequals(protocol, "ah") ? proto_ah : ((tsk_strequals(protocol, "esp")) ? proto_esp : proto_both)
#define TIPSEC_PROTOCOL_TO_STR(protocol)	(protocol == proto_esp ? "esp" : (protocol == proto_ah ? "ah" : "ah/esp"))

/**
 * @def	TIPSEC_KEY_SIZE
 *
 * @brief	Size of IK (Integrity Key) and CK (Confidentiality Key).
 *
 * @remarks	Mamadou, 2/21/2010. 
**/
#define TIPSEC_KEY_LEN			16
#define TIPSEC_IK_LEN			20
#define TIPSEC_CK_LEN			24

typedef uint32_t tipsec_lifetime_t;
typedef uint32_t tipsec_spi_t;
typedef uint16_t tipsec_port_t;
//typedef uint8_t tipsec_key_t[TIPSEC_KEY_SIZE];
typedef void tipsec_key_t;

/**
 * @enum	tipsec_mode_e
 *
 * @brief	List of IPSec modes.
**/
typedef enum tipsec_mode_e
{
	//! IPSec transport mode. 
	mode_trans,
	//! IPSec tunneling mode. 
	mode_tun
}
tipsec_mode_t;

/**
 * @enum	tipsec_protocol_e
 *
 * @brief	List of IPSec protocols.
**/
typedef enum tipsec_protocol_e
{
	//! AH protocol. 
	proto_ah,
	//! ESP protocol. 
	proto_esp,
	//! Both AH and ESP protocols. 
	proto_both
}
tipsec_protocol_t;

/**
 * @enum	tipsec_ipproto_e
 *
 * @brief	List of Internet protocols.
**/
typedef enum tipsec_ipproto_e
{
	//! UDP. 
	ipproto_udp,
	//! TCP. 
	ipproto_tcp
}
tipsec_ipproto_t;

/**
 * @enum	tipsec_algorithm_e
 *
 * @brief	List of IPSec algorithms.
**/
typedef enum tipsec_algorithm_e
{
	//! hmac-md5-96 algorithm. 
	algo_hmac_md5_96,
	//! hmac-sha-1-96 algorithm. 
	algo_hmac_sha_1_96
}
tipsec_algorithm_t;

/**
 * @enum	tipsec_ealgorithm_e
 *
 * @brief	List of IPSec encrypt-algorithms.
**/
typedef enum tipsec_ealgorithm_e
{
	//! des-ede3-cbc encrypt-algorithm. 
	ealg_des_ede3_cbc,
	//! aes encrypt-algorithm. 
	ealg_aes,
	//! null encrypt-algorithm. 
	ealg_null
}
tipsec_ealgorithm_t;

/**
 * @enum	tipsec_state_e
 *
 * @brief	List of IPSec states.
**/
typedef enum tipsec_state_e
{
	//! The default state. At this state no SA is created. It's the first and default state.
	state_initial,
	//! Partial state. At this state only inbound SAs (with their SPIs) have been created. 
	state_inbound,
	//! Full state. At this state both inbound and outbound SAs have been create. It's the final state.
	state_full,
	//! All SAs are in active mode. 
	state_active
}
tipsec_state_t;

typedef struct tipsec_context_s
{
	TSK_DECLARE_OBJECT;
	
	//! Indicates whether the context have been initialized or not. 
	unsigned initialized;
	//! Indicates whether the context have been started or not. 
	unsigned started:1;
	
	//! The current state of the IPSec context. 
	tipsec_state_t state;
	
	//! Indicates whether to use IPv6 addresses or not. 
	unsigned use_ipv6:1;
	//! The network protocol. 
	tipsec_ipproto_t ipproto;
	
	//! IPSec mode. 
	tipsec_mode_t mode;
	//! Encrypt algorithm (). 
	tipsec_ealgorithm_t ealg;
	//! Algorithm. 
	tipsec_algorithm_t alg;
	//! IPSec protocol. 
	tipsec_protocol_t protocol;
	
	//! Remote address (Proxy-CSCF). 
	void* addr_remote;
	//! Proxy-CSCF client SPI. 
	tipsec_spi_t spi_pc;
	//! Proxy-CSCF server SPI.
	tipsec_spi_t spi_ps;
	//! Proxy-CSCF client port.
	tipsec_port_t port_pc;
	//! Proxy-CSCF server port.
	tipsec_port_t port_ps;
	
	//! Local address (UE). 
	void* addr_local;
	//! UE client SPI. 
	tipsec_spi_t spi_uc;
	//! UE server SPI.
	tipsec_spi_t spi_us;
	//! UE client port. 
	tipsec_port_t  port_uc;
	//! UE server port. 
	tipsec_port_t port_us;
	
	//! The confidentiality key. 
	tipsec_key_t *ck;
	//! The integrity key. 
	tipsec_key_t *ik;

	//! reg-await-auth timer value. 
	tipsec_lifetime_t lifetime;
}
tipsec_context_t;

#define TINYIPSEC_DECLARE_CONTEXT tipsec_context_t context

TINYIPSEC_GEXTERN const void *tipsec_context_def_t;

TINYIPSEC_API int tipsec_start(tipsec_context_t* ctx);
TINYIPSEC_API int tipsec_set_local(tipsec_context_t* ctx, const char* addr_local, const char* addr_remote, tipsec_port_t port_uc, tipsec_port_t port_us);
TINYIPSEC_API int tipsec_set_remote(tipsec_context_t* ctx, tipsec_spi_t spi_pc, tipsec_spi_t spi_ps, tipsec_port_t port_pc, tipsec_port_t port_ps, tipsec_lifetime_t lifetime);
TINYIPSEC_API int tipsec_stop(tipsec_context_t* ctx);

TSK_END_DECLS

#endif /* TINYIPSEC_IPSEC_COMMON_H */
