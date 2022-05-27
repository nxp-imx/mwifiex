/** @file  moal_uap_priv.c
 *
 * @brief This file contains wireless extension standard ioctl functions
 *
 *
 * Copyright 2010-2021 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code (Materials) are owned by NXP, its
 * suppliers and/or its licensors. Title to the Materials remains with NXP,
 * its suppliers and/or its licensors. The Materials contain
 * trade secrets and proprietary and confidential information of NXP, its
 * suppliers and/or its licensors. The Materials are protected by worldwide
 * copyright and trade secret laws and treaty provisions. No part of the
 * Materials may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without NXP's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by NXP in writing.
 *
 *  Alternatively, this software may be distributed under the terms of GPL v2.
 *  SPDX-License-Identifier:    GPL-2.0
 *
 */

/************************************************************************
Change log:
    08/06/2010: initial version
************************************************************************/

#include "moal_main.h"
#include "moal_uap.h"
#include "moal_uap_priv.h"

/********************************************************
			Local Variables
********************************************************/

/********************************************************
			Global Variables
********************************************************/

/********************************************************
			Local Functions
********************************************************/

/********************************************************
			Global Functions
********************************************************/

/**
 *  @brief ioctl function for wireless IOCTLs
 *
 *  @param dev      A pointer to net_device structure
 *  @param req      A pointer to ifreq structure
 *  @param cmd      Command
 *
 *  @return          0 --success, otherwise fail
 */
int woal_uap_do_priv_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	moal_private *priv = (moal_private *)netdev_priv(dev);
	struct iwreq *wrq = (struct iwreq *)req;
	int ret = 0;

	ENTER();

	switch (cmd) {
	case WOAL_UAP_SETNONE_GETNONE:
		switch (wrq->u.data.flags) {
		case WOAL_UAP_START:
			break;
		case WOAL_UAP_STOP:
			ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT,
						UAP_BSS_STOP);
			break;
		case WOAL_AP_BSS_START:
			ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT,
						UAP_BSS_START);
			break;
		case WOAL_AP_BSS_STOP:
			ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT,
						UAP_BSS_STOP);
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case WOAL_UAP_SETONEINT_GETWORDCHAR:
		switch (wrq->u.data.flags) {
		case WOAL_UAP_VERSION:
			ret = woal_get_driver_version(priv, req);
			break;
		case WOAL_UAP_VEREXT:
			ret = woal_get_driver_verext(priv, req);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	case WOAL_UAP_SET_GET_256_CHAR:
		switch (wrq->u.data.flags) {
		case WOAL_WL_FW_RELOAD:
			break;
		case WOAL_AP_SET_CFG:
			ret = woal_uap_set_ap_cfg(
				priv, (t_u8 __force *)wrq->u.data.pointer,
				wrq->u.data.length);
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case WOAL_UAP_SETONEINT_GETONEINT:
		switch (wrq->u.data.flags) {
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
		case WOAL_UAP_SET_GET_BSS_ROLE:
			ret = woal_set_get_bss_role(priv, wrq);
			break;
#endif
#endif
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case WOAL_UAP_HOST_CMD:
		ret = woal_host_command(priv, wrq);
		break;
	case WOAL_UAP_FROYO_START:
		break;
	case WOAL_UAP_FROYO_STOP:
		ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_STOP);
		break;
	case WOAL_UAP_FROYO_AP_BSS_START:
		ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_START);
		break;
	case WOAL_UAP_FROYO_AP_BSS_STOP:
		ret = woal_uap_bss_ctrl(priv, MOAL_IOCTL_WAIT, UAP_BSS_STOP);
		break;
	case WOAL_UAP_FROYO_WL_FW_RELOAD:
		break;
	case WOAL_UAP_FROYO_AP_SET_CFG:
		ret = woal_uap_set_ap_cfg(priv,
					  (t_u8 __force *)wrq->u.data.pointer,
					  wrq->u.data.length);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	LEAVE();
	return ret;
}

/**
 *  @brief Handle get info resp
 *
 *  @param priv     Pointer to moal_private structure
 *  @param info     Pointer to mlan_ds_get_info structure
 *
 *  @return         N/A
 */
void woal_ioctl_get_uap_info_resp(moal_private *priv, mlan_ds_get_info *info)
{
	ENTER();
	switch (info->sub_command) {
	case MLAN_OID_GET_STATS:
		priv->w_stats.discard.fragment =
			info->param.ustats.fcs_error_count;
		priv->w_stats.discard.retries = info->param.ustats.retry_count;
		priv->w_stats.discard.misc =
			info->param.ustats.ack_failure_count;
		break;
	default:
		break;
	}
	LEAVE();
}
