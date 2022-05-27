/** @file moal_sta_cfg80211.h
 *
 * @brief This file contains the STA CFG80211 specific defines.
 *
 *
 * Copyright 2011-2021 NXP
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

#ifndef _MOAL_STA_CFG80211_H_
#define _MOAL_STA_CFG80211_H_

/** Convert RSSI signal strength from dBm to mBm (100*dBm) */
#define RSSI_DBM_TO_MDM(x) ((x)*100)

mlan_status woal_register_sta_cfg80211(struct net_device *dev, t_u8 bss_type);

#endif /* _MOAL_STA_CFG80211_H_ */
