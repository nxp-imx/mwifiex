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

#include <nanotls-host.h>
#include <nanotls-common.h>
#include <asm/string.h>
#include <p256-m.h>

static enum ecode nanotls_host_handle_error(struct nanotls_host_ctx *ctx,
					    enum ecode ret)
{
	ctx->error_count++;

#ifdef NANOTLS_CONFIG_MAX_ERR_CNT
	if (ctx->error_count >= NANOTLS_CONFIG_MAX_ERR_CNT) {
		nanotls_host_cleanup(ctx);
		ctx->state = NANOTLS_HOST_ERR;
	}
#endif

	return ret;
}

enum ecode
nanotls_host_init(struct nanotls_host_ctx *ctx,
		  const uint8_t ecdsa_pub[NANOTLS_ECDSA_PUBLIC_KEY_SIZE])
{
	enum ecode ret;

	ret = nanotls_secure_random(ctx, sizeof(*ctx));
	if (ret != E_OK)
		return ret;

	nanotls_sha256_init(&ctx->handshake_hash);

	if (ecdsa_pub) {
		memcpy(ctx->ecdsa_pub, ecdsa_pub, sizeof(ctx->ecdsa_pub));
	}

	ctx->error_count = 0;
	ctx->state = NANOTLS_HOST_INIT;
	ctx->pub_key_req_cb = NULL;

	return E_OK;
}

enum ecode
nanotls_host_init_pubkey_cb(struct nanotls_host_ctx *ctx,
			    nanotls_host_pub_key_req_cb pub_key_req_cb)
{
	enum ecode ret;

	ret = nanotls_host_init(ctx, NULL);
	if (ret != E_OK)
		return ret;

	ctx->pub_key_req_cb = pub_key_req_cb;

	return E_OK;
}

enum ecode nanotls_host_do_hello(struct nanotls_host_ctx *ctx,
				 struct nanotls_host_hello *msg)
{
	enum ecode ret = E_INVALID;

	if (ctx->state != NANOTLS_HOST_INIT)
		return nanotls_host_handle_error(ctx, E_INVALID);

	memset(msg, 0, sizeof(*msg));
	nanotls_hdr_init(&msg->hdr, sizeof(*msg), NANOTLS_HOST_HELLO);

	msg->sig_alg = NANOTLS_ECDSA_SECP256R1_SHA256;
	msg->key_exchange_type = NANOTLS_ECDHE_SECP256R1;
	msg->cipher_suite = NANOTLS_AES_128_GCM_SHA256;

	/*
	 * Host's nonce - used for device authentication by host,
	 * replay protection, and MiTM attack protection.
	 */
	ret = nanotls_secure_random(msg->random, sizeof(msg->random));
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	/* Host's ECDHE key share (ephemeral public key) */
	ret = nanotls_p256_gen_keypair(ctx->ecdh_priv,
				       msg->host_key_share.pubkey);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	/*
	 * Feed the entire HostHello to the transcript hash context.
	 * Important: our copy, hashed before sending (i.e. trusted).
	 */
	nanotls_sha256_update(&ctx->handshake_hash, msg, sizeof(*msg));

	ctx->state = NANOTLS_HOST_HELLO_SENT;

	return E_OK;
}

static enum ecode
nanotls_host_device_hello_sig_verify(const struct nanotls_host_ctx *ctx,
				     const struct nanotls_device_hello *msg)
{
	uint8_t hash_sig[NANOTLS_HASH_SIZE];

	/* Produce the hash value to sign, as defined in TLS */
	nanotls_handshake_sig_hash(ctx->handshake_h2_hash,
				   NANOTLS_DEVICE_HS_SIG_LABEL, hash_sig);

	return nanotls_p256_ecdsa_verify(msg->enc.device_handshake_sig.sig,
					 ctx->ecdsa_pub, hash_sig,
					 sizeof(hash_sig));
}

static enum ecode
nanotls_host_derive_hs_secrets(struct nanotls_host_ctx *ctx,
			       uint8_t hs_traffic_secret[NANOTLS_SECRET_SIZE])
{
	uint8_t shared_secret[NANOTLS_SECRET_SIZE];
	enum ecode ret = E_INVALID;

	/* Compute ECDHE shared secret */
	ret = nanotls_p256_ecdh_shared_secret(shared_secret, ctx->ecdh_priv,
					      ctx->device_key_share.pubkey);
	if (ret != E_OK)
		return ret;

	/* Erase ECDHE keys */
	nanotls_mem_erase(ctx->ecdh_priv, sizeof(ctx->ecdh_priv));
	nanotls_mem_erase(ctx->device_key_share.pubkey,
			  sizeof(ctx->device_key_share.pubkey));

	/*
	 * handshake_secret = HKDF-Extract(salt = "", ikm = ecdh_shared_secret)
	 *
	 * TLS deviation: it takes a different fixed value of salt
	 * since we don't support PSK, we just use empty hash as fixed value for
	 * simplification (one fixed value vs another one).
	 */
	nanotls_hkdf_sha256_extract(NULL, 0, shared_secret,
				    sizeof(shared_secret),
				    ctx->handshake_secret);

	/* Erase hared secret as it's no longer needed */
	nanotls_mem_erase(shared_secret, sizeof(shared_secret));

	/*
	 * Derive handshake traffic secret with Derive-Secret (HKDF-Expand)
	 * hs_traffic_secret = Derive-Secret(handshake_secret, label, hash) =
	 *                   = HKDF-Expand(handshake_secret, 32 | label |
	 * transcript_hash, 32)
	 */
	nanotls_hkdf_derive_secret(ctx->handshake_secret,
				   NANOTLS_DEVICE_HS_TS_LABEL,
				   ctx->handshake_h2_hash, hs_traffic_secret);

	return E_OK;
}

