/** @file moal_cfg80211_util.c
 *
 * @brief This file contains the functions for CFG80211 vendor.
 *
 *
 * Copyright 2015-2022, 2025 NXP
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
 *
 */

#include "moal_cfg80211_util.h"
#include "moal_cfg80211.h"
#include "moal_eth_ioctl.h"

/********************************************************
 *				Local Variables
 ********************************************************/

/********************************************************
 *				Global Variables
 ********************************************************/

/********************************************************
 *				Local Functions
 ********************************************************/

/********************************************************
 *				Global Functions
 ********************************************************/

#if KERNEL_VERSION(3, 14, 0) <= CFG80211_VERSION_CODE
/**nxp vendor command and event*/
#define MRVL_VENDOR_ID 0x005043
/** vendor events */
static const struct nl80211_vendor_cmd_info vendor_events[] = {
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_hang,
	}, /*event_id 0*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_fw_dump_done,
	}, /*event_id 1*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_fw_reset_success,
	}, /*event_id 2*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_fw_reset_failure,
	}, /*event_id 3*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_fw_reset_start,
	}, /*event_id 4*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_rssi_monitor,
	}, /*event_id 0x1501*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_set_key_mgmt_offload,
	}, /*event_id 0x10001*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_fw_roam_success,
	}, /*event_id 0x10002*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_cloud_keep_alive,
	}, /*event_id 0x10003*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_dfs_radar_detected,
	}, /*event_id 0x10004*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_dfs_cac_started,
	}, /*event_id 0x10005*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_dfs_cac_finished,
	}, /*event_id 0x10006*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_dfs_cac_aborted,
	}, /*event_id 0x10007*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_dfs_nop_finished,
	}, /*event_id 0x10008*/
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_wifi_logger_ring_buffer_data,
	},
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_wifi_logger_alert,
	},
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_packet_fate_monitor,
	},
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_wake_reason_report,
	},
	{
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = event_rtt_result,
	}, /*event_id ???*/
	/**add vendor event here*/
};

/**nxp vendor policies*/
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE

static const struct nla_policy woal_ll_stat_policy[ATTR_LL_STATS_MAX + 1] = {
	[ATTR_LL_STATS_MPDU_SIZE_THRESHOLD] = {.type = NLA_U32},
	[ATTR_LL_STATS_AGGRESSIVE_STATS_GATHERING] = {.type = NLA_U32},
	[ATTR_LL_STATS_CLEAR_REQ_MASK] = {.type = NLA_U32},
	[ATTR_LL_STATS_STOP_REQ] = {.type = NLA_U8}};

static const struct nla_policy woal_logger_policy[ATTR_WIFI_LOGGER_MAX + 1] = {
	[ATTR_WIFI_LOGGER_RING_ID] = {.type = NLA_STRING},
	[ATTR_WIFI_LOGGER_VERBOSE_LEVEL] = {.type = NLA_U32},
	[ATTR_WIFI_LOGGER_FLAGS] = {.type = NLA_U32},
	[ATTR_WIFI_LOGGER_MIN_DATA_SIZE] = {.type = NLA_U32},
	[ATTR_WIFI_LOGGER_MAX_INTERVAL_SEC] = {.type = NLA_U32}};

static const struct nla_policy woal_attr_policy[ATTR_WIFI_MAX + 1] = {
	[ATTR_CHANNELS_BAND] = {.type = NLA_U32},
	[ATTR_SCAN_MAC_OUI_SET] = {.type = NLA_STRING, .len = 3},
	[ATTR_NODFS_VALUE] = {.type = NLA_U32},
	[ATTR_GET_CONCURRENCY_MATRIX_SET_SIZE_MAX] = {.type = NLA_U32},
	[ATTR_SCAN_BAND_SET] = {.type = NLA_U8},
};
static const struct nla_policy woal_secure_ranging_ctx_policy
	[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_MAX + 1] = {
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_ACTION] =
			{.type = NLA_U32},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_SRC_ADDR] =
			{.type = NLA_STRING, .len = ETH_ALEN},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_PEER_MAC_ADDR] =
			{.type = NLA_STRING, .len = ETH_ALEN},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_SHA_TYPE] =
			{.type = NLA_U32},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_TK] =
			{.type = NLA_STRING, .len = 32},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_CIPHER] =
			{.type = NLA_U32},
		[MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_LTF_KEYSEED] =
			{.type = NLA_STRING, .len = 48},
};
// clang-format off
static const struct nla_policy
        woal_nd_offload_policy[ATTR_ND_OFFLOAD_MAX + 1] = {
		[ATTR_ND_OFFLOAD_CONTROL] = {.type = NLA_U8},
};
// clang-format on

static const struct nla_policy woal_rtt_policy[ATTR_RTT_MAX + 1] = {
	[ATTR_RTT_TARGET_NUM] = {.type = NLA_U8},
	[ATTR_RTT_TARGET_CONFIG] = {.type = NLA_BINARY},
	[ATTR_RTT_TARGET_ADDR] = {.type = NLA_STRING, .len = ETH_ALEN},
	[ATTR_RTT_CHANNEL_INFO] = {.type = NLA_BINARY},
	[ATTR_RTT_MAX_DUR_SEC] = {.type = NLA_U32},
	[ATTR_RTT_LCI_INFO] = {.type = NLA_BINARY},
	[ATTR_RTT_LCR_INFO] = {.type = NLA_BINARY},

};

static const struct nla_policy
	woal_rssi_monitor_policy[ATTR_RSSI_MONITOR_MAX + 1] = {
		[ATTR_RSSI_MONITOR_CONTROL] = {.type = NLA_U32},
		[ATTR_RSSI_MONITOR_MIN_RSSI] = {.type = NLA_S8},
		[ATTR_RSSI_MONITOR_MAX_RSSI] = {.type = NLA_S8},
};

static const struct nla_policy
	woal_packet_filter_policy[ATTR_PACKET_FILTER_MAX + 1] = {
		[ATTR_PACKET_FILTER_TOTAL_LENGTH] = {.type = NLA_U32},
		[ATTR_PACKET_FILTER_PROGRAM] = {.type = NLA_STRING},
};

// clang-format off
// clang-format on

static const struct nla_policy
	woal_keep_alive_policy[MKEEP_ALIVE_ATTRIBUTE_MAX + 1] = {
		[MKEEP_ALIVE_ATTRIBUTE_ID] = {.type = NLA_U8},
		[MKEEP_ALIVE_ATTRIBUTE_ETHER_TYPE] = {.type = NLA_U16},
		[MKEEP_ALIVE_ATTRIBUTE_IP_PKT] = {.type = NLA_BINARY},
		[MKEEP_ALIVE_ATTRIBUTE_IP_PKT_LEN] = {.type = NLA_U16},
		[MKEEP_ALIVE_ATTRIBUTE_SRC_MAC_ADDR] = {.type = NLA_STRING,
							.len = ETH_ALEN},
		[MKEEP_ALIVE_ATTRIBUTE_DST_MAC_ADDR] = {.type = NLA_STRING,
							.len = ETH_ALEN},
		[MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC] = {.type = NLA_U32},
		[MKEEP_ALIVE_ATTRIBUTE_RETRY_INTERVAL] = {.type = NLA_U32},
		[MKEEP_ALIVE_ATTRIBUTE_RETRY_CNT] = {.type = NLA_U8},
};
#endif

static const struct nla_policy
	woal_usable_channel_policy[ATTR_USABLE_CHANNEL_MAX + 1] = {
		[ATTR_USABLE_CHANNEL_BAND] = {.type = NLA_U32},
		[ATTR_USABLE_CHANNEL_IFACE_MODE] = {.type = NLA_U32},
		[ATTR_USABLE_CHANNEL_FILTER] = {.type = NLA_U32},
		[ATTR_USABLE_CHANNEL_MAX_SIZE] = {.type = NLA_U32},
};

/**
 * @brief get the event id of the events array
 *
 * @param event     vendor event
 *
 * @return    index of events array
 */
static int woal_get_event_id(int event)
{
	int i = 0;

	for (i = 0; i < (int)ARRAY_SIZE(vendor_events); i++) {
		if ((int)vendor_events[i].subcmd == event)
			return i;
	}

	return event_max;
}

/**
 * @brief send vendor event to kernel
 *
 * @param priv       A pointer to moal_private
 * @param event    vendor event
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
int woal_cfg80211_vendor_event(moal_private *priv, int event, t_u8 *data,
			       int len)
{
	struct wiphy *wiphy = NULL;
	struct sk_buff *skb = NULL;
	int event_id = 0;
	t_u8 *pos = NULL;
	int ret = 0;

	ENTER();

	if (!priv || !priv->wdev || !priv->wdev->wiphy) {
		LEAVE();
		return ret;
	}
	wiphy = priv->wdev->wiphy;
	PRINTM(MEVENT, "vendor event :0x%x\n", event);
	event_id = woal_get_event_id(event);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = 1;
		LEAVE();
		return ret;
	}

	/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev, len, event_id,
					  GFP_ATOMIC);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, len, event_id, GFP_ATOMIC);
#endif

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = 1;
		LEAVE();
		return ret;
	}
	pos = skb_put(skb, len);
	moal_memcpy_ext(priv->phandle, pos, data, len, len);
	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

	LEAVE();
	return ret;
}

#if KERNEL_VERSION(3, 14, 0) <= CFG80211_VERSION_CODE
/**
 * @brief send fw dump complete event to vendorhal
 *
 * @param priv       A pointer to moal_private
 *
 * @return      N/A
 */
void woal_cfg80211_vendor_event_fw_dump(moal_private *priv)
{
	PRINTM(MEVENT, "wlan: Notify FW dump complete event\n");
	woal_cfg80211_vendor_event(priv, event_fw_dump_done, CUS_EVT_FW_DUMP,
				   strlen(CUS_EVT_FW_DUMP));
}
#endif

/**
 * @brief send vendor event to kernel
 *
 * @param priv       A pointer to moal_private
 * @param event    vendor event
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static struct sk_buff *woal_cfg80211_alloc_vendor_event(moal_private *priv,
							int event, int len)
{
	struct wiphy *wiphy = NULL;
	struct sk_buff *skb = NULL;
	int event_id = 0;

	ENTER();

	if (!priv || !priv->wdev || !priv->wdev->wiphy) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		goto done;
	}
	wiphy = priv->wdev->wiphy;
	PRINTM(MEVENT, "vendor event :0x%x\n", event);
	event_id = woal_get_event_id(event);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		goto done;
	}

	/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev, len, event_id,
					  GFP_ATOMIC);
#else
	skb = cfg80211_vendor_event_alloc(wiphy, len, event_id, GFP_ATOMIC);
#endif

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		goto done;
	}

done:
	LEAVE();
	return skb;
}

/**
 * @brief send dfs vendor event to kernel
 *
 * @param priv       A pointer to moal_private
 * @param event      dfs vendor event
 * @param chandef    a pointer to struct cfg80211_chan_def
 *
 * @return      N/A
 */
void woal_cfg80211_dfs_vendor_event(moal_private *priv, int event,
				    struct cfg80211_chan_def *chandef)
{
	dfs_event evt;

	ENTER();
	if (!chandef) {
		LEAVE();
		return;
	}
	memset(&evt, 0, sizeof(dfs_event));
	evt.freq = chandef->chan->center_freq;
	evt.chan_width = chandef->width;
	evt.cf1 = chandef->center_freq1;
	evt.cf2 = chandef->center_freq2;
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		evt.ht_enabled = 0;
		break;
	case NL80211_CHAN_WIDTH_20:
		evt.ht_enabled = 1;
		break;
	case NL80211_CHAN_WIDTH_40:
		evt.ht_enabled = 1;
		if (chandef->center_freq1 < chandef->chan->center_freq)
			evt.chan_offset = -1;
		else
			evt.chan_offset = 1;
		break;
	case NL80211_CHAN_WIDTH_80:
	case NL80211_CHAN_WIDTH_80P80:
	case NL80211_CHAN_WIDTH_160:
		evt.ht_enabled = 1;
		break;
	default:
		break;
	}
	woal_cfg80211_vendor_event(priv, event, (t_u8 *)&evt,
				   sizeof(dfs_event));
	LEAVE();
}

/**
 * @brief vendor command to set drvdbg
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_set_drvdbg(struct wiphy *wiphy,
					   struct wireless_dev *wdev,
					   const void *data, int data_len)
{
#ifdef DEBUG_LEVEL1
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	t_u8 *pos = NULL;
#endif
	int ret = 1;

	ENTER();
#ifdef DEBUG_LEVEL1
	/**handle this sub command*/
	DBG_HEXDUMP(MCMD_D, "Vendor drvdbg", (const t_u8 *)data, data_len);

	if (data_len) {
		/* Get the driver debug bit masks from user */
		drvdbg = *((const t_u32 *)data);
		PRINTM(MIOCTL, "new drvdbg %x\n", drvdbg);
		/* Set the driver debug bit masks into mlan */
		if (woal_set_drvdbg(priv, drvdbg)) {
			PRINTM(MERROR, "Set drvdbg failed!\n");
			ret = 1;
		}
	}
	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(drvdbg));
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = 1;
		LEAVE();
		return ret;
	}
	pos = skb_put(skb, sizeof(drvdbg));
	moal_memcpy_ext(priv->phandle, pos, &drvdbg, sizeof(drvdbg),
			sizeof(drvdbg));
	ret = cfg80211_vendor_cmd_reply(skb);
#endif
	LEAVE();
	return ret;
}

/**
 * @brief process one channel in bucket
 *
 * @param priv       A pointer to moal_private struct
 *
 * @param channel     a pointer to channel
 *
 * @return      0: success  other: fail
 */
static mlan_status woal_band_to_valid_channels(moal_private *priv,
					       wifi_band w_band, int channel[],
					       t_u32 *nchannel)
{
	int band = 0;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	int i = 0;
	t_u8 cnt = 0;
	int *ch_ptr = channel;

	for (band = 0; band < IEEE80211_NUM_BANDS; band++) {
		if (!priv->wdev->wiphy->bands[band])
			continue;
		if ((band == IEEE80211_BAND_2GHZ) && !(w_band & WIFI_BAND_BG))
			continue;
		if ((band == IEEE80211_BAND_5GHZ) &&
		    !((w_band & WIFI_BAND_A) || (w_band & WIFI_BAND_A_DFS)))
			continue;
		sband = priv->wdev->wiphy->bands[band];
		for (i = 0; (i < sband->n_channels); i++) {
			ch = &sband->channels[i];
			if (ch->flags & IEEE80211_CHAN_DISABLED) {
				PRINTM(MERROR, "Skip DISABLED channel %d\n",
				       ch->center_freq);
				continue;
			}
			if (band == IEEE80211_BAND_5GHZ) {
				if (((ch->flags & IEEE80211_CHAN_RADAR) &&
				     !(w_band & WIFI_BAND_A_DFS)) ||
				    (!(ch->flags & IEEE80211_CHAN_RADAR) &&
				     !(w_band & WIFI_BAND_A)))
					continue;
			}
			if (cnt >= *nchannel) {
				PRINTM(MERROR,
				       "cnt=%d is exceed %d, cur ch=%d %dMHz\n",
				       cnt, *nchannel, ch->hw_value,
				       ch->center_freq);
				break;
			}
			*ch_ptr = ch->center_freq;
			ch_ptr++;
			cnt++;
		}
	}

	PRINTM(MCMND, "w_band=%d cnt=%d\n", w_band, cnt);
	*nchannel = cnt;
	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief GSCAN subcmd - enable full scan results
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 *
 * @param data     a pointer to data
 * @param  data_len     data length
 *
 * @return      0: success  other: fail
 */
static int woal_cfg80211_subcmd_get_valid_channels(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct nlattr *tb[ATTR_WIFI_MAX + 1];
	t_u32 band = 0;
	int ch_out[MAX_CHANNEL_NUM];
	t_u32 nchannel = 0;
	t_u32 mem_needed = 0;
	struct sk_buff *skb = NULL;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_WIFI_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		err = -EFAULT;
		goto done;
	}

	if (!tb[ATTR_CHANNELS_BAND]) {
		PRINTM(MERROR, "%s: null attr: tb[ATTR_GET_CH]=%p\n", __func__,
		       tb[ATTR_CHANNELS_BAND]);
		err = -EINVAL;
		goto done;
	}
	band = nla_get_u32(tb[ATTR_CHANNELS_BAND]);
	if (band > WIFI_BAND_MAX) {
		PRINTM(MERROR, "%s: invalid band=%d\n", __func__, band);
		err = -EINVAL;
		goto done;
	}

	memset(ch_out, 0x00, sizeof(ch_out));
	nchannel = MAX_CHANNEL_NUM;
	if (woal_band_to_valid_channels(priv, band, ch_out, &nchannel) !=
	    MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "get_channel_list: woal_band_to_valid_channels fail\n");
		return -EFAULT;
	}

	mem_needed = nla_total_size(nchannel * sizeof(ch_out[0])) +
		     nla_total_size(sizeof(nchannel)) + VENDOR_REPLY_OVERHEAD;
	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, mem_needed);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed");
		err = -ENOMEM;
		goto done;
	}

	if (nla_put_u32(skb, ATTR_NUM_CHANNELS, nchannel) ||
	    nla_put(skb, ATTR_CHANNEL_LIST, nchannel * sizeof(ch_out[0]),
		    ch_out)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		err = -ENOMEM;
		goto done;
	}
	err = cfg80211_vendor_cmd_reply(skb);
	if (err) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", err);
		goto done;
	}

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to get driver version
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_drv_version(struct wiphy *wiphy,
						struct wireless_dev *wdev,
						const void *data, int data_len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	int ret = 0;
	t_u32 drv_len = 0;
	char drv_version[MLAN_MAX_VER_STR_LEN] = {0};
	char *pos;

	ENTER();
	moal_memcpy_ext(priv->phandle, drv_version,
			&priv->phandle->driver_version, MLAN_MAX_VER_STR_LEN,
			MLAN_MAX_VER_STR_LEN);
	// drv_version is already initialized to 0 and hence null terminated
	// coverity[cert_str32_c_violation:SUPPRESS]
	drv_len = strlen(drv_version);
	pos = strstr(drv_version, "%s");
	/* remove 3 char "-%s" in driver_version string */
	if (pos != NULL)
		moal_memcpy_ext(priv->phandle, pos, pos + 3, strlen(pos) - 3,
				strlen(pos));

	reply_len = strlen(drv_version) + 1;
	drv_len -= 3;
	drv_version[drv_len] = '\0';

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	nla_put(skb, MRVL_WLAN_VENDOR_ATTR_NAME, reply_len,
		(t_u8 *)drv_version);
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get firmware version
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_fw_version(struct wiphy *wiphy,
					       struct wireless_dev *wdev,
					       const void *data, int data_len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	char end_c = '\0';
	int ret = 0;
	char fw_ver[32] = {0};
	t_u8 hotfix_ver = 0;
	union {
		t_u32 l;
		t_u8 c[4];
	} ver;

	ENTER();

	hotfix_ver = priv->phandle->fw_hotfix_version;
	ver.l = priv->phandle->fw_release_number;
	if (hotfix_ver) {
		if (snprintf(fw_ver, sizeof(fw_ver), "%u.%u.%u.p%u.%u%c",
			     ver.c[2], ver.c[1], ver.c[0], ver.c[3], hotfix_ver,
			     end_c) <= 0)
			PRINTM(MERROR, "Failed to write fw hotfix version\n");
	} else {
		if (snprintf(fw_ver, sizeof(fw_ver), "%u.%u.%u.p%u%c", ver.c[2],
			     ver.c[1], ver.c[0], ver.c[3], end_c) <= 0)
			PRINTM(MERROR, "Failed to write fw version\n");
	}
	reply_len = strlen(fw_ver) + 1;

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	nla_put(skb, MRVL_WLAN_VENDOR_ATTR_NAME, reply_len, (t_u8 *)fw_ver);
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get firmware memory dump
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_fw_dump(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int data_len)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	moal_handle *handle = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	int length = 0;
	struct sk_buff *skb = NULL;

	ENTER();
	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	handle = priv->phandle;

	if (!handle) {
		ret = -EFAULT;
		goto done;
	}
	memset(handle->firmware_dump_file, 0,
	       sizeof(handle->firmware_dump_file));

	length = sizeof(handle->firmware_dump_file);
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, length);
	if (!skb) {
		PRINTM(MERROR, "Failed to allocate memory for skb\n");
		ret = -ENOMEM;
		goto done;
	}
	nla_put(skb, ATTR_FW_DUMP_PATH, sizeof(handle->firmware_dump_file),
		handle->firmware_dump_file);
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get driver memory dump
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_drv_dump(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int data_len)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	moal_handle *handle = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	int length = 0;
	char driver_dump_file[128];
#ifndef DUMP_TO_PROC
	char path_name[64];
#endif
	struct sk_buff *skb = NULL;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	handle = priv->phandle;
#ifdef DUMP_TO_PROC
	memset(driver_dump_file, 0, sizeof(driver_dump_file));
	if (snprintf(driver_dump_file, sizeof(driver_dump_file),
		     "/proc/mwlan/") <= 0)
		PRINTM(MERROR, "Fail to print file path of driver_dump_file\n");
	if (handle->handle_idx) {
		if (snprintf(driver_dump_file, sizeof(driver_dump_file),
			     "drv_dump%d", handle->handle_idx) <= 0)
			PRINTM(MERROR,
			       "Fail to print handle_idx into driver_dump_file\n");
	} else {
		if (snprintf(driver_dump_file, sizeof(driver_dump_file),
			     "drv_dump") <= 0)
			PRINTM(MERROR,
			       "Fail to print drv_dump string into driver_dump_file\n");
	}
#else
	memset(path_name, 0, sizeof(path_name));
	woal_create_dump_dir(handle, path_name, sizeof(path_name));
	PRINTM(MMSG, "driver dump path name is %s\n", path_name);
	woal_dump_drv_info(handle, path_name);
	memset(driver_dump_file, 0, sizeof(driver_dump_file));
	snprintf(driver_dump_file, sizeof(driver_dump_file), "%s/%s", path_name,
		 "file_drv_info");
#endif
	PRINTM(MMSG, "driver dump file is %s\n", driver_dump_file);
	length = sizeof(driver_dump_file);
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, length);
	if (!skb) {
		PRINTM(MERROR, "Failed to allocate memory for skb\n");
		ret = -ENOMEM;
		goto done;
	}
	if (nla_put_string(skb, ATTR_DRV_DUMP_PATH, driver_dump_file)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		ret = -ENOMEM;
		goto done;
	}
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get supported feature set
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_supp_feature_set(struct wiphy *wiphy,
						     struct wireless_dev *wdev,
						     const void *data,
						     int data_len)
{
	struct sk_buff *skb = NULL;

	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_fw_info fw_info;

	t_u32 reply_len = 0;
	int ret = 0;
	t_u32 supp_feature_set = 0;

	ENTER();
	supp_feature_set = WLAN_FEATURE_INFRA
#if defined(UAP_SUPPORT) && defined(STA_SUPPORT)
			   | WLAN_FEATURE_AP_STA
#endif
			   | WLAN_FEATURE_LINK_LAYER_STATS |
			   WLAN_FEATURE_LOGGER | WLAN_FEATURE_RSSI_MONITOR |
			   WLAN_FEATURE_CONFIG_NDO | WLAN_FEATURE_SCAN_RAND |
			   WLAN_FEATURE_MKEEP_ALIVE | WLAN_FEATURE_PNO |
			   WLAN_FEATURE_TDLS;

	memset(&fw_info, 0, sizeof(mlan_fw_info));
	if (MLAN_STATUS_SUCCESS !=
	    woal_request_get_fw_info(priv, MOAL_IOCTL_WAIT, &fw_info)) {
		PRINTM(MERROR, "Fail to get fw info\n");
		ret = -EFAULT;
		goto done;
	}
	if (fw_info.fw_bands & BAND_A)
		supp_feature_set |= WLAN_FEATURE_INFRA_5G;
	if (fw_info.rtt_support)
		supp_feature_set |= WLAN_FEATURE_D2AP_RTT;
	priv->phandle->wifi_hal_flag = MTRUE;

	reply_len = sizeof(supp_feature_set);
	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}
	if (nla_put_u32(skb, ATTR_FEATURE_SET, supp_feature_set)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		ret = -ENOMEM;
		goto done;
	}
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to set country code
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_set_country_code(struct wiphy *wiphy,
						 struct wireless_dev *wdev,
						 const void *data, int data_len)
{
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	int ret = 0, rem, type;
	const struct nlattr *iter;
	char country[COUNTRY_CODE_LEN] = {0};

	ENTER();

	nla_for_each_attr (iter, data, data_len, rem) {
		type = nla_type(iter);
		switch (type) {
		case ATTR_COUNTRY_CODE:
			strncpy(country, nla_data(iter),
				MIN((int)sizeof(country) - 1, nla_len(iter)));
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			return ret;
		}
	}

	if (!moal_extflg_isset((moal_handle *)woal_get_wiphy_priv(wiphy),
			       EXT_DISABLE_REGD_BY_DRIVER))
		regulatory_hint(wiphy, country);

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get supported feature set
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_wifi_logger_supp_feature_set(
	struct wiphy *wiphy, struct wireless_dev *wdev, const void *data,
	int data_len)
{
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	int ret = 0;
	t_u32 supp_feature_set = 0;

	ENTER();

	supp_feature_set = WIFI_LOGGER_CONNECT_EVENT_SUPPORTED;
	reply_len = sizeof(supp_feature_set);
	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}
	if (nla_put_u32(skb, ATTR_WIFI_LOGGER_FEATURE_SET, supp_feature_set)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		ret = -ENOMEM;
		goto done;
	}
	ret = cfg80211_vendor_cmd_reply(skb);
	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief get ring status
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_id    ring buffer id
 * @param status     a pointer to wifi_ring_buffer_status
 *
 * @return    void
 */
static void woal_get_ring_status(moal_private *priv, int ring_id,
				 wifi_ring_buffer_status *status)
{
	int id = 0;
	wifi_ring_buffer *ring;

	ENTER();
	for (id = 0; id < RING_ID_MAX; id++) {
		ring = (wifi_ring_buffer *)priv->rings[id];
		if (ring && VALID_RING(ring->ring_id) &&
		    ring_id == ring->ring_id) {
			moal_memcpy(priv->phandle, status->name, ring->name,
				    sizeof(status->name) - 1);
			status->ring_id = ring->ring_id;
			status->ring_buffer_byte_size = ring->ring_size;
			status->written_bytes = ring->ctrl.written_bytes;
			status->written_records = ring->ctrl.written_records;
			status->read_bytes = ring->ctrl.read_bytes;
			status->verbose_level = ring->log_level;
			PRINTM(MINFO,
			       "%s, name: %s, ring_id: %d, ring_size : %d, written_bytes: %d, written_records: %d, read_bytes: %d\n",
			       __func__, status->name, status->ring_id,
			       status->ring_buffer_byte_size,
			       status->written_bytes, status->written_records,
			       status->read_bytes);
			break;
		}
	}
	LEAVE();
}

/**
 * @brief vendor command to get ring buffer status
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_ring_buff_status(struct wiphy *wiphy,
						     struct wireless_dev *wdev,
						     const void *data,
						     int data_len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	t_u8 ringIdx, ring_cnt = 0;
	int ret = 0;
	wifi_ring_buffer_status status[RING_ID_MAX];
	wifi_ring_buffer_status ring_status;

	ENTER();
	reply_len =
		RING_ID_MAX * sizeof(wifi_ring_buffer_status) + sizeof(t_u32);
	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* should sync with HAL side to decide payload layout.
	 * just ring buffer status or ring buffer num + ring buffer status
	 * currently only have ring buffer status
	 */
	for (ringIdx = 0; ringIdx < RING_ID_MAX; ringIdx++) {
		memset(&ring_status, 0, sizeof(wifi_ring_buffer_status));
		woal_get_ring_status(priv, ringIdx, &ring_status);
		moal_memcpy_ext(priv->phandle, &status[ring_cnt++],
				&ring_status, sizeof(wifi_ring_buffer_status),
				sizeof(wifi_ring_buffer_status));
	}

	if (nla_put_u32(skb, ATTR_NUM_RINGS, ring_cnt) ||
	    nla_put(skb, ATTR_RING_BUFFER_STATUS,
		    sizeof(wifi_ring_buffer_status) * ring_cnt, status)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);

	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief return ring_id based on ring_name
 *
 * @param priv          A pointer to moal_private struct
 * @param ring_name     A pointer to ring_name
 *
 * @return      An invalid ring id for failure or valid ring id on success.
 */
static int woal_get_ring_id_by_name(moal_private *priv, char *ring_name)
{
	int id;
	wifi_ring_buffer *ring;

	ENTER();

	for (id = 0; id < RING_ID_MAX; id++) {
		ring = (wifi_ring_buffer *)priv->rings[id];
		if (ring &&
		    !strncmp(ring->name, ring_name, sizeof(ring->name) - 1))
			break;
	}

	LEAVE();
	return id;
}

/**
 * @brief start logger
 *
 * @param priv          A pointer to moal_private struct
 * @param ring_name     string ring_name
 * @param log_level     log level to record
 * @param flags         reserved
 * @param time_intval   interval to report log to HAL
 * @param threshold     buffer threshold to report log to HAL
 *
 * @return      0: success  1: fail
 */
static int woal_start_logging(moal_private *priv, char *ring_name,
			      int log_level, int flags, int time_intval,
			      int threshold)
{
	int ret = 0;
	int ring_id;
	wifi_ring_buffer *ring_buffer;
	unsigned long lock_flags;

	ENTER();

	ring_id = woal_get_ring_id_by_name(priv, ring_name);
	if (!VALID_RING(ring_id)) {
		ret = -EINVAL;
		goto done;
	}

	PRINTM(MCMND,
	       "%s , log_level : %d, time_intval : %d, threshod %d Bytes\n",
	       __func__, log_level, time_intval, threshold);

	ring_buffer = (wifi_ring_buffer *)priv->rings[ring_id];
	if (!ring_buffer) {
		PRINTM(MERROR, "Ring is stopped!\n");
		ret = -EAGAIN;
		goto done;
	}

	spin_lock_irqsave(&ring_buffer->lock, lock_flags);
	if (ring_buffer->state == RING_STOP) {
		spin_unlock_irqrestore(&ring_buffer->lock, lock_flags);
		PRINTM(MERROR, "Ring is stopped!\n");
		ret = -EAGAIN;
		goto done;
	}
	ring_buffer->log_level = log_level;
	ring_buffer->threshold = threshold;
	if (log_level == 0)
		ring_buffer->state = RING_SUSPEND;
	else
		ring_buffer->state = RING_ACTIVE;
	ring_buffer->interval = msecs_to_jiffies(time_intval * MSEC_PER_SEC);
	spin_unlock_irqrestore(&ring_buffer->lock, lock_flags);

	if (log_level == 0) {
		cancel_delayed_work_sync(&ring_buffer->work);
	} else {
		if (ring_buffer->interval)
			schedule_delayed_work(&ring_buffer->work,
					      ring_buffer->interval);
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to start logging
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_start_logging(struct wiphy *wiphy,
					      struct wireless_dev *wdev,
					      const void *data, int len)
{
	int ret = 0, rem, type;
	char ring_name[RING_NAME_MAX] = {0};
	int log_level = 0, flags = 0, time_intval = 0, threshold = 0;
	const struct nlattr *iter;
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);

	ENTER();

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case ATTR_WIFI_LOGGER_RING_ID:
			strncpy(ring_name, nla_data(iter),
				MIN(sizeof(ring_name) - 1, nla_len(iter)));
			break;
		case ATTR_WIFI_LOGGER_VERBOSE_LEVEL:
			log_level = nla_get_u32(iter);
			break;
		case ATTR_WIFI_LOGGER_FLAGS:
			flags = nla_get_u32(iter);
			break;
		case ATTR_WIFI_LOGGER_MAX_INTERVAL_SEC:
			time_intval = nla_get_u32(iter);
			break;
		case ATTR_WIFI_LOGGER_MIN_DATA_SIZE:
			threshold = nla_get_u32(iter);
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
			goto done;
		}
	}

	ret = woal_start_logging(priv, ring_name, log_level, flags, time_intval,
				 threshold);
	if (ret < 0)
		PRINTM(MERROR, "Start_logging is failed ret: %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief get log data in ring buffer
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_name  ring name string
 *
 * @return      0: success  1: fail
 */
static int woal_trigger_get_ring_data(moal_private *priv, char *ring_name)
{
	int ret = 0;
	int ring_id;
	wifi_ring_buffer *ring_buffer;

	ENTER();

	ring_id = woal_get_ring_id_by_name(priv, ring_name);
	if (!VALID_RING(ring_id)) {
		PRINTM(MERROR, "invalid ring_id\n");
		ret = -EINVAL;
		goto done;
	}

	ring_buffer = (wifi_ring_buffer *)priv->rings[ring_id];
	if (!ring_buffer) {
		PRINTM(MERROR, "invalid ring_buffer\n");
		ret = -EINVAL;
		goto done;
	}
	if (ring_buffer->interval)
		cancel_delayed_work_sync(&ring_buffer->work);
	schedule_delayed_work(&ring_buffer->work, 0);

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get ring buffer data
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_ring_data(struct wiphy *wiphy,
					      struct wireless_dev *wdev,
					      const void *data, int len)
{
	int ret = 0, rem, type;
	char ring_name[RING_NAME_MAX] = {0};
	const struct nlattr *iter;
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);

	ENTER();

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case ATTR_WIFI_LOGGER_RING_ID:
			strncpy(ring_name, nla_data(iter),
				MIN(sizeof(ring_name) - 1, nla_len(iter)));
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
			goto done;
		}
	}

	ret = woal_trigger_get_ring_data(priv, ring_name);
	if (ret < 0)
		PRINTM(MERROR, "trigger_get_data failed ret:%d\n", ret);

done:
	LEAVE();
	return ret;
}

