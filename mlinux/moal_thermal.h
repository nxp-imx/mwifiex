// SPDX-License-Identifier: GPL-2.0
/** @file moal_thermal.h
 *
 * @brief Linux Thermal Framework Integration for NXP Wi-Fi driver
 *
 * Copyright 2026 NXP
 *
 * This software file (the File) is distributed by NXP
 * under the terms of the GNU General Public License Version 2, June 1991
 * (the License).  You may use, redistribute and/or modify the File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
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
