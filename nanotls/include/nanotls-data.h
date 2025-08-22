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

#ifndef NANOTLS_DATA_H
#define NANOTLS_DATA_H

#include <linux/types.h>
#include <asm/string.h>
#include <nanotls-common.h>
#include <nanotls-proto.h>
#include <nanotls-hooks.h>

/*
 * NanoTLS data layer - used for message encryption/decryption
 * once handshake is completed.
 */

struct nanotls_ctx_data {
	enum nanotls_role role;

	uint64_t enc_seq_no;
	uint8_t enc_iv[NANOTLS_IV_SIZE];
	nanotls_aes_context aes_enc;
	size_t enc_len_remaining;

	uint64_t dec_seq_no;
	uint8_t dec_iv[NANOTLS_IV_SIZE];
	nanotls_aes_context aes_dec;
	size_t dec_len_remaining;
	size_t dec_tag_mismatch_count;

#ifdef NANOTLS_CONFIG_KEY_UPDATE
	uint8_t enc_secret[NANOTLS_SECRET_SIZE];
	uint8_t dec_secret[NANOTLS_SECRET_SIZE];
#endif
};

enum ecode nanotls_data_ctx_init(struct nanotls_ctx_data *ctx,
				 enum nanotls_role role,
				 const struct nanotls_traffic_keys *keys);
enum ecode nanotls_data_ctx_destroy(struct nanotls_ctx_data *ctx);

enum ecode nanotls_data_write_start(struct nanotls_ctx_data *ctx,
				    size_t data_size);
/*
 * All but the final invocation MUST be called with length mod 16 == 0.
 * Only the final call can have a partial block length of < 128 bits.
 */
enum ecode nanotls_data_write_update(struct nanotls_ctx_data *ctx,
				     void *enc_out, const void *plain_in,
				     size_t data_size);
enum ecode nanotls_data_write_finish(struct nanotls_ctx_data *ctx,
				     uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

enum ecode nanotls_data_write(struct nanotls_ctx_data *ctx, void *enc_out,
			      const void *plain_in, size_t data_size,
			      uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

enum ecode nanotls_data_read_start(struct nanotls_ctx_data *ctx,
				   size_t data_size);
/*
 * All but the final invocation MUST be called with length mod 16 == 0.
 * Only the final call can have a partial block length of < 128 bits.
 */
enum ecode nanotls_data_read_update(struct nanotls_ctx_data *ctx,
				    void *plain_out, const void *enc_in,
				    size_t data_size);
enum ecode
nanotls_data_read_finish(struct nanotls_ctx_data *ctx,
			 const uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

enum ecode nanotls_data_read(struct nanotls_ctx_data *ctx, void *plain_out,
			     const void *enc_in, size_t data_size,
			     const uint8_t tag[NANOTLS_AES_GCM_TAG_SIZE]);

#ifdef NANOTLS_CONFIG_KEY_UPDATE
enum ecode nanotls_data_write_key_update(struct nanotls_ctx_data *ctx);
enum ecode nanotls_data_read_key_update(struct nanotls_ctx_data *ctx);
#endif

#endif /* NANOTLS_DATA_H */
