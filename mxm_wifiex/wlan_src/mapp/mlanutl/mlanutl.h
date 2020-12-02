/** @file  mlanutl.h
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
#ifndef _MLANUTL_H_
#define _MLANUTL_H_

/** Include header files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <netinet/ether.h>
#include <linux/if_packet.h>

/** Type definition: boolean */
typedef enum { FALSE, TRUE } boolean;

/** 16 bits byte swap */
#define swap_byte_16(x)                                                        \
	((t_u16)((((t_u16)(x)&0x00ffU) << 8) | (((t_u16)(x)&0xff00U) >> 8)))

/** 32 bits byte swap */
#define swap_byte_32(x)                                                        \
	((t_u32)((((t_u32)(x)&0x000000ffUL) << 24) |                           \
		 (((t_u32)(x)&0x0000ff00UL) << 8) |                            \
		 (((t_u32)(x)&0x00ff0000UL) >> 8) |                            \
		 (((t_u32)(x)&0xff000000UL) >> 24)))

/** Convert to correct endian format */
#ifdef BIG_ENDIAN_SUPPORT
/** CPU to little-endian convert for 16-bit */
#define cpu_to_le16(x) swap_byte_16(x)
/** CPU to little-endian convert for 32-bit */
#define cpu_to_le32(x) swap_byte_32(x)
/** Little-endian to CPU convert for 16-bit */
#define le16_to_cpu(x) swap_byte_16(x)
/** Little-endian to CPU convert for 32-bit */
#define le32_to_cpu(x) swap_byte_32(x)
#else
/** Do nothing */
#define cpu_to_le16(x) (x)
/** Do nothing */
#define cpu_to_le32(x) (x)
/** Do nothing */
#define le16_to_cpu(x) (x)
/** Do nothing */
#define le32_to_cpu(x) (x)
#endif

/** TLV header */
#define TLVHEADER /** Tag */                                                   \
	t_u16 tag;                                                             \
	/** Length */                                                          \
	t_u16 length

/** Length of TLV header */
#define TLVHEADER_LEN 4

/** Character, 1 byte */
typedef signed char t_s8;
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

enum _mlan_act_ioctl {
	MLAN_ACT_SET = 1,
	MLAN_ACT_GET,
	MLAN_ACT_CANCEL,
	MLAN_ACT_CLEAR,
	MLAN_ACT_RESET,
	MLAN_ACT_DEFAULT
};

/** The attribute pack used for structure packing */
#ifndef __ATTRIB_PACK__
#define __ATTRIB_PACK__ __attribute__((packed))
#endif

/** Success */
#define MLAN_STATUS_SUCCESS (0)
/** Failure */
#define MLAN_STATUS_FAILURE (-1)
/** Not found */
#define MLAN_STATUS_NOTFOUND (1)

/** IOCTL number */
#define MLAN_ETH_PRIV (SIOCDEVPRIVATE + 14)

/** Command buffer max length */
#define BUFFER_LENGTH (3 * 1024)

/** Find number of elements */
#define NELEMENTS(x) (sizeof(x) / sizeof(x[0]))

/** BIT value */
#define MBIT(x) (((t_u32)1) << (x))

#ifndef MIN
/** Find minimum value */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif /* MIN */

/** Length of ethernet address */
#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/** Action field value : get */
#define ACTION_GET 0
/** Action field value : set */
#define ACTION_SET 1

/** Maximum number of TID */
#define MAX_NUM_TID 8

/** Device name */
extern char dev_name[IFNAMSIZ + 1];

#define HOSTCMD "hostcmd"

/** NXP private command identifier */
#define CMD_NXP "MRVL_CMD"

struct command_node {
	char *name;
	int (*handler)(int, char **);
};

/** Private command structure */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
struct eth_priv_cmd {
	/** Command buffer pointer */
	t_u64 buf;
	/** buffer updated by driver */
	int used_len;
	/** buffer sent by application */
	int total_len;
} __ATTRIB_PACK__;
#else
struct eth_priv_cmd {
	/** Command buffer */
	t_u8 *buf;
	/** Used length */
	int used_len;
	/** Total length */
	int total_len;
};
#endif

