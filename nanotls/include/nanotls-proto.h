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

#ifndef NANOTLS_PROTO_H
#define NANOTLS_PROTO_H

#include <linux/types.h>
#include <asm/string.h>
#include <nanotls-config.h>

#define NANOTLS_MAGIC 0x43b826f3
#define NANOTLS_VERSION 1

#define NANOTLS_HASH_SIZE NANOTLS_SHA256_HASH_SIZE
#define NANOTLS_SECRET_SIZE NANOTLS_HASH_SIZE
#define NANOTLS_KEY_SIZE (128 / 8)
#define NANOTLS_IV_SIZE 12
#define NANOTLS_AUTH_TAG_SIZE 16

#define NANOTLS_ECDSA_PUBLIC_KEY_SIZE 64
#define NANOTLS_ECDSA_PRIVATE_KEY_SIZE 32
#define NANOTLS_ECDH_PUBLIC_KEY_SIZE 64
#define NANOTLS_ECDH_PRIVATE_KEY_SIZE 32

#define NANOTLS_UUID_LEN 16

#define NANOTLS_MAX_DATA_LEN 0xFFFF

#define NANOTLS_LABEL(string) string, strlen(string)
#define NANOTLS_DEVICE_HS_TS_LABEL NANOTLS_LABEL("D HS TS")
#define NANOTLS_DEVICE_AP_TS_LABEL NANOTLS_LABEL("D AP TS")
#define NANOTLS_HOST_HS_TS_LABEL NANOTLS_LABEL("H HS TS")
#define NANOTLS_HOST_AP_TS_LABEL NANOTLS_LABEL("H AP TS")
#define NANOTLS_FINISHED_LABEL NANOTLS_LABEL("finished")
#define NANOTLS_DERIVED_LABEL NANOTLS_LABEL("derived")
#define NANOTLS_KEY_UPDATE_LABEL NANOTLS_LABEL("key update")
#define NANOTLS_KEY_INSTANCE_LABEL NANOTLS_LABEL("key instance")
#define NANOTLS_DEVICE_HS_SIG_LABEL NANOTLS_LABEL("D HS SIG")
#define NANOTLS_HOST_HS_SIG_LABEL NANOTLS_LABEL("H HS SIG")
#define NANOTLS_KEYING_IV_LABEL NANOTLS_LABEL("iv")
#define NANOTLS_KEYING_KEY_LABEL NANOTLS_LABEL("key")

enum nanotls_message_id {
	NANOTLS_HOST_HELLO = 1,
	NANOTLS_DEVICE_HELLO = 2,
	NANOTLS_HOST_FINISHED = 3,
};

enum nanotls_signature_algorithm {
	NANOTLS_ECDSA_SECP256R1_SHA256 = 0x0403,
};

enum nanotls_key_exchange_type {
	NANOTLS_ECDHE_SECP256R1 = 0x0017,
};

enum nanotls_cipher_suite {
	NANOTLS_AES_128_GCM_SHA256 = 0x1301,
};

struct nanotls_key_share {
	uint8_t pubkey[NANOTLS_ECDH_PUBLIC_KEY_SIZE]; /* P-256 ECDH public key,
							 two points */
};

struct nanotls_message_hdr {
	uint32_t magic;
	uint16_t len;
	uint8_t message_id;
	uint8_t protocol_version;
};

struct nanotls_signature {
	uint8_t sig[64]; /* P-256 ECDSA signature, two points */
};

struct nanotls_finished {
	uint8_t verify_data[32];
};

struct auth_tag {
	uint8_t tag[NANOTLS_AUTH_TAG_SIZE];
};

struct nanotls_host_hello {
	struct nanotls_message_hdr hdr;

	uint16_t sig_alg;
	uint16_t key_exchange_type;
	uint16_t cipher_suite;
	uint16_t reserved;

	uint8_t random[32];

	struct nanotls_key_share host_key_share; /* ECDHE */
};

/* DeviceHello's submessage */
struct nanotls_device_info {
	uint16_t chip_id;
	uint16_t device_flags;
	uint8_t reserved[12];
	uint8_t uuid[NANOTLS_UUID_LEN];
};

struct nanotls_device_hello {
	struct nanotls_message_hdr hdr;

	uint32_t reserved;

	uint8_t random[32];

	struct nanotls_key_share device_key_share; /* ECDHE */

	/* Encrypted portion */
	struct {
		struct nanotls_device_info device_info; /* aka Certificate */
		struct nanotls_signature device_handshake_sig; /* aka TLS
								  CertificateVerify
								*/
		struct nanotls_finished device_finished;
	} enc;
	struct auth_tag auth_tag; /* Auth tag for the encrypted portion */
};

#define NANOTLS_DEVICE_HELLO_SIG_CUTOFF_POS                                    \
	offsetof(struct nanotls_device_hello, enc)
#define NANOTLS_DEVICE_HELLO_FINISHED_ENC_CUTOFF_POS                           \
	(offsetof(struct nanotls_device_hello, enc.device_finished) -          \
	 NANOTLS_DEVICE_HELLO_SIG_CUTOFF_POS)

struct nanotls_host_finished {
	struct nanotls_message_hdr hdr;

	uint32_t reserved;

	/* Encrypted portion */
	struct {
		struct nanotls_signature reserved2;
		struct nanotls_finished host_finished;
	} enc;
	struct auth_tag auth_tag; /* Auth tag for the encrypted portion */
};

#define NANOTLS_HOST_HELLO_FINISHED_CUTOFF_POS                                 \
	offsetof(struct nanotls_host_finished, enc.host_finished)

struct nanotls_data_add {
	uint8_t version; /* NANOTLS_VERSION */
	uint8_t reserved[5]; /* zeroes */
	uint16_t len;
};

struct nanotls_traffic_keys {
	/*
	 * Data secrets are saved for future possible key update.
	 * !! Should not be used directly for encryption/decryption.
	 * !! Keys should be derived from it using HDKF-Expand().
	 */
	uint8_t h2d_secret[NANOTLS_SECRET_SIZE];
	uint8_t d2h_secret[NANOTLS_SECRET_SIZE];
	/* These keys below should be used for message encryption/decryption */
	uint8_t h2d_iv[NANOTLS_IV_SIZE];
	uint8_t h2d_key[NANOTLS_KEY_SIZE];
	uint8_t d2h_iv[NANOTLS_IV_SIZE];
	uint8_t d2h_key[NANOTLS_KEY_SIZE];
};

#endif /* NANOTLS_PROTO_H */
