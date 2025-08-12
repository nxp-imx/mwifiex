/** @file mlan_hostcmd_ids.h
 *
 *  @brief This file contains host command ID definitions.
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

/** Firmware Host Command ID Constants */
/** Host Command ID : Get hardware specifications */
ENUM_ELEMENT(HostCmd_CMD_GET_HW_SPEC, 0x0003),
	/** Host Command ID : 802.11 scan */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SCAN, 0x0006),
	/** Host Command ID : 802.11 get log */
	ENUM_ELEMENT(HostCmd_CMD_802_11_GET_LOG, 0x000b),

	/** Host Command id: GET_TX_RX_PKT_STATS */
	ENUM_ELEMENT(HostCmd_CMD_TX_RX_PKT_STATS, 0x008d),

	/** Host Command ID : 802.11 get/set link layer statistic */
	ENUM_ELEMENT(HostCmd_CMD_802_11_LINK_STATS, 0x0256),

	/** Host Command ID : MAC multicast address */
	ENUM_ELEMENT(HostCmd_CMD_MAC_MULTICAST_ADR, 0x0010),
	/** Host Command ID : 802.11 EEPROM access */
	ENUM_ELEMENT(HostCmd_CMD_802_11_EEPROM_ACCESS, 0x0059),
	/** Host Command ID : 802.11 associate */
	ENUM_ELEMENT(HostCmd_CMD_802_11_ASSOCIATE, 0x0012),

	/** Host Command ID : 802.11 SNMP MIB */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SNMP_MIB, 0x0016),
	/** Host Command ID : MAC register access */
	ENUM_ELEMENT(HostCmd_CMD_MAC_REG_ACCESS, 0x0019),
	/** Host Command ID : BBP register access */
	ENUM_ELEMENT(HostCmd_CMD_BBP_REG_ACCESS, 0x001a),
	/** Host Command ID : RF register access */
	ENUM_ELEMENT(HostCmd_CMD_RF_REG_ACCESS, 0x001b),

	/** Host Command ID : 802.11 radio control */
	ENUM_ELEMENT(HostCmd_CMD_802_11_RADIO_CONTROL, 0x001c),
	/** Host Command ID : 802.11 RF channel */
	ENUM_ELEMENT(HostCmd_CMD_802_11_RF_CHANNEL, 0x001d),
	/** Host Command ID : 802.11 RF Tx power */
	ENUM_ELEMENT(HostCmd_CMD_802_11_RF_TX_POWER, 0x001e),

	/** Host Command ID : 802.11 RF antenna */
	ENUM_ELEMENT(HostCmd_CMD_802_11_RF_ANTENNA, 0x0020),

	/** Host Command ID : 802.11 deauthenticate */
	ENUM_ELEMENT(HostCmd_CMD_802_11_DEAUTHENTICATE, 0x0024),
	/** Host Command ID: 802.11 disassoicate */
	ENUM_ELEMENT(HostCmd_CMD_802_11_DISASSOCIATE, 0x0026),
	/** Host Command ID : MAC control */
	ENUM_ELEMENT(HostCmd_CMD_MAC_CONTROL, 0x0028),

	/** Host Command ID : 802.11 key material */
	ENUM_ELEMENT(HostCmd_CMD_802_11_KEY_MATERIAL, 0x005e),

	/** Host Command ID : GPIO Configuration */
	ENUM_ELEMENT(HostCmd_CMD_GPIO_CFG, 0x0041),

	/** Host Command ID : 802.22 MAC address */
	ENUM_ELEMENT(HostCmd_CMD_802_11_MAC_ADDRESS, 0x004D),

	/** Host Command ID : LED control config  */
	ENUM_ELEMENT(HostCmd_CMD_802_11_LED_CONTROL, 0x004E),

	/** Host Command ID : WMM Traffic Stream Status */
	ENUM_ELEMENT(HostCmd_CMD_WMM_TS_STATUS, 0x005d),

	/** Host Command ID : 802.11 D domain information */
	ENUM_ELEMENT(HostCmd_CMD_802_11D_DOMAIN_INFO, 0x005b),

	/*This command gets/sets the Transmit Rate-based Power Control (TRPC)
	 * channel configuration.*/
	ENUM_ELEMENT(HostCmd_CMD_CHANNEL_TRPC_CONFIG, 0x00fb),

	/** Host Command ID : 802.11 TPC information */
	ENUM_ELEMENT(HostCmd_CMD_802_11_TPC_INFO, 0x005f),
	/** Host Command ID : 802.11 TPC adapt req */
	ENUM_ELEMENT(HostCmd_CMD_802_11_TPC_ADAPT_REQ, 0x0060),
	/** Host Command ID : 802.11 channel SW ann */
	ENUM_ELEMENT(HostCmd_CMD_802_11_CHAN_SW_ANN, 0x0061),

	/** Host Command ID : Measurement request */
	ENUM_ELEMENT(HostCmd_CMD_MEASUREMENT_REQUEST, 0x0062),
	/** Host Command ID : Measurement report */
	ENUM_ELEMENT(HostCmd_CMD_MEASUREMENT_REPORT, 0x0063),

	/** Host Command ID : 802.11 sleep parameters */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SLEEP_PARAMS, 0x0066),

	/** Host Command ID : 802.11 ps inactivity timeout */
	ENUM_ELEMENT(HostCmd_CMD_802_11_PS_INACTIVITY_TIMEOUT, 0x0067),

	/** Host Command ID : 802.11 sleep period */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SLEEP_PERIOD, 0x0068),

	/** Host Command ID: 802.11 BG scan config */
	ENUM_ELEMENT(HostCmd_CMD_802_11_BG_SCAN_CONFIG, 0x006b),
	/** Host Command ID : 802.11 BG scan query */
	ENUM_ELEMENT(HostCmd_CMD_802_11_BG_SCAN_QUERY, 0x006c),

	/** Host Command ID : WMM ADDTS req */
	ENUM_ELEMENT(HostCmd_CMD_WMM_ADDTS_REQ, 0x006E),
	/** Host Command ID : WMM DELTS req */
	ENUM_ELEMENT(HostCmd_CMD_WMM_DELTS_REQ, 0x006F),
	/** Host Command ID : WMM queue configuration */
	ENUM_ELEMENT(HostCmd_CMD_WMM_QUEUE_CONFIG, 0x0070),
	/** Host Command ID : 802.11 get status */
	ENUM_ELEMENT(HostCmd_CMD_WMM_GET_STATUS, 0x0071),

	/** Host Command ID : 802.11 subscribe event */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SUBSCRIBE_EVENT, 0x0075),

	/** Host Command ID : 802.11 Tx rate query */
	ENUM_ELEMENT(HostCmd_CMD_802_11_TX_RATE_QUERY, 0x007f),
	/** Host Command ID :Get timestamp value */
	ENUM_ELEMENT(HostCmd_CMD_GET_TSF, 0x0080),

	/** Host Command ID : WMM queue stats */
	ENUM_ELEMENT(HostCmd_CMD_WMM_QUEUE_STATS, 0x0081),

	/** Host Command ID : KEEP ALIVE command */
	ENUM_ELEMENT(HostCmd_CMD_AUTO_TX, 0x0082),

	/** Host Command ID : Memory access */
	ENUM_ELEMENT(HostCmd_CMD_MEM_ACCESS, 0x0086),

