// SPDX-License-Identifier: GPL-2.0
/** @file moal_thermal.c
 *
 * @brief Linux Thermal Framework Integration for NXP Wi-Fi driver
 *
 * Registers the NXP Wi-Fi chip internal CAU temperature sensor as a
 * Linux thermal zone device, allowing the host thermal framework to
 * monitor Wi-Fi chip temperature and react to trip points.
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

#ifdef LINUX_THERMAL_SUPPORT
#ifndef ANDROID_SDK_VERSION

#include <linux/version.h>
#include <linux/thermal.h>
#include <linux/err.h>
#include "moal_main.h"
#include "moal_thermal.h"

static int woal_tz_get_temp(struct thermal_zone_device *tz, int *temp);
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
static int woal_tz_get_trip_type(struct thermal_zone_device *tz, int trip,
				 enum thermal_trip_type *type);
static int woal_tz_get_trip_temp(struct thermal_zone_device *tz, int trip,
				 int *temp);
#endif

static bool woal_thermal_is_device_ready(moal_handle *handle)
{
	if (handle->surprise_removed)
		return false;
	if (handle->is_suspended)
		return false;
	if (handle->fw_dump)
		return false;
	if (handle->driver_status)
		return false;
	return true;
}

static int woal_read_chip_temp(moal_handle *handle, int *temp_mc)
{
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *cfg = NULL;
	mlan_status status;
	int ret = 0;

	if (!woal_thermal_is_device_ready(handle))
		return -EAGAIN;
	priv = woal_get_priv(handle, MLAN_BSS_ROLE_STA);
	if (!priv)
		return -EAGAIN;
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (!req) {
		PRINTM(MERROR, "thermal: failed to alloc ioctl req\n");
		return -ENOMEM;
	}
	cfg = (mlan_ds_misc_cfg *)req->pbuf;
	cfg->sub_command = MLAN_OID_MISC_GET_SENSOR_TEMP;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = MLAN_ACT_GET;
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "thermal: MLAN_OID_MISC_GET_SENSOR_TEMP failed, status=%d\n",
		       status);
		ret = -EFAULT;
		goto done;
	}
	/* firmware cau_temperature is in integer °C; convert to milli-Celsius
	 */
	{
		int cau_temp = (int)cfg->param.sensor_temp.cau_temperature;

		/* Guard against implausible readings before scaling to mC */
		if (cau_temp > 200)
			cau_temp = 200;
		*temp_mc = cau_temp * 1000;
	}
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	return ret;
}