/** data structure for cmd getdatarate */
struct eth_priv_data_rate {
	/** Tx data rate */
	t_u32 tx_data_rate;
	/** Rx data rate */
	t_u32 rx_data_rate;

	/** Tx channel bandwidth */
	t_u32 tx_bw;
	/** Tx guard interval */
	t_u32 tx_gi;
	/** Rx channel bandwidth */
	t_u32 rx_bw;
	/** Rx guard interval */
	t_u32 rx_gi;
	/** MCS index */
	t_u32 tx_mcs_index;
	t_u32 rx_mcs_index;
	/** NSS */
	t_u32 tx_nss;
	t_u32 rx_nss;
	/* LG rate: 0, HT rate: 1, VHT rate: 2 */
	t_u32 tx_rate_format;
	t_u32 rx_rate_format;
};

/** data structure for cmd getlog */
struct eth_priv_get_log {
	/** Multicast transmitted frame count */
	t_u32 mcast_tx_frame;
	/** Failure count */
	t_u32 failed;
	/** Retry count */
	t_u32 retry;
	/** Multi entry count */
	t_u32 multi_retry;
	/** Duplicate frame count */
	t_u32 frame_dup;
	/** RTS success count */
	t_u32 rts_success;
	/** RTS failure count */
	t_u32 rts_failure;
	/** Ack failure count */
	t_u32 ack_failure;
	/** Rx fragmentation count */
	t_u32 rx_frag;
	/** Multicast Tx frame count */
	t_u32 mcast_rx_frame;
	/** FCS error count */
	t_u32 fcs_error;
	/** Tx frame count */
	t_u32 tx_frame;
	/** WEP ICV error count */
	t_u32 wep_icv_error[4];
	/** beacon recv count */
	t_u32 bcn_rcv_cnt;
	/** beacon miss count */
	t_u32 bcn_miss_cnt;
	/** received amsdu count*/
	t_u32 amsdu_rx_cnt;
	/** received msdu count in amsdu*/
	t_u32 msdu_in_rx_amsdu_cnt;
	/** tx amsdu count*/
	t_u32 amsdu_tx_cnt;
	/** tx msdu count in amsdu*/
	t_u32 msdu_in_tx_amsdu_cnt;
	/** Tx frag count */
	t_u32 tx_frag_cnt;
	/** Qos Tx frag count */
	t_u32 qos_tx_frag_cnt[8];
	/** Qos failed count */
	t_u32 qos_failed_cnt[8];
	/** Qos retry count */
	t_u32 qos_retry_cnt[8];
	/** Qos multi retry count */
	t_u32 qos_multi_retry_cnt[8];
	/** Qos frame dup count */
	t_u32 qos_frm_dup_cnt[8];
	/** Qos rts success count */
	t_u32 qos_rts_suc_cnt[8];
	/** Qos rts failure count */
	t_u32 qos_rts_failure_cnt[8];
	/** Qos ack failure count */
	t_u32 qos_ack_failure_cnt[8];
	/** Qos Rx frag count */
	t_u32 qos_rx_frag_cnt[8];
	/** Qos Tx frame count */
	t_u32 qos_tx_frm_cnt[8];
	/** Qos discarded frame count */
	t_u32 qos_discarded_frm_cnt[8];
	/** Qos mpdus Rx count */
	t_u32 qos_mpdus_rx_cnt[8];
	/** Qos retry rx count */
	t_u32 qos_retries_rx_cnt[8];
	/** CMACICV errors count */
	t_u32 cmacicv_errors;
	/** CMAC replays count */
	t_u32 cmac_replays;
	/** mgmt CCMP replays count */
	t_u32 mgmt_ccmp_replays;
	/** TKIP ICV errors count */
	t_u32 tkipicv_errors;
	/** TKIP replays count */
	t_u32 tkip_replays;
	/** CCMP decrypt errors count */
	t_u32 ccmp_decrypt_errors;
	/** CCMP replays count */
	t_u32 ccmp_replays;
	/** Tx amsdu count */
	t_u32 tx_amsdu_cnt;
	/** failed amsdu count */
	t_u32 failed_amsdu_cnt;
	/** retry amsdu count */
	t_u32 retry_amsdu_cnt;
	/** multi-retry amsdu count */
	t_u32 multi_retry_amsdu_cnt;
	/** Tx octets in amsdu count */
	t_u64 tx_octets_in_amsdu_cnt;
	/** amsdu ack failure count */
	t_u32 amsdu_ack_failure_cnt;
	/** Rx amsdu count */
	t_u32 rx_amsdu_cnt;
	/** Rx octets in amsdu count */
	t_u64 rx_octets_in_amsdu_cnt;
	/** Tx ampdu count */
	t_u32 tx_ampdu_cnt;
	/** tx mpdus in ampdu count */
	t_u32 tx_mpdus_in_ampdu_cnt;
	/** tx octets in ampdu count */
	t_u64 tx_octets_in_ampdu_cnt;
	/** ampdu Rx count */
	t_u32 ampdu_rx_cnt;
	/** mpdu in Rx ampdu count */
	t_u32 mpdu_in_rx_ampdu_cnt;
	/** Rx octets ampdu count */
	t_u64 rx_octets_in_ampdu_cnt;
	/** ampdu delimiter CRC error count */
	t_u32 ampdu_delimiter_crc_error_cnt;
	/** Rx Stuck Related Info*/
	/** Rx Stuck Issue count */
	t_u32 rx_stuck_issue_cnt[2];
	/** Rx Stuck Recovery count */
	t_u32 rx_stuck_recovery_cnt;
	/** Rx Stuck TSF */
	t_u64 rx_stuck_tsf[2];
	/** Tx Watchdog Recovery Related Info */
	/** Tx Watchdog Recovery count */
	t_u32 tx_watchdog_recovery_cnt;
	/** Tx Watchdog TSF */
	t_u64 tx_watchdog_tsf[2];
	/** Channel Switch Related Info */
	/** Channel Switch Announcement Sent */
	t_u32 channel_switch_ann_sent;
	/** Channel Switch State */
	t_u32 channel_switch_state;
	/** Register Class */
	t_u32 reg_class;
	/** Channel Number */
	t_u32 channel_number;
	/** Channel Switch Mode */
	t_u32 channel_switch_mode;
};

