/** @file moal_shc.c
 *
 * @brief This file contains callback functions for secure host interface.
 *
 *
 * Copyright 2025 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code (Materials) are owned by NXP, its
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
 *
 *  Alternatively, this software may be distributed under the terms of GPL v2.
 *  SPDX-License-Identifier:    GPL-2.0
 *
 */

#include "moal_shc.h"

t_u8 public_key[NANOTLS_ECDSA_PUBLIC_KEY_SIZE];
t_u8 fw_uuid[NANOTLS_UUID_LEN];

/**
 *  @brief This function determines secure host message id
 *
 *  @param msg       A pointer to message
 *
 *  @return          success:valid message id, invalid otherwise
 */
t_u8 moal_secure_host_get_msg_id(t_void *msg)
{
	struct nanotls_message_hdr *hdr = (struct nanotls_message_hdr *)msg;
	return hdr->message_id;
}

/**
 *  @brief This function reads publick key extracted from fw to nanotls library
 *
 *  @param ctx               A pointer to nanotls host context
 *  @param device_info       A pointer to nanotls device info
 *  @ecdsa_pub_key           Public key
 *
 *  @return                  E_OK
 */
static enum ecode
moal_get_pub_key(const struct nanotls_host_ctx *ctx,
		 const struct nanotls_device_info *device_info,
		 uint8_t ecdsa_pub_key[NANOTLS_ECDSA_PUBLIC_KEY_SIZE])
{
	memcpy(ecdsa_pub_key, public_key, NANOTLS_ECDSA_PUBLIC_KEY_SIZE);
	return E_OK;
}

/**
 *  @brief This function initialized host context
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param key       A pointer to public key
 *  @uuid            Device uuid
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_init(t_void *pmoal,
				  const t_u8 key[NANOTLS_ECDSA_PUBLIC_KEY_SIZE],
				  const t_u8 uuid[NANOTLS_UUID_LEN])
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;

	handle->secure = kzalloc(sizeof(secure_host_t), GFP_KERNEL);
	if (!handle->secure) {
		PRINTM(MERROR, "Failed to alloc memory for shc handler\n");
		return MLAN_STATUS_FAILURE;
	}

	secure_host_t *secure = (secure_host_t *)handle->secure;
	secure->host_ctx = kzalloc(sizeof(struct nanotls_host_ctx), GFP_KERNEL);
	if (!secure->host_ctx) {
		PRINTM(MERROR, "Failed to alloc memory for shc context\n");
		return MLAN_STATUS_FAILURE;
	}

	memcpy(public_key, key, NANOTLS_ECDSA_PUBLIC_KEY_SIZE);
	memcpy(fw_uuid, uuid, NANOTLS_UUID_LEN);

	ret = nanotls_host_init_pubkey_cb(secure->host_ctx, moal_get_pub_key);
	if (ret)
		PRINTM(MERROR, "Failed to init shc context\n");

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function free up secure host memory
 *
 *  @param pmoal     A pointer to moal_handle structure
 *
 *  @return          N/A
 */
void moal_secure_host_cleanup(t_void *pmoal)
{
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	int idx = 0;

	if (!handle->secure)
		return;

	PRINTM(MMSG, "Free secure host context\n");

	if (secure->host_ctx)
		nanotls_host_cleanup(secure->host_ctx);

	for (idx = 0; idx < MAX_CTX_AND_KEY; idx++) {
		if (secure->data_ctx[idx])
			kfree(secure->data_ctx[idx]);
		if (secure->host_keys[idx])
			kfree(secure->host_keys[idx]);
	}

	if (secure->host_finished)
		kfree(secure->host_finished);

	if (secure->device_hello)
		kfree(secure->device_hello);

	if (secure->host_hello)
		kfree(secure->host_hello);

	if (secure->host_ctx)
		kfree(secure->host_ctx);

	kfree(secure);
}