/**
 * @brief ring reset
 *
 * @param priv       A pointer to moal_private struct
 * @param ring    A pointer to wifi_ring_buffer
 *
 * @return    void
 */
static void woal_ring_reset(wifi_ring_buffer *ring)
{
	ENTER();
	ring->wp = 0;
	ring->rp = 0;
	memset(&(ring->ctrl), 0x00, sizeof(ring->ctrl));
	memset(ring->ring_buf, 0, ring->ring_size);

	LEAVE();
}

/**
 * @brief get ring buffer entry
 *
 * @param ring       A pointer to wifi_ring_buffer struct
 * @param offset     entry offset
 *
 * @return     A pointer to wifi_ring_buffer_entry struct
 */
static wifi_ring_buffer_entry *woal_get_ring_entry(wifi_ring_buffer *ring,
						   t_u32 offset)
{
	wifi_ring_buffer_entry *entry =
		(wifi_ring_buffer_entry *)(ring->ring_buf + offset);

	ENTER();

	if (!entry->entry_size)
		return (wifi_ring_buffer_entry *)ring->ring_buf;

	LEAVE();
	return entry;
}

/**
 * @brief get next ring buffer entry
 *
 * @param ring       A pointer to wifi_ring_buffer struct
 * @param offset     next entry offset
 *
 * @return     offset of next entry
 */
static t_u32 woal_get_ring_next_entry(wifi_ring_buffer *ring, t_u32 offset)
{
	wifi_ring_buffer_entry *entry =
		(wifi_ring_buffer_entry *)(ring->ring_buf + offset);
	wifi_ring_buffer_entry *next_entry = NULL;

	ENTER();
	/* make sure ring_size > entry's header size) */
	if ((offset + RING_ENTRY_SIZE) >= ring->ring_size) {
		/* move to head */
		LEAVE();
		return 0;
	}
	if (!entry->entry_size) {
		/* move to head */
		LEAVE();
		return 0;
	}
	/* make sure ring_size > next_entry's header len) */
	if ((offset + ENTRY_LENGTH(entry) + RING_ENTRY_SIZE) >=
	    ring->ring_size) {
		/* move to head */
		LEAVE();
		return 0;
	}
	next_entry = (wifi_ring_buffer_entry *)((t_u8 *)ring->ring_buf +
						offset + ENTRY_LENGTH(entry));
	if (!next_entry->entry_size) {
		/* move to head */
		LEAVE();
		return 0;
	}
	LEAVE();
	return offset + ENTRY_LENGTH(entry);
}

/**
 * @brief prepare log data to send to HAL
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_id    ring ID
 * @param data       A pointer to data buffer
 * @param buf_len    log data length
 *
 * @return      data length
 */
static int woal_ring_pull_data(moal_private *priv, int ring_id, void *data,
			       t_s32 buf_len)
{
	t_s32 r_len = 0;
	wifi_ring_buffer *ring;
	wifi_ring_buffer_entry *hdr;
	t_s32 buf_left = buf_len;

	ENTER();

	if (ring_id < 0) {
		PRINTM(MERROR, "%s(): Invalid ring id\n", __FUNCTION__);
		LEAVE();
		return r_len;
	}
	ring = (wifi_ring_buffer *)priv->rings[ring_id];

	/* get a fresh pending length */
	while (buf_left > 0) {
		hdr = woal_get_ring_entry(ring, ring->rp);
		if (buf_left < ENTRY_LENGTH(hdr))
			break;
		moal_memcpy_ext(priv->phandle, data, hdr, ENTRY_LENGTH(hdr),
				buf_left);
		r_len += ENTRY_LENGTH(hdr);
		data = (void *)((t_u8 *)data + ENTRY_LENGTH(hdr));
		buf_left -= ENTRY_LENGTH(hdr);
		ring->ctrl.read_bytes += ENTRY_LENGTH(hdr);
		if (!buf_left) {
			ring->rp += ENTRY_LENGTH(hdr);
			break;
		}
		if (ring->ctrl.read_bytes == ring->ctrl.written_bytes)
			break;
		ring->rp = woal_get_ring_next_entry(ring, ring->rp);
	}
	PRINTM(MDATA, "Ring pull data: [wp=%d rp=%d] r_len=%d buf_len=%d\n",
	       ring->wp, ring->rp, r_len, buf_len);

	LEAVE();
	return r_len;
}

/**
 * @brief send vendor event to kernel
 *
 * @param priv       A pointer to moal_private
 * @param event    vendor event
 * @param data     a pointer to data
 * @param  len     data length
 * @param ring_status    A pointer to wifi_ring_buffer status struct
 *
 * @return      0: success  1: fail
 */
static int
woal_ring_buffer_data_vendor_event(moal_private *priv, int ring_id, t_u8 *data,
				   int len,
				   wifi_ring_buffer_status *ring_status)
{
	struct wiphy *wiphy = NULL;
	struct sk_buff *skb = NULL;
	int event_id = 0;
	int ret = 0;

	ENTER();

	if (!priv || !priv->wdev || !priv->wdev->wiphy) {
		PRINTM(MERROR, "priv is null\n");
		ret = -EINVAL;
		goto done;
	}
	wiphy = priv->wdev->wiphy;
	PRINTM(MEVENT, "%s ring_id:%d\n", __func__, ring_id);
	event_id = woal_get_event_id(event_wifi_logger_ring_buffer_data);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = -EINVAL;
		goto done;
	}

/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev,
					  len + sizeof(wifi_ring_buffer_status),
					  event_id, GFP_ATOMIC);
#else
	skb = cfg80211_vendor_event_alloc(wiphy,
					  len + sizeof(wifi_ring_buffer_status),
					  event_id, GFP_ATOMIC);
#endif

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = -ENOMEM;
		goto done;
	}
	nla_put(skb, ATTR_RING_BUFFER_STATUS, sizeof(wifi_ring_buffer_status),
		ring_status);
	DBG_HEXDUMP(MEVT_D, "ring_buffer_data", data, len);
	nla_put(skb, ATTR_RING_BUFFER, len, data);

	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

done:
	LEAVE();
	return ret;
}

/**
 * @brief send log data to HAL
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_id    ring ID
 * @param data       A pointer to data buffer
 * @param len    log data length
 * @param ring_status    A pointer to wifi_ring_buffer status struct
 *
 * @return void
 */
static void woal_ring_data_send(moal_private *priv, int ring_id, void *data,
				const t_u32 len,
				wifi_ring_buffer_status *ring_status)
{
	struct net_device *ndev = priv->netdev;

	ENTER();

	if (!ndev)
		goto done;
	woal_ring_buffer_data_vendor_event(priv, ring_id, (t_u8 *)data, len,
					   ring_status);

done:
	LEAVE();
}

/**
 * @brief main thread to send log data
 *
 * @param work       A pointer to work_struct struct
 *
 * @return void
 */
static void woal_ring_poll_worker(struct work_struct *work)
{
	struct delayed_work *d_work = to_delayed_work(work);
	wifi_ring_buffer *ring_info =
		// Coverity violation raised for kernel's API
		// coverity[cert_arr39_c_violation:SUPPRESS]
		container_of(d_work, wifi_ring_buffer, work);
	moal_private *priv = ring_info->priv;
	int ringid = ring_info->ring_id;
	wifi_ring_buffer_status ring_status;
	void *buf = NULL;
	wifi_ring_buffer_entry *hdr;
	t_s32 buflen, rlen;
	unsigned long flags;

	ENTER();
	if (ring_info->state != RING_ACTIVE) {
		PRINTM(MERROR, "Ring is not active!\n");
		goto exit;
	}
	buf = kmalloc(ring_info->ring_size, GFP_KERNEL);
	if (!buf) {
		PRINTM(MERROR, "Fail to allocate buffer for poll work\n");
		goto exit;
	}
	spin_lock_irqsave(&ring_info->lock, flags);
	memset(&ring_status, 0, sizeof(ring_status));
	woal_get_ring_status(priv, ringid, &ring_status);
	PRINTM(MDATA, "Ring work: ringid %d write %d, read %d, size %d\n",
	       ringid, ring_status.written_bytes, ring_status.read_bytes,
	       ring_status.ring_buffer_byte_size);
	if (ring_status.written_bytes > ring_status.read_bytes) {
		buflen = ring_status.written_bytes - ring_status.read_bytes;
		if (buflen > ring_info->ring_size) {
			PRINTM(MERROR, "Pending data bigger than ring size\n");
			spin_unlock_irqrestore(&ring_info->lock, flags);
			goto exit;
		}
	} else {
		spin_unlock_irqrestore(&ring_info->lock, flags);
		PRINTM(MINFO, "No new records\n");
		goto exit;
	}

	rlen = woal_ring_pull_data(priv, ringid, buf, buflen);
	spin_unlock_irqrestore(&ring_info->lock, flags);
	hdr = (wifi_ring_buffer_entry *)buf;
	while (rlen > 0) {
		ring_status.read_bytes += ENTRY_LENGTH(hdr);
		woal_ring_data_send(priv, ringid, hdr, ENTRY_LENGTH(hdr),
				    &ring_status);
		rlen -= ENTRY_LENGTH(hdr);
		hdr = (wifi_ring_buffer_entry *)((t_u8 *)hdr +
						 ENTRY_LENGTH(hdr));
	}
exit:
	kfree(buf);
	if (ring_info->interval)
		schedule_delayed_work(&ring_info->work, ring_info->interval);

	LEAVE();
}

/**
 * @brief add log data to ring buffer
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_id    ring ID
 * @param hdr        A pointer to wifi_ring_buffer_entry struct
 * @param data       A pointer to data buffer
 *
 * @return      0: success  -1: fail
 */
static int woal_ring_push_data(moal_private *priv, int ring_id,
			       wifi_ring_buffer_entry *hdr, void *data)
{
	unsigned long flags;
	t_u32 w_len;
	wifi_ring_buffer *ring;
	wifi_ring_buffer_entry *w_entry;
	int ret = 0;

	ENTER();

	if (ring_id < 0) {
		PRINTM(MERROR, "%s(): Invalid ring id\n", __FUNCTION__);
		ret = -EINVAL;
		goto done;
	}
	ring = (wifi_ring_buffer *)priv->rings[ring_id];

	if (!ring || ring->state != RING_ACTIVE) {
		PRINTM(MERROR, "Ring is not active\n");
		ret = -EINVAL;
		goto done;
	}

	w_len = ENTRY_LENGTH(hdr);
	if (w_len > ring->ring_size) {
		PRINTM(MERROR, "event size too big =%d\n", w_len);
		ret = -EINVAL;
		goto done;
	}
	spin_lock_irqsave(&ring->lock, flags);
	PRINTM(MDATA, "Ring push data: rp=%d  wp=%d, w_len=%d\n", ring->rp,
	       ring->wp, w_len);
	/* prep the space */
	do {
		if (ring->rp == ring->wp) {
			if (ring->wp)
				woal_ring_reset(ring);
			break;
		}
		if (ring->rp < ring->wp) {
			if (ring->ring_size - ring->wp >= w_len) {
				break;
			} else if (ring->ring_size - ring->wp < w_len) {
				if (ring->wp)
					woal_ring_reset(ring);
				break;
			}
		}
		if (ring->rp > ring->wp) {
			PRINTM(MERROR, "ring->rp > ring->wp, reset ring\n");
			woal_ring_reset(ring);
			break;
		}
	} while (1);

	if ((ring->wp + w_len) > ring->ring_size ||
	    (ring->ctrl.written_bytes + w_len) > ring->ring_size) {
		spin_unlock_irqrestore(&ring->lock, flags);
		PRINTM(MERROR,
		       "Ring push buffer overflow: rp=%d  wp=%d, write_bytes=%d\n",
		       ring->rp, ring->wp, ring->ctrl.written_bytes);
		goto done;
	}

	w_entry = (wifi_ring_buffer_entry *)(ring->ring_buf + ring->wp);
	/* header */
	moal_memcpy_ext(priv->phandle, w_entry, hdr, RING_ENTRY_SIZE,
			ENTRY_LENGTH(w_entry));
	/* payload */
	moal_memcpy_ext(priv->phandle, (char *)w_entry + RING_ENTRY_SIZE, data,
			w_entry->entry_size, w_entry->entry_size);
	/* update write pointer */
	ring->wp += w_len;
	/* update statistics */
	ring->ctrl.written_records++;
	ring->ctrl.written_bytes += w_len;
	spin_unlock_irqrestore(&ring->lock, flags);

	PRINTM(MDATA,
	       "Ring push data: wp=%d rp=%d  write_bytes=%d, read_bytes=%d\n",
	       ring->wp, ring->rp, ring->ctrl.written_bytes,
	       ring->ctrl.read_bytes);

	/* if the current pending size is bigger than threshold */
	if (ring->threshold && (READ_AVAIL_SPACE(ring) >= ring->threshold)) {
		PRINTM(MDATA, "Ring threshold reached\n");
		if (ring->interval)
			cancel_delayed_work_sync(&ring->work);
		schedule_delayed_work(&ring->work, 0);
	}

done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function will init wifi logger ring buffer
 *
 *  @param priv          A pointer to moal_private structure
 *  @param ring_buff     A pointer to wifi_ring_buffer structure
 *  @param ringIdx       Ring buffer ID
 *  @param name          Ring buffer name
 *  @param ring_sz       Ring buffer size
 *
 *  @return        0 - success
 */
static int woal_init_ring_buffer_internal(moal_private *priv, void **ring,
					  t_u16 ringIdx, t_u8 *name,
					  t_u32 ring_sz)
{
	unsigned long flags;
	wifi_ring_buffer *ring_buff;

	ENTER();

	*ring = vmalloc(sizeof(wifi_ring_buffer));
	if (!unlikely(*ring)) {
		PRINTM(MERROR, "WiFi Logger: ring alloc failed\n");
		goto done;
	}
	memset(*ring, 0, sizeof(wifi_ring_buffer));
	ring_buff = (wifi_ring_buffer *)*ring;

	ring_buff->ring_buf = vmalloc(ring_sz);
	if (!unlikely(ring_buff->ring_buf)) {
		PRINTM(MERROR, "WiFi Logger: ring buffer data alloc failed\n");
		vfree(ring_buff);
		*ring = NULL;
		goto done;
	}
	memset(ring_buff->ring_buf, 0, ring_sz);
	spin_lock_init(&ring_buff->lock);
	spin_lock_irqsave(&ring_buff->lock, flags);
	ring_buff->rp = ring_buff->wp = 0;
	INIT_DELAYED_WORK(&ring_buff->work, woal_ring_poll_worker);
	moal_memcpy_ext(priv->phandle, ring_buff->name, name, strlen(name),
			RING_NAME_MAX - 1);
	ring_buff->name[RING_NAME_MAX - 1] = 0;
	ring_buff->ring_id = ringIdx;
	ring_buff->ring_size = ring_sz;
	ring_buff->state = RING_SUSPEND;
	ring_buff->threshold = ring_buff->ring_size / 2;
	ring_buff->priv = priv;
	spin_unlock_irqrestore(&ring_buff->lock, flags);
done:
	LEAVE();
	return 0;
}

/**
 * @brief init each ring in moal_private
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      data length
 */
static int woal_init_ring_buffer(moal_private *priv)
{
	ENTER();
	woal_init_ring_buffer_internal(priv, &priv->rings[VERBOSE_RING_ID],
				       VERBOSE_RING_ID, VERBOSE_RING_NAME,
				       DEFAULT_RING_BUFFER_SIZE);
	woal_init_ring_buffer_internal(priv, &priv->rings[EVENT_RING_ID],
				       EVENT_RING_ID, EVENT_RING_NAME,
				       DEFAULT_RING_BUFFER_SIZE);
	LEAVE();
	return 0;
}

/**
 * @brief deinit each ring in moal_private
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      data length
 */
static int woal_deinit_ring_buffer(moal_private *priv)
{
	int i;
	enum ring_state ring_state = RING_STOP;
	unsigned long lock_flags = 0;
	wifi_ring_buffer *ring_buff;

	ENTER();

	for (i = 0; i < RING_ID_MAX; i++) {
		ring_buff = (wifi_ring_buffer *)priv->rings[i];
		if (!ring_buff)
			continue;
		spin_lock_irqsave(&ring_buff->lock, lock_flags);
		ring_state = ring_buff->state;
		if (ring_state == RING_ACTIVE) {
			ring_buff->state = RING_STOP;
			ring_buff->interval = 0;
		}
		spin_unlock_irqrestore(&ring_buff->lock, lock_flags);
		cancel_delayed_work_sync(&ring_buff->work);
		vfree(ring_buff->ring_buf);
		ring_buff->ring_buf = NULL;
		vfree(ring_buff);
		priv->rings[i] = NULL;
	}

	LEAVE();
	return 0;
}

/**
 * @brief add log data to ring buffer
 *
 * @param priv       A pointer to moal_private struct
 * @param ring_id    ring ID
 * @param hdr        A pointer to wifi_ring_buffer_entry struct
 * @param data       A pointer to data buffer
 *
 * @return      0: success  -1: fail
 */
int woal_ring_event_logger(moal_private *priv, int ring_id, pmlan_event pmevent)
{
	t_u8 event_buf[100] = {0};
	wifi_ring_buffer_driver_connectivity_event *connectivity_event;
	tlv_log *tlv;
	t_u8 *pos;
	wifi_ring_buffer_entry msg_hdr;
	wifi_ring_buffer *ring;

	ENTER();

	if (ring_id < 0 || ring_id >= RING_ID_MAX) {
		PRINTM(MERROR, "Ring_id is invalid \n");
		goto done;
	}

	ring = (wifi_ring_buffer *)priv->rings[ring_id];

	if (!ring || ring->state != RING_ACTIVE) {
		PRINTM(MINFO, "Ring is not active\n");
		goto done;
	}

	switch (pmevent->event_id) {
	case MLAN_EVENT_ID_DRV_ASSOC_SUCC_LOGGER:
		if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
			assoc_logger_data *pbss_desc =
				(assoc_logger_data *)pmevent->event_buf;
			memset(&msg_hdr, 0, sizeof(msg_hdr));
			msg_hdr.flags |= RING_BUFFER_ENTRY_FLAGS_HAS_TIMESTAMP;
			msg_hdr.type = ENTRY_TYPE_CONNECT_EVENT;
			connectivity_event =
				(wifi_ring_buffer_driver_connectivity_event *)
					event_buf;
			connectivity_event->event = WIFI_EVENT_ASSOC_COMPLETE;
			pos = (t_u8 *)connectivity_event->tlvs;

			tlv = (tlv_log *)pos;
			tlv->tag = WIFI_TAG_VENDOR_SPECIFIC;
			tlv->length = MLAN_MAC_ADDR_LENGTH / 2;
			moal_memcpy_ext(priv->phandle, tlv->value,
					pbss_desc->oui, tlv->length,
					sizeof(event_buf) -
						(tlv->value - event_buf));
			msg_hdr.entry_size += tlv->length + TLV_LOG_HEADER_LEN;
			pos = pos + tlv->length + TLV_LOG_HEADER_LEN;

			tlv = (tlv_log *)pos;
			tlv->tag = WIFI_TAG_BSSID;
			tlv->length = sizeof(pbss_desc->bssid);
			moal_memcpy_ext(
				priv->phandle, tlv->value, pbss_desc->bssid,
				sizeof(pbss_desc->bssid),
				sizeof(event_buf) - (tlv->value - event_buf));
			msg_hdr.entry_size += tlv->length + TLV_LOG_HEADER_LEN;
			pos = pos + tlv->length + TLV_LOG_HEADER_LEN;

			tlv = (tlv_log *)pos;
			tlv->tag = WIFI_TAG_SSID;
			tlv->length = pbss_desc->ssid_len;
			moal_memcpy_ext(priv->phandle, tlv->value,
					pbss_desc->ssid, tlv->length,
					sizeof(event_buf) -
						(tlv->value - event_buf));
			msg_hdr.entry_size += tlv->length + TLV_LOG_HEADER_LEN;
			pos = pos + tlv->length + TLV_LOG_HEADER_LEN;

			if (pbss_desc->rssi) {
				tlv = (tlv_log *)pos;
				tlv->tag = WIFI_TAG_RSSI;
				tlv->length = sizeof(pbss_desc->rssi);
				moal_memcpy_ext(
					priv->phandle, tlv->value,
					&pbss_desc->rssi, tlv->length,
					sizeof(event_buf) -
						(tlv->value - event_buf));
				msg_hdr.entry_size +=
					tlv->length + TLV_LOG_HEADER_LEN;
				pos = pos + tlv->length + TLV_LOG_HEADER_LEN;
			}
			if (pbss_desc->channel) {
				tlv = (tlv_log *)pos;
				tlv->tag = WIFI_TAG_CHANNEL;
				tlv->length = sizeof(pbss_desc->channel);
				moal_memcpy_ext(
					priv->phandle, tlv->value,
					&pbss_desc->channel,
					sizeof(pbss_desc->channel),
					sizeof(event_buf) -
						(tlv->value - event_buf));
				msg_hdr.entry_size +=
					tlv->length + TLV_LOG_HEADER_LEN;
			}
			msg_hdr.entry_size =
				MIN(UINT16_MAX,
				    msg_hdr.entry_size +
					    sizeof(connectivity_event->event));
			/* msg_hdr.entry_size is carefully bounded using MIN
			 * function and all data fits within the statically
			 * sized event_buf, ensuring safe access */
			// coverity[overrun-buffer-arg:SUPPRESS]
			// coverity[cert_arr30_c_violation:SUPPRESS]
			// coverity[cert_str31_c_violation:SUPPRESS]
			DBG_HEXDUMP(MCMD_D, "connectivity_event",
				    (t_u8 *)connectivity_event,
				    msg_hdr.entry_size);
			woal_ring_push_data(priv, ring_id, &msg_hdr,
					    connectivity_event);
		}
		break;
	case MLAN_EVENT_ID_DRV_ASSOC_FAILURE_LOGGER:
		if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
			int status_code = *(int *)pmevent->event_buf;

			memset(&msg_hdr, 0, sizeof(msg_hdr));
			msg_hdr.flags |= RING_BUFFER_ENTRY_FLAGS_HAS_TIMESTAMP;
			msg_hdr.type = ENTRY_TYPE_CONNECT_EVENT;
			connectivity_event =
				(wifi_ring_buffer_driver_connectivity_event *)
					event_buf;
			connectivity_event->event = WIFI_EVENT_ASSOC_COMPLETE;
			pos = (t_u8 *)connectivity_event->tlvs;
			tlv = (tlv_log *)pos;
			tlv->tag = WIFI_TAG_STATUS;
			tlv->length = sizeof(status_code);
			moal_memcpy_ext(priv->phandle, tlv->value, &status_code,
					sizeof(status_code),
					sizeof(event_buf) -
						(tlv->value - event_buf));
			msg_hdr.entry_size += tlv->length + 4;
			msg_hdr.entry_size += sizeof(connectivity_event->event);
			woal_ring_push_data(priv, ring_id, &msg_hdr,
					    connectivity_event);
		}
		break;
	case MLAN_EVENT_ID_DRV_DISCONNECT_LOGGER:
		if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) {
			t_u16 reason_code = *(t_u16 *)pmevent->event_buf;

			memset(&msg_hdr, 0, sizeof(msg_hdr));
			msg_hdr.flags |= RING_BUFFER_ENTRY_FLAGS_HAS_TIMESTAMP;
			msg_hdr.type = ENTRY_TYPE_CONNECT_EVENT;
			connectivity_event =
				(wifi_ring_buffer_driver_connectivity_event *)
					event_buf;
			connectivity_event->event = WIFI_EVENT_ASSOC_COMPLETE;
			pos = (t_u8 *)connectivity_event->tlvs;
			tlv = (tlv_log *)pos;
			tlv->tag = WIFI_TAG_REASON_CODE;
			tlv->length = sizeof(reason_code);
			moal_memcpy_ext(priv->phandle, tlv->value, &reason_code,
					sizeof(reason_code),
					sizeof(event_buf) -
						(tlv->value - event_buf));
			msg_hdr.entry_size += tlv->length + 4;
			msg_hdr.entry_size += sizeof(connectivity_event->event);
			woal_ring_push_data(priv, ring_id, &msg_hdr,
					    connectivity_event);
		}
		break;
	default:
		break;
	}
