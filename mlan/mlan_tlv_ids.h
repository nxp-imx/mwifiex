/** @file mlan_tlv_ids.h
 *
 *  @brief This file contains TLV ID definitions.
 *
 *
 *  Copyright 2023-2025 NXP
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

#ifndef _MLAN_TLV_IDS_H_
#define _MLAN_TLV_IDS_H_

/* include guard is absent intentionally */

/** Terminating TLV type */
#define NXP_TERMINATE_TLV_ID 0xffff

/** TLV ID Constants */

/** IEEE TLV IDs*/
/** TLV type : SSID */
#define TLV_TYPE_SSID 0x0000
/** TLV type : Rates */
#define TLV_TYPE_RATES 0x0001
/** TLV type : PHY FH */
#define TLV_TYPE_PHY_FH 0x0002
/** TLV type : PHY DS */
#define TLV_TYPE_PHY_DS 0x0003
/** TLV type : CF */
#define TLV_TYPE_CF 0x0004
/** TLV type : Domain */
#define TLV_TYPE_DOMAIN 0x0007
/** TLV type : Power constraint */
#define TLV_TYPE_POWER_CONSTRAINT 0x0020
/** TLV type : Power capability */
#define TLV_TYPE_POWER_CAPABILITY 0x0021
#define TLV_TYPE_HT_CAPABILITY 0x002d
/** TLV type : Vendor Specific IE */
#define TLV_TYPE_VENDOR_SPECIFIC_IE 0x00dd
#define TLV_TYPE_EXTENSION_ID 0x00ff

/** Proprietary TLV IDs */
/** TLV type : Key material */
#define TLV_TYPE_KEY_MATERIAL (PROPRIETARY_TLV_BASE_ID + 0) /* 0x0100 */
/** TLV type : Channel list */
#define TLV_TYPE_CHANLIST (PROPRIETARY_TLV_BASE_ID + 1) /* 0x0101 */
/** TLV type : Number of probes */
#define TLV_TYPE_NUMPROBES (PROPRIETARY_TLV_BASE_ID + 2) /* 0x0102 */
/** TLV type : Beacon RSSI low */
#define TLV_TYPE_RSSI_LOW (PROPRIETARY_TLV_BASE_ID + 4) /* 0x0104 */
/** TLV type : Beacon SNR low */
#define TLV_TYPE_SNR_LOW (PROPRIETARY_TLV_BASE_ID + 5) /* 0x0105 */
/** TLV type : Fail count */
#define TLV_TYPE_FAILCOUNT (PROPRIETARY_TLV_BASE_ID + 6) /* 0x0106 */
/** TLV type : BCN miss */
#define TLV_TYPE_BCNMISS (PROPRIETARY_TLV_BASE_ID + 7) /* 0x0107 */
/** TLV type : LED behavior */
#define TLV_TYPE_LEDBEHAVIOR (PROPRIETARY_TLV_BASE_ID + 9) /* 0x0109 */
/** TLV type : Passthrough */
#define TLV_TYPE_PASSTHROUGH (PROPRIETARY_TLV_BASE_ID + 10) /* 0x010a */
/** TLV type : Power TBL 2.4 Ghz */
#define TLV_TYPE_POWER_TBL_2_4GHZ (PROPRIETARY_TLV_BASE_ID + 12) /* 0x010c */
/** TLV type : Power TBL 5 GHz */
#define TLV_TYPE_POWER_TBL_5GHZ (PROPRIETARY_TLV_BASE_ID + 13) /* 0x010d */
/** TLV type : WMM queue status */
#define TLV_TYPE_WMMQSTATUS (PROPRIETARY_TLV_BASE_ID + 16) /* 0x0110 */
/** TLV type : Wildcard SSID */
#define TLV_TYPE_WILDCARDSSID (PROPRIETARY_TLV_BASE_ID + 18) /* 0x0112 */
/** TLV type : TSF timestamp */
#define TLV_TYPE_TSFTIMESTAMP (PROPRIETARY_TLV_BASE_ID + 19) /* 0x0113 */
/** TLV type : ARP filter */
#define TLV_TYPE_ARP_FILTER (PROPRIETARY_TLV_BASE_ID + 21) /* 0x0115 */
/** TLV type : Beacon RSSI high */
#define TLV_TYPE_RSSI_HIGH (PROPRIETARY_TLV_BASE_ID + 22) /* 0x0116 */
/** TLV type : Beacon SNR high */
#define TLV_TYPE_SNR_HIGH (PROPRIETARY_TLV_BASE_ID + 23) /* 0x0117 */
/** TLV type : Start BG scan later */
#define TLV_TYPE_STARTBGSCANLATER (PROPRIETARY_TLV_BASE_ID + 30) /* 0x011e */
/** TLV type : Authentication type */
#define TLV_TYPE_AUTH_TYPE (PROPRIETARY_TLV_BASE_ID + 31) /* 0x011f */
/** TLV type : STA Mac address */
#define TLV_TYPE_STA_MAC_ADDRESS (PROPRIETARY_TLV_BASE_ID + 32) /* 0x0120 */
/** TLV type : BSSID */
#define TLV_TYPE_BSSID (PROPRIETARY_TLV_BASE_ID + 35) /* 0x0123 */
/** TLV type : Link Quality */
#define TLV_TYPE_LINK_QUALITY (PROPRIETARY_TLV_BASE_ID + 36) /* 0x0124 */
/** TLV type : Data RSSI low */
#define TLV_TYPE_RSSI_LOW_DATA (PROPRIETARY_TLV_BASE_ID + 38) /* 0x0126 */
/** TLV type : Data SNR low */
#define TLV_TYPE_SNR_LOW_DATA (PROPRIETARY_TLV_BASE_ID + 39) /* 0x0127 */
/** TLV type : Data RSSI high */
#define TLV_TYPE_RSSI_HIGH_DATA (PROPRIETARY_TLV_BASE_ID + 40) /* 0x0128 */
/** TLV type : Data SNR high */
#define TLV_TYPE_SNR_HIGH_DATA (PROPRIETARY_TLV_BASE_ID + 41) /* 0x0129 */
/** TLV type : Channel band list */
#define TLV_TYPE_CHANNELBANDLIST (PROPRIETARY_TLV_BASE_ID + 42) /* 0x012a */
#ifdef UAP_SUPPORT
/** TLV type : AP Channel band Config */
#define TLV_TYPE_UAP_CHAN_BAND_CONFIG                                          \
	(PROPRIETARY_TLV_BASE_ID + 42) /* 0x012a */
