/** @file moal_shc.c
 *
 * @brief This file contains callback functions for secure host interface.
 *
 *
 * Copyright 2025 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code (Materials) are owned by NXP, its
 * suppliers and/or its licensors. Title to the Materials remains with NXP,
 * its suppliers and/or its licensors. The Materials contain
 * trade secrets and proprietary and confidential information of NXP, its
 * suppliers and/or its licensors. The Materials are protected by worldwide
 * copyright and trade secret laws and treaty provisions. No part of the
 * Materials may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without NXP's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by NXP in writing.
 *
 *  Alternatively, this software may be distributed under the terms of GPL v2.
 *  SPDX-License-Identifier:    GPL-2.0
 *
 */

#ifndef _MOAL_SHC_H_
#define _MOAL_SHC_H_

#include "moal_main.h"
#include "nanotls-device.h"
#include "nanotls-host.h"
#include "nanotls-common.h"
#include "nanotls-data.h"

/* Unique data context and traffic key per MAC */
#define MAX_CTX_AND_KEY 2

typedef struct _secure_host_t {
	struct nanotls_host_ctx *host_ctx;
	struct nanotls_host_hello *host_hello;
	struct nanotls_device_hello *device_hello;
	struct nanotls_host_finished *host_finished;
	struct nanotls_traffic_keys *host_keys[MAX_CTX_AND_KEY];
	struct nanotls_ctx_data *data_ctx[MAX_CTX_AND_KEY];
} secure_host_t;

t_u8 moal_secure_host_get_msg_id(t_void *msg);
mlan_status moal_secure_host_init(t_void *pmoal,
				  const t_u8 key[NANOTLS_ECDSA_PUBLIC_KEY_SIZE],
				  const t_u8 uuid[NANOTLS_UUID_LEN]);
void moal_secure_host_cleanup(t_void *pmoal);
mlan_status moal_secure_host_do_hello(t_void *pmoal, t_void **msg);
mlan_status moal_secure_host_device_hello_rcvd(t_void *pmoal, t_void *msg);
mlan_status moal_secure_host_do_finished(t_void *pmoal, t_void **msg);
mlan_status moal_secure_host_derive_traffic_keys(t_void *pmoal);
mlan_status moal_secure_host_data_ctx_init(t_void *pmoal);
mlan_status moal_secure_host_data_encrypt(t_void *pmoal, t_void **enc_data,
					  t_void **payload, t_u32 len);
mlan_status moal_secure_host_data_decrypt(t_void *pmoal, t_void **dec_data,
					  t_void **payload, t_u32 len);

#endif /* _MOAL_SHC_H_ */