done:
	LEAVE();
	return 0;
}

/**
 * @brief send vendor event to kernel
 *
 * @param priv               A pointer to moal_private struct
 * @param wake_reason        wake_reason
 *
 * @return      0: success  1: fail
 */
static int woal_wake_reason_vendor_event(moal_private *priv,
					 mlan_ds_hs_wakeup_reason wake_reason)
{
	struct wiphy *wiphy = NULL;
	struct sk_buff *skb = NULL;
	int event_id = 0;
	int ret = 0;

	ENTER();

	if (!priv || !priv->wdev || !priv->wdev->wiphy) {
		PRINTM(MERROR, "priv is null\n");
		ret = -EINVAL;
		goto done;
	}
	wiphy = priv->wdev->wiphy;

	event_id = woal_get_event_id(event_wake_reason_report);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev,
					  sizeof(wake_reason.hs_wakeup_reason),
					  event_id, GFP_ATOMIC);
#else
	skb = cfg80211_vendor_event_alloc(wiphy,
					  sizeof(wake_reason.hs_wakeup_reason),
					  event_id, GFP_ATOMIC);
#endif

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = -ENOMEM;
		goto done;
	}

	PRINTM(MINFO, "wake_reason.hs_wakeup_reason = %d\n",
	       wake_reason.hs_wakeup_reason);
	nla_put_u16(skb, ATTR_WAKE_REASON_STAT, wake_reason.hs_wakeup_reason);
	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

done:
	PRINTM(MINFO, "wake reason vendor event ret %d\n", ret);
	LEAVE();
	return ret;
}

/**
 * @brief log wake reason
 *
 * @param priv       A pointer to moal_private struct
 * @param wake_reason    wake_reason
 *
 * @return      0: success  -1: fail
 */
int woal_wake_reason_logger(moal_private *priv,
			    mlan_ds_hs_wakeup_reason wake_reason)
{
	int ret = 0;

	ENTER();

	ret = woal_wake_reason_vendor_event(priv, wake_reason);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to start packet fate monitor
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int
woal_cfg80211_subcmd_start_packet_fate_monitor(struct wiphy *wiphy,
					       struct wireless_dev *wdev,
					       const void *data, int data_len)
{
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	int ret = 0;

	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);

	ENTER();

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Enable pkt fate monitor and use drvdbg MDAT_D to control forwarding
	 * data packet to kernel */
	priv->pkt_fate_monitor_enable = MTRUE;

	ret = cfg80211_vendor_cmd_reply(skb);

	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief send vendor event to kernel
 *
 * @param priv               A pointer to moal_private struct
 * @param pkt_type           tx or rx
 * @param fate               packet fate
 * @param payload_type       type of payload
 * @param drv_ts_usec        driver time in usec
 * @param fw_ts_usec         firmware time in usec
 * @param data               frame data
 * @param len                frame data len
 *
 * @return      0: success  1: fail
 */
static int woal_packet_fate_vendor_event(moal_private *priv,
					 packet_fate_packet_type pkt_type,
					 t_u8 fate, frame_type payload_type,
					 t_u32 drv_ts_usec, t_u32 fw_ts_usec,
					 t_u8 *data, t_u32 len)
{
	struct wiphy *wiphy = NULL;
	struct sk_buff *skb = NULL;
	int event_id = 0;
	int ret = 0;
	PACKET_FATE_REPORT fate_report;

	ENTER();

	if (!priv || !priv->wdev || !priv->wdev->wiphy) {
		PRINTM(MERROR, "priv is null\n");
		ret = -EINVAL;
		goto done;
	}
	wiphy = priv->wdev->wiphy;

	event_id = woal_get_event_id(event_packet_fate_monitor);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev,
					  len + sizeof(PACKET_FATE_REPORT),
					  event_id, GFP_ATOMIC);
#else
	skb = cfg80211_vendor_event_alloc(
		wiphy, len + sizeof(PACKET_FATE_REPORT), event_id, GFP_ATOMIC);
#endif

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = -ENOMEM;
		goto done;
	}

	memset(&fate_report, 0, sizeof(PACKET_FATE_REPORT));
	if (pkt_type == PACKET_TYPE_TX) {
		fate_report.u.tx_report_i.fate = fate;
		fate_report.u.tx_report_i.frame_inf.payload_type = payload_type;
		fate_report.u.tx_report_i.frame_inf.driver_timestamp_usec =
			drv_ts_usec;
		fate_report.u.tx_report_i.frame_inf.firmware_timestamp_usec =
			fw_ts_usec;
		fate_report.u.tx_report_i.frame_inf.frame_len = len;
		nla_put(skb, ATTR_PACKET_FATE_TX, sizeof(PACKET_FATE_REPORT),
			&fate_report);
	} else {
		fate_report.u.rx_report_i.fate = fate;
		fate_report.u.rx_report_i.frame_inf.payload_type = payload_type;
		fate_report.u.rx_report_i.frame_inf.driver_timestamp_usec =
			drv_ts_usec;
		fate_report.u.rx_report_i.frame_inf.firmware_timestamp_usec =
			fw_ts_usec;
		fate_report.u.rx_report_i.frame_inf.frame_len = len;
		nla_put(skb, ATTR_PACKET_FATE_RX, sizeof(PACKET_FATE_REPORT),
			&fate_report);
	}
	DBG_HEXDUMP(MCMD_D, "fate_report", (t_u8 *)&fate_report,
		    sizeof(PACKET_FATE_REPORT));

	nla_put(skb, ATTR_PACKET_FATE_DATA, len, data);
	DBG_HEXDUMP(MCMD_D, "packet_fate_data", data, len);

	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

done:
	PRINTM(MINFO, "packet fate vendor event ret %d\n", ret);
	LEAVE();
	return ret;
}

/**
 * @brief log packet fate
 *
 * @param priv               A pointer to moal_private struct
 * @param pkt_type           tx or rx
 * @param fate               packet fate
 * @param payload_type       type of payload
 * @param drv_ts_usec        driver time in usec
 * @param fw_ts_usec         firmware time in usec
 * @param data               frame data
 * @param len                frame data len
 *
 * @return      0: success  -1: fail
 */
int woal_packet_fate_monitor(moal_private *priv,
			     packet_fate_packet_type pkt_type, t_u8 fate,
			     frame_type payload_type, t_u32 drv_ts_usec,
			     t_u32 fw_ts_usec, t_u8 *data, t_u32 len)
{
	int ret = 0;

	ENTER();

	if (priv->pkt_fate_monitor_enable)
		ret = woal_packet_fate_vendor_event(priv, pkt_type, fate,
						    payload_type, drv_ts_usec,
						    fw_ts_usec, data, len);

	PRINTM(MINFO, "packet fate monitor ret %d\n", ret);
	LEAVE();
	return ret;
}

/**
 * @brief init packet_filter in moal_private
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      0: success  -1: fail
 */
static int woal_init_packet_filter(moal_private *priv)
{
	int ret = 0;
	packet_filter *pkt_filter = NULL;

	ENTER();

	pkt_filter = vmalloc(sizeof(packet_filter));
	if (!unlikely(pkt_filter)) {
		PRINTM(MERROR, "WiFi Logger: packet_filter alloc failed\n");
		ret = -ENOMEM;
		goto done;
	}

	memset(pkt_filter, 0, sizeof(packet_filter));

	spin_lock_init(&pkt_filter->lock);
	pkt_filter->packet_filter_max_len = PACKET_FILTER_MAX_LEN;
	pkt_filter->packet_filter_len = 0;
	pkt_filter->packet_filter_version = APF_VERSION;
	pkt_filter->state = PACKET_FILTER_STATE_STOP;

	priv->packet_filter = pkt_filter;

done:
	LEAVE();
	return ret;
}

/**
 * @brief deinit packet_filter in moal_private
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      0: success  -1: fail
 */
static int woal_deinit_packet_filter(moal_private *priv)
{
	int ret = 0;
	packet_filter *pkt_filter = NULL;
	unsigned long flags;

	ENTER();

	pkt_filter = priv->packet_filter;

	if (!unlikely(pkt_filter))
		goto done;

	spin_lock_irqsave(&pkt_filter->lock, flags);
	pkt_filter->state = PACKET_FILTER_STATE_INIT;
	spin_unlock_irqrestore(&pkt_filter->lock, flags);

	vfree(pkt_filter);
	/* packet_filter is cleared after deinitialization and free, with no
	 * expected concurrent access, making locking unnecessary */
	// coverity[LOCK_EVASION:SUPPRESS]
	priv->packet_filter = NULL;

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to set packet filter
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_set_packet_filter(struct wiphy *wiphy,
						  struct wireless_dev *wdev,
						  const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	int ret = 0, rem, type;
	const struct nlattr *iter;
	packet_filter *pkt_filter = NULL;
	t_u32 packet_filter_len = 0;
	unsigned long flags;

	ENTER();
	pkt_filter = priv->packet_filter;
	if (!unlikely(pkt_filter)) {
		PRINTM(MERROR, "packet_filter not init\n");
		ret = -EINVAL;
		LEAVE();
		return ret;
	}

	spin_lock_irqsave(&pkt_filter->lock, flags);
	if (pkt_filter->state == PACKET_FILTER_STATE_INIT) {
		spin_unlock_irqrestore(&pkt_filter->lock, flags);
		PRINTM(MERROR, "packet_filter not init\n");
		ret = -EINVAL;
		goto done;
	}

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case ATTR_PACKET_FILTER_TOTAL_LENGTH:
			packet_filter_len = nla_get_u32(iter);
			if (packet_filter_len >
			    pkt_filter->packet_filter_max_len) {
				spin_unlock_irqrestore(&pkt_filter->lock,
						       flags);
				PRINTM(MERROR,
				       "packet_filter_len exceed max\n");
				ret = -EINVAL;
				goto done;
			}
			break;
		case ATTR_PACKET_FILTER_PROGRAM:
			strncpy(pkt_filter->packet_filter_program,
				nla_data(iter),
				MIN(packet_filter_len, nla_len(iter)));
			pkt_filter->packet_filter_len =
				(t_u8)MIN(packet_filter_len, nla_len(iter));
			pkt_filter->state = PACKET_FILTER_STATE_START;
			break;
		default:
			spin_unlock_irqrestore(&pkt_filter->lock, flags);
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
			goto done;
		}
	}

	spin_unlock_irqrestore(&pkt_filter->lock, flags);
	if (pkt_filter->packet_filter_len)
		DBG_HEXDUMP(MDAT_D, "packet_filter_program",
			    pkt_filter->packet_filter_program,
			    MIN(pkt_filter->packet_filter_len,
				PACKET_FILTER_MAX_LEN));

	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get packet filter capability
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_get_packet_filter_capability(
	struct wiphy *wiphy, struct wireless_dev *wdev, const void *data,
	int data_len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	t_u32 reply_len = 0;
	int ret = 0;
	packet_filter *pkt_filter;

	ENTER();
	pkt_filter = priv->packet_filter;
	if (!unlikely(pkt_filter)) {
		PRINTM(MERROR, "packet_filter not init\n");
		ret = -EINVAL;
		goto done;
	}

	reply_len = sizeof(pkt_filter->packet_filter_version) +
		    sizeof(pkt_filter->packet_filter_max_len);
	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	if (nla_put_u32(skb, ATTR_PACKET_FILTER_VERSION,
			pkt_filter->packet_filter_version) ||
	    nla_put_u32(skb, ATTR_PACKET_FILTER_MAX_LEN,
			pkt_filter->packet_filter_max_len)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);

	if (ret)
		PRINTM(MERROR, "Vendor command reply failed ret = %d\n", ret);
done:
	LEAVE();
	return ret;
}

/**
 * Runs a packet filtering program over a packet.
 *
 * @param program the program bytecode.
 * @param program_len the length of {@code apf_program} in bytes.
 * @param packet the packet bytes, starting from the 802.3 header and not
 *               including any CRC bytes at the end.
 * @param packet_len the length of {@code packet} in bytes.
 * @param filter_age the number of seconds since the filter was programmed.
 *
 * @return non-zero if packet should be passed to AP, zero if
 *         packet should be dropped.
 */
static int process_packet(const t_u8 *program, t_u32 program_len,
			  const t_u8 *packet, t_u32 packet_len,
			  t_u32 filter_age)
{
	/* Program counter */
	t_u32 pc = 0;
	/* Memory slot values. */
	t_u32 mem[MEM_ITEMS] = {};
	/* Register values. */
	t_u32 reg[2] = {};
	/* Number of instructions remaining to execute. This is done to make
	 * sure there is upper bound on execution time. It should never be hit
	 * and is only for safety. Initialize to the number of bytes in the
	 * program which is an
	 * upper bound on the number of instructions in the program.
	 */
	t_u32 instructions_left = program_len;

	t_u8 bytecode;
	t_u32 opcode;
	t_u32 reg_num;
	t_u32 len_field;
	t_u32 imm_len;
	t_u32 end_offs;
	t_u32 val = 0;
	t_u32 last_pkt_offs;
	t_u32 imm = 0;
	int32_t sign_imm = 0;
	t_u32 offs = 0;
	t_u32 i;
	t_u32 load_size;

/* Is offset within program bounds? */
#define WITHIN_PROGRAM_BOUNDS(p) (ENFORCE_UNSIGNED(p) && (p) < program_len)
/* Is offset within packet bounds? */
#define WITHIN_PACKET_BOUNDS(p) (ENFORCE_UNSIGNED(p) && (p) < packet_len)
/* Verify an internal condition and accept packet if it fails. */
#define ASSERT_RETURN(c)                                                       \
	do {                                                                   \
		if (!(c))                                                      \
			return PASS_PKT;                                       \
	} while (0)
/* If not within program bounds, Accept the packet */
#define ASSERT_WITHIN_PROGRAM_BOUNDS(p) ASSERT_RETURN(WITHIN_PROGRAM_BOUNDS(p))
/* If not within packet bounds, Accept the packet */
#define ASSERT_WITHIN_PKT_BOUNDS(p) ASSERT_RETURN(WITHIN_PACKET_BOUNDS(p))
/* If not within program or not ahead of program counter, Accept the packet */
#define ASSERT_FORWARD_WITHIN_PROGRAM(p)                                       \
	ASSERT_RETURN(WITHIN_PROGRAM_BOUNDS(p) && (p) >= pc)

	/* Fill in pre-filled memory slot values. */
	mem[MEM_OFFSET_PKT_SIZE] = packet_len;
	mem[MEM_OFFSET_FILTER_AGE] = filter_age;
	ASSERT_WITHIN_PKT_BOUNDS(APF_FRAME_HEADER_SIZE);

	/* Only populate if IP version is IPv4. */
	if ((packet[APF_FRAME_HEADER_SIZE] & 0xf0) == 0x40) {
		mem[MEM_OFFSET_IPV4_HEADER_SIZE] =
			(packet[APF_FRAME_HEADER_SIZE] & 15) * 4;
	}

	do {
		if (pc == program_len)
			return PASS_PKT;
		else if (pc == (program_len + 1))
			return DROP_PKT;
		ASSERT_WITHIN_PROGRAM_BOUNDS(pc);
		bytecode = program[pc++];
		opcode = GET_OPCODE(bytecode);
		reg_num = GET_REGISTER(bytecode);

		/* All instructions have immediate fields, so load them now. */
		len_field = GET_IMM_LENGTH(bytecode);
		imm = 0;
		sign_imm = 0;

#define REG (reg[reg_num])
#define OTHER_REG (reg[reg_num ^ 1])

		if (len_field) {
			imm_len = 1 << (len_field - 1);
			ASSERT_FORWARD_WITHIN_PROGRAM(pc + imm_len - 1);
			for (i = 0; i < imm_len; i++)
				imm = (imm << 8) | program[pc++];
			/* Sign extend imm into signed_imm. */
			sign_imm = imm << ((4 - imm_len) * 8);
			sign_imm >>= (4 - imm_len) * 8;
		}
		switch (opcode) {
		case NXP_LDB_OPCODE:
		case NXP_LDH_OPCODE:
		case NXP_LDW_OPCODE:
		case NXP_LDBX_OPCODE:
		case NXP_LDHX_OPCODE:
		case NXP_LDWX_OPCODE: {
			offs = imm;
			if (opcode >= NXP_LDBX_OPCODE) {
				/* Note: this can overflow and actually decrease
				 * offs.
				 */
				offs += reg[1];
			}
			ASSERT_WITHIN_PKT_BOUNDS(offs);
			switch (opcode) {
			case NXP_LDB_OPCODE:
			case NXP_LDBX_OPCODE:
				load_size = 1;
				break;
			case NXP_LDH_OPCODE:
			case NXP_LDHX_OPCODE:
				load_size = 2;
				break;
			case NXP_LDW_OPCODE:
			case NXP_LDWX_OPCODE:
				load_size = 4;
				break;
				/* Immediately enclosing switch statement
				 * guarantees
				 * opcode cannot be any other value.
				 */
			}
			end_offs = offs + (load_size - 1);
			/* Catch overflow/wrap-around. */
			ASSERT_RETURN(end_offs >= offs);
			ASSERT_WITHIN_PKT_BOUNDS(end_offs);
			val = 0;
			while (load_size--)
				val = (val << 8) | packet[offs++];
			REG = val;
			break;
		}
		case NXP_JMP_OPCODE:
			/* This can jump backwards. Infinite looping prevented
			 * by instructions_remaining.
			 */
			pc += imm;
			break;
		case NXP_JEQ_OPCODE:
		case NXP_JNE_OPCODE:
		case NXP_JGT_OPCODE:
		case NXP_JLT_OPCODE:
		case NXP_JSET_OPCODE:
		case NXP_JNEBS_OPCODE: {
			/* Load second immediate field. */
			t_u32 cmp_imm = 0;

			if (reg_num == 1) {
				cmp_imm = reg[1];
			} else if (len_field != 0) {
				t_u32 cmp_imm_len = 1 << (len_field - 1);

				ASSERT_FORWARD_WITHIN_PROGRAM(pc + cmp_imm_len -
							      1);
				for (i = 0; i < cmp_imm_len; i++)
					cmp_imm =
						(cmp_imm << 8) | program[pc++];
			}
			switch (opcode) {
			case NXP_JEQ_OPCODE:
				if (reg[0] == cmp_imm)
					pc += imm;
				break;
			case NXP_JNE_OPCODE:
				if (reg[0] != cmp_imm)
					pc += imm;
				break;
			case NXP_JGT_OPCODE:
				if (reg[0] > cmp_imm)
					pc += imm;
				break;
			case NXP_JLT_OPCODE:
				if (reg[0] < cmp_imm)
					pc += imm;
				break;
			case NXP_JSET_OPCODE:
				if (reg[0] & cmp_imm)
					pc += imm;
				break;
			case NXP_JNEBS_OPCODE: {
				/* cmp_imm is size in bytes of data to compare.
				 * pc is offset of program bytes to compare.
				 * imm is jump target offset.
				 * REG is offset of packet bytes to compare.
				 */
				ASSERT_FORWARD_WITHIN_PROGRAM(pc + cmp_imm - 1);
				ASSERT_WITHIN_PKT_BOUNDS(REG);
				last_pkt_offs = REG + cmp_imm - 1;
				ASSERT_RETURN(last_pkt_offs >= REG);
				ASSERT_WITHIN_PKT_BOUNDS(last_pkt_offs);
				if (memcmp(program + pc, packet + REG, cmp_imm))
					pc += imm;
				/* skip past comparison bytes */
				pc += cmp_imm;
				break;
			}
			}
			break;
		}
		case NXP_ADD_OPCODE:
			reg[0] += reg_num ? reg[1] : imm;
			break;
		case NXP_MUL_OPCODE:
			reg[0] *= reg_num ? reg[1] : imm;
			break;
		case NXP_DIV_OPCODE: {
			const t_u32 div_operand = reg_num ? reg[1] : imm;

			ASSERT_RETURN(div_operand);
			reg[0] /= div_operand;
			break;
		}
		case NXP_AND_OPCODE:
			reg[0] &= reg_num ? reg[1] : imm;
			break;
		case NXP_OR_OPCODE:
			reg[0] |= reg_num ? reg[1] : imm;
			break;
		case NXP_SH_OPCODE: {
			const int32_t shift_val =
				reg_num ? (int32_t)reg[1] : sign_imm;
			if (shift_val > 0)
				reg[0] <<= shift_val;
			else
				reg[0] >>= -shift_val;
			break;
		}
		case NXP_LI_OPCODE:
			REG = sign_imm;
			break;
		case NXP_EXT_OPCODE:
			if (
/* If LDM_EXT_OPCODE is 0 and imm is compared with it, a compiler error will
 * result,
 * instead just enforce that imm is unsigned (so it's always greater or equal to
 * 0).
 */
#if NXP_LDM_EXT_OPCODE == 0
				ENFORCE_UNSIGNED(imm) &&
#else
				imm >= NXP_LDM_EXT_OPCODE &&
#endif
				imm < (NXP_LDM_EXT_OPCODE + MEM_ITEMS)) {
				REG = mem[imm - NXP_LDM_EXT_OPCODE];
			} else if (imm >= NXP_STM_EXT_OPCODE &&
				   imm < (NXP_STM_EXT_OPCODE + MEM_ITEMS)) {
				mem[imm - NXP_STM_EXT_OPCODE] = REG;
			} else
				switch (imm) {
				case NXP_NOT_EXT_OPCODE:
					REG = ~REG;
					break;
				case NXP_NEG_EXT_OPCODE:
					REG = -REG;
					break;
				case NXP_SWAP_EXT_OPCODE: {
					t_u32 tmp = REG;

					REG = OTHER_REG;
					OTHER_REG = tmp;
					break;
				}
				case NXP_MOV_EXT_OPCODE:
					REG = OTHER_REG;
					break;
				/* Unknown extended opcode */
				default:
					/* Bail out */
					return PASS_PKT;
				}
			break;
		/* Unknown opcode */
		default:
			/* Bail out */
			return PASS_PKT;
		}
	} while (instructions_left--);
	return PASS_PKT;
}

/**
 * @brief filter packet
 *
 * @param priv          A pointer to moal_private struct
 * @param data          packet data
 * @param len           packet len
 * @param filter_age    filter age
 * @return non-zero if packet should be passed to AP, zero if
 *         packet should be dropped.
 */
int woal_filter_packet(moal_private *priv, t_u8 *data, t_u32 len,
		       t_u32 filter_age)
{
	packet_filter *pkt_filter = NULL;
	int ret = PASS_PKT;
	unsigned long flags;

	ENTER();
	pkt_filter = (packet_filter *)priv->packet_filter;
	if (!unlikely(pkt_filter)) {
		PRINTM(MINFO, "packet_filter not init\n");
		goto done;
	}

	if (pkt_filter->state != PACKET_FILTER_STATE_START)
		goto done;

	/* pkt_filter is already validated in call of unlikely macro */
	// coverity[misra_c_2012_directive_4_14_violation:SUPPRESS]
	// coverity[tainted_data:SUPPRESS]
	DBG_HEXDUMP(MDAT_D, "packet_filter_program",
		    pkt_filter->packet_filter_program,
		    pkt_filter->packet_filter_len);
	DBG_HEXDUMP(MDAT_D, "packet_filter_data", data, len);
	spin_lock_irqsave(&pkt_filter->lock, flags);
	/* pkt_filter is already validated in call of unlikely macro */
	// coverity[misra_c_2012_directive_4_14_violation:SUPPRESS]
	// coverity[tainted_data:SUPPRESS]
	ret = process_packet(pkt_filter->packet_filter_program,
			     pkt_filter->packet_filter_len, data, len,
			     filter_age);
	spin_unlock_irqrestore(&pkt_filter->lock, flags);

done:
	PRINTM(MINFO, "packet filter ret %d\n", ret);
	LEAVE();
	return ret;
}

/**
 * @brief init wifi hal
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      0: success  1: fail
 */