#if defined(SDIO)
	/** Host Command ID : SDIO GPIO interrupt configuration */
	ENUM_ELEMENT(HostCmd_CMD_SDIO_GPIO_INT_CONFIG, 0x0088),
#endif

	/** Host Command ID : Mfg command */
	ENUM_ELEMENT(HostCmd_CMD_MFG_COMMAND, 0x0089),
	/** Host Command ID : Inactivity timeout ext */
	ENUM_ELEMENT(HostCmd_CMD_INACTIVITY_TIMEOUT_EXT, 0x008a),

	/** Host Command ID : DBGS configuration */
	ENUM_ELEMENT(HostCmd_CMD_DBGS_CFG, 0x008b),
	/** Host Command ID : Get memory */
	ENUM_ELEMENT(HostCmd_CMD_GET_MEM, 0x008c),

	/** Host Command ID : Cal data dnld */
	ENUM_ELEMENT(HostCmd_CMD_CFG_DATA, 0x008f),

	/** Host Command ID : SDIO pull control */
	ENUM_ELEMENT(HostCmd_CMD_SDIO_PULL_CTRL, 0x0093),

	/** Host Command ID : ECL system clock configuration */
	ENUM_ELEMENT(HostCmd_CMD_ECL_SYSTEM_CLOCK_CONFIG, 0x0094),

	/** Host Command ID : Extended version */
	ENUM_ELEMENT(HostCmd_CMD_VERSION_EXT, 0x0097),

	/** Host Command ID : MEF configuration */
	ENUM_ELEMENT(HostCmd_CMD_MEF_CFG, 0x009a),
	/** Host Command ID : 802.11 RSSI INFO*/
	ENUM_ELEMENT(HostCmd_CMD_RSSI_INFO, 0x00a4),
	/** Host Command ID : Function initialization */
	ENUM_ELEMENT(HostCmd_CMD_FUNC_INIT, 0x00a9),
	/** Host Command ID : Function shutdown */
	ENUM_ELEMENT(HostCmd_CMD_FUNC_SHUTDOWN, 0x00aa),