/** TLV type : AP Mac address */
#define TLV_TYPE_UAP_MAC_ADDRESS (PROPRIETARY_TLV_BASE_ID + 43) /* 0x012b */
/** TLV type : AP Beacon period */
#define TLV_TYPE_UAP_BEACON_PERIOD (PROPRIETARY_TLV_BASE_ID + 44) /* 0x012c */
/** TLV type : AP DTIM period */
#define TLV_TYPE_UAP_DTIM_PERIOD (PROPRIETARY_TLV_BASE_ID + 45) /* 0x012d */
/** TLV type : AP Tx power */
#define TLV_TYPE_UAP_TX_POWER (PROPRIETARY_TLV_BASE_ID + 47) /* 0x012f */
/** TLV type : AP SSID broadcast control */
#define TLV_TYPE_UAP_BCAST_SSID_CTL (PROPRIETARY_TLV_BASE_ID + 48) /* 0x0130   \
								    */
/** TLV type : AP Preamble control */
#define TLV_TYPE_UAP_PREAMBLE_CTL (PROPRIETARY_TLV_BASE_ID + 49) /* 0x0131 */
/** TLV type : AP Antenna control */
#define TLV_TYPE_UAP_ANTENNA_CTL (PROPRIETARY_TLV_BASE_ID + 50) /* 0x0132 */
/** TLV type : AP RTS threshold */
#define TLV_TYPE_UAP_RTS_THRESHOLD (PROPRIETARY_TLV_BASE_ID + 51) /* 0x0133 */
/** TLV type : AP Tx data rate */
#define TLV_TYPE_UAP_TX_DATA_RATE (PROPRIETARY_TLV_BASE_ID + 53) /* 0x0135 */
/** TLV type: AP Packet forwarding control */
#define TLV_TYPE_UAP_PKT_FWD_CTL (PROPRIETARY_TLV_BASE_ID + 54) /* 0x0136 */
/** TLV type: STA information */
#define TLV_TYPE_UAP_STA_INFO (PROPRIETARY_TLV_BASE_ID + 55) /* 0x0137 */
/** TLV type: AP STA MAC address filter */
#define TLV_TYPE_UAP_STA_MAC_ADDR_FILTER                                       \
	(PROPRIETARY_TLV_BASE_ID + 56) /* 0x0138 */
/** TLV type: AP STA ageout timer */
#define TLV_TYPE_UAP_STA_AGEOUT_TIMER                                          \
	(PROPRIETARY_TLV_BASE_ID + 57) /* 0x0139 */
#endif /* UAP_SUPPORT */
/** TLV type : Security Cfg */
#define TLV_TYPE_SECURITY_CFG (PROPRIETARY_TLV_BASE_ID + 58) /* 0x013a */
#ifdef UAP_SUPPORT
/** TLV type: AP WEP keys */
#define TLV_TYPE_UAP_WEP_KEY (PROPRIETARY_TLV_BASE_ID + 59) /* 0x013b */
#endif /* UAP_SUPPORT */
/** TLV type : Passphrase */
#define TLV_TYPE_PASSPHRASE (PROPRIETARY_TLV_BASE_ID + 60) /* 0x013c */
#ifdef UAP_SUPPORT
/** TLV type: AP WPA passphrase */
#define TLV_TYPE_UAP_WPA_PASSPHRASE (PROPRIETARY_TLV_BASE_ID + 60) /* 0x013c   \
								    */