int woal_init_wifi_hal(moal_private *priv)
{
	ENTER();
	woal_init_ring_buffer(priv);
	priv->pkt_fate_monitor_enable = MFALSE;
	woal_init_packet_filter(priv);

	LEAVE();
	return 0;
}

/**
 * @brief deinit wifi hal
 *
 * @param priv       A pointer to moal_private struct
 *
 * @return      0: success  1: fail
 */
int woal_deinit_wifi_hal(moal_private *priv)
{
	ENTER();
	woal_deinit_ring_buffer(priv);
	priv->pkt_fate_monitor_enable = MFALSE;
	woal_deinit_packet_filter(priv);

	LEAVE();
	return 0;
}

/**
 * @brief Prints the scancfg params from the mlan_ds_scan struct
 *
 * @param scan	A pointer to mlan_ds_scan struct
 *
 * @return      void
 */
static void woal_print_scancfg_params(mlan_ds_scan *scan)
{
	if (!scan)
		return;
	PRINTM(MCMND,
	       "scancfg params: scan_type = 0x%x, scan_mode = 0x%x, scan_probe = 0x%x \n",
	       scan->param.scan_cfg.scan_type, scan->param.scan_cfg.scan_mode,
	       scan->param.scan_cfg.scan_probe);

	PRINTM(MCMND, "scancfg params: passive_to_active_scan = 0x%x \n",
	       scan->param.scan_cfg.passive_to_active_scan);

	PRINTM(MCMND,
	       "scancfg params: specific_scan_time = 0x%x, active_scan_time = 0x%x, passive_scan_time = 0x%x \n",
	       scan->param.scan_cfg.scan_time.specific_scan_time,
	       scan->param.scan_cfg.scan_time.active_scan_time,
	       scan->param.scan_cfg.scan_time.passive_scan_time);

	PRINTM(MCMND, "scancfg params: ext_scan = 0x%x\n",
	       scan->param.scan_cfg.ext_scan);
	PRINTM(MCMND, "scancfg params: scan_chan_gap = 0x%x\n",
	       scan->param.scan_cfg.scan_chan_gap);
}

/**
 * @brief Parse the vendor cmd input data based on attribute len
 * 	  and copy each attrubute into a output buffer/integer array
 *
 * @param data			A pointer to input data buffer
 * @param data_len		Input data buffer total len
 * @param user_buff		A pointer to output data buffer after the
 * parsing
 * @param buff_len		Maximum no. of data attributes to be parsed
 * @param user_data_len		No. of data attributes that are parsed
 *
 * @return      0: success  -1: fail
 */
static int woal_parse_vendor_cmd_attributes(t_u8 *data, t_u32 data_len,
					    t_u32 *user_buff, t_u32 buff_len,
					    t_u16 *user_data_len)
{
	t_u16 i = 0, j = 0, len = 0;

	len = strlen(data);
	for (i = 0, j = 0; (i < data_len) && (j < buff_len); ++j) {
		t_u32 value = 0, value1 = 0;
		t_u8 attr_len = 0;
		attr_len = (t_u8) * (data + i);
		++i;
		if (attr_len > 0) {
			t_u8 k = 0;
			for (k = 0; k < attr_len; ++k) {
				value1 = (t_u8) * (data + i + k);
				value = (value << 8) + value1;
			}
			i = i + k;
		} else {
			PRINTM(MERROR, "\nIn parse_args: Invalid attr_len \n");
			*user_data_len = 0;
			return -1;
		}
		user_buff[j] = value;
	}
	*user_data_len = j;
	return 0;
}

/**
 * @brief Vendor cmd to trigger the scancfg params.
 *	Set/Get the scancfg params to/from driver
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_set_get_scancfg(struct wiphy *wiphy,
						struct wireless_dev *wdev,
						const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_scan *scan = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	struct sk_buff *skb = NULL;
	t_s32 user_data[9];
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u16 ret_length = 1;
	t_u8 get_data = 0, get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;
	ENTER();

	if (len < 1 || (len + 1) < 0) {
		PRINTM(MERROR, "vendor cmd: scancfg - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	data_buff[len] = '\0';

	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get scancfg params!\n");
		get_val = (t_u8) * (data_buff);

		/* Get scancfg works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get scancfg failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		get_data = 1;
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set scancfg params!\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     ARRAY_SIZE(user_data),
						     &user_data_len)) {
			PRINTM(MMSG,
			       "vendor cmd: Couldn't parse the scancfg params!\n");
			ret = -EINVAL;
			goto done;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_scan));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, scancfg!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	scan = (mlan_ds_scan *)req->pbuf;
	scan->sub_command = MLAN_OID_SCAN_CONFIG;
	req->req_id = MLAN_IOCTL_SCAN;

	/* Validate each scancfg parameters */
	if (user_data_len) {
		DBG_HEXDUMP(MCMD_D, "scancfg input dump: ", (t_u8 *)user_data,
			    (user_data_len * sizeof(t_u32)));
		moal_memcpy_ext(priv->phandle, &scan->param.scan_cfg, user_data,
				sizeof(user_data),
				sizeof(scan->param.scan_cfg));
		if (scan->param.scan_cfg.scan_type > MLAN_SCAN_TYPE_PASSIVE) {
			PRINTM(MERROR,
			       "vendor cmd:Invalid argument for scan type\n");
			ret = -EINVAL;
			goto done;
		}
		if (scan->param.scan_cfg.scan_mode > MLAN_SCAN_MODE_ANY) {
			PRINTM(MERROR,
			       "vendor cmd:Invalid argument for scan mode\n");
			ret = -EINVAL;
			goto done;
		}
		if (scan->param.scan_cfg.scan_probe > MAX_PROBES) {
			PRINTM(MERROR,
			       "vendor cmd:Invalid argument for scan probes\n");
			ret = -EINVAL;
			goto done;
		}
		if ((scan->param.scan_cfg.scan_time.specific_scan_time >
		     MRVDRV_MAX_ACTIVE_SCAN_CHAN_TIME) ||
		    (scan->param.scan_cfg.scan_time.active_scan_time >
		     MRVDRV_MAX_ACTIVE_SCAN_CHAN_TIME) ||
		    (scan->param.scan_cfg.scan_time.passive_scan_time >
		     MRVDRV_MAX_PASSIVE_SCAN_CHAN_TIME)) {
			PRINTM(MERROR, "Invalid argument for scan time\n");
			ret = -EINVAL;
			goto done;
		}
		if (scan->param.scan_cfg.passive_to_active_scan >
		    MLAN_PASS_TO_ACT_SCAN_DIS) {
			PRINTM(MERROR,
			       "Invalid argument for Passive to Active Scan\n");
			ret = -EINVAL;
			goto done;
		}
		if (scan->param.scan_cfg.ext_scan > MLAN_EXT_SCAN_ENH) {
			PRINTM(MERROR, "Invalid argument for extended scan\n");
			ret = -EINVAL;
			goto done;
		}
		if (scan->param.scan_cfg.scan_chan_gap >
		    MRVDRV_MAX_SCAN_CHAN_GAP_TIME) {
			PRINTM(MERROR,
			       "Invalid argument for scan channel gap\n");
			ret = -EINVAL;
			goto done;
		}

		req->action = MLAN_ACT_SET;
		if (scan->param.scan_cfg.scan_time.specific_scan_time)
			priv->phandle->user_scan_cfg = MTRUE;
		PRINTM(MINFO, "vendor cmd: SET ioctl request for scanfg\n");
		woal_print_scancfg_params(scan);
	} else {
		PRINTM(MINFO, "vendor cmd: GET ioctl request for scanfg\n");
		req->action = MLAN_ACT_GET;
	}

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status == MLAN_STATUS_SUCCESS) {
		PRINTM(MMSG, "Set/Get scancfg ioctl successfull\n");
		if (!user_data_len) {
			moal_memcpy_ext(priv->phandle, user_data,
					&scan->param.scan_cfg,
					sizeof(scan->param.scan_cfg),
					sizeof(user_data));
			DBG_HEXDUMP(MCMD_D, "scancfg dump: ", (t_u8 *)user_data,
				    sizeof(user_data));
			ret_length = sizeof(mlan_scan_cfg);
		}
	} else {
		PRINTM(MERROR, "Set/Get scancfg ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, ret_length);
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}
	/* Get scancfg if an input argument passed is 00 */
	if (!user_data_len && get_data == 1) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		scan = (mlan_ds_scan *)req->pbuf;
		pos = skb_put(skb, sizeof(mlan_scan_cfg));
		moal_memcpy_ext(priv->phandle, pos, &scan->param.scan_cfg,
				sizeof(mlan_scan_cfg), sizeof(mlan_scan_cfg));
		woal_print_scancfg_params(scan);
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);

	LEAVE();
	return ret;
}

/*
 * @brief A common function copies an user data(from integer array) into
 * different types of structures. Declare a layout based on each member size of
 * a strucure within the caller().
 *
 * @param phandle		A pointer to moal handler
 * @param dest_struct		Final destination strucuture
 * @param src_data		A pointer to input user data/integer array
 * @param src_data_len		Input user data length
 * @param dest_struct_len	Destination data structure length
 * @param layout		A pointer to integer array/layout describing struct
 * member
 * *
 * @return      0: success  -1: fail
 * */
static void
woal_memcpy_user_intarray_to_struct(moal_handle *phandle, void *dest_struct,
				    t_u32 *src_data, t_u32 src_data_len,
				    t_u32 dest_struct_len, t_u32 *layout)
{
	t_u8 *dest = (t_u8 *)dest_struct;
	t_u16 i = 0;

	if (!dest_struct || !src_data) {
		PRINTM(MERROR, "dest/src pointer is null\n");
	}

	for (i = 0; (layout[i] > 0 && i < src_data_len); ++i) {
		moal_memcpy_ext(phandle, dest, src_data, layout[i], layout[i]);
		dest += layout[i];
		if (layout[i] > sizeof(t_u32)) {
			src_data += layout[i] / sizeof(t_u32);
			src_data += (layout[i] % sizeof(t_u32)) ? 1 : 0;
		} else
			src_data += 1;
	}
}

/*
 * @brief A common function directly copies different type of structures into
 * user data buffer(integere array). Declare a layout based on each member size
 * of a strucure within the caller function.
 *
 * @param phandle		A pointer to moal handler
 * @param dest_data		A pointer to destination data buffer/integer
 * array
 * @param src_struct		A pointer to source structure
 * @param src_struct_len	Source data structure length
 * @param dest_data_len		Destination user data length
 * @param layout		A pointer to integer array/layout describing struct
 * member
 * *
 * @return      0: success  -1: fail
 * */
static void
woal_memcpy_struct_to_user_intarray(moal_handle *phandle, t_u32 *dest_data,
				    void *src_struct, t_u32 src_struct_len,
				    t_u32 dest_data_len, t_u32 *layout)
{
	t_u8 *src = (t_u8 *)src_struct;
	t_u16 i = 0;

	if (!dest_data || !src_struct) {
		PRINTM(MERROR, "dest/src pointer is null\n");
	}

	for (i = 0; (layout[i] > 0 && i < dest_data_len); ++i) {
		moal_memcpy_ext(phandle, dest_data, src, layout[i], layout[i]);
		src += layout[i];
		if (layout[i] > sizeof(t_u32)) {
			dest_data += layout[i] / sizeof(t_u32);
			dest_data += (layout[i] % sizeof(t_u32)) ? 1 : 0;
		} else
			dest_data += 1;
	}
}

/**
 * @brief Prints the addba params from the woal_print_addba_param
 *
 * @param scan	A pointer to woal_print_addba_param struct
 *
 * @return      void
 */
static void woal_print_addba_params(mlan_ds_11n_addba_param *addba)
{
	if (!addba) {
		PRINTM(MERROR, "addba param is null\n");
		return;
	}

	PRINTM(MCMND,
	       "ADDBA: timeout:%d txwinsize:%d rxwinsize:%d txamsdu=%d, rxamsdu=%d\n",
	       addba->timeout, addba->txwinsize, addba->rxwinsize,
	       addba->txamsdu, addba->rxamsdu);
	return;
}

/**
 * @brief API to trigger the addba params.
 *	It sets or gets the addba params
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_set_get_addbaparams(struct wiphy *wiphy,
						    struct wireless_dev *wdev,
						    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_11n_cfg *cfg_addba = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	struct sk_buff *skb = NULL;
	t_u32 user_data[5];
	/* Define layout as per required structure */
	t_u32 layout[5] = {sizeof(t_u32), sizeof(t_u32), sizeof(t_u32),
			   sizeof(char), sizeof(char)};
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u16 ret_length = 1;
	t_u8 get_data = 0, get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;
	ENTER();

	if (len < 1 || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: addbaparams - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	data_buff[len] = '\0';

	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get addbaparams!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get addbaparams failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		get_data = 1;
		memset((char *)user_data, 0, sizeof(user_data));
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set addbaparams !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     ARRAY_SIZE(user_data),
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the addbaparams!\n");
			ret = -EINVAL;
			goto done;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_11n_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, addbaparams!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	cfg_addba = (mlan_ds_11n_cfg *)req->pbuf;
	cfg_addba->sub_command = MLAN_OID_11N_CFG_ADDBA_PARAM;
	req->req_id = MLAN_IOCTL_11N_CFG;

	/* Validate each addbaparams parameters */
	if (user_data_len) {
		DBG_HEXDUMP(MCMD_D,
			    "addbaparams input dump: ", (t_u8 *)user_data,
			    (user_data_len * sizeof(t_u32)));
		/* To copy an user data in an integer array format into strcture
		 */
		woal_memcpy_user_intarray_to_struct(
			priv->phandle, (void *)&cfg_addba->param.addba_param,
			user_data, ARRAY_SIZE(user_data),
			sizeof(cfg_addba->param.addba_param), layout);

		woal_print_addba_params(&cfg_addba->param.addba_param);
		if (cfg_addba->param.addba_param.timeout >
		    MLAN_DEFAULT_BLOCK_ACK_TIMEOUT) {
			PRINTM(MERROR, "Incorrect addba timeout value.\n");
			ret = -EINVAL;
			goto done;
		}
		if (cfg_addba->param.addba_param.txwinsize >
		    MLAN_AMPDU_MAX_TXWINSIZE) {
			PRINTM(MERROR, "Incorrect Tx window size.\n");
			ret = -EINVAL;
			goto done;
		}
		if (cfg_addba->param.addba_param.rxwinsize >
		    MLAN_AMPDU_MAX_RXWINSIZE) {
			PRINTM(MERROR, "Incorrect Rx window size.\n");
			ret = -EINVAL;
			goto done;
		}
		if (cfg_addba->param.addba_param.txamsdu > 1 ||
		    cfg_addba->param.addba_param.rxamsdu > 1) {
			PRINTM(MERROR, "Incorrect Tx/Rx amsdu.\n");
			ret = -EINVAL;
			goto done;
		}
		req->action = MLAN_ACT_SET;
		PRINTM(MINFO,
		       "vendor cmd: SET ioctl request for addbaparams\n");
	} else {
		PRINTM(MINFO,
		       "vendor cmd: GET ioctl request for addbaparams\n");
		req->action = MLAN_ACT_GET;
	}

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status == MLAN_STATUS_SUCCESS) {
		PRINTM(MMSG, "Set/Get addbaparams ioctl successfull\n");
		if (!user_data_len) {
			/* To copy an strcture members into user data/integer
			 * array separately */
			woal_memcpy_struct_to_user_intarray(
				priv->phandle, user_data,
				(void *)&cfg_addba->param.addba_param,
				sizeof(cfg_addba->param.addba_param),
				ARRAY_SIZE(user_data), layout);
			woal_print_addba_params(&cfg_addba->param.addba_param);
			ret_length = sizeof(user_data);
		}
	} else {
		PRINTM(MERROR, "Set/Get addbaparams ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, ret_length);
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get addbaparams if an input data argument is 00 */
	if (!user_data_len && get_data == 1) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		DBG_HEXDUMP(MCMD_D, "addbaparams dump: ", (t_u8 *)user_data,
			    sizeof(user_data));
		cfg_addba = (mlan_ds_11n_cfg *)req->pbuf;
		pos = skb_put(skb, sizeof(user_data));
		moal_memcpy_ext(priv->phandle, pos, user_data,
				sizeof(user_data), sizeof(user_data));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief API to trigger the vendor cmd related to
 * hostcmd/sys_cfg_80211d_country_ie. It sets/get/clear the function/operation
 * that is specific the hostcmd.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_hostcmd(struct wiphy *wiphy,
					struct wireless_dev *wdev,
					const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_misc_cfg *misc_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	struct sk_buff *skb = NULL;
	HostCmd_DS_GEN cmd_info;
	t_s32 ret = 0;
	t_u16 ret_length = 1;
	t_u16 action = 0;
	t_u8 get_data = 0;
	const t_u8 *data_buff = (const t_u8 *)data;
	t_u8 *pos = NULL;
	ENTER();

	if (len < (sizeof(HostCmd_DS_GEN) + sizeof(action))) {
		PRINTM(MERROR, "vendor cmd: Invalid hostcmd!\n");
		ret = -EINVAL;
		goto done;
	}

	moal_memcpy_ext(priv->phandle, &cmd_info, data_buff,
			sizeof(HostCmd_DS_GEN), sizeof(HostCmd_DS_GEN));
	action = (const u16) * (data_buff + sizeof(cmd_info));

	PRINTM(MMSG, "vendor cmd: hostcmd len=%d, action=%d\n", len, action);
	if (action == 0)
		get_data = 1;

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl memory, hostcmd!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	misc_cfg = (mlan_ds_misc_cfg *)req->pbuf;
	misc_cfg->sub_command = MLAN_OID_MISC_HOST_CMD;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	req->action = action;
	misc_cfg->param.hostcmd.len = woal_le16_to_cpu(cmd_info.size);

	/* Copy the entire command data into hostcmd cmd buffer */
	moal_memcpy_ext(priv->phandle, misc_cfg->param.hostcmd.cmd, data_buff,
			misc_cfg->param.hostcmd.len, MRVDRV_SIZE_OF_CMD_BUFFER);

	DBG_HEXDUMP(MCMD_D, "vendor cmd: hostcmd cmd dump",
		    (t_u8 *)misc_cfg->param.hostcmd.cmd,
		    misc_cfg->param.hostcmd.len);

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status == MLAN_STATUS_SUCCESS) {
		PRINTM(MMSG, "Set/Clear/Get hostcmd ioctl successfull\n");
		if (get_data) {
			ret_length = misc_cfg->param.hostcmd.len;
			PRINTM(MMSG, "vendor cmd: hostcmd GET, len=%d\n",
			       ret_length);
		}
	} else {
		PRINTM(MERROR, "Set/Clear/Get hostcmd ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, ret_length);
	if (!skb) {
		PRINTM(MERROR, "vendor cmd: memory allocation failed \n");
		ret = -ENOMEM;
		goto done;
	}

	if (get_data && ret_length > 1) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		DBG_HEXDUMP(MCMD_D, "vendor cmd: hostcmd dump",
			    (t_u8 *)misc_cfg->param.hostcmd.cmd, ret_length);
		pos = skb_put(skb, ret_length);
		moal_memcpy_ext(priv->phandle, pos, misc_cfg->param.hostcmd.cmd,
				misc_cfg->param.hostcmd.len,
				misc_cfg->param.hostcmd.len);
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);
done:
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get link layer statistic
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_link_statistic_get(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct sk_buff *skb = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_get_info *info = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	wifi_radio_stat *radio_stat = NULL;
	wifi_radio_stat *radio_stat_tmp = NULL;
	wifi_iface_stat *iface_stat = NULL;
	t_u32 num_radio = 0, iface_stat_len = 0, radio_stat_len = 0;
	int err = -1, length = 0, i;
	char *ioctl_link_stats_buf = NULL;
	t_u64 cur_time = 0;
	t_u64 inter_msec = 0;
	t_u64 max_msec = (t_u64)24 * (t_u64)24 * (t_u64)3600 * (t_u64)1000;
	moal_handle *handle = priv->phandle;

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(t_u32) + BUF_MAXLEN);
	if (req == NULL) {
		PRINTM(MERROR, "Could not allocate mlan ioctl request!\n");
		return -ENOMEM;
	}

	/* Fill request buffer */
	info = (mlan_ds_get_info *)req->pbuf;
	info->sub_command = MLAN_OID_LINK_STATS;
	req->req_id = MLAN_IOCTL_GET_INFO;
	req->action = MLAN_ACT_GET;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "get link layer statistic fail\n");
		goto done;
	}

	ioctl_link_stats_buf = info->param.link_statistic;
	num_radio = *((t_u32 *)info->param.link_statistic);

	radio_stat =
		(wifi_radio_stat *)(ioctl_link_stats_buf + sizeof(num_radio));
	radio_stat_len = num_radio * sizeof(wifi_radio_stat);

	/* Re-write on_time/tx_time/rx_time/on_time_scan from moal handle */
	PRINTM(MINFO, "handle->on_time=%llu\n", handle->on_time);
	if (handle->on_time) {
		moal_get_boot_ktime(handle, &cur_time);
		inter_msec = moal_do_div(cur_time - handle->on_time, 1000000);
		PRINTM(MINFO, "cur_time=%llu inter_msec=%llu max_msec=%llu\n",
		       cur_time, inter_msec, max_msec);
		/* When we report the time up, u32 is not big enough(represent
		 * max 49days) and might out of range, make the max value to
		 * 24days.
		 */
		if (inter_msec > max_msec) {
			PRINTM(MINFO,
			       "Out of range, set inter_msec=%llu to max_msec=%llu\n",
			       inter_msec, max_msec);
			inter_msec = max_msec;
		}
	}
	PRINTM(MINFO, "handle->tx_time=%llu\n", handle->tx_time);
	PRINTM(MINFO, "handle->rx_time=%llu\n", handle->rx_time);
	PRINTM(MINFO, "handle->scan_time=%llu\n", handle->scan_time);
	radio_stat_tmp = radio_stat;
	for (i = 0; i < num_radio; i++) {
		radio_stat_tmp->on_time = (t_u32)inter_msec;
		radio_stat_tmp->tx_time =
			(t_u32)moal_do_div(handle->tx_time, 1000);
		radio_stat_tmp->rx_time =
			(t_u32)moal_do_div(handle->rx_time, 1000);
		radio_stat_tmp->on_time_scan =
			(t_u32)moal_do_div(handle->scan_time, 1000);
		radio_stat_tmp++;
	}

	iface_stat = (wifi_iface_stat *)(ioctl_link_stats_buf +
					 sizeof(num_radio) + radio_stat_len);

	iface_stat_len = sizeof(wifi_iface_stat);

	/* could get peer info with separate cmd */
	for (i = 0; i < iface_stat->num_peers; i++) {
		/* no need copy, just increase iface_stat length*/
		iface_stat_len += sizeof(wifi_peer_info) +
				  sizeof(wifi_rate_stat) *
					  iface_stat->peer_info[i].num_rate;
	}

	for (i = 0; i < MAX_AC_QUEUES; i++) {
		iface_stat->ac[i].rx_mpdu = priv->rx_pkt_ac[i];
	}

	/* Here the length doesn't contain addition 2 attribute header length */
	length = NLA_HDRLEN * 2 + sizeof(num_radio) + radio_stat_len +
		 iface_stat_len;

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, length);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed\n");
		goto done;
	}

	if (nla_put_u32(skb, ATTR_LL_STATS_NUM_RADIO, num_radio) ||
	    nla_put(skb, ATTR_LL_STATS_RADIO, radio_stat_len, radio_stat) ||
	    nla_put(skb, ATTR_LL_STATS_IFACE, iface_stat_len, iface_stat)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		goto done;
	}

	PRINTM(MDAT_D, "%s: <<< Start DUMP\n", __func__);
	PRINTM(MDAT_D, "sizeof(wifi_radio_stat)=%zu\n",
	       sizeof(wifi_radio_stat));
	DBG_HEXDUMP(MDAT_D, "radio_stat", (t_u8 *)radio_stat, radio_stat_len);
	PRINTM(MDAT_D, "sizeof(wifi_channel_stat)=%zu\n",
	       sizeof(wifi_channel_stat));
	DBG_HEXDUMP(MDAT_D, "iface_stat", (t_u8 *)iface_stat, iface_stat_len);
	PRINTM(MDAT_D, "num_radio=%d\n", num_radio);
	radio_stat_tmp = radio_stat;
	for (i = 0; i < num_radio; i++) {
		PRINTM(MDAT_D, "--radio_stat[%d]--\n", i);
		PRINTM(MDAT_D, "radio=%d\n", radio_stat_tmp->radio);
		PRINTM(MDAT_D, "on_time=%d\n", radio_stat_tmp->on_time);
		PRINTM(MDAT_D, "tx_time=%d\n", radio_stat_tmp->tx_time);
		PRINTM(MDAT_D, "reserved0=%d\n", radio_stat_tmp->reserved0);
		PRINTM(MDAT_D, "rx_time=%d\n", radio_stat_tmp->rx_time);
		PRINTM(MDAT_D, "on_time_scan=%d\n",
		       radio_stat_tmp->on_time_scan);
		PRINTM(MDAT_D, "on_time_nbd=%d\n", radio_stat_tmp->on_time_nbd);
		PRINTM(MDAT_D, "on_time_gscan=%d\n",
		       radio_stat_tmp->on_time_gscan);
		PRINTM(MDAT_D, "on_time_roam_scan=%d\n",
		       radio_stat_tmp->on_time_roam_scan);
		PRINTM(MDAT_D, "on_time_pno_scan=%d\n",
		       radio_stat_tmp->on_time_pno_scan);
		PRINTM(MDAT_D, "on_time_hs20=%d\n",
		       radio_stat_tmp->on_time_hs20);
		PRINTM(MDAT_D, "num_channels=%d\n",
		       radio_stat_tmp->num_channels);
		radio_stat_tmp++;
	}
	PRINTM(MDAT_D, "%s: >>> End DUMP\n", __func__);

	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err))
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", err);

done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	return err;
}

/**
 * @brief API to trigger the link layer statistics collection.
 *   Unless his API is invoked - link layer statistics will not be collected.
 *   Radio statistics (once started) do not stop or get reset unless
 *   wifi_clear_link_stats is invoked, Interface statistics (once started)
 *   reset and start afresh after each connection.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_link_statistic_set(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	moal_private *priv = (moal_private *)woal_get_netdev_priv(wdev->netdev);
	struct nlattr *tb[ATTR_LL_STATS_MAX + 1];
	wifi_link_layer_params ll_params;
	mlan_ioctl_req *req = NULL;
	mlan_ds_get_info *info = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	int err = 0;

	err = nla_parse(tb, ATTR_LL_STATS_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err)
		return err;

	if (!tb[ATTR_LL_STATS_MPDU_SIZE_THRESHOLD] ||
	    !tb[ATTR_LL_STATS_AGGRESSIVE_STATS_GATHERING])
		return -EINVAL;

	ll_params.mpdu_size_threshold =
		nla_get_u32(tb[ATTR_LL_STATS_MPDU_SIZE_THRESHOLD]);
	ll_params.aggressive_statistics_gathering =
		nla_get_u32(tb[ATTR_LL_STATS_AGGRESSIVE_STATS_GATHERING]);

	PRINTM(MEVENT,
	       "link layer params mpdu_size_threshold = 0x%x, aggressive_statistics_gathering = 0x%x\n",
	       ll_params.mpdu_size_threshold,
	       ll_params.aggressive_statistics_gathering);

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(t_u32) +
					sizeof(mlan_ds_get_info));
	if (req == NULL) {
		PRINTM(MERROR, "Could not allocate mlan ioctl request!\n");
		return -ENOMEM;
	}

	/* Fill request buffer */
	info = (mlan_ds_get_info *)req->pbuf;
	info->sub_command = MLAN_OID_LINK_STATS;
	req->req_id = MLAN_IOCTL_GET_INFO;
	req->action = MLAN_ACT_SET;

	/* Configure parameter to firmware */
	moal_memcpy_ext(priv->phandle, info->param.link_statistic, &ll_params,
			sizeof(ll_params),
			sizeof(mlan_ds_get_info) - sizeof(t_u32));

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status == MLAN_STATUS_SUCCESS)
		PRINTM(MMSG, "enable link layer statistic successfully\n");

	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	return 0;
}

