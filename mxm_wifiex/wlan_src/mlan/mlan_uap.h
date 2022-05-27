/** @file mlan_uap.h
 *
 *  @brief This file contains related macros, enum, and struct
 *  of uap functionalities
 *
 *
 *  Copyright 2009-2021 NXP
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

/********************************************************
Change log:
    02/05/2009: initial version
********************************************************/

#ifndef _MLAN_UAP_H_
#define _MLAN_UAP_H_

mlan_status wlan_uap_get_channel(pmlan_private pmpriv);

mlan_status wlan_uap_set_channel(pmlan_private pmpriv,
				 Band_Config_t uap_band_cfg, t_u8 channel);

mlan_status wlan_uap_get_beacon_dtim(pmlan_private pmpriv);

mlan_status wlan_ops_uap_ioctl(t_void *adapter, pmlan_ioctl_req pioctl_req);

mlan_status wlan_ops_uap_prepare_cmd(t_void *priv, t_u16 cmd_no,
				     t_u16 cmd_action, t_u32 cmd_oid,
				     t_void *pioctl_buf, t_void *pdata_buf,
				     t_void *pcmd_buf);

mlan_status wlan_ops_uap_process_cmdresp(t_void *priv, t_u16 cmdresp_no,
					 t_void *pcmd_buf, t_void *pioctl);

mlan_status wlan_ops_uap_process_rx_packet(t_void *adapter, pmlan_buffer pmbuf);

mlan_status wlan_ops_uap_process_event(t_void *priv);

t_void *wlan_ops_uap_process_txpd(t_void *priv, pmlan_buffer pmbuf);

mlan_status wlan_ops_uap_init_cmd(t_void *priv, t_u8 first_bss);

#endif /* _MLAN_UAP_H_ */
