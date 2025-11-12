/** @file mlan_11n_aggr.c
 *
 *  @brief This file contains functions for 11n Aggregation.
 *
 *
 *  Copyright 2008-2021, 2025 NXP
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

/********************************************************
Change log:
    11/10/2008: initial version
********************************************************/

#include "mlan.h"
#include "mlan_join.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_main.h"
#include "mlan_wmm.h"
#include "mlan_11n.h"
#include "mlan_11n_aggr.h"

/********************************************************
			Local Variables
********************************************************/

/********************************************************
			Global Variables
********************************************************/

/********************************************************
			Local Functions
********************************************************/
/**
 *  @brief Aggregate individual packets into one AMSDU packet
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *  @param amsdu_buf A pointer to packet buffer
 *  @param data      A pointer to aggregated data packet being formed
 *  @param pkt_len   Length of current packet to aggregate
 *  @param pad       Pad
 *
 *  @return         Final packet size
 */
static int wlan_11n_form_amsdu_pkt(pmlan_adapter pmadapter, t_u8 *amsdu_buf,
				   t_u8 *data, int pkt_len, int *pad)
{
	int dt_offset, amsdu_buf_offset;
	Rfc1042Hdr_t snap = {
		0xaa, /* LLC DSAP */
		0xaa, /* LLC SSAP */
		0x03, /* LLC CTRL */
		{0x00, 0x00, 0x00}, /* SNAP OUI */
		0x0000 /* SNAP type */
		/*
		 * This field will be overwritten
		 * later with ethertype
		 */
	};

	ENTER();

	memcpy_ext(pmadapter, amsdu_buf, data, (MLAN_MAC_ADDR_LENGTH)*2,
		   (MLAN_MAC_ADDR_LENGTH)*2);
	dt_offset = amsdu_buf_offset = (MLAN_MAC_ADDR_LENGTH)*2;

	snap.snap_type = read_u16_unaligned(pmadapter, data + dt_offset);
	dt_offset += sizeof(t_u16);
	write_u16_unaligned(
		pmadapter, amsdu_buf + amsdu_buf_offset,
		mlan_htons(pkt_len + LLC_SNAP_LEN -
			   ((2 * MLAN_MAC_ADDR_LENGTH) + sizeof(t_u16))));
	amsdu_buf_offset += sizeof(t_u16);
	memcpy_ext(pmadapter, amsdu_buf + amsdu_buf_offset, &snap, LLC_SNAP_LEN,
		   LLC_SNAP_LEN);
	amsdu_buf_offset += LLC_SNAP_LEN;

	memcpy_ext(pmadapter, amsdu_buf + amsdu_buf_offset, data + dt_offset,
		   pkt_len - dt_offset, pkt_len - dt_offset);
	*pad = (((pkt_len + LLC_SNAP_LEN) & 3)) ?
		       (4 - (((pkt_len + LLC_SNAP_LEN)) & 3)) :
		       0;

	LEAVE();
	return pkt_len + LLC_SNAP_LEN + *pad;
}

/**
 *  @brief Add TxPD to AMSDU header
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param mbuf		Pointer to buffer where the TxPD will be formed
 *
 *  @return		N/A
 */
static void wlan_11n_form_amsdu_txpd(mlan_private *priv, mlan_buffer *mbuf)
{
	TxPD *ptx_pd;
	mlan_adapter *pmadapter = priv->adapter;

	ENTER();

	ptx_pd = (TxPD *)mbuf->pbuf;
	_memset(pmadapter, ptx_pd, 0, Tx_PD_SIZEOF(pmadapter));

	/*
	 * Original priority has been overwritten
	 */
	ptx_pd->priority = (t_u8)mbuf->priority;
	ptx_pd->bss_num = GET_BSS_NUM(priv);
	ptx_pd->bss_type = priv->bss_type;
	/* Always zero as the data is followed by TxPD */
	ptx_pd->tx_pkt_offset = Tx_PD_SIZEOF(pmadapter);
	ptx_pd->tx_pkt_type = PKT_TYPE_AMSDU;
	if (mbuf->flags & MLAN_BUF_FLAG_TDLS)
		ptx_pd->flags = MRVDRV_TxPD_FLAGS_TDLS_PACKET;

	if (mbuf->flags & MLAN_BUF_FLAG_EASYMESH) {
		ptx_pd->flags |= MRVDRV_TxPD_FLAGS_EASYMESH;
		memcpy_ext(priv->adapter, ptx_pd->ra_mac, mbuf->mac,
			   MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);
	}
	if (ptx_pd->tx_control == 0)
		/* TxCtrl set by user or default */
		ptx_pd->tx_control = priv->pkt_tx_ctrl;

	endian_convert_TxPD(ptx_pd);

	LEAVE();
}

/**
 *  @brief free pkts in amsdu_txq
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *
 *  @return  N/A
 */
static INLINE void wlan_free_amsdu_txq(pmlan_adapter pmadapter)
{
	pmlan_buffer pmbuf;

	ENTER();
	while ((pmbuf = (pmlan_buffer)util_peek_list(pmadapter->pmoal_handle,
						     &pmadapter->amsdu_txq,
						     MNULL, MNULL))) {
		util_unlink_list(pmadapter->pmoal_handle, &pmadapter->amsdu_txq,
				 (pmlan_linked_list)pmbuf, MNULL, MNULL);
		wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
	}
	LEAVE();
}