/**
 * @brief clear function should download command to fimrware,
 *	  so that firmware could cleanup per peer statistic number
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_link_statistic_clr(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	moal_private *priv = (moal_private *)woal_get_netdev_priv(wdev->netdev);
	struct nlattr *tb[ATTR_LL_STATS_MAX + 1];
	mlan_ioctl_req *req = NULL;
	mlan_ds_get_info *info = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	u32 stats_clear_req_mask = 0x0, stats_clear_rsp_mask = 0x0;
	u8 stop_req = 0x0, stop_rsp = 0x0;
	struct sk_buff *skb = NULL;
	int err = 0, length = 0;

	err = nla_parse(tb, ATTR_LL_STATS_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err)
		return err;

	if (!tb[ATTR_LL_STATS_CLEAR_REQ_MASK])
		return -EINVAL;
	else
		stats_clear_req_mask =
			nla_get_u32(tb[ATTR_LL_STATS_CLEAR_REQ_MASK]);

	if (!tb[ATTR_LL_STATS_STOP_REQ])
		return -EINVAL;
	else
		stop_req = nla_get_u8(tb[ATTR_LL_STATS_STOP_REQ]);

	PRINTM(MEVENT,
	       "link layer clear stats_clear_req_mask = 0x%x, stop_req = 0x%x\n",
	       stats_clear_req_mask, stop_req);

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(t_u32) +
					sizeof(mlan_ds_get_info));
	if (req == NULL) {
		PRINTM(MERROR, "Could not allocate mlan ioctl request!\n");
		return -ENOMEM;
	}

	/* Fill request buffer */
	info = (mlan_ds_get_info *)req->pbuf;
	info->sub_command = MLAN_OID_LINK_STATS;
	req->req_id = MLAN_IOCTL_GET_INFO;
	req->action = MLAN_ACT_CLEAR;

	/* Send IOCTL request to MLAN */
	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status == MLAN_STATUS_SUCCESS)
		PRINTM(MMSG, "disable link layer statistic successfully\n");

	length = NLA_HDRLEN + sizeof(stats_clear_rsp_mask) + sizeof(stop_rsp);
	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, length);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed\n");
		err = -EINVAL;
		goto exit;
	}

	/*  clear api to reset statistics, stats_clear_rsp_mask identifies what
	 * stats have been cleared stop_req = 1 will imply whether to stop the
	 * statistics collection. stop_rsp = 1 will imply that stop_req was
	 * honored and statistics collection was stopped.
	 */
	stats_clear_rsp_mask = WIFI_STATS_RADIO | WIFI_STATS_IFACE;
	stop_rsp = 1;
	if (nla_put_u32(skb, ATTR_LL_STATS_CLEAR_RSP_MASK,
			stats_clear_rsp_mask) ||
	    nla_put_u8(skb, ATTR_LL_STATS_STOP_RSP, stop_rsp)) {
		PRINTM(MERROR, "nla_put failed!\n");
		kfree_skb(skb);
		err = -EINVAL;
		goto exit;
	}

	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err)) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", err);
		goto exit;
		;
	}

exit:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	return err;
}

#ifdef STA_CFG80211
#define RSSI_MONOTOR_START 1
#define RSSI_MONOTOR_STOP 0

/**
 * @brief vendor command to control rssi monitor
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rssi_monitor(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int len)
{
	struct nlattr *tb[ATTR_RSSI_MONITOR_MAX + 1];
	moal_private *priv = (moal_private *)woal_get_netdev_priv(wdev->netdev);
	u32 rssi_monitor_control = 0x0;
	s8 rssi_min = 0, rssi_max = 0;
	int err = 0;
	t_u8 *pos = NULL;
	struct sk_buff *skb = NULL;
	int ret = 0;

	ENTER();

	if (!priv->media_connected) {
		ret = -EINVAL;
		goto done;
	}

	ret = nla_parse(tb, ATTR_RSSI_MONITOR_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (ret)
		goto done;

	if (!tb[ATTR_RSSI_MONITOR_CONTROL]) {
		ret = -EINVAL;
		goto done;
	}
	rssi_monitor_control = nla_get_u32(tb[ATTR_RSSI_MONITOR_CONTROL]);

	if (rssi_monitor_control == RSSI_MONOTOR_START) {
		if ((!tb[ATTR_RSSI_MONITOR_MIN_RSSI]) ||
		    (!tb[ATTR_RSSI_MONITOR_MAX_RSSI])) {
			ret = -EINVAL;
			goto done;
		}

		rssi_min = nla_get_s8(tb[ATTR_RSSI_MONITOR_MIN_RSSI]);
		rssi_max = nla_get_s8(tb[ATTR_RSSI_MONITOR_MAX_RSSI]);

		PRINTM(MEVENT,
		       "start rssi monitor rssi_min = %d, rssi_max= %d\n",
		       rssi_min, rssi_max);

		/* set rssi low/high threshold */
		priv->cqm_rssi_high_thold = rssi_max;
		priv->cqm_rssi_thold = rssi_min;
		priv->cqm_rssi_hyst = 4;
		if (MLAN_STATUS_SUCCESS !=
		    woal_set_rssi_threshold(priv, 0, MOAL_IOCTL_WAIT)) {
			PRINTM(MERROR, "set rssi threhold fail\n");
			ret = -EFAULT;
			goto done;
		}
	} else if (rssi_monitor_control == RSSI_MONOTOR_STOP) {
		/* stop rssi monitor */
		PRINTM(MEVENT, "stop rssi monitor\n");
		/* set both rssi_thold/hyst to 0, will trigger subscribe event
		 * clear
		 */
		priv->cqm_rssi_high_thold = 0;
		priv->cqm_rssi_thold = 0;
		priv->cqm_rssi_hyst = 0;
		if (MLAN_STATUS_SUCCESS !=
		    woal_set_rssi_threshold(priv, 0, MOAL_IOCTL_WAIT)) {
			PRINTM(MERROR, "set rssi threhold fail\n");
			ret = -EFAULT;
			goto done;
		}
	} else {
		PRINTM(MERROR, "invalid rssi_monitor control request\n");
		ret = -EINVAL;
		goto done;
	}

	/* Alloc the SKB for cmd reply */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed\n");
		ret = -EINVAL;
		goto done;
	}
	pos = skb_put(skb, len);
	moal_memcpy_ext(priv->phandle, pos, data, len, len);
	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err)) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", err);
		ret = err;
	}
done:
	LEAVE();
	return ret;
}

/**
 * @brief send rssi event to kernel
 *
 * @param priv       A pointer to moal_private
 * @param rssi       current rssi value
 *
 * @return      N/A
 */
void woal_cfg80211_rssi_monitor_event(moal_private *priv, t_s16 rssi)
{
	struct sk_buff *skb = NULL;
	t_s8 rssi_value = 0;

	ENTER();

	skb = dev_alloc_skb(NLA_HDRLEN * 2 + ETH_ALEN + sizeof(t_s8));
	if (!skb)
		goto done;
	/* convert t_s16 to t_s8*/
	rssi_value = -abs(rssi);
	if (nla_put(skb, ATTR_RSSI_MONITOR_CUR_BSSID, ETH_ALEN,
		    priv->conn_bssid) ||
	    nla_put_s8(skb, ATTR_RSSI_MONITOR_CUR_RSSI, rssi_value)) {
		PRINTM(MERROR, "nla_put failed!\n");
		dev_kfree_skb(skb);
		goto done;
	}
	woal_cfg80211_vendor_event(priv, event_rssi_monitor, (t_u8 *)skb->data,
				   skb->len);
	dev_kfree_skb(skb);
done:
	LEAVE();
}

/**
 * @brief send driver hang vendor event to kernel
 *
 * @param priv          A pointer to moal_private
 * @param reload_mode   reload mode
 *
 * @return      N/A
 */
void woal_cfg80211_driver_hang_event(moal_private *priv, t_u8 reload_mode)
{
	struct sk_buff *skb = NULL;

	ENTER();

	skb = dev_alloc_skb(NLA_HDRLEN + sizeof(t_u8));
	if (!skb) {
		PRINTM(MERROR,
		       "woal_cfg80211_driver_hang_event: Failed to allocate skb");
		goto done;
	}

	if (nla_put_u8(skb, ATTR_FW_RELOAD_MODE, reload_mode)) {
		PRINTM(MERROR,
		       "woal_cfg80211_driver_hang_event: nla_put failed!\n");
		dev_kfree_skb(skb);
		goto done;
	}

	PRINTM(MMSG,
	       "woal_cfg80211_driver_hang_event: Send event_hang with reload mode: %d",
	       reload_mode);
	woal_cfg80211_vendor_event(priv, event_hang, (t_u8 *)skb->data,
				   skb->len);

	dev_kfree_skb(skb);
done:
	LEAVE();
}

#endif // STA_CFG80211

/**
 * @brief vendor command to key_mgmt_set_key
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  fail otherwise
 */
static int
woal_cfg80211_subcmd_set_roaming_offload_key(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int data_len)
{
	moal_private *priv;
	struct net_device *dev;
	struct sk_buff *skb = NULL;
	const t_u8 *pos = (const t_u8 *)data;
	t_u8 *skb_pos = NULL;
	int ret = MLAN_STATUS_SUCCESS;

	ENTER();

	if (data)
		DBG_HEXDUMP(MCMD_D, "Vendor pmk", (const t_u8 *)data, data_len);

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}

	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	if (!priv || !pos) {
		LEAVE();
		return -EFAULT;
	}

	if (data_len > MLAN_MAX_KEY_LENGTH) {
		moal_memcpy_ext(priv->phandle, &priv->pmk.pmk_r0, pos,
				MLAN_MAX_KEY_LENGTH, MLAN_MAX_KEY_LENGTH);
		pos += MLAN_MAX_KEY_LENGTH;
		moal_memcpy_ext(priv->phandle, &priv->pmk.pmk_r0_name, pos,
				data_len - MLAN_MAX_KEY_LENGTH,
				MLAN_MAX_PMKR0_NAME_LENGTH);
	} else {
		moal_memcpy_ext(priv->phandle, &priv->pmk.pmk, data, data_len,
				MLAN_MAX_KEY_LENGTH);
	}
	priv->pmk_saved = MTRUE;

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, data_len);
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		LEAVE();
		return -EFAULT;
	}
	skb_pos = skb_put(skb, data_len);
	moal_memcpy_ext(priv->phandle, skb_pos, data, data_len, data_len);
	ret = cfg80211_vendor_cmd_reply(skb);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to supplicant to update AP info
 *
 * @param priv     A pointer to moal_private
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
int woal_roam_ap_info(moal_private *priv, t_u8 *data, int len)
{
	struct wiphy *wiphy = priv->wdev->wiphy;
	struct sk_buff *skb = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	key_info *pkey = NULL;
	apinfo *pinfo = NULL;
	apinfo *req_tlv = NULL;
	MrvlIEtypesHeader_t *tlv = NULL;
	t_u16 tlv_type = 0, tlv_len = 0, tlv_buf_left = 0;
	int event_id = 0;
	t_u8 authorized = 1;

	ENTER();

	event_id = woal_get_event_id(event_fw_roam_success);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = 1;
		LEAVE();
		return ret;
	}
	/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev, len + 50,
#else
	skb = cfg80211_vendor_event_alloc(wiphy, len + 50,
#endif
					  event_id, GFP_ATOMIC);

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = 1;
		LEAVE();
		return ret;
	}

	nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_BSSID,
		MLAN_MAC_ADDR_LENGTH, (t_u8 *)data);
	nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_AUTHORIZED,
		sizeof(authorized), &authorized);
	tlv = (MrvlIEtypesHeader_t *)(data + MLAN_MAC_ADDR_LENGTH);
	tlv_buf_left = len - MLAN_MAC_ADDR_LENGTH;
	while (tlv_buf_left >= sizeof(MrvlIEtypesHeader_t)) {
		tlv_type = woal_le16_to_cpu(tlv->type);
		tlv_len = woal_le16_to_cpu(tlv->len);

		if (tlv_buf_left < (tlv_len + sizeof(MrvlIEtypesHeader_t))) {
			PRINTM(MERROR,
			       "Error processing firmware roam success TLVs, bytes left < TLV length\n");
			break;
		}

		switch (tlv_type) {
		case TLV_TYPE_APINFO:
			pinfo = (apinfo *)tlv;
			nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_RESP_IE,
				pinfo->header.len, pinfo->rsp_ie);
			break;
		case TLV_TYPE_ASSOC_REQ_IE:
			req_tlv = (apinfo *)tlv;
			nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_REQ_IE,
				req_tlv->header.len, req_tlv->rsp_ie);
			break;
		case TLV_TYPE_KEYINFO:
			pkey = (key_info *)tlv;
			nla_put(skb,
				MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_KEY_REPLAY_CTR,
				MLAN_REPLAY_CTR_LEN, pkey->key.replay_ctr);
			nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_PTK_KCK,
				MLAN_KCK_LEN, pkey->key.kck);
			nla_put(skb, MRVL_WLAN_VENDOR_ATTR_ROAM_AUTH_PTK_KEK,
				MLAN_KEK_LEN, pkey->key.kek);
			break;
		default:
			break;
		}
		tlv_buf_left -= tlv_len + sizeof(MrvlIEtypesHeader_t);
		tlv = (MrvlIEtypesHeader_t *)((t_u8 *)tlv + tlv_len +
					      sizeof(MrvlIEtypesHeader_t));
	}

	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to enable/disable 11k
 *
 * @param wiphy         A pointer to wiphy struct
 * @param wdev          A pointer to wireless_dev struct
 * @param data           a pointer to data
 * @param data_len     data length
 *
 * @return      0: success  <0: fail
 */
static int woal_cfg80211_subcmd_11k_cfg(struct wiphy *wiphy,
					struct wireless_dev *wdev,
					const void *data, int data_len)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	struct nlattr *tb_vendor[ATTR_ND_OFFLOAD_MAX + 1];
	int ret = 0;
	int status = MLAN_STATUS_SUCCESS;

	ENTER();
	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}

	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	nla_parse(tb_vendor, ATTR_ND_OFFLOAD_MAX, (const struct nlattr *)data,
		  data_len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
		  ,
		  NULL
#endif
	);
	if (!tb_vendor[ATTR_ND_OFFLOAD_CONTROL]) {
		PRINTM(MINFO, "%s: ATTR_ND_OFFLOAD not found\n", __func__);
		ret = -EFAULT;
		goto done;
	}
done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to set scan mac oui
 *
 * @param wiphy         A pointer to wiphy struct
 * @param wdev          A pointer to wireless_dev struct
 * @param data           a pointer to data
 * @param data_len     data length
 *
 * @return      0: success  <0: fail
 */
static int woal_cfg80211_subcmd_set_scan_mac_oui(struct wiphy *wiphy,
						 struct wireless_dev *wdev,
						 const void *data, int data_len)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	struct nlattr *tb_vendor[ATTR_WIFI_MAX + 1];
	t_u8 mac_oui[3] = {0};
	int ret = MLAN_STATUS_SUCCESS;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	nla_parse(tb_vendor, ATTR_WIFI_MAX, (const struct nlattr *)data,
		  data_len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
		  ,
		  NULL
#endif
	);
	if (!tb_vendor[ATTR_SCAN_MAC_OUI_SET]) {
		PRINTM(MINFO, "%s: ATTR_SCAN_MAC_OUI_SET not found\n",
		       __func__);
		ret = -EFAULT;
		goto done;
	}
	moal_memcpy_ext(priv->phandle, mac_oui,
			nla_data(tb_vendor[ATTR_SCAN_MAC_OUI_SET]), 3, 3);
	moal_memcpy_ext(priv->phandle, priv->random_mac, priv->current_addr,
			ETH_ALEN, MLAN_MAC_ADDR_LENGTH);
	moal_memcpy_ext(priv->phandle, priv->random_mac, mac_oui, 3,
			MLAN_MAC_ADDR_LENGTH);
	PRINTM(MCMND, "random_mac is " FULL_MACSTR "\n",
	       FULL_MAC2STR(priv->random_mac));
done:
	LEAVE();
	return ret;
}
/**
 * @brief vendor command to start
 *     woal_cfg80211_subcmd_secure_ranging_ctx
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  fail otherwise
 */
static int woal_cfg80211_subcmd_secure_ranging_ctx(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	moal_private *priv;
	struct net_device *dev;
	mlan_ioctl_req *req = NULL;
	mlan_ds_sec_cfg *sec = NULL;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int type, rem;
	t_u32 action = 0;
	int key_len = 0;
	int peer_addr_set = 0;
	t_u8 peer_addr[ETH_ALEN] = {0};
	t_u8 own_addr[ETH_ALEN];
	t_u32 cipher = 0;
	t_u32 sha_type = 0;
	t_u8 key[MLAN_MAX_KEY_LENGTH] = {0};
	t_u8 *ltf_keyseed = NULL;
	t_u8 bcast_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	const struct nlattr *iter;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}

	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	if (!priv) {
		LEAVE();
		return -EFAULT;
	}

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_ACTION:
			action = nla_get_u32(iter);
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_SRC_ADDR:
			moal_memcpy_ext(priv->phandle, own_addr, nla_data(iter),
					nla_len(iter), ETH_ALEN);
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_PEER_MAC_ADDR:
			moal_memcpy_ext(priv->phandle, peer_addr,
					nla_data(iter), nla_len(iter),
					ETH_ALEN);
			peer_addr_set = 1;
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_SHA_TYPE:
			sha_type = nla_get_u32(iter);
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_TK:
			moal_memcpy_ext(priv->phandle, key, nla_data(iter),
					nla_len(iter), MLAN_MAX_KEY_LENGTH);
			key_len = nla_len(iter);
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_CIPHER:
			cipher = nla_get_u32(iter);
			break;
		case MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_LTF_KEYSEED:
			moal_memcpy_ext(priv->phandle, ltf_keyseed,
					nla_data(iter), nla_len(iter), 48);
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_sec_cfg));
	if (req == NULL) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill request buffer */
	sec = (mlan_ds_sec_cfg *)req->pbuf;
	sec->sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;
	req->req_id = MLAN_IOCTL_SEC_CFG;
	if (action == MLAN_ACT_PASN_SET_KEY) {
		req->action = MLAN_ACT_PASN_KEY_DNLD;
		if (key_len) {
			moal_memcpy_ext(priv->phandle,
					sec->param.encrypt_key.key_material,
					key, key_len, MLAN_MAX_KEY_LENGTH);
			sec->param.encrypt_key.key_len = key_len;
		}
		if (peer_addr_set) {
			moal_memcpy_ext(priv->phandle,
					sec->param.encrypt_key.mac_addr,
					peer_addr, ETH_ALEN,
					MLAN_MAC_ADDR_LENGTH);
			if (memcmp(sec->param.encrypt_key.mac_addr, bcast_addr,
				   ETH_ALEN) == 0)
				sec->param.encrypt_key.key_flags =
					KEY_FLAG_GROUP_KEY;
			else
				sec->param.encrypt_key.key_flags =
					KEY_FLAG_SET_TX_KEY;
		} else {
			moal_memcpy_ext(priv->phandle,
					sec->param.encrypt_key.mac_addr,
					bcast_addr, ETH_ALEN,
					MLAN_MAC_ADDR_LENGTH);
			sec->param.encrypt_key.key_flags =
				KEY_FLAG_GROUP_KEY | KEY_FLAG_SET_TX_KEY;
		}

#if KERNEL_VERSION(3, 6, 0) <= CFG80211_VERSION_CODE
		if (cipher == WLAN_CIPHER_SUITE_GCMP)
			sec->param.encrypt_key.key_flags |= KEY_FLAG_GCMP;
#endif
#if KERNEL_VERSION(4, 0, 0) <= CFG80211_VERSION_CODE
		else if (cipher == WLAN_CIPHER_SUITE_GCMP_256)
			sec->param.encrypt_key.key_flags |= KEY_FLAG_GCMP_256;
#endif
#if KERNEL_VERSION(4, 0, 0) <= CFG80211_VERSION_CODE
		if (cipher == WLAN_CIPHER_SUITE_CCMP_256)
			sec->param.encrypt_key.key_flags |= KEY_FLAG_CCMP_256;
#endif

		if (cipher == WLAN_CIPHER_SUITE_AES_CMAC
#if KERNEL_VERSION(4, 0, 0) <= CFG80211_VERSION_CODE
		    || cipher == WLAN_CIPHER_SUITE_BIP_GMAC_128 ||
		    cipher == WLAN_CIPHER_SUITE_BIP_GMAC_256
#endif
		) {
			sec->param.encrypt_key.key_flags |=
				KEY_FLAG_AES_MCAST_IGTK;

#if KERNEL_VERSION(4, 0, 0) <= CFG80211_VERSION_CODE
			if (cipher == WLAN_CIPHER_SUITE_BIP_GMAC_128)
				sec->param.encrypt_key.key_flags |=
					KEY_FLAG_GMAC_128;
			else if (cipher == WLAN_CIPHER_SUITE_BIP_GMAC_256)
				sec->param.encrypt_key.key_flags |=
					KEY_FLAG_GMAC_256;
#endif
		}
	} else {
		req->action = MLAN_ACT_CLEAR;
		sec->param.encrypt_key.key_len = MLAN_MAX_KEY_LENGTH;
		sec->param.encrypt_key.key_index = MLAN_KEY_INDEX_UNICAST;
		sec->param.encrypt_key.key_flags = KEY_FLAG_REMOVE_KEY;
		moal_memcpy_ext(priv->phandle, sec->param.encrypt_key.mac_addr,
				(u8 *)peer_addr, ETH_ALEN,
				MLAN_MAC_ADDR_LENGTH);
		memset(sec->param.encrypt_key.key_material, 0,
		       sizeof(sec->param.encrypt_key.key_material));
	}
	/* Send IOCTL request to MLAN */
	ret = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

done:
	if (ret != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return ret;
}
/**
 * @brief vendor command to
 *
 * @param wiphy         A pointer to wiphy struct
 * @param wdev          A pointer to wireless_dev struct
 * @param data           a pointer to data
 * @param data_len     data length
 *
 * @return      0: success  <0: fail
 */
static int woal_cfg80211_subcmd_set_scan_band(struct wiphy *wiphy,
					      struct wireless_dev *wdev,
					      const void *data, int data_len)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	struct nlattr *tb_vendor[ATTR_WIFI_MAX + 1];
	int ret = MLAN_STATUS_SUCCESS;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	nla_parse(tb_vendor, ATTR_WIFI_MAX, (const struct nlattr *)data,
		  data_len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
		  ,
		  NULL
#endif
	);
	if (!tb_vendor[ATTR_SCAN_BAND_SET]) {
		PRINTM(MERROR, "%s: ATTR_SCAN_BAND_SET not found\n", __func__);
		ret = -EFAULT;
		goto done;
	}
	priv->scan_setband_mask =
		*(u8 *)nla_data(tb_vendor[ATTR_SCAN_BAND_SET]);
	PRINTM(MMSG,
	       "woal_cfg80211_subcmd_set_scan_band: scan_setband_mask :%d",
	       priv->scan_setband_mask);

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to start keep alive
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  fail otherwise
 */
static int woal_cfg80211_subcmd_start_keep_alive(struct wiphy *wiphy,
						 struct wireless_dev *wdev,
						 const void *data, int len)
{
	moal_private *priv;
	struct net_device *dev;
	int ret = MLAN_STATUS_SUCCESS;
	int type, rem;
	t_u8 mkeep_alive_id = 0;
	t_u16 ether_type = 0;
	t_u8 *ip_pkt = NULL;
	t_u16 ip_pkt_len = 0;
	t_u8 src_mac[ETH_ALEN];
	t_u8 dst_mac[ETH_ALEN];
	t_u32 period_msec = 0;
	t_u32 retry_interval = 0;
	t_u8 retry_cnt = 0;
	const struct nlattr *iter;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}

	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	if (!priv) {
		LEAVE();
		return -EFAULT;
	}

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case MKEEP_ALIVE_ATTRIBUTE_ID:
			mkeep_alive_id = nla_get_u8(iter);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_ETHER_TYPE:
			ether_type = nla_get_u16(iter);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_IP_PKT_LEN:
			ip_pkt_len = nla_get_u16(iter);
			if (ip_pkt_len > MKEEP_ALIVE_IP_PKT_MAX) {
				ret = -EINVAL;
				goto exit;
			}
			break;
		case MKEEP_ALIVE_ATTRIBUTE_IP_PKT:
			if (ip_pkt_len) {
				if (ip_pkt) {
					kfree(ip_pkt);
					ip_pkt = NULL;
				}
				ip_pkt = (u8 *)kzalloc(ip_pkt_len, GFP_ATOMIC);
				if (ip_pkt == NULL) {
					ret = -ENOMEM;
					PRINTM(MERROR,
					       "Failed to allocate mem for ip packet\n");
					goto exit;
				}
				moal_memcpy_ext(priv->phandle, ip_pkt,
						(u8 *)nla_data(iter),
						nla_len(iter), ip_pkt_len);
			}
			break;
		case MKEEP_ALIVE_ATTRIBUTE_SRC_MAC_ADDR:
			moal_memcpy_ext(priv->phandle, src_mac, nla_data(iter),
					nla_len(iter), ETH_ALEN);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_DST_MAC_ADDR:
			moal_memcpy_ext(priv->phandle, dst_mac, nla_data(iter),
					nla_len(iter), ETH_ALEN);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_PERIOD_MSEC:
			period_msec = nla_get_u32(iter);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_RETRY_INTERVAL:
			retry_interval = nla_get_u32(iter);
			break;
		case MKEEP_ALIVE_ATTRIBUTE_RETRY_CNT:
			retry_cnt = nla_get_u8(iter);
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
			goto exit;
		}
	}

	ret = woal_priv_save_cloud_keep_alive_params(
		priv, mkeep_alive_id, true, ether_type, ip_pkt, ip_pkt_len,
		src_mac, dst_mac, period_msec, retry_interval, retry_cnt);
	if (ret < 0)
		PRINTM(MERROR, "start_mkeep_alive is failed ret: %d\n", ret);

exit:
	if (ip_pkt)
		kfree(ip_pkt);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to stop keep alive
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  fail otherwise
 */