#endif /* UAP_SUPPORT */
/** TLV type : Encryption Protocol TLV */
#define TLV_TYPE_ENCRYPTION_PROTO (PROPRIETARY_TLV_BASE_ID + 64) /* 0x0140 */
#ifdef UAP_SUPPORT
/** TLV type: AP protocol */
#define TLV_TYPE_UAP_ENCRYPT_PROTOCOL                                          \
	(PROPRIETARY_TLV_BASE_ID + 64) /* 0x0140 */
/** TLV type: AP AKMP */
#define TLV_TYPE_UAP_AKMP (PROPRIETARY_TLV_BASE_ID + 65) /* 0x0141 */
#endif /* UAP_SUPPORT */
/** TLV type : Cipher TLV */
#define TLV_TYPE_CIPHER (PROPRIETARY_TLV_BASE_ID + 66) /* 0x0142 */
/** TLV type : PMK */
#define TLV_TYPE_PMK (PROPRIETARY_TLV_BASE_ID + 68) /* 0x0144 */
#ifdef UAP_SUPPORT
/** TLV type: AP Fragment threshold */
#define TLV_TYPE_UAP_FRAG_THRESHOLD (PROPRIETARY_TLV_BASE_ID + 70) /* 0x0146   \
								    */
/** TLV type: AP Group rekey timer */
#define TLV_TYPE_UAP_GRP_REKEY_TIME (PROPRIETARY_TLV_BASE_ID + 71) /* 0x0147   \
								    */
#endif /* UAP_SUPPORT */
/** TLV type : BCN miss */
#define TLV_TYPE_PRE_BCNMISS (PROPRIETARY_TLV_BASE_ID + 73) /* 0x0149 */
/** TLV type : HT Capabilities */
#define TLV_TYPE_HT_CAP (PROPRIETARY_TLV_BASE_ID + 74) /* 0x014a */
/** TLV type : HT Information */
#define TLV_TYPE_HT_INFO (PROPRIETARY_TLV_BASE_ID + 75) /* 0x014b */
/** TLV type : Secondary Channel Offset */
#define TLV_SECONDARY_CHANNEL_OFFSET (PROPRIETARY_TLV_BASE_ID + 76) /* 0x014c  \
								     */
/** TLV type : 20/40 BSS Coexistence */
#define TLV_TYPE_2040BSS_COEXISTENCE (PROPRIETARY_TLV_BASE_ID + 77) /* 0x014d  \
								     */
/** TLV type : Overlapping BSS Scan Parameters */
#define TLV_TYPE_OVERLAP_BSS_SCAN_PARAM                                        \
	(PROPRIETARY_TLV_BASE_ID + 78) /* 0x014e */
/** TLV type : Extended capabilities */
#define TLV_TYPE_EXTCAP (PROPRIETARY_TLV_BASE_ID + 79) /* 0x014f */
/** TLV type : Set of MCS values that STA desires to use within the BSS */
#define TLV_TYPE_HT_OPERATIONAL_MCS_SET                                        \
	(PROPRIETARY_TLV_BASE_ID + 80) /* 0x0150 */
/** TLV type : Rate scope */
#define TLV_TYPE_RATE_DROP_PATTERN (PROPRIETARY_TLV_BASE_ID + 81) /* 0x0151 */
/** TLV type : Rate drop pattern */
#define TLV_TYPE_RATE_DROP_CONTROL (PROPRIETARY_TLV_BASE_ID + 82) /* 0x0152 */
/** TLV type : Rate scope */
#define TLV_TYPE_RATE_SCOPE (PROPRIETARY_TLV_BASE_ID + 83) /* 0x0153 */
/** TLV type : Power group */
#define TLV_TYPE_POWER_GROUP (PROPRIETARY_TLV_BASE_ID + 84) /* 0x0154 */
#ifdef UAP_SUPPORT
/**TLV type : AP Max Station number */
#define TLV_TYPE_UAP_MAX_STA_CNT (PROPRIETARY_TLV_BASE_ID + 85) /* 0x0155 */
#endif /* UAP_SUPPORT */
/** TLV type : Scan Response */
#define TLV_TYPE_BSS_SCAN_RSP (PROPRIETARY_TLV_BASE_ID + 86) /* 0x0156 */
/** TLV type : Scan Response Stats */
#define TLV_TYPE_BSS_SCAN_INFO (PROPRIETARY_TLV_BASE_ID + 87) /* 0x0157 */
/** TLV type : 11h Basic Rpt */
#define TLV_TYPE_CHANRPT_11H_BASIC (PROPRIETARY_TLV_BASE_ID + 91) /* 0x015b */
#ifdef UAP_SUPPORT
/**TLV type : AP Retry limit */
#define TLV_TYPE_UAP_RETRY_LIMIT (PROPRIETARY_TLV_BASE_ID + 93) /* 0x015d */
#endif /* UAP_SUPPORT */
/** TLV type: WAPI IE */
#define TLV_TYPE_WAPI_IE (PROPRIETARY_TLV_BASE_ID + 94) /* 0x015e */
#ifdef UAP_SUPPORT
/** TLV type : AP MCBC data rate */
#define TLV_TYPE_UAP_MCBC_DATA_RATE (PROPRIETARY_TLV_BASE_ID + 98) /* 0x0162   \
								    */