static int woal_tz_get_temp(struct thermal_zone_device *tz, int *temp)
{
	struct woal_thermal_priv *tp;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
	tp = (struct woal_thermal_priv *)thermal_zone_device_priv(tz);
#else
	tp = (struct woal_thermal_priv *)tz->devdata;
#endif
	if (!tp || !tp->handle)
		return -ENODEV;
	return woal_read_chip_temp(tp->handle, temp);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
static int woal_tz_get_trip_type(struct thermal_zone_device *tz, int trip,
				 enum thermal_trip_type *type)
{
	switch (trip) {
	case 0:
		*type = THERMAL_TRIP_PASSIVE;
		break;
	case 1:
		*type = THERMAL_TRIP_CRITICAL;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int woal_tz_get_trip_temp(struct thermal_zone_device *tz, int trip,
				 int *temp)
{
	switch (trip) {
	case 0:
		*temp = WOAL_THERMAL_TRIP0_TEMP_MC;
		break;
	case 1:
		*temp = WOAL_THERMAL_TRIP1_TEMP_MC;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static struct thermal_zone_device_ops woal_thermal_zone_ops = {
	.get_temp = woal_tz_get_temp,
	.get_trip_type = woal_tz_get_trip_type,
	.get_trip_temp = woal_tz_get_trip_temp,
};

#elif LINUX_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
static struct thermal_zone_device_ops woal_thermal_zone_ops = {
	.get_temp = woal_tz_get_temp,
};

static struct thermal_trip woal_thermal_trips[WOAL_THERMAL_NUM_TRIPS] = {
	{.temperature = WOAL_THERMAL_TRIP0_TEMP_MC,
	 .type = THERMAL_TRIP_PASSIVE},
	{.temperature = WOAL_THERMAL_TRIP1_TEMP_MC,
	 .type = THERMAL_TRIP_CRITICAL},
};

#else
static const struct thermal_zone_device_ops woal_thermal_zone_ops = {
	.get_temp = woal_tz_get_temp,
};

static const struct thermal_trip woal_thermal_trips[WOAL_THERMAL_NUM_TRIPS] = {
	{.temperature = WOAL_THERMAL_TRIP0_TEMP_MC,
	 .type = THERMAL_TRIP_PASSIVE},
	{.temperature = WOAL_THERMAL_TRIP1_TEMP_MC,
	 .type = THERMAL_TRIP_CRITICAL},
};
#endif /* LINUX_VERSION_CODE */

int woal_thermal_register(moal_handle *handle)
{
	struct woal_thermal_priv *tp;
	int ret = 0;

	ENTER();
	tp = kzalloc(sizeof(*tp), GFP_KERNEL);
	if (!tp) {
		PRINTM(MERROR, "thermal: failed to alloc woal_thermal_priv\n");
		ret = -ENOMEM;
		goto done;
	}
	tp->handle = handle;
	handle->thermal_priv = tp;

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
	tp->tz_dev = thermal_zone_device_register("nxp-wifi-tz",
						  WOAL_THERMAL_NUM_TRIPS, 0, tp,
						  &woal_thermal_zone_ops, NULL,
						  1000, 5000);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
	tp->tz_dev = thermal_zone_device_register_with_trips(
		"nxp-wifi-tz", woal_thermal_trips, WOAL_THERMAL_NUM_TRIPS, 0,
		tp, &woal_thermal_zone_ops, NULL, 1000, 5000);
#else
	tp->tz_dev = thermal_zone_device_register_with_trips(
		"nxp-wifi-tz", woal_thermal_trips, WOAL_THERMAL_NUM_TRIPS, tp,
		&woal_thermal_zone_ops, NULL, 1000, 5000);
#endif

	if (IS_ERR(tp->tz_dev)) {
		PRINTM(MERROR, "thermal: zone device register failed (%ld)\n",
		       PTR_ERR(tp->tz_dev));
		ret = PTR_ERR(tp->tz_dev);
		tp->tz_dev = NULL;
		kfree(tp);
		handle->thermal_priv = NULL;
		goto done;
	}
	PRINTM(MMSG, "thermal: registered nxp-wifi-tz\n");
done:
	LEAVE();
	return ret;
}

void woal_thermal_unregister(moal_handle *handle)
{
	struct woal_thermal_priv *tp;

	ENTER();
	if (!handle || !handle->thermal_priv) {
		LEAVE();
		return;
	}
	tp = (struct woal_thermal_priv *)handle->thermal_priv;
	if (tp->tz_dev) {
		thermal_zone_device_unregister(tp->tz_dev);
		tp->tz_dev = NULL;
	}
	kfree(tp);
	handle->thermal_priv = NULL;
	PRINTM(MMSG, "thermal: unregistered nxp-wifi-tz\n");
	LEAVE();
}

void woal_thermal_notify(moal_handle *handle)
{
	struct woal_thermal_priv *tp;

	if (!handle || !handle->thermal_priv)
		return;
	tp = (struct woal_thermal_priv *)handle->thermal_priv;
	if (tp->tz_dev)
		thermal_zone_device_update(tp->tz_dev,
					   THERMAL_EVENT_UNSPECIFIED);
}

#endif /* !ANDROID_SDK_VERSION */
#endif /* LINUX_THERMAL_SUPPORT */
