/** @file mlan_shc.c
 *
 *  @brief This file contains the secure host interface functions.
 *
 *
 *  Copyright 2025 NXP
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
