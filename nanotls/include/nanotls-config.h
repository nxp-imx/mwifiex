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

#ifndef NANOTLS_CONFIG_H
#define NANOTLS_CONFIG_H

/* nanoTLS compile-time configuration options */

/*
 * Harden implementation against glitching, SCA, etc.
 * Will increase the code size.
 */
//#define NANOTLS_CONFIG_HARDENING

/*
 * Enable support for key update
 */
#define NANOTLS_CONFIG_KEY_UPDATE

/*
 * Maximum number of failures during handshake before transitioning
 * into error state.
 * If not defined, then there is no limit (unsafe).
 */
#ifndef NANOTLS_CONFIG_MAX_ERR_CNT
#define NANOTLS_CONFIG_MAX_ERR_CNT 4
#endif

/*
 * Maximum number of consecutive data AES-GCM tag failures before going into
 * error state and erasing the keys. If not defined, then there is no limit
 * (unsafe).
 */
#ifndef NANOTLS_CONFIG_MAX_DATA_TAG_MISMATCH_CNT
#define NANOTLS_CONFIG_MAX_DATA_TAG_MISMATCH_CNT 256
#endif

/*
 * Enable encryption during handshake.
 * ECDSA signature and Finished messages are encrypted.
 * AES-GCM is used and authentication tag is generated and verified.
 * Introduces dependency on AES API during handshake.
 *
 * NB: TLS 1.2 does not encrypt these messages, encryption introduced in TLS 1.3
 */
#define NANOTLS_CONFIG_ENCRYPTED_HANDSHAKE

#endif /* NANOTLS_CONFIG_H */
