/** @file mlan_shc.c
 *
 *  @brief This file contains the secure host interface functions.
 *
 *
 *  Copyright 2025 NXP
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

#ifndef _MLAN_SHC_H_
#define _MLAN_SHC_H_
#ifdef STA_SUPPORT
#include "mlan_join.h"
#endif
#include "mlan_main.h"

mlan_status mlan_shc_handshake(pmlan_adapter pmadapter, t_u8 type, t_void *msg);
mlan_status wlan_shc_process_secure_host_event(pmlan_private pmpriv, t_u8 *data,
					       t_u32 len);
mlan_status wlan_shc_secure_hostcmd_process(pmlan_adapter pmadapter,
					    HostCmd_DS_COMMAND *pcmd);
t_bool wlan_is_secure_host_cmd(t_u16 cmd_id);
mlan_status wlan_shc_secure_hostresp_process(pmlan_adapter pmadapter,
					     HostCmd_DS_COMMAND *resp);
mlan_status wlan_shc_prep_for_requeue(pmlan_adapter pmadapter,
				      HostCmd_DS_COMMAND *pcmd);
#endif