static int woal_cfg80211_subcmd_stop_keep_alive(struct wiphy *wiphy,
						struct wireless_dev *wdev,
						const void *data, int len)
{
	moal_private *priv;
	struct net_device *dev;
	int ret = MLAN_STATUS_SUCCESS;
	int type, rem;
	t_u8 mkeep_alive_id = 0;
	const struct nlattr *iter;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}

	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);
	if (!priv) {
		LEAVE();
		return -EFAULT;
	}

	nla_for_each_attr (iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
		case MKEEP_ALIVE_ATTRIBUTE_ID:
			mkeep_alive_id = nla_get_u8(iter);
			break;
		default:
			PRINTM(MERROR, "Unknown type: %d\n", type);
			ret = -EINVAL;
			break;
		}
	}

	ret = woal_stop_mkeep_alive(priv, mkeep_alive_id, 0, NULL, NULL);
	if (ret < 0)
		PRINTM(MERROR, "stop_mkeep_alive is failed ret: %d\n", ret);

	LEAVE();
	return ret;
}

/**
 * @brief               Upload last keep alive packet to Host through vendor
 event
 *
 * @param priv          Pointer to moal_private structure
 * @param mkeep_alive   Pointer to mlan_ds_misc_keep_alive structure

 * @return      0: success  fail otherwise
 */
int woal_mkeep_alive_vendor_event(moal_private *priv,
				  mlan_ds_misc_keep_alive *mkeep_alive)
{
	struct wiphy *wiphy = priv->wdev->wiphy;
	struct sk_buff *skb = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	int event_id = 0;
	t_u16 len = 0;

	ENTER();

	event_id = woal_get_event_id(event_cloud_keep_alive);
	if (event_max == event_id) {
		PRINTM(MERROR, "Not find this event %d\n", event_id);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (!mkeep_alive) {
		PRINTM(MERROR, "Parameter is NULL\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	len = mkeep_alive->pkt_len;

	/**allocate skb*/
#if KERNEL_VERSION(4, 1, 0) <= CFG80211_VERSION_CODE
	skb = cfg80211_vendor_event_alloc(wiphy, priv->wdev, len + 50,
#else
	skb = cfg80211_vendor_event_alloc(wiphy, len + 50,
#endif
					  event_id, GFP_ATOMIC);

	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor event\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	nla_put(skb, MKEEP_ALIVE_ATTRIBUTE_IP_PKT_LEN, sizeof(t_u16), &len);
	nla_put(skb, MKEEP_ALIVE_ATTRIBUTE_IP_PKT, len, mkeep_alive->packet);

	/**send event*/
	cfg80211_vendor_event(skb, GFP_ATOMIC);

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to set enable/disable dfs offload
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  1: fail
 */
static int woal_cfg80211_subcmd_set_dfs_offload(struct wiphy *wiphy,
						struct wireless_dev *wdev,
						const void *data, int data_len)
{
	struct sk_buff *skb = NULL;
	moal_handle *handle = (moal_handle *)woal_get_wiphy_priv(wiphy);
	int dfs_offload;
	int ret = 1;

	ENTER();
	dfs_offload = moal_extflg_isset(handle, EXT_DFS_OFFLOAD);

	/** Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(dfs_offload));
	if (!skb) {
		PRINTM(MERROR, "allocate memory fail for vendor cmd\n");
		ret = 1;
		LEAVE();
		return ret;
	}
	nla_put(skb, MRVL_WLAN_VENDOR_ATTR_DFS, sizeof(t_u32), &dfs_offload);
	ret = cfg80211_vendor_cmd_reply(skb);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get rtt capability
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_get_capa(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	moal_handle *handle = priv->phandle;
	struct sk_buff *skb = NULL;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "CfgVendor: cfg80211_subcmd_rtt_get_capa\n");

	// Casting is done to read the value
	// coverity[misra_c_2012_rule_11_8_violation:SUPPRESS]
	DBG_HEXDUMP(MCMD_D, "input data", (t_u8 *)data, len);

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, nla_total_size(sizeof(handle->rtt_capa)) +
			       VENDOR_REPLY_OVERHEAD);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed in %s\n", __func__);
		goto done;
	}

	/* Put the attribute to the skb */
	nla_put(skb, ATTR_RTT_CAPA, sizeof(handle->rtt_capa),
		&(handle->rtt_capa));

	PRINTM(MCMND, "NL80211_CMD_VENDOR=0x%x\n", NL80211_CMD_VENDOR);
	PRINTM(MCMND, "NL80211_ATTR_WIPHY=0x%x\n", NL80211_ATTR_WIPHY);
	PRINTM(MCMND, "NL80211_ATTR_VENDOR_ID=0x%x\n", NL80211_ATTR_VENDOR_ID);
	PRINTM(MCMND, "NL80211_ATTR_VENDOR_SUBCMD=0x%x\n",
	       NL80211_ATTR_VENDOR_SUBCMD);
	PRINTM(MCMND, "NL80211_ATTR_VENDOR_DATA=0x%x\n",
	       NL80211_ATTR_VENDOR_DATA);
	PRINTM(MCMND, "NL80211_ATTR_VENDOR_EVENTS=0x%x\n",
	       NL80211_ATTR_VENDOR_EVENTS);

	DBG_HEXDUMP(MCMD_D, "output data skb->head", (t_u8 *)skb->head, 50);
	DBG_HEXDUMP(MCMD_D, "output data skb->data", (t_u8 *)skb->data, 50);
	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err))
		PRINTM(MERROR, "Vendor Command reply failed err:%d\n", err);

done:
	LEAVE();
	return err;
}

static void woal_dump_rtt_params(wifi_rtt_config_params_t *rtt_params)
{
	int i = 0;

	PRINTM(MMSG, "===== Start DUMP RTT Params =====\n");
	PRINTM(MMSG, "rtt_config_num=%d\n\n", rtt_params->rtt_config_num);

	for (i = 0; i < rtt_params->rtt_config_num; i++) {
		PRINTM(MMSG, "----------[%d]----------\n", i);
		PRINTM(MMSG, "rtt_config[%d].addr=" MACSTR "\n", i,
		       MAC2STR(rtt_params->rtt_config[i].addr));
		PRINTM(MMSG, "rtt_config[%d].type=%d\n", i,
		       rtt_params->rtt_config[i].type);
		PRINTM(MMSG, "rtt_config[%d].peer=%d\n", i,
		       rtt_params->rtt_config[i].peer);
		PRINTM(MMSG, "rtt_config[%d].channel=[%d %d %d %d]\n", i,
		       rtt_params->rtt_config[i].channel.width,
		       rtt_params->rtt_config[i].channel.center_freq,
		       rtt_params->rtt_config[i].channel.center_freq0,
		       rtt_params->rtt_config[i].channel.center_freq1);
		PRINTM(MMSG, "rtt_config[%d].burst_period=%d\n", i,
		       rtt_params->rtt_config[i].burst_period);
		PRINTM(MMSG, "rtt_config[%d].num_burst=%d\n", i,
		       rtt_params->rtt_config[i].num_burst);
		PRINTM(MMSG, "rtt_config[%d].num_frames_per_burst=%d\n", i,
		       rtt_params->rtt_config[i].num_frames_per_burst);
		PRINTM(MMSG, "rtt_config[%d].num_retries_per_rtt_frame=%d\n", i,
		       rtt_params->rtt_config[i].num_retries_per_rtt_frame);
		PRINTM(MMSG, "rtt_config[%d].num_retries_per_ftmr=%d\n", i,
		       rtt_params->rtt_config[i].num_retries_per_ftmr);
		PRINTM(MMSG, "rtt_config[%d].LCI_request=%d\n", i,
		       rtt_params->rtt_config[i].LCI_request);
		PRINTM(MMSG, "rtt_config[%d].LCR_request=%d\n", i,
		       rtt_params->rtt_config[i].LCR_request);
		PRINTM(MMSG, "rtt_config[%d].burst_duration=%d\n", i,
		       rtt_params->rtt_config[i].burst_duration);
		PRINTM(MMSG, "rtt_config[%d].preamble=%d\n", i,
		       rtt_params->rtt_config[i].preamble);
		PRINTM(MMSG, "rtt_config[%d].bw=%d\n", i,
		       rtt_params->rtt_config[i].bw);
		PRINTM(MMSG, "\n");
	}
}

/**
 * @brief vendor command to request rtt range
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_range_request(struct wiphy *wiphy,
						  struct wireless_dev *wdev,
						  const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	moal_handle *handle = priv->phandle;
	struct nlattr *tb[ATTR_RTT_MAX + 1];
	t_u8 zero_mac[MLAN_MAC_ADDR_LENGTH] = {0};
	t_u8 rtt_config_num = 0;
	wifi_rtt_config *rtt_config = NULL;
	t_u8 i = 0, j = 0;
	wifi_rtt_config_params_t rtt_params;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_RTT_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		err = -EFAULT;
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		goto done;
	}

	if (!tb[ATTR_RTT_TARGET_NUM] || !tb[ATTR_RTT_TARGET_CONFIG]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_RTT_TARGET_NUM]=%p tb[ATTR_RTT_TARGET_CONFIG]=%p\n",
		       __func__, tb[ATTR_RTT_TARGET_NUM],
		       tb[ATTR_RTT_TARGET_CONFIG]);
		err = -EINVAL;
		goto done;
	}

	rtt_config_num = nla_get_u8(tb[ATTR_RTT_TARGET_NUM]);

	if ((rtt_config_num == 0) || ((handle->rtt_params.rtt_config_num +
				       rtt_config_num) > MAX_RTT_CONFIG_NUM)) {
		PRINTM(MERROR, "%s: invalid num=%d  num in handle=%d  MAX=%d\n",
		       __func__, rtt_config_num,
		       handle->rtt_params.rtt_config_num, MAX_RTT_CONFIG_NUM);
		err = -EINVAL;
		goto done;
	}
	if (nla_len(tb[ATTR_RTT_TARGET_CONFIG]) !=
	    sizeof(rtt_params.rtt_config[0]) * rtt_config_num) {
		PRINTM(MERROR, "%s: invalid %d(total) != %d(num) * %u(each)\n",
		       __func__, nla_len(tb[ATTR_RTT_TARGET_CONFIG]),
		       rtt_config_num, (t_u32)sizeof(rtt_params.rtt_config[0]));
		err = -EINVAL;
		goto done;
	}

	rtt_config = (wifi_rtt_config *)nla_data(tb[ATTR_RTT_TARGET_CONFIG]);
	memset(&rtt_params, 0, sizeof(rtt_params));
	/** Strip the zero mac config */
	for (i = 0; i < rtt_config_num; i++) {
		if (!memcmp(rtt_config[i].addr, zero_mac,
			    sizeof(rtt_config[i].addr)))
			continue;
		else {
			moal_memcpy_ext(
				handle,
				&rtt_params
					 .rtt_config[rtt_params.rtt_config_num],
				&rtt_config[i],
				sizeof(rtt_params.rtt_config
					       [rtt_params.rtt_config_num]),
				sizeof(wifi_rtt_config));
			rtt_params.rtt_config_num++;
		}
	}
	if (!rtt_params.rtt_config_num) {
		PRINTM(MERROR, "%s: no valid mac addr\n", __func__);
		goto done;
	}
	woal_dump_rtt_params(&rtt_params);

	ret = woal_config_rtt(priv, MOAL_IOCTL_WAIT, &rtt_params);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_config_rtt() failed\n", __func__);
		err = -EFAULT;
		goto done;
	}

	for (i = 0; i < rtt_params.rtt_config_num; i++) {
		for (j = 0; j < handle->rtt_params.rtt_config_num; j++) {
			if (!memcmp(handle->rtt_params.rtt_config[j].addr,
				    rtt_params.rtt_config[i].addr,
				    sizeof(handle->rtt_params.rtt_config[j]
						   .addr)))
				break;
		}
		moal_memcpy_ext(handle, &(handle->rtt_params.rtt_config[j]),
				&(rtt_params.rtt_config[i]),
				sizeof(handle->rtt_params.rtt_config[j]),
				sizeof(wifi_rtt_config));
		if (j == handle->rtt_params.rtt_config_num)
			handle->rtt_params.rtt_config_num++;
	}

	woal_dump_rtt_params(&(handle->rtt_params));

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to cancel rtt range
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_range_cancel(struct wiphy *wiphy,
						 struct wireless_dev *wdev,
						 const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	moal_handle *handle = priv->phandle;
	t_u8 rtt_config_num = handle->rtt_params.rtt_config_num;
	struct nlattr *tb[ATTR_RTT_MAX + 1];
	t_u32 target_num = 0;
	t_u8 addr[MAX_RTT_CONFIG_NUM][MLAN_MAC_ADDR_LENGTH] = {0x00};
	int i = 0, j = 0;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_RTT_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		goto done;
	}

	if (!tb[ATTR_RTT_TARGET_NUM] || !tb[ATTR_RTT_TARGET_ADDR]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_RTT_TARGET_NUM]=%p tb[ATTR_RTT_TARGET_ADDR]=%p\n",
		       __func__, tb[ATTR_RTT_TARGET_NUM],
		       tb[ATTR_RTT_TARGET_ADDR]);
		err = -EINVAL;
		goto done;
	}

	target_num = nla_get_u8(tb[ATTR_RTT_TARGET_NUM]);

	if ((target_num <= 0 || target_num > MAX_RTT_CONFIG_NUM) ||
	    (nla_len(tb[ATTR_RTT_TARGET_ADDR]) !=
	     sizeof(t_u8) * MLAN_MAC_ADDR_LENGTH * target_num)) {
		PRINTM(MERROR, "%s: Check if %din[1-%d] or %d*%u=%d\n",
		       __func__, target_num, MAX_RTT_CONFIG_NUM, target_num,
		       (t_u32)(sizeof(t_u8) * MLAN_MAC_ADDR_LENGTH),
		       nla_len(tb[ATTR_RTT_TARGET_ADDR]));
		err = -EINVAL;
		goto done;
	}
	woal_dump_rtt_params(&(handle->rtt_params));

	moal_memcpy_ext(handle, addr, nla_data(tb[ATTR_RTT_TARGET_ADDR]),
			nla_len(tb[ATTR_RTT_TARGET_ADDR]), sizeof(addr));

	for (i = 0; i < target_num; i++)
		PRINTM(MMSG, "cancel[%d].addr=" MACSTR "\n", i,
		       MAC2STR(addr[i]));

	for (i = 0; i < target_num; i++) {
		for (j = 0; j < handle->rtt_params.rtt_config_num; j++) {
			if (!memcmp(addr[i],
				    handle->rtt_params.rtt_config[j].addr,
				    sizeof(addr[0]))) {
				memset(&(handle->rtt_params.rtt_config[j]),
				       0x00,
				       sizeof(handle->rtt_params.rtt_config[0]));
				if ((j + 1) <
				    handle->rtt_params.rtt_config_num) {
					memmove(&(handle->rtt_params
							  .rtt_config[j]),
						&(handle->rtt_params
							  .rtt_config[j + 1]),
						sizeof(handle->rtt_params
							       .rtt_config[0]) *
							(handle->rtt_params
								 .rtt_config_num -
							 (j + 1)));
					memset(&(handle->rtt_params.rtt_config
							 [handle->rtt_params
								  .rtt_config_num -
							  1]),
					       0x00,
					       sizeof(handle->rtt_params
							      .rtt_config[0]));
				}
				handle->rtt_params.rtt_config_num--;
				continue;
			}
		}
	}

	if (handle->rtt_params.rtt_config_num >= rtt_config_num) {
		PRINTM(MERROR, "%s: No matched mac addr in rtt_config\n",
		       __func__);
		goto done;
	}

	ret = woal_cancel_rtt(priv, MOAL_IOCTL_WAIT, target_num, addr);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_cancel_rtt() failed\n", __func__);
		err = -EFAULT;
		goto done;
	}
	woal_dump_rtt_params(&(handle->rtt_params));

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor event to report RTT Results
 *
 * @param priv     A pointer to moal_private
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      mlan_status
 */
mlan_status woal_cfg80211_event_rtt_result(moal_private *priv, t_u8 *data,
					   int len)
{
	// moal_handle *handle = priv->phandle;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	t_u8 *pos = data;
	t_u32 event_left_len = len;
	struct sk_buff *skb = NULL;
	t_u32 vdr_event_len = 0;
	t_u32 complete = 0;
	wifi_rtt_result_element *rtt_result_elem = NULL;
	t_u32 num_results = 0;

	ENTER();

	PRINTM(MEVENT, "Enter %s()\n", __func__);

	vdr_event_len = nla_total_size(sizeof(complete)) +
			nla_total_size(sizeof(num_results)) +
			nla_total_size(len) + NLA_ALIGNTO * num_results +
			VENDOR_REPLY_OVERHEAD;
	PRINTM(MEVENT, "vdr_event_len = %d\n", vdr_event_len);
	skb = woal_cfg80211_alloc_vendor_event(priv, event_rtt_result,
					       vdr_event_len);
	if (!skb)
		goto done;

	complete = *pos;
	nla_put(skb, ATTR_RTT_RESULT_COMPLETE, sizeof(complete), &complete);
	pos = (t_u8 *)(pos + 1);
	event_left_len--;

	while (event_left_len > sizeof(wifi_rtt_result_element)) {
		rtt_result_elem = (wifi_rtt_result_element *)pos;

		nla_put(skb, ATTR_RTT_RESULT_FULL, rtt_result_elem->len,
			rtt_result_elem->data);
		num_results++;

		pos += sizeof(*rtt_result_elem) + rtt_result_elem->len;
		event_left_len -=
			sizeof(*rtt_result_elem) + rtt_result_elem->len;
	}

	nla_put(skb, ATTR_RTT_RESULT_NUM, sizeof(num_results), &num_results);

	DBG_HEXDUMP(MEVT_D, "output data skb->data", (t_u8 *)skb->data,
		    skb->len);
	/**send event*/
	cfg80211_vendor_event(skb, GFP_KERNEL);

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get rtt responder info
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int
woal_cfg80211_subcmd_rtt_get_responder_info(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_rtt_responder rtt_rsp_cfg;
	struct sk_buff *skb = NULL;
	wifi_rtt_responder rtt_rsp;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	memset(&rtt_rsp_cfg, 0x00, sizeof(rtt_rsp_cfg));
	rtt_rsp_cfg.action = RTT_GET_RESPONDER_INFO;
	ret = woal_rtt_responder_cfg(priv, MOAL_IOCTL_WAIT, &rtt_rsp_cfg);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_rtt_responder_cfg() failed\n",
		       __func__);
		err = -EFAULT;
		goto done;
	}
	PRINTM(MCMD_D,
	       "mlan_rtt_responder from FW: channel=%d bandcfg=%d %d %d %d preamble=%d\n",
	       rtt_rsp_cfg.u.info.channel, rtt_rsp_cfg.u.info.bandcfg.chanBand,
	       rtt_rsp_cfg.u.info.bandcfg.chanWidth,
	       rtt_rsp_cfg.u.info.bandcfg.chan2Offset,
	       rtt_rsp_cfg.u.info.bandcfg.scanMode,
	       rtt_rsp_cfg.u.info.preamble);

	memset(&rtt_rsp, 0x00, sizeof(rtt_rsp));
	woal_bandcfg_to_channel_info(priv, &(rtt_rsp_cfg.u.info.bandcfg),
				     rtt_rsp_cfg.u.info.channel,
				     &(rtt_rsp.channel));
	rtt_rsp.preamble = rtt_rsp_cfg.u.info.preamble;
	PRINTM(MCMD_D, "wifi_rtt_responder report to HAL:\n");
	PRINTM(MCMD_D,
	       "channel: width=%d center_freq=%d center_freq0=%d center_freq1=%d\n",
	       rtt_rsp.channel.width, rtt_rsp.channel.center_freq,
	       rtt_rsp.channel.center_freq0, rtt_rsp.channel.center_freq1);
	PRINTM(MCMD_D, "preamble=%d\n", rtt_rsp.preamble);

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, nla_total_size(sizeof(rtt_rsp)) + VENDOR_REPLY_OVERHEAD);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed in %s\n", __func__);
		goto done;
	}

	/* Put the attribute to the skb */
	nla_put(skb, ATTR_RTT_CHANNEL_INFO, sizeof(rtt_rsp.channel),
		&(rtt_rsp.channel));
	nla_put(skb, ATTR_RTT_PREAMBLE, sizeof(rtt_rsp.preamble),
		&(rtt_rsp.preamble));
	DBG_HEXDUMP(MCMD_D, "output data skb->data", (t_u8 *)skb->data,
		    skb->len);

	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err))
		PRINTM(MERROR, "Vendor Command reply failed err:%d\n", err);

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to enable rtt responder
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_enable_responder(struct wiphy *wiphy,
						     struct wireless_dev *wdev,
						     const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct nlattr *tb[ATTR_RTT_MAX + 1];
	wifi_channel_info *ch_info = NULL;
	t_u32 max_dur_sec = 0;
	mlan_rtt_responder rtt_rsp_cfg;
	wifi_rtt_responder rtt_rsp;
	struct sk_buff *skb = NULL;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_RTT_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		err = -EFAULT;
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		goto done;
	}

	if (!tb[ATTR_RTT_CHANNEL_INFO] || !tb[ATTR_RTT_MAX_DUR_SEC]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_RTT_TARGET_NUM]=%p tb[ATTR_RTT_TARGET_CONFIG]=%p\n",
		       __func__, tb[ATTR_RTT_CHANNEL_INFO],
		       tb[ATTR_RTT_MAX_DUR_SEC]);
		err = -EINVAL;
		goto done;
	}
	ch_info = (wifi_channel_info *)nla_data(tb[ATTR_RTT_CHANNEL_INFO]);
	max_dur_sec = nla_get_u32(tb[ATTR_RTT_MAX_DUR_SEC]);
	PRINTM(MCMD_D, "HAL input:\n");
	PRINTM(MCMD_D,
	       "wifi_channel_info: width=%d center_freq=%d center_freq0=%d center_freq1=%d\n",
	       ch_info->width, ch_info->center_freq, ch_info->center_freq0,
	       ch_info->center_freq1);
	PRINTM(MCMD_D, "max_dur_sec=%d\n", max_dur_sec);

	memset(&rtt_rsp_cfg, 0x00, sizeof(rtt_rsp_cfg));
	rtt_rsp_cfg.action = RTT_SET_RESPONDER_ENABLE;
	rtt_rsp_cfg.u.encfg.channel =
		ieee80211_frequency_to_channel(ch_info->center_freq);
	woal_channel_info_to_bandcfg(priv, ch_info,
				     &(rtt_rsp_cfg.u.encfg.bandcfg));
	rtt_rsp_cfg.u.encfg.max_dur_sec = max_dur_sec;
	PRINTM(MCMD_D, "HAL input to rtt_responder_encfg:\n");
	PRINTM(MCMD_D,
	       "channel=%d bandcfg=[chanBand=%d chanWidth=%d chan2Offset=%d scanMode=%d]\n",
	       rtt_rsp_cfg.u.encfg.channel,
	       rtt_rsp_cfg.u.encfg.bandcfg.chanBand,
	       rtt_rsp_cfg.u.encfg.bandcfg.chanWidth,
	       rtt_rsp_cfg.u.encfg.bandcfg.chan2Offset,
	       rtt_rsp_cfg.u.encfg.bandcfg.scanMode);
	PRINTM(MCMD_D, "max_dur_sec=%d\n", rtt_rsp_cfg.u.encfg.max_dur_sec);
	ret = woal_rtt_responder_cfg(priv, MOAL_IOCTL_WAIT, &rtt_rsp_cfg);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_rtt_responder_cfg() failed\n",
		       __func__);
		err = -EFAULT;
		goto done;
	}

	memset(&rtt_rsp, 0x00, sizeof(rtt_rsp));
	woal_bandcfg_to_channel_info(priv, &(rtt_rsp_cfg.u.info.bandcfg),
				     rtt_rsp_cfg.u.info.channel,
				     &(rtt_rsp.channel));
	rtt_rsp.preamble = rtt_rsp_cfg.u.info.preamble;
	PRINTM(MCMD_D, "wifi_rtt_responder report to HAL:\n");
	PRINTM(MCMD_D,
	       "channel: width=%d center_freq=%d center_freq0=%d center_freq1=%d\n",
	       rtt_rsp.channel.width, rtt_rsp.channel.center_freq,
	       rtt_rsp.channel.center_freq0, rtt_rsp.channel.center_freq1);
	PRINTM(MCMD_D, "preamble=%d\n", rtt_rsp.preamble);

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, nla_total_size(sizeof(rtt_rsp)) + VENDOR_REPLY_OVERHEAD);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed in %s\n", __func__);
		goto done;
	}

	/* Put the attribute to the skb */
	nla_put(skb, ATTR_RTT_CHANNEL_INFO, sizeof(rtt_rsp.channel),
		&(rtt_rsp.channel));
	nla_put(skb, ATTR_RTT_PREAMBLE, sizeof(rtt_rsp.preamble),
		&(rtt_rsp.preamble));
	DBG_HEXDUMP(MCMD_D, "output data skb->data", (t_u8 *)skb->data,
		    skb->len);

	err = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(err))
		PRINTM(MERROR, "Vendor Command reply failed err:%d\n", err);

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to disable rtt responder
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_disable_responder(struct wiphy *wiphy,
						      struct wireless_dev *wdev,
						      const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_rtt_responder rtt_rsp_cfg;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	memset(&rtt_rsp_cfg, 0x00, sizeof(rtt_rsp_cfg));
	rtt_rsp_cfg.action = RTT_SET_RESPONDER_DISABLE;
	ret = woal_rtt_responder_cfg(priv, MOAL_IOCTL_WAIT, &rtt_rsp_cfg);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_rtt_responder_cfg() failed\n",
		       __func__);
		err = -EFAULT;
		goto done;
	}

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to set rtt lci
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_set_lci(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct nlattr *tb[ATTR_RTT_MAX + 1];
	mlan_rtt_responder rtt_rsp_cfg;
	wifi_lci_information *lci_info;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_RTT_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		err = -EFAULT;
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		goto done;
	}

	if (!tb[ATTR_RTT_LCI_INFO]) {
		PRINTM(MERROR, "%s: null attr: tb[ATTR_RTT_LCI_INFO]=%p\n",
		       __func__, tb[ATTR_RTT_LCI_INFO]);
		err = -EINVAL;
		goto done;
	}
	lci_info = (wifi_lci_information *)nla_data(tb[ATTR_RTT_LCI_INFO]);
	PRINTM(MCMD_D, "HAL input:\n");
	PRINTM(MCMD_D,
	       "wifi_lci_information: latitude=%lu longitude=%lu altitude=%d latitude_unc=%d longitude_unc=%d altitude_unc=%d\n",
	       lci_info->latitude, lci_info->longitude, lci_info->altitude,
	       lci_info->latitude_unc, lci_info->longitude_unc,
	       lci_info->altitude_unc);
	PRINTM(MCMD_D,
	       "wifi_lci_information: motion_pattern=%d floor=%d height_above_floor=%d height_unc=%d\n",
	       lci_info->motion_pattern, lci_info->floor,
	       lci_info->height_above_floor, lci_info->height_unc);

	memset(&rtt_rsp_cfg, 0x00, sizeof(rtt_rsp_cfg));
	rtt_rsp_cfg.action = RTT_SET_RESPONDER_LCI;
	moal_memcpy_ext(priv->phandle, &(rtt_rsp_cfg.u.lci), lci_info,
			sizeof(rtt_rsp_cfg.u.lci), sizeof(rtt_rsp_cfg.u.lci));
	ret = woal_rtt_responder_cfg(priv, MOAL_IOCTL_WAIT, &rtt_rsp_cfg);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_rtt_responder_cfg() failed\n",
		       __func__);
		err = -EFAULT;
		goto done;
	}

