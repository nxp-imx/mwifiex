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

#ifndef NANOTLS_COMMON_H
#define NANOTLS_COMMON_H

#include <nanotls-config.h>
#include <nanotls-hooks.h>
#include <nanotls-proto.h>
#include <nanotls-types.h>
#include <uassert.h>

// TODO: cleanup
#undef memset

#define ABS(x) (((x) < 0) ? -(x) : (x))
#define offsetdiff(s, field_a, field_b)                                        \
	ABS(offsetof(s, field_b) - offsetof(s, field_a))

enum ecode nanotls_mem_eq(const void *a, const void *b, size_t size);
void nanotls_mem_erase(void *buf, size_t size);

static inline void nanotls_hdr_init(struct nanotls_message_hdr *hdr, size_t len,
				    enum nanotls_message_id id)
{
	hdr->magic = NANOTLS_MAGIC;
	hdr->len = (uint16_t)len;
	hdr->message_id = (uint8_t)id;
	hdr->protocol_version = NANOTLS_VERSION;
}

static inline enum ecode
nanotls_hdr_check(const struct nanotls_message_hdr *hdr, size_t len,
		  enum nanotls_message_id id)
{
	if (hdr->magic != NANOTLS_MAGIC)
		return E_INVALID;

	if (hdr->len != (uint16_t)len)
		return E_INVALID;

	if (hdr->message_id != (uint8_t)id)
		return E_INVALID;

	if (hdr->protocol_version != NANOTLS_VERSION)
		return E_INVALID;

	return E_OK;
}

static inline enum ecode nanotls_data_add_init(struct nanotls_data_add *add,
					       size_t data_size)
{
	if (data_size > NANOTLS_MAX_DATA_LEN)
		return E_INVALID;

	memset(add, 0, sizeof(*add));

	add->version = NANOTLS_VERSION;
	add->len = data_size;

	return E_OK;
}

void nanotls_hkdf_sha256_extract(const void *salt, size_t salt_len,
				 const void *ikm, size_t ikm_len,
				 uint8_t prk_result[NANOTLS_SHA256_HASH_SIZE]);

/*
 * TLS HKDF-Expand-Label(Secret, Label, Context, Hash.length) =
 *     HKDF-Expand(Secret, Length || Label || Context, Length) =
 *     HMAC-Hash(Secret, Length || Label || Context || 0x01, Length)
 *
 * Here Length is fixed to 32.
 */
void nanotls_hkdf_expand_label(const uint8_t secret[NANOTLS_SHA256_HASH_SIZE],
			       const void *label, size_t label_size,
			       const uint8_t *context, size_t context_size,
			       void *output, size_t output_length);

/* TLS Derive-Secret with fixed context size = hash size */
static inline void
nanotls_hkdf_derive_secret(const uint8_t secret[NANOTLS_SHA256_HASH_SIZE],
			   const void *label, size_t label_size,
			   const uint8_t context[NANOTLS_SHA256_HASH_SIZE],
			   uint8_t output[NANOTLS_SECRET_SIZE])
{
	nanotls_hkdf_expand_label(secret, label, label_size, context,
				  NANOTLS_SHA256_HASH_SIZE, output,
				  NANOTLS_SECRET_SIZE);
}

/*
 * HKDF-Expand-Label(Secret, Label, Context = "", Length) =
 *     HKDF-Expand(Secret, Length || Label || Context, Length) =
 *     HKDF-Expand(Secret, Length || Label, Length)
 */
static inline void
nanotls_hkdf_expand_label_noctx(const uint8_t secret[NANOTLS_SHA256_HASH_SIZE],
				const void *label, size_t label_size,
				uint8_t *output, size_t output_length)
{
	nanotls_hkdf_expand_label(secret, label, label_size, NULL, 0, output,
				  output_length);
}