static enum ecode nanotls_host_verify_device_finished(
	const struct nanotls_host_ctx *ctx,
	const struct nanotls_device_hello *msg,
	const uint8_t hs_traffic_secret[NANOTLS_SECRET_SIZE])
{
	uint8_t verify_data[NANOTLS_SHA256_HASH_SIZE];

	nanotls_write_finished(&ctx->handshake_hash, hs_traffic_secret,
			       verify_data);

	return nanotls_mem_eq(verify_data, msg->enc.device_finished.verify_data,
			      sizeof(verify_data));
}

enum ecode nanotls_host_device_hello_rcvd(struct nanotls_host_ctx *ctx,
					  struct nanotls_device_hello *msg)
{
	const size_t sig_cutoff_pos = NANOTLS_DEVICE_HELLO_SIG_CUTOFF_POS;
	uint8_t dev_hs_traffic_secret[NANOTLS_SECRET_SIZE];
	enum ecode ret = E_INVALID;

	if (ctx->state != NANOTLS_HOST_HELLO_SENT)
		return nanotls_host_handle_error(ctx, E_INVALID);

	ret = nanotls_hdr_check(&msg->hdr, sizeof(*msg), NANOTLS_DEVICE_HELLO);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	/*
	 * [H2]
	 * Feed the DeviceHello content up until the handshake_sig to transcript
	 * hash context.
	 */
	nanotls_sha256_update(&ctx->handshake_hash, msg, sig_cutoff_pos);
	nanotls_sha256_digest(&ctx->handshake_hash,
			      ctx->handshake_h2_hash); /* [H2] */

	/* Save Device's ECDHE key share (ephemeral public key) */
	ctx->device_key_share = msg->device_key_share;

	/*
	 * All ECDHE steps have been completed, now calculate ECDHE shared
	 * secret and derive Handshake Secret from it.
	 */
	ret = nanotls_host_derive_hs_secrets(ctx, dev_hs_traffic_secret);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	ret = nanotls_handshake_decrypt_verify(&msg->enc, sizeof(msg->enc),
					       &msg->auth_tag,
					       dev_hs_traffic_secret);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	memcpy(&ctx->device_info, &msg->enc.device_info,
	       sizeof(ctx->device_info));

	/*
	 * Verify ECDSA signature handshake_sig using Device's public key.
	 *
	 * This is the key point where Device authentication happens:
	 * - Host generates a random (HostHello.random)
	 * - Device signs the entire handshake (incl. Host's random) with its
	 *   private key (DeviceHello.device_handshake_sig)
	 * - Host now verifies ECDSA signature generated by device using
	 * Device's public key
	 *
	 * Only the device that possesses the proper private key could sign the
	 * Host's random.
	 * For this reason Host's random should be of a good quality and never
	 * repeat.
	 *
	 * If the device is an impostor and does not pose a valid private key,
	 * the handshake will fail at this point.
	 */
	if (ctx->pub_key_req_cb) {
		/* device public key is not valid, and we need to first fetch it
		 * using callback */
		ret = ctx->pub_key_req_cb(ctx, &msg->enc.device_info,
					  ctx->ecdsa_pub);
		if (ret != E_OK)
			return nanotls_host_handle_error(ctx, ret);
	}

	ret = nanotls_host_device_hello_sig_verify(ctx, msg);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	/* [H7] Update handshake transcript with the part before Finished */
	nanotls_sha256_update(&ctx->handshake_hash, &msg->enc,
			      NANOTLS_DEVICE_HELLO_FINISHED_ENC_CUTOFF_POS);

	/*
	 * Verify device_finished message.
	 * It's a MAC over transcript hash computed using handshake secret
	 * computed by the Device.
	 */
	ret = nanotls_host_verify_device_finished(ctx, msg,
						  dev_hs_traffic_secret);
	if (ret != E_OK)
		return nanotls_host_handle_error(ctx, ret);

	/* [H3] Update handshake transcript with just processed device_finished
	 */
	nanotls_sha256_update(&ctx->handshake_hash, &msg->enc.device_finished,
			      sizeof(msg->enc.device_finished));