#endif /* UAP_SUPPORT */
#ifdef UAP_SUPPORT
/**TLV type: AP RSN replay protection */
#define TLV_TYPE_UAP_RSN_REPLAY_PROTECT                                        \
	(PROPRIETARY_TLV_BASE_ID + 100) /* 0x0164 */
/** TLV ID : WAPI Information */
#define TLV_TYPE_AP_WAPI_INFO (PROPRIETARY_TLV_BASE_ID + 103) /* 0x0167 */
#endif /* UAP_SUPPORT */
/** TLV ID : Management Frame */
#define TLV_TYPE_MGMT_FRAME (PROPRIETARY_TLV_BASE_ID + 104) /* 0x0168 */
/** TLV type: MGMT IE */
#define TLV_TYPE_MGMT_IE (PROPRIETARY_TLV_BASE_ID + 105) /* 0x0169 */
#ifdef UAP_SUPPORT
/** TLV type: AP Sleep param */
#define TLV_TYPE_AP_SLEEP_PARAM (PROPRIETARY_TLV_BASE_ID + 106) /* 0x016a */
/** TLV type: AP Inactivity Sleep param */
#define TLV_TYPE_AP_INACT_SLEEP_PARAM                                          \
	(PROPRIETARY_TLV_BASE_ID + 107) /* 0x016b */
/**TLV type: AP mgmt IE passthru mask */
#define TLV_TYPE_UAP_MGMT_IE_PASSTHRU_MASK                                     \
	(PROPRIETARY_TLV_BASE_ID + 112) /* 0x0170 */
#endif /* UAP_SUPPORT */
/** TLV type : auto ds param */
#define TLV_TYPE_AUTO_DS_PARAM (PROPRIETARY_TLV_BASE_ID + 113) /* 0x0171 */
/** TLV type : ps param */
#define TLV_TYPE_PS_PARAM (PROPRIETARY_TLV_BASE_ID + 114) /* 0x0172 */
#ifdef UAP_SUPPORT
/**TLV type: AP pairwise handshake timeout */
#define TLV_TYPE_UAP_EAPOL_PWK_HSK_TIMEOUT                                     \
	(PROPRIETARY_TLV_BASE_ID + 117) /* 0x0175 */
/**TLV type: AP pairwise handshake retries */
#define TLV_TYPE_UAP_EAPOL_PWK_HSK_RETRIES                                     \
	(PROPRIETARY_TLV_BASE_ID + 118) /* 0x0176 */
/**TLV type: AP groupwise handshake timeout */
#define TLV_TYPE_UAP_EAPOL_GWK_HSK_TIMEOUT                                     \
	(PROPRIETARY_TLV_BASE_ID + 119) /* 0x0177 */
/**TLV type: AP groupwise handshake retries */
#define TLV_TYPE_UAP_EAPOL_GWK_HSK_RETRIES                                     \
	(PROPRIETARY_TLV_BASE_ID + 120) /* 0x0178 */
#endif /* UAP_SUPPORT */
#ifdef OPCHAN
/** TLV type : OpChannel control */
#define TLV_TYPE_OPCHAN_CONTROL_DESC                                           \
	(PROPRIETARY_TLV_BASE_ID + 121) /* 0x0179 */
/** TLV type : OpChannel channel group control */
#define TLV_TYPE_OPCHAN_CHANGRP_CTRL                                           \
	(PROPRIETARY_TLV_BASE_ID + 122) /* 0x017a */
#endif /* OPCHAN */
#ifdef UAP_SUPPORT
/** TLV type: AP PS STA ageout timer */
#define TLV_TYPE_UAP_PS_STA_AGEOUT_TIMER                                       \
	(PROPRIETARY_TLV_BASE_ID + 123) /* 0x017b */
#endif /* UAP_SUPPORT */
#ifdef WIFI_DIRECT_SUPPORT
/** TLV type : p2p NOA */
#define TLV_TYPE_WIFI_DIRECT_NOA (PROPRIETARY_TLV_BASE_ID + 131) /* 0x0183 */
/** TLV type : p2p opp ps */
#define TLV_TYPE_WIFI_DIRECT_OPP_PS (PROPRIETARY_TLV_BASE_ID + 132) /* 0x0184  \
								     */