/** MLAN MAC Address Length */
#define MLAN_MAC_ADDR_LENGTH 6
#define COUNTRY_CODE_LEN 3
/** Type definition of eth_priv_countrycode for CMD_COUNTRYCODE */
struct eth_priv_countrycode {
	/** Country Code */
	t_u8 country_code[COUNTRY_CODE_LEN];
};

/** Type enumeration of WMM AC_QUEUES */
typedef enum _mlan_wmm_ac_e {
	WMM_AC_BK,
	WMM_AC_BE,
	WMM_AC_VI,
	WMM_AC_VO
} __ATTRIB_PACK__ mlan_wmm_ac_e;

/** IEEE Type definitions  */
typedef enum _IEEEtypes_ElementId_e {
	SSID = 0,
	SUPPORTED_RATES = 1,
	FH_PARAM_SET = 2,
	DS_PARAM_SET = 3,
	CF_PARAM_SET = 4,

	IBSS_PARAM_SET = 6,

	COUNTRY_INFO = 7,

	POWER_CONSTRAINT = 32,
	POWER_CAPABILITY = 33,
	TPC_REQUEST = 34,
	TPC_REPORT = 35,
	SUPPORTED_CHANNELS = 36,
	CHANNEL_SWITCH_ANN = 37,
	QUIET = 40,
	IBSS_DFS = 41,
	HT_CAPABILITY = 45,
	HT_OPERATION = 61,
	BSSCO_2040 = 72,
	OVERLAPBSSSCANPARAM = 74,
	EXT_CAPABILITY = 127,

	VHT_CAPABILITY = 191,
	VHT_OPERATION = 192,
	EXT_BSS_LOAD = 193,
	BW_CHANNEL_SWITCH = 194,
	VHT_TX_POWER_ENV = 195,
	EXT_POWER_CONSTR = 196,
	AID_INFO = 197,
	QUIET_CHAN = 198,
	OPER_MODE_NTF = 199,

	ERP_INFO = 42,
	EXTENDED_SUPPORTED_RATES = 50,

	VENDOR_SPECIFIC_221 = 221,
	WMM_IE = VENDOR_SPECIFIC_221,

	WPS_IE = VENDOR_SPECIFIC_221,

	WPA_IE = VENDOR_SPECIFIC_221,
	RSN_IE = 48,
	EXTENSION = 255,
} __ATTRIB_PACK__ IEEEtypes_ElementId_e;