#ifdef OPCHAN
	ENUM_ELEMENT(HostCmd_CMD_OPCHAN_CONFIG, 0x00f8),
	ENUM_ELEMENT(HostCmd_CMD_OPCHAN_CHANGROUP_CONFIG, 0x00f9),
#endif

	/** Host Command ID : Robustcoex */
	ENUM_ELEMENT(HostCmd_CMD_802_11_ROBUSTCOEX, 0x00e0),

	/** Host Command ID :EAPOL PKT */
	ENUM_ELEMENT(HostCmd_CMD_802_11_EAPOL_PKT, 0x012e),

	ENUM_ELEMENT(HostCmd_CMD_EDMAC_CFG, 0x0130),

	/** Host Command ID :MIMO SWITCH **/
	ENUM_ELEMENT(HostCmd_CMD_802_11_MIMO_SWITCH, 0x0235),

	/** Host Command ID : 802.11 RSSI INFO EXT*/
	ENUM_ELEMENT(HostCmd_CMD_RSSI_INFO_EXT, 0x0237),
	/** Host Command ID : ROAMING OFFLOAD TO FW*/
	ENUM_ELEMENT(HostCmd_CMD_ROAM_OFFLOAD, 0x0245),

	/** Host Command ID: Multi chan config */
	ENUM_ELEMENT(HostCmd_CMD_MULTI_CHAN_CONFIG, 0x011e),
	/** Host Command ID: Multi chan policy */
	ENUM_ELEMENT(HostCmd_CMD_MULTI_CHAN_POLICY, 0x0121),
	/** Host Command ID: DRCS config */
	ENUM_ELEMENT(HostCmd_CMD_DRCS_CONFIG, 0x024a),

	/** Host Command ID : Channel report request */
	ENUM_ELEMENT(HostCmd_CMD_CHAN_REPORT_REQUEST, 0x00dd),

	/** Host Command ID: SUPPLICANT_PMK */
	ENUM_ELEMENT(HostCmd_CMD_SUPPLICANT_PMK, 0x00c4),
	/** Host Command ID: SUPPLICANT_PROFILE */
	ENUM_ELEMENT(HostCmd_CMD_SUPPLICANT_PROFILE, 0x00c5),

	/** Host Command ID : Add Block Ack Request */
	ENUM_ELEMENT(HostCmd_CMD_11N_ADDBA_REQ, 0x00ce),
	/** Host Command ID : Delete a Block Ack Request */
	ENUM_ELEMENT(HostCmd_CMD_11N_CFG, 0x00cd),
	/** Host Command ID : Add Block Ack Response */
	ENUM_ELEMENT(HostCmd_CMD_11N_ADDBA_RSP, 0x00cf),
	/** Host Command ID : Delete a Block Ack Request */
	ENUM_ELEMENT(HostCmd_CMD_11N_DELBA, 0x00d0),
	/** Host Command ID: Configure Tx Buf size */
	ENUM_ELEMENT(HostCmd_CMD_RECONFIGURE_TX_BUFF, 0x00d9),
	/** Host Command ID: AMSDU Aggr Ctrl */
	ENUM_ELEMENT(HostCmd_CMD_AMSDU_AGGR_CTRL, 0x00df),
	/** Host Command ID: 11AC config */
	ENUM_ELEMENT(HostCmd_CMD_11AC_CFG, 0x0112),
	/** Host Command ID: Configure TX Beamforming capability */
	ENUM_ELEMENT(HostCmd_CMD_TX_BF_CFG, 0x0104),

	/** Host Command ID : 802.11 TX power configuration */
	ENUM_ELEMENT(HostCmd_CMD_TXPWR_CFG, 0x00d1),

	/** Host Command ID : Soft Reset */
	ENUM_ELEMENT(HostCmd_CMD_SOFT_RESET, 0x00d5),

	/** Host Command ID : 802.11 b/g/n rate configration */
	ENUM_ELEMENT(HostCmd_CMD_TX_RATE_CFG, 0x00d6),

	/** Host Command ID : Enhanced PS mode */
	ENUM_ELEMENT(HostCmd_CMD_802_11_PS_MODE_ENH, 0x00e4),

	/** Host command action : Host sleep configuration */
	ENUM_ELEMENT(HostCmd_CMD_802_11_HS_CFG_ENH, 0x00e5),

	/** Host Command ID : CAU register access */
	ENUM_ELEMENT(HostCmd_CMD_CAU_REG_ACCESS, 0x00ed),

	/** Host Command ID : mgmt IE list */
	ENUM_ELEMENT(HostCmd_CMD_MGMT_IE_LIST, 0x00f2),

	ENUM_ELEMENT(HostCmd_CMD_802_11_BAND_STEERING, 0x026f),
	/*** Host Command ID " MC_AGGR_CFG */
	ENUM_ELEMENT(HostCmd_CMD_MC_AGGR_CFG, 0x027a),
	ENUM_ELEMENT(HostCmd_CMD_802_11_STATS, 0x0014),
	ENUM_ELEMENT(HostCmd_CMD_GET_CH_LOAD, 0x027b),

	/** Host Command ID: CROSS CHIP SYNCH */
	ENUM_ELEMENT(HostCmd_CMD_CROSS_CHIP_SYNCH, 0x027d),

	/** Host Command ID : TDLS configuration */
	ENUM_ELEMENT(HostCmd_CMD_TDLS_CONFIG, 0x0100),
	/** Host Command ID : TDLS operation */
	ENUM_ELEMENT(HostCmd_CMD_TDLS_OPERATION, 0x0122),

	/** Host Command ID : Packet aggregation over host interface */
	ENUM_ELEMENT(HostCmd_CMD_PACKET_AGGR_OVER_HOST_INTERFACE, 0x0117),

