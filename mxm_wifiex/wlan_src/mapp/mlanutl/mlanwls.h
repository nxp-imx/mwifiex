/** @file  mlanwls.h
 *
 * @brief 11mc/11az Wifi location services application
 *
 *
 * Copyright 2022 NXP
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
     01/24/2022: initial version
************************************************************************/
#ifndef _WLS_H_
#define _WLS_H_

/** Size of command buffer */
#define MRVDRV_SIZE_OF_CMD_BUFFER (3 * 1024)

/** MAC BROADCAST */
#define MAC_BROADCAST 0x1FF
/** MAC MULTICAST */
#define MAC_MULTICAST 0x1FE
/** Default scan interval in second*/
#define DEFAULT_SCAN_INTERVAL 300

/** Netlink protocol number */
#define NETLINK_NXP (MAX_LINKS - 1)
/** Netlink maximum payload size */
#define NL_MAX_PAYLOAD 1024
/** Default wait time in seconds for events */
#define UAP_RECV_WAIT_DEFAULT 10
#ifndef NLMSG_HDRLEN
/** NL message header length */
#define NLMSG_HDRLEN ((int)NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#endif

/** Host Command ID : FTM session config and control */
#define HostCmd_CMD_FTM_SESSION_CFG 0x024d
#define HostCmd_CMD_FTM_SESSION_CTRL 0x024E
#define HostCmd_CMD_FTM_FEATURE_CTRL 0x024f
#define HostCmd_CMD_WLS_REQ_FTM_RANGE 0x0250

/** Events*/
#define EVENT_WLS_FTM_COMPLETE 0x00000086
#define WLS_SUB_EVENT_FTM_COMPLETE 0
#define WLS_SUB_EVENT_RADIO_RECEIVED 1
#define WLS_SUB_EVENT_RADIO_RPT_RECEIVED 2
#define WLS_SUB_EVENT_ANQP_RESP_RECEIVED 3
#define WLS_SUB_EVENT_RTT_RESULTS 4

/** Custom events definitions */
/** AP connected event */
#define CUS_EVT_AP_CONNECTED "EVENT=AP_CONNECTED"
/** Custom events definitions end */

/*TLVs*/
/** TLV  type ID definition */
#define PROPRIETARY_TLV_BASE_ID 0x0100
#define FTM_SESSION_CFG_INITATOR_TLV_ID (PROPRIETARY_TLV_BASE_ID + 273)
#define FTM_NTB_RANGING_CFG_TLV_ID (PROPRIETARY_TLV_BASE_ID + 343)
#define FTM_RANGE_REPORT_TLV_ID                                                \
	(PROPRIETARY_TLV_BASE_ID + 0x10C) /* 0x0100 + 0x10C = 0x20C */
#define FTM_SESSION_CFG_LCI_TLV_ID (PROPRIETARY_TLV_BASE_ID + 270)
#define FTM_SESSION_CFG_LOCATION_CIVIC_TLV_ID (PROPRIETARY_TLV_BASE_ID + 271)

/** Structure of command table*/
typedef struct {
	/** User Command ID*/
	int cmd_id;
	/** Command name */
	char *cmd;
	/** Command function pointer */
	int (*func)(int argc, char *argv[], void *param);
	/** Command usuage */
	char **help;
} wls_app_command_table;

/** Structure of FTM_SESSION_CFG_NTB_RANGING TLV data*/
typedef struct _ntb_ranging_cfg {
	/** Indicates the channel BW for session*/
	/*0: HE20, 1: HE40, 2: HE80, 3: HE80+80, 4: HE160, 5:HE160_SRF*/
	t_u8 format_bw;
	/** indicates for bandwidths less than or equal to 80 MHz the maximum
	 * number of space-time streams to be used in DL/UL NDP frames in the
	 * session*/
	t_u8 max_i2r_sts_upto80;
	/**indicates for bandwidths less than or equal to 80 MHz the maximum
	 * number of space-time streams to be used in DL/UL NDP frames in the
	 * session*/
	t_u8 max_r2i_sts_upto80;
	/**Specify measurement freq in Hz to calculate measurement interval*/
	t_u8 az_measurement_freq;
	/**Indicates the number of measurements to be done for session*/
	t_u8 az_number_of_measurements;
	/**Include location civic request (Expect location civic from
	 * responder)*/
	t_u8 civic_req;
	/**Include LCI request (Expect LCI info from responder)*/
	t_u8 lci_req;
} __ATTRIB_PACK__ ntb_ranging_cfg_t;

