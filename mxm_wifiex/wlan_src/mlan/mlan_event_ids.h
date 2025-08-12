/** @file mlan_event_ids.h
 *
 *  @brief This file contains FW event ID definitions.
 *
 *
 *  Copyright 2023 NXP
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

/* include guard is absent intentionally */

/* FW -> Host Event ID Constants */

/** Card Event definition : Dummy host wakeup signal */
ENUM_ELEMENT(EVENT_DUMMY_HOST_WAKEUP_SIGNAL, 0x0001),
	/** Card Event definition : Link lost */
	ENUM_ELEMENT(EVENT_LINK_LOST, 0x0003),
	/** Card Event definition : Link sensed */
	ENUM_ELEMENT(EVENT_LINK_SENSED, 0x0004),
	/** Card Event definition : MIB changed */
	ENUM_ELEMENT(EVENT_MIB_CHANGED, 0x0006),
	/** Card Event definition : Init done */
	ENUM_ELEMENT(EVENT_INIT_DONE, 0x0007),
	/** Card Event definition : Deauthenticated */
	ENUM_ELEMENT(EVENT_DEAUTHENTICATED, 0x0008),
	/** Card Event definition : Disassociated */
	ENUM_ELEMENT(EVENT_DISASSOCIATED, 0x0009),
	/** Card Event definition : Power save awake */
	ENUM_ELEMENT(EVENT_PS_AWAKE, 0x000a),
	/** Card Event definition : Power save sleep */
	ENUM_ELEMENT(EVENT_PS_SLEEP, 0x000b),
	/** Card Event definition : MIC error multicast */
	ENUM_ELEMENT(EVENT_MIC_ERR_MULTICAST, 0x000d),
	/** Card Event definition : MIC error unicast */
	ENUM_ELEMENT(EVENT_MIC_ERR_UNICAST, 0x000e),

	/** Card Event definition : Stop Tx */
	ENUM_ELEMENT(EVENT_STOP_TX, 0x0013),
	/** Card Event definition : Start Tx */
	ENUM_ELEMENT(EVENT_START_TX, 0x0014),
	/** Card Event definition : Channel switch */
	ENUM_ELEMENT(EVENT_CHANNEL_SWITCH, 0x0015),

	/** Card Event definition : MEAS report ready */
	ENUM_ELEMENT(EVENT_MEAS_REPORT_RDY, 0x0016),

	/** Card Event definition : WMM status change */
	ENUM_ELEMENT(EVENT_WMM_STATUS_CHANGE, 0x0017),

	/** Card Event definition : BG scan report */
	ENUM_ELEMENT(EVENT_BG_SCAN_REPORT, 0x0018),
	/** Card Event definition : BG scan stopped */
	ENUM_ELEMENT(EVENT_BG_SCAN_STOPPED, 0x0065),

	/** Card Event definition : Beacon RSSI low */
	ENUM_ELEMENT(EVENT_RSSI_LOW, 0x0019),
	/** Card Event definition : Beacon SNR low */
	ENUM_ELEMENT(EVENT_SNR_LOW, 0x001a),
	/** Card Event definition : Maximum fail */
	ENUM_ELEMENT(EVENT_MAX_FAIL, 0x001b),
	/** Card Event definition : Beacon RSSI high */
	ENUM_ELEMENT(EVENT_RSSI_HIGH, 0x001c),
	/** Card Event definition : Beacon SNR high */
	ENUM_ELEMENT(EVENT_SNR_HIGH, 0x001d),

	/** Card Event definition : Data RSSI low */
	ENUM_ELEMENT(EVENT_DATA_RSSI_LOW, 0x0024),
	/** Card Event definition : Data SNR low */
	ENUM_ELEMENT(EVENT_DATA_SNR_LOW, 0x0025),
	/** Card Event definition : Data RSSI high */
	ENUM_ELEMENT(EVENT_DATA_RSSI_HIGH, 0x0026),
	/** Card Event definition : Data SNR high */
	ENUM_ELEMENT(EVENT_DATA_SNR_HIGH, 0x0027),

	/** Card Event definition : Link Quality */
	ENUM_ELEMENT(EVENT_LINK_QUALITY, 0x0028),

	/** Card Event definition : Port release event */
	ENUM_ELEMENT(EVENT_PORT_RELEASE, 0x002b),

#ifdef UAP_SUPPORT
	/** Event ID: STA deauth */
	ENUM_ELEMENT(EVENT_MICRO_AP_STA_DEAUTH, 0x002c),
	/** Event ID: STA assoicated */
	ENUM_ELEMENT(EVENT_MICRO_AP_STA_ASSOC, 0x002d),
	/** Event ID: BSS started */
	ENUM_ELEMENT(EVENT_MICRO_AP_BSS_START, 0x002e),
	/** Event ID: BSS idle event */
	ENUM_ELEMENT(EVENT_MICRO_AP_BSS_IDLE, 0x0043),
	/** Event ID: BSS active event */
	ENUM_ELEMENT(EVENT_MICRO_AP_BSS_ACTIVE, 0x0044),

	/** Event ID: MIC countermeasures event */
	ENUM_ELEMENT(EVENT_MICRO_AP_MIC_COUNTERMEASURES, 0x004c),
