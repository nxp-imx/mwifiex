// SPDX-License-Identifier: GPL-2.0
/** @file moal_thermal.h
 *
 * @brief Linux Thermal Framework Integration for NXP Wi-Fi driver
 *
 * Copyright 2026 NXP
 *
 * NXP CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code (Materials) are owned by NXP, its suppliers and/or its
 * licensors. Title to the Materials remains with NXP, its suppliers and/or
 * its licensors. The Materials contain trade secrets and proprietary and
 * confidential information of NXP, its suppliers and/or its licensors.
 * The Materials are protected by worldwide copyright and trade secret laws
 * and treaty provisions. No part of the Materials may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without NXP prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by NXP in writing.
 *
 *  Alternatively, this software may be distributed under the terms of GPL v2.
 */

#ifndef _MOAL_THERMAL_H_
#define _MOAL_THERMAL_H_

#ifdef LINUX_THERMAL_SUPPORT
#ifndef ANDROID_SDK_VERSION

#include <linux/version.h>
#include <linux/thermal.h>
#include "moal_main.h"

#define WOAL_THERMAL_NUM_TRIPS 2
#define WOAL_THERMAL_TRIP0_TEMP_MC 85000
#define WOAL_THERMAL_TRIP1_TEMP_MC 105000

struct woal_thermal_priv {
	struct thermal_zone_device *tz_dev;
	moal_handle *handle;
};

int woal_thermal_register(moal_handle *handle);
void woal_thermal_unregister(moal_handle *handle);
void woal_thermal_notify(moal_handle *handle);

#endif /* !ANDROID_SDK_VERSION */
#endif /* LINUX_THERMAL_SUPPORT */
#endif /* _MOAL_THERMAL_H_ */