/** Structure of FTM_SESSION_CFG TLV data*/
typedef struct _ftm_session_cfg {
	/** Indicates how many burst instances are requested for the FTM
	 * session*/
	t_u8 burst_exponent;
	/** Indicates the duration of a burst instance*/
	t_u8 burst_duration;
	/**Minimum time between consecutive FTM frames*/
	t_u8 min_delta_FTM;
	/**ASAP/non-ASAP casel*/
	t_u8 is_ASAP;
	/**Number of FTMs per burst*/
	t_u8 per_burst_FTM;
	/**FTM channel spacing: HT20/HT40/VHT80/…*/
	t_u8 channel_spacing;
	/**Indicates the interval between two consecutive burst instances*/
	t_u16 burst_period;
} __ATTRIB_PACK__ ftm_session_cfg_t;

/** Structure for FTM_SESSION_CFG_LOCATION_CIVIC TLV data*/
typedef struct _civic_loc_cfg {
	/**Civic location type*/
	t_u8 civic_location_type;
	/**Country code*/
	t_u16 country_code;
	/**Civic address type*/
	t_u8 civic_address_type;
	/**Civic address length*/
	t_u8 civic_address_length;
	/**Civic Address*/
	t_u8 civic_address[];
} __ATTRIB_PACK__ civic_loc_cfg_t;

/** Structure for FTM_SESSION_CFG_LCI TLV data*/
typedef struct _lci_cfg {
	/** known longitude*/
	double longitude;
	/** known Latitude*/
	double latitude;
	/** known altitude*/
	double altitude;
	/** known Latitude uncertainty*/
	t_u8 lat_unc;
	/** known Longitude uncertainty*/
	t_u8 long_unc;
	/** Known Altitude uncertainty*/
	t_u8 alt_unc;
	/** 1 word for additional Z information */
	t_u32 z_info;
} __ATTRIB_PACK__ lci_cfg_t;

/** Structure for FTM_SESSION_CFG_NTB_RANGING TLV*/
typedef struct _ntb_ranging_cfg_tlv {
	/** Type*/
	t_u16 type;
	/** Length*/
	t_u16 len;
	/** Value*/
	ntb_ranging_cfg_t val;
} __ATTRIB_PACK__ ntb_ranging_cfg_tlv_t;

/** Structure for FTM_SESSION_CFG  TLV*/
typedef struct _ftm_session_cfg_tlv {
	/** Type*/
	t_u16 type;
	/** Length*/
	t_u16 len;
	/** Value*/
	ftm_session_cfg_t val;
	t_u8 civic_req;
	t_u8 lci_req;
} __ATTRIB_PACK__ ftm_session_cfg_tlv_t;

/** Structure for FTM_SESSION_CFG_LOCATION_CIVIC TLV*/
typedef struct _civic_loc_tlv {
	/** Type*/
	t_u16 type;
	/** Length*/
	t_u16 len;
	/** Value*/
	civic_loc_cfg_t val;
} __ATTRIB_PACK__ civic_loc_tlv_t;

/** Structure for FTM_SESSION_CFG_LCI TLV*/
typedef struct _lci_tlv {
	/** Type*/
	t_u16 type;
	/** Length*/
	t_u16 len;
	/** Value*/
	lci_cfg_t val;
} __ATTRIB_PACK__ lci_tlv_t;

/** Structure for DOT11MC FTM_SESSION_CFG */
typedef struct _dot11mc_ftm_cfg {
	/** FTM session cfg*/
	ftm_session_cfg_tlv_t sess_tlv;
	/** Location Request cfg*/
	lci_tlv_t lci_tlv;
	/** Civic location cfg*/
	civic_loc_tlv_t civic_tlv;

} __ATTRIB_PACK__ dot11mc_ftm_cfg_t;

/** Structure for DOT11AZ FTM_SESSION_CFG */
typedef struct _dot11az_ftmcfg_ntb_t {
	/** NTB session cfg */
	ntb_ranging_cfg_tlv_t ntb_tlv;
} __ATTRIB_PACK__ dot11az_ftm_cfg_t;

/** Type definition for hostcmd_ftm_session_cfg */
typedef struct _hostcmd_ftm_session_cfg {
	/** 0:Get, 1:Set */
	t_u16 action;
	/** FTM_SESSION_CFG_TLVs*/
	union {
		/**11az cfg*/
		dot11az_ftm_cfg_t cfg_11az;
		/** 11mc cfg*/
		dot11mc_ftm_cfg_t cfg_11mc;
	} tlv;
} __ATTRIB_PACK__ hostcmd_ftm_session_cfg;

