/** @file moal_wext.h
 *
 * @brief This file contains definition for extended private IOCTL call.
 *
 *
 * Copyright 2008-2021 NXP
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

/********************************************************
Change log:
    10/21/2008: initial version
********************************************************/

#ifndef _WOAL_WEXT_H_
#define _WOAL_WEXT_H_

/** NF value for default scan */
#define MRVDRV_NF_DEFAULT_SCAN_VALUE (-96)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
/** Add event */
#define IWE_STREAM_ADD_EVENT(i, c, e, w, l)                                    \
	iwe_stream_add_event((i), (c), (e), (w), (l))
/** Add point */
#define IWE_STREAM_ADD_POINT(i, c, e, w, p)                                    \
	iwe_stream_add_point((i), (c), (e), (w), (p))
/** Add value */
#define IWE_STREAM_ADD_VALUE(i, c, v, e, w, l)                                 \
	iwe_stream_add_value((i), (c), (v), (e), (w), (l))
#else
/** Add event */
#define IWE_STREAM_ADD_EVENT(i, c, e, w, l)                                    \
	iwe_stream_add_event((c), (e), (w), (l))
/** Add point */
#define IWE_STREAM_ADD_POINT(i, c, e, w, p)                                    \
	iwe_stream_add_point((c), (e), (w), (p))
/** Add value */
#define IWE_STREAM_ADD_VALUE(i, c, v, e, w, l)                                 \
	iwe_stream_add_value((c), (v), (e), (w), (l))
#endif

extern struct iw_handler_def woal_handler_def;
struct iw_statistics *woal_get_wireless_stats(struct net_device *dev);
#endif /* _WOAL_WEXT_H_ */
