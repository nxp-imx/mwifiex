/** @file  mlanutl.c
  *
  * @brief Program to control parameters in the mlandriver
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
     11/04/2011: initial version
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <sys/stat.h>
#include <net/ethernet.h>

#include    "mlanutl.h"

/** Supported stream modes */
#define HT_STREAM_MODE_1X1   0x11
#define HT_STREAM_MODE_2X2   0x22

/** mlanutl version number */
#define MLANUTL_VER "M1.3"

/** Initial number of total private ioctl calls */
#define IW_INIT_PRIV_NUM    128
/** Maximum number of total private ioctl calls supported */
#define IW_MAX_PRIV_NUM     1024

/** Termination flag */
int terminate_flag = 0;

typedef struct {
	t_u8 chanNum;	  /**< Channel Number */
	t_u8 chanLoad;	  /**< Channel Load fraction */
	t_s16 anpi;	  /**< Channel ANPI */

} ChanRptInfo_t;

/********************************************************
			Local Variables
********************************************************/
#define BAND_B       (1U << 0)
#define BAND_G       (1U << 1)
#define BAND_A       (1U << 2)
#define BAND_GN      (1U << 3)
#define BAND_AN      (1U << 4)
#define BAND_GAC     (1U << 5)
#define BAND_AAC     (1U << 6)
#define BAND_GAX     (1U << 8)
#define BAND_AAX     (1U << 9)
static char *band[] = {
	"B",
	"G",
	"A",
	"GN",
	"AN",
	"GAC",
	"AAC",
	"11P",
	"GAX",
	"AAX",
};

/** Stringification of rateId enumeration */
const char *rateIdStr[] = { "1", "2", "5.5", "11", "--",
	"6", "9", "12", "18", "24", "36", "48", "54", "--",
	"M0", "M1", "M2", "M3", "M4", "M5", "M6", "M7",
	"H0", "H1", "H2", "H3", "H4", "H5", "H6", "H7"
};

#ifdef DEBUG_LEVEL1
#define MMSG        MBIT(0)
#define MFATAL      MBIT(1)
#define MERROR      MBIT(2)
#define MDATA       MBIT(3)
#define MCMND       MBIT(4)
#define MEVENT      MBIT(5)
#define MINTR       MBIT(6)
#define MIOCTL      MBIT(7)

#define MREG_D      MBIT(9)

#define MMPA_D      MBIT(15)
#define MDAT_D      MBIT(16)
#define MCMD_D      MBIT(17)
#define MEVT_D      MBIT(18)
#define MFW_D       MBIT(19)
#define MIF_D       MBIT(20)

#define MENTRY      MBIT(28)
#define MWARN       MBIT(29)
#define MINFO       MBIT(30)
#define MHEX_DUMP   MBIT(31)
#endif
static int process_dot11_txrx(int argc, char *argv[]);
#ifdef RX_PACKET_COALESCE
static int process_rx_pkt_coalesce_cfg(int argc, char *argv[]);
static void print_rx_packet_coalesc_help(void);
#endif
static int process_version(int argc, char *argv[]);
static int process_bandcfg(int argc, char *argv[]);
static int process_hostcmd(int argc, char *argv[]);
static int process_httxcfg(int argc, char *argv[]);
static int process_htcapinfo(int argc, char *argv[]);
static int process_addbapara(int argc, char *argv[]);
static int process_aggrpriotbl(int argc, char *argv[]);
static int process_addbareject(int argc, char *argv[]);
static int process_delba(int argc, char *argv[]);
static int process_rejectaddbareq(int argc, char *argv[]);
static int process_vhtcfg(int argc, char *argv[]);
static int process_opermodecfg(int argc, char *argv[]);
static int process_datarate(int argc, char *argv[]);
static int process_txratecfg(int argc, char *argv[]);
static int process_getlog(int argc, char *argv[]);
static int process_getcfgchanlist(int argc, char *argv[]);
static int process_esuppmode(int argc, char *argv[]);
static int process_passphrase(int argc, char *argv[]);
static int process_deauth(int argc, char *argv[]);
#ifdef UAP_SUPPORT
static int process_getstalist(int argc, char *argv[]);
#endif
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
static int process_bssrole(int argc, char *argv[]);
#endif
#endif
#ifdef STA_SUPPORT
static int process_setuserscan(int argc, char *argv[]);
static int wlan_process_getscantable(int argc, char *argv[],
				     wlan_ioctl_user_scan_cfg *scan_req);
static int wlan_process_getchanstats(int argc, char *argv[]);
static int process_getchanstats(int argc, char *argv[]);
static int process_getscantable(int argc, char *argv[]);
static int process_extcapcfg(int argc, char *argv[]);
static int process_cancelscan(int argc, char *argv[]);
#endif
static int process_deepsleep(int argc, char *argv[]);
static int process_ipaddr(int argc, char *argv[]);
static int process_otpuserdata(int argc, char *argv[]);
static int process_countrycode(int argc, char *argv[]);
static int process_tcpackenh(int argc, char *argv[]);
#ifdef REASSOCIATION
static int process_assocessid(int argc, char *argv[]);
#endif
#ifdef STA_SUPPORT
static int process_listeninterval(int argc, char *argv[]);
static int process_psmode(int argc, char *argv[]);
#endif
#ifdef DEBUG_LEVEL1
static int process_drvdbg(int argc, char *argv[]);
#endif
static int process_hscfg(int argc, char *argv[]);
static int process_hssetpara(int argc, char *argv[]);
static int process_wakeupresaon(int argc, char *argv[]);
static int process_mgmtfilter(int argc, char *argv[]);
static int process_scancfg(int argc, char *argv[]);
static int process_aggrctrl(int argc, char *argv[]);
static int process_usbaggrctrl(int argc, char *argv[]);
static int process_warmreset(int argc, char *argv[]);
static int process_txpowercfg(int argc, char *argv[]);
static int process_pscfg(int argc, char *argv[]);
static int process_bcntimeoutcfg(int argc, char *argv[]);
static int process_sleeppd(int argc, char *argv[]);
static int process_txcontrol(int argc, char *argv[]);
static int process_dfs_offload_enable(int argc, char *argv[]);
static int process_customie(int argc, char *argv[]);
static int process_regrdwr(int argc, char *argv[]);
static int process_rdeeprom(int argc, char *argv[]);
static int process_memrdwr(int argc, char *argv[]);
#ifdef SDIO
static int process_sdcmd52rw(int argc, char *argv[]);
#endif
static int process_mefcfg(int argc, char *argv[]);
static int process_cfgdata(int argc, char *argv[]);
static int process_mgmtframetx(int argc, char *argv[]);
static int process_mgmt_frame_passthrough(int argc, char *argv[]);
static int process_hotspot_config(int argc, char *argv[]);
static int process_qconfig(int argc, char *argv[]);
static int process_addts(int argc, char *argv[]);
static int process_delts(int argc, char *argv[]);
static int process_wmm_qstatus(int argc, char *argv[]);
static int process_wmm_ts_status(int argc, char *argv[]);
static int process_qos_config(int argc, char *argv[]);
static int process_macctrl(int argc, char *argv[]);
static int process_fwmacaddr(int argc, char *argv[]);
static int process_regioncode(int argc, char *argv[]);
static int process_cfpinfo(int argc, char *argv[]);
static int process_offchannel(int argc, char *argv[]);
static int process_linkstats(int argc, char *argv[]);
#if defined(STA_SUPPORT)
static int process_pmfcfg(int argc, char *argv[]);
#endif
static int process_verext(int argc, char *argv[]);
static int process_usb_suspend(int argc, char *argv[]);
static int process_usb_resume(int argc, char *argv[]);
#if defined(STA_SUPPORT) && defined(STA_WEXT)
static int process_radio_ctrl(int argc, char *argv[]);
#endif
static int process_wmm_cfg(int argc, char *argv[]);
static int process_wmm_param_config(int argc, char *argv[]);
static int process_min_ba_threshold_cfg(int argc, char *argv[]);
#if defined(STA_SUPPORT)
static int process_11d_cfg(int argc, char *argv[]);
static int process_11d_clr_tbl(int argc, char *argv[]);
#endif
#ifndef OPCHAN
static int process_wws_cfg(int argc, char *argv[]);
#endif
#if defined(REASSOCIATION)
static int process_set_get_reassoc(int argc, char *argv[]);
#endif
static int process_txbuf_cfg(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_set_get_auth_type(int argc, char *argv[]);
#endif
static int process_11h_local_pwr_constraint(int argc, char *argv[]);
static int process_ht_stream_cfg(int argc, char *argv[]);
static int process_mimo_switch(int argc, char *argv[]);
static int process_thermal(int argc, char *argv[]);
static int process_beacon_interval(int argc, char *argv[]);
static int process_cwmode(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_get_signal(int argc, char *argv[]);
static int process_get_signal_ext(int argc, char *argv[]);
static int process_signalext_cfg(int argc, char *argv[]);
#endif
static int process_inactivity_timeout_ext(int argc, char *argv[]);
static int process_11n_amsdu_aggr_ctrl(int argc, char *argv[]);
static int process_tx_bf_cap_ioctl(int argc, char *argv[]);
#ifdef SDIO
static int process_sdio_clock_ioctl(int argc, char *argv[]);
#endif
#ifdef SDIO
static int process_sdio_mpa_ctrl(int argc, char *argv[]);
#endif
static int process_sleep_params(int argc, char *argv[]);
static int process_dfs_testing(int argc, char *argv[]);
static int process_cfp_code(int argc, char *argv[]);
static int process_set_get_tx_rx_ant(int argc, char *argv[]);
static int process_get_txpwrlimit(int argc, char *argv[]);
static int process_get_chnrgpwr(int argc, char *argv[]);
static int process_compare_rgpwr(int argc, char *argv[]);
static int process_comparetrpc(int argc, char *argv[]);
static int process_sysclock(int argc, char *argv[]);
static int process_get_key(int argc, char *argv[]);
static int process_associate_ssid_bssid(int argc, char *argv[]);
static int process_tx_bf_cfg(int argc, char *argv[]);
static int process_wps_cfg(int argc, char *argv[]);
static int process_port_ctrl(int argc, char *argv[]);
static int process_bypassed_packet(int argc, char *argv[]);
/* #ifdef FW_WAKEUP_METHOD */
static int process_fw_wakeup_method(int argc, char *argv[]);
/* #endif */
#ifdef SDIO
static int process_sdcmd53rw(int argc, char *argv[]);
#endif
#ifdef WIFI_DIRECT_SUPPORT
static int process_cfg_noa_opp_ps(int argc, char *argv[]);
#endif
static int process_dscpmap(int argc, char *argv[]);
#ifdef WIFI_DIRECT_SUPPORT
static int process_miracastcfg(int argc, char *argv[]);
#endif
static int process_coex_rx_winsize(int argc, char *argv[]);
static int process_dfs_repeater(int argc, char *argv[]);
#ifdef PCIE
static int process_pcie_reg_rw(int argc, char *argv[]);
static int process_pcie_bar0_reg_rw(int argc, char *argv[]);
#endif
static int process_get_sensor_temp(int argc, char *argv[]);
static int process_chan_graph(int argc, char *argv[]);
static int process_extend_channel_switch(int argc, char *argv[]);
static int process_auto_arp(int argc, char *argv[]);
static int process_txrxhistogram(int argc, char *argv[]);
static int process_per_pkt_cfg(int argc, char *argv[]);
static int process_ind_rst_cfg(int argc, char *argv[]);
static int process_cloud_keep_alive(int argc, char *argv[]);
int process_tsf(int argc, char *argv[]);
static int process_dyn_bw(int argc, char *argv[]);
static int process_robustcoex(int argc, char *argv[]);
static int process_dmcs(int argc, char *argv[]);
#if defined(PCIE)
static int process_ssu_cmd(int argc, char *argv[]);
#endif
static int process_ctrldeauth(int argc, char *argv[]);
static int process_bootsleep(int argc, char *argv[]);
static int process_11axcfg(int argc, char *argv[]);
static int process_11axcmdcfg(int argc, char *argv[]);
static int process_twt_setup(int argc, char *argv[]);
static int process_twt_teardown(int argc, char *argv[]);

static int process_rx_abort_cfg(int argc, char *argv[]);
static int process_rx_abort_cfg_ext(int argc, char *argv[]);
static int process_tx_ampdu_prot_mode(int argc, char *argv[]);
static int process_rate_adapt_cfg(int argc, char *argv[]);
static int process_cck_desense_cfg(int argc, char *argv[]);
static int process_lpm(int argc, char *argv[]);

struct command_node command_list[] = {
	{"version", process_version},
	{"bandcfg", process_bandcfg},
	{"hostcmd", process_hostcmd},
	{"httxcfg", process_httxcfg},
	{"htcapinfo", process_htcapinfo},
	{"addbapara", process_addbapara},
	{"aggrpriotbl", process_aggrpriotbl},
	{"addbareject", process_addbareject},
	{"delba", process_delba},
	{"rejectaddbareq", process_rejectaddbareq},
	{"vhtcfg", process_vhtcfg},
	{"opermodecfg", process_opermodecfg},
	{"getdatarate", process_datarate},
	{"txratecfg", process_txratecfg},
	{"getlog", process_getlog},
	{"esuppmode", process_esuppmode},
	{"passphrase", process_passphrase},
	{"deauth", process_deauth},
#ifdef UAP_SUPPORT
	{"getstalist", process_getstalist},
#endif
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
	{"bssrole", process_bssrole},
#endif
#endif
#ifdef STA_SUPPORT
	{"setuserscan", process_setuserscan},
	{"getscantable", process_getscantable},
	{"getchanstats", process_getchanstats},
	{"extcapcfg", process_extcapcfg},
	{"cancelscan", process_cancelscan},
#endif
	{"deepsleep", process_deepsleep},
	{"ipaddr", process_ipaddr},
	{"otpuserdata", process_otpuserdata},
	{"countrycode", process_countrycode},
	{"tcpackenh", process_tcpackenh},
#ifdef REASSOCIATION
	{"assocessid", process_assocessid},
	{"assocessid_bssid", process_assocessid},
#endif
#ifdef STA_SUPPORT
	{"listeninterval", process_listeninterval},
	{"psmode", process_psmode},
#endif
#ifdef DEBUG_LEVEL1
	{"drvdbg", process_drvdbg},
#endif
	{"hscfg", process_hscfg},
	{"hssetpara", process_hssetpara},
	{"wakeupreason", process_wakeupresaon},
	{"mgmtfilter", process_mgmtfilter},
	{"scancfg", process_scancfg},
	{"aggrctrl", process_aggrctrl},
	{"usbaggrctrl", process_usbaggrctrl},
	{"warmreset", process_warmreset},
	{"txpowercfg", process_txpowercfg},
	{"pscfg", process_pscfg},
	{"bcntimeoutcfg", process_bcntimeoutcfg},
	{"sleeppd", process_sleeppd},
	{"txcontrol", process_txcontrol},
	{"dfs_offload", process_dfs_offload_enable},
	{"customie", process_customie},
	{"regrdwr", process_regrdwr},
	{"rdeeprom", process_rdeeprom},
	{"memrdwr", process_memrdwr},
#ifdef SDIO
	{"sdcmd52rw", process_sdcmd52rw},
#endif
	{"mefcfg", process_mefcfg},
	{"cfgdata", process_cfgdata},
	{"mgmtframetx", process_mgmtframetx},
	{"mgmtframectrl", process_mgmt_frame_passthrough},
	{"hotspotcfg", process_hotspot_config},
	{"qconfig", process_qconfig},
	{"addts", process_addts},
	{"delts", process_delts},
	{"ts_status", process_wmm_ts_status},
	{"qstatus", process_wmm_qstatus},
	{"qoscfg", process_qos_config},
	{"macctrl", process_macctrl},
	{"fwmacaddr", process_fwmacaddr},
	{"regioncode", process_regioncode},
	{"cfpinfo", process_cfpinfo},
	{"offchannel", process_offchannel},
	{"linkstats", process_linkstats},
#if defined(STA_SUPPORT)
	{"pmfcfg", process_pmfcfg},
#endif
	{"verext", process_verext},
	{"usbsuspend", process_usb_suspend},
	{"usbresume", process_usb_resume},
#if defined(STA_SUPPORT) && defined(STA_WEXT)
	{"radioctrl", process_radio_ctrl},
#endif
	{"wmmcfg", process_wmm_cfg},
	{"wmmparamcfg", process_wmm_param_config},
	{"min_ba_threshold", process_min_ba_threshold_cfg},
#if defined(STA_SUPPORT)
	{"11dcfg", process_11d_cfg},
	{"11dclrtbl", process_11d_clr_tbl},
#endif
#ifndef OPCHAN
	{"wwscfg", process_wws_cfg},
#endif
#if defined(REASSOCIATION)
	{"reassoctrl", process_set_get_reassoc},
#endif
	{"txbufcfg", process_txbuf_cfg},
#ifdef STA_SUPPORT
	{"authtype", process_set_get_auth_type},
#endif
	{"powercons", process_11h_local_pwr_constraint},
	{"htstreamcfg", process_ht_stream_cfg},
	{"mimoswitch", process_mimo_switch},
	{"thermal", process_thermal},
	{"bcninterval", process_beacon_interval},
	{"cwmode", process_cwmode},
#ifdef STA_SUPPORT
	{"getsignal", process_get_signal},
	{"getsignalext", process_get_signal_ext},
	{"getsignalextv2", process_get_signal_ext},
	{"signalextcfg", process_signalext_cfg},
#endif
	{"inactivityto", process_inactivity_timeout_ext},
	{"amsduaggrctrl", process_11n_amsdu_aggr_ctrl},
	{"httxbfcap", process_tx_bf_cap_ioctl},
#ifdef SDIO
	{"sdioclock", process_sdio_clock_ioctl},
#endif
#ifdef SDIO
	{"mpactrl", process_sdio_mpa_ctrl},
#endif
	{"sleepparams", process_sleep_params},
	{"dfstesting", process_dfs_testing},
	{"cfpcode", process_cfp_code},
	{"antcfg", process_set_get_tx_rx_ant},
	{"get_chnrgpwr", process_get_chnrgpwr},
	{"comparergpwr", process_compare_rgpwr},
	{"get_txpwrlimit", process_get_txpwrlimit},
	{"getcfgchanlist", process_getcfgchanlist},
	{"comparetrpc", process_comparetrpc},
	{"dscpmap", process_dscpmap},
	{"changraph", process_chan_graph},
	{"getkey", process_get_key},
	{"associate", process_associate_ssid_bssid},
	{"httxbfcfg", process_tx_bf_cfg},
	{"wpssession", process_wps_cfg},
	{"port_ctrl", process_port_ctrl},
	{"pb_bypass", process_bypassed_packet},
/* #ifdef FW_WAKEUP_METHOD */
	{"fwwakeupmethod", process_fw_wakeup_method},
/* #endif */
	{"sysclock", process_sysclock},
#ifdef SDIO
	{"sdcmd53rw", process_sdcmd53rw},
#endif
#ifdef RX_PACKET_COALESCE
	{"rxpktcoal_cfg", process_rx_pkt_coalesce_cfg},
#endif
#ifdef WIFI_DIRECT_SUPPORT
	{"cfg_noa", process_cfg_noa_opp_ps},
	{"cfg_opp_ps", process_cfg_noa_opp_ps},
#endif
#ifdef WIFI_DIRECT_SUPPORT
	{"miracastcfg", process_miracastcfg},
#endif
	{"coex_rx_winsize", process_coex_rx_winsize},
	{"dfs_repeater", process_dfs_repeater},
#ifdef PCIE
	{"pcieregrw", process_pcie_reg_rw},
	{"pciebar0regrw", process_pcie_bar0_reg_rw},
#endif
	{"get_sensor_temp", process_get_sensor_temp},
	{"channel_switch", process_extend_channel_switch},
	{"auto_arp", process_auto_arp},
	{"txrxhistogram", process_txrxhistogram},
	{"indrstcfg", process_ind_rst_cfg},
	{"cloud_keep_alive", process_cloud_keep_alive},
	{"tsf", process_tsf},
	{"dot11_txrx", process_dot11_txrx},
	{"per_pkt_cfg", process_per_pkt_cfg},
	{"dyn_bw", process_dyn_bw},
	{"robustcoex", process_robustcoex},
#if defined(PCIE)
	{"ssu", process_ssu_cmd},
#endif
	{"ctrldeauth", process_ctrldeauth},
	{"bootsleep", process_bootsleep},
	{"dmcs", process_dmcs},
	{"11axcfg", process_11axcfg},
	{"11axcmd", process_11axcmdcfg},
	{"twt_setup", process_twt_setup},
	{"twt_teardown", process_twt_teardown},
	{"rx_abort_cfg", process_rx_abort_cfg},
	{"rx_abort_cfg_ext", process_rx_abort_cfg_ext},
	{"tx_ampdu_prot_mode", process_tx_ampdu_prot_mode},
	{"rate_adapt_cfg", process_rate_adapt_cfg},
	{"cck_desense_cfg", process_cck_desense_cfg},
	{"lpm", process_lpm},
};

static char *usage[] = {
	"Usage: ",
	"   mlanutl -v  (version)",
	"   mlanutl <ifname> <cmd> [...]",
	"   where",
	"   ifname : wireless network interface name, such as mlanX or uapX",
	"   cmd :",
	"         11dcfg",
	"         11dclrtbl",
	"         addbapara",
	"         addbareject",
	"         addts",
	"         aggrpriotbl",
	"         amsduaggrctrl",
	"         antcfg",
#ifdef REASSOCIATION
	"         assocessid",
	"         assocessid_bssid",
#endif
	"         associate",
	"         authtype",
	"         bandcfg",
	"         bcninterval",
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
	"         bssrole",
#endif
#endif
	"         cfgdata",
	"         cfpcode",
	"         changraph",
	"         coex_rx_winsize",
	"         countrycode",
	"         customie",
	"         deauth",
	"         deepsleep",
	"         delba",
	"         delts",
	"         dfstesting",
	"         dfs_repeater",
#ifdef DEBUG_LEVEL1
	"         drvdbg",
#endif
	"         dscpmap",
	"         esuppmode",
#ifdef STA_SUPPORT
	"         extcapcfg",
	"         cancelscan",
#endif
	"         fwmacaddr",
/* #ifdef FW_WAKEUP_METHOD */
	"         fwwakeupmethod",
/* #endif */
	"         getdatarate",
	"         getkey",
	"         getlog",
#ifdef STA_SUPPORT
	"         getscantable",
#endif
	"         getsignal",
#ifdef UAP_SUPPORT
	"         getstalist",
#endif
	"         hostcmd",
	"         hotspotcfg",
	"         hscfg",
	"         hssetpara",
	"         mgmtfilter",
	"         htcapinfo",
	"         htstreamcfg",
	"         mimoswitch",
#ifdef STA_SUPPORT
	"         signalextcfg",
	"         getsignalext",
	"         getsignalextv2",
#endif
	"         httxbfcap",
	"         httxbfcfg",
	"         httxcfg",
	"         inactivityto",
	"         ipaddr",
	"         linkstats",
#ifdef STA_SUPPORT
	"         listeninterval",
#endif
	"         macctrl",
	"         mefcfg",
	"         memrdwr",
#ifdef WIFI_DIRECT_SUPPORT
	"         miracastcfg",
#endif
	"         mgmtframectrl",
	"         mgmtframetx",
#ifdef SDIO
	"         mpactrl",
#endif
#ifdef WIFI_DIRECT_SUPPORT
	"         cfg_noa",
	"         cfg_opp_ps",
#endif
	"         offchannel",
	"         otpuserdata",
	"         passphrase",
	"         pb_bypass",
#ifdef PCIE
	"         pcieregrw",
#endif
#if defined(STA_SUPPORT)
	"         pmfcfg",
#endif
	"         port_ctrl",
	"         powercons",
	"         pscfg",
#ifdef STA_SUPPORT
	"         psmode",
#endif
	"         qconfig",
	"         qoscfg",
	"         qstatus",
#ifdef STA_WEXT
	"         radioctrl",
#endif
	"         rdeeprom",
#if defined(REASSOCIATION)
	"         reassoctrl",
#endif
	"         regioncode",
	"         cfpinfo",
	"         regrdwr",
	"         rejectaddbareq",
	"         scancfg",
#ifdef SDIO
	"         sdcmd52rw",
	"         sdcmd53rw",
	"         sdioclock",
#endif
#ifdef STA_SUPPORT
	"         setuserscan",
#endif
	"         sleepparams",
	"         sleeppd",
	"         sysclock",
	"         tcpackenh",
	"         thermal",
	"         ts_status",
	"         tsf",
	"         txbufcfg",
	"         txcontrol",
	"         txpowercfg",
	"         txratecfg",
	"         aggrctrl",
	"         usbaggrctrl",
	"         usbresume",
	"         usbsuspend",
	"         verext",
	"         version",
	"         vhtcfg",
	"         opermodecfg",
	"         wakeupreason",
	"         warmreset",
	"         wmmcfg",
	"         wmmparamcfg",
	"         min_ba_threshold",
	"         wpssession",
#ifndef OPCHAN
	"         wwscfg",
#endif
#ifdef RX_PACKET_COALESCE
	"         rxpktcoal_cfg",
#endif
	"         get_sensor_temp",
	"         channel_switch",
	"         indrstcfg",
	"         cloud_keep_alive",
	"         dfs_offload",

	"         cwmode",
	"         dyn_bw",
	"         txrxhistogram",
	"         per_pkt_cfg",
	"         dot11_txrx",
	"         robustcoex",
	"         ctrldeauth",
	"         dmcs",
	"         11axcfg",
	"         11axcmd",
	"         twt_setup",
	"         twt_teardown",
	"         rx_abort_cfg",
	"         rx_abort_cfg_ext",
	"         tx_ampdu_prot_mode",
	"         rate_adapt_cfg",
	"         cck_desense_cfg",
	"         get_chnrgpwr",
	"         comparergpwr",
	"         get_txpwrlimit",
	"         comparetrpc",
	"         getcfgchanlist",
	"         lpm",
};

/** Socket */
t_s32 sockfd;
/** Device name */
char dev_name[IFNAMSIZ + 1];
#define HOSTCMD "hostcmd"

static char *config_get_line(char *s, int size, FILE * stream, int *line,
			     char **_pos);
static int parse_line(char *line, char *args[], t_u16 args_count);
#define BSSID_FILTER 1
#define SSID_FILTER 2
/********************************************************
			Global Variables
********************************************************/

int setuserscan_filter = 0;
int num_ssid_filter = 0;
/********************************************************
			Local Functions
********************************************************/

/**
 *    @brief isdigit for String.
 *
 *    @param x            Char string
 *    @return             MLAN_STATUS_FAILURE for non-digit.
 *                        MLAN_STATUS_SUCCESS for digit
 */
static int
ISDIGIT(char *x)
{
	unsigned int i;
	for (i = 0; i < strlen(x); i++)
		if (isdigit(x[i]) == 0)
			return MLAN_STATUS_FAILURE;
	return MLAN_STATUS_SUCCESS;
}

/**
 * Check of decimal or hex string
 * @param   num string
 */
#define IS_HEX_OR_DIGIT(num) \
    (strncasecmp("0x", (num), 2)?ISDIGIT((num)):ishexstring((num)))

/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char to convert
 *  @return         Hex integer or 0
 */
int
hexval(t_s32 chr)
{
	if (chr >= '0' && chr <= '9')
		return chr - '0';
	if (chr >= 'A' && chr <= 'F')
		return chr - 'A' + 10;
	if (chr >= 'a' && chr <= 'f')
		return chr - 'a' + 10;

	return 0;
}

/**
 *  @brief Hump hex data
 *
 *  @param prompt   A pointer prompt buffer
 *  @param p        A pointer to data buffer
 *  @param len      The len of data buffer
 *  @param delim    Delim char
 *  @return         Hex integer
 */
t_void
hexdump(char *prompt, t_void *p, t_s32 len, char delim)
{
	t_s32 i;
	t_u8 *s = p;

	if (prompt) {
		printf("%s: len=%d\n", prompt, (int)len);
	}
	for (i = 0; i < len; i++) {
		if (i != len - 1)
			printf("%02x%c", *s++, delim);
		else
			printf("%02x\n", *s);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
}

/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char
 *  @return         Hex integer
 */
t_u8
hexc2bin(char chr)
{
	if (chr >= '0' && chr <= '9')
		chr -= '0';
	else if (chr >= 'A' && chr <= 'F')
		chr -= ('A' - 10);
	else if (chr >= 'a' && chr <= 'f')
		chr -= ('a' - 10);

	return chr;
}

/**
 *  @brief Convert string to hex integer
 *
 *  @param s        A pointer string buffer
 *  @return         Hex integer
 */
t_u32
a2hex(char *s)
{
	t_u32 val = 0;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}

	while (*s && isxdigit(*s)) {
		val = (val << 4) + hexc2bin(*s++);
	}

	return val;
}

/*
 *  @brief Convert String to integer
 *
 *  @param value    A pointer to string
 *  @return         Integer
 */
t_u32
a2hex_or_atoi(char *value)
{
	if (value[0] == '0' && (value[1] == 'X' || value[1] == 'x')) {
		return a2hex(value + 2);
	} else {
		return (t_u32)atoi(value);
	}
}

/**
 *  @brief Convert string to hex
 *
 *  @param ptr      A pointer to data buffer
 *  @param chr      A pointer to return integer
 *  @return         A pointer to next data field
 */
char *
convert2hex(char *ptr, t_u8 *chr)
{
	t_u8 val;

	for (val = 0; *ptr && isxdigit(*ptr); ptr++) {
		val = (val * 16) + hexval(*ptr);
	}

	*chr = val;

	return ptr;
}

/**
 *  @brief Check the Hex String
 *  @param s  A pointer to the string
 *  @return   MLAN_STATUS_SUCCESS --HexString, MLAN_STATUS_FAILURE --not HexString
 */
int
ishexstring(char *s)
{
	int ret = MLAN_STATUS_FAILURE;
	t_s32 tmp;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}
	while (*s) {
		tmp = toupper(*s);
		if (((tmp >= 'A') && (tmp <= 'F')) ||
		    ((tmp >= '0') && (tmp <= '9'))) {
			ret = MLAN_STATUS_SUCCESS;
		} else {
			ret = MLAN_STATUS_FAILURE;
			break;
		}
		s++;
	}

	return ret;
}

/**
 *  @brief Convert String to Integer
 *  @param buf      A pointer to the string
 *  @return         Integer
 */
int
atoval(char *buf)
{
	if (!strncasecmp(buf, "0x", 2))
		return a2hex(buf + 2);
	else if (!ishexstring(buf))
		return a2hex(buf);
	else
		return atoi(buf);
}

/**
 *  @brief Display usage
 *
 *  @return       NA
 */
static t_void
display_usage(t_void)
{
	t_u32 i;
	for (i = 0; i < NELEMENTS(usage); i++)
		fprintf(stderr, "%s\n", usage[i]);
}

/**
 *  @brief Find and execute command
 *
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS for success, otherwise failure
 */
static int
process_command(int argc, char *argv[])
{
	int i = 0, ret = MLAN_STATUS_NOTFOUND;
	struct command_node *node = NULL;

	for (i = 0; i < (int)NELEMENTS(command_list); i++) {
		node = &command_list[i];
		if (!strcasecmp(node->name, argv[2])) {
			ret = node->handler(argc, argv);
			break;
		}
	}

	return ret;
}

/**
 * @brief Converts a string to hex value
 *
 * @param str      A pointer to the string
 * @param raw      A pointer to the raw data buffer
 * @return         Number of bytes read
 **/
int
string2raw(char *str, unsigned char *raw)
{
	int len = (strlen(str) + 1) / 2;

	do {
		if (!isxdigit(*str)) {
			return -1;
		}
		*str = toupper(*str);
		*raw = CHAR2INT(*str) << 4;
		++str;
		*str = toupper(*str);
		if (*str == '\0')
			break;
		*raw |= CHAR2INT(*str);
		++raw;
	} while (*++str != '\0');
	return len;
}

/**
 *  @brief Prepare command buffer
 *  @param buffer   Command buffer to be filled
 *  @param cmd      Command id
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int
prepare_buffer(t_u8 *buffer, char *cmd, t_u32 num, char *args[])
{
	t_u8 *pos = NULL;
	unsigned int i = 0;

	memset(buffer, 0, BUFFER_LENGTH);

	/* Flag it for our use */
	pos = buffer;
	strncpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));

	/* Insert command */
	strncpy((char *)pos, (char *)cmd, strlen(cmd));
	pos += (strlen(cmd));

	/* Insert arguments */
	for (i = 0; i < num; i++) {
		strncpy((char *)pos, args[i], strlen(args[i]));
		pos += strlen(args[i]);
		if (i < (num - 1)) {
			strncpy((char *)pos, " ", strlen(" "));
			pos += 1;
		}
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Determine the netlink number
 *
 *  @param i socket number
 *
 *  @return         Netlink number to use
 */
static int
get_netlink_num(int i)
{
	FILE *fp;
	int netlink_num = NETLINK_NXP;
	char str[64];
	char *srch = "netlink_num";
	char filename[64];

	/* Try to open old driver proc: /proc/mwlan/configX first */
	if (i == 0)
		strcpy(filename, "/proc/mwlan/config");
	else if (i > 0)
		sprintf(filename, "/proc/mwlan/config%d", i);
	fp = fopen(filename, "r");
	if (!fp) {
		/* Try to open multi-adapter driver proc: /proc/mwlan/adapterX/config if old proc access fail  */
		snprintf(filename, sizeof(filename),
			 "/proc/mwlan/adapter%d/config", i);
		fp = fopen(filename, "r");
	}

	if (fp) {
		while (!feof(fp)) {
			fgets(str, sizeof(str), fp);
			if (strncmp(str, srch, strlen(srch)) == 0) {
				netlink_num = atoi(str + strlen(srch) + 1);
				break;
			}
		}
		fclose(fp);
	} else {
		return -1;
	}
	return netlink_num;
}

/**
 *  @brief Read event data from netlink socket
 *
 *  @param sk_fd    Netlink socket handler
 *  @param buffer   Pointer to the data buffer
 *  @param nlh      Pointer to netlink message header
 *  @param msg      Pointer to message header
 *
 *  @return         Number of bytes read or MLAN_EVENT_FAILURE
 */
int
read_event_netlink_socket(int sk_fd, unsigned char *buffer,
			  struct nlmsghdr *nlh, struct msghdr *msg)
{
	int count = -1;
	count = recvmsg(sk_fd, msg, 0);
#if DEBUG
	printf("DBG:Waiting for message from NETLINK.\n");
#endif
	if (count < 0) {
		printf("ERR:NETLINK read failed!\n");
		terminate_flag++;
		return MLAN_EVENT_FAILURE;
	}
#if DEBUG
	printf("DBG:Received message payload (%d)\n", count);
#endif
	if (count > NLMSG_SPACE(NL_MAX_PAYLOAD)) {
		printf("ERR:Buffer overflow!\n");
		return MLAN_EVENT_FAILURE;
	}
	memset(buffer, 0, NL_MAX_PAYLOAD);
	memcpy(buffer, NLMSG_DATA(nlh), count - NLMSG_HDRLEN);
#if DEBUG
	hexdump(buffer, count - NLMSG_HDRLEN, ' ');
#endif
	return count - NLMSG_HDRLEN;
}

/**
 *  @brief Configure and read event data from netlink socket
 *
 *  @param sk_fd    Array of netlink sockets
 *  @param no_of_sk Number of netlink sockets opened
 *  @param recv_buf Pointer to the array of evt_buf structures
 *  @param timeout  Socket listen timeout value
 *  @param nlh      Pointer to netlink message header
 *  @param msg      Pointer to message header
 *
 *  @return         Number of bytes read or MLAN_EVENT_FAILURE
 */
int
read_event(int *sk_fd, int no_of_sk, evt_buf *recv_buf, int timeout,
	   struct nlmsghdr *nlh, struct msghdr *msg)
{
	struct timeval tv;
	fd_set rfds;
	int i = 0, max_sk_fd = sk_fd[0];
	int ret = MLAN_EVENT_FAILURE;

	/* Setup read fds */
	FD_ZERO(&rfds);
	for (i = 0; i < no_of_sk; i++) {
		if (sk_fd[i] > max_sk_fd)
			max_sk_fd = sk_fd[i];

		if (sk_fd[i] > 0)
			FD_SET(sk_fd[i], &rfds);
	}

	/* Initialize timeout value */
	if (timeout != 0)
		tv.tv_sec = timeout;
	else
		tv.tv_sec = UAP_RECV_WAIT_DEFAULT;
	tv.tv_usec = 0;

	/* Wait for reply */
	ret = select(max_sk_fd + 1, &rfds, NULL, NULL, &tv);
	if (ret == -1) {
		/* Error */
		terminate_flag++;
		return MLAN_EVENT_FAILURE;
	} else if (!ret) {
		/* Timeout. Try again */
		return MLAN_EVENT_FAILURE;
	}
	for (i = 0; i < no_of_sk; i++) {
		if (sk_fd[i] > 0) {
			if (FD_ISSET(sk_fd[i], &rfds)) {
				/* Success */
				recv_buf[i].flag = 1;
				recv_buf[i].length =
					read_event_netlink_socket(sk_fd[i],
								  recv_buf[i].
								  buffer, nlh,
								  msg);
				ret += recv_buf[i].length;
			}
		}
	}
	return ret;
}

/**
 *  @brief Signal handler
 *
 *  @param sig      Received signal number
 *
 *  @return         N/A
 */
void
sig_handler(int sig)
{
	printf("Stopping application.\n");
	terminate_flag = 1;
}

/**
 * @brief      Wait event specified by event ID, and return back the pointer.
 *
 * @param eventID                     Event ID
 * @param pEvent                   Pointer to the Event buffer
 * @param pEventLen                Pointer to the Event Length
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
wait_event(t_u32 eventID, t_u8 *pEvent, int *pEventLen)
{
	int nl_sk[MAX_NO_OF_DEVICES];
	struct nlmsghdr *nlh = NULL;
	struct sockaddr_nl src_addr, dest_addr;
	struct msghdr msg;
	struct iovec iov;
	evt_buf evt_recv_buf[MAX_NO_OF_DEVICES];
	int num_events = 0;
	event_header *event = NULL;
	int ret = MLAN_EVENT_FAILURE;
	int netlink_num[MAX_NO_OF_DEVICES];
	char if_name[IFNAMSIZ + 1];
	t_u32 event_id = 0;
	int i = 0, no_of_sk = 0;
	int buf_len = 0;

	if (!pEvent || !pEventLen) {
		printf("ERR:Input paramater(s) 'pEvent' or 'pEventLen' is NULL");
		goto done;
	}

	buf_len = *pEventLen;
	*pEventLen = 0;

	/* Currently, we support maximum 4 devices */
	/* TODO: determine no_of_sk at run time */
	no_of_sk = MAX_NO_OF_DEVICES;

	for (i = 0; i < no_of_sk; i++) {
		/* Initialise */
		nl_sk[i] = -1;
		netlink_num[i] = get_netlink_num(i);
		if (netlink_num[i] >= 0) {
			/* Open netlink socket */
			nl_sk[i] = socket(PF_NETLINK, SOCK_RAW, netlink_num[i]);
			if (nl_sk[i] < 0) {
				printf("ERR:Could not open netlink socket.\n");
				ret = MLAN_EVENT_FAILURE;
				goto done;
			}

			/* Set source address */
			memset(&src_addr, 0, sizeof(src_addr));
			src_addr.nl_family = AF_NETLINK;
			src_addr.nl_pid = getpid();	/* Our PID */
			src_addr.nl_groups = NL_MULTICAST_GROUP;

			/* Bind socket with source address */
			if (bind
			    (nl_sk[i], (struct sockaddr *)&src_addr,
			     sizeof(src_addr)) < 0) {
				printf("ERR:Could not bind socket!\n");
				ret = MLAN_EVENT_FAILURE;
				goto done;
			}

			/* Set destination address */
			memset(&dest_addr, 0, sizeof(dest_addr));
			dest_addr.nl_family = AF_NETLINK;
			dest_addr.nl_pid = 0;	/* Kernel */
			dest_addr.nl_groups = NL_MULTICAST_GROUP;

			/* Initialize netlink header */
			nlh = (struct nlmsghdr *)
				malloc(NLMSG_SPACE(NL_MAX_PAYLOAD));
			if (!nlh) {
				printf("ERR: Could not alloc buffer\n");
				ret = MLAN_EVENT_FAILURE;
				goto done;
			}
			memset(nlh, 0, NLMSG_SPACE(NL_MAX_PAYLOAD));

			/* Initialize I/O vector */
			iov.iov_base = (void *)nlh;
			iov.iov_len = NLMSG_SPACE(NL_MAX_PAYLOAD);

			/* Initialize message header */
			memset(&msg, 0, sizeof(struct msghdr));
			msg.msg_name = (void *)&dest_addr;
			msg.msg_namelen = sizeof(dest_addr);
			msg.msg_iov = &iov;
			msg.msg_iovlen = 1;

			memset(&evt_recv_buf[i], 0, sizeof(evt_buf));
		}
	}

	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGALRM, sig_handler);
	while (1) {
		if (terminate_flag) {
			printf("Stopping!\n");
			break;
		}
		ret = read_event(nl_sk, no_of_sk, evt_recv_buf, 0, nlh, &msg);

		/* No result. Loop again */
		if (ret == MLAN_EVENT_FAILURE) {
			continue;
		}
		if (ret == 0) {
			/* Zero bytes received */
			printf("ERR:Received zero bytes!\n");
			continue;
		}
		for (i = 0; i < no_of_sk; i++) {
			if (evt_recv_buf[i].flag == 1) {
				num_events++;

				memcpy(&event_id, evt_recv_buf[i].buffer,
				       sizeof(event_id));
				if (((event_id & 0xFF000000) == 0x80000000) ||
				    ((event_id & 0xFF000000) == 0)) {
					event = (event_header
						 *)(evt_recv_buf[i].buffer);
				} else {
					memset(if_name, 0, IFNAMSIZ + 1);
					memcpy(if_name, evt_recv_buf[i].buffer,
					       IFNAMSIZ);
					event = (event_header
						 *)((t_u8 *)(evt_recv_buf[i].
							     buffer) +
						    IFNAMSIZ);
					ret -= IFNAMSIZ;
					evt_recv_buf[i].length -= IFNAMSIZ;
				}
				if (event->event_id == eventID) {
					if (buf_len > evt_recv_buf[i].length) {
						*pEventLen =
							evt_recv_buf[i].length;
						memcpy(pEvent, (t_u8 *)event,
						       evt_recv_buf[i].length);
					} else {
						printf("ERR:Buffer length exceeded \n");
					}
					goto done;
				}
				/* Reset event flag after reading */
				evt_recv_buf[i].flag = 0;
			}
		}
		fflush(stdout);
	}

done:
	for (i = 0; i < no_of_sk; i++) {
		if (nl_sk[i] > 0)
			close(nl_sk[i]);
	}
	if (nlh)
		free(nlh);
	return 0;

}

/**
 * @brief      Set Robustcoex gpiocfg
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_robustcoex(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc <= 4) {
		printf("Err: Invalid number of arguments\n");
		printf("Usage: ./mlanutl <interface> robustcoex [gpiocfg] [value]\n");
		return MLAN_STATUS_FAILURE;
	}
	if (strcmp(argv[3], "gpiocfg") == 0) {
		if (argc != 7 && argc != 5) {
			printf("ERR: Invalid number of arguments\n");
			printf("Usage: ./mlanutl <interface> robustcoex gpiocfg [Enable][Gpionum][Gpiopolarity]\n");
			printf("Usage: ./mlanutl <interface> robustcoex gpiocfg [Disable]\n");
			return MLAN_STATUS_FAILURE;
		}
	} else {
		printf("ERR: Invalid arguments\n");
		printf("Usage: ./mlanutl <interface> robustcoex [gpiocfg][value]\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: robustcoex fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Set/get DMCS config
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_dmcs(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	struct eth_priv_dmcs_status *status;
	int i = 0, j = 0;

	if (argc <= 3 || argc > 5) {
		printf("Err: Invalid number of arguments\n");
		printf("Usage: ./mlanutl <interface> dmcs [subcmd] [value]\n");
		return MLAN_STATUS_FAILURE;
	}
	if ((*argv[3] != '0') && (*argv[3] != '1')) {
		printf("Err: Invalid input argument of [subcmd]!\n");
		printf("Currently, we only support 0 and 1 for [subcmd]\n");
		return MLAN_STATUS_FAILURE;
	}
	if (*argv[3] == '0') {
		if (argc != 5) {
			printf("Err: Invalid number of arguments for disable/enable DMCS\n");
			printf("Usage: ./mlanutl <interface> dmcs [subcmd] [value]\n");
			return MLAN_STATUS_FAILURE;
		}
	}
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dmcs fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Process get status response */
	if (*argv[3] == '1') {
		status = (struct eth_priv_dmcs_status *)buffer;
		printf("mapping policy: %d\n", status->mapping_policy);
		for (i = 0; i < MAX_NUM_MAC; i++) {
			printf("radio_status[%d]:\n", i);
			printf("\tradio id: %d\n",
			       status->radio_status[i].radio_id);
			printf("\trunning mode: %d\n",
			       status->radio_status[i].running_mode);
			for (j = 0; j < 2; j++) {
				printf("\tchan_status[%d]:\n", j);
				printf("\t\tchannel: %d\n",
				       status->radio_status[i].chan_status[j].
				       channel);
				printf("\t\tap count: %d\n",
				       status->radio_status[i].chan_status[j].
				       ap_count);
				printf("\t\tsta count: %d\n",
				       status->radio_status[i].chan_status[j].
				       sta_count);
			}
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return MLAN_STATUS_SUCCESS;
}

#if defined(PCIE)
/**
 * @brief      Enable SSU support
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_ssu_cmd(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	char *args[30], *pos = NULL;
	int li = 0, cmd_found = 0;
	char *line = NULL;
	FILE *fp = NULL;
	ssu_params_cfg *ssu_params = NULL;
	int ret = 0;
	int ssu_mode = 0;
	int used_len = 0;

	if ((argc != 3) && (argc != 4)) {
		printf("Err: Invalid number of arguments\n");
		printf("Usage: ./mlanutl <interface> ssu\n");
		printf("Usage: ./mlanutl <interface> ssu config/ssu.conf\n");
		printf("Usage: ./mlanutl <interface> ssu 2\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 4) {
		ssu_mode = (t_u8)a2hex_or_atoi(argv[3]);
		used_len =
			strlen(CMD_NXP) + strlen(argv[2]) +
			sizeof(ssu_params_cfg);
	} else {
		used_len = strlen(CMD_NXP) + strlen(argv[2]);
	}
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc == 4 && (ssu_mode != 2)) {
		line = (char *)malloc(MAX_CONFIG_LINE);
		if (!line) {
			printf("ERR:Cannot allocate memory for line\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		memset(line, 0, MAX_CONFIG_LINE);
		ssu_params =
			(ssu_params_cfg *) (buffer + strlen(CMD_NXP) +
					    strlen(argv[2]));
		ssu_params->ssu_mode = 0;
		fp = fopen(argv[3], "r");
		if (fp == NULL) {
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		/* Parse file and process */
		while (config_get_line(line, MAX_CONFIG_LINE, fp, &li, &pos)) {
			parse_line(line, args, 30);
			if (!cmd_found &&
			    strncmp(args[0], "ssu_params_cfg", strlen(args[0])))
				continue;
			cmd_found = 1;
			if (strcmp(args[0], "nskip") == 0)
				ssu_params->nskip =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "nsel") == 0)
				ssu_params->nsel =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "adcdownsample") == 0)
				ssu_params->adcdownsample =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "mask_adc_pkt") == 0)
				ssu_params->mask_adc_pkt =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "out_16bits") == 0)
				ssu_params->out_16bits =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "spec_pwr_enable") == 0)
				ssu_params->spec_pwr_enable =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "rate_reduction") == 0)
				ssu_params->rate_deduction =
					(t_u32)a2hex_or_atoi(args[1]);
			else if (strcmp(args[0], "n_pkt_avg") == 0)
				ssu_params->n_pkt_avg =
					(t_u32)a2hex_or_atoi(args[1]);
		}
	} else if (ssu_mode == 2) {
		ssu_params =
			(ssu_params_cfg *) (buffer + strlen(CMD_NXP) +
					    strlen(argv[2]));
		ssu_params->ssu_mode = 2;
	}
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;

	}
	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = used_len;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: ssu command fail\n");
		ret = MLAN_STATUS_FAILURE;
	}
done:
	if (line)
		free(line);
	if (fp)
		fclose(fp);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Get one line from the File
 *
 *  @param fp       File handler
 *  @param str      Storage location for data.
 *  @param size     Maximum number of characters to read.
 *  @param lineno   A pointer to return current line number
 *  @return         returns string or NULL
 */
char *
mlan_config_get_line(FILE * fp, char *str, t_s32 size, int *lineno)
{
	char *start, *end;
	int out, next_line;

	if (!fp || !str)
		return NULL;

	do {
read_line:
		if (!fgets(str, size, fp))
			break;
		start = str;
		start[size - 1] = '\0';
		end = start + strlen(str);
		(*lineno)++;

		out = 1;
		while (out && (start < end)) {
			next_line = 0;
			/* Remove empty lines and lines starting with # */
			switch (start[0]) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
				start++;
				break;
			case '#':
			case '\n':
			case '\0':
				next_line = 1;
				break;
			case '\r':
				if (start[1] == '\n')
					next_line = 1;
				else
					start++;
				break;
			default:
				out = 0;
				break;
			}
			if (next_line)
				goto read_line;
		}

		/* Remove # comments unless they are within a double quoted
		 * string. Remove trailing white space. */
		end = strstr(start, "\"");
		if (end) {
			end = strstr(end + 1, "\"");
			if (!end)
				end = start;
		} else
			end = start;

		end = strstr(end + 1, "#");
		if (end)
			*end-- = '\0';
		else
			end = start + strlen(start) - 1;

		out = 1;
		while (out && (start < end)) {
			switch (*end) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
			case '\n':
			case '\r':
				*end = '\0';
				end--;
				break;
			default:
				out = 0;
				break;
			}
		}

		if (*start == '\0')
			continue;

		return start;
	} while (1);

	return NULL;
}

/**
 *  @brief          Parse function for a configuration line
 *
 *  @param s        Storage buffer for data
 *  @param size     Maximum size of data
 *  @param stream   File stream pointer
 *  @param line     Pointer to current line within the file
 *  @param _pos     Output string or NULL
 *  @return         String or NULL
 */
static char *
config_get_line(char *s, int size, FILE * stream, int *line, char **_pos)
{
	*_pos = mlan_config_get_line(stream, s, size, line);
	return *_pos;
}

/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 *                  MAC_BROADCAST   - if broadcast mac
 *                  MAC_MULTICAST   - if multicast mac
 */
static int
mac2raw(char *mac, t_u8 *raw)
{
	unsigned int temp_raw[ETH_ALEN];
	int num_tokens = 0;
	int i;

	if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1))) {
		return MLAN_STATUS_FAILURE;
	}
	num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x",
			    temp_raw + 0, temp_raw + 1, temp_raw + 2,
			    temp_raw + 3, temp_raw + 4, temp_raw + 5);
	if (num_tokens != ETH_ALEN) {
		return MLAN_STATUS_FAILURE;
	}
	for (i = 0; i < num_tokens; i++)
		raw[i] = (t_u8)temp_raw[i];

	if (memcmp(raw, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) {
		return MAC_BROADCAST;
	} else if (raw[0] & 0x01) {
		return MAC_MULTICAST;
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @param args_count Max number of elements which can be filled in buffer 'args'
 *  @return         Number of arguments in the line or EOF
 */
static int
parse_line(char *line, char *args[], t_u16 args_count)
{
	int arg_num = 0;
	int is_start = 0;
	int is_quote = 0;
	int length = 0;
	int i = 0;

	arg_num = 0;
	length = strlen(line);
	/* Process line */

	/* Find number of arguments */
	is_start = 0;
	is_quote = 0;
	for (i = 0; (i < length) && (arg_num < args_count); i++) {
		/* Ignore leading spaces */
		if (is_start == 0) {
			if (line[i] == ' ') {
				continue;
			} else if (line[i] == '\t') {
				continue;
			} else if (line[i] == '\n') {
				break;
			} else {
				is_start = 1;
				args[arg_num] = &line[i];
				arg_num++;
			}
		}
		if (is_start == 1) {
			/* Ignore comments */
			if (line[i] == '#') {
				if (is_quote == 0) {
					line[i] = '\0';
					arg_num--;
				}
				break;
			}
			/* Separate by '=' */
			if (line[i] == '=') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Separate by ',' */
			if (line[i] == ',') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Change ',' to ' ', but not inside quotes */
			if ((line[i] == ',') && (is_quote == 0)) {
				line[i] = ' ';
				continue;
			}
		}
		/* Remove newlines */
		if (line[i] == '\n') {
			line[i] = '\0';
		}
		/* Check for quotes */
		if (line[i] == '"') {
			is_quote = (is_quote == 1) ? 0 : 1;
			continue;
		}
		if (((line[i] == ' ') || (line[i] == '\t')) && (is_quote == 0)) {
			line[i] = '\0';
			is_start = 0;
			continue;
		}
	}
	return arg_num;
}

/**
 *  @brief Process version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_version(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: version fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Version string received: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process band configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bandcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	int i;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_bandcfg *bandcfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bandcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	bandcfg = (struct eth_priv_bandcfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Band Configuration:\n");
		printf("  Infra Band: 0x%x (", (int)bandcfg->config_bands);
		for (i = 0; i < 10; i++) {
			if ((bandcfg->config_bands >> i) & 0x1)
				printf(" %s", band[i]);
		}
		printf(" )\n");
		printf("  Adhoc Start Band: 0x%x (",
		       (int)bandcfg->adhoc_start_band);
		for (i = 0; i < 10; i++) {
			if ((bandcfg->adhoc_start_band >> i) & 0x1)
				printf(" %s", band[i]);
		}
		printf(" )\n");
		printf("  Adhoc Start Channel: %d\n",
		       (int)bandcfg->adhoc_channel);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief get hostcmd data
 *
 *  @param ln           A pointer to line number
 *  @param buf          A pointer to hostcmd data
 *  @param size         A pointer to the return size of hostcmd buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static int
mlan_get_hostcmd_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s32 errors = 0, i;
	char line[512], *pos, *pos1, *pos2, *pos3;
	t_u16 len;

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		(*ln)++;
		if (strcmp(pos, "}") == 0) {
			break;
		}

		pos1 = strchr(pos, ':');
		if (pos1 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';

		pos2 = strchr(pos1, '=');
		if (pos2 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos2++ = '\0';

		len = a2hex_or_atoi(pos1);
		if (len < 1 || len > BUFFER_LENGTH) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}

		*size += len;

		if (*pos2 == '"') {
			pos2++;
			pos3 = strchr(pos2, '"');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = '\0';
			memset(buf, 0, len);
			memmove(buf, pos2, MIN(strlen(pos2), len));
			buf += len;
		} else if (*pos2 == '\'') {
			pos2++;
			pos3 = strchr(pos2, '\'');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = ',';
			for (i = 0; i < len; i++) {
				pos3 = strchr(pos2, ',');
				if (pos3 != NULL) {
					*pos3 = '\0';
					*buf++ = (t_u8)a2hex_or_atoi(pos2);
					pos2 = pos3 + 1;
				} else
					*buf++ = 0;
			}
		} else if (*pos2 == '{') {
			t_u16 tlvlen = 0, tmp_tlvlen;
			mlan_get_hostcmd_data(fp, ln, buf + len, &tlvlen);
			tmp_tlvlen = tlvlen;
			while (len--) {
				*buf++ = (t_u8)(tmp_tlvlen & 0xff);
				tmp_tlvlen >>= 8;
			}
			*size += tlvlen;
			buf += tlvlen;
		} else {
			t_u32 value = a2hex_or_atoi(pos2);
			while (len--) {
				*buf++ = (t_u8)(value & 0xff);
				value >>= 8;
			}
		}
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prepare host-command buffer
 *  @param fp       File handler
 *  @param cmd_name Command name
 *  @param buf      A pointer to comand buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
prepare_host_cmd_buffer(FILE * fp, char *cmd_name, t_u8 *buf)
{
	char line[256], cmdname[256], *pos, cmdcode[10];
	HostCmd_DS_GEN *hostcmd;
	t_u32 hostcmd_size = 0;
	int ln = 0;
	int cmdname_found = 0, cmdcode_found = 0;

	hostcmd = (HostCmd_DS_GEN *)(buf + sizeof(t_u32));
	hostcmd->command = 0xffff;

	snprintf(cmdname, sizeof(cmdname), "%s={", cmd_name);
	cmdname_found = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdcode, sizeof(cmdcode), "CmdCode=");
			cmdcode_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdcode, strlen(cmdcode)) == 0) {
					t_u16 len = 0;
					cmdcode_found = 1;
					hostcmd->command =
						a2hex_or_atoi(pos +
							      strlen(cmdcode));
					hostcmd->size = S_DS_GEN;
					mlan_get_hostcmd_data(fp, &ln,
							      buf +
							      sizeof(t_u32) +
							      hostcmd->size,
							      &len);
					hostcmd->size += len;
					break;
				}
			}
			if (!cmdcode_found) {
				fprintf(stderr,
					"mlanutl: CmdCode not found in conf file\n");
				return MLAN_STATUS_FAILURE;
			}
			break;
		}
	}

	if (!cmdname_found) {
		fprintf(stderr,
			"mlanutl: cmdname '%s' is not found in conf file\n",
			cmd_name);
		return MLAN_STATUS_FAILURE;
	}

	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	hostcmd->command = cpu_to_le16(hostcmd->command);
	hostcmd->size = cpu_to_le16(hostcmd->size);

	hostcmd_size = (t_u32)(hostcmd->size);
	memcpy(buf, (t_u8 *)&hostcmd_size, sizeof(t_u32));

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prints a MAC address in colon separated form from raw data
 *
 *  @param raw      A pointer to the hex data buffer
 *  @return         N/A
 */
void
print_mac(t_u8 *raw)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int)raw[0],
	       (unsigned int)raw[1], (unsigned int)raw[2], (unsigned int)raw[3],
	       (unsigned int)raw[4], (unsigned int)raw[5]);
	return;
}

/**
 *  @brief Process host_cmd response
 *  @param cmd_name Command name
 *  @param buf      A pointer to the response buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_host_cmd_resp(char *cmd_name, t_u8 *buf)
{
	t_u32 hostcmd_size = 0;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;

	buf += strlen(CMD_NXP) + strlen(cmd_name);
	memcpy((t_u8 *)&hostcmd_size, buf, sizeof(t_u32));
	buf += sizeof(t_u32);

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = le16_to_cpu(hostcmd->command);
	hostcmd->size = le16_to_cpu(hostcmd->size);
	hostcmd->seq_num = le16_to_cpu(hostcmd->seq_num);
	hostcmd->result = le16_to_cpu(hostcmd->result);

	hostcmd->command &= ~HostCmd_RET_BIT;
	if (!hostcmd->result) {
		switch (hostcmd->command) {
		case HostCmd_CMD_CFG_DATA:
			{
				HostCmd_DS_802_11_CFG_DATA *pstcfgData =
					(HostCmd_DS_802_11_CFG_DATA *)(buf +
								       S_DS_GEN);
				pstcfgData->data_len =
					le16_to_cpu(pstcfgData->data_len);
				pstcfgData->action =
					le16_to_cpu(pstcfgData->action);

				if (pstcfgData->action == HostCmd_ACT_GEN_GET) {
					hexdump("cfgdata", pstcfgData->data,
						pstcfgData->data_len, ' ');
				}
				break;
			}
		case HostCmd_CMD_802_11_TPC_ADAPT_REQ:
			{
				mlan_ioctl_11h_tpc_resp *tpcIoctlResp =
					(mlan_ioctl_11h_tpc_resp *)(buf +
								    S_DS_GEN);
				if (tpcIoctlResp->status_code == 0) {
					printf("tpcrequest:  txPower(%d), linkMargin(%d), rssi(%d)\n", tpcIoctlResp->tx_power, tpcIoctlResp->link_margin, tpcIoctlResp->rssi);
				} else {
					printf("tpcrequest:  failure, status = %d\n", tpcIoctlResp->status_code);
				}
				break;
			}
		case HostCmd_CMD_802_11_CRYPTO:
			{
				t_u16 alg =
					le16_to_cpu((t_u16)
						    *(buf + S_DS_GEN +
						      sizeof(t_u16)));
				if (alg == CIPHER_TEST_AES_CCM ||
				    alg == CIPHER_TEST_GCMP) {
					HostCmd_DS_802_11_CRYPTO_AES_CCM
						*cmd_aes_ccm =
						(HostCmd_DS_802_11_CRYPTO_AES_CCM
						 *)(buf + S_DS_GEN);

					cmd_aes_ccm->encdec
						=
						le16_to_cpu(cmd_aes_ccm->
							    encdec);
					cmd_aes_ccm->algorithm =
						le16_to_cpu(cmd_aes_ccm->
							    algorithm);
					cmd_aes_ccm->key_length =
						le16_to_cpu(cmd_aes_ccm->
							    key_length);
					cmd_aes_ccm->nonce_length =
						le16_to_cpu(cmd_aes_ccm->
							    nonce_length);
					cmd_aes_ccm->AAD_length =
						le16_to_cpu(cmd_aes_ccm->
							    AAD_length);
					cmd_aes_ccm->data.header.type =
						le16_to_cpu(cmd_aes_ccm->data.
							    header.type);
					cmd_aes_ccm->data.header.len =
						le16_to_cpu(cmd_aes_ccm->data.
							    header.len);

					printf("crypto_result: encdec=%d algorithm=%d, KeyLen=%d," " NonceLen=%d,AADLen=%d,dataLen=%d\n", cmd_aes_ccm->encdec, cmd_aes_ccm->algorithm, cmd_aes_ccm->key_length, cmd_aes_ccm->nonce_length, cmd_aes_ccm->AAD_length, cmd_aes_ccm->data.header.len);

					hexdump("Key", cmd_aes_ccm->key,
						cmd_aes_ccm->key_length, ' ');
					hexdump("Nonce", cmd_aes_ccm->nonce,
						cmd_aes_ccm->nonce_length, ' ');
					hexdump("AAD", cmd_aes_ccm->AAD,
						cmd_aes_ccm->AAD_length, ' ');
					hexdump("Data", cmd_aes_ccm->data.data,
						cmd_aes_ccm->data.header.len,
						' ');
				} else if (alg == CIPHER_TEST_WAPI) {
					HostCmd_DS_802_11_CRYPTO_WAPI *cmd_wapi
						=
						(HostCmd_DS_802_11_CRYPTO_WAPI
						 *) (buf + S_DS_GEN);

					cmd_wapi->encdec
						= le16_to_cpu(cmd_wapi->encdec);
					cmd_wapi->algorithm
						=
						le16_to_cpu(cmd_wapi->
							    algorithm);
					cmd_wapi->key_length =
						le16_to_cpu(cmd_wapi->
							    key_length);
					cmd_wapi->nonce_length =
						le16_to_cpu(cmd_wapi->
							    nonce_length);
					cmd_wapi->AAD_length =
						le16_to_cpu(cmd_wapi->
							    AAD_length);

					printf("crypto_result: encdec=%d algorithm=%d, KeyLen=%d," " NonceLen=%d,AADLen=%d,dataLen=%d\n", cmd_wapi->encdec, cmd_wapi->algorithm, cmd_wapi->key_length, cmd_wapi->nonce_length, cmd_wapi->AAD_length, cmd_wapi->data_length);

					hexdump("Key", cmd_wapi->key,
						cmd_wapi->key_length, ' ');
					hexdump("Nonce", cmd_wapi->nonce,
						cmd_wapi->nonce_length, ' ');
					hexdump("AAD", cmd_wapi->AAD,
						cmd_wapi->AAD_length, ' ');
				} else {
					HostCmd_DS_802_11_CRYPTO *cmd =
						(HostCmd_DS_802_11_CRYPTO *)(buf
									     +
									     S_DS_GEN);
					cmd->encdec = le16_to_cpu(cmd->encdec);
					cmd->algorithm =
						le16_to_cpu(cmd->algorithm);
					cmd->key_IV_length =
						le16_to_cpu(cmd->key_IV_length);
					cmd->key_length =
						le16_to_cpu(cmd->key_length);
					cmd->data.header.type =
						le16_to_cpu(cmd->data.header.
							    type);
					cmd->data.header.len =
						le16_to_cpu(cmd->data.header.
							    len);

					printf("crypto_result: encdec=%d algorithm=%d,KeyIVLen=%d," " KeyLen=%d,dataLen=%d\n", cmd->encdec, cmd->algorithm, cmd->key_IV_length, cmd->key_length, cmd->data.header.len);
					hexdump("KeyIV", cmd->keyIV,
						cmd->key_IV_length, ' ');
					hexdump("Key", cmd->key,
						cmd->key_length, ' ');
					hexdump("Data", cmd->data.data,
						cmd->data.header.len, ' ');
				}
				break;
			}
		case HostCmd_CMD_802_11_AUTO_TX:
			{
				HostCmd_DS_802_11_AUTO_TX *at =
					(HostCmd_DS_802_11_AUTO_TX *)(buf +
								      S_DS_GEN);

				if (le16_to_cpu(at->action) ==
				    HostCmd_ACT_GEN_GET) {
					if (S_DS_GEN + sizeof(at->action) ==
					    hostcmd->size) {
						printf("auto_tx not configured\n");

					} else {
						MrvlIEtypesHeader_t *header =
							&at->auto_tx.header;

						header->type =
							le16_to_cpu(header->
								    type);
						header->len =
							le16_to_cpu(header->
								    len);

						if ((S_DS_GEN +
						     sizeof(at->action)
						     +
						     sizeof(MrvlIEtypesHeader_t)
						     + header->len ==
						     hostcmd->size) &&
						    (header->type ==
						     TLV_TYPE_AUTO_TX)) {

							AutoTx_MacFrame_t *atmf
								=
								&at->auto_tx.
								auto_tx_mac_frame;

							printf("Interval: %d second(s)\n", le16_to_cpu(atmf->interval));
							printf("Priority: %#x\n", atmf->priority);
							printf("Frame Length: %d\n", le16_to_cpu(atmf->frame_len));
							printf("Dest Mac Address: " "%02x:%02x:%02x:%02x:%02x:%02x\n", atmf->dest_mac_addr[0], atmf->dest_mac_addr[1], atmf->dest_mac_addr[2], atmf->dest_mac_addr[3], atmf->dest_mac_addr[4], atmf->dest_mac_addr[5]);
							printf("Src Mac Address: " "%02x:%02x:%02x:%02x:%02x:%02x\n", atmf->src_mac_addr[0], atmf->src_mac_addr[1], atmf->src_mac_addr[2], atmf->src_mac_addr[3], atmf->src_mac_addr[4], atmf->src_mac_addr[5]);

							hexdump("Frame Payload",
								atmf->payload,
								le16_to_cpu
								(atmf->
								 frame_len)
								-
								MLAN_MAC_ADDR_LENGTH
								* 2, ' ');
						} else {
							printf("incorrect auto_tx command response\n");
						}
					}
				}
				break;
			}
		case HostCmd_CMD_802_11_SUBSCRIBE_EVENT:
			{
				HostCmd_DS_802_11_SUBSCRIBE_EVENT *se =
					(HostCmd_DS_802_11_SUBSCRIBE_EVENT
					 *)(buf + S_DS_GEN);
				if (le16_to_cpu(se->action) ==
				    HostCmd_ACT_GEN_GET) {
					int len =
						S_DS_GEN +
						sizeof
						(HostCmd_DS_802_11_SUBSCRIBE_EVENT);
					printf("\nEvent\t\tValue\tFreq\tsubscribed\n\n");
					while (len < hostcmd->size) {
						MrvlIEtypesHeader_t *header =
							(MrvlIEtypesHeader_t
							 *)(buf + len);
						switch (le16_to_cpu
							(header->type)) {
						case TLV_TYPE_RSSI_LOW:
							{
								MrvlIEtypes_RssiThreshold_t
									*low_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *)(buf
									    +
									    len);
								printf("Beacon Low RSSI\t%d\t%d\t%s\n", low_rssi->RSSI_value, low_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0001) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_LOW:
							{
								MrvlIEtypes_SnrThreshold_t
									*low_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *)(buf
									    +
									    len);
								printf("Beacon Low SNR\t%d\t%d\t%s\n", low_snr->SNR_value, low_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0002) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_FAILCOUNT:
							{
								MrvlIEtypes_FailureCount_t
									*failure_count
									=
									(MrvlIEtypes_FailureCount_t
									 *)(buf
									    +
									    len);
								printf("Failure Count\t%d\t%d\t%s\n", failure_count->fail_value, failure_count->fail_freq, (le16_to_cpu(se->events) & 0x0004) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_BCNMISS:
							{
								MrvlIEtypes_BeaconsMissed_t
									*bcn_missed
									=
									(MrvlIEtypes_BeaconsMissed_t
									 *)(buf
									    +
									    len);
								printf("Beacon Missed\t%d\tN/A\t%s\n", bcn_missed->beacon_missed, (le16_to_cpu(se->events) & 0x0008) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_HIGH:
							{
								MrvlIEtypes_RssiThreshold_t
									*high_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *)(buf
									    +
									    len);
								printf("Bcn High RSSI\t%d\t%d\t%s\n", high_rssi->RSSI_value, high_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0010) ? "yes" : "no");
								break;
							}

						case TLV_TYPE_SNR_HIGH:
							{
								MrvlIEtypes_SnrThreshold_t
									*high_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *)(buf
									    +
									    len);
								printf("Beacon High SNR\t%d\t%d\t%s\n", high_snr->SNR_value, high_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0020) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_LOW_DATA:
							{
								MrvlIEtypes_RssiThreshold_t
									*low_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *)(buf
									    +
									    len);
								printf("Data Low RSSI\t%d\t%d\t%s\n", low_rssi->RSSI_value, low_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0040) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_LOW_DATA:
							{
								MrvlIEtypes_SnrThreshold_t
									*low_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *)(buf
									    +
									    len);
								printf("Data Low SNR\t%d\t%d\t%s\n", low_snr->SNR_value, low_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0080) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_HIGH_DATA:
							{
								MrvlIEtypes_RssiThreshold_t
									*high_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *)(buf
									    +
									    len);
								printf("Data High RSSI\t%d\t%d\t%s\n", high_rssi->RSSI_value, high_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0100) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_HIGH_DATA:
							{
								MrvlIEtypes_SnrThreshold_t
									*high_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *)(buf
									    +
									    len);
								printf("Data High SNR\t%d\t%d\t%s\n", high_snr->SNR_value, high_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0200) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_LINK_QUALITY:
							{
								MrvlIEtypes_LinkQuality_t
									*link_qual
									=
									(MrvlIEtypes_LinkQuality_t
									 *)(buf
									    +
									    len);
								printf("Link Quality Parameters:\n");
								printf("------------------------\n");
								printf("Link Quality Event Subscribed\t%s\n", (le16_to_cpu(se->events) & 0x0400) ? "yes" : "no");
								printf("Link SNR Threshold   = %d\n", le16_to_cpu(link_qual->link_SNR_thrs));
								printf("Link SNR Frequency   = %d\n", le16_to_cpu(link_qual->link_SNR_freq));
								printf("Min Rate Value       = %d\n", le16_to_cpu(link_qual->min_rate_val));
								printf("Min Rate Frequency   = %d\n", le16_to_cpu(link_qual->min_rate_freq));
								printf("Tx Latency Value     = %d\n", le32_to_cpu(link_qual->tx_latency_val));
								printf("Tx Latency Threshold = %d\n", le32_to_cpu(link_qual->tx_latency_thrs));

								break;
							}
						case TLV_TYPE_PRE_BEACON_LOST:
							{
								MrvlIEtypes_PreBeaconLost_t
									*pre_bcn_lost
									=
									(MrvlIEtypes_PreBeaconLost_t
									 *)(buf
									    +
									    len);
								printf("------------------------\n");
								printf("Pre-Beacon Lost Event Subscribed\t%s\n", (le16_to_cpu(se->events) & 0x0800) ? "yes" : "no");
								printf("Pre-Beacon Lost: %d\n", pre_bcn_lost->pre_beacon_lost);
								break;
							}
						default:
							printf("Unknown subscribed event TLV Type=%#x," " Len=%d\n", le16_to_cpu(header->type), le16_to_cpu(header->len));
							break;
						}

						len += (sizeof
							(MrvlIEtypesHeader_t)
							+
							le16_to_cpu(header->
								    len));
					}
				}
				break;
			}
		case HostCmd_CMD_MAC_REG_ACCESS:
		case HostCmd_CMD_BBP_REG_ACCESS:
		case HostCmd_CMD_RF_REG_ACCESS:
		case HostCmd_CMD_CAU_REG_ACCESS:
			{
				HostCmd_DS_REG *preg =
					(HostCmd_DS_REG *)(buf + S_DS_GEN);
				preg->action = le16_to_cpu(preg->action);
				if (preg->action == HostCmd_ACT_GEN_GET) {
					preg->value = le32_to_cpu(preg->value);
					printf("value = 0x%08x\n", preg->value);
				}
				break;
			}
		case HostCmd_CMD_MEM_ACCESS:
			{
				HostCmd_DS_MEM *pmem =
					(HostCmd_DS_MEM *)(buf + S_DS_GEN);
				pmem->action = le16_to_cpu(pmem->action);
				if (pmem->action == HostCmd_ACT_GEN_GET) {
					pmem->value = le32_to_cpu(pmem->value);
					printf("value = 0x%08x\n", pmem->value);
				}
				break;
			}
		case HostCmd_CMD_LINK_STATS_SUMMARY:
			{
				HostCmd_DS_LINK_STATS_SUMMARY *linkstats =
					(HostCmd_DS_LINK_STATS_SUMMARY *)(buf +
									  S_DS_GEN);
				/* GET operation */
				printf("Link Statistics: \n");
				/* format */
				printf("Duration:   %u\n",
				       (int)le32_to_cpu(linkstats->
							timeSinceLastQuery_ms));

				printf("Beacon count:     %u\n",
				       le16_to_cpu(linkstats->bcnCnt));
				printf("Beacon missing:   %u\n",
				       le16_to_cpu(linkstats->bcnMiss));
				printf("Beacon RSSI avg:  %d\n",
				       le16_to_cpu(linkstats->bcnRssiAvg));
				printf("Beacon SNR avg:   %d\n",
				       le16_to_cpu(linkstats->bcnSnrAvg));

				printf("Rx packets:       %u\n",
				       (int)le32_to_cpu(linkstats->rxPkts));
				printf("Rx RSSI avg:      %d\n",
				       le16_to_cpu(linkstats->rxRssiAvg));
				printf("Rx SNR avg:       %d\n",
				       le16_to_cpu(linkstats->rxSnrAvg));

				printf("Tx packets:       %u\n",
				       (int)le32_to_cpu(linkstats->txPkts));
				printf("Tx Attempts:      %u\n",
				       (int)le32_to_cpu(linkstats->txAttempts));
				printf("Tx Failures:      %u\n",
				       (int)le32_to_cpu(linkstats->txFailures));
				printf("Tx Initial Rate:  %s\n",
				       rateIdStr[linkstats->txInitRate]);

				printf("Tx AC VO:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_VO]),
				       (int)le32_to_cpu(linkstats->
							txQueueDelay[WMM_AC_VO])
				       / 1000);
				printf("Tx AC VI:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_VI]),
				       (int)le32_to_cpu(linkstats->
							txQueueDelay[WMM_AC_VI])
				       / 1000);
				printf("Tx AC BE:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_BE]),
				       (int)le32_to_cpu(linkstats->
							txQueueDelay[WMM_AC_BE])
				       / 1000);
				printf("Tx AC BK:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_BK]),
				       (int)le32_to_cpu(linkstats->
							txQueueDelay[WMM_AC_BK])
				       / 1000);
				break;
			}
		case HostCmd_CMD_WMM_PARAM_CONFIG:
			{
				HostCmd_DS_WMM_PARAM_CONFIG *wmm_param =
					(HostCmd_DS_WMM_PARAM_CONFIG *) (buf +
									 S_DS_GEN);
				printf("WMM Params: \n");
				printf("\tBE: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n", wmm_param->ac_params[AC_BE].aci_aifsn.aifsn, wmm_param->ac_params[AC_BE].ecw.ecw_max, wmm_param->ac_params[AC_BE].ecw.ecw_min, le16_to_cpu(wmm_param->ac_params[AC_BE].tx_op_limit));
				printf("\tBK: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n", wmm_param->ac_params[AC_BK].aci_aifsn.aifsn, wmm_param->ac_params[AC_BK].ecw.ecw_max, wmm_param->ac_params[AC_BK].ecw.ecw_min, le16_to_cpu(wmm_param->ac_params[AC_BK].tx_op_limit));
				printf("\tVI: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n", wmm_param->ac_params[AC_VI].aci_aifsn.aifsn, wmm_param->ac_params[AC_VI].ecw.ecw_max, wmm_param->ac_params[AC_VI].ecw.ecw_min, le16_to_cpu(wmm_param->ac_params[AC_VI].tx_op_limit));
				printf("\tVO: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n", wmm_param->ac_params[AC_VO].aci_aifsn.aifsn, wmm_param->ac_params[AC_VO].ecw.ecw_max, wmm_param->ac_params[AC_VO].ecw.ecw_min, le16_to_cpu(wmm_param->ac_params[AC_VO].tx_op_limit));
				break;
			}
		default:
			printf("HOSTCMD_RESP: CmdCode=%#04x, Size=%#04x,"
			       " SeqNum=%#04x, Result=%#04x\n",
			       hostcmd->command, hostcmd->size,
			       hostcmd->seq_num, hostcmd->result);
			hexdump("payload",
				(t_void *)(buf + S_DS_GEN),
				hostcmd->size - S_DS_GEN, ' ');
			break;
		}
	} else {
		printf("HOSTCMD failed: CmdCode=%#04x, Size=%#04x,"
		       " SeqNum=%#04x, Result=%#04x\n",
		       hostcmd->command, hostcmd->size,
		       hostcmd->seq_num, hostcmd->result);
	}
	return ret;
}

/**
 *  @brief Trims leading and traling spaces only
 *  @param str  A pointer to argument string
 *  @return     pointer to trimmed string
 */
char *
trim_spaces(char *str)
{
	char *str_end = NULL;

	if (!str)
		return NULL;

	/* Trim leading spaces */
	while (!*str && isspace(*str))
		str++;

	if (*str == 0)		/* All spaces? */
		return str;

	/* Trim trailing spaces */
	str_end = str + strlen(str) - 1;
	while (str_end > str && isspace(*str_end))
		str_end--;

	/* null terminate the string */
	*(str_end + 1) = '\0';

	return str;
}

/**
 *  @brief Process hostcmd command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hostcmd(int argc, char *argv[])
{
	t_u8 *buffer = NULL, *raw_buf = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	FILE *fp_raw = NULL;
	FILE *fp_dtsi = NULL;
	char cmdname[256];
	boolean call_ioctl = TRUE;
	t_u32 buf_len = 0, i, j, k;
	char *line = NULL, *pos = NULL;
	int li = 0, blk_count = 0, ob = 0;
	int ret = MLAN_STATUS_SUCCESS;

	struct cmd_node {
		char cmd_string[256];
		struct cmd_node *next;
	};
	struct cmd_node *command = NULL, *header = NULL, *new_node = NULL;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> <cmdname>\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	snprintf(cmdname, sizeof(cmdname), "%s", argv[4]);

	if (!strcmp(cmdname, "generate_raw")) {
		call_ioctl = FALSE;
	}

	if (!call_ioctl && argc != 6) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> %s <raw_data_file>\n", cmdname);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		fclose(fp);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	if (call_ioctl) {
		/* Prepare the hostcmd buffer */
		prepare_buffer(buffer, argv[2], 0, NULL);
		if (MLAN_STATUS_FAILURE ==
		    prepare_host_cmd_buffer(fp, cmdname,
					    buffer + strlen(CMD_NXP) +
					    strlen(argv[2]))) {
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		fclose(fp);
	} else {
		line = (char *)malloc(MAX_CONFIG_LINE);
		if (!line) {
			printf("ERR:Cannot allocate memory for line\n");
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		memset(line, 0, MAX_CONFIG_LINE);

		while (config_get_line(line, MAX_CONFIG_LINE, fp, &li, &pos)) {
			line = trim_spaces(line);
			if (line[strlen(line) - 1] == '{') {
				if (ob == 0) {
					new_node =
						(struct cmd_node *)
						malloc(sizeof(struct cmd_node));
					if (!new_node) {
						printf("ERR:Cannot allocate memory for cmd_node\n");
						fclose(fp);
						ret = MLAN_STATUS_FAILURE;
						goto done;
					}
					memset(new_node, 0,
					       sizeof(struct cmd_node));
					new_node->next = NULL;
					if (blk_count == 0) {
						header = new_node;
						command = new_node;
					} else {
						command->next = new_node;
						command = new_node;
					}
					strncpy(command->cmd_string, line,
						(strchr(line, '=') - line));
					memmove(command->cmd_string,
						trim_spaces(command->
							    cmd_string),
						strlen(trim_spaces
						       (command->cmd_string)) +
						1);
				}
				ob++;
				continue;	/* goto while() */
			}
			if (line[strlen(line) - 1] == '}') {
				ob--;
				if (ob == 0)
					blk_count++;
				continue;	/* goto while() */
			}
		}

		rewind(fp);	/* Set the source file pointer to the beginning again */
		command = header;	/* Set 'command' at the beginning of the command list */

		fp_raw = fopen(argv[5], "w");
		if (fp_raw == NULL) {
			fprintf(stderr,
				"Cannot open the destination raw_data file %s\n",
				argv[5]);
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* prepare .dtsi output */
		snprintf(cmdname, sizeof(cmdname), "%s.dtsi", argv[5]);
		fp_dtsi = fopen(cmdname, "w");
		if (fp_dtsi == NULL) {
			fprintf(stderr, "Cannot open the destination file %s\n",
				cmdname);
			fclose(fp);
			fclose(fp_raw);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		for (k = 0; k < blk_count && command != NULL; k++) {
			if (MLAN_STATUS_FAILURE ==
			    prepare_host_cmd_buffer(fp, command->cmd_string,
						    buffer))
				memset(buffer, 0, BUFFER_LENGTH);

			memcpy(&buf_len, buffer, sizeof(t_u32));
			if (buf_len) {
				raw_buf = buffer + sizeof(t_u32);	/* raw_buf points to start of actual <raw data> */
				printf("buf_len = %d\n", (int)buf_len);
				if (k > 0)
					fprintf(fp_raw, "\n\n");
				fprintf(fp_raw, "%s={\n", command->cmd_string);
				fprintf(fp_dtsi,
					"/ {\n\tmarvell_cfgdata {\n\t\tmarvell,%s = /bits/ 8 <\n",
					command->cmd_string);
				i = j = 0;
				while (i < buf_len) {
					for (j = 0; j < 16; j++) {
						fprintf(fp_raw, "%02x ",
							*(raw_buf + i));
						if (i >= 8) {
							fprintf(fp_dtsi,
								"0x%02x",
								*(raw_buf + i));
							if ((j < 16 - 1) &&
							    (i < buf_len - 1))
								fprintf(fp_dtsi,
									" ");
						}
						if (++i >= buf_len)
							break;
					}
					fputc('\n', fp_raw);
					fputc('\n', fp_dtsi);
				}
				fprintf(fp_raw, "}");
				fprintf(fp_dtsi, "\t\t>;\n\t};\n};\n");
			}
			command = command->next;
			rewind(fp);
		}

		fclose(fp_dtsi);
		fclose(fp_raw);
		fclose(fp);
	}

	if (call_ioctl) {
		cmd = (struct eth_priv_cmd *)
			malloc(sizeof(struct eth_priv_cmd));
		if (!cmd) {
			printf("ERR:Cannot allocate buffer for command!\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
		memset(cmd, 0, sizeof(struct eth_priv_cmd));
		memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
		cmd->buf = buffer;
#endif
		cmd->used_len = 0;
		cmd->total_len = BUFFER_LENGTH;

		/* Perform IOCTL */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			perror("mlanutl");
			fprintf(stderr, "mlanutl: hostcmd fail\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Process result */
		process_host_cmd_resp(argv[2], buffer);
	}

done:
	while (header) {
		command = header;
		header = header->next;
		free(command);
	}
	if (line)
		free(line);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process HT Tx configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_httxcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	t_u32 *data = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get result */
		data = (t_u32 *)buffer;
		printf("HT Tx cfg: \n");
		printf("    BG band:  0x%08x\n", data[0]);
		printf("     A band:  0x%08x\n", data[1]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT capability configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_htcapinfo(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_htcapinfo *ht_cap = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		ht_cap = (struct eth_priv_htcapinfo *)buffer;
		printf("HT cap info: \n");
		printf("    BG band:  0x%08x\n", ht_cap->ht_cap_info_bg);
		printf("     A band:  0x%08x\n", ht_cap->ht_cap_info_a);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addbapara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_addba *addba = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbapara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		addba = (struct eth_priv_addba *)buffer;
		printf("Add BA configuration: \n");
		printf("    Time out : %d\n", addba->time_out);
		printf("    TX window: %d\n", addba->tx_win_size);
		printf("    RX window: %d\n", addba->rx_win_size);
		printf("    TX AMSDU : %d\n", addba->tx_amsdu);
		printf("    RX AMSDU : %d\n", addba->rx_amsdu);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process Aggregation priority table parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_aggrpriotbl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: aggrpriotbl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Aggregation priority table cfg: \n");
		printf("    TID      AMPDU      AMSDU \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %3d        %3d \n",
			       i, buffer[2 * i], buffer[2 * i + 1]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA reject configurations
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addbareject(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbareject fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Add BA reject configuration: \n");
		printf("    TID      Reject \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %d\n", i, buffer[i]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Del BA command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_delba(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: delba fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process reject addba req command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rejectaddbareq(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rejectaddbareq fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Reject addba req command response: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Implement Minimum BA Threshold command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_min_ba_threshold_cfg(int argc, char *argv[])
{
	int ret = 0;
	t_u8 min_ba_thres = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX min_ba_threshold [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: min_ba_threshold fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&min_ba_thres, buffer, sizeof(min_ba_thres));
		printf("Min Tx BA Threshold: %d\n", min_ba_thres);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Process VHT configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_vhtcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_vhtcfg vhtcfg;
	struct ifreq ifr;
	t_u8 i, num = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 5) {
		printf("Insufficient parameters\n");
		printf("For STA interface: mlanutl mlanX vhtcfg <band> <txrx> [bwcfg] [vhtcap]\n");
		printf("For uAP interface: mlanutl uapX vhtcfg <band> <txrx> [bwcfg] [vhtcap] [vht_tx_mcs] [vht_rx_mcs]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: vhtcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	/* the first attribute is the number of vhtcfg entries */
	num = *buffer;
	if (argc == 5) {
		/* GET operation */
		printf("11AC VHT Configuration: \n");
		for (i = 0; i < num; i++) {
			memcpy(&vhtcfg, buffer + 1 + i * sizeof(vhtcfg),
			       sizeof(vhtcfg));
			/* Band */
			if (vhtcfg.band == 1)
				printf("Band: 2.4G\n");
			else
				printf("Band: 5G\n");
			/* BW confi9 */

			if (vhtcfg.bwcfg == 0)
				printf("    BW config: Follow BW in the 11N config\n");
			else
				printf("    BW config: Follow BW in VHT Capabilities\n");

			/* Tx/Rx */
			if (vhtcfg.txrx & 0x1)
				printf("    VHT operation for Tx: 0x%08x\n",
				       vhtcfg.vht_cap_info);
			if (vhtcfg.txrx & 0x2)
				/* VHT capabilities */
				printf("    VHT Capabilities Info: 0x%08x\n",
				       vhtcfg.vht_cap_info);
			/* MCS */
			if (vhtcfg.txrx & 0x2) {
				printf("    Tx MCS set: 0x%04x\n",
				       vhtcfg.vht_tx_mcs);
				printf("    Rx MCS set: 0x%04x\n",
				       vhtcfg.vht_rx_mcs);
			}
		}
	} else {
		/* SET operation */
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process Operating Mode Notification configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_opermodecfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_opermodecfg *cfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: opermodecfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	cfg = (struct eth_priv_opermodecfg *)(buffer);
	printf("11AC Operating Mode Notification Configuration: \n");
	printf("    bw: %d\n", cfg->bw);
	printf("    nss: %d\n", cfg->nss);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

static char *rate_format[4] = { "LG", "HT", "VHT", "HE" };

static char *lg_rate[] = { "1 Mbps", "2 Mbps", "5.5 Mbps", "11 Mbps",
	"6 Mbps", "9 Mbps", "12 Mbps", "18 Mbps",
	"24 Mbps", "36 Mbps", "48 Mbps", "54 Mbps"
};

/**
 *  @brief Process Get data rate
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_datarate(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_data_rate *datarate = NULL;
	struct ifreq ifr;
	char *bw[] = { "20 MHz", "40 MHz", "80 MHz", "160 MHz" };

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getdatarate fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	datarate = (struct eth_priv_data_rate *)buffer;
	printf("Data Rate:\n");
	printf("  TX: \n");
	if (datarate->tx_rate_format <= 3) {
		printf("    Type: %s\n", rate_format[datarate->tx_rate_format]);
		if ((datarate->tx_rate_format == 0) &&
		    datarate->tx_data_rate <= 11)
			/* LG */
			printf("    Rate: %s\n",
			       lg_rate[datarate->tx_data_rate]);
		else {
			/* HT and VHT */
			if (datarate->tx_bw <= 3)
				printf("    BW:   %s\n", bw[datarate->tx_bw]);
			if (datarate->tx_rate_format < 3) {
				if (datarate->tx_gi == 0)
					printf("    GI:   Long\n");
				else
					printf("    GI:   Short\n");
			} else if (datarate->tx_rate_format == 3) {
				switch (datarate->tx_gi) {
				case 0:
					printf("    GI:   1xHELTF + GI 0.8us  \n");
					break;
				case 1:
					printf("    GI:   2xHELTF + GI 0.8us  \n");
					break;
				case 2:
					printf("    GI:   2xHELTF + GI 1.6us  \n");
					break;
				case 3:
					printf("    GI:   4xHELTF + GI 0.8us DCM=0 and STBC=0 or\n" "          4xHELTF + GI 3.2us Otherwise  \n");
					break;
				}
			}
			if (datarate->tx_rate_format >= 2)
				printf("    NSS:  %d\n", datarate->tx_nss + 1);
			if (datarate->tx_mcs_index != 0xFF)
				printf("    MCS:  MCS %d\n",
				       (int)datarate->tx_mcs_index);
			else
				printf("    MCS:  Auto\n");
			if (datarate->tx_rate_format < 3)
				printf("    Rate: %f Mbps\n",
				       (float)datarate->tx_data_rate / 2);
		}
	}

	printf("  RX: \n");
	if (datarate->rx_rate_format <= 3) {
		printf("    Type: %s\n", rate_format[datarate->rx_rate_format]);
		if ((datarate->rx_rate_format == 0) &&
		    datarate->rx_data_rate <= 11)
			/* LG */
			printf("    Rate: %s\n",
			       lg_rate[datarate->rx_data_rate]);
		else {
			/* HT and VHT */
			if (datarate->rx_bw <= 3)
				printf("    BW:   %s\n", bw[datarate->rx_bw]);
			if (datarate->rx_rate_format < 3) {
				if (datarate->rx_gi == 0)
					printf("    GI:   Long\n");
				else
					printf("    GI:   Short\n");
			} else if (datarate->rx_rate_format == 3) {
				switch (datarate->rx_gi) {
				case 0:
					printf("    GI:   1xHELTF + GI 0.8us  \n");
					break;
				case 1:
					printf("    GI:   2xHELTF + GI 0.8us  \n");
					break;
				case 2:
					printf("    GI:   2xHELTF + GI 1.6us  \n");
					break;
				case 3:
					printf("    GI:   4xHELTF + GI 0.8us DCM=0 and STBC=0 or\n" "          4xHELTF + GI 3.2us Otherwise  \n");
					break;
				}
			}
			if (datarate->rx_rate_format >= 2)
				printf("    NSS:  %d\n", datarate->rx_nss + 1);
			if (datarate->rx_mcs_index != 0xFF)
				printf("    MCS:  MCS %d\n",
				       (int)datarate->rx_mcs_index);
			else
				printf("    MCS:  Auto\n");
			if (datarate->rx_rate_format < 3)
				printf("    Rate: %f Mbps\n",
				       (float)datarate->rx_data_rate / 2);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx rate configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txratecfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_tx_rate_cfg *txratecfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txratecfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	txratecfg = (struct eth_priv_tx_rate_cfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Tx Rate Configuration: \n");
		/* format */
		if (txratecfg->rate_format == 0xFF) {
			printf("    Type:       0xFF (Auto)\n");
		} else if (txratecfg->rate_format <= 3) {
			printf("    Type:       %d (%s)\n",
			       txratecfg->rate_format,
			       rate_format[txratecfg->rate_format]);
			if (txratecfg->rate_format == 0)
				printf("    Rate Index: %d (%s)\n",
				       txratecfg->rate_index,
				       lg_rate[txratecfg->rate_index]);
			else if (txratecfg->rate_format >= 1)
				printf("    MCS Index:  %d\n",
				       (int)txratecfg->rate_index);
			if (txratecfg->rate_format == 2 ||
			    txratecfg->rate_format == 3)
				printf("    NSS:        %d\n",
				       (int)txratecfg->nss);
			if (txratecfg->rate_setting == 0xffff)
				printf("Rate setting :Preamble type/BW/GI/STBC/.. : auto \n");
			else {
				printf("Preamble type: %x\n",
				       (txratecfg->rate_setting & 0x0003));
				printf("BW: %x\n",
				       (txratecfg->rate_setting & 0x001C) >> 2);
				printf("LTF + GI size %x\n",
				       (txratecfg->rate_setting & 0x0060) >> 5);
				printf("STBC %x\n",
				       (txratecfg->rate_setting & 0x0080) >> 7);
				printf("DCM %x\n",
				       (txratecfg->rate_setting & 0x0100) >> 8);
				printf("Coding %x\n",
				       (txratecfg->rate_setting & 0x0200) >> 9);
				printf("maxPE %x\n",
				       (txratecfg->
					rate_setting & 0x3000) >> 12);
			}
		} else {
			printf("    Unknown rate format.\n");
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process get channel list used by cfg80211
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getcfgchanlist(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i = 0;
	wlan_ieee80211_chan_list *chan_list = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getcfgchanlist\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Process results */
	chan_list = (wlan_ieee80211_chan_list *) buffer;
	printf("Channel Flags:\n");
	printf("            IEEE80211_CHAN_DISABLED		= 1<<0\n");
	printf("            IEEE80211_CHAN_NO_IR		= 1<<1\n");
	printf("            IEEE80211_CHAN_RADAR		= 1<<3\n");
	printf("            IEEE80211_CHAN_NO_HT40PLUS		= 1<<4\n");
	printf("            IEEE80211_CHAN_NO_HT40MINUS		= 1<<5\n");
	printf("            IEEE80211_CHAN_NO_OFDM		= 1<<6\n");
	printf("            IEEE80211_CHAN_NO_80MHZ		= 1<<7\n");
	printf("            IEEE80211_CHAN_NO_160MHZ		= 1<<8\n");
	printf("            IEEE80211_CHAN_INDOOR_ONLY		= 1<<9\n");
	printf("            IEEE80211_CHAN_IR_CONCURRENT	= 1<<10\n");
	printf("            IEEE80211_CHAN_NO_20MHZ		= 1<<11\n");
	printf("            IEEE80211_CHAN_NO_10MHZ		= 1<<12\n");
	printf("--------------------------------------------");
	printf("--------------------------------------------\n");
	printf("# | chan  |  freq  | disable | NO_IR | RADAR |NO_OFDM|NO_HT40+|NO_HT40-|NO_80| dfs_state\n");
	printf("--------------------------------------------");
	printf("--------------------------------------------\n");
	for (i = 0; i < chan_list->num_chan; i++) {
		if (!(chan_list->chan_list[i].flags & IEEE80211_CHAN_RADAR)) {
			printf("    [%03d]     %04d       %c       %c       %c       %c       %c       %c        %c       %c\n", chan_list->chan_list[i].hw_value, chan_list->chan_list[i].center_freq, (chan_list->chan_list[i].flags & IEEE80211_CHAN_DISABLED) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_IR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_RADAR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_OFDM) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40PLUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40MINUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_80MHZ) ? 'Y' : '-', '-');
		} else if (chan_list->chan_list[i].dfs_state == DFS_USEABLE) {
			printf("    [%03d]     %04d       %c       %c       %c       %c       %c       %c        %c       %c\n", chan_list->chan_list[i].hw_value, chan_list->chan_list[i].center_freq, (chan_list->chan_list[i].flags & IEEE80211_CHAN_DISABLED) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_IR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_RADAR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_OFDM) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40PLUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40MINUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_80MHZ) ? 'Y' : '-', 'U');
		} else if (chan_list->chan_list[i].dfs_state == DFS_UNAVAILABLE) {
			printf("    [%03d]     %04d       %c       %c       %c       %c       %c       %c        %c       %c\n", chan_list->chan_list[i].hw_value, chan_list->chan_list[i].center_freq, (chan_list->chan_list[i].flags & IEEE80211_CHAN_DISABLED) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_IR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_RADAR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_OFDM) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40PLUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40MINUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_80MHZ) ? 'Y' : '-', 'N');
		} else if (chan_list->chan_list[i].dfs_state == DFS_AVAILABLE) {
			printf("    [%03d]     %04d       %c       %c       %c       %c       %c       %c        %c       %c\n", chan_list->chan_list[i].hw_value, chan_list->chan_list[i].center_freq, (chan_list->chan_list[i].flags & IEEE80211_CHAN_DISABLED) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_IR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_RADAR) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_OFDM) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40PLUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_HT40MINUS) ? 'Y' : '-', (chan_list->chan_list[i].flags & IEEE80211_CHAN_NO_80MHZ) ? 'Y' : '-', 'A');
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;

}

/**
 *  @brief Process get wireless stats
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getlog(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_get_log *stats = NULL;
	struct ifreq ifr;
	struct timeval tv;
	int i = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getlog fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	gettimeofday(&tv, NULL);

	/* Process results */
	stats = (struct eth_priv_get_log *)buffer;
	printf("Get log: timestamp %d.%06d sec\n", (int)tv.tv_sec,
	       (int)tv.tv_usec);
	printf("dot11GroupTransmittedFrameCount    %u\n"
	       "dot11FailedCount                   %u\n"
	       "dot11RetryCount                    %u\n"
	       "dot11MultipleRetryCount            %u\n"
	       "dot11FrameDuplicateCount           %u\n"
	       "dot11RTSSuccessCount               %u\n"
	       "dot11RTSFailureCount               %u\n"
	       "dot11ACKFailureCount               %u\n"
	       "dot11ReceivedFragmentCount         %u\n"
	       "dot11GroupReceivedFrameCount       %u\n"
	       "dot11FCSErrorCount                 %u\n"
	       "dot11TransmittedFrameCount         %u\n"
	       "wepicverrcnt-1                     %u\n"
	       "wepicverrcnt-2                     %u\n"
	       "wepicverrcnt-3                     %u\n"
	       "wepicverrcnt-4                     %u\n"
	       "beaconReceivedCount                %u\n"
	       "beaconMissedCount                  %u\n", stats->mcast_tx_frame,
	       stats->failed, stats->retry, stats->multi_retry,
	       stats->frame_dup, stats->rts_success, stats->rts_failure,
	       stats->ack_failure, stats->rx_frag, stats->mcast_rx_frame,
	       stats->fcs_error, stats->tx_frame, stats->wep_icv_error[0],
	       stats->wep_icv_error[1], stats->wep_icv_error[2],
	       stats->wep_icv_error[3], stats->bcn_rcv_cnt,
	       stats->bcn_miss_cnt);
	if (cmd->used_len == sizeof(struct eth_priv_get_log)) {
		printf("dot11TransmittedFragmentCount      %u\n",
		       stats->tx_frag_cnt);
		printf("dot11QosTransmittedFragmentCount   ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_tx_frag_cnt[i]);
		}
		printf("\ndot11QosFailedCount                ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_failed_cnt[i]);
		}
		printf("\ndot11QosRetryCount                 ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_retry_cnt[i]);
		}
		printf("\ndot11QosMultipleRetryCount         ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_multi_retry_cnt[i]);
		}
		printf("\ndot11QosFrameDuplicateCount        ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_frm_dup_cnt[i]);
		}
		printf("\ndot11QosRTSSuccessCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rts_suc_cnt[i]);
		}
		printf("\ndot11QosRTSFailureCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rts_failure_cnt[i]);
		}
		printf("\ndot11QosACKFailureCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_ack_failure_cnt[i]);
		}
		printf("\ndot11QosReceivedFragmentCount      ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rx_frag_cnt[i]);
		}
		printf("\ndot11QosTransmittedFrameCount      ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_tx_frm_cnt[i]);
		}
		printf("\ndot11QosDiscardedFrameCount        ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_discarded_frm_cnt[i]);
		}
		printf("\ndot11QosMPDUsReceivedCount         ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_mpdus_rx_cnt[i]);
		}
		printf("\ndot11QosRetriesReceivedCount       ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_retries_rx_cnt[i]);
		}
		printf("\ndot11RSNAStatsCMACICVErrors          %u\n"
		       "dot11RSNAStatsCMACReplays            %u\n"
		       "dot11RSNAStatsRobustMgmtCCMPReplays  %u\n"
		       "dot11RSNAStatsTKIPICVErrors          %u\n"
		       "dot11RSNAStatsTKIPReplays            %u\n"
		       "dot11RSNAStatsCCMPDecryptErrors      %u\n"
		       "dot11RSNAstatsCCMPReplays            %u\n"
		       "dot11TransmittedAMSDUCount           %u\n"
		       "dot11FailedAMSDUCount                %u\n"
		       "dot11RetryAMSDUCount                 %u\n"
		       "dot11MultipleRetryAMSDUCount         %u\n"
		       "dot11TransmittedOctetsInAMSDUCount   %llu\n"
		       "dot11AMSDUAckFailureCount            %u\n"
		       "dot11ReceivedAMSDUCount              %u\n"
		       "dot11ReceivedOctetsInAMSDUCount      %llu\n"
		       "dot11TransmittedAMPDUCount           %u\n"
		       "dot11TransmittedMPDUsInAMPDUCount    %u\n"
		       "dot11TransmittedOctetsInAMPDUCount   %llu\n"
		       "dot11AMPDUReceivedCount              %u\n"
		       "dot11MPDUInReceivedAMPDUCount        %u\n"
		       "dot11ReceivedOctetsInAMPDUCount      %llu\n"
		       "dot11AMPDUDelimiterCRCErrorCount     %u\n",
		       stats->cmacicv_errors,
		       stats->cmac_replays,
		       stats->mgmt_ccmp_replays,
		       stats->tkipicv_errors,
		       stats->tkip_replays,
		       stats->ccmp_decrypt_errors,
		       stats->ccmp_replays,
		       stats->tx_amsdu_cnt,
		       stats->failed_amsdu_cnt,
		       stats->retry_amsdu_cnt,
		       stats->multi_retry_amsdu_cnt,
		       stats->tx_octets_in_amsdu_cnt,
		       stats->amsdu_ack_failure_cnt,
		       stats->rx_amsdu_cnt,
		       stats->rx_octets_in_amsdu_cnt,
		       stats->tx_ampdu_cnt,
		       stats->tx_mpdus_in_ampdu_cnt,
		       stats->tx_octets_in_ampdu_cnt,
		       stats->ampdu_rx_cnt,
		       stats->mpdu_in_rx_ampdu_cnt,
		       stats->rx_octets_in_ampdu_cnt,
		       stats->ampdu_delimiter_crc_error_cnt);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process esuppmode command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_esuppmode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_esuppmode_cfg *esuppmodecfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: esuppmode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	esuppmodecfg = (struct eth_priv_esuppmode_cfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Esupplicant Mode Configuration: \n");
		/* RSN mode */
		printf("    RSN mode:         0x%x ( ", esuppmodecfg->rsn_mode);
		if (esuppmodecfg->rsn_mode & MBIT(0))
			printf("No-RSN ");
		if (esuppmodecfg->rsn_mode & MBIT(3))
			printf("WPA ");
		if (esuppmodecfg->rsn_mode & MBIT(4))
			printf("WPA-None ");
		if (esuppmodecfg->rsn_mode & MBIT(5))
			printf("WPA2 ");
		printf(")\n");
		/* Pairwise cipher */
		printf("    Pairwise cipher:  0x%x ( ",
		       esuppmodecfg->pairwise_cipher);
		if (esuppmodecfg->pairwise_cipher & MBIT(2))
			printf("TKIP ");
		if (esuppmodecfg->pairwise_cipher & MBIT(3))
			printf("AES ");
		printf(")\n");
		/* Group cipher */
		printf("    Group cipher:     0x%x ( ",
		       esuppmodecfg->group_cipher);
		if (esuppmodecfg->group_cipher & MBIT(2))
			printf("TKIP ");
		if (esuppmodecfg->group_cipher & MBIT(3))
			printf("AES ");
		printf(")\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process passphrase command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_passphrase(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_NXP) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: passphrase fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Passphrase Configuration: %s\n", (char *)buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process deauth command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_deauth(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_NXP) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: deauth fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef UAP_SUPPORT
/**
 *  @brief Process getstalist command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getstalist(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_getstalist *list = NULL;
	int i = 0, rssi = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getstalist fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	list = (struct eth_priv_getstalist *)(buffer + strlen(CMD_NXP) +
					      strlen(argv[2]));

	printf("Number of STA = %d\n\n", list->sta_count);

	for (i = 0; i < list->sta_count; i++) {
		printf("STA %d information:\n", i + 1);
		printf("=====================\n");
		printf("MAC Address: ");
		print_mac(list->client_info[i].mac_address);
		printf("\nPower mgmt status: %s\n",
		       (list->client_info[i].power_mgmt_status ==
			0) ? "active" : "power save");

	/** On some platform, s8 is same as unsigned char*/
		rssi = (int)list->client_info[i].rssi;
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("Rssi : %d dBm\n\n", rssi);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
/**
 *  @brief Process BSS role command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bssrole(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bssrole fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("BSS role: %d\n", buffer[0]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif
#endif

#ifdef STA_SUPPORT
/**
 *  @brief Helper function for process_getscantable_idx
 *
 *  @param pbuf     A pointer to the buffer
 *  @param buf_len  Buffer length
 *
 *  @return         NA
 *
 */
static void
dump_scan_elems(const t_u8 *pbuf, uint buf_len)
{
	uint idx;
	uint marker = 2 + pbuf[1];

	for (idx = 0; idx < buf_len; idx++) {
		if (idx % 0x10 == 0) {
			printf("\n%04x: ", idx);
		}

		if (idx == marker) {
			printf("|");
			marker = idx + pbuf[idx + 1] + 2;
		} else {
			printf(" ");
		}

		printf("%02x ", pbuf[idx]);
	}

	printf("\n");
}

/**
 *  @brief Helper function for process_getscantable_idx
 *  Find next element
 *
 *  @param pp_ie_out    Pointer of a IEEEtypes_Generic_t structure pointer
 *  @param p_buf_left   Integer pointer, which contains the number of left p_buf
 *
 *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
 */
static int
scantable_elem_next(IEEEtypes_Generic_t **pp_ie_out, int *p_buf_left)
{
	IEEEtypes_Generic_t *pie_gen;
	t_u8 *p_next;

	if (*p_buf_left < 2) {
		return MLAN_STATUS_FAILURE;
	}

	pie_gen = *pp_ie_out;

	p_next = (t_u8 *)pie_gen + (pie_gen->ieee_hdr.len
				    + sizeof(pie_gen->ieee_hdr));
	*p_buf_left -= (p_next - (t_u8 *)pie_gen);

	*pp_ie_out = (IEEEtypes_Generic_t *)p_next;

	if (*p_buf_left <= 0) {
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

 /**
  *  @brief Helper function for process_getscantable_idx
  *         scantable find element
  *
  *  @param ie_buf       Pointer of the IE buffer
  *  @param ie_buf_len   IE buffer length
  *  @param ie_type      IE type
  *  @param ppie_out     Pointer to the IEEEtypes_Generic_t structure pointer
  *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
  */
static int
scantable_find_elem(t_u8 *ie_buf,
		    unsigned int ie_buf_len,
		    IEEEtypes_ElementId_e ie_type,
		    IEEEtypes_Generic_t **ppie_out)
{
	int found;
	unsigned int ie_buf_left;

	ie_buf_left = ie_buf_len;

	found = FALSE;

	*ppie_out = (IEEEtypes_Generic_t *)ie_buf;

	do {
		found = ((*ppie_out)->ieee_hdr.element_id == ie_type);

	} while (!found &&
		 (scantable_elem_next(ppie_out, (int *)&ie_buf_left) == 0));

	if (!found) {
		*ppie_out = NULL;
	}

	return found ? MLAN_STATUS_SUCCESS : MLAN_STATUS_FAILURE;
}

 /**
  *  @brief Helper function for process_getscantable_idx
  *         It gets SSID from IE
  *
  *  @param ie_buf       IE buffer
  *  @param ie_buf_len   IE buffer length
  *  @param pssid        SSID
  *  @param ssid_buf_max Size of SSID
  *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
  */
static int
scantable_get_ssid_from_ie(t_u8 *ie_buf,
			   unsigned int ie_buf_len,
			   t_u8 *pssid, unsigned int ssid_buf_max)
{
	int retval;
	IEEEtypes_Generic_t *pie_gen;

	retval = scantable_find_elem(ie_buf, ie_buf_len, SSID, &pie_gen);
	if (retval == MLAN_STATUS_SUCCESS)
		memcpy(pssid, pie_gen->data,
		       MIN(pie_gen->ieee_hdr.len, ssid_buf_max));
	else
		return MLAN_STATUS_FAILURE;

	return retval;
}

/**
 *  @brief Display detailed information for a specific scan table entry
 *
 *  @param cmd_name         Command name
 *  @param prsp_info_req    Scan table entry request structure
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_getscantable_idx(char *cmd_name,
			 wlan_ioctl_get_scan_table_info *prsp_info_req)
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	t_u8 *pcurrent;
	char ssid[33];
	t_u16 tmp_cap;
	t_u8 tsf[8];
	t_u16 beacon_interval;
	t_u16 cap_info;
	wlan_ioctl_get_scan_table_info *prsp_info;

	wlan_get_scan_table_fixed fixed_fields;
	t_u32 fixed_field_length;
	t_u32 bss_info_length;

	memset(ssid, 0x00, sizeof(ssid));

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, cmd_name, 0, NULL);

	prsp_info =
		(wlan_ioctl_get_scan_table_info *)(buffer + strlen(CMD_NXP) +
						   strlen(cmd_name));

	memcpy(prsp_info, prsp_info_req,
	       sizeof(wlan_ioctl_get_scan_table_info));

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/*
	 * Set up and execute the ioctl call
	 */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		if (errno == EAGAIN) {
			ret = -EAGAIN;
		} else {
			perror("mlanutl");
			fprintf(stderr, "mlanutl: getscantable fail\n");
			ret = MLAN_STATUS_FAILURE;
		}
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return ret;
	}

	prsp_info = (wlan_ioctl_get_scan_table_info *)buffer;
	if (prsp_info->scan_number == 0) {
		printf("mlanutl: getscantable ioctl - index out of range\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return -EINVAL;
	}

	pcurrent = prsp_info->scan_table_entry_buf;

	memcpy((t_u8 *)&fixed_field_length,
	       (t_u8 *)pcurrent, sizeof(fixed_field_length));
	pcurrent += sizeof(fixed_field_length);

	memcpy((t_u8 *)&bss_info_length,
	       (t_u8 *)pcurrent, sizeof(bss_info_length));
	pcurrent += sizeof(bss_info_length);

	memcpy((t_u8 *)&fixed_fields, (t_u8 *)pcurrent, sizeof(fixed_fields));
	pcurrent += fixed_field_length;

	/* Time stamp is 8 byte long */
	memcpy(tsf, pcurrent, sizeof(tsf));
	pcurrent += sizeof(tsf);
	bss_info_length -= sizeof(tsf);

	/* Beacon interval is 2 byte long */
	memcpy(&beacon_interval, pcurrent, sizeof(beacon_interval));
	pcurrent += sizeof(beacon_interval);
	bss_info_length -= sizeof(beacon_interval);

	/* Capability information is 2 byte long */
	memcpy(&cap_info, pcurrent, sizeof(cap_info));
	pcurrent += sizeof(cap_info);
	bss_info_length -= sizeof(cap_info);

	scantable_get_ssid_from_ie(pcurrent,
				   bss_info_length, (t_u8 *)ssid, sizeof(ssid));

	printf("\n*** [%s], %02x:%02x:%02x:%02x:%02x:%2x\n",
	       ssid,
	       fixed_fields.bssid[0],
	       fixed_fields.bssid[1],
	       fixed_fields.bssid[2],
	       fixed_fields.bssid[3],
	       fixed_fields.bssid[4], fixed_fields.bssid[5]);
	memcpy(&tmp_cap, &cap_info, sizeof(tmp_cap));
	printf("Channel = %d, SS = %d, ChanLoad = %d%% CapInfo = 0x%04x, BcnIntvl = %d\n", fixed_fields.channel, 255 - fixed_fields.rssi, fixed_fields.chan_load, tmp_cap, beacon_interval);

	printf("TSF Values: AP(0x%02x%02x%02x%02x%02x%02x%02x%02x), ",
	       tsf[7], tsf[6], tsf[5], tsf[4], tsf[3], tsf[2], tsf[1], tsf[0]);

	printf("Network(0x%016llx)\n", fixed_fields.network_tsf);
	printf("\n");
	printf("Element Data (%d bytes)\n", (int)bss_info_length);
	printf("------------");
	dump_scan_elems(pcurrent, bss_info_length);
	printf("\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief channel validation.
 *  @param scan_req  A pointer to wlan_ioctl_user_scan_cfg structure
 *  @param chan_num  channel number
 *
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
wlan_is_channel_valid(wlan_ioctl_user_scan_cfg *scan_req, t_u8 chan_number)
{
	int ret = -1;
	int i;
	if (scan_req && scan_req->chan_list[0].chan_number) {
		for (i = 0;
		     i < WLAN_IOCTL_USER_SCAN_CHAN_MAX &&
		     scan_req->chan_list[i].chan_number; i++) {
			if (scan_req->chan_list[i].chan_number == chan_number) {
				ret = 0;
				break;
			}
		}
	} else {
		ret = 0;
	}
	return ret;
}

/**
 *  @brief filter_ssid_result
 *  @param scan_req  A pointer to wlan_ioctl_user_scan_cfg structure
 *  @param num_ssid_rqst Number of SSIDs which are filterted
 *  @param bss_info  A pointer to current bss information structure
 *  @return 0--success, otherwise--fail
 */

int
filter_ssid_result(wlan_ioctl_user_scan_cfg *scan_req, int num_ssid_rqst,
		   wlan_ioctl_get_bss_info *bss_info)
{
	int i, ret = 1;

	for (i = 0; i < num_ssid_rqst; i++) {
		if ((memcmp(scan_req->ssid_list[i].ssid, bss_info->ssid,
			    (int)bss_info->ssid_len)) == 0) {
			return 0;
		}
	}
	return ret;
}

/**
 *  @brief Process getchanstats
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wlan_process_getchanstats(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int ret = 0;
	int idx = 0;
	int i = 0;
	chan_stats *stats = NULL;
	t_u16 chan_load = 0;

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	prepare_buffer(buffer, argv[2], 0, NULL);

	/* Set up and execute the ioctl call */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getchload fail\n");
		ret = MLAN_STATUS_FAILURE;
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return ret;
	}

	stats = (chan_stats *) buffer;
	/* Display scan results */
	printf("----------------------------------------------------------- \n");
	printf("# | Channel |  BSS  |  minrss  |  maxrss  |  NF   | CH load \n");
	printf("----------------------------------------------------------- \n");

	for (i = 0; i < stats->num_in_chan_stats; i++) {
		if (stats->stats[i].cca_scan_duration) {
			chan_load =
				stats->stats[i].cca_busy_duration * 100 /
				stats->stats[i].cca_scan_duration;
			printf("%02u|   %03d   |  %03d  |    %02d    |    %02d    |  %02d  |   %02d\n", idx, stats->stats[i].chan_num, stats->stats[i].total_networks, stats->stats[i].min_rss, stats->stats[i].max_rss, stats->stats[i].noise, chan_load);
			idx++;
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process getscantable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @param scan_req  A pointer to wlan_ioctl_user_scan_cfg structure
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wlan_process_getscantable(int argc, char *argv[],
			  wlan_ioctl_user_scan_cfg *scan_req)
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	struct wlan_ioctl_get_scan_list *scan_list_head = NULL;
	struct wlan_ioctl_get_scan_list *scan_list_node = NULL;
	struct wlan_ioctl_get_scan_list *curr = NULL, *next = NULL;

	t_u32 total_scan_res = 0;

	unsigned int scan_start;
	int idx, ret = 0;
	int i = 0;
	t_u8 *pcurrent;
	t_u8 *pnext;
	IEEEtypes_ElementId_e *pelement_id;
	t_u8 *pelement_len;
	int ssid_idx;
	int insert = 0;
	int sort_by_channel = 0;
	t_u8 *pbyte;
	t_u16 new_ss = 0;
	t_u16 curr_ss = 0;
	t_u8 new_ch = 0;
	t_u8 curr_ch = 0;

	IEEEtypes_VendorSpecific_t *pwpa_ie;
	const t_u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };

	IEEEtypes_WmmParameter_t *pwmm_ie;
	const t_u8 wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };
	IEEEtypes_VendorSpecific_t *pwps_ie;
	const t_u8 wps_oui[4] = { 0x00, 0x50, 0xf2, 0x04 };

	int displayed_info;

	wlan_ioctl_get_scan_table_info rsp_info_req;
	wlan_ioctl_get_scan_table_info *prsp_info;

	wlan_get_scan_table_fixed fixed_fields;
	t_u32 fixed_field_length;
	t_u32 bss_info_length;
	wlan_ioctl_get_bss_info *bss_info;

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	if (argc > 3 && (strcmp(argv[3], "tsf") != 0)
	    && (strcmp(argv[3], "help") != 0)
	    && (strcmp(argv[3], "ch") != 0)) {

		idx = strtol(argv[3], NULL, 10);

		if (idx >= 0) {
			rsp_info_req.scan_number = idx;
			ret = process_getscantable_idx(argv[2], &rsp_info_req);
			if (buffer)
				free(buffer);
			if (cmd)
				free(cmd);
			return ret;
		}
	}

	displayed_info = FALSE;
	scan_start = 1;

	do {
		prepare_buffer(buffer, argv[2], 0, NULL);
		prsp_info =
			(wlan_ioctl_get_scan_table_info *)(buffer +
							   strlen(CMD_NXP) +
							   strlen(argv[2]));

		prsp_info->scan_number = scan_start;

		/*
		 * Set up and execute the ioctl call
		 */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			if (errno == EAGAIN) {
				ret = -EAGAIN;
			} else {
				perror("mlanutl");
				fprintf(stderr, "mlanutl: getscantable fail\n");
				ret = MLAN_STATUS_FAILURE;
			}
			if (cmd)
				free(cmd);
			if (buffer)
				free(buffer);
			return ret;
		}

		prsp_info = (wlan_ioctl_get_scan_table_info *)buffer;
		pcurrent = 0;
		pnext = prsp_info->scan_table_entry_buf;
		total_scan_res += prsp_info->scan_number;

		for (idx = 0; (unsigned int)idx < prsp_info->scan_number; idx++) {
			/* Alloc memory for new node for next BSS */
			scan_list_node = (struct wlan_ioctl_get_scan_list *)
				malloc(sizeof(struct wlan_ioctl_get_scan_list));
			if (scan_list_node == NULL) {
				printf("Error: allocate memory for scan_list_head failed\n");
				return -ENOMEM;
			}
			memset(scan_list_node, 0,
			       sizeof(struct wlan_ioctl_get_scan_list));

			/*
			 * Set pcurrent to pnext in case pad bytes are at the end
			 *   of the last IE we processed.
			 */
			pcurrent = pnext;

			/* Start extracting each BSS to prepare a linked list */
			memcpy((t_u8 *)&fixed_field_length,
			       (t_u8 *)pcurrent, sizeof(fixed_field_length));
			pcurrent += sizeof(fixed_field_length);

			memcpy((t_u8 *)&bss_info_length,
			       (t_u8 *)pcurrent, sizeof(bss_info_length));
			pcurrent += sizeof(bss_info_length);

			memcpy((t_u8 *)&fixed_fields,
			       (t_u8 *)pcurrent, sizeof(fixed_fields));
			pcurrent += fixed_field_length;

			scan_list_node->fixed_buf.fixed_field_length =
				fixed_field_length;
			scan_list_node->fixed_buf.bss_info_length =
				bss_info_length;
			scan_list_node->fixed_buf.fixed_fields = fixed_fields;

			bss_info = &scan_list_node->bss_info_buf;

			/* Set next to be the start of the next scan entry */
			pnext = pcurrent + bss_info_length;

			if (bss_info_length >=
			    (sizeof(bss_info->tsf) +
			     sizeof(bss_info->beacon_interval) +
			     sizeof(bss_info->cap_info))) {

				/* time stamp is 8 byte long */
				memcpy(bss_info->tsf, pcurrent,
				       sizeof(bss_info->tsf));
				pcurrent += sizeof(bss_info->tsf);
				bss_info_length -= sizeof(bss_info->tsf);

				/* beacon interval is 2 byte long */
				memcpy(&bss_info->beacon_interval, pcurrent,
				       sizeof(bss_info->beacon_interval));
				pcurrent += sizeof(bss_info->beacon_interval);
				bss_info_length -=
					sizeof(bss_info->beacon_interval);
				/* capability information is 2 byte long */
				memcpy(&bss_info->cap_info, pcurrent,
				       sizeof(bss_info->cap_info));
				pcurrent += sizeof(bss_info->cap_info);
				bss_info_length -= sizeof(bss_info->cap_info);
			}

			bss_info->wmm_cap = ' ';	/* M (WMM), C (WMM-Call Admission Control) */
			bss_info->wps_cap = ' ';	/* "S" */
			bss_info->dot11k_cap = ' ';	/* "K" */
			bss_info->dot11r_cap = ' ';	/* "R" */
			bss_info->ht_cap = ' ';	/* "N" */
			bss_info->vht_cap[0] = ' ';
			bss_info->vht_cap[1] = ' ';
			/* "P" for Privacy (WEP) since "W" is WPA, and "2" is RSN/WPA2 */
			bss_info->priv_cap =
				bss_info->cap_info.privacy ? 'P' : ' ';

			memset(bss_info->ssid, 0, MRVDRV_MAX_SSID_LENGTH + 1);
			bss_info->ssid_len = 0;

			while (bss_info_length >= 2) {
				pelement_id = (IEEEtypes_ElementId_e *)pcurrent;
				pelement_len = pcurrent + 1;
				pcurrent += 2;

				if ((bss_info_length - 2) < *pelement_len) {
					printf("Error when processing bss info, bss_info_length < element length\n");
					bss_info_length = 0;
					continue;
				}

				switch (*pelement_id) {
				case SSID:
					if (*pelement_len &&
					    *pelement_len <=
					    MRVDRV_MAX_SSID_LENGTH) {
						memcpy(bss_info->ssid, pcurrent,
						       *pelement_len);
						bss_info->ssid_len =
							*pelement_len;
					}
					break;

				case WPA_IE:
					pwpa_ie =
						(IEEEtypes_VendorSpecific_t *)
						pelement_id;
					if ((memcmp
					     (pwpa_ie->vend_hdr.oui, wpa_oui,
					      sizeof(pwpa_ie->vend_hdr.oui)) ==
					     0)
					    && (pwpa_ie->vend_hdr.oui_type ==
						wpa_oui[3])) {
						/* WPA IE found, 'W' for WPA */
						bss_info->priv_cap = 'W';
					} else {
						pwmm_ie =
							(IEEEtypes_WmmParameter_t
							 *)pelement_id;
						if ((memcmp
						     (pwmm_ie->vend_hdr.oui,
						      wmm_oui,
						      sizeof(pwmm_ie->vend_hdr.
							     oui)) == 0)
						    && (pwmm_ie->vend_hdr.
							oui_type ==
							wmm_oui[3])) {
							/* Check the subtype: 1 == parameter, 0 == info */
							if ((pwmm_ie->vend_hdr.
							     oui_subtype == 1)
							    && pwmm_ie->
							    ac_params
							    [WMM_AC_VO].
							    aci_aifsn.acm) {
								/* Call admission on VO; 'C' for CAC */
								bss_info->
									wmm_cap
									= 'C';
							} else {
								/* No CAC; 'M' for uh, WMM */
								bss_info->
									wmm_cap
									= 'M';
							}
						} else {
							pwps_ie =
								(IEEEtypes_VendorSpecific_t
								 *)pelement_id;
							if ((memcmp
							     (pwps_ie->vend_hdr.
							      oui, wps_oui,
							      sizeof(pwps_ie->
								     vend_hdr.
								     oui)) == 0)
							    && (pwps_ie->
								vend_hdr.
								oui_type ==
								wps_oui[3])) {
								bss_info->
									wps_cap
									= 'S';
							}
						}
					}
					break;

				case RSN_IE:
					/* RSN IE found; '2' for WPA2 (RSN) */
					bss_info->priv_cap = '2';
					break;
				case HT_CAPABILITY:
					bss_info->ht_cap = 'N';
					break;
				case VHT_CAPABILITY:
					bss_info->vht_cap[0] = 'A';
					bss_info->vht_cap[1] = 'C';
					break;
				default:
					break;
				}

				pcurrent += *pelement_len;
				bss_info_length -= (2 + *pelement_len);
			}

			/* Create a sorted list of BSS using Insertion Sort. */
			if ((argc > 3) && !strcmp(argv[3], "ch")) {
				/* Sort by channel number (ascending order) */
				new_ch = fixed_fields.channel;
				sort_by_channel = 1;
			} else {
				/* Sort as per Signal Strength (descending order) (Default case) */
				new_ss = 255 - fixed_fields.rssi;
			}
			if (scan_list_head == NULL) {
				/* Node is the first element in the list. */
				scan_list_head = scan_list_node;
				scan_list_node->next = NULL;
				scan_list_node->prev = NULL;
			} else {
				curr = scan_list_head;
				insert = 0;
				do {
					if (sort_by_channel) {
						curr_ch =
							curr->fixed_buf.
							fixed_fields.channel;
						if (new_ch < curr_ch)
							insert = 1;
					} else {
						curr_ss =
							255 -
							curr->fixed_buf.
							fixed_fields.rssi;
						if (new_ss > curr_ss) {
							insert = 1;
						}
					}
					if (insert) {
						if (curr == scan_list_head) {
							/* Insert the node to head of the list */
							scan_list_node->next =
								scan_list_head;
							scan_list_head->prev =
								scan_list_node;
							scan_list_head =
								scan_list_node;
						} else {
							/* Insert the node to current position in list */
							scan_list_node->prev =
								curr->prev;
							scan_list_node->next =
								curr;
							(curr->prev)->next =
								scan_list_node;
							curr->prev =
								scan_list_node;
						}
						break;
					}
					if (curr->next == NULL) {
						/* Insert the node to tail of the list */
						curr->next = scan_list_node;
						scan_list_node->prev = curr;
						scan_list_node->next = NULL;
						break;
					}
					curr = curr->next;
				} while (curr != NULL);
			}
		}
		scan_start += prsp_info->scan_number;
	} while (prsp_info->scan_number);

	/* Display scan results */
	printf("---------------------------------------");
	printf("---------------------------------------\n");
	printf("# | ch  | ss  | ld  |       bssid       |   cap      |   SSID \n");
	printf("---------------------------------------");
	printf("---------------------------------------\n");

	for (curr = scan_list_head, idx = 0;
	     (curr != NULL) && ((unsigned int)idx < total_scan_res);
	     curr = curr->next, idx++) {
		fixed_fields = curr->fixed_buf.fixed_fields;
		bss_info = &curr->bss_info_buf;
		if (wlan_is_channel_valid(scan_req, fixed_fields.channel))
			continue;

		if (setuserscan_filter == BSSID_FILTER) {
			if (scan_req->bssid_num) {
				for (i = 0; i < scan_req->bssid_num; i++) {
					if (memcmp
					    (scan_req->bssid_list[i].bssid,
					     fixed_fields.bssid, ETH_ALEN))
						continue;
				}
			} else if (memcmp
				   (scan_req->specific_bssid,
				    fixed_fields.bssid, ETH_ALEN))
				continue;
		}
		if (setuserscan_filter == SSID_FILTER) {
			if (filter_ssid_result
			    (scan_req, num_ssid_filter, bss_info))
				continue;
		}
		printf("%02u| %03d | %03d | %03d | %02x:%02x:%02x:%02x:%02x:%02x |", idx, fixed_fields.channel, -fixed_fields.rssi, fixed_fields.chan_load, fixed_fields.bssid[0], fixed_fields.bssid[1], fixed_fields.bssid[2], fixed_fields.bssid[3], fixed_fields.bssid[4], fixed_fields.bssid[5]);
		displayed_info = TRUE;

		/* "A" for Adhoc
		 * "I" for Infrastructure,
		 * "D" for DFS (Spectrum Mgmt)
		 */
		printf(" %c%c%c%c%c%c%c%c%c%c | ", bss_info->cap_info.ibss ? 'A' : 'I', bss_info->priv_cap,	/* P (WEP), W (WPA), 2 (WPA2) */
		       bss_info->cap_info.spectrum_mgmt ? 'D' : ' ', bss_info->wmm_cap,	/* M (WMM), C (WMM-Call Admission Control) */
		       bss_info->dot11k_cap,	/* K */
		       bss_info->dot11r_cap,	/* R */
		       bss_info->wps_cap,	/* S */
		       bss_info->ht_cap,	/* N */
		       bss_info->vht_cap[0],	/* AC */
		       bss_info->vht_cap[1]);

		/* Print out the ssid or the hex values if non-printable */
		for (ssid_idx = 0; ssid_idx < (int)bss_info->ssid_len;
		     ssid_idx++) {
			if (isprint(bss_info->ssid[ssid_idx])) {
				printf("%c", bss_info->ssid[ssid_idx]);
			} else {
				printf("\\%02x", bss_info->ssid[ssid_idx]);
			}
		}

		printf("\n");

		if (argc > 3 && strcmp(argv[3], "tsf") == 0) {
			/* TSF is a u64, some formatted printing libs have trouble
			   printing long longs, so cast and dump as bytes */
			pbyte = (t_u8 *)&fixed_fields.network_tsf;
			printf("    TSF=%02x%02x%02x%02x%02x%02x%02x%02x\n",
			       pbyte[7], pbyte[6], pbyte[5], pbyte[4],
			       pbyte[3], pbyte[2], pbyte[1], pbyte[0]);
		}
	}

	if (displayed_info == TRUE) {
		if (argc > 3 && strcmp(argv[3], "help") == 0) {
			printf("\n\n"
			       "Capability Legend (Not all may be supported)\n"
			       "-----------------\n"
			       " I [ Infrastructure ]\n"
			       " A [ Ad-hoc ]\n"
			       " W [ WPA IE ]\n"
			       " 2 [ WPA2/RSN IE ]\n"
			       " M [ WMM IE ]\n"
			       " C [ Call Admission Control - WMM IE, VO ACM set ]\n"
			       " D [ Spectrum Management - DFS (11h) ]\n"
			       " K [ 11k ]\n"
			       " R [ 11r ]\n"
			       " S [ WPS ]\n"
			       " N [ HT (11n) ]\n"
			       " AC [VHT (11ac) ]\n" "\n\n");
		}
	} else {
		printf("< No Scan Results >\n");
	}

	for (curr = scan_list_head; curr != NULL; curr = next) {
		next = curr->next;
		free(curr);
	}
	argv[2] = "getchanstats";
	process_getchanstats(argc, argv);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process getschstats command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getchanstats(int argc, char *argv[])
{
	return wlan_process_getchanstats(argc, argv);
}

/**
 *  @brief Process getscantable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @param scan_req  A pointer to wlan_ioctl_user_scan_cfg structure
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getscantable(int argc, char *argv[])
{
	return wlan_process_getscantable(argc, argv, NULL);
}

/**
 *  @brief Prepare setuserscan command buffer
 *  @param scan_req pointer to wlan_ioctl_user_scan_cfg structure
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int
prepare_setuserscan_buffer(wlan_ioctl_user_scan_cfg *scan_req, t_u32 num,
			   char *args[])
{
	int arg_idx = 0;
	int num_ssid = 0;
	int num_bssid = 0;
	char *parg_tok = NULL;
	char *pchan_tok = NULL;
	char *parg_cookie = NULL;
	char *pchan_cookie = NULL;
	int chan_parse_idx = 0;
	int chan_cmd_idx = 0;
	char chan_scratch[MAX_CHAN_SCRATCH];
	char *pscratch = NULL;
	int tmp_idx = 0;
	int scan_time = 0;
	int is_radio_set = 0;
	unsigned int mac[ETH_ALEN];

	for (arg_idx = 0; arg_idx < (int)num; arg_idx++) {
		if (strncmp(args[arg_idx], "ssid=", strlen("ssid=")) == 0) {
			/* "ssid" token string handler */
			if (num_ssid < MRVDRV_MAX_SSID_LIST_LENGTH) {
				strncpy(scan_req->ssid_list[num_ssid].ssid,
					args[arg_idx] + strlen("ssid="),
					sizeof(scan_req->ssid_list[num_ssid].
					       ssid));

				scan_req->ssid_list[num_ssid].max_len = 0;
				setuserscan_filter = SSID_FILTER;
				num_ssid++;
				num_ssid_filter++;
			}
		} else if (strncmp(args[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			/* "bssid" token string handler */
			if (num_bssid < MAX_BSSID_FILTER_LIST) {
				sscanf(args[arg_idx] + strlen("bssid="),
				       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0,
				       mac + 1, mac + 2, mac + 3, mac + 4,
				       mac + 5);
				setuserscan_filter = BSSID_FILTER;
				for (tmp_idx = 0;
				     (unsigned int)tmp_idx < NELEMENTS(mac);
				     tmp_idx++) {
					scan_req->bssid_list[num_bssid].
						bssid[tmp_idx] =
						(t_u8)mac[tmp_idx];
				}
				/* Update BSSID into specific_bssid field */
				if (scan_req->bssid_num == 0) {
					for (tmp_idx = 0;
					     (unsigned int)tmp_idx <
					     NELEMENTS(mac); tmp_idx++) {
						scan_req->
							specific_bssid[tmp_idx]
							= (t_u8)mac[tmp_idx];
					}
				}
				num_bssid++;
				scan_req->bssid_num = num_bssid;
			}

		} else if (strncmp(args[arg_idx], "chan=", strlen("chan=")) ==
			   0) {
			/* "chan" token string handler */
			parg_tok = args[arg_idx] + strlen("chan=");

			if (strlen(parg_tok) > MAX_CHAN_SCRATCH) {
				printf("Error: Specified channels exceeds max limit\n");
				return MLAN_STATUS_FAILURE;
			}
			is_radio_set = FALSE;

			while ((parg_tok =
				strtok_r(parg_tok, ",",
					 &parg_cookie)) != NULL) {

				memset(chan_scratch, 0x00,
				       sizeof(chan_scratch));
				pscratch = chan_scratch;

				for (chan_parse_idx = 0;
				     (unsigned int)chan_parse_idx <
				     strlen(parg_tok); chan_parse_idx++) {
					if (isalpha
					    (*(parg_tok + chan_parse_idx))) {
						*pscratch++ = ' ';
					}

					*pscratch++ =
						*(parg_tok + chan_parse_idx);
				}
				*pscratch = 0;
				parg_tok = NULL;

				pchan_tok = chan_scratch;

				while ((pchan_tok = strtok_r(pchan_tok, " ",
							     &pchan_cookie)) !=
				       NULL) {
					if (isdigit(*pchan_tok)) {
						scan_req->
							chan_list[chan_cmd_idx].
							chan_number =
							atoi(pchan_tok);
						if (scan_req->
						    chan_list[chan_cmd_idx].
						    chan_number >
						    MAX_CHAN_BG_BAND)
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 1;
					} else {
						switch (toupper(*pchan_tok)) {
						case 'A':
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 1;
							is_radio_set = TRUE;
							break;
						case 'B':
						case 'G':
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 0;
							is_radio_set = TRUE;
							break;
						case 'N':
							break;
						case 'C':
							scan_req->
								chan_list
								[chan_cmd_idx].
								scan_type =
								MLAN_SCAN_TYPE_ACTIVE;
							break;
						case 'P':
							scan_req->
								chan_list
								[chan_cmd_idx].
								scan_type =
								MLAN_SCAN_TYPE_PASSIVE;
							break;
						default:
							printf("Error: Band type not supported!\n");
							return -EOPNOTSUPP;
						}
						if (!chan_cmd_idx &&
						    !scan_req->
						    chan_list[chan_cmd_idx].
						    chan_number && is_radio_set)
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type |=
								BAND_SPECIFIED;
					}
					pchan_tok = NULL;
				}
				if (((scan_req->chan_list[chan_cmd_idx].
				      chan_number > MAX_CHAN_BG_BAND)
				     && !scan_req->chan_list[chan_cmd_idx].
				     radio_type) ||
				    ((scan_req->chan_list[chan_cmd_idx].
				      chan_number < MAX_CHAN_BG_BAND)
				     && (scan_req->chan_list[chan_cmd_idx].
					 radio_type == 1))) {
					printf("Error: Invalid Radio type: chan=%d radio_type=%d\n", scan_req->chan_list[chan_cmd_idx].chan_number, scan_req->chan_list[chan_cmd_idx].radio_type);
					return MLAN_STATUS_FAILURE;
				}
				chan_cmd_idx++;
			}
		} else if (strncmp(args[arg_idx], "gap=", strlen("gap=")) == 0) {
			scan_req->scan_chan_gap =
				atoi(args[arg_idx] + strlen("gap="));
		} else if (strncmp(args[arg_idx], "keep=", strlen("keep=")) ==
			   0) {
			/* "keep" token string handler */
			scan_req->keep_previous_scan =
				atoi(args[arg_idx] + strlen("keep="));
		} else if (strncmp(args[arg_idx], "dur=", strlen("dur=")) == 0) {
			/* "dur" token string handler */
			scan_time = atoi(args[arg_idx] + strlen("dur="));
			scan_req->chan_list[0].scan_time = scan_time;

		} else if (strncmp(args[arg_idx], "wc=", strlen("wc=")) == 0) {

			if (num_ssid < MRVDRV_MAX_SSID_LIST_LENGTH) {
				/* "wc" token string handler */
				pscratch = strrchr(args[arg_idx], ',');

				if (pscratch) {
					*pscratch = 0;
					pscratch++;

					if (isdigit(*pscratch)) {
						scan_req->ssid_list[num_ssid].
							max_len =
							atoi(pscratch);
					} else {
						scan_req->ssid_list[num_ssid].
							max_len = *pscratch;
					}
				} else {
					/* Standard wildcard matching */
					scan_req->ssid_list[num_ssid].max_len =
						0xFF;
				}

				strncpy(scan_req->ssid_list[num_ssid].ssid,
					args[arg_idx] + strlen("wc="),
					sizeof(scan_req->ssid_list[num_ssid].
					       ssid));

				num_ssid++;
			}
		} else if (strncmp(args[arg_idx], "probes=", strlen("probes="))
			   == 0) {
			/* "probes" token string handler */
			scan_req->num_probes =
				atoi(args[arg_idx] + strlen("probes="));
			if (scan_req->num_probes > MAX_PROBES) {
				fprintf(stderr, "Invalid probes (> %d)\n",
					MAX_PROBES);
				return -EOPNOTSUPP;
			}
		} else if (strncmp
			   (args[arg_idx], "bss_type=",
			    strlen("bss_type=")) == 0) {
			/* "bss_type" token string handler */
			scan_req->bss_mode =
				atoi(args[arg_idx] + strlen("bss_type="));
			switch (scan_req->bss_mode) {
			case MLAN_SCAN_MODE_BSS:
			case MLAN_SCAN_MODE_IBSS:
				break;
			case MLAN_SCAN_MODE_ANY:
			default:
				/* Set any unknown types to ANY */
				scan_req->bss_mode = MLAN_SCAN_MODE_ANY;
				break;
			}
		} else if (strncmp
			   (args[arg_idx], "scan_type=",
			    strlen("scan_type=")) == 0) {
			/* "scan_type" token string handler */
			scan_req->ext_scan_type =
				atoi(args[arg_idx] + strlen("scan_type="));
		}
	}

	/* Update all the channels to have the same scan time */
	for (tmp_idx = 1; tmp_idx < chan_cmd_idx; tmp_idx++) {
		scan_req->chan_list[tmp_idx].scan_time = scan_time;
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process setuserscan command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_setuserscan(int argc, char *argv[])
{
	wlan_ioctl_user_scan_cfg *scan_req = NULL;
	t_u8 *pos = NULL;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int status = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	memset(buffer, 0, BUFFER_LENGTH);

	/* Flag it for our use */
	pos = buffer;
	strncpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));

	/* Insert command */
	strncpy((char *)pos, (char *)argv[2], strlen(argv[2]));
	pos += (strlen(argv[2]));

	/* Insert arguments */
	scan_req = (wlan_ioctl_user_scan_cfg *)pos;

	if (prepare_setuserscan_buffer(scan_req, (argc - 3), &argv[3])) {
		printf("ERR:Invalid parameter\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: setuserscan fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc > 3) {
		if (!strcmp(argv[3], "sort_by_ch")) {
			argv[3] = "ch";
		} else {
			argc = 0;
		}
	}
	do {
		argv[2] = "getscantable";
		status = wlan_process_getscantable(argc, argv, scan_req);
	} while (status == -EAGAIN);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process extended capabilities configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_extcapcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL, ext_cap[9];
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	IEEEtypes_Header_t ie;

	if (argc > 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 4 && MLAN_STATUS_FAILURE == ishexstring(argv[3])) {
		printf("ERR:Only hex digits are allowed.\n");
		printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Only insert command */
	prepare_buffer(buffer, argv[2], 0, NULL);

	if (argc == 4) {
		if (!strncasecmp("0x", argv[3], 2))
			argv[3] += 2;

		if (strlen(argv[3]) > 2 * sizeof(ext_cap)) {
			printf("ERR:Incorrect length of arguments.\n");
			printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
			free(buffer);
			return MLAN_STATUS_FAILURE;
		}

		memset(ext_cap, 0, sizeof(ext_cap));
		ie.element_id = TLV_TYPE_EXTCAP;
		ie.len = sizeof(ext_cap);

		string2raw(argv[3], ext_cap);
		memcpy(buffer + strlen(CMD_NXP) + strlen(argv[2]), &ie,
		       sizeof(ie));
		memcpy(buffer + strlen(CMD_NXP) + strlen(argv[2]) + sizeof(ie),
		       ext_cap, sizeof(ext_cap));
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr,
			"mlanutl: extended capabilities configure fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hexdump("Extended capabilities", buffer + sizeof(IEEEtypes_Header_t),
		((IEEEtypes_Header_t *)buffer)->len, ' ');

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Cancel ongoing scan
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cancelscan(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc != 3) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX cancelscan\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Only insert command */
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cancel scan fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif /* STA_SUPPORT */

/**
 *  @brief Process deep sleep configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_deepsleep(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: deepsleep fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Deepsleep command response: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process ipaddr command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_ipaddr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_NXP) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: ipaddr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("IP address Configuration: %s\n", (char *)buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process otpuserdata command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_otpuserdata(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4) {
		printf("ERR:No argument\n");
		return MLAN_STATUS_FAILURE;
	}

	if (a2hex_or_atoi(argv[3]) > BUFFER_LENGTH) {
		printf("ERR: user_data_length too big\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: otpuserdata fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hexdump("OTP user data: ", buffer,
		MIN(cmd->used_len, a2hex_or_atoi(argv[3])), ' ');

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process countrycode setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_countrycode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_countrycode *countrycode = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_NXP) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: countrycode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	countrycode = (struct eth_priv_countrycode *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Country code: %s\n", countrycode->country_code);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process TCP ACK enhancement configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tcpackenh(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: tcpackenh fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("TCP Ack enhancement: ");
	if (buffer[0])
		printf("enabled.\n");
	else
		printf("disabled.\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef REASSOCIATION
/**
 *  @brief Process asynced essid setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_assocessid(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_NXP) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: assocessid fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Set Asynced ESSID: %s\n", (char *)buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;

}
#endif

#ifdef STA_SUPPORT
/**
 *  @brief Process listen interval configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_listeninterval(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: listen interval fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3)
		printf("Listen interval command response: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process power save mode setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_psmode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int psmode = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: psmode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	psmode = *(int *)buffer;
	printf("PS mode: %d\n", psmode);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

#ifdef DEBUG_LEVEL1
/**
 *  @brief Process driver debug configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_drvdbg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 drvdbg;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: drvdbg config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&drvdbg, buffer, sizeof(drvdbg));
		printf("drvdbg: 0x%08x\n", drvdbg);
#ifdef DEBUG_LEVEL2
		printf("MINFO  (%08x) %s\n", MINFO,
		       (drvdbg & MINFO) ? "X" : "");
		printf("MWARN  (%08x) %s\n", MWARN,
		       (drvdbg & MWARN) ? "X" : "");
		printf("MENTRY (%08x) %s\n", MENTRY,
		       (drvdbg & MENTRY) ? "X" : "");
#endif
		printf("MMPA_D (%08x) %s\n", MMPA_D,
		       (drvdbg & MMPA_D) ? "X" : "");
		printf("MIF_D  (%08x) %s\n", MIF_D,
		       (drvdbg & MIF_D) ? "X" : "");
		printf("MFW_D  (%08x) %s\n", MFW_D,
		       (drvdbg & MFW_D) ? "X" : "");
		printf("MEVT_D (%08x) %s\n", MEVT_D,
		       (drvdbg & MEVT_D) ? "X" : "");
		printf("MCMD_D (%08x) %s\n", MCMD_D,
		       (drvdbg & MCMD_D) ? "X" : "");
		printf("MDAT_D (%08x) %s\n", MDAT_D,
		       (drvdbg & MDAT_D) ? "X" : "");
		printf("MREG_D (%08x) %s\n", MREG_D,
		       (drvdbg & MREG_D) ? "X" : "");
		printf("MIOCTL (%08x) %s\n", MIOCTL,
		       (drvdbg & MIOCTL) ? "X" : "");
		printf("MINTR  (%08x) %s\n", MINTR,
		       (drvdbg & MINTR) ? "X" : "");
		printf("MEVENT (%08x) %s\n", MEVENT,
		       (drvdbg & MEVENT) ? "X" : "");
		printf("MCMND  (%08x) %s\n", MCMND,
		       (drvdbg & MCMND) ? "X" : "");
		printf("MDATA  (%08x) %s\n", MDATA,
		       (drvdbg & MDATA) ? "X" : "");
		printf("MERROR (%08x) %s\n", MERROR,
		       (drvdbg & MERROR) ? "X" : "");
		printf("MFATAL (%08x) %s\n", MFATAL,
		       (drvdbg & MFATAL) ? "X" : "");
		printf("MMSG   (%08x) %s\n", MMSG, (drvdbg & MMSG) ? "X" : "");

	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Process hscfg configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hscfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_hs_cfg *hscfg;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: hscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hscfg = (struct eth_priv_hs_cfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("HS Configuration:\n");
		printf("  Conditions: %d\n", (int)hscfg->conditions);
		printf("  GPIO: %d\n", (int)hscfg->gpio);
		printf("  GAP: %d\n", (int)hscfg->gap);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process hssetpara configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hssetpara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: hssetpara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process wakeup reason
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wakeupresaon(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get wakeup reason fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Get wakeup reason response: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process hscfg management frame config
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmtfilter(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	char line[256], cmdname[256], *pos = NULL, *filter = NULL;
	int cmdname_found = 0, name_found = 0;
	int ln = 0, i = 0, numEntries = 0, len = 0;
	eth_priv_mgmt_frame_wakeup hs_mgmt_filter[2];

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX mgmtfilter <mgmtfilter.conf>\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(&hs_mgmt_filter, 0, sizeof(hs_mgmt_filter));
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	pos = (char *)buffer;
	strncpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));
	len += (strlen(CMD_NXP));

	/* Insert command */
	strncpy((char *)pos, argv[2], strlen(argv[2]));
	pos += (strlen(argv[2]));
	len += (strlen(argv[2]));

	filter = pos;

	cmdname_found = 0;
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[2]);

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		if (buffer)
			free(buffer);
		goto done;
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "entry_num=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					numEntries =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					if (numEntries > 2) {
						printf("mlanutl: NumEntries exceed max number.\
						We support two entries in currently\n");
						free(buffer);
						if (fp)
							fclose(fp);
						return MLAN_STATUS_FAILURE;
					}
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: NumEntries not found in file '%s'\n",
					argv[3]);
				break;
			}
			for (i = 0; i < numEntries; i++) {
				snprintf(cmdname, sizeof(cmdname), "entry_%d={",
					 i);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "action=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						hs_mgmt_filter[i].action =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "type=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						hs_mgmt_filter[i].type =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "frame_mask=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						hs_mgmt_filter[i].frame_mask =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
			}
			break;
		}
	}
	fclose(fp);
	if (!cmdname_found) {
		fprintf(stderr, "mlanutl: ipPkt data not found in file '%s'\n",
			argv[3]);
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	memcpy(filter, (t_u8 *)hs_mgmt_filter,
	       sizeof(eth_priv_mgmt_frame_wakeup) * numEntries);
	len += sizeof(eth_priv_mgmt_frame_wakeup) * numEntries;
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = len;
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cloud keep alive fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

done:
	return ret;
}

/**
 *  @brief Process scancfg configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_scancfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_scan_cfg *scancfg;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: scancfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	scancfg = (struct eth_priv_scan_cfg *)buffer;
	/* Show scan configure */
	printf("Scan Configuration:\n");
	printf("    Scan Type:              %d (%s)\n", scancfg->scan_type,
	       (scancfg->scan_type == 1) ? "Active" : (scancfg->scan_type ==
						       2) ? "Passive" : "");
	printf("    Scan Mode:              %d (%s)\n", scancfg->scan_mode,
	       (scancfg->scan_mode == 1) ? "BSS" : (scancfg->scan_mode ==
						    2) ? "IBSS" : (scancfg->
								   scan_mode ==
								   3) ? "Any" :
	       "");
	printf("    Scan Probes:            %d (%s)\n", scancfg->scan_probe,
	       "per channel");
	printf("    Specific Scan Time:     %d ms\n",
	       scancfg->scan_time.specific_scan_time);
	printf("    Active Scan Time:       %d ms\n",
	       scancfg->scan_time.active_scan_time);
	printf("    Passive Scan Time:      %d ms\n",
	       scancfg->scan_time.passive_scan_time);
	printf("    Passive to Active Scan: %d (%s)\n",
	       scancfg->passive_to_active_scan,
	       (scancfg->passive_to_active_scan ==
		MLAN_PASS_TO_ACT_SCAN_EN) ? "Enable" : (scancfg->
							passive_to_active_scan
							==
							MLAN_PASS_TO_ACT_SCAN_DIS)
	       ? "Disable" : "");
	printf("    Extended Scan Support:  %d (%s)\n", scancfg->ext_scan,
	       (scancfg->ext_scan == 1) ? "No" : (scancfg->ext_scan ==
						  2) ? "Yes" : (scancfg->
								ext_scan ==
								3) ? "Enhanced"
	       : "");
	printf("    Scan channel Gap:       %d ms\n", scancfg->scan_chan_gap);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process packet aggregation configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_aggrctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	aggr_ctrl_params *param = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: usbaggrctrl failed.\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* GET operation */
		param = (aggr_ctrl_params *) buffer;
		printf("TX Aggr Ctrl:\n");
		printf("  %s, Max = %d bytes, Max Pkt Num = %d, Align=%d Timeout = %d us\n", param->tx.enable ? "Enabled" : "Disabled", param->tx.aggr_max_size, param->tx.aggr_max_num, param->tx.aggr_align, param->tx.aggr_tmo);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process USB packet aggregation configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_usbaggrctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	aggr_params *param = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: usbaggrctrl failed.\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* GET operation */
		param = (aggr_params *)buffer;
		printf("Tx aggregation:\n");
		printf("  %s, Max = %d %s, Alignment = %d bytes, Timeout = %d us\n", param->tx_aggr_ctrl.enable ? "Enabled" : "Disabled", param->tx_aggr_ctrl.aggr_max, (param->tx_aggr_ctrl.aggr_max < 512) ? "packets" : "bytes", param->tx_aggr_ctrl.aggr_align, param->tx_aggr_ctrl.aggr_tmo);
		printf("Rx aggregation:\n");
		printf("  %s, Max = %d %s, Alignment = %d bytes, Timeout = %d us\n", param->rx_deaggr_ctrl.enable ? "Enabled" : "Disabled", param->rx_deaggr_ctrl.aggr_max, (param->rx_deaggr_ctrl.aggr_max < 512) ? "packets" : "bytes", param->rx_deaggr_ctrl.aggr_align, param->rx_deaggr_ctrl.aggr_tmo);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process warmreset command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_warmreset(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: warmreset fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

static char *bandwidth[4] = { "20 MHz", "40 MHz", "80 MHz", "160 MHz" };

/**
 *  @brief Process txpowercfg command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txpowercfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_power_cfg_ext *power_ext = NULL;
	struct eth_priv_power_group *power_group = NULL;
	int i = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(2 * BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = 2 * BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txpowercfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	power_ext = (struct eth_priv_power_cfg_ext *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Tx Power Configurations:\n");
		for (i = 0; i < power_ext->num_pwr_grp; i++) {
			power_group = &power_ext->power_group[i];
			if (power_group->rate_format == MLAN_RATE_FORMAT_HT) {
				if (power_group->bandwidth == MLAN_HT_BW20) {
					power_group->first_rate_ind += 12;
					power_group->last_rate_ind += 12;
				} else if (power_group->bandwidth ==
					   MLAN_HT_BW40) {
					power_group->first_rate_ind += 140;
					power_group->last_rate_ind += 140;
				}
			}
			printf("    Power Group %d: \n", i);
			printf("        Bandwidth:        %3s %s\n",
			       rate_format[power_group->rate_format],
			       bandwidth[power_group->bandwidth]);
			if (power_group->rate_format == 2)
    /** NSS */
				printf("        NSS:              %3d\n",
				       power_group->nss);
			printf("        first rate index: %3d\n",
			       power_group->first_rate_ind);
			printf("        last rate index:  %3d\n",
			       power_group->last_rate_ind);
			printf("        minimum power:    %3d dBm\n",
			       power_group->power_min);
			printf("        maximum power:    %3d dBm\n",
			       power_group->power_max);
			printf("        power step:       %3d\n",
			       power_group->power_step);
			printf("\n");
			power_group++;
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process pscfg command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_pscfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_ps_cfg *ps_cfg = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	ps_cfg = (struct eth_priv_ds_ps_cfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("PS Configurations:\n");
		printf("%d", (int)ps_cfg->ps_null_interval);
		printf("  %d", (int)ps_cfg->multiple_dtim_interval);
		printf("  %d", (int)ps_cfg->listen_interval);
		printf("  %d", (int)ps_cfg->bcn_miss_timeout);
		printf("  %d", (int)ps_cfg->delay_to_ps);
		printf("  %d", (int)ps_cfg->ps_mode);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process bcntimeoutcfg command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bcntimeoutcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc != 7) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX bcntimeoutcfg [l] [m] [o] [p]\n");
		return -EINVAL;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return -ENOMEM;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return -ENOMEM;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bcntimeoutcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return -EFAULT;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return 0;
}

/**
 *  @brief Process sleeppd configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_sleeppd(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int sleeppd = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sleeppd fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	sleeppd = *(int *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Sleep Period: %d ms\n", sleeppd);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx control configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txcontrol(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 txcontrol = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txcontrol fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	txcontrol = *(t_u32 *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Tx control: 0x%x\n", txcontrol);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/** custom IE, auto mask value */
#define	CUSTOM_IE_AUTO_MASK	0xffff

/**
 * @brief Show usage information for the customie
 * command
 *
 * $return         N/A
 **/
void
print_custom_ie_usage(void)
{
	printf("\nUsage : customie [INDEX] [MASK] [IEBuffer]");
	printf("\n         empty - Get all IE settings\n");
	printf("\n         INDEX:  0 - Get/Set IE index 0 setting");
	printf("\n                 1 - Get/Set IE index 1 setting");
	printf("\n                 2 - Get/Set IE index 2 setting");
	printf("\n                 3 - Get/Set IE index 3 setting");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                -1 - Append/Delete IE automatically");
	printf("\n                     Delete will delete the IE from the matching IE buffer");
	printf("\n                     Append will append the IE to the buffer with the same mask");
	printf("\n         MASK :  Management subtype mask value as per bit defintions");
	printf("\n              :  Bit 0 - Association request.");
	printf("\n              :  Bit 1 - Association response.");
	printf("\n              :  Bit 2 - Reassociation request.");
	printf("\n              :  Bit 3 - Reassociation response.");
	printf("\n              :  Bit 4 - Probe request.");
	printf("\n              :  Bit 5 - Probe response.");
	printf("\n              :  Bit 8 - Beacon.");
	printf("\n         MASK :  MASK = 0 to clear the mask and the IE buffer");
	printf("\n         IEBuffer :  IE Buffer in hex (max 256 bytes)\n\n");
	return;
}

/**
 * @brief Creates a hostcmd request for custom IE settings
 * and sends to the driver
 *
 * Usage: "customie [INDEX] [MASK] [IEBuffer]"
 *
 * Options: INDEX :      0 - Get/Delete IE index 0 setting
 *                       1 - Get/Delete IE index 1 setting
 *                       2 - Get/Delete IE index 2 setting
 *                       3 - Get/Delete IE index 3 setting
 *                       .
 *                       .
 *                       .
 *                      -1 - Append IE at the IE buffer with same MASK
 *          MASK  :      Management subtype mask value
 *          IEBuffer:    IE Buffer in hex
 *   					       empty - Get all IE settings
 *
 * @param argc     Number of arguments
 * @param argv     Pointer to the arguments
 * @return         N/A
 **/
static int
process_customie(int argc, char *argv[])
{
	eth_priv_ds_misc_custom_ie *tlv = NULL;
	tlvbuf_max_mgmt_ie *max_mgmt_ie_tlv = NULL;
	t_u16 mgmt_subtype_mask = 0;
	custom_ie *ie_ptr = NULL;
	int ie_buf_len = 0, ie_len = 0, i = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int ret = MLAN_STATUS_SUCCESS;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* mlanutl mlan0 customie idx flag buf */
	if (argc > 6) {
		printf("ERR:Too many arguments.\n");
		print_custom_ie_usage();
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	/* Error checks and initialize the command length */
	if (argc > 3) {
		if (((IS_HEX_OR_DIGIT(argv[3]) == MLAN_STATUS_FAILURE) &&
		     (atoi(argv[3]) != -1)) || (atoi(argv[3]) < -1)) {
			printf("ERR:Illegal index %s\n", argv[3]);
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}
	switch (argc) {
	case 3:
		break;
	case 4:
		if (atoi(argv[3]) < 0) {
			printf("ERR:Illegal index %s. Must be either greater than or equal to 0 for Get Operation \n", argv[3]);
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		break;
	case 5:
		if (MLAN_STATUS_FAILURE == ishexstring(argv[4]) ||
		    A2HEXDECIMAL(argv[4]) != 0) {
			printf("ERR: Mask value should be 0 to clear IEBuffers.\n");
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		if (atoi(argv[3]) == -1) {
			printf("ERR: You must provide buffer for automatic deletion.\n");
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		break;
	case 6:
		/* This is to check negative numbers and special symbols */
		if (MLAN_STATUS_FAILURE == IS_HEX_OR_DIGIT(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n");
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		/* If above check is passed and mask is not hex, then it must be 0 */
		if ((ISDIGIT(argv[4]) == MLAN_STATUS_SUCCESS) && atoi(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n ");
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		if (MLAN_STATUS_FAILURE == ishexstring(argv[5])) {
			printf("ERR:Only hex digits are allowed\n");
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		ie_buf_len = strlen(argv[5]);
		if (!strncasecmp("0x", argv[5], 2)) {
			ie_len = (ie_buf_len - 2 + 1) / 2;
			argv[5] += 2;
		} else
			ie_len = (ie_buf_len + 1) / 2;
		if (ie_len > MAX_IE_BUFFER_LEN) {
			printf("ERR:Incorrect IE length %d\n", ie_buf_len);
			print_custom_ie_usage();
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		mgmt_subtype_mask = (t_u16)A2HEXDECIMAL(argv[4]);
		break;
	}
	/* Initialize the command buffer */
	tlv = (eth_priv_ds_misc_custom_ie *)(buffer + strlen(CMD_NXP) +
					     strlen(argv[2]));

	if (tlv == NULL) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	tlv->type = MRVL_MGMT_IE_LIST_TLV_ID;
	if (argc == 3 || argc == 4) {
		if (argc == 3)
			tlv->len = 0;
		else {
			tlv->len = sizeof(t_u16);
			ie_ptr = (custom_ie *)(tlv->ie_data);
			ie_ptr->ie_index = (t_u16)(atoi(argv[3]));
		}
	} else {
		/* Locate headers */
		ie_ptr = (custom_ie *)(tlv->ie_data);
		/* Set TLV fields */
		tlv->len = sizeof(custom_ie) + ie_len;
		ie_ptr->ie_index = atoi(argv[3]);
		ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
		ie_ptr->ie_length = ie_len;
		if (argc == 6)
			string2raw(argv[5], ie_ptr->ie_buffer);
	}
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[CUSTOM_IE_CFG]");
		printf("ERR:Command sending failed!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	/* Print response */
	if (argc > 4) {
		printf("Custom IE setting successful\n");
	} else {
		printf("Querying custom IE successful\n");
		ie_len = tlv->len;
		ie_ptr = (custom_ie *)(tlv->ie_data);
		while (ie_len >= (int)sizeof(custom_ie)) {
			printf("Index [%d]\n", ie_ptr->ie_index);
			if (ie_ptr->ie_length)
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask) == 0 ?
				       CUSTOM_IE_AUTO_MASK :
				       (ie_ptr->mgmt_subtype_mask));
			else
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask));
			hexdump("IE Buffer", (void *)ie_ptr->ie_buffer,
				ie_ptr->ie_length, ' ');
			ie_len -= sizeof(custom_ie) + ie_ptr->ie_length;
			ie_ptr = (custom_ie *)((t_u8 *)ie_ptr +
					       sizeof(custom_ie) +
					       ie_ptr->ie_length);
		}
	}
	max_mgmt_ie_tlv = (tlvbuf_max_mgmt_ie *)((t_u8 *)tlv +
						 sizeof
						 (eth_priv_ds_misc_custom_ie) +
						 tlv->len);
	if (max_mgmt_ie_tlv) {
		if (max_mgmt_ie_tlv->type == MRVL_MAX_MGMT_IE_TLV_ID) {
			for (i = 0; i < max_mgmt_ie_tlv->count; i++) {
				printf("buf%d_size = %d\n", i,
				       max_mgmt_ie_tlv->info[i].buf_size);
				printf("number of buffers = %d\n",
				       max_mgmt_ie_tlv->info[i].buf_count);
				printf("\n");
			}
		}
	}

	ret = MLAN_STATUS_SUCCESS;

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process regrdwr command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_regrdwr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_reg_rw *reg = NULL;

	if (argc < 5 || argc > 6) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: regrdwr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	reg = (struct eth_priv_ds_reg_rw *)buffer;
	if (argc == 5) {
		/* GET operation */
		printf("Value = 0x%x\n", reg->value);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process rdeeprom command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rdeeprom(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_read_eeprom *eeprom = NULL;
	int i = 0;

	if (argc != 5) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rdeeprom fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	eeprom = (struct eth_priv_ds_read_eeprom *)buffer;
	if (argc == 5) {
		/* GET operation */
		printf("Value:\n");
		for (i = 0; i < MIN(MAX_EEPROM_DATA, eeprom->byte_count); i++)
			printf(" %02x", eeprom->value[i]);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process memrdwr command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_memrdwr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_mem_rw *mem = NULL;

	if (argc < 4 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: memrdwr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mem = (struct eth_priv_ds_mem_rw *)buffer;
	if (argc == 4) {
		/* GET operation */
		printf("Value = 0x%x\n", mem->value);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef SDIO
/**
 *  @brief Process sdcmd52rw command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_sdcmd52rw(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 5 || argc > 6) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdcmd52rw fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 5) {
		/* GET operation */
		printf("Value = 0x%x\n", (int)(*buffer));
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif /* SDIO */

#define STACK_NBYTES            	100	/**< Number of bytes in stack */
#define MAX_BYTESEQ 		       	6	/**< Maximum byte sequence */
#define TYPE_DNUM           		1 /**< decimal number */
#define TYPE_BYTESEQ        		2 /**< byte sequence */
#define MAX_OPERAND         		0x40	/**< Maximum operands */
#define TYPE_EQ         		(MAX_OPERAND+1)	/**< byte comparison:    == operator */
#define TYPE_EQ_DNUM    		(MAX_OPERAND+2)	/**< decimal comparison: =d operator */
#define TYPE_EQ_BIT     		(MAX_OPERAND+3)	/**< bit comparison:     =b operator */
#define TYPE_AND        		(MAX_OPERAND+4)	/**< && operator */
#define TYPE_OR         		(MAX_OPERAND+5)	/**< || operator */

typedef struct {
	t_u16 sp;		      /**< Stack pointer */
	t_u8 byte[STACK_NBYTES];      /**< Stack */
} mstack_t;

typedef struct {
	t_u8 type;		  /**< Type */
	t_u8 reserve[3];   /**< so 4-byte align val array */
	/* byte sequence is the largest among all the operands and operators. */
	/* byte sequence format: 1 byte of num of bytes, then variable num bytes */
	t_u8 val[MAX_BYTESEQ + 1];/**< Value */
} op_t;

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param nbytes		number of byte to push to stack
 *
 *  @param val			a pointer to data buffer
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push_n(mstack_t * s, t_u8 nbytes, t_u8 *val)
{
	if ((s->sp + nbytes) < STACK_NBYTES) {
		memcpy((void *)(s->byte + s->sp), (const void *)val,
		       (size_t) nbytes);
		s->sp += nbytes;
		/* printf("push: n %d sp %d\n", nbytes, s->sp); */
		return TRUE;
	} else			/* stack full */
		return FALSE;
}

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param op			a pointer to op_t structure
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push(mstack_t * s, op_t * op)
{
	t_u8 nbytes;
	switch (op->type) {
	case TYPE_DNUM:
		if (push_n(s, 4, op->val))
			return push_n(s, 1, &op->type);
		return FALSE;
	case TYPE_BYTESEQ:
		nbytes = op->val[0];
		if (push_n(s, nbytes, op->val + 1) &&
		    push_n(s, 1, op->val) && push_n(s, 1, &op->type))
			return TRUE;
		return FALSE;
	default:
		return push_n(s, 1, &op->type);
	}
}

/**
 *  @brief parse RPN string
 *
 *  @param s			a pointer to Null-terminated string to scan.
 *
 *  @param first_time		a pointer to return first_time
 *
 *  @return			A pointer to the last token found in string.
 *  				NULL is returned when there are no more tokens to be found.
 *
 */
static char *
getop(char *s, int *first_time)
{
	const char delim[] = " \t\n";
	char *p;
	if (*first_time) {
		p = strtok(s, delim);
		*first_time = FALSE;
	} else {
		p = strtok(NULL, delim);
	}
	return p;
}

/**
 *  @brief Verify hex digit.
 *
 *  @param c			input ascii char
 *  @param h			a pointer to return integer value of the digit char.
 *  @return			TURE -- c is hex digit, FALSE -- c is not hex digit.
 */
static int
ishexdigit(char c, t_u8 *h)
{
	if (c >= '0' && c <= '9') {
		*h = c - '0';
		return TRUE;
	} else if (c >= 'a' && c <= 'f') {
		*h = c - 'a' + 10;
		return TRUE;
	} else if (c >= 'A' && c <= 'F') {
		*h = c - 'A' + 10;
		return TRUE;
	}
	return FALSE;
}

/**
 *  @brief convert hex string to integer.
 *
 *  @param s			A pointer to hex string, string length up to 2 digits.
 *  @return			integer value.
 */
static t_u8
hex_atoi(char *s)
{
	int i;
	t_u8 digit;		/* digital value */
	t_u8 t = 0;		/* total value */

	for (i = 0, t = 0; ishexdigit(s[i], &digit) && i < 2; i++)
		t = 16 * t + digit;
	return t;
}

/**
 *  @brief Parse byte sequence in hex format string to a byte sequence.
 *
 *  @param opstr		A pointer to byte sequence in hex format string, with ':' as delimiter between two byte.
 *  @param val			A pointer to return byte sequence string
 *  @return			NA
 */
static void
parse_hex(char *opstr, t_u8 *val)
{
	char delim = ':';
	char *p;
	char *q;
	t_u8 i;

	/* +1 is for skipping over the preceding h character. */
	p = opstr + 1;

	/* First byte */
	val[1] = hex_atoi(p++);

	/* Parse subsequent bytes. */
	/* Each byte is preceded by the : character. */
	for (i = 1; *p; i++) {
		q = strchr(p, delim);
		if (!q)
			break;
		p = q + 1;
		val[i + 1] = hex_atoi(p);
	}
	/* Set num of bytes */
	val[0] = i;
}

/**
 *  @brief str2bin, convert RPN string to binary format
 *
 *  @param str			A pointer to rpn string
 *  @param stack		A pointer to mstack_t structure
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
str2bin(char *str, mstack_t * stack)
{
	int first_time;
	char *opstr;
	op_t op;		/* operator/operand */
	int dnum;
	int ret = MLAN_STATUS_SUCCESS;

	memset(stack, 0, sizeof(mstack_t));
	first_time = TRUE;
	while ((opstr = getop(str, &first_time)) != NULL) {
		if (isdigit(*opstr)) {
			op.type = TYPE_DNUM;
			dnum = cpu_to_le32(atoi(opstr));
			memcpy((t_u8 *)op.val, &dnum, sizeof(dnum));
			if (!push(stack, &op)) {
				printf("push decimal number failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (*opstr == 'h') {
			op.type = TYPE_BYTESEQ;
			parse_hex(opstr, op.val);
			if (!push(stack, &op)) {
				printf("push byte sequence failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "==")) {
			op.type = TYPE_EQ;
			if (!push(stack, &op)) {
				printf("push byte cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=d")) {
			op.type = TYPE_EQ_DNUM;
			if (!push(stack, &op)) {
				printf("push decimal cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=b")) {
			op.type = TYPE_EQ_BIT;
			if (!push(stack, &op)) {
				printf("push bit cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "&&")) {
			op.type = TYPE_AND;
			if (!push(stack, &op)) {
				printf("push AND operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "||")) {
			op.type = TYPE_OR;
			if (!push(stack, &op)) {
				printf("push OR operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else {
			printf("Unknown operand\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
	}
	return ret;
}

#define FILTER_BYTESEQ 		TYPE_EQ	/**< byte sequence */
#define FILTER_DNUM    		TYPE_EQ_DNUM /**< decimal number */
#define FILTER_BITSEQ		TYPE_EQ_BIT /**< bit sequence */
#define FILTER_TEST		(FILTER_BITSEQ+1) /**< test */

#define NAME_TYPE		1	    /**< Field name 'type' */
#define NAME_PATTERN		2	/**< Field name 'pattern' */
#define NAME_OFFSET		3	    /**< Field name 'offset' */
#define NAME_NUMBYTE		4	/**< Field name 'numbyte' */
#define NAME_REPEAT		5	    /**< Field name 'repeat' */
#define NAME_BYTE		6	    /**< Field name 'byte' */
#define NAME_MASK		7	    /**< Field name 'mask' */
#define NAME_DEST		8	    /**< Field name 'dest' */

static struct mef_fields {
	char *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name Id. */
} mef_fields[] = {
	{
	"type", NAME_TYPE}, {
	"pattern", NAME_PATTERN}, {
	"offset", NAME_OFFSET}, {
	"numbyte", NAME_NUMBYTE}, {
	"repeat", NAME_REPEAT}, {
	"byte", NAME_BYTE}, {
	"mask", NAME_MASK}, {
	"dest", NAME_DEST}
};

/**
 *  @brief get filter data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_filter_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s32 errors = 0, i;
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u16 type = 0;
	t_u32 pattern = 0;
	t_u16 repeat = 0;
	t_u16 offset = 0;
	char byte_seq[50];
	char mask_seq[50];
	t_u16 numbyte = 0;
	t_s8 type_find = 0;
	t_s8 pattern_find = 0;
	t_s8 offset_find = 0;
	t_s8 numbyte_find = 0;
	t_s8 repeat_find = 0;
	t_s8 byte_find = 0;
	t_s8 mask_find = 0;
	t_s8 dest_find = 0;
	char dest_seq[50];

	*size = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_filter line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		for (i = 0; (t_u32)i < NELEMENTS(mef_fields); i++) {
			if (strncmp
			    (pos, mef_fields[i].name,
			     strlen(mef_fields[i].name)) == 0) {
				switch (mef_fields[i].nameid) {
				case NAME_TYPE:
					type = a2hex_or_atoi(pos1);
					if ((type != FILTER_DNUM) &&
					    (type != FILTER_BYTESEQ)
					    && (type != FILTER_BITSEQ) &&
					    (type != FILTER_TEST)) {
						printf("Invalid filter type:%d\n", type);
						return MLAN_STATUS_FAILURE;
					}
					type_find = 1;
					break;
				case NAME_PATTERN:
					pattern = a2hex_or_atoi(pos1);
					pattern_find = 1;
					break;
				case NAME_OFFSET:
					offset = a2hex_or_atoi(pos1);
					offset_find = 1;
					break;
				case NAME_NUMBYTE:
					numbyte = a2hex_or_atoi(pos1);
					numbyte_find = 1;
					break;
				case NAME_REPEAT:
					repeat = a2hex_or_atoi(pos1);
					repeat_find = 1;
					break;
				case NAME_BYTE:
					memset(byte_seq, 0, sizeof(byte_seq));
					strncpy(byte_seq, pos1,
						(sizeof(byte_seq) - 1));
					byte_find = 1;
					break;
				case NAME_MASK:
					memset(mask_seq, 0, sizeof(mask_seq));
					strncpy(mask_seq, pos1,
						(sizeof(mask_seq) - 1));
					mask_find = 1;
					break;
				case NAME_DEST:
					memset(dest_seq, 0, sizeof(dest_seq));
					strncpy(dest_seq, pos1,
						(sizeof(dest_seq) - 1));
					dest_find = 1;
					break;
				}
				break;
			}
		}
		if (i == NELEMENTS(mef_fields)) {
			printf("Line %d: unknown mef field '%s'.\n",
			       *line, pos);
			errors++;
		}
	}
	if (type_find == 0) {
		printf("Can not find filter type\n");
		return MLAN_STATUS_FAILURE;
	}
	switch (type) {
	case FILTER_DNUM:
		if (!pattern_find || !offset_find || !numbyte_find) {
			printf("Missing field for FILTER_DNUM: pattern=%d,offset=%d,numbyte=%d\n", pattern_find, offset_find, numbyte_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d %d %d =d ", pattern, offset,
			 numbyte);
		break;
	case FILTER_BYTESEQ:
		if (!byte_find || !offset_find || !repeat_find) {
			printf("Missing field for FILTER_BYTESEQ: byte=%d,offset=%d,repeat=%d\n", byte_find, offset_find, repeat_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d h%s %d == ", repeat, byte_seq,
			 offset);
		break;
	case FILTER_BITSEQ:
		if (!byte_find || !offset_find || !mask_find) {
			printf("Missing field for FILTER_BITSEQ: byte=%d,offset=%d,mask_find=%d\n", byte_find, offset_find, mask_find);
			return MLAN_STATUS_FAILURE;
		}
		if (strlen(byte_seq) != strlen(mask_seq)) {
			printf("byte string's length is different with mask's length!\n");
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s =b ", byte_seq, offset,
			 mask_seq);
		break;
	case FILTER_TEST:
		if (!byte_find || !offset_find || !repeat_find || !dest_find) {
			printf("Missing field for FILTER_TEST: byte=%d,offset=%d,repeat=%d,dest=%d\n", byte_find, offset_find, repeat_find, dest_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s %d ", dest_seq, repeat,
			 byte_seq, offset);
		break;
	}
	memcpy(buf, line, strlen(line));
	*size = strlen(line);
	return MLAN_STATUS_SUCCESS;
}

#define NAME_MODE	1	/**< Field name 'mode' */
#define NAME_ACTION	2	/**< Field name 'action' */
#define NAME_FILTER_NUM	3   /**< Field name 'filter_num' */
#define NAME_RPN	4	/**< Field name 'RPN' */
static struct mef_entry_fields {
	char *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name id */
} mef_entry_fields[] = {
	{
	"mode", NAME_MODE}, {
	"action", NAME_ACTION}, {
	"filter_num", NAME_FILTER_NUM}, {
"RPN", NAME_RPN},};

typedef struct _MEF_ENTRY {
    /** Mode */
	t_u8 Mode;
    /** Size */
	t_u8 Action;
    /** Size of expression */
	t_u16 ExprSize;
} MEF_ENTRY;

/**
 *  @brief get mef_entry data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_mef_entry_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u8 mode, action, filter_num = 0;
	char rpn[256];
	t_s8 mode_find = 0;
	t_s8 action_find = 0;
	t_s8 filter_num_find = 0;
	t_s8 rpn_find = 0;
	char rpn_str[256];
	int rpn_len = 0;
	char filter_name[50];
	t_s8 name_found = 0;
	t_u16 len = 0;
	int i;
	int first_time = TRUE;
	char *opstr = NULL;
	char filter_action[10];
	t_s32 errors = 0;
	MEF_ENTRY *pMefEntry = (MEF_ENTRY *) buf;
	mstack_t stack;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_entry line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		if (!mode_find || !action_find || !filter_num_find || !rpn_find) {
			for (i = 0;
			     (unsigned int)i < NELEMENTS(mef_entry_fields);
			     i++) {
				if (strncmp
				    (pos, mef_entry_fields[i].name,
				     strlen(mef_entry_fields[i].name)) == 0) {
					switch (mef_entry_fields[i].nameid) {
					case NAME_MODE:
						mode = a2hex_or_atoi(pos1);
						if (mode & ~0x7) {
							printf("invalid mode=%d\n", mode);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Mode = mode;
						mode_find = 1;
						break;
					case NAME_ACTION:
						action = a2hex_or_atoi(pos1);
						if (action & ~0xff) {
							printf("invalid action=%d\n", action);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Action = action;
						action_find = 1;
						break;
					case NAME_FILTER_NUM:
						filter_num =
							a2hex_or_atoi(pos1);
						filter_num_find = 1;
						break;
					case NAME_RPN:
						memset(rpn, 0, sizeof(rpn));
						strncpy(rpn, pos1,
							(sizeof(rpn) - 1));
						rpn_find = 1;
						break;
					}
					break;
				}
			}
			if (i == NELEMENTS(mef_fields)) {
				printf("Line %d: unknown mef_entry field '%s'.\n", *line, pos);
				return MLAN_STATUS_FAILURE;
			}
		}
		if (mode_find && action_find && filter_num_find && rpn_find) {
			for (i = 0; i < filter_num; i++) {
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				snprintf(filter_name, sizeof(filter_name),
					 "%s={", opstr);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     ln))) {
					if (strncmp
					    (pos, filter_name,
					     strlen(filter_name)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file\n",
						filter_name);
					return MLAN_STATUS_FAILURE;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_filter_data(fp, ln,
							 (t_u8 *)(rpn_str +
								  rpn_len),
							 &len))
					break;
				rpn_len += len;
				if (i > 0) {
					memcpy(rpn_str + rpn_len, filter_action,
					       strlen(filter_action));
					rpn_len += strlen(filter_action);
				}
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				memset(filter_action, 0, sizeof(filter_action));
				snprintf(filter_action, sizeof(filter_action),
					 "%s ", opstr);
			}
			/* Remove the last space */
			if (rpn_len > 0) {
				rpn_len--;
				rpn_str[rpn_len] = 0;
			}
			if (MLAN_STATUS_FAILURE == str2bin(rpn_str, &stack)) {
				printf("Fail on str2bin!\n");
				return MLAN_STATUS_FAILURE;
			}
			*size = sizeof(MEF_ENTRY);
			pMefEntry->ExprSize = cpu_to_le16(stack.sp);
			memmove(buf + sizeof(MEF_ENTRY), stack.byte, stack.sp);
			*size += stack.sp;
			break;
		} else if (mode_find && action_find && filter_num_find &&
			   (filter_num == 0)) {
			pMefEntry->ExprSize = 0;
			*size = sizeof(MEF_ENTRY);
			break;
		}
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process cloud keep alive command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cloud_keep_alive(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	char line[256], cmdname[256], *pos = NULL;
	int cmdname_found = 0, name_found = 0, arg_num = 0;
	int ln = 0, i = 0;
	char *args[256];
	cloud_keep_alive *keep_alive = NULL;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX cloud_keep_alive <keep_alive.conf> <start/stop/reset>\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));

	keep_alive = (cloud_keep_alive *) (buffer + strlen(argv[2]));

	cmdname_found = 0;
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[4]);

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		if (buffer)
			free(buffer);
		goto done;
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "mkeep_alive_id=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					keep_alive->mkeep_alive_id =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: keep alive id not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "enable=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					keep_alive->enable =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: enable not found in file '%s'\n",
					argv[3]);
				break;
			}
			if (strcmp(argv[4], "reset") == 0) {
				snprintf(cmdname, sizeof(cmdname), "reset=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->reset =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: reset not found in file '%s'\n",
						argv[3]);
					break;
				}
			}
			if (strcmp(argv[4], "start") == 0) {
				snprintf(cmdname, sizeof(cmdname),
					 "sendInterval=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->sendInterval =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: sendInterval not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "retryInterval=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->retryInterval =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: retryInterval not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "retryCount=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->retryCount =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: retryCount not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "destMacAddr=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						mac2raw(pos + strlen(cmdname),
							keep_alive->dst_mac);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: destination MAC address not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "srcMacAddr=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						mac2raw(pos + strlen(cmdname),
							keep_alive->src_mac);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: source MAC address not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "pktLen=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->pkt_len =
							a2hex_or_atoi(pos +
								      strlen
								      (cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: ip packet length not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "ipPkt=");
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						arg_num =
							parse_line(line, args,
								   256);
						if (arg_num <
						    keep_alive->pkt_len) {
							fprintf(stderr,
								"Invalid ipPkt or pkt_len in '%s'\n",
								argv[3]);
							break;
						}
						for (i = 0;
						     i < keep_alive->pkt_len;
						     i++)
							keep_alive->pkt[i] =
								(t_u8)
								atoval(args
								       [i + 1]);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: ipPkt data not found in file '%s'\n",
						argv[3]);
					break;
				}
			}
		}
	}
	if (!cmdname_found) {
		fprintf(stderr, "mlanutl: ipPkt data not found in file '%s'\n",
			argv[3]);
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cloud keep alive fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Process result */
	keep_alive = (cloud_keep_alive *) (buffer + strlen(argv[2]));
	if (strcmp(argv[4], "start") != 0) {
		hexdump("Last cloud keep alive packet info", keep_alive->pkt,
			keep_alive->pkt_len, ' ');
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

done:
	return ret;
}

/**
 *  @brief Process enable/disable DFS offload
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_dfs_offload_enable(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check arguments */
	if (argc < 3 || argc > 4) {
		printf("ERR:Incorrect number of arguments!\n");
		printf("Syntax: ./mlanutl mlanX dfs_offload <0/1>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dfs offload enable/disable fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#define MEFCFG_CMDCODE	0x009a

/**
 *  @brief Process mefcfg command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mefcfg(int argc, char *argv[])
{
	char line[256], cmdname[256], *pos = NULL;
	int cmdname_found = 0, name_found = 0;
	int ln = 0;
	int ret = MLAN_STATUS_SUCCESS;
	int i;
	t_u8 *buffer = NULL;
	t_u16 len = 0;
	HostCmd_DS_MEF_CFG *mefcmd = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	FILE *fp = NULL;
	t_u32 cmd_len = 0, cmd_header_len;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlan0 mefcfg <mef.conf>\n");
		exit(1);
	}

	cmd_header_len = strlen(CMD_NXP) + strlen("HOSTCMD");
	cmd_len = sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_MEF_CFG);
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		fprintf(stderr, "Cannot alloc memory\n");
		exit(1);
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, HOSTCMD, 0, NULL);

	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN> */
	hostcmd = (HostCmd_DS_GEN *)(buffer + cmd_header_len + sizeof(t_u32));
	hostcmd->command = cpu_to_le16(MEFCFG_CMDCODE);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN><HostCmd_DS_MEF_CFG> */
	mefcmd = (HostCmd_DS_MEF_CFG *)(buffer + cmd_header_len +
					sizeof(t_u32) + S_DS_GEN);

/* Host Command Population */
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[2]);
	cmdname_found = 0;
	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[4]);
		exit(1);
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "Criteria=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->Criteria =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: criteria not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "NumEntries=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->NumEntries =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: NumEntries not found in file '%s'\n",
					argv[3]);
				break;
			}
			for (i = 0; i < mefcmd->NumEntries; i++) {
				snprintf(cmdname, sizeof(cmdname),
					 "mef_entry_%d={", i);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_mef_entry_data(fp, &ln,
							    (t_u8 *)hostcmd +
							    cmd_len, &len)) {
					ret = MLAN_STATUS_FAILURE;
					break;
				}
				cmd_len += len;
			}
			break;
		}
	}
	fclose(fp);
	/* buf = MRVL_CMD<cmd><hostcmd_size> */
	memcpy(buffer + cmd_header_len, (t_u8 *)&cmd_len, sizeof(t_u32));

	if (!cmdname_found)
		fprintf(stderr,
			"mlanutl: cmdname '%s' not found in file '%s'\n",
			argv[4], argv[3]);

	if (!cmdname_found || !name_found) {
		ret = MLAN_STATUS_FAILURE;
		goto mef_exit;
	}
	hostcmd->size = cpu_to_le16(cmd_len);
	mefcmd->Criteria = cpu_to_le32(mefcmd->Criteria);
	mefcmd->NumEntries = cpu_to_le16(mefcmd->NumEntries);
	hexdump("mefcfg", buffer + cmd_header_len, cmd_len, ' ');

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[MEF_CFG]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		if (cmd)
			free(cmd);
		return MLAN_STATUS_FAILURE;
	}

	ret = process_host_cmd_resp(HOSTCMD, buffer);

mef_exit:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;

}

/**
 *  @brief parse hex data
 *  @param fp       File handler
 *  @param dst      A pointer to receive hex data
 *  @return         length of hex data
 */
int
fparse_for_hex(FILE * fp, t_u8 *dst)
{
	char *ptr = NULL;
	t_u8 *dptr = NULL;
	char buf[256];

	dptr = dst;
	while (fgets(buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && (isspace(*ptr) || *ptr == '\t'))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (isxdigit(*ptr)) {
				ptr = convert2hex(ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return dptr - dst;
}

/** Config data header length */
#define CFG_DATA_HEADER_LEN 6

/**
 *  @brief Prepare cfg-data buffer
 *  @param argc             Number of arguments
 *  @param argv             A pointer to arguments array
 *  @param fp               File handler
 *  @param buf              A pointer to comand buffer
 *  @param cmd_header_len   Length of the command header
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
prepare_cfg_data_buffer(int argc, char *argv[], FILE * fp, t_u8 *buf,
			int cmd_header_len)
{
	int ln = 0, type;
	HostCmd_DS_GEN *hostcmd = NULL;
	HostCmd_DS_802_11_CFG_DATA *pcfg_data = NULL;

	memset(buf, 0, BUFFER_LENGTH - cmd_header_len - sizeof(t_u32));
	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_CFG_DATA);
	pcfg_data = (HostCmd_DS_802_11_CFG_DATA *)(buf + S_DS_GEN);
	pcfg_data->action =
		(argc == 4) ? HostCmd_ACT_GEN_GET : HostCmd_ACT_GEN_SET;
	type = atoi(argv[3]);
	if ((type < 1) || (type > 2)) {
		fprintf(stderr, "mlanutl: Invalid register type\n");
		return MLAN_STATUS_FAILURE;
	} else {
		pcfg_data->type = type;
	}
	if (argc == 5) {
		ln = fparse_for_hex(fp, pcfg_data->data);
	}
	pcfg_data->data_len = ln;
	hostcmd->size =
		cpu_to_le16(pcfg_data->data_len + S_DS_GEN +
			    CFG_DATA_HEADER_LEN);
	pcfg_data->data_len = cpu_to_le16(pcfg_data->data_len);
	pcfg_data->type = cpu_to_le16(pcfg_data->type);
	pcfg_data->action = cpu_to_le16(pcfg_data->action);

	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process cfgdata
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cfgdata(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	FILE *fp = NULL;
	int cmd_header_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX cfgdata <register type> <filename>\n");
		exit(1);
	}

	if (argc == 5) {
		fp = fopen(argv[4], "r");
		if (fp == NULL) {
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			exit(1);
		}
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(HOSTCMD);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for hostcmd failed\n");
		if (argc == 5)
			fclose(fp);
		return -ENOMEM;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		if (fp) {
			fclose(fp);
		}
		return MLAN_STATUS_FAILURE;
	}
	memset(cmd, 0, sizeof(struct eth_priv_cmd));

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, HOSTCMD, 0, NULL);

	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN> */
	hostcmd = (HostCmd_DS_GEN *)(buffer + cmd_header_len + sizeof(t_u32));

	ret = prepare_cfg_data_buffer(argc, argv, fp, (t_u8 *)hostcmd,
				      cmd_header_len);
	if (argc == 5)
		fclose(fp);

	if (ret == MLAN_STATUS_FAILURE)
		goto _exit_;

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[CFG_DATA]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		if (cmd)
			free(cmd);
		return MLAN_STATUS_FAILURE;
	}
	ret = process_host_cmd_resp(HOSTCMD, buffer);

_exit_:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Process transmission of mgmt frames
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmtframetx(int argc, char *argv[])
{
	struct ifreq ifr;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, arg_num = 0, ret = 0, i = 0;
	char *args[100], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 data_len = 0, type = 0, subtype = 0;
	t_u16 seq_num = 0, frag_num = 0, from_ds = 0, to_ds = 0;
	eth_priv_mgmt_frame_tx *pmgmt_frame = NULL;
	t_u8 *buffer = NULL;
	pkt_header *hdr = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX mgmtframetx <config/mgmt_frame.conf>\n");
		exit(1);
	}

	data_len = sizeof(eth_priv_mgmt_frame_tx);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);
	hdr = (pkt_header *)buffer;
	pmgmt_frame = (eth_priv_mgmt_frame_tx *)(buffer + sizeof(pkt_header));

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		arg_num = parse_line(line, args, 100);
		if (strcmp(args[0], "PktType") == 0) {
			type = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl = (type & 0x3) << 2;
		} else if (strcmp(args[0], "PktSubType") == 0) {
			subtype = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl |= subtype << 4;
		} else if (strncmp(args[0], "Addr", 4) == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("%s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				if (ret == MLAN_STATUS_FAILURE)
					goto done;
			}
			i = atoi(args[0] + 4);
			switch (i) {
			case 1:
				memcpy(pmgmt_frame->addr1, peer_mac, ETH_ALEN);
				break;
			case 2:
				memcpy(pmgmt_frame->addr2, peer_mac, ETH_ALEN);
				break;
			case 3:
				memcpy(pmgmt_frame->addr3, peer_mac, ETH_ALEN);
				break;
			case 4:
				memcpy(pmgmt_frame->addr4, peer_mac, ETH_ALEN);
				break;
			}
		} else if (strcmp(args[0], "Data") == 0) {
			for (i = 0; i < arg_num - 1; i++)
				pmgmt_frame->payload[i] =
					(t_u8)A2HEXDECIMAL(args[i + 1]);
			data_len += arg_num - 1;
		} else if (strcmp(args[0], "SeqNum") == 0) {
			seq_num = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->seq_ctl = seq_num << 4;
		} else if (strcmp(args[0], "FragNum") == 0) {
			frag_num = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->seq_ctl |= frag_num;
		} else if (strcmp(args[0], "FromDS") == 0) {
			from_ds = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl |= (from_ds & 0x1) << 9;
		} else if (strcmp(args[0], "ToDS") == 0) {
			to_ds = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl |= (to_ds & 0x1) << 8;
		}
	}
	pmgmt_frame->frm_len = data_len - sizeof(pmgmt_frame->frm_len);
#define MRVL_PKT_TYPE_MGMT_FRAME 0xE5
	hdr->pkt_len = data_len;
	hdr->TxPktType = MRVL_PKT_TYPE_MGMT_FRAME;
	hdr->TxControl = 0;
	hexdump("Frame Tx", buffer, data_len + sizeof(pkt_header), ' ');
	/* Send collective command */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)buffer;

	/* Perform ioctl */
	if (ioctl(sockfd, FRAME_TX_IOCTL, &ifr)) {
		perror("");
		printf("ERR:Could not send management frame.\n");
	} else {
		printf("Mgmt Frame sucessfully sent.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief set/get management frame passthrough
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmt_frame_passthrough(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 mask = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mask = *(t_u32 *)buffer;
	if (argc == 3)
		printf("Registed Management Frame Mask: 0x%x\n", mask);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief set/get hotspot enable/disable config
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hotspot_config(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 hotspotcfg = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hotspotcfg = *(t_u32 *)buffer;
	if (argc == 3)
		printf("HotSpot 2.0 Status: 0x%x\n", hotspotcfg);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief read current command
 *  @param ptr      A pointer to data
 *  @param curCmd   A pointer to the buf which will hold current command
 *  @return         NULL or the pointer to the left command buf
 */
static t_s8 *
readCurCmd(t_s8 *ptr, t_s8 *curCmd)
{
	t_s32 i = 0;
#define MAX_CMD_SIZE 64	/**< Max command size */

	while (*ptr != ']' && i < (MAX_CMD_SIZE - 1))
		curCmd[i++] = *(++ptr);

	if (*ptr != ']')
		return NULL;

	curCmd[i - 1] = '\0';

	return ++ptr;
}

/**
 *  @brief parse command and hex data
 *  @param fp       A pointer to FILE stream
 *  @param dst      A pointer to the dest buf
 *  @param cmd      A pointer to command buf for search
 *  @return         Length of hex data or MLAN_STATUS_FAILURE
 */
static int
fparse_for_cmd_and_hex(FILE * fp, t_u8 *dst, t_u8 *cmd)
{
	t_s8 *ptr;
	t_u8 *dptr;
	t_s8 buf[256], curCmd[64];
	t_s32 isCurCmd = 0;

	dptr = dst;
	while (fgets((char *)buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && isspace(*ptr))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (*ptr == '[' && *(ptr + 1) != '/') {
				ptr = readCurCmd(ptr, curCmd);
				if (!ptr)
					return MLAN_STATUS_FAILURE;

				if (strcasecmp((char *)curCmd, (char *)cmd))	/* Not equal */
					isCurCmd = 0;
				else
					isCurCmd = 1;
			}

			/* Ignore the rest if it is not correct cmd */
			if (!isCurCmd)
				break;

			if (*ptr == '[' && *(ptr + 1) == '/')
				return dptr - dst;

			if (isxdigit(*ptr)) {
				ptr = (t_s8 *)convert2hex((char *)ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return MLAN_STATUS_FAILURE;
}

/**
 * @brief      11ax HE capability and operation configure
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */

static int
process_11axcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int id_len = 0;
	FILE *fp = NULL;
	int ret = 0, cmd_header_len = 0;
	char config_id[20];
	char filename[256];

	if (argc != 3 && argc != 4) {
		printf("Err: Invalid number of arguments\n");
		printf("Usage: ./mlanutl <interface> 11axcfg [11axcfg.conf]\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);
	prepare_buffer(buffer, argv[2], 0, NULL);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = cmd_header_len;
	cmd->total_len = BUFFER_LENGTH;

	if (argc == 4) {
		memset(filename, 0, sizeof(filename));
		strncpy(filename, argv[3], sizeof(filename) - 1);

		fp = fopen(filename, "r");
		if (fp == NULL) {
			perror("fopen");
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			ret = -EFAULT;;
			goto done;
		}

		snprintf(config_id, sizeof(config_id), "Band");
		id_len = fparse_for_cmd_and_hex(fp, buffer + cmd_header_len,
						(t_u8 *)config_id);

		snprintf(config_id, sizeof(config_id), "HECap");
		fparse_for_cmd_and_hex(fp, buffer + cmd_header_len + id_len,
				       (t_u8 *)config_id);
		hexdump("Set 11axcfg", buffer + cmd_header_len,
			sizeof(mlan_ds_11ax_he_cfg), ' ');
		cmd->used_len = cmd_header_len + sizeof(mlan_ds_11ax_he_cfg);
	}
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: 11axcfg");
		ret = -EFAULT;
		goto done;
	}
	hexdump("11axcfg", buffer + cmd_header_len, sizeof(mlan_ds_11ax_he_cfg),
		' ');
done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process 11ax command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11axcmdcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	mlan_ds_11ax_cmd_cfg *axcmd = NULL;
	t_u32 action = 0;
	t_u32 prefix_len = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	if (strcmp(argv[3], "obss_pd_offset") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_SR_OBSS_PD_OFFSET);
	} else if (strcmp(argv[3], "enable_sr") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_SR_ENABLE);
	} else if (strcmp(argv[3], "beam_change") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_BEAM_CHANGE);
	} else if (strcmp(argv[3], "enable_htc") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_HTC_ENABLE);
	} else {
		printf("ERR:unknown command %s!\n", argv[3]);
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = strlen((char *)buffer);
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11axcmd fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prefix_len += strlen(CMD_NXP) + strlen(argv[2]);
	action = *(t_u32 *)(buffer + prefix_len);
	if (action == MLAN_ACT_GET) {
		axcmd = (mlan_ds_11ax_cmd_cfg *) (buffer + prefix_len +
						  sizeof(t_u32));
		switch (axcmd->sub_id) {
		case MLAN_11AXCMD_SR_SUBID:
			if (axcmd->param.sr_cfg.type ==
			    MRVL_DOT11AX_OBSS_PD_OFFSET_TLV_ID) {
				printf("HE SR <NON_SRG_OffSET, SRG_OFFSET> = %d,%d\n", axcmd->param.sr_cfg.param.obss_pd_offset.offset[0], axcmd->param.sr_cfg.param.obss_pd_offset.offset[1]);
			} else if (axcmd->param.sr_cfg.type ==
				   MRVL_DOT11AX_ENABLE_SR_TLV_ID) {
				printf("HE SR Spatial Reuse is %s(%d)\n",
				       axcmd->param.sr_cfg.param.sr_control.
				       control == 1 ? "enabled" : "disabled",
				       axcmd->param.sr_cfg.param.sr_control.
				       control);
			} else {
				printf("Unknown SR type 0x%x\n",
				       axcmd->param.sr_cfg.type);
			}
			break;
		case MLAN_11AXCMD_BEAM_SUBID:
			printf("HE Beam change %s(%d)\n",
			       axcmd->param.beam_cfg.value ==
			       1 ? "disabled" : "enabled",
			       axcmd->param.beam_cfg.value);
			break;
		case MLAN_11AXCMD_HTC_SUBID:
			printf("HTC transmission %s(%d)\n",
			       axcmd->param.htc_cfg.value ==
			       1 ? "enabled" : "disabled",
			       axcmd->param.htc_cfg.value);
			break;
		default:
			printf("Unknown sub_command 0x%x\n", axcmd->sub_id);
			break;
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;

}

/**
 *  @brief Send a WMM AC Queue configuration command to get/set/default params
 *
 *  Configure or get the parameters of a WMM AC queue. The command takes
 *    an optional Queue Id as a last parameter.  Without the queue id, all
 *    queues will be acted upon.
 *
 *  mlanutl mlanX qconfig set msdu <lifetime in TUs> [Queue Id: 0-3]
 *  mlanutl mlanX qconfig get [Queue Id: 0-3]
 *  mlanutl mlanX qconfig def [Queue Id: 0-3]
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qconfig(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_queue_config_t queue_config_cmd;
	mlan_wmm_ac_e ac_idx;
	mlan_wmm_ac_e ac_idx_start;
	mlan_wmm_ac_e ac_idx_stop;
	int cmd_header_len = 0;
	int ret = 0;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (argc < 4) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		return -ENOMEM;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	memset(&queue_config_cmd, 0x00, sizeof(wlan_ioctl_wmm_queue_config_t));
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (strcmp(argv[3], "get") == 0) {
		/*    3     4    5   */
		/* qconfig get [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				ret = -EINVAL;
				goto done;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			ret = -EINVAL;
			goto done;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_GET;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			memcpy(buffer + cmd_header_len,
			       (t_u8 *)&queue_config_cmd,
			       sizeof(queue_config_cmd));
			if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
				perror("qconfig ioctl");
			} else {
				memcpy((t_u8 *)&queue_config_cmd,
				       buffer + cmd_header_len,
				       sizeof(queue_config_cmd));
				printf("qconfig %s(%d): MSDU Lifetime GET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else if (strcmp(argv[3], "set") == 0) {
		if ((argc >= 5) && strcmp(argv[4], "msdu") == 0) {
			/*    3     4    5     6      7   */
			/* qconfig set msdu <value> [qid] */
			if (argc == 6) {
				ac_idx_start = WMM_AC_BK;
				ac_idx_stop = WMM_AC_VO;
			} else if (argc == 7) {
				if (atoi(argv[6]) < WMM_AC_BK ||
				    atoi(argv[6]) > WMM_AC_VO) {
					fprintf(stderr,
						"ERROR: Invalid Queue ID!\n");
					ret = -EINVAL;
					goto done;
				}
				ac_idx_start = atoi(argv[6]);
				ac_idx_stop = ac_idx_start;
			} else {
				fprintf(stderr,
					"Invalid number of parameters!\n");
				ret = -EINVAL;
				goto done;
			}
			queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_SET;
			queue_config_cmd.msduLifetimeExpiry = atoi(argv[5]);

			for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop;
			     ac_idx++) {
				queue_config_cmd.accessCategory = ac_idx;
				memcpy(buffer + cmd_header_len,
				       (t_u8 *)&queue_config_cmd,
				       sizeof(queue_config_cmd));
				if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
					perror("qconfig ioctl");
				} else {
					memcpy((t_u8 *)&queue_config_cmd,
					       buffer + cmd_header_len,
					       sizeof(queue_config_cmd));
					printf("qconfig %s(%d): MSDU Lifetime SET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
				}
			}
		} else {
			/* Only MSDU Lifetime provisioning accepted for now */
			fprintf(stderr, "Invalid set parameter: s/b [msdu]\n");
			ret = -EINVAL;
			goto done;
		}
	} else if (strncmp(argv[3], "def", strlen("def")) == 0) {
		/*    3     4    5   */
		/* qconfig def [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				ret = -EINVAL;
				goto done;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			ret = -EINVAL;
			goto done;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_DEFAULT;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			memcpy(buffer + cmd_header_len,
			       (t_u8 *)&queue_config_cmd,
			       sizeof(queue_config_cmd));
			if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
				perror("qconfig ioctl");
			} else {
				memcpy((t_u8 *)&queue_config_cmd,
				       buffer + cmd_header_len,
				       sizeof(queue_config_cmd));
				printf("qconfig %s(%d): MSDU Lifetime DEFAULT = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else {
		fprintf(stderr,
			"Invalid qconfig command; s/b [set, get, default]\n");
		ret = -EINVAL;
		goto done;
	}

	ret = MLAN_STATUS_SUCCESS;
done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Send an ADDTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in an ADDTS request to the associated AP.
 *
 *  Return the execution status of the command as well as the ADDTS response
 *    from the AP if any.
 *
 *  mlanutl mlanX addts <filename.conf> <section# of tspec> <timeout in ms>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addts(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_addts_req_t addtsReq;
	FILE *fp = NULL;
	char filename[48];
	char config_id[20];
	int cmd_header_len = 0, ret = 0, copy_len = 0;

	memset(&addtsReq, 0x00, sizeof(addtsReq));
	memset(filename, 0x00, sizeof(filename));

	if (argc != 6) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for buffer failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	strncpy(filename, argv[3], sizeof(filename) - 1);

	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = -EFAULT;
		goto done;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	addtsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    addtsReq.ieData,
						    (t_u8 *)config_id);

	if (addtsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)addtsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		ret = -EFAULT;
		goto done;
	}

	addtsReq.timeout_ms = atoi(argv[5]);

	printf("Cmd Input:\n");
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');
	copy_len = sizeof(addtsReq);
	memcpy(buffer + cmd_header_len, (t_u8 *)&addtsReq, copy_len);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: addts ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&addtsReq, buffer, strlen((const char *)buffer));
	printf("Cmd Output:\n");
	printf("ADDTS Command Result = %d\n", addtsReq.commandResult);
	printf("ADDTS IEEE Status    = %d\n", addtsReq.ieeeStatusCode);
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');

done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Send a DELTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in a DELTS request to the associated AP.
 *
 *  Return the execution status of the command.  There is no response to a
 *    DELTS from the AP.
 *
 *  mlanutl mlanX delts <filename.conf> <section# of tspec>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_delts(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_delts_req_t deltsReq;
	FILE *fp = NULL;
	char filename[48];
	char config_id[20];
	int cmd_header_len = 0, ret = 0, copy_len = 0;

	memset(&deltsReq, 0x00, sizeof(deltsReq));
	memset(filename, 0x00, sizeof(filename));

	if (argc != 5) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for buffer failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	strncpy(filename, argv[3], sizeof(filename) - 1);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = -EFAULT;;
		goto done;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	deltsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    deltsReq.ieData,
						    (t_u8 *)config_id);

	if (deltsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)deltsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		ret = -EFAULT;
		goto done;
	}

	printf("Cmd Input:\n");
	hexdump(config_id, deltsReq.ieData, deltsReq.ieDataLen, ' ');

	copy_len =
		sizeof(deltsReq) - sizeof(deltsReq.ieData) + deltsReq.ieDataLen;
	memcpy(buffer + cmd_header_len, (t_u8 *)&deltsReq, copy_len);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("delts ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&deltsReq, buffer, strlen((const char *)buffer));
	printf("Cmd Output:\n");
	printf("DELTS Command Result = %d\n", deltsReq.commandResult);

done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Get the current status of the WMM Queues
 *
 *  Command: mlanutl mlanX qstatus
 *
 *  Retrieve the following information for each AC if wmm is enabled:
 *        - WMM IE ACM Required
 *        - Firmware Flow Required
 *        - Firmware Flow Established
 *        - Firmware Queue Enabled
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_qstatus(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_queue_status_t qstatus;
	int ret = 0;
	mlan_wmm_ac_e acVal;

	if (argc != 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	memset(&qstatus, 0x00, sizeof(qstatus));

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("qstatus ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&qstatus, buffer, strlen((const char *)buffer));
	for (acVal = WMM_AC_BK; acVal <= WMM_AC_VO; acVal++) {
		switch (acVal) {
		case WMM_AC_BK:
			printf("BK: ");
			break;
		case WMM_AC_BE:
			printf("BE: ");
			break;
		case WMM_AC_VI:
			printf("VI: ");
			break;
		case WMM_AC_VO:
			printf("VO: ");
			break;
		default:
			printf("??: ");
		}

		printf("ACM[%c], FlowReq[%c], FlowCreated[%c], Enabled[%c],"
		       " DE[%c], TE[%c]\n",
		       (qstatus.acStatus[acVal].wmmAcm ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowRequired ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowCreated ? 'X' : ' '),
		       (qstatus.acStatus[acVal].disabled ? ' ' : 'X'),
		       (qstatus.acStatus[acVal].deliveryEnabled ? 'X' : ' '),
		       (qstatus.acStatus[acVal].triggerEnabled ? 'X' : ' '));
	}

done:

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Get the current status of the WMM Traffic Streams
 *
 *  Command: mlanutl mlanX ts_status
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_ts_status(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_ts_status_t ts_status;
	int tid;
	int cmd_header_len = 0, ret = 0;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (argc != 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */

	printf("\nTID   Valid    AC   UP   PSB   FlowDir  MediumTime\n");
	printf("---------------------------------------------------\n");

	for (tid = 0; tid <= 7; tid++) {
		memset(buffer, 0, BUFFER_LENGTH);
		prepare_buffer(buffer, argv[2], 0, NULL);
		memset(&ts_status, 0x00, sizeof(ts_status));
		ts_status.tid = tid;

		/* Initialize the ifr structure */
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		memcpy(buffer + cmd_header_len, (t_u8 *)&ts_status,
		       sizeof(ts_status));

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			perror("ts_status ioctl");
			ret = -EFAULT;
			goto done;
		}

		memcpy(&ts_status, buffer, strlen((const char *)buffer));
		printf(" %02d     %3s    %2s    %u     %c    ",
		       ts_status.tid,
		       (ts_status.valid ? "Yes" : "No"),
		       (ts_status.
			valid ? ac_str_tbl[ts_status.accessCategory] : "--"),
		       ts_status.userPriority, (ts_status.psb ? 'U' : 'L'));

		if ((ts_status.flowDir & 0x03) == 0) {
			printf("%s", " ---- ");
		} else {
			printf("%2s%4s",
			       (ts_status.flowDir & 0x01 ? "Up" : ""),
			       (ts_status.flowDir & 0x02 ? "Down" : ""));
		}

		printf("%12u\n", ts_status.mediumTime);
	}
	printf("\n");

done:

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Set/get WMM IE QoS info parameter
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qos_config(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX qoscfg [QoS]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: qoscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("WMM QoS Info: %#x\n", *buffer);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set/get MAC control configuration
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int
process_macctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 mac_ctrl = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX macctrl [macctrl]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: macctrl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mac_ctrl = *(t_u32 *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("MAC Control: 0x%08x\n", mac_ctrl);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process generic commands
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_generic(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if ((ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) < 0) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: %s fail\n", argv[2]);
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("%s command response received: %s\n", argv[2], buffer);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set/Get mlanX FW side MAC address
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_fwmacaddr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX fwmacaddr [fwmacaddr]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: fwmacaddr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("FW MAC address = ");
		print_mac(buffer);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef RX_PACKET_COALESCE
static void
print_rx_packet_coalesc_help()
{
	printf("\nUSAGE: rxpktcoal_cfg [PKT-THRESHOLD] [TIMEOUT]\n\n");
	printf("OPTIONS:");
	printf("PKT-THRESHOLD: count after which packets would be sent to host. Valid values 1-7\n");
	printf("\tTIMEOUT: Time after which packets would be sent to host Valid values 1-4\n");
	printf("\tCoalescing is disabled if both or either of PKT-THRESHOLD or TIMEOUT is zero\n\n");
	printf("\tEmpty - Get current packet coalescing settings\n");
}

static int
process_rx_pkt_coalesce_cfg(int argc, char *argv[])
{

	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	tlvbuf_rx_pkt_coal_t *rx_pkt_info;
	int ret = MLAN_STATUS_SUCCESS;

	if ((argc != 3) && (argc != 5)) {
		printf("ERR:Invalid no. of arguments\n");
		print_rx_packet_coalesc_help();
		return MLAN_STATUS_FAILURE;
	}

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if ((ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) < 0) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: %s fail\n", argv[2]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {	/* GET operation */
		rx_pkt_info = (tlvbuf_rx_pkt_coal_t *)(buffer);
		printf("RX packet coalesce configuraion:\n");
		printf("Packet threshold=%d\n", rx_pkt_info->rx_pkt_count);
		printf("Timeout=%dms\n", rx_pkt_info->delay);
	} else {
		printf("RX packet coalesce configuration set successfully.\n");
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;

}
#endif

/**
 *  @brief Process regioncode configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_regioncode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 regioncode;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: regioncode config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&regioncode, buffer, sizeof(regioncode));
		printf("regioncode: %d\n", regioncode);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief	Process cfpinfo get command
 *  @param argc	Number of arguments
 *  @param argv	Pointer to arguments array
 *  @return		MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cfpinfo(int argc, char *argv[])
{
	t_u8 *data, *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 i, size = 0;
	t_u32 j;
	t_u32 rows, cols;
	struct chan_freq_power {
		t_u16 channel;
		t_u32 freq;
		t_u16 max_tx_power;
		t_u8 passive_scan_or_radar_detect;
		t_u16 flags;
		t_u8 blacklist;
	} *cfp;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cfpinfo cmd failed\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	data = buffer;
	size = *(t_u32 *)data;
	if (!size)
		goto out;

	/* Region code is stored in first 2 bytes, country code in next 3 bytes
	 * and environment, if available, in the following byte
	 */
	data += sizeof(size);
	printf("Region Code  : 0x%x\n", *(t_u16 *)data);
	data += 2;
	printf("Country Code : %c%c\n", *data, *(data + 1));
	data += 3;
	if (size == 6) {
		printf("Environment  : 0x%x\n", *data);
		data++;
	}
	/* Print cfp tables */
	size = *(t_u32 *)data;
	if (!size)
		goto out;

	data += sizeof(size);
	printf("\n2.4GHz Channels:\n");
	printf("%8s%10s%6s%11s%16s%16s%17s\n", "Channel", "isPassive",
	       "isDFS",
	       "isDisabled", "is40MHzDisabled",
	       "is80MHzDisabled", "is160MHzDisabled");
	i = 0;
	while (i < size) {
		if (!(*data))
			goto out;
		cfp = (struct chan_freq_power *)data;
		printf("%8u%10u%6u%11u%16u%16c%17c\n",
		       cfp->channel,
		       (cfp->flags & NXP_CHANNEL_PASSIVE) ? 1 : 0,
		       cfp->passive_scan_or_radar_detect,
		       (cfp->flags & NXP_CHANNEL_DISABLED) ? 1 : 0,
		       (cfp->flags & NXP_CHANNEL_NOHT40) ? 1 : 0, '-', '-');
		data += sizeof(struct chan_freq_power);
		i += sizeof(struct chan_freq_power);
	}
	size = *(t_u32 *)data;
	if (!size)
		goto out;

	data += sizeof(size);

	printf("\n5GHz Channels:\n");
	printf("%8s%10s%6s%11s%16s%16s%17s\n", "Channel", "isPassive",
	       "isDFS",
	       "isDisabled", "is40MHzDisabled",
	       "is80MHzDisabled", "is160MHzDisabled");
	i = 0;
	while (i < size) {
		if (!(*data))
			goto out;
		cfp = (struct chan_freq_power *)data;
		printf("%8u%10u%6u%11u%16u%16u%17c\n",
		       cfp->channel,
		       (cfp->flags & NXP_CHANNEL_PASSIVE) ? 1 : 0,
		       cfp->passive_scan_or_radar_detect,
		       (cfp->flags & NXP_CHANNEL_DISABLED) ? 1 : 0,
		       (cfp->flags & NXP_CHANNEL_NOHT40) ? 1 : 0,
		       (cfp->flags & NXP_CHANNEL_NOHT80) ? 1 : 0, '-');
		data += sizeof(struct chan_freq_power);
		i += sizeof(struct chan_freq_power);
	}

	/* Print power tables */
	size = *(t_u32 *)data;
	if (!size)
		goto out;

	data += sizeof(size);
	rows = *(t_u32 *)data;
	data += sizeof(size);
	cols = *(t_u32 *)data;
	data += sizeof(size);
	if (!rows || !cols)
		goto out;

	printf("\n2.4GHz Power Table:\n");
	printf("%8s ", "Channel");
	for (i = 0; i < cols - 1; i++)
		printf(" m%02d", i);

	for (i = 0; i < rows; i++) {
		printf("\n%8u ", *data++);
		for (j = 1; j < cols; j++)
			printf("%3u ", *data++);
	}
	printf("\n");
	size = *(t_u32 *)data;
	if (!size)
		goto out;

	data += sizeof(size);
	rows = *(t_u32 *)data;
	data += sizeof(size);
	cols = *(t_u32 *)data;
	data += sizeof(size);
	if (!rows || !cols)
		goto out;

	printf("\n5GHz Power Table:\n");
	printf("%8s ", "Channel");
	for (i = 0; i < cols - 1; i++)
		printf(" m%02d", i);

	for (i = 0; i < rows; i++) {
		printf("\n%8u ", *data++);
		for (j = 1; j < cols; j++)
			printf("%3u ", *data++);
	}
	/* Print Modulation Conversion Information */
	printf("\n\nModulation Conversions:\n");
	j = 0;
	for (i = 0; i < 10; i++)
		printf("m%02d: %s\n", j++, mod_conv_bg_1x1[i]);
	for (i = 0; i < 6; i++)
		printf("m%02d: %s\n", j++, mod_conv_bg_2x2[i]);
	if (j >= cols - 1)
		goto out;
	for (i = 0; i < 6; i++)
		printf("m%02d: %s\n", j++, mod_conv_a_1x1[i]);
	for (i = 0; i < 6; i++)
		printf("m%02d: %s\n", j++, mod_conv_a_2x2[i]);
out:
	printf("\n");
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process offchannel configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_offchannel(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 7) {
		printf("Incorrect number of parameters\n");
		printf("mlanutl mlanX offchannel <action> [<channel> <duration> <bandwidth>]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: offchannel config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("%s\n", buffer);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process link statistics
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_linkstats(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	t_u8 *data = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	t_u32 cmd_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* prepare the host cmd */
	data = buffer;
	prepare_buffer(buffer, HOSTCMD, 0, NULL);
	data += strlen(CMD_NXP) + strlen(HOSTCMD);
	/* add buffer size field */
	cmd_len =
		sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_LINK_STATS_SUMMARY);
	cmd_len = cpu_to_le32(cmd_len);
	memcpy(data, &cmd_len, sizeof(t_u32));
	data += sizeof(t_u32);
	/* add cmd header */
	hostcmd = (HostCmd_DS_GEN *)data;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_LINK_STATS_SUMMARY);
	hostcmd->size = cpu_to_le16(sizeof(HostCmd_DS_GEN));
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: linkstats fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	ret = process_host_cmd_resp(HOSTCMD, buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(STA_SUPPORT)
/**
 * @brief      Configure PMF parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_pmfcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct eth_priv_pmfcfg pmfcfg;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc > 5) || (argc == 4) ||
	    ((argc == 5) && ((!atoi(argv[3])) && atoi(argv[4])))) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pmfcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memcpy(&pmfcfg, buffer, sizeof(struct eth_priv_pmfcfg));
		printf("Management Frame Protection Capability: %s\n",
		       (pmfcfg.mfpc ? "Yes" : "No"));
		if (pmfcfg.mfpc)
			printf("Management Frame Protection: %s\n",
			       (pmfcfg.mfpr ? "Required" : "Optional"));
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Process extended version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_verext(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX verext [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: verext fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (cmd->used_len)
		printf("Extended Version string received: %s\n", buffer);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Makes USB device to suspend
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_usb_suspend(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX usbsuspend\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: usbsuspend fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memcpy(&status, buffer, sizeof(status));
	if (status == -EFAULT)
		printf("USB Device is already suspended\n");
	else if (status == 0)
		printf("USB Device has been suspended\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process extended version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_usb_resume(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX usbresume\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: usbresume fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memcpy(&status, buffer, sizeof(status));
	if (status == -EFAULT)
		printf("USB Device is already resumed\n");
	else if (status == 0)
		printf("USB Device has been resumed\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

#if defined(STA_SUPPORT) && defined(STA_WEXT)
/**
 *  @brief Set/Get radio
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_radio_ctrl(int argc, char *argv[])
{
	int ret = 0, radio = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX radioctrl [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: radioctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&radio, buffer, sizeof(radio));
		if (radio == 0) {
			printf("Radio is Disabled\n");
		} else if (radio == 1) {
			printf("Radio is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Implement WMM enable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX wmmcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wmmcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, buffer, sizeof(status));
		if (status == 0) {
			printf("WMM is Disabled\n");
		} else if (status == 1) {
			printf("WMM is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

static int
process_wmm_param_config(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	t_u8 *data = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	t_u32 cmd_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i = 0;
	t_u32 wmm_param[MAX_AC_QUEUES][5];
	t_u8 aci = 0;
	t_u8 aifsn = 0;
	t_u8 ecwmax = 0;
	t_u8 ecwmin = 0;
	t_u16 txop = 0;
	HostCmd_DS_WMM_PARAM_CONFIG *cmd_data = NULL;

	/* Sanity tests */
	if ((argc < 3) || (argc > (3 + 5 * MAX_AC_QUEUES)) ||
	    (((argc - 3) % 5) != 0)) {
		printf("Incorrect number of parameters\n");
		printf("Format reference: mlanutl mlanX wmm_param_config \n");
		printf("\t[AC_BE AIFSN ECW_MAX ECW_MIN TX_OP]\n");
		printf("\t[AC_BK AIFSN ECW_MAX ECW_MIN TX_OP]\n");
		printf("\t[AC_VI AIFSN ECW_MAX ECW_MIN TX_OP]\n");
		printf("\t[AC_VO AIFSN ECW_MAX ECW_MIN TX_OP]\n");
		return MLAN_STATUS_FAILURE;
	}

	for (i = 3; i < argc; i++) {
		if (IS_HEX_OR_DIGIT(argv[i]) == MLAN_STATUS_FAILURE) {
			printf("ERR: Only Number values are allowed\n");
			return MLAN_STATUS_FAILURE;
		}
	}

	i = 3;
	memset(wmm_param, 0x00, sizeof(wmm_param));
	while (i < argc) {
		aci = A2HEXDECIMAL(argv[i]);
		aifsn = A2HEXDECIMAL(argv[i + 1]);
		ecwmax = A2HEXDECIMAL(argv[i + 2]);
		ecwmin = A2HEXDECIMAL(argv[i + 3]);
		txop = A2HEXDECIMAL(argv[i + 4]);
		if ((aci <= 3) && !wmm_param[aci][0]) {
			if (((aifsn >= 2) && (aifsn <= 15))
			    && (ecwmax <= 15)
			    && (ecwmin <= 15)
			    && (txop <= 0xFFFF)) {
				wmm_param[aci][0] = TRUE;
				wmm_param[aci][1] = aifsn;
				wmm_param[aci][2] = ecwmax;
				wmm_param[aci][3] = ecwmin;
				wmm_param[aci][4] = txop;
			} else {
				printf("wmm parmams invalid\n");
				return MLAN_STATUS_FAILURE;
			}
		} else {
			printf("aci out of range or repeated \n");
			return MLAN_STATUS_FAILURE;
		}
		i = i + 5;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	memset(buffer, 0x00, BUFFER_LENGTH);

	/* prepare the host cmd */
	data = buffer;
	prepare_buffer(buffer, HOSTCMD, 0, NULL);
	data += strlen(CMD_NXP) + strlen(HOSTCMD);
	/* add buffer size field */
	cmd_len = sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_WMM_PARAM_CONFIG);
	cmd_len = cpu_to_le32(cmd_len);
	memcpy(data, &cmd_len, sizeof(t_u32));
	data += sizeof(t_u32);
	/* add cmd header */
	hostcmd = (HostCmd_DS_GEN *)data;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_WMM_PARAM_CONFIG);
	hostcmd->size =
		cpu_to_le16(sizeof(HostCmd_DS_GEN) +
			    sizeof(HostCmd_DS_WMM_PARAM_CONFIG));
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	data += sizeof(HostCmd_DS_GEN);
	cmd_data = (HostCmd_DS_WMM_PARAM_CONFIG *) data;
	if (argc > 3) {
		cmd_data->action = ACTION_SET;
		for (i = 0; i < MAX_AC_QUEUES; i++) {
			if (wmm_param[i][0] == TRUE) {
				cmd_data->ac_params[i].aci_aifsn.acm = 1;
				cmd_data->ac_params[i].aci_aifsn.aci = (t_u8)i;
				cmd_data->ac_params[i].aci_aifsn.aifsn =
					(t_u8)(wmm_param[i][1]);
				cmd_data->ac_params[i].ecw.ecw_max =
					(t_u8)(wmm_param[i][2]);
				cmd_data->ac_params[i].ecw.ecw_min =
					(t_u8)(wmm_param[i][3]);
				cmd_data->ac_params[i].tx_op_limit =
					cpu_to_le16((t_u16)(wmm_param[i][4]));
			}
		}
	} else
		cmd_data->action = ACTION_GET;

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: linkstats fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	ret = process_host_cmd_resp(HOSTCMD, buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(STA_SUPPORT)
/**
 *  @brief Implement 802.11D enable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11d_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX 11dcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11dcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, buffer, sizeof(status));
		if (status == 0) {
			printf("802.11D is Disabled\n");
		} else if (status == 1) {
			printf("802.11D is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Implement 802.11D clear chan table command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11d_clr_tbl(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX 11dclrtbl\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11dclrtbl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

#ifndef OPCHAN
/**
 *  @brief Set/Get WWS configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wws_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX wwscfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wwscfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, buffer, sizeof(status));
		if (status == 1) {
			printf("WWS is Enabled\n");
		} else if (status == 0) {
			printf("WWS is Disabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

#if defined(REASSOCIATION)
/**
 *  @brief Set/Get reassociation settings
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_set_get_reassoc(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX reassoctrl [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: reassoctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, buffer, sizeof(status));
		if (status == 1) {
			printf("Re-association is Enabled\n");
		} else if (status == 0) {
			printf("Re-association is Disabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Get Transmit buffer size
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txbuf_cfg(int argc, char *argv[])
{
	int ret = 0, buf_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX txbufcfg\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txbufcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memcpy(&buf_size, buffer, sizeof(buf_size));
	printf("Transmit buffer size is %d\n", buf_size);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

#ifdef STA_SUPPORT
/**
 *  @brief Set/Get auth type
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_set_get_auth_type(int argc, char *argv[])
{
	int ret = 0, auth_type = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX authtype [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: authtype fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&auth_type, buffer, sizeof(auth_type));
		if (auth_type == 1) {
			printf("802.11 shared key authentication\n");
		} else if (auth_type == 0) {
			printf("802.11 open system authentication\n");
		} else if (auth_type == 3) {
			printf("802.11 WPA3 SAE authentication\n");
		} else if (auth_type == 255) {
			printf("Allow open system or shared key authentication\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Set/get user provisioned local power constraint
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11h_local_pwr_constraint(int argc, char *argv[])
{
	int ret = 0, power_cons = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX powercons [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: powercons fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		/* Process result */
		memcpy(&power_cons, buffer, sizeof(power_cons));
		printf("Local power constraint is %d dbm\n", power_cons);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Set/get HT stream configurations
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_ht_stream_cfg(int argc, char *argv[])
{
	int ret = 0, mode = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX htstreamcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htstreamcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&mode, buffer, sizeof(mode));
	if (mode == HT_STREAM_MODE_1X1)
		printf("HT stream is in 1x1 mode\n");
	else if (mode == HT_STREAM_MODE_2X2)
		printf("HT stream is in 2x2 mode\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Set mimo switch configurations
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mimo_switch(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	t_u8 tx_antmode = 0, rx_antmode = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 5) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX mimoswitch [txpath_antmode] [rxpath_antmode]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	tx_antmode = (t_u8)a2hex_or_atoi(argv[3]);
	rx_antmode = (t_u8)a2hex_or_atoi(argv[4]);
	if (!
	    ((tx_antmode == 1 && rx_antmode == 1) ||
	     (tx_antmode == 2 && rx_antmode == 2) || (rx_antmode == 3 &&
						      (tx_antmode == 1 ||
						       tx_antmode == 3)))) {
		printf("Error: invalid arguments\n");
		printf("The valid values of txpath_antmode and rxpath_antmode are 1/1 2/2 3/3 1/3 \n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: mimoswitch fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	printf("Successfully set Tx path antenna mode: %d, Rx path antenna mode: %d\n", tx_antmode, rx_antmode);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Get thermal reading
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_thermal(int argc, char *argv[])
{
	int ret = 0, thermal = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX thermal\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: thermal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&thermal, buffer, sizeof(thermal));
	printf("Thermal reading is %d\n", thermal);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#ifdef STA_SUPPORT
/**
 *  @brief Get signal
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_get_signal(int argc, char *argv[])
{
#define DATA_SIZE   12
	int ret = 0, data[DATA_SIZE], i = 0, copy_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	memset(data, 0, sizeof(data));
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX getsignal [m] [n]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getsignal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	copy_size = MIN(cmd->used_len, DATA_SIZE * sizeof(int));
	memcpy(&data, buffer, copy_size);
	printf("Get signal output is\t");
	for (i = 0; i < (int)(copy_size / sizeof(int)); i++)
		printf("%d\t", data[i]);
	printf("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Set signalext cfg
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_signalext_cfg(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX signalextcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: signalext cfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Get signal
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_get_signal_ext(int argc, char *argv[])
{
#define MAX_NUM_PATH 3
#define PATH_SIZE  13
#define PATH_A 		1
#define PATH_B		2
#define PATH_AB		3
	int ret = 0, data[PATH_SIZE * MAX_NUM_PATH] = { 0 };
	int i = 0, copy_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u8 num_path = 0;

	memset(data, 0, sizeof(data));
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3 && argc != 4) {
		printf("Error: invalid no of arguments\n");
		if (strncmp(argv[2], "getsignalextv2", strlen("getsignalextv2"))
		    == 0)
			printf("mlanutl mlanX getsignalextv2 [m]\n");
		else if (strncmp
			 (argv[2], "getsignalext", strlen("getsignalext")) == 0)
			printf("mlanutl mlanX getsignalext [m]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getsignal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	copy_size = cmd->used_len;
	memcpy(&data, (int *)buffer, copy_size);

	num_path = copy_size / sizeof(int) / PATH_SIZE;
	for (i = 0; i < num_path; i++) {
		if (data[i * PATH_SIZE] == PATH_A)
			printf("PATH A:   %d %d %d %d %d %d %d %d %d %d %d %d\n", data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2], data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4], data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6], data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8], data[i * PATH_SIZE + 9], data[i * PATH_SIZE + 10], data[i * PATH_SIZE + 11], data[i * PATH_SIZE + 12]);
		else if (data[i * PATH_SIZE] == PATH_B)
			printf("PATH B:   %d %d %d %d %d %d %d %d %d %d %d %d\n", data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2], data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4], data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6], data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8], data[i * PATH_SIZE + 9], data[i * PATH_SIZE + 10], data[i * PATH_SIZE + 11], data[i * PATH_SIZE + 12]);
		else if (data[i * PATH_SIZE] == PATH_AB)
			printf("PATH A+B: %d %d %d %d %d %d %d %d %d %d %d %d\n", data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2], data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4], data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6], data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8], data[i * PATH_SIZE + 9], data[i * PATH_SIZE + 10], data[i * PATH_SIZE + 11], data[i * PATH_SIZE + 12]);
	}
	printf("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif /* #ifdef STA_SUPPORT */

/**
 *  @brief Set/Get beacon interval
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_beacon_interval(int argc, char *argv[])
{
	int ret = 0, bcninterval = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX bcninterval [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bcninterval fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&bcninterval, buffer, sizeof(bcninterval));
	printf("Beacon interval is %d\n", bcninterval);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Get/Set inactivity timeout extend
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_inactivity_timeout_ext(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	int data[4];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 6) && (argc != 7)) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: inactivityto fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("Timeout unit is %d us\n"
		       "Inactivity timeout for unicast data is %d ms\n"
		       "Inactivity timeout for multicast data is %d ms\n"
		       "Inactivity timeout for new Rx traffic is %d ms\n",
		       data[0], data[1], data[2], data[3]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get chnrgpwr
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
get_chnrgpwr(FILE * fp_raw, char *argv[], t_u8 *buffer, t_u16 len,
	     struct eth_priv_cmd *cmd)
{
	struct ifreq ifr;
	mlan_ds_misc_chnrgpwr_cfg *chnrgpwr_cfg = NULL;
	int i = 0;
	int j = 0;

	memset(buffer, 0, len);
	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));
	chnrgpwr_cfg = (mlan_ds_misc_chnrgpwr_cfg *) (buffer + strlen(argv[2]));
	if (cmd) {
		/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
		memset(cmd, 0, sizeof(struct eth_priv_cmd));
		memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
		cmd->buf = buffer;
#endif
		cmd->used_len = 0;
		cmd->total_len = len;
	}
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get_chnrgpwr fail\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Process result */
	printf("Get region channel power table:  len=%d\n",
	       chnrgpwr_cfg->length);
	if (fp_raw) {
		i = j = 0;
		while (i < chnrgpwr_cfg->length) {
			for (j = 0; j < 16; j++) {
				fprintf(fp_raw, "%02x ",
					chnrgpwr_cfg->chnrgpwr_buf[i]);
				if (++i >= chnrgpwr_cfg->length)
					break;
			}
			fputc('\n', fp_raw);
		}
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Process Get region code channel power
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_get_chnrgpwr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	FILE *fp_raw = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(sizeof(mlan_ds_misc_chnrgpwr_cfg) +
				strlen(argv[2]));
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, sizeof(mlan_ds_misc_chnrgpwr_cfg) + strlen(argv[2]));
	/* Sanity tests */
	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX get_chnrgpwr logfile_name\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	fp_raw = fopen(argv[3], "w");
	if (fp_raw == NULL) {
		fprintf(stderr,
			"Cannot open the destination raw_data file %s\n",
			argv[4]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	ret = get_chnrgpwr(fp_raw, argv, buffer,
			   sizeof(mlan_ds_misc_chnrgpwr_cfg) + strlen(argv[2]),
			   cmd);
done:
	if (fp_raw)
		fclose(fp_raw);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 * @brief      Get txpwrlimit
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
get_txpwrlimit(FILE * fp_raw, char *argv[], t_u16 sub_band, t_u8 *buffer,
	       t_u16 len, struct eth_priv_cmd *cmd)
{
	struct ifreq ifr;
	mlan_ds_misc_chan_trpc_cfg *trcp_cfg = NULL;
	MrvlIETypes_ChanTRPCConfig_t *trpc_tlv = NULL;
	MrvlIEtypes_Data_t *pTlvHdr;
	int left_len;
	int mod_num = 0;
	int i = 0;
	int j = 0;
	t_u8 *pByte = NULL;

	memset(buffer, 0, len);
	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));
	trcp_cfg = (mlan_ds_misc_chan_trpc_cfg *) (buffer + strlen(argv[2]));
	trcp_cfg->sub_band = sub_band;
	if (cmd) {
		/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
		memset(cmd, 0, sizeof(struct eth_priv_cmd));
		memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
		cmd->buf = buffer;
#endif
		cmd->used_len = 0;
		cmd->total_len = len;
	}
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get_txpwrlimit fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Process result */
	printf("------------------------------------------------------------------------------------\n");
	printf("Get txpwrlimit: sub_band=0x%x len=%d\n", trcp_cfg->sub_band,
	       trcp_cfg->length);
	pByte = trcp_cfg->trpc_buf + S_DS_GEN + 4;
	left_len = trcp_cfg->length - S_DS_GEN - 4;
	while (left_len >= sizeof(pTlvHdr->header)) {
		pTlvHdr = (MrvlIEtypes_Data_t *)pByte;
		pTlvHdr->header.len = le16_to_cpu(pTlvHdr->header.len);

		switch (le16_to_cpu(pTlvHdr->header.type)) {
		case TLV_TYPE_CHAN_TRPC_CONFIG:
			trpc_tlv = (MrvlIETypes_ChanTRPCConfig_t *) pTlvHdr;
			printf("StartFreq: %d\n", trpc_tlv->start_freq);
			printf("ChanWidth: %d\n", trpc_tlv->width);
			printf("ChanNum:   %d\n", trpc_tlv->chan_num);
			mod_num =
				(pTlvHdr->header.len -
				 4) / sizeof(mod_group_setting);
			printf("Pwr:");
			for (i = 0; i < mod_num; i++) {
				if (i == (mod_num - 1))
					printf("%d,%d",
					       trpc_tlv->mod_group[i].mod_group,
					       trpc_tlv->mod_group[i].power);
				else
					printf("%d,%d,",
					       trpc_tlv->mod_group[i].mod_group,
					       trpc_tlv->mod_group[i].power);
			}
			printf("\n");
			break;
		default:
			break;
		}
		left_len -= (pTlvHdr->header.len + sizeof(pTlvHdr->header));
		pByte += pTlvHdr->header.len + sizeof(pTlvHdr->header);
	}
	if (fp_raw) {
		switch (sub_band) {
		case 0:
			fprintf(fp_raw, "txpwrlimit_2g_get={\n");
			break;
		case 0x10:
			fprintf(fp_raw, "txpwrlimit_5g_sub0_get={\n");
			break;
		case 0x11:
			fprintf(fp_raw, "txpwrlimit_5g_sub1_get={\n");
			break;
		case 0x12:
			fprintf(fp_raw, "txpwrlimit_5g_sub2_get={\n");
			break;
		default:
			break;
		}
		i = j = 0;
		while (i < trcp_cfg->length) {
			for (j = 0; j < 16; j++) {
				fprintf(fp_raw, "%02x ", trcp_cfg->trpc_buf[i]);
				if (++i >= trcp_cfg->length)
					break;
			}
			fputc('\n', fp_raw);
		}
		fprintf(fp_raw, "}\n\n");
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get txpwrlimit
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_get_txpwrlimit(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	t_u16 sub_band = 0;
	FILE *fp_raw = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(sizeof(mlan_ds_misc_chan_trpc_cfg) +
				strlen(argv[2]));
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, sizeof(mlan_ds_misc_chan_trpc_cfg) + strlen(argv[2]));
	/* Sanity tests */
	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX get_txpwrlimit [0/0x10/0x11/0x12/0x1f/0xff]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	sub_band = a2hex_or_atoi(argv[3]);
	if (argc == 5) {
		fp_raw = fopen(argv[4], "w");
		if (fp_raw == NULL) {
			fprintf(stderr,
				"Cannot open the destination raw_data file %s\n",
				argv[4]);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}
	switch (sub_band) {
	case 0:
	case 0x10:
	case 0x11:
	case 0x12:
		ret = get_txpwrlimit(fp_raw, argv, sub_band, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		break;
	case 0x1f:
		ret = get_txpwrlimit(fp_raw, argv, 0x10, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x11, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x12, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		break;
	case 0xff:
		ret = get_txpwrlimit(fp_raw, argv, 0, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x10, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x11, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x12, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
				     strlen(argv[2]), cmd);
		break;
	default:
		printf("Error: invalid arguments\n");
		printf("mlanutl mlanX/uapX get_txpwrlimit [0/0x10/0x11/0x12/0x1f/0xff]\n");
		break;
	}
done:
	if (fp_raw)
		fclose(fp_raw);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief get hostcmd data
 *
 *  @param ln           A pointer to line number
 *  @param buf          A pointer to hostcmd data
 *  @param size         A pointer to the return size of hostcmd buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static int
get_hostcmd_raw_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	char line[512], *pos;
	char *ptr = NULL;
	t_u8 *dptr = NULL;

	dptr = buf;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		(*ln)++;
		if (strcmp(pos, "}") == 0) {
			break;
		}
		ptr = line;
		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && (isspace(*ptr) || *ptr == '\t'))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (isxdigit(*ptr)) {
				ptr = convert2hex(ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}
	*size = dptr - buf;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief parse the trpc command raw data and fill in trpc table.
 *
 *  @param trpc_raw_data   trpc hostcmd raw data
 *  @param length          total length of trcp raw data
 *  @param trcp_buf        trpc table buffer
 *  @param max_chan        max channel number support in trpc table
 *  @param chan_idx        index in trcp table
 *  @param cmd_no		   buffer pointer to return cmd_no
 *  @param cmd_act		   buffer pointer to return cmd_action.
 *  @return                next index in trpc table
 */
int
parse_trpc_raw_data(t_u8 *trpc_raw_data, t_u16 length, t_u8 *trpc_buf,
		    t_u8 max_chan, int *chan_idx, t_u16 *cmd_no, t_u16 *cmd_act)
{
	MrvlIETypes_ChanTRPCConfig_t *trpc_tlv = NULL;
	MrvlIEtypes_Data_t *pTlvHdr;
	int left_len;
	t_u8 *pByte = NULL;
	int idx = *chan_idx;
	ChanTRPCConfig_t *pchan_trpc =
		(ChanTRPCConfig_t *) ((t_u8 *)trpc_buf +
				      idx * sizeof(ChanTRPCConfig_t));
	*cmd_no = le16_to_cpu(*(t_u16 *)trpc_raw_data);
	*cmd_act = le16_to_cpu(*(t_u16 *)(trpc_raw_data + S_DS_GEN));

	pByte = trpc_raw_data + S_DS_GEN + 4;
	left_len = length - S_DS_GEN - 4;
	while (left_len >= sizeof(pTlvHdr->header)) {
		if (idx >= max_chan) {
			printf("trpc buf too small, idx=%d\n", idx);
			break;
		}
		pTlvHdr = (MrvlIEtypes_Data_t *)pByte;
		pTlvHdr->header.len = le16_to_cpu(pTlvHdr->header.len);

		switch (le16_to_cpu(pTlvHdr->header.type)) {
		case TLV_TYPE_CHAN_TRPC_CONFIG:
			trpc_tlv = (MrvlIETypes_ChanTRPCConfig_t *) pTlvHdr;
			memset(pchan_trpc->mod_group, 0xff,
			       sizeof(pchan_trpc->mod_group));
			memcpy(&pchan_trpc->start_freq, &trpc_tlv->start_freq,
			       pTlvHdr->header.len);
			idx++;
			pchan_trpc++;
			break;
		default:
			break;
		}
		left_len -= (pTlvHdr->header.len + sizeof(pTlvHdr->header));
		pByte += pTlvHdr->header.len + sizeof(pTlvHdr->header);
	}
	*chan_idx = idx;
	return idx;
}

/**
 *  @brief get trpc data from file and fill in trpc table.
 *
 *  @param fp              file handle for trpc file
 *  @param trpc_buf        trpc table buffer
 *  @param max_chan        max channel supported in trpc table
 *  @param get_flag        only allow action get in the raw data
 *  @return                num_of channel filled in trpc table
 */
int
get_trpc_data(FILE * fp, t_u8 *trpc_buf, t_u8 max_chan, t_u8 get_flag)
{
	char line[256], *pos;
	int ln = 0;
	t_u8 *buffer = NULL;
	t_u16 size = BUFFER_LENGTH;
	int chan_idx = 0;
	t_u16 cmd_act = 0;
	t_u16 cmd_no = 0;

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		goto done;
	}
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strstr(pos, "={")) {
			get_hostcmd_raw_data(fp, &ln, buffer, &size);
			parse_trpc_raw_data(buffer, size, trpc_buf, max_chan,
					    &chan_idx, &cmd_no, &cmd_act);
			if (cmd_no != HostCmd_CHANNEL_TRPC_CONFIG) {
				chan_idx = 0;
				printf("Invalid cmd_no 0x%x cmd act=%d in file\n", cmd_no, (int)cmd_act);
				break;
			}
		}
	}
done:
	if (buffer)
		free(buffer);
	return chan_idx;
}

/**
 *  @brief find the same chanTrpc on another table
 *
 *  @param trpc_buf        trpc table buffer
 *  @param nun_chan        num of chan in trpc table buffer
 *  @param chan            channel number
 *  @return                NULL or find chan_trpc entry in the table
 */
ChanTRPCConfig_t *
get_trpc_entry(t_u8 *trpc_buf, t_u8 num_chan, t_u8 chan)
{
	int i = 0;
	int find = 0;
	ChanTRPCConfig_t *pentry;
	pentry = (ChanTRPCConfig_t *) trpc_buf;
	for (i = 0; i < num_chan; i++) {
		if (pentry->chan_num == chan) {
			find = 1;
			break;
		}
		pentry++;
	}
	if (find)
		return pentry;
	else
		return NULL;
}

/**
 *  @brief get max mod_group in trpbc table
 *
 *  @param trpc_buf        trpc table buffer
 *  @param nun_chan        num of chan in trpc table buffer
 *  @return                max mod_group number
 */
t_u8
get_max_mod_group(t_u8 *trpc_buf, t_u8 num_chan)
{
	t_u8 max_mod_group = 0;
	int i, j;
	ChanTRPCConfig_t *pentry;
	pentry = (ChanTRPCConfig_t *) trpc_buf;
	for (i = 0; i < num_chan; i++) {
		for (j = 0; j < MAX_MOD_GROUP; j++) {
			if (pentry->mod_group[j].mod_group == 0xff)
				break;
			if (pentry->mod_group[j].mod_group > max_mod_group)
				max_mod_group = pentry->mod_group[j].mod_group;
		}
		pentry++;
	}
	return max_mod_group;
}

/**
 *  @brief get mod_group setting for specifc mod_group
 *
 *  @param pchan_trpc      a pointer to channel trpc setting
 *  @param pmod_group      a point to mod_group setting.
 *  @return                NULL or the pointer to specfic mod_group setting.
 */
mod_group_setting *
get_chan_mod_group(ChanTRPCConfig_t * pchan_trpc,
		   mod_group_setting * pmod_group)
{
	int i;
	for (i = 0; i < MAX_MOD_GROUP; i++) {
		if (pchan_trpc->mod_group[i].mod_group == 0xff)
			break;
		if (pmod_group->mod_group == pchan_trpc->mod_group[i].mod_group) {
			return &pchan_trpc->mod_group[i];
		}
	}
	return NULL;
}

/**
 *  @brief get the power for specifc mod_group
 *
 *  @param pchan_trpc      a pointer to channel trpc setting
 *  @param pmod_group      mod group value
 *  @return                the power of specific mod group.
 */
t_u8
get_mod_power(ChanTRPCConfig_t * pchan_trpc, t_u8 mod_group)
{
	int i;
	for (i = 0; i < MAX_MOD_GROUP; i++) {
		if (pchan_trpc->mod_group[i].mod_group == 0xff)
			break;
		if (pchan_trpc->mod_group[i].mod_group == mod_group) {
			return pchan_trpc->mod_group[i].power;
		}
	}
	return 0xff;
}

/**
 *  @brief compare to chan_trpc entry
 *
 *  @param psrc_entry                pointer to chan_trpc entry in src file
 *  @param ptarget_entry             pointer to the target chan_trpc entry to compare
 *  @return                		     0 - match or 1
 */
int
check_chan_trpc_error(ChanTRPCConfig_t * psrc_entry,
		      ChanTRPCConfig_t * ptarget_entry, t_u8 min_mod_group,
		      t_u8 max_mod_group)
{
	int diff = 0;
	int i = 0;
	mod_group_setting *pmod_group = NULL;
	if (psrc_entry->start_freq != ptarget_entry->start_freq)
		diff++;
	if (psrc_entry->width != ptarget_entry->width)
		diff++;
	for (i = min_mod_group; i <= max_mod_group; i++) {
		if (psrc_entry->mod_group[i].mod_group == 0xff)
			break;
		pmod_group =
			get_chan_mod_group(ptarget_entry,
					   &psrc_entry->mod_group[i]);
		if (!pmod_group)
			diff++;
		else if (pmod_group->power != psrc_entry->mod_group[i].power)
			diff++;
	}
	if (diff)
		return 1;
	else
		return 0;
}

/**
 *  @brief display and compare to chan_trpc entry
 *
 *  @param psrc_entry              pointer to chan_trpc entry in src file
 *  @param ptarget_entry           pointer to the target chan_trpc entry to compare
 *  @return                		   0 - match or 1
 */
int
compare_and_display_chan_trpc(ChanTRPCConfig_t * psrc_entry,
			      ChanTRPCConfig_t * ptarget_entry)
{
	int diff = 0;
	int i = 0;
	mod_group_setting *pmod_group = NULL;
	if (psrc_entry && ptarget_entry) {
		if (psrc_entry->start_freq != ptarget_entry->start_freq)
			diff++;
		if (psrc_entry->width != ptarget_entry->width)
			diff++;
		for (i = 0; i < MAX_MOD_GROUP; i++) {
			if (psrc_entry->mod_group[i].mod_group == 0xff)
				break;
			pmod_group =
				get_chan_mod_group(ptarget_entry,
						   &psrc_entry->mod_group[i]);
			if (pmod_group &&
			    (pmod_group->power ==
			     psrc_entry->mod_group[i].power)) {
				printf("ch:%03d startfreq:%04d width:%02d mod:%02d power:%02d %02d  PASS\n", (int)psrc_entry->chan_num, (int)psrc_entry->start_freq, (int)psrc_entry->width, (int)psrc_entry->mod_group[i].mod_group, (int)pmod_group->power, (int)psrc_entry->mod_group[i].power);
			} else if (pmod_group) {
				printf("ch:%03d startfreq:%04d width:%02d mod:%02d power:%02d %02d  FAIL\n", (int)psrc_entry->chan_num, (int)psrc_entry->start_freq, (int)psrc_entry->width, (int)psrc_entry->mod_group[i].mod_group, (int)pmod_group->power, (int)psrc_entry->mod_group[i].power);
				diff++;
			} else {
				printf("ch:%03d startfreq:%04d width:%02d mod:%02d power:%02d     NOT FOUND\n", (int)psrc_entry->chan_num, (int)psrc_entry->start_freq, (int)psrc_entry->width, (int)psrc_entry->mod_group[i].mod_group, (int)psrc_entry->mod_group[i].power);
				diff++;
			}
		}
	} else if (psrc_entry) {
		printf("-------------------------------------------------------------------------------------------\n");
		printf("Missing expected channel %d TRPC setting in target file\n", psrc_entry->chan_num);
		printf("ch:%03d startfreq:%04d width:%02d Pwr:",
		       (int)psrc_entry->chan_num, (int)psrc_entry->start_freq,
		       (int)psrc_entry->width);
		for (i = 0; i < MAX_MOD_GROUP; i++) {
			if (psrc_entry->mod_group[i].mod_group == 0xff)
				break;
			if (i == 0)
				printf("%d,%d",
				       psrc_entry->mod_group[i].mod_group,
				       psrc_entry->mod_group[i].power);
			else
				printf(",%d,%d",
				       psrc_entry->mod_group[i].mod_group,
				       psrc_entry->mod_group[i].power);
		}
		printf("\n");
		printf("-------------------------------------------------------------------------------------------\n");
		diff++;
	}
	if (diff)
		return 1;
	else
		return 0;
}

/**
 *  @brief get channel list in 2 trpc table
 *
 *  @param target_trpc_buf        trpc buf from target file
 *  @param target_chan_num        num of channe in target file
 *  @param src_trpc_buf           trpc buf from src file
 *  @param src_chan_num           num of channel in src file
 *  @param chan_flag              0- all channel 1--5g only
 *  @return                       num of total channel in src trpc buf and target trpc buf
*/
int
get_trpc_channel_list(t_u8 *target_trpc_buf, t_u8 target_chan_num,
		      t_u8 *src_trpc_buf, t_u8 src_chan_num, t_u8 *pchan,
		      t_u8 chan_size, t_u8 chan_flag)
{
	ChanTRPCConfig_t *ptarget_entry = NULL;
	ChanTRPCConfig_t *psrc_entry = NULL;
	int target_chan_left = target_chan_num;
	int src_chan_left = src_chan_num;
	int total_chan = 0;
	int i = 0;

	memset(pchan, 0, chan_size);
	ptarget_entry = (ChanTRPCConfig_t *) target_trpc_buf;
	psrc_entry = (ChanTRPCConfig_t *) src_trpc_buf;

	if (chan_flag) {
		for (i = 0; i < target_chan_num; i++) {
			if (ptarget_entry->chan_num > MAX_BG_CHANNEL)
				break;
			ptarget_entry++;
			target_chan_left--;
		}
		for (i = 0; i < src_chan_num; i++) {
			if (psrc_entry->chan_num > MAX_BG_CHANNEL)
				break;
			psrc_entry++;
			src_chan_left--;
		}
		if (src_chan_left <= 0)
			psrc_entry = NULL;
		if (target_chan_left <= 0)
			ptarget_entry = NULL;
	}
	while (1) {
		if (!target_chan_left && !src_chan_left)
			break;
		if (ptarget_entry && psrc_entry) {
			if (ptarget_entry->chan_num == psrc_entry->chan_num) {
				*pchan = ptarget_entry->chan_num;
				pchan++;
				total_chan++;
				ptarget_entry++;
				psrc_entry++;
				target_chan_left--;
				src_chan_left--;
			} else if (ptarget_entry->chan_num <
				   psrc_entry->chan_num) {
				*pchan = ptarget_entry->chan_num;
				pchan++;
				total_chan++;
				ptarget_entry++;
				target_chan_left--;
			} else {
				*pchan = psrc_entry->chan_num;
				pchan++;
				total_chan++;
				psrc_entry++;
				src_chan_left--;
			}
		} else if (ptarget_entry) {
			*pchan = ptarget_entry->chan_num;
			pchan++;
			total_chan++;
			ptarget_entry++;
			target_chan_left--;
		} else if (psrc_entry) {
			*pchan = psrc_entry->chan_num;
			pchan++;
			total_chan++;
			psrc_entry++;
			src_chan_left--;
		}
		if (src_chan_left <= 0)
			psrc_entry = NULL;
		if (target_chan_left <= 0)
			ptarget_entry = NULL;
	}
	return total_chan;
}

/**
 *  @brief compare two trpc table's data
 *
 *  @param target_trpc_buf        trpc buf from target file
 *  @param target_chan_num        num of channe in target file
 *  @param src_trpc_buf           trpc buf from src file
 *  @param src_chan_num           num of channel in src file
 *  @param dispaly mode           1: text mode, 2 table mode.
 *  @return                        N/A
 */
void
process_trpc_data(t_u8 *target_trpc_buf, t_u8 target_chan_num,
		  t_u8 *src_trpc_buf, t_u8 src_chan_num, t_u8 display_mode)
{
	int i, j;
	t_u32 diff = 0;
	t_u16 total_diff = 0;
	t_u8 power_target;
	t_u8 power_src;
	t_u8 max_mod_group_target = 0;
	t_u8 max_mod_group_src = 0;
	t_u8 total_chan = 0;
	t_u8 chan_list[WLAN_IOCTL_USER_SCAN_CHAN_MAX];
	ChanTRPCConfig_t *ptarget_entry = NULL;
	ChanTRPCConfig_t *psrc_entry = NULL;

	printf("ModulationGroup:\n");
	printf(" 0: CCK (1,2,5.5,11 Mbps)            1: OFDM (6,9,12,18 Mbps)\n");
	printf(" 2: OFDM (24,36 Mbps)                3: OFDM (48,54 Mbps)\n");
	printf(" 4: HT20 (MCS0,1,2)                  5: HT20 (MCS3,4)\n");
	printf(" 6: HT20 (MCS5,6,7)                  7: HT40 (MCS0,1,2)\n");
	printf(" 8: HT40 (MCS3,4)                    9: HT40 (MCS5,6,7)\n");
	printf("10: HT2_20 (MCS8,9,10)               11: HT2_20 (MCS11,12)\n");
	printf("12: HT2_20 (MCS13,14,15)             13: HT2_40 (MCS8,9,10)\n");
	printf("14: HT2_40 (MCS11,12)                15: HT2_40 (MCS13,14,15)\n");
	printf("16: VHT_QAM256 (MCS8)                17: VHT_40_QAM256 (MCS8,9)\n");
	printf("18: VHT_80_PSK (MCS0,1,2)            19: VHT_80_QAM16 (MCS3,4)\n");
	printf("20: VHT_80_QAM64 (MCS5,6,7)          21: VHT_80_QAM256 (MCS8,9)\n");
	printf("22: VHT2_20_QAM256 (MCS8)            23: VHT2_40_QAM256 (MCS8,9)\n");
	printf("24: VHT2_80_PSK (MCS0,1,2)           25: VHT2_80_QAM16 (MCS3,4)\n");
	printf("26: VHT2_80_QAM64 (MCS5,6,7)         27: VHT2_80_QAM256 (MCS8,9)\n");
	printf("28: HE_20_QAM256 (MCS8,9)            29: HE_20_QAM1024 (MCS10,11)\n");
	printf("30: HE_40_QAM1024 (MCS10,11)         31: HE_80_QAM1024 (MCS10,11)\n");
	printf("32: HE2_20_QAM256 (MCS8,9)           33: HE2_20_QAM1024 (MCS10,11)\n");
	printf("34: HE2_40_QAM1024 (MCS10,11)        35: HE2_80_QAM1024 (MCS10,11)\n");
	printf("\n");
	if (display_mode == 1) {
		psrc_entry = (ChanTRPCConfig_t *) src_trpc_buf;
		for (i = 0; i < src_chan_num; i++) {
			ptarget_entry =
				get_trpc_entry(target_trpc_buf, target_chan_num,
					       psrc_entry->chan_num);
			if (ptarget_entry) {
				diff += compare_and_display_chan_trpc
					(psrc_entry, ptarget_entry);
			} else {
				diff += compare_and_display_chan_trpc
					(psrc_entry, NULL);
			}
			psrc_entry++;
		}
		if (diff)
			printf("Number of different channel trpc setting:%d in target file\n", diff);
		else
			printf("PASS\n");
	} else {
		printf("Column=ModGroup, Row=Channel, Value=Target,Source\n");
		printf(" CH  00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15\n");
		ptarget_entry = (ChanTRPCConfig_t *) target_trpc_buf;
		total_chan =
			get_trpc_channel_list(target_trpc_buf, target_chan_num,
					      src_trpc_buf, src_chan_num,
					      chan_list,
					      WLAN_IOCTL_USER_SCAN_CHAN_MAX, 0);
		for (i = 0; i < total_chan; i++) {
			ptarget_entry =
				get_trpc_entry(target_trpc_buf, target_chan_num,
					       chan_list[i]);
			psrc_entry =
				get_trpc_entry(src_trpc_buf, src_chan_num,
					       chan_list[i]);
			diff = 0;
			if (psrc_entry && ptarget_entry)
				diff = check_chan_trpc_error(psrc_entry,
							     ptarget_entry, 0,
							     15);
			else if (psrc_entry)
				diff++;
			if (diff)
				printf("*%03d ", (int)chan_list[i]);
			else
				printf(" %03d ", (int)chan_list[i]);
			for (j = 0; j <= 15; j++) {
				if (ptarget_entry)
					power_target =
						get_mod_power(ptarget_entry, j);
				else
					power_target = 0xff;
				if (psrc_entry)
					power_src =
						get_mod_power(psrc_entry, j);
				else
					power_src = 0xff;
				if (power_target != 0xff)
					printf("%02d", power_target);
				else
					printf("--");
				if (power_src != 0xff)
					printf(",%02d ", power_src);
				else
					printf(",-- ");
			}
			if (diff)
				total_diff++;
			printf("\n");
		}
		max_mod_group_target =
			get_max_mod_group(target_trpc_buf, target_chan_num);
		max_mod_group_src =
			get_max_mod_group(src_trpc_buf, src_chan_num);
		if (max_mod_group_target > 15 || max_mod_group_src > 15) {
			printf(" CH  16    17    18    19    20    21    22    23    24    25    26    27\n");
			ptarget_entry = (ChanTRPCConfig_t *) target_trpc_buf;
			total_chan =
				get_trpc_channel_list(target_trpc_buf,
						      target_chan_num,
						      src_trpc_buf,
						      src_chan_num, chan_list,
						      WLAN_IOCTL_USER_SCAN_CHAN_MAX,
						      1);
			for (i = 0; i < total_chan; i++) {
				diff = 0;
				ptarget_entry =
					get_trpc_entry(target_trpc_buf,
						       target_chan_num,
						       chan_list[i]);
				psrc_entry =
					get_trpc_entry(src_trpc_buf,
						       src_chan_num,
						       chan_list[i]);
				if (psrc_entry && ptarget_entry)
					diff = check_chan_trpc_error(psrc_entry,
								     ptarget_entry,
								     16, 27);
				else if (psrc_entry)
					diff++;
				if (diff)
					printf("*%03d ", (int)chan_list[i]);
				else
					printf(" %03d ", (int)chan_list[i]);

				for (j = 16; j <= 27; j++) {
					if (ptarget_entry)
						power_target =
							get_mod_power
							(ptarget_entry, j);
					else
						power_target = 0xff;
					if (psrc_entry)
						power_src =
							get_mod_power
							(psrc_entry, j);
					else
						power_src = 0xff;
					if (power_target != 0xff)
						printf("%02d", power_target);
					else
						printf("--");
					if (power_src != 0xff)
						printf(",%02d ", power_src);
					else
						printf(",-- ");
				}
				if (diff)
					total_diff++;
				printf("\n");
			}
		}
		max_mod_group_target =
			get_max_mod_group(target_trpc_buf, target_chan_num);
		max_mod_group_src =
			get_max_mod_group(src_trpc_buf, src_chan_num);
		if (max_mod_group_target > 27 || max_mod_group_src > 27) {
			printf(" CH  28    29    30    31    32    33    34    35\n");
			ptarget_entry = (ChanTRPCConfig_t *) target_trpc_buf;
			total_chan =
				get_trpc_channel_list(target_trpc_buf,
						      target_chan_num,
						      src_trpc_buf,
						      src_chan_num, chan_list,
						      WLAN_IOCTL_USER_SCAN_CHAN_MAX,
						      1);
			for (i = 0; i < total_chan; i++) {
				diff = 0;
				ptarget_entry =
					get_trpc_entry(target_trpc_buf,
						       target_chan_num,
						       chan_list[i]);
				psrc_entry =
					get_trpc_entry(src_trpc_buf,
						       src_chan_num,
						       chan_list[i]);
				if (psrc_entry && ptarget_entry)
					diff = check_chan_trpc_error(psrc_entry,
								     ptarget_entry,
								     16, 27);
				else if (psrc_entry)
					diff++;
				if (diff)
					printf("*%03d ", (int)chan_list[i]);
				else
					printf(" %03d ", (int)chan_list[i]);

				for (j = 28; j <= 35; j++) {
					if (ptarget_entry)
						power_target =
							get_mod_power
							(ptarget_entry, j);
					else
						power_target = 0xff;
					if (psrc_entry)
						power_src =
							get_mod_power
							(psrc_entry, j);
					else
						power_src = 0xff;
					if (power_target != 0xff)
						printf("%02d", power_target);
					else
						printf("--");
					if (power_src != 0xff)
						printf(",%02d ", power_src);
					else
						printf(",-- ");
				}
				if (diff)
					total_diff++;
				printf("\n");
			}
		}
		if (total_diff)
			printf("Number of different channel trpc setting:%d in target file\n", total_diff);
		else
			printf("PASS\n");
	}

	return;
}

/**
 * @brief      Process compare the trpc from 2 bin file
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_comparetrpc(int argc, char *argv[])
{
	FILE *target_fp = NULL;
	FILE *src_fp = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *target_trpc_buf = NULL;
	t_u8 *src_trpc_buf = NULL;
	t_u8 target_chan_num = WLAN_IOCTL_USER_SCAN_CHAN_MAX;
	t_u8 src_chan_num = WLAN_IOCTL_USER_SCAN_CHAN_MAX;
	int display_mode = 1;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX comparetrpc target_file src_file 1/2\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 6) {
		display_mode = a2hex_or_atoi(argv[5]);
		if (display_mode != 1 && display_mode != 2) {
			printf("Error: invalid display mode\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}
	target_fp = fopen(argv[3], "r");
	if (target_fp == NULL) {
		perror("target file fopen failed");
		goto done;
	}
	target_trpc_buf =
		(t_u8 *)malloc(sizeof(ChanTRPCConfig_t) *
			       WLAN_IOCTL_USER_SCAN_CHAN_MAX);
	if (!target_trpc_buf) {
		printf("ERR:Cannot allocate buffer for target trpc buf!\n");
		goto done;
	}
	memset(target_trpc_buf, 0,
	       sizeof(ChanTRPCConfig_t) * WLAN_IOCTL_USER_SCAN_CHAN_MAX);
	target_chan_num =
		get_trpc_data(target_fp, target_trpc_buf,
			      WLAN_IOCTL_USER_SCAN_CHAN_MAX, 1);
	if (!target_chan_num) {
		printf("Invalid target file %s: it should be the raw data file save from get_txpwrlimit command\n", argv[3]);
		goto done;
	}
	src_fp = fopen(argv[4], "r");
	if (src_fp == NULL) {
		perror("src file fopen failed");
		goto done;
	}
	src_trpc_buf =
		(t_u8 *)malloc(sizeof(ChanTRPCConfig_t) *
			       WLAN_IOCTL_USER_SCAN_CHAN_MAX);
	if (!src_trpc_buf) {
		printf("ERR:Cannot allocate buffer for src trpc buf!\n");
		goto done;
	}
	memset(src_trpc_buf, 0,
	       sizeof(ChanTRPCConfig_t) * WLAN_IOCTL_USER_SCAN_CHAN_MAX);
	src_chan_num =
		get_trpc_data(src_fp, src_trpc_buf,
			      WLAN_IOCTL_USER_SCAN_CHAN_MAX, 0);
	if (!src_chan_num) {
		printf("Invalid source file %s: it should be raw data file download to FW during driver init\n", argv[4]);
		goto done;
	}
	printf("Comparing %s to %s\n", argv[3], argv[4]);
	process_trpc_data(target_trpc_buf, target_chan_num, src_trpc_buf,
			  src_chan_num, display_mode);
done:
	if (target_fp)
		fclose(target_fp);
	if (target_trpc_buf)
		free(target_trpc_buf);
	if (src_fp)
		fclose(src_fp);
	if (src_trpc_buf)
		free(src_trpc_buf);
	return ret;
}

/**
 *  @brief get source region power data and fill in region power table.
 *
 *  @param fp              file handle for region power file
 *  @param file_size       file size;
 *  @param src_rgpwr_buf   power table buffer

 *  @return 0 -- success else failure
 */
int
get_src_rgpwr_data(FILE * src_fp, int file_size,
		   region_chan_pwr_tbl * rgchnpwr_tbl)
{
	t_u8 *tmp_buf = NULL;
	int ret = 0;
	t_u8 *pdata = NULL;
	int left_size = file_size;
	mfg_rghdr_t *rghdr = NULL;
	mfg_rgdatahdr_t *rgdatahdr = NULL;
	int max_chan_bg = 14;
	int max_chan_a = 0;
	int max_power_bg = 0;
	int max_power_a = 0;
	int i = 0;

	tmp_buf = (t_u8 *)malloc(file_size);
	if (!tmp_buf) {
		printf("fail to alloc temp buf, size=%d\n", file_size);
		return -EFAULT;
	}
	if (fread(tmp_buf, sizeof(t_u8), file_size, src_fp) == 0) {
		printf("ERR:read source image failed!\n");
		ret = -EFAULT;
		goto done;
	}
	//hexdump("Uncompressed data", tmp_buf, file_size, ' ');
	pdata = tmp_buf;
	rghdr = (mfg_rghdr_t *) pdata;
	if (rghdr->identifier != OTP_IDENTIFIER) {
		ret = -EFAULT;
		printf("ERR: Invalid identifier in uncompressed file 0x%x\n",
		       rghdr->identifier);
		goto done;
	}
	rgchnpwr_tbl->ptbaseversion = rghdr->ptbaseversion;
	left_size -= sizeof(mfg_rghdr_t);
	pdata += sizeof(mfg_rghdr_t);
	rgdatahdr = (mfg_rgdatahdr_t *) pdata;
	memcpy(&rgchnpwr_tbl->countrycode, &rgdatahdr->countrycode,
	       sizeof(rgchnpwr_tbl->countrycode));
	rgchnpwr_tbl->environment = rgdatahdr->environment;
	rgchnpwr_tbl->regioncode = rgdatahdr->regioncode;
	left_size -= sizeof(mfg_rgdatahdr_t);
	pdata += sizeof(mfg_rgdatahdr_t);
	switch (rghdr->ptbaseversion) {
	case PTVER_2X2_AC:
		max_chan_a = 39;
		max_power_a = 28;
		max_power_bg = 16;
		break;
	case PTVER_1X1_AC:
		max_chan_a = 39;
		max_power_a = 16;
		max_power_bg = 10;
		break;
	case PTVER_2X2_N:
		max_chan_a = 39;
		max_power_a = 16;
		max_power_bg = 16;
		break;
	case PTVER_1X1_N:
		max_chan_a = 39;
		max_power_a = 10;
		max_power_bg = 10;
		break;
	case PTVER_1X1_AC_11P:
		max_chan_a = 40;
		max_power_a = 16;
		max_power_bg = 10;
		break;
	case PTVER_2X2_AC_2GVHT:
		max_chan_a = 39;
		max_power_a = 28;
		max_power_bg = 24;
		break;
	case PTVER_1X1_AC_2GVHT:
		max_chan_a = 39;
		max_power_a = 16;
		max_power_bg = 12;
		break;
	case PTVER_1X1_AC_2GVHT_11P:
		max_chan_a = 40;
		max_power_a = 16;
		max_power_bg = 12;
		break;
	}
	rgchnpwr_tbl->max_chan_bg = max_chan_bg;
	rgchnpwr_tbl->max_chan_a = max_chan_a;
	rgchnpwr_tbl->max_power_a = max_power_a;
	rgchnpwr_tbl->max_power_bg = max_power_bg;
	if (left_size < ((max_power_bg + 1) * max_chan_bg)) {
		printf("ERR: left size not enough for 2g\n");
		goto done;
	}
	for (i = 0; i < max_chan_bg; i++) {
		rgchnpwr_tbl->bg_pwr_tbl[i].chan_no = *pdata;
		pdata++;
		memset(rgchnpwr_tbl->bg_pwr_tbl[i].pwr, 0xff,
		       MAX_MCS_POWER_INDEX);
		memcpy(rgchnpwr_tbl->bg_pwr_tbl[i].pwr, pdata, max_power_bg);
		pdata += max_power_bg;
	}
	left_size -= (max_power_bg + 1) * max_chan_bg;
	if (left_size < ((max_power_a + 1) * max_chan_a)) {
		printf("ERR: left size not enough for 5g\n");
		goto done;
	}
	for (i = 0; i < max_chan_a; i++) {
		rgchnpwr_tbl->a_pwr_tbl[i].chan_no = *pdata;
		pdata++;
		memset(rgchnpwr_tbl->a_pwr_tbl[i].pwr, 0xff,
		       MAX_MCS_POWER_INDEX);
		memcpy(rgchnpwr_tbl->a_pwr_tbl[i].pwr, pdata, max_power_a);
		pdata += max_power_a;
	}
	left_size -= (max_power_a + 1) * max_chan_a;
	if (left_size < max_chan_bg + max_chan_a) {
		printf("ERR: left size not enough for chan attr\n");
		goto done;
	}
	for (i = 0; i < max_chan_bg; i++) {
		rgchnpwr_tbl->bg_pwr_tbl[i].chan_attr = *pdata;
		pdata++;
	}
	for (i = 0; i < max_chan_a; i++) {
		rgchnpwr_tbl->a_pwr_tbl[i].chan_attr = *pdata;
		pdata++;
	}
	left_size -= max_chan_bg + max_chan_a;
	if (left_size)
		printf("ERR: left_size=%d\n", left_size);
done:
	if (tmp_buf)
		free(tmp_buf);
	return ret;
}

/**
 *  @brief get FW region power table
 *
 *  @param fp              file handle for region power file
 *  @param rgchnpwr_tbl    a pointer to regchnpwr tbl
 *
 *  @return 0 -- success else failure
 */
int
get_fw_rgpwr_data(FILE * fp, region_chan_pwr_tbl * rgchnpwr_tbl)
{
	int ln = 0;
	int i;
	t_u8 *buffer = NULL;
	t_u8 *tlv_buf = NULL;
	t_u16 size = BUFFER_LENGTH;
	int ret = 0;
	HostCmd_DS_GEN *hostcmd = NULL;
	t_u16 tlv_buf_left = 0;
	t_u16 tlv_type, tlv_len;
	MrvlIEtypesHeader_t *tlv;
	MrvlIEtypes_otp_region_info_t *rginfo_tlv;
	MrvlIEtypes_power_table_attr_t *pwr_tbl_attr_tlv;
	MrvlIEtypes_chan_attr_t *chan_attr_tlv;
	MrvlIEtypes_power_table_t *power_tlv;
	t_u8 *pdata = NULL;
	chan_power_t *power = NULL;

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		goto done;
	}
	get_hostcmd_raw_data(fp, &ln, buffer, &size);
	//hexdump("cmdresp", buffer,size, ' ');
	hostcmd = (HostCmd_DS_GEN *)buffer;
	hostcmd->command = le16_to_cpu(hostcmd->command);
	hostcmd->size = le16_to_cpu(hostcmd->size);
	if (hostcmd->command != HostCmd_CMD_CHAN_REGION_CFG) {
		printf("ERR: target file don't have the hostcmd id 0x242\n");
		ret = -EFAULT;
		goto done;
	}
	tlv_buf_left = hostcmd->size - S_DS_GEN - sizeof(t_u16);
	tlv_buf = buffer + S_DS_GEN + sizeof(t_u16);
	while (tlv_buf_left >= sizeof(MrvlIEtypesHeader_t)) {
		tlv = (MrvlIEtypesHeader_t *)tlv_buf;
		tlv_type = le16_to_cpu(tlv->type);
		tlv_len = le16_to_cpu(tlv->len);
		switch (tlv_type) {
		case TLV_TYPE_POWER_TABLE_ATTR:
			pwr_tbl_attr_tlv =
				(MrvlIEtypes_power_table_attr_t *) tlv_buf;
			rgchnpwr_tbl->max_chan_bg = pwr_tbl_attr_tlv->rows_bg;
			rgchnpwr_tbl->max_power_bg =
				pwr_tbl_attr_tlv->cols_bg - 1;
			rgchnpwr_tbl->max_chan_a = pwr_tbl_attr_tlv->rows_a;
			rgchnpwr_tbl->max_power_a =
				pwr_tbl_attr_tlv->cols_a - 1;
			break;
		case TLV_TYPE_REGION_INFO:
			rginfo_tlv = (MrvlIEtypes_otp_region_info_t *) tlv_buf;
			memcpy(&rgchnpwr_tbl->countrycode,
			       &rginfo_tlv->countrycode,
			       sizeof(rgchnpwr_tbl->countrycode));
			rgchnpwr_tbl->regioncode = rginfo_tlv->regioncode;
			rgchnpwr_tbl->environment = rginfo_tlv->environment;
			break;
		case TLV_TYPE_CHAN_ATTR_CFG:
			chan_attr_tlv = (MrvlIEtypes_chan_attr_t *) tlv_buf;
			if (tlv_len <
			    ((rgchnpwr_tbl->max_chan_a +
			      rgchnpwr_tbl->max_chan_bg) *
			     sizeof(chan_attr_t))) {
				printf("ERR: Invalid chan attr tlv\n");
				hexdump("chan attrib tlv", tlv_buf,
					tlv_len + sizeof(MrvlIEtypesHeader_t),
					' ');
				ret = -EFAULT;
				goto done;
			}
			for (i = 0; i < rgchnpwr_tbl->max_chan_bg; i++) {
				if (!chan_attr_tlv->chan_attr[i].chan_no) {
					printf("ERR:Invalid chan attr tlv\n");
					hexdump("chan attrib tlv", tlv_buf,
						tlv_len +
						sizeof(MrvlIEtypesHeader_t),
						' ');
					ret = -EFAULT;
					goto done;
				}
				rgchnpwr_tbl->bg_pwr_tbl[i].chan_attr =
					chan_attr_tlv->chan_attr[i].chan_attr;
				rgchnpwr_tbl->bg_pwr_tbl[i].chan_no =
					chan_attr_tlv->chan_attr[i].chan_no;
			}
			for (i = 0; i < rgchnpwr_tbl->max_chan_a; i++) {
				rgchnpwr_tbl->a_pwr_tbl[i].chan_attr =
					chan_attr_tlv->chan_attr[i +
								 rgchnpwr_tbl->
								 max_chan_bg].
					chan_attr;
				rgchnpwr_tbl->a_pwr_tbl[i].chan_no =
					chan_attr_tlv->chan_attr[i +
								 rgchnpwr_tbl->
								 max_chan_bg].
					chan_no;
			}
			break;
		case TLV_TYPE_POWER_TABLE:
			power_tlv = (MrvlIEtypes_power_table_t *) tlv_buf;
			if (tlv_len <
			    (rgchnpwr_tbl->max_chan_bg *
			     (rgchnpwr_tbl->max_power_bg + 1)
			     +
			     rgchnpwr_tbl->max_chan_a *
			     (rgchnpwr_tbl->max_power_a + 1))) {
				printf("ERR: Invalid power table tlv\n");
				hexdump("power table tlv", tlv_buf,
					tlv_len + sizeof(MrvlIEtypesHeader_t),
					' ');
				ret = -EFAULT;
				goto done;
			}
			pdata = (t_u8 *)&power_tlv->chan_power;
			for (i = 0; i < rgchnpwr_tbl->max_chan_bg; i++) {
				power = (chan_power_t *) pdata;
				if (!power->chan_no ||
				    (rgchnpwr_tbl->bg_pwr_tbl[i].chan_no !=
				     power->chan_no)) {
					printf("ERR: Invalid power table tlv, channel miss match\n");
					hexdump("power table tlv", tlv_buf,
						tlv_len +
						sizeof(MrvlIEtypesHeader_t),
						' ');
					ret = -EFAULT;
					goto done;
				}
				memset(rgchnpwr_tbl->bg_pwr_tbl[i].pwr, 0xff,
				       sizeof(rgchnpwr_tbl->bg_pwr_tbl[i].pwr));
				memcpy(rgchnpwr_tbl->bg_pwr_tbl[i].pwr,
				       power->power,
				       rgchnpwr_tbl->max_power_bg);
				pdata += rgchnpwr_tbl->max_power_bg + 1;
			}
			for (i = 0; i < rgchnpwr_tbl->max_chan_a; i++) {
				power = (chan_power_t *) pdata;
				if (rgchnpwr_tbl->a_pwr_tbl[i].chan_no !=
				    power->chan_no) {
					printf("ERR: Invalid power table tlv, channel miss match\n");
					hexdump("power table tlv", tlv_buf,
						tlv_len +
						sizeof(MrvlIEtypesHeader_t),
						' ');
					ret = -EFAULT;
					goto done;
				}
				memset(rgchnpwr_tbl->a_pwr_tbl[i].pwr, 0xff,
				       sizeof(rgchnpwr_tbl->a_pwr_tbl[i].pwr));
				memcpy(rgchnpwr_tbl->a_pwr_tbl[i].pwr,
				       power->power, rgchnpwr_tbl->max_power_a);
				pdata += rgchnpwr_tbl->max_power_a + 1;
			}
			break;
		}
		tlv_buf += sizeof(MrvlIEtypesHeader_t) + tlv_len;
		tlv_buf_left -= sizeof(MrvlIEtypesHeader_t) + tlv_len;
	}
done:
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief display region_chan_pwr_table header
 *
 *  @param tbl		           pointer to table
 *  @return                        void
 */
void
display_tbl_hdr(region_chan_pwr_tbl * tbl)
{
	printf("====================2G_Power_Index_Map=========================================\n");
	printf("Pwr index    mapping		Pwr index    mapping     \n");
	printf("===============================================================================\n");
	switch (tbl->ptbaseversion) {
	case PTVER_1X1_AC:
	case PTVER_1X1_N:
	case PTVER_1X1_AC_11P:
		printf(" 1     11b (11M - 1M)               2       11g (18M - 6M)\n");
		printf(" 3    11g (36M - 24M)               4       11g (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 40 (MCS2 - MCS0, 1x1)\n");
		printf(" 9   11n 40 (MCS4 - MCS3, 1x1)     10     11n 40 (MCS7 - MCS5, 1x1)\n");
		break;
	case PTVER_2X2_N:
	case PTVER_2X2_AC:
		printf(" 1     11b (11M - 1M)               2       11g (18M - 6M)\n");
		printf(" 3    11g (36M - 24M)               4       11g (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 20 (MCS10 - MCS8, 2x2)\n");
		printf(" 9   11n 20 (MCS12 - MCS11, 2x2)   10     11n 20 (MCS15 - MCS13, 2x2)\n");
		printf("11   11n 40 (MCS2 - MCS0, 1x1)     12     11n 40 (MCS4 - MCS3, 1x1)\n");
		printf("13   11n 40 (MCS7 - MCS5, 1x1)     14     11n 40 (MCS10 - MCS8, 2x2)\n");
		printf("15   11n 40 (MCS12 - MCS11, 2x2)   16     11n 40 (MCS15 - MCS13, 2x2)\n");
		break;
	case PTVER_1X1_AC_2GVHT:
	case PTVER_1X1_AC_2GVHT_11P:
		printf(" 1     11b (11M - 1M)               2       11g (18M - 6M)\n");
		printf(" 3    11g (36M - 24M)               4       11g (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 20 (MCS10 - MCS8, 2x2)\n");
		printf(" 9   11n 20 (MCS12 - MCS11, 2x2)   10     11n 40 (MCS2 - MCS0, 1x1)\n");
		printf(" 11  11n 40 (MCS4 - MCS3, 1x1)     12     11n 40 (MCS7 - MCS5, 1x1)\n");
		break;
	case PTVER_2X2_AC_2GVHT:
		printf(" 1     11b (11M - 1M)               2       11g (18M - 6M)\n");
		printf(" 3    11g (36M - 24M)               4       11g (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 20 (MCS10 - MCS8, 2x2)\n");
		printf(" 9   11n 20 (MCS12 - MCS11, 2x2)   10     11n 20 (MCS15 - MCS13, 2x2)\n");
		printf("11   11ac 20 (mcs8, 1x1)   		   12     11ac 20 (mcs9-mcs8, 2x2)\n");
		printf("13   11n 40 (MCS2 - MCS0, 1x1)     14     11n 40 (MCS4 - MCS3, 1x1)\n");
		printf("15   11n 40 (MCS7 - MCS5, 1x1)     16     11n 40 (MCS10 - MCS8, 2x2)\n");
		printf("17   11n 40 (MCS12 - MCS11, 2x2)   18     11ac 40 (mcs9-mcs8, 1x1)\n");
		printf("19   11ac 40 (mcs9-mcs8, 2x2)      20     11n 40 (MCS15 - MCS13, 2x2)\n");
		printf("21   11ac 80 (mcs2-mcs0, 1x1)      22     11ac 80 (mcs4 - mcs3, 1x1)\n");
		printf("23   11ac 80 (mcs7 - mcs5, 1x1)    24     11ac 80 (mcs9-8, 1x1)\n");
		break;
	}
	printf("====================5G_Power_Index_Map=========================================\n");
	printf("Pwr index    mapping		Pwr index    mapping     \n");
	printf("===============================================================================\n");
	switch (tbl->ptbaseversion) {
	case PTVER_1X1_AC:
	case PTVER_1X1_AC_11P:
	case PTVER_1X1_AC_2GVHT:
	case PTVER_1X1_AC_2GVHT_11P:
		printf(" 1     11b (11M - 1M)               2       11a (18M - 6M)\n");
		printf(" 3    11a (36M - 24M)               4       11a (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11ac 20 (mcs8, 1x1)\n");
		printf(" 9   11n 40 (mcs2 - mcs0, 1x1)     10     11n 40 (mcs4 - mcs3, 1x1)\n");
		printf("11   11n 40 (mcs7 - mcs5, 1x1)     12     11ac 40 (mcs9-mcs8, 1x1)\n");
		printf("13   11ac 80 (mcs2-mcs0, 1x1)      14     11ac 80 (mcs4 - mcs3, 1x1)\n");
		printf("15   11ac 80 (mcs7 - mcs5, 1x1)    16     11ac 80 (mcs9-8, 1x1)\n");
		break;
	case PTVER_1X1_N:
		printf(" 1     11b (11M - 1M)               2       11a (18M - 6M)\n");
		printf(" 3    11a (36M - 24M)               4       11a (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 40 (mcs2 - mcs0, 1x1)\n");
		printf(" 9   11n 40 (mcs4 - mcs3, 1x1)     10     11n 40 (mcs7 - mcs5, 1x1)\n");
		break;
	case PTVER_2X2_AC_2GVHT:
	case PTVER_2X2_AC:
		printf(" 1     11b (11M - 1M)               2       11a (18M - 6M)\n");
		printf(" 3    11a (36M - 24M)               4       11a (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 20 (mcs10 - mcs8, 2x2)\n");
		printf(" 9   11n 20 (mcs12 - mcs11, 2x2)   10     11n 20 (mcs15 - mcs13, 2x2)\n");
		printf("11   11ac 20 (mcs8, 1x1)     	   12     11ac 20 (mcs9-mcs8, 2x2)\n");
		printf("13   11n 40 (mcs2 - mcs0, 1x1)     14     11n 40 (mcs4 - mcs3, 1x1)\n");
		printf("15   11n 40 (mcs7 - mcs5, 1x1)     16     11n 40 (mcs10 - mcs8, 2x2)\n");
		printf("17   11n 40 (mcs12 - mcs11, 2x2)   18     11ac 40 (mcs9-mcs8, 1x1) \n");
		printf("19   11ac 40 (mcs9-mcs8, 2x2)      20     11n 40 (mcs15 - mcs13, 2x2)\n");
		printf("21   11ac 80 (mcs2-mcs0, 1x1)      22     11ac 80 (mcs4 - mcs3, 1x1)\n");
		printf("23   11ac 80 (mcs7 - mcs5, 1x1)    24     11ac 80 (mcs9-8, 1x1)\n");
		printf("25   11ac 80 (mcs2-mcs0, 2x2)      26     11ac 80 (mcs4 - mcs3, 2x2)\n");
		printf("27   11ac 80 (mcs7 - mcs5, 2x2)    28     11ac 80 (mcs9-8, 2x2)\n");
		break;
	case PTVER_2X2_N:
		printf(" 1     11b (11M - 1M)               2       11a (18M - 6M)\n");
		printf(" 3    11a (36M - 24M)               4       11a (54M - 48M)\n");
		printf(" 5   11n 20 (MCS2 - MCS0, 1x1)      6     11n 20 (MCS4 - MCS3, 1x1)\n");
		printf(" 7   11n 20 (MCS7 - MCS5, 1x1)      8     11n 20 (mcs10 - mcs8, 2x2)\n");
		printf(" 9   11n 20 (mcs12 - mcs11, 2x2)   10     11n 20 (mcs15 - mcs13, 2x2)\n");
		printf("11   11n 40 (mcs2 - mcs0, 1x1)     12     11n 40 (mcs4 - mcs3, 1x1)\n");
		printf("13   11n 40 (mcs7 - mcs5, 1x1)     14     11n 40 (mcs10 - mcs8, 2x2)\n");
		printf("15   11n 40 (mcs12 - mcs11, 2x2)   16     11n 40 (mcs15 - mcs13, 2x2) \n");
		break;
	}
	printf("\n\n");
	return;
}

/**
 *  @brief display and compare the channel power table
 *
 *  @param tbl		           pointer to table
 *  @param target_tbl 		   pointer to target table
 *  @return                        void
 */
void
display_regpwr_tbl(region_chan_pwr_tbl * tbl, region_chan_pwr_tbl * comp_tbl)
{
	int i, j;
	if (!comp_tbl)
		display_tbl_hdr(tbl);
	if (comp_tbl && (tbl->environment != comp_tbl->environment ||
			 tbl->regioncode != comp_tbl->regioncode ||
			 tbl->countrycode[0] != comp_tbl->countrycode[0] ||
			 tbl->countrycode[1] != comp_tbl->countrycode[1]))
		printf("* country code is ( %c%c )  region code is ( %d ) environment code is ( %d )\n", tbl->countrycode[0], tbl->countrycode[1], tbl->regioncode, tbl->environment);
	else
		printf(" country code is ( %c%c )  region code is ( %d ) environment code is ( %d )\n", tbl->countrycode[0], tbl->countrycode[1], tbl->regioncode, tbl->environment);
	switch (tbl->max_power_bg) {
	case 10:
		printf("=========================2G_POWER_TABLE=========================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10 \n");
		printf("================================================================================\n");
		break;
	case 12:
		printf("=========================2G_POWER_TABLE===================================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10   11   12 \n");
		printf("==========================================================================================\n");
		break;
	case 16:
		printf("=========================2G_POWER_TABLE=======================================================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16 \n");
		printf("==============================================================================================================\n");
		break;
	case 24:
		printf("=========================5G_POWER_TABLE=========================================================================================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24 \n");
		printf("================================================================================================================================================\n");
		break;
	}
	for (i = 0; i < 14; i++) {
		if (comp_tbl &&
		    memcmp(&tbl->bg_pwr_tbl[i], &comp_tbl->bg_pwr_tbl[i],
			   sizeof(rgchan_pwr_t)))
			printf(" *%3d (%02x)                ",
			       tbl->bg_pwr_tbl[i].chan_no,
			       tbl->bg_pwr_tbl[i].chan_attr);
		else
			printf("  %3d (%02x)                ",
			       tbl->bg_pwr_tbl[i].chan_no,
			       tbl->bg_pwr_tbl[i].chan_attr);
		for (j = 0; j < tbl->max_power_bg; j++) {
			printf("%2d   ", tbl->bg_pwr_tbl[i].pwr[j]);
		}
		printf("\n");
	}
	printf("						 \n");
	switch (tbl->max_power_a) {
	case 10:
		printf("=========================5G_POWER_TABLE=========================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10 \n");
		printf("================================================================================\n");
		break;
	case 16:
		printf("=========================5G_POWER_TABLE=================================================================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16 \n");
		printf("========================================================================================================================\n");
		break;
	case 28:
		printf("=========================5G_POWER_TABLE=============================================================================================================================\n");
		printf("chan_no(Attr), Pindx(dBm) 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28 \n");
		printf("====================================================================================================================================================================\n");
		break;
	}
	for (i = 0; i < tbl->max_chan_a; i++) {
		if (comp_tbl &&
		    memcmp(&tbl->a_pwr_tbl[i], &comp_tbl->a_pwr_tbl[i],
			   sizeof(rgchan_pwr_t)))
			printf(" *%3d (%02x)                ",
			       tbl->a_pwr_tbl[i].chan_no,
			       tbl->a_pwr_tbl[i].chan_attr);
		else
			printf("  %3d (%02x)                ",
			       tbl->a_pwr_tbl[i].chan_no,
			       tbl->a_pwr_tbl[i].chan_attr);
		for (j = 0; j < tbl->max_power_a; j++) {
			printf("%2d   ", tbl->a_pwr_tbl[i].pwr[j]);
		}
		printf("\n");
	}
	return;
}

/**
 * @brief      Process compare the regionpowertable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_compare_rgpwr(int argc, char *argv[])
{

	FILE *src_fp = NULL;
	FILE *target_fp = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	struct stat st;
	t_u8 *src_rgpwr_buf = NULL;
	t_u8 *target_rgpwr_buf = NULL;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX comparergpwr uncompressed_file target_file\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 5) {
		target_fp = fopen(argv[4], "r");
		if (target_fp == NULL) {
			ret = -EFAULT;
			perror("target file fopen failed");
			goto done;
		}
		target_rgpwr_buf = (t_u8 *)malloc(sizeof(region_chan_pwr_tbl));
		if (!target_rgpwr_buf) {
			ret = -EFAULT;
			printf("ERR:Cannot allocate buffer target_rgpwr_buf!\n");
			goto done;
		}
		memset(target_rgpwr_buf, 0, sizeof(region_chan_pwr_tbl));
		if (get_fw_rgpwr_data
		    (target_fp, (region_chan_pwr_tbl *) target_rgpwr_buf)) {
			ret = -EFAULT;
			printf("ERR:Fail to parse target file\n");
			goto done;
		}
	}
	if (stat(argv[3], &st)) {
		printf("cannot stat %s\n", argv[3]);
		ret = -EFAULT;
		goto done;
	}

	src_fp = fopen(argv[3], "r");
	if (src_fp == NULL) {
		perror("src file fopen failed");
		goto done;
	}
	src_rgpwr_buf = (t_u8 *)malloc(sizeof(region_chan_pwr_tbl));
	if (!src_rgpwr_buf) {
		ret = -EFAULT;
		printf("ERR:Cannot allocate buffer for src_rgpwr_buf!\n");
		goto done;
	}
	memset(src_rgpwr_buf, 0, sizeof(region_chan_pwr_tbl));
	if (get_src_rgpwr_data
	    (src_fp, (int)st.st_size, (region_chan_pwr_tbl *) src_rgpwr_buf)) {
		printf("ERR: Fail to parse source file!\n");
		goto done;
	}
	if (src_rgpwr_buf && target_rgpwr_buf) {
		if (!memcmp
		    (src_rgpwr_buf, target_rgpwr_buf,
		     sizeof(region_chan_pwr_tbl) - 1)) {
			printf("PASS\n");
			display_regpwr_tbl((region_chan_pwr_tbl *)
					   src_rgpwr_buf, NULL);
		} else {
			printf("FAIL\n");
			printf("SRC Region Channel Power Table:\n");
			display_regpwr_tbl((region_chan_pwr_tbl *)
					   src_rgpwr_buf, NULL);
			printf("==============================================================================\n");
			printf("FW Region Channel Power Table:\n");
			display_regpwr_tbl((region_chan_pwr_tbl *)
					   target_rgpwr_buf,
					   (region_chan_pwr_tbl *)
					   src_rgpwr_buf);
		}
	} else if (src_rgpwr_buf) {
		printf("SRC Region Channel Power Table:\n");
		display_regpwr_tbl((region_chan_pwr_tbl *) src_rgpwr_buf, NULL);
	}
done:
	if (target_fp)
		fclose(target_fp);
	if (target_rgpwr_buf)
		free(target_rgpwr_buf);
	if (src_fp)
		fclose(src_fp);
	if (src_rgpwr_buf)
		free(src_rgpwr_buf);
	return ret;
}

/**
 * @brief      Enable/Disable amsdu_aggr_ctrl
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_11n_amsdu_aggr_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, data[2];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: amsduaggrctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(data, 0, sizeof(data));
	memcpy(data, buffer, sizeof(data));
	if (data[0] == 1)
		printf("Feature is enabled\n");
	if (data[0] == 0)
		printf("Feature is disabled\n");
	printf("Current AMSDU buffer size is %d\n", data[1]);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get Transmit beamforming capabilities
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_tx_bf_cap_ioctl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, bf_cap;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxbfcap fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memcpy(&bf_cap, buffer, sizeof(int));
	printf("Current TX beamforming capability is 0x%x\n", bf_cap);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#ifdef SDIO
/**
 * @brief      Turn on/off the sdio clock
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sdio_clock_ioctl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, clock_state = 0;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdioclock fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		memcpy(&clock_state, buffer, sizeof(clock_state));
		printf("Current SDIO clock state is %d\n", clock_state);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

#ifdef SDIO
/**
 * @brief      Set SDIO Multi-point aggregation control parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sdio_mpa_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, data[6];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc < 3) && (argc > 9)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: diaglooptest fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("Enable MP aggregation for Tx is %d\n", data[0]);
		printf("Enable MP aggregation for Rx is %d\n", data[1]);
		printf("Tx buffer size is %d\n", data[2]);
		printf("Rx buffer size is %d\n", data[3]);
		printf("maximum Tx port is %d\n", data[4]);
		printf("maximum Rx port is %d\n", data[5]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 * @brief      Configure sleep parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sleep_params(int argc, char *argv[])
{
	int ret = 0, data[6];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 9)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);
/*    prepare_buffer(buffer, argv[2], 0, NULL); */

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sleepparams fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(data, 0, sizeof(data));
	memcpy(data, buffer, sizeof(data));
	printf("Sleep clock error in ppm is %d\n", data[0]);
	printf("Wakeup offset in usec is %d\n", data[1]);
	printf("Clock stabilization time in usec is %d\n", data[2]);
	printf("Control periodic calibration is %d\n", data[3]);
	printf("Control the use of external sleep clock is %d\n", data[4]);
	printf("Debug is %d\n", data[5]);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get cw mode
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_cwmode(int argc, char *argv[])
{
	t_u8 *buffer = NULL, *pos = NULL, *action;
	char *data = NULL, *args[100];
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	cw_mode_ctrl *cwmode = NULL;
	FILE *config_file = NULL;
	char *line = NULL;
	int li = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	memset(buffer, 0, BUFFER_LENGTH);
	pos = buffer;
	strncpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));
	strcpy((char *)pos, argv[2]);
	pos += (strlen(argv[2]));
	action = pos;
	pos++;

	if (argc > 3) {
		*action = 1;
		config_file = fopen(argv[3], "r");
		if (config_file == NULL) {
			perror("CONFIG");
			exit(1);
		}

		line = (char *)malloc(MAX_CONFIG_LINE);
		if (!line) {
			printf("ERR:Cannot allocate memory for line\n");
			exit(1);
		}
		memset(line, 0, MAX_CONFIG_LINE);
		/* Parse file and process */
		while (config_get_line
		       (line, MAX_CONFIG_LINE, config_file, &li, &data)) {
			parse_line(line, args, 100);

			if (strcmp(args[0], "CW_MODE") == 0)
				cwmode = (cw_mode_ctrl *) pos;
			else {
				if (cwmode) {
					if (strcmp(args[0], "Mode") == 0)
						cwmode->mode =
							a2hex_or_atoi(args[1]);
					else if (strcmp(args[0], "Channel") ==
						 0)
						cwmode->channel =
							a2hex_or_atoi(args[1]);
					else if (strcmp(args[0], "Chaninfo") ==
						 0)
						cwmode->chanInfo =
							a2hex_or_atoi(args[1]);
					else if (strcmp(args[0], "TxPower") ==
						 0)
						cwmode->txPower =
							a2hex_or_atoi(args[1]);
					else if (strcmp(args[0], "PktLength") ==
						 0)
						cwmode->pktLength =
							a2hex_or_atoi(args[1]);
					else if (strcmp(args[0], "RateInfo") ==
						 0)
						cwmode->rateInfo =
							a2hex_or_atoi(args[1]);
				} else {
					printf("invalid config file\n");
					printf("Specify the parameters in CW_MODE header as shown below:\n");
					printf("CW_MODE = {\n");
					printf("//\n");
					printf("// specify parameters between CW_MODE header only\n");
					printf("//\n");
					printf("}\n");
					exit(1);
				}
			}
		}
	}
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cwmode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	cwmode = (cw_mode_ctrl *) buffer;
	printf("CW Mode:\nMode = %d\nChannel = %d\nChanInfo = 0x%x\n"
	       "TxPower=%d\nRateInfo=0x%x\nPacket Length=%d\n",
	       cwmode->mode, cwmode->channel, cwmode->chanInfo,
	       cwmode->txPower, cwmode->rateInfo, cwmode->pktLength);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Set/Get DFS Testing settings
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_dfs_testing(int argc, char *argv[])
{
	int ret = 0, data[4];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 7)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dfstesting fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("User-configured Channel Availability Check in msec is %d\n", data[0]);
		printf("User-configured Non-Occupancy Period in sec is %d\n",
		       data[1]);
		printf("No channel change on radar enabled is %d\n", data[2]);
		printf("User-configured channel to change to on radar is %d\n",
		       data[3]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get CFP table codes
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_cfp_code(int argc, char *argv[])
{
	int ret = 0, data[2];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4) && (argc != 5)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cfpcode fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("Code of the CFP table for 2.4GHz is %d\n", data[0]);
		printf("Code of the CFP table for 5GHz is %d\n", data[1]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get Tx/Rx antenna
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_set_get_tx_rx_ant(int argc, char *argv[])
{
	int ret = 0;
	int data[3] = { 0 };
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)
	    && (argc != 5)
		) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: antcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		if (cmd->used_len < sizeof(data)) {
			memcpy(data, buffer, cmd->used_len);
			printf("Mode of Tx path is 0x%x\n", data[0]);
			if (cmd->used_len == (sizeof(int) * 2))
				printf("Mode of Rx path is 0x%x\n", data[1]);
		} else {
			memcpy(data, buffer, sizeof(data));
			printf("Mode of Tx/Rx path is 0x%x\n", data[0]);
			/* Evaluate time is valid only when SAD is enabled */
			if (data[0] == 0xffff) {
				printf("Evaluate time = %d\n", data[1]);
				/* Current antenna value should be 1,2,3. 0 is invalid value */
				if (data[2] > 0)
					printf("Current antenna is %d\n",
					       data[2]);
			}
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Get/Set system clock
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sysclock(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	int data[65], i = 0;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sysclock fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("sysclock = ");
		for (i = 1; i <= data[0]; i++) {
			printf("%d ", data[i]);
		}
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get GTK/PTK
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_get_key(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc != 3) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getkey fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (cmd->used_len)
		printf("Key is %s\n", buffer);
	else
		printf("Key is not set\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Associate to a specific indexed entry in the ScanTable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_associate_ssid_bssid(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc == 3) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: associate fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Set/Get Transmit beamforming configuration
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_tx_bf_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	char *param = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 param_cnt = 0;
	int status = MLAN_STATUS_SUCCESS;
	int bf_action;
	bf_global_cfg *bf_cfg = NULL;

	if (argc == 3) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		status = MLAN_STATUS_FAILURE;
		goto exit;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxbfcfg fail\n");
		status = MLAN_STATUS_FAILURE;
		goto exit;
	}

	param = argv[3];
	param_cnt = strlen(param);
	if (param_cnt == 1) {
		/* The first byte represents the beamforming action */
		bf_action = atoi(argv[3]);
		switch (bf_action) {
		case BF_GLOBAL_CONFIGURATION:
			bf_cfg = (bf_global_cfg *) buffer;
			printf("Action: BF Configuration (%d) \n", bf_action);
			printf("Enable:             %d\n", bf_cfg->bf_enbl);
			printf("Sounding Enable:    %d\n",
			       bf_cfg->sounding_enbl);
			printf("Feedback Type:      %d\n", bf_cfg->fb_type);
			printf("SNR Threshold:      %d\n",
			       bf_cfg->snr_threshold);
			printf("Sounding Interval:  %d\n",
			       bf_cfg->sounding_interval);
			printf("BF Mode:            %d\n", bf_cfg->bf_mode);
			break;
		default:
			break;
		}
	}

exit:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return status;
}

/**
 * @brief      Control WPS Session Enable/Disable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_wps_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wpssession fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	printf("%s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Set/Get Port Control mode
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_port_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int port_ctrl = 0;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: port_ctrl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		port_ctrl = (int)*buffer;
		if (port_ctrl == 1)
			printf("port_ctrl is Enabled\n");
		else
			printf("port_ctrl is Disabled\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Private IOCTL entry to get the By-passed TX packet from upper layer
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_bypassed_packet(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pb_bypass fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/* #ifdef FW_WAKEUP_METHOD */
/**
 * @brief      Set/Get module configuration
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_fw_wakeup_method(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int fw_wakeup_method = 0;
	int gpio_pin = 0;

	if (argc < 3 || argc > 5) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: fwwakeupmethod fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		fw_wakeup_method = (int)*buffer;
		gpio_pin = (int)*(buffer + 4);
		if (fw_wakeup_method == 1)
			printf("FW wakeup method is interface\n");
		else if (fw_wakeup_method == 2)
			printf("FW wakeup method is gpio, GPIO Pin is %d\n",
			       gpio_pin);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/* #endif */

#ifdef SDIO
/**
 *  @brief SD comand53 read/write
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_sdcmd53rw(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	int cmd_header_len = 0;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	t_u8 *buffer = NULL;
	t_u8 *pos = NULL;
	int addr, mode, blklen, blknum, i = 0, rw;
	t_u16 cmd_len = 0;

	if (argc < 8) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);

	pos = buffer + cmd_header_len + sizeof(cmd_len);

	if (argc == 8) {
		rw = pos[0] = 0;	/* CMD53 read */
	} else {
		rw = pos[0] = 1;	/* CMD53 write */
	}
	pos[1] = atoval(argv[3]);	/* func (0/1/2) */
	addr = atoval(argv[4]);	/* address */
	pos[2] = addr & 0xff;
	pos[3] = (addr >> 8) & 0xff;
	pos[4] = (addr >> 16) & 0xff;
	pos[5] = (addr >> 24) & 0xff;
	mode = atoval(argv[5]);	/* byte mode/block mode (0/1) */
	pos[6] = (t_u8)mode;
	blklen = atoval(argv[6]);	/* block size */
	pos[7] = blklen & 0xff;
	pos[8] = (blklen >> 8) & 0xff;
	blknum = atoval(argv[7]);	/* block number or byte number */
	pos[9] = blknum & 0xff;
	pos[10] = (blknum >> 8) & 0xff;
	if (pos[0]) {
		for (i = 0; i < (argc - 8); i++)
			pos[11 + i] = atoval(argv[8 + i]);
	}
	cmd_len = 11 + i;
	memcpy(buffer + cmd_header_len, &cmd_len, sizeof(cmd_len));

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdcmd53rw fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (mode) {
		fprintf(stderr, "CMD53rw blklen = %d, blknum = %d\n", blklen,
			blknum);
	} else {
		blklen = 1;
		fprintf(stderr, "CMD53rw bytelen = %d\n", blknum);
	}
	if (!rw)
		hexdump("CMD53 data", buffer, blklen * blknum, ' ');

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

#ifdef WIFI_DIRECT_SUPPORT
/**
 * @brief      Set/Get P2P NoA (Notice of Absence) Or OPP-PS parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_cfg_noa_opp_ps(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	mlan_ds_wifi_direct_config *cfg;

	if (argc < 3 || argc > 8) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: noa_cfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		cfg = (mlan_ds_wifi_direct_config *)buffer;
		if (!cfg) {
			printf("Err : Could not get P2P noa configuration\n");
			if (cmd)
				free(cmd);
			if (buffer)
				free(buffer);
			return MLAN_STATUS_FAILURE;
		}

		if (cfg->flags & WIFI_DIRECT_NOA) {
			printf("noa_enable : %d\n", cfg->noa_enable);
			printf("index : %d\n", cfg->index);
			printf("noa_count : %d\n", cfg->noa_count);
			printf("noa_duration : %d\n", cfg->noa_duration);
			printf("noa_interval : %d\n", cfg->noa_interval);
		}
		if (cfg->flags & WIFI_DIRECT_OPP_PS) {
			printf("opp_ps_enable : %d\n", cfg->opp_ps_enable);
			printf("ct_window : %d\n", cfg->ct_window);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Issue a dscp map command
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_dscpmap(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	unsigned int dscp, tid, idx;
	t_u8 dscp_map[64];

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		fprintf(stderr, "Cannot alloc memory\n");
		ret = ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = ENOMEM;
		goto done;
	}

	/* buffer = MRVL_CMD<cmd> */
	strncpy((char *)buffer, CMD_NXP, strlen(CMD_NXP));
	strncpy((char *)buffer + strlen(CMD_NXP), argv[2], strlen(argv[2]));

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[dscpmap]");
		printf("ERR:Command sending failed!\n");
		ret = -EFAULT;
		goto done;
	}
	memcpy(dscp_map, buffer, sizeof(dscp_map));

	if ((argc == 4) && (strcmp(argv[3], "reset") == 0)) {
		memset(dscp_map, 0xff, sizeof(dscp_map));
	} else if (argc == (3 + sizeof(dscp_map))) {
		/* Update the entire dscp table */
		for (idx = 3; idx < (3 + sizeof(dscp_map)); idx++) {
			tid = a2hex_or_atoi(argv[idx]);

			if (tid < 8) {
				dscp_map[idx - 3] = tid;
			}
		}
	} else if (argc > 3 && argc <= (3 + sizeof(dscp_map))) {
		/* Update any dscp entries provided on the command line */
		for (idx = 3; idx < argc; idx++) {
			if ((sscanf(argv[idx], "%x=%x", &dscp, &tid) == 2)
			    && (dscp < sizeof(dscp_map))
			    && (tid < 8)) {
				dscp_map[dscp] = tid;
			}
		}
	} else if (argc != 3) {
		printf("Invalid number of arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(buffer, 0, BUFFER_LENGTH);
	/* buffer = MRVL_CMD<cmd> */
	strncpy((char *)buffer, CMD_NXP, strlen(CMD_NXP));
	strncpy((char *)buffer + strlen(CMD_NXP), argv[2], strlen(argv[2]));
	if (argc > 3)
		memcpy(buffer + strlen(CMD_NXP) + strlen(argv[2]),
		       dscp_map, sizeof(dscp_map));
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[dscpmap]");
		printf("ERR:Command sending failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Display the active dscp -> TID mapping table */
	if (cmd->used_len) {
		printf("DscpMap:\n");
		hexdump(NULL, buffer, cmd->used_len, ' ');
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Set/Get DF repeater mode parameters
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_dfs_repeater(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	dfs_repeater *dfs_rptr = NULL;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: %s fail\n", __func__);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		dfs_rptr = (dfs_repeater *) buffer;
		printf("DFS repeater mode: %s\n",
		       (dfs_rptr->mode) ? "On" : "Off");
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#ifdef WIFI_DIRECT_SUPPORT
/**
 *  @brief Set/Get miracast configuration parameters
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_miracastcfg(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	int data[3];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 3 && argc > 6) {
		fprintf(stderr, "mlanutl: Invalid number of arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		fprintf(stderr, "mlanutl: Cannot allocate memory\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		fprintf(stderr,
			"mlanutl: Cannot allocate buffer for command\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: miracastcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(data, 0, sizeof(data));
	/* Process result */
	memcpy(data, buffer, sizeof(data));
	if (argc == 3) {
		/* GET operation */
		printf("Miracast Configuration:\n");
		printf("  Mode: %d\n", data[0]);
		printf("  Scan time: %d\n", data[1]);
		printf("  Scan channel gap: %d\n", data[2]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif /* WIFI_DIRECT_SUPPORT */

/**
 * @brief      Set/get control to coex RX window size
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_coex_rx_winsize(int argc, char *argv[])
{
	int ret = 0;
	int coex_rx_winsize = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR: Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR: Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: coex_rx_winsize fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memcpy(&coex_rx_winsize, buffer, sizeof(coex_rx_winsize));
		printf("COEX RX winsize is %s\n",
		       coex_rx_winsize ? "enabled" : "disabled");
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#ifdef PCIE
/**
 * @brief      Read/Write PCIE register
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_pcie_reg_rw(int argc, char *argv[])
{
	int ret = 0, data[2];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 4) && (argc != 5)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pcieregrw fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 4) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("Offset of PCIE register is %#x\n", data[0]);
		printf("Value at the register is %#x\n", data[1]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Read/Write PCIE register/memory from BAR0
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_pcie_bar0_reg_rw(int argc, char *argv[])
{
	int ret = 0, data[2];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 4) && (argc != 5)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pcieregrw fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 4) {
		memset(data, 0, sizeof(data));
		memcpy(data, buffer, sizeof(data));
		printf("Offset of PCIE register is %#x\n", data[0]);
		printf("Value at the register is %#x\n", data[1]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 * @brief      get SOC sensor temperature.
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_get_sensor_temp(int argc, char *argv[])
{
	int ret = 0;
	t_u32 temp = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX get_sensor_temp\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: temp_sensor fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&temp, buffer, sizeof(t_u32));
	printf("SOC temperature is %u C \n", temp);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief    Get channel report values
 *
 *  @param chanNum   Number of channels
 *  @param startFreq      Start Frequency
 *  @param duration        Duration
 *  @param pAnpi            Pointer to the anpi value
 *  @param pLoadPercent      Pointer to the LoadPercent value
 *  @param chanWidth    Channel width
 *
 *  @return       MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
chanrpt_getValues(t_u8 chanNum, t_u16 startFreq,
		  t_u32 duration, t_s16 *pAnpi, t_u8 *pLoadPercent,
		  t_u8 chanWidth)
{
	int ret = MLAN_STATUS_SUCCESS;
	struct ifreq ifr;
	t_u8 *buffer = NULL, *pos = NULL;
	t_u32 cmd_len = 0, cmd_header_len;
	struct eth_priv_cmd *cmd = NULL;
	t_u8 *pByte;
	int eventLen = 0;
	MrvlIEtypes_Data_t *pTlvHdr;
	HostCmd_DS_GEN *hostcmd;
	HostCmd_DS_CHAN_RPT_RSP *pChanRptRsp = NULL;
	HostCmd_DS_CHAN_RPT_REQ *pChanRptReq = NULL;
	t_u8 *pChanRptEvent = NULL;
	MrvlIEtypes_ChanRptChanLoad_t *pLoadRpt = NULL;
	MrvlIEtypes_ChanRptNoiseHist_t *pNoiseRpt = NULL;

	cmd_header_len = strlen(CMD_NXP) + strlen(HOSTCMD);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		fprintf(stderr, "Cannot alloc memory\n");
		ret = ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = ENOMEM;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* buffer = MRVL_CMD<cmd> */
	strncpy((char *)buffer, CMD_NXP, strlen(CMD_NXP));
	strncpy((char *)buffer + strlen(CMD_NXP), HOSTCMD, strlen(HOSTCMD));

	/* buffer = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN><CMD_DS> */
	hostcmd = (HostCmd_DS_GEN *)(buffer + cmd_header_len + sizeof(t_u32));

	/* Point after host command header */
	pos = (t_u8 *)hostcmd + S_DS_GEN;

	cmd_len = S_DS_GEN + sizeof(HostCmd_DS_CHAN_RPT_REQ);

	hostcmd->command = cpu_to_le16(HostCmd_CMD_CHAN_REPORT_REQUEST);
	hostcmd->size = cpu_to_le16(cmd_len);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	pChanRptReq = (HostCmd_DS_CHAN_RPT_REQ *)pos;

	memset((void *)pChanRptReq, 0x00, sizeof(HostCmd_DS_CHAN_RPT_REQ));

	pChanRptReq->chanDesc.chanNum = chanNum;
	pChanRptReq->chanDesc.startFreq = cpu_to_le16(startFreq);
	pChanRptReq->chanDesc.chanWidth = chanWidth;
	pChanRptReq->millisecDwellTime = duration;

	/* Put buffer length */
	memcpy(buffer + cmd_header_len, &cmd_len, sizeof(t_u32));

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[chanrpt hostcmd]");
		printf("ERR:Command sending failed!\n");
		ret = -EFAULT;
		goto done;
	}

	eventLen = NL_MAX_PAYLOAD;
	pChanRptEvent = (t_u8 *)malloc(NL_MAX_PAYLOAD);

	if (!pChanRptEvent) {
		printf("ERR:Could not allocate buffer!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	ret = wait_event(EVENT_CHANNEL_REPORT_RDY, pChanRptEvent, &eventLen);

	if (ret || !pChanRptEvent || !eventLen) {
		printf("ERR: wait_event failed!\n");
		ret = -EFAULT;
		goto done;
	}

	pChanRptRsp = (HostCmd_DS_CHAN_RPT_RSP *)(pChanRptEvent + EVENT_ID_LEN);
	eventLen -= EVENT_ID_LEN;

	/* TSF is a t_u64, some formatted printing libs have
	 *   trouble printing long longs, so cast and dump as bytes
	 */
	pByte = (t_u8 *)&pChanRptRsp->startTsf;

	pByte = pChanRptRsp->tlvBuffer;

	eventLen -= sizeof(pChanRptRsp->commandResult);
	eventLen -= sizeof(pChanRptRsp->startTsf);
	eventLen -= sizeof(pChanRptRsp->duration);

	pByte = pChanRptRsp->tlvBuffer;

	while ((unsigned int)eventLen >= sizeof(pTlvHdr->header)) {
		pTlvHdr = (MrvlIEtypes_Data_t *)pByte;
		pTlvHdr->header.len = le16_to_cpu(pTlvHdr->header.len);

		switch (le16_to_cpu(pTlvHdr->header.type)) {
		case TLV_TYPE_CHANRPT_CHAN_LOAD:
			pLoadRpt = (MrvlIEtypes_ChanRptChanLoad_t *)pTlvHdr;
			*pLoadPercent = (pLoadRpt->ccaBusyFraction * 100) / 255;
			break;

		case TLV_TYPE_CHANRPT_NOISE_HIST:
			pNoiseRpt = (MrvlIEtypes_ChanRptNoiseHist_t *)pTlvHdr;
			*pAnpi = pNoiseRpt->anpi;
			break;

		default:
			break;
		}

		pByte += (pTlvHdr->header.len + sizeof(pTlvHdr->header));
		eventLen -= (pTlvHdr->header.len + sizeof(pTlvHdr->header));
		eventLen = (eventLen > 0) ? eventLen : 0;
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	if (pChanRptEvent)
		free(pChanRptEvent);
	return ret;
}

/**
 *  @brief    Print x Axis
 *
 *  @param pChanRpt    Pointer to the chan report
 *  @param numChans   Number of channels
 *
 *  @return void
 */
static void
chanrpt_print_xAxis(ChanRptInfo_t * pChanRpt, int numChans)
{
	int idx;

	printf("   `-");

	for (idx = 0; idx < numChans; idx++) {
		printf("----");
	}

	printf("\n     ");

	for (idx = 0; idx < numChans; idx++) {
		printf("%03d ", (pChanRpt + idx)->chanNum);
	}

	printf("\n");
}

/**
 *  @brief    Print anpi
 *
 *  @param pChanRpt    Pointer to the chan report
 *  @param numChans   Number of channels
 *
 *  @return void
 */
static void
chanrpt_print_anpi(ChanRptInfo_t * pChanRpt, int numChans)
{
	int dumpIdx;
	int yAxis;
	int yPrint;

	printf("\n");
	printf("                  Average Noise Power Indicator\n");
	printf("                  -----------------------------\n");

	yPrint = 0;
	for (yAxis = -55; yAxis >= -95; yAxis -= 2) {
		if (yPrint % 2 == 1) {
			printf("%2d| ", yAxis);
		} else {
			printf("   | ");
		}

		yPrint++;

		for (dumpIdx = 0; dumpIdx < numChans; dumpIdx++) {
			if ((pChanRpt + dumpIdx)->anpi >= yAxis) {
				printf("### ");
			} else if ((pChanRpt + dumpIdx)->anpi >= yAxis - 2) {
				printf("%3d ", (pChanRpt + dumpIdx)->anpi);
			} else {
				printf("    ");
			}
		}

		printf("\n");
	}

	chanrpt_print_xAxis(pChanRpt, numChans);
}

/**
 *  @brief    Print chan load
 *
 *  @param pChanRpt    Pointer to the chan report
 *  @param numChans   Number of channels
 *
 *  @return void
 */
static void
chanrpt_print_chanLoad(ChanRptInfo_t * pChanRpt, int numChans)
{
	int dumpIdx;
	int yAxis;
	int yPrint;

	printf("\n");
	printf("                         Channel Load\n");
	printf("                         ------------\n");

	yPrint = 0;
	for (yAxis = 100; yAxis >= 0; yAxis -= 5) {
		if (yPrint % 2 == 1) {
			printf("%2d%%| ", yAxis);
		} else {
			printf("   | ");
		}

		yPrint++;

		for (dumpIdx = 0; dumpIdx < numChans; dumpIdx++) {
			if ((pChanRpt + dumpIdx)->chanLoad >= yAxis) {
				printf("### ");
			} else if ((pChanRpt + dumpIdx)->chanLoad >= yAxis - 5) {
				printf("%2d%% ",
				       (pChanRpt + dumpIdx)->chanLoad);
			} else {
				printf("    ");
			}
		}

		printf("\n");
	}
	chanrpt_print_xAxis(pChanRpt, numChans);
}

/**
 *  @brief    Get chanrpt values and print graph
 *
 *  @param void
 *
 *  @return void
 */
static void
chanrpt_graph(void)
{
	int idx;
	ChanRptInfo_t chanRpt[14];

	memset(chanRpt, 0x00, sizeof(chanRpt));
	for (idx = 0; (unsigned int)idx < NELEMENTS(chanRpt); idx++) {
		chanRpt[idx].chanNum = idx + 1;
		chanrpt_getValues(idx + 1,
				  0,
				  100,
				  &chanRpt[idx].anpi, &chanRpt[idx].chanLoad,
				  0);
	}

	chanrpt_print_anpi(chanRpt, NELEMENTS(chanRpt));
	chanrpt_print_chanLoad(chanRpt, NELEMENTS(chanRpt));
}

/**
 *  @brief    Loops to get chanrpt values and print graph, at end print 1 average graph
 *
 *  @param loopOnLoad     Print Load graph loop on
 *  @param loopOnAnpi      Print anpi graph loop on
 *  @param loops                Loops
 *
 *  @return  void
 */
static void
chanrpt_graph_loop(boolean loopOnLoad, boolean loopOnAnpi, int loops)
{
	int idx;
	int loopsLeft;
	ChanRptInfo_t chanRpt[14];
	ChanRptInfo_t chanRptAvg[14];

	memset(chanRpt, 0x00, sizeof(chanRpt));
	memset(chanRptAvg, 0x00, sizeof(chanRptAvg));
	for (idx = 0; (unsigned int)idx < NELEMENTS(chanRpt); idx++) {
		chanRpt[idx].chanNum = idx + 1;
		chanrpt_getValues(idx + 1,
				  0,
				  100,
				  &chanRpt[idx].anpi, &chanRpt[idx].chanLoad,
				  0);
	}

	idx = 0;
	loopsLeft = loops;

	while (loopsLeft) {
		chanRpt[idx].chanNum = idx + 1;
		chanrpt_getValues(idx + 1,
				  0,
				  75,
				  &chanRpt[idx].anpi, &chanRpt[idx].chanLoad,
				  0);

		chanRptAvg[idx].chanNum = idx + 1;
		chanRptAvg[idx].anpi
			= (chanRptAvg[idx].anpi * (loops - loopsLeft)
			   + chanRpt[idx].anpi) / (loops - loopsLeft + 1);

		chanRptAvg[idx].chanLoad
			= (chanRptAvg[idx].chanLoad * (loops - loopsLeft)
			   + chanRpt[idx].chanLoad) / (loops - loopsLeft + 1);

		idx = (idx + 1) % NELEMENTS(chanRpt);

		if (idx == 0) {
			if (loopOnAnpi) {
				chanrpt_print_anpi(chanRpt, NELEMENTS(chanRpt));
			}

			if (loopOnLoad) {
				chanrpt_print_chanLoad(chanRpt,
						       NELEMENTS(chanRpt));
			}

			loopsLeft--;
		}
	}

	if (loopOnAnpi) {
		chanrpt_print_anpi(chanRptAvg, NELEMENTS(chanRptAvg));
	}

	if (loopOnLoad) {
		chanrpt_print_chanLoad(chanRptAvg, NELEMENTS(chanRptAvg));
	}
}

/**
 *  @brief Issue a changraph command
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_chan_graph(int argc, char *argv[])
{
	if (argc == 3) {
		chanrpt_graph();
	} else if (argc == 5) {
		t_u8 input = atoi(argv[4]);

		if (input <= 100) {
			if (strcmp(argv[3], "load") == 0) {
				chanrpt_graph_loop(TRUE, FALSE, input);
			} else if (strcmp(argv[3], "anpi") == 0) {
				chanrpt_graph_loop(FALSE, TRUE, input);
			} else if (strcmp(argv[3], "anpiload") == 0) {
				chanrpt_graph_loop(TRUE, TRUE, input);
			} else {
				printf("\nchangraph syntax:"
				       " changraph <load | anpi | anpiload> <loops>\n The value of <loops> should be <= 100\n");
			}
		} else {
			printf("\nchangraph syntax:"
			       " changraph <load | anpi | anpiload> <loops>\n The value of <loops> should be <= 100\n");
		}
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process extended channel switch(ECSA)
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_extend_channel_switch(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: extended channel switch fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get/Set per packet Txctl and Rxinfo configuration
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_per_pkt_cfg(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS, i, j;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	MrvlIEtypes_per_pkt_cfg_t *per_pkt_cfg = NULL;
	struct ifreq ifr;
	t_u8 *pos = NULL;

	/* Sanity tests */
	if (argc != 3 && argc != 4 && argc < 6) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX perpktcfg [tx_rx_control] [type_num] [ether_type1 ...] [tx_rx_control] [type_num] [ether_type1 ...]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(buffer, 0, BUFFER_LENGTH);

	/* Flag it for our use */
	pos = buffer;
	strncpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));
	/* Insert command */
	strncpy((char *)pos, argv[2], strlen(argv[2]));
	pos += (strlen(argv[2]));

	if (argc == 3) {
		*pos = ACTION_GET;
	} else {
		*pos = ACTION_SET;
		pos++;
		i = 3;
		while (i < argc) {
			per_pkt_cfg = (MrvlIEtypes_per_pkt_cfg_t *) pos;
			per_pkt_cfg->header.type = TLV_TYPE_PER_PKT_CFG;
			if (a2hex_or_atoi(argv[i]) <= MAX_TXRX_CTRL) {
				per_pkt_cfg->tx_rx_control =
					(t_u8)a2hex_or_atoi(argv[i++]);
				per_pkt_cfg->header.len += sizeof(t_u8);
				if (per_pkt_cfg->tx_rx_control == 0)
					break;
				else if (i >= argc) {
					printf("Error: invalid arguments, type_num is needed when tx_rx_control != 0.\n");
					ret = MLAN_STATUS_FAILURE;
					goto done;
				}
			} else {
				printf("Error: invalid arguments, tx_rx_control <=3\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}

			if (a2hex_or_atoi(argv[i]) <= MAX_NUM_ETHER_TYPE &&
			    a2hex_or_atoi(argv[i]) > 0)
				per_pkt_cfg->proto_type_num =
					a2hex_or_atoi(argv[i++]);
			else {
				printf("Error: invalid arguments, type_num <=8\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}

			for (j = 0; j < per_pkt_cfg->proto_type_num; j++) {
				if (i < argc)
					per_pkt_cfg->ether_type[j] =
						(t_u16)a2hex_or_atoi(argv[i++]);
				else {
					printf("Error: invalid arguments, number of ether type less than type_num \n");
					ret = MLAN_STATUS_FAILURE;
					goto done;
				}
			}
			per_pkt_cfg->header.len =
				sizeof(MrvlIEtypes_per_pkt_cfg_t) +
				per_pkt_cfg->proto_type_num * sizeof(t_u16) -
				sizeof(MrvlIEtypesHeader_t);
			pos += per_pkt_cfg->header.len +
				sizeof(MrvlIEtypesHeader_t);
		}
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: perpktcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process Get result */
	pos = buffer + (strlen(CMD_NXP)) + strlen(argv[2]);
	if (*pos == ACTION_GET) {
		per_pkt_cfg = (MrvlIEtypes_per_pkt_cfg_t *)++ pos;
		while (per_pkt_cfg->header.type == TLV_TYPE_PER_PKT_CFG) {
			if (per_pkt_cfg->tx_rx_control & TX_PKT_CTRL) {
				printf("The ethernet type of per packet Txctrl:\n");
				for (j = 0; j < per_pkt_cfg->proto_type_num;
				     j++) {
					printf("0x%04x  ",
					       per_pkt_cfg->ether_type[j]);
				}
				printf("\n");
			} else if (per_pkt_cfg->tx_rx_control & RX_PKT_INFO) {
				printf("The ethernet type of per packet Rxinfo:\n");
				for (j = 0; j < per_pkt_cfg->proto_type_num;
				     j++) {
					printf("0x%04x  ",
					       per_pkt_cfg->ether_type[j]);
				}
				printf("\n");
			}
			pos += per_pkt_cfg->header.len +
				sizeof(MrvlIEtypesHeader_t);
			per_pkt_cfg = (MrvlIEtypes_per_pkt_cfg_t *) pos;
		}
	} else if (argc == 4) {
		printf("Disable per packet control!\n");
	} else {
		printf("Successfully do the TXctl/Rxinfo per packet configuration!\n");
	}
done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 * @brief      enable auto_arp  1->enable  0->disable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_auto_arp(int argc, char *argv[])
{
	int ret = 0, status;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3 && argc != 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX auto_arp [0/1]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], argc - 3, &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: auto_arp fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process Get result */
	if (argc == 3) {
		memcpy(&status, buffer, sizeof(status));
		if (status == 0) {
			printf("Auto ARP is Disabled\n");
		} else if (status == 1) {
			printf("Auto ARP is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 * @brief Creates a tx/rx histogram statistic request and send to driver
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_txrxhistogram(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = MLAN_STATUS_SUCCESS, i;
	struct ifreq ifr;
	tx_rx_histogram *tx_rx_info;
	tx_pkt_ht_rate_info *tx_ht_info;
	tx_pkt_vht_rate_info *tx_vht_info;
	tx_pkt_he_rate_info *tx_he_info;
	tx_pkt_rate_info *tx_info;
	rx_pkt_ht_rate_info *rx_ht_info;
	rx_pkt_vht_rate_info *rx_vht_info;
	rx_pkt_he_rate_info *rx_he_info;
	rx_pkt_rate_info *rx_info;
	t_u16 size = 0;
	t_u8 *pos = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 5 && argc != 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX txrxhistogram [action: 0/1/2] [tx_rx_statics: 1/2/3]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));
	tx_rx_info = (tx_rx_histogram *) (buffer + strlen(argv[2]));
	tx_rx_info->enable = (t_u8)a2hex_or_atoi(argv[3]);
	if (argc == 5 && tx_rx_info->enable == GET_TX_RX_HISTOGRAM)
		tx_rx_info->action = (t_u8)a2hex_or_atoi(argv[4]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get tx/rx histogram fail\n");;
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	pos = buffer + strlen(argv[2]) + 2 * sizeof(t_u8);
	memcpy(&size, pos, sizeof(size));
	pos += sizeof(size);
	if (tx_rx_info->enable & GET_TX_RX_HISTOGRAM) {
		if (tx_rx_info->action & FLAG_TX_HISTOGRAM) {
			printf("The TX histogram statistic:\n");
			printf("============================================\n");
			tx_ht_info = (tx_pkt_ht_rate_info *) pos;
			for (i = 0; i < 16; i++) {
				printf("htmcs_txcnt[%d]       = %u\n", i,
				       tx_ht_info->htmcs_txcnt[i]);
				printf("htsgi_txcnt[%d]       = %u\n", i,
				       tx_ht_info->htsgi_txcnt[i]);
				printf("htstbcrate_txcnt[%d]  = %u\n", i,
				       tx_ht_info->htstbcrate_txcnt[i]);
			}
			pos += sizeof(tx_pkt_ht_rate_info);
			tx_vht_info = (tx_pkt_vht_rate_info *) pos;
			for (i = 0; i < 10; i++) {
				printf("vhtmcs_txcnt[%d]      = %u\n", i,
				       tx_vht_info->vhtmcs_txcnt[i]);
				printf("vhtsgi_txcnt[%d]      = %u\n", i,
				       tx_vht_info->vhtsgi_txcnt[i]);
				printf("vhtstbcrate_txcnt[%d] = %u\n", i,
				       tx_vht_info->vhtstbcrate_txcnt[i]);
			}
			pos += sizeof(tx_pkt_vht_rate_info);
			if (size ==
			    (sizeof(tx_pkt_ht_rate_info) +
			     sizeof(tx_pkt_vht_rate_info) +
			     sizeof(tx_pkt_he_rate_info) +
			     sizeof(tx_pkt_rate_info))
			    || size ==
			    (sizeof(tx_pkt_ht_rate_info) +
			     sizeof(tx_pkt_vht_rate_info) +
			     sizeof(tx_pkt_he_rate_info) +
			     sizeof(tx_pkt_rate_info)
			     + sizeof(rx_pkt_ht_rate_info) +
			     sizeof(rx_pkt_vht_rate_info) +
			     sizeof(rx_pkt_he_rate_info) +
			     sizeof(rx_pkt_rate_info))) {
				tx_he_info = (tx_pkt_he_rate_info *) pos;
				for (i = 0; i < 12; i++) {
					printf("hemcs_txcnt[%d]      = %u\n", i,
					       tx_he_info->hemcs_txcnt[i]);
					printf("hestbcrate_txcnt[%d] = %u\n", i,
					       tx_he_info->hestbcrate_txcnt[i]);
				}
				pos += sizeof(tx_pkt_he_rate_info);
			}
			tx_info = (tx_pkt_rate_info *) pos;
			for (i = 0; i < 2; i++)
				printf("nss_txcnt[%d]         = %u\n", i,
				       tx_info->nss_txcnt[i]);
			for (i = 0; i < 3; i++)
				printf("bandwidth_txcnt[%d]   = %u\n", i,
				       tx_info->bandwidth_txcnt[i]);
			for (i = 0; i < 4; i++)
				printf("preamble_txcnt[%d]    = %u\n", i,
				       tx_info->preamble_txcnt[i]);
			printf("ldpc_txcnt           = %u\n",
			       tx_info->ldpc_txcnt);
			printf("rts_txcnt            = %u\n",
			       tx_info->rts_txcnt);
			printf("ack_RSSI             = %d\n\n",
			       tx_info->ack_RSSI);
			pos += sizeof(tx_pkt_rate_info);
		}

		if (tx_rx_info->action & FLAG_RX_HISTOGRAM) {
			printf("The RX histogram statistic:\n");
			printf("============================================\n");
			rx_ht_info = (rx_pkt_ht_rate_info *) pos;
			for (i = 0; i < 16; i++) {
				printf("htmcs_rxcnt[%d]       = %u\n", i,
				       rx_ht_info->htmcs_rxcnt[i]);
				printf("htsgi_rxcnt[%d]       = %u\n", i,
				       rx_ht_info->htsgi_rxcnt[i]);
				printf("htstbcrate_rxcnt[%d]  = %u\n", i,
				       rx_ht_info->htstbcrate_rxcnt[i]);
			}
			pos += sizeof(rx_pkt_ht_rate_info);
			rx_vht_info = (rx_pkt_vht_rate_info *) pos;
			for (i = 0; i < 10; i++) {
				printf("vhtmcs_rxcnt[%d]      = %u\n", i,
				       rx_vht_info->vhtmcs_rxcnt[i]);
				printf("vhtsgi_rxcnt[%d]      = %u\n", i,
				       rx_vht_info->vhtsgi_rxcnt[i]);
				printf("vhtstbcrate_rxcnt[%d] = %u\n", i,
				       rx_vht_info->vhtstbcrate_rxcnt[i]);
			}
			pos += sizeof(rx_pkt_vht_rate_info);
			if (size ==
			    (sizeof(rx_pkt_ht_rate_info) +
			     sizeof(rx_pkt_vht_rate_info) +
			     sizeof(rx_pkt_he_rate_info) +
			     sizeof(rx_pkt_rate_info))
			    || size ==
			    (sizeof(tx_pkt_ht_rate_info) +
			     sizeof(tx_pkt_vht_rate_info) +
			     sizeof(tx_pkt_he_rate_info) +
			     sizeof(tx_pkt_rate_info)
			     + sizeof(rx_pkt_ht_rate_info) +
			     sizeof(rx_pkt_vht_rate_info) +
			     sizeof(rx_pkt_he_rate_info) +
			     sizeof(rx_pkt_rate_info))) {
				rx_he_info = (rx_pkt_he_rate_info *) pos;
				for (i = 0; i < 12; i++) {
					printf("hemcs_txcnt[%d]      = %u\n", i,
					       rx_he_info->hemcs_rxcnt[i]);
					printf("hestbcrate_txcnt[%d] = %u\n", i,
					       rx_he_info->hestbcrate_rxcnt[i]);
				}
				pos += sizeof(rx_pkt_he_rate_info);
			}
			rx_info = (rx_pkt_rate_info *) pos;
			for (i = 0; i < 2; i++)
				printf("nss_rxcnt[%d]         = %u\n", i,
				       rx_info->nss_rxcnt[i]);
			printf("nsts_rxcnt           = %u\n",
			       rx_info->nsts_rxcnt);
			for (i = 0; i < 3; i++)
				printf("bandwidth_rxcnt[%d]   = %u\n", i,
				       rx_info->bandwidth_rxcnt[i]);
			for (i = 0; i < 6; i++)
				printf("preamble_rxcnt[%d]    = %u\n", i,
				       rx_info->preamble_rxcnt[i]);
			for (i = 0; i < 2; i++)
				printf("ldpc_txbfcnt[%d]      = %u\n", i,
				       rx_info->ldpc_txbfcnt[i]);
			for (i = 0; i < 2; i++)
				printf("rssi_value[%d]        = %d\n", i,
				       rx_info->rssi_value[i]);
			for (i = 0; i < 4; i++)
				printf("rssi_chain0[%d]       = %d\n", i,
				       rx_info->rssi_chain0[i]);
			for (i = 0; i < 4; i++)
				printf("rssi_chain1[%d]       = %d\n", i,
				       rx_info->rssi_chain1[i]);
			printf("\n");
		}
	} else if (tx_rx_info->enable & ENABLE_TX_RX_HISTOGRAM)
		printf("Enable the TX and RX histogram statistic\n");
	else
		printf("Disable the TX and RX histogram statistic\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get out band independent reset
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_ind_rst_cfg(int argc, char *argv[])
{
	int ret = 0;
	int data[3] = { 0 };
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc < 3) || (argc > 5)) {
		printf("ERR: Invalid arguments\n");
		printf("usage: mlanutl <interface> indrstcfg <ir_mode> [gpio_pin]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (((argc == 4) || (argc == 5)) &&
	    ((atoi(argv[3]) < 0) || (atoi(argv[3]) > 2))) {
		printf("ERR: Mode must be 0, 1 or 2\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: indrstcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memcpy(data, buffer, sizeof(data));
		/* Display the result */
		printf("Independent Reset Mode = %s\n",
		       (data[0] ==
			0) ? "disabled" : ((data[0] ==
					    1) ? "Out Band" : "In Band"));
		if (data[0] == 1)
			printf("GPIO Pin = %d\n", data[1]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

static void
send_dot11_packet(char *ifName, char *file_name)
{
	t_u8 sendbuf[BUF_SIZ], buff[BUF_SIZ];
	char *args[100], *pos, mac_addr[20];
	struct ether_header *eh = (struct ether_header *)sendbuf;
	int data_len = 0, tx_len = 0;
	dot11_txcontrol *txc;
	struct sockaddr_ll socket_address;
	wsmp_header *header;
	t_u8 mac[ETH_ALEN];
	FILE *config_file = NULL;
	char *line = NULL;
	int li = 0, arg_num = 0, ret = 0, i = 0;
	int protocol = 0;
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;

	memset(buff, 0, sizeof(buff));
	memset(mac, 0, sizeof(mac));
	memset(mac_addr, 0, sizeof(mac_addr));

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	txc = (dot11_txcontrol *) (sendbuf + sizeof(struct ether_header));
	config_file = fopen(file_name, "r");
	if (config_file == NULL) {
		perror("CONFIG");
		exit(1);
	}

	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		exit(1);
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		arg_num = parse_line(line, args, 100);

		if (strcmp(args[0], "Datarate") == 0) {
			txc->tx_datarate = ((t_u16)A2HEXDECIMAL(args[1]));
			printf("datarate(in 0.5Mbps) = %d\n", txc->tx_datarate);
		} else if (strcmp(args[0], "Channel") == 0) {
			txc->tx_channel = atoi(args[1]);
			printf("channel = %d\n", txc->tx_channel);
		} else if (strcmp(args[0], "Bandwidth") == 0) {
			txc->tx_Bw = (t_u8)A2HEXDECIMAL(args[1]);
			printf("Bandwidth = %d\n", txc->tx_Bw);
		} else if (strcmp(args[0], "Power") == 0) {
			txc->tx_power = (t_u8)A2HEXDECIMAL(args[1]);
			printf("powerlevel = %d\n", txc->tx_power);
		} else if (strcmp(args[0], "Priority") == 0) {
			txc->pkt_priority = atoi(args[1]);
			printf("Pkt_priority = %d\n", txc->pkt_priority);
		} else if (strcmp(args[0], "Retry_limit") == 0) {
			txc->retry_limit = atoi(args[1]);
			printf("Retry_limit = %d\n", txc->retry_limit);
		} else if (strncmp(args[0], "Addr", 4) == 0) {

			strncpy(mac_addr, args[1], sizeof(mac_addr) - 1);
			ret = mac2raw(mac_addr, mac);
			printf("destination MAC : %s\n", mac_addr);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("%s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
			}
		} else if (strcmp(args[0], "Data") == 0) {
			for (i = 0; i < arg_num - 1; i++) {
				buff[i] = (t_u8)A2HEXDECIMAL(args[i + 1]);
			}
		} else if (strcmp(args[0], "Protocol") == 0) {
			protocol = A2HEXDECIMAL(args[1]);
			printf("protocol = %x\n", protocol);
		}
		data_len = arg_num - 1;
	}

	/* Open RAW socket to send/recv on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(protocol))) == -1) {
		perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");

	/*get mac address of the interface */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");

	memcpy(eh->ether_shost, (u_int8_t *) & if_mac.ifr_hwaddr.sa_data,
	       MLAN_MAC_ADDR_LENGTH);

	/*  destination mac address */
	for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++) {
		eh->ether_dhost[i] = (uint8_t) mac[i];
	}
	/* Ethertype field */
	eh->ether_type = htons(protocol);
	tx_len += sizeof(struct ether_header);
	/*Add the length of tx header */
	tx_len += sizeof(dot11_txcontrol);

	if (protocol == ETH_P_WSMP) {
		header = (wsmp_header *) (sendbuf + tx_len);
		header->version = 2;
		header->sec_type = 5;
		header->chan = txc->tx_channel;
		header->rate = txc->tx_datarate / 2;
		header->tx_pow = txc->tx_power;
		header->app_class = 14;
		header->acm_len = 0;
		header->len = data_len;
		tx_len += sizeof(wsmp_header);
	}
	memcpy(sendbuf + tx_len, buff, data_len);

	tx_len += data_len;

	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length */
	socket_address.sll_halen = ETH_ALEN;
	memcpy(&socket_address.sll_addr, (uint8_t *) mac, MLAN_MAC_ADDR_LENGTH);
	if (sendto(sockfd, sendbuf, tx_len, 0,
		   (struct sockaddr *)&socket_address,
		   sizeof(struct sockaddr_ll)) < 0)
		perror("Send failed\n");
	else
		printf("packet sent\n");

	close(sockfd);
	fclose(config_file);
}

static void
receive_dot11_packet(char *ifName, int protocol, int verbose)
{
	int sockopt = 0, i, j, prev_channel = 0;
	t_u8 buf[BUF_SIZ];
	ssize_t numbytes;
	struct ether_header *eh = (struct ether_header *)buf;
	dot11_rxcontrol *rxctrl;
	int sockfd = 0;
	struct ifreq if_idx;
	struct ifreq if_mac;

	/* Open RAW socket to recv on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(protocol))) == -1) {
		perror("socket");
	}
	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");

	/*get mac address of the interface */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");

	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt
	    (sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt,
	     sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	/* Bind to device */
	if (setsockopt
	    (sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ - 1) == -1) {
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	printf("protocol type : %x\n", protocol);
	printf("waiting to receive data...\n");
	rxctrl = (dot11_rxcontrol *) (buf + sizeof(struct ether_header));
	while (1) {
		numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
		if (verbose == 1) {
			printf("\nReceived data from peer - ");
			for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++) {
				printf("%02x", eh->ether_shost[i]);
				if (i < (MLAN_MAC_ADDR_LENGTH - 1))
					printf(":");
			}
			printf("\n");
			printf("\nchannel = %d\ndata rate = %d\n"
			       "antenna = %d\nRSSI = %d\n",
			       rxctrl->rx_channel,
			       rxctrl->rx_datarate,
			       rxctrl->rx_antenna, rxctrl->rx_RSSI);

			i = sizeof(struct ether_header) +
				sizeof(dot11_rxcontrol);

			if (protocol == ETH_P_WSMP) {
				j = i;
				printf("WSMP header : \n");
				for (; i < j + sizeof(wsmp_header); i++)
					printf("%02x ", buf[i]);
				printf("\n");
			}
			printf("Data:\n");

			for (; i < numbytes; i++)
				printf("%02x ", buf[i]);
			printf("\n");
		} else {
			if (rxctrl->rx_channel != prev_channel) {
				printf("channel %d : ", rxctrl->rx_channel);
				for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++)
					printf("%x ", eh->ether_shost[i]);
				printf("\n");
				prev_channel = rxctrl->rx_channel;
			} else
				printf("*");
			fflush(stdout);
		}
	}
}

/**
 * @brief    process request to send/recv WSMP packets
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 * */
static int
process_dot11_txrx(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	char ifName[IFNAMSIZ], file_name[100];
	int receive_protocol, verbose = 0;

	memset(ifName, 0, sizeof(ifName));
	memset(file_name, 0, sizeof(file_name));
	if (argc != 5 && argc != 6) {
		fprintf(stderr, "Invalid no. of arguments\n");
		fprintf(stderr,
			"Usage : ./mlanutil <interface> dot11_txrx <send/recv> <options>\n"
			"./mlanutil <interface> dot11_txrx send <config/tx_ctrl.conf>\n"
			"./mlanutil <interface> dot11_txrx recv <protocol> [v]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/*copy interface name */
	strncpy(ifName, argv[1], sizeof(ifName) - 1);

	if (!strcmp(argv[3], "send")) {
		strncpy(file_name, argv[4], sizeof(file_name) - 1);
		if (argc == 5)
			send_dot11_packet(ifName, file_name);
		else {
			fprintf(stderr, "Usage: "
				"./mlanutil <interface> dot11_txrx send <config/tx_ctrl.conf>\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	} else if (!strcmp(argv[3], "recv")) {
		receive_protocol = A2HEXDECIMAL(argv[4]);
		if (argc == 6 && strcmp(argv[5], "v") == 0)
			verbose = 1;
		receive_dot11_packet(ifName, receive_protocol, verbose);
	} else {
		fprintf(stderr, "Invalid option after dot11_txrx\n");
		fprintf(stderr,
			"Usage : ./mlanutil <interface> dot11_txrx <send/recv> [options]\n"
			"./mlanutil <interface> dot11_txrx send <config/tx_ctrl.conf> "
			"./mlanutil <interface> dot11_txrx recv <protocol> [v]\n");
	}

done:
	return ret;
}

/**
 *  @brief Issue a tsf command
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_tsf(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	int x;
	struct ifreq ifr;
	t_u8 *buffer = NULL, *pos = NULL;
	t_u32 cmd_len = 0, cmd_header_len;
	struct eth_priv_cmd *cmd = NULL;
	HostCmd_DS_GEN *hostcmd;

	cmd_header_len = strlen(CMD_NXP) + strlen(HOSTCMD);

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		fprintf(stderr, "Cannot alloc memory\n");
		ret = ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = ENOMEM;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* buffer = MRVL_CMD<cmd> */
	strncpy((char *)buffer, CMD_NXP, strlen(CMD_NXP));
	strncpy((char *)buffer + strlen(CMD_NXP), HOSTCMD, strlen(HOSTCMD));

	/* buffer = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN><CMD_DS> */
	hostcmd = (HostCmd_DS_GEN *)(buffer + cmd_header_len + sizeof(t_u32));

	/* Point after host command header */
	pos = (t_u8 *)hostcmd + S_DS_GEN;

	cmd_len = S_DS_GEN + sizeof(t_u64);

	hostcmd->command = cpu_to_le16(HostCmd_CMD_GET_TSF);
	hostcmd->size = cpu_to_le16(cmd_len);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	/* Put buffer length */
	memcpy(buffer + cmd_header_len, &cmd_len, sizeof(t_u32));

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[hostcmd]");
		printf("ERR:Command sending failed!\n");
		ret = -EFAULT;
		goto done;
	}

	printf("TSF=");

	for (x = 7; x >= 0; x--) {
		printf("%02x", pos[x]);
	}

	puts("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process dynamic bandwidth set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_dyn_bw(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int dyn_bw = 0;

	/* Check arguments */
	if (argc < 3 || argc > 4) {
		printf("ERR:Incorrect number of arguments!\n");
		printf("Syntax: ./mlanutl mlanX dyn_bw <bw>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dyn_bw fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	dyn_bw = *(int *)buffer;
	printf("Dynamic bandwidth: 0x%02x\n", dyn_bw);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process set/get deauth control when uap move to another channel
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_ctrldeauth(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: deauthctrl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Deauth control: ");
	if (buffer[0])
		printf("enabled.\n");
	else
		printf("disabled.\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process boot sleep configure command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bootsleep(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check arguments */
	if (argc != 3 && argc != 4) {
		printf("ERR:Incorrect number of arguments!\n");
		printf("Syntax: ./mlanutl mlanX bootsleep <1/0>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bootsleep fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process Get result */
	if (argc == 3) {
		printf("boot sleep status: %u\n", *(t_u16 *)buffer);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process static rx abort config set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rx_abort_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	rx_abort_cfg_para data;
	int rssi = 0;

	/* Check arguments */
	if (argc < 3 || (argc > 5)) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rx_abort_cfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	memset((void *)&data, 0, sizeof(data));
	memcpy((void *)&data, buffer, sizeof(data));

	printf("Static Rx Abort %s\n",
	       (data.enable == 1) ? "enabled" : "disabled");

	if (data.enable) {
		/* on some platforms, t_s8 is same as unsigned char */
		rssi = (int)(data.rssi_threshold);
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("RSSI Threshold : %s%ddBm\n", ((rssi > 0) ? "-" : ""),
		       rssi);
	}
	printf("\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process dynamic rx abort config set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rx_abort_cfg_ext(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	rx_abort_cfg_ext_para data;
	int rssi;

	/* Check arguments */
	if (argc < 3 || (argc > 6)) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rx_abort_cfg_ext fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	memset((void *)&data, 0, sizeof(data));
	memcpy((void *)&data, buffer, sizeof(data));

	printf("Dynamic Rx Abort %s\n",
	       (data.enable == 1) ? "enabled" : "disabled");

	if (data.enable) {
		/* on some platforms, t_s8 is same as unsigned char */
		rssi = (int)(data.rssi_margin);
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("RSSI Margin : %s%ddBm\n", ((rssi > 0) ? "-" : ""),
		       rssi);

		rssi = (int)(data.ceil_rssi_threshold);
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("Ceil RSSI Threshold : %s%ddBm\n",
		       ((rssi > 0) ? "-" : ""), rssi);
	}
	printf("\n");
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx ampdu protection mode set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tx_ampdu_prot_mode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	tx_ampdu_prot_mode_para data;

	/* Check arguments */
	if (argc < 3 || (argc > 4)) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: tx_ampdu_prot_mode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	memset((void *)&data, 0, sizeof(data));
	memcpy((void *)&data, buffer, sizeof(data));

	printf("Tx AMPDU protection mode: ");
	if (data.mode == TX_AMPDU_RTS_CTS)
		printf("RTS/CTS\n");
	else if (data.mode == TX_AMPDU_CTS_2_SELF)
		printf("CTS-2-SELF\n");
	else if (data.mode == TX_AMPDU_DYNAMIC_RTS_CTS)
		printf("DYNAMIC RTS/CTS\n");
	else
		printf("Disabled\n");
	printf("\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process rate adapt config set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rate_adapt_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	rate_adapt_cfg_para data;

	/* Check arguments */
	if ((argc < 3) || (argc > 4 && argc != 7)) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rate_adapt_cfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	memset((void *)&data, 0, sizeof(data));
	memcpy((void *)&data, buffer, sizeof(data));

	printf("Rate Adapt Cfg:\n");

	if (data.sr_rateadapt == 0)
		printf(" Legacy RateAdapt Enabled\n");
	else {
		printf(" SR RateAdapt Enabled\n");
		if ((data.ra_low_thresh & data.ra_high_thresh) == 0xff)
			printf("Dynamic rate adaptation mode based on noise level active\n");
		else {
			printf("Aggregated data Tx success rate static thresholds:\n");
			printf("    Low   : %u\n", data.ra_low_thresh);
			printf("    High  : %u\n", data.ra_high_thresh);
		}
		printf("Eval Timer interval  : %u  i.e. %ums\n",
		       data.ra_interval, 10 * data.ra_interval);
		printf("(in multiples of 10)\n\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process cck desense config set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cck_desense_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	cck_desense_cfg_para data;
	int rssi;

	/* Check arguments */
	if (argc < 3 || argc > 8) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cck_desense_cfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	memset((void *)&data, 0, sizeof(data));
	memcpy((void *)&data, buffer, sizeof(data));

	printf("CCK Desense %s\n", (data.mode) ? "enabled" : "disabled");

	if (data.mode != CCK_DESENSE_MODE_DISABLED) {
		printf("Mode: %s\n",
		       (data.mode == CCK_DESENSE_MODE_DYNAMIC) ?
		       "Dynamic" : "Dynamic Enhanced");
		/* on some platforms, t_s8 is same as unsigned char */
		rssi = (int)(data.margin);
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("Margin : %s%ddBm\n", ((rssi > 0) ? "-" : ""), rssi);

		rssi = (int)(data.ceil_thresh);
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("Ceil RSSI Threshold : %s%ddBm\n",
		       ((rssi > 0) ? "-" : ""), rssi);
	}
	if (data.mode == CCK_DESENSE_MODE_DYN_ENH) {
		printf("Num ON intervals  : %d\n", data.num_on_intervals);
		printf("Num OFF intervals : %d\n", data.num_off_intervals);
	}
	printf("\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process low power mode config set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_lpm(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	t_u16 lpm = 0;

	/* Check arguments */
	if (argc < 3 || argc > 4) {
		printf("ERR:Incorrect number of arguments!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: lpm fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* GET operation */
		lpm = *(t_u16 *)buffer;
		printf("low power mode is %d\n", lpm);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process twt_setup
 *  @param argc    number of arguments
 *  @param argv    a pointer to arguments array
 *  @return        MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_twt_setup(int argc, char *argv[])
{
	twt_setup *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = MLAN_STATUS_SUCCESS, cmd_found = 0, cmd_header_len =
		0;
	char *args[30], *pos = NULL;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX twt_setup <config/twt.conf>\n");
		return MLAN_STATUS_FAILURE;
	}

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);
	prepare_buffer(buffer, argv[2], 0, NULL);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR: Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = cmd_header_len;
	cmd->total_len = BUFFER_LENGTH;

	param_buf = (twt_setup *) ((t_u8 *)buffer + cmd_header_len);

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Could not open Config file.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args, 30);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "Implicit") == 0) {
			param_buf->implicit = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "Announced") == 0) {
			param_buf->announced = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TriggerEnabled") == 0) {
			param_buf->triggerEn = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TWTInformationDisabled") == 0) {
			param_buf->twtInfoDisabled =
				(t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "NegotiationType") == 0) {
			param_buf->negotiationType =
				(t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TWTWakeupDuration") == 0) {
			param_buf->twtWakeupDuration =
				(t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "FlowIdentifier") == 0) {
			param_buf->flowIdentifier = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "HardConstraint") == 0) {
			param_buf->hardConstraint = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TWTExponent") == 0) {
			param_buf->twtExponent = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TWTMantissa") == 0) {
			param_buf->twtMantissa = (t_u16)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}

	if (!cmd_found) {
		printf("Command %s not found in the config file!\n"
		       "Syntax: ./mlanutl mlanX twt_setup <config/twt.conf>\n",
		       (char *)argv[2]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	cmd->used_len = cmd_header_len + sizeof(twt_setup);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));

	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: twt_setup");
		fprintf(stderr, "mlanutl:twt_setup failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (line)
		free(line);
	if (config_file)
		fclose(config_file);
	if (cmd)
		free(cmd);
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief Process twt_teardown
 *  @param argc    number of arguments
 *  @param argv    A pointer to arguments array
 *  @return        MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_twt_teardown(int argc, char *argv[])
{
	twt_teardown *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = MLAN_STATUS_SUCCESS, cmd_found = 0, cmd_header_len =
		0;
	char *args[30], *pos = NULL;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX twt_teardown <config/twt.conf>\n");
		return MLAN_STATUS_FAILURE;
	}

	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);
	prepare_buffer(buffer, argv[2], 0, NULL);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR: Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = cmd_header_len;
	cmd->total_len = BUFFER_LENGTH;

	param_buf = (twt_teardown *) ((t_u8 *)buffer + cmd_header_len);

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Could not open Config file.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args, 30);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "FlowIdentifier") == 0) {
			param_buf->flowIdentifier = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "NegotiationType") == 0) {
			param_buf->negotiationType =
				(t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "TearDownAllTWT") == 0) {
			param_buf->teardownAllTWT = (t_u8)A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}

	if (!cmd_found) {
		printf("Command %s not found in the config file!\n"
		       "Syntax: ./mlanutl mlanX twt_teardown <config/twt.conf>\n",
		       (char *)argv[2]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	cmd->used_len = cmd_header_len + sizeof(twt_teardown);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));

	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: twt_teardown");
		fprintf(stderr, "mlanutl:twt_teardown failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (line)
		free(line);
	if (config_file)
		fclose(config_file);
	if (cmd)
		free(cmd);
	if (buffer)
		free(buffer);
	return ret;
}

/********************************************************
			Global Functions
********************************************************/

/**
 *  @brief Entry function for mlanutl
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
main(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;

	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		fprintf(stdout, "NXP mlanutl version %s\n", MLANUTL_VER);
		exit(0);
	}
	if (argc < 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		display_usage();
		exit(1);
	}

	strncpy(dev_name, argv[1], IFNAMSIZ - 1);

	/*
	 * Create a socket
	 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "mlanutl: Cannot open socket.\n");
		exit(1);
	}

	ret = process_command(argc, argv);

	if (ret == MLAN_STATUS_NOTFOUND) {
		ret = process_generic(argc, argv);

		if (ret) {
			fprintf(stderr, "Invalid command specified!\n");
			display_usage();
			ret = 1;
		}
	}

	close(sockfd);
	return ret;
}
