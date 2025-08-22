/*
 * Copyright 2025 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Materials") are owned by NXP, its
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
 */

#ifndef NANOTLS_DEVICE_H
#define NANOTLS_DEVICE_H

#include <nanotls-config.h>
#include <nanotls-hooks.h>
#include <nanotls-proto.h>

enum nanotls_device_state {
	NANOTLS_DEV_UNINIT = 0,
#ifdef NANOTLS_CONFIG_HARDENING
	NANOTLS_DEV_INIT = 0x32eaf5,
	NANOTLS_DEV_HELLO_RCVD = 0x7a52ae,
	NANOTLS_DEV_HELLO_SENT = 0x5e3a0a,
	NANOTLS_DEV_DONE = 0xb09835,
	NANOTLS_DEV_ERR = 0xed2312,
#else
	NANOTLS_DEV_INIT,
	NANOTLS_DEV_HELLO_RCVD,
	NANOTLS_DEV_HELLO_SENT,
	NANOTLS_DEV_DONE,
	NANOTLS_DEV_ERR = -1,
#endif
};

struct nanotls_device_ctx {
	enum nanotls_device_state state;
	size_t error_count;

	struct nanotls_device_info device_info;

	/* ECDSA - Authentication */
	uint8_t ecdsa_priv[NANOTLS_ECDSA_PRIVATE_KEY_SIZE];

	/* ECDH - Key negotiation */
	uint8_t ecdh_priv[NANOTLS_ECDH_PRIVATE_KEY_SIZE];
	struct nanotls_key_share host_key_share;

	/* Running transcript hash context */
	nanotls_sha_ctx handshake_hash;
	uint8_t handshake_h2_hash[NANOTLS_HASH_SIZE];

	uint8_t handshake_secret[NANOTLS_SECRET_SIZE];
	uint8_t master_secret[NANOTLS_SECRET_SIZE];
};

/**
 * Initialize the device handshake context.
 *
 * @param ctx device handshake context
 * @param ecdsa_priv device ECDSA private key (matching the host public key)
 * @param device_info device information to be forwarded to the host as part of
 * DeviceHello. Could optionally be NULL, then device_info will be filled with
 * zeroes.
 * @return E_OK in case of success, error code otherwise
 */
enum ecode
nanotls_device_init(struct nanotls_device_ctx *ctx,
		    const uint8_t ecdsa_priv[NANOTLS_ECDSA_PRIVATE_KEY_SIZE],
		    const struct nanotls_device_info *device_info);

enum ecode nanotls_device_host_hello_rcvd(struct nanotls_device_ctx *ctx,
					  const struct nanotls_host_hello *msg);
enum ecode nanotls_device_do_hello(struct nanotls_device_ctx *ctx,
				   struct nanotls_device_hello *msg);

enum ecode nanotls_device_host_finished_rcvd(struct nanotls_device_ctx *ctx,
					     struct nanotls_host_finished *msg);

enum ecode nanotls_device_derive_traffic_keys_instance(
	const struct nanotls_device_ctx *ctx, struct nanotls_traffic_keys *keys,
	size_t instance_idx);

enum ecode
nanotls_device_derive_traffic_keys(const struct nanotls_device_ctx *ctx,
				   struct nanotls_traffic_keys *keys);

enum ecode nanotls_device_cleanup(struct nanotls_device_ctx *ctx);

#endif /* NANOTLS_DEVICE_H */