#ifdef SDIO
	/** Host Command ID : SDIO single port RX aggr */
	ENUM_ELEMENT(HostCmd_CMD_SDIO_SP_RX_AGGR_CFG, 0x0223),
#endif

	/** Host Command ID : Extended scan support */
	ENUM_ELEMENT(HostCmd_CMD_802_11_SCAN_EXT, 0x0107),

	/** Host Command ID : Forward mgmt frame */
	ENUM_ELEMENT(HostCmd_CMD_RX_MGMT_IND, 0x010c),

#ifdef PCIE
	/** Host Command ID: Host buffer description */
	ENUM_ELEMENT(HostCmd_CMD_PCIE_HOST_BUF_DETAILS, 0x00fa),
#endif

	/** Host Command ID : Set BSS_MODE */
	ENUM_ELEMENT(HostCmd_CMD_SET_BSS_MODE, 0x00f7),

#ifdef UAP_SUPPORT
	/**  Host Command id: SYS_INFO */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_SYS_INFO, 0x00ae),
	/** Host Command id: sys_reset */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_SYS_RESET, 0x00af),
	/** Host Command id: SYS_CONFIGURE  */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_SYS_CONFIGURE, 0x00b0),
	/** Host Command id: BSS_START */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_BSS_START, 0x00b1),
	/** Host Command id: BSS_STOP  */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_BSS_STOP, 0x00b2),
	/** Host Command id: sta_list */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_STA_LIST, 0x00b3),
	/** Host Command id: STA_DEAUTH */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_STA_DEAUTH, 0x00b5),

	/** Host Command id: REPORT_MIC */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_REPORT_MIC, 0x00ee),
	/** Host Command id: UAP_OPER_CTRL */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_OPER_CTRL, 0x0233),
#endif /* UAP_SUPPORT */

	/** Host Command id: PMIC CONFIGURE*/
	ENUM_ELEMENT(HostCmd_CMD_PMIC_CONFIGURE, 0x23E),

	/** Host Command ID: 802.11 Network Monitor */
	ENUM_ELEMENT(HostCmd_CMD_802_11_NET_MONITOR, 0x0102),

	/** Host Command ID: Tx data pause */
	ENUM_ELEMENT(HostCmd_CMD_CFG_TX_DATA_PAUSE, 0x0103),