done:
	LEAVE();
	return err;
}

/**
 * @brief vendor command to set rtt lcr
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_rtt_set_lcr(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct nlattr *tb[ATTR_RTT_MAX + 1];
	mlan_rtt_responder rtt_rsp_cfg;
	wifi_lcr_information *lcr_info;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	int err = 0;

	ENTER();
	PRINTM(MCMND, "Enter %s()\n", __func__);

	err = nla_parse(tb, ATTR_RTT_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		err = -EFAULT;
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		goto done;
	}

	if (!tb[ATTR_RTT_LCR_INFO]) {
		PRINTM(MERROR, "%s: null attr: tb[ATTR_RTT_LCR_INFO]=%p\n",
		       __func__, tb[ATTR_RTT_LCR_INFO]);
		err = -EINVAL;
		goto done;
	}
	lcr_info = (wifi_lcr_information *)nla_data(tb[ATTR_RTT_LCR_INFO]);
	PRINTM(MCMD_D, "HAL input:\n");
	PRINTM(MCMD_D, "wifi_lcr_information: country_code='%c' '%c'\n",
	       lcr_info->country_code[0], lcr_info->country_code[1]);
	PRINTM(MCMD_D, "wifi_lci_information: length=%d civic_info=%s\n",
	       lcr_info->length, lcr_info->civic_info);

	memset(&rtt_rsp_cfg, 0x00, sizeof(rtt_rsp_cfg));
	rtt_rsp_cfg.action = RTT_SET_RESPONDER_LCR;
	moal_memcpy_ext(priv->phandle, &(rtt_rsp_cfg.u.lcr), lcr_info,
			sizeof(rtt_rsp_cfg.u.lcr), sizeof(rtt_rsp_cfg.u.lcr));
	ret = woal_rtt_responder_cfg(priv, MOAL_IOCTL_WAIT, &rtt_rsp_cfg);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "%s: woal_rtt_responder_cfg() failed\n",
		       __func__);
		err = -EFAULT;
		goto done;
	}

done:
	LEAVE();
	return err;
}

#define CSI_DUMP_FILE_MAX 1200000

/**
 * @brief vendor command to set CSI params
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 * @param csi_enable    enable/disable CSI
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_set_csi(struct wiphy *wiphy,
					struct wireless_dev *wdev,
					const void *data, int len,
					int csi_enable)
{
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_ds_misc_cfg *cfg = NULL;
	struct nlattr *tb_vendor[ATTR_CSI_MAX + 1];
	int ret = 0;
	int status = MLAN_STATUS_SUCCESS;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		PRINTM(MERROR, "Could not allocate mlan ioctl request!\n");
		ret = -EFAULT;
		goto done;
	}
	req->req_id = MLAN_IOCTL_MISC_CFG;
	cfg = (mlan_ds_misc_cfg *)req->pbuf;
	cfg->sub_command = MLAN_OID_MISC_CSI;

	priv->csi_enable = csi_enable;
	if (csi_enable == 1) {
		nla_parse(tb_vendor, ATTR_CSI_MAX, (const struct nlattr *)data,
			  len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			  ,
			  NULL
#endif
		);
		if (!tb_vendor[ATTR_CSI_CONFIG]) {
			ret = -EFAULT;
			goto done;
		}
		moal_memcpy_ext(priv->phandle, &cfg->param.csi_params,
				(mlan_ds_csi_params *)nla_data(
					tb_vendor[ATTR_CSI_CONFIG]),
				sizeof(mlan_ds_csi_params),
				sizeof(mlan_ds_csi_params));
		moal_memcpy_ext(priv->phandle, &priv->csi_config,
				&cfg->param.csi_params,
				sizeof(mlan_ds_csi_params),
				sizeof(mlan_ds_csi_params));
		if (tb_vendor[ATTR_CSI_DUMP_FORMAT])
			priv->csi_dump_format =
				nla_get_u8(tb_vendor[ATTR_CSI_DUMP_FORMAT]);
	} else if (csi_enable == 0) {
		nla_parse(tb_vendor, ATTR_CSI_MAX, (const struct nlattr *)data,
			  len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			  ,
			  NULL
#endif
		);
		if (!tb_vendor[ATTR_PEER_MAC_ADDR]) {
			ret = -EFAULT;
			goto done;
		}
		memset(&cfg->param.csi_params, 0, sizeof(mlan_ds_csi_params));
		moal_memcpy_ext(priv->phandle,
				cfg->param.csi_params.csi_filter[0].mac_addr,
				(t_u8 *)nla_data(tb_vendor[ATTR_PEER_MAC_ADDR]),
				MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		ret = -EFAULT;
		goto done;
	}

done:
	if (status != MLAN_STATUS_PENDING)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to enable CSI
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_csi_enable(struct wiphy *wiphy,
					   struct wireless_dev *wdev,
					   const void *data, int len)
{
	int ret = 0;

	ENTER();

	ret = woal_cfg80211_subcmd_set_csi(wiphy, wdev, data, len, 1);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to disable CSI
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_csi_disable(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	int ret = 0;

	ENTER();

	ret = woal_cfg80211_subcmd_set_csi(wiphy, wdev, data, len, 0);

	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get CSI dump path
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_get_csi_dump_path(struct wiphy *wiphy,
						  struct wireless_dev *wdev,
						  const void *data, int len)
{
	int ret = 0;
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	struct sk_buff *skb = NULL;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
						  sizeof(priv->csi_dump_path));
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Push the data to the skb */
	nla_put(skb, ATTR_CSI_DUMP_PATH, sizeof(priv->csi_dump_path),
		(t_u8 *)priv->csi_dump_path);

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret)) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", ret);
		goto done;
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get CSI config
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_get_csi_config(struct wiphy *wiphy,
					       struct wireless_dev *wdev,
					       const void *data, int len)
{
	int ret = 0;
	struct net_device *dev = NULL;
	moal_private *priv = NULL;
	struct sk_buff *skb = NULL;

	ENTER();

	if (!wdev || !wdev->netdev) {
		LEAVE();
		return -EFAULT;
	}
	dev = wdev->netdev;
	priv = (moal_private *)woal_get_netdev_priv(dev);

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
						  sizeof(priv->csi_config));
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	/* Push the data to the skb */
	nla_put(skb, ATTR_CSI_CONFIG, sizeof(mlan_ds_csi_params),
		(t_u8 *)&priv->csi_config);

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret)) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", ret);
		goto done;
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor command to get CSI capability
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_get_csi_capa(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int len)
{
	ENTER();
	LEAVE();
	return 0;
}

/**
 * @brief Save CSI dump to file
 *
 * @param dir_name    Directory name
 * @param file_name    File name
 * @param buf    Pointer to dump buffer
 * @param buf_len    Length of buf
 * @param name    Full path name of CSI dump
 *
 * @return      0: success  -1: fail
 */
static mlan_status woal_save_csi_dump_to_file(char *dir_name, char *file_name,
					      t_u8 *buf, int buf_len,
					      t_u8 format, char *name)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
#ifndef DUMP_TO_PROC
	struct file *pfile = NULL;
	loff_t pos;
	char dw_string[10];
	int i = 0;
	t_u32 *tmp = NULL;
