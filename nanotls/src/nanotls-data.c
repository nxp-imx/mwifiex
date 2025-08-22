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

#include <nanotls-data.h>
#include <nanotls-hooks.h>
#include <asm/string.h>
#include <uassert.h>

static enum ecode nanotls_data_check_ctx(const struct nanotls_ctx_data *ctx)
{
	if (ctx->role != NANOTLS_ROLE_HOST && ctx->role != NANOTLS_ROLE_DEVICE)
		return E_INVALID;

	return E_OK;
}

/* NONCE = IV ^ SEQ_NO */
static inline void nanotls_data_get_nonce(uint8_t nonce[NANOTLS_AES_IV_SIZE],
					  const uint8_t iv[NANOTLS_AES_IV_SIZE],
					  uint64_t seq_no)
{
	size_t offset = sizeof(seq_no);
	uint64_t tmp;

	memcpy(&tmp, iv, sizeof(tmp));

	tmp ^= seq_no;

	memcpy(nonce, &tmp, sizeof(tmp));
	memcpy(nonce + offset, iv + offset, NANOTLS_AES_IV_SIZE - offset);
}

enum ecode nanotls_data_ctx_init(struct nanotls_ctx_data *ctx,
				 enum nanotls_role role,
				 const struct nanotls_traffic_keys *keys)
{
	const uint8_t *enc_key = NULL;
	const uint8_t *dec_key = NULL;
	enum ecode ret;

	memset(ctx, 0, sizeof(*ctx));

	ctx->role = role;

	switch (role) {
	case NANOTLS_ROLE_HOST:
		memcpy(ctx->enc_iv, keys->h2d_iv, sizeof(ctx->enc_iv));
		memcpy(ctx->dec_iv, keys->d2h_iv, sizeof(ctx->dec_iv));
#ifdef NANOTLS_CONFIG_KEY_UPDATE
		memcpy(ctx->enc_secret, keys->h2d_secret,
		       sizeof(ctx->enc_secret));
		memcpy(ctx->dec_secret, keys->d2h_secret,
		       sizeof(ctx->dec_secret));
#endif
		enc_key = keys->h2d_key;
		dec_key = keys->d2h_key;
		break;

	case NANOTLS_ROLE_DEVICE:
		memcpy(ctx->enc_iv, keys->d2h_iv, sizeof(ctx->enc_iv));
		memcpy(ctx->dec_iv, keys->h2d_iv, sizeof(ctx->dec_iv));
#ifdef NANOTLS_CONFIG_KEY_UPDATE
		memcpy(ctx->enc_secret, keys->d2h_secret,
		       sizeof(ctx->enc_secret));
		memcpy(ctx->dec_secret, keys->h2d_secret,
		       sizeof(ctx->dec_secret));
#endif
		enc_key = keys->d2h_key;
		dec_key = keys->h2d_key;
		break;
	default:
		return E_INVALID;
	}

	ret = nanotls_aes_ctx_init(&ctx->aes_enc, enc_key);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_ctx_init(&ctx->aes_dec, dec_key);
	if (ret != E_OK)
		return ret;

	ctx->dec_tag_mismatch_count = 0;

	return E_OK;
}

enum ecode nanotls_data_ctx_destroy(struct nanotls_ctx_data *ctx)
{
	enum ecode ret;

	ret = nanotls_aes_ctx_destroy(&ctx->aes_enc);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_ctx_destroy(&ctx->aes_dec);
	if (ret != E_OK)
		return ret;

	memset(ctx, 0, sizeof(*ctx));

	ctx->role = NANOTLS_ROLE_UNKNOWN;

	return E_OK;
}

enum ecode nanotls_data_write_start(struct nanotls_ctx_data *ctx,
				    size_t data_size)
{
	struct nanotls_data_add add;
	uint8_t nonce[NANOTLS_AES_IV_SIZE];
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	ret = nanotls_data_add_init(&add, data_size);
	if (ret != E_OK)
		return ret;

	nanotls_data_get_nonce(nonce, ctx->enc_iv, ctx->enc_seq_no);

	ret = nanotls_aes_start_enc(&ctx->aes_enc, nonce, &add, sizeof(add));
	if (ret != E_OK)
		return ret;

	ctx->enc_len_remaining = data_size;

	return E_OK;
}

enum ecode nanotls_data_write_update(struct nanotls_ctx_data *ctx,
				     void *enc_out, const void *plain_in,
				     size_t data_size)
{
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	if (data_size > ctx->enc_len_remaining)
		return E_INVALID;

	ret = nanotls_aes_update(&ctx->aes_enc, enc_out, plain_in, data_size);
	if (ret != E_OK)
		return ret;

	ctx->enc_len_remaining -= data_size;

	return E_OK;
}

enum ecode nanotls_data_write_finish(struct nanotls_ctx_data *ctx,
				     uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	if (ctx->enc_len_remaining > 0)
		return E_INVALID;

	ret = nanotls_aes_finish(&ctx->aes_enc, tag);
	if (ret != E_OK)
		return ret;

	ctx->enc_seq_no++;
	uassert(ctx->enc_seq_no != 0); /* should never happen */

	return E_OK;
}

