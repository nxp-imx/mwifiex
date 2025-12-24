/** @file moal_shc.c
 *
 * @brief This file contains callback functions for secure host interface.
 *
 *
 * Copyright 2025 NXP
 *
 * This software file (the File) is distributed by NXP
 * under the terms of the GNU General Public License Version 2, June 1991
 * (the License).  You may use, redistribute and/or modify the File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
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