typedef enum _IEEEtypes_Ext_ElementId_e {
	HE_CAPABILITY = 35,
	HE_OPERATION = 36
} IEEEtypes_Ext_ElementId_e;

/** Capability Bit Map*/
#ifdef BIG_ENDIAN_SUPPORT
typedef struct _IEEEtypes_CapInfo_t {
	t_u8 rsrvd1 : 2;
	t_u8 dsss_ofdm : 1;
	t_u8 rsvrd2 : 2;
	t_u8 short_slot_time : 1;
	t_u8 rsrvd3 : 1;
	t_u8 spectrum_mgmt : 1;
	t_u8 chan_agility : 1;
	t_u8 pbcc : 1;
	t_u8 short_preamble : 1;
	t_u8 privacy : 1;
	t_u8 cf_poll_rqst : 1;
	t_u8 cf_pollable : 1;
	t_u8 ibss : 1;
	t_u8 ess : 1;
} __ATTRIB_PACK__ IEEEtypes_CapInfo_t, *pIEEEtypes_CapInfo_t;
#else
typedef struct _IEEEtypes_CapInfo_t {
	/** Capability Bit Map : ESS */
	t_u8 ess : 1;
	/** Capability Bit Map : IBSS */
	t_u8 ibss : 1;
	/** Capability Bit Map : CF pollable */
	t_u8 cf_pollable : 1;
	/** Capability Bit Map : CF poll request */
	t_u8 cf_poll_rqst : 1;
	/** Capability Bit Map : privacy */
	t_u8 privacy : 1;
	/** Capability Bit Map : Short preamble */
	t_u8 short_preamble : 1;
	/** Capability Bit Map : PBCC */
	t_u8 pbcc : 1;
	/** Capability Bit Map : Channel agility */
	t_u8 chan_agility : 1;
	/** Capability Bit Map : Spectrum management */
	t_u8 spectrum_mgmt : 1;
	/** Capability Bit Map : Reserved */
	t_u8 rsrvd3 : 1;
	/** Capability Bit Map : Short slot time */
	t_u8 short_slot_time : 1;
	/** Capability Bit Map : APSD */
	t_u8 apsd : 1;
	/** Capability Bit Map : Reserved */
	t_u8 rsvrd2 : 1;
	/** Capability Bit Map : DSS OFDM */
	t_u8 dsss_ofdm : 1;
	/** Capability Bit Map : Reserved */
	t_u8 rsrvd1 : 2;
} __ATTRIB_PACK__ IEEEtypes_CapInfo_t, *pIEEEtypes_CapInfo_t;
#endif /* BIG_ENDIAN_SUPPORT */

/** IEEE IE header */
typedef struct _IEEEtypes_Header_t {
	/** Element ID */
	t_u8 element_id;
	/** Length */
	t_u8 len;
} __ATTRIB_PACK__ IEEEtypes_Header_t, *pIEEEtypes_Header_t;

/** IEEE IE header */
#define IEEE_HEADER_LEN sizeof(IEEEtypes_Header_t)

/** Maximum size of IEEE Information Elements */
#define IEEE_MAX_IE_SIZE 256