#endif /* WIFI_DIRECT_SUPPORT */
/** TLV type : Action frame */
#define TLV_TYPE_IEEE_ACTION_FRAME (PROPRIETARY_TLV_BASE_ID + 140) /* 0x018c   \
								    */
#ifdef UAP_SUPPORT
/** TLV type : Pairwise Cipher */
#define TLV_TYPE_PWK_CIPHER (PROPRIETARY_TLV_BASE_ID + 145) /* 0x0191 */
/** TLV type : Group Cipher */
#define TLV_TYPE_GWK_CIPHER (PROPRIETARY_TLV_BASE_ID + 146) /* 0x0192 */
/** TLV type : BSS Status */
#define TLV_TYPE_BSS_STATUS (PROPRIETARY_TLV_BASE_ID + 147) /* 0x0193 */
#endif /* UAP_SUPPORT */
/** TLV type : TX pause TLV */
#define TLV_TYPE_TX_PAUSE (PROPRIETARY_TLV_BASE_ID + 148) /* 0x0194 */
/** TLV : 20/40 coex config */
#define TLV_TYPE_2040_BSS_COEX_CONTROL                                         \
	(PROPRIETARY_TLV_BASE_ID + 152) /* 0x0198 */
/** TLV type : RXBA_SYNC */
#define TLV_TYPE_RXBA_SYNC (PROPRIETARY_TLV_BASE_ID + 153) /* 0x0199 */
#define TLV_TYPE_COALESCE_RULE (PROPRIETARY_TLV_BASE_ID + 154) /* 0x019a */
/** TLV type: key param v2 */
#define TLV_TYPE_KEY_PARAM_V2 (PROPRIETARY_TLV_BASE_ID + 156) /* 0x019c */
#ifdef WIFI_DIRECT_SUPPORT
/** TLV type : AP PSK */
#define TLV_TYPE_UAP_PSK (PROPRIETARY_TLV_BASE_ID + 168) /* 0x01a8 */
#endif /* WIFI_DIRECT_SUPPORT */
/** TLV type: MAX_MGMT_IE */
#define TLV_TYPE_MAX_MGMT_IE (PROPRIETARY_TLV_BASE_ID + 170) /* 0x01aa */
/** TLV : Region Domain Code */
#define TLV_TYPE_REGION_DOMAIN_CODE (PROPRIETARY_TLV_BASE_ID + 171) /* 0x01ab  \
								     */
/** TLV type: BG scan repeat count */
#define TLV_TYPE_REPEAT_COUNT (PROPRIETARY_TLV_BASE_ID + 176) /* 0x01b0 */
#ifdef USB
/** TLV ID : USB Aggregation parameters */
#define NXP_USB_AGGR_PARAM_TLV_ID (PROPRIETARY_TLV_BASE_ID + 177) /* 0x01b1 */
#endif /* USB */
/** TLV type: ps params in hs */
#define TLV_TYPE_PS_PARAMS_IN_HS (PROPRIETARY_TLV_BASE_ID + 181) /* 0x01b5 */
/** TLV type: hs wake hold off */
#define TLV_TYPE_HS_WAKE_HOLDOFF (PROPRIETARY_TLV_BASE_ID + 182) /* 0x01b6 */
/** TLV ID for multi chan info */
#define TLV_TYPE_MULTI_CHAN_INFO (PROPRIETARY_TLV_BASE_ID + 183) /* 0x01b7 */
/** TLV ID for multi chan group info */
#define TLV_TYPE_MULTI_CHAN_GROUP_INFO_TLV_ID                                  \
	(PROPRIETARY_TLV_BASE_ID + 184) /* 0x01b8 */
/** TLV type : TDLS IDLE TIMEOUT */
#define TLV_TYPE_TDLS_IDLE_TIMEOUT (PROPRIETARY_TLV_BASE_ID + 194) /* 0x01c2   \
								    */
/** TLV type : SCAN channel gap */
#define TLV_TYPE_SCAN_CHANNEL_GAP (PROPRIETARY_TLV_BASE_ID + 197) /* 0x01c5 */
/** TLV type : Channel statistics */
#define TLV_TYPE_CHANNEL_STATS (PROPRIETARY_TLV_BASE_ID + 198) /* 0x01c6 */
/** FW VERSION tlv */
#define TLV_TYPE_FW_VER_INFO (PROPRIETARY_TLV_BASE_ID + 199) /* 0x01c7 */
/** TLV type :  aggr win size */
#define TLV_BTCOEX_WL_AGGR_WINSIZE (PROPRIETARY_TLV_BASE_ID + 202) /* 0x01ca   \
								    */