static inline void nanotls_derive_master_secret(
	uint8_t master_secret[NANOTLS_SECRET_SIZE],
	const uint8_t handshake_secret[NANOTLS_SECRET_SIZE])
{
	uint8_t zeroes[NANOTLS_HASH_SIZE] = {0};
	uint8_t dhs[NANOTLS_SECRET_SIZE];

	nanotls_hkdf_expand_label_noctx(handshake_secret, NANOTLS_DERIVED_LABEL,
					dhs, sizeof(dhs));
	nanotls_hkdf_sha256_extract(dhs, sizeof(dhs), zeroes, sizeof(zeroes),
				    master_secret);

	nanotls_mem_erase(dhs, sizeof(dhs));
}

void nanotls_write_finished(const nanotls_sha_ctx *handshake_hash,
			    const uint8_t hs_traffic_secret[NANOTLS_SECRET_SIZE],
			    uint8_t verify_data[NANOTLS_SHA256_HASH_SIZE]);

void nanotls_derive_key_iv(uint8_t key[NANOTLS_KEY_SIZE],
			   uint8_t iv[NANOTLS_IV_SIZE],
			   const uint8_t secret[NANOTLS_SECRET_SIZE]);

static inline void
nanotls_handshake_encrypt(void *buf, size_t size, struct auth_tag *auth_tag,
			  const uint8_t traffic_secret[NANOTLS_SECRET_SIZE])
{
#ifdef NANOTLS_CONFIG_ENCRYPTED_HANDSHAKE
	nanotls_aes_context aes_ctx;
	const struct nanotls_data_add add = {.version = NANOTLS_VERSION,
					     .len = (uint16_t)size};
	uint8_t key[NANOTLS_KEY_SIZE];
	uint8_t iv[NANOTLS_KEY_SIZE];

	uassert(size <= NANOTLS_MAX_DATA_LEN);

	nanotls_derive_key_iv(key, iv, traffic_secret);

	nanotls_aes_ctx_init(&aes_ctx, key);
	nanotls_mem_erase(key, sizeof(key));
	nanotls_aes_encrypt(&aes_ctx, buf, buf, size, iv, &add, sizeof(add),
			    auth_tag->tag);
	nanotls_mem_erase(iv, sizeof(iv));
	nanotls_aes_ctx_destroy(&aes_ctx);
#else
	(void)buf;
	(void)size;
	(void)auth_tag;
	(void)traffic_secret;
#endif
}

static inline enum ecode nanotls_handshake_decrypt_verify(
	void *buf, size_t size, const struct auth_tag *auth_tag,
	const uint8_t traffic_secret[NANOTLS_SECRET_SIZE])
{
#ifdef NANOTLS_CONFIG_ENCRYPTED_HANDSHAKE
	nanotls_aes_context aes_ctx;
	const struct nanotls_data_add add = {.version = NANOTLS_VERSION,
					     .len = (uint16_t)size};
	uint8_t key[NANOTLS_KEY_SIZE];
	uint8_t iv[NANOTLS_KEY_SIZE];
	uint8_t tag_calc[NANOTLS_AUTH_TAG_SIZE];

	uassert(size <= NANOTLS_MAX_DATA_LEN);

	nanotls_derive_key_iv(key, iv, traffic_secret);

	nanotls_aes_ctx_init(&aes_ctx, key);
	nanotls_mem_erase(key, sizeof(key));
	nanotls_aes_decrypt(&aes_ctx, buf, buf, size, iv, &add, sizeof(add),
			    tag_calc);
	nanotls_mem_erase(iv, sizeof(iv));
	nanotls_aes_ctx_destroy(&aes_ctx);

	return nanotls_mem_eq(tag_calc, auth_tag->tag, sizeof(tag_calc));
#else
	(void)buf;
	(void)size;
	(void)auth_tag;
	(void)traffic_secret;

	return E_OK;
#endif
}