#ifdef PCIEAW693
/**
 *  @brief Add TxPD to AMSDU header
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param pmbuf	A pointer to buffer where the TxPD will be formed
 *  @param amsdu_pkt_len   amsdu packet length
 *
 *  @return		increased length (TxPD + intf_hr_len + padding for DMA
 * alignment)
 */
static t_u16 wlan_form_amsdu_txpd(mlan_private *priv, mlan_buffer *pmbuf,
				  t_u16 amsdu_pkt_len)
{
	mlan_adapter *pmadapter = priv->adapter;
	TxPD *ptx_pd;
	t_u8 *head_ptr = MNULL;
	t_u32 data_len = pmbuf->data_len;
	t_u16 len = 0;
	t_s32 offset = 0;
	ENTER();

	head_ptr = pmbuf->pbuf + pmbuf->data_offset - Tx_PD_SIZEOF(pmadapter) -
		   priv->intf_hr_len;
	/*making data buffer 8 ytes aligned for increasing TP with PCIE Scatter
	 * Gather*/
	head_ptr = (t_u8 *)((t_ptr)head_ptr & ~((t_ptr)(7)));
	ptx_pd = (TxPD *)(head_ptr + priv->intf_hr_len);
	_memset(pmadapter, ptx_pd, 0, Tx_PD_SIZEOF(pmadapter));

	/* Set the BSS number to TxPD */
	ptx_pd->bss_num = GET_BSS_NUM(priv);
	ptx_pd->bss_type = priv->bss_type;
	ptx_pd->priority = (t_u8)pmbuf->priority;
	ptx_pd->tx_pkt_type = PKT_TYPE_AMSDU;
	ptx_pd->tx_pkt_length = amsdu_pkt_len;

	ptx_pd->tx_pkt_offset = (t_u16)((t_ptr)pmbuf->pbuf +
					pmbuf->data_offset - (t_ptr)ptx_pd);
	if (pmbuf->flags & MLAN_BUF_FLAG_TDLS)
		ptx_pd->flags = MRVDRV_TxPD_FLAGS_TDLS_PACKET;
	if (ptx_pd->tx_control == 0)
		/* TxCtrl set by user or default */
		ptx_pd->tx_control = priv->pkt_tx_ctrl;

	if (pmbuf->flags & MLAN_BUF_FLAG_EASYMESH) {
		ptx_pd->flags |= MRVDRV_TxPD_FLAGS_EASYMESH;
		memcpy_ext(priv->adapter, ptx_pd->ra_mac, pmbuf->mac,
			   MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);
	}

	endian_convert_TxPD(ptx_pd);

	/* Adjust the data offset and length to include TxPD in pmbuf */
	pmbuf->data_len += pmbuf->data_offset;
	offset = head_ptr - pmbuf->pbuf;
	pmbuf->data_offset = (t_u32)offset;
	if (pmbuf->data_len > pmbuf->data_offset)
		pmbuf->data_len -= pmbuf->data_offset;
	if (pmbuf->data_len > data_len)
		len = (t_u16)(pmbuf->data_len - data_len);
	PRINTM(MDATA, "amsdu_pkt_len=%d, extra_len=%d\n", amsdu_pkt_len, len);
	DBG_HEXDUMP(MDAT_D, "AMSDU TxPD", ptx_pd, Tx_PD_SIZEOF(pmadapter));

	LEAVE();
	return len;
}

/**
 *  @brief Form amsdu subframe
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *  @param pmbuf     A pointer to mlan_buffer
 *  @param pad       Pad
 *
 *  @return         Final packet size
 */
static int wlan_form_amsdu_subframe(pmlan_adapter pmadapter, mlan_buffer *pmbuf,
				    int *pad)
{
	int dt_offset, amsdu_buf_offset;
	Rfc1042Hdr_t snap = {
		0xaa, /* LLC DSAP */
		0xaa, /* LLC SSAP */
		0x03, /* LLC CTRL */
		{0x00, 0x00, 0x00}, /* SNAP OUI */
		0x0000 /* SNAP type */
		/*
		 * This field will be overwritten
		 * later with ethertype
		 */
	};
	t_u8 *amsdu_buf = MNULL;
	t_u8 *data = pmbuf->pbuf + pmbuf->data_offset;
	int pkt_len = pmbuf->data_len;
	t_u16 total_len;

	ENTER();
	if (pmbuf->data_offset >= sizeof(Rfc1042Hdr_t))
		pmbuf->data_offset -= sizeof(Rfc1042Hdr_t);
	else
		PRINTM(MERROR,
		       "Truncated value assigned to pmbuf->data_offset");
	pmbuf->data_len += sizeof(Rfc1042Hdr_t);

	amsdu_buf = pmbuf->pbuf + pmbuf->data_offset;

	memcpy_ext(pmadapter, amsdu_buf, data, (MLAN_MAC_ADDR_LENGTH)*2,
		   (MLAN_MAC_ADDR_LENGTH)*2);
	dt_offset = amsdu_buf_offset = (MLAN_MAC_ADDR_LENGTH)*2;

	snap.snap_type = read_u16_unaligned(pmadapter, data + dt_offset);
	dt_offset += sizeof(t_u16);
	total_len = (t_u16)(pkt_len + LLC_SNAP_LEN -
			    ((2 * MLAN_MAC_ADDR_LENGTH) + sizeof(t_u16)));

	write_u16_unaligned(pmadapter, amsdu_buf + amsdu_buf_offset,
			    mlan_htons(total_len));

	amsdu_buf_offset += sizeof(t_u16);
	memcpy_ext(pmadapter, amsdu_buf + amsdu_buf_offset, &snap, LLC_SNAP_LEN,
		   LLC_SNAP_LEN);
	amsdu_buf_offset += LLC_SNAP_LEN;
	*pad = (((pkt_len + LLC_SNAP_LEN) & 3)) ?
		       (4 - (((pkt_len + LLC_SNAP_LEN)) & 3)) :
		       0;
	pmbuf->data_len += *pad;

	DBG_HEXDUMP(MDAT_D, "AMSDU subframe", pmbuf->pbuf + pmbuf->data_offset,
		    MIN(pmbuf->data_len, MAX_DATA_DUMP_LEN));
	PRINTM(MDATA, "AMSDU subframe len=%d\n", pkt_len + LLC_SNAP_LEN + *pad);
	LEAVE();
	/* pkt_len is limited by MLAN_TX_DATA_BUF_SIZE */
	// coverity[overflow_sink:SUPPRESS]
	return pkt_len + LLC_SNAP_LEN + *pad;
}
#endif