/**
 *  @brief This function prepares host do hello message
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param msg       A pointer to do hello message
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_do_hello(t_void *pmoal, t_void **msg)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;

	secure->host_hello =
		kzalloc(sizeof(struct nanotls_host_hello), GFP_KERNEL);
	if (!secure->host_hello) {
		PRINTM(MERROR, "Failed to alloc memory for host hello\n");
		return MLAN_STATUS_FAILURE;
	}

	*msg = kzalloc(sizeof(struct nanotls_host_hello), GFP_KERNEL);
	if (!secure->host_hello) {
		PRINTM(MERROR, "Failed to alloc memory for host hello msg\n");
		return MLAN_STATUS_FAILURE;
	}

	ret = nanotls_host_do_hello(secure->host_ctx, secure->host_hello);
	if (!ret) {
		moal_memcpy_ext(handle, *msg, secure->host_hello,
				sizeof(struct nanotls_host_hello),
				sizeof(struct nanotls_host_hello));
	} else {
		PRINTM(MERROR, "Failed to generate host hello\n");
	}

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function process received device hello message
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param msg       A pointer to device hello message
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_device_hello_rcvd(t_void *pmoal, t_void *msg)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	secure->device_hello =
		kzalloc(sizeof(struct nanotls_device_hello), GFP_KERNEL);
	if (!secure->device_hello) {
		PRINTM(MERROR, "Failed to alloc memory for device hello\n");
		return MLAN_STATUS_FAILURE;
	}

	moal_memcpy_ext(handle, secure->device_hello, msg,
			sizeof(struct nanotls_device_hello),
			sizeof(struct nanotls_device_hello));

	ret = nanotls_host_device_hello_rcvd(secure->host_ctx,
					     secure->device_hello);
	if (ret)
		PRINTM(MERROR, "Failed to process device hello\n");

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares secure host finished message
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param msg       A pointer to secure host finished message
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_do_finished(t_void *pmoal, t_void **msg)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	secure->host_finished =
		kzalloc(sizeof(struct nanotls_host_finished), GFP_KERNEL);
	if (!secure->host_finished) {
		PRINTM(MERROR, "Failed to alloc memory for host finished\n");
		return MLAN_STATUS_FAILURE;
	}

	*msg = kzalloc(sizeof(struct nanotls_host_finished), GFP_KERNEL);
	if (!(*msg)) {
		PRINTM(MERROR, "Failed to alloc memory for host finish msg\n");
		return MLAN_STATUS_FAILURE;
	}

	ret = nanotls_host_do_finished(secure->host_ctx, secure->host_finished);
	if (!ret) {
		moal_memcpy_ext(handle, *msg, secure->host_finished,
				sizeof(struct nanotls_host_finished),
				sizeof(struct nanotls_host_finished));
	} else {
		PRINTM(MERROR, "Failed to generate host finished msg\n");
	}

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function derives traffice keys
 *
 *  @param pmoal     A pointer to moal_handle structure
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_derive_traffic_keys(t_void *pmoal)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	t_u8 idx = 0;

	idx = handle->second_mac;
	secure->host_keys[idx] =
		kzalloc(sizeof(struct nanotls_traffic_keys), GFP_KERNEL);
	if (!secure->host_keys[idx]) {
		PRINTM(MERROR, "Failed to alloc memory for host keys\n");
		return MLAN_STATUS_FAILURE;
	}

	ret = nanotls_host_derive_traffic_keys_instance(
		secure->host_ctx, secure->host_keys[idx], idx);
	if (ret)
		PRINTM(MERROR, "Failed to derive host keys\n");

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function initializes data context
 *
 *  @param pmoal     A pointer to moal_handle structure
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_data_ctx_init(t_void *pmoal)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	t_u8 idx = 0;
	idx = handle->second_mac;
	secure->data_ctx[idx] =
		kzalloc(sizeof(struct nanotls_ctx_data), GFP_KERNEL);
	if (!secure->data_ctx[idx]) {
		PRINTM(MERROR, "Failed to alloc memory for data context\n");
		return MLAN_STATUS_FAILURE;
	}

	ret = nanotls_data_ctx_init(secure->data_ctx[idx], NANOTLS_ROLE_HOST,
				    secure->host_keys[idx]);
	if (ret)
		PRINTM(MERROR, "Failed to init data context\n");

	return (ret != 0) ? MLAN_STATUS_FAILURE : MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function encrypts the payload
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param enc_data  A pointer to encrypted data
 *  @param payload   A pointer to payload to be encrpted
 *  @param len       Payload length
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_data_encrypt(t_void *pmoal, t_void **enc_data,
					  t_void **payload, t_u32 len)
{
	int ret = 0;
	t_u8 tag[NANOTLS_AES_GCM_TAG_SIZE];
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	t_u8 idx = 0;

	if (!secure) {
		PRINTM(MERROR, "encrypt:shc context not initialized\n");
		return MLAN_STATUS_FAILURE;
	}

	idx = handle->second_mac;
	ret = nanotls_data_write(secure->data_ctx[idx], *enc_data, *payload,
				 len, tag);
	if (!ret) {
		moal_memcpy_ext(handle, *payload, *enc_data, len, len);
		moal_memcpy_ext(handle, *payload + len, tag,
				NANOTLS_AES_GCM_TAG_SIZE,
				NANOTLS_AES_GCM_TAG_SIZE);
		return MLAN_STATUS_SUCCESS;
	}

	PRINTM(MERROR, "Failed to encrypt command request\n");
	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief This function decrypts the payload
 *
 *  @param pmoal     A pointer to moal_handle structure
 *  @param dec_data  A pointer to decrypted data
 *  @param payload   A pointer to encrypted payload
 *  @param len       Payload length
 *
 *  @return          sucess:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status moal_secure_host_data_decrypt(t_void *pmoal, t_void **dec_data,
					  t_void **payload, t_u32 len)
{
	int ret = 0;
	moal_handle *handle = (moal_handle *)pmoal;
	secure_host_t *secure = (secure_host_t *)handle->secure;
	t_u8 idx = 0;

	if (!secure) {
		PRINTM(MERROR, "decrypt: shc context not initialized\n");
		return MLAN_STATUS_FAILURE;
	}

	idx = handle->second_mac;

	ret = nanotls_data_read(secure->data_ctx[idx], *dec_data, *payload,
				len - NANOTLS_AES_GCM_TAG_SIZE,
				*payload + (len - NANOTLS_AES_GCM_TAG_SIZE));
	if (!ret) {
		moal_memcpy_ext(handle, *payload, *dec_data,
				len - NANOTLS_AES_GCM_TAG_SIZE,
				len - NANOTLS_AES_GCM_TAG_SIZE);
		return MLAN_STATUS_SUCCESS;
	}

	PRINTM(MERROR, "Failed to decrypt command response\n");
	return MLAN_STATUS_FAILURE;
}