static inline void
nanotls_handshake_sig_hash(const uint8_t transcript_hash[NANOTLS_HASH_SIZE],
			   const char *context, size_t context_len,
			   uint8_t output_hash[NANOTLS_SHA256_HASH_SIZE])
{
	nanotls_sha_ctx sha_ctx;
	const uint8_t zero = 0;

	/*
	 * The digital signature is then computed over the concatenation of:
	 *  -  A string that consists of octet 32 (0x20) repeated 64 times
	 *  -  The context string
	 *  -  A single 0 byte which serves as the separator
	 *  -  The content to be signed
	 */

	memset(output_hash, 0x20, NANOTLS_SHA256_HASH_SIZE);

	nanotls_sha256_init(&sha_ctx);
	/* 2x hash size = block size of 0x20 */
	nanotls_sha256_update(&sha_ctx, output_hash, NANOTLS_SHA256_HASH_SIZE);
	nanotls_sha256_update(&sha_ctx, output_hash, NANOTLS_SHA256_HASH_SIZE);

	nanotls_sha256_update(&sha_ctx, context, context_len);
	nanotls_sha256_update(&sha_ctx, &zero, sizeof(zero));

	nanotls_sha256_update(&sha_ctx, transcript_hash,
			      NANOTLS_SHA256_HASH_SIZE);
	nanotls_sha256_digest_final(&sha_ctx, output_hash);
}

static inline void nanotls_derive_traffic_keys(
	const uint8_t master_secret[NANOTLS_SECRET_SIZE],
	const uint8_t transcript_hash[NANOTLS_SHA256_HASH_SIZE],
	const uint8_t *instance_label, size_t instance_label_len,
	struct nanotls_traffic_keys *keys)
{
	/*
	 * Device application traffic secret:
	 * device_traffic_secret = Derive-Secret(master_secret, label,
	 *     transcript_hash(HostHello || DeviceHello || HostFinished)) =
	 *     HKDF-Expand(master_secret, 32 || label || transcript_hash, 32)
	 */
	nanotls_hkdf_derive_secret(master_secret, NANOTLS_DEVICE_AP_TS_LABEL,
				   transcript_hash, keys->d2h_secret);

	if (instance_label) {
		/*
		 * Device application traffic (instance) secret:
		 * device_traffic_secret_n =
		 * HKDF-Expand-Label(device_traffic_secret, instance_label, "")
		 */
		nanotls_hkdf_expand_label_noctx(
			keys->d2h_secret, instance_label, instance_label_len,
			keys->d2h_secret, NANOTLS_SECRET_SIZE);
	}

	nanotls_derive_key_iv(keys->d2h_key, keys->d2h_iv, keys->d2h_secret);

	/*
	 * Host application traffic secret:
	 * host_traffic_secret = Derive-Secret(master_secret, label,
	 *     transcript_hash(HostHello || DeviceHello || HostFinished)) =
	 *     HKDF-Expand(master_secret, 32 || label || transcript_hash, 32)
	 */
	nanotls_hkdf_derive_secret(master_secret, NANOTLS_HOST_AP_TS_LABEL,
				   transcript_hash, keys->h2d_secret);

	if (instance_label) {
		/*
		 * Host application traffic (instance) secret:
		 * host_traffic_secret_n =
		 * HKDF-Expand-Label(host_traffic_secret, instance_label, "")
		 */
		nanotls_hkdf_expand_label_noctx(
			keys->h2d_secret, instance_label, instance_label_len,
			keys->h2d_secret, NANOTLS_SECRET_SIZE);
	}

	nanotls_derive_key_iv(keys->h2d_key, keys->h2d_iv, keys->h2d_secret);
}

#ifdef NANOTLS_CONFIG_KEY_UPDATE

static inline enum ecode
nanotls_update_traffic_secret(uint8_t secret[NANOTLS_SECRET_SIZE],
			      uint8_t key[NANOTLS_KEY_SIZE],
			      uint8_t iv[NANOTLS_IV_SIZE])
{
	/*
	 * Update the D2H traffic secret:
	 * xxx_secret(N+1) = HKDF-Expand-Label(xxx_secret(N), label, "")
	 */
	nanotls_hkdf_expand_label_noctx(secret, NANOTLS_KEY_UPDATE_LABEL,
					secret, NANOTLS_SECRET_SIZE);

	nanotls_derive_key_iv(key, iv, secret);

	return E_OK;
}

#endif

#endif /* NANOTLS_COMMON_H */
