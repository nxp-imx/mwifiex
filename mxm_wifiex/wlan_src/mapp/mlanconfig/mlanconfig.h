/** @file  mlanconfig.h
  *
  * @brief This file contains definitions for application
  *
  *
  * Copyright 2014-2020 NXP
  *
  * This software file (the File) is distributed by NXP
  * under the terms of the GNU General Public License Version 2, June 1991
  * (the License).  You may use, redistribute and/or modify the File in
  * accordance with the terms and conditions of the License, a copy of which
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
  * this warranty disclaimer.
  *
  */
/************************************************************************
Change log:
     11/26/2008: initial version
************************************************************************/
#ifndef _MLANCONFIG_H_
#define _MLANCONFIG_H_

/** Include header files */
#include    <stdio.h>
#include    <ctype.h>
#include    <unistd.h>
#include    <string.h>
#include    <stdlib.h>
#include    <sys/socket.h>
#include    <sys/ioctl.h>
#include    <errno.h>
#include    <linux/if.h>
#include    <linux/wireless.h>
#include    <sys/types.h>
#include    <linux/if_ether.h>
#include    <time.h>

#if (BYTE_ORDER == LITTLE_ENDIAN)
#undef BIG_ENDIAN_SUPPORT
#endif

/** Type definition: boolean */
typedef enum { FALSE, TRUE } boolean;

/**
 * This macro specifies the attribute pack used for structure packing
 */
#ifndef __ATTRIB_PACK__
#define __ATTRIB_PACK__  __attribute__((packed))
#endif

/** 16 bits byte swap */
#define swap_byte_16(x) \
((t_u16)((((t_u16)(x) & 0x00ffU) << 8) | \
         (((t_u16)(x) & 0xff00U) >> 8)))

/** 32 bits byte swap */
#define swap_byte_32(x) \
((t_u32)((((t_u32)(x) & 0x000000ffUL) << 24) | \
         (((t_u32)(x) & 0x0000ff00UL) <<  8) | \
         (((t_u32)(x) & 0x00ff0000UL) >>  8) | \
         (((t_u32)(x) & 0xff000000UL) >> 24)))

/** Convert to correct endian format */
#ifdef 	BIG_ENDIAN_SUPPORT
/** CPU to little-endian convert for 16-bit */
#define 	cpu_to_le16(x)	swap_byte_16(x)
/** CPU to little-endian convert for 32-bit */
#define		cpu_to_le32(x)  swap_byte_32(x)
/** Little-endian to CPU convert for 16-bit */
#define 	le16_to_cpu(x)	swap_byte_16(x)
/** Little-endian to CPU convert for 32-bit */
#define		le32_to_cpu(x)  swap_byte_32(x)
#else
/** Do nothing */
#define		cpu_to_le16(x)	(x)
/** Do nothing */
#define		cpu_to_le32(x)  (x)
/** Do nothing */
#define 	le16_to_cpu(x)	(x)
/** Do nothing */
#define 	le32_to_cpu(x)	(x)
#endif

/** Character, 1 byte */
typedef char t_s8;
/** Unsigned character, 1 byte */
typedef unsigned char t_u8;

/** Short integer */
typedef signed short t_s16;
/** Unsigned short integer */
typedef unsigned short t_u16;

/** Integer */
typedef signed int t_s32;
/** Unsigned integer */
typedef unsigned int t_u32;

/** Long long integer */
typedef signed long long t_s64;
/** Unsigned long long integer */
typedef unsigned long long t_u64;

/** Void pointer (4-bytes) */
typedef void t_void;

/** Success */
#define MLAN_STATUS_SUCCESS         (0)
/** Failure */
#define MLAN_STATUS_FAILURE         (-1)

t_s8 *mlan_config_get_line(FILE * fp, t_s8 *s, t_s32 size, int *line);
int get_priv_ioctl(char *ioctl_name, int *ioctl_val, int *subioctl_val);
int fparse_for_hex(FILE * fp, t_u8 *dst);

/**
 * Hex or Decimal to Integer
 * @param   num string to convert into decimal or hex
 */
#define A2HEXDECIMAL(num)  \
    (strncasecmp("0x", (num), 2)?(unsigned int) strtoll((num),NULL,0):a2hex((num)))