	nanotls_mem_erase(dev_hs_traffic_secret, sizeof(dev_hs_traffic_secret));

	ctx->state = NANOTLS_HOST_HELLO_RCVD;

	return E_OK;
}

enum ecode nanotls_host_get_device_info(const struct nanotls_host_ctx *ctx,
					struct nanotls_device_info *device_info)
{
	switch (ctx->state) {
	case NANOTLS_HOST_HELLO_RCVD:
	case NANOTLS_HOST_DONE:
		break;
	default:
		return E_INVALID;
	}

	memcpy(device_info, &ctx->device_info, sizeof(ctx->device_info));

	return E_OK;
}

enum ecode nanotls_host_do_finished(struct nanotls_host_ctx *ctx,
				    struct nanotls_host_finished *msg)
{
	const size_t finished_cutoff_pos =
		NANOTLS_HOST_HELLO_FINISHED_CUTOFF_POS;
	uint8_t host_hs_traffic_secret[NANOTLS_SECRET_SIZE];

	if (ctx->state != NANOTLS_HOST_HELLO_RCVD)
		return nanotls_host_handle_error(ctx, E_INVALID);

	memset(msg, 0, sizeof(*msg));
	nanotls_hdr_init(&msg->hdr, sizeof(*msg), NANOTLS_HOST_FINISHED);

	/* [H9] Update handshake transcript with the starting part of
	 * HostFinished */
	nanotls_sha256_update(&ctx->handshake_hash, msg, finished_cutoff_pos);

	/*
	 * Derive handshake traffic secret with Derive-Secret (HKDF-Expand)
	 * hs_traffic_secret = Derive-Secret(handshake_secret, label, hash) =
	 *                   = HKDF-Expand(handshake_secret, 32 | label |
	 * transcript_hash, 32)
	 */
	nanotls_hkdf_derive_secret(ctx->handshake_secret,
				   NANOTLS_HOST_HS_TS_LABEL,
				   ctx->handshake_h2_hash,
				   host_hs_traffic_secret);

	/* Produce host_finished message (MAC over transcript hash [H9]). */
	nanotls_write_finished(&ctx->handshake_hash, host_hs_traffic_secret,
			       msg->enc.host_finished.verify_data);

	/*
	 * [H4] - Full transcript hash
	 * Update handshake transcript with just created host_finished.
	 * H4 = SHA256(HostHello | DeviceHello | HostFinished)
	 */
	nanotls_sha256_update(&ctx->handshake_hash, &msg->enc.host_finished,
			      sizeof(msg->enc.host_finished));

	nanotls_handshake_encrypt(&msg->enc, sizeof(msg->enc), &msg->auth_tag,
				  host_hs_traffic_secret);

	/*
	 * At this point, handshake is completed from the Host's perspective.
	 * Host Authenticated the Device and has everything to derive the final
	 * Master Secret.
	 *
	 * Now call nanotls_host_derive_traffic_keys() to get the encryption
	 * keys.
	 */
	nanotls_derive_master_secret(ctx->master_secret, ctx->handshake_secret);

	nanotls_mem_erase(host_hs_traffic_secret,
			  sizeof(host_hs_traffic_secret));

	ctx->state = NANOTLS_HOST_DONE;

	return E_OK;
}

enum ecode
nanotls_host_derive_traffic_keys_instance(const struct nanotls_host_ctx *ctx,
					  struct nanotls_traffic_keys *keys,
					  size_t instance_idx)
{
	uint8_t transcript_hash[NANOTLS_SHA256_HASH_SIZE];
	uint8_t instance_label_hash[NANOTLS_SHA256_HASH_SIZE];

	if (ctx->state != NANOTLS_HOST_DONE)
		return E_INVALID;

	nanotls_hkdf_sha256_extract(&instance_idx, sizeof(instance_idx),
				    NANOTLS_KEY_INSTANCE_LABEL,
				    instance_label_hash);

	nanotls_sha256_digest(&ctx->handshake_hash, transcript_hash); /* [H4] */
	nanotls_derive_traffic_keys(ctx->master_secret, transcript_hash,
				    instance_label_hash,
				    sizeof(instance_label_hash), keys);

	return E_OK;
}

enum ecode nanotls_host_derive_traffic_keys(const struct nanotls_host_ctx *ctx,
					    struct nanotls_traffic_keys *keys)
{
	uint8_t hash[NANOTLS_SHA256_HASH_SIZE];

	if (ctx->state != NANOTLS_HOST_DONE)
		return E_INVALID;

	nanotls_sha256_digest(&ctx->handshake_hash, hash); /* [H4] */
	nanotls_derive_traffic_keys(ctx->master_secret, hash, NULL, 0, keys);

	return E_OK;
}

enum ecode nanotls_host_cleanup(struct nanotls_host_ctx *ctx)
{
	nanotls_mem_erase(ctx, sizeof(*ctx));

	ctx->state = NANOTLS_HOST_UNINIT;

	return E_OK;
}