/** TLV type :  scan time */
#define TLV_BTCOEX_WL_SCANTIME (PROPRIETARY_TLV_BASE_ID + 203) /* 0x01cb */
/** TLV type : BSS_MODE */
#define TLV_TYPE_BSS_MODE (PROPRIETARY_TLV_BASE_ID + 206) /* 0x01ce */
/** TLV type : Ewpa_eapol_pkt */
#define TLV_TYPE_EAPOL_PKT (PROPRIETARY_TLV_BASE_ID + 207) /* 0x01cf */
#ifdef UAP_SUPPORT
/** TLV type :  AP WMM params */
#define TLV_TYPE_AP_WMM_PARAM (PROPRIETARY_TLV_BASE_ID + 208) /* 0x01d0 */
#endif /* UAP_SUPPORT */
/** TLV type : ESS scan*/
#define TLV_TYPE_ENERGYEFFICIENTSCAN                                           \
	(PROPRIETARY_TLV_BASE_ID + 218) /* 0x01da */
/** TLV type : EES Configuration */
#define TLV_TYPE_EES_CFG (PROPRIETARY_TLV_BASE_ID + 218) /* 0x01da */
/** TLV type : EES Network Configuration */
#define TLV_TYPE_EES_NET_CFG (PROPRIETARY_TLV_BASE_ID + 219) /* 0x01db */
/** TLV rssi info */
#define TLV_TYPE_RSSI_INFO (PROPRIETARY_TLV_BASE_ID + 229) /* 0x01e5 */
#define TLV_TYPE_IPV6_RA_OFFLOAD (PROPRIETARY_TLV_BASE_ID + 230) /* 0x01e6 */
/** TLV type :  FILS IP config params */
#define TLV_TYPE_AP_FILS_IP_CFG (PROPRIETARY_TLV_BASE_ID + 231) /* 0x01e7 */
/** TLV to indicate firmware only keep probe response while scan */
#define TLV_TYPE_ONLYPROBERESP (PROPRIETARY_TLV_BASE_ID + 233) /* 0x01e9 */
#define TLV_TYPE_RANDOM_MAC (PROPRIETARY_TLV_BASE_ID + 236) /* 0x01ec */
#define TLV_TYPE_CHAN_ATTR_CFG (PROPRIETARY_TLV_BASE_ID + 237) /* 0x01ed */
#define TLV_TYPE_REGION_INFO (PROPRIETARY_TLV_BASE_ID + 238) /* 0x01ee */
/** TLV type : ENABLE ROAM IE */
#define TLV_TYPE_ROAM (PROPRIETARY_TLV_BASE_ID + 245) /* 0x01f5 */
/** TLV type : AP LIST IE */
#define TLV_TYPE_APLIST (PROPRIETARY_TLV_BASE_ID + 246) /* 0x01f6 */
/** TLV type : PMK */
#define TLV_TYPE_PMK_R0 (PROPRIETARY_TLV_BASE_ID + 247) /* 0x01f7 */
/** TLV type : PMK */
#define TLV_TYPE_PMK_R0_NAME (PROPRIETARY_TLV_BASE_ID + 248) /* 0x01f8 */
/** TLV for cloud keep alive control info */
#define TLV_TYPE_CLOUD_KEEP_ALIVE (PROPRIETARY_TLV_BASE_ID + 258) /* 0x0202 */
/** TLV for cloud keep alive control info */
#define TLV_TYPE_KEEP_ALIVE_CTRL (PROPRIETARY_TLV_BASE_ID + 259) /* 0x0203 */
/** TLV for cloud keep alive packet */
#define TLV_TYPE_KEEP_ALIVE_PKT (PROPRIETARY_TLV_BASE_ID + 260) /* 0x0204 */
/** TLV type: wake up source */
#define TLV_TYPE_HS_WAKEUP_SOURCE_GPIO                                         \
	(PROPRIETARY_TLV_BASE_ID + 261) /* 0x0205 */
#define TLV_TYPE_POWER_TABLE (PROPRIETARY_TLV_BASE_ID + 262) /* 0x0206 */
/** TLV ID for DRCS TimeSlice */
#define NXP_DRCS_TIME_SLICE_TLV_ID (PROPRIETARY_TLV_BASE_ID + 263) /* 0x0207   \
								    */
/** TLV type : TRIGGER CONDITION*/
#define TLV_TYPE_ROM_TRIGGER (PROPRIETARY_TLV_BASE_ID + 264) /* 0x0208 */
/** TLV type : RETRY_COUNT*/
#define TLV_TYPE_ROM_RETRY_COUNT (PROPRIETARY_TLV_BASE_ID + 265) /* 0x0209 */
/** TLV type : BGSCAN SETTING*/
#define TLV_TYPE_ROM_BGSCAN (PROPRIETARY_TLV_BASE_ID + 266) /* 0x020a */
/** TLV type : PARA RSSI*/
#define TLV_TYPE_ROM_PARA_RSSI (PROPRIETARY_TLV_BASE_ID + 267) /* 0x020b */
/** TLV type: management filter  */
#define TLV_TYPE_MGMT_FRAME_WAKEUP (PROPRIETARY_TLV_BASE_ID + 278) /* 0x0216   \
								    */
