/* SPDX-License-Identifier: GPL-2.0 */
/** @file ioctl_error_codes.h
 *
 *  @brief This file contains IOCTL error code ID definitions.
 *
 *
 *  Copyright 2024-2026 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code (Materials) are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide
 *  copyright and trade secret laws and treaty provisions. No part of the
 *  Materials may be used, copied, reproduced, modified, published, uploaded,
 *  posted, transmitted, distributed, or disclosed in any way without NXP's
 *  prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 *  Alternatively, this software may be distributed under the terms of GPL v2.
 *
 *
 */

/** No error */
ENUM_ELEMENT(NO_ERROR, 0),

	/** Firmware/device errors below (MSB=0) */
	ENUM_ELEMENT(FW_NOT_READY, 0x00000001),
	ENUM_ELEMENT(FW_BUSY, 0x00000002), ENUM_ELEMENT(FW_CMDRESP, 0x00000003),
	ENUM_ELEMENT(DATA_TX_FAIL, 0x00000004),
	ENUM_ELEMENT(DATA_RX_FAIL, 0x00000005),

	/** Driver errors below (MSB=1) */
	ENUM_ELEMENT(PKT_SIZE_INVALID, 0x80000001),
	ENUM_ELEMENT(PKT_TIMEOUT, 0x80000002),
	ENUM_ELEMENT(PKT_INVALID, 0x80000003),
	ENUM_ELEMENT(CMD_INVALID, 0x80000004),
	ENUM_ELEMENT(CMD_TIMEOUT, 0x80000005),
	ENUM_ELEMENT(CMD_DNLD_FAIL, 0x80000006),
	ENUM_ELEMENT(CMD_CANCEL, 0x80000007),
	ENUM_ELEMENT(CMD_RESP_FAIL, 0x80000008),
	ENUM_ELEMENT(CMD_ASSOC_FAIL, 0x80000009),
	ENUM_ELEMENT(CMD_SCAN_FAIL, 0x8000000A),
	ENUM_ELEMENT(IOCTL_INVALID, 0x8000000B),
	ENUM_ELEMENT(IOCTL_FAIL, 0x8000000C),
	ENUM_ELEMENT(EVENT_UNKNOWN, 0x8000000D),
	ENUM_ELEMENT(INVALID_PARAMETER, 0x8000000E),
	ENUM_ELEMENT(NO_MEM, 0x8000000F),

	/* Always keep this last */
	ENUM_ELEMENT_LAST(__IOCTL_ERROR_LAST)