/** Type definition for hostcmd_ftm_session_ctrl */
typedef struct _hostcmd_ftm_session_ctrl {
	/** 0: Not used, 1: Start, 2: Stop*/
	t_u16 action;
	/*FTM for ranging*/
	t_u8 for_ranging;
	/** Mac address of the peer with whom FTM session is required*/
	t_u8 peer_mac[ETH_ALEN];
	/** Channel on which FTM must be started */
	t_u8 chan;
} __ATTRIB_PACK__ hostcmd_ftm_session_ctrl;

/** Type definition for generic Hostcmd for 11AZ FTM Session */
typedef struct _hostcmd_ds_ftm_session_cmd {
	/** HostCmd_DS_GEN */
	HostCmd_DS_GEN cmd_hdr;
	/** Command Body */
	union {
		/** hostcmd for session_ctrl user command */
		hostcmd_ftm_session_ctrl ftm_session_ctrl;
		/** hostcmd for session_cfg user command */
		hostcmd_ftm_session_cfg ftm_session_cfg;
	} cmd;
} __ATTRIB_PACK__ hostcmd_ds_ftm_session_cmd;

/** Type definition for FTM Session Events */

/** Event ID length */
#define EVENT_ID_LEN 4

/**Structure for RTT results subevent*/
typedef struct _wls_subevent_rtt_results_t {
	/** complete */
	t_u8 complete;
	/** tlv buffer */
	/** MrvlIEtypes_RTTResult_t */
	t_u8 tlv_buffer[];
} __ATTRIB_PACK__ wls_subevent_rtt_results_t;

/**Structure for FTM complete subevent*/
typedef struct _wls_subevent_ftm_complete {
	/** BSS Number */
	t_u8 bssNum;
	/** BSS Type */
	t_u8 bssType;
	/** MAC address of the responder */
	t_u8 mac[ETH_ALEN];
	/** Average RTT */
	t_u32 avg_rtt;
	/** Average Clock offset */
	t_u32 avg_clk_offset;
	/** Measure start timestamp */
	t_u32 meas_start_tsf;
} __ATTRIB_PACK__ wls_subevent_ftm_complete_t;

/** TLV for FTM Range Report */
typedef struct _range_report_tlv_t {
	/**Type*/
	t_u16 type;
	/**Length*/
	t_u16 len;
	/** MAC address of the responder */
	t_u8 mac[ETH_ALEN];
	/** Average RTT */
	t_u32 avg_rtt;
	/** Average Clock offset */
	t_u32 avg_clk_offset;
	/** LCI and Location Civic TLV */
} __ATTRIB_PACK__ range_report_tlv_t;

/** Structure for FTM events*/
typedef struct _wls_event_t {
	/** Event ID */
	t_u16 event_id;
	/** BSS index number for multiple BSS support */
	t_u8 bss_index;
	/** BSS type */
	t_u8 bss_type;
	/** sub event id */
	t_u8 sub_event_id;
	union {
		/** FTM Complete Sub event*/
		wls_subevent_ftm_complete_t ftm_complete;
	} e;
} __ATTRIB_PACK__ wls_event_t;

/*Application Global Data*/
typedef struct {
	/** Average RTT */
	t_u32 avg_rtt;
	/** Average Clock offset */
	t_u32 avg_clk_offset;
	/*Range*/
	t_s64 range;
} range_results_t;

/** Structure for ftm command private data*/
typedef struct _wls_app_data {
	/** 0 : 80211mc, 1:80211az*/
	t_u8 protocol_type;
	/** num of times to run FTM*/
	t_u8 loop_cnt;
	/** flag to run nonstop*/
	t_u8 run_nonstop;
	/** flag is associated */
	t_u8 associated;
	/** 0 - STA, 1- AP*/
	t_u8 bss_type;
	/**flag for ftm started */
	t_u8 ftm_started;
	/** flag for app to terminate ftm session*/
	t_u8 terminate_app;
	/**flag for debug print level */
	t_u8 debug_level;
	/**peer mac address */
	t_u8 peer_mac[ETH_ALEN];
	/**AP mac address */
	t_u8 ap_mac[ETH_ALEN];
	/** Channel number for FTM session*/
	t_u8 channel;
	/**SET/GET action */
	t_u8 hostcmd_action;
	/**Is LCI data available in cfg*/
	t_u8 lci_request;
	/** Is civic data available in cfg*/
	t_u8 civic_request;
	/**ntb cfg param*/
	ntb_ranging_cfg_t ntb_cfg;
	/** 11mc session cfg param*/
	ftm_session_cfg_t session_cfg;
	/** lci cfg data*/
	lci_cfg_t lci_cfg;
	/** civic cfg data - this should be last field*/
	civic_loc_cfg_t civic_cfg;
} __ATTRIB_PACK__ wls_app_data_t;

int mlanwls_main(int argc, char *argv[]);
#endif /* _WLS_H_ */