/** TLV type : FW support max connection TLV */
#define TLV_TYPE_MAX_CONN (PROPRIETARY_TLV_BASE_ID + 279) /* 0x0217 */
/** TLV type: extend wakeup source */
#define TLV_TYPE_WAKEUP_EXTEND (PROPRIETARY_TLV_BASE_ID + 280) /* 0x0218 */
/** TLV type: HS antenna mode */
#define TLV_TYPE_HS_ANTMODE (PROPRIETARY_TLV_BASE_ID + 281) /* 0x0219 */
/** TLV type: robustcoex mode */
#define TLV_TYPE_ROBUSTCOEX (PROPRIETARY_TLV_BASE_ID + 283) /* 0x021b */
/** TLV type : BSSID blacklist*/
#define TLV_TYPE_BLACKLIST_BSSID (PROPRIETARY_TLV_BASE_ID + 285) /* 0x021d */
/** TLV type : BAND & RSSI*/
#define TLV_TYPE_BAND_RSSI (PROPRIETARY_TLV_BASE_ID + 286) /* 0x021e */
/** TLV type : beacon timeout */
#define TLV_TYPE_BCN_TIMEOUT (PROPRIETARY_TLV_BASE_ID + 287) /* 0x021f */
#ifdef UAP_SUPPORT
/** TLV type : AP Tx beacon rate */
#define TLV_TYPE_UAP_TX_BEACON_RATE (PROPRIETARY_TLV_BASE_ID + 288) /* 0x0220  \
								     */
#endif /* UAP_SUPPORT */
/** TLV type : KEY params*/
#define TLV_TYPE_ROAM_OFFLOAD_USER_SET_PMK                                     \
	(PROPRIETARY_TLV_BASE_ID + 291) /* 0x0223 */
/** TLV for RTT Range Request */
#define TLV_TYPE_RTT_RANGE_REQUEST (PROPRIETARY_TLV_BASE_ID + 293) /* 0x0225   \
								    */
/** TLV for RTT Range Cancel */
#define TLV_TYPE_RTT_RANGE_CANCEL (PROPRIETARY_TLV_BASE_ID + 294) /* 0x0226 */
/** TLV for RTT Result */
#define TLV_TYPE_RTT_RESULT (PROPRIETARY_TLV_BASE_ID + 295) /* 0x0227 */
/** TLV for RTTResponderInfo */
#define TLV_TYPE_RTT_RESPONDER_INFO (PROPRIETARY_TLV_BASE_ID + 296) /* 0x0228  \
								     */
/** TLV for RTTResponderEnCfg */
#define TLV_TYPE_RTT_RESPONDER_EN_CFG                                          \
	(PROPRIETARY_TLV_BASE_ID + 297) /* 0x0229 */
/** TLV for RTTLCICfg */
#define TLV_TYPE_RTT_LCI_CFG (PROPRIETARY_TLV_BASE_ID + 298) /* 0x022a */
/** TLV for RTTLCRCfg */
#define TLV_TYPE_RTT_LCR_CFG (PROPRIETARY_TLV_BASE_ID + 299) /* 0x022b */
#define TLV_TYPE_LL_STAT_IFACE (PROPRIETARY_TLV_BASE_ID + 300) /* 0x022c */
#define TLV_TYPE_LL_STAT_RADIO (PROPRIETARY_TLV_BASE_ID + 301) /* 0x022d */
/**TLV type : Host MLME Flag*/
#define TLV_TYPE_HOST_MLME (PROPRIETARY_TLV_BASE_ID + 307) /* 0x0233 */
#ifdef UAP_SUPPORT
/** TLV id: station flag */
#define TLV_TYPE_UAP_STA_FLAGS (PROPRIETARY_TLV_BASE_ID + 313) /* 0x0239 */
#endif /* UAP_SUPPORT */
#define TLV_TYPE_DMCS_STATUS (PROPRIETARY_TLV_BASE_ID + 314) /* 0x023a */
/** TLV type : ZERO DFS Operation */
#define TLV_TYPE_ZERO_DFS_OPERATION (PROPRIETARY_TLV_BASE_ID + 315) /* 0x023b  \
								     */
#if defined(PCIE9098) || defined(SDAW693) || defined(SD9098) ||                \
	defined(USB9098) || defined(PCIE9097) || defined(USB9097) ||           \
	defined(SDIW624) || defined(PCIEAW693) || defined(PCIEIW624) ||        \
	defined(USBIW624) || defined(SD9097)
