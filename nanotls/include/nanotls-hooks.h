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

#ifndef NANOTLS_HOOKS_H
#define NANOTLS_HOOKS_H

#include <linux/types.h>
#include <asm/string.h>
#include <nanotls-types.h>
#include <sha2.h>
#include <hmac.h>
#include <gcm.h>

#define NANOTLS_SHA256_HASH_SIZE 32
#define NANOTLS_SHA256_BLOCK_SIZE 64

typedef cf_sha256_context nanotls_sha_ctx;
typedef cf_hmac_ctx nanotls_hmac_ctx;

// TODO: cleanup
#undef MIN
#undef MAX

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define ROUND_UP(x, align) ((((x) + (align)-1) / (align)) * (align))
#define ROUND_DN(x, align) (((x) / (align)) * (align))
// TODO: cleanup
//#define DIV_ROUND_UP(x, y)      (((x) + (y)-1) / (y))

enum ecode nanotls_secure_random(void *buf, size_t size);

enum ecode nanotls_p256_gen_keypair(uint8_t priv[32], uint8_t pub[64]);
enum ecode nanotls_p256_ecdh_shared_secret(uint8_t secret[32],
					   const uint8_t priv[32],
					   const uint8_t pub[64]);
enum ecode nanotls_p256_ecdsa_sign(uint8_t sig[64], const uint8_t priv[32],
				   const uint8_t *hash, size_t hlen);
enum ecode nanotls_p256_ecdsa_verify(const uint8_t sig[64],
				     const uint8_t pub[64], const uint8_t *hash,
				     size_t hlen);

enum ecode nanotls_sha256_init(nanotls_sha_ctx *ctx);
enum ecode nanotls_sha256_update(nanotls_sha_ctx *ctx, const void *buf,
				 size_t size);
enum ecode nanotls_sha256_digest(const nanotls_sha_ctx *ctx,
				 uint8_t hash[NANOTLS_SHA256_HASH_SIZE]);
enum ecode nanotls_sha256_digest_final(nanotls_sha_ctx *ctx,
				       uint8_t hash[NANOTLS_SHA256_HASH_SIZE]);

enum ecode nanotls_hmac_sha256_init(nanotls_hmac_ctx *ctx, const void *key,
				    size_t key_size);
enum ecode nanotls_hmac_sha256_update(nanotls_hmac_ctx *ctx, const void *data,
				      size_t data_size);
enum ecode nanotls_hmac_sha256_finish(nanotls_hmac_ctx *ctx,
				      uint8_t result[NANOTLS_SHA256_HASH_SIZE]);
enum ecode nanotls_hmac_sha256(const void *key, size_t key_size,
			       const void *data, size_t data_size,
			       uint8_t result[NANOTLS_SHA256_HASH_SIZE]);

/*
 * AES API - needed post-handshake.
 * Only AES-GCM-128 is supported with fixed 12 byte IV size.
 */

#define NANOTLS_AES_KEY_SIZE 16
#define NANOTLS_AES_IV_SIZE 12
#define NANOTLS_AES_GCM_TAG_SIZE 16

typedef gcm_context nanotls_aes_context;

enum ecode nanotls_aes_ctx_init(nanotls_aes_context *ctx,
				const uint8_t key[NANOTLS_AES_KEY_SIZE]);
enum ecode nanotls_aes_start_enc(nanotls_aes_context *ctx,
				 const uint8_t iv[NANOTLS_AES_IV_SIZE],
				 const void *add, size_t add_size);
enum ecode nanotls_aes_start_dec(nanotls_aes_context *ctx,
				 const uint8_t iv[NANOTLS_AES_IV_SIZE],
				 const void *add, size_t add_size);
/*
 * All but the final invocation MUST be called with length mod 16 == 0.
 * Only the final call can have a partial block length of < 128 bits.
 */
enum ecode nanotls_aes_update(nanotls_aes_context *ctx, void *out,
			      const void *in, size_t data_size);
enum ecode nanotls_aes_finish(nanotls_aes_context *ctx,
			      uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);
enum ecode nanotls_aes_ctx_destroy(nanotls_aes_context *ctx);

enum ecode nanotls_aes_encrypt(nanotls_aes_context *ctx, void *out_data,
			       const void *in_data, size_t data_size,
			       const uint8_t iv[NANOTLS_AES_IV_SIZE],
			       const void *add, size_t add_size,
			       uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

enum ecode nanotls_aes_decrypt(nanotls_aes_context *ctx, void *out_data,
			       const void *in_data, size_t data_size,
			       const uint8_t iv[NANOTLS_AES_IV_SIZE],
			       const void *add, size_t add_size,
			       uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

#endif /* NANOTLS_HOOKS_H */