/**
 *  @brief Update the TxPktLength field in TxPD after the complete AMSDU
 *  packet is formed
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param mbuf     TxPD buffer
 *
 *  @return         N/A
 */
static INLINE void wlan_11n_update_pktlen_amsdu_txpd(mlan_private *priv,
						     pmlan_buffer mbuf)
{
	TxPD *ptx_pd;
	ENTER();

	ptx_pd = (TxPD *)mbuf->pbuf;
	ptx_pd->tx_pkt_length = (t_u16)wlan_cpu_to_le16(
		mbuf->data_len - Tx_PD_SIZEOF(priv->adapter));
	ptx_pd->pkt_delay_2ms =
		wlan_wmm_compute_driver_packet_delay(priv, mbuf);

#ifdef STA_SUPPORT
	if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
	    (priv->adapter->pps_uapsd_mode)) {
		if (MTRUE == wlan_check_last_packet_indication(priv)) {
			priv->adapter->tx_lock_flag = MTRUE;
			ptx_pd->flags |= MRVDRV_TxPD_POWER_MGMT_LAST_PACKET;
		}
	}
#endif /* STA_SUPPORT */
	LEAVE();
}

/**
 *  @brief check if UAP AMSDU packet need forward out to connected peers
 *
 *  @param priv       A pointer to mlan_private
 *  @param data       A pointer to 802.3 eth header data
 *
 *  @return			  MTRUE--packet need forward
 *
 */
static t_u8 wlan_uap_check_forward(mlan_private *priv, t_u8 *data)
{
	Eth803Hdr_t *hdr = (Eth803Hdr_t *)data;
	/** include multicast packet */
	if (hdr->dest_addr[0] & 0x01)
		return MTRUE;
	/** include unicast packet to another station */
	if (wlan_get_station_entry(priv, hdr->dest_addr))
		return MTRUE;
	return MFALSE;
}

/**
 *  @brief Get number of aggregated packets
 *
 *  @param priv		A pointer to mlan_private structure
 *  @param data			A pointer to packet data
 *  @param total_pkt_len	Total packet length
 *  @param forward      A pointer forward flag
 *
 *  @return			Number of packets
 */
static int wlan_11n_get_num_aggrpkts(mlan_private *priv, t_u8 *data,
				     int total_pkt_len, t_u8 *forward)
{
	int pkt_count = 0, pkt_len, pad;
	t_u8 hdr_len = sizeof(Eth803Hdr_t);

	t_u8 forward_flag = MFALSE;

	ENTER();
	if (!data) {
		PRINTM(MERROR, "%s(): null packet data\n", __func__);
		LEAVE();
		return pkt_count;
	}
	while (total_pkt_len >= hdr_len) {
		if (priv->bss_role == MLAN_BSS_ROLE_UAP &&
		    wlan_uap_check_forward(priv, data))
			forward_flag = MTRUE;
		/* Length will be in network format, change it to host */
		pkt_len = mlan_ntohs(read_u16_unaligned(
			priv->adapter, data + (2 * MLAN_MAC_ADDR_LENGTH)));
		if (pkt_len > total_pkt_len) {
			PRINTM(MERROR, "Error in packet length.\n");
			break;
		}

		pad = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ?
			      (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) :
			      0;
		data += pkt_len + pad + sizeof(Eth803Hdr_t);
		total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);
		++pkt_count;
	}
	*forward = forward_flag;
	LEAVE();
	return pkt_count;
}

/********************************************************
			Global Functions
********************************************************/

/**
 *  @brief Deaggregate the received AMSDU packet
 *
 *  @param priv		A pointer to mlan_private structure
 *  @param pmbuf	A pointer to aggregated data packet
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status wlan_11n_deaggregate_pkt(mlan_private *priv, pmlan_buffer pmbuf)
{
	t_u16 pkt_len;
	int total_pkt_len;
	t_u8 *data = MNULL;
	mlan_adapter *pmadapter = priv->adapter;
	t_u32 max_rx_data_size = MLAN_RX_DATA_BUF_SIZE;
	int pad;
	mlan_status ret = MLAN_STATUS_FAILURE;
	RxPacketHdr_t *prx_pkt;
	mlan_buffer *daggr_mbuf = MNULL;
	t_u8 rfc1042_eth_hdr[MLAN_MAC_ADDR_LENGTH] = {0xaa, 0xaa, 0x03,
						      0x00, 0x00, 0x00};
	t_u8 hdr_len = sizeof(Eth803Hdr_t);
	t_u8 forward = MFALSE;
	t_u8 eapol_type[2] = {0x88, 0x8e};
	t_u8 tdls_action_type[2] = {0x89, 0x0d};
	t_u32 in_ts_sec, in_ts_usec;
	t_u32 out_ts_sec, out_ts_usec;
	t_u32 in_copy_ts_sec, in_copy_ts_usec;
	t_u32 out_copy_ts_sec, out_copy_ts_usec;
	t_u32 copy_delay = 0;
	t_u32 delay = 0;
	t_u8 num_subframes = 0;

	ENTER();

	data = (t_u8 *)(pmbuf->pbuf + pmbuf->data_offset);
	if (!data) {
		PRINTM(MERROR, "%s(): null rx aggr pkt data\n");
		goto done;
	}
	total_pkt_len = pmbuf->data_len;

	/* Sanity test */
