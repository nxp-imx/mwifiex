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

#include "aes.h"

/*
 *  The following static local tables must be filled-in before the first use of
 *  the GCM or AES ciphers. They are used for the AES key expansion/scheduling
 *  and once built are read-only and thread safe.
 *
 *  NOTE: GCM Encryption/Decryption does NOT REQUIRE AES decryption. Since
 *  GCM uses AES in counter-mode, where the AES cipher output is XORed with
 *  the GCM input, we ONLY NEED AES encryption.
 */

#include <aes_tables.h>

enum aes_rounds {
	AES_128_ROUNDS = 10,
	AES_192_ROUNDS = 12,
	AES_256_ROUNDS = 14,
};

/*
 * Platform Endianness Neutralizing Load and Store Macro definitions
 * AES wants platform-neutral Little Endian (LE) byte ordering
 */
#define GET_UINT32_LE(n, b, i)                                                 \
	{                                                                      \
		(n) = ((uint32_t)(b)[(i)]) | ((uint32_t)(b)[(i) + 1] << 8) |   \
		      ((uint32_t)(b)[(i) + 2] << 16) |                         \
		      ((uint32_t)(b)[(i) + 3] << 24);                          \
	}

#define PUT_UINT32_LE(n, b, i)                                                 \
	{                                                                      \
		(b)[(i)] = (uint8_t)((n));                                     \
		(b)[(i) + 1] = (uint8_t)((n) >> 8);                            \
		(b)[(i) + 2] = (uint8_t)((n) >> 16);                           \
		(b)[(i) + 3] = (uint8_t)((n) >> 24);                           \
	}

/*
 *  AES forward and reverse encryption round processing macros
 */
#define AES_FROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3)                             \
	{                                                                      \
		X0 = *RK++ ^ FT0[(Y0)&0xFF] ^ FT1[(Y1 >> 8) & 0xFF] ^          \
		     FT2[(Y2 >> 16) & 0xFF] ^ FT3[(Y3 >> 24) & 0xFF];          \
                                                                               \
		X1 = *RK++ ^ FT0[(Y1)&0xFF] ^ FT1[(Y2 >> 8) & 0xFF] ^          \
		     FT2[(Y3 >> 16) & 0xFF] ^ FT3[(Y0 >> 24) & 0xFF];          \
                                                                               \
		X2 = *RK++ ^ FT0[(Y2)&0xFF] ^ FT1[(Y3 >> 8) & 0xFF] ^          \
		     FT2[(Y0 >> 16) & 0xFF] ^ FT3[(Y1 >> 24) & 0xFF];          \
                                                                               \
		X3 = *RK++ ^ FT0[(Y3)&0xFF] ^ FT1[(Y0 >> 8) & 0xFF] ^          \
		     FT2[(Y1 >> 16) & 0xFF] ^ FT3[(Y2 >> 24) & 0xFF];          \
	}

#define AES_RROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3)                             \
	{                                                                      \
		X0 = *RK++ ^ RT0[(Y0)&0xFF] ^ RT1[(Y3 >> 8) & 0xFF] ^          \
		     RT2[(Y2 >> 16) & 0xFF] ^ RT3[(Y1 >> 24) & 0xFF];          \
                                                                               \
		X1 = *RK++ ^ RT0[(Y1)&0xFF] ^ RT1[(Y0 >> 8) & 0xFF] ^          \
		     RT2[(Y3 >> 16) & 0xFF] ^ RT3[(Y2 >> 24) & 0xFF];          \
                                                                               \
		X2 = *RK++ ^ RT0[(Y2)&0xFF] ^ RT1[(Y1 >> 8) & 0xFF] ^          \
		     RT2[(Y0 >> 16) & 0xFF] ^ RT3[(Y3 >> 24) & 0xFF];          \
                                                                               \
		X3 = *RK++ ^ RT0[(Y3)&0xFF] ^ RT1[(Y2 >> 8) & 0xFF] ^          \
		     RT2[(Y1 >> 16) & 0xFF] ^ RT3[(Y0 >> 24) & 0xFF];          \
	}

