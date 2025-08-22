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

#ifndef AES_HEADER
#define AES_HEADER

#include <linux/types.h>
#include <asm/string.h>

#define AES_CONFIG_128_BIT_ONLY

enum aes_op_mode {
	AES_MODE_NONE = 0,
	AES_MODE_ENCRYPT = 0x231d14,
	AES_MODE_DECRYPT = 0x1676a1,
};

/******************************************************************************
 *  AES_CONTEXT : cipher context / holds inter-call data
 ******************************************************************************/
typedef struct {
	int rounds; // keysize-based rounds count
	uint32_t *rk; // pointer to current round key
	uint32_t buf[68]; // key expansion buffer
} aes_context;

/******************************************************************************
 *  AES_SETKEY : called to expand the key for encryption or decryption
 ******************************************************************************/
int aes_setkey(aes_context *ctx, // pointer to context
	       const uint8_t *key, // AES input key
	       size_t keysize); // size in bytes (must be 16, 24, 32 for
				// 128, 192 or 256-bit keys respectively)
				// returns 0 for success

/******************************************************************************
 *  AES_CIPHER : called to encrypt or decrypt ONE 128-bit block of data
 ******************************************************************************/
int aes_cipher(aes_context *ctx, // pointer to context
	       const uint8_t input[16], // 128-bit block to en/decipher
	       uint8_t output[16]); // 128-bit output result block
				    // returns 0 for success

#endif /* AES_HEADER */