#if defined(USB)
	if (IS_USB(pmadapter->card_type) &&
	    pmadapter->pcard_usb->usb_rx_deaggr.aggr_ctrl.enable) {
		max_rx_data_size =
			pmadapter->pcard_usb->usb_rx_deaggr.aggr_ctrl.aggr_max;
		if (pmadapter->pcard_usb->usb_rx_deaggr.aggr_ctrl.aggr_mode ==
		    MLAN_USB_AGGR_MODE_NUM) {
			max_rx_data_size *=
				MAX(MLAN_USB_MAX_PKT_SIZE,
				    pmadapter->pcard_usb->usb_rx_deaggr
					    .aggr_ctrl.aggr_align);
			max_rx_data_size =
				MAX(max_rx_data_size, MLAN_RX_DATA_BUF_SIZE);
		}
	}
#endif
	if (total_pkt_len > (int)max_rx_data_size) {
		PRINTM(MERROR,
		       "Total packet length greater than tx buffer"
		       " size %d\n",
		       total_pkt_len);
		goto done;
	}
	if (pmadapter->tp_state_on)
		pmadapter->callbacks.moal_get_system_time(
			pmadapter->pmoal_handle, &in_ts_sec, &in_ts_usec);
	num_subframes = pmbuf->use_count =
		wlan_11n_get_num_aggrpkts(priv, data, total_pkt_len, &forward);

	// rx_trace 7
	if (pmadapter->tp_state_on) {
		pmadapter->callbacks.moal_tp_accounting(
			pmadapter->pmoal_handle, pmbuf, 7 /*RX_DROP_P3*/);
		pmadapter->callbacks.moal_tp_accounting_rx_param(
			pmadapter->pmoal_handle, 4, pmbuf->use_count);
	}
	if (pmadapter->tp_state_drop_point == 7 /*RX_DROP_P3*/)
		goto done;
	prx_pkt = (RxPacketHdr_t *)data;
	/**  check if packet need send to host only */
	if (pmbuf->pdesc && !forward) {
		if (pmadapter->callbacks.moal_recv_amsdu_packet) {
			ret = pmadapter->callbacks.moal_recv_amsdu_packet(
				pmadapter->pmoal_handle, pmbuf);
			if (ret == MLAN_STATUS_PENDING) {
#ifdef USB
				if (IS_USB(pmadapter->card_type))
					pmadapter->callbacks.moal_recv_complete(
						pmadapter->pmoal_handle, MNULL,
						pmadapter->rx_data_ep, ret);
#endif
				priv->msdu_in_rx_amsdu_cnt += num_subframes;
				priv->amsdu_rx_cnt++;
				return ret;
			}
			goto done;
		}
	}
	/* total_pkt_len is limited up to MLAN_RX_DATA_BUF_SIZE */
	// coverity[misra_c_2012_directive_4_14_violation:SUPPRESS]
	while (total_pkt_len >= hdr_len) {
		prx_pkt = (RxPacketHdr_t *)data;
		/* Length will be in network format, change it to host */
		pkt_len = mlan_ntohs(read_u16_unaligned(
			priv->adapter, data + (2 * MLAN_MAC_ADDR_LENGTH)));
		if (pkt_len > total_pkt_len) {
			PRINTM(MERROR,
			       "Error in packet length: total_pkt_len = %d, pkt_len = %d\n",
			       total_pkt_len, pkt_len);
			ret = MLAN_STATUS_FAILURE;
			break;
		}

		pad = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ?
			      (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) :
			      0;

		total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);

		if (memcmp(pmadapter, &prx_pkt->rfc1042_hdr, rfc1042_eth_hdr,
			   sizeof(rfc1042_eth_hdr)) == 0) {
			memmove(pmadapter, data + LLC_SNAP_LEN, data,
				(2 * MLAN_MAC_ADDR_LENGTH));
			if (!wlan_secure_add(&data, LLC_SNAP_LEN, &data,
					     TYPE_PTR)) {
				PRINTM(MERROR, "rx amsdu pkt data overflow\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
			pkt_len += sizeof(Eth803Hdr_t) - LLC_SNAP_LEN;
		} else {
			write_u16_unaligned(priv->adapter,
					    data + (2 * MLAN_MAC_ADDR_LENGTH),
					    0);
			pkt_len += sizeof(Eth803Hdr_t);
		}
		daggr_mbuf = wlan_alloc_mlan_buffer(pmadapter,
						    pkt_len + MLAN_NET_IP_ALIGN,
						    0, MOAL_ALLOC_MLAN_BUFFER);
		if (daggr_mbuf == MNULL) {
			PRINTM(MERROR, "Error allocating daggr mlan_buffer\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
		daggr_mbuf->data_offset += MLAN_NET_IP_ALIGN;
		daggr_mbuf->bss_index = pmbuf->bss_index;
		daggr_mbuf->buf_type = pmbuf->buf_type;
		daggr_mbuf->data_len = pkt_len;
		daggr_mbuf->in_ts_sec = pmbuf->in_ts_sec;
		daggr_mbuf->in_ts_usec = pmbuf->in_ts_usec;
		daggr_mbuf->flags = pmbuf->flags;
		daggr_mbuf->extra_ts_sec = pmbuf->extra_ts_sec;
		daggr_mbuf->extra_ts_usec = pmbuf->extra_ts_usec;
		daggr_mbuf->pparent = pmbuf;
		daggr_mbuf->priority = pmbuf->priority;
		if (pmadapter->tp_state_on)
			pmadapter->callbacks.moal_get_system_time(
				pmadapter->pmoal_handle, &in_copy_ts_sec,
				&in_copy_ts_usec);
		memcpy_ext(pmadapter,
			   daggr_mbuf->pbuf + daggr_mbuf->data_offset, data,
			   pkt_len, daggr_mbuf->data_len);
		if (pmadapter->tp_state_on) {
			pmadapter->callbacks.moal_get_system_time(
				pmadapter->pmoal_handle, &out_copy_ts_sec,
				&out_copy_ts_usec);
			copy_delay +=
				(t_u32)(out_copy_ts_sec - in_copy_ts_sec) *
				1000000;
			copy_delay +=
				(t_u32)(out_copy_ts_usec - in_copy_ts_usec);
		}
#ifdef UAP_SUPPORT
		if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
			if (pmbuf->flags & MLAN_BUF_FLAG_EASYMESH)
				memcpy_ext(priv->adapter, daggr_mbuf->mac,
					   pmbuf->mac, MLAN_MAC_ADDR_LENGTH,
					   MLAN_MAC_ADDR_LENGTH);
			ret = wlan_uap_recv_packet(priv, daggr_mbuf);
		} else {
#endif /* UAP_SUPPORT */
			/** send EAPOL from AMSDU pkt to firmware */
			if (priv->sec_info.ewpa_enabled &&
			    (!memcmp(pmadapter,
				     daggr_mbuf->pbuf +
					     daggr_mbuf->data_offset +
					     MLAN_ETHER_PKT_TYPE_OFFSET,
				     eapol_type, sizeof(eapol_type)))) {
				ret = wlan_prepare_cmd(
					priv, HostCmd_CMD_802_11_EAPOL_PKT, 0,
					0, MNULL, daggr_mbuf);
				if (ret == MLAN_STATUS_SUCCESS)
					wlan_recv_event(
						priv,
						MLAN_EVENT_ID_DRV_DEFER_HANDLING,
						MNULL);
				wlan_free_mlan_buffer(pmadapter, daggr_mbuf);
				if (!wlan_secure_add(&data, pkt_len + pad,
						     &data, TYPE_PTR)) {
					PRINTM(MERROR,
					       "rx amsdu pkt data overflow\n");
					ret = MLAN_STATUS_FAILURE;
					break;
				}
				continue;
			}
			/**process tdls packet*/
			if (!memcmp(pmadapter,
				    daggr_mbuf->pbuf + daggr_mbuf->data_offset +
					    MLAN_ETHER_PKT_TYPE_OFFSET,
				    tdls_action_type,
				    sizeof(tdls_action_type))) {
				PRINTM(MEVENT,
				       "Recevie AMSDU TDLS action frame\n");
				wlan_process_tdls_action_frame(
					priv,
					daggr_mbuf->pbuf +
						daggr_mbuf->data_offset,
					daggr_mbuf->data_len);
			}

			ret = pmadapter->callbacks.moal_recv_packet(
				pmadapter->pmoal_handle, daggr_mbuf);
#ifdef UAP_SUPPORT
		}
#endif /* UAP_SUPPORT */
		switch (ret) {
		case MLAN_STATUS_PENDING:
			break;
		case MLAN_STATUS_FAILURE:
			PRINTM(MERROR, "Deaggr, send to moal failed\n");
			daggr_mbuf->status_code = MLAN_ERROR_PKT_INVALID;
			fallthrough;
		case MLAN_STATUS_SUCCESS:
			wlan_recv_packet_complete(pmadapter, daggr_mbuf, ret);
			break;
		default:
			break;
		}

		if (!wlan_secure_add(&data, pkt_len + pad, &data, TYPE_PTR)) {
			PRINTM(MERROR, "rx amsdu pkt data overflow\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
	}
	if (pmadapter->tp_state_on) {
		pmadapter->callbacks.moal_get_system_time(
			pmadapter->pmoal_handle, &out_ts_sec, &out_ts_usec);
		delay += (t_u32)(out_ts_sec - in_ts_sec) * 1000000;
		delay += (t_u32)(out_ts_usec - in_ts_usec);
		// input values are internally generated and controlled, not
		// externally sourced
		// coverity[misra_c_2012_directive_4_14_violation:SUPPRESS]
		pmadapter->callbacks.moal_amsdu_tp_accounting(
			pmadapter->pmoal_handle, delay, copy_delay);
	}

done:
	priv->msdu_in_rx_amsdu_cnt += pmbuf->use_count;
	priv->amsdu_rx_cnt++;
	/** we should free the aggr buffer after deaggr */
	pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
	LEAVE();
	return ret;
}

#ifdef PCIEAW693
/**
 *  @brief Send amsdu subframe list to interface
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param pra_list Pointer to the RA List table containing the pointers
 *                  to packets.
 *  @param headroom Any interface specific headroom that may be need. TxPD
 *                  will be formed leaving this headroom.
 *  @param ptrindex Pointer index
 *
 *  @return     Final packet size or MLAN_STATUS_FAILURE
 */
static int wlan_send_amsdu_subframe_list(mlan_private *priv,
					 raListTbl *pra_list, int headroom,
					 int ptrindex)
{
	int pkt_size = 0;
	pmlan_adapter pmadapter = priv->adapter;
	mlan_buffer *pmbuf_src;
	mlan_buffer *pmbuf_last = MNULL;
	mlan_buffer *pmbuf_first = MNULL;
	int pad = 0;
	mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef DEBUG_LEVEL1
	t_u32 sec = 0, usec = 0;
#endif
	t_u32 max_amsdu_size = MIN(pra_list->max_amsdu, pmadapter->tx_buf_size);
	t_u32 max_msdu_count = 0;
	t_u32 msdu_in_tx_amsdu_cnt = 0;
	ENTER();

	if (ptrindex < 0) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	max_msdu_count = pmadapter->ops.get_max_msdu_cnt(pmadapter);
	pmbuf_src = (pmlan_buffer)util_peek_list(
		pmadapter->pmoal_handle, &pra_list->buf_head, MNULL, MNULL);
	if (pmbuf_src) {
		pmbuf_first = pmbuf_src;
	} else {
		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
		goto exit;
	}

	wlan_wmm_consume_mpdu_budget(pra_list);

	while (pmbuf_src &&
	       ((pkt_size + (pmbuf_src->data_len + LLC_SNAP_LEN) + headroom) <=
		max_amsdu_size) &&
	       (msdu_in_tx_amsdu_cnt < max_msdu_count)) {
		pmbuf_src =
			(pmlan_buffer)util_dequeue_list(pmadapter->pmoal_handle,
							&pra_list->buf_head,
							MNULL, MNULL);
		wlan_wmm_consume_byte_budget(pra_list, pmbuf_src);
		/* Collects TP statistics */
		if (pmadapter->tp_state_on &&
		    (pkt_size > Tx_PD_SIZEOF(pmadapter)))
			pmadapter->callbacks.moal_tp_accounting(
				pmadapter->pmoal_handle, pmbuf_src, 3);
		pra_list->total_pkts--;

		/* decrement for every PDU taken from the list */
		priv->wmm.pkts_queued[ptrindex]--;
		util_scalar_decrement(pmadapter->pmoal_handle,
				      &priv->wmm.tx_pkts_queued, MNULL, MNULL);

		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);

		if (pmbuf_src) {
			pkt_size += wlan_form_amsdu_subframe(pmadapter,
							     pmbuf_src, &pad);
			util_enqueue_list_tail(pmadapter->pmoal_handle,
					       &pmadapter->amsdu_txq,
					       (pmlan_linked_list)pmbuf_src,
					       MNULL, MNULL);
			pmbuf_last = pmbuf_src;
			priv->msdu_in_tx_amsdu_cnt++;
			msdu_in_tx_amsdu_cnt++;
		}
		pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
						    priv->wmm.ra_list_spinlock);

		if (!wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
			pmadapter->callbacks.moal_spin_unlock(
				pmadapter->pmoal_handle,
				priv->wmm.ra_list_spinlock);
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}

		pmbuf_src =
			(pmlan_buffer)util_peek_list(pmadapter->pmoal_handle,
						     &pra_list->buf_head, MNULL,
						     MNULL);
	}

	pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
					      priv->wmm.ra_list_spinlock);
	if (!pmbuf_last) {
		PRINTM(MERROR,
		       "SG_AGGR ERROR: pkt_size=%d max_msdu_count=%d max_amsdu_size=%d msdu_in_tx_amsdu_cnt=%d\n",
		       pkt_size, max_msdu_count, max_amsdu_size,
		       msdu_in_tx_amsdu_cnt);
		goto exit;
	}
	/* Last AMSDU packet does not need padding */
	pkt_size -= pad;
	pmbuf_last->data_len -= pad;

	pkt_size += wlan_form_amsdu_txpd(priv, pmbuf_first, pkt_size);
	/* Collects TP statistics */
	if (pmadapter->tp_state_on) {
		mlan_buffer mbuf;
		mbuf.data_len = pkt_size;
		pmadapter->callbacks.moal_tp_accounting(pmadapter->pmoal_handle,
							&mbuf, 4);
		pmadapter->callbacks.moal_tp_accounting_rx_param(
			pmadapter->pmoal_handle, 5, msdu_in_tx_amsdu_cnt);
	}
	/* Drop Tx packets at drop point 4 */
	if (pmadapter->tp_state_drop_point == 4) {
		wlan_free_amsdu_txq(pmadapter);
		goto exit;
	}
	ret = pmadapter->ops.send_data_list(pmadapter, MLAN_TYPE_DATA,
					    msdu_in_tx_amsdu_cnt, pkt_size);

	if (ret != MLAN_STATUS_RESOURCE) {
		pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
						    priv->wmm.ra_list_spinlock);
		if (wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
			priv->wmm.packets_out[ptrindex]++;
			priv->wmm.tid_tbl_ptr[ptrindex].ra_list_curr = pra_list;
		}
		wlan_advance_bss_on_pkt_push(
			pmadapter, &pmadapter->bssprio_tbl[priv->bss_priority]);
		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
	}

	PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
	PRINTM_NETINTF(MDATA, priv);
	PRINTM(MDATA, "%lu.%06lu : Data => FW\n", sec, usec);
	priv->amsdu_tx_cnt++;