/*
 *  These macros improve the readability of the key
 *  generation initialization code by collapsing
 *  repetitive common operations into logical pieces.
 */
#define ROTL8(x) ((x << 8) & 0xFFFFFFFF) | (x >> 24)
#define XTIME(x) ((x << 1) ^ ((x & 0x80) ? 0x1B : 0x00))
#define MUL(x, y) ((x && y) ? pow[(log[x] + log[y]) % 255] : 0)
#define MIX(x, y)                                                              \
	{                                                                      \
		y = ((y << 1) | (y >> 7)) & 0xFF;                              \
		x ^= y;                                                        \
	}
#define CPY128                                                                 \
	{                                                                      \
		*RK++ = *SK++;                                                 \
		*RK++ = *SK++;                                                 \
		*RK++ = *SK++;                                                 \
		*RK++ = *SK++;                                                 \
	}

/******************************************************************************
 *
 *  AES_SET_ENCRYPTION_KEY
 *
 *  This is called by 'aes_setkey' when we're establishing a key for
 *  subsequent encryption.  We give it a pointer to the encryption
 *  context, a pointer to the key, and the key's length in bytes.
 *  Valid lengths are: 16, 24 or 32 bytes (128, 192, 256 bits).
 *
 ******************************************************************************/
int aes_set_encryption_key(aes_context *ctx, const uint8_t *key, size_t keysize)
{
	size_t i; // general purpose iteration local
	uint32_t *RK = ctx->rk; // initialize our RoundKey buffer pointer

	for (i = 0; i < (keysize >> 2); i++) {
		GET_UINT32_LE(RK[i], key, i << 2);
	}

	switch (ctx->rounds) {
	case 10:
		for (i = 0; i < 10; i++, RK += 4) {
			RK[4] = RK[0] ^ (uint32_t)RCON[i] ^
				((uint32_t)FSb[(RK[3] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[3] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[3] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[3]) & 0xFF] << 24);

			RK[5] = RK[1] ^ RK[4];
			RK[6] = RK[2] ^ RK[5];
			RK[7] = RK[3] ^ RK[6];
		}
		break;

#ifndef AES_CONFIG_128_BIT_ONLY
	case 12:
		for (i = 0; i < 8; i++, RK += 6) {
			RK[6] = RK[0] ^ (uint32_t)RCON[i] ^
				((uint32_t)FSb[(RK[5] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[5] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[5] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[5]) & 0xFF] << 24);

			RK[7] = RK[1] ^ RK[6];
			RK[8] = RK[2] ^ RK[7];
			RK[9] = RK[3] ^ RK[8];
			RK[10] = RK[4] ^ RK[9];
			RK[11] = RK[5] ^ RK[10];
		}
		break;

	case 14:
		for (i = 0; i < 7; i++, RK += 8) {
			RK[8] = RK[0] ^ (uint32_t)RCON[i] ^
				((uint32_t)FSb[(RK[7] >> 8) & 0xFF]) ^
				((uint32_t)FSb[(RK[7] >> 16) & 0xFF] << 8) ^
				((uint32_t)FSb[(RK[7] >> 24) & 0xFF] << 16) ^
				((uint32_t)FSb[(RK[7]) & 0xFF] << 24);

			RK[9] = RK[1] ^ RK[8];
			RK[10] = RK[2] ^ RK[9];
			RK[11] = RK[3] ^ RK[10];

			RK[12] = RK[4] ^ ((uint32_t)FSb[(RK[11]) & 0xFF]) ^
				 ((uint32_t)FSb[(RK[11] >> 8) & 0xFF] << 8) ^
				 ((uint32_t)FSb[(RK[11] >> 16) & 0xFF] << 16) ^
				 ((uint32_t)FSb[(RK[11] >> 24) & 0xFF] << 24);

			RK[13] = RK[5] ^ RK[12];
			RK[14] = RK[6] ^ RK[13];
			RK[15] = RK[7] ^ RK[14];
		}
		break;
#endif

	default:
		return -1;
	}
	return (0);
}