/** Vendor specific IE header */
typedef struct _IEEEtypes_VendorHeader_t {
	/** Element ID */
	t_u8 element_id;
	/** Length */
	t_u8 len;
	/** OUI */
	t_u8 oui[3];
	/** OUI type */
	t_u8 oui_type;
	/** OUI subtype */
	t_u8 oui_subtype;
	/** Version */
	t_u8 version;
} __ATTRIB_PACK__ IEEEtypes_VendorHeader_t, *pIEEEtypes_VendorHeader_t;

/** Vendor specific IE */
typedef struct _IEEEtypes_VendorSpecific_t {
	/** Vendor specific IE header */
	IEEEtypes_VendorHeader_t vend_hdr;
	/** IE Max - size of previous fields */
	t_u8 data[IEEE_MAX_IE_SIZE - sizeof(IEEEtypes_VendorHeader_t)];
} __ATTRIB_PACK__ IEEEtypes_VendorSpecific_t, *pIEEEtypes_VendorSpecific_t;

/** IEEE IE */
typedef struct _IEEEtypes_Generic_t {
	/** Generic IE header */
	IEEEtypes_Header_t ieee_hdr;
	/** IE Max - size of previous fields */
	t_u8 data[IEEE_MAX_IE_SIZE - sizeof(IEEEtypes_Header_t)];
} __ATTRIB_PACK__ IEEEtypes_Generic_t, *pIEEEtypes_Generic_t;

/** Convert character to integer */
#define CHAR2INT(x) (((x) >= 'A') ? ((x) - 'A' + 10) : ((x) - '0'))

/** Command RET code, MSB is set to 1 */
#define HostCmd_RET_BIT 0x8000
/** General purpose action : Get */
#define HostCmd_ACT_GEN_GET 0x0000
/** General purpose action : Set */
#define HostCmd_ACT_GEN_SET 0x0001
/** General purpose action : Clear */
#define HostCmd_ACT_GEN_CLEAR 0x0004
/** General purpose action : Remove */
#define HostCmd_ACT_GEN_REMOVE 0x0004

/** TLV  type ID definition */
#define PROPRIETARY_TLV_BASE_ID 0x0100

/** MrvlIEtypesHeader_t */
typedef struct MrvlIEtypesHeader {
	/** Header type */
	t_u16 type;
	/** Header length */
	t_u16 len;
} __ATTRIB_PACK__ MrvlIEtypesHeader_t;

/** MrvlIEtypes_Data_t */
typedef struct MrvlIEtypes_Data_t {
	/** Header */
	MrvlIEtypesHeader_t header;
	/** Data */
	t_u8 data[1];
} __ATTRIB_PACK__ MrvlIEtypes_Data_t;

/** channel band */
enum { BAND_2GHZ = 0,
       BAND_5GHZ = 1,
       BAND_4GHZ = 2,
};

/** channel offset */
enum { SEC_CHAN_NONE = 0,
       SEC_CHAN_ABOVE = 1,
       SEC_CHAN_5MHZ = 2,
       SEC_CHAN_BELOW = 3 };

/** channel bandwidth */
enum { CHAN_BW_20MHZ = 0,
       CHAN_BW_10MHZ,
       CHAN_BW_40MHZ,
       CHAN_BW_80MHZ,
};

/** Band_Config_t */
typedef struct _Band_Config_t {
#ifdef BIG_ENDIAN_SUPPORT
	/** Channel Selection Mode - (00)=manual, (01)=ACS,  (02)=user*/
	t_u8 scanMode : 2;
	/** Secondary Channel Offset - (00)=None, (01)=Above, (11)=Below */
	t_u8 chan2Offset : 2;
	/** Channel Width - (00)=20MHz, (10)=40MHz, (11)=80MHz */
	t_u8 chanWidth : 2;
	/** Band Info - (00)=2.4GHz, (01)=5GHz */
	t_u8 chanBand : 2;
#else
	/** Band Info - (00)=2.4GHz, (01)=5GHz */
	t_u8 chanBand : 2;
	/** Channel Width - (00)=20MHz, (10)=40MHz, (11)=80MHz */
	t_u8 chanWidth : 2;
	/** Secondary Channel Offset - (00)=None, (01)=Above, (11)=Below */
	t_u8 chan2Offset : 2;
	/** Channel Selection Mode - (00)=manual, (01)=ACS, (02)=Adoption mode*/
	t_u8 scanMode : 2;
#endif
} __ATTRIB_PACK__ Band_Config_t;