exit:
	LEAVE();
	return pkt_size;
}
#endif
/**
 *  @brief Aggregate multiple packets into one single AMSDU packet
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param pra_list Pointer to the RA List table containing the pointers
 *                  to packets.
 *  @param headroom Any interface specific headroom that may be need. TxPD
 *                  will be formed leaving this headroom.
 *  @param ptrindex Pointer index
 *
 *  @return     Final packet size or MLAN_STATUS_FAILURE
 */
int wlan_11n_aggregate_pkt(mlan_private *priv, raListTbl *pra_list,
			   int headroom, int ptrindex)
{
	int pkt_size = 0;
	pmlan_adapter pmadapter = priv->adapter;
	mlan_buffer *pmbuf_aggr, *pmbuf_src;
	t_u8 *data;
	int pad = 0;
	mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef DEBUG_LEVEL1
	t_u32 sec = 0, usec = 0;
#endif
	mlan_tx_param tx_param;
#ifdef STA_SUPPORT
	TxPD *ptx_pd = MNULL;
#endif
	t_u32 max_amsdu_size = MIN(pra_list->max_amsdu, pmadapter->tx_buf_size);
	t_u32 msdu_in_tx_amsdu_cnt = 0;
	ENTER();

	if (ptrindex < 0) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	PRINTM(MDAT_D, "Handling Aggr packet\n");
#ifdef PCIEAW693
	if (!wlan_copy_on_tx_enabled(pmadapter) &&
	    IS_PCIEAW693(pmadapter->card_type)) {
		return wlan_send_amsdu_subframe_list(priv, pra_list, headroom,
						     ptrindex);
	}
#endif
	pmbuf_src = (pmlan_buffer)util_peek_list(
		pmadapter->pmoal_handle, &pra_list->buf_head, MNULL, MNULL);
	if (pmbuf_src) {
		pmbuf_aggr = wlan_alloc_mlan_buffer(
			pmadapter, pmadapter->tx_buf_size, headroom,
			MOAL_MEM_FLAG_DIRTY | MOAL_MALLOC_BUFFER |
				MOAL_MEM_FLAG_ATOMIC);
		if (!pmbuf_aggr) {
			PRINTM(MERROR, "Error allocating mlan_buffer\n");
			pmadapter->callbacks.moal_spin_unlock(
				pmadapter->pmoal_handle,
				priv->wmm.ra_list_spinlock);
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}

		data = pmbuf_aggr->pbuf + headroom;
		pmbuf_aggr->bss_index = pmbuf_src->bss_index;
		pmbuf_aggr->buf_type = pmbuf_src->buf_type;
		pmbuf_aggr->priority = pmbuf_src->priority;
		pmbuf_aggr->pbuf = data;
		pmbuf_aggr->data_offset = 0;
		pmbuf_aggr->in_ts_sec = pmbuf_src->in_ts_sec;
		pmbuf_aggr->in_ts_usec = pmbuf_src->in_ts_usec;
		pmbuf_aggr->extra_ts_sec = pmbuf_src->extra_ts_sec;
		pmbuf_aggr->extra_ts_usec = pmbuf_src->extra_ts_usec;
		if (pmbuf_src->flags & MLAN_BUF_FLAG_TDLS)
			pmbuf_aggr->flags |= MLAN_BUF_FLAG_TDLS;
		if (pmbuf_src->flags & MLAN_BUF_FLAG_TCP_ACK)
			pmbuf_aggr->flags |= MLAN_BUF_FLAG_TCP_ACK;

		if (pmbuf_src->flags & MLAN_BUF_FLAG_EASYMESH) {
			pmbuf_aggr->flags |= MLAN_BUF_FLAG_EASYMESH;
			memcpy_ext(priv->adapter, pmbuf_aggr->mac,
				   pmbuf_src->mac, MLAN_MAC_ADDR_LENGTH,
				   MLAN_MAC_ADDR_LENGTH);
		}
		/* Form AMSDU */
		wlan_11n_form_amsdu_txpd(priv, pmbuf_aggr);
		pkt_size = Tx_PD_SIZEOF(pmadapter);
#ifdef STA_SUPPORT
		if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
			ptx_pd = (TxPD *)pmbuf_aggr->pbuf;
#endif
		priv->msdu_in_tx_amsdu_cnt++;
	} else {
		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
		goto exit;
	}

	wlan_wmm_consume_mpdu_budget(pra_list);

	while (pmbuf_src && ((pkt_size + (pmbuf_src->data_len + LLC_SNAP_LEN) +
			      headroom) <= max_amsdu_size)) {
		pmbuf_src =
			(pmlan_buffer)util_dequeue_list(pmadapter->pmoal_handle,
							&pra_list->buf_head,
							MNULL, MNULL);
		wlan_wmm_consume_byte_budget(pra_list, pmbuf_src);
		/* Collects TP statistics */
		if (pmadapter->tp_state_on &&
		    (pkt_size > Tx_PD_SIZEOF(pmadapter)))
			pmadapter->callbacks.moal_tp_accounting(
				pmadapter->pmoal_handle, pmbuf_src, 3);
		pra_list->total_pkts--;

		/* decrement for every PDU taken from the list */
		priv->wmm.pkts_queued[ptrindex]--;
		util_scalar_decrement(pmadapter->pmoal_handle,
				      &priv->wmm.tx_pkts_queued, MNULL, MNULL);

		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);

		if (pmbuf_src) {
			pkt_size += wlan_11n_form_amsdu_pkt(
				pmadapter, (data + pkt_size),
				pmbuf_src->pbuf + pmbuf_src->data_offset,
				pmbuf_src->data_len, &pad);

			DBG_HEXDUMP(MDAT_D, "pmbuf_src", pmbuf_src,
				    sizeof(mlan_buffer));
			wlan_write_data_complete(pmadapter, pmbuf_src,
						 MLAN_STATUS_SUCCESS);
		}

		pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
						    priv->wmm.ra_list_spinlock);

		if (!wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
			pmadapter->callbacks.moal_spin_unlock(
				pmadapter->pmoal_handle,
				priv->wmm.ra_list_spinlock);
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}

		pmbuf_src =
			(pmlan_buffer)util_peek_list(pmadapter->pmoal_handle,
						     &pra_list->buf_head, MNULL,
						     MNULL);
		priv->msdu_in_tx_amsdu_cnt++;
		msdu_in_tx_amsdu_cnt++;
	}

	pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
					      priv->wmm.ra_list_spinlock);

	/* Last AMSDU packet does not need padding */
	pkt_size -= pad;
	pmbuf_aggr->data_len = pkt_size;
	wlan_11n_update_pktlen_amsdu_txpd(priv, pmbuf_aggr);
	pmbuf_aggr->data_len += headroom;
	pmbuf_aggr->pbuf = data - headroom;
	tx_param.next_pkt_len =
		((pmbuf_src) ? pmbuf_src->data_len + Tx_PD_SIZEOF(pmadapter) :
			       0);
	/* Collects TP statistics */
	if (pmadapter->tp_state_on) {
		pmadapter->callbacks.moal_tp_accounting(pmadapter->pmoal_handle,
							pmbuf_aggr, 4);
		pmadapter->callbacks.moal_tp_accounting_rx_param(
			pmadapter->pmoal_handle, 5, msdu_in_tx_amsdu_cnt);
	}

	/* Drop Tx packets at drop point 4 */
	if (pmadapter->tp_state_drop_point == 4) {
		wlan_write_data_complete(pmadapter, pmbuf_aggr, ret);
		goto exit;
	} else
		ret = pmadapter->ops.host_to_card(priv, MLAN_TYPE_DATA,
						  pmbuf_aggr, &tx_param);
	switch (ret) {
#ifdef USB
	case MLAN_STATUS_PRESOURCE:
		PRINTM(MINFO, "MLAN_STATUS_PRESOURCE is returned\n");
		break;
#endif
	case MLAN_STATUS_RESOURCE:
		pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
						    priv->wmm.ra_list_spinlock);

		if (!wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
			pmadapter->callbacks.moal_spin_unlock(
				pmadapter->pmoal_handle,
				priv->wmm.ra_list_spinlock);
			pmbuf_aggr->status_code = MLAN_ERROR_PKT_INVALID;
			wlan_write_data_complete(pmadapter, pmbuf_aggr,
						 MLAN_STATUS_FAILURE);
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}
#ifdef STA_SUPPORT
		/* reset tx_lock_flag */
		if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
		    pmadapter->pps_uapsd_mode &&
		    (pmadapter->tx_lock_flag == MTRUE)) {
			pmadapter->tx_lock_flag = MFALSE;
			if (ptx_pd != MNULL)
				ptx_pd->flags = 0;
		}