#endif /* UAP_SUPPORT */

	/** Card Event definition : Pre-Beacon Lost */
	ENUM_ELEMENT(EVENT_PRE_BEACON_LOST, 0x0031),

	ENUM_ELEMENT(EVENT_WATCHDOG_TMOUT, 0x0032),

	/** Card Event definition : Add BA event */
	ENUM_ELEMENT(EVENT_ADDBA, 0x0033),
	/** Card Event definition : Del BA event */
	ENUM_ELEMENT(EVENT_DELBA, 0x0034),
	/** Card Event definition: BA stream timeout*/
	ENUM_ELEMENT(EVENT_BA_STREAM_TIMEOUT, 0x0037),

	/** Card Event definition : AMSDU aggr control */
	ENUM_ELEMENT(EVENT_AMSDU_AGGR_CTRL, 0x0042),

	/** Card Event definition: WEP ICV error */
	ENUM_ELEMENT(EVENT_WEP_ICV_ERR, 0x0046),

	/** Card Event definition : Host sleep enable */
	ENUM_ELEMENT(EVENT_HS_ACT_REQ, 0x0047),

	/** Card Event definition : BW changed */
	ENUM_ELEMENT(EVENT_BW_CHANGE, 0x0048),

#ifdef WIFI_DIRECT_SUPPORT
	/** WIFIDIRECT generic event */
	ENUM_ELEMENT(EVENT_WIFIDIRECT_GENERIC_EVENT, 0x0049),
	/** WIFIDIRECT service discovery event */
	ENUM_ELEMENT(EVENT_WIFIDIRECT_SERVICE_DISCOVERY, 0x004a),
#endif
	/** Remain on Channel expired event */
	ENUM_ELEMENT(EVENT_REMAIN_ON_CHANNEL_EXPIRED, 0x005f),

	/** TDLS generic event */
	ENUM_ELEMENT(EVENT_TDLS_GENERIC_EVENT, 0x0052),

	ENUM_ELEMENT(EVENT_MEF_HOST_WAKEUP, 0x004f),

	/** Card Event definition: Channel switch pending announcment */
	ENUM_ELEMENT(EVENT_CHANNEL_SWITCH_ANN, 0x0050),

	/** Event definition:  Radar Detected by card */
	ENUM_ELEMENT(EVENT_RADAR_DETECTED, 0x0053),

	/** Event definition:  Radar Detected by card */
	ENUM_ELEMENT(EVENT_CHANNEL_REPORT_RDY, 0x0054),

	/** Event definition:  Scan results through event */
	ENUM_ELEMENT(EVENT_EXT_SCAN_REPORT, 0x0058),
	/** Enhance ext scan done event */
	ENUM_ELEMENT(EVENT_EXT_SCAN_STATUS_REPORT, 0x007f),

	/** Event definition: RXBA_SYNC */
	ENUM_ELEMENT(EVENT_RXBA_SYNC, 0x0059),

	/** Event ID: TX data pause event */
	ENUM_ELEMENT(EVENT_TX_DATA_PAUSE, 0x0055),

	/** Event definition : FW debug information */
	ENUM_ELEMENT(EVENT_FW_DEBUG_INFO, 0x0063),

	/** Event ID: SAD Report */
	ENUM_ELEMENT(EVENT_SAD_REPORT, 0x0066),

	/** Event ID: Multi Chan Info*/
	ENUM_ELEMENT(EVENT_MULTI_CHAN_INFO, 0x006a),

	ENUM_ELEMENT(EVENT_FW_DUMP_INFO, 0x0073),
	/** Event ID: Tx status */
	ENUM_ELEMENT(EVENT_TX_STATUS_REPORT, 0x0074),

	ENUM_ELEMENT(EVENT_NAN_GENERIC, 0x0075),

	ENUM_ELEMENT(EVENT_BT_COEX_WLAN_PARA_CHANGE, 0x0076),

	ENUM_ELEMENT(EVENT_VDLL_IND, 0x0081),

	ENUM_ELEMENT(EVENT_ROAM_OFFLOAD, 0x0083),

	ENUM_ELEMENT(EVENT_WLS_FTM_COMPLETE, 0x0086),

	ENUM_ELEMENT(EVENT_EXCEED_MAX_P2P_CONN, 0x0089),

#if defined(PCIE)
	ENUM_ELEMENT(EVENT_SSU_DUMP_DMA, 0x008C),
#endif

	ENUM_ELEMENT(EVENT_CSI, 0x008D),
	ENUM_ELEMENT(EVENT_FW_HANG_REPORT, 0x008F),
	ENUM_ELEMENT(EVENT_DPD_LOG, 0x0091),
	/** Card Event definition : RESET PN */

	ENUM_ELEMENT(EVENT_ASSOC_REQ_IE, 0x0095),

	ENUM_ELEMENT(EVENT_CHAN_LOAD, 0x0099),

	ENUM_ELEMENT(EVENT_IMD3_CAL_START, 0x00A0),
	ENUM_ELEMENT(EVENT_IMD3_CAL_END, 0x00A1),

	/* Always keep this last */
	ENUM_ELEMENT_LAST(__HostEvent_Last)