#endif
	ENTER();

	if (!dir_name || !file_name || !buf) {
		PRINTM(MERROR, "Can't save dump info to file\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
#ifndef DUMP_TO_PROC
	snprintf(name, MAX_BUF_LEN, "%s/%s", dir_name, file_name);
	pfile = filp_open(name, O_CREAT | O_RDWR | O_APPEND, 0644);

	if (IS_ERR(pfile)) {
		PRINTM(MMSG,
		       "Create file %s error, try to save dump file in /var\n",
		       name);
		snprintf(name, MAX_BUF_LEN, "%s/%s", "/var", file_name);
		pfile = filp_open(name, O_CREAT | O_RDWR | O_APPEND, 0644);
	}
	if (IS_ERR(pfile)) {
		PRINTM(MERROR, "Create Dump file for %s error\n", name);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	PRINTM(MMSG, "Dump data %s saved in %s\n", file_name, name);

	pos = 0;
	/* Save CSI dump directly to file */
	if (format == 1) {
#if KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE
		vfs_write(pfile, (const char __user *)buf, buf_len, &pos);
#else
		kernel_write(pfile, buf, buf_len, &pos);
#endif
	} else {
		tmp = (t_u32 *)buf;
		for (i = 0; i < buf_len / 4; i++) {
			if ((i + 1) % 8 == 0)
				snprintf(dw_string, sizeof(dw_string), "%08x\n",
					 *tmp);
			else
				snprintf(dw_string, sizeof(dw_string), "%08x ",
					 *tmp);
#if KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE
			vfs_write(pfile, (const char __user *)dw_string, 9,
				  &pos);
#else
			kernel_write(pfile, dw_string, 9, &pos);
#endif
			tmp++;
		}
	}
	filp_close(pfile, NULL);

	PRINTM(MMSG, "Dump data saved in %s successfully\n", name);
#endif
done:
	LEAVE();
	return ret;
}

/**
 * @brief vendor event to upload csi dump
 *
 * @param priv     A pointer to moal_private
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      mlan_status
 */
mlan_status woal_cfg80211_event_csi_dump(moal_private *priv, t_u8 *data,
					 int len)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	char path_name[20];
	char file_name[20];

	ENTER();

	DBG_HEXDUMP(MCMD_D, "CSI dump data", data, len);
	if (snprintf(path_name, sizeof(path_name), "/data") <= 0)
		PRINTM(MERROR, "Fail to print path name in path_name buffer\n");
	if (priv->csi_dump_format == 1) {
		if (snprintf(file_name, sizeof(file_name), "csi_dump.bin") <= 0)
			PRINTM(MERROR,
			       "Fail to print csi_dump.bin in file_name buffer\n");
	} else {
		if (snprintf(file_name, sizeof(file_name), "csi_dump.txt") <= 0)
			PRINTM(MERROR,
			       "Fail to print csi_dump.txt in file_name buffer\n");
	}
	priv->csi_dump_len += len;
	if (priv->csi_dump_len > CSI_DUMP_FILE_MAX) {
		PRINTM(MERROR,
		       "Reached file maximum size. Not saving CSI records.\n");
		goto done;
	}
	/* Save CSI dump to file */
	ret = woal_save_csi_dump_to_file(path_name, file_name, data, len,
					 priv->csi_dump_format,
					 priv->csi_dump_path);
	if (ret != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to save CSI dump to file\n");
		goto done;
	}

done:
	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the dmcs.
 *	It sets or gets the dmcs
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_dmcs(struct wiphy *wiphy,
				     struct wireless_dev *wdev,
				     const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_misc_cfg *dmcs_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_u32 user_data[2];
	struct sk_buff *skb = NULL;
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u32 user_data_array_size = 0;
	t_u8 get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR, "vendor cmd: dmcs - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	user_data_array_size = (t_u32)sizeof(user_data) / sizeof(t_u32);
	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get dmcs!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get dmcs failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		memset((char *)user_data, 0, sizeof(user_data));
		user_data[0] = 1;
		user_data_len = 1;
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set dmcs !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     user_data_array_size,
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the dmcs!\n");
			ret = -EINVAL;
			goto done;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, dmcs!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	dmcs_cfg = (mlan_ds_misc_cfg *)req->pbuf;
	req->req_id = MLAN_IOCTL_MISC_CFG;
	dmcs_cfg->sub_command = MLAN_OID_MISC_DMCS_CONFIG;
	dmcs_cfg->param.dmcs_policy.subcmd = (t_u16)user_data[0];
	switch (user_data[0]) {
	case 0:
		if (user_data_len != 2) {
			PRINTM(MERROR, "Please provide mapping policy\n");
			ret = -EINVAL;
			goto done;
		}
		req->action = MLAN_ACT_SET;
		dmcs_cfg->param.dmcs_policy.mapping_policy =
			(t_u16)user_data[1];
		break;
	case 1:
		req->action = MLAN_ACT_GET;
		break;
	default:
		break;
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "vendor cmd: Set/Get dmcs ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, sizeof(mlan_ds_misc_dmcs_status));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get dmcs if an input data argument is 00 */
	if (req->action == MLAN_ACT_GET) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		pos = skb_put(skb, sizeof(mlan_ds_misc_dmcs_status));
		moal_memcpy_ext(priv->phandle, pos,
				(void *)&dmcs_cfg->param.dmcs_status,
				sizeof(mlan_ds_misc_dmcs_status),
				sizeof(mlan_ds_misc_dmcs_status));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the edmac.
 *	It sets or gets the edmac
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_edmac(struct wiphy *wiphy,
				      struct wireless_dev *wdev,
				      const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_misc_cfg *edmac_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	BOOLEAN is_etsi = MFALSE;
	t_u32 user_data[1];
	struct sk_buff *skb = NULL;
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u32 user_data_array_size = 0;
	t_u8 get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR, "vendor cmd: edmac - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	user_data_array_size = (t_u32)sizeof(user_data) / sizeof(t_u32);
	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get edmac!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get edmac failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		memset((char *)user_data, 0, sizeof(user_data));
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set edmac !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     user_data_array_size,
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the edmac!\n");
			ret = -EINVAL;
			goto done;
		}
	}

	if (user_data_len) {
		if (priv->phandle->params.wacp_mode)
			is_etsi = MTRUE;
		else {
			is_etsi = woal_is_etsi_country(
				priv->phandle->country_code);

			if (is_etsi == MFALSE &&
			    priv->phandle->is_edmac_enabled == MFALSE)
				goto done;

			if (is_etsi == MTRUE &&
			    priv->phandle->is_edmac_enabled == MTRUE)
				goto done;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, edmac!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	edmac_cfg = (mlan_ds_misc_cfg *)req->pbuf;
	edmac_cfg->sub_command = MLAN_OID_MISC_EDMAC_CONFIG;
	req->req_id = MLAN_IOCTL_MISC_CFG;

	if (user_data_len) {
		if (is_etsi && user_data[0] == 1)
			req->action = MLAN_ACT_SET;
		else
			req->action = MLAN_ACT_CLEAR;
	} else {
		req->action = MLAN_ACT_GET;
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "vendor cmd: Set/Get edmac ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	if (is_etsi)
		priv->phandle->is_edmac_enabled = MTRUE;
	else
		priv->phandle->is_edmac_enabled = MFALSE;

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
						  sizeof(mlan_ds_ed_mac_cfg));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get dmcs if an input data argument is 00 */
	if (req->action == MLAN_ACT_GET) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		pos = skb_put(skb, sizeof(mlan_ds_ed_mac_cfg));
		moal_memcpy_ext(priv->phandle, pos,
				(void *)&edmac_cfg->param.edmac_cfg,
				sizeof(mlan_ds_ed_mac_cfg),
				sizeof(mlan_ds_ed_mac_cfg));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the aggrpriotbl.
 *	It sets or gets the aggrpriotbl
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_aggrpriotbl(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_11n_cfg *cfg_11n = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_u32 user_data[MAX_NUM_TID * 2], i, j;
	struct sk_buff *skb = NULL;
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u32 user_data_array_size = 0;
	t_u8 get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: aggrpriotbl - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	user_data_array_size = (t_u32)sizeof(user_data) / sizeof(t_u32);
	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get aggrpriotbl!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get aggrpriotbl failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		memset((char *)user_data, 0, sizeof(user_data));
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set aggrpriotbl !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     user_data_array_size,
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the aggrpriotbl!\n");
			ret = -EINVAL;
			goto done;
		}
		if (user_data_len != user_data_array_size) {
			PRINTM(MERROR, "Invalid number of arguments\n");
			ret = -EINVAL;
			goto done;
		}
		for (i = 0; i < user_data_len; i = i + 2) {
			if ((user_data[i] > 7 && user_data[i] != 0xff) ||
			    (user_data[i + 1] > 7 &&
			     user_data[i + 1] != 0xff)) {
				PRINTM(MERROR,
				       "Invalid priority, valid value 0-7 or 0xff.\n");
				ret = -EFAULT;
				goto done;
			}
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_11n_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, aggrpriotbl!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	cfg_11n = (mlan_ds_11n_cfg *)req->pbuf;
	cfg_11n->sub_command = MLAN_OID_11N_CFG_AGGR_PRIO_TBL;
	req->req_id = MLAN_IOCTL_11N_CFG;

	if (user_data_len == 0) {
		/* Get aggr priority table from MLAN */
		req->action = MLAN_ACT_GET;
	} else {
		for (i = 0, j = 0; i < user_data_len; i = i + 2, ++j) {
			cfg_11n->param.aggr_prio_tbl.ampdu[j] = user_data[i];
			cfg_11n->param.aggr_prio_tbl.amsdu[j] =
				user_data[i + 1];
		}
		/* Update aggr priority table in MLAN */
		req->action = MLAN_ACT_SET;
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "vendor cmd: Set/Get aggrpriotbl ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(user_data));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get dmcs if an input data argument is 00 */
	if (req->action == MLAN_ACT_GET) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		pos = skb_put(skb, sizeof(mlan_ds_11n_aggr_prio_tbl));
		moal_memcpy_ext(priv->phandle, pos,
				(void *)&cfg_11n->param.aggr_prio_tbl,
				sizeof(mlan_ds_11n_aggr_prio_tbl),
				sizeof(mlan_ds_11n_aggr_prio_tbl));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the addbareject.
 *	It sets or gets the addbareject
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_addbareject(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_11n_cfg *cfg_11n = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_u32 user_data[MAX_NUM_TID], i;
	struct sk_buff *skb = NULL;
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u32 user_data_array_size = 0;
	t_u8 get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: addbareject - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	user_data_array_size = (t_u32)sizeof(user_data) / sizeof(t_u32);
	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get addbareject!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get addbareject failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		memset((char *)user_data, 0, sizeof(user_data));
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set addbareject !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     user_data_array_size,
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the addbareject!\n");
			ret = -EINVAL;
			goto done;
		}
		if (user_data_len != user_data_array_size) {
			PRINTM(MERROR, "Invalid number of arguments\n");
			ret = -EINVAL;
			goto done;
		}
		for (i = 0; i < user_data_len; i++) {
			if (user_data[i] != 0 && user_data[i] != 1) {
				PRINTM(MERROR,
				       "addba reject only takes argument as 0 or 1\n");
				ret = -EFAULT;
				goto done;
			}
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_11n_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, addbareject!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	cfg_11n = (mlan_ds_11n_cfg *)req->pbuf;
	cfg_11n->sub_command = MLAN_OID_11N_CFG_ADDBA_REJECT;
	req->req_id = MLAN_IOCTL_11N_CFG;

	if (user_data_len == 0) {
		/* Get add BA reject configuration from MLAN */
		req->action = MLAN_ACT_GET;
	} else {
		for (i = 0; i < user_data_len; i++)
			cfg_11n->param.addba_reject[i] = user_data[i];
		/* Update add BA reject configuration in MLAN */
		req->action = MLAN_ACT_SET;
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "vendor cmd: Set/Get addbareject ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(user_data));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get dmcs if an input data argument is 00 */
	if (req->action == MLAN_ACT_GET) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		pos = skb_put(skb, sizeof(cfg_11n->param.addba_reject));
		moal_memcpy_ext(priv->phandle, pos,
				(void *)&cfg_11n->param.addba_reject,
				sizeof(cfg_11n->param.addba_reject),
				sizeof(cfg_11n->param.addba_reject));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the protection mode.
 *	It sets or gets the tx_ampdu_prot_mode
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_tx_ampdu_prot_mode(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_misc_cfg *misc = NULL;
	mlan_ioctl_req *req = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_u32 user_data[1];
	struct sk_buff *skb = NULL;
	t_s32 ret = 0;
	t_u16 user_data_len = 0;
	t_u32 user_data_array_size = 0;
	t_u8 get_val = 0;
	t_u8 *data_buff = NULL;
	t_u8 *pos = NULL;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: protection mode - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);
	user_data_array_size = (t_u32)sizeof(user_data) / sizeof(t_u32);
	if (len == 1) {
		PRINTM(MMSG, "vendor cmd: Get protection mode!\n");
		get_val = (t_u8) * (data_buff);

		/* Get addbaparams works if an input argument passed is 00 */
		if (get_val) {
			PRINTM(MERROR,
			       "vendor cmd: Get protection mode failed due to Invalid argument!\n");
			ret = -EINVAL;
			goto done;
		}
		memset((char *)user_data, 0, sizeof(user_data));
	} else if (len > 1) {
		PRINTM(MMSG, "Vendor cmd: Set protection mode !\n");
		memset((char *)user_data, 0, sizeof(user_data));

		/* vendor cmd : the user_data_len is set only for set cmd */
		if (woal_parse_vendor_cmd_attributes(data_buff, len, user_data,
						     user_data_array_size,
						     &user_data_len)) {
			PRINTM(MERROR,
			       "vendor cmd: Couldn't parse the protection mode!\n");
			ret = -EINVAL;
			goto done;
		}
		if (user_data_len > 1) {
			PRINTM(MERROR, "Invalid number of arguments\n");
			ret = -EINVAL;
			goto done;
		}
		if (user_data[0] > TX_AMPDU_DYNAMIC_RTS_CTS) {
			PRINTM(MERROR, "Invalid protection mode\n");
			ret = -EINVAL;
			goto done;
		}
	}

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_misc_cfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, protection mode!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	misc = (mlan_ds_misc_cfg *)req->pbuf;
	misc->sub_command = MLAN_OID_MISC_TX_AMPDU_PROT_MODE;
	req->req_id = MLAN_IOCTL_MISC_CFG;

	if (user_data_len == 0) {
		/* GET operation */
		req->action = MLAN_ACT_GET;
	} else {
		/* SET operation */
		misc->param.tx_ampdu_prot_mode.mode = (t_u16)user_data[0];
		req->action = MLAN_ACT_SET;
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);
	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "vendor cmd: Set/Get addbareject ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(user_data));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Get dmcs if an input data argument is 00 */
	if (req->action == MLAN_ACT_GET) {
		PRINTM(MINFO, "vendor cmd: copying the response into buffer\n");
		pos = skb_put(skb, sizeof(mlan_ds_misc_tx_ampdu_prot_mode));
		moal_memcpy_ext(priv->phandle, pos,
				(void *)&misc->param.tx_ampdu_prot_mode,
				sizeof(mlan_ds_misc_tx_ampdu_prot_mode),
				sizeof(mlan_ds_misc_tx_ampdu_prot_mode));
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);
	LEAVE();
	return ret;
}

/**
 *  @brief              Extract numeric value of specified attribute(substring)
 * from string buffer.
 *
 *  @param str          Pointer to received string buffer
 *  @param str_len      length of the received string buffer
 *  @param substr       Pointer to received substring (attributes of twt cmds)
 *  @param substr_len   length of the received substring
 *  @return             Numeric value or 0
 */
static t_u16 extractNumericVal(char *str, t_u32 str_len, char *substr,
			       t_u8 substr_len)
{
	char result[6]; //  result holds argument value which can be of max size
			//  of t_u16 (65535)
	t_u8 res_len = 0;
	t_u8 i = 0, j = substr_len - 1;
	t_u16 finalVal = 0;
	char *findStr = strstr(str, substr);
	if (findStr == NULL)
		return finalVal;

	/* Function usage is controlled and input validation ensures safe
	 * operation */
	// coverity[misra_c_2012_rule_21_13_violation:SUPPRESS]
	// coverity[overflow_sink:SUPPRESS]
	while ((j < str_len) && (findStr[j] != '\0') && isdigit(findStr[j])) {
		/* Loop bounds protected by str_len check and isdigit()
		 * validation. Buffer overflow prevented by result[6] sizing for
		 * maximum expected value */
		// coverity[overflow_sink:SUPPRESS]
		result[i++] = findStr[j];
		j++;
	}
	result[i] = '\0';
	res_len = sizeof(result);

	for (i = 0; result[i] != '\0'; i++) {
		if (result[i] >= '0' && result[i] <= '9') {
			finalVal = finalVal * 10 + (result[i] - '0');
		}
	}
	return finalVal;
}

/**
 *  @brief              ascii data buffer into String buffer
 *
 *  @param raw_data     Pointer to received ascii data buffer
 *  @param len          length of the received ascii data buffer
 *  @return             string buffer or NULL
 */
static void asciiToString(t_u8 *raw_data, t_u32 len, char *str, t_u32 *str_len)
{
	t_u32 i, j;

	for (i = 0, j = 0; i < len; i++) {
		// Ignore Spaces and new line character.
		if ((0x20 == raw_data[i]) || (0x0a == raw_data[i]))
			continue;

		if (j < len)
			str[j++] = (char)raw_data[i];
	}

	str[j] = '\0';
	*str_len = j + 1;
}

/**
 * @brief Vendor cmd to trigger the twt_setup cmd.
 *  It sets the twt configuration to start the twt agreement.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_twt_setup(struct wiphy *wiphy,
					  struct wireless_dev *wdev,
					  const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_twtcfg *twt_setup_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	struct sk_buff *skb = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_s32 ret = 0;
	t_u8 *data_buff = NULL;
	char *strBuffer = NULL;
	t_u32 strBuff_len = 0;
	t_u8 implicit = 0, announced = 0, triggered_enabled = 0,
	     info_disabled = 0, negotiation_type = 0, wakeup_duration = 0;
	t_u8 flow_identifier = 0, hard_constraint = 0, exponent = 0,
	     request_type = 0;
	t_u16 mantissa = 0, bcn_miss_threshold = 0;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: twt_setup - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);

	strBuffer = (char *)kzalloc(len + 1, GFP_ATOMIC);
	if (strBuffer == NULL) {
		ret = -ENOMEM;
		goto done;
	}

	// Convert ascii data into string buffer
	asciiToString(data_buff, len, strBuffer, &strBuff_len);

	implicit = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					   "Implicit=", sizeof("Implicit="));
	announced = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "Announced=", sizeof("Announced="));
	triggered_enabled = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"TriggerEnabled=", sizeof("TriggerEnabled="));
	info_disabled = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"TWTInformationDisabled=", sizeof("TWTInformationDisabled="));
	negotiation_type = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"NegotiationType=", sizeof("NegotiationType="));
	wakeup_duration = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"TWTWakeupDuration=", sizeof("TWTWakeupDuration="));
	flow_identifier = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"FlowIdentifier=", sizeof("FlowIdentifier="));
	hard_constraint = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"HardConstraint=", sizeof("HardConstraint="));
	exponent = (t_u8)extractNumericVal(
		strBuffer, strBuff_len, "TWTExponent=", sizeof("TWTExponent="));
	mantissa = extractNumericVal(strBuffer, strBuff_len,
				     "TWTMantissa=", sizeof("TWTMantissa="));
	request_type = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"TWTRequestType=", sizeof("TWTRequestType="));
	bcn_miss_threshold = extractNumericVal(
		strBuffer, strBuff_len,
		"BeaconMissThreshold=", sizeof("BeaconMissThreshold="));

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_twtcfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, twt_setup!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	twt_setup_cfg = (mlan_ds_twtcfg *)req->pbuf;
	twt_setup_cfg->sub_command = MLAN_OID_11AX_TWT_CFG;
	twt_setup_cfg->sub_id = MLAN_11AX_TWT_SETUP_SUBID;
	req->req_id = MLAN_IOCTL_11AX_CFG;
	req->action = MLAN_ACT_SET;

	twt_setup_cfg->param.twt_setup.implicit = implicit;
	twt_setup_cfg->param.twt_setup.announced = announced;
	twt_setup_cfg->param.twt_setup.trigger_enabled = triggered_enabled;
	twt_setup_cfg->param.twt_setup.twt_info_disabled = info_disabled;
	twt_setup_cfg->param.twt_setup.negotiation_type = negotiation_type;
	twt_setup_cfg->param.twt_setup.twt_wakeup_duration = wakeup_duration;
	twt_setup_cfg->param.twt_setup.flow_identifier = flow_identifier;
	twt_setup_cfg->param.twt_setup.hard_constraint = hard_constraint;
	twt_setup_cfg->param.twt_setup.twt_exponent = exponent;
	twt_setup_cfg->param.twt_setup.twt_mantissa = mantissa;
	twt_setup_cfg->param.twt_setup.twt_request = request_type;
	twt_setup_cfg->param.twt_setup.bcnMiss_threshold = bcn_miss_threshold;

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "vendor cmd: TWT Setup ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
						  sizeof(mlan_ds_twt_setup));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (strBuffer)
		kfree(strBuffer);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);

	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger the twt_teardown cmd.
 * It terminates the twt agreement.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_twt_teardown(struct wiphy *wiphy,
					     struct wireless_dev *wdev,
					     const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_twtcfg *twt_teardown_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	struct sk_buff *skb = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_s32 ret = 0;
	t_u8 *data_buff = NULL;
	char *strBuffer = NULL;
	t_u32 strBuff_len = 0;
	t_u8 flow_identifier = 0, negotiation_type = 0, teardown_all_twt = 0;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: twt_teardown - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);

	strBuffer = (char *)kzalloc(len + 1, GFP_ATOMIC);
	if (strBuffer == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	// Convert ascii data into string buffer.
	asciiToString(data_buff, len, strBuffer, &strBuff_len);

	flow_identifier = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"FlowIdentifier=", sizeof("FlowIdentifier="));
	negotiation_type = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"NegotiationType=", sizeof("NegotiationType="));
	teardown_all_twt = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"TearDownAllTWT=", sizeof("TearDownAllTWT="));

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_twtcfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, twt_teardown!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	twt_teardown_cfg = (mlan_ds_twtcfg *)req->pbuf;
	twt_teardown_cfg->sub_command = MLAN_OID_11AX_TWT_CFG;
	twt_teardown_cfg->sub_id = MLAN_11AX_TWT_TEARDOWN_SUBID;
	req->req_id = MLAN_IOCTL_11AX_CFG;
	req->action = MLAN_ACT_SET;

	twt_teardown_cfg->param.twt_teardown.flow_identifier = flow_identifier;
	twt_teardown_cfg->param.twt_teardown.negotiation_type =
		negotiation_type;
	twt_teardown_cfg->param.twt_teardown.teardown_all_twt =
		teardown_all_twt;

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "vendor cmd: TWT Teardown ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
						  sizeof(mlan_ds_twt_teardown));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (strBuffer)
		kfree(strBuffer);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);

	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger btwt_ap_config_set cmd.
 * It sets the current uAP BTWT config sets.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
// Declaration with identifier \"woal_cfg80211_subcmd_btwt_ap_config_set\",
// which is ambiguous. coverity[misra_c_2012_rule_5_2_violation:SUPPRESS]
static int woal_cfg80211_subcmd_btwt_ap_config_set(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_twtcfg *btwt_ap_config_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	struct sk_buff *skb = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_s32 ret = 0;
	t_u8 *data_buff = NULL;
	char *strBuffer = NULL;
	t_u32 strBuff_len = 0;
	t_u8 ap_bcast_bet_sta_wait, bcastTWTLI, count, btwtId[5],
		Ap_Bcast_Exponent[5], nominalwake[5];
	t_u16 Ap_Bcast_Offset, Ap_Bcast_Mantissa[5];
	t_u8 i;
	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: btwt_ap_config_set - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}

	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);

	strBuffer = (char *)kzalloc(len + 1, GFP_ATOMIC);
	if (strBuffer == NULL) {
		ret = -ENOMEM;
		goto done;
	}

	// Convert ascii data into string buffer
	asciiToString(data_buff, len, strBuffer, &strBuff_len);

	ap_bcast_bet_sta_wait = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"ap_bcast_bet_sta_wait=", sizeof("ap_bcast_bet_sta_wait="));
	Ap_Bcast_Offset = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Offset=", sizeof("Ap_Bcast_Offset="));
	bcastTWTLI = (t_u8)extractNumericVal(
		strBuffer, strBuff_len, "bcastTWTLI=", sizeof("bcastTWTLI="));
	count = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					"count=", sizeof("count="));

	btwtId[0] = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "btwtId0=", sizeof("btwtId0="));
	Ap_Bcast_Mantissa[0] = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Mantissa0=", sizeof("Ap_Bcast_Mantissa0="));
	Ap_Bcast_Exponent[0] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Exponent0=", sizeof("Ap_Bcast_Exponent0="));
	nominalwake[0] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"nominalwake0=", sizeof("nominalwake0="));

	btwtId[1] = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "btwtId1=", sizeof("btwtId1="));
	Ap_Bcast_Mantissa[1] = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Mantissa1=", sizeof("Ap_Bcast_Mantissa1="));
	Ap_Bcast_Exponent[1] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Exponent1=", sizeof("Ap_Bcast_Exponent1="));
	nominalwake[1] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"nominalwake1=", sizeof("nominalwake1="));

	btwtId[2] = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "btwtId2=", sizeof("btwtId2="));
	Ap_Bcast_Mantissa[2] = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Mantissa2=", sizeof("Ap_Bcast_Mantissa2="));
	Ap_Bcast_Exponent[2] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Exponent2=", sizeof("Ap_Bcast_Exponent2="));
	nominalwake[2] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"nominalwake2=", sizeof("nominalwake2="));

	btwtId[3] = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "btwtId3=", sizeof("btwtId3="));
	Ap_Bcast_Mantissa[3] = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Mantissa3=", sizeof("Ap_Bcast_Mantissa3="));
	Ap_Bcast_Exponent[3] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Exponent3=", sizeof("Ap_Bcast_Exponent3="));
	nominalwake[3] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"nominalwake3=", sizeof("nominalwake3="));

	btwtId[4] = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					    "btwtId4=", sizeof("btwtId4="));
	Ap_Bcast_Mantissa[4] = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Mantissa4=", sizeof("Ap_Bcast_Mantissa4="));
	Ap_Bcast_Exponent[4] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Exponent4=", sizeof("Ap_Bcast_Exponent4="));
	nominalwake[4] = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"nominalwake4=", sizeof("nominalwake4="));

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_twtcfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, btwt_ap_config_set!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	btwt_ap_config_cfg = (mlan_ds_twtcfg *)req->pbuf;
	btwt_ap_config_cfg->sub_command = MLAN_OID_11AX_TWT_CFG;
	btwt_ap_config_cfg->sub_id = MLAN_11AX_BTWT_AP_CONFIG_SUBID;
	req->req_id = MLAN_IOCTL_11AX_CFG;
	req->action = MLAN_ACT_SET;

	btwt_ap_config_cfg->param.btwt_ap_config.ap_bcast_bet_sta_wait =
		ap_bcast_bet_sta_wait;
	btwt_ap_config_cfg->param.btwt_ap_config.Ap_Bcast_Offset =
		Ap_Bcast_Offset;
	btwt_ap_config_cfg->param.btwt_ap_config.bcastTWTLI = bcastTWTLI;
	btwt_ap_config_cfg->param.btwt_ap_config.count = count;

	for (i = 0; i < 5; i++) {
		btwt_ap_config_cfg->param.btwt_ap_config.BTWT_sets[i].btwtId =
			btwtId[i];
		btwt_ap_config_cfg->param.btwt_ap_config.BTWT_sets[i]
			.Ap_Bcast_Mantissa = Ap_Bcast_Mantissa[i];
		btwt_ap_config_cfg->param.btwt_ap_config.BTWT_sets[i]
			.Ap_Bcast_Exponent = Ap_Bcast_Exponent[i];
		btwt_ap_config_cfg->param.btwt_ap_config.BTWT_sets[i]
			.nominalwake = nominalwake[i];
	}

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "vendor cmd: BTWT AP Config SET ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, sizeof(mlan_ds_btwt_ap_config));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (strBuffer)
		kfree(strBuffer);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);

	LEAVE();
	return ret;
}

/**
 * @brief Vendor cmd to trigger btwt_ap_config_get cmd.
 * It gets the current uAP BTWT config sets.
 *
 * @param wiphy    A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 * @param data     a pointer to data
 * @param  len     data length
 *
 * @return      0: success  -1: fail
 */
static int woal_cfg80211_subcmd_btwt_ap_config_get(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	mlan_ds_twtcfg *btwt_ap_config_cfg = NULL;
	mlan_ioctl_req *req = NULL;
	struct sk_buff *skb = NULL;
	mlan_status status = MLAN_STATUS_SUCCESS;
	t_s32 ret = 0;
	t_u8 *data_buff = NULL;
	char *strBuffer = NULL;
	t_u32 strBuff_len = 0;
	t_u8 ap_bcast_bet_sta_wait, bcastTWTLI, count;
	t_u16 Ap_Bcast_Offset;

	ENTER();

	if ((len < 1) || (len + 1) < 0) {
		PRINTM(MERROR,
		       "vendor cmd: btwt_ap_config_get - Invalid data length!\n");
		ret = -EINVAL;
		goto done;
	}
	data_buff = (t_u8 *)kzalloc(len + 1, GFP_ATOMIC);
	if (data_buff == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	memset((char *)data_buff, 0, len + 1);
	moal_memcpy_ext(priv->phandle, data_buff, data, len, len);

	strBuffer = (char *)kzalloc(len + 1, GFP_ATOMIC);
	if (strBuffer == NULL) {
		ret = -ENOMEM;
		goto done;
	}
	// Convert ascii data into string buffer.
	asciiToString(data_buff, len, strBuffer, &strBuff_len);

	ap_bcast_bet_sta_wait = (t_u8)extractNumericVal(
		strBuffer, strBuff_len,
		"ap_bcast_bet_sta_wait=", sizeof("ap_bcast_bet_sta_wait="));
	Ap_Bcast_Offset = (t_u16)extractNumericVal(
		strBuffer, strBuff_len,
		"Ap_Bcast_Offset=", sizeof("Ap_Bcast_Offset="));
	bcastTWTLI = (t_u8)extractNumericVal(
		strBuffer, strBuff_len, "bcastTWTLI=", sizeof("bcastTWTLI="));
	count = (t_u8)extractNumericVal(strBuffer, strBuff_len,
					"count=", sizeof("count="));

	/* Allocate an IOCTL request buffer */
	req = woal_alloc_mlan_ioctl_req(sizeof(mlan_ds_twtcfg));
	if (req == NULL) {
		PRINTM(MERROR,
		       "vendor cmd: Could not allocate mlan ioctl request, btwt_ap_config_get!\n");
		ret = -ENOMEM;
		goto done;
	}

	/* Fill request buffer */
	btwt_ap_config_cfg = (mlan_ds_twtcfg *)req->pbuf;
	btwt_ap_config_cfg->sub_command = MLAN_OID_11AX_TWT_CFG;
	btwt_ap_config_cfg->sub_id = MLAN_11AX_BTWT_AP_CONFIG_SUBID;
	req->req_id = MLAN_IOCTL_11AX_CFG;
	req->action = MLAN_ACT_GET;

	btwt_ap_config_cfg->param.btwt_ap_config.ap_bcast_bet_sta_wait =
		ap_bcast_bet_sta_wait;
	btwt_ap_config_cfg->param.btwt_ap_config.Ap_Bcast_Offset =
		Ap_Bcast_Offset;
	btwt_ap_config_cfg->param.btwt_ap_config.bcastTWTLI = bcastTWTLI;
	btwt_ap_config_cfg->param.btwt_ap_config.count = count;

	status = woal_request_ioctl(priv, req, MOAL_IOCTL_WAIT);

	if (status != MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR,
		       "vendor cmd: BTWT AP CONFIG GET ioctl failed!\n");
		ret = -EFAULT;
		goto done;
	}

	/* Allocate skb for cmd reply*/
	skb = cfg80211_vendor_cmd_alloc_reply_skb(
		wiphy, sizeof(mlan_ds_btwt_ap_config));
	if (!skb) {
		PRINTM(MERROR,
		       "vendor cmd: allocate memory fail for vendor cmd\n");
		ret = -ENOMEM;
		goto done;
	}

	ret = cfg80211_vendor_cmd_reply(skb);
	if (unlikely(ret))
		PRINTM(MERROR, "vendor cmd: reply failed with ret:%d \n", ret);

done:
	if (data_buff)
		kfree(data_buff);
	if (strBuffer)
		kfree(strBuffer);
	if (status != MLAN_STATUS_PENDING && req)
		kfree(req);

	LEAVE();
	return ret;
}

/**
 * @brief Request list of usable channels for requested bands and modes.
 *        Usable implies channel is allowed as per regulatory for current
 *        country code and not restricted due to other hard limitations.
 *        This allows driver to return list of usable channels for each
 *        mode(STA, AP, P2P-go etc) uniquely to distinguish cases where
 *        only limited set of modes are allowed on a given channel.
 *
 * @param wiphy       A pointer to wiphy struct
 * @param wdev     A pointer to wireless_dev struct
 *
 * @param data     a pointer to data
 * @param  data_len     data length
 *
 * @return      0: success  other: fail
 */
static int woal_cfg80211_subcmd_get_usable_channels(struct wiphy *wiphy,
						    struct wireless_dev *wdev,
						    const void *data, int len)
{
	struct net_device *dev = wdev->netdev;
	moal_private *priv = (moal_private *)woal_get_netdev_priv(dev);
	struct nlattr *tb[ATTR_WIFI_MAX + 1];
	t_u32 band = 0, iface_mode, filter, max_size, size;
	t_u8 cnt = 0, i, j;
	t_u32 mem_needed = 0;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	struct sk_buff *skb = NULL;
	int err = 0;
	wifi_usable_channel *channels = NULL;

	ENTER();

	err = nla_parse(tb, ATTR_USABLE_CHANNEL_MAX, data, len, NULL
#if KERNEL_VERSION(4, 12, 0) <= CFG80211_VERSION_CODE
			,
			NULL
#endif
	);
	if (err) {
		PRINTM(MERROR, "%s: nla_parse fail\n", __func__);
		err = -EFAULT;
		goto done;
	}

	if (!tb[ATTR_USABLE_CHANNEL_BAND]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_USABLE_CHANNEL_BAND]=%p\n",
		       __func__, tb[ATTR_USABLE_CHANNEL_BAND]);
		err = -EINVAL;
		goto done;
	}
	band = nla_get_u32(tb[ATTR_USABLE_CHANNEL_BAND]);
	if (!tb[ATTR_USABLE_CHANNEL_IFACE_MODE]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_USABLE_CHANNEL_IFACE_MODE]=%p\n",
		       __func__, tb[ATTR_USABLE_CHANNEL_IFACE_MODE]);
		err = -EINVAL;
		goto done;
	}
	iface_mode = nla_get_u32(tb[ATTR_USABLE_CHANNEL_IFACE_MODE]);

	if (!tb[ATTR_USABLE_CHANNEL_FILTER]) {
		PRINTM(MERROR,
		       "%s: null attr: tb[ATTR_USABLE_CHANNEL_FILTER]=%p\n",
		       __func__, tb[ATTR_USABLE_CHANNEL_FILTER]);
		err = -EINVAL;
		goto done;
	}
	filter = nla_get_u32(tb[ATTR_USABLE_CHANNEL_FILTER]);

	if (!tb[ATTR_USABLE_CHANNEL_MAX_SIZE]) {
		PRINTM(MERROR, "%s: null attr: tb[ATTR_USABLE_MAX_SIZE]=%p\n",
		       __func__, tb[ATTR_USABLE_CHANNEL_MAX_SIZE]);
		err = -EINVAL;
		goto done;
	}
	max_size = nla_get_u32(tb[ATTR_USABLE_CHANNEL_MAX_SIZE]);

	PRINTM(MCMND,
	       "get usable channels for - band: %d, iface_mode: %d, filter: %d,"
	       " max_size: %d",
	       band, iface_mode, filter, max_size);

	channels = kzalloc(sizeof(wifi_usable_channel) *
				   MIN(MAX_CHANNEL_NUM, max_size),
			   GFP_ATOMIC);
	if (!channels) {
		PRINTM(MERROR,
		       "Failed to allocate memory for wifi_usable_channel of size: %d",
		       MIN(MAX_CHANNEL_NUM, max_size));
		err = -ENOMEM;
		goto done;
	}

	for (i = 0; i < IEEE80211_NUM_BANDS; i++) {
		if (!priv->wdev->wiphy->bands[i])
			continue;
		if ((i == IEEE80211_BAND_2GHZ) && !(band & WLAN_MAC_2_4_BAND))
			continue;
		if ((i == IEEE80211_BAND_5GHZ) && !(band & WLAN_MAC_5_0_BAND))
			continue;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
		if ((i == IEEE80211_BAND_6GHZ) && !(band & WLAN_MAC_6_0_BAND))
			continue;
#endif
		sband = priv->wdev->wiphy->bands[i];
		for (j = 0; (j < sband->n_channels); j++) {
			ch = &sband->channels[j];
			if (ch->flags & IEEE80211_CHAN_DISABLED) {
				PRINTM(MINFO, "Skip DISABLED channels %d\n",
				       ieee80211_frequency_to_channel(
					       ch->center_freq));
				continue;
			}
			if (cnt >= MIN(MAX_CHANNEL_NUM, max_size)) {
				PRINTM(MERROR,
				       "cnt=%d exceeds %d, hence ignore remaining channels. "
				       "cur ch = %dMHz",
				       cnt, MIN(MAX_CHANNEL_NUM, max_size),
				       ch->center_freq);
				break;
			}
			channels[cnt].freq = ch->center_freq;
			channels[cnt].iface_mode_mask =
				IFACE_MODE_STA | IFACE_MODE_SOFTAP |
				IFACE_MODE_P2P_CLIENT | IFACE_MODE_P2P_GO;
			if (!(ch->flags & IEEE80211_CHAN_NO_80MHZ)) {
				channels[cnt].width = WIFI_CHAN_WIDTH_80;
			} else if (!(ch->flags & IEEE80211_CHAN_NO_HT40PLUS) ||
				   !(ch->flags & IEEE80211_CHAN_NO_HT40MINUS)) {
				channels[cnt].width = WIFI_CHAN_WIDTH_40;
			} else if (!(ch->flags & IEEE80211_CHAN_NO_20MHZ)) {
				channels[cnt].width = WIFI_CHAN_WIDTH_20;
			} else {
				PRINTM(MERROR, "Invalid channel width");
				channels[cnt].width = WIFI_CHAN_WIDTH_INVALID;
			}
			PRINTM(MINFO, "channel : %d width: %d",
			       ieee80211_frequency_to_channel(
				       channels[cnt].freq),
			       channels[cnt].width);
			cnt++;
		}
	}
	PRINTM(MCMND, "Usable channel count: %d\n", cnt);
	size = cnt;

	mem_needed = nla_total_size(size * sizeof(wifi_usable_channel)) +
		     nla_total_size(sizeof(size)) + VENDOR_REPLY_OVERHEAD;
	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, mem_needed);
	if (unlikely(!skb)) {
		PRINTM(MERROR, "skb alloc failed");
		err = -ENOMEM;
		goto done;
	}

	if (nla_put_u32(skb, ATTR_USABLE_CHANNEL_SIZE, size)) {
		PRINTM(MERROR, "nla_put ATTR_USABLE_CHANNEL_SIZE failed!\n");
		kfree_skb(skb);
		err = -ENOMEM;
		goto done;
	}
	if (nla_put(skb, ATTR_USABLE_CHANNEL_LIST,
		    size * sizeof(wifi_usable_channel), channels)) {
		PRINTM(MERROR, "nla_put ATTR_USABLE_CHANNEL_LIST failed!\n");
		kfree_skb(skb);
		err = -ENOMEM;
		goto done;
	}

	err = cfg80211_vendor_cmd_reply(skb);
	if (err) {
		PRINTM(MERROR, "Vendor Command reply failed ret:%d\n", err);
		goto done;
	}

done:
	if (channels)
		kfree(channels);
	LEAVE();
	return err;
}

// clang-format off
static const struct wiphy_vendor_command vendor_commands[] = {
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_drvdbg,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_drvdbg,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_valid_channels,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_valid_channels,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_attr_policy,
		.maxattr = ATTR_WIFI_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_scan_mac_oui,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_scan_mac_oui,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_attr_policy,
		.maxattr = ATTR_WIFI_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_scan_band,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_scan_band,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_attr_policy,
		.maxattr = ATTR_WIFI_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_SET_GET_SCANCFG,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_get_scancfg,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_SET_GET_ADDBAPARAMS,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_get_addbaparams,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_SET_GET_CLR_HOSTCMD,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_hostcmd,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_secure_ranging_ctx,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_secure_ranging_ctx,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_secure_ranging_ctx_policy,
		.maxattr = MRVL_WLAN_VENDOR_ATTR_SECURE_RANGING_CTX_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_link_statistic_set,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_link_statistic_set,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_ll_stat_policy,
		.maxattr = ATTR_LL_STATS_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_link_statistic_get,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_link_statistic_get,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_link_statistic_clr,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_link_statistic_clr,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_ll_stat_policy,
		.maxattr = ATTR_LL_STATS_MAX,
#endif
	},
#ifdef STA_CFG80211
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_rssi_monitor,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rssi_monitor,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_rssi_monitor_policy,
		.maxattr = ATTR_RSSI_MONITOR_MAX,
#endif
	},
#endif
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_roaming_offload_key,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_roaming_offload_key,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_start_keep_alive,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_start_keep_alive,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_keep_alive_policy,
		.maxattr = MKEEP_ALIVE_ATTRIBUTE_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_stop_keep_alive,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_stop_keep_alive,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_dfs_capability,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_dfs_offload,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},


	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_GET_CAPA,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_get_capa,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,

#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_RANGE_REQUEST,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_range_request,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_RANGE_CANCEL,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_range_cancel,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_GET_RESPONDER_INFO,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_get_responder_info,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_ENABLE_RESPONDER,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_enable_responder,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_DISABLE_RESPONDER,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_disable_responder,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_SET_LCI,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_set_lci,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = SUBCMD_RTT_SET_LCR,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_rtt_set_lcr,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
                .policy = woal_rtt_policy,
                .maxattr = ATTR_RTT_MAX,
#endif
	},

	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_nd_offload
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_11k_cfg,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_nd_offload_policy,
		.maxattr = ATTR_ND_OFFLOAD_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_drv_version,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_drv_version,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_fw_version,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_fw_version,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_wifi_supp_feature_set,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_supp_feature_set,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_country_code,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_set_country_code,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd =
				   sub_cmd_get_wifi_logger_supp_feature_set,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_wifi_logger_supp_feature_set,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_logger_policy,
		.maxattr = ATTR_WIFI_LOGGER_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_ring_buff_status,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_get_ring_buff_status,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_logger_policy,
		.maxattr = ATTR_WIFI_LOGGER_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_start_logging,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_start_logging,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_logger_policy,
		.maxattr = ATTR_WIFI_LOGGER_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_ring_buff_data,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_get_ring_data,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_logger_policy,
		.maxattr = ATTR_WIFI_LOGGER_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_start_packet_fate_monitor,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_start_packet_fate_monitor,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_fw_mem_dump,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_get_fw_dump,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_drv_mem_dump,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_get_drv_dump,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_set_packet_filter,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_set_packet_filter,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = woal_packet_filter_policy,
		.maxattr = ATTR_PACKET_FILTER_MAX,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = sub_cmd_get_packet_filter_capability,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV |
			 WIPHY_VENDOR_CMD_NEED_RUNNING,
		.doit = woal_cfg80211_subcmd_get_packet_filter_capability,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_cfr_request,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_csi_enable,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_cfr_cancel,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_csi_disable,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_get_csi_dump_path,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_csi_dump_path,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_get_csi_config,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_csi_config,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_get_csi_capa,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = woal_cfg80211_subcmd_get_csi_capa,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_set_get_dmcs,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = &woal_cfg80211_subcmd_dmcs,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_set_get_edmac,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = &woal_cfg80211_subcmd_edmac,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_set_get_aggrpriotbl,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = &woal_cfg80211_subcmd_aggrpriotbl,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_set_get_addbareject,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = &woal_cfg80211_subcmd_addbareject,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
	{
		.info = {
				.vendor_id = MRVL_VENDOR_ID,
				.subcmd = subcmd_set_get_tx_ampdu_prot_mode,
			},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
			 WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = &woal_cfg80211_subcmd_tx_ampdu_prot_mode,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
		.policy = VENDOR_CMD_RAW_DATA,
#endif
	},
    {
        .info = {
                .vendor_id = MRVL_VENDOR_ID,
                .subcmd = subcmd_twt_setup,
            },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV |
            WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = &woal_cfg80211_subcmd_twt_setup,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
        .policy = VENDOR_CMD_RAW_DATA,
#endif
    },
    {
        .info = {
                .vendor_id = MRVL_VENDOR_ID,
                .subcmd = subcmd_twt_teardown,
            },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV |
            WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = &woal_cfg80211_subcmd_twt_teardown,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
        .policy = VENDOR_CMD_RAW_DATA,
#endif
    },
    {
        .info = {
                .vendor_id = MRVL_VENDOR_ID,
                .subcmd = subcmd_btwt_ap_config_set,
            },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV |
            WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = &woal_cfg80211_subcmd_btwt_ap_config_set,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
        .policy = VENDOR_CMD_RAW_DATA,
#endif
    },
    {
        .info = {
                .vendor_id = MRVL_VENDOR_ID,
                .subcmd = subcmd_btwt_ap_config_get,
        },
        .flags = WIPHY_VENDOR_CMD_NEED_WDEV |
            WIPHY_VENDOR_CMD_NEED_NETDEV,
        .doit = &woal_cfg80211_subcmd_btwt_ap_config_get,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
        .policy = VENDOR_CMD_RAW_DATA,
#endif
    },
	{
	.info = {
		.vendor_id = MRVL_VENDOR_ID,
		.subcmd = subcmd_get_usable_channels,
	},
	.flags = WIPHY_VENDOR_CMD_NEED_WDEV |
		WIPHY_VENDOR_CMD_NEED_NETDEV,
	.doit = woal_cfg80211_subcmd_get_usable_channels,
#if KERNEL_VERSION(5, 3, 0) <= CFG80211_VERSION_CODE
	.policy = woal_usable_channel_policy,
	.maxattr = ATTR_USABLE_CHANNEL_MAX,
#endif
	},
};
// clang-format on

/**
 * @brief register vendor commands and events
 *
 * @param wiphy       A pointer to wiphy struct
 *
 * @return
 */
void woal_register_cfg80211_vendor_command(struct wiphy *wiphy)
{
	ENTER();
	wiphy->vendor_commands = vendor_commands;
	wiphy->n_vendor_commands = ARRAY_SIZE(vendor_commands);
	wiphy->vendor_events = vendor_events;
	wiphy->n_vendor_events = ARRAY_SIZE(vendor_events);
	LEAVE();
}
#endif
