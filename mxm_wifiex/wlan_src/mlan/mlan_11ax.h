/** @file mlan_11ax.h
 *
 *  @brief This file defines the private and adapter data
 *  structures and declares global function prototypes used
 *  in MLAN module.
 *
 *
 *  Copyright 2018-2022 NXP
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
 *  SPDX-License-Identifier:    GPL-2.0
 *
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
