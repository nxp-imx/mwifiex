/** @file  moal_uap_priv.h
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

#ifndef _MOAL_UAP_PRIV_H_
#define _MOAL_UAP_PRIV_H_

/** Private command ID */
#define WOAL_UAP_IOCTL 0x8BE0

/** Private command to get/set 256 chars */
#define WOAL_UAP_SET_GET_256_CHAR (WOAL_UAP_IOCTL + 1)
/** Private command ID to FW reload */
#define WOAL_WL_FW_RELOAD 1
/** Private command ID to set AP configuration */
#define WOAL_AP_SET_CFG 2

/** Private command ID to set/get none */
#define WOAL_UAP_SETNONE_GETNONE (WOAL_UAP_IOCTL + 2)
/** Private command ID to start UAP */
#define WOAL_UAP_START 1
/** Private command ID to stop UAP */
#define WOAL_UAP_STOP 2
/** Private command ID to start AP BSS */
#define WOAL_AP_BSS_START 3
/** Private command ID to stop AP BSS */
#define WOAL_AP_BSS_STOP 4

/** Private command ID to set one int/get word char */
#define WOAL_UAP_SETONEINT_GETWORDCHAR (WOAL_UAP_IOCTL + 3)
/** Private command ID to get version */
#define WOAL_UAP_VERSION 1
/** Private command ID to get extended version */
#define WOAL_UAP_VEREXT 2

/** Private command ID to set one int/get one int */
#define WOAL_UAP_SETONEINT_GETONEINT (WOAL_UAP_IOCTL + 5)
#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
/** Private command ID for set/get BSS role */
#define WOAL_UAP_SET_GET_BSS_ROLE 1
#endif
#endif

/** Private command ID for hostcmd */
#define WOAL_UAP_HOST_CMD (WOAL_UAP_IOCTL + 17)

/** The following command IDs are for Froyo app */
/** Private command ID to start AP BSS */
#define WOAL_UAP_FROYO_AP_BSS_START (WOAL_UAP_IOCTL + 24)
/** Private command ID to stop AP BSS */
#define WOAL_UAP_FROYO_AP_BSS_STOP (WOAL_UAP_IOCTL + 26)
/** Private command ID to set AP config */
#define WOAL_UAP_FROYO_AP_SET_CFG (WOAL_UAP_IOCTL + 27)
/** Private command ID to start driver */
#define WOAL_UAP_FROYO_START (WOAL_UAP_IOCTL + 28)
/** Private command ID to reload FW */
#define WOAL_UAP_FROYO_WL_FW_RELOAD (WOAL_UAP_IOCTL + 29)
/** Private command ID to stop driver */
#define WOAL_UAP_FROYO_STOP (WOAL_UAP_IOCTL + 30)

#endif /* _MOAL_UAP_PRIV_H_ */
