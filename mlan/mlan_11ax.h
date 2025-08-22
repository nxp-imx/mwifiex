/** @file mlan_11ax.h
 *
 *  @brief This file contains the functions for station ioctl.
 *
 *
 *  Copyright 2018-2022, 2025 NXP
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

#ifndef _MLAN_11AX_H_
#define _MLAN_11AX_H_

/** device support 2.4G 40MHZ*/
#define AX_2G_40MHZ_SUPPORT MBIT(1)
/** device support 2.4G 242 tone RUs */
#define AX_2G_20MHZ_SUPPORT MBIT(5)

/*
 * 0 indicates support for HE-MCS 0-7 for n spatial streams
 * 1 indicates support for HE-MCS 0-9 for n spatial streams
 * 2 indicates support for HE-MCS 0-11 for n spatial streams
 * 3 indicates that n spatial streams is not supported for HE PPDUs
 */
#define GET_HE_NSSMCS(MCSMapSet, nss) ((MCSMapSet >> (2 * (nss - 1))) & 0x3)
#define SET_HE_NSSMCS(MCSMapSet, nss, value)                                   \
	(MCSMapSet |= (value & 0x3) << (2 * (nss - 1)))

t_u8 wlan_check_11ax_twt_supported(mlan_private *pmpriv,
				   BSSDescriptor_t *pbss_desc);
mlan_status wlan_11ax_ioctl_twtcfg(pmlan_adapter pmadapter,
				   pmlan_ioctl_req pioctl_req);
mlan_status wlan_cmd_twt_cfg(pmlan_private pmpriv, HostCmd_DS_COMMAND *cmd,
			     t_u16 cmd_action, t_void *pdata_buf);
t_u8 wlan_fill_he_cap_ie(mlan_private *pmpriv, IEEEtypes_HECap_t *hecap_ie,
			 t_u16 band);
t_u8 wlan_fill_he_op_ie(mlan_private *pmpriv, IEEEtypes_HeOp_t *heop_ie);
t_u16 wlan_fill_he_cap_tlv(mlan_private *pmpriv, t_u16 band,
			   MrvlIEtypes_Extension_t *phe_cap, t_u8 flag);
void wlan_update_11ax_cap(mlan_adapter *pmadapter,
			  MrvlIEtypes_Extension_t *hw_he_cap);

/** GET 6G Band CapInfo : MAX MPDU Length */
#define GET_6G_BAND_CAP_MAXMPDULEN(BandCapInfo) ((BandCapInfo >> 6) & 0x3)
#define IS_116E_ENABLED(priv)                                                  \
	((priv->config_bands & BAND_6G) &&                                     \
	 priv->curr_bss_params.bss_descriptor.phe_cap &&                       \
	 priv->curr_bss_params.bss_descriptor.phe_6g_cap)

void wlan_update_11ax_6g_cap(mlan_adapter *pmadapter,
			     MrvlIEtypes_Extension_t *hw_he_6g_cap);
void wlan_fill_he_6g_cap_tlv(mlan_private *pmpriv,
			     MrvlIEtypes_He_6g_cap_t *phe_6g_cap);
t_u16 wlan_116e_bandconfig_allowed(mlan_private *pmpriv,
				   BSSDescriptor_t *pbss_desc);
int wlan_cmd_append_116e_tlv(mlan_private *pmpriv, BSSDescriptor_t *pbss_desc,
			     t_u8 **ppbuffer);
t_u8 wlan_get_6g_ap_bandconfig(BSSDescriptor_t *pbss_desc,
			       Band_Config_t *bandcfg);
t_bool is_6ghz_op_class(t_u8 op_class);

int wlan_cmd_append_11ax_tlv(mlan_private *pmpriv, BSSDescriptor_t *pbss_desc,
			     t_u8 **ppbuffer);
t_u16 wlan_11ax_bandconfig_allowed(mlan_private *pmpriv,
				   BSSDescriptor_t *pbss_desc);
mlan_status wlan_11ax_cfg_ioctl(pmlan_adapter pmadapter,
				pmlan_ioctl_req pioctl_req);
mlan_status wlan_11ax_ioctl_cmd(pmlan_adapter pmadapter,
				pmlan_ioctl_req pioctl_req);

mlan_status wlan_cmd_11ax_cfg(pmlan_private pmpriv, HostCmd_DS_COMMAND *cmd,
			      t_u16 cmd_action, t_void *pdata_buf);
mlan_status wlan_ret_11ax_cfg(pmlan_private pmpriv, HostCmd_DS_COMMAND *resp,
			      mlan_ioctl_req *pioctl_buf);
mlan_status wlan_cmd_11ax_cmd(pmlan_private pmpriv, HostCmd_DS_COMMAND *cmd,
			      t_u16 cmd_action, t_void *pdata_buf);
mlan_status wlan_ret_11ax_cmd(pmlan_private pmpriv, HostCmd_DS_COMMAND *resp,
			      mlan_ioctl_req *pioctl_buf);

#endif /* _MLAN_11AX_H_ */