/* TLV type: reg type */
#define TLV_TYPE_REG_ACCESS_CTRL (PROPRIETARY_TLV_BASE_ID + 316) /* 0x023c*/
#endif
#define TLV_TYPE_POWER_TABLE_ATTR (PROPRIETARY_TLV_BASE_ID + 317) /* 0x023d */
/** TLV type: fw cap info */
#define TLV_TYPE_FW_CAP_INFO (PROPRIETARY_TLV_BASE_ID + 318) /* 0x023e */
/** TLV type : TX RATE CFG, rename from TLV_TYPE_GI_LTF_SIZE to include CMD and
 * HE ER SU settings to this tlv */
#define TLV_TYPE_TX_RATE_CFG (PROPRIETARY_TLV_BASE_ID + 319) /* 0x023f */
#ifdef UAP_SUPPORT
#define TLV_TYPE_UAP_MAX_STA_CNT_PER_CHIP                                      \
	(PROPRIETARY_TLV_BASE_ID + 320) /* 0x0240 */
#endif /* UAP_SUPPORT */
/** TLV type : SAE Password */
#define TLV_TYPE_SAE_PASSWORD (PROPRIETARY_TLV_BASE_ID + 321) /* 0x0241 */
/** TLV type : DFS W53 Configuration */
#define TLV_TYPE_DFS_W53_CFG (PROPRIETARY_TLV_BASE_ID + 325) /* 0x0245 */
/** TLV type: MULTI AP Flag */
#define TLV_TYPE_MULTI_AP (PROPRIETARY_TLV_BASE_ID + 326) /* 0x0246 */
/** TLV type : AP wacp mode */
#define TLV_TYPE_UAP_WACP_MODE (PROPRIETARY_TLV_BASE_ID + 327) /* 0x0247 */
#define TLV_TYPE_PREV_BSSID (PROPRIETARY_TLV_BASE_ID + 330) /* 0x024a */
/** TLV type : SAE PWE Derivation Mode */
#define TLV_TYPE_WPA3_SAE_PWE_DERIVATION_MODE                                  \
	(PROPRIETARY_TLV_BASE_ID + 339) /* 0x0253 */
/** TLV type : GPIO TSF LATCH CONFIG */
#define TLV_TYPE_GPIO_TSF_LATCH_CONFIG                                         \
	(PROPRIETARY_TLV_BASE_ID + 340) /* 0x0254 */
/** TLV type : GPIO TSF LATCH REPORT*/
#define TLV_TYPE_GPIO_TSF_LATCH_REPORT                                         \
	(PROPRIETARY_TLV_BASE_ID + 341) /* 0x0255 */
#define NXP_ACTION_CHAN_SWITCH_ANNOUNCE                                        \
	(PROPRIETARY_TLV_BASE_ID + 342) /* 0x0256 */
#ifdef UAP_SUPPORT
#define NXP_802_11_PER_PEER_STATS_CFG_TLV_ID                                   \
	(PROPRIETARY_TLV_BASE_ID + 346) /* 0x025a */
#define NXP_802_11_PER_PEER_STATS_ENTRY_TLV_ID                                 \
	(PROPRIETARY_TLV_BASE_ID + 347) /* 0x025b */
#endif /* UAP_SUPPORT */
/** TLV type: secure boot uuid */
#define TLV_TYPE_SECURE_BOOT_UUID (PROPRIETARY_TLV_BASE_ID + 348) /* 0x025c */
#define TLV_TYPE_CLOUD_KEEP_ALIVE_ACK                                          \
	(PROPRIETARY_TLV_BASE_ID + 349) /* 0x025d */
#define TLV_TYPE_6G_SCAN_PARAMS (PROPRIETARY_TLV_BASE_ID + 350) /* 0x025e */
/** TLV type: ps_ext_param */
#define TLV_TYPE_PS_EXT_PARAM (PROPRIETARY_TLV_BASE_ID + 351) /* 0x025f */
#define TLV_TYPE_MCLIENT_FW_CAPS (PROPRIETARY_TLV_BASE_ID + 352) /* 0x0260 */
#define NXP_CSI_MONITOR_TLV_ID (PROPRIETARY_TLV_BASE_ID + 354) /* 0x0262 */
#define TLV_TYPE_BOOT_TIME_CFG (PROPRIETARY_TLV_BASE_ID + 355) /* 0x0263 */

#define VENDOR_IE_OUIS_TLV_ID (PROPRIETARY_TLV_BASE_ID + 357) /* 0x0265 */
#define TLV_TYPE_AP_INFO (PROPRIETARY_TLV_BASE_ID + 358) /* 0x0266 */
#define TLV_TYPE_CSI_AGC_CONFIG (PROPRIETARY_TLV_BASE_ID + 363) /* 0x026b */
#define TLV_TYPE_PER_BAND_TXPWR_CAP (PROPRIETARY_TLV_BASE_ID + 365) /* 0x026d  \
								     */

#endif /* !MLAN_TLV_IDS_H_ */