/******************************************************************************
 *
 *  AES_SETKEY
 *
 *  Invoked to establish the key schedule for subsequent encryption/decryption
 *
 ******************************************************************************/
int aes_setkey(aes_context *ctx, // AES context provided by our caller
	       const uint8_t *key, // pointer to the key
	       size_t keysize) // key length in bytes
{
	ctx->rk = ctx->buf; // initialize our round key pointer

	switch (keysize) // set the rounds count based upon the keysize
	{
	case 16:
		ctx->rounds = AES_128_ROUNDS;
		break; // 16-byte, 128-bit key
#ifndef AES_CONFIG_128_BIT_ONLY
	case 24:
		ctx->rounds = AES_192_ROUNDS;
		break; // 24-byte, 192-bit key
	case 32:
		ctx->rounds = AES_256_ROUNDS;
		break; // 32-byte, 256-bit key
#endif
	default:
		return (-1);
	}

	return (aes_set_encryption_key(ctx, key, keysize));
}

/******************************************************************************
 *
 *  AES_CIPHER
 *
 *  Perform AES encryption and decryption.
 *  The AES context will have been setup with the encryption mode
 *  and all keying information appropriate for the task.
 *
 ******************************************************************************/
int aes_cipher(aes_context *ctx, const uint8_t input[16], uint8_t output[16])
{
	uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3; // general purpose locals
	size_t rounds_loop = (ctx->rounds >> 1) - 1;
	int i;

#ifdef AES_CONFIG_128_BIT_ONLY
	rounds_loop = (AES_128_ROUNDS >> 1) - 1;
#endif

	RK = ctx->rk;

	GET_UINT32_LE(X0, input, 0);
	X0 ^= *RK++; // load our 128-bit
	GET_UINT32_LE(X1, input, 4);
	X1 ^= *RK++; // input buffer in a storage
	GET_UINT32_LE(X2, input, 8);
	X2 ^= *RK++; // memory endian-neutral way
	GET_UINT32_LE(X3, input, 12);
	X3 ^= *RK++;

	for (i = (ctx->rounds >> 1) - 1; i > 0; i--) {
		AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);
		AES_FROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3);
	}

	AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);

	X0 = *RK++ ^ ((uint32_t)FSb[(Y0)&0xFF]) ^
	     ((uint32_t)FSb[(Y1 >> 8) & 0xFF] << 8) ^
	     ((uint32_t)FSb[(Y2 >> 16) & 0xFF] << 16) ^
	     ((uint32_t)FSb[(Y3 >> 24) & 0xFF] << 24);

	X1 = *RK++ ^ ((uint32_t)FSb[(Y1)&0xFF]) ^
	     ((uint32_t)FSb[(Y2 >> 8) & 0xFF] << 8) ^
	     ((uint32_t)FSb[(Y3 >> 16) & 0xFF] << 16) ^
	     ((uint32_t)FSb[(Y0 >> 24) & 0xFF] << 24);

	X2 = *RK++ ^ ((uint32_t)FSb[(Y2)&0xFF]) ^
	     ((uint32_t)FSb[(Y3 >> 8) & 0xFF] << 8) ^
	     ((uint32_t)FSb[(Y0 >> 16) & 0xFF] << 16) ^
	     ((uint32_t)FSb[(Y1 >> 24) & 0xFF] << 24);

	X3 = *RK++ ^ ((uint32_t)FSb[(Y3)&0xFF]) ^
	     ((uint32_t)FSb[(Y0 >> 8) & 0xFF] << 8) ^
	     ((uint32_t)FSb[(Y1 >> 16) & 0xFF] << 16) ^
	     ((uint32_t)FSb[(Y2 >> 24) & 0xFF] << 24);

	PUT_UINT32_LE(X0, output, 0);
	PUT_UINT32_LE(X1, output, 4);
	PUT_UINT32_LE(X2, output, 8);
	PUT_UINT32_LE(X3, output, 12);

	return (0);
}
/* end of aes.c */