/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE 1024
/** MAC BROADCAST */
#define MAC_BROADCAST 0x1FF
/** MAC MULTICAST */
#define MAC_MULTICAST 0x1FE

/** HostCmd_DS_GEN */
typedef struct MAPP_HostCmd_DS_GEN {
	/** Command */
	t_u16 command;
	/** Size */
	t_u16 size;
	/** Sequence number */
	t_u16 seq_num;
	/** Result */
	t_u16 result;
} __ATTRIB_PACK__ HostCmd_DS_GEN;

/** Size of HostCmd_DS_GEN */
#define S_DS_GEN sizeof(HostCmd_DS_GEN)

/** max mod group */
#define MAX_MOD_GROUP 35

/** modulation setting */
typedef struct _mod_group_setting {
	/** modulation group */
	t_u8 mod_group;
	/** power */
	t_u8 power;
} __ATTRIB_PACK__ mod_group_setting;

/** chan trpc config */
typedef struct _ChanTRPCConfig_t {
	/** start freq */
	t_u16 start_freq;
	/* channel width */
	t_u8 width;
	/** channel number */
	t_u8 chan_num;
	mod_group_setting mod_group[MAX_MOD_GROUP];
} __ATTRIB_PACK__ ChanTRPCConfig_t;

/** MrvlIETypes_ChanTRPCConfig_t */
typedef struct _MrvlIETypes_ChanTRPCConfig_t {
	/** Header */
	MrvlIEtypesHeader_t header;
	/** start freq */
	t_u16 start_freq;
	/* channel width */
	t_u8 width;
	/** channel number */
	t_u8 chan_num;
	/** mode groups */
	mod_group_setting mod_group[];
} __ATTRIB_PACK__ MrvlIETypes_ChanTRPCConfig_t;

/*This command gets/sets the Transmit Rate-based Power Control (TRPC) channel
 * configuration.*/
#define HostCmd_CHANNEL_TRPC_CONFIG 0x00fb

/** TLV OF CHAN_TRPC_CONFIG */
#define TLV_TYPE_CHAN_TRPC_CONFIG (PROPRIETARY_TLV_BASE_ID + 137)

/** mlan_ds_misc_chan_trpc_cfg */
typedef struct _mlan_ds_misc_chan_trpc_cfg {
	/** sub_band */
	t_u16 sub_band;
	/** length */
	t_u16 length;
	/** trpc buf */
	t_u8 trpc_buf[BUFFER_LENGTH];
} __ATTRIB_PACK__ mlan_ds_misc_chan_trpc_cfg;

struct eth_priv_addba {
	t_u32 time_out;
	t_u32 tx_win_size;
	t_u32 rx_win_size;
	t_u32 tx_amsdu;
	t_u32 rx_amsdu;
};

struct eth_priv_htcapinfo {
	t_u32 ht_cap_info_bg;
	t_u32 ht_cap_info_a;
};

/** data_structure for cmd vhtcfg */
struct eth_priv_vhtcfg {
	/** Band (1: 2.4G, 2: 5 G, 3: both 2.4G and 5G) */
	t_u32 band;
	/** TxRx (1: Tx, 2: Rx, 3: both Tx and Rx) */
	t_u32 txrx;
	/** BW CFG (0: 11N CFG, 1: vhtcap) */
	t_u32 bwcfg;
	/** VHT capabilities. */
	t_u32 vht_cap_info;
	/** VHT Tx mcs */
	t_u32 vht_tx_mcs;
	/** VHT Rx mcs */
	t_u32 vht_rx_mcs;
	/** VHT rx max rate */
	t_u16 vht_rx_max_rate;
	/** VHT max tx rate */
	t_u16 vht_tx_max_rate;
};

#endif /* _MLANUTL_H_ */