#ifdef WIFI_DIRECT_SUPPORT
	/** Host Command ID: P2P PARAMS CONFIG */
	ENUM_ELEMENT(HostCmd_CMD_P2P_PARAMS_CONFIG, 0x00ea),
	/** Host Command ID: WIFI_DIRECT_MODE_CONFIG */
	ENUM_ELEMENT(HostCmd_CMD_WIFI_DIRECT_MODE_CONFIG, 0x00eb),
#endif

	/** Host Command ID: GPIO TSF LATCH */
	ENUM_ELEMENT(HostCmd_CMD_GPIO_TSF_LATCH_PARAM_CONFIG, 0x0278),
	/** Host Command ID: Remain On Channel */
	ENUM_ELEMENT(HostCmd_CMD_802_11_REMAIN_ON_CHANNEL, 0x010d),

	ENUM_ELEMENT(HostCmd_CMD_COALESCE_CFG, 0x010a),

	/** Host Command ID: GTK REKEY OFFLOAD CFG */
	ENUM_ELEMENT(HostCmd_CMD_GTK_REKEY_OFFLOAD_CFG, 0x010f),

	/** Host Command ID : OTP user data */
	ENUM_ELEMENT(HostCmd_CMD_OTP_READ_USER_DATA, 0x0114),

	/** Host Command ID: fw auto reconnect */
	ENUM_ELEMENT(HostCmd_CMD_FW_AUTO_RECONNECT, 0x0115),

	/** Host Command ID: HS wakeup reason */
	ENUM_ELEMENT(HostCmd_CMD_HS_WAKEUP_REASON, 0x0116),

	/** Host Command ID: reject addba request */
	ENUM_ELEMENT(HostCmd_CMD_REJECT_ADDBA_REQ, 0x0119),

	ENUM_ELEMENT(HostCmd_CMD_FW_DUMP_EVENT, 0x0125),

	ENUM_ELEMENT(HostCmd_CMD_CONFIG_LOW_POWER_MODE, 0x0128),

	/** Host Command ID : Target device access */
	ENUM_ELEMENT(HostCmd_CMD_TARGET_ACCESS, 0x012a),

	/** Host Command ID: BCA device access */
	ENUM_ELEMENT(HostCmd_CMD_BCA_REG_ACCESS, 0x0272),
	/** Host Command ID: register device access */
	ENUM_ELEMENT(HostCmd_CMD_REG_ACCESS, 0x027C),

	/** Host Command ID: DFS repeater mode */
	ENUM_ELEMENT(HostCmd_CMD_DFS_REPEATER_MODE, 0x012b),

	/** Host Command ID: ACS scan */
	ENUM_ELEMENT(HostCmd_CMD_APCMD_ACS_SCAN, 0x0224),

	/** Host Command ID: Get sensor temp*/
	ENUM_ELEMENT(HostCmd_CMD_DS_GET_SENSOR_TEMP, 0x0227),

	/** Host Command ID: NAN Parameters cofiguration */
	ENUM_ELEMENT(HostCmd_CMD_NAN_PARAMS_CONFIG, 0x0228),
	/** Host Command ID: NAN mode cofiguration */
	ENUM_ELEMENT(HostCmd_CMD_NAN_MODE_CONFIG, 0x0229),
	/** Host Command ID: NAN service discovery functionality */
	ENUM_ELEMENT(HostCmd_CMD_NAN_SDF, 0x022a),
	/** Host Command ID: NAN service hash */
	ENUM_ELEMENT(HostCmd_CMD_NAN_SERVICE_HASH, 0x022b),
	/** Host Command ID: NAN service state info (get/set) */
	ENUM_ELEMENT(HostCmd_CMD_NAN_STATE_INFO, 0x022c),

	/** Host Command ID : Configure ADHOC_OVER_IP parameters */
	ENUM_ELEMENT(HostCmd_CMD_WMM_PARAM_CONFIG, 0x023a),

	ENUM_ELEMENT(HostCmd_CMD_IPV6_RA_OFFLOAD_CFG, 0x0238),

#ifdef STA_SUPPORT
	/** Host Command ID :  set/get sta configure */
	ENUM_ELEMENT(HostCmd_CMD_STA_CONFIGURE, 0x023f),