#endif
		util_enqueue_list_head(pmadapter->pmoal_handle,
				       &pra_list->buf_head,
				       (pmlan_linked_list)pmbuf_aggr, MNULL,
				       MNULL);

		pra_list->total_pkts++;

		/* add back only one: aggregated packet is requeued as one */
		priv->wmm.pkts_queued[ptrindex]++;
		util_scalar_increment(pmadapter->pmoal_handle,
				      &priv->wmm.tx_pkts_queued, MNULL, MNULL);
		pmbuf_aggr->flags |= MLAN_BUF_FLAG_REQUEUED_PKT;
		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
		PRINTM(MINFO, "MLAN_STATUS_RESOURCE is returned\n");
		pmbuf_aggr->status_code = MLAN_ERROR_PKT_INVALID;
		break;
	case MLAN_STATUS_FAILURE:
		pmbuf_aggr->status_code = MLAN_ERROR_DATA_TX_FAIL;
		pmadapter->dbg.num_tx_host_to_card_failure++;
		wlan_write_data_complete(pmadapter, pmbuf_aggr, ret);
		goto exit;
	case MLAN_STATUS_PENDING:
		break;
	case MLAN_STATUS_SUCCESS:
		wlan_write_data_complete(pmadapter, pmbuf_aggr, ret);
		break;
	default:
		break;
	}
	if (ret != MLAN_STATUS_RESOURCE) {
		pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
						    priv->wmm.ra_list_spinlock);
		if (wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
			priv->wmm.packets_out[ptrindex]++;
			priv->wmm.tid_tbl_ptr[ptrindex].ra_list_curr = pra_list;
		}

		wlan_advance_bss_on_pkt_push(
			pmadapter, &pmadapter->bssprio_tbl[priv->bss_priority]);

		pmadapter->callbacks.moal_spin_unlock(
			pmadapter->pmoal_handle, priv->wmm.ra_list_spinlock);
	}
	PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
	PRINTM_NETINTF(MDATA, priv);
	PRINTM(MDATA, "%lu.%06lu : Data => FW\n", sec, usec);
	priv->amsdu_tx_cnt++;

exit:
	LEAVE();
	return MIN((pkt_size + headroom), INT_MAX);
}
