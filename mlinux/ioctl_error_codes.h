/** @file ioctl_error_codes.h
 *
 *  @brief This file contains IOCTL error code ID definitions.
 *
 *
 *  Copyright 2024-2025 NXP
 *
 *  This software file (the File) is distributed by NXP
 *  under the terms of the GNU General Public License Version 2, June 1991
 *  (the License).  You may use, redistribute and/or modify the File in
 *  accordance with the terms and conditions of the License, a copy of which
 *  is available by writing to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 *  worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 *  THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 *  ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 *  this warranty disclaimer.
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
