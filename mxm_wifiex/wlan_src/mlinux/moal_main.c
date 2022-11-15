/** @file moal_main.c
 *
 * @brief This file contains the major functions in WLAN
 * driver.
 *
 *
 * Copyright 2008-2022 NXP
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

/********************************************************
Change log:
    10/21/2008: initial version
********************************************************/

#include "moal_main.h"
#ifdef USB
#include "moal_usb.h"
#endif
#ifdef SDIO
#include "moal_sdio.h"
#endif
#ifdef PCIE
#include "moal_pcie.h"
#endif
#ifdef UAP_SUPPORT
#include "moal_uap.h"
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#include "moal_cfg80211.h"
#include "moal_cfg80211_util.h"
#endif
#ifdef STA_CFG80211
#ifdef STA_SUPPORT
#include "moal_sta_cfg80211.h"
#endif
#endif
#ifdef UAP_CFG80211
#ifdef UAP_SUPPORT
#include "moal_uap_cfg80211.h"
#endif
#endif
#include "moal_eth_ioctl.h"

#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <net/tcp.h>
#include <net/dsfield.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
#include <linux/mpls.h>
#endif
#include <linux/if_vlan.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
#include <net/addrconf.h>
#endif
#endif

/********************************************************
		 Global Variables
 ********************************************************/
/** the pointer of new fwdump fname for each dump**/
static char *fwdump_fname;
/** Semaphore for add/remove card */
struct semaphore AddRemoveCardSem;
/**
 * The global variable of a pointer to moal_handle
 * structure variable
 **/
moal_handle *m_handle[MAX_MLAN_ADAPTER];
static int reg_work;
/********************************************************
		Local Variables
********************************************************/

#ifdef SD8801
static struct _card_info card_info_SD8801 = {
	.embedded_supp = 0,
	.drcs = 0,
	.go_noa = 0,
	.v14_fw_api = 1,
	.v16_fw_api = 0,
	.v17_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 0,
	.low_power_enable = 1,
	.rx_rate_max = 76,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.fw_name = SD8801_DEFAULT_WLAN_FW_NAME,
	.fw_name_wlan = SD8801_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = 0,
	.dump_fw_ctrl_reg = 0x63,
	.dump_fw_start_reg = 0x64,
	.dump_fw_end_reg = 0x6A,
	.dump_fw_host_ready = 0xee,
	.dump_reg.reg_table = {0x28, 0x30, 0x34, 0x38, 0x3c},
	.dump_reg.reg_table_size = 5,
	.scratch_reg = 0x60,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x64,
	.fw_reset_val = 0,
	.slew_rate_reg = 0x8000231C,
	.slew_rate_bit_offset = 14,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 0,
};
#endif
#ifdef SD8887
static struct _card_info card_info_SD8887 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 1,
	.low_power_enable = 1,
	.rx_rate_max = 196,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.rev_id_reg = 0xc8,
	.fw_name = SD8887_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8887_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V2,
	.dump_fw_ctrl_reg = 0xa2,
	.dump_fw_start_reg = 0xa3,
	.dump_fw_end_reg = 0xaa,
	.dump_fw_host_ready = 0xee,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0x90,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0B6,
	.fw_reset_val = 1,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 0,
	.per_pkt_cfg_support = 0,
};
#endif

#ifdef SD8897
static struct _card_info card_info_SD8897 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0xbc,
	.fw_name = SD8897_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8897_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V2,
	.dump_fw_ctrl_reg = 0xe2,
	.dump_fw_start_reg = 0xe3,
	.dump_fw_end_reg = 0xea,
	.dump_fw_host_ready = 0xee,
	.dump_reg.reg_table = {0x4C, 0x50, 0x54, 0x55, 0x58, 0x59, 0x5c, 0x5d},
	.dump_reg.reg_table_size = 8,
	.scratch_reg = 0xc0,
	.func1_reg_start = 0x04,
	.func1_reg_end = 0x0b,
	.fw_reset_reg = 0x0E8,
	.fw_reset_val = 1,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 0,
	.per_pkt_cfg_support = 0,
};
#endif

#ifdef PCIE8897
static struct _card_info card_info_PCIE8897 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0x0c58,
	.fw_name = PCIE8897_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = PCIE8897_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 0,
	.per_pkt_cfg_support = 0,
};
#endif

#ifdef USB8897
static struct _card_info card_info_USB8897 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.fw_name = USB8897_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = USB8897_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 0,
	.per_pkt_cfg_support = 0,
};
#endif

#ifdef SD8977
static struct _card_info card_info_SD8977 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 76,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD8977_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8977_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD8978
static struct _card_info card_info_SD8978 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 76,
	.histogram_table_num = 1,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD8978_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8978_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD8997
static struct _card_info card_info_SD8997 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD8997_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8997_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD9098
static struct _card_info card_info_SD9098 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD9098_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD9098_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x90002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD9097
static struct _card_info card_info_SD9097 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD9097_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD9097_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x90002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SDNW62X
static struct _card_info card_info_SDNW62X = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SDNW62X_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SDNW62X_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x90002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD9177
static struct _card_info card_info_SD9177 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.rev_id_reg = 0xc8,
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.fw_name = SD9177_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD9177_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x90002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef PCIE8997
static struct _card_info card_info_PCIE8997 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0x8,
	.host_strap_reg = 0x0cd0,
	.magic_reg = 0x0cd4,
	.fw_name = PCIE8997_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = PCIE8997_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef PCIE9097
static struct _card_info card_info_PCIE9097 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0x8,
	.host_strap_reg = 0x1c70,
	.magic_reg = 0x1c74,
	.fw_name = PCIE9097_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = PCIE9097_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef PCIE9098
static struct _card_info card_info_PCIE9098 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0x8,
	.host_strap_reg = 0x1c70,
	.magic_reg = 0x1c74,
	.fw_name = PCIE9098_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = PCIE9098_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef PCIENW62X
static struct _card_info card_info_PCIENW62X = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.histogram_table_num = 3,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.rev_id_reg = 0x8,
	.host_strap_reg = 0x1c70,
	.magic_reg = 0x1c74,
	.fw_name = PCIENW62X_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = PCIENW62X_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef USB8801
static struct _card_info card_info_USB8801 = {
	.embedded_supp = 0,
	.drcs = 0,
	.go_noa = 0,
	.v14_fw_api = 1,
	.v16_fw_api = 0,
	.v17_fw_api = 0,
	.pmic = 0,
	.cal_data_cfg = 0,
	.low_power_enable = 1,
	.rx_rate_max = 76,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.histogram_table_num = 1,
	.fw_name = USB8801_DEFAULT_WLAN_FW_NAME,
	.fw_name_wlan = USB8801_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 0,
};
#endif

#ifdef USB8978
static struct _card_info card_info_USB8978 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 76,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.histogram_table_num = 1,
	.fw_name = USB8978_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = USB8978_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef USB8997
static struct _card_info card_info_USB8997 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 0,
	.rx_rate_max = 196,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.histogram_table_num = 3,
	.fw_name = USB8997_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = USB8997_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef USB9098
static struct _card_info card_info_USB9098 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.histogram_table_num = 3,
	.fw_name = USB9098_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = USB9098_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef USB9097
static struct _card_info card_info_USB9097 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.histogram_table_num = 3,
	.fw_name = USBUSB9097_COMBO_V1_FW_NAME,
	.fw_name_wlan = USB9097_WLAN_V1_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef USBNW62X
static struct _card_info card_info_USBNW62X = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 1,
	.v16_fw_api = 1,
	.v17_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 0,
	.low_power_enable = 0,
	.rx_rate_max = 412,
	.feature_control = FEATURE_CTRL_DEFAULT,
	.histogram_table_num = 3,
	.fw_name = USBNW62X_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = USBNW62X_DEFAULT_WLAN_FW_NAME,
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

#ifdef SD8987
static struct _card_info card_info_SD8987 = {
	.embedded_supp = 1,
	.drcs = 1,
	.go_noa = 0,
	.v16_fw_api = 1,
	.pmic = 1,
	.cal_data_cfg = 1,
	.low_power_enable = 1,
	.rx_rate_max = 196,
	.feature_control = FEATURE_CTRL_DEFAULT & (~FEATURE_CTRL_STREAM_2X2),
	.host_strap_reg = 0xf4,
	.magic_reg = 0xf0,
	.histogram_table_num = 3,
	.fw_name = SD8987_DEFAULT_COMBO_FW_NAME,
	.fw_name_wlan = SD8987_DEFAULT_WLAN_FW_NAME,
#ifdef SDIO
	.dump_fw_info = DUMP_FW_SDIO_V3,
	.dump_fw_ctrl_reg = 0xf9,
	.dump_fw_start_reg = 0xf1,
	.dump_fw_end_reg = 0xf8,
	.dump_fw_host_ready = 0xcc,
	.dump_reg.reg_table = {0x08, 0x58, 0x5C, 0x5D, 0x60, 0x61, 0x62, 0x64,
			       0x65, 0x66, 0x68, 0x69, 0x6a},
	.dump_reg.reg_table_size = 13,
	.scratch_reg = 0xe8,
	.func1_reg_start = 0x10,
	.func1_reg_end = 0x17,
	.fw_reset_reg = 0x0EE,
	.fw_reset_val = 0x99,
	.slew_rate_reg = 0x80002328,
	.slew_rate_bit_offset = 12,
#endif
	.sniffer_support = 1,
	.per_pkt_cfg_support = 1,
};
#endif

/** Driver version */
char driver_version[] =
	INTF_CARDTYPE KERN_VERSION "--" MLAN_RELEASE_VERSION "-GPL"
				   "-("
				   "FP" FPNUM ")"
#ifdef DEBUG_LEVEL2
				   "-dbg"
#endif
				   " ";

/** woal_callbacks */
static mlan_callbacks woal_callbacks = {
	.moal_get_fw_data = moal_get_fw_data,
	.moal_get_vdll_data = moal_get_vdll_data,
	.moal_get_hw_spec_complete = moal_get_hw_spec_complete,
	.moal_init_fw_complete = moal_init_fw_complete,
	.moal_shutdown_fw_complete = moal_shutdown_fw_complete,
	.moal_send_packet_complete = moal_send_packet_complete,
	.moal_recv_packet = moal_recv_packet,
	.moal_recv_amsdu_packet = moal_recv_amsdu_packet,
	.moal_recv_event = moal_recv_event,
	.moal_ioctl_complete = moal_ioctl_complete,
	.moal_alloc_mlan_buffer = moal_alloc_mlan_buffer,
	.moal_free_mlan_buffer = moal_free_mlan_buffer,
#ifdef USB
	.moal_recv_complete = moal_recv_complete,
	.moal_write_data_async = moal_write_data_async,
#endif /* USB */

#if defined(SDIO) || defined(PCIE)
	.moal_write_reg = moal_write_reg,
	.moal_read_reg = moal_read_reg,
#endif /* SDIO || PCIE */
	.moal_write_data_sync = moal_write_data_sync,
	.moal_read_data_sync = moal_read_data_sync,
	.moal_malloc = moal_malloc,
	.moal_mfree = moal_mfree,
	.moal_vmalloc = moal_vmalloc,
	.moal_vfree = moal_vfree,
#ifdef PCIE
	.moal_malloc_consistent = moal_malloc_consistent,
	.moal_mfree_consistent = moal_mfree_consistent,
	.moal_map_memory = moal_map_memory,
	.moal_unmap_memory = moal_unmap_memory,
#endif /* PCIE */
	.moal_memset = moal_memset,
	.moal_memcpy = moal_memcpy,
	.moal_memcpy_ext = moal_memcpy_ext,
	.moal_memmove = moal_memmove,
	.moal_memcmp = moal_memcmp,
	.moal_udelay = moal_udelay,
	.moal_usleep_range = moal_usleep_range,
	.moal_get_system_time = moal_get_system_time,
	.moal_get_boot_ktime = moal_get_boot_ktime,
	.moal_init_timer = moal_init_timer,
	.moal_free_timer = moal_free_timer,
	.moal_start_timer = moal_start_timer,
	.moal_stop_timer = moal_stop_timer,
	.moal_init_lock = moal_init_lock,
	.moal_free_lock = moal_free_lock,
	.moal_spin_lock = moal_spin_lock,
	.moal_spin_unlock = moal_spin_unlock,
	.moal_print = moal_print,
	.moal_print_netintf = moal_print_netintf,
	.moal_assert = moal_assert,
	.moal_hist_data_add = moal_hist_data_add,
	.moal_updata_peer_signal = moal_updata_peer_signal,
	.moal_do_div = moal_do_div,
#if defined(DRV_EMBEDDED_AUTHENTICATOR) || defined(DRV_EMBEDDED_SUPPLICANT)
	.moal_wait_hostcmd_complete = moal_wait_hostcmd_complete,
	.moal_notify_hostcmd_complete = moal_notify_hostcmd_complete,
#endif
	.moal_tp_accounting = moal_tp_accounting,
	.moal_tp_accounting_rx_param = moal_tp_accounting_rx_param,
	.moal_amsdu_tp_accounting = moal_amsdu_tp_accounting,
};

int woal_open(struct net_device *dev);
int woal_close(struct net_device *dev);
int woal_set_mac_address(struct net_device *dev, void *addr);
int woal_change_mtu(struct net_device *dev, int new_mtu);
void woal_tx_timeout(struct net_device *dev
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
		     ,
		     unsigned int txqueue
#endif
);
struct net_device_stats *woal_get_stats(struct net_device *dev);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 2, 0)
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb,
		      struct net_device *sb_dev);
#else
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb,
		      struct net_device *sb_dev,
		      select_queue_fallback_t fallback);
#endif
#else
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb,
		      void *accel_priv, select_queue_fallback_t fallback);
#endif
#else
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb,
		      void *accel_priv);
#endif
#else
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb);
#endif
#endif

static moal_handle *reset_handle;
/** Hang workqueue */
static struct workqueue_struct *hang_workqueue;
/** Hang work */
static struct work_struct hang_work;
/** register workqueue */
static struct workqueue_struct *register_workqueue;
/** register work */
static struct work_struct register_work;

/**
 *  @brief This function send fw dump event to kernel
 *
 *  @param priv       Pointer to structure moal_private
 *
 *  @return        N/A
 */
void woal_send_fw_dump_complete_event(moal_private *priv)
{
	int cfg80211_wext = priv->phandle->params.cfg80211_wext;
#ifdef STA_WEXT
	if (IS_STA_WEXT(cfg80211_wext))
		woal_send_iwevcustom_event(priv, CUS_EVT_FW_DUMP_DONE);
#endif
#ifdef STA_CFG80211
#if KERNEL_VERSION(3, 14, 0) <= CFG80211_VERSION_CODE
	if (IS_STA_CFG80211(cfg80211_wext))
		woal_cfg80211_vendor_event_fw_dump(priv);
#endif
#endif
	if (MLAN_STATUS_SUCCESS !=
	    woal_broadcast_event(priv, CUS_EVT_FW_DUMP_DONE,
				 strlen(CUS_EVT_FW_DUMP_DONE)))
		PRINTM(MINFO, "%s: woal_broadcast_event failed \n", __func__);

	return;
}

/**
 *  @brief This function process FW hang
 *
 *  @param handle       Pointer to structure moal_handle
 *
 *  @return        N/A
 */
static void woal_hang_work_queue(struct work_struct *work)
{
	int i;
	moal_private *priv;
	int cfg80211_wext = 0;
	int ret = 0;
	ENTER();
	if (!reset_handle) {
		LEAVE();
		return;
	}

	mlan_ioctl(reset_handle->pmlan_adapter, NULL);
	cfg80211_wext = reset_handle->params.cfg80211_wext;
	// stop pending scan
#ifdef STA_CFG80211
	if (IS_STA_CFG80211(cfg80211_wext) && reset_handle->scan_request &&
	    reset_handle->scan_priv) {
		moal_private *scan_priv = reset_handle->scan_priv;
		/** some supplicant can not handle SCAN abort event */
		if (scan_priv->bss_type == MLAN_BSS_TYPE_STA)
			woal_cfg80211_scan_done(reset_handle->scan_request,
						MTRUE);
		else
			woal_cfg80211_scan_done(reset_handle->scan_request,
						MFALSE);
		reset_handle->scan_request = NULL;
		reset_handle->scan_priv = NULL;
		cancel_delayed_work_sync(&reset_handle->scan_timeout_work);
		reset_handle->scan_pending_on_block = MFALSE;
		MOAL_REL_SEMAPHORE(&reset_handle->async_sem);
	}
#endif

	for (i = 0; i < reset_handle->priv_num; i++) {
		if (reset_handle->priv[i]) {
			priv = reset_handle->priv[i];
			woal_stop_queue(priv->netdev);
			if (netif_carrier_ok(priv->netdev))
				netif_carrier_off(priv->netdev);
			priv->media_connected = MFALSE;
			// disconnect
			moal_connection_status_check_pmqos(priv->phandle);
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
			if (IS_STA_CFG80211(cfg80211_wext) && priv->wdev &&
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || IMX_ANDROID_13)
			    priv->wdev->connected) {
#else
			    priv->wdev->current_bss) {
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
				if (priv->host_mlme)
					woal_host_mlme_disconnect(
						priv,
						MLAN_REASON_DEAUTH_LEAVING,
						NULL);
				else
#endif
					cfg80211_disconnected(priv->netdev, 0,
							      NULL, 0,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
							      true,
#endif
							      GFP_KERNEL);
			}
#endif
#endif
			// stop bgscan
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
			if (IS_STA_CFG80211(cfg80211_wext) &&
			    priv->sched_scanning) {
				priv->bg_scan_start = MFALSE;
				priv->bg_scan_reported = MFALSE;
				cfg80211_sched_scan_stopped(priv->wdev->wiphy
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
							    ,
							    priv->bg_scan_reqid
#endif
				);
				priv->sched_scanning = MFALSE;
			}
#endif
#endif
		}
	}
	woal_flush_workqueue(reset_handle);
	if (reset_handle->params.auto_fw_reload) {
		priv = woal_get_priv(reset_handle, MLAN_BSS_ROLE_ANY);
		if (priv) {
			woal_broadcast_event(priv, CUS_EVT_FW_RECOVER_START,
					     strlen(CUS_EVT_FW_RECOVER_START));
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (IS_STA_OR_UAP_CFG80211(cfg80211_wext))
				woal_cfg80211_vendor_event(
					priv, event_fw_reset_start,
					CUS_EVT_FW_RECOVER_START,
					strlen(CUS_EVT_FW_RECOVER_START));
#endif
#endif
		}
		if (IS_SD(reset_handle->card_type)) {
			PRINTM(MMSG, "WIFI auto_fw_reload: fw_reload=1\n");
			ret = woal_request_fw_reload(
				reset_handle, FW_RELOAD_SDIO_INBAND_RESET);
		}
#ifdef PCIE
		else if (IS_PCIE(reset_handle->card_type)) {
			reset_handle->init_wait_q_woken = MFALSE;
			PRINTM(MMSG, "WIFI auto_fw_reload: fw_reload=4\n");
			ret = woal_request_fw_reload(reset_handle,
						     FW_RELOAD_PCIE_RESET);
			if (!ret) {
				/* Wait for FLR to complete */
				wait_event_timeout(
					reset_handle->init_wait_q,
					reset_handle->init_wait_q_woken,
					10 * HZ);
				if (reset_handle->hardware_status !=
				    HardwareStatusReady)
					ret = -1;
			}
		}
#endif
		priv = woal_get_priv(reset_handle, MLAN_BSS_ROLE_ANY);
		if (priv) {
			if (ret) {
				woal_broadcast_event(
					priv, CUS_EVT_FW_RECOVER_FAIL,
					strlen(CUS_EVT_FW_RECOVER_FAIL));
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				if (IS_STA_OR_UAP_CFG80211(cfg80211_wext))
					woal_cfg80211_vendor_event(
						priv, event_fw_reset_failure,
						CUS_EVT_FW_RECOVER_FAIL,
						strlen(CUS_EVT_FW_RECOVER_FAIL));
#endif
#endif
			} else {
				woal_broadcast_event(
					priv, CUS_EVT_FW_RECOVER_SUCCESS,
					strlen(CUS_EVT_FW_RECOVER_SUCCESS));
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				if (IS_STA_OR_UAP_CFG80211(cfg80211_wext))
					woal_cfg80211_vendor_event(
						priv, event_fw_reset_success,
						CUS_EVT_FW_RECOVER_SUCCESS,
						strlen(CUS_EVT_FW_RECOVER_SUCCESS));
#endif
#endif
			}
		}
		reset_handle = NULL;
		LEAVE();
		return;
	}
	priv = woal_get_priv(reset_handle, MLAN_BSS_ROLE_ANY);
	if (priv) {
		woal_broadcast_event(priv, CUS_EVT_DRIVER_HANG,
				     strlen(CUS_EVT_DRIVER_HANG));
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext))
			woal_cfg80211_vendor_event(priv, event_hang,
						   CUS_EVT_DRIVER_HANG,
						   strlen(CUS_EVT_DRIVER_HANG));
#endif
#endif
	}
	reset_handle = NULL;
	LEAVE();
}

/**
 *  @brief This function process FW hang
 *
 *  @param handle       Pointer to structure moal_handle
 *
 *  @return        N/A
 */
void woal_process_hang(moal_handle *handle)
{
	ENTER();
	if (reset_handle == NULL) {
		PRINTM(MMSG, "Start to process hanging\n");
		reset_handle = handle;
		queue_work(hang_workqueue, &hang_work);
#ifdef ANDROID_KERNEL
#define WAKE_LOCK_HANG 5000
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
		__pm_wakeup_event(&reset_handle->ws, WAKE_LOCK_HANG);
#else
		wake_lock_timeout(&reset_handle->wake_lock,
				  msecs_to_jiffies(WAKE_LOCK_HANG));
#endif
#endif
	}
	LEAVE();
}

/**
 *  @brief Check if any interface is active
 *
 *  @param handle        A pointer to moal_handle
 *
 *
 *  @return              MTRUE/MFALSE;
 */
t_u8 woal_is_any_interface_active(moal_handle *handle)
{
	int i;
	for (i = 0; i < handle->priv_num; i++) {
		if (!handle->priv[i])
			continue;
#ifdef STA_SUPPORT
		if (GET_BSS_ROLE(handle->priv[i]) == MLAN_BSS_ROLE_STA) {
			if (handle->priv[i]->media_connected == MTRUE)
				return MTRUE;
		}
#endif
#ifdef UAP_SUPPORT
		if (GET_BSS_ROLE(handle->priv[i]) == MLAN_BSS_ROLE_UAP) {
			if (handle->priv[i]->bss_started == MTRUE)
				return MTRUE;
		}
#endif
	}
	return MFALSE;
}

#ifdef STA_CFG80211
/**  @brief This function set/clear pmk to FW
 *
 *  @param priv     A Pointer to the moal_private structure
 *  @param action     set/clear action
 *
 *  @return      0: success  fail otherwise
 */
int woal_set_clear_pmk(moal_private *priv, t_u8 action)
{
	mlan_ioctl_req *req;
	mlan_ds_sec_cfg *sec;
	mlan_status status;
	int ret = 0;
	t_u8 zero[MLAN_MAX_KEY_LENGTH] = {0};
	ENTER();

	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_sec_cfg));

	if (req == NULL) {
		ret = -ENOMEM;
	} else {
		sec = (mlan_ds_sec_cfg *)req->pbuf;
		sec->sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;
		req->req_id = MLAN_IOCTL_SEC_CFG;
		req->action = action;

		if (action == MLAN_ACT_SET) {
			sec->param.passphrase.psk_type = MLAN_PSK_PMK;
			if (memcmp(priv->pmk.pmk, zero, MLAN_MAX_KEY_LENGTH))
				moal_memcpy_ext(
					priv->phandle,
					&sec->param.passphrase.psk.pmk.pmk,
					priv->pmk.pmk, MLAN_MAX_KEY_LENGTH,
					sizeof(sec->param.passphrase.psk.pmk
						       .pmk));
			if (memcmp(priv->pmk.pmk_r0, zero,
				   MLAN_MAX_KEY_LENGTH) &&
			    memcmp(priv->pmk.pmk_r0_name, zero,
				   MLAN_MAX_PMKR0_NAME_LENGTH)) {
				moal_memcpy_ext(
					priv->phandle,
					&sec->param.passphrase.psk.pmk.pmk_r0,
					priv->pmk.pmk_r0, MLAN_MAX_KEY_LENGTH,
					sizeof(sec->param.passphrase.psk.pmk
						       .pmk_r0));
				moal_memcpy_ext(
					priv->phandle,
					&sec->param.passphrase.psk.pmk
						 .pmk_r0_name,
					priv->pmk.pmk_r0_name,
					MLAN_MAX_PMKR0_NAME_LENGTH,
					sizeof(sec->param.passphrase.psk.pmk
						       .pmk_r0_name));
			}
		}

		status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
		if (MLAN_STATUS_SUCCESS != status)
			ret = -EFAULT;
		if (status != MLAN_STATUS_PENDING)
			kfree(req);
	}

	LEAVE();
	return ret;
}
#endif

/**
 *  @brief This function handle the net interface ipaddr change event
 *
 *  @param nb      pointer to the notifier_block
 *  @param event   event type
 *  @param ptr     pointer to event struct
 *
 *  @return        NOTIFY_DONE or NOTIFY_OK
 */
static int woal_netdevice_event(struct notifier_block *nb, unsigned long event,
				void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *ndev;
	moal_private *priv;

	int ret = NOTIFY_OK;
#ifdef STA_CFG80211
	char rssi_low[11];
#endif
	ENTER();

	ndev = ifa->ifa_dev->dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	if (!ndev || ndev->netdev_ops->ndo_open != woal_open)
#else
	if (!ndev || ndev->open != woal_open)
#endif
	{
		PRINTM(MIOCTL, "IP changes not for us, ignore. ndev[%p]\n",
		       ndev);
		if (ndev)
			PRINTM(MIOCTL, "changes on %s\n", ndev->name);
		ret = NOTIFY_DONE;
		goto done;
	}
	priv = (moal_private *)netdev_priv(ndev);
	if (priv->bss_type != MLAN_BSS_TYPE_STA
#ifdef WIFI_DIRECT_SUPPORT
	    && priv->bss_type != MLAN_BSS_TYPE_WIFIDIRECT
#endif
	) {
		PRINTM(MIOCTL, "Bss type [%d] is not STA/P2P, ignore\n",
		       (int)priv->bss_type);
		ret = NOTIFY_DONE;
		goto done;
	}

	switch (event) {
	case NETDEV_UP:
		PRINTM(MIOCTL, "[%s]: New ip addr: 0x%08x\n", ndev->name,
		       ifa->ifa_address);
		/* Save the IP addr now */
		moal_memcpy_ext(priv->phandle, priv->ip_addr, &ifa->ifa_address,
				sizeof(ifa->ifa_address),
				sizeof(priv->ip_addr));
		priv->ip_addr_type = IPADDR_TYPE_IPV4;
#ifdef STA_CFG80211
		if (!moal_extflg_isset(priv->phandle, EXT_HW_TEST) &&
		    priv->roaming_enabled) {
			snprintf(rssi_low, sizeof(rssi_low), "%d",
				 priv->rssi_low);
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_rssi_low_threshold(priv, rssi_low,
							MOAL_IOCTL_WAIT)) {
				PRINTM(MERROR,
				       "%s: woal_set_rssi_low_threshold failed \n",
				       __func__);
				goto done;
			}
		}
#endif
#ifdef STA_CFG80211
		if (priv->phandle->fw_roam_enable &&
		    (priv->phandle->fw_roam_enable != AUTO_RECONNECT) &&
		    !moal_extflg_isset(priv->phandle, EXT_ROAMOFFLOAD_IN_HS)) {
			snprintf(rssi_low, sizeof(rssi_low), "%d",
				 priv->rssi_low);
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_rssi_low_threshold(priv, rssi_low,
							MOAL_IOCTL_WAIT)) {
				PRINTM(MERROR,
				       "%s: woal_set_rssi_low_threshold failed \n",
				       __func__);
				goto done;
			}
			if (priv->pmk_saved) {
				woal_set_clear_pmk(priv, MLAN_ACT_SET);
				priv->pmk_saved = false;
			}
		}
#endif
		break;
	case NETDEV_DOWN:
		PRINTM(MIOCTL, "[%s]: Ip addr removed.\n", ndev->name);
		priv->ip_addr_type = IPADDR_TYPE_NONE;
		memset(priv->ip_addr, 0, sizeof(priv->ip_addr));
		break;
	default:
		PRINTM(MIOCTL, "[%s]: Ignore event: %u\n", ndev->name,
		       (unsigned int)event);
		ret = NOTIFY_DONE;
		goto done;
	}

done:
	LEAVE();
	return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
/**
 *  @brief This function handle the net interface ipv6 address change event
 *
 *  @param nb      pointer to the notifier_block
 *  @param event   event type
 *  @param ptr     pointer to event struct
 *
 *  @return        NOTIFY_DONE or NOTIFY_OK
 */
static int woal_inet6_netdeive_event(struct notifier_block *nb,
				     unsigned long event, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *ndev = ifa->idev->dev;
	moal_private *priv;
	int ret = NOTIFY_OK;

	ENTER();

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	if (!ndev || ndev->netdev_ops->ndo_open != woal_open)
#else
	if (!ndev || ndev->open != woal_open)
#endif
	{
		PRINTM(MIOCTL, "IPV6 changes not for us, ignore. ndev[%p]\n",
		       ndev);
		if (ndev)
			PRINTM(MIOCTL, "changes on %s\n", ndev->name);
		ret = NOTIFY_DONE;
		goto done;
	}
	priv = (moal_private *)netdev_priv(ndev);
	if (!priv) {
		PRINTM(MERROR, "Invalid private structure\n");
		goto done;
	}
	if (priv->bss_type != MLAN_BSS_TYPE_STA
#ifdef WIFI_DIRECT_SUPPORT
	    && priv->bss_type != MLAN_BSS_TYPE_WIFIDIRECT
#endif
	) {
		PRINTM(MIOCTL, "Bss type [%d] is not STA/P2P, ignore\n",
		       (int)priv->bss_type);
		ret = NOTIFY_DONE;
		goto done;
	}

	switch (event) {
	case NETDEV_UP:
		PRINTM(MIOCTL, "[%s]: New ipv6 addr\n", ndev->name);
		moal_memcpy_ext(priv->phandle, priv->ipv6_addr,
				(t_u8 *)&ifa->addr, sizeof(priv->ipv6_addr),
				sizeof(priv->ipv6_addr));
		priv->ipv6_addr_configured = MTRUE;
		break;
	case NETDEV_DOWN:
		PRINTM(MIOCTL, "[%s]: Ipv6 addr removed.\n", ndev->name);
		memset(priv->ipv6_addr, 0, sizeof(priv->ipv6_addr));
		priv->ipv6_addr_configured = MFALSE;
		break;
	default:
		PRINTM(MIOCTL, "[%s]: Ignore event: %u\n", ndev->name,
		       (unsigned int)event);
		ret = NOTIFY_DONE;
		goto done;
	}
done:
	LEAVE();
	return ret;
}
#endif
#endif

/**
 *  @brief This function validates a SSID as being able to be printed
 *
 *  @param pssid   SSID structure to validate
 *
 *  @return        MTRUE or MFALSE
 */
BOOLEAN woal_ssid_valid(mlan_802_11_ssid *pssid)
{
#ifdef ASCII_SSID_CHECK
	unsigned int ssid_idx;

	ENTER();

	for (ssid_idx = 0; ssid_idx < pssid->ssid_len; ssid_idx++) {
		if ((pssid->ssid[ssid_idx] < 0x20) ||
		    (pssid->ssid[ssid_idx] > 0x7e)) {
			LEAVE();
			return MFALSE;
		}
	}
	LEAVE();
#endif
	return MTRUE;
}

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
/**
 *  @brief Remain on Channel timeout function
 *
 *  @param context  A pointer to context
 *  @return         N/A
 */
void woal_remain_timer_func(void *context)
{
	moal_handle *handle = (moal_handle *)context;
	moal_private *priv = handle->priv[handle->remain_bss_index];

	ENTER();

	PRINTM(MEVENT, "remain_timer fired.\n");
	if (handle->cookie) {
		cfg80211_remain_on_channel_expired(
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
			priv->netdev,
#else
			priv->wdev,
#endif
			handle->cookie, &handle->chan,
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
			handle->channel_type,
#endif
			GFP_ATOMIC);
		handle->cookie = 0;
	}
	handle->is_remain_timer_set = MFALSE;

	LEAVE();
	return;
}
#endif
#endif

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
/**
 *  @brief GO timeout function
 *
 *  @param context  A pointer to context
 *  @return         N/A
 */
void woal_go_timer_func(void *context)
{
	moal_handle *handle = (moal_handle *)context;

	ENTER();

	PRINTM(MEVENT, "go_timer fired.\n");
	handle->is_go_timer_set = MFALSE;

	LEAVE();
	return;
}

#endif
#endif

/**
 *  @brief check if we already connect to the AP.
 *  @param priv         A pointer to moal_private structure
 *  @param ssid_bssid   A pointer to mlan_ssid_bssid structure
 *
 *  @return             MTRUE/MFALSE;
 */
int woal_is_connected(moal_private *priv, mlan_ssid_bssid *ssid_bssid)
{
	mlan_bss_info bss_info;
	int ret = MFALSE;
	t_u8 zero_mac[] = {0, 0, 0, 0, 0, 0};
	ENTER();
	memset(&bss_info, 0, sizeof(bss_info));
	if (MLAN_STATUS_SUCCESS !=
	    woal_get_bss_info(priv, MOAL_IOCTL_WAIT, &bss_info))
		goto done;
	if (bss_info.media_connected) {
		if (memcmp(ssid_bssid->bssid, zero_mac, sizeof(zero_mac))) {
			if (ssid_bssid->ssid.ssid_len) { /* compare ssid and
							    bssid */
				if ((ssid_bssid->ssid.ssid_len ==
				     bss_info.ssid.ssid_len) &&
				    !memcmp(ssid_bssid->ssid.ssid,
					    bss_info.ssid.ssid,
					    bss_info.ssid.ssid_len) &&
				    !memcmp(ssid_bssid->bssid, bss_info.bssid,
					    MLAN_MAC_ADDR_LENGTH))
					ret = MTRUE;
			} else { /* compare bssid */
				if (!memcmp(ssid_bssid->bssid, bss_info.bssid,
					    MLAN_MAC_ADDR_LENGTH)) {
					moal_memcpy_ext(
						priv->phandle,
						&ssid_bssid->ssid,
						&bss_info.ssid,
						sizeof(bss_info.ssid),
						sizeof(ssid_bssid->ssid));
					ret = MTRUE;
				}
			}
		} else { /* compare ssid */
			if (ssid_bssid->ssid.ssid_len &&
			    (ssid_bssid->ssid.ssid_len ==
			     bss_info.ssid.ssid_len) &&
			    !memcmp(ssid_bssid->ssid.ssid, bss_info.ssid.ssid,
				    bss_info.ssid.ssid_len)) {
				moal_memcpy_ext(priv->phandle,
						&ssid_bssid->bssid,
						&bss_info.bssid,
						MLAN_MAC_ADDR_LENGTH,
						sizeof(ssid_bssid->bssid));
				ret = MTRUE;
			}
		}
	}
done:
	LEAVE();
	return ret;
}

/**
 * @brief Look up specific IE in a buf
 *
 * @param ie              Pointer to IEs
 * @param len             Total length of ie
 * @param id              Element id to lookup
 *
 * @return                Pointer of the specific IE -- success, NULL -- fail
 */
const t_u8 *woal_parse_ie_tlv(const t_u8 *ie, int len, t_u8 id)
{
	int left_len = len;
	const t_u8 *pos = ie;
	int length;

	/* IE format:
	 * |   u8  |   id   |
	 * |   u8  |   len  |
	 * |   var |   data |
	 */
	while (left_len >= 2) {
		length = *(pos + 1);
		if ((*pos == id) && (length + 2) <= left_len)
			return pos;
		pos += (length + 2);
		left_len -= (length + 2);
	}

	return NULL;
}

/**
 * @brief Look up specific IE in Extension IE
 *
 * @param ie              Pointer to IEs
 * @param len             Total length of ie
 * @param ext_id         Extended Element id to lookup
 *
 * @return                Pointer of the specific Extended IE -- success, NULL
 * -- fail
 */
const t_u8 *woal_parse_ext_ie_tlv(const t_u8 *ie, int len, t_u8 ext_id)
{
	int left_len = len;
	const t_u8 *pos = ie;
	int length;

	/* Extension IE format:
	 * |   u8  |   id   |
	 * |   u8  |   len  |
	 * |   u8  |   ext_id |
	 * |   var |   data |
	 */
	while (left_len >= 2) {
		length = *(pos + 1);
		if ((*pos == EXTENSION) && (length + 2) <= left_len) {
			if (*(pos + 2) == ext_id)
				return pos;
		}
		pos += (length + 2);
		left_len -= (length + 2);
	}
	return NULL;
}

/**
 *  @brief Get mode
 *
 *  @param priv          A pointer to moal_private structure
 *  @param wait_option   Wait option (MOAL_WAIT or MOAL_NO_WAIT)
 *
 *  @return              Wireless mode
 */
t_u32 woal_get_mode(moal_private *priv, t_u8 wait_option)
{
	mlan_ds_bss *bss = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_u32 mode = 0;

	ENTER();

#if defined(STA_WEXT) || defined(UAP_WEXT)
	mode = priv->w_stats.status;
#endif
	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_bss));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	bss = (mlan_ds_bss *)req->pbuf;
	bss->sub_command = MLAN_OID_BSS_MODE;
	req->req_id = MLAN_IOCTL_BSS;
	req->action = MLAN_ACT_GET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
	if (status == MLAN_STATUS_SUCCESS) {
		switch (bss->param.bss_mode) {
		case MLAN_BSS_MODE_INFRA:
			mode = MW_MODE_INFRA;
			break;
		case MLAN_BSS_MODE_IBSS:
			mode = MW_MODE_ADHOC;
			break;
		default:
			mode = MW_MODE_AUTO;
			break;
		}
	}
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return mode;
}

/********************************************************
		Local Functions
********************************************************/
/**
 *  @brief This function update the default firmware name
 *
 *  @param handle           A pointer to moal_handle structure
 *
 *  @return        N/A
 */
void woal_update_firmware_name(moal_handle *handle)
{
	if (handle->params.fw_name) {
		handle->drv_mode.fw_name = handle->params.fw_name;
	} else {
		if (!moal_extflg_isset(handle, EXT_FW_SERIAL) ||
		    handle->fw_reload || handle->params.fw_reload) {
			handle->drv_mode.fw_name =
				handle->card_info->fw_name_wlan;
		} else
			handle->drv_mode.fw_name = handle->card_info->fw_name;
	}
}
/**
 *  @brief This function dynamically populates the driver mode table
 *
 *  @param handle           A pointer to moal_handle structure
 *  @param drv_mode_local   Driver mode
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_update_drv_tbl(moal_handle *handle, int drv_mode_local)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	unsigned int intf_num = 0;
	int i = 0, j = 0;
	mlan_bss_attr *bss_tbl = NULL;
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	int last_wfd_index = 0;
	int max_vir_bss = handle->params.max_vir_bss;
#endif
#endif
#ifdef STA_SUPPORT
	int max_sta_bss = handle->params.max_sta_bss;
#endif
#ifdef UAP_SUPPORT
	int max_uap_bss = handle->params.max_uap_bss;
#endif
#ifdef WIFI_DIRECT_SUPPORT
	int max_wfd_bss = handle->params.max_wfd_bss;
#endif
	int max_dfs_bss = MAX_DFS_BSS;

	ENTER();

	/* Calculate number of interfaces */
#ifdef STA_SUPPORT
	if (drv_mode_local & DRV_MODE_STA) {
		if ((max_sta_bss < 1) || (max_sta_bss > MAX_STA_BSS)) {
			PRINTM(MWARN,
			       "Unsupported max_sta_bss (%d), setting to default\n",
			       max_sta_bss);
			max_sta_bss = DEF_STA_BSS;
		}
		intf_num += max_sta_bss;
		handle->params.max_sta_bss = max_sta_bss;
	}
#endif /* STA_SUPPORT */

#ifdef UAP_SUPPORT
	if (drv_mode_local & DRV_MODE_UAP) {
		if ((max_uap_bss < 1) || (max_uap_bss > MAX_UAP_BSS)) {
			PRINTM(MWARN,
			       "Unsupported max_uap_bss (%d), setting to default\n",
			       max_uap_bss);
			max_uap_bss = DEF_UAP_BSS;
		}
		intf_num += max_uap_bss;
		handle->params.max_uap_bss = max_uap_bss;
	}
#endif /* UAP_SUPPORT */

#ifdef WIFI_DIRECT_SUPPORT
	if (drv_mode_local & DRV_MODE_WIFIDIRECT) {
		if ((max_wfd_bss < 1) || (max_wfd_bss > MAX_WIFIDIRECT_BSS)) {
			PRINTM(MWARN,
			       "Unsupported max_wfd_bss (%d), setting to default\n",
			       max_wfd_bss);
			max_wfd_bss = DEF_WIFIDIRECT_BSS;
		}
		intf_num += max_wfd_bss;
		handle->params.max_wfd_bss = max_wfd_bss;
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
		intf_num += max_vir_bss;
#endif
	}

#endif /* WIFI_DIRECT_SUPPORT */

	if (drv_mode_local & DRV_MODE_DFS)
		intf_num += max_dfs_bss;
	/* Create BSS attribute table */
	if ((intf_num == 0) || (intf_num > MLAN_MAX_BSS_NUM)) {
		PRINTM(MERROR, "Unsupported number of BSS %d\n", intf_num);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	} else {
		/* Create new table */
		bss_tbl = kmalloc(sizeof(mlan_bss_attr) * intf_num, GFP_KERNEL);
		if (!bss_tbl) {
			PRINTM(MERROR,
			       "Could not create BSS attribute table\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}

	/* Populate BSS attribute table */
#ifdef STA_SUPPORT
	if (drv_mode_local & DRV_MODE_STA) {
		for (j = 0; j < max_sta_bss; j++) {
			if (i >= (int)intf_num)
				break;
			bss_tbl[i].bss_type = MLAN_BSS_TYPE_STA;
			bss_tbl[i].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
			bss_tbl[i].active = MTRUE;
			bss_tbl[i].bss_priority = 0;
			bss_tbl[i].bss_num = j;
			bss_tbl[i].bss_virtual = MFALSE;
			i++;
		}
	}
#endif /* STA_SUPPORT */

#ifdef UAP_SUPPORT
	if (drv_mode_local & DRV_MODE_UAP) {
		for (j = 0; j < max_uap_bss; j++) {
			if (i >= (int)intf_num)
				break;
			bss_tbl[i].bss_type = MLAN_BSS_TYPE_UAP;
			bss_tbl[i].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
			bss_tbl[i].active = MTRUE;
			bss_tbl[i].bss_priority = 0;
			bss_tbl[i].bss_num = j;
			bss_tbl[i].bss_virtual = MFALSE;
			i++;
		}
	}
#endif /* UAP_SUPPORT */

#ifdef WIFI_DIRECT_SUPPORT
	if (drv_mode_local & DRV_MODE_WIFIDIRECT) {
		for (j = 0; j < max_wfd_bss; j++) {
			if (i >= (int)intf_num)
				break;
			bss_tbl[i].bss_type = MLAN_BSS_TYPE_WIFIDIRECT;
			bss_tbl[i].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
			bss_tbl[i].active = MTRUE;
			bss_tbl[i].bss_priority = 0;
			bss_tbl[i].bss_num = j;
			bss_tbl[i].bss_virtual = MFALSE;
			i++;
		}
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
		last_wfd_index = j;
#endif
	}
#endif /* WIFI_DIRECT_SUPPORT */

	if (drv_mode_local & DRV_MODE_DFS) {
		for (j = 0; j < max_dfs_bss; j++) {
			if (i >= (int)intf_num)
				break;
			bss_tbl[i].bss_type = MLAN_BSS_TYPE_DFS;
			bss_tbl[i].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
			bss_tbl[i].active = MTRUE;
			bss_tbl[i].bss_priority = 0;
			bss_tbl[i].bss_num = j;
			bss_tbl[i].bss_virtual = MFALSE;
			i++;
		}
	}

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	/** append virtual interface at the end of table */
	for (j = 0; j < max_vir_bss; j++) {
		if (i >= (int)intf_num)
			break;
		bss_tbl[i].bss_type = MLAN_BSS_TYPE_WIFIDIRECT;
		bss_tbl[i].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
		bss_tbl[i].active = MTRUE;
		bss_tbl[i].bss_priority = 0;
		bss_tbl[i].bss_num = j + last_wfd_index;
		bss_tbl[i].bss_virtual = MTRUE;
		i++;
	}
#endif
#endif

	/* Clear existing table, if any */
	kfree(handle->drv_mode.bss_attr);
	handle->drv_mode.bss_attr = NULL;

	/* Create moal_drv_mode entry */
	handle->drv_mode.drv_mode = handle->params.drv_mode;
	handle->drv_mode.intf_num = intf_num;
	handle->drv_mode.bss_attr = bss_tbl;

	/* update default firmware name */
	woal_update_firmware_name(handle);
done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function initializes software
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_init_sw(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	unsigned int i;
	mlan_device device;
	t_void *pmlan;
	int cfg80211_wext = handle->params.cfg80211_wext;

	ENTER();

	/* Initialize moal_handle structure */
	handle->hardware_status = HardwareStatusInitializing;
	handle->main_state = MOAL_STATE_IDLE;

#ifdef STA_SUPPORT
	if ((handle->params.drv_mode & DRV_MODE_STA)
#ifdef STA_WEXT
	    && !IS_STA_WEXT(cfg80211_wext)
#endif
#ifdef STA_CFG80211
	    && !IS_STA_CFG80211(cfg80211_wext)
#endif
#ifdef MFG_CMD_SUPPORT
	    && !handle->params.mfg_mode
#endif
	) {
		PRINTM(MERROR,
		       "STA without WEXT or CFG80211 bit definition!\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
#endif /* STA_SUPPORT */

#if defined(STA_CFG80211) && defined(STA_SUPPORT)
	if (IS_STA_CFG80211(cfg80211_wext))
		cfg80211_wext |= STA_CFG80211_MASK | UAP_CFG80211_MASK;
#endif

#if defined(UAP_CFG80211) && defined(UAP_SUPPORT)
	if (IS_UAP_CFG80211(cfg80211_wext))
		cfg80211_wext |= STA_CFG80211_MASK | UAP_CFG80211_MASK;
#endif
	handle->params.cfg80211_wext = cfg80211_wext;
	moal_memcpy_ext(handle, handle->driver_version, driver_version,
			strlen(driver_version), MLAN_MAX_VER_STR_LEN - 1);

	if (woal_update_drv_tbl(handle, handle->params.drv_mode) !=
	    MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Could not update driver mode table\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	/** user config file */
	init_waitqueue_head(&handle->init_user_conf_wait_q);

	/* PnP and power profile */
	handle->surprise_removed = MFALSE;
	init_waitqueue_head(&handle->init_wait_q);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	spin_lock_init(&handle->queue_lock);
#endif
	spin_lock_init(&handle->driver_lock);
	spin_lock_init(&handle->ioctl_lock);
	spin_lock_init(&handle->scan_req_lock);

	handle->is_suspended = MFALSE;
	handle->hs_activated = MFALSE;
	handle->hs_auto_arp = MTRUE;
	handle->suspend_fail = MFALSE;
	handle->hs_skip_count = 0;
	handle->hs_force_count = 0;
#ifdef SDIO
	if (IS_SD(handle->card_type)) {
#ifdef SDIO_SUSPEND_RESUME
		handle->suspend_notify_req = MFALSE;
#endif
		handle->cmd52_func = 0;
		handle->cmd52_reg = 0;
		handle->cmd52_val = 0;
	}
#endif

	if (handle->params.scan_chan_gap &&
	    (handle->params.scan_chan_gap <= MRVDRV_MAX_SCAN_CHAN_GAP_TIME))
		handle->scan_chan_gap = handle->params.scan_chan_gap;
	else
		handle->scan_chan_gap = DEF_SCAN_CHAN_GAP;

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#ifdef WIFI_DIRECT_SUPPORT
	handle->miracast_scan_time = DEF_MIRACAST_SCAN_TIME;
#define DEF_NOA_DURATION 0
#define DEF_NOA_INTERVAL 100
	handle->noa_duration = DEF_NOA_DURATION;
	handle->noa_interval = DEF_NOA_INTERVAL;
#endif
#endif

#ifdef STA_CFG80211
	handle->scan_timeout = SCAN_TIMEOUT_25S;
#endif
	if (IS_USB(handle->card_type))
		init_waitqueue_head(&handle->suspend_wait_q);
	init_waitqueue_head(&handle->hs_activate_wait_q);

	/* Initialize measurement wait queue */
	handle->meas_wait_q_woken = MFALSE;
	handle->meas_start_jiffies = 0;
	handle->cac_period = MFALSE;
	handle->delay_bss_start = MFALSE;
	init_waitqueue_head(&handle->meas_wait_q);
#if defined(UAP_SUPPORT)
	handle->chsw_wait_q_woken = MFALSE;
	init_waitqueue_head(&handle->chsw_wait_q);
#endif

	handle->cac_period_jiffies = 0;
	handle->usr_nop_period_sec = 0;
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
	memset(&handle->dfs_channel, 0, sizeof(struct cfg80211_chan_def));
	woal_initialize_timer(&handle->cac_timer, woal_cac_timer_func, handle);
	handle->is_cac_timer_set = MFALSE;
	handle->cac_bss_index = 0xff;
#endif
#endif
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	handle->mon_if = NULL;
#endif

#ifdef REASSOCIATION
	MOAL_INIT_SEMAPHORE(&handle->reassoc_sem);
	handle->reassoc_on = 0;

	/* Initialize the timer for the reassociation */
	woal_initialize_timer(&handle->reassoc_timer, woal_reassoc_timer_func,
			      handle);

	handle->is_reassoc_timer_set = MFALSE;
#endif /* REASSOCIATION */

	/* Initialize the timer for the FW dump*/
	woal_initialize_timer(&handle->fw_dump_timer, woal_fw_dump_timer_func,
			      handle);
	handle->is_fw_dump_timer_set = MFALSE;

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	/* Initialize the timer for GO timeout */
	woal_initialize_timer(&handle->go_timer, woal_go_timer_func, handle);

	handle->is_go_timer_set = MFALSE;
#endif
#endif

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
	handle->remain_on_channel = MFALSE;

	/* Initialize the timer for remain on channel */
	woal_initialize_timer(&handle->remain_timer, woal_remain_timer_func,
			      handle);

	handle->is_remain_timer_set = MFALSE;
#endif
#endif

	/* Register to MLAN */
	memset(&device, 0, sizeof(mlan_device));
	device.pmoal_handle = handle;
	device.card_type = handle->card_type;
	device.card_rev = handle->card_rev;
#ifdef USB
	if (IS_USB(handle->card_type)) {
		struct usb_card_rec *cardp = handle->card;
		device.tx_cmd_ep = cardp->tx_cmd_ep;
		device.rx_cmd_ep = cardp->rx_cmd_ep;
		device.tx_data_ep = cardp->tx_data_ep;
		device.rx_data_ep = cardp->rx_data_ep;
	}
#endif
#ifdef MFG_CMD_SUPPORT
	device.mfg_mode = (t_u32)handle->params.mfg_mode;
#endif
#ifdef DEBUG_LEVEL1
	device.drvdbg = drvdbg;
#endif
	device.fixed_beacon_buffer =
		(t_u32)moal_extflg_isset(handle, EXT_FIX_BCN_BUF);
	device.auto_ds = (t_u32)handle->params.auto_ds;
	device.ext_scan = (t_u8)handle->params.ext_scan;
	device.ps_mode = (t_u32)handle->params.ps_mode;
	device.passive_to_active_scan = (t_u8)handle->params.p2a_scan;
	device.max_tx_buf = (t_u32)handle->params.max_tx_buf;
#if defined(STA_SUPPORT)
	device.cfg_11d = (t_u32)handle->params.cfg_11d;
#endif
	device.indrstcfg = (t_u32)handle->params.indrstcfg;
#ifdef PCIE
	if (IS_PCIE(handle->card_type))
		device.ring_size = handle->params.ring_size;
#endif
#ifdef SDIO
	if (IS_SD(handle->card_type)) {
		device.sdio_rx_aggr_enable =
			moal_extflg_isset(handle, EXT_SDIO_RX_AGGR);
		device.int_mode = (t_u32)moal_extflg_isset(handle, EXT_INTMODE);
		device.gpio_pin = (t_u32)handle->params.gpiopin;
#ifdef SDIO_MMC
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 36)
		device.max_segs = ((struct sdio_mmc_card *)handle->card)
					  ->func->card->host->max_segs;
		device.max_seg_size = ((struct sdio_mmc_card *)handle->card)
					      ->func->card->host->max_seg_size;
#endif
		PRINTM(MMSG, "SDIO: max_segs=%d max_seg_size=%d\n",
		       device.max_segs, device.max_seg_size);
#ifdef MMC_QUIRK_BLKSZ_FOR_BYTE_MODE
		device.mpa_tx_cfg = MLAN_INIT_PARA_ENABLED;
		device.mpa_rx_cfg = MLAN_INIT_PARA_ENABLED;
#else
		device.mpa_tx_cfg = MLAN_INIT_PARA_DISABLED;
		device.mpa_rx_cfg = MLAN_INIT_PARA_DISABLED;
#endif
#else
		device.mpa_tx_cfg = MLAN_INIT_PARA_ENABLED;
		device.mpa_rx_cfg = MLAN_INIT_PARA_ENABLED;
#endif /* SDIO_MMC */
	}
#endif /* SDIO */
	device.feature_control = handle->card_info->feature_control;
	handle->feature_control = device.feature_control;

	if (handle->params.rx_work == MLAN_INIT_PARA_ENABLED)
		device.rx_work = MTRUE;
	else if (handle->params.rx_work == MLAN_INIT_PARA_DISABLED)
		device.rx_work = MFALSE;
	else {
		if (num_possible_cpus() > 1)
			device.rx_work = MTRUE;
		else
			device.rx_work = MFALSE;
	}
	PRINTM(MMSG, "rx_work=%d cpu_num=%d\n", device.rx_work,
	       num_possible_cpus());
	if (moal_extflg_isset(handle, EXT_NAPI))
		device.rx_work = MTRUE;

	device.dev_cap_mask = handle->params.dev_cap_mask;

	device.multi_dtim = handle->params.multi_dtim;

	device.inact_tmo = handle->params.inact_tmo;
#ifdef UAP_SUPPORT
	device.uap_max_sta = handle->params.uap_max_sta;
#endif
	device.mcs32 = handle->params.mcs32;
	device.hs_wake_interval = handle->params.hs_wake_interval;
	device.indication_gpio = handle->params.indication_gpio;
	device.hs_mimo_switch = moal_extflg_isset(handle, EXT_HS_MIMO_SWITCH);

	device.dfs53cfg = handle->params.dfs53cfg;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	device.dfs_offload = moal_extflg_isset(handle, EXT_DFS_OFFLOAD);
#endif

	for (i = 0; i < handle->drv_mode.intf_num; i++) {
		device.bss_attr[i].bss_type =
			handle->drv_mode.bss_attr[i].bss_type;
		device.bss_attr[i].frame_type =
			handle->drv_mode.bss_attr[i].frame_type;
		device.bss_attr[i].active = handle->drv_mode.bss_attr[i].active;
		device.bss_attr[i].bss_priority =
			handle->drv_mode.bss_attr[i].bss_priority;
		device.bss_attr[i].bss_num =
			handle->drv_mode.bss_attr[i].bss_num;
		device.bss_attr[i].bss_virtual =
			handle->drv_mode.bss_attr[i].bss_virtual;
	}
	moal_memcpy_ext(handle, &device.callbacks, &woal_callbacks,
			sizeof(mlan_callbacks), sizeof(mlan_callbacks));
	if (!handle->params.amsdu_deaggr)
		device.callbacks.moal_recv_amsdu_packet = NULL;
	device.drv_mode = handle->params.drv_mode;
	if (MLAN_STATUS_SUCCESS == mlan_register(&device, &pmlan))
		handle->pmlan_adapter = pmlan;
	else
		ret = MLAN_STATUS_FAILURE;

	LEAVE();
	return ret;
}

/**
 *  @brief This function frees the structure of moal_handle
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return         N/A
 */
void woal_free_moal_handle(moal_handle *handle)
{
	moal_handle *ref_handle = NULL;

	ENTER();
	if (!handle) {
		PRINTM(MERROR, "The handle is NULL\n");
		LEAVE();
		return;
	}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	/* Unregister wiphy device and free */
	if (handle->wiphy) {
		wiphy_unregister(handle->wiphy);
		woal_cfg80211_free_bands(handle->wiphy);
		wiphy_free(handle->wiphy);
		handle->wiphy = NULL;
	}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
	if ((handle->nl_sk) && ((handle->nl_sk)->sk_socket)) {
		sock_release((handle->nl_sk)->sk_socket);
		handle->nl_sk = NULL;
	}
#else
	netlink_kernel_release(handle->nl_sk);
#endif

	if (handle->pmlan_adapter) {
		mlan_unregister(handle->pmlan_adapter);
		handle->pmlan_adapter = NULL;
	}

	/* Free BSS attribute table */
	kfree(handle->drv_mode.bss_attr);
	handle->drv_mode.bss_attr = NULL;
	PRINTM(MINFO, "Free Adapter\n");
	if (atomic_read(&handle->lock_count) ||
	    atomic_read(&handle->malloc_count) ||
	    atomic_read(&handle->mbufalloc_count)) {
		PRINTM(MERROR,
		       "mlan has memory leak: lock_count=%d, malloc_count=%d, mbufalloc_count=%d\n",
		       atomic_read(&handle->lock_count),
		       atomic_read(&handle->malloc_count),
		       atomic_read(&handle->mbufalloc_count));
	}
#ifdef PCIE
	if (IS_PCIE(handle->card_type) &&
	    atomic_read(&handle->malloc_cons_count)) {
		PRINTM(MERROR, "mlan has memory leak: malloc_cons_count=%d\n",
		       atomic_read(&handle->malloc_cons_count));
	}
#endif

	/* Free allocated memory for fwdump filename */
	kfree(handle->fwdump_fname);
	if (fwdump_fname) {
		kfree(fwdump_fname);
		fwdump_fname = NULL;
	}
	/* Free module params */
	woal_free_module_param(handle);
	/** clear pref_mac to avoid later crash */
	if (handle->pref_mac) {
		ref_handle = (moal_handle *)handle->pref_mac;
		if (ref_handle->pref_mac && (ref_handle->pref_mac == handle))
			ref_handle->pref_mac = NULL;
	}
	/* Free the moal handle itself */
	kfree(handle);
	LEAVE();
}

/**
 *    @brief WOAL get one line data from ASCII format data
 *
 *    @param data         Source data
 *    @param size         Source data length
 *    @param line_pos     Destination data
 *    @return             routnine status
 */
static t_size parse_cfg_get_line(t_u8 *data, t_size size, t_u8 *line_pos)
{
	t_u8 *src, *dest;
	static t_s32 pos;

	ENTER();

	if (pos >= (t_s32)size) { /* reach the end */
		pos = 0; /* Reset position for rfkill */
		LEAVE();
		return -1;
	}
	memset(line_pos, 0, MAX_LINE_LEN);
	src = data + pos;
	dest = line_pos;

	while ((dest - line_pos < MAX_LINE_LEN - 1) && pos < (t_s32)size &&
	       *src != '\x0A' && *src != '\0') {
		if (*src != ' ' && *src != '\t') /* parse space */
			*dest++ = *src++;
		else
			src++;
		pos++;
	}
	/* parse new line */
	pos++;
	*dest = '\0';
	LEAVE();
	return strlen(line_pos);
}

/**
 *  @brief Process register access request
 *  @param type_string     String format Register type
 *  @param offset_string   String format Register offset
 *  @param value_string    String format Pointer to value
 *  @return                MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static t_u32 woal_process_regrdwr(moal_handle *handle, t_u8 *type_string,
				  t_u8 *offset_string, t_u8 *value_string)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	int type, offset, value;
	pmlan_ioctl_req ioctl_req = NULL;
	mlan_ds_reg_mem *reg = NULL;

	ENTER();

	/* Alloc ioctl_req */
	ioctl_req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_reg_mem));

	if (ioctl_req == NULL) {
		PRINTM(MERROR, "Can't alloc memory\n");
		goto done;
	}

	if (MLAN_STATUS_SUCCESS != woal_atoi(&type, type_string))
		goto done;
	if (MLAN_STATUS_SUCCESS != woal_atoi(&offset, offset_string))
		goto done;
	if (MLAN_STATUS_SUCCESS != woal_atoi(&value, value_string))
		goto done;

	ioctl_req->req_id = MLAN_IOCTL_REG_MEM;
	ioctl_req->action = MLAN_ACT_SET;

	reg = (mlan_ds_reg_mem *)ioctl_req->pbuf;
	reg->sub_command = MLAN_OID_REG_RW;
	if (type < 5) {
		reg->param.reg_rw.type = type;
	} else {
		PRINTM(MERROR, "Unsupported Type\n");
		goto done;
	}
	reg->param.reg_rw.offset = offset;
	reg->param.reg_rw.value = value;

	/* request ioctl for STA */
	ret = woal_request_ioctl(handle->priv[0], ioctl_req, MOAL_IOCTL_WAIT);
	if (ret != MLAN_STATUS_SUCCESS)
		goto done;
	PRINTM(MINFO, "Register type: %d, offset: 0x%x, value: 0x%x\n", type,
	       offset, value);
	ret = MLAN_STATUS_SUCCESS;

done:
	if (ret != MLAN_STATUS_PENDING)
		kfree(ioctl_req);
	LEAVE();
	return ret;
}

#if defined(SDIO)
/**
 * @brief Read/Write registers value
 *
 * @param priv         A pointer to moal_private structure
 * @param action      get / set action
 * @param type   type of register
 * @param offset   offset of register
 * @param value   value of registere
 *
 * @return         0 --success, otherwise fail
 */
static int woal_getset_regrdwr(moal_private *priv, t_u32 action, t_u32 type,
			       t_u32 offset, t_u32 *value)
{
	int ret = 0;
	mlan_ioctl_req *req = NULL;
	mlan_ds_reg_mem *reg_mem = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;

	ENTER();

	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_reg_mem));
	if (req == NULL) {
		ret = -ENOMEM;
		goto done;
	}

	reg_mem = (mlan_ds_reg_mem *)req->pbuf;
	reg_mem->sub_command = MLAN_OID_REG_RW;
	req->req_id = MLAN_IOCTL_REG_MEM;
	req->action = action;

	reg_mem->param.reg_rw.type = type;
	reg_mem->param.reg_rw.offset = offset;
	if (req->action == MLAN_ACT_SET)
		reg_mem->param.reg_rw.value = *value;

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		ret = -EFAULT;
		goto done;
	}

	*value = reg_mem->param.reg_rw.value;
	PRINTM(MINFO, "woal_getset_regrdwr value=%x\n", *value);

done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 *    @brief set slew rate mode
 *
 *    @param handle       MOAL handle
 *    @return             MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static t_u32 woal_set_sdio_slew_rate(moal_handle *handle)
{
	t_u32 value = 0;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	moal_private *priv = NULL;
	t_u32 new_value = 0;
	t_u32 reg_type = MLAN_REG_MAC;

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv)
		return MLAN_STATUS_FAILURE;

	if ((handle->card_info->slew_rate_reg != 0) &&
	    (handle->params.slew_rate > 3 || handle->params.slew_rate < 0))
		return MLAN_STATUS_FAILURE;
#if defined(SD9098) || defined(SD9097) || defined(SDNW62X) || defined(SD9177)
	if (IS_SD9098(handle->card_type) || IS_SD9097(handle->card_type) ||
	    IS_SDNW62X(handle->card_type) || IS_SD9177(handle->card_type))
		reg_type = MLAN_REG_CIU;
#endif

	ret = woal_getset_regrdwr(priv, MLAN_ACT_GET, reg_type,
				  handle->card_info->slew_rate_reg, &value);
	if (ret < 0) {
		PRINTM(MERROR, "woal_getset_regrdwr get REG_MAC failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	new_value = value & ~(0x3 << handle->card_info->slew_rate_bit_offset);
	new_value |= (t_u32)handle->params.slew_rate
		     << handle->card_info->slew_rate_bit_offset;

	if (value != new_value) {
		PRINTM(MMSG, "Set REG 0x%8x: 0x%x slew_rate=%d\n",
		       handle->card_info->slew_rate_reg, new_value,
		       handle->params.slew_rate);
		ret = woal_getset_regrdwr(priv, MLAN_ACT_SET, reg_type,
					  handle->card_info->slew_rate_reg,
					  &new_value);
		if (ret < 0) {
			PRINTM(MERROR,
			       "woal_getset_regrdwr get REG_MAC failed\n");
			ret = MLAN_STATUS_FAILURE;
		}
	}
done:
	return ret;
}
#endif /* SDIO */

#ifdef UAP_SUPPORT
/**
 *    @brief set uap operation contrl value
 *
 *    @param handle       MOAL handle
 *    @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_set_uap_operation_ctrl(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	moal_private *priv = NULL;
	mlan_ds_bss *bss = NULL;
	mlan_ioctl_req *req = NULL;
	int uap_oper_ctrl;

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_UAP);
	if (!priv) {
		PRINTM(MERROR,
		       "woal_set_uap_operation_ctrl failed, no uap interface\n");
		LEAVE();
		return ret;
	}
	uap_oper_ctrl = handle->params.uap_oper_ctrl;
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_bss));
	if (req == NULL) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	bss = (mlan_ds_bss *)req->pbuf;
	bss->sub_command = MLAN_OID_UAP_OPER_CTRL;
	req->req_id = MLAN_IOCTL_BSS;
	req->action = MLAN_ACT_SET;

	bss->param.ap_oper_ctrl.ctrl_value =
		(t_u16)((uap_oper_ctrl & 0xffff0000) >> 16);
	bss->param.ap_oper_ctrl.chan_opt = (t_u16)(uap_oper_ctrl & 0xffff);
	PRINTM(MMSG, "Uap oper_ctrl=0x%x chan_opt=0x%x\n",
	       bss->param.ap_oper_ctrl.ctrl_value,
	       bss->param.ap_oper_ctrl.chan_opt);
	ret = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

done:
	if (ret != MLAN_STATUS_PENDING)
		kfree(req);

	LEAVE();
	return ret;
}
#endif

/**
 *    @brief WOAL parse ASCII format data to MAC address
 *
 *    @param handle       MOAL handle
 *    @param data         Source data
 *    @param size         data length
 *    @return             MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static t_u32 woal_process_init_cfg(moal_handle *handle, t_u8 *data, t_size size)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 *pos;
	t_u8 *intf_s, *intf_e;
	t_u8 s[MAX_LINE_LEN]; /* 1 line data */
	t_size line_len;
	t_u8 index = 0;
	t_u32 i;
	t_u8 bss_mac_addr[MAX_MAC_ADDR_LEN];
	t_u8 bss_mac_name[MAX_PARAM_LEN];
	t_u8 type[MAX_PARAM_LEN];
	t_u8 offset[MAX_PARAM_LEN];
	t_u8 value[MAX_PARAM_LEN];

	ENTER();

	while ((int)(line_len = parse_cfg_get_line(data, size, s)) != -1) {
		pos = s;
		while (*pos == ' ' || *pos == '\t')
			pos++;

		if (*pos == '#' || (*pos == '\r' && *(pos + 1) == '\n') ||
		    *pos == '\n' || *pos == '\0')
			continue; /* Needn't process this line */

		/* Process MAC addr */
		if (strncmp(pos, "mac_addr", 8) == 0) {
			intf_s = strchr(pos, '=');
			if (intf_s != NULL)
				intf_e = strchr(intf_s, ':');
			else
				intf_e = NULL;
			if (intf_s != NULL && intf_e != NULL) {
				strncpy(bss_mac_addr, intf_e + 1,
					MAX_MAC_ADDR_LEN - 1);
				bss_mac_addr[MAX_MAC_ADDR_LEN - 1] = '\0';
				if ((intf_e - intf_s) > MAX_PARAM_LEN) {
					PRINTM(MERROR,
					       "Too long interface name %d\n",
					       __LINE__);
					goto done;
				}
				strncpy(bss_mac_name, intf_s + 1,
					intf_e - intf_s - 1);
				bss_mac_name[intf_e - intf_s - 1] = '\0';
				for (i = 0; i < handle->priv_num; i++) {
					if (strcmp(bss_mac_name,
						   handle->priv[i]
							   ->netdev->name) ==
					    0) {
						memset(handle->priv[i]
							       ->current_addr,
						       0, ETH_ALEN);
						PRINTM(MINFO,
						       "Interface name: %s mac: %s\n",
						       bss_mac_name,
						       bss_mac_addr);
						woal_mac2u8(
							handle->priv[i]
								->current_addr,
							bss_mac_addr);
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
						if (handle->priv[i]->bss_type ==
						    MLAN_BSS_TYPE_WIFIDIRECT) {
							handle->priv[i]
								->current_addr[0] |=
								0x02;
							PRINTM(MCMND,
							       "Set WFD device addr: " MACSTR
							       "\n",
							       MAC2STR(handle->priv[i]
									       ->current_addr));
						}
#endif
#endif
#endif
						/* Set WLAN MAC addresses */
						if (MLAN_STATUS_SUCCESS !=
						    woal_request_set_mac_address(
							    handle->priv[i],
							    MOAL_IOCTL_WAIT)) {
							PRINTM(MERROR,
							       "Set MAC address failed\n");
							goto done;
						}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
						eth_hw_addr_set(
							handle->priv[i]->netdev,
							handle->priv[i]
								->current_addr);
#else
						moal_memcpy_ext(
							handle,
							handle->priv[i]
								->netdev
								->dev_addr,
							handle->priv[i]
								->current_addr,
							ETH_ALEN, ETH_ALEN);
#endif
						index++; /* Mark found one
							    interface matching
							  */
					}
				}
			} else {
				PRINTM(MERROR, "Wrong config file format %d\n",
				       __LINE__);
				goto done;
			}
		}
		/* Process REG value */
		else if (strncmp(pos, "wlan_reg", 8) == 0) {
			intf_s = strchr(pos, '=');
			if (intf_s != NULL)
				intf_e = strchr(intf_s, ',');
			else
				intf_e = NULL;
			if (intf_s != NULL && intf_e != NULL) {
				/* Copy type */
				strncpy(type, intf_s + 1, 1);
				type[1] = '\0';
			} else {
				PRINTM(MERROR, "Wrong config file format %d\n",
				       __LINE__);
				goto done;
			}
			intf_s = intf_e + 1;
			intf_e = strchr(intf_s, ',');
			if (intf_e != NULL) {
				if ((intf_e - intf_s) >= MAX_PARAM_LEN) {
					PRINTM(MERROR,
					       "Regsier offset is too long %d\n",
					       __LINE__);
					goto done;
				}
				/* Copy offset */
				strncpy(offset, intf_s, intf_e - intf_s);
				offset[intf_e - intf_s] = '\0';
			} else {
				PRINTM(MERROR, "Wrong config file format %d\n",
				       __LINE__);
				goto done;
			}
			intf_s = intf_e + 1;
			if ((strlen(intf_s) >= MAX_PARAM_LEN)) {
				PRINTM(MERROR, "Regsier value is too long %d\n",
				       __LINE__);
				goto done;
			}
			/* Copy value */
			memcpy(value, intf_s,
			       MIN((MAX_PARAM_LEN - 1), strlen(intf_s)));

			if (MLAN_STATUS_SUCCESS !=
			    woal_process_regrdwr(handle, type, offset, value)) {
				PRINTM(MERROR, "Access Reg failed\n");
				goto done;
			}
			PRINTM(MINFO, "Reg type: %s, offset: %s, value: %s\n",
			       type, offset, value);
		}
	}

	if (index == 0)
		PRINTM(MINFO, "Can't find any matching MAC Address");
	ret = MLAN_STATUS_SUCCESS;

done:
	LEAVE();
	return ret;
}

/**
 *    @brief WOAL parse ASCII format raw data to hex format
 *
 *    @param handle       MOAL handle
 *    @param data         Source data
 *    @param size         data length
 *    @param wait_option  wait option
 *    @return             MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static mlan_status woal_process_hostcmd_cfg(moal_handle *handle, t_u8 *data,
					    t_size size, t_u8 wait_option)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	t_u8 *pos = data;
	t_u8 *intf_s, *intf_e;
	t_u8 *buf = NULL;
	t_u8 *ptr = NULL;
	t_u32 cmd_len = 0;
	t_u8 start_raw = MFALSE;
	gfp_t flag;

#define CMD_STR "MRVL_CMDhostcmd"
#define CMD_BUF_LEN 2048

	ENTER();
	flag = (in_atomic() || irqs_disabled()) ? GFP_ATOMIC : GFP_KERNEL;
	buf = kzalloc(CMD_BUF_LEN, flag);
	if (!buf) {
		PRINTM(MERROR, "Could not allocate buffer space!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	ptr = buf;
	strcpy(ptr, CMD_STR);
	ptr = buf + strlen(CMD_STR) + sizeof(t_u32);
	while ((pos - data) < size) {
		while (*pos == ' ' || *pos == '\t')
			pos++;
		if (*pos == '#') { /* Line comment */
			while (*pos != '\n')
				pos++;
			pos++;
		}
		if ((*pos == '\r' && *(pos + 1) == '\n') || *pos == '\n' ||
		    *pos == '\0') {
			pos++;
			continue; /* Needn't process this line */
		}

		if (*pos == '}') {
			cmd_len = *((t_u16 *)(buf + strlen(CMD_STR) +
					      sizeof(t_u32) + sizeof(t_u16)));
			moal_memcpy_ext(handle, buf + strlen(CMD_STR), &cmd_len,
					sizeof(t_u32),
					CMD_BUF_LEN - strlen(CMD_STR));

			/* fire the hostcommand from here */
			woal_priv_hostcmd(handle->priv[0], buf, CMD_BUF_LEN,
					  wait_option);
			memset(buf + strlen(CMD_STR), 0,
			       CMD_BUF_LEN - strlen(CMD_STR));
			ptr = buf + strlen(CMD_STR) + sizeof(t_u32);
			start_raw = MFALSE;
			pos++;
			continue;
		}

		if (start_raw == MFALSE) {
			intf_s = strchr(pos, '=');
			if (intf_s)
				intf_e = strchr(intf_s, '{');
			else
				intf_e = NULL;

			if (intf_s && intf_e) {
				start_raw = MTRUE;
				pos = intf_e + 1;
				continue;
			}
		}

		if (start_raw) {
			/* Raw data block exists */
			while (*pos != '\n') {
				if ((*pos <= 'f' && *pos >= 'a') ||
				    (*pos <= 'F' && *pos >= 'A') ||
				    (*pos <= '9' && *pos >= '0')) {
					*ptr++ = woal_atox(pos);
					pos += 2;
				} else
					pos++;
			}
		}
	}

done:
	kfree(buf);
	LEAVE();
	return ret;
}
#define INIT_CFG_DATA 0x00
#define INIT_HOSTCMD_CFG_DATA 0x02
#define COUNTRY_POWER_TABLE 0x04
#define BAND_STEER_CFG_DATA 0x08

/**
 * @brief Request init conf firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void
woal_request_init_user_conf_callback(const struct firmware *firmware,
				     void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	if (!handle) {
		LEAVE();
		return;
	}
	if (firmware)
		handle->user_data = firmware;
	else
		PRINTM(MERROR, "User init config request firmware failed\n");

	handle->init_user_conf_wait_flag = MTRUE;
	wake_up_interruptible(&handle->init_user_conf_wait_q);

	LEAVE();
	return;
}

/**
 * @brief Request init conf firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void woal_request_init_dpd_conf_callback(const struct firmware *firmware,
						void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	if (!handle) {
		LEAVE();
		return;
	}
	if (firmware && handle)
		handle->dpd_data = firmware;
	else
		PRINTM(MERROR, "User init cfg data request firmware failed\n");

	handle->init_user_conf_wait_flag = MTRUE;
	wake_up_interruptible(&handle->init_user_conf_wait_q);

	LEAVE();
	return;
}

/**
 * @brief Request init conf firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void woal_request_vdll_fw_callback(const struct firmware *firmware,
					  void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	if (!handle) {
		LEAVE();
		return;
	}
	if (firmware && handle)
		handle->firmware = firmware;
	else
		PRINTM(MERROR, "VDLL: Request firmware failed\n");

	handle->init_user_conf_wait_flag = MTRUE;
	wake_up_interruptible(&handle->init_user_conf_wait_q);

	LEAVE();
	return;
}

/**
 * @brief Request firmware image for VDLL
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_vdll_req_fw(moal_handle *handle)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);
	char *vdll_fw = handle->drv_mode.fw_name;

	ENTER();
	if (vdll_fw) {
		PRINTM(MMSG, "VDLL: Request firmware: %s\n", vdll_fw);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT, vdll_fw,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_vdll_fw_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG, vdll_fw,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_vdll_fw_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG, vdll_fw,
				    handle->hotplug_device, handle,
				    woal_request_vdll_fw_callback)) < 0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, vdll_fw,
				    handle->hotplug_device, handle,
				    woal_request_vdll_fw_callback)) < 0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "VDLL: request_firmware_nowait() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->firmware, vdll_fw,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR,
				       "VDLL: request_firmware() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
		}
	}
done:
	LEAVE();
	return ret;
}

/**
 * @brief Request init conf firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void
woal_request_init_txpwr_conf_callback(const struct firmware *firmware,
				      void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	if (!handle) {
		LEAVE();
		return;
	}
	if (firmware && handle)
		handle->txpwr_data = firmware;
	else
		PRINTM(MERROR, "User init cfg data request firmware failed\n");

	handle->init_user_conf_wait_flag = MTRUE;
	wake_up_interruptible(&handle->init_user_conf_wait_q);

	LEAVE();
	return;
}

/**
 * @brief Request init conf firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void woal_request_init_cfg_data_callback(const struct firmware *firmware,
						void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	if (!handle) {
		LEAVE();
		return;
	}
	if (firmware && handle)
		handle->init_cfg_data = firmware;
	else
		PRINTM(MERROR, "User init cfg data request firmware failed\n");

	handle->init_user_conf_wait_flag = MTRUE;
	wake_up_interruptible(&handle->init_user_conf_wait_q);

	LEAVE();
	return;
}

/**
 *    @brief WOAL set user defined init data and param
 *
 *    @param handle       MOAL handle structure
 *    @param type         type argument
 *    @param wait_option  wait option
 *    @param country_txpwrlimit Configure Tx Power Limit
 *    @return             MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static t_u32 woal_set_user_init_data(moal_handle *handle, int type,
				     t_u8 wait_option, char *country_txpwrlimit)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 *cfg_data = NULL;
	t_size len;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);
	char *init_cfg = handle->params.init_cfg;
	char *init_hostcmd_cfg = handle->params.init_hostcmd_cfg;
	char *band_steer_cfg = handle->params.band_steer_cfg;

	ENTER();

	if (type == INIT_CFG_DATA) {
		PRINTM(MMSG, "Request firmware: %s\n", init_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT, init_cfg,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_init_cfg_data_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG, init_cfg,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_init_cfg_data_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG, init_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_cfg_data_callback)) < 0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, init_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_cfg_data_callback)) < 0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "Init config file request_firmware_nowait() failed\n");
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->init_cfg_data, init_cfg,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR,
				       "Init config file request_firmware() failed\n");
				goto done;
			}
		}
	} else if (type == COUNTRY_POWER_TABLE) {
		if (country_txpwrlimit == NULL) {
			PRINTM(MERROR,
			       "The parameter 'country_txpwrlimit' is NULL\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		PRINTM(MMSG, "Request firmware: %s\n", country_txpwrlimit);
		/* 'country_txpwrlimit' holds the value of Configured Tx Power
		 * Limit */
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT,
				    country_txpwrlimit, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    country_txpwrlimit, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    country_txpwrlimit, handle->hotplug_device,
				    handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, country_txpwrlimit,
				    handle->hotplug_device, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "country txpwrlimit config file request_firmware_nowait() failed\n");
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			int status =
				request_firmware_direct(&handle->user_data,
							country_txpwrlimit,
							handle->hotplug_device);
#else
			int status = request_firmware(&handle->user_data,
						      country_txpwrlimit,
						      handle->hotplug_device);
#endif
			/* File does not exist, skip download */
			if (status == -ENOENT) {
				ret = MLAN_STATUS_FILE_ERR;
				PRINTM(MIOCTL,
				       "Country power table file does not exist\n");
				goto done;
			} else if (status) {
				PRINTM(MERROR,
				       "country txpwrlimit config file request_firmware() failed\n");
				goto done;
			}
		}
	} else if (type == INIT_HOSTCMD_CFG_DATA) {
		PRINTM(MMSG, "Request firmware: %s\n", init_hostcmd_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT,
				    init_hostcmd_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    init_hostcmd_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    init_hostcmd_cfg, handle->hotplug_device,
				    handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, init_hostcmd_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "Init hostcmd config file request_firmware_nowait() failed\n");
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->user_data,
					      init_hostcmd_cfg,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR,
				       "Init hostcmd config file request_firmware() failed\n");
				goto done;
			}
		}
	}
	if (type == BAND_STEER_CFG_DATA) {
		PRINTM(MMSG, "Request firmware: %s\n", band_steer_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT,
				    band_steer_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    band_steer_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    band_steer_cfg, handle->hotplug_device,
				    handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, band_steer_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "band_steer_cfg request_firmware_nowait() failed\n");
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->user_data,
					      band_steer_cfg,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR,
				       "band_steer_cfg file request_firmware() failed\n");
				goto done;
			}
		}
	}
	if (handle->user_data) {
		cfg_data = (t_u8 *)(handle->user_data)->data;
		len = (handle->user_data)->size;
		if (type == INIT_HOSTCMD_CFG_DATA ||
		    type == BAND_STEER_CFG_DATA ||
		    type == COUNTRY_POWER_TABLE) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_process_hostcmd_cfg(handle, cfg_data, len,
						     wait_option)) {
				PRINTM(MERROR,
				       "Can't process hostcmd config file\n");
				goto done;
			}
		}
		ret = MLAN_STATUS_SUCCESS;
	} else if (type == INIT_CFG_DATA && handle->init_cfg_data) {
		PRINTM(MIOCTL, "Load init_cfg success\n");
		ret = MLAN_STATUS_SUCCESS;
	}
done:
	if (handle->user_data) {
		release_firmware(handle->user_data);
		handle->user_data = NULL;
	}

	LEAVE();
	return ret;
}

static int woal_netdevice_event(struct notifier_block *nb, unsigned long event,
				void *ptr);

#ifdef UAP_SUPPORT
/**
 *  @brief Configure WACP Mode
 *
 *  @param priv         A pointer to moal_private structure
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_set_wacp_mode(moal_private *priv, t_u8 wait_option)
{
	// moal_private      *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *pcfg_misc = NULL;
	mlan_status status;

	ENTER();

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	pcfg_misc = (mlan_ds_misc_cfg *)req->pbuf;
	pcfg_misc->sub_command = MLAN_OID_MISC_WACP_MODE;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = MLAN_ACT_SET;
	pcfg_misc->param.wacp_mode = priv->phandle->params.wacp_mode;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return status;
}
#endif

/**
 *  @brief Configure aggrctrl
 *
 *  @param priv         A pointer to moal_private structure
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_init_aggr_ctrl(moal_handle *handle, t_u8 wait_option)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *pcfg_misc = NULL;
	mlan_status status;

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	pcfg_misc = (mlan_ds_misc_cfg *)req->pbuf;
	pcfg_misc->sub_command = MLAN_OID_MISC_AGGR_CTRL;
	req->req_id = MLAN_IOCTL_MISC_CFG;

	req->action = MLAN_ACT_SET;
	pcfg_misc->param.aggr_params.tx.enable = MTRUE;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return status;
}

/**
 * @brief Add interfaces DPC
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_add_card_dpc(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int i;
	char str_buf[MLAN_MAX_VER_STR_LEN];

	ENTER();

#if defined(USB)
	if (IS_USB(handle->card_type) && handle->boot_state == USB_FW_DNLD) {
		/* Return now */
		LEAVE();
		return ret;
	}
#endif /* USB_NEW_FW_DNLD */

#ifdef CONFIG_PROC_FS
	/* Initialize proc fs */
	woal_proc_init(handle);
#endif /* CONFIG_PROC_FS */

	/* Add interfaces */
	for (i = 0; i < handle->drv_mode.intf_num; i++) {
		if (handle->drv_mode.bss_attr[i].bss_virtual)
			continue;
		if (!woal_add_interface(handle, handle->priv_num,
					handle->drv_mode.bss_attr[i].bss_type)) {
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}
	woal_get_version(handle, str_buf, sizeof(str_buf) - 1);
	PRINTM(MMSG, "wlan: version = %s\n", str_buf);

	handle->woal_notifier.notifier_call = woal_netdevice_event;
	if (register_inetaddr_notifier(&handle->woal_notifier)) {
		PRINTM(MFATAL,
		       "Error registering register_inetaddr_notifier\n");
		goto err;
	}
#ifdef MFG_CMD_SUPPORT
	if (handle->params.mfg_mode == MLAN_INIT_PARA_ENABLED)
		goto done;
#endif

	if (handle->params.init_cfg && handle->init_cfg_data) {
		if (MLAN_STATUS_SUCCESS !=
		    woal_process_init_cfg(handle,
					  (t_u8 *)(handle->init_cfg_data)->data,
					  (handle->init_cfg_data)->size)) {
			PRINTM(MERROR, "Can't process init config file\n");
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}

	if (moal_extflg_isset(handle, EXT_AGGR_CTRL)) {
		/* Enable aggregation in FW */
		if (woal_init_aggr_ctrl(handle, MOAL_IOCTL_WAIT)) {
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}

#ifdef USB
	if (handle->params.usb_aggr == 1) {
		/* Enable USB aggregation in FW */
		if (woal_usb_aggr_init(handle)) {
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}
#endif
	/* Add low power mode check */
	if (moal_extflg_isset(handle, EXT_LOW_PW_MODE) &&
	    handle->card_info->low_power_enable &&
	    woal_set_low_pwr_mode(handle, MOAL_IOCTL_WAIT)) {
		/* Proceed with Warning */
		PRINTM(MERROR, "Unable to set Low Power Mode\n");
	}

	/* Add channel tracking check */
	woal_set_chan_track_mode(handle, MOAL_IOCTL_WAIT);

#if defined(SDIO)
	if (IS_SD(handle->card_type))
		woal_set_sdio_slew_rate(handle);
#endif

	if (moal_extflg_isset(handle, EXT_PMIC) && handle->card_info->pmic) {
		if (MLAN_STATUS_SUCCESS !=
		    woal_pmic_configure(handle, MOAL_IOCTL_WAIT)) {
			PRINTM(MFATAL, "Failed to configure PMIC\n");
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}

	if (handle->params.antcfg) {
		if (MLAN_STATUS_SUCCESS !=
		    woal_set_user_antcfg(handle, MOAL_IOCTL_WAIT)) {
			PRINTM(MFATAL, "Set user antcfg data failed\n");
			ret = MLAN_STATUS_FAILURE;
			goto err;
		}
	}

#ifdef UAP_SUPPORT
	if (handle->params.uap_oper_ctrl)
		woal_set_uap_operation_ctrl(handle);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
	handle->woal_inet6_notifier.notifier_call = woal_inet6_netdeive_event;
	if (register_inet6addr_notifier(&handle->woal_inet6_notifier)) {
		PRINTM(MFATAL,
		       "Error registering register_inet6addr_notifier\n");
		goto err;
	}
#endif
#endif

#ifdef MFG_CMD_SUPPORT
done:
#endif
err:
	if (handle->params.init_cfg && handle->init_cfg_data) {
		release_firmware(handle->init_cfg_data);
		handle->init_cfg_data = NULL;
	}
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to add interface\n");
		unregister_inetaddr_notifier(&handle->woal_notifier);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
		unregister_inet6addr_notifier(&handle->woal_inet6_notifier);
#endif
#endif

		for (i = 0; i < MIN(MLAN_MAX_BSS_NUM, handle->priv_num); i++)
			woal_remove_interface(handle, i);
		handle->priv_num = 0;
#ifdef CONFIG_PROC_FS
		woal_proc_exit(handle);
#endif
	}
	LEAVE();
	return ret;
}

/**
 * @brief Request dpd data
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_req_dpd_data(moal_handle *handle,
				     mlan_init_param *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);
	char *dpd_data_cfg = handle->params.dpd_data_cfg;
	mlan_status status = MLAN_STATUS_SUCCESS;

	ENTER();

	if (dpd_data_cfg && strncmp(dpd_data_cfg, "none", strlen("none"))) {
		PRINTM(MMSG, "Request firmware: %s\n", dpd_data_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT, dpd_data_cfg,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_init_dpd_conf_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    dpd_data_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_dpd_conf_callback)) < 0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    dpd_data_cfg, handle->hotplug_device,
				    handle,
				    woal_request_init_dpd_conf_callback)) < 0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, dpd_data_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_dpd_conf_callback)) < 0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "DPD data request_firmware_nowait() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			status = request_firmware(&handle->dpd_data,
						  dpd_data_cfg,
						  handle->hotplug_device);
			if (status < 0 && status != -ENOENT) {
				PRINTM(MERROR,
				       "DPD data request_firmware() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
		}
		if (handle->dpd_data) {
			param->pdpd_data_buf = (t_u8 *)handle->dpd_data->data;
			param->dpd_data_len = handle->dpd_data->size;
		} else {
			param->dpd_data_len = UNKNOW_DPD_LENGTH;
		}
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief Request TX Power data
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_req_txpwr_data(moal_handle *handle,
				       mlan_init_param *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);
	char *txpwrlimit_cfg = handle->params.txpwrlimit_cfg;

	ENTER();

	if (txpwrlimit_cfg && strncmp(txpwrlimit_cfg, "none", strlen("none"))) {
		PRINTM(MMSG, "Download txpwrlimit_cfg=%s\n",
		       handle->params.txpwrlimit_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT,
				    txpwrlimit_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_txpwr_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    txpwrlimit_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_txpwr_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    txpwrlimit_cfg, handle->hotplug_device,
				    handle,
				    woal_request_init_txpwr_conf_callback)) <
			    0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, txpwrlimit_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_txpwr_conf_callback)) <
			    0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "Region txpwrlimit cfg data "
				       "request_firmware_nowait() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->txpwr_data,
					      txpwrlimit_cfg,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR, "Region txpwrlimit cfg data "
					       "request_firmware() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
		}
		if (handle->txpwr_data) {
			param->ptxpwr_data_buf =
				(t_u8 *)handle->txpwr_data->data;
			param->txpwr_data_len = handle->txpwr_data->size;
		}
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief Request Cal data
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_req_cal_data(moal_handle *handle,
				     mlan_init_param *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);
	char *cal_data_cfg = handle->params.cal_data_cfg;

	ENTER();
	/** Cal data request */
	if (cal_data_cfg && strncmp(cal_data_cfg, "none", strlen("none"))) {
		PRINTM(MMSG, "Request firmware: %s\n", cal_data_cfg);
		if (req_fw_nowait) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_UEVENT, cal_data_cfg,
				    handle->hotplug_device, GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    cal_data_cfg, handle->hotplug_device,
				    GFP_KERNEL, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
			if ((request_firmware_nowait(
				    THIS_MODULE, FW_ACTION_HOTPLUG,
				    cal_data_cfg, handle->hotplug_device,
				    handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#else
			if ((request_firmware_nowait(
				    THIS_MODULE, cal_data_cfg,
				    handle->hotplug_device, handle,
				    woal_request_init_user_conf_callback)) <
			    0) {
#endif
#endif
#endif
				PRINTM(MERROR,
				       "Cal data request_firmware_nowait() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			handle->init_user_conf_wait_flag = MFALSE;
			wait_event_interruptible(
				handle->init_user_conf_wait_q,
				handle->init_user_conf_wait_flag);
		} else {
			if ((request_firmware(&handle->user_data, cal_data_cfg,
					      handle->hotplug_device)) < 0) {
				PRINTM(MERROR,
				       "Cal data request_firmware() failed\n");
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
		}
	} else if (!cal_data_cfg && handle->card_info->cal_data_cfg) {
		PRINTM(MERROR,
		       "Please add cal_data_cfg for 8887/8977/8997/8987/8978\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (handle->user_data) {
		param->pcal_data_buf = (t_u8 *)handle->user_data->data;
		param->cal_data_len = handle->user_data->size;
	}
done:
	return ret;
}

#if defined(USB)
/**
 * @brief Download and Initialize firmware DPC
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_reset_usb_dev(moal_handle *handle)
{
	int ret = MLAN_STATUS_SUCCESS;
	struct usb_device *udev = ((struct usb_card_rec *)(handle->card))->udev;
	int usbRstDev_ret = 0;

	ENTER();

	/* Return now */
	PRINTM(MMSG, "WLAN FW is downloaded\n");

	/* Reset USB device to get re-enumeration */
	if (udev) {
#define USB_WAIT_FW_READY (500)
		mdelay(USB_WAIT_FW_READY);
		usbRstDev_ret = usb_reset_device(udev);
		if ((usbRstDev_ret == 0) ||
		    (usbRstDev_ret == -ENODEV) || /* expected
						     since
						     chip
						     re-enumerates
						   */
		    (usbRstDev_ret == -EINVAL)) { /* expected if USB FW detaches
						     first */
			PRINTM(MMSG, "usb_reset_device() successful.\n");
		} else {
			PRINTM(MERROR,
			       "usb_reset_device() failed with error code =%d\n",
			       usbRstDev_ret);
			ret = MLAN_STATUS_FAILURE;
		}
	} else {
		PRINTM(MERROR, "ERR: No handle to call usb_reset_device()!\n");
		ret = MLAN_STATUS_FAILURE;
	}

	LEAVE();
	return ret;
}
#endif

/**
 * @brief Download and Initialize firmware DPC
 *
 * @param handle    A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_init_fw_dpc(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	mlan_fw_image fw;
	mlan_init_param param;

	ENTER();

	if (handle->firmware) {
		memset(&fw, 0, sizeof(mlan_fw_image));
		fw.pfw_buf = (t_u8 *)handle->firmware->data;
		fw.fw_len = handle->firmware->size;
		if (handle->params.fw_reload == FW_RELOAD_SDIO_INBAND_RESET)
			fw.fw_reload = handle->params.fw_reload;
		else
			fw.fw_reload = 0;
		wifi_status = WIFI_STATUS_FW_DNLD;
		ret = mlan_dnld_fw(handle->pmlan_adapter, &fw);
		if (ret == MLAN_STATUS_FAILURE) {
			wifi_status = WIFI_STATUS_DNLD_FW_FAIL;
			PRINTM(MERROR,
			       "WLAN: Fail download FW with nowwait: %u\n",
			       moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT));
			if (handle->ops.reg_dbg)
				handle->ops.reg_dbg(handle);
			goto done;
		}
		wifi_status = WIFI_STATUS_FW_DNLD_COMPLETE;

#if defined(USB)
		if (handle->boot_state == USB_FW_DNLD) {
			if (!IS_USB8997(handle->card_type) &&
			    !IS_USB9098(handle->card_type) &&
			    !IS_USB9097(handle->card_type) &&
			    !IS_USBNW62X(handle->card_type) &&
			    !IS_USB8978(handle->card_type))
				ret = woal_reset_usb_dev(handle);
			goto done;
		}
#endif /* USB_NEW_FW_DNLD */
		PRINTM(MMSG, "WLAN FW is active\n");
		if (!handle->fw_reload)
			handle->driver_status = MFALSE;
	}

	moal_get_boot_ktime(handle, &handle->on_time);
	PRINTM(MMSG, "on_time is %llu\n", handle->on_time);

	/** data request */
	memset(&param, 0, sizeof(mlan_init_param));

	ret = woal_req_dpd_data(handle, &param);
	if (ret != MLAN_STATUS_SUCCESS)
		goto done;

	ret = woal_req_txpwr_data(handle, &param);
	if (ret != MLAN_STATUS_SUCCESS)
		goto done;

	ret = woal_req_cal_data(handle, &param);
	if (ret != MLAN_STATUS_SUCCESS)
		goto done;

	handle->hardware_status = HardwareStatusFwReady;
#ifdef USB
	if (IS_USB(handle->card_type) && !handle->fw_reload) {
		if (handle->skip_fw_dnld != MTRUE) {
			ret = woal_usb_rx_init(handle);
			if (ret == MLAN_STATUS_SUCCESS)
				ret = woal_usb_tx_init(handle);
		}
		if (ret != MLAN_STATUS_SUCCESS)
			goto done;
	}
#endif /* USB */
	ret = mlan_set_init_param(handle->pmlan_adapter, &param);
	if (handle->fw_reload) {
		LEAVE();
		return ret;
	}
	handle->init_wait_q_woken = MFALSE;
	wifi_status = WIFI_STATUS_INIT_FW;
	ret = mlan_init_fw(handle->pmlan_adapter);
	if (ret == MLAN_STATUS_FAILURE) {
		wifi_status = WIFI_STATUS_INIT_FW_FAIL;
		goto done;
	} else if (ret == MLAN_STATUS_SUCCESS) {
		wifi_status = WIFI_STATUS_OK;
		handle->hardware_status = HardwareStatusReady;
		goto done;
	}
	/* Wait for mlan_init to complete */
	wait_event_timeout(handle->init_wait_q, handle->init_wait_q_woken,
			   10 * HZ);
	if (handle->hardware_status != HardwareStatusReady) {
		wifi_status = WIFI_STATUS_INIT_FW_FAIL;
		if (handle->ops.reg_dbg)
			handle->ops.reg_dbg(handle);
#ifdef DEBUG_LEVEL1
		if (drvdbg & MFW_D) {
			drvdbg &= ~MFW_D;
			if (handle->ops.dump_fw_info)
				handle->ops.dump_fw_info(handle);
		}
#endif
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	wifi_status = WIFI_STATUS_OK;
	ret = MLAN_STATUS_SUCCESS;
done:
	if (handle->dpd_data) {
		release_firmware(handle->dpd_data);
		handle->dpd_data = NULL;
	}
	if (handle->txpwr_data) {
		release_firmware(handle->txpwr_data);
		handle->txpwr_data = NULL;
	}
	if (handle->user_data) {
		release_firmware(handle->user_data);
		handle->user_data = NULL;
	}

	LEAVE();
	return ret;
}

/**
 * @brief Request firmware DPC
 *
 * @param handle    A pointer to moal_handle structure
 * @param firmware  A pointer to firmware image
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_request_fw_dpc(moal_handle *handle,
				       const struct firmware *firmware)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	wifi_timeval tstamp;
	ENTER();

	if (!firmware) {
		woal_get_monotonic_time(&tstamp);
		if (tstamp.time_sec >
		    (handle->req_fw_time.time_sec + REQUEST_FW_TIMEOUT)) {
			PRINTM(MERROR,
			       "No firmware image found. Skipping download\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		PRINTM(MERROR,
		       "request_firmware_nowait failed for %s. Retrying..\n",
		       handle->drv_mode.fw_name);
		woal_sched_timeout(MOAL_TIMER_1S);
		ret = woal_request_fw(handle);
		if (ret != MLAN_STATUS_SUCCESS)
			PRINTM(MERROR, "%s: woal_request_fw failed!\n",
			       __func__);
		LEAVE();
		return ret;
	}
	handle->firmware = firmware;

	ret = woal_init_fw_dpc(handle);
	if (ret)
		goto done;
	ret = woal_add_card_dpc(handle);
	if (ret)
		goto done;

done:
	/* We should hold the semaphore until callback finishes execution */
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
	LEAVE();
	return ret;
}

/**
 * @brief Request firmware callback
 *        This function is invoked by request_firmware_nowait system call
 *
 * @param firmware  A pointer to firmware image
 * @param context   A pointer to moal_handle structure
 *
 * @return          N/A
 */
static void woal_request_fw_callback(const struct firmware *firmware,
				     void *context)
{
	moal_handle *handle;

	ENTER();

	handle = (moal_handle *)context;
	handle->firmware = firmware;
	if (MLAN_STATUS_SUCCESS !=
	    woal_request_fw_dpc((moal_handle *)context, firmware))
		PRINTM(MERROR, "woal_request_fw_dpc failed\n");
	if (firmware) {
		release_firmware(firmware);
		handle->firmware = NULL;
	}
	LEAVE();
	return;
}

/**
 * @brief   Download firmware using helper
 *
 * @param handle  A pointer to moal_handle structure
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_request_fw(moal_handle *handle)
{
	int err;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	t_u8 req_fw_nowait = moal_extflg_isset(handle, EXT_REQ_FW_NOWAIT);

	ENTER();

	PRINTM(MMSG, "Request firmware: %s\n", handle->drv_mode.fw_name);

	if (req_fw_nowait && !handle->fw_reload) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
		err = request_firmware_nowait(THIS_MODULE, FW_ACTION_UEVENT,
					      handle->drv_mode.fw_name,
					      handle->hotplug_device,
					      GFP_KERNEL, handle,
					      woal_request_fw_callback);
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
		err = request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
					      handle->drv_mode.fw_name,
					      handle->hotplug_device,
					      GFP_KERNEL, handle,
					      woal_request_fw_callback);
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
		err = request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
					      handle->drv_mode.fw_name,
					      handle->hotplug_device, handle,
					      woal_request_fw_callback);
#else
		err = request_firmware_nowait(THIS_MODULE,
					      handle->drv_mode.fw_name,
					      handle->hotplug_device, handle,
					      woal_request_fw_callback);
#endif
#endif
#endif
		if (err < 0) {
			PRINTM(MFATAL,
			       "WLAN: request_firmware_nowait() failed, error code = %d\n",
			       err);
			ret = MLAN_STATUS_FAILURE;
		}
	} else {
		err = request_firmware(&handle->firmware,
				       handle->drv_mode.fw_name,
				       handle->hotplug_device);
		if (err < 0) {
			PRINTM(MFATAL,
			       "WLAN: request_firmware() failed, error code = %d\n",
			       err);
			ret = MLAN_STATUS_FAILURE;
		} else {
			if (handle->fw_reload)
				ret = woal_init_fw_dpc(handle);
			else
				ret = woal_request_fw_dpc(handle,
							  handle->firmware);
			release_firmware(handle->firmware);
			handle->firmware = NULL;
		}
	}
	LEAVE();
	return ret;
}

/**
 *  @brief This function initializes firmware
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_init_fw(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;

	ENTER();

#ifdef USB
	if (IS_USB(handle->card_type)) {
		if (MFALSE || (handle->skip_fw_dnld == MTRUE)
#if defined(USB)
		    || (handle->boot_state == USB_FW_READY)
#endif /* USB_NEW_FW_DNLD */
		) {
			PRINTM(MMSG, "WLAN FW is active\n");
			/* Set it to NULL to skip downloading firmware to card
			 */
			handle->firmware = NULL;
			ret = woal_init_fw_dpc(handle);
			if (ret)
				goto done;
			ret = woal_add_card_dpc(handle);
			if (ret)
				goto done;
			/* Release semaphore if download is not required */
			MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
		done:
			LEAVE();
			return ret;
		}
	}
#endif /* USB */

	woal_get_monotonic_time(&handle->req_fw_time);
	ret = woal_request_fw(handle);
	if (ret == MLAN_STATUS_FAILURE) {
		PRINTM(MFATAL, "woal_request_fw failed\n");
	}

	LEAVE();
	return ret;
}

/**
 *  @brief This function will fill in the mlan_buffer
 *
 *  @param pmbuf   A pointer to mlan_buffer
 *  @param skb     A pointer to struct sk_buff
 *
 *  @return        N/A
 */
void woal_fill_mlan_buffer(moal_private *priv, mlan_buffer *pmbuf,
			   struct sk_buff *skb)
{
	wifi_timeval tstamp;
	struct ethhdr *eth;
	dot11_txcontrol *txcontrol;
	t_u8 tx_ctrl_flag = MFALSE;
	int i = 0;
	ENTER();

	eth = (struct ethhdr *)skb->data;

	switch (eth->h_proto) {
	case __constant_htons(ETH_P_IP):
		PRINTM(MINFO, "packet type ETH_P_IP: %04x, prio=%#x\n",
		       eth->h_proto, skb->priority);
		break;
	case __constant_htons(ETH_P_IPV6):
		PRINTM(MINFO, "packet type ETH_P_IPV6: %04x, prio=%#x\n",
		       eth->h_proto, skb->priority);
		break;
	case __constant_htons(ETH_P_ARP):
		skb->priority = 0;
		PRINTM(MINFO, "ARP packet %04x prio=%#x\n", eth->h_proto,
		       skb->priority);
		break;
	default:
		skb->priority = 0;
		if (priv->tx_protocols.protocol_num) {
			for (i = 0; i < priv->tx_protocols.protocol_num; i++) {
				if (eth->h_proto ==
				    __constant_htons(
					    priv->tx_protocols.protocols[i]))
					tx_ctrl_flag = MTRUE;
			}
		}
		if (tx_ctrl_flag) {
			txcontrol = (dot11_txcontrol *)(skb->data +
							sizeof(struct ethhdr));
			pmbuf->u.tx_info.data_rate = txcontrol->datarate;
			pmbuf->u.tx_info.channel = txcontrol->channel;
			pmbuf->u.tx_info.bw = txcontrol->bw;
			pmbuf->u.tx_info.tx_power.val = txcontrol->power;
			pmbuf->u.tx_info.retry_limit = txcontrol->retry_limit;
			skb->priority = txcontrol->priority;
			memmove(skb->data + sizeof(dot11_txcontrol), skb->data,
				sizeof(struct ethhdr));
			skb_pull(skb, sizeof(dot11_txcontrol));
			pmbuf->flags |= MLAN_BUF_FLAG_TX_CTRL;
		}
		break;
	}
	PRINTM(MDAT_D, "packet %04x prio=%#x\n", eth->h_proto, skb->priority);

	if (priv->enable_mc_aggr && priv->num_mcast_addr) {
		if (woal_find_mcast_node_tx(priv, skb)) {
			mc_txcontrol *tx_ctrl =
				(mc_txcontrol *)(skb->data + skb->len -
						 sizeof(mc_txcontrol));
			moal_memcpy_ext(priv->phandle,
					(t_u8 *)&pmbuf->u.mc_tx_info,
					(t_u8 *)tx_ctrl, sizeof(mc_txcontrol),
					sizeof(mc_txcontrol));
			pmbuf->flags |= MLAN_BUF_FLAG_MC_AGGR_PKT;
			// use AC_VO
			skb->priority = 6;
		}
	}

	/* Record the current time the packet was queued; used to determine
	 *   the amount of time the packet was queued in the driver before it
	 *   was sent to the firmware.  The delay is then sent along with the
	 *   packet to the firmware for aggregate delay calculation for stats
	 *   and MSDU lifetime expiry.
	 */
	woal_get_monotonic_time(&tstamp);
	skb->tstamp = ktime_get_real();

	pmbuf->pdesc = skb;
	pmbuf->pbuf = skb->head + sizeof(mlan_buffer);
#ifdef PCIE
	if (IS_PCIE(priv->phandle->card_type)) {
		pmbuf->buf_pa = 0;
	}
#endif
	pmbuf->data_offset = skb->data - (skb->head + sizeof(mlan_buffer));
	pmbuf->data_len = skb->len;
	pmbuf->priority = skb->priority;
	pmbuf->buf_type = 0;
	pmbuf->in_ts_sec = tstamp.time_sec;
	pmbuf->in_ts_usec = tstamp.time_usec;

	LEAVE();
	return;
}

#if defined(STA_CFG80211) && defined(UAP_CFG80211)
/**
 * @brief This function opens the network device for monitor interface
 *
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
static int woal_mon_open(struct net_device *ndev)
{
	ENTER();
	LEAVE();
	return 0;
}

/**
 * @brief This function closes the network device for monitor interface
 *
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
static int woal_mon_close(struct net_device *ndev)
{
	ENTER();
	LEAVE();
	return 0;
}

/**
 * @brief This function sets the MAC address to firmware for monitor interface
 *
 * @param dev             A pointer to net_device structure
 * @param addr            MAC address to set
 *
 * @return                0 -- success, otherwise fail
 */
static int woal_mon_set_mac_address(struct net_device *ndev, void *addr)
{
	ENTER();
	LEAVE();
	return 0;
}

/**
 * @brief This function sets multicast address to firmware for monitor interface
 *
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
static void woal_mon_set_multicast_list(struct net_device *ndev)
{
	ENTER();
	LEAVE();
}

/**
 * @brief This function handles packet transmission for monitor interface
 *
 * @param skb             A pointer to sk_buff structure
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
static netdev_tx_t woal_mon_hard_start_xmit(struct sk_buff *skb,
					    struct net_device *ndev)
{
	int len_rthdr;
	int qos_len = 0;
	int dot11_hdr_len = 24;
	int snap_len = 6;
	unsigned char *pdata;
	unsigned short fc;
	unsigned char src_mac_addr[6];
	unsigned char dst_mac_addr[6];
	struct ieee80211_hdr *dot11_hdr;
	struct ieee80211_radiotap_header *prthdr =
		(struct ieee80211_radiotap_header *)skb->data;
	monitor_iface *mon_if = netdev_priv(ndev);

	ENTER();

	if (mon_if == NULL || mon_if->base_ndev == NULL) {
		goto fail;
	}

	/* check for not even having the fixed radiotap header part */
	if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header))) {
		PRINTM(MERROR,
		       "Invalid radiotap hdr length,"
		       "skb->len: %d\n",
		       skb->len);
		goto fail; /* too short to be possibly valid */
	}

	/* is it a header version we can trust to find length from? */
	if (unlikely(prthdr->it_version))
		goto fail; /* only version 0 is supported */

	/* then there must be a radiotap header with a length we can use */
	len_rthdr = ieee80211_get_radiotap_len(skb->data);

	/* does the skb contain enough to deliver on the alleged length? */
	if (unlikely((int)skb->len < len_rthdr)) {
		PRINTM(MERROR,
		       "Invalid data length,"
		       "skb->len: %d\n",
		       skb->len);
		goto fail; /* skb too short for claimed rt header extent */
	}

	/* Skip the ratiotap header */
	skb_pull(skb, len_rthdr);

	dot11_hdr = (struct ieee80211_hdr *)skb->data;
	fc = le16_to_cpu(dot11_hdr->frame_control);
	if ((fc & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_DATA) {
		/* Check if this ia a Wireless Distribution System (WDS) frame
		 * which has 4 MAC addresses
		 */
		if (dot11_hdr->frame_control & (__force __le16)0x0080)
			qos_len = 2;
		if ((dot11_hdr->frame_control & (__force __le16)0x0300) ==
		    (__force __le16)0x0300)
			dot11_hdr_len += 6;

		moal_memcpy_ext(NULL, dst_mac_addr, dot11_hdr->addr1,
				sizeof(dst_mac_addr), sizeof(dst_mac_addr));
		moal_memcpy_ext(NULL, src_mac_addr, dot11_hdr->addr2,
				sizeof(src_mac_addr), sizeof(src_mac_addr));

		/* Skip the 802.11 header, QoS (if any) and SNAP, but leave
		 * spaces for for two MAC addresses
		 */
		skb_pull(skb, dot11_hdr_len + qos_len + snap_len -
				      sizeof(src_mac_addr) * 2);
		pdata = (unsigned char *)skb->data;
		moal_memcpy_ext(NULL, pdata, dst_mac_addr, sizeof(dst_mac_addr),
				(t_u32)skb->len);
		moal_memcpy_ext(NULL, pdata + sizeof(dst_mac_addr),
				src_mac_addr, sizeof(src_mac_addr),
				(t_u32)skb->len - sizeof(dst_mac_addr));

		LEAVE();
		return woal_hard_start_xmit(skb, mon_if->base_ndev);
	}

fail:
	dev_kfree_skb(skb);
	LEAVE();
	return NETDEV_TX_OK;
}

/**
 *  @brief This function returns the network statistics
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        A pointer to net_device_stats structure
 */
static struct net_device_stats *woal_mon_get_stats(struct net_device *dev)
{
	monitor_iface *mon_if = (monitor_iface *)netdev_priv(dev);
	return &mon_if->stats;
}

static const struct net_device_ops woal_cfg80211_mon_if_ops = {
	.ndo_open = woal_mon_open,
	.ndo_start_xmit = woal_mon_hard_start_xmit,
	.ndo_stop = woal_mon_close,
	.ndo_get_stats = woal_mon_get_stats,
	.ndo_set_mac_address = woal_mon_set_mac_address,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	.ndo_set_rx_mode = woal_mon_set_multicast_list,
#else
	.ndo_set_multicast_list = woal_mon_set_multicast_list,
#endif
};

/**
 * @brief This function setup monitor interface
 *
 * @param dev             A pointer to net_device structure
 *
 * @return                0 -- success, otherwise fail
 */
static void woal_mon_if_setup(struct net_device *dev)
{
	ENTER();
	ether_setup(dev);
	dev->netdev_ops = &woal_cfg80211_mon_if_ops;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 9)
	dev->needs_free_netdev = true;
#else
	dev->destructor = free_netdev;
#endif
	LEAVE();
}

/**
 * @brief Request the driver to add a monitor interface
 *
 * @param priv             A pointer to moal_private
 * @param name              Virtual interface name
 * @param name_assign_type  Interface name assignment type
 * @param sniffer_mode           Sniffer mode
 *
 * @return                  A pointer to monitor_iface
 */
monitor_iface *woal_prepare_mon_if(moal_private *priv, const char *name,
				   unsigned char name_assign_type,
				   int sniffer_mode)
{
	int ret = 0;
	moal_handle *handle = priv->phandle;
	struct net_device *ndev = NULL;
	monitor_iface *mon_if = NULL;

	ENTER();

	if (sniffer_mode != CHANNEL_SPEC_SNIFFER_MODE) {
		PRINTM(MERROR, "Sniffer mode is not valid\n");
		ret = -EFAULT;
		goto fail;
	}
	if ((sniffer_mode == CHANNEL_SPEC_SNIFFER_MODE) &&
	    woal_is_any_interface_active(handle)) {
		PRINTM(MERROR,
		       "Cannot start channel specified net monitor when Interface Active\n");
		ret = -EFAULT;
		goto fail;
	}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
	ndev = alloc_netdev_mq(sizeof(*mon_if), name, name_assign_type,
			       woal_mon_if_setup, 1);
#else
	ndev = alloc_netdev_mq(sizeof(*mon_if), name, NET_NAME_UNKNOWN,
			       woal_mon_if_setup, 1);
#endif
#else
	ndev = alloc_netdev_mq(sizeof(*mon_if), name, woal_mon_if_setup, 1);
#endif
#else
	ndev = alloc_netdev_mq(sizeof(*mon_if), name, woal_mon_if_setup);
#endif
	if (!ndev) {
		PRINTM(MFATAL, "Init virtual ethernet device failed\n");
		ret = -EFAULT;
		goto fail;
	}

	ret = dev_alloc_name(ndev, ndev->name);
	if (ret < 0) {
		PRINTM(MFATAL, "Net device alloc name fail.\n");
		ret = -EFAULT;
		goto fail;
	}

	//?memcpy(ndev->dev_addr, ndev->perm_addr, ETH_ALEN);

	mon_if = netdev_priv(ndev);
	moal_memcpy_ext(handle, mon_if->ifname, ndev->name, IFNAMSIZ, IFNAMSIZ);

	ndev->type = ARPHRD_IEEE80211_RADIOTAP;
	ndev->netdev_ops = &woal_cfg80211_mon_if_ops;

	mon_if->priv = priv;
	mon_if->mon_ndev = ndev;
	mon_if->base_ndev = priv->netdev;
	mon_if->sniffer_mode = sniffer_mode;
	mon_if->radiotap_enabled = 1;
	mon_if->flag = 1;

fail:
	if (ret) {
		if (ndev)
			free_netdev(ndev);
		LEAVE();
		return NULL;
	}

	LEAVE();
	return mon_if;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
static struct device_type wlan_type = {
	.name = "wlan",
};
#endif

#ifdef STA_SUPPORT
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
/** Network device handlers */
const struct net_device_ops woal_netdev_ops = {
	.ndo_open = woal_open,
	.ndo_start_xmit = woal_hard_start_xmit,
	.ndo_stop = woal_close,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	.ndo_siocdevprivate = woal_do_ioctl,
#else
	.ndo_do_ioctl = woal_do_ioctl,
#endif
	.ndo_set_mac_address = woal_set_mac_address,
	.ndo_change_mtu = woal_change_mtu,
	.ndo_tx_timeout = woal_tx_timeout,
	.ndo_get_stats = woal_get_stats,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	.ndo_set_rx_mode = woal_set_multicast_list,
#else
	.ndo_set_multicast_list = woal_set_multicast_list,
#endif
	.ndo_select_queue = woal_select_queue,
	.ndo_validate_addr = eth_validate_addr,
};
#endif

#define MAX_MTU_SIZE 2000
/**
 *  @brief This function initializes the private structure
 *          and dev structure for station mode
 *
 *  @param dev      A pointer to net_device structure
 *  @param priv     A pointer to moal_private structure
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status woal_init_sta_dev(struct net_device *dev, moal_private *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
	mlan_fw_info fw_info;
#endif
	ENTER();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
#ifdef MFG_CMD_SUPPORT
	if (priv->phandle->params.mfg_mode != MLAN_INIT_PARA_ENABLED) {
#endif
		memset(&fw_info, 0, sizeof(mlan_fw_info));
		if (MLAN_STATUS_SUCCESS !=
		    woal_request_get_fw_info(priv, MOAL_IOCTL_WAIT, &fw_info)) {
			PRINTM(MERROR, "%s: get_fw_info failed \n", __func__);
			return MLAN_STATUS_FAILURE;
		}
		if (fw_info.tx_buf_size >
		    (MAX_MTU_SIZE + MLAN_MIN_DATA_HEADER_LEN +
		     priv->extra_tx_head_len)) {
			dev->max_mtu = MAX_MTU_SIZE;
			PRINTM(MINFO, "wlan: %s set max_mtu %d\n", dev->name,
			       dev->max_mtu);
		}
#ifdef MFG_CMD_SUPPORT
	}
#endif
#endif
	/* Setup the OS Interface to our functions */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 29)
	dev->open = woal_open;
	dev->hard_start_xmit = woal_hard_start_xmit;
	dev->stop = woal_close;
	dev->do_ioctl = woal_do_ioctl;
	dev->set_mac_address = woal_set_mac_address;
	dev->tx_timeout = woal_tx_timeout;
	dev->get_stats = woal_get_stats;
	dev->set_multicast_list = woal_set_multicast_list;
#else
	dev->netdev_ops = &woal_netdev_ops;
#endif
	dev->watchdog_timeo = MRVDRV_DEFAULT_WATCHDOG_TIMEOUT;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0)
	dev->needed_headroom += MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
				priv->extra_tx_head_len;
#else
	dev->hard_header_len += MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
				priv->extra_tx_head_len;
#endif
#ifdef STA_WEXT
	if (IS_STA_WEXT(priv->phandle->params.cfg80211_wext)) {
#if WIRELESS_EXT < 21
		dev->get_wireless_stats = woal_get_wireless_stats;
#endif
		dev->wireless_handlers =
			(struct iw_handler_def *)&woal_handler_def;
	}
#endif
	dev->flags |= IFF_BROADCAST | IFF_MULTICAST;

#ifdef STA_CFG80211
	if (IS_STA_CFG80211(priv->phandle->params.cfg80211_wext))
		init_waitqueue_head(&priv->ft_wait_q);
#endif
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}
#endif /* STA_SUPPORT */

#ifdef UAP_SUPPORT
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
/** Network device handlers */
const struct net_device_ops woal_uap_netdev_ops = {
	.ndo_open = woal_open,
	.ndo_start_xmit = woal_hard_start_xmit,
	.ndo_stop = woal_close,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
	.ndo_siocdevprivate = woal_uap_do_ioctl,
#else
	.ndo_do_ioctl = woal_uap_do_ioctl,
#endif
	.ndo_set_mac_address = woal_set_mac_address,
	.ndo_change_mtu = woal_change_mtu,
	.ndo_tx_timeout = woal_tx_timeout,
	.ndo_get_stats = woal_get_stats,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	.ndo_set_rx_mode = woal_uap_set_multicast_list,
#else
	.ndo_set_multicast_list = woal_uap_set_multicast_list,
#endif
	.ndo_select_queue = woal_select_queue,
	.ndo_validate_addr = eth_validate_addr,
};
#endif

/**
 *  @brief This function initializes the private structure
 *          and dev structure for uap mode
 *
 *  @param dev      A pointer to net_device structure
 *  @param priv     A pointer to moal_private structure
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status woal_init_uap_dev(struct net_device *dev, moal_private *priv)
{
	mlan_status status = MLAN_STATUS_SUCCESS;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
	mlan_fw_info fw_info;
#endif

	ENTER();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
	memset(&fw_info, 0, sizeof(mlan_fw_info));
	if (MLAN_STATUS_SUCCESS !=
	    woal_request_get_fw_info(priv, MOAL_IOCTL_WAIT, &fw_info)) {
		PRINTM(MERROR, "%s: get_fw_info failed \n", __func__);
		return MLAN_STATUS_FAILURE;
	}
	if (fw_info.tx_buf_size > (MAX_MTU_SIZE + MLAN_MIN_DATA_HEADER_LEN +
				   priv->extra_tx_head_len)) {
		dev->max_mtu = MAX_MTU_SIZE;
		PRINTM(MMSG, "wlan: %s set max_mtu %d\n", dev->name,
		       dev->max_mtu);
	}
#endif
	/* Setup the OS Interface to our functions */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 29)
	dev->open = woal_open;
	dev->hard_start_xmit = woal_hard_start_xmit;
	dev->stop = woal_close;
	dev->set_mac_address = woal_set_mac_address;
	dev->tx_timeout = woal_tx_timeout;
	dev->get_stats = woal_get_stats;
	dev->do_ioctl = woal_uap_do_ioctl;
	dev->set_multicast_list = woal_uap_set_multicast_list;
#else
	dev->netdev_ops = &woal_uap_netdev_ops;
#endif
	dev->watchdog_timeo = MRVDRV_DEFAULT_UAP_WATCHDOG_TIMEOUT;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0)
	dev->needed_headroom += MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
				priv->extra_tx_head_len;
#else
	dev->hard_header_len += MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
				priv->extra_tx_head_len;
#endif
	/** don't need register to wext */
	if (priv->bss_type == MLAN_BSS_TYPE_DFS) {
		LEAVE();
		return status;
	}
#ifdef UAP_WEXT
	if (IS_UAP_WEXT(priv->phandle->params.cfg80211_wext)) {
#if WIRELESS_EXT < 21
		dev->get_wireless_stats = woal_get_uap_wireless_stats;
#endif
		dev->wireless_handlers =
			(struct iw_handler_def *)&woal_uap_handler_def;
	}
#endif /* UAP_WEXT */
	dev->flags |= IFF_BROADCAST | IFF_MULTICAST;

	LEAVE();
	return status;
}
#endif /* UAP_SUPPORT */

/**
 * @brief This function adds a new interface. It will
 *      allocate, initialize and register the device.
 *
 *  @param handle    A pointer to moal_handle structure
 *  @param bss_index BSS index number
 *  @param bss_type  BSS type
 *
 *  @return          A pointer to the new priv structure
 */
moal_private *woal_add_interface(moal_handle *handle, t_u8 bss_index,
				 t_u8 bss_type)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	char name[256];
	int i = 0;
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
	char csa_str[256];
#endif
#endif
	ENTER();

	memset(name, 0, sizeof(name));
	switch (bss_type) {
#ifdef STA_SUPPORT
	case MLAN_BSS_TYPE_STA:
		if (handle->params.sta_name)
			snprintf(name, sizeof(name), "%s%%d",
				 handle->params.sta_name);
		else if (handle->second_mac)
			snprintf(name, sizeof(name), "m%s", default_mlan_name);
		else
			snprintf(name, sizeof(name), "%s", default_mlan_name);
		break;
#endif
#ifdef UAP_SUPPORT
	case MLAN_BSS_TYPE_UAP:
		if (handle->params.uap_name)
			snprintf(name, sizeof(name), "%s%%d",
				 handle->params.uap_name);
		else if (handle->second_mac)
			snprintf(name, sizeof(name), "m%s", default_uap_name);
		else
			snprintf(name, sizeof(name), "%s", default_uap_name);
		break;
#endif
#ifdef WIFI_DIRECT_SUPPORT
	case MLAN_BSS_TYPE_WIFIDIRECT:
		if (handle->params.wfd_name)
			snprintf(name, sizeof(name), "%s%%d",
				 handle->params.wfd_name);
		else if (handle->second_mac)
			snprintf(name, sizeof(name), "m%s", default_wfd_name);
		else
			snprintf(name, sizeof(name), "%s", default_wfd_name);
		break;
#endif
	case MLAN_BSS_TYPE_DFS:
		if (handle->second_mac)
			snprintf(name, sizeof(name), "m%s", default_dfs_name);
		else
			snprintf(name, sizeof(name), "%s", default_dfs_name);
		break;
	default:
		PRINTM(MERROR, "woal_add_interface: invalid bss_type=%d\n",
		       bss_type);
		return NULL;
	}

#define MAX_WMM_QUEUE 4
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
	/* Allocate an net device */
	dev = alloc_netdev_mq(sizeof(moal_private), name, NET_NAME_UNKNOWN,
			      ether_setup, MAX_WMM_QUEUE);
#else
	dev = alloc_netdev_mq(sizeof(moal_private), name, ether_setup,
			      MAX_WMM_QUEUE);
#endif

	if (!dev) {
		PRINTM(MERROR, "alloc_netdev failed\n");
		goto error;
	}
	priv = (moal_private *)netdev_priv(dev);
	/* Save the priv to handle */
	handle->priv[bss_index] = priv;

	/* Use the same handle structure */
	priv->phandle = handle;
	priv->netdev = dev;
	priv->bss_index = bss_index;
	priv->bss_type = bss_type;
	priv->extra_tx_head_len = 0;
	if (bss_type == MLAN_BSS_TYPE_STA) {
		priv->bss_role = MLAN_BSS_ROLE_STA;
	} else if (bss_type == MLAN_BSS_TYPE_UAP) {
		priv->bss_role = MLAN_BSS_ROLE_UAP;
	}
#ifdef WIFI_DIRECT_SUPPORT
	else if (bss_type == MLAN_BSS_TYPE_WIFIDIRECT)
		priv->bss_role = MLAN_BSS_ROLE_STA;
#endif
	else if (bss_type == MLAN_BSS_TYPE_DFS)
		priv->bss_role = MLAN_BSS_ROLE_UAP;

	INIT_LIST_HEAD(&priv->tcp_sess_queue);
	spin_lock_init(&priv->tcp_sess_lock);
#ifdef STA_SUPPORT
	INIT_LIST_HEAD(&priv->tdls_list);
	spin_lock_init(&priv->tdls_lock);
#endif

	INIT_LIST_HEAD(&priv->tx_stat_queue);
	spin_lock_init(&priv->tx_stat_lock);
	INIT_LIST_HEAD(&priv->mcast_list);
	spin_lock_init(&priv->mcast_lock);

#ifdef STA_CFG80211
#ifdef STA_SUPPORT
	spin_lock_init(&priv->connect_lock);
#endif
#endif

#ifdef STA_SUPPORT
	INIT_LIST_HEAD(&priv->pmksa_cache_list);
	if (bss_type == MLAN_BSS_TYPE_STA) {
		init_waitqueue_head(&priv->okc_wait_q);
		spin_lock_init(&priv->pmksa_list_lock);
		priv->okc_roaming_ie = NULL;
		priv->okc_ie_len = 0;
	}
#endif
#if defined(DRV_EMBEDDED_AUTHENTICATOR)
	init_waitqueue_head(&priv->hostcmd_wait_q);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	SET_MODULE_OWNER(dev);
#endif
#ifdef STA_SUPPORT
	if (bss_type == MLAN_BSS_TYPE_STA
#ifdef WIFI_DIRECT_SUPPORT
	    || bss_type == MLAN_BSS_TYPE_WIFIDIRECT
#endif
	)
		woal_init_sta_dev(dev, priv);
#endif
#ifdef UAP_SUPPORT
	if (bss_type == MLAN_BSS_TYPE_UAP || bss_type == MLAN_BSS_TYPE_DFS) {
		if (MLAN_STATUS_SUCCESS != woal_init_uap_dev(dev, priv))
			goto error;
	}
#endif
	if (!handle->priv_num
#ifdef MFG_CMD_SUPPORT
	    && (handle->params.mfg_mode != MLAN_INIT_PARA_ENABLED)
#endif
	) {
		if (handle->params.init_cfg) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_user_init_data(handle, INIT_CFG_DATA,
						    MOAL_IOCTL_WAIT, NULL)) {
				PRINTM(MFATAL,
				       "Set user init data and param failed\n");
				goto error;
			}
		}
		if (handle->params.init_hostcmd_cfg) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_user_init_data(handle,
						    INIT_HOSTCMD_CFG_DATA,
						    MOAL_IOCTL_WAIT, NULL)) {
				PRINTM(MFATAL,
				       "Set user init hostcmd data and param failed\n");
				goto error;
			}
		}
	}
#ifdef MFG_CMD_SUPPORT
	if (handle->params.mfg_mode != MLAN_INIT_PARA_ENABLED) {
#endif
		if (bss_type == MLAN_BSS_TYPE_UAP &&
		    priv->phandle->params.band_steer_cfg) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_user_init_data(priv->phandle,
						    BAND_STEER_CFG_DATA,
						    MOAL_IOCTL_WAIT, NULL)) {
				PRINTM(MFATAL,
				       "Set band steering configure param failed\n");
			}
		}
#ifdef MFG_CMD_SUPPORT
	}
#endif

	handle->priv_num++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	if (!priv->phandle->wiphy &&
	    IS_STA_OR_UAP_CFG80211(handle->params.cfg80211_wext)) {
		if (woal_register_cfg80211(priv)) {
			PRINTM(MERROR, "Cannot register with cfg80211\n");
			goto error;
		}
	}
#endif

#ifdef STA_CFG80211
#ifdef STA_SUPPORT
	if ((priv->bss_role == MLAN_BSS_ROLE_STA) &&
	    IS_STA_CFG80211(handle->params.cfg80211_wext)) {
		if (bss_type == MLAN_BSS_TYPE_STA
#ifdef WIFI_DIRECT_SUPPORT
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
		    || bss_type == MLAN_BSS_TYPE_WIFIDIRECT
#endif
#endif
		)
			/* Register cfg80211 for STA or Wifi direct */
			if (woal_register_sta_cfg80211(dev, bss_type)) {
				PRINTM(MERROR,
				       "Cannot register STA with cfg80211\n");
				goto error;
			}
	}

#endif /* STA_SUPPORT */
#endif /* STA_CFG80211 */
#ifdef UAP_CFG80211
#ifdef UAP_SUPPORT
	if ((priv->bss_role == MLAN_BSS_ROLE_UAP) &&
	    IS_UAP_CFG80211(handle->params.cfg80211_wext) &&
	    bss_type != MLAN_BSS_TYPE_DFS) {
		/* Register cfg80211 for UAP */
		if (woal_register_uap_cfg80211(dev, bss_type)) {
			PRINTM(MERROR, "Cannot register UAP with cfg80211\n");
			goto error;
		}
	}
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
	strcpy(csa_str, "CSA");
	strcat(csa_str, name);
	priv->csa_workqueue = alloc_workqueue(
		csa_str, WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
	if (!priv->csa_workqueue) {
		PRINTM(MERROR, "cannot alloc csa workqueue \n");
		goto error;
	}
	INIT_DELAYED_WORK(&priv->csa_work, woal_csa_work_queue);
#endif
#endif
#endif /*UAP_CFG80211 */

	/* Create workqueue for main process */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	priv->mclist_workqueue =
		alloc_workqueue("MCLIST_WORK_QUEUE",
				WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
	priv->mclist_workqueue = create_workqueue("MCLIST_WORK_QUEUE");
#endif
	if (!priv->mclist_workqueue) {
		PRINTM(MERROR, "cannot alloc mclist workqueue \n");
		goto error;
	}
	MLAN_INIT_WORK(&priv->mclist_work, woal_mclist_work_queue);

	/* Initialize priv structure */
	woal_init_priv(priv, MOAL_IOCTL_WAIT);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	SET_NETDEV_DEV(dev, handle->hotplug_device);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
	SET_NETDEV_DEVTYPE(dev, &wlan_type);
#endif

	/* Register network device */
	if (register_netdev(dev)) {
		PRINTM(MERROR, "Cannot register virtual network device\n");
		goto error;
	}
	netif_carrier_off(dev);
	woal_stop_queue(dev);

	PRINTM(MMSG, "Register NXP 802.11 Adapter %s\n", dev->name);

	if (bss_type == MLAN_BSS_TYPE_STA ||
	    priv->bss_type == MLAN_BSS_TYPE_UAP) {
#if defined(SD8887) || defined(SD8987)
		mlan_fw_info fw_info;
		memset(&fw_info, 0, sizeof(mlan_fw_info));
		if (MLAN_STATUS_SUCCESS !=
		    woal_request_get_fw_info(priv, MOAL_IOCTL_WAIT, &fw_info)) {
			PRINTM(MERROR, "%s: get_fw_info failed \n", __func__);
			goto error;
		}
		if (MFALSE
#ifdef SD8887
		    || IS_SD8887(handle->card_type)
#endif
#ifdef SD8987
		    || IS_SD8987(handle->card_type)
#endif
		) {
			if ((fw_info.antinfo & ANT_DIVERSITY_2G) &&
			    (fw_info.antinfo & ANT_DIVERSITY_5G))
				handle->card_info->histogram_table_num = 4;
		}
#endif

		for (i = 0; i < handle->card_info->histogram_table_num; i++) {
			priv->hist_data[i] =
				kmalloc(sizeof(hgm_data) +
						handle->card_info->rx_rate_max *
							sizeof(atomic_t),
					GFP_KERNEL);
			if (!(priv->hist_data[i])) {
				PRINTM(MERROR,
				       "kmalloc priv->hist_data[%d] failed\n",
				       i);
				goto error;
			}
		}
		woal_hist_data_reset(priv);
	}
#ifdef CONFIG_PROC_FS
	woal_create_proc_entry(priv);
	woal_debug_entry(priv);
#endif /* CONFIG_PROC_FS */

	LEAVE();
	return priv;
error:
	handle->priv_num = bss_index;
	/* Unregister wiphy device and free */
	if (priv) {
		if (priv->mclist_workqueue) {
			flush_workqueue(priv->mclist_workqueue);
			destroy_workqueue(priv->mclist_workqueue);
			priv->mclist_workqueue = NULL;
		}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		if (priv->wdev &&
		    IS_STA_OR_UAP_CFG80211(handle->params.cfg80211_wext))
			priv->wdev = NULL;
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		if (priv->csa_workqueue) {
			destroy_workqueue(priv->csa_workqueue);
			priv->csa_workqueue = NULL;
		}
#endif
#endif
#endif
	}
	if (dev && dev->reg_state == NETREG_REGISTERED)
		unregister_netdev(dev);
	if (dev)
		free_netdev(dev);
	LEAVE();
	return NULL;
}

/**
 *  @brief This function removes an interface.
 *
 *  @param handle       A pointer to the moal_handle structure
 *  @param bss_index    BSS index number
 *
 *  @return             N/A
 */
void woal_remove_interface(moal_handle *handle, t_u8 bss_index)
{
	struct net_device *dev = NULL;
	moal_private *priv = handle->priv[bss_index];
#if defined(STA_WEXT) || defined(UAP_WEXT)
	union iwreq_data wrqu;
#endif
	int i = 0;

	ENTER();

	if (!priv || !priv->netdev)
		goto error;
	dev = priv->netdev;

	if (priv->media_connected == MTRUE) {
		priv->media_connected = MFALSE;
		moal_connection_status_check_pmqos(handle);
#if defined(STA_WEXT) || defined(UAP_WEXT)
		if (IS_STA_OR_UAP_WEXT(handle->params.cfg80211_wext) &&
		    GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
			memset(wrqu.ap_addr.sa_data, 0x00, ETH_ALEN);
			wrqu.ap_addr.sa_family = ARPHRD_ETHER;
			wireless_send_event(priv->netdev, SIOCGIWAP, &wrqu,
					    NULL);
		}
#endif
	}
	woal_flush_tcp_sess_queue(priv);

	woal_flush_tx_stat_queue(priv);

#ifdef STA_CFG80211
	if (priv->bss_type == MLAN_BSS_TYPE_STA)
		woal_flush_tdls_list(priv);
#endif
	woal_flush_mcast_list(priv);

#ifdef STA_CFG80211
	if (priv->bss_type == MLAN_BSS_TYPE_STA &&
	    IS_STA_CFG80211(handle->params.cfg80211_wext)) {
		if (woal_flush_pmksa_list(priv))
			PRINTM(MERROR, "%s: woal_flush_pmksa_list failed!\n",
			       __func__);
		if (priv->okc_roaming_ie) {
			kfree(priv->okc_roaming_ie);
			priv->okc_roaming_ie = NULL;
			priv->okc_ie_len = 0;
		}
	}
#endif

	if (priv->bss_type == MLAN_BSS_TYPE_STA ||
	    priv->bss_type == MLAN_BSS_TYPE_UAP) {
		for (i = 0; i < handle->card_info->histogram_table_num; i++) {
			kfree(priv->hist_data[i]);
			priv->hist_data[i] = NULL;
		}
	}
#ifdef CONFIG_PROC_FS
	/* Remove proc debug */
	woal_debug_remove(priv);
	woal_proc_remove(priv);
#endif /* CONFIG_PROC_FS */
	/* Last reference is our one */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)
	PRINTM(MINFO, "refcnt = %d\n", atomic_read(&dev->refcnt));
#else
	PRINTM(MINFO, "refcnt = %d\n", netdev_refcnt_read(dev));
#endif

	PRINTM(MINFO, "netdev_finish_unregister: %s\n", dev->name);

	if (dev->reg_state == NETREG_REGISTERED)
		unregister_netdev(dev);

	if (priv->mclist_workqueue) {
		flush_workqueue(priv->mclist_workqueue);
		destroy_workqueue(priv->mclist_workqueue);
		priv->mclist_workqueue = NULL;
	}

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	/* Unregister wiphy device and free */
	if (priv->wdev && IS_STA_OR_UAP_CFG80211(handle->params.cfg80211_wext))
		priv->wdev = NULL;
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
	if (priv->csa_workqueue) {
		flush_workqueue(priv->csa_workqueue);
		destroy_workqueue(priv->csa_workqueue);
		priv->csa_workqueue = NULL;
	}
#endif
#endif
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA ||
	    GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP)
		woal_deinit_wifi_hal(priv);
#endif
#endif

		/* Clear the priv in handle */
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 8, 13)
	if (IS_STA_OR_UAP_CFG80211(handle->params.cfg80211_wext))
		priv->phandle->wiphy->extended_capabilities = NULL;
#endif
#endif
	priv->phandle->priv[priv->bss_index] = NULL;
	priv->phandle = NULL;
	priv->netdev = NULL;
	free_netdev(dev);
error:
	LEAVE();
	return;
}

/**
 *  @brief Configure pmic in firmware
 *
 *  @param handle    A pointer to moal_handle
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_pmic_configure(moal_handle *handle, t_u8 wait_option)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *misc = NULL;
	mlan_status status;

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	/* Allocate an IOCTL request buffer */
	req = (mlan_ioctl_req *)woal_alloc_mlan_ioctl_req(
		sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	misc = (mlan_ds_misc_cfg *)req->pbuf;
	misc->sub_command = MLAN_OID_MISC_PMIC_CFG;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = MLAN_ACT_SET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	kfree(req);
	LEAVE();
	return status;
}

/**
 *  @brief Configure antcfg
 *
 *  @param handle    A pointer to moal_handle
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_set_user_antcfg(moal_handle *handle, t_u8 wait_option)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_radio_cfg *radio = NULL;
	mlan_status status;
	int antcfg;

	ENTER();
	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	antcfg = handle->params.antcfg;
	/* Allocate an IOCTL request buffer */
	req = (mlan_ioctl_req *)woal_alloc_mlan_ioctl_req(
		sizeof(mlan_ds_radio_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	radio = (mlan_ds_radio_cfg *)req->pbuf;
	radio->sub_command = MLAN_OID_ANT_CFG;
	req->req_id = MLAN_IOCTL_RADIO_CFG;
	req->action = MLAN_ACT_SET;

	if (handle->feature_control & FEATURE_CTRL_STREAM_2X2) {
		if (IS_CARD9098(handle->card_type) ||
		    IS_CARD9097(handle->card_type)) {
			radio->param.ant_cfg.tx_antenna =
				radio->param.ant_cfg.rx_antenna = antcfg;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
			woal_cfg80211_notify_antcfg(priv, priv->phandle->wiphy,
						    radio);
#endif
		} else {
			radio->param.ant_cfg.tx_antenna =
				(antcfg & 0x0030) >> 4;
			radio->param.ant_cfg.rx_antenna = antcfg & 0x0003;
		}
	} else
		radio->param.ant_cfg_1x1.antenna = antcfg;
	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	kfree(req);
	LEAVE();
	return status;
}

/**
 *  @brief Configure MLAN for low power mode
 *
 *  @param priv         A pointer to moal_private structure
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_set_low_pwr_mode(moal_handle *handle, t_u8 wait_option)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *misc = NULL;
	mlan_status status;

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	/* Allocate an IOCTL request buffer */
	req = (mlan_ioctl_req *)woal_alloc_mlan_ioctl_req(
		sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	misc = (mlan_ds_misc_cfg *)req->pbuf;
	misc->sub_command = MLAN_OID_MISC_LOW_PWR_MODE;
	misc->param.low_pwr_mode = moal_extflg_isset(handle, EXT_LOW_PW_MODE);
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = MLAN_ACT_SET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	kfree(req);
	LEAVE();
	return status;
}

/**
 *  @brief Configure MLAN for channel tracking mode
 *
 *  @param priv         A pointer to moal_private structure
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_set_chan_track_mode(moal_handle *handle, t_u8 wait_option)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_snmp_mib *mib = NULL;
	mlan_status status;

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	/* Allocate an IOCTL request buffer */
	req = (mlan_ioctl_req *)woal_alloc_mlan_ioctl_req(
		sizeof(mlan_ds_snmp_mib));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	mib = (mlan_ds_snmp_mib *)req->pbuf;
	mib->sub_command = MLAN_OID_SNMP_MIB_CHAN_TRACK;
	mib->param.chan_track = moal_extflg_isset(handle, EXT_CHAN_TRACK);
	req->req_id = MLAN_IOCTL_SNMP_MIB;
	req->action = MLAN_ACT_SET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
done:
	kfree(req);
	LEAVE();
	return status;
}

/**
 *  @brief Send FW shutdown command to MLAN
 *
 *  @param priv         A pointer to moal_private structure
 *  @param wait_option  Wait option
 *
 *  @return             MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING -- success,
 *                          otherwise fail
 */
mlan_status woal_shutdown_fw(moal_private *priv, t_u8 wait_option)
{
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *misc = NULL;
	mlan_status status;

	ENTER();

	/* Allocate an IOCTL request buffer */
	req = (mlan_ioctl_req *)woal_alloc_mlan_ioctl_req(
		sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		status = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	misc = (mlan_ds_misc_cfg *)req->pbuf;
	misc->sub_command = MLAN_OID_MISC_INIT_SHUTDOWN;
	misc->param.func_init_shutdown = MLAN_FUNC_SHUTDOWN;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = MLAN_ACT_SET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, wait_option);
	/* add 100 ms delay to avoid back to back init/shutdown */
	mdelay(100);
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return status;
}

/**
 *  @brief Return hex value of a give character
 *
 *  @param chr      Character to be converted
 *
 *  @return         The converted character if chr is a valid hex, else 0
 */
int woal_hexval(char chr)
{
	if (chr >= '0' && chr <= '9')
		return chr - '0';
	if (chr >= 'A' && chr <= 'F')
		return chr - 'A' + 10;
	if (chr >= 'a' && chr <= 'f')
		return chr - 'a' + 10;

	return 0;
}

#ifdef STA_SUPPORT
#endif

/**
 *  @brief This function flush event queue
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
static void woal_flush_evt_queue(moal_handle *handle)
{
	struct woal_event *evt = NULL, *tmp_node;
	unsigned long flags;
	spin_lock_irqsave(&handle->evt_lock, flags);
	list_for_each_entry_safe (evt, tmp_node, &handle->evt_queue, link) {
		list_del(&evt->link);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		kfree(evt);
		spin_lock_irqsave(&handle->evt_lock, flags);
	}
	INIT_LIST_HEAD(&handle->evt_queue);
	spin_unlock_irqrestore(&handle->evt_lock, flags);
}

/**
 *  @brief This function flush all works in the queue
 *
 *  @param handle    A pointer to moal_handle
 *
 *  @return        N/A
 */
void woal_flush_workqueue(moal_handle *handle)
{
	ENTER();

	/* Terminate main workqueue */
	if (handle->workqueue)
		flush_workqueue(handle->workqueue);
	if (handle->rx_workqueue)
		flush_workqueue(handle->rx_workqueue);
	if (handle->evt_workqueue)
		flush_workqueue(handle->evt_workqueue);
	if (handle->tx_workqueue)
		flush_workqueue(handle->tx_workqueue);
	LEAVE();
}

/**
 *  @brief This function cancel all works in the queue
 *  and destroy the main workqueue.
 *
 *  @param handle    A pointer to moal_handle
 *
 *  @return        N/A
 */
void woal_terminate_workqueue(moal_handle *handle)
{
	ENTER();

	/* Terminate main workqueue */
	if (handle->workqueue) {
		flush_workqueue(handle->workqueue);
		destroy_workqueue(handle->workqueue);
		handle->workqueue = NULL;
	}
	if (handle->rx_workqueue) {
		flush_workqueue(handle->rx_workqueue);
		destroy_workqueue(handle->rx_workqueue);
		handle->rx_workqueue = NULL;
	}
	if (handle->evt_workqueue) {
		woal_flush_evt_queue(handle);
		flush_workqueue(handle->evt_workqueue);
		destroy_workqueue(handle->evt_workqueue);
		handle->evt_workqueue = NULL;
	}
	if (handle->tx_workqueue) {
		flush_workqueue(handle->tx_workqueue);
		destroy_workqueue(handle->tx_workqueue);
		handle->tx_workqueue = NULL;
	}
	LEAVE();
}

/********************************************************
		Global Functions
********************************************************/

/**
 *  @brief This function opens the network device
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        0 --success, otherwise fail
 */
int woal_open(struct net_device *dev)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
#if defined(USB)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34)
	struct usb_interface *intf =
		((struct usb_card_rec *)priv->phandle->card)->intf;
#else
	// struct usb_device *udev = ((struct usb_card_rec
	// *)(priv->phandle->card))->udev;
#endif /* < 2.6.34 */
#endif /* USB_SUSPEND_RESUME */
	t_u8 carrier_on = MFALSE;

	ENTER();

	if (priv->phandle->surprise_removed == MTRUE) {
		PRINTM(MERROR,
		       "open is not allowed in surprise remove state.\n");
		LEAVE();
		return -EFAULT;
	}
#if defined(USB)
	if (IS_USB(priv->phandle->card_type)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34)
		/* Error enabling PM on interface */
		if (usb_autopm_get_interface(intf)) {
			LEAVE();
			return -EIO;
		}
#else
		// atomic_set(&udev->dev.power.usage_count, 1);
#endif /* < 2.6.34 */
	}
#endif /* USB_SUSPEND_RESUME */

#if defined(USB) || defined(SYSKT)
	/* On some systems the device open handler will be called before HW
	   ready.
		Use the following flag check and wait function to work around
	   the issue. */
	if (MTRUE
#ifdef SDIO
	    && !IS_SD(priv->phandle->card_type)
#endif
#ifdef PCIE
	    && !IS_PCIE(priv->phandle->card_type)
#endif
	) {
		int i = 0;

		while ((priv->phandle->hardware_status !=
			HardwareStatusReady) &&
		       (i < MAX_WAIT_DEVICE_READY_COUNT)) {
			i++;
			woal_sched_timeout(100);
		}
		if (i >= MAX_WAIT_DEVICE_READY_COUNT) {
			PRINTM(MFATAL,
			       "HW not ready, wlan_open() return failure\n");
			LEAVE();
			return -EFAULT;
		}
	}
#endif /* USB || SYSKT || SYSKT_MULTI */
	if (!MODULE_GET) {
		LEAVE();
		return -EFAULT;
	}
#ifdef UAP_SUPPORT
	if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) &&
	    (priv->media_connected))
		carrier_on = MTRUE;
#endif
#ifdef STA_SUPPORT
	if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
	    (priv->media_connected || priv->is_adhoc_link_sensed))
		carrier_on = MTRUE;
#endif

	if (carrier_on == MTRUE) {
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_wake_queue(priv->netdev);
	} else {
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
	}

	LEAVE();
	return 0;
}

/**
 *  @brief This function closes the network device
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        0
 */
int woal_close(struct net_device *dev)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
#if defined(USB)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34)
	struct usb_interface *intf =
		((struct usb_card_rec *)priv->phandle->card)->intf;
#else
	// struct usb_device *udev = ((struct usb_card_rec
	// *)(priv->phandle->card))->udev;
#endif /* < 2.6.34 */
#endif /* USB_SUSPEND_RESUME */
#ifdef STA_CFG80211
	int cfg80211_wext = priv->phandle->params.cfg80211_wext;
#endif
	ENTER();

	woal_flush_tx_stat_queue(priv);

#ifdef STA_SUPPORT
#ifdef STA_CFG80211
	if (IS_STA_CFG80211(cfg80211_wext))
		woal_clear_conn_params(priv);
	woal_cancel_scan(priv, MOAL_IOCTL_WAIT);

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || IMX_ANDROID_13)
	if (IS_STA_CFG80211(cfg80211_wext) && priv->wdev->connected) {
#else
	if (IS_STA_CFG80211(cfg80211_wext) && priv->wdev->current_bss) {
#endif
		priv->cfg_disconnect = MTRUE;
		cfg80211_disconnected(priv->netdev, 0, NULL, 0,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
				      true,
#endif
				      GFP_KERNEL);
	}
#endif

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	if (IS_STA_CFG80211(cfg80211_wext) && priv->sched_scanning) {
		woal_stop_bg_scan(priv, MOAL_IOCTL_WAIT);
		priv->bg_scan_start = MFALSE;
		priv->bg_scan_reported = MFALSE;
		cfg80211_sched_scan_stopped(priv->wdev->wiphy
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
					    ,
					    priv->bg_scan_reqid
#endif
		);
		priv->sched_scanning = MFALSE;
	}
#endif
#endif
#endif
	if (!priv->bss_virtual)
		woal_stop_queue(priv->netdev);
	MODULE_PUT;
#if defined(USB)
	if (IS_USB(priv->phandle->card_type)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34)
		usb_autopm_put_interface(intf);
#else
		// atomic_set(&udev->dev.power.usage_count, 0);
#endif /* < 2.6.34 */
	}
#endif /* USB_SUSPEND_RESUME */

	LEAVE();
	return 0;
}

#define DEF_MTU_SIZE 1500
/**
 *  @brief This function disable the ampdu
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param new_mtu  new mtu size
 *
 *  @return        0 --success, otherwise fail
 */
void woal_disable_ampdu(moal_private *priv)
{
	mlan_ds_11n_aggr_prio_tbl aggr_prio_tbl;
	int i;
	memset(&aggr_prio_tbl, 0, sizeof(aggr_prio_tbl));
	if (MLAN_STATUS_SUCCESS !=
	    woal_ioctl_aggr_prio_tbl(priv, MLAN_ACT_GET, &aggr_prio_tbl)) {
		goto done;
	}
	for (i = 0; i < MAX_NUM_TID; i++)
		aggr_prio_tbl.ampdu[i] = 0xff;
	if (MLAN_STATUS_SUCCESS !=
	    woal_ioctl_aggr_prio_tbl(priv, MLAN_ACT_SET, &aggr_prio_tbl)) {
		goto done;
	}
done:
	return;
}

/**
 *  @brief This function change the MTU size
 *
 *  @param dev     A pointer to mlan_private structure
 *  @param new_mtu  new mtu size
 *
 *  @return        0 --success, otherwise fail
 */
int woal_change_mtu(struct net_device *dev, int new_mtu)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	dev->mtu = new_mtu;
	// disable AMPDU with mtu size > 1500
	if (new_mtu > DEF_MTU_SIZE)
		woal_disable_ampdu(priv);
	PRINTM(MCMND, "wlan: change_mtu to %d\n", new_mtu);

	return 0;
}

/**
 *  @brief This function sets the MAC address to firmware.
 *
 *  @param dev     A pointer to mlan_private structure
 *  @param addr    MAC address to set
 *
 *  @return        0 --success, otherwise fail
 */
int woal_set_mac_address(struct net_device *dev, void *addr)
{
	int ret = 0;
	moal_private *priv = (moal_private *)netdev_priv(dev);
	struct sockaddr *phw_addr = (struct sockaddr *)addr;
	t_u8 prev_addr[ETH_ALEN];

	ENTER();

	if (priv->phandle->surprise_removed == MTRUE) {
		PRINTM(MERROR,
		       "Set mac address is not allowed in surprise remove state.\n");
		LEAVE();
		return -EFAULT;
	}

	moal_memcpy_ext(priv->phandle, prev_addr, priv->current_addr, ETH_ALEN,
			ETH_ALEN);
	memset(priv->current_addr, 0, ETH_ALEN);
	/* dev->dev_addr is 6 bytes */
	HEXDUMP("dev->dev_addr:", (t_u8 *)dev->dev_addr, ETH_ALEN);

	HEXDUMP("addr:", (t_u8 *)phw_addr->sa_data, ETH_ALEN);
	moal_memcpy_ext(priv->phandle, priv->current_addr, phw_addr->sa_data,
			ETH_ALEN, ETH_ALEN);
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
	if (priv->bss_type == MLAN_BSS_TYPE_WIFIDIRECT) {
		priv->current_addr[0] |= 0x02;
		PRINTM(MCMND, "Set WFD device addr: " MACSTR "\n",
		       MAC2STR(priv->current_addr));
	}
#endif
#endif
#endif
	if (MLAN_STATUS_SUCCESS !=
	    woal_request_set_mac_address(priv, MOAL_IOCTL_WAIT)) {
		PRINTM(MERROR, "Set MAC address failed\n");
		/* For failure restore the MAC address */
		moal_memcpy_ext(priv->phandle, priv->current_addr, prev_addr,
				ETH_ALEN, ETH_ALEN);
		ret = -EFAULT;
		goto done;
	}
	HEXDUMP("priv->MacAddr:", priv->current_addr, ETH_ALEN);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	eth_hw_addr_set(dev, priv->current_addr);
#else
	moal_memcpy_ext(priv->phandle, dev->dev_addr, priv->current_addr,
			ETH_ALEN, ETH_ALEN);
#endif
done:
	LEAVE();
	return ret;
}

/**
 *  @brief Check driver status
 *
 *  @param handle   A pointer to moal_handle
 *
 *  @return         MTRUE/MFALSE
 */
t_u8 woal_check_driver_status(moal_handle *handle)
{
	moal_private *priv = NULL;
	wifi_timeval t;
	int i = 0;
	mlan_debug_info *info = &(handle->debug_info);

	ENTER();

	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (!priv || woal_get_debug_info(priv, MOAL_IOCTL_WAIT, info)) {
		PRINTM(MERROR,
		       "Could not retrieve debug information from MLAN\n");
		LEAVE();
		return MTRUE;
	}
#define MOAL_CMD_TIMEOUT_MAX 9
#define MOAL_CMD_TIMEOUT 20
	woal_get_monotonic_time(&t);
	if (info->dnld_cmd_in_secs && info->pending_cmd &&
	    (t.time_sec > (info->dnld_cmd_in_secs + MOAL_CMD_TIMEOUT_MAX))) {
		if (t.time_sec > (info->dnld_cmd_in_secs + MOAL_CMD_TIMEOUT) &&
		    !info->num_cmd_timeout) {
			PRINTM(MERROR, "Ignore invalid time, wait=%d\n",
			       (int)(t.time_sec - info->dnld_cmd_in_secs));
		} else {
			PRINTM(MERROR, "Timeout cmd id = 0x%x wait=%d\n",
			       info->pending_cmd,
			       (int)(t.time_sec - info->dnld_cmd_in_secs));
			LEAVE();
			return MTRUE;
		}
	}
	if (info->num_cmd_timeout) {
		PRINTM(MERROR, "num_cmd_timeout = %d\n", info->num_cmd_timeout);
		PRINTM(MERROR, "Timeout cmd id = 0x%x, act = 0x%x\n",
		       info->timeout_cmd_id, info->timeout_cmd_act);
		LEAVE();
		return MTRUE;
	}
	if (info->num_cmd_host_to_card_failure) {
		PRINTM(MERROR, "num_cmd_host_to_card_failure = %d\n",
		       info->num_cmd_host_to_card_failure);
		LEAVE();
		return MTRUE;
	}
	if (info->num_no_cmd_node) {
		PRINTM(MERROR, "num_no_cmd_node = %d\n", info->num_no_cmd_node);
		LEAVE();
		return MTRUE;
	}
	for (i = 0; i < handle->priv_num; i++) {
		priv = handle->priv[i];
		if (priv) {
			if (priv->num_tx_timeout >= NUM_TX_TIMEOUT_THRESHOLD) {
				PRINTM(MERROR, "num_tx_timeout = %d\n",
				       priv->num_tx_timeout);
				LEAVE();
				return MTRUE;
			}
		}
	}
	if (info->pm_wakeup_card_req && info->pm_wakeup_fw_try) {
#define MAX_WAIT_TIME 3
		if (t.time_sec > (info->pm_wakeup_in_secs + MAX_WAIT_TIME)) {
			PRINTM(MERROR,
			       "wakeup_dev_req=%d wakeup_tries=%d wait=%d\n",
			       info->pm_wakeup_card_req, info->pm_wakeup_fw_try,
			       (int)(t.time_sec - info->pm_wakeup_in_secs));
			LEAVE();
			return MTRUE;
		}
	}
	if (info->fw_hang_report) {
		PRINTM(MERROR, "fw_hang_report = %d\n", info->fw_hang_report);
		LEAVE();
		return MTRUE;
	}

	if (priv && priv->phandle->driver_status) {
		LEAVE();
		return MTRUE;
	}

	LEAVE();
	return MFALSE;
}

/**
 *  @brief Display MLAN debug information
 *
 *  @param priv     A pointer to moal_private
 *
 *  @return         N/A
 */
void woal_mlan_debug_info(moal_private *priv)
{
	int i;
#ifdef SDIO
	int j;
	t_u8 mp_aggr_pkt_limit = 0;
#endif
	char str[512] = {0};
	char *s;
	mlan_debug_info *info = NULL;

	ENTER();

	if (!priv || !priv->phandle) {
		PRINTM(MERROR, "priv or priv->phandle is null\n");
		LEAVE();
		return;
	}

	info = &(priv->phandle->debug_info);

	if (woal_get_debug_info(priv, MOAL_IOCTL_WAIT, info)) {
		PRINTM(MERROR,
		       "Could not retrieve debug information from MLAN\n");
		LEAVE();
		return;
	}
	PRINTM(MERROR, "------------mlan_debug_info-------------\n");
	PRINTM(MERROR, "mlan_processing =%d\n", info->mlan_processing);
	PRINTM(MERROR, "main_lock_flag =%d\n", info->main_lock_flag);
	PRINTM(MERROR, "main_process_cnt =%d\n", info->main_process_cnt);
	PRINTM(MERROR, "delay_task_flag =%d\n", info->delay_task_flag);
	PRINTM(MERROR, "mlan_rx_processing =%d\n", info->mlan_rx_processing);
	PRINTM(MERROR, "rx_pkts_queued=%d\n", info->rx_pkts_queued);
	PRINTM(MERROR, "tx_pkts_queued=%d\n", info->tx_pkts_queued);
	PRINTM(MERROR, "fw_hang_report = %d\n", info->fw_hang_report);
	PRINTM(MERROR, "num_cmd_timeout = %d\n", info->num_cmd_timeout);
	PRINTM(MERROR, "Timeout cmd id = 0x%x, act = 0x%x\n",
	       info->timeout_cmd_id, info->timeout_cmd_act);

	PRINTM(MERROR, "last_cmd_index = %d\n", info->last_cmd_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_id[i]);
	PRINTM(MERROR, "last_cmd_id = %s\n", str);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_act[i]);
	PRINTM(MERROR, "last_cmd_act = %s\n", str);
	PRINTM(MERROR, "last_cmd_resp_index = %d\n", info->last_cmd_resp_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_resp_id[i]);
	PRINTM(MERROR, "last_cmd_resp_id = %s\n", str);
	PRINTM(MERROR, "last_event_index = %d\n", info->last_event_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_event[i]);
	PRINTM(MERROR, "last_event = %s", str);

	PRINTM(MERROR, "num_data_h2c_failure = %d\n",
	       info->num_tx_host_to_card_failure);
	PRINTM(MERROR, "num_cmd_h2c_failure = %d\n",
	       info->num_cmd_host_to_card_failure);
	PRINTM(MERROR, "num_alloc_buffer_failure = %d\n",
	       info->num_alloc_buffer_failure);
	PRINTM(MERROR, "num_pkt_dropped = %d\n", info->num_pkt_dropped);

#ifdef SDIO
	if (IS_SD(priv->phandle->card_type)) {
		PRINTM(MERROR, "num_data_c2h_failure = %d\n",
		       info->num_rx_card_to_host_failure);
		PRINTM(MERROR, "num_cmdevt_c2h_failure = %d\n",
		       info->num_cmdevt_card_to_host_failure);
		PRINTM(MERROR, "num_int_read_failure = %d\n",
		       info->num_int_read_failure);
		PRINTM(MERROR, "last_int_status = %d\n", info->last_int_status);

		PRINTM(MERROR, "mp_rd_bitmap=0x%x curr_rd_port=0x%x\n",
		       (unsigned int)info->mp_rd_bitmap, info->curr_rd_port);
		PRINTM(MERROR, "mp_wr_bitmap=0x%x curr_wr_port=0x%x\n",
		       (unsigned int)info->mp_wr_bitmap, info->curr_wr_port);
		PRINTM(MERROR, "mp_data_port_mask=0x%x\n",
		       info->mp_data_port_mask);
		PRINTM(MERROR,
		       "last_recv_rd_bitmap=0x%x mp_invalid_update=%d\n",
		       info->last_recv_rd_bitmap, info->mp_invalid_update);
		mp_aggr_pkt_limit = info->mp_aggr_pkt_limit;
		PRINTM(MERROR, "last_recv_wr_bitmap=0x%x last_mp_index = %d\n",
		       info->last_recv_wr_bitmap, info->last_mp_index);
		for (i = 0; i < SDIO_MP_DBG_NUM; i++) {
			for (s = str, j = 0; j < mp_aggr_pkt_limit; j++)
				s += sprintf(
					s, "0x%02x ",
					info->last_mp_wr_info
						[i * mp_aggr_pkt_limit + j]);

			PRINTM(MERROR,
			       "mp_wr_bitmap: 0x%x mp_wr_ports=0x%x len=%d curr_wr_port=0x%x\n%s\n",
			       info->last_mp_wr_bitmap[i],
			       info->last_mp_wr_ports[i],
			       info->last_mp_wr_len[i],
			       info->last_curr_wr_port[i], str);
		}
	}
#endif
#ifdef PCIE
	if (IS_PCIE(priv->phandle->card_type)) {
		PRINTM(MERROR, "txbd_rdptr=0x%x txbd_wrptr=0x%x\n",
		       info->txbd_rdptr, info->txbd_wrptr);
		PRINTM(MERROR, "rxbd_rdptr=0x%x rxbd_wrptr=0x%x\n",
		       info->rxbd_rdptr, info->rxbd_wrptr);
		PRINTM(MERROR, "eventbd_rdptr=0x%x event_wrptr=0x%x\n",
		       info->eventbd_rdptr, info->eventbd_wrptr);
		PRINTM(MERROR, "last_wr_index:%d\n",
		       info->txbd_wrptr & (info->txrx_bd_size - 1));
		PRINTM(MERROR, "TxRx BD size:%d\n", info->txrx_bd_size);
	}
#endif
	PRINTM(MERROR, "num_event_deauth = %d\n", info->num_event_deauth);
	PRINTM(MERROR, "num_event_disassoc = %d\n", info->num_event_disassoc);
	PRINTM(MERROR, "num_event_link_lost = %d\n", info->num_event_link_lost);
	PRINTM(MERROR, "num_cmd_deauth = %d\n", info->num_cmd_deauth);
	PRINTM(MERROR, "num_cmd_assoc_success = %d\n",
	       info->num_cmd_assoc_success);
	PRINTM(MERROR, "num_cmd_assoc_failure = %d\n",
	       info->num_cmd_assoc_failure);
	PRINTM(MERROR, "num_cons_assoc_failure = %d\n",
	       info->num_cons_assoc_failure);
	PRINTM(MERROR, "cmd_resp_received = %d\n", info->cmd_resp_received);
	PRINTM(MERROR, "event_received = %d\n", info->event_received);

	PRINTM(MERROR, "max_tx_buf_size = %d\n", info->max_tx_buf_size);
	PRINTM(MERROR, "tx_buf_size = %d\n", info->tx_buf_size);
	PRINTM(MERROR, "curr_tx_buf_size = %d\n", info->curr_tx_buf_size);
	PRINTM(MERROR, "bypass_pkt_count=%d\n", info->bypass_pkt_count);

	PRINTM(MERROR, "data_sent=%d cmd_sent=%d\n", info->data_sent,
	       info->cmd_sent);
	PRINTM(MERROR, "data_sent_cnt=%u\n", info->data_sent_cnt);

	PRINTM(MERROR, "ps_mode=%d ps_state=%d\n", info->ps_mode,
	       info->ps_state);
	PRINTM(MERROR, "wakeup_dev_req=%d wakeup_tries=%d wakeup_timeout=%d\n",
	       info->pm_wakeup_card_req, info->pm_wakeup_fw_try,
	       info->pm_wakeup_timeout);
	PRINTM(MERROR, "hs_configured=%d hs_activated=%d\n",
	       info->is_hs_configured, info->hs_activated);
	PRINTM(MERROR, "pps_uapsd_mode=%d sleep_pd=%d\n", info->pps_uapsd_mode,
	       info->sleep_pd);
	PRINTM(MERROR, "tx_lock_flag = %d\n", info->tx_lock_flag);
	PRINTM(MERROR, "port_open = %d\n", info->port_open);
	PRINTM(MERROR, "scan_processing = %d\n", info->scan_processing);
	PRINTM(MERROR, "scan_state = 0x%x\n", info->scan_state);
	for (i = 0; i < (int)info->ralist_num; i++) {
		PRINTM(MERROR,
		       "ralist ra: %02x:%02x:%02x:%02x:%02x:%02x tid=%d pkts=%d pause=%d\n",
		       info->ralist[i].ra[0], info->ralist[i].ra[1],
		       info->ralist[i].ra[2], info->ralist[i].ra[3],
		       info->ralist[i].ra[4], info->ralist[i].ra[5],
		       info->ralist[i].tid, info->ralist[i].total_pkts,
		       info->ralist[i].tx_pause);
	}

#ifdef PCIE
	if (IS_PCIE(priv->phandle->card_type)) {
		PRINTM(MERROR, "txbd: rdptr=0x%x wrptr=0x%x\n",
		       info->txbd_rdptr, info->txbd_wrptr);
		PRINTM(MERROR, "rxbd: rdptr=0x%x wrptr=0x%x\n",
		       info->rxbd_rdptr, info->rxbd_wrptr);
		PRINTM(MERROR, "eventbd: rdptr=0x%x wrptr=0x%x\n",
		       info->eventbd_rdptr, info->eventbd_wrptr);
	}
#endif
	PRINTM(MERROR, "------------mlan_debug_info End-------------\n");
	LEAVE();
}

/**
 *  @brief This function handle the shutdown timeout issue
 *
 *  @param handle   Pointer to structure moal_handle
 *
 *  @return         N/A
 */
static void woal_ioctl_timeout(moal_handle *handle)
{
	moal_private *priv = NULL;

	ENTER();

	PRINTM(MMSG, "woal_ioctl_timout.\n");
	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (priv) {
		woal_mlan_debug_info(priv);
		woal_moal_debug_info(priv, NULL, MFALSE);
	}
	LEAVE();
	return;
}

/**
 *  @brief This function handles the timeout of packet
 *          transmission
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        N/A
 */
void woal_tx_timeout(struct net_device *dev
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
		     ,
		     unsigned int txqueue
#endif
)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	t_u8 auto_fw_dump = MFALSE;
	ENTER();

	priv->num_tx_timeout++;
	PRINTM(MERROR, "%lu : %s (bss=%d): Tx timeout (%d)\n", jiffies,
	       dev->name, priv->bss_index, priv->num_tx_timeout);
	woal_set_trans_start(dev);

	if (priv->num_tx_timeout == NUM_TX_TIMEOUT_THRESHOLD &&
	    priv->txwatchdog_disable == MFALSE) {
#ifdef DEBUG_LEVEL1
		if (drvdbg & MFW_D)
			auto_fw_dump = MTRUE;
#endif
		woal_mlan_debug_info(priv);
		woal_moal_debug_info(priv, NULL, MFALSE);
		priv->phandle->driver_status = MTRUE;
		if (!auto_fw_dump && !priv->phandle->fw_dump)
			woal_process_hang(priv->phandle);

		wifi_status = WIFI_STATUS_TX_TIMEOUT;
	}

	LEAVE();
}

/**
 *  @brief This function returns the network statistics
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        A pointer to net_device_stats structure
 */
struct net_device_stats *woal_get_stats(struct net_device *dev)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	return &priv->stats;
}

#if !defined(STA_CFG80211) && !defined(UAP_CFG80211)
/**
 *  @brief This function determine the 802.1p/1d tag to use
 *
 *  @param skb
 *
 *  @return          tid
 */
unsigned int woal_classify8021d(struct sk_buff *skb)
{
	unsigned int dscp;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
	unsigned char vlan_priority;
#endif
	unsigned int tid;

	/* skb->priority values from 256->263 are magic values to
	 * directly indicate a specific 802.1d priority.  This is used
	 * to allow 802.1d priority to be passed directly in from VLAN
	 * tags, etc.
	 */
	if (skb->priority >= 256 && skb->priority <= 263) {
		tid = skb->priority - 256;
		goto out;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
	if (skb_vlan_tag_present(skb)) {
		vlan_priority = (skb_vlan_tag_get(skb) & VLAN_PRIO_MASK) >>
				VLAN_PRIO_SHIFT;
		if (vlan_priority > 0) {
			tid = vlan_priority;
			goto out;
		}
	}
#endif
	switch (skb->protocol) {
	case htons(ETH_P_IP):
		dscp = ipv4_get_dsfield(ip_hdr(skb)) & 0xfc;
		break;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
	case htons(ETH_P_IPV6):
		dscp = ipv6_get_dsfield(ipv6_hdr(skb)) & 0xfc;
		break;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
	case htons(ETH_P_MPLS_UC):
	case htons(ETH_P_MPLS_MC): {
		struct mpls_label mpls_tmp, *mpls;

		mpls = skb_header_pointer(skb, sizeof(struct ethhdr),
					  sizeof(*mpls), &mpls_tmp);
		if (!mpls)
			return 0;

		tid = (ntohl(mpls->entry) & MPLS_LS_TC_MASK) >>
		      MPLS_LS_TC_SHIFT;
		goto out;
	}
	case htons(ETH_P_80221):
		/* 802.21 is always network control traffic */
		return 7;
#endif
	default:
		return 0;
	}
	tid = dscp >> 5;
out:
	return tid;
}
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
/**
 *  @brief This function handles wmm queue select
 *
 *  @param dev     A pointer to net_device structure
 *  @param skb     A pointer to sk_buff structure
 *
 *  @return        tx_queue index (0-3)
 */
u16 woal_select_queue(struct net_device *dev, struct sk_buff *skb
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
		      ,
		      struct net_device *sb_dev
#else
		      ,
		      void *accel_priv
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0) &&                          \
	LINUX_VERSION_CODE < KERNEL_VERSION(5, 2, 0)
		      ,
		      select_queue_fallback_t fallback
#endif
#endif
)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	t_u8 tid = 0;
	t_u8 index = 0;

	ENTER();
	if (priv->phandle->surprise_removed == MTRUE) {
		LEAVE();
		return index;
	}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	tid = skb->priority = cfg80211_classify8021d(skb, NULL);
#else
	tid = skb->priority = cfg80211_classify8021d(skb);
#endif
#else
	tid = skb->priority = woal_classify8021d(skb);
#endif
#define NXP_ETH_P_EAPOL 0x888E
#define NXP_ETH_P_WAPI 0x8884
	switch (skb->protocol) {
	case htons(ETH_P_ARP):
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	case htons(ETH_P_TDLS):
#endif
	case htons(NXP_ETH_P_EAPOL):
	case htons(NXP_ETH_P_WAPI):
		tid = skb->priority = 7;
		break;
	}
	index = mlan_select_wmm_queue(priv->phandle->pmlan_adapter,
				      priv->bss_index, tid);
	PRINTM(MDATA, "select queue: tid=%d, index=%d\n", tid, index);
	LEAVE();
	return index;
}
#endif

/**
 *  @brief This function flush tx status queue
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
void woal_flush_tx_stat_queue(moal_private *priv)
{
	struct tx_status_info *tx_info = NULL, *tmp_node;
	unsigned long flags;
	struct sk_buff *skb = NULL;
	spin_lock_irqsave(&priv->tx_stat_lock, flags);
	list_for_each_entry_safe (tx_info, tmp_node, &priv->tx_stat_queue,
				  link) {
		list_del(&tx_info->link);
		spin_unlock_irqrestore(&priv->tx_stat_lock, flags);
		skb = (struct sk_buff *)tx_info->tx_skb;
		if (tx_info->tx_cookie) {
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
			cfg80211_mgmt_tx_status(priv->netdev,
						tx_info->tx_cookie, skb->data,
						skb->len, true, GFP_ATOMIC);
#else
			cfg80211_mgmt_tx_status(priv->wdev, tx_info->tx_cookie,
						skb->data, skb->len, true,
						GFP_ATOMIC);
#endif
#endif
#endif
		}
		dev_kfree_skb_any(skb);
		kfree(tx_info);
		spin_lock_irqsave(&priv->tx_stat_lock, flags);
	}
	INIT_LIST_HEAD(&priv->tx_stat_queue);
	spin_unlock_irqrestore(&priv->tx_stat_lock, flags);
	spin_lock_bh(&(priv->tx_q.lock));
	__skb_queue_purge(&priv->tx_q);
	spin_unlock_bh(&(priv->tx_q.lock));
}

/**
 *  @brief This function gets tx info from tx_stat_queue
 *
 *  @param priv      	A pointer to moal_private structure
 *  @param tx_seq_num   tx seq number
 *
 *  @return          A pointer to the tcp tx_status_info structure, if found.
 *                   Otherwise, null
 */
struct tx_status_info *woal_get_tx_info(moal_private *priv, t_u8 tx_seq_num)
{
	struct tx_status_info *tx_info = NULL;
	ENTER();

	list_for_each_entry (tx_info, &priv->tx_stat_queue, link) {
		if (tx_info->tx_seq_num == tx_seq_num) {
			LEAVE();
			return tx_info;
		}
	}
	LEAVE();
	return NULL;
}

/**
 *  @brief This function remove tx info from queue
 *
 *  @param priv      		A pointer to moal_private structure
 *  @param tx_seq_num           tx seq number
 *
 *  @return	         N/A
 */
void woal_remove_tx_info(moal_private *priv, t_u8 tx_seq_num)
{
	struct tx_status_info *tx_info, *tmp = NULL;
	unsigned long flags;
	ENTER();

	spin_lock_irqsave(&priv->tx_stat_lock, flags);
	list_for_each_entry_safe (tx_info, tmp, &priv->tx_stat_queue, link) {
		if (tx_info->tx_seq_num == tx_seq_num) {
			list_del(&tx_info->link);
			dev_kfree_skb_any((struct sk_buff *)tx_info->tx_skb);
			kfree(tx_info);
			break;
		}
	}
	spin_unlock_irqrestore(&priv->tx_stat_lock, flags);

	LEAVE();
}

/**
 *  @brief This function flush mcast list
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
void woal_flush_mcast_list(moal_private *priv)
{
	struct mcast_node *node = NULL, *tmp_node;
	unsigned long flags;
	spin_lock_irqsave(&priv->mcast_lock, flags);
	list_for_each_entry_safe (node, tmp_node, &priv->mcast_list, link) {
		list_del(&node->link);
		kfree(node);
	}
	INIT_LIST_HEAD(&priv->mcast_list);
	priv->num_mcast_addr = 0;
	spin_unlock_irqrestore(&priv->mcast_lock, flags);
}

/**
 *  @brief  find mcast node from tx packet
 *
 *  @param priv      A pointer to moal_private structure
 *  @param skb       A pointer to skb buffer.
 *
 *  @return          N/A
 */
t_u8 woal_find_mcast_node_tx(moal_private *priv, struct sk_buff *skb)
{
	struct mcast_node *node = NULL;
	unsigned long flags;
	t_u8 ret = MFALSE;
	t_u8 ra[MLAN_MAC_ADDR_LENGTH];
	ENTER();
	moal_memcpy_ext(priv->phandle, ra, skb->data, MLAN_MAC_ADDR_LENGTH,
			sizeof(ra));
	if (ra[0] & 0x01) {
		spin_lock_irqsave(&priv->mcast_lock, flags);
		list_for_each_entry (node, &priv->mcast_list, link) {
			if (!memcmp(node->mcast_addr, ra, ETH_ALEN)) {
				ret = MTRUE;
				break;
			}
		}
		spin_unlock_irqrestore(&priv->mcast_lock, flags);
	}
	LEAVE();
	return ret;
}

/**
 * @brief add mcast node
 *
 * @param priv                  A pointer to moal_private structure
 * @param peer                  A point to peer address
 *
 * @return                      N/A
 */
t_void woal_add_mcast_node(moal_private *priv, t_u8 *mcast_addr)
{
	struct mcast_node *node = NULL;
	unsigned long flags;
	t_u8 find_node = MFALSE;
	if (priv) {
		spin_lock_irqsave(&priv->mcast_lock, flags);
		list_for_each_entry (node, &priv->mcast_list, link) {
			if (!memcmp(node->mcast_addr, mcast_addr, ETH_ALEN)) {
				find_node = MTRUE;
				break;
			}
		}
		if (!find_node) {
			/* create new mcast node */
			node = kzalloc(sizeof(struct mcast_node), GFP_ATOMIC);
			if (node) {
				moal_memcpy_ext(priv->phandle, node->mcast_addr,
						mcast_addr, ETH_ALEN, ETH_ALEN);
				INIT_LIST_HEAD(&node->link);
				list_add_tail(&node->link, &priv->mcast_list);
				PRINTM(MCMND,
				       "Add to mcast list: node=" MACSTR "\n",
				       MAC2STR(mcast_addr));
			}
		}
		spin_unlock_irqrestore(&priv->mcast_lock, flags);
	}
}

/**
 *  @brief This function remove mcast node
 *
 *  @param priv      		A pointer to moal_private structure
 *  @param mcast_addr       mcast address
 *
 *  @return	         N/A
 */
void woal_remove_mcast_node(moal_private *priv, t_u8 *mcast_addr)
{
	struct mcast_node *node, *tmp = NULL;
	unsigned long flags;
	ENTER();

	spin_lock_irqsave(&priv->mcast_lock, flags);
	list_for_each_entry_safe (node, tmp, &priv->mcast_list, link) {
		if (!memcmp(node->mcast_addr, mcast_addr, ETH_ALEN)) {
			list_del(&node->link);
			kfree(node);
			break;
		}
	}
	spin_unlock_irqrestore(&priv->mcast_lock, flags);

	LEAVE();
}

#ifdef STA_CFG80211
/**
 *  @brief This function flush tcp session queue
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
void woal_flush_tdls_list(moal_private *priv)
{
	struct tdls_peer *peer = NULL, *tmp_node;
	unsigned long flags;
	spin_lock_irqsave(&priv->tdls_lock, flags);
	list_for_each_entry_safe (peer, tmp_node, &priv->tdls_list, link) {
		list_del(&peer->link);
		kfree(peer);
	}
	INIT_LIST_HEAD(&priv->tdls_list);
	spin_unlock_irqrestore(&priv->tdls_lock, flags);
	priv->tdls_check_tx = MFALSE;
}

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
/**
 *  @brief  check the tx packet for tdls auto set up
 *
 *  @param priv      A pointer to moal_private structure
 *  @param skb       A pointer to skb buffer.
 *
 *  @return          N/A
 */
static void woal_tdls_check_tx(moal_private *priv, struct sk_buff *skb)
{
	struct tdls_peer *peer = NULL;
	unsigned long flags;
	t_u8 ra[MLAN_MAC_ADDR_LENGTH];
	ENTER();
	moal_memcpy_ext(priv->phandle, ra, skb->data, MLAN_MAC_ADDR_LENGTH,
			sizeof(ra));
	spin_lock_irqsave(&priv->tdls_lock, flags);
	list_for_each_entry (peer, &priv->tdls_list, link) {
		if (!memcmp(peer->peer_addr, ra, ETH_ALEN)) {
			if (peer->rssi &&
			    (peer->rssi <= TDLS_RSSI_HIGH_THRESHOLD)) {
				if ((peer->link_status == TDLS_NOT_SETUP) &&
				    (peer->num_failure <
				     TDLS_MAX_FAILURE_COUNT)) {
					peer->link_status =
						TDLS_SETUP_INPROGRESS;
					PRINTM(MMSG,
					       "Wlan: Set up TDLS link,peer=" MACSTR
					       " rssi=%d\n",
					       MAC2STR(peer->peer_addr),
					       -peer->rssi);
					cfg80211_tdls_oper_request(
						priv->netdev, peer->peer_addr,
						NL80211_TDLS_SETUP, 0,
						GFP_ATOMIC);
					priv->tdls_check_tx = MFALSE;
				}
			}
			break;
		}
	}
	spin_unlock_irqrestore(&priv->tdls_lock, flags);
	LEAVE();
}
#endif
#endif

/**
 *  @brief This function flush tcp session queue
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
void woal_flush_tcp_sess_queue(moal_private *priv)
{
	struct tcp_sess *tcp_sess = NULL, *tmp_node;
	unsigned long flags;
	struct sk_buff *skb;
	spin_lock_irqsave(&priv->tcp_sess_lock, flags);
	list_for_each_entry_safe (tcp_sess, tmp_node, &priv->tcp_sess_queue,
				  link) {
		list_del(&tcp_sess->link);
		if (atomic_read(&tcp_sess->is_timer_set))
			woal_cancel_timer(&tcp_sess->ack_timer);
		skb = (struct sk_buff *)tcp_sess->ack_skb;
		if (skb)
			dev_kfree_skb_any(skb);
		kfree(tcp_sess);
	}
	INIT_LIST_HEAD(&priv->tcp_sess_queue);
	priv->tcp_ack_drop_cnt = 0;
	priv->tcp_ack_cnt = 0;
	spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
}

/**
 *  @brief This function gets tcp session from the tcp session queue
 *
 *  @param priv      A pointer to moal_private structure
 *  @param src_ip    IP address of the device
 *  @param src_port  TCP port of the device
 *  @param dst_ip    IP address of the client
 *  @param dst_port  TCP port of the client
 *
 *  @return          A pointer to the tcp session data structure, if found.
 *                   Otherwise, null
 */
static inline struct tcp_sess *woal_get_tcp_sess(moal_private *priv,
						 t_u32 src_ip, t_u16 src_port,
						 t_u32 dst_ip, t_u16 dst_port)
{
	struct tcp_sess *tcp_sess = NULL;
	ENTER();

	list_for_each_entry (tcp_sess, &priv->tcp_sess_queue, link) {
		if ((tcp_sess->src_ip_addr == src_ip) &&
		    (tcp_sess->src_tcp_port == src_port) &&
		    (tcp_sess->dst_ip_addr == dst_ip) &&
		    (tcp_sess->dst_tcp_port == dst_port)) {
			LEAVE();
			return tcp_sess;
		}
	}
	LEAVE();
	return NULL;
}

#define TCP_SESS_AGEOUT 300
/**
 *  @brief This function flush tcp session queue
 *
 *  @param priv      A pointer to moal_private structure
 *
 *  @return          N/A
 */
static void woal_ageout_tcp_sess_queue(moal_private *priv)
{
	struct tcp_sess *tcp_sess = NULL, *tmp_node;
	wifi_timeval t;
	struct sk_buff *skb;
	woal_get_monotonic_time(&t);
	list_for_each_entry_safe (tcp_sess, tmp_node, &priv->tcp_sess_queue,
				  link) {
		if (t.time_sec >
		    (tcp_sess->update_time.time_sec + TCP_SESS_AGEOUT)) {
			PRINTM(MDATA, "wlan: ageout TCP seesion %p\n",
			       tcp_sess);
			list_del(&tcp_sess->link);
			if (atomic_read(&tcp_sess->is_timer_set))
				woal_cancel_timer(&tcp_sess->ack_timer);
			skb = (struct sk_buff *)tcp_sess->ack_skb;
			if (skb)
				dev_kfree_skb_any(skb);
			kfree(tcp_sess);
		}
	}
}

/**
 *  @brief This function send the holding tcp ack packet
 *  re-assoc thread.
 *
 *  @param context  A pointer to context
 *  @return         N/A
 */
static void woal_tcp_ack_timer_func(void *context)
{
	struct tcp_sess *tcp_session = (struct tcp_sess *)context;
	moal_private *priv = (moal_private *)tcp_session->priv;
	unsigned long flags;
	mlan_buffer *pmbuf;
	struct sk_buff *skb;
	mlan_status status;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	t_u32 index = 0;
#endif
	ENTER();
	spin_lock_irqsave(&priv->tcp_sess_lock, flags);
	atomic_set(&tcp_session->is_timer_set, MFALSE);
	skb = (struct sk_buff *)tcp_session->ack_skb;
	pmbuf = (mlan_buffer *)tcp_session->pmbuf;
	tcp_session->ack_skb = NULL;
	tcp_session->pmbuf = NULL;
	spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
	if (skb && pmbuf) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
		index = skb_get_queue_mapping(skb);
#endif
		status = mlan_send_packet(priv->phandle->pmlan_adapter, pmbuf);
		switch (status) {
		case MLAN_STATUS_PENDING:
			atomic_inc(&priv->phandle->tx_pending);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
			atomic_inc(&priv->wmm_tx_pending[index]);
			if (atomic_read(&priv->wmm_tx_pending[index]) >=
			    MAX_TX_PENDING) {
				struct netdev_queue *txq = netdev_get_tx_queue(
					priv->netdev, index);
				netif_tx_stop_queue(txq);
				moal_tp_accounting_rx_param(
					(t_void *)priv->phandle, 8, 0);
				PRINTM(MINFO, "Stop Kernel Queue : %d\n",
				       index);
			}
#else
			if (atomic_read(&priv->phandle->tx_pending) >=
			    MAX_TX_PENDING)
				woal_stop_queue(priv->netdev);
#endif /*#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29)*/
			queue_work(priv->phandle->workqueue,
				   &priv->phandle->main_work);
			break;
		case MLAN_STATUS_SUCCESS:
			priv->stats.tx_packets++;
			priv->stats.tx_bytes += skb->len;
			dev_kfree_skb_any(skb);
			break;
		case MLAN_STATUS_FAILURE:
		default:
			priv->stats.tx_dropped++;
			dev_kfree_skb_any(skb);
			break;
		}
	}
	LEAVE();
	return;
}

/**
 *  @brief This function send the tcp ack
 *
 *
 *  @param priv         A pointer to moal_private structure
 *  @param tcp_session  A pointer to tcp_session
 *  @return         N/A
 */
static void woal_send_tcp_ack(moal_private *priv, struct tcp_sess *tcp_session)
{
	mlan_status status;
	struct sk_buff *skb = (struct sk_buff *)tcp_session->ack_skb;
	mlan_buffer *pmbuf = (mlan_buffer *)tcp_session->pmbuf;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	t_u32 index = 0;
#endif
	ENTER();
	if (atomic_cmpxchg(&tcp_session->is_timer_set, MTRUE, MFALSE)) {
		woal_cancel_timer(&tcp_session->ack_timer);
	}
	tcp_session->ack_skb = NULL;
	tcp_session->pmbuf = NULL;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	index = skb_get_queue_mapping(skb);
#endif
	status = mlan_send_packet(priv->phandle->pmlan_adapter, pmbuf);
	switch (status) {
	case MLAN_STATUS_PENDING:
		atomic_inc(&priv->phandle->tx_pending);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
		atomic_inc(&priv->wmm_tx_pending[index]);
		if (atomic_read(&priv->wmm_tx_pending[index]) >=
		    MAX_TX_PENDING) {
			struct netdev_queue *txq =
				netdev_get_tx_queue(priv->netdev, index);
			netif_tx_stop_queue(txq);
			moal_tp_accounting_rx_param((t_void *)priv->phandle, 8,
						    0);
			PRINTM(MINFO, "Stop Kernel Queue : %d\n", index);
		}
#else
		if (atomic_read(&priv->phandle->tx_pending) >= MAX_TX_PENDING)
			woal_stop_queue(priv->netdev);
#endif /*#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29)*/
		queue_work(priv->phandle->workqueue, &priv->phandle->main_work);
		break;
	case MLAN_STATUS_SUCCESS:
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += skb->len;
		dev_kfree_skb_any(skb);
		break;
	case MLAN_STATUS_FAILURE:
	default:
		priv->stats.tx_dropped++;
		dev_kfree_skb_any(skb);
		break;
	}
	LEAVE();
}

/**
 *  @brief This function get the tcp ack session node
 *
 *  @param priv      A pointer to moal_private structure
 *  @param pmbuf     A pointer to mlan_buffer associated with a skb
 *
 *  @return          1, if it's dropped; 2, if it's hold 0, if not dropped and
 * not hold
 *
 */
static int woal_process_tcp_ack(moal_private *priv, mlan_buffer *pmbuf)
{
	int ret = 0;
	unsigned long flags;
	struct tcp_sess *tcp_session;
	struct ethhdr *ethh = NULL;
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	t_u32 ack_seq;
	struct sk_buff *skb;

	ENTER();

	/** check the tcp packet */
	ethh = (struct ethhdr *)(pmbuf->pbuf + pmbuf->data_offset);
	if (ntohs(ethh->h_proto) != ETH_P_IP) {
		LEAVE();
		return 0;
	}
	iph = (struct iphdr *)((t_u8 *)ethh + sizeof(struct ethhdr));
	if (iph->protocol != IPPROTO_TCP) {
		LEAVE();
		return 0;
	}
	tcph = (struct tcphdr *)((t_u8 *)iph + iph->ihl * 4);

	if (*((t_u8 *)tcph + 13) == 0x10) {
		/* Only replace ACK */
		if (ntohs(iph->tot_len) > (iph->ihl + tcph->doff) * 4) {
			priv->tcp_ack_payload++;
			/* Don't drop ACK with payload */
			/* TODO: should we delete previous TCP session */
			LEAVE();
			return ret;
		}
		priv->tcp_ack_cnt++;
		spin_lock_irqsave(&priv->tcp_sess_lock, flags);
		tcp_session = woal_get_tcp_sess(priv, (__force t_u32)iph->saddr,
						(__force t_u16)tcph->source,
						(__force t_u32)iph->daddr,
						(__force t_u16)tcph->dest);
		if (!tcp_session) {
			/* check any aging out sessions can be removed */
			woal_ageout_tcp_sess_queue(priv);

			tcp_session =
				kmalloc(sizeof(struct tcp_sess), GFP_ATOMIC);
			if (!tcp_session) {
				PRINTM(MERROR, "Fail to allocate tcp_sess.\n");
				spin_unlock_irqrestore(&priv->tcp_sess_lock,
						       flags);
				goto done;
			}
			woal_get_monotonic_time(&tcp_session->update_time);
			PRINTM(MDATA, "wlan: create TCP seesion %p\n",
			       tcp_session);

			tcp_session->ack_skb = pmbuf->pdesc;
			tcp_session->pmbuf = pmbuf;
			pmbuf->flags |= MLAN_BUF_FLAG_TCP_ACK;
			tcp_session->src_ip_addr = (__force t_u32)iph->saddr;
			tcp_session->dst_ip_addr = (__force t_u32)iph->daddr;
			tcp_session->src_tcp_port = (__force t_u32)tcph->source;
			tcp_session->dst_tcp_port = (__force t_u32)tcph->dest;
			tcp_session->ack_seq = ntohl(tcph->ack_seq);
			tcp_session->priv = (void *)priv;
			skb = (struct sk_buff *)pmbuf->pdesc;
			skb->cb[0] = 0;
			/* Initialize the timer for tcp ack */
			woal_initialize_timer(&tcp_session->ack_timer,
					      woal_tcp_ack_timer_func,
					      tcp_session);
			atomic_set(&tcp_session->is_timer_set, MTRUE);
			woal_mod_timer(&tcp_session->ack_timer, MOAL_TIMER_1MS);
			list_add_tail(&tcp_session->link,
				      &priv->tcp_sess_queue);
			spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
			ret = HOLD_TCP_ACK;
			LEAVE();
			return ret;
		} else if (!tcp_session->ack_skb) {
			woal_get_monotonic_time(&tcp_session->update_time);
			tcp_session->ack_skb = pmbuf->pdesc;
			tcp_session->pmbuf = pmbuf;
			pmbuf->flags |= MLAN_BUF_FLAG_TCP_ACK;
			tcp_session->ack_seq = ntohl(tcph->ack_seq);
			tcp_session->priv = (void *)priv;
			skb = (struct sk_buff *)pmbuf->pdesc;
			skb->cb[0] = 0;
			atomic_set(&tcp_session->is_timer_set, MTRUE);
			woal_mod_timer(&tcp_session->ack_timer, MOAL_TIMER_1MS);
			spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
			ret = HOLD_TCP_ACK;
			LEAVE();
			return ret;
		}
		woal_get_monotonic_time(&tcp_session->update_time);
		ack_seq = ntohl(tcph->ack_seq);
		skb = (struct sk_buff *)tcp_session->ack_skb;
		if (likely(ack_seq > tcp_session->ack_seq) &&
		    (skb->len == pmbuf->data_len)) {
			moal_memcpy_ext(priv->phandle, skb->data,
					pmbuf->pbuf + pmbuf->data_offset,
					pmbuf->data_len, skb->len);
			tcp_session->ack_seq = ack_seq;
			ret = DROP_TCP_ACK;
			skb->cb[0]++;
			if (skb->cb[0] >= priv->tcp_ack_max_hold)
				woal_send_tcp_ack(priv, tcp_session);
			spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
			skb = (struct sk_buff *)pmbuf->pdesc;
			dev_kfree_skb_any(skb);
			priv->tcp_ack_drop_cnt++;
		} else {
			pmbuf->flags |= MLAN_BUF_FLAG_TCP_ACK;
			spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
			LEAVE();
			return ret;
		}
	}
#if 0
	/* Might have race conditions with woal_tcp_ack_timer_func
	 * Causing kernel panic, ageout handler will free tcp_sess
	 * for now.
	 */
	else if((*((t_u8 *)tcph + 13) & 0x11) == 0x11){
		/* TCP ACK + Fin */
		spin_lock_irqsave(&priv->tcp_sess_lock, flags);
		tcp_session = woal_get_tcp_sess(priv, (__force t_u32)iph->saddr, (__force t_u16)tcph->source,
			(__force t_u32)iph->daddr, (__force t_u16)tcph->dest);
		if (tcp_session) {
			PRINTM(MDATA,"wlan: delete TCP seesion %p\n",tcp_session);
			list_del(&tcp_session->link);
			if (atomic_read(&tcp_session->is_timer_set))
				woal_cancel_timer(&tcp_session->ack_timer);
			skb = (struct sk_buff *)tcp_session->ack_skb;
			if (skb)
				dev_kfree_skb_any(skb);
			kfree(tcp_session);
		}
		spin_unlock_irqrestore(&priv->tcp_sess_lock, flags);
 	}
#endif

done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function handles packet transmission
 *
 *  @param skb     A pointer to sk_buff structure
 *  @param dev     A pointer to net_device structure
 *
 *  @return        N/A
 */
static void woal_start_xmit(moal_private *priv, struct sk_buff *skb)
{
	mlan_buffer *pmbuf = NULL;
	mlan_status status;
	struct sk_buff *new_skb = NULL;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	t_u32 index = 0;
#endif
	int ret = 0;

	ENTER();

	priv->num_tx_timeout = 0;
	if (!skb->len ||
	    (skb->len > (priv->netdev->mtu + sizeof(struct ethhdr)))) {
		PRINTM(MERROR, "Tx Error: Bad skb length %d : %d\n", skb->len,
		       priv->netdev->mtu);
		dev_kfree_skb_any(skb);
		priv->stats.tx_dropped++;
		goto done;
	}
	// kernel crash with cloned skb without copy
	// 2 AGO case
	// uap0  <-->muap0 bridge
	if (moal_extflg_isset(priv->phandle, EXT_TX_SKB_CLONE) || skb->cloned ||
	    (skb_headroom(skb) <
	     (MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
	      priv->extra_tx_head_len))) {
		PRINTM(MINFO,
		       "Tx: skb cloned %d skb headroom %d tx_skb_clone=%d \n",
		       skb->cloned, skb_headroom(skb),
		       moal_extflg_isset(priv->phandle, EXT_TX_SKB_CLONE));
		/* Insufficient skb headroom - allocate a new skb */
		new_skb = skb_realloc_headroom(
			skb, MLAN_MIN_DATA_HEADER_LEN + sizeof(mlan_buffer) +
				     priv->extra_tx_head_len);
		moal_tp_accounting_rx_param((t_void *)priv->phandle, 7, 0);
		if (unlikely(!new_skb)) {
			PRINTM(MERROR, "Tx: Cannot allocate skb\n");
			dev_kfree_skb_any(skb);
			priv->stats.tx_dropped++;
			goto done;
		}
		if (new_skb != skb)
			dev_kfree_skb_any(skb);
		skb = new_skb;

		PRINTM(MINFO, "new skb headroom %d\n", skb_headroom(skb));
	}
	pmbuf = (mlan_buffer *)skb->head;
	memset((t_u8 *)pmbuf, 0, sizeof(mlan_buffer));
	pmbuf->bss_index = priv->bss_index;
	woal_fill_mlan_buffer(priv, pmbuf, skb);

	if (priv->enable_tcp_ack_enh == MTRUE) {
		ret = woal_process_tcp_ack(priv, pmbuf);
		if (ret)
			goto done;
	}
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
	if (priv->enable_auto_tdls && priv->tdls_check_tx)
		woal_tdls_check_tx(priv, skb);
#endif
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	index = skb_get_queue_mapping(skb);
#endif

	if (is_zero_timeval(priv->phandle->tx_time_start)) {
		priv->phandle->tx_time_start.time_sec = pmbuf->in_ts_sec;
		priv->phandle->tx_time_start.time_usec = pmbuf->in_ts_usec;
		PRINTM(MINFO, "%s : start_timeval=%d:%d \n", __func__,
		       priv->phandle->tx_time_start.time_sec,
		       priv->phandle->tx_time_start.time_usec);
	}
	status = mlan_send_packet(priv->phandle->pmlan_adapter, pmbuf);
	switch (status) {
	case MLAN_STATUS_PENDING:
		atomic_inc(&priv->phandle->tx_pending);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
		atomic_inc(&priv->wmm_tx_pending[index]);
		if (atomic_read(&priv->wmm_tx_pending[index]) >=
		    MAX_TX_PENDING) {
			struct netdev_queue *txq =
				netdev_get_tx_queue(priv->netdev, index);
			netif_tx_stop_queue(txq);
			moal_tp_accounting_rx_param((t_void *)priv->phandle, 8,
						    0);
			PRINTM(MINFO, "Stop Kernel Queue : %d\n", index);
		}
#else
		if (atomic_read(&priv->phandle->tx_pending) >= MAX_TX_PENDING)
			woal_stop_queue(priv->netdev);
#endif /*#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29)*/

		if (!mlan_is_main_process_running(priv->phandle->pmlan_adapter))
			queue_work(priv->phandle->workqueue,
				   &priv->phandle->main_work);
		break;
	case MLAN_STATUS_SUCCESS:
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += skb->len;
		dev_kfree_skb_any(skb);
		break;
	case MLAN_STATUS_FAILURE:
	default:
		priv->stats.tx_dropped++;
		dev_kfree_skb_any(skb);
		break;
	}
done:
	LEAVE();
	return;
}

/**
 *  @brief This function handles packet transmission
 *
 *  @param skb     A pointer to sk_buff structure
 *  @param dev     A pointer to net_device structure
 *
 *  @return        0 --success
 */
netdev_tx_t woal_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	ENTER();
	PRINTM(MDATA, "%lu : %s (bss=%d): Data <= kernel\n", jiffies, dev->name,
	       priv->bss_index);

	/* Collect TP statistics */
	if (priv->phandle->tp_acnt.on)
		moal_tp_accounting(priv->phandle, skb, 1);
	/* Drop Tx packets at drop point 1 */
	if (priv->phandle->tp_acnt.drop_point == 1) {
		dev_kfree_skb_any(skb);
		LEAVE();
		return 0;
	}
	if (priv->phandle->surprise_removed == MTRUE) {
		dev_kfree_skb_any(skb);
		priv->stats.tx_dropped++;
		goto done;
	}
	if (moal_extflg_isset(priv->phandle, EXT_TX_WORK)) {
		spin_lock_bh(&(priv->tx_q.lock));
		__skb_queue_tail(&(priv->tx_q), skb);
		spin_unlock_bh(&(priv->tx_q.lock));

		queue_work(priv->phandle->tx_workqueue,
			   &priv->phandle->tx_work);
		goto done;
	}
	woal_start_xmit(priv, skb);
done:
	LEAVE();
	return 0;
}

/**
 *  @brief Convert ascii string to Hex integer
 *
 *  @param d        A pointer to integer buf
 *  @param s        A pointer to ascii string
 *  @param dlen     The byte number of ascii string in hex
 *
 *  @return         Number of integer
 */
int woal_ascii2hex(t_u8 *d, char *s, t_u32 dlen)
{
	unsigned int i;
	t_u8 n;

	ENTER();

	memset(d, 0x00, dlen);

	for (i = 0; i < dlen * 2; i++) {
		if ((s[i] >= 48) && (s[i] <= 57))
			n = s[i] - 48;
		else if ((s[i] >= 65) && (s[i] <= 70))
			n = s[i] - 55;
		else if ((s[i] >= 97) && (s[i] <= 102))
			n = s[i] - 87;
		else
			break;
		if (!(i % 2))
			n = n * 16;
		d[i / 2] += n;
	}

	LEAVE();
	return i;
}

/**
 *  @brief Return integer value of a given ascii string
 *
 *  @param data    Converted data to be returned
 *  @param a       String to be converted
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_atoi(int *data, char *a)
{
	int i, val = 0, len;
	int mul = 1;

	ENTER();

	len = strlen(a);
	if (len > 2) {
		if (!strncmp(a, "0x", 2)) {
			a = a + 2;
			len -= 2;
			*data = woal_atox(a);
			LEAVE();
			return MLAN_STATUS_SUCCESS;
		}
	}
	for (i = 0; i < len; i++) {
		if (isdigit(a[i])) {
			val = val * 10 + (a[i] - '0');
		} else {
			if ((i == 0) && (a[i] == '-')) {
				mul = -1;
			} else if (a[i] == 0xa) {
				// line feed
				break;
			} else {
				PRINTM(MERROR, "Invalid char %c in string %s\n",
				       a[i], a);
				LEAVE();
				return MLAN_STATUS_FAILURE;
			}
		}
	}
	*data = (mul * val);

	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Return hex value of a given ascii string
 *
 *  @param a        String to be converted to ascii
 *
 *  @return         The converted character if a is a valid hex, else 0
 */
int woal_atox(char *a)
{
	int i = 0;

	ENTER();

	while (isxdigit(*a))
		i = i * 16 + woal_hexval(*a++);

	LEAVE();
	return i;
}

/**
 *  @brief Extension of strsep lib command. This function will also take care
 *      escape character
 *
 *  @param s         A pointer to array of chars to process
 *  @param delim     The delimiter character to end the string
 *  @param esc       The escape character to ignore for delimiter
 *
 *  @return          Pointer to the separated string if delim found, else NULL
 */
char *woal_strsep(char **s, char delim, char esc)
{
	char *se = *s, *sb;

	ENTER();

	if (!(*s) || (*se == '\0')) {
		LEAVE();
		return NULL;
	}

	for (sb = *s; *sb != '\0'; ++sb) {
		if (*sb == esc && *(sb + 1) == esc) {
			/*
			 * We get a esc + esc seq then keep the one esc
			 * and chop off the other esc character
			 */
			memmove(sb, sb + 1, strlen(sb));
			continue;
		}
		if (*sb == esc && *(sb + 1) == delim) {
			/*
			 * We get a delim + esc seq then keep the delim
			 * and chop off the esc character
			 */
			memmove(sb, sb + 1, strlen(sb));
			continue;
		}
		if (*sb == delim)
			break;
	}

	if (*sb == '\0')
		sb = NULL;
	else
		*sb++ = '\0';

	*s = sb;

	LEAVE();
	return se;
}

/**
 *  @brief Convert mac address from string to t_u8 buffer.
 *
 *  @param mac_addr The buffer to store the mac address in.
 *  @param buf      The source of mac address which is a string.
 *
 *  @return         N/A
 */
void woal_mac2u8(t_u8 *mac_addr, char *buf)
{
	char *begin, *end, *mac_buff;
	int i;

	ENTER();

	if (!buf) {
		LEAVE();
		return;
	}

	mac_buff = kzalloc(strlen(buf) + 1, GFP_KERNEL);
	if (!mac_buff) {
		LEAVE();
		return;
	}
	moal_memcpy_ext(NULL, mac_buff, buf, strlen(buf), strlen(buf) + 1);

	begin = mac_buff;
	for (i = 0; i < ETH_ALEN; ++i) {
		end = woal_strsep(&begin, ':', '/');
		if (end)
			mac_addr[i] = woal_atox(end);
	}

	kfree(mac_buff);
	LEAVE();
}

#ifdef STA_SUPPORT
/**
 *  @brief This function sets multicast addresses to firmware
 *
 *  @param dev     A pointer to net_device structure
 *
 *  @return        N/A
 */
void woal_set_multicast_list(struct net_device *dev)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	ENTER();
	queue_work(priv->mclist_workqueue, &priv->mclist_work);
	LEAVE();
}
#endif

/**
 *  @brief This function initializes the private structure
 *          and set default value to the member of moal_private.
 *
 *  @param priv             A pointer to moal_private structure
 *  @param wait_option      Wait option
 *
 *  @return                 N/A
 */
void woal_init_priv(moal_private *priv, t_u8 wait_option)
{
	ENTER();
#ifdef STA_SUPPORT
	if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
		priv->current_key_index = 0;
		priv->rate_index = AUTO_RATE;
		priv->is_adhoc_link_sensed = MFALSE;
		priv->scan_type = MLAN_SCAN_TYPE_ACTIVE;
		priv->bg_scan_start = MFALSE;
		priv->bg_scan_reported = MFALSE;
		priv->sched_scanning = MFALSE;
#ifdef STA_CFG80211
		priv->roaming_enabled = MFALSE;
		priv->roaming_required = MFALSE;
#endif

		memset(&priv->nick_name, 0, sizeof(priv->nick_name));
		priv->num_tx_timeout = 0;
		priv->rx_filter = 0;

#ifdef REASSOCIATION
		priv->reassoc_on = MFALSE;
		priv->set_asynced_essid_flag = MFALSE;
#endif
		priv->auto_assoc_priv.auto_assoc_type_on = 2;
		priv->auto_assoc_priv.auto_assoc_trigger_flag =
			AUTO_ASSOC_TYPE_DRV_RECONN;
		memset(&priv->auto_assoc_priv.drv_assoc, 0,
		       sizeof(drv_auto_assoc));
		memset(&priv->auto_assoc_priv.drv_reconnect, 0,
		       sizeof(drv_auto_assoc));
		priv->auto_assoc_priv.drv_reconnect.retry_count = 0xff;
#ifdef STA_CFG80211
		memset(&priv->sme_current, 0,
		       sizeof(struct cfg80211_connect_params));
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		woal_init_wifi_hal(priv);
#endif
#endif
	}
#endif /* STA_SUPPORT */
#ifdef UAP_SUPPORT
	if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
		priv->bss_started = MFALSE;
		priv->uap_host_based = MFALSE;
		priv->skip_cac = MFALSE;

#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		memset(&priv->chan, 0, sizeof(struct cfg80211_chan_def));
		memset(&priv->csa_chan, 0, sizeof(struct cfg80211_chan_def));
		priv->uap_tx_blocked = MFALSE;
		memset(&priv->beacon_after, 0,
		       sizeof(struct cfg80211_beacon_data));
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		woal_init_wifi_hal(priv);
#endif
#endif
#endif
	}
#endif

	skb_queue_head_init(&priv->tx_q);
	memset(&priv->tx_protocols, 0, sizeof(dot11_protocol));
	memset(&priv->rx_protocols, 0, sizeof(dot11_protocol));
	priv->media_connected = MFALSE;

	memset(priv->dscp_map, 0xFF, sizeof(priv->dscp_map));

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	priv->probereq_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->beacon_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->proberesp_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->assocresp_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->beacon_wps_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->proberesp_p2p_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->assocresp_qos_map_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->beacon_vendor_index = MLAN_CUSTOM_IE_AUTO_IDX_MASK;
	priv->mgmt_subtype_mask = 0;
	priv->cfg_disconnect = MFALSE;
	priv->cfg_connect = MFALSE;
#endif
#ifdef STA_SUPPORT
#ifdef STA_CFG80211
	priv->pmk_saved = MFALSE;
	memset(&priv->pmk, 0, sizeof(mlan_pmk_t));
#endif
#endif

	priv->enable_tcp_ack_enh = MTRUE;
	priv->tcp_ack_drop_cnt = 0;
	priv->tcp_ack_cnt = 0;
	priv->tcp_ack_payload = 0;
	priv->tcp_ack_max_hold = TCP_ACK_MAX_HOLD;

	priv->enable_auto_tdls = MFALSE;
	priv->tdls_check_tx = MFALSE;

	priv->gtk_data_ready = MFALSE;
	memset(&priv->gtk_rekey_data, 0, sizeof(mlan_ds_misc_gtk_rekey_data));

	if (MLAN_STATUS_SUCCESS !=
	    woal_request_get_fw_info(priv, wait_option, NULL)) {
		PRINTM(MERROR, "%s: get_fw_info failed \n", __func__);
		return;
	}
	/* Set MAC address from the insmod command line */
	if (priv->phandle->set_mac_addr &&
	    priv->bss_type != MLAN_BSS_TYPE_DFS) {
		memset(priv->current_addr, 0, ETH_ALEN);
		moal_memcpy_ext(priv->phandle, priv->current_addr,
				priv->phandle->mac_addr, ETH_ALEN, ETH_ALEN);
	}

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
#ifdef MFG_CMD_SUPPORT
	if (priv->phandle->params.mfg_mode != MLAN_INIT_PARA_ENABLED)
#endif
		if (priv->bss_type == MLAN_BSS_TYPE_WIFIDIRECT) {
			if (priv->bss_virtual) {
				if (priv->pa_netdev) {
					moal_memcpy_ext(
						priv->phandle,
						priv->current_addr,
						priv->pa_netdev->dev_addr,
						ETH_ALEN, ETH_ALEN);
					priv->current_addr[4] ^= 0x80;
					if (priv->phandle->second_mac)
						priv->current_addr[5] ^= 0x80;
					PRINTM(MCMND,
					       "Set WFD interface addr: " MACSTR
					       "\n",
					       MAC2STR(priv->current_addr));
				}
			} else {
				priv->current_addr[0] |= 0x02;
				PRINTM(MCMND,
				       "Set WFD device addr: " MACSTR "\n",
				       MAC2STR(priv->current_addr));
			}
		}
#endif
#endif
#endif

		/* Set MAC address for UAPx/MLANx/WFDx/OCBx and let them
		 * different with each other */
#ifdef WIFI_DIRECT_SUPPORT
	if (priv->bss_type != MLAN_BSS_TYPE_WIFIDIRECT)
#endif
	{
		if (priv->bss_index) {
			priv->current_addr[0] |= 0x02;
			priv->current_addr[4] += priv->bss_index;
		}
		PRINTM(MCMND, "Set %s interface addr: " MACSTR "\n",
		       priv->netdev->name, MAC2STR(priv->current_addr));
	}

	/* ZeroDFS interface doesn't need to set mac address to fw */
	if (priv->bss_type != MLAN_BSS_TYPE_DFS) {
		if (MLAN_STATUS_SUCCESS !=
		    woal_request_set_mac_address(priv, MOAL_IOCTL_WAIT))
			PRINTM(MERROR, "%s: set mac address failed \n",
			       __func__);
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	eth_hw_addr_set(priv->netdev, priv->current_addr);
#else
	moal_memcpy_ext(priv->phandle, priv->netdev->dev_addr,
			priv->current_addr, ETH_ALEN, ETH_ALEN);
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
	priv->host_mlme = 0;
	priv->auth_flag = 0;
	priv->auth_alg = 0xFFFF;
#endif

#ifdef UAP_SUPPORT
	priv->target_chan = 0;
	priv->backup_chan = 0;
	priv->user_cac_period_msec = 0;
	priv->chan_under_nop = MFALSE;
#endif
	LEAVE();
}

/**
 *  @brief Reset all interfaces if all_intf flag is TRUE,
 *          otherwise specified interface only
 *
 *  @param priv          A pointer to moal_private structure
 *  @param wait_option   Wait option
 *  @param all_intf      TRUE  : all interfaces
 *                       FALSE : current interface only
 *
 *  @return             MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status woal_reset_intf(moal_private *priv, t_u8 wait_option, int all_intf)
{
	int ret = MLAN_STATUS_SUCCESS;
	int intf_num;
	moal_handle *handle = NULL;
	mlan_bss_info bss_info;

	ENTER();

	if (!priv) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	handle = priv->phandle;

#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	/* Unregister and detach connected radiotap net device */
	if (handle->mon_if) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		if (MLAN_STATUS_SUCCESS !=
		    woal_set_net_monitor(handle->mon_if->priv, wait_option,
					 MFALSE, 0, NULL)) {
			PRINTM(MERROR, "%s: stop net monitor failed \n",
			       __func__);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
#endif
		netif_device_detach(handle->mon_if->mon_ndev);
		if (handle->mon_if->mon_ndev->reg_state == NETREG_REGISTERED)
			unregister_netdev(handle->mon_if->mon_ndev);
		handle->mon_if = NULL;
	}
#endif
	if (handle->rf_test_mode)
		woal_process_rf_test_mode(handle, MFG_CMD_UNSET_TEST_MODE);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
#endif
#ifdef STA_SUPPORT
	if (MLAN_STATUS_SUCCESS != woal_cancel_scan(priv, wait_option)) {
		PRINTM(MERROR, "%s: cancel scan failed \n", __func__);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
#endif

	/* Stop queue and detach device */
	if (!all_intf) {
		woal_stop_queue(priv->netdev);
		netif_device_detach(priv->netdev);
	} else {
		for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
			woal_stop_queue(handle->priv[intf_num]->netdev);
			netif_device_detach(handle->priv[intf_num]->netdev);
		}
	}

	/* Get BSS info */
	memset(&bss_info, 0, sizeof(bss_info));
	if (MLAN_STATUS_SUCCESS !=
	    woal_get_bss_info(priv, wait_option, &bss_info)) {
		PRINTM(MERROR, "%s: get bss info failed \n", __func__);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Cancel host sleep */
	if (bss_info.is_hs_configured) {
		if (MLAN_STATUS_SUCCESS != woal_cancel_hs(priv, wait_option)) {
			ret = -EFAULT;
			goto done;
		}
	}

	/* Disconnect from network */
	if (!all_intf) {
		/* Disconnect specified interface only */
		if ((priv->media_connected == MTRUE)
#ifdef UAP_SUPPORT
		    || (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP)
#endif
		) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_disconnect(priv, wait_option, NULL,
					    DEF_DEAUTH_REASON_CODE)) {
				PRINTM(MERROR, "%s: woal_disconnect failed \n",
				       __func__);
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			priv->media_connected = MFALSE;
		}
	} else {
		/* Disconnect all interfaces */
		for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
			if (handle->priv[intf_num]->media_connected == MTRUE
#ifdef UAP_SUPPORT
			    || (GET_BSS_ROLE(handle->priv[intf_num]) ==
				MLAN_BSS_ROLE_UAP)
#endif
			) {
				if (MLAN_STATUS_SUCCESS !=
				    woal_disconnect(handle->priv[intf_num],
						    wait_option, NULL,
						    DEF_DEAUTH_REASON_CODE)) {
					PRINTM(MERROR,
					       "%s: woal_disconnect failed \n",
					       __func__);
					ret = MLAN_STATUS_FAILURE;
					goto done;
				}
				handle->priv[intf_num]->media_connected =
					MFALSE;
			}
		}
	}

#ifdef REASSOCIATION
	/* Reset the reassoc timer and status */
	if (!all_intf) {
		handle->reassoc_on &= ~MBIT(priv->bss_index);
		priv->reassoc_on = MFALSE;
		priv->auto_assoc_priv.drv_assoc.status = MFALSE;
		priv->auto_assoc_priv.drv_reconnect.status = MFALSE;
		priv->set_asynced_essid_flag = MFALSE;
	} else {
		handle->reassoc_on = 0;
		for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
			handle->priv[intf_num]->reassoc_on = MFALSE;
			handle->priv[intf_num]
				->auto_assoc_priv.drv_assoc.status = MFALSE;
			handle->priv[intf_num]
				->auto_assoc_priv.drv_reconnect.status = MFALSE;
			handle->priv[intf_num]->set_asynced_essid_flag = MFALSE;
		}
	}
	if (!handle->reassoc_on && handle->is_reassoc_timer_set) {
		woal_cancel_timer(&handle->reassoc_timer);
		handle->is_reassoc_timer_set = MFALSE;
	}
#endif /* REASSOCIATION */

	if (handle->is_fw_dump_timer_set) {
		woal_cancel_timer(&handle->fw_dump_timer);
		handle->is_fw_dump_timer_set = MFALSE;
	}

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	if (handle->is_go_timer_set) {
		woal_cancel_timer(&handle->go_timer);
		handle->is_go_timer_set = MFALSE;
	}
#endif
#endif

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
	if (handle->is_remain_timer_set) {
		woal_cancel_timer(&handle->remain_timer);
		woal_remain_timer_func(handle);
	}
#endif
#endif

done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function return the point to structure moal_private
 *
 *  @param handle       Pointer to structure moal_handle
 *  @param bss_index    BSS index number
 *
 *  @return             moal_private pointer or NULL
 */
moal_private *woal_bss_index_to_priv(moal_handle *handle, t_u8 bss_index)
{
	int i;

	ENTER();
	if (!handle || !handle->priv_num) {
		LEAVE();
		return NULL;
	}
	for (i = 0; i < MLAN_MAX_BSS_NUM; i++) {
		if (handle->priv[i] &&
		    (handle->priv[i]->bss_index == bss_index)) {
			LEAVE();
			return handle->priv[i];
		}
	}

	LEAVE();
	return NULL;
}

/**
 *  @brief This function alloc mlan_buffer.
 *  @param handle  A pointer to moal_handle structure
 *  @param size	   buffer size to allocate
 *
 *  @return        mlan_buffer pointer or NULL
 */
pmlan_buffer woal_alloc_mlan_buffer(moal_handle *handle, int size)
{
	mlan_buffer *pmbuf = NULL;
	struct sk_buff *skb;
	gfp_t flag;

	ENTER();

	flag = (in_atomic() || irqs_disabled()) ? GFP_ATOMIC : GFP_KERNEL;
	if (size <= 0) {
		PRINTM(MERROR, "Buffer size must be positive\n");
		LEAVE();
		return NULL;
	}

	skb = __dev_alloc_skb(size + sizeof(mlan_buffer), flag);
	if (!skb) {
		PRINTM(MERROR, "%s: No free skb\n", __func__);
		LEAVE();
		return NULL;
	}
	skb_reserve(skb, sizeof(mlan_buffer));
	pmbuf = (mlan_buffer *)skb->head;
	memset((u8 *)pmbuf, 0, sizeof(mlan_buffer));
	pmbuf->pdesc = (t_void *)skb;
	pmbuf->pbuf = (t_u8 *)skb->data;
	atomic_inc(&handle->mbufalloc_count);
	LEAVE();
	return pmbuf;
}

/**
 *  @brief This function alloc mlan_ioctl_req.
 *
 *  @param size	   buffer size to allocate
 *
 *  @return        mlan_ioctl_req pointer or NULL
 */
pmlan_ioctl_req woal_alloc_mlan_ioctl_req(int size)
{
	mlan_ioctl_req *req = NULL;
	gfp_t flag;

	ENTER();

	flag = (in_atomic() || irqs_disabled()) ? GFP_ATOMIC : GFP_KERNEL;
	req = kzalloc((sizeof(mlan_ioctl_req) + size + sizeof(int) +
		       sizeof(wait_queue)),
		      flag);
	if (!req) {
		PRINTM(MERROR, "%s: Fail to alloc ioctl buffer\n", __func__);
		LEAVE();
		return NULL;
	}
	req->pbuf = (t_u8 *)req + sizeof(mlan_ioctl_req) + sizeof(wait_queue);
	req->buf_len = (t_u32)size;
	req->reserved_1 = (t_ptr)((t_u8 *)req + sizeof(mlan_ioctl_req));

	LEAVE();
	return req;
}

/**
 *  @brief This function frees mlan_buffer.
 *  @param handle  A pointer to moal_handle structure
 *  @param pmbuf   Pointer to mlan_buffer
 *
 *  @return        N/A
 */
void woal_free_mlan_buffer(moal_handle *handle, pmlan_buffer pmbuf)
{
	ENTER();
	if (!pmbuf) {
		LEAVE();
		return;
	}
	if (pmbuf->pdesc)
		dev_kfree_skb_any((struct sk_buff *)pmbuf->pdesc);
	else
		PRINTM(MERROR, "free mlan buffer without pdesc\n");
	atomic_dec(&handle->mbufalloc_count);
	LEAVE();
	return;
}

/**
 *  @brief This function get card info from card type
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         0-success , otherwise failure.
 */
static int woal_get_card_info(moal_handle *phandle)
{
	int ret = 0;

	ENTER();

	switch (phandle->card_type) {
#ifdef SD8801
	case CARD_TYPE_SD8801:
		phandle->card_info = &card_info_SD8801;
		break;
#endif
#ifdef SD8887
	case CARD_TYPE_SD8887:
		phandle->card_info = &card_info_SD8887;
		break;
#endif
#if defined(SD8897)
	case CARD_TYPE_SD8897:
		phandle->card_info = &card_info_SD8897;
		break;
#endif
#if defined(PCIE8897)
	case CARD_TYPE_PCIE8897:
		phandle->card_info = &card_info_PCIE8897;
		break;
#endif
#if defined(USB8897)
	case CARD_TYPE_USB8897:
		phandle->card_info = &card_info_USB8897;
		break;
#endif
#ifdef SD8977
	case CARD_TYPE_SD8977:
		phandle->card_info = &card_info_SD8977;
		break;
#endif
#ifdef SD8978
	case CARD_TYPE_SD8978:
		phandle->card_info = &card_info_SD8978;
		break;
#endif
#ifdef SD8997
	case CARD_TYPE_SD8997:
		phandle->card_info = &card_info_SD8997;
		break;
#endif
#ifdef SD9098
	case CARD_TYPE_SD9098:
		phandle->card_info = &card_info_SD9098;
		break;
#endif
#ifdef SD9097
	case CARD_TYPE_SD9097:
		phandle->card_info = &card_info_SD9097;
		break;
#endif
#ifdef SDNW62X
	case CARD_TYPE_SDNW62X:
		phandle->card_info = &card_info_SDNW62X;
		break;
#endif
#ifdef SD9177
	case CARD_TYPE_SD9177:
		phandle->card_info = &card_info_SD9177;
		phandle->event_fw_dump = MFALSE;
		break;
#endif
#ifdef PCIE8997
	case CARD_TYPE_PCIE8997:
		phandle->card_info = &card_info_PCIE8997;
		break;
#endif
#ifdef PCIE9097
	case CARD_TYPE_PCIE9097:
		phandle->card_info = &card_info_PCIE9097;
		break;
#endif
#ifdef PCIENW62X
	case CARD_TYPE_PCIENW62X:
		phandle->card_info = &card_info_PCIENW62X;
		break;
#endif
#ifdef PCIE9098
	case CARD_TYPE_PCIE9098:
		phandle->card_info = &card_info_PCIE9098;
		phandle->event_fw_dump = MTRUE;
		break;
#endif
#ifdef USB8801
	case CARD_TYPE_USB8801:
		phandle->card_info = &card_info_USB8801;
		break;
#endif
#ifdef USB8997
	case CARD_TYPE_USB8997:
		phandle->card_info = &card_info_USB8997;
		break;
#endif
#ifdef USB8978
	case CARD_TYPE_USB8978:
		phandle->card_info = &card_info_USB8978;
		break;
#endif
#ifdef USB9098
	case CARD_TYPE_USB9098:
		phandle->card_info = &card_info_USB9098;
		break;
#endif
#ifdef USB9097
	case CARD_TYPE_USB9097:
		phandle->card_info = &card_info_USB9097;
		break;
#endif
#ifdef USBNW62X
	case CARD_TYPE_USBNW62X:
		phandle->card_info = &card_info_USBNW62X;
		break;
#endif
#ifdef SD8987
	case CARD_TYPE_SD8987:
		phandle->card_info = &card_info_SD8987;
		break;
#endif
	default:
		PRINTM(MERROR,
		       "woal_get_card_info can't get right card type \n");
		ret = -1;
		break;
	}

	LEAVE();
	return ret;
}

#ifdef STA_SUPPORT
#endif /* STA_SUPPORT */

/**
 *  @brief This function handles events generated by firmware
 *
 *  @param priv     A pointer to moal_private structure
 *  @param payload  A pointer to payload buffer
 *  @param len      Length of the payload
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_broadcast_event(moal_private *priv, t_u8 *payload, t_u32 len)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;
	moal_handle *handle = priv->phandle;
	struct net_device *netdev = priv->netdev;
	struct sock *sk = handle->nl_sk;

	ENTER();

	/* interface name to be prepended to event */
	/* NL_MAX_PAYLOAD = 3 * 1024 */
	if ((len + IFNAMSIZ) > NL_MAX_PAYLOAD) {
		PRINTM(MERROR, "event size is too big, len=%d\n", (int)len);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (sk) {
		/* Allocate skb */
		skb = alloc_skb(NLMSG_SPACE(NL_MAX_PAYLOAD), GFP_ATOMIC);
		if (!skb) {
			PRINTM(MERROR, "Could not allocate skb for netlink\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		memset(skb->data, 0, NLMSG_SPACE(NL_MAX_PAYLOAD));

		nlh = (struct nlmsghdr *)skb->data;
		nlh->nlmsg_len = NLMSG_SPACE(len + IFNAMSIZ);

		/* From kernel */
		nlh->nlmsg_pid = 0;
		nlh->nlmsg_flags = 0;

		/* Data */
		skb_put(skb, nlh->nlmsg_len);
		moal_memcpy_ext(handle, NLMSG_DATA(nlh), netdev->name, IFNAMSIZ,
				nlh->nlmsg_len - NLMSG_LENGTH(0));
		moal_memcpy_ext(handle, ((t_u8 *)(NLMSG_DATA(nlh))) + IFNAMSIZ,
				payload, len,
				nlh->nlmsg_len - NLMSG_LENGTH(IFNAMSIZ));

		/* From Kernel */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
		NETLINK_CB(skb).pid = 0;
#else
		NETLINK_CB(skb).portid = 0;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
		/* Multicast message */
		NETLINK_CB(skb).dst_pid = 0;
#endif

		/* Multicast group number */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
		NETLINK_CB(skb).dst_groups = NL_MULTICAST_GROUP;
#else
		NETLINK_CB(skb).dst_group = NL_MULTICAST_GROUP;
#endif

		/* Send message */
		ret = netlink_broadcast(sk, skb, 0, NL_MULTICAST_GROUP,
					GFP_ATOMIC);
		if (ret) {
			PRINTM(MWARN, "netlink_broadcast failed: ret=%d\n",
			       ret);
			goto done;
		}

		ret = MLAN_STATUS_SUCCESS;
	} else {
		PRINTM(MERROR,
		       "Could not send event through NETLINK. Link down.\n");
		ret = MLAN_STATUS_FAILURE;
	}
done:
	LEAVE();
	return ret;
}

#ifdef REASSOCIATION
/**
 *  @brief This function handles re-association. it is triggered
 *  by re-assoc timer.
 *
 *  @param data    A pointer to wlan_thread structure
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
int woal_reassociation_thread(void *data)
{
	moal_thread *pmoal_thread = data;
	moal_private *priv = NULL;
	moal_handle *handle = (moal_handle *)pmoal_thread->handle;
#if CFG80211_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	wait_queue_t wait;
#else
	wait_queue_entry_t wait;
#endif
	int i;
	BOOLEAN reassoc_timer_req;
	mlan_802_11_ssid req_ssid;
	mlan_ssid_bssid *ssid_bssid = NULL;
	mlan_status status;
	mlan_bss_info bss_info;
	t_u32 timer_val = MOAL_TIMER_10S;
	t_u32 retry_count = 0;
	t_u32 retry_interval = 0;
	t_u8 zero_mac[] = {0, 0, 0, 0, 0, 0};
	ENTER();
	ssid_bssid = kmalloc(sizeof(mlan_ssid_bssid), GFP_KERNEL);
	if (!ssid_bssid) {
		LEAVE();
		return 0;
	}

	woal_activate_thread(pmoal_thread);
	init_waitqueue_entry(&wait, current);

	current->flags |= PF_NOFREEZE;

	for (;;) {
		add_wait_queue(&pmoal_thread->wait_q, &wait);
		set_current_state(TASK_INTERRUPTIBLE);

		schedule();

		set_current_state(TASK_RUNNING);
		remove_wait_queue(&pmoal_thread->wait_q, &wait);
#if defined(USB)
		if (IS_USB(handle->card_type)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 13)
			try_to_freeze(0); /* Argument is not used by the kernel
					   */
#else
			try_to_freeze();
#endif
		}
#endif

		/* Cancel re-association timer */
		if (handle->is_reassoc_timer_set == MTRUE) {
			woal_cancel_timer(&handle->reassoc_timer);
			handle->is_reassoc_timer_set = MFALSE;
		}

		if (handle->surprise_removed)
			break;
		if (kthread_should_stop())
			break;

		if (handle->hardware_status != HardwareStatusReady) {
			PRINTM(MINFO,
			       "Reassoc: Hardware status is not correct\n");
			continue;
		}

		PRINTM(MEVENT, "Reassoc: Thread waking up...\n");
		reassoc_timer_req = MFALSE;
#ifdef STA_CFG80211
		for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM) &&
			    (priv = handle->priv[i]);
		     i++) {
			if (priv->roaming_required) {
				priv->roaming_required = MFALSE;
				PRINTM(MEVENT, "Try to roaming......\n");
				woal_start_roaming(priv);
				break;
			}
		}
#endif

		for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM) &&
			    (priv = handle->priv[i]);
		     i++) {
			if (priv->reassoc_required == MFALSE) {
				priv->auto_assoc_priv.drv_assoc.status = MFALSE;
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				continue;
			}

			if (priv->auto_assoc_priv.auto_assoc_trigger_flag ==
				    AUTO_ASSOC_TYPE_DRV_RECONN ||
			    priv->auto_assoc_priv.auto_assoc_trigger_flag ==
				    AUTO_ASSOC_TYPE_DRV_ASSOC) {
				if (priv->auto_assoc_priv
					    .auto_assoc_trigger_flag ==
				    AUTO_ASSOC_TYPE_DRV_RECONN) {
					retry_count = priv->auto_assoc_priv
							      .drv_reconnect
							      .retry_count;
					retry_interval =
						priv->auto_assoc_priv
							.drv_reconnect
							.retry_interval *
						1000;
					if (retry_count == 0xff)
						retry_count =
							AUTO_ASSOC_RETRY_FOREVER;
					priv->auto_assoc_priv.drv_reconnect
						.status = MTRUE;
					PRINTM(MINFO,
					       "Auto assoc: driver auto re-connect triggered\n");
				}
				if (priv->auto_assoc_priv
					    .auto_assoc_trigger_flag ==
				    AUTO_ASSOC_TYPE_DRV_ASSOC) {
					/* disconnect before driver
					 * assoc */
					woal_disconnect(priv, MOAL_IOCTL_WAIT,
							NULL,
							DEF_DEAUTH_REASON_CODE);
					if (priv->auto_assoc_priv
						    .auto_assoc_type_on &
					    (0x1 << (AUTO_ASSOC_TYPE_DRV_ASSOC -
						     1))) {
						retry_count =
							priv->auto_assoc_priv
								.drv_assoc
								.retry_count;
						retry_interval =
							priv->auto_assoc_priv
								.drv_assoc
								.retry_interval *
							1000;
						if (retry_count == 0xff)
							retry_count =
								AUTO_ASSOC_RETRY_FOREVER;
						else
							retry_count =
								retry_count + 1;
						PRINTM(MINFO,
						       "Auto assoc: driver auto assoc triggered\n");
					} else {
						retry_count = 1;
						retry_interval = 0;
						PRINTM(MINFO,
						       "Auto assoc: set asynced essid with drv auto assoc disable\n");
					}
					priv->reassoc_required = MTRUE;
					priv->auto_assoc_priv.drv_assoc.status =
						MTRUE;
				}
				priv->auto_assoc_priv.auto_assoc_trigger_flag =
					AUTO_ASSOC_TYPE_NONE;
			}

			if (retry_count == 0 &&
			    (priv->auto_assoc_priv.drv_assoc.status == MTRUE ||
			     priv->auto_assoc_priv.drv_reconnect.status ==
				     MTRUE)) {
				PRINTM(MINFO,
				       "Auto assoc: stop driver auto assoc: the retry count is 0\n");
				priv->auto_assoc_priv.drv_assoc.status = MFALSE;
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				continue;
			}

			memset(&bss_info, 0x00, sizeof(bss_info));

			if (MLAN_STATUS_SUCCESS !=
			    woal_get_bss_info(priv, MOAL_IOCTL_WAIT,
					      &bss_info)) {
				PRINTM(MINFO, "Ressoc: Fail to get bss info\n");
				priv->reassoc_required = MFALSE;
				priv->auto_assoc_priv.drv_assoc.status = MFALSE;
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				continue;
			}

			if (bss_info.bss_mode != MLAN_BSS_MODE_INFRA ||
			    priv->media_connected != MFALSE) {
				PRINTM(MINFO,
				       "Reassoc: ad-hoc mode or media connected\n");
				priv->reassoc_required = MFALSE;
				priv->auto_assoc_priv.drv_assoc.status = MFALSE;
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				continue;
			}
			/** avoid on going scan from other thread */
			if (handle->scan_pending_on_block) {
				reassoc_timer_req = MTRUE;
				break;
			}

			/* The semaphore is used to avoid reassociation
			   thread and wlan_set_scan/wlan_set_essid
			   interrupting each other. Reassociation should
			   be disabled completely by application if
			   wlan_set_user_scan_ioctl/wlan_set_wap is
			   used.
			*/
			if (MOAL_ACQ_SEMAPHORE_BLOCK(&handle->reassoc_sem)) {
				PRINTM(MERROR,
				       "Acquire semaphore error, reassociation thread\n");
				reassoc_timer_req = MTRUE;
				break;
			}

			PRINTM(MINFO, "Reassoc: Required ESSID: %s\n",
			       priv->prev_ssid_bssid.ssid.ssid);
			PRINTM(MINFO, "Reassoc: Performing Active Scan\n");

			memset(&req_ssid, 0x00, sizeof(mlan_802_11_ssid));
			moal_memcpy_ext(priv->phandle, &req_ssid,
					&priv->prev_ssid_bssid.ssid,
					sizeof(mlan_802_11_ssid),
					sizeof(mlan_802_11_ssid));

			/* Do specific SSID scanning */
			if (MLAN_STATUS_SUCCESS !=
			    woal_request_scan(priv, MOAL_IOCTL_WAIT,
					      &req_ssid)) {
				PRINTM(MERROR,
				       "Reassoc: Fail to do specific scan\n");
				reassoc_timer_req = MTRUE;
				MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
				break;
			}

			if (handle->surprise_removed) {
				MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
				break;
			}

			memset(ssid_bssid, 0, sizeof(mlan_ssid_bssid));

			if (priv->auto_assoc_priv.drv_assoc.status == MTRUE) {
				if (priv->assoc_with_mac &&
				    memcmp(priv->prev_ssid_bssid.bssid,
					   zero_mac, MLAN_MAC_ADDR_LENGTH)) {
					/* Search AP by BSSID & SSID */
					PRINTM(MINFO,
					       "Reassoc: Search AP by BSSID & SSID\n");
					moal_memcpy_ext(
						priv->phandle,
						&ssid_bssid->bssid,
						&priv->prev_ssid_bssid.bssid,
						MLAN_MAC_ADDR_LENGTH,
						sizeof(mlan_802_11_mac_addr));
				} else {
					/* Search AP by ESSID for driver
					 * auto reassoc */
					PRINTM(MINFO,
					       "Reassoc: Search AP by ESSID\n");
				}

				moal_memcpy_ext(priv->phandle,
						&ssid_bssid->ssid,
						&priv->prev_ssid_bssid.ssid,
						sizeof(mlan_802_11_ssid),
						sizeof(mlan_802_11_ssid));

			} else {
				/* Search AP by BSSID first */
				PRINTM(MINFO,
				       "Reassoc: Search AP by BSSID first\n");
				moal_memcpy_ext(priv->phandle,
						&ssid_bssid->bssid,
						&priv->prev_ssid_bssid.bssid,
						MLAN_MAC_ADDR_LENGTH,
						sizeof(mlan_802_11_mac_addr));
			}

			status = woal_find_best_network(priv, MOAL_IOCTL_WAIT,
							ssid_bssid);
#ifdef STA_WEXT
			if (status == MLAN_STATUS_SUCCESS) {
				if (MLAN_STATUS_SUCCESS !=
				    woal_11d_check_ap_channel(priv,
							      MOAL_IOCTL_WAIT,
							      ssid_bssid)) {
					PRINTM(MERROR,
					       "Reassoc: The AP's channel is invalid for current region\n");
					status = MLAN_STATUS_FAILURE;
				}
			}
#endif
			/** The find AP without ssid, we need re-search
			 */
			if (status == MLAN_STATUS_SUCCESS &&
			    !ssid_bssid->ssid.ssid_len) {
				PRINTM(MINFO,
				       "Reassoc: Skip AP without ssid\n");
				status = MLAN_STATUS_FAILURE;
			}

			if (priv->auto_assoc_priv.drv_assoc.status != MTRUE &&
			    MLAN_STATUS_SUCCESS != status) {
				PRINTM(MINFO,
				       "Reassoc: AP not found in scan list\n");
				PRINTM(MINFO, "Reassoc: Search AP by SSID\n");
				/* Search AP by SSID */
				memset(ssid_bssid, 0, sizeof(mlan_ssid_bssid));
				moal_memcpy_ext(priv->phandle,
						&ssid_bssid->ssid,
						&priv->prev_ssid_bssid.ssid,
						sizeof(mlan_802_11_ssid),
						sizeof(mlan_802_11_ssid));
				status = woal_find_best_network(
					priv, MOAL_IOCTL_WAIT, ssid_bssid);
#ifdef STA_WEXT
				if (status == MLAN_STATUS_SUCCESS) {
					if (MLAN_STATUS_SUCCESS !=
					    woal_11d_check_ap_channel(
						    priv, MOAL_IOCTL_WAIT,
						    ssid_bssid)) {
						PRINTM(MERROR,
						       "Reassoc: The AP's channel is invalid for current region\n");
						status = MLAN_STATUS_FAILURE;
					}
				}
#endif
			}

			if (status == MLAN_STATUS_SUCCESS) {
				/* set the wep key */
				if (bss_info.wep_status) {
					if (MLAN_STATUS_SUCCESS !=
					    woal_enable_wep_key(
						    priv, MOAL_IOCTL_WAIT)) {
						PRINTM(MERROR,
						       "Reassoc: woal_enable_wep_key failed\n");
						status = MLAN_STATUS_FAILURE;
					}
				}
				/* Zero SSID implies use BSSID to
				 * connect */
				memset(&ssid_bssid->ssid, 0,
				       sizeof(mlan_802_11_ssid));
				status = woal_bss_start(priv, MOAL_IOCTL_WAIT,
							ssid_bssid);
				if (status != MLAN_STATUS_SUCCESS)
					PRINTM(MERROR,
					       "Reassoc: woal_bss_start failed\n");
			}

			if (priv->media_connected == MFALSE)
				reassoc_timer_req = MTRUE;
			else {
				mlan_ds_rate *rate = NULL;
				mlan_ioctl_req *req = NULL;

				reassoc_timer_req = MFALSE;
				if (priv->auto_assoc_priv.drv_assoc.status ==
				    MTRUE) {
					memset(&bss_info, 0, sizeof(bss_info));
					if (MLAN_STATUS_SUCCESS !=
					    woal_get_bss_info(priv,
							      MOAL_IOCTL_WAIT,
							      &bss_info)) {
						PRINTM(MINFO,
						       "Ressoc: Fail to get bss info after driver auto reassoc\n");
					} else {
						moal_memcpy_ext(
							priv->phandle,
							&priv->prev_ssid_bssid
								 .ssid,
							&bss_info.ssid,
							sizeof(mlan_802_11_ssid),
							sizeof(mlan_802_11_ssid));
						moal_memcpy_ext(
							priv->phandle,
							&priv->prev_ssid_bssid
								 .bssid,
							&bss_info.bssid,
							MLAN_MAC_ADDR_LENGTH,
							sizeof(priv->prev_ssid_bssid
								       .bssid));
					}
					priv->auto_assoc_priv.drv_assoc.status =
						MFALSE;
				}
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				if (priv->rate_index != AUTO_RATE) {
					req = woal_alloc_mlan_ioctl_req(
						sizeof(mlan_ds_rate));

					if (req == NULL) {
						LEAVE();
						return MLAN_STATUS_FAILURE;
					}

					rate = (mlan_ds_rate *)req->pbuf;
					rate->param.rate_cfg.rate_type =
						MLAN_RATE_INDEX;
					rate->sub_command = MLAN_OID_RATE_CFG;
					req->req_id = MLAN_IOCTL_RATE;

					req->action = MLAN_ACT_SET;

					rate->param.rate_cfg.rate =
						priv->rate_index;

					status = woal_request_ioctl(
						priv, req, MOAL_IOCTL_WAIT);
					if (status != MLAN_STATUS_SUCCESS) {
						if (status !=
						    MLAN_STATUS_PENDING)
							kfree(req);
						LEAVE();
						return MLAN_STATUS_FAILURE;
					}
					kfree(req);
				}
			}

			MOAL_REL_SEMAPHORE(&handle->reassoc_sem);
		}

		if (handle->surprise_removed)
			break;

		if (reassoc_timer_req == MTRUE) {
			handle->is_reassoc_timer_set = MTRUE;
			if (priv &&
			    (priv->auto_assoc_priv.drv_assoc.status == MTRUE ||
			     priv->auto_assoc_priv.drv_reconnect.status ==
				     MTRUE)) {
				PRINTM(MEVENT,
				       "Auto assoc: No AP found or assoc failed. "
				       "Restarting re-assoc Timer: %d\n",
				       (int)retry_interval);
				if (retry_count != AUTO_ASSOC_RETRY_FOREVER)
					retry_count--;
				woal_mod_timer(&handle->reassoc_timer,
					       retry_interval);
			} else {
				PRINTM(MEVENT,
				       "Reassoc: No AP found or assoc failed. "
				       "Restarting re-assoc Timer: %d\n",
				       (int)timer_val);
				woal_mod_timer(&handle->reassoc_timer,
					       timer_val);
			}
		} else {
			if (priv) {
				priv->auto_assoc_priv.drv_assoc.status = MFALSE;
				priv->auto_assoc_priv.drv_reconnect.status =
					MFALSE;
				priv->set_asynced_essid_flag = MFALSE;
			}
		}
	}
	woal_deactivate_thread(pmoal_thread);
	kfree(ssid_bssid);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function triggers re-association by waking up
 *  re-assoc thread.
 *
 *  @param context  A pointer to context
 *  @return         N/A
 */
void woal_reassoc_timer_func(void *context)
{
	moal_handle *handle = (moal_handle *)context;

	ENTER();

	PRINTM(MINFO, "reassoc_timer fired.\n");
	handle->is_reassoc_timer_set = MFALSE;

	PRINTM(MINFO, "Waking Up the Reassoc Thread\n");
	wake_up_interruptible(&handle->reassoc_thread.wait_q);

	LEAVE();
	return;
}
#endif /* REASSOCIATION */

/**
 *  @brief This function triggers process hang
 *  re-assoc thread.
 *
 *  @param context  A pointer to context
 *  @return         N/A
 */
void woal_fw_dump_timer_func(void *context)
{
	moal_handle *handle = (moal_handle *)context;

	ENTER();

	PRINTM(MMSG, "fw_dump_timer fired.\n");
	handle->is_fw_dump_timer_set = MFALSE;
	if (handle->priv_num)
		woal_process_hang(handle);
	LEAVE();
	return;
}

#ifdef STA_SUPPORT
/**
 *  @brief update dscp mapping from assoc_resp/reassoc_resp
 *
 *  @param priv      Pointer to the moal_private driver data struct
 *
 *  @return          N/A
 */
void woal_update_dscp_mapping(moal_private *priv)
{
	mlan_ds_misc_assoc_rsp *assoc_rsp = NULL;
	IEEEtypes_AssocRsp_t *passoc_rsp = NULL;
	IEEEtypes_Header_t *qos_mapping_ie = NULL;
	DSCP_Range_t *pdscp_range = NULL;
	t_u8 dscp_except_num = 0;
	DSCP_Exception_t dscp_except[MAX_DSCP_EXCEPTION_NUM];
	int i, j;
	ENTER();
	assoc_rsp = kmalloc(sizeof(mlan_ds_misc_assoc_rsp), GFP_KERNEL);
	if (!assoc_rsp) {
		LEAVE();
		return;
	}
	memset(assoc_rsp, 0, sizeof(mlan_ds_misc_assoc_rsp));
	if (MLAN_STATUS_FAILURE ==
	    woal_get_assoc_rsp(priv, assoc_rsp, MOAL_NO_WAIT)) {
		PRINTM(MERROR, "woal_get_assoc_rsp failed\n");
		kfree(assoc_rsp);
		LEAVE();
		return;
	}

	passoc_rsp = (IEEEtypes_AssocRsp_t *)assoc_rsp->assoc_resp_buf;
	memset(priv->dscp_map, 0xFF, sizeof(priv->dscp_map));
	qos_mapping_ie = (IEEEtypes_Header_t *)woal_parse_ie_tlv(
		passoc_rsp->ie_buffer,
		assoc_rsp->assoc_resp_len - ASSOC_RESP_FIXED_SIZE, QOS_MAPPING);
	if (qos_mapping_ie &&
	    (qos_mapping_ie->len >= (sizeof(DSCP_Range_t) * MAX_NUM_TID))) {
		dscp_except_num = (qos_mapping_ie->len -
				   sizeof(DSCP_Range_t) * MAX_NUM_TID) /
				  sizeof(DSCP_Exception_t);
		if (dscp_except_num > MAX_DSCP_EXCEPTION_NUM) {
			PRINTM(MERROR, "dscp_except_num exceeds MAX limit\n");
			kfree(assoc_rsp);
			LEAVE();
			return;
		}
		moal_memcpy_ext(priv->phandle, dscp_except,
				(t_u8 *)qos_mapping_ie +
					sizeof(IEEEtypes_Header_t),
				dscp_except_num * sizeof(DSCP_Exception_t),
				sizeof(dscp_except));
		pdscp_range =
			(DSCP_Range_t *)((t_u8 *)qos_mapping_ie +
					 sizeof(IEEEtypes_Header_t) +
					 dscp_except_num *
						 sizeof(DSCP_Exception_t));
		for (i = 0; i < MAX_NUM_TID; i++) {
			PRINTM(MEVENT, "TID %d: dscp_low=%d, dscp_high=%d\n", i,
			       pdscp_range->dscp_low_value,
			       pdscp_range->dscp_high_value);
			if (pdscp_range->dscp_low_value != 0xff &&
			    pdscp_range->dscp_high_value != 0xff &&
			    pdscp_range->dscp_high_value <= 63) {
				for (j = pdscp_range->dscp_low_value;
				     j <= pdscp_range->dscp_high_value; j++)
					priv->dscp_map[j] = i;
			}
			pdscp_range++;
		}
		for (i = 0; i < dscp_except_num; i++) {
			if ((dscp_except[i].dscp_value <= 63) &&
			    (dscp_except[i].user_priority <= 7)) {
				PRINTM(MEVENT,
				       "dscp excpt: value=%d priority=%d\n",
				       dscp_except[i].dscp_value,
				       dscp_except[i].user_priority);
				priv->dscp_map[dscp_except[i].dscp_value] =
					dscp_except[i].user_priority;
			}
		}
	}
	kfree(assoc_rsp);
	LEAVE();
}

/**
 *  @brief Sends disconnect event
 *
 *  @param priv A pointer to moal_private struct
 *  @param disconnect_reason  disconnect reason code
 *  @return     N/A
 */
t_void woal_send_disconnect_to_system(moal_private *priv,
				      t_u16 disconnect_reason)
{
	int custom_len = 0;
	t_u8 event_buf[32];
#ifdef STA_WEXT
	union iwreq_data wrqu;
#endif
#ifdef STA_CFG80211
	unsigned long flags;
#endif
	int cfg80211_wext = priv->phandle->params.cfg80211_wext;
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
	mlan_ds_misc_gtk_rekey_data zero_gtk;
#endif
#endif
#ifdef STA_CFG80211
	t_u16 reason_code = 0;
#endif
	ENTER();
	priv->media_connected = MFALSE;
#ifdef STA_CFG80211
	if (!disconnect_reason)
		reason_code = MLAN_REASON_DEAUTH_LEAVING;
	else
		reason_code = disconnect_reason;
#endif
	woal_stop_queue(priv->netdev);
	if (netif_carrier_ok(priv->netdev))
		netif_carrier_off(priv->netdev);
	woal_flush_tcp_sess_queue(priv);

#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
	priv->gtk_data_ready = MFALSE;
	memset(&zero_gtk, 0x00, sizeof(zero_gtk));
	if (priv->phandle->params.gtk_rekey_offload ==
		    GTK_REKEY_OFFLOAD_ENABLE &&
	    memcmp(&priv->gtk_rekey_data, &zero_gtk,
		   sizeof(priv->gtk_rekey_data)) != 0) {
		PRINTM(MCMND, "clear GTK in woal_send_disconnect_to_system\n");
		if (MLAN_STATUS_FAILURE == woal_set_rekey_data(priv, NULL,
							       MLAN_ACT_CLEAR,
							       MOAL_NO_WAIT))
			PRINTM(MERROR, "%s: clear GTK failed!\n", __func__);
	}
	memset(&priv->gtk_rekey_data, 0, sizeof(mlan_ds_misc_gtk_rekey_data));
#endif
#endif

#ifdef STA_CFG80211
	if (priv->bss_type == MLAN_BSS_TYPE_STA)
		woal_flush_tdls_list(priv);
#endif
	woal_flush_mcast_list(priv);
#ifdef STA_CFG80211
	if (priv->bss_type == MLAN_BSS_TYPE_STA &&
	    IS_STA_CFG80211(cfg80211_wext)) {
		if (woal_flush_pmksa_list(priv))
			PRINTM(MERROR, "%s: woal_flush_pmksa_list failed!\n",
			       __func__);
		if (priv->okc_roaming_ie) {
			kfree(priv->okc_roaming_ie);
			priv->okc_roaming_ie = NULL;
			priv->okc_ie_len = 0;
		}
	}
#endif
	if (priv->bss_type == MLAN_BSS_TYPE_STA)
		woal_hist_data_reset(priv);

#ifdef STA_WEXT
	if (IS_STA_WEXT(cfg80211_wext)) {
		memset(wrqu.ap_addr.sa_data, 0x00, ETH_ALEN);
		wrqu.ap_addr.sa_family = ARPHRD_ETHER;
		wireless_send_event(priv->netdev, SIOCGIWAP, &wrqu, NULL);
	}
#endif
#ifdef STA_CFG80211
	if (IS_STA_CFG80211(cfg80211_wext)) {
		spin_lock_irqsave(&priv->connect_lock, flags);
		if (!priv->cfg_disconnect && !priv->cfg_connect && priv->wdev &&
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || IMX_ANDROID_13)
		    priv->wdev->connected) {
#else
		    priv->wdev->current_bss) {
#endif
			PRINTM(MMSG,
			       "wlan: Disconnected from " MACSTR
			       ": Reason code %d\n",
			       MAC2STR(priv->cfg_bssid), reason_code);
			spin_unlock_irqrestore(&priv->connect_lock, flags);
			priv->cfg_disconnect = MTRUE;
			/* This function must be called only when
			   disconnect issued by the FW, i.e.
			   disconnected by AP. For IBSS mode this call
			   is not valid */
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			if (priv->host_mlme)
				woal_host_mlme_disconnect(priv, reason_code,
							  NULL);
			else
#endif
				cfg80211_disconnected(priv->netdev, reason_code,
						      NULL, 0,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
						      false,
#endif
						      GFP_KERNEL);
		} else {
			spin_unlock_irqrestore(&priv->connect_lock, flags);
		}
		if (!woal_is_any_interface_active(priv->phandle)) {
			if (MLAN_STATUS_SUCCESS !=
			    woal_set_scan_time(priv, ACTIVE_SCAN_CHAN_TIME,
					       PASSIVE_SCAN_CHAN_TIME,
					       SPECIFIC_SCAN_CHAN_TIME))
				PRINTM(MERROR, "%s: set scan time failed \n",
				       __func__);
		}
		priv->ft_ie_len = 0;
		priv->ft_pre_connect = MFALSE;
		priv->ft_md = 0;
		priv->ft_cap = 0;
		memset(priv->dscp_map, 0xFF, sizeof(priv->dscp_map));
	}
#endif /* STA_CFG80211 */

	memset(event_buf, 0, sizeof(event_buf));
	custom_len = strlen(CUS_EVT_AP_CONNECTED);
	memcpy(event_buf, CUS_EVT_AP_CONNECTED,
	       MIN((int)(sizeof(event_buf) - 1), custom_len));
	if (MLAN_STATUS_SUCCESS !=
	    woal_broadcast_event(priv, event_buf, custom_len + ETH_ALEN))
		PRINTM(MINFO, "%s: woal_broadcast_event failed!\n", __func__);
	LEAVE();
}
#endif /* STA_SUPPORT */

#define OFFSET_SEQNUM 4
#define OFFSET_TYPE 8
#define DUMP_TYPE_ENDE 2
/**
 *  @brief  This function stores the FW dumps received from events
 *
 *  @param phandle     A pointer to moal_handle
 *  @param pmevent  A pointer to mlan_event structure
 *
 *  @return         N/A
 */
t_void woal_store_firmware_dump(moal_handle *phandle, mlan_event *pmevent)
{
	int ret = 0;
	t_u16 seqnum;
	t_u16 type = 0;
	t_u8 *pos;

	ENTER();

	if (!phandle || !pmevent) {
		PRINTM(MERROR, "Could not dump firmware info\n");
		LEAVE();
		return;
	}
	seqnum = woal_le16_to_cpu(
		*(t_u16 *)(pmevent->event_buf + OFFSET_SEQNUM));
	type = woal_le16_to_cpu(*(t_u16 *)(pmevent->event_buf + OFFSET_TYPE));

	if (seqnum == 1) {
#ifdef DEBUG_LEVEL1
		if (drvdbg & MFW_D)
			phandle->fw_dump_status = MTRUE;
#endif
		if (phandle->fw_dump == MFALSE) {
			PRINTM(MMSG, "=====FW trigger dump====\n");
			phandle->fw_dump = MTRUE;
			phandle->is_fw_dump_timer_set = MTRUE;
			woal_mod_timer(&phandle->fw_dump_timer, MOAL_TIMER_5S);
		}

		if (!phandle->fw_dump_buf) {
			ret = moal_vmalloc(phandle, FW_DUMP_INFO_LEN,
					   &phandle->fw_dump_buf);
			if (ret != MLAN_STATUS_SUCCESS ||
			    !phandle->fw_dump_buf) {
				PRINTM(MERROR,
				       "Failed to vmalloc fw dump buffer\n");
				LEAVE();
				return;
			}
		} else {
			memset(phandle->fw_dump_buf, 0x00, FW_DUMP_INFO_LEN);
		}
		phandle->fw_dump_len = 0;
		PRINTM(MMSG, "==== Start Receive FW dump event  ====\n");
	} else {
		if (!phandle->fw_dump_buf || !phandle->fw_dump_len) {
			PRINTM(MERROR,
			       "Error! Fw dump buffer is null or dump len is zero\n");
			LEAVE();
			return;
		}
	}
	pos = phandle->fw_dump_buf + phandle->fw_dump_len;
	moal_memcpy_ext(phandle, pos, pmevent->event_buf + OFFSET_SEQNUM,
			pmevent->event_len - OFFSET_SEQNUM,
			FW_DUMP_INFO_LEN - phandle->fw_dump_len);
	phandle->fw_dump_len += pmevent->event_len - OFFSET_SEQNUM;

	PRINTM(MINFO, "fw dump event: evt_len=%d toal_len=%ld\n",
	       pmevent->event_len, (long int)phandle->fw_dump_len);
	if (type == DUMP_TYPE_ENDE) {
		PRINTM(MMSG, "==== FW DUMP END: %ld bytes ====\n",
		       (long int)phandle->fw_dump_len);
		woal_append_end_block(phandle);
		phandle->fw_dump = MFALSE;
		if (phandle->is_fw_dump_timer_set) {
			woal_cancel_timer(&phandle->fw_dump_timer);
			phandle->is_fw_dump_timer_set = MFALSE;
		}
		if (phandle->priv_num) {
			woal_send_fw_dump_complete_event(
				woal_get_priv(phandle, MLAN_BSS_ROLE_ANY));
			mlan_pm_wakeup_card(phandle->pmlan_adapter, MFALSE);
			woal_process_hang(phandle);
		}
	}

	LEAVE();
	return;
}

#define DRV_INFO_SIZE 0x60000
#define DRV_INFO_PER_INTF 0x11000
#define ROW_SIZE_16 16
#define ROW_SIZE_32 32

/**
 *  @brief This function dump hex to file
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer to buffer to dump
 *  @param len       lengh of buf
 *  @param ascii     Whether add ascii at the end
 *  @param save_buf  Buffer which is saved to
 *
 *  @return          The length of this log
 */
static int woal_save_hex_dump(int rowsize, const void *buf, size_t len,
			      bool ascii, t_u8 *save_buf)
{
	const u8 *ptr = buf;
	int i, linelen, remaining = len;
	unsigned char linebuf[ROW_SIZE_32 * 3 + 2 + ROW_SIZE_32 + 1];
	char *pos = (char *)save_buf;

	if (rowsize != ROW_SIZE_16 && rowsize != ROW_SIZE_32)
		rowsize = ROW_SIZE_16;

	for (i = 0; i < (int)len; i += rowsize) {
		linelen = min(remaining, rowsize);
		remaining -= rowsize;

		hex_dump_to_buffer(ptr + i, linelen, rowsize, 1, linebuf,
				   sizeof(linebuf), false);

		pos += sprintf(pos, "%s\n", linebuf);
	}

	return pos - (char *)save_buf;
}

/**
 *  @brief This function save moal_priv's debug log
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer buffer saving log
 *
 *  @return          The length of this log
 */
static int woal_dump_priv_drv_info(moal_handle *handle, t_u8 *buf)
{
	char *ptr = (char *)buf;
	int index;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	int i = 0;
#endif
	moal_private *priv;

	ENTER();
	if (!handle || !buf) {
		PRINTM(MMSG, "%s: can't retreive info\n", __func__);
		LEAVE();
		return 0;
	}
	for (index = 0; index < MIN(handle->priv_num, MLAN_MAX_BSS_NUM);
	     index++) {
		priv = handle->priv[index];
		if (priv) {
			ptr += sprintf(ptr, "[Interface : %s]\n",
				       priv->proc_entry_name);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
			ptr += sprintf(ptr, "wmm_tx_pending[0] = %d\n",
				       atomic_read(&priv->wmm_tx_pending[0]));
			ptr += sprintf(ptr, "wmm_tx_pending[1] = %d\n",
				       atomic_read(&priv->wmm_tx_pending[1]));
			ptr += sprintf(ptr, "wmm_tx_pending[2] = %d\n",
				       atomic_read(&priv->wmm_tx_pending[2]));
			ptr += sprintf(ptr, "wmm_tx_pending[3] = %d\n",
				       atomic_read(&priv->wmm_tx_pending[3]));
#endif
			ptr += sprintf(ptr, "Media state = \"%s\"\n",
				       ((priv->media_connected == MFALSE) ?
						"Disconnected" :
						"Connected"));
			ptr += sprintf(ptr, "carrier %s\n",
				       ((netif_carrier_ok(priv->netdev)) ?
						"on" :
						"off"));
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
			for (i = 0; i < (int)(priv->netdev->num_tx_queues);
			     i++) {
				ptr += sprintf(
					ptr, "tx queue %d: %s\n", i,
					((netif_tx_queue_stopped(
						 netdev_get_tx_queue(
							 priv->netdev, i))) ?
						 "stopped" :
						 "started"));
			}
#else
			ptr += sprintf(ptr, "tx queue %s\n",
				       ((netif_queue_stopped(priv->netdev)) ?
						"stopped" :
						"started"));
#endif
			ptr += sprintf(ptr, "%s: num_tx_timeout = %d\n",
				       priv->netdev->name,
				       priv->num_tx_timeout);
		}
	}

	LEAVE();
	return ptr - (char *)buf;
}

/**
 *  @brief This function save moal_handle's info
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer buffer saving log
 *
 *  @return          The length of this log
 */
static int woal_dump_moal_drv_info(moal_handle *phandle, t_u8 *buf)
{
	char *ptr;
#ifdef USB
	struct usb_card_rec *cardp = NULL;
#endif
	char str_buf[MLAN_MAX_VER_STR_LEN];

	ENTER();
	if (!phandle || !buf) {
		PRINTM(MMSG, "%s: can't retreive info\n", __func__);
		LEAVE();
		return 0;
	}
#ifdef USB
	if (IS_USB(phandle->card_type))
		cardp = (struct usb_card_rec *)phandle->card;
#endif
	ptr = (char *)buf;
	ptr += sprintf(ptr, "------------moal_debug_info-------------\n");
	woal_get_version(phandle, str_buf, sizeof(str_buf) - 1);
	ptr += sprintf(ptr, "Driver version = %s\n", str_buf);
	ptr += sprintf(ptr, "main_state = %d\n", phandle->main_state);
#ifdef USB
	if (IS_USB(phandle->card_type)) {
		ptr += sprintf(ptr, "tx_cmd_urb_pending = %d\n",
			       atomic_read(&cardp->tx_cmd_urb_pending));
		ptr += sprintf(ptr, "tx_data_urb_pending = %d\n",
			       atomic_read(&cardp->tx_data_urb_pending));
#ifdef USB_CMD_DATA_EP
		ptr += sprintf(ptr, "rx_cmd_urb_pending = %d\n",
			       atomic_read(&cardp->rx_cmd_urb_pending));
#endif
		ptr += sprintf(ptr, "rx_data_urb_pending = %d\n",
			       atomic_read(&cardp->rx_data_urb_pending));
	}
#endif
	ptr += sprintf(ptr, "ioctl_pending = %d\n",
		       atomic_read(&phandle->ioctl_pending));
	ptr += sprintf(ptr, "tx_pending = %d\n",
		       atomic_read(&phandle->tx_pending));
	ptr += sprintf(ptr, "rx_pending = %d\n",
		       atomic_read(&phandle->rx_pending));
	ptr += sprintf(ptr, "lock_count = %d\n",
		       atomic_read(&phandle->lock_count));
	ptr += sprintf(ptr, "malloc_count = %d\n",
		       atomic_read(&phandle->malloc_count));
	ptr += sprintf(ptr, "mbufalloc_count = %d\n",
		       atomic_read(&phandle->mbufalloc_count));
#ifdef PCIE
	if (IS_PCIE(phandle->card_type)) {
		ptr += sprintf(ptr, "malloc_cons_count = %d\n",
			       atomic_read(&phandle->malloc_cons_count));
	}
#endif
	ptr += sprintf(ptr, "hs_skip_count = %u\n", phandle->hs_skip_count);
	ptr += sprintf(ptr, "hs_force_count = %u\n", phandle->hs_force_count);

	ptr += woal_dump_priv_drv_info(phandle, ptr);
	ptr += sprintf(ptr, "------------moal_debug_info End-------------\n");

	if (phandle->ops.dump_reg_info)
		ptr += phandle->ops.dump_reg_info(phandle, ptr);

	LEAVE();
	return ptr - (char *)buf;
}

/**
 *  @brief This function save mlan's info
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer buffer saving log
 *
 *  @return          The length of this log
 */
static int woal_dump_mlan_drv_info(moal_private *priv, t_u8 *buf)
{
	char *ptr = (char *)buf;
	int i;
#ifdef SDIO
	int j;
	t_u8 mp_aggr_pkt_limit = 0;
#endif
	char str[11 * DBG_CMD_NUM + 1] = {0};
	char *s;
	mlan_debug_info *info = NULL;

	ENTER();
	if (!priv || !priv->phandle) {
		PRINTM(MERROR, "priv or priv->phandle is null\n");
		LEAVE();
		return 0;
	}
	info = &(priv->phandle->debug_info);
	if (woal_get_debug_info(priv, MOAL_IOCTL_WAIT, info)) {
		PRINTM(MERROR,
		       "Could not retrieve debug information from MLAN\n");
		LEAVE();
		return 0;
	}
	ptr += sprintf(ptr, "------------mlan_debug_info-------------\n");
	ptr += sprintf(ptr, "mlan_processing =%d\n", info->mlan_processing);
	ptr += sprintf(ptr, "main_lock_flag =%d\n", info->main_lock_flag);
	ptr += sprintf(ptr, "main_process_cnt =%d\n", info->main_process_cnt);
	ptr += sprintf(ptr, "delay_task_flag =%d\n", info->delay_task_flag);
	ptr += sprintf(ptr, "mlan_rx_processing =%d\n",
		       info->mlan_rx_processing);
	ptr += sprintf(ptr, "rx_pkts_queued =%d\n", info->rx_pkts_queued);
	ptr += sprintf(ptr, "tx_pkts_queued =%d\n", info->tx_pkts_queued);
	ptr += sprintf(ptr, "fw_hang_report = %d\n", info->fw_hang_report);
	ptr += sprintf(ptr, "num_cmd_timeout = %d\n", info->num_cmd_timeout);
	ptr += sprintf(ptr, "Timeout cmd id = 0x%x, act = 0x%x\n",
		       info->timeout_cmd_id, info->timeout_cmd_act);
	ptr += sprintf(ptr, "last_cmd_index = %d\n", info->last_cmd_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_id[i]);
	ptr += sprintf(ptr, "last_cmd_id = %s\n", str);

	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_act[i]);

	ptr += sprintf(ptr, "last_cmd_act = %s\n", str);
	ptr += sprintf(ptr, "last_cmd_resp_index = %d\n",
		       info->last_cmd_resp_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_cmd_resp_id[i]);

	ptr += sprintf(ptr, "last_cmd_resp_id = %s\n", str);
	ptr += sprintf(ptr, "last_event_index = %d\n", info->last_event_index);
	for (s = str, i = 0; i < DBG_CMD_NUM; i++)
		s += sprintf(s, "0x%x ", info->last_event[i]);

	ptr += sprintf(ptr, "last_event = %s\n", str);
	ptr += sprintf(ptr, "num_data_h2c_failure = %d\n",
		       info->num_tx_host_to_card_failure);
	ptr += sprintf(ptr, "num_cmd_h2c_failure = %d\n",
		       info->num_cmd_host_to_card_failure);
	ptr += sprintf(ptr, "num_alloc_buffer_failure = %d\n",
		       info->num_alloc_buffer_failure);
	ptr += sprintf(ptr, "num_pkt_dropped = %d\n", info->num_pkt_dropped);
#ifdef SDIO
	if (IS_SD(priv->phandle->card_type)) {
		ptr += sprintf(ptr, "num_data_c2h_failure = %d\n",
			       info->num_rx_card_to_host_failure);
		ptr += sprintf(ptr, "num_cmdevt_c2h_failure = %d\n",
			       info->num_cmdevt_card_to_host_failure);
		ptr += sprintf(ptr, "num_int_read_failure = %d\n",
			       info->num_int_read_failure);
		ptr += sprintf(ptr, "last_int_status = %d\n",
			       info->last_int_status);
		ptr += sprintf(ptr, "mp_rd_bitmap=0x%x curr_rd_port=0x%x\n",
			       (unsigned int)info->mp_rd_bitmap,
			       info->curr_rd_port);
		ptr += sprintf(ptr, "mp_wr_bitmap=0x%x curr_wr_port=0x%x\n",
			       (unsigned int)info->mp_wr_bitmap,
			       info->curr_wr_port);
		ptr += sprintf(ptr, "mp_data_port_mask=0x%x\n",
			       info->mp_data_port_mask);
		ptr += sprintf(
			ptr, "last_recv_rd_bitmap=0x%x mp_invalid_update=%d\n",
			info->last_recv_rd_bitmap, info->mp_invalid_update);
		mp_aggr_pkt_limit = info->mp_aggr_pkt_limit;
		ptr += sprintf(ptr,
			       "last_recv_wr_bitmap=0x%x last_mp_index = %d\n",
			       info->last_recv_wr_bitmap, info->last_mp_index);
		for (i = 0; i < SDIO_MP_DBG_NUM; i++) {
			for (s = str, j = 0; j < mp_aggr_pkt_limit; j++)
				s += sprintf(
					s, "0x%02x ",
					info->last_mp_wr_info
						[i * mp_aggr_pkt_limit + j]);

			ptr += sprintf(
				ptr,
				"mp_wr_bitmap: 0x%x mp_wr_ports=0x%x len=%d curr_wr_port=0x%x\n%s\n",
				info->last_mp_wr_bitmap[i],
				info->last_mp_wr_ports[i],
				info->last_mp_wr_len[i],
				info->last_curr_wr_port[i], str);
		}
	}
#endif
#ifdef PCIE
	if (IS_PCIE(priv->phandle->card_type)) {
		ptr += sprintf(ptr, "txbd_rdptr=0x%x txbd_wrptr=0x%x\n",
			       info->txbd_rdptr, info->txbd_wrptr);
		ptr += sprintf(ptr, "rxbd_rdptr=0x%x rxbd_wrptr=0x%x\n",
			       info->rxbd_rdptr, info->rxbd_wrptr);
		ptr += sprintf(ptr, "eventbd_rdptr=0x%x event_wrptr=0x%x\n",
			       info->eventbd_rdptr, info->eventbd_wrptr);
		ptr += sprintf(ptr, "last_wr_index:%d\n",
			       info->txbd_wrptr & (info->txrx_bd_size - 1));
		ptr += sprintf(ptr, "TxRx BD size:%d\n", info->txrx_bd_size);
	}
#endif
	ptr += sprintf(ptr, "num_event_deauth = %d\n", info->num_event_deauth);
	ptr += sprintf(ptr, "num_event_disassoc = %d\n",
		       info->num_event_disassoc);
	ptr += sprintf(ptr, "num_event_link_lost = %d\n",
		       info->num_event_link_lost);
	ptr += sprintf(ptr, "num_cmd_deauth = %d\n", info->num_cmd_deauth);
	ptr += sprintf(ptr, "num_cmd_assoc_success = %d\n",
		       info->num_cmd_assoc_success);
	ptr += sprintf(ptr, "num_cmd_assoc_failure = %d\n",
		       info->num_cmd_assoc_failure);
	ptr += sprintf(ptr, "num_cons_assoc_failure = %d\n",
		       info->num_cons_assoc_failure);
	ptr += sprintf(ptr, "cmd_resp_received = %d\n",
		       info->cmd_resp_received);
	ptr += sprintf(ptr, "event_received = %d\n", info->event_received);
	ptr += sprintf(ptr, "max_tx_buf_size = %d\n", info->max_tx_buf_size);
	ptr += sprintf(ptr, "tx_buf_size = %d\n", info->tx_buf_size);
	ptr += sprintf(ptr, "curr_tx_buf_size = %d\n", info->curr_tx_buf_size);
	ptr += sprintf(ptr, "bypass_pkt_count=%d\n", info->bypass_pkt_count);

	ptr += sprintf(ptr, "data_sent=%d cmd_sent=%d\n", info->data_sent,
		       info->cmd_sent);
	ptr += sprintf(ptr, "data_sent_cnt=%u\n", info->data_sent_cnt);
	ptr += sprintf(ptr, "ps_mode=%d ps_state=%d\n", info->ps_mode,
		       info->ps_state);
	ptr += sprintf(
		ptr, "wakeup_dev_req=%d wakeup_tries=%d pm_wakeup_timeout=%d\n",
		info->pm_wakeup_card_req, info->pm_wakeup_fw_try,
		info->pm_wakeup_timeout);
	ptr += sprintf(ptr, "hs_configured=%d hs_activated=%d\n",
		       info->is_hs_configured, info->hs_activated);
	ptr += sprintf(ptr, "pps_uapsd_mode=%d sleep_pd=%d\n",
		       info->pps_uapsd_mode, info->sleep_pd);
	ptr += sprintf(ptr, "tx_lock_flag = %d\n", info->tx_lock_flag);
	ptr += sprintf(ptr, "port_open = %d\n", info->port_open);
	ptr += sprintf(ptr, "scan_processing = %d\n", info->scan_processing);
	ptr += sprintf(ptr, "scan_state = %d\n", info->scan_state);

#ifdef PCIE
	if (IS_PCIE(priv->phandle->card_type)) {
		ptr += sprintf(ptr, "txbd: rdptr=0x%x wrptr=0x%x\n",
			       info->txbd_rdptr, info->txbd_wrptr);
		ptr += sprintf(ptr, "rxbd: rdptr=0x%x wrptr=0x%x\n",
			       info->rxbd_rdptr, info->rxbd_wrptr);
		ptr += sprintf(ptr, "eventbd: rdptr=0x%x wrptr=0x%x\n",
			       info->eventbd_rdptr, info->eventbd_wrptr);
		ptr += sprintf(ptr, "TXBD Ring:\n");
		ptr += woal_save_hex_dump(ROW_SIZE_16, info->txbd_ring_vbase,
					  info->txbd_ring_size, MTRUE, ptr);
		ptr += sprintf(ptr, "RXBD Ring:\n");
		ptr += woal_save_hex_dump(ROW_SIZE_16, info->rxbd_ring_vbase,
					  info->rxbd_ring_size, MTRUE, ptr);
		ptr += sprintf(ptr, "EVTBD Ring:\n");
		ptr += woal_save_hex_dump(ROW_SIZE_16, info->evtbd_ring_vbase,
					  info->evtbd_ring_size, MTRUE, ptr);
	}
#endif
	ptr += sprintf(ptr, "------------mlan_debug_info End-------------\n");

	LEAVE();
	return ptr - (char *)buf;
}

/**
 *  @brief This function dump moal hex to file
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer to buffer
 *
 *  @return          The length of this log
 */
static int woal_dump_moal_hex(moal_handle *phandle, t_u8 *buf)
{
	char *ptr = (char *)buf;
	int i;
	ENTER();

	if (!phandle || !buf) {
		PRINTM(MMSG, "%s: can't retreive info\n", __func__);
		LEAVE();
		return 0;
	}

	ptr += sprintf(ptr, "<--moal_handle-->\n");
	ptr += sprintf(ptr, "moal_handle=%p, size=%ld(0x%lx)\n", phandle,
		       (long int)sizeof(*phandle),
		       (long unsigned int)sizeof(*phandle));
	ptr += woal_save_hex_dump(ROW_SIZE_16, phandle, sizeof(*phandle), MTRUE,
				  ptr);
	ptr += sprintf(ptr, "<--moal_handle End-->\n");

	for (i = 0; i < phandle->priv_num; i++) {
		if (!phandle->priv[i])
			continue;
		ptr += sprintf(ptr, "<--moal_private(%d) %s-->\n", i,
			       phandle->priv[i]->netdev->name);
		ptr += sprintf(ptr, "moal_private=%p, size=%ld(0x%lx)\n",
			       phandle->priv[i],
			       (long int)sizeof(*(phandle->priv[i])),
			       (long unsigned int)sizeof(*(phandle->priv[i])));
		ptr += woal_save_hex_dump(ROW_SIZE_16, phandle->priv[i],
					  sizeof(*(phandle->priv[i])), MTRUE,
					  ptr);
		ptr += sprintf(ptr, "<--moal_private(%d) End-->\n", i);
	}
	LEAVE();
	return ptr - (char *)buf;
}

/**
 *  @brief This function dump mlan hex to file
 *
 *  @param priv   A pointer to moal_private structure
 *  @param buf       A pointer to buffer
 *
 *  @return          The length of this log
 */
static int woal_dump_mlan_hex(moal_private *priv, t_u8 *buf)
{
	char *ptr = (char *)buf;
	int i;
	mlan_debug_info *info = NULL;

	ENTER();

	if (!buf || !priv || !priv->phandle) {
		PRINTM(MERROR, "buf priv or priv->phandle is null\n");
		LEAVE();
		return 0;
	}
	info = &(priv->phandle->debug_info);
	if (woal_get_debug_info(priv, MOAL_IOCTL_WAIT, info)) {
		PRINTM(MMSG, "%s: can't retreive info\n", __func__);
		LEAVE();
		return 0;
	}

	ptr += sprintf(ptr, "<--mlan_adapter-->\n");
	ptr += sprintf(ptr, "mlan_adapter=%p, size=%d(0x%x)\n",
		       info->mlan_adapter, info->mlan_adapter_size,
		       info->mlan_adapter_size);
	ptr += woal_save_hex_dump(ROW_SIZE_16, info->mlan_adapter,
				  info->mlan_adapter_size, MTRUE, ptr);
	ptr += sprintf(ptr, "<--mlan_adapter End-->\n");
#ifdef SDIO
	if (IS_SD(priv->phandle->card_type) && info->mpa_buf &&
	    info->mpa_buf_size) {
		ptr += sprintf(ptr, "<--mlan_mpa_buf-->\n");
		ptr += sprintf(ptr, "mlan_mpa_buf=%p, size=%d(0x%x)\n",
			       info->mpa_buf, info->mpa_buf_size,
			       info->mpa_buf_size);
		ptr += woal_save_hex_dump(ROW_SIZE_16, info->mpa_buf,
					  info->mpa_buf_size, MTRUE, ptr);
		ptr += sprintf(ptr, "<--mlan_mpa_buf End-->\n");
	}
#endif
	for (i = 0; i < info->mlan_priv_num; i++) {
		ptr += sprintf(ptr, "<--mlan_private(%d)-->\n", i);
		ptr += sprintf(ptr, "mlan_private=%p, size=%d(0x%x)\n",
			       info->mlan_priv[i], info->mlan_priv_size[i],
			       info->mlan_priv_size[i]);
		ptr += woal_save_hex_dump(ROW_SIZE_16, info->mlan_priv[i],
					  info->mlan_priv_size[i], MTRUE, ptr);
		ptr += sprintf(ptr, "<--mlan_private(%d) End-->\n", i);
	}

	LEAVE();
	return ptr - (char *)buf;
}

/**
 *  @brief This function dump drv info to file
 *
 *  @param phandle   A pointer to moal_handle
 *  @param dump_len  A point to hold the len of drv info memory
 *
 *  @return         A pointer to drv_info memory
 */
t_u8 *woal_dump_drv_info(moal_handle *phandle, t_u32 *dump_len)
{
	t_u8 *drv_buf = NULL;
	t_u32 len = 0;
	t_u32 total_len = 0;
	t_u32 drv_info_size = DRV_INFO_SIZE;
	int ret;
	if (!phandle->priv_num)
		return NULL;
	if (phandle->priv_num > 3)
		drv_info_size += (phandle->priv_num - 3) * DRV_INFO_PER_INTF;
	PRINTM(MERROR, "=== START DRIVER INFO DUMP===");
	ret = moal_vmalloc(phandle, drv_info_size, &drv_buf);
	if ((ret != MLAN_STATUS_SUCCESS) || !drv_buf) {
		PRINTM(MERROR, "Error: vmalloc drv buffer failed!\n");
		goto done;
	}

	len = woal_dump_moal_drv_info(phandle, drv_buf);
	total_len += len;
	len = woal_dump_mlan_drv_info(woal_get_priv(phandle, MLAN_BSS_ROLE_ANY),
				      drv_buf + total_len);
	total_len += len;
	len = woal_dump_moal_hex(phandle, drv_buf + total_len);
	total_len += len;
	len = woal_dump_mlan_hex(woal_get_priv(phandle, MLAN_BSS_ROLE_ANY),
				 drv_buf + total_len);
	total_len += len;

	PRINTM(MERROR, "Drv info total bytes = %ld (0x%lx)\n",
	       (long int)total_len, (long unsigned int)total_len);
	PRINTM(MERROR, "=== DRIVER INFO DUMP END===");
	*dump_len = total_len;
done:
	return drv_buf;
}

/**
 *  @brief This function adds header and copy the src data to buf
 *
 *  @param phandle   A pointer to moal_handle
 *  @param src     A ponter to source buffer
 *  @param len     Length of raw data
 *  @param type   Dump type
 *
 *  return Total len of buf
 */
int woal_save_dump_info_to_buf(moal_handle *phandle, t_u8 *src, t_u32 len,
			       t_u32 type)
{
	mem_dump_header header;
	t_u32 left_len = 0;
	t_u32 len_to_copy = 0;
	int total_len = 0;
	t_u8 *dest = NULL;
	int count = 0;
	int pad = 0;
	dest = phandle->fw_dump_buf + phandle->fw_dump_len;

	left_len = len;
	while (left_len) {
		header.type = type;
		if (left_len < 1024)
			len_to_copy = left_len;
		else
			len_to_copy = 1024;
		header.len = (t_u16)(len_to_copy + sizeof(t_u32));
		header.start_addr = count * 1024;
		moal_memcpy_ext(phandle, dest, &header, sizeof(mem_dump_header),
				FW_DUMP_INFO_LEN - phandle->fw_dump_len);
		dest += sizeof(mem_dump_header);
		moal_memcpy_ext(phandle, dest, src, len_to_copy,
				FW_DUMP_INFO_LEN - phandle->fw_dump_len -
					sizeof(mem_dump_header));
		dest += len_to_copy;
		src += len_to_copy;
		left_len -= len_to_copy;
		// 8 bytes align
		pad = (header.len & 7) ? (8 - (header.len & 7)) : 0;
		dest += pad;
		total_len += pad + len_to_copy + sizeof(mem_dump_header);
		count++;
	}
	phandle->fw_dump_len += total_len;
	PRINTM(MMSG, "type=%d, len=%d  block=%d  total=%d\n", type, len, count,
	       total_len);
	return total_len;
}

/**
 *  @brief This function append end block to dump file
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  return N/A
 */
void woal_append_end_block(moal_handle *phandle)
{
	mem_dump_header header;
	t_u8 *pos = phandle->fw_dump_buf + phandle->fw_dump_len;
	ENTER();
	memset(&header, 0, sizeof(header));
	header.type = FW_DUMP_TYPE_ENDED;
	header.len = 0;
	moal_memcpy_ext(phandle, pos, &header, sizeof(mem_dump_header),
			FW_DUMP_INFO_LEN - phandle->fw_dump_len);
	phandle->fw_dump_len += sizeof(mem_dump_header);
	PRINTM(MMSG, "fw dump total length is %ld\n",
	       (long int)phandle->fw_dump_len);
	LEAVE();
	return;
}

/**
 *  @brief This function displays extra MOAL debug information
 *
 *  @param priv     A pointer to moal_private
 *  @param handle   A pointer to moal_handle
 *  @param flag     Indicates whether register read can be done directly
 *
 *  @return         N/A
 */
void woal_moal_debug_info(moal_private *priv, moal_handle *handle, u8 flag)
{
	moal_handle *phandle = NULL;
#ifdef USB
	struct usb_card_rec *cardp = NULL;
#endif
	char buf[MLAN_MAX_VER_STR_LEN];
	int i = 0;

	ENTER();

	if (!priv) {
		if (handle) {
			phandle = handle;
		} else {
			PRINTM(MERROR,
			       "Could not retrieve debug information from MOAL\n");
			LEAVE();
			return;
		}
	} else {
		phandle = priv->phandle;
	}
#ifdef USB
	if (IS_USB(phandle->card_type)) {
		cardp = (struct usb_card_rec *)phandle->card;
		PRINTM(MERROR, "tx_cmd_urb_pending = %d\n",
		       atomic_read(&cardp->tx_cmd_urb_pending));
		PRINTM(MERROR, "tx_data_urb_pending = %d\n",
		       atomic_read(&cardp->tx_data_urb_pending));
#ifdef USB_CMD_DATA_EP
		PRINTM(MERROR, "rx_cmd_urb_pending = %d\n",
		       atomic_read(&cardp->rx_cmd_urb_pending));
#endif
		PRINTM(MERROR, "rx_data_urb_pending = %d\n",
		       atomic_read(&cardp->rx_data_urb_pending));
	}
#endif

	woal_get_version(phandle, buf, sizeof(buf) - 1);
	PRINTM(MERROR, "Driver version = %s\n", buf);
	PRINTM(MERROR, "main_state = %d\n", phandle->main_state);
	PRINTM(MERROR, "ioctl_pending = %d\n",
	       atomic_read(&phandle->ioctl_pending));
	PRINTM(MERROR, "tx_pending = %d\n", atomic_read(&phandle->tx_pending));
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	if (priv) {
		PRINTM(MERROR, "wmm_tx_pending[0] = %d\n",
		       atomic_read(&priv->wmm_tx_pending[0]));
		PRINTM(MERROR, "wmm_tx_pending[1] = %d\n",
		       atomic_read(&priv->wmm_tx_pending[1]));
		PRINTM(MERROR, "wmm_tx_pending[2] = %d\n",
		       atomic_read(&priv->wmm_tx_pending[2]));
		PRINTM(MERROR, "wmm_tx_pending[3] = %d\n",
		       atomic_read(&priv->wmm_tx_pending[3]));
	}
#endif
	PRINTM(MERROR, "rx_pending = %d\n", atomic_read(&phandle->rx_pending));
	PRINTM(MERROR, "lock_count = %d\n", atomic_read(&phandle->lock_count));
	PRINTM(MERROR, "malloc_count = %d\n",
	       atomic_read(&phandle->malloc_count));
	PRINTM(MERROR, "mbufalloc_count = %d\n",
	       atomic_read(&phandle->mbufalloc_count));
#ifdef PCIE
	if (IS_PCIE(phandle->card_type)) {
		PRINTM(MERROR, "malloc_cons_count = %d\n",
		       atomic_read(&phandle->malloc_cons_count));
	}
#endif
	PRINTM(MERROR, "hs_skip_count = %u\n", phandle->hs_skip_count);
	PRINTM(MERROR, "hs_force_count = %u\n", phandle->hs_force_count);

	if (priv && priv->netdev) {
		PRINTM(MERROR, "Media state = \"%s\"\n",
		       ((priv->media_connected == MFALSE) ? "Disconnected" :
							    "Connected"));
		PRINTM(MERROR, "carrier %s\n",
		       ((netif_carrier_ok(priv->netdev)) ? "on" : "off"));
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
		for (i = 0; i < (int)(priv->netdev->num_tx_queues); i++) {
			PRINTM(MERROR, "tx queue %d: %s\n", i,
			       ((netif_tx_queue_stopped(
					netdev_get_tx_queue(priv->netdev, i))) ?
					"stopped" :
					"started"));
		}
#else
		PRINTM(MERROR, "tx queue %s\n",
		       ((netif_queue_stopped(priv->netdev)) ? "stopped" :
							      "started"));
#endif
	}

	for (i = 0; i < phandle->priv_num; i++) {
		priv = phandle->priv[i];
		if (priv && priv->netdev)
			PRINTM(MERROR, "%s: num_tx_timeout = %d\n",
			       priv->netdev->name, priv->num_tx_timeout);
	}

	if (phandle->is_suspended == MTRUE) {
		LEAVE();
		return;
	}

#ifdef PCIE
	if (IS_PCIE(phandle->card_type)) {
#ifdef DEBUG_LEVEL1
		if (phandle->ops.reg_dbg && (drvdbg & (MREG_D | MFW_D))) {
			if (!phandle->event_fw_dump)
				phandle->ops.reg_dbg(phandle);
		}
#endif
	}
#endif
#ifdef SDIO
	if (IS_SD(phandle->card_type)) {
		if (flag && ((phandle->main_state == MOAL_END_MAIN_PROCESS) ||
			     (phandle->main_state == MOAL_STATE_IDLE))) {
#ifdef DEBUG_LEVEL1
			if (phandle->ops.reg_dbg &&
			    (drvdbg & (MREG_D | MFW_D))) {
				if (!phandle->event_fw_dump)
					phandle->ops.reg_dbg(phandle);
			}
#endif
		} else {
#ifdef DEBUG_LEVEL1
			if (drvdbg & (MREG_D | MFW_D)) {
				if (!phandle->event_fw_dump) {
					phandle->reg_dbg = MTRUE;
					queue_work(phandle->workqueue,
						   &phandle->main_work);
				}
			}
#endif
		}
	}
#endif
#ifdef DEBUG_LEVEL1
	if ((drvdbg & MFW_D) && !phandle->fw_dump_status) {
		phandle->fw_dump_status = MTRUE;
		phandle->fw_dbg = MTRUE;
		queue_work(phandle->workqueue, &phandle->main_work);
	}
#endif
	LEAVE();
	return;
}

/**
 *    @brief Download power table to firmware for a specific country
 *
 *    @param priv         A pointer to moal_private
 *    @param country      ISO 3166-1 alpha-2 country code
 *
 *    @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_request_country_power_table(moal_private *priv, char *country)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	moal_handle *handle = NULL;
	char country_name[128];
	char file_path[256];
	char *last_slash = NULL;
	char *fw_name = NULL;

	ENTER();

	if (!priv || !priv->phandle) {
		PRINTM(MERROR, "Priv or handle is null\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	if (!country) {
		PRINTM(MERROR, "Country is null\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	handle = priv->phandle;
	memset(country_name, 0, sizeof(country_name));
	if (handle->params.hw_name)
		sprintf(country_name, "%s_txpower_XX.bin",
			handle->params.hw_name);
	else
		memcpy(country_name, "txpower_XX.bin",
		       strlen("txpower_XX.bin"));
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	if (handle->params.cntry_txpwr == CNTRY_RGPOWER_MODE) {
		memset(country_name, 0, sizeof(country_name));
		if (handle->params.hw_name)
			sprintf(country_name, "%s_rgpower_XX.bin",
				handle->params.hw_name);
		else
			memcpy(country_name, "rgpower_XX.bin",
			       strlen("rgpower_XX.bin"));
	}
#endif
#endif

	/* Replace XX with ISO 3166-1 alpha-2 country code */
	memcpy(strstr(country_name, "XX"), country, strlen(country));
	fw_name = handle->params.fw_name;
	memset(file_path, 0, sizeof(file_path));
	/* file_path should be Null terminated */
	if (fw_name) {
		moal_memcpy(handle, file_path, fw_name, sizeof(file_path) - 1);
		last_slash = strrchr(file_path, '/');
		if (last_slash)
			memset(last_slash + 1, 0,
			       sizeof(file_path) - 1 -
				       (last_slash - file_path));
		else
			memset(file_path, 0, sizeof(file_path));
	} else {
		memcpy(file_path, "nxp/",
		       MIN((sizeof(file_path) - 1), strlen("nxp/")));
	}

	if ((strlen(file_path) + strlen(country_name)) <
	    (sizeof(file_path) - 1))
		strncpy(file_path + strlen(file_path), country_name,
			sizeof(file_path) - strlen(file_path) - 1);
	else {
		PRINTM(MERROR,
		       "file path buffer too small, fail to dnld power table\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	PRINTM(MMSG, "Trying download country_power_tble: %s\n", file_path);
	ret = woal_set_user_init_data(handle, COUNTRY_POWER_TABLE,
				      MOAL_IOCTL_WAIT, file_path);
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	/* Try download WW rgpowertable */
	if ((handle->params.cntry_txpwr == CNTRY_RGPOWER_MODE) &&
	    (ret == MLAN_STATUS_FILE_ERR)) {
		memset(country_name, 0, sizeof(country_name));
		if (handle->params.hw_name)
			sprintf(country_name, "%s_rgpower_WW.bin",
				handle->params.hw_name);
		else
			memcpy(country_name, "rgpower_WW.bin",
			       strlen("rgpower_WW.bin"));
		last_slash = strrchr(file_path, '/');
		if (last_slash)
			memset(last_slash + 1, 0,
			       sizeof(file_path) - 1 -
				       (last_slash - file_path));
		else
			memset(file_path, 0, sizeof(file_path));
		if ((strlen(file_path) + strlen(country_name)) <
		    (sizeof(file_path) - 1))
			strncpy(file_path + strlen(file_path), country_name,
				sizeof(file_path) - strlen(file_path) - 1);
		else {
			PRINTM(MERROR,
			       "file path buffer too small, fail to dnld power table\n");
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}
		PRINTM(MMSG, "Trying again download country_power_tble: %s\n",
		       file_path);
		ret = woal_set_user_init_data(handle, COUNTRY_POWER_TABLE,
					      MOAL_IOCTL_WAIT, file_path);
		if (!ret) {
			handle->country_code[0] = '0';
			handle->country_code[1] = '0';
		}
	}
#endif
#endif
	LEAVE();
	return ret;
}

/**
 *  @brief napi polling call back function.
 *
 *  @param napi     A pointer to napi_struct
 *  @param budget   the limit of packets driver should poll
 *
 *  @return       packets received
 */
static int woal_netdev_poll_rx(struct napi_struct *napi, int budget)
{
	moal_handle *handle = container_of(napi, moal_handle, napi_rx);
	t_u8 recv = budget;

	ENTER();
	if (handle->surprise_removed == MTRUE) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
		if (false == napi_complete(napi))
			PRINTM(MINFO, "%s: napi_complete with false \n",
			       __func__);
#else
		napi_complete(napi);
#endif
		LEAVE();
		return 0;
	}
	if (MLAN_STATUS_SUCCESS !=
	    mlan_rx_process(handle->pmlan_adapter, &recv))
		PRINTM(MERROR, "%s: mlan_rx_process failed \n", __func__);
	if (recv < budget) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
		if (false == napi_complete(napi))
			PRINTM(MINFO, "%s: napi_complete with false \n",
			       __func__);
#else
		napi_complete(napi);
#endif
	}
	LEAVE();
	return recv;
}

/**
 *  @brief This workqueue function handles set multicast_list
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_mclist_work_queue(struct work_struct *work)
{
	moal_private *priv = container_of(work, moal_private, mclist_work);
	woal_request_set_multicast_list(priv, priv->netdev);
}

#ifdef STA_CFG80211
/**
 *  @brief This workqueue function handles woal scan timeout work
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_scan_timeout_handler(struct work_struct *work)
{
	struct delayed_work *delayed_work = to_delayed_work(work);
	moal_handle *handle =
		container_of(delayed_work, moal_handle, scan_timeout_work);
	unsigned long flags;
	moal_private *priv = woal_get_priv(handle, MLAN_BSS_ROLE_STA);
	t_u8 auto_fw_dump = MFALSE;

	ENTER();

	if (IS_STA_CFG80211(handle->params.cfg80211_wext)) {
		if (handle->scan_request && handle->fake_scan_complete) {
			PRINTM(MMSG, "wlan: Send fake scan result\n");
			if (priv)
				woal_inform_bss_from_scan_result(priv, NULL,
								 MOAL_NO_WAIT);
			spin_lock_irqsave(&handle->scan_req_lock, flags);
			woal_cfg80211_scan_done(handle->scan_request, MFALSE);
			handle->scan_request = NULL;
			spin_unlock_irqrestore(&handle->scan_req_lock, flags);
		} else if (handle->scan_request) {
			PRINTM(MMSG, "wlan: scan timeout!\n");
#ifdef DEBUG_LEVEL1
			if (drvdbg & MFW_D)
				auto_fw_dump = MTRUE;
#endif
			if (priv) {
				woal_mlan_debug_info(priv);
				woal_moal_debug_info(priv, NULL, MFALSE);
			}
			handle->driver_status = MTRUE;
			if (!auto_fw_dump && !handle->fw_dump && priv)
				woal_process_hang(priv->phandle);
			wifi_status = WIFI_STATUS_SCAN_TIMEOUT;
		}
	}

	LEAVE();
}
#endif

/**
 *  @brief This workqueue function handles woal event queue
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_evt_work_queue(struct work_struct *work)
{
	moal_handle *handle = container_of(work, moal_handle, evt_work);
	struct woal_event *evt;
	unsigned long flags;
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
	moal_private *priv;
#endif
#endif
	ENTER();
	if (handle->surprise_removed == MTRUE) {
		LEAVE();
		return;
	}
	spin_lock_irqsave(&handle->evt_lock, flags);
	while (!list_empty(&handle->evt_queue)) {
		evt = list_first_entry(&handle->evt_queue, struct woal_event,
				       link);
		list_del(&evt->link);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		switch (evt->type) {
		case WOAL_EVENT_CHAN_SWITCH:
#if defined(UAP_SUPPORT) || defined(STA_SUPPORT)
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
			woal_cfg80211_notify_channel((moal_private *)evt->priv,
						     &evt->chan_info);
#endif
#endif
#endif
			break;
		case WOAL_EVENT_RX_MGMT_PKT:
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
			priv = evt->priv;
			mutex_lock(&priv->wdev->mtx);
			cfg80211_rx_mlme_mgmt(priv->netdev, evt->evt.event_buf,
					      evt->evt.event_len);
			mutex_unlock(&priv->wdev->mtx);
#endif
#endif
			break;
		case WOAL_EVENT_BGSCAN_STOP:
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
			woal_cfg80211_notify_sched_scan_stop(
				(moal_private *)evt->priv);
#endif
#endif
			break;
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		case WOAL_EVENT_DEAUTH:
			priv = evt->priv;
			woal_host_mlme_disconnect(evt->priv, evt->reason_code,
						  priv->cfg_bssid);
			break;

		case WOAL_EVENT_ASSOC_RESP:
			woal_host_mlme_process_assoc_resp(
				(moal_private *)evt->priv, &evt->assoc_info);
			break;
#endif
#endif
#ifdef UAP_SUPPORT
		case WOAL_EVENT_CHAN_RPT:
			woal_process_chan_event((moal_private *)evt->priv,
						WOAL_EVENT_CHAN_RPT,
						evt->radar_info.channel,
						evt->radar_info.radar);
			break;
		case WOAL_EVENT_RADAR:
			woal_process_chan_event((moal_private *)evt->priv,
						WOAL_EVENT_RADAR,
						evt->radar_info.channel,
						evt->radar_info.radar);
			break;
#endif
#ifdef UAP_CFG80211
#if KERNEL_VERSION(3, 12, 0) <= CFG80211_VERSION_CODE
		case WOAL_EVENT_CANCEL_CHANRPT:
			woal_process_cancel_chanrpt_event(
				(moal_private *)evt->priv);
			break;
#endif
#endif
		default:
			break;
		}
		kfree(evt);
		spin_lock_irqsave(&handle->evt_lock, flags);
	}
	spin_unlock_irqrestore(&handle->evt_lock, flags);
	LEAVE();
}
/**
 *  @brief This workqueue function handles rx_process
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_rx_work_queue(struct work_struct *work)
{
	moal_handle *handle = container_of(work, moal_handle, rx_work);
	wifi_timeval start_timeval;
	wifi_timeval end_timeval;

	ENTER();
	if (handle->surprise_removed == MTRUE) {
		LEAVE();
		return;
	}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
	if (handle->cfg80211_suspend == MTRUE) {
		LEAVE();
		return;
	}
#endif
#endif
	woal_get_monotonic_time(&start_timeval);
	if (MLAN_STATUS_SUCCESS != mlan_rx_process(handle->pmlan_adapter, NULL))
		PRINTM(MERROR, "%s: mlan_rx_process failed \n", __func__);

	woal_get_monotonic_time(&end_timeval);
	handle->rx_time += (t_u64)(timeval_to_usec(end_timeval) -
				   timeval_to_usec(start_timeval));
	PRINTM(MINFO,
	       "%s : start_timeval=%d:%d end_timeval=%d:%d inter=%llu rx_time=%llu\n",
	       __func__, start_timeval.time_sec, start_timeval.time_usec,
	       end_timeval.time_sec, end_timeval.time_usec,
	       (t_u64)(timeval_to_usec(end_timeval) -
		       timeval_to_usec(start_timeval)),
	       handle->rx_time);
	LEAVE();
}

/**
 *  @brief This function dequeue pkt from list
 *
 *  @param list    A pointer to struct sk_buff_head
 *
 *  @return        skb buffer
 */

struct sk_buff *woal_skb_dequeue_spinlock(struct sk_buff_head *list)
{
	struct sk_buff *result;

	spin_lock_bh(&list->lock);
	result = __skb_dequeue(list);
	spin_unlock_bh(&list->lock);
	return result;
}

/**
 *  @brief This workqueue function handles rx_work_process
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_tx_work_handler(struct work_struct *work)
{
	moal_handle *handle = container_of(work, moal_handle, tx_work);
	moal_private *priv = NULL;
	int i = 0;
	struct sk_buff *skb = NULL;

	ENTER();
	if (handle->surprise_removed == MTRUE) {
		LEAVE();
		return;
	}

	for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM); i++) {
		priv = handle->priv[i];
		while ((skb = woal_skb_dequeue_spinlock(&priv->tx_q)) != NULL) {
			woal_start_xmit(priv, skb);
		}
	}

	LEAVE();
}

/**
 *  @brief This workqueue function handles main_process
 *
 *  @param work    A pointer to work_struct
 *
 *  @return        N/A
 */
t_void woal_main_work_queue(struct work_struct *work)
{
	moal_handle *handle = container_of(work, moal_handle, main_work);
#ifdef USB
	struct usb_card_rec *cardp = (struct usb_card_rec *)handle->card;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
	struct sched_param sp = {.sched_priority = wq_sched_prio};
#endif

	ENTER();

	if (handle->surprise_removed == MTRUE) {
		LEAVE();
		return;
	}
	if (handle->reg_dbg == MTRUE) {
		handle->reg_dbg = MFALSE;
		if (handle->ops.reg_dbg)
			handle->ops.reg_dbg(handle);
	}
	if (handle->fw_dbg == MTRUE) {
		handle->fw_dbg = MFALSE;
		if (handle->ops.dump_fw_info)
			handle->ops.dump_fw_info(handle);
		LEAVE();
		return;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
	/* Change the priority and scheduling policy of main work queue
	 */
	if ((handle->params.wq_sched_prio != current->rt_priority) ||
	    (handle->params.wq_sched_policy != current->policy)) {
		PRINTM(MMSG,
		       "Set work queue priority %d and scheduling policy %d\n",
		       handle->params.wq_sched_prio,
		       handle->params.wq_sched_policy);
		sched_setscheduler(current, handle->params.wq_sched_policy,
				   &sp);
	}
#endif

	handle->main_state = MOAL_ENTER_WORK_QUEUE;
#ifdef USB
	/* WAR for no free skb issue */
	if (IS_USB(handle->card_type) && !atomic_read(&handle->rx_pending) &&
	    atomic_read(&cardp->rx_data_urb_pending) < MVUSB_RX_DATA_URB) {
		PRINTM(MWARN, "Try to resubmit Rx data URBs\n");
		woal_usb_submit_rx_data_urbs(handle);
	}
#endif
	handle->main_state = MOAL_START_MAIN_PROCESS;
	/* Call MLAN main process */
	mlan_main_process(handle->pmlan_adapter);
	handle->main_state = MOAL_END_MAIN_PROCESS;

	LEAVE();
}

#ifdef IMX_SUPPORT
/**
 *  @brief This function allocates the interrupt line to wakeup
 *         the host, and initializes the device for wakeup
 *
 *  @param handle  A pointer to moal_handle structure
 *
 *  @return        N/A
 */
void woal_regist_oob_wakeup_irq(moal_handle *handle)
{
	int ret;
	struct device *dev = handle->hotplug_device;
	struct device_node *node;

	ENTER();

	node = of_find_compatible_node(NULL, NULL, "nxp,wifi-wake-host");
	if (!node)
		goto err_exit;

	handle->irq_oob_wakeup = irq_of_parse_and_map(node, 0);
	if (!handle->irq_oob_wakeup) {
		dev_dbg(dev, "fail to parse irq_oob_wakeup from device tree\n");
		goto err_exit;
	}

	ret = devm_request_threaded_irq(dev, handle->irq_oob_wakeup, NULL,
					woal_oob_wakeup_irq_handler,
					IRQF_SHARED | IRQF_ONESHOT,
					"wifi_oob_wakeup", handle);
	if (ret) {
		dev_err(dev, "Failed to request irq_oob_wakeup %d (%d)\n",
			handle->irq_oob_wakeup, ret);
		goto err_exit;
	}

	disable_irq(handle->irq_oob_wakeup);

	LEAVE();
	return;

err_exit:
	handle->irq_oob_wakeup = -1;
}

/**
 *  @brief This function frees the wakeup interrupt line
 *
 *  @param handle  A pointer to moal_handle structure
 *
 *  @return        N/A
 */
void woal_unregist_oob_wakeup_irq(moal_handle *handle)
{
	struct device *dev = handle->hotplug_device;

	ENTER();
	if (handle->irq_oob_wakeup >= 0) {
		devm_free_irq(dev, handle->irq_oob_wakeup, handle);
	}
	LEAVE();
}

/**
 *  @brief This function disables power management wakeup
 *
 *  @param handle  A pointer to moal_handle structure
 *
 *  @return        N/A
 */
void woal_disable_oob_wakeup_irq(moal_handle *handle)
{
	ENTER();

	if (handle->irq_oob_wakeup >= 0) {
		if (handle->wake_by_wifi)
			disable_irq_wake(handle->irq_oob_wakeup);
		else {
			disable_irq_wake(handle->irq_oob_wakeup);
			disable_irq(handle->irq_oob_wakeup);
		}
	}

	LEAVE();
}

/**
 *  @brief This function enables power management wakeup
 *
 *  @param handle  A pointer to moal_handle structure
 *
 *  @return        N/A
 */
void woal_enable_oob_wakeup_irq(moal_handle *handle)
{
	ENTER();

	/* Enable platform specific wakeup interrupt */
	if (handle->irq_oob_wakeup >= 0) {
		handle->wake_by_wifi = false;
		enable_irq(handle->irq_oob_wakeup);
		enable_irq_wake(handle->irq_oob_wakeup);
	}

	LEAVE();
}

/**
 *  @brief This function will be called when the wakeup IRQ occurs
 *
 *  @param irq     Wakeup interrupt line
 *  @param priv    A void pointer to store moal_handle structure pointer
 *
 *  @return        Returns status of interrupt handler.
 */
irqreturn_t woal_oob_wakeup_irq_handler(int irq, void *priv)
{
	moal_handle *handle = priv;
	struct device *dev = handle->hotplug_device;

	ENTER();

	dev_dbg(dev, "%s: OOB wakeup by wifi", __func__);
	handle->wake_by_wifi = true;
	disable_irq_nosync(irq);

	/* Notify PM core we are wakeup source */
	pm_wakeup_event(dev, 0);
	pm_system_wakeup();

	LEAVE();
	return IRQ_HANDLED;
}
#endif /* IMX_SUPPORT */

/**
 * @brief This function adds the card. it will probe the
 *      card, allocate the mlan_private and initialize the device.
 *
 *  @param card    A pointer to card
 *  @param dev     A pointer to device structure
 *  @param if_ops  A pointer to interface ops
 *  @param card_type card type id
 *
 *  @return        A pointer to moal_handle structure
 */
moal_handle *woal_add_card(void *card, struct device *dev, moal_if_ops *if_ops,
			   t_u16 card_type)
{
	moal_handle *handle = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	int netlink_num = NETLINK_NXP;
	int index = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
	struct netlink_kernel_cfg cfg = {
		.groups = NL_MULTICAST_GROUP,
	};
#endif

	ENTER();

	if (MOAL_ACQ_SEMAPHORE_BLOCK(&AddRemoveCardSem))
		goto exit_sem_err;

	for (index = 0; index < MAX_MLAN_ADAPTER; index++) {
		if (m_handle[index] == NULL)
			break;
	}
	if (index >= MAX_MLAN_ADAPTER)
		goto err_handle;

	/* Allocate buffer for moal_handle */
	handle = kzalloc(sizeof(moal_handle), GFP_KERNEL);
	if (!handle) {
		PRINTM(MERROR, "Allocate buffer for moal_handle failed!\n");
		goto err_handle;
	}

	/* Init moal_handle */
	handle->card = card;

	/* Save the handle */
	m_handle[index] = handle;
	handle->handle_idx = index;

	handle->hotplug_device = dev;
	handle->card_type = card_type;
	/* Attach moal handle ops */
	PRINTM(MMSG, "Attach moal handle ops, card interface type: 0x%x\n",
	       handle->card_type);
	moal_memcpy_ext(handle, &handle->ops, if_ops, sizeof(*if_ops),
			sizeof(handle->ops));
	handle->second_mac = handle->ops.is_second_mac(handle);
	if (handle->second_mac) {
		if ((index >= 1) && m_handle[index - 1]) {
			handle->pref_mac = (void *)m_handle[index - 1];
			m_handle[index - 1]->pref_mac = (void *)handle;
		}
	}

	/* Init module parameters */
	if (woal_init_module_param(handle)) {
		PRINTM(MERROR, "Fail to load module parameter file\n");
		goto err_kmalloc;
	}
	if (!handle->params.drv_mode) {
		PRINTM(MMSG, "wlan: stop init_adapter, drv_mode=%d\n",
		       handle->params.drv_mode);
		goto err_kmalloc;
	}
#ifdef IMX_SUPPORT
#ifdef SDIO
	if (IS_SD(handle->card_type)) {
		moal_extflg_set(handle, EXT_TX_WORK);
		moal_extflg_set(handle, EXT_TX_SKB_CLONE);
	}
#endif
#endif

#ifdef DEBUG_LEVEL1
	drvdbg = handle->params.drvdbg;
#endif

	if (handle->params.mac_addr
#ifdef MFG_CMD_SUPPORT
	    && handle->params.mfg_mode != MLAN_INIT_PARA_ENABLED
#endif
	) {
		t_u8 temp[20];
		t_u8 len = strlen(handle->params.mac_addr) + 1;
		if (len < sizeof(temp)) {
			moal_memcpy_ext(handle, temp, handle->params.mac_addr,
					len, sizeof(temp));
			handle->set_mac_addr = 1;
			/* note: the following function overwrites the
			 * temp buffer */
			woal_mac2u8(handle->mac_addr, temp);
		}
	}

	/* Get card info */
	if (woal_get_card_info(handle)) {
		PRINTM(MERROR, "Fail to get card info\n");
		goto err_kmalloc;
	}
	/** Get card revision */
	handle->ops.get_fw_name(handle);
#ifdef STA_SUPPORT
	handle->scan_pending_on_block = MFALSE;
	MOAL_INIT_SEMAPHORE(&handle->async_sem);
#endif

#if defined(USB)
	if (IS_USB(handle->card_type))
		handle->boot_state = ((struct usb_card_rec *)card)->boot_state;
#endif /* USB_NEW_FW_DNLD */
	/* Init SW */
	if (MLAN_STATUS_SUCCESS != woal_init_sw(handle)) {
		PRINTM(MFATAL, "Software Init Failed\n");
		goto err_kmalloc;
	}

	do {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
		handle->nl_sk = netlink_kernel_create(netlink_num, NULL);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
		handle->nl_sk = netlink_kernel_create(
			netlink_num, NL_MULTICAST_GROUP, NULL, THIS_MODULE);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
		handle->nl_sk =
			netlink_kernel_create(netlink_num, NL_MULTICAST_GROUP,
					      NULL, NULL, THIS_MODULE);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
		handle->nl_sk = netlink_kernel_create(&init_net, netlink_num,
						      NL_MULTICAST_GROUP, NULL,
						      NULL, THIS_MODULE);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
		handle->nl_sk = netlink_kernel_create(&init_net, netlink_num,
						      THIS_MODULE, &cfg);
#else
		handle->nl_sk =
			netlink_kernel_create(&init_net, netlink_num, &cfg);
#endif
#endif
#endif
#endif
#endif
		if (handle->nl_sk) {
			PRINTM(MINFO, "Netlink number = %d\n", netlink_num);
			handle->netlink_num = netlink_num;
			break;
		}
		netlink_num--;
	} while (netlink_num > 0);

	if (handle->nl_sk == NULL) {
		PRINTM(MERROR,
		       "Could not initialize netlink event passing mechanism!\n");
		goto err_kmalloc;
	}

	/* Create workqueue for main process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
	/* For kernel less than 2.6.14 name can not be
	 * greater than 10 characters */
	handle->workqueue = create_workqueue("MOAL_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	handle->workqueue = alloc_workqueue(
		"MOAL_WORK_QUEUE", WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
	handle->workqueue = create_workqueue("MOAL_WORK_QUEUE");
#endif
#endif
	if (!handle->workqueue)
		goto err_kmalloc;

	MLAN_INIT_WORK(&handle->main_work, woal_main_work_queue);

	/* Create workqueue for event process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
	/* For kernel less than 2.6.14 name can not be
	 * greater than 10 characters */
	handle->evt_workqueue = create_workqueue("MOAL_EVT_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	handle->evt_workqueue =
		alloc_workqueue("MOAL_EVT_WORK_QUEUE",
				WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
	handle->evt_workqueue = create_workqueue("MOAL_EVT_WORK_QUEUE");
#endif
#endif
	if (!handle->evt_workqueue) {
		woal_terminate_workqueue(handle);
		goto err_kmalloc;
	}

	MLAN_INIT_WORK(&handle->evt_work, woal_evt_work_queue);
	INIT_LIST_HEAD(&handle->evt_queue);
	spin_lock_init(&handle->evt_lock);

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
	MLAN_INIT_WORK(&handle->regulatory_work, woal_regulatory_work_queue);
#endif
#endif

#ifdef STA_CFG80211
	INIT_DELAYED_WORK(&handle->scan_timeout_work,
			  woal_scan_timeout_handler);
#endif

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
	MLAN_INIT_WORK(&handle->host_mlme_work, woal_host_mlme_work_queue);
#endif
#endif

	if (!moal_extflg_isset(handle, EXT_NAPI)) {
		/* Create workqueue for rx process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
		/* For kernel less than 2.6.14 name can not be
		 * greater than 10 characters */
		handle->rx_workqueue = create_workqueue("MOAL_RX_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
		handle->rx_workqueue = alloc_workqueue(
			"MOAL_RX_WORK_QUEUE",
			WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
		handle->rx_workqueue = create_workqueue("MOAL_RX_WORK_QUEUE");
#endif
#endif
		if (!handle->rx_workqueue) {
			woal_terminate_workqueue(handle);
			goto err_kmalloc;
		}
		MLAN_INIT_WORK(&handle->rx_work, woal_rx_work_queue);
	}
#define NAPI_BUDGET 64
	if (moal_extflg_isset(handle, EXT_NAPI)) {
		init_dummy_netdev(&handle->napi_dev);
		netif_napi_add(&handle->napi_dev, &handle->napi_rx,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
			       woal_netdev_poll_rx);
#else
			       woal_netdev_poll_rx, NAPI_BUDGET);
#endif
		napi_enable(&handle->napi_rx);
	}

	if (moal_extflg_isset(handle, EXT_TX_WORK)) {
		/* Create workqueue for tx process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
		handle->tx_workqueue = create_workqueue("MOAL_TX_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
		handle->tx_workqueue = alloc_workqueue(
			"MOAL_TX_WORK_QUEUE",
			WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
		handle->tx_workqueue = create_workqueue("MOAL_TX_WORK_QUEUE");
#endif
#endif
		if (!handle->tx_workqueue) {
			woal_terminate_workqueue(handle);
			goto err_kmalloc;
		}
		MLAN_INIT_WORK(&handle->tx_work, woal_tx_work_handler);
	}

#ifdef REASSOCIATION
	PRINTM(MINFO, "Starting re-association thread...\n");
	handle->reassoc_thread.handle = handle;
	woal_create_thread(woal_reassociation_thread, &handle->reassoc_thread,
			   "woal_reassoc_service");

	while (!handle->reassoc_thread.pid)
		woal_sched_timeout(2);
#endif /* REASSOCIATION */

	/* Register the device. Fill up the private data structure with
	 * relevant information from the card and request for the
	 * required IRQ.
	 */
	if (handle->ops.register_dev(handle) != MLAN_STATUS_SUCCESS) {
		PRINTM(MFATAL, "Failed to register wlan device!\n");
		goto err_registerdev;
	}
	woal_update_firmware_name(handle);
#ifdef ANDROID_KERNEL
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
	wakeup_source_init(&handle->ws, "mwlan");
#else
	wake_lock_init(&handle->wake_lock, WAKE_LOCK_SUSPEND, "mwlan");
#endif
#endif

	/* Init FW and HW */
	if (MLAN_STATUS_SUCCESS != woal_init_fw(handle)) {
		PRINTM(MFATAL, "Firmware Init Failed\n");
		goto err_init_fw;
	}
	device_init_wakeup(dev, true);
#ifdef SD8887
	if (IS_SD8887(handle->card_type)) {
		union {
			t_u32 l;
			t_u8 c[4];
		} ver;
		ver.l = handle->fw_release_number;
		if (ver.c[1] == 75) {
			handle->card_info->embedded_supp = 0;
			PRINTM(MMSG,
			       "Disable EMBEDED Supplicant for SD8887-FP75\n");
		}
	}
#endif
	LEAVE();
	return handle;

err_init_fw:
	if ((handle->hardware_status == HardwareStatusFwReady) ||
	    (handle->hardware_status == HardwareStatusReady)) {
		PRINTM(MINFO, "shutdown mlan\n");
		handle->init_wait_q_woken = MFALSE;
		status = mlan_shutdown_fw(handle->pmlan_adapter);
		if (status == MLAN_STATUS_PENDING)
			wait_event_interruptible(handle->init_wait_q,
						 handle->init_wait_q_woken);
	}
#ifdef ANDROID_KERNEL
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
	wakeup_source_trash(&handle->ws);
#else
	wake_lock_destroy(&handle->wake_lock);
#endif
#endif
	/* Unregister device */
	PRINTM(MINFO, "unregister device\n");
	handle->ops.unregister_dev(handle);
err_registerdev:
	handle->surprise_removed = MTRUE;
#ifdef REASSOCIATION
	if (handle->reassoc_thread.pid)
		wake_up_interruptible(&handle->reassoc_thread.wait_q);
	/* waiting for main thread quit */
	while (handle->reassoc_thread.pid)
		woal_sched_timeout(2);
#endif /* REASSOCIATION */
	if (moal_extflg_isset(handle, EXT_NAPI))
		netif_napi_del(&handle->napi_rx);
	woal_terminate_workqueue(handle);
err_kmalloc:
	woal_free_moal_handle(handle);
	if (index < MAX_MLAN_ADAPTER)
		m_handle[index] = NULL;
err_handle:
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
exit_sem_err:
	LEAVE();
	return NULL;
}

/**
 *  @brief This function removes the card.
 *
 *  @param card    A pointer to card
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
mlan_status woal_remove_card(void *card)
{
	moal_handle *handle = NULL;
	moal_private *priv = NULL;
	mlan_status status;
	int i;
	int index = 0;

	ENTER();

	if (MOAL_ACQ_SEMAPHORE_BLOCK(&AddRemoveCardSem))
		goto exit_sem_err;
	/* Find the correct handle */
	for (index = 0; index < MAX_MLAN_ADAPTER; index++) {
		if (m_handle[index] && (m_handle[index]->card == card)) {
			handle = m_handle[index];
			break;
		}
	}
	if (!handle)
		goto exit_remove;
	device_init_wakeup(handle->hotplug_device, false);
#ifdef MFG_CMD_SUPPORT
	if (handle->params.mfg_mode == MLAN_INIT_PARA_ENABLED
#if defined(USB)
	    && handle->boot_state == USB_FW_READY
#endif
	) {
		if (handle->params.fw_name) {
			kfree(handle->params.fw_name);
			handle->params.fw_name = NULL;
		}
	}
#endif
	if (handle->rf_test_mode)
		woal_process_rf_test_mode(handle, MFG_CMD_UNSET_TEST_MODE);
	handle->surprise_removed = MTRUE;

	woal_flush_workqueue(handle);

	if (moal_extflg_isset(handle, EXT_NAPI)) {
		napi_disable(&handle->napi_rx);
		netif_napi_del(&handle->napi_rx);
	}

	/* Stop data */
	for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM); i++) {
		priv = handle->priv[i];
		if (priv) {
			if (priv->netdev) {
				woal_stop_queue(priv->netdev);
				if (netif_carrier_ok(priv->netdev))
					netif_carrier_off(priv->netdev);
			}
		}
	}
	if ((handle->hardware_status == HardwareStatusFwReady) ||
	    (handle->hardware_status == HardwareStatusReady)) {
		/* Shutdown firmware */
		PRINTM(MIOCTL, "mlan_shutdown_fw.....\n");
		handle->init_wait_q_woken = MFALSE;

		status = mlan_shutdown_fw(handle->pmlan_adapter);
		if (status == MLAN_STATUS_PENDING)
			wait_event_interruptible(handle->init_wait_q,
						 handle->init_wait_q_woken);
		PRINTM(MIOCTL, "mlan_shutdown_fw done!\n");
	}
	/* Unregister mlan */
	if (handle->pmlan_adapter) {
		mlan_unregister(handle->pmlan_adapter);
		handle->pmlan_adapter = NULL;
	}
	if (atomic_read(&handle->rx_pending) ||
	    atomic_read(&handle->tx_pending) ||
	    atomic_read(&handle->ioctl_pending)) {
		PRINTM(MERROR,
		       "ERR: rx_pending=%d,tx_pending=%d,ioctl_pending=%d\n",
		       atomic_read(&handle->rx_pending),
		       atomic_read(&handle->tx_pending),
		       atomic_read(&handle->ioctl_pending));
	}
	unregister_inetaddr_notifier(&handle->woal_notifier);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
	unregister_inet6addr_notifier(&handle->woal_inet6_notifier);
#endif
#endif

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
	if (handle->is_remain_timer_set) {
		woal_cancel_timer(&handle->remain_timer);
		woal_remain_timer_func(handle);
	}
#endif
#endif

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	if (handle->is_go_timer_set) {
		woal_cancel_timer(&handle->go_timer);
		handle->is_go_timer_set = MFALSE;
	}
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
	/* Remove virtual interface */
	woal_remove_virtual_interface(handle);
#endif
#endif
#endif
	if (handle->tp_acnt.on) {
		handle->tp_acnt.on = 0;
		handle->tp_acnt.drop_point = 0;
		if (handle->is_tp_acnt_timer_set) {
			woal_cancel_timer(&handle->tp_acnt.timer);
			handle->is_tp_acnt_timer_set = MFALSE;
		}
	}

	/* Remove interface */
	for (i = 0; i < MIN(MLAN_MAX_BSS_NUM, handle->priv_num); i++)
		woal_remove_interface(handle, i);

#if defined(STA_CFG80211) && defined(UAP_CFG80211)
	/* Unregister and detach connected radiotap net device */
	if (handle->mon_if) {
		netif_device_detach(handle->mon_if->mon_ndev);
		if (handle->mon_if->mon_ndev->reg_state == NETREG_REGISTERED)
			unregister_netdev(handle->mon_if->mon_ndev);
		handle->mon_if = NULL;
	}
#endif

	woal_terminate_workqueue(handle);

#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
	if (handle->is_cac_timer_set) {
		woal_cancel_timer(&handle->cac_timer);
		handle->is_cac_timer_set = MFALSE;
	}
#endif
#endif
#ifdef REASSOCIATION
	PRINTM(MINFO, "Free reassoc_timer\n");
	if (handle->is_reassoc_timer_set) {
		woal_cancel_timer(&handle->reassoc_timer);
		handle->is_reassoc_timer_set = MFALSE;
	}
	if (handle->reassoc_thread.pid)
		wake_up_interruptible(&handle->reassoc_thread.wait_q);

	/* waiting for main thread quit */
	while (handle->reassoc_thread.pid)
		woal_sched_timeout(2);
#endif /* REASSOCIATION */

	PRINTM(MINFO, "Free FW dump timer\n");
	if (handle->is_fw_dump_timer_set) {
		woal_cancel_timer(&handle->fw_dump_timer);
		handle->is_fw_dump_timer_set = MFALSE;
	}

#ifdef CONFIG_PROC_FS
	woal_proc_exit(handle);
#endif
	/* Unregister device */
	PRINTM(MINFO, "unregister device\n");
	handle->ops.unregister_dev(handle);
#ifdef ANDROID_KERNEL
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
	wakeup_source_trash(&handle->ws);
#else
	wake_lock_destroy(&handle->wake_lock);
#endif
#endif
	/* Free adapter structure */
	PRINTM(MINFO, "Free Adapter\n");
	woal_free_moal_handle(handle);

	for (index = 0; index < MAX_MLAN_ADAPTER; index++) {
		if (m_handle[index] == handle) {
			m_handle[index] = NULL;
			break;
		}
	}
exit_remove:
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
exit_sem_err:
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

#ifdef CONFIG_PROC_FS
/**
 *  @brief This function switch the drv_mode
 *
 *  @param handle   A pointer to moal_handle structure
 *  @param mode     new drv_mode to switch.
 *
 *  @return        MLAN_STATUS_SUCCESS /MLAN_STATUS_FAILURE
 * /MLAN_STATUS_PENDING
 */
mlan_status woal_switch_drv_mode(moal_handle *handle, t_u32 mode)
{
	unsigned int i;
	mlan_status status = MLAN_STATUS_SUCCESS;
	moal_private *priv = NULL;

	ENTER();

	if (MOAL_ACQ_SEMAPHORE_BLOCK(&AddRemoveCardSem))
		goto exit_sem_err;

	if (woal_update_drv_tbl(handle, mode) != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Could not update driver mode table!\n");
		status = MLAN_STATUS_FAILURE;
		goto exit;
	}

	/* Reset all interfaces */
	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	if (MLAN_STATUS_SUCCESS !=
	    woal_reset_intf(priv, MOAL_IOCTL_WAIT, MTRUE)) {
		PRINTM(MERROR, "woal_reset_inf failed!\n");
		goto exit;
	}

	status = woal_shutdown_fw(priv, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "func shutdown failed!\n");
		goto exit;
	}
#ifdef USB
	handle->skip_fw_dnld = MTRUE;
#endif

	/* Shutdown firmware */
	PRINTM(MIOCTL, "mlan_shutdown_fw.....\n");
	handle->init_wait_q_woken = MFALSE;
	status = mlan_shutdown_fw(handle->pmlan_adapter);
	if (status == MLAN_STATUS_PENDING)
		wait_event_interruptible(handle->init_wait_q,
					 handle->init_wait_q_woken);
	PRINTM(MIOCTL, "mlan_shutdown_fw done!\n");
	/* Unregister mlan */
	if (handle->pmlan_adapter) {
		mlan_unregister(handle->pmlan_adapter);
		handle->pmlan_adapter = NULL;
	}
	if (atomic_read(&handle->rx_pending) ||
	    atomic_read(&handle->tx_pending) ||
	    atomic_read(&handle->ioctl_pending)) {
		PRINTM(MERROR,
		       "ERR: rx_pending=%d,tx_pending=%d,ioctl_pending=%d\n",
		       atomic_read(&handle->rx_pending),
		       atomic_read(&handle->tx_pending),
		       atomic_read(&handle->ioctl_pending));
	}

	unregister_inetaddr_notifier(&handle->woal_notifier);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
	unregister_inet6addr_notifier(&handle->woal_inet6_notifier);
#endif
#endif

	/* Remove interface */
	for (i = 0; i < MIN(MLAN_MAX_BSS_NUM, handle->priv_num); i++)
		woal_remove_interface(handle, i);

	if (atomic_read(&handle->lock_count) ||
	    atomic_read(&handle->malloc_count) ||
	    atomic_read(&handle->mbufalloc_count)) {
		PRINTM(MERROR,
		       "mlan has memory leak: lock_count=%d, malloc_count=%d, mbufalloc_count=%d\n",
		       atomic_read(&handle->lock_count),
		       atomic_read(&handle->malloc_count),
		       atomic_read(&handle->mbufalloc_count));
	}
#ifdef PCIE
	if (IS_PCIE(handle->card_type) &&
	    atomic_read(&handle->malloc_cons_count)) {
		PRINTM(MERROR, "mlan has memory leak: malloc_cons_count=%d\n",
		       atomic_read(&handle->malloc_cons_count));
	}
#endif

	handle->priv_num = 0;
	handle->params.drv_mode = mode;
	/* Init SW */
	if (woal_init_sw(handle)) {
		PRINTM(MFATAL, "Software Init Failed\n");
		goto exit;
	}
	/* Init FW and HW */
	if (woal_init_fw(handle)) {
		PRINTM(MFATAL, "Firmware Init Failed\n");
		goto exit;
	}
	LEAVE();
	return status;
exit:
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
exit_sem_err:
	LEAVE();
	return status;
}
#endif

#ifdef SDIO_MMC
#define FW_POLL_TRIES 100

/**
 *  @brief This function reload fw
 *
 *  @param handle   A pointer to moal_handle structure
 *  @param mode     FW_RELOAD_SDIO_INBAND_RESET or FW_RELOAD_SDIO_HW_RESET
 *
 *  @return        0--success, otherwise failure
 */
static int woal_reset_and_reload_fw(moal_handle *handle, t_u8 mode)
{
	int ret = 0, tries = 0;
	t_u32 value = 1;
	t_u32 reset_reg = handle->card_info->fw_reset_reg;
	t_u8 reset_val = handle->card_info->fw_reset_val;

	ENTER();
	if (mode == FW_RELOAD_SDIO_HW_RESET) {
		PRINTM(MCMND, "woal_sdio_reset_fw...\n");
		woal_sdio_reset_hw(handle);
		goto reload_fw;
	}
	mlan_pm_wakeup_card(handle->pmlan_adapter, MTRUE);
	/** wait SOC fully wake up */
	for (tries = 0; tries < FW_POLL_TRIES; ++tries) {
		ret = handle->ops.write_reg(handle, reset_reg, 0xba);
		if (ret == MLAN_STATUS_SUCCESS) {
			handle->ops.read_reg(handle, reset_reg, &value);
			if (value == 0xba) {
				PRINTM(MMSG, "FW wake up\n");
				break;
			}
		}
		udelay(1000);
	}
	/* Write register to notify FW */
	if (handle->ops.write_reg(handle, reset_reg, reset_val) !=
	    MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to write register.\n");
		ret = -EFAULT;
		goto done;
	}
#if defined(SD9098) || defined(SD9097) || defined(SDNW62X) || defined(SD9177)
	if (IS_SD9098(handle->card_type) || IS_SD9097(handle->card_type) ||
	    IS_SDNW62X(handle->card_type) || IS_SD9177(handle->card_type))
		handle->ops.write_reg(handle, 0x00, 0x10);
#endif
	/* Poll register around 100 ms */
	for (tries = 0; tries < FW_POLL_TRIES; ++tries) {
		handle->ops.read_reg(handle, reset_reg, &value);
		if (value == 0)
			/* FW is ready */
			break;
		udelay(1000);
	}

	if (value) {
		PRINTM(MERROR, "Failed to poll FW reset register %X=0x%x\n",
		       reset_reg, value);
		ret = -EFAULT;
		goto done;
	}
	mlan_pm_wakeup_card(handle->pmlan_adapter, MFALSE);
reload_fw:
	/* Download FW */
	ret = woal_request_fw(handle);
	if (ret) {
		ret = -EFAULT;
		goto done;
	}
	PRINTM(MMSG, "FW Reload successfully.");
done:
	LEAVE();
	return ret;
}
#endif

/**
 *  @brief This function reload fw
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return        0--success, otherwise failure
 */
static int woal_reload_fw(moal_handle *handle)
{
	int ret = 0;
	ENTER();
	/* Download FW */
	ret = woal_request_fw(handle);
	if (ret) {
		ret = -EFAULT;
		goto done;
	}
	PRINTM(MMSG, "FW Reload successfully.");
done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function handle the pre_reset
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return        NULL;
 */
static void woal_pre_reset(moal_handle *handle)
{
	int intf_num;
	t_u8 driver_status = handle->driver_status;
	t_u8 i;
	moal_private *priv = woal_get_priv(handle, MLAN_BSS_ROLE_STA);
	mlan_debug_info *info = &(handle->debug_info);
	int ioctl_pending = 0;

	ENTER();
	if (!driver_status && priv)
		woal_cancel_scan(priv, MOAL_IOCTL_WAIT);
	handle->driver_status = MTRUE;

	if (!driver_status)
		woal_sched_timeout_uninterruptible(MOAL_TIMER_1S);

	// wait for IOCTL return
	if (!driver_status && priv) {
		for (i = 0; i < 5; i++) {
			if (woal_get_debug_info(priv, MOAL_IOCTL_WAIT, info))
				PRINTM(MERROR,
				       "Could not retrieve debug information from MLAN\n");
			ioctl_pending = atomic_read(&handle->ioctl_pending);
			if (!info->pending_cmd && !ioctl_pending) {
				PRINTM(MCMND,
				       "fw_reload: No pending command and IOCTL\n");
				break;
			}
			woal_sched_timeout_uninterruptible(MOAL_TIMER_1S);
		}
	}
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
	/* Remove virtual interface */
	woal_remove_virtual_interface(handle);
#endif
#endif
#endif

	/** detach network interface */
	for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
		if (handle->priv[intf_num]) {
			woal_stop_queue(handle->priv[intf_num]->netdev);
			netif_device_detach(handle->priv[intf_num]->netdev);
		}
	}
	/** mask host interrupt from firmware */
	mlan_disable_host_int(handle->pmlan_adapter);
	/** cancel all pending commands */
	mlan_ioctl(handle->pmlan_adapter, NULL);
	woal_flush_workqueue(handle);

	handle->fw_reload = MTRUE;
	woal_update_firmware_name(handle);
#ifdef USB
	if (IS_USB(handle->card_type))
		woal_kill_urbs(handle);
#endif
	LEAVE();
}

/**
 *  @brief This function handle pose_reset
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return        NULL;
 */
static void woal_post_reset(moal_handle *handle)
{
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *misc = NULL;
	int intf_num;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	moal_private *priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	t_u8 country_code[COUNTRY_CODE_LEN];
#endif
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
#if defined(STA_WEXT) || defined(UAP_WEXT)
	t_u8 bss_role = MLAN_BSS_ROLE_STA;
#endif
#endif
#endif /* WIFI_DIRECT_SUPPORT */

	ENTER();
	/** un-block IOCTL */
	handle->fw_reload = MFALSE;
	/* Restart the firmware */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req) {
		misc = (mlan_ds_misc_cfg *)req->pbuf;
		misc->sub_command = MLAN_OID_MISC_WARM_RESET;
		misc->param.fw_reload = MTRUE;
		req->req_id = MLAN_IOCTL_MISC_CFG;
		req->action = MLAN_ACT_SET;
		if (MLAN_STATUS_SUCCESS !=
		    woal_request_ioctl(woal_get_priv(handle, MLAN_BSS_ROLE_ANY),
				       req, MOAL_IOCTL_WAIT_TIMEOUT)) {
			PRINTM(MERROR, "%s: warm reset failed \n", __func__);
			kfree(req);
			goto done;
		}
		kfree(req);
	}
#ifdef DEBUG_LEVEL1
	drvdbg = handle->params.drvdbg;
#endif
	handle->fw_dump_status = MFALSE;
	handle->driver_status = MFALSE;
	handle->hardware_status = HardwareStatusReady;
#ifdef STA_CFG80211
	handle->scan_timeout = SCAN_TIMEOUT_25S;
#endif
	if (!handle->wifi_hal_flag) {
		PRINTM(MMSG, "wlan: post_reset remove/add interface\n");
		handle->surprise_removed = MTRUE;
		for (intf_num = 0;
		     intf_num < MIN(MLAN_MAX_BSS_NUM, handle->priv_num);
		     intf_num++)
			woal_remove_interface(handle, intf_num);
		handle->priv_num = 0;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		/* Unregister wiphy device and free */
		if (handle->wiphy) {
			wiphy_unregister(handle->wiphy);
			woal_cfg80211_free_bands(handle->wiphy);
			wiphy_free(handle->wiphy);
			handle->wiphy = NULL;
		}
#endif
		handle->surprise_removed = MFALSE;

		for (intf_num = 0; intf_num < handle->drv_mode.intf_num;
		     intf_num++) {
			if (handle->drv_mode.bss_attr[intf_num].bss_virtual)
				continue;
			if (!woal_add_interface(handle, handle->priv_num,
						handle->drv_mode
							.bss_attr[intf_num]
							.bss_type)) {
				PRINTM(MERROR, "%s: add interface %d failed \n",
				       __func__, handle->priv_num);
				goto done;
			}
		}
		PRINTM(MMSG, "wlan: post_reset remove/add interface done\n");
		goto done;
	}
	/* Reset all interfaces */
	woal_reset_intf(woal_get_priv(handle, MLAN_BSS_ROLE_ANY),
			MOAL_IOCTL_WAIT, MTRUE);
	/* Initialize private structures */
	for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
		if (handle->priv[intf_num]) {
			woal_init_priv(handle->priv[intf_num], MOAL_IOCTL_WAIT);
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
#if defined(STA_WEXT) || defined(UAP_WEXT)
			if ((handle->priv[intf_num]->bss_type ==
			     MLAN_BSS_TYPE_WIFIDIRECT) &&
			    (GET_BSS_ROLE(handle->priv[intf_num]) ==
			     MLAN_BSS_ROLE_UAP)) {
				if (MLAN_STATUS_SUCCESS !=
				    woal_bss_role_cfg(handle->priv[intf_num],
						      MLAN_ACT_SET,
						      MOAL_IOCTL_WAIT,
						      &bss_role)) {
					goto done;
				}
			}
#endif /* STA_WEXT || UAP_WEXT */
#endif /* STA_SUPPORT && UAP_SUPPORT */
#endif /* WIFI_DIRECT_SUPPORT */
		}
	}

	/* Enable interfaces */
	for (intf_num = 0; intf_num < handle->priv_num; intf_num++) {
		if (handle->priv[intf_num]) {
			netif_device_attach(handle->priv[intf_num]->netdev);
			woal_start_queue(handle->priv[intf_num]->netdev);
		}
	}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	if (handle->country_code[0] && handle->country_code[1]) {
		memset(country_code, 0, sizeof(country_code));
		if (MTRUE ==
		    is_cfg80211_special_region_code(handle->country_code)) {
			country_code[0] = 'W';
			country_code[1] = 'W';
		} else {
			country_code[0] = handle->country_code[0];
			country_code[1] = handle->country_code[1];
		}

		if (handle->params.cntry_txpwr && priv)
			woal_request_country_power_table(priv, country_code);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
		if (handle->params.cntry_txpwr == CNTRY_RGPOWER_MODE)
			queue_work(handle->evt_workqueue,
				   &handle->regulatory_work);
#endif
	}
#endif

done:
	if (handle->dpd_data) {
		release_firmware(handle->dpd_data);
		handle->dpd_data = NULL;
	}
	if (handle->txpwr_data) {
		release_firmware(handle->txpwr_data);
		handle->txpwr_data = NULL;
	}
	if (handle->user_data) {
		release_firmware(handle->user_data);
		handle->user_data = NULL;
	}
	LEAVE();
	return;
}

/**
 *  @brief This function reload fw
 *
 *  @param phandle   A pointer to moal_handle structure
 *  @param mode     FW reload mode
 *
 *  @return        0--success, otherwise failure
 */
int woal_request_fw_reload(moal_handle *phandle, t_u8 mode)
{
	int ret = 0;

#ifdef PCIE
	ppcie_service_card card = NULL;
	struct pci_dev *pdev = NULL;
#endif
	moal_handle *handle = phandle;
	moal_handle *ref_handle = NULL;

	ENTER();
	wifi_status = WIFI_STATUS_FW_RELOAD;
#ifdef PCIE
	if (mode == FW_RELOAD_PCIE_RESET) {
		card = (pcie_service_card *)handle->card;
		pdev = card->dev;
		if (pci_reset_function(pdev)) {
			PRINTM(MERROR, "%s: pci_reset_function failed \n",
			       __func__);
			ret = -1;
		}
		LEAVE();
		return ret;
	}
#endif

	// handle-> mac0 , ref_handle->second mac
	if (handle->pref_mac) {
		if (phandle->second_mac) {
			handle = (moal_handle *)handle->pref_mac;
			ref_handle = phandle;
		} else {
			ref_handle = (moal_handle *)handle->pref_mac;
		}
	}

	if (mode == FW_RELOAD_WITH_EMULATION) {
		fw_reload = FW_RELOAD_WITH_EMULATION;
		PRINTM(MMSG, "FW reload with re-emulation...\n");
		LEAVE();
		return ret;
	}
	woal_pre_reset(handle);
	if (ref_handle)
		woal_pre_reset(ref_handle);
	if (mode == FW_RELOAD_NO_EMULATION) {
		ret = woal_reload_fw(handle);
		if (ret) {
			PRINTM(MERROR, "woal_reload_fw fail\n");
			goto done;
		}
		if (ref_handle) {
			ret = woal_reload_fw(ref_handle);
			if (ret) {
				PRINTM(MERROR, "woal_reload_fw fail\n");
				goto done;
			}
		}
	}
#ifdef SDIO_MMC
	else if ((mode == FW_RELOAD_SDIO_INBAND_RESET ||
		  mode == FW_RELOAD_SDIO_HW_RESET) &&
		 IS_SD(handle->card_type)) {
		ret = woal_reset_and_reload_fw(handle, mode);
		if (ret) {
			PRINTM(MERROR, "woal_reset_and_reload_fw fail\n");
			goto done;
		}
		if (ref_handle) {
			ret = woal_reload_fw(ref_handle);
			if (ret) {
				PRINTM(MERROR, "woal_reload_fw fail\n");
				goto done;
			}
		}
	}
#endif
	else
		ret = -EFAULT;
	if (ret) {
		PRINTM(MERROR, "FW reload fail\n");
		goto done;
	}
	woal_post_reset(handle);
	if (ref_handle)
		woal_post_reset(ref_handle);
	wifi_status = WIFI_STATUS_OK;
done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function register to bus driver
 *
 *  @param work   a pointer to struct work_struct
 *
 *  @return        N/A
 */
static void woal_bus_register(struct work_struct *work)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	PRINTM(MMSG, "wlan: Register to Bus Driver...\n");
#ifdef SDIO
#ifdef SDIO_MMC
	/* Register SDIO driver */
	ret = woal_sdiommc_bus_register();
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to register sdio_mmc bus driver\n");
		goto out;
	}
#else
	ret = woal_sdio_bus_register();
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to register sdio_mmc bus driver\n");
		goto out;
	}
#endif
#endif
#ifdef USB
	/* Register USB driver */
	ret = woal_usb_bus_register();
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to register usb bus driver\n");
		goto out;
	}
#endif
#ifdef PCIE
	/* Register PCIE driver */
	ret = woal_pcie_bus_register();
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to register pcie bus driver\n");
		goto out;
	}
#endif
	PRINTM(MMSG, "wlan: Register to Bus Driver Done\n");
out:
	return;
}

/**
 *  @brief This function unregister from bus driver
 *
 *
 *  @return        N/A
 */
static void woal_bus_unregister(void)
{
#ifdef SDIO
#ifdef SDIO_MMC
	/* Unregister SDIO driver */
	woal_sdiommc_bus_unregister();
#else
	woal_sdio_bus_unregister();
#endif
#endif
#ifdef USB
	/* Unregister USB driver */
	woal_usb_bus_unregister();
#endif
#ifdef PCIE
	/* Unregister PCIE driver */
	woal_pcie_bus_unregister();
#endif
}

/**
 *  @brief This function initializes module.
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int woal_init_module(void)
{
	int ret = (int)MLAN_STATUS_SUCCESS;
	int index = 0;

	ENTER();

	PRINTM(MMSG, "wlan: Loading MWLAN driver\n");
	/* Init the wlan_private pointer array first */
	for (index = 0; index < MAX_MLAN_ADAPTER; index++)
		m_handle[index] = NULL;
	/* Init mutex */
	MOAL_INIT_SEMAPHORE(&AddRemoveCardSem);

	if (woal_root_proc_init() != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "woal_init_module: Unable to create /proc/mwlan/ directory\n");
		LEAVE();
		return -EFAULT;
	}

#ifdef CONFIG_OF
	woal_init_from_dev_tree();
#endif

	/* Create workqueue for hang process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
	/* For kernel less than 2.6.14 name can not be greater than 10
	   characters */
	hang_workqueue = create_workqueue("MOAL_HANG_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	hang_workqueue =
		alloc_workqueue("MOAL_HANG_WORK_QUEUE",
				WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
	hang_workqueue = create_workqueue("MOAL_HANG_WORK_QUEUE");
#endif
#endif
	MLAN_INIT_WORK(&hang_work, woal_hang_work_queue);

	if (reg_work) {
		/* Create workqueue for hang process */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
		/* For kernel less than 2.6.14 name can not be greater than 10
			characters */
		register_workqueue = create_workqueue("MOAL_REGISTER_WORKQ");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
		register_workqueue = alloc_workqueue(
			"MOAL_REGISTER_WORK_QUEUE",
			WQ_HIGHPRI | WQ_MEM_RECLAIM | WQ_UNBOUND, 1);
#else
		register_workqueue =
			create_workqueue("MOAL_REGISTER_WORK_QUEUE");
#endif
#endif

		MLAN_INIT_WORK(&register_work, woal_bus_register);
		queue_work(register_workqueue, &register_work);
	} else {
		woal_bus_register(NULL);
	}
	PRINTM(MMSG, "wlan: Driver loaded successfully\n");
	LEAVE();
	return ret;
}

/**
 *  @brief This function cleans module
 *
 *  @return        N/A
 */
static void woal_cleanup_module(void)
{
	moal_handle *handle = NULL;
	int index = 0;
	int i;
#if defined(STA_SUPPORT) && defined(STA_CFG80211)
	unsigned long flags;
#endif

	ENTER();

	PRINTM(MMSG, "wlan: Unloading MWLAN driver\n");
	if (MOAL_ACQ_SEMAPHORE_BLOCK(&AddRemoveCardSem))
		goto exit_sem_err;
	for (index = 0; index < MAX_MLAN_ADAPTER; index++) {
		handle = m_handle[index];
		if (!handle)
			continue;
		if (!handle->priv_num)
			goto exit;
		if (MTRUE == woal_check_driver_status(handle))
			goto exit;

#ifdef USB
#ifdef CONFIG_USB_SUSPEND
		if (IS_USB(handle->card_type) &&
		    handle->is_suspended == MTRUE) {
			woal_exit_usb_suspend(handle);
		}
#endif /* CONFIG_USB_SUSPEND */
#endif

#ifdef SDIO
#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
		if (handle->is_suspended == MTRUE) {
			woal_sdio_resume(
				&(((struct sdio_mmc_card *)handle->card)->func)
					 ->dev);
		}
#endif /* MMC_PM_KEEP_POWER */
#endif /* SDIO_SUSPEND_RESUME */
#endif

		if (handle->rf_test_mode)
			woal_process_rf_test_mode(handle,
						  MFG_CMD_UNSET_TEST_MODE);
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
		/* Unregister all connected radiotap net devices */
		if (handle->mon_if) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			woal_set_net_monitor(handle->mon_if->priv,
					     MOAL_IOCTL_WAIT, MFALSE, 0, NULL);
			if (handle->ioctl_timeout) {
				woal_ioctl_timeout(handle);
				goto exit;
			}
#endif
			netif_device_detach(handle->mon_if->mon_ndev);
			if (handle->mon_if->mon_ndev->reg_state ==
			    NETREG_REGISTERED)
				unregister_netdev(handle->mon_if->mon_ndev);
			handle->mon_if = NULL;
		}
#endif

		for (i = 0; i < handle->priv_num; i++) {
			/** cancel dfs monitor on deinit */
			if (handle->priv[i] &&
			    handle->priv[i]->bss_type == MLAN_BSS_TYPE_DFS) {
				woal_11h_cancel_chan_report_ioctl(
					handle->priv[i], MOAL_IOCTL_WAIT);
				continue;
			}
#ifdef STA_SUPPORT
			if (GET_BSS_ROLE(handle->priv[i]) ==
			    MLAN_BSS_ROLE_STA) {
				if (handle->priv[i]->media_connected == MTRUE) {
					woal_disconnect(handle->priv[i],
							MOAL_IOCTL_WAIT_TIMEOUT,
							NULL,
							DEF_DEAUTH_REASON_CODE);
					if (handle->ioctl_timeout) {
						woal_ioctl_timeout(handle);
						goto exit;
					}
				}
#ifdef STA_CFG80211
				if (IS_STA_CFG80211(
					    handle->params.cfg80211_wext))
					woal_clear_conn_params(handle->priv[i]);
				spin_lock_irqsave(&handle->scan_req_lock,
						  flags);
				if (IS_STA_CFG80211(
					    handle->params.cfg80211_wext) &&
				    handle->scan_request) {
					cancel_delayed_work(
						&handle->scan_timeout_work);
					woal_cfg80211_scan_done(
						handle->scan_request, MTRUE);
					handle->scan_request = NULL;
					handle->scan_priv = NULL;
				}
				spin_unlock_irqrestore(&handle->scan_req_lock,
						       flags);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
				if (IS_STA_CFG80211(
					    handle->params.cfg80211_wext) &&
				    handle->priv[i]->sched_scanning) {
					woal_stop_bg_scan(
						handle->priv[i],
						MOAL_IOCTL_WAIT_TIMEOUT);
					if (handle->ioctl_timeout) {
						woal_ioctl_timeout(handle);
						goto exit;
					}
					handle->priv[i]->bg_scan_start = MFALSE;
					handle->priv[i]->bg_scan_reported =
						MFALSE;
					cfg80211_sched_scan_stopped(
						handle->priv[i]->wdev->wiphy
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
						,
						handle->priv[i]->bg_scan_reqid
#endif
					);
					handle->priv[i]->sched_scanning =
						MFALSE;
				}
#endif
#endif
			}
#endif
#ifdef UAP_SUPPORT
			if (GET_BSS_ROLE(handle->priv[i]) ==
			    MLAN_BSS_ROLE_UAP) {
#ifdef MFG_CMD_SUPPORT
				if (handle->params.mfg_mode !=
				    MLAN_INIT_PARA_ENABLED)
#endif
				{
					woal_disconnect(handle->priv[i],
							MOAL_IOCTL_WAIT_TIMEOUT,
							NULL,
							DEF_DEAUTH_REASON_CODE);
					if (handle->ioctl_timeout) {
						woal_ioctl_timeout(handle);
						goto exit;
					}
				}
			}
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
			woal_clear_all_mgmt_ies(handle->priv[i],
						MOAL_IOCTL_WAIT_TIMEOUT);
			if (handle->ioctl_timeout) {
				woal_ioctl_timeout(handle);
				goto exit;
			}
			woal_flush_tx_stat_queue(handle->priv[i]);
#endif
		}

#ifdef MFG_CMD_SUPPORT
		if (handle->params.mfg_mode != MLAN_INIT_PARA_ENABLED)
#endif
		{
			woal_set_deep_sleep(woal_get_priv(handle,
							  MLAN_BSS_ROLE_ANY),
					    MOAL_IOCTL_WAIT_TIMEOUT, MFALSE, 0);
		}

#ifdef MFG_CMD_SUPPORT
		if (handle->params.mfg_mode != MLAN_INIT_PARA_ENABLED)
#endif
		{
			woal_shutdown_fw(woal_get_priv(handle,
						       MLAN_BSS_ROLE_ANY),
					 MOAL_IOCTL_WAIT_TIMEOUT);
			if (handle->ioctl_timeout) {
				woal_ioctl_timeout(handle);
				goto exit;
			}
		}
	}

exit:
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
exit_sem_err:
	/* Unregister from bus */
	woal_bus_unregister();
	PRINTM(MMSG, "wlan: Driver unloaded\n");
	if (hang_workqueue) {
		flush_workqueue(hang_workqueue);
		destroy_workqueue(hang_workqueue);
		hang_workqueue = NULL;
	}
	if (reg_work && register_workqueue) {
		flush_workqueue(register_workqueue);
		destroy_workqueue(register_workqueue);
		register_workqueue = NULL;
	}

	woal_root_proc_remove();

	LEAVE();
}

#ifndef MODULE
#ifdef MFG_CMD_SUPPORT
/**
 *  @brief This function handle the mfg_mode from kernel boot command
 *
 *  @param str     buffer for mfg_mode
 *  @return        N/A
 */
static int __init mfg_mode_setup(char *str)
{
	int val = -1;
	get_option(&str, &val);
	if (val > 0)
		mfg_mode = 1;
	PRINTM(MMSG, "mfg_mode=%d\n", mfg_mode);
	return 1;
}
__setup("mfg_mode=", mfg_mode_setup);
#endif
#endif

module_init(woal_init_module);
module_exit(woal_cleanup_module);

module_param(reg_work, int, 0);
MODULE_PARM_DESC(
	reg_work,
	"0: disable register work_queue; 1: enable register work_queue");

MODULE_DESCRIPTION("M-WLAN Driver");
MODULE_AUTHOR("NXP");
MODULE_VERSION(MLAN_RELEASE_VERSION);
MODULE_LICENSE("GPL");
MODULE_LICENSE("GPL");
