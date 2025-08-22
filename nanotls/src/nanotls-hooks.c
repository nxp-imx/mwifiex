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

#include <nanotls-hooks.h>
#include <asm/string.h>
#include <linux/random.h>
#include <p256-m.h>

#include "nanotls-common.h"

enum ecode nanotls_secure_random(void *buf, size_t size)
{
	get_random_bytes(buf, size);
	return E_OK;
}

enum ecode nanotls_p256_gen_keypair(uint8_t priv[32], uint8_t pub[64])
{
	return (p256_gen_keypair(priv, pub) == P256_SUCCESS) ? E_OK : E_INVALID;
}

enum ecode nanotls_p256_ecdh_shared_secret(uint8_t secret[32],
					   const uint8_t priv[32],
					   const uint8_t pub[64])
{
	return (p256_ecdh_shared_secret(secret, priv, pub) == P256_SUCCESS) ?
		       E_OK :
		       E_INVALID;
}

enum ecode nanotls_p256_ecdsa_sign(uint8_t sig[64], const uint8_t priv[32],
				   const uint8_t *hash, size_t hlen)
{
	return (p256_ecdsa_sign(sig, priv, hash, hlen) == P256_SUCCESS) ?
		       E_OK :
		       E_INVALID;
}

enum ecode nanotls_p256_ecdsa_verify(const uint8_t sig[64],
				     const uint8_t pub[64], const uint8_t *hash,
				     size_t hlen)
{
	return (p256_ecdsa_verify(sig, pub, hash, hlen) == P256_SUCCESS) ?
		       E_OK :
		       E_INVALID;
}

enum ecode nanotls_sha256_init(nanotls_sha_ctx *ctx)
{
	cf_sha256_init(ctx);

	return E_OK;
}

enum ecode nanotls_sha256_update(nanotls_sha_ctx *ctx, const void *buf,
				 size_t size)
{
	cf_sha256_update(ctx, buf, size);

	return E_OK;
}

enum ecode nanotls_sha256_digest(const nanotls_sha_ctx *ctx,
				 uint8_t hash[NANOTLS_SHA256_HASH_SIZE])
{
	cf_sha256_digest(ctx, hash);

	return E_OK;
}

enum ecode nanotls_sha256_digest_final(nanotls_sha_ctx *ctx,
				       uint8_t hash[NANOTLS_SHA256_HASH_SIZE])
{
	cf_sha256_digest_final(ctx, hash);

	return E_OK;
}

enum ecode nanotls_hmac_sha256_init(nanotls_hmac_ctx *ctx, const void *key,
				    size_t key_size)
{
	cf_hmac_init(ctx, &cf_sha256, key, key_size);

	return E_OK;
}

enum ecode nanotls_hmac_sha256_update(nanotls_hmac_ctx *ctx, const void *data,
				      size_t data_size)
{
	cf_hmac_update(ctx, data, data_size);

	return E_OK;
}

enum ecode nanotls_hmac_sha256_finish(nanotls_hmac_ctx *ctx,
				      uint8_t result[NANOTLS_SHA256_HASH_SIZE])
{
	cf_hmac_finish(ctx, result);

	return E_OK;
}

enum ecode nanotls_hmac_sha256(const void *key, size_t key_size,
			       const void *data, size_t data_size,
			       uint8_t result[NANOTLS_SHA256_HASH_SIZE])
{
	nanotls_hmac_ctx ctx;
	enum ecode ret = E_INVALID;

	ret = nanotls_hmac_sha256_init(&ctx, key, key_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_hmac_sha256_update(&ctx, data, data_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_hmac_sha256_finish(&ctx, result);
	if (ret != E_OK)
		return ret;

	return E_OK;
}

/*
 * AES API - needed post-handshake.
 * Only AES-GCM-128 is supported with fixed 12 byte IV size.
 */

enum ecode nanotls_aes_ctx_init(nanotls_aes_context *ctx,
				const uint8_t key[NANOTLS_AES_KEY_SIZE])
{
	int ret;

	ret = gcm_setkey(ctx, key, NANOTLS_AES_KEY_SIZE);

	return (ret == 0) ? E_OK : E_INVALID;
}

enum ecode nanotls_aes_start_enc(nanotls_aes_context *ctx,
				 const uint8_t iv[NANOTLS_AES_IV_SIZE],
				 const void *add, size_t add_size)
{
	int ret;

	ret = gcm_start(ctx, AES_MODE_ENCRYPT, iv, NANOTLS_AES_IV_SIZE, add,
			add_size);

	return (ret == 0) ? E_OK : E_INVALID;
}

enum ecode nanotls_aes_start_dec(nanotls_aes_context *ctx,
				 const uint8_t iv[NANOTLS_AES_IV_SIZE],
				 const void *add, size_t add_size)
{
	int ret;

	ret = gcm_start(ctx, AES_MODE_DECRYPT, iv, NANOTLS_AES_IV_SIZE, add,
			add_size);

	return (ret == 0) ? E_OK : E_INVALID;
}

enum ecode nanotls_aes_update(nanotls_aes_context *ctx, void *out,
			      const void *in, size_t data_size)
{
	int ret;

	ret = gcm_update(ctx, data_size, in, out);

	return (ret == 0) ? E_OK : E_INVALID;
}

enum ecode nanotls_aes_finish(nanotls_aes_context *ctx,
			      uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	int ret;

	ret = gcm_finish(ctx, tag, NANOTLS_AES_GCM_TAG_SIZE);

	return (ret == 0) ? E_OK : E_INVALID;
}

enum ecode nanotls_aes_ctx_destroy(nanotls_aes_context *ctx)
{
	nanotls_mem_erase(ctx, sizeof(*ctx));
	ctx->mode = AES_MODE_NONE;

	/* Not needed as we're erasing it already */
	/* gcm_zero_ctx(ctx); */

	return E_OK;
}

enum ecode nanotls_aes_encrypt(nanotls_aes_context *ctx, void *out_data,
			       const void *in_data, size_t data_size,
			       const uint8_t iv[NANOTLS_AES_IV_SIZE],
			       const void *add, size_t add_size,
			       uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	enum ecode ret;

	ret = nanotls_aes_start_enc(ctx, iv, add, add_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_update(ctx, out_data, in_data, data_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_finish(ctx, tag);
	if (ret != E_OK)
		return ret;

	return E_OK;
}

enum ecode nanotls_aes_decrypt(nanotls_aes_context *ctx, void *out_data,
			       const void *in_data, size_t data_size,
			       const uint8_t iv[NANOTLS_AES_IV_SIZE],
			       const void *add, size_t add_size,
			       uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	enum ecode ret;

	ret = nanotls_aes_start_dec(ctx, iv, add, add_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_update(ctx, out_data, in_data, data_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_finish(ctx, tag);
	if (ret != E_OK)
		return ret;

	return E_OK;
}

/* Provide random API to P256 library (for ECDSA & ECDH) */
int p256_generate_random(uint8_t *output, unsigned output_size)
{
	return (nanotls_secure_random(output, output_size) == E_OK) ? 0 : -1;
}

void p256_mem_zeroize(void *buf, size_t bytes)
{
	nanotls_mem_erase(buf, bytes);
}
