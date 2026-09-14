/* SPDX-License-Identifier: GPL-2.0 */
/** @file mlan_init.h
 *
 *  @brief This file defines the FW initialization data
 *  structures.
 *
 *
 *  Copyright 2008-2021, 2025-2026 NXP
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

/******************************************************
 * Change log:
 * 10/13/2008: initial version
 ******************************************************
 */

#ifndef _MLAN_INIT_H_
#define _MLAN_INIT_H_

/** Tx buffer size for firmware download*/
#define FW_DNLD_TX_BUF_SIZE 2312
/** Rx buffer size for firmware download*/
#define FW_DNLD_RX_BUF_SIZE 2048
/** Max firmware retry */
#define MAX_FW_RETRY 3

/** Firmware has last block */
#define FW_HAS_LAST_BLOCK 0x00000004
/** CMD id for CMD4 */
#define FW_CMD_4 0x00000004
/** CMD id for CMD6 */
#define FW_CMD_6 0x00000006
/** CMD id for CMD7 */
#define FW_CMD_7 0x00000007
/** CMD id for CMD10 */
#define FW_CMD_10 0x0000000a
/** CMD id for CMD21 */
#define FW_CMD_21 0x00000015

/** Firmware data transmit size */
#define FW_DATA_XMIT_SIZE (sizeof(FWHeader) + DataLength + sizeof(t_u32))

/** FWHeader */
typedef MLAN_PACK_START struct _FWHeader {
	/** FW download command */
	t_u32 dnld_cmd;
	/** FW base address */
	t_u32 base_addr;
	/** FW data length */
	t_u32 data_length;
	/** FW CRC */
	t_u32 crc;
} MLAN_PACK_END FWHeader;

/** FWData */
typedef MLAN_PACK_START struct _FWData {
	/** FW data header */
	FWHeader fw_header;
	/** FW data sequence number */
	t_u32 seq_num;
	/** FW data buffer */
	t_u8 data[1];
} MLAN_PACK_END FWData;

/** FWSyncHeader */
typedef MLAN_PACK_START struct _FWSyncHeader {
	/** FW sync header command */
	t_u32 cmd;
	/** FW sync header sequence number */
	t_u32 seq_num;
	/** Extended header */
	t_u32 magic;
	/** Chip rev */
	t_u32 chip_rev;
	/** Strap */
	t_u32 strap;
	/** Status */
	t_u32 status;
	/** Offset */
	t_u32 offset;
} MLAN_PACK_END FWSyncHeader;

/** FW Sync pkt */
typedef MLAN_PACK_START struct _FWSyncPkt {
	/** pkt type */
	t_u32 pkt_type;
	/** FW sync header command */
	t_u32 cmd;
	/** FW sync header sequence number */
	t_u32 seq_num;
	/** chip rev */
	t_u32 chip_rev;
	/** fw status */
	t_u32 fw_ready;
} MLAN_PACK_END FWSyncPkt;

/** Firmware Meta data section */
#define FW_CMD_24 0x00000018

#define TLV_FLAG_WIFI 0x1

#define TLV_ID_UUID 0x40
#define TLV_ID_UUID_LEN 16

#define TLV_ID_PUBLIC_KEY 0x50
#define TLV_ID_PUBLIC_KEY_LEN 64

#define TLV_ID_PUBLICK_KEY_OFFSET 33

#define META_MAGIC_OFFSET 12
#define META_MAGIC_LEN 8
#define DATA_CRC_OFFSET 4
#define DATA_CRC_LEN 4

/*
 * Minimum payload len is sum of a must required fields length for Wi-Fi
 * ==========================================================================
 * |  ID  |    |   |PAY |   ID    |    |   |   | PAY|   |  META  | META|DATA|
 * |(UUID)|FLAG|LEN|LOAD|(PUB KEY)|FLAG|LEN|PAD|LOAD|PAD|DATA LEN|MAGIC|CRC |
 * ==========================================================================
 *   2B     2B   4B  16B   2B       2B  4B  1B  64B  3B    4B      8B    4B
 */
#define MIN_PAYLOAD_LEN 116

typedef MLAN_PACK_START struct _FWMetaData {
	t_u16 id;
	t_u16 flag;
	t_u32 len;
} MLAN_PACK_END FWMetaData;

#ifdef BIG_ENDIAN_SUPPORT
/** Convert sequence number and command fields
 *  of fwheader to correct endian format
 */
#define endian_convert_syncfwheader(x)                                         \
	{                                                                      \
		(x)->cmd = wlan_le32_to_cpu((x)->cmd);                         \
		(x)->seq_num = wlan_le32_to_cpu((x)->seq_num);                 \
		(x)->status = wlan_le32_to_cpu((x)->status);                   \
		(x)->offset = wlan_le32_to_cpu((x)->offset);                   \
	}
#else
/** Convert sequence number and command fields
 *  of fwheader to correct endian format
 */
#define endian_convert_syncfwheader(x)
#endif /* BIG_ENDIAN_SUPPORT */

#endif /* _MLAN_INIT_H_ */