enum ecode nanotls_data_write(struct nanotls_ctx_data *ctx, void *enc_out,
			      const void *plain_in, size_t data_size,
			      uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	enum ecode ret;

	ret = nanotls_data_write_start(ctx, data_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_data_write_update(ctx, enc_out, plain_in, data_size);
	if (ret != E_OK)
		return ret;

	return nanotls_data_write_finish(ctx, tag);
}

enum ecode nanotls_data_read_start(struct nanotls_ctx_data *ctx,
				   size_t data_size)
{
	struct nanotls_data_add add;
	uint8_t nonce[NANOTLS_AES_IV_SIZE];
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	ret = nanotls_data_add_init(&add, data_size);
	if (ret != E_OK)
		return ret;

	nanotls_data_get_nonce(nonce, ctx->dec_iv, ctx->dec_seq_no);

	ret = nanotls_aes_start_dec(&ctx->aes_dec, nonce, &add, sizeof(add));
	if (ret != E_OK)
		return ret;

	ctx->dec_len_remaining = data_size;

	return E_OK;
}

enum ecode nanotls_data_read_update(struct nanotls_ctx_data *ctx,
				    void *plain_out, const void *enc_in,
				    size_t data_size)
{
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	if (data_size > ctx->dec_len_remaining)
		return E_INVALID;

	ret = nanotls_aes_update(&ctx->aes_dec, plain_out, enc_in, data_size);
	if (ret != E_OK)
		return ret;

	ctx->dec_len_remaining -= data_size;

	return E_OK;
}

enum ecode nanotls_data_read_finish(struct nanotls_ctx_data *ctx,
				    const uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	uint8_t computed_tag[NANOTLS_AES_GCM_TAG_SIZE];
	enum ecode ret;

	ret = nanotls_data_check_ctx(ctx);
	if (ret != E_OK)
		return ret;

	if (ctx->dec_len_remaining > 0)
		return E_INVALID;

	ret = nanotls_aes_finish(&ctx->aes_dec, computed_tag);
	if (ret != E_OK)
		return ret;

	ret = nanotls_mem_eq(computed_tag, tag, NANOTLS_AES_GCM_TAG_SIZE);
	if (ret != E_OK) {
		ctx->dec_tag_mismatch_count++;
#ifdef NANOTLS_CONFIG_MAX_DATA_TAG_MISMATCH_CNT
		if (ctx->dec_tag_mismatch_count >
		    NANOTLS_CONFIG_MAX_DATA_TAG_MISMATCH_CNT)
			nanotls_data_ctx_destroy(ctx);
#endif
		return ret;
	}

	ctx->dec_seq_no++;
	uassert(ctx->dec_seq_no != 0); /* should never happen */

	ctx->dec_tag_mismatch_count = 0;

	return E_OK;
}

enum ecode nanotls_data_read(struct nanotls_ctx_data *ctx, void *plain_out,
			     const void *enc_in, size_t data_size,
			     const uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE])
{
	enum ecode ret;

	ret = nanotls_data_read_start(ctx, data_size);
	if (ret != E_OK)
		return ret;

	ret = nanotls_data_read_update(ctx, plain_out, enc_in, data_size);
	if (ret != E_OK)
		return ret;

	return nanotls_data_read_finish(ctx, tag);
}

#ifdef NANOTLS_CONFIG_KEY_UPDATE

static enum ecode nanotls_data_key_update(nanotls_aes_context *aes_ctx,
					  uint8_t iv[NANOTLS_IV_SIZE],
					  uint8_t secret[NANOTLS_SECRET_SIZE])
{
	uint8_t key[NANOTLS_KEY_SIZE];
	enum ecode ret;

	ret = nanotls_update_traffic_secret(secret, key, iv);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_ctx_destroy(aes_ctx);
	if (ret != E_OK)
		return ret;

	ret = nanotls_aes_ctx_init(aes_ctx, key);
	if (ret != E_OK)
		return ret;

	nanotls_mem_erase(key, sizeof(key));

	return E_OK;
}

enum ecode nanotls_data_write_key_update(struct nanotls_ctx_data *ctx)
{
	uint8_t old_iv[NANOTLS_IV_SIZE];
	enum ecode ret;

	memcpy(old_iv, ctx->enc_iv, sizeof(old_iv));

	ret = nanotls_data_key_update(&ctx->aes_enc, ctx->enc_iv,
				      ctx->enc_secret);
	if (ret != E_OK)
		return ret;

	/* old IV and new IV should not match */
	uassert(nanotls_mem_eq(old_iv, ctx->enc_iv, sizeof(old_iv)) != E_OK);

	ctx->enc_seq_no = 0;

	return E_OK;
}

enum ecode nanotls_data_read_key_update(struct nanotls_ctx_data *ctx)
{
	uint8_t old_iv[NANOTLS_IV_SIZE];
	enum ecode ret;

	memcpy(old_iv, ctx->dec_iv, sizeof(old_iv));

	ret = nanotls_data_key_update(&ctx->aes_dec, ctx->dec_iv,
				      ctx->dec_secret);
	if (ret != E_OK)
		return ret;

	/* old IV and new IV should not match */
	uassert(nanotls_mem_eq(old_iv, ctx->dec_iv, sizeof(old_iv)) != E_OK);

	ctx->dec_seq_no = 0;

	return E_OK;
}

#endif
