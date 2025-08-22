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

#include <nanotls-common.h>
#include <asm/string.h>

#include "uassert.h"

enum ecode nanotls_mem_eq(const void *a, const void *b, size_t size)
{
	const volatile uint8_t *va = a;
	const volatile uint8_t *vb = b;
	uint8_t diff = 0;

	while (size--) {
		diff |= *va++ ^ *vb++;
	}

	return (diff == 0) ? E_OK : E_INVALID;
}

void nanotls_mem_erase(void *buf, size_t size)
{
	memset(buf, 0, size);
}

void nanotls_hkdf_sha256_extract(const void *salt, size_t salt_len,
				 const void *ikm, size_t ikm_len,
				 uint8_t prk_result[NANOTLS_SHA256_HASH_SIZE])
{
	if (salt_len > 0) {
		nanotls_hmac_sha256(salt, salt_len, ikm, ikm_len, prk_result);
	} else {
		uint8_t zeroes[NANOTLS_SHA256_HASH_SIZE] = {0};

		nanotls_hmac_sha256(zeroes, NANOTLS_SHA256_HASH_SIZE, ikm,
				    ikm_len, prk_result);
	}
}

void nanotls_hkdf_expand_label(const uint8_t secret[NANOTLS_SHA256_HASH_SIZE],
			       const void *label, size_t label_size,
			       const uint8_t *context, size_t context_size,
			       void *output, size_t output_length)
{
	uint8_t hmac_out[NANOTLS_SHA256_HASH_SIZE];
	nanotls_hmac_ctx hmac_ctx;
	const uint16_t length = output_length;
	const uint8_t one = 0x01;

	uassert(length <= NANOTLS_SHA256_HASH_SIZE);

	nanotls_hmac_sha256_init(&hmac_ctx, secret, NANOTLS_SHA256_HASH_SIZE);

	nanotls_hmac_sha256_update(&hmac_ctx, &length, sizeof(length));
	nanotls_hmac_sha256_update(&hmac_ctx, label, label_size);

	if (context && context_size > 0)
		nanotls_hmac_sha256_update(&hmac_ctx, context, context_size);

	nanotls_hmac_sha256_update(&hmac_ctx, &one, sizeof(one));

	nanotls_hmac_sha256_finish(&hmac_ctx, hmac_out);

	memcpy(output, hmac_out, output_length);

	nanotls_mem_erase(hmac_out, sizeof(hmac_out));
}

void nanotls_write_finished(const nanotls_sha_ctx *handshake_hash,
			    const uint8_t hs_traffic_secret[NANOTLS_SECRET_SIZE],
			    uint8_t verify_data[NANOTLS_SHA256_HASH_SIZE])
{
	uint8_t transcript_hash[NANOTLS_SHA256_HASH_SIZE];
	uint8_t finished_key[NANOTLS_SHA256_HASH_SIZE]; /* HMAC key (not AES),
							   hence it's bigger */

	/* Calculate transcript hash (transcript_hash) */
	nanotls_sha256_digest(handshake_hash, transcript_hash);

	/*
	 * Derive "Finished" key
	 * finished_key = HKDF-Expand-Label(hs_traffic_secret, "finished", "",
	 * Hash.length)
	 */
	nanotls_hkdf_expand_label_noctx(hs_traffic_secret,
					NANOTLS_FINISHED_LABEL, finished_key,
					sizeof(finished_key));

	/*
	 * Create Finished.verify_data
	 * verify_data = HMAC(finished_key, transcript_hash)
	 */
	nanotls_hmac_sha256(finished_key, sizeof(finished_key), transcript_hash,
			    sizeof(transcript_hash), verify_data);

	nanotls_mem_erase(finished_key, sizeof(finished_key));
}

void nanotls_derive_key_iv(uint8_t key[NANOTLS_KEY_SIZE],
			   uint8_t iv[NANOTLS_IV_SIZE],
			   const uint8_t secret[NANOTLS_SECRET_SIZE])
{
	/* key = HKDF-Expand-Label(secret, "key", "") */
	nanotls_hkdf_expand_label_noctx(secret, NANOTLS_KEYING_KEY_LABEL, key,
					NANOTLS_KEY_SIZE);

	/* iv = HKDF-Expand-Label(secret, "iv", "") */
	nanotls_hkdf_expand_label_noctx(secret, NANOTLS_KEYING_IV_LABEL, iv,
					NANOTLS_IV_SIZE);
}