#endif

	/** Host Command ID : GPIO independent reset configure */
	ENUM_ELEMENT(HostCmd_CMD_INDEPENDENT_RESET_CFG, 0x0243),

	/** use to query chan region cfg setting in firmware */
	ENUM_ELEMENT(HostCmd_CMD_CHAN_REGION_CFG, 0x0242),
	/** used in hostcmd to download region power cfg setting to firmware */
	ENUM_ELEMENT(HostCmd_CMD_REGION_POWER_CFG, 0x0249),

	/** Host Command ID : Packet aggregation CTRL */
	ENUM_ELEMENT(HostCmd_CMD_PACKET_AGGR_CTRL, 0x0251),

	ENUM_ELEMENT(HostCmd_CMD_DYN_BW, 0x0252),

	ENUM_ELEMENT(HostCmd_CMD_BOOT_SLEEP, 0x0258),

	ENUM_ELEMENT(HostCmd_CMD_RX_ABORT_CFG, 0x0261),
	ENUM_ELEMENT(HostCmd_CMD_RX_ABORT_CFG_EXT, 0x0262),
	ENUM_ELEMENT(HostCmd_CMD_TX_AMPDU_PROT_MODE, 0x0263),
	ENUM_ELEMENT(HostCmd_CMD_RATE_ADAPT_CFG, 0x0264),
	ENUM_ELEMENT(HostCmd_CMD_CCK_DESENSE_CFG, 0x0265),

	ENUM_ELEMENT(HostCmd_CMD_FTM_CONFIG_SESSION_PARAMS, 0x024d),
	ENUM_ELEMENT(HostCmd_CMD_FTM_SESSION_CTRL, 0x024e),
	ENUM_ELEMENT(HostCmd_CMD_FTM_FEATURE_CTRL, 0x024f),
	ENUM_ELEMENT(HostCmd_CMD_WLS_REQ_FTM_RANGE, 0x0250),

	ENUM_ELEMENT(HostCmd_CMD_FTM_CONFIG_RESPONDER, 0x0255),

	ENUM_ELEMENT(HostCmd_CMD_VDLL, 0x0240),
#if defined(PCIE)
	ENUM_ELEMENT(HostCmd_CMD_SSU, 0x0259),
#endif
	ENUM_ELEMENT(HostCmd_CMD_CSI, 0x025b),

#if defined(UAP_SUPPORT)
	/** Host Command ID : Add New Station */
	ENUM_ELEMENT(HostCmd_CMD_ADD_NEW_STATION, 0x025f),
#endif

	ENUM_ELEMENT(HostCmd_CMD_DMCS_CONFIG, 0x0260),

	/** Host Command ID: 11AX config */
	ENUM_ELEMENT(HostCmd_CMD_11AX_CFG, 0x0266),
	/** Host Command ID: 11AX command */
	ENUM_ELEMENT(HostCmd_CMD_11AX_CMD, 0x026d),
	/** Host Command ID: Range ext command */
	ENUM_ELEMENT(HostCmd_CMD_RANGE_EXT, 0x0274),
	/** Host Command ID: TWT cfg command */
	ENUM_ELEMENT(HostCmd_CMD_TWT_CFG, 0x0270),

	ENUM_ELEMENT(HostCmd_CMD_LOW_POWER_MODE_CFG, 0x026e),
	ENUM_ELEMENT(HostCmd_CMD_UAP_BEACON_STUCK_CFG, 0x0271),
	ENUM_ELEMENT(HostCmd_CMD_ARB_CONFIG, 0x0273),
	ENUM_ELEMENT(HostCmd_CMD_DOT11MC_UNASSOC_FTM_CFG, 0x0275),
	ENUM_ELEMENT(HostCmd_CMD_HAL_PHY_CFG, 0x0276),

	/** Host Command ID : IPS Config */
	ENUM_ELEMENT(HostCmd_CMD_IPS_CONFIG, 0x0279),
	ENUM_ELEMENT(HostCmd_CMD_OFDM_DESENSE_CFG, 0x027f),
	ENUM_ELEMENT(HostCmd_CMD_NAV_MITIGATION_CFG, 0x0282),

	/** Host Command ID: Tx Frame */
	ENUM_ELEMENT(HostCmd_CMD_802_11_TX_FRAME, 0x0283),
	/** Host Command ID: PCIE ADMA INIT */
	ENUM_ELEMENT(HostCmd_CMD_PCIE_ADMA_INIT, 0x0284),

	/* Always keep this last */
	ENUM_ELEMENT_LAST(__HostCmd_CMD_Last)
