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

#ifndef NANOTLS_TYPES_H
#define NANOTLS_TYPES_H

#include <nanotls-config.h>

enum nanotls_role {
	NANOTLS_ROLE_UNKNOWN = 0,
#ifdef NANOTLS_CONFIG_HARDENING
	NANOTLS_ROLE_HOST = 0x1ea71e,
	NANOTLS_ROLE_DEVICE = 0xdb25cc,
#else
	NANOTLS_ROLE_HOST = 1,
	NANOTLS_ROLE_DEVICE = 2,
#endif
};

enum ecode {
#ifdef NANOTLS_CONFIG_HARDENING
	/* Error codes with big Hamming distance for better hardening */
	/* Not trivial enough, but still could be generated with single RISC-V
	   instruction */
	E_OK = -1446, /* 0xFFFFFA5A */
	E_INVALID = 0x153E95A5, /* Invalid value or state */
#else
	E_OK = 0,
	E_INVALID = -1,
#endif
};

#endif /* NANOTLS_TYPES_H */
