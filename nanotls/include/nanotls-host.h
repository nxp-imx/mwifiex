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

#ifndef NANOTLS_HOST_H
#define NANOTLS_HOST_H

#include <nanotls-config.h>
#include <nanotls-hooks.h>
#include <nanotls-proto.h>

enum nanotls_host_state {
	NANOTLS_HOST_UNINIT = 0,
#ifdef NANOTLS_CONFIG_HARDENING
	NANOTLS_HOST_INIT = 0x32eaf5,
	NANOTLS_HOST_HELLO_SENT = 0x7a52ae,
	NANOTLS_HOST_HELLO_RCVD = 0x5e3a0a,
	NANOTLS_HOST_DONE = 0xb09835,
	NANOTLS_HOST_ERR = 0xed2312,
#else
	NANOTLS_HOST_INIT,
	NANOTLS_HOST_HELLO_SENT,
	NANOTLS_HOST_HELLO_RCVD,
	NANOTLS_HOST_DONE,
	NANOTLS_HOST_ERR = -1,
#endif
};

struct nanotls_host_ctx;

/**
 * Public key request callback.
 * A callback called from nanotls_host_device_hello_rcvd() called to
 * request a device public key if it has not been provided during context init
 * time.
 * @param ctx [in] host handshake context (for reference)
 * @param device_info [in] device information data provided by the device in
 * DeviceHello. Could be used by the driver to narrow down the FW image search.
 * @param ecdsa_pub [out] destination buffer callback function should write
 *        resulting ecdsa public key to
 * @return E_OK in case of public key found and successfully written to
 * ecdsa_pub buffer, error code otherwise
 */
typedef enum ecode (*nanotls_host_pub_key_req_cb)(
	const struct nanotls_host_ctx *ctx,
	const struct nanotls_device_info *device_info,
	uint8_t ecdsa_pub[NANOTLS_ECDSA_PUBLIC_KEY_SIZE]);

struct nanotls_host_ctx {
	enum nanotls_host_state state;
	size_t error_count;

	struct nanotls_device_info device_info;

	/* ECDSA - Authentication */
	uint8_t ecdsa_pub[NANOTLS_ECDSA_PUBLIC_KEY_SIZE];
	nanotls_host_pub_key_req_cb pub_key_req_cb; /* callback to request the
						       public key */

	/* ECDH - Key negotiation */
	uint8_t ecdh_priv[NANOTLS_ECDH_PRIVATE_KEY_SIZE];
	struct nanotls_key_share device_key_share;

	/* Running transcript hash context */
	nanotls_sha_ctx handshake_hash;
	uint8_t handshake_h2_hash[NANOTLS_HASH_SIZE];

	uint8_t handshake_secret[NANOTLS_SECRET_SIZE];
	uint8_t master_secret[NANOTLS_SECRET_SIZE];
};

/**
 * Initialize the host handshake context with the known public key.
 *
 * This init function flavor assumes the public key is known at the init time.
 *
 * @param ctx host handshake context
 * @param ecdsa_pub host ECDSA public key (matching the device private key)
 * @return E_OK in case of success, error code otherwise
 */
enum ecode
nanotls_host_init(struct nanotls_host_ctx *ctx,
		  const uint8_t ecdsa_pub[NANOTLS_ECDSA_PUBLIC_KEY_SIZE]);

/**
 * Initialize the host handshake context with deferred public key lookup.
 *
 * This init function flavor assumes the public key is not known at the init
 * time, so the pub_key_req_cb callback is called from
 * nanotls_host_device_hello_rcvd() to look up and provide the public key using
 * device_info provided in DeviceHello.
 *
 * @param ctx host handshake context
 * @param pub_key_req_cb a callback to be called from
 * nanotls_host_device_hello_rcvd() to provide lookup for the public key using
 * device_info provided in DeviceHello.
 * @return E_OK in case of success, error code otherwise
 */
enum ecode
nanotls_host_init_pubkey_cb(struct nanotls_host_ctx *ctx,
			    nanotls_host_pub_key_req_cb pub_key_req_cb);

enum ecode nanotls_host_do_hello(struct nanotls_host_ctx *ctx,
				 struct nanotls_host_hello *msg);

enum ecode nanotls_host_device_hello_rcvd(struct nanotls_host_ctx *ctx,
					  struct nanotls_device_hello *msg);

/* Get device info after DeviceHello has been processed */
enum ecode
nanotls_host_get_device_info(const struct nanotls_host_ctx *ctx,
			     struct nanotls_device_info *device_info);

enum ecode nanotls_host_do_finished(struct nanotls_host_ctx *ctx,
				    struct nanotls_host_finished *msg);

enum ecode
nanotls_host_derive_traffic_keys_instance(const struct nanotls_host_ctx *ctx,
					  struct nanotls_traffic_keys *keys,
					  size_t instance_idx);

enum ecode nanotls_host_derive_traffic_keys(const struct nanotls_host_ctx *ctx,
					    struct nanotls_traffic_keys *keys);

enum ecode nanotls_host_cleanup(struct nanotls_host_ctx *ctx);

#endif /* NANOTLS_HOST_H */