/** Convert character to integer */
#define CHAR2INT(x) (((x) >= 'A') ? ((x) - 'A' + 10) : ((x) - '0'))

/** Convert TLV header from little endian format to CPU format */
#define endian_convert_tlv_header_in(x)            \
    {                                               \
        (x)->tag = le16_to_cpu((x)->tag);       \
        (x)->length = le16_to_cpu((x)->length); \
    }

/** Convert TLV header to little endian format from CPU format */
#define endian_convert_tlv_header_out(x)            \
    {                                               \
        (x)->tag = cpu_to_le16((x)->tag);       \
        (x)->length = cpu_to_le16((x)->length); \
    }
/** Private command ID to pass custom IE list */
#define CUSTOM_IE_CFG          (SIOCDEVPRIVATE + 13)
/* TLV Definitions */
/** TLV header */
#define TLVHEADER       /** Tag */      \
                        t_u16 tag;      \
                        /** Length */   \
                        t_u16 length

/** Maximum IE buffer length */
#define MAX_IE_BUFFER_LEN 256

/** TLV: Management IE list */
#define MRVL_MGMT_IE_LIST_TLV_ID          (PROPRIETARY_TLV_BASE_ID + 0x69)	//0x0169

/** TLV: Max Management IE */
#define MRVL_MAX_MGMT_IE_TLV_ID           (PROPRIETARY_TLV_BASE_ID + 0xaa)	//0x01aa

/** custom IE info */
typedef struct _custom_ie_info {
    /** size of buffer */
	t_u16 buf_size;
    /** no of buffers of buf_size */
	t_u16 buf_count;
} __ATTRIB_PACK__ custom_ie_info;

/** TLV buffer : custom IE */
typedef struct _tlvbuf_max_mgmt_ie {
    /** Header */
	TLVHEADER;
    /** No of tuples */
	t_u16 count;
    /** custom IE info tuples */
	custom_ie_info info[];
} __ATTRIB_PACK__ tlvbuf_max_mgmt_ie;

/** custom IE */
typedef struct _custom_ie {
    /** IE Index */
	t_u16 ie_index;
    /** Mgmt Subtype Mask */
	t_u16 mgmt_subtype_mask;
    /** IE Length */
	t_u16 ie_length;
    /** IE buffer */
	t_u8 ie_buffer[];
} __ATTRIB_PACK__ custom_ie;

/** TLV buffer : custom IE */
typedef struct _tlvbuf_custom_ie {
    /** Header */
	TLVHEADER;
    /** custom IE data */
	custom_ie ie_data[];
} __ATTRIB_PACK__ tlvbuf_custom_ie;

/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE                 1024
/** Ethernet address length */
#define ETH_ALEN                        6
/** MAC BROADCAST */
#define MAC_BROADCAST   0x1FF
/** MAC MULTICAST */
#define MAC_MULTICAST   0x1FE

/** pkt_header */
typedef struct _pkt_header {
    /** pkt_len */
	t_u32 pkt_len;
    /** pkt_type */
	t_u32 TxPktType;
    /** tx control */
	t_u32 TxControl;
} pkt_header;

/** wlan_802_11_header packet from FW with length */
typedef struct _wlan_mgmt_frame_tx {
    /** Packet Length */
	t_u16 frm_len;
    /** Frame Control */
	t_u16 frm_ctl;
    /** Duration ID */
	t_u16 duration_id;
    /** Address1 */
	t_u8 addr1[ETH_ALEN];
    /** Address2 */
	t_u8 addr2[ETH_ALEN];
    /** Address3 */
	t_u8 addr3[ETH_ALEN];
    /** Sequence Control */
	t_u16 seq_ctl;
    /** Address4 */
	t_u8 addr4[ETH_ALEN];
    /** Frame payload */
	t_u8 payload[];
} __ATTRIB_PACK__ wlan_mgmt_frame_tx;

/** frame tx ioctl number */
#define FRAME_TX_IOCTL               (SIOCDEVPRIVATE + 12)

extern t_s32 sockfd;  /**< socket */
extern t_s8 dev_name[IFNAMSIZ + 1];   /**< device name */

#endif /* _MLANCONFIG_H_ */
