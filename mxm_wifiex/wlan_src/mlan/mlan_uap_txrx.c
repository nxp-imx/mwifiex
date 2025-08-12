/** @file mlan_uap_txrx.c
 *
 *  @brief This file contains AP mode transmit and receive functions
 *
 *
 *  Copyright 2009-2021 NXP
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
    02/05/2009: initial version
********************************************************/

#include "mlan.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#ifdef STA_SUPPORT
#include "mlan_join.h"
#endif
#include "mlan_main.h"
#include "mlan_uap.h"
#include "mlan_wmm.h"
#include "mlan_11n_aggr.h"
#include "mlan_11n_rxreorder.h"

/********************************************************
			Local Functions
********************************************************/

/**
 *  @brief This function processes received packet and forwards it
 *          to kernel/upper layer
 *
 *  @param pmadapter A pointer to mlan_adapter
 *  @param pmbuf     A pointer to mlan_buffer which includes the received packet
 *
 *  @return          MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_upload_uap_rx_packet(pmlan_adapter pmadapter,
					     pmlan_buffer pmbuf)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef DEBUG_LEVEL1
	pmlan_private priv = pmadapter->priv[pmbuf->bss_index];
#endif
	PRxPD prx_pd;
	ENTER();
	prx_pd = (RxPD *)(pmbuf->pbuf + pmbuf->data_offset);

	/* Chop off RxPD */
	if (pmbuf->data_len > prx_pd->rx_pkt_offset) {
		pmbuf->data_len -= prx_pd->rx_pkt_offset;
	} else {
		PRINTM(MERROR,
		       "pmbuf->data_len is smaller than prx_pd->rx_pkt_offset\n");
		pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
	}
	pmbuf->data_offset += prx_pd->rx_pkt_offset;
	pmbuf->pparent = MNULL;

	DBG_HEXDUMP(MDAT_D, "uAP RxPD", (t_u8 *)prx_pd,
		    MIN(sizeof(RxPD), MAX_DATA_DUMP_LEN));
	DBG_HEXDUMP(MDAT_D, "uAP Rx Payload",
		    ((t_u8 *)prx_pd + prx_pd->rx_pkt_offset),
		    MIN(prx_pd->rx_pkt_length, MAX_DATA_DUMP_LEN));

	pmadapter->callbacks.moal_get_system_time(pmadapter->pmoal_handle,
						  &pmbuf->out_ts_sec,
						  &pmbuf->out_ts_usec);
	PRINTM_NETINTF(MDATA, priv);
	PRINTM(MDATA, "%lu.%06lu : Data => kernel seq_num=%d tid=%d\n",
	       pmbuf->out_ts_sec, pmbuf->out_ts_usec, prx_pd->seq_num,
	       prx_pd->priority);
	ret = pmadapter->callbacks.moal_recv_packet(pmadapter->pmoal_handle,
						    pmbuf);
	if (ret == MLAN_STATUS_FAILURE) {
		PRINTM(MERROR,
		       "uAP Rx Error: moal_recv_packet returned error\n");
		pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
	}
	if (ret != MLAN_STATUS_PENDING)
		pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
#ifdef USB
	else if (IS_USB(pmadapter->card_type))
		pmadapter->callbacks.moal_recv_complete(pmadapter->pmoal_handle,
							MNULL,
							pmadapter->rx_data_ep,
							ret);
#endif
	LEAVE();

	return ret;
}

/**
 *  @brief This function will check if unicast packet need be dropped
 *
 *  @param priv    A pointer to mlan_private
 *  @param mac     mac address to find in station list table
 *
 *  @return	       MLAN_STATUS_FAILURE -- drop packet, otherwise forward to
 * network stack
 */
static mlan_status wlan_check_unicast_packet(mlan_private *priv, t_u8 *mac)
{
	int j;
	sta_node *sta_ptr = MNULL;
	pmlan_adapter pmadapter = priv->adapter;
	pmlan_private pmpriv = MNULL;
	t_u8 pkt_type = 0;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	ENTER();
	for (j = 0; j < MLAN_MAX_BSS_NUM; ++j) {
		pmpriv = pmadapter->priv[j];
		if (pmpriv) {
			if (GET_BSS_ROLE(pmpriv) == MLAN_BSS_ROLE_STA)
				continue;
			sta_ptr = wlan_get_station_entry(pmpriv, mac);
			if (sta_ptr) {
				if (pmpriv == priv)
					pkt_type = PKT_INTRA_UCAST;
				else
					pkt_type = PKT_INTER_UCAST;
				break;
			}
		}
	}
	if ((pkt_type == PKT_INTRA_UCAST) &&
	    (priv->pkt_fwd & PKT_FWD_INTRA_UCAST)) {
		PRINTM(MDATA, "Drop INTRA_UCAST packet\n");
		ret = MLAN_STATUS_FAILURE;
	} else if ((pkt_type == PKT_INTER_UCAST) &&
		   (priv->pkt_fwd & PKT_FWD_INTER_UCAST)) {
		PRINTM(MDATA, "Drop INTER_UCAST packet\n");
		ret = MLAN_STATUS_FAILURE;
	}
	LEAVE();
	return ret;
}
/********************************************************
			Global Functions
********************************************************/
/**
 *  @brief This function fill the txpd for tx packet
 *
 *  @param priv	   A pointer to mlan_private structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *
 *  @return        headptr or MNULL
 */
t_void *wlan_ops_uap_process_txpd(t_void *priv, pmlan_buffer pmbuf)
{
	pmlan_private pmpriv = (pmlan_private)priv;
	TxPD *plocal_tx_pd;
	t_u8 *head_ptr = MNULL;
	t_u32 pkt_type;
	t_u32 tx_control;
	t_u8 dst_mac[MLAN_MAC_ADDR_LENGTH];

	ENTER();

	if (!pmbuf->data_len) {
		PRINTM(MERROR, "uAP Tx Error: Invalid packet length: %d\n",
		       pmbuf->data_len);
		pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
		goto done;
	}
	if (pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA) {
		memcpy_ext(pmpriv->adapter, &pkt_type,
			   pmbuf->pbuf + pmbuf->data_offset, sizeof(pkt_type),
			   sizeof(pkt_type));
		memcpy_ext(pmpriv->adapter, &tx_control,
			   pmbuf->pbuf + pmbuf->data_offset + sizeof(pkt_type),
			   sizeof(tx_control), sizeof(tx_control));
		pmbuf->data_offset += sizeof(pkt_type) + sizeof(tx_control);
		pmbuf->data_len -= sizeof(pkt_type) + sizeof(tx_control);
	}
	if (pmbuf->data_offset <
	    (sizeof(TxPD) + pmpriv->intf_hr_len + DMA_ALIGNMENT)) {
		PRINTM(MERROR,
		       "not enough space for TxPD: headroom=%d pkt_len=%d, required=%d\n",
		       pmbuf->data_offset, pmbuf->data_len,
		       sizeof(TxPD) + pmpriv->intf_hr_len + DMA_ALIGNMENT);
		DBG_HEXDUMP(MDAT_D, "drop pkt",
			    pmbuf->pbuf + pmbuf->data_offset, pmbuf->data_len);
		pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
		goto done;
	}

	/* head_ptr should be aligned */
	head_ptr = pmbuf->pbuf + pmbuf->data_offset - sizeof(TxPD) -
		   pmpriv->intf_hr_len;
	head_ptr = (t_u8 *)((t_ptr)head_ptr & ~((t_ptr)(DMA_ALIGNMENT - 1)));

	plocal_tx_pd = (TxPD *)(head_ptr + pmpriv->intf_hr_len);
	memset(pmpriv->adapter, plocal_tx_pd, 0, sizeof(TxPD));

	/* Set the BSS number to TxPD */
	plocal_tx_pd->bss_num = GET_BSS_NUM(pmpriv);
	plocal_tx_pd->bss_type = pmpriv->bss_type;

	plocal_tx_pd->tx_pkt_length = (t_u16)pmbuf->data_len;

	plocal_tx_pd->priority = (t_u8)pmbuf->priority;
	plocal_tx_pd->pkt_delay_2ms =
		wlan_wmm_compute_driver_packet_delay(pmpriv, pmbuf);

	if (plocal_tx_pd->priority <
	    NELEMENTS(pmpriv->wmm.user_pri_pkt_tx_ctrl))
		/*
		 * Set the priority specific tx_control field, setting of 0 will
		 *   cause the default value to be used later in this function
		 */
		plocal_tx_pd->tx_control =
			pmpriv->wmm.user_pri_pkt_tx_ctrl[plocal_tx_pd->priority];

	if (pmbuf->flags & MLAN_BUF_FLAG_TX_STATUS) {
		plocal_tx_pd->tx_control_1 |= pmbuf->tx_seq_num << 8;
		plocal_tx_pd->flags |= MRVDRV_TxPD_FLAGS_TX_PACKET_STATUS;
	}

	/* Offset of actual data */
	plocal_tx_pd->tx_pkt_offset =
		(t_u16)((t_ptr)pmbuf->pbuf + pmbuf->data_offset -
			(t_ptr)plocal_tx_pd);

	if (!plocal_tx_pd->tx_control) {
		/* TxCtrl set by user or default */
		plocal_tx_pd->tx_control = pmpriv->pkt_tx_ctrl;
	}

	if (pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA) {
		plocal_tx_pd->tx_pkt_type = (t_u16)pkt_type;
		plocal_tx_pd->tx_control = tx_control;
	}
	if (pmbuf->flags & MLAN_BUF_FLAG_EASYMESH) {
		plocal_tx_pd->flags |= MRVDRV_TxPD_FLAGS_EASYMESH;
		memcpy_ext(pmpriv->adapter, plocal_tx_pd->ra_mac, pmbuf->mac,
			   MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);
	}

	if (pmbuf->flags & MLAN_BUF_FLAG_TX_CTRL) {
		if (pmbuf->u.tx_info.data_rate) {
			memcpy_ext(pmpriv->adapter, dst_mac,
				   pmbuf->pbuf + pmbuf->data_offset,
				   sizeof(dst_mac), sizeof(dst_mac));
			plocal_tx_pd->tx_control |=
				(wlan_ieee_rateid_to_mrvl_rateid(
					 pmpriv, pmbuf->u.tx_info.data_rate,
					 dst_mac)
				 << 16);
			plocal_tx_pd->tx_control |= TXPD_TXRATE_ENABLE;
		}
		plocal_tx_pd->tx_control_1 |= pmbuf->u.tx_info.channel << 21;
		if (pmbuf->u.tx_info.bw) {
			plocal_tx_pd->tx_control_1 |= pmbuf->u.tx_info.bw << 16;
			plocal_tx_pd->tx_control_1 |= TXPD_BW_ENABLE;
		}
		if (pmbuf->u.tx_info.tx_power.tp.hostctl)
			plocal_tx_pd->tx_control |=
				(t_u32)pmbuf->u.tx_info.tx_power.val;
		if (pmbuf->u.tx_info.retry_limit) {
			plocal_tx_pd->tx_control |= pmbuf->u.tx_info.retry_limit
						    << 8;
			plocal_tx_pd->tx_control |= TXPD_RETRY_ENABLE;
		}
	}
	if (pmbuf->flags & MLAN_BUF_FLAG_MC_AGGR_PKT) {
		tx_ctrl *ctrl = (tx_ctrl *)&plocal_tx_pd->tx_control;
		mc_tx_ctrl *mc_ctrl =
			(mc_tx_ctrl *)&plocal_tx_pd->pkt_delay_2ms;
		plocal_tx_pd->tx_pkt_type = PKT_TYPE_802DOT11_MC_AGGR;
		if (pmbuf->u.mc_tx_info.mc_pkt_flags & MC_FLAG_START_CYCLE)
			ctrl->mc_cycle_start = MTRUE;
		else
			ctrl->mc_cycle_start = MFALSE;
		if (pmbuf->u.mc_tx_info.mc_pkt_flags & MC_FLAG_END_CYCLE)
			ctrl->mc_cycle_end = MTRUE;
		else
			ctrl->mc_cycle_end = MFALSE;
		if (pmbuf->u.mc_tx_info.mc_pkt_flags & MC_FLAG_START_AMPDU)
			ctrl->mc_ampdu_start = MTRUE;
		else
			ctrl->mc_ampdu_start = MFALSE;
		if (pmbuf->u.mc_tx_info.mc_pkt_flags & MC_FLAG_END_AMPDU)
			ctrl->mc_ampdu_end = MTRUE;
		else
			ctrl->mc_ampdu_end = MFALSE;
		if (pmbuf->u.mc_tx_info.mc_pkt_flags & MC_FLAG_RETRY)
			ctrl->mc_pkt_retry = MTRUE;
		else
			ctrl->mc_pkt_retry = MFALSE;
		ctrl->bw = pmbuf->u.mc_tx_info.bandwidth & 0x7;
		ctrl->tx_rate = pmbuf->u.mc_tx_info.mcs_index & 0x1f;
		mc_ctrl->abs_tsf_expirytime =
			wlan_cpu_to_le32(pmbuf->u.mc_tx_info.pkt_expiry);
		mc_ctrl->mc_seq = wlan_cpu_to_le16(pmbuf->u.mc_tx_info.seq_num);
	}

	endian_convert_TxPD(plocal_tx_pd);

	/* Adjust the data offset and length to include TxPD in pmbuf */
	pmbuf->data_len += pmbuf->data_offset;
	pmbuf->data_offset = (t_u32)((t_ptr)head_ptr - (t_ptr)pmbuf->pbuf);
	pmbuf->data_len -= pmbuf->data_offset;

done:
	LEAVE();
	return head_ptr;
}

/**
 *  @brief This function processes received packet and forwards it
 *          to kernel/upper layer
 *
 *  @param adapter   A pointer to mlan_adapter
 *  @param pmbuf     A pointer to mlan_buffer which includes the received packet
 *
 *  @return          MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_ops_uap_process_rx_packet(t_void *adapter, pmlan_buffer pmbuf)
{
	pmlan_adapter pmadapter = (pmlan_adapter)adapter;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	RxPD *prx_pd;
	wlan_mgmt_pkt *puap_pkt_hdr = MNULL;

	RxPacketHdr_t *prx_pkt;
	pmlan_private priv = pmadapter->priv[pmbuf->bss_index];
	t_u8 ta[MLAN_MAC_ADDR_LENGTH];
	t_u16 rx_pkt_type = 0;
	sta_node *sta_ptr = MNULL;
	t_u16 adj_rx_rate = 0;
	t_u8 antenna = 0;

	t_u32 last_rx_sec = 0;
	t_u32 last_rx_usec = 0;
	RxPD *prx_pd2;
	EthII_Hdr_t *peth_hdr2;
	wlan_802_11_header *pwlan_hdr;
	IEEEtypes_FrameCtl_t *frmctl;
	pmlan_buffer pmbuf2 = MNULL;
	mlan_802_11_mac_addr src_addr = {0x00};
	mlan_802_11_mac_addr dest_addr = {0x00};
	t_u16 hdr_len;
	t_u8 snap_eth_hdr[5] = {0xaa, 0xaa, 0x03, 0x00, 0x00};
	t_u8 ext_rate_info = 0;

	ENTER();

	prx_pd = (RxPD *)(pmbuf->pbuf + pmbuf->data_offset);
	/* Endian conversion */
	endian_convert_RxPD(prx_pd);

	if (prx_pd->flags & RXPD_FLAG_EXTRA_HEADER) {
		endian_convert_RxPD_extra_header(
			(rxpd_extra_info *)((t_u8 *)prx_pd + sizeof(*prx_pd)));
	}

	if (priv->adapter->pcard_info->v14_fw_api) {
		t_u8 rxpd_rate_info_orig = prx_pd->rate_info;
		prx_pd->rate_info = wlan_convert_v14_rx_rate_info(
			priv, rxpd_rate_info_orig);
		PRINTM(MINFO,
		       "UAP RX: v14_fw_api=%d rx_rate =%d rxpd_rate_info=0x%x->0x%x\n",
		       priv->adapter->pcard_info->v14_fw_api, prx_pd->rx_rate,
		       rxpd_rate_info_orig, prx_pd->rate_info);
	}

	if (priv->rx_pkt_info) {
		ext_rate_info = (t_u8)(prx_pd->rx_info >> 16);
		pmbuf->u.rx_info.data_rate =
			wlan_index_to_data_rate(priv->adapter, prx_pd->rx_rate,
						prx_pd->rate_info,
						ext_rate_info);
		pmbuf->u.rx_info.channel =
			(prx_pd->rx_info & RXPD_CHAN_MASK) >> 5;
		pmbuf->u.rx_info.antenna = prx_pd->antenna;
		pmbuf->u.rx_info.rssi = prx_pd->snr - prx_pd->nf;
	}

	rx_pkt_type = prx_pd->rx_pkt_type;
	if (prx_pd->flags & RXPD_FLAG_PKT_EASYMESH) {
		PRINTM_NETINTF(MDAT_D, priv);
		PRINTM(MDAT_D, "UAP Rx Easymesh pkt flags : 0x%x\n",
		       prx_pd->flags);
		ret = wlan_check_easymesh_pkt(priv, pmbuf, prx_pd);
		if (ret != MLAN_STATUS_SUCCESS)
			goto done;
	}
	prx_pkt = (RxPacketHdr_t *)((t_u8 *)prx_pd + prx_pd->rx_pkt_offset);

	PRINTM(MINFO,
	       "RX Data: data_len - prx_pd->rx_pkt_offset = %d - %d = %d\n",
	       pmbuf->data_len, prx_pd->rx_pkt_offset,
	       pmbuf->data_len - prx_pd->rx_pkt_offset);

	if ((prx_pd->rx_pkt_offset + prx_pd->rx_pkt_length) !=
	    (t_u16)pmbuf->data_len) {
		PRINTM(MERROR,
		       "Wrong rx packet: len=%d,rx_pkt_offset=%d,"
		       " rx_pkt_length=%d\n",
		       pmbuf->data_len, prx_pd->rx_pkt_offset,
		       prx_pd->rx_pkt_length);
		pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
		ret = MLAN_STATUS_FAILURE;
		pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
		goto done;
	}
	pmbuf->data_len = prx_pd->rx_pkt_offset + prx_pd->rx_pkt_length;

	if (pmadapter->priv[pmbuf->bss_index]->mgmt_frame_passthru_mask &&
	    prx_pd->rx_pkt_type == PKT_TYPE_MGMT_FRAME) {
		/* Check if this is mgmt packet and needs to
		 * forwarded to app as an event
		 */
		puap_pkt_hdr = (wlan_mgmt_pkt *)((t_u8 *)prx_pd +
						 prx_pd->rx_pkt_offset);
		puap_pkt_hdr->frm_len = wlan_le16_to_cpu(puap_pkt_hdr->frm_len);
		if ((puap_pkt_hdr->wlan_header.frm_ctl &
		     IEEE80211_FC_MGMT_FRAME_TYPE_MASK) == 0)
			wlan_process_802dot11_mgmt_pkt(
				pmadapter->priv[pmbuf->bss_index],
				(t_u8 *)&puap_pkt_hdr->wlan_header,
				puap_pkt_hdr->frm_len + sizeof(wlan_mgmt_pkt) -
					sizeof(puap_pkt_hdr->frm_len),
				(RxPD *)prx_pd);
		pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
		goto done;
	}
	if (pmadapter->enable_net_mon &&
	    (prx_pd->flags & RXPD_FLAG_UCAST_PKT)) {
		pwlan_hdr = (wlan_802_11_header *)((t_u8 *)prx_pd +
						   prx_pd->rx_pkt_offset);
		frmctl = (IEEEtypes_FrameCtl_t *)pwlan_hdr;
		if (frmctl->type == 0x02) {
			/* This is a valid unicast destined data packet, with
			 * 802.11 and rtap headers attached. Duplicate this
			 * packet and process this copy as a sniffed packet,
			 * meant for monitor iface
			 */
			pmbuf2 = wlan_alloc_mlan_buffer(pmadapter,
							MLAN_RX_DATA_BUF_SIZE,
							MLAN_RX_HEADER_LEN,
							MOAL_ALLOC_MLAN_BUFFER);
			if (!pmbuf2) {
				PRINTM(MERROR,
				       "Unable to allocate mlan_buffer for Rx");
				PRINTM(MERROR, "sniffed packet\n");
			} else {
				pmbuf2->bss_index = pmbuf->bss_index;
				pmbuf2->buf_type = pmbuf->buf_type;
				pmbuf2->priority = pmbuf->priority;
				pmbuf2->in_ts_sec = pmbuf->in_ts_sec;
				pmbuf2->in_ts_usec = pmbuf->in_ts_usec;
				pmbuf2->data_len = pmbuf->data_len;
				memcpy(pmadapter,
				       pmbuf2->pbuf + pmbuf2->data_offset,
				       pmbuf->pbuf + pmbuf->data_offset,
				       pmbuf->data_len);

				prx_pd2 = (RxPD *)(pmbuf2->pbuf +
						   pmbuf2->data_offset);
				/* set pkt type of duplicated pkt to 802.11 */
				prx_pd2->rx_pkt_type = PKT_TYPE_802DOT11;
				wlan_process_uap_rx_packet(priv, pmbuf2);
			}

			/* Now, process this pkt as a normal data packet.
			 * rx_pkt_offset points to the 802.11 hdr. Construct
			 * 802.3 header from 802.11 hdr fields and attach it
			 * just before the payload.
			 */
			memcpy(pmadapter, (t_u8 *)&dest_addr, pwlan_hdr->addr1,
			       sizeof(pwlan_hdr->addr1));
			memcpy(pmadapter, (t_u8 *)&src_addr, pwlan_hdr->addr2,
			       sizeof(pwlan_hdr->addr2));

			hdr_len = sizeof(wlan_802_11_header);

			/* subtract mac addr field size for 3 address mac80211
			 * header */
			if (!(frmctl->from_ds && frmctl->to_ds))
				hdr_len -= sizeof(mlan_802_11_mac_addr);

			/* add 2 bytes of qos ctrl flags */
			if (frmctl->sub_type & QOS_DATA)
				hdr_len += 2;

			if (prx_pd->rx_pkt_type == PKT_TYPE_AMSDU) {
				/* no need to generate 802.3 hdr, update pkt
				 * offset */
				prx_pd->rx_pkt_offset += hdr_len;
				prx_pd->rx_pkt_length -= hdr_len;
			} else {
				/* skip 6-byte snap and 2-byte type */
				if (memcmp(pmadapter,
					   (t_u8 *)pwlan_hdr + hdr_len,
					   snap_eth_hdr,
					   sizeof(snap_eth_hdr)) == 0)
					hdr_len += 8;

				peth_hdr2 =
					(EthII_Hdr_t *)((t_u8 *)prx_pd +
							prx_pd->rx_pkt_offset +
							hdr_len -
							sizeof(EthII_Hdr_t));
				memcpy(pmadapter, peth_hdr2->dest_addr,
				       (t_u8 *)&dest_addr,
				       sizeof(peth_hdr2->dest_addr));
				memcpy(pmadapter, peth_hdr2->src_addr,
				       (t_u8 *)&src_addr,
				       sizeof(peth_hdr2->src_addr));

				/* Update the rx_pkt_offset to point the 802.3
				 * hdr */
				prx_pd->rx_pkt_offset +=
					(hdr_len - sizeof(EthII_Hdr_t));
				prx_pd->rx_pkt_length -=
					(hdr_len - sizeof(EthII_Hdr_t));
			}
			/* update the prx_pkt pointer */
			prx_pkt = (RxPacketHdr_t *)((t_u8 *)prx_pd +
						    prx_pd->rx_pkt_offset);
		} else {
			pmbuf->status_code = MLAN_ERROR_PKT_SIZE_INVALID;
			ret = MLAN_STATUS_FAILURE;
			PRINTM(MERROR,
			       "Drop invalid unicast sniffer pkt, subType=0x%x, flag=0x%x, pkt_type=%d\n",
			       frmctl->sub_type, prx_pd->flags,
			       prx_pd->rx_pkt_type);
			wlan_free_mlan_buffer(pmadapter, pmbuf);
			goto done;
		}
	}

	if (rx_pkt_type != PKT_TYPE_BAR) {
		priv->rxpd_rate = prx_pd->rx_rate;
		priv->rxpd_rate_info = prx_pd->rate_info;
		priv->rxpd_rx_info = (t_u8)(prx_pd->rx_info >> 16);

		if (priv->bss_type == MLAN_BSS_TYPE_UAP) {
			antenna = wlan_adjust_antenna(priv, (RxPD *)prx_pd);
			adj_rx_rate = wlan_adjust_data_rate(
				priv, priv->rxpd_rate, priv->rxpd_rate_info);
			pmadapter->callbacks.moal_hist_data_add(
				pmadapter->pmoal_handle, pmbuf->bss_index,
				adj_rx_rate, prx_pd->snr, prx_pd->nf, antenna);
		}
	}

	if (prx_pd->flags & RXPD_FLAG_PKT_EASYMESH)
		sta_ptr = wlan_get_station_entry(priv, prx_pd->ta_mac);
	else
		sta_ptr = wlan_get_station_entry(priv,
						 prx_pkt->eth803_hdr.src_addr);
	if (sta_ptr) {
		sta_ptr->snr = prx_pd->snr;
		sta_ptr->nf = prx_pd->nf;
		pmadapter->callbacks.moal_get_system_time(
			pmadapter->pmoal_handle, &last_rx_sec, &last_rx_usec);
		sta_ptr->stats.last_rx_in_msec =
			(t_u64)last_rx_sec * 1000 + (t_u64)last_rx_usec / 1000;
		if (rx_pkt_type != PKT_TYPE_BAR) {
			sta_ptr->stats.rx_packets++;
			sta_ptr->stats.rx_bytes += prx_pd->rx_pkt_length;
		}
	}

	pmbuf->priority |= prx_pd->priority;
	if (pmadapter->enable_net_mon &&
	    (prx_pd->rx_pkt_type == PKT_TYPE_802DOT11)) {
		wlan_process_uap_rx_packet(priv, pmbuf);
		goto done;
	}
	if (prx_pd->flags & RXPD_FLAG_PKT_EASYMESH)
		memcpy_ext(pmadapter, ta, prx_pd->ta_mac, MLAN_MAC_ADDR_LENGTH,
			   MLAN_MAC_ADDR_LENGTH);
	else
		memcpy_ext(pmadapter, ta, prx_pkt->eth803_hdr.src_addr,
			   MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);
	if ((rx_pkt_type != PKT_TYPE_BAR) && (prx_pd->priority < MAX_NUM_TID)) {
		sta_ptr = wlan_get_station_entry(priv, ta);
		if (sta_ptr) {
			sta_ptr->rx_seq[prx_pd->priority] = prx_pd->seq_num;
			sta_ptr->snr = prx_pd->snr;
			sta_ptr->nf = prx_pd->nf;
		}
	}
	/* check if UAP enable 11n */
	if ((!priv->is_11n_enabled && !priv->is_11ax_enabled) ||
	    (!wlan_11n_get_rxreorder_tbl((mlan_private *)priv, prx_pd->priority,
					 ta) &&
	     (prx_pd->rx_pkt_type != PKT_TYPE_AMSDU))) {
		if (priv->pkt_fwd)
			wlan_process_uap_rx_packet(priv, pmbuf);
		else
			wlan_upload_uap_rx_packet(pmadapter, pmbuf);
		goto done;
	}
	/* Reorder and send to OS */
	ret = mlan_11n_rxreorder_pkt(priv, prx_pd->seq_num, prx_pd->priority,
				     ta, (t_u8)prx_pd->rx_pkt_type,
				     (void *)pmbuf);
	if (ret || (rx_pkt_type == PKT_TYPE_BAR)) {
		pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
	}
done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function processes received packet and forwards it
 *          to kernel/upper layer or send back to firmware
 *
 *  @param priv      A pointer to mlan_private
 *  @param pmbuf     A pointer to mlan_buffer which includes the received packet
 *
 *  @return          MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_uap_recv_packet(mlan_private *priv, pmlan_buffer pmbuf)
{
	pmlan_adapter pmadapter = priv->adapter;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	RxPacketHdr_t *prx_pkt;
	pmlan_buffer newbuf = MNULL;

	ENTER();

	prx_pkt = (RxPacketHdr_t *)((t_u8 *)pmbuf->pbuf + pmbuf->data_offset);

	DBG_HEXDUMP(MDAT_D, "uap_recv_packet", pmbuf->pbuf + pmbuf->data_offset,
		    MIN(pmbuf->data_len, MAX_DATA_DUMP_LEN));

	PRINTM(MDATA, "AMSDU dest " MACSTR "\n",
	       MAC2STR(prx_pkt->eth803_hdr.dest_addr));

	/* don't do packet forwarding in disconnected state */
	if (priv->media_connected == MFALSE)
		goto upload;

	if (prx_pkt->eth803_hdr.dest_addr[0] & 0x01) {
		if (!(priv->pkt_fwd & PKT_FWD_INTRA_BCAST)) {
			/* Multicast pkt */
			newbuf =
				wlan_alloc_mlan_buffer(pmadapter,
						       MLAN_TX_DATA_BUF_SIZE_2K,
						       0, MOAL_MALLOC_BUFFER);
			if (newbuf) {
				newbuf->bss_index = pmbuf->bss_index;
				newbuf->buf_type = pmbuf->buf_type;
				newbuf->priority = pmbuf->priority;
				newbuf->in_ts_sec = pmbuf->in_ts_sec;
				newbuf->in_ts_usec = pmbuf->in_ts_usec;
				newbuf->data_offset =
					(sizeof(TxPD) + priv->intf_hr_len +
					 DMA_ALIGNMENT);
				util_scalar_increment(
					pmadapter->pmoal_handle,
					&pmadapter->pending_bridge_pkts,
					pmadapter->callbacks.moal_spin_lock,
					pmadapter->callbacks.moal_spin_unlock);

				newbuf->flags |= MLAN_BUF_FLAG_BRIDGE_BUF;

				/* copy the data */
				memcpy_ext(pmadapter,
					   (t_u8 *)newbuf->pbuf +
						   newbuf->data_offset,
					   pmbuf->pbuf + pmbuf->data_offset,
					   pmbuf->data_len,
					   MLAN_TX_DATA_BUF_SIZE_2K);
				newbuf->data_len = pmbuf->data_len;
				wlan_wmm_add_buf_txqueue(pmadapter, newbuf);
				if (util_scalar_read(
					    pmadapter->pmoal_handle,
					    &pmadapter->pending_bridge_pkts,
					    pmadapter->callbacks.moal_spin_lock,
					    pmadapter->callbacks
						    .moal_spin_unlock) >
				    RX_HIGH_THRESHOLD)
					wlan_drop_tx_pkts(priv);
				wlan_recv_event(
					priv, MLAN_EVENT_ID_DRV_DEFER_HANDLING,
					MNULL);
			}
		}
	} else {
		if ((!(priv->pkt_fwd & PKT_FWD_INTRA_UCAST)) &&
		    (wlan_get_station_entry(priv,
					    prx_pkt->eth803_hdr.dest_addr))) {
			/* Intra BSS packet */
			newbuf =
				wlan_alloc_mlan_buffer(pmadapter,
						       MLAN_TX_DATA_BUF_SIZE_2K,
						       0, MOAL_MALLOC_BUFFER);
			if (newbuf) {
				newbuf->bss_index = pmbuf->bss_index;
				newbuf->buf_type = pmbuf->buf_type;
				newbuf->priority = pmbuf->priority;
				newbuf->in_ts_sec = pmbuf->in_ts_sec;
				newbuf->in_ts_usec = pmbuf->in_ts_usec;
				newbuf->data_offset =
					(sizeof(TxPD) + priv->intf_hr_len +
					 DMA_ALIGNMENT);
				util_scalar_increment(
					pmadapter->pmoal_handle,
					&pmadapter->pending_bridge_pkts,
					pmadapter->callbacks.moal_spin_lock,
					pmadapter->callbacks.moal_spin_unlock);
				newbuf->flags |= MLAN_BUF_FLAG_BRIDGE_BUF;

				/* copy the data */
				memcpy_ext(pmadapter,
					   (t_u8 *)newbuf->pbuf +
						   newbuf->data_offset,
					   pmbuf->pbuf + pmbuf->data_offset,
					   pmbuf->data_len,
					   MLAN_TX_DATA_BUF_SIZE_2K);
				newbuf->data_len = pmbuf->data_len;
				wlan_wmm_add_buf_txqueue(pmadapter, newbuf);
				if (util_scalar_read(
					    pmadapter->pmoal_handle,
					    &pmadapter->pending_bridge_pkts,
					    pmadapter->callbacks.moal_spin_lock,
					    pmadapter->callbacks
						    .moal_spin_unlock) >
				    RX_HIGH_THRESHOLD)
					wlan_drop_tx_pkts(priv);
				wlan_recv_event(
					priv, MLAN_EVENT_ID_DRV_DEFER_HANDLING,
					MNULL);
			}
			goto done;
		} else if (MLAN_STATUS_FAILURE ==
			   wlan_check_unicast_packet(
				   priv, prx_pkt->eth803_hdr.dest_addr)) {
			/* drop packet */
			PRINTM(MDATA, "Drop AMSDU dest " MACSTR "\n",
			       MAC2STR(prx_pkt->eth803_hdr.dest_addr));
			goto done;
		}
	}
upload:
	/** send packet to moal */
	ret = pmadapter->callbacks.moal_recv_packet(pmadapter->pmoal_handle,
						    pmbuf);
done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function processes received packet and forwards it
 *          to kernel/upper layer or send back to firmware
 *
 *  @param priv      A pointer to mlan_private
 *  @param pmbuf     A pointer to mlan_buffer which includes the received packet
 *
 *  @return          MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_process_uap_rx_packet(mlan_private *priv, pmlan_buffer pmbuf)
{
	pmlan_adapter pmadapter = priv->adapter;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	RxPD *prx_pd;
	RxPacketHdr_t *prx_pkt;
	pmlan_buffer newbuf = MNULL;

	ENTER();

	prx_pd = (RxPD *)(pmbuf->pbuf + pmbuf->data_offset);
	prx_pkt = (RxPacketHdr_t *)((t_u8 *)prx_pd + prx_pd->rx_pkt_offset);

	DBG_HEXDUMP(MDAT_D, "uAP RxPD", prx_pd,
		    MIN(sizeof(RxPD), MAX_DATA_DUMP_LEN));
	DBG_HEXDUMP(MDAT_D, "uAP Rx Payload",
		    ((t_u8 *)prx_pd + prx_pd->rx_pkt_offset),
		    MIN(prx_pd->rx_pkt_length, MAX_DATA_DUMP_LEN));

	PRINTM(MINFO,
	       "RX Data: data_len - prx_pd->rx_pkt_offset = %d - %d = %d\n",
	       pmbuf->data_len, prx_pd->rx_pkt_offset,
	       pmbuf->data_len - prx_pd->rx_pkt_offset);
	PRINTM(MDATA, "Rx dest " MACSTR "\n",
	       MAC2STR(prx_pkt->eth803_hdr.dest_addr));

	if (pmadapter->enable_net_mon) {
		if (prx_pd->rx_pkt_type == PKT_TYPE_802DOT11) {
			pmbuf->flags |= MLAN_BUF_FLAG_NET_MONITOR;
			goto upload;
		}
	}

	/* don't do packet forwarding in disconnected state */
	/* don't do packet forwarding when packet > 1514 */
	if (priv->media_connected == MFALSE)
		goto upload;

	if (prx_pkt->eth803_hdr.dest_addr[0] & 0x01) {
		if (!(priv->pkt_fwd & PKT_FWD_INTRA_BCAST)) {
			/* Multicast pkt */
			newbuf =
				wlan_alloc_mlan_buffer(pmadapter,
						       MLAN_TX_DATA_BUF_SIZE_2K,
						       0, MOAL_MALLOC_BUFFER);
			if (newbuf) {
				newbuf->bss_index = pmbuf->bss_index;
				newbuf->buf_type = pmbuf->buf_type;
				newbuf->priority = pmbuf->priority;
				newbuf->in_ts_sec = pmbuf->in_ts_sec;
				newbuf->in_ts_usec = pmbuf->in_ts_usec;
				newbuf->data_offset =
					(sizeof(TxPD) + priv->intf_hr_len +
					 DMA_ALIGNMENT);
				util_scalar_increment(
					pmadapter->pmoal_handle,
					&pmadapter->pending_bridge_pkts,
					pmadapter->callbacks.moal_spin_lock,
					pmadapter->callbacks.moal_spin_unlock);
				newbuf->flags |= MLAN_BUF_FLAG_BRIDGE_BUF;

				/* copy the data, skip rxpd */
				memcpy_ext(pmadapter,
					   (t_u8 *)newbuf->pbuf +
						   newbuf->data_offset,
					   pmbuf->pbuf + pmbuf->data_offset +
						   prx_pd->rx_pkt_offset,
					   pmbuf->data_len -
						   prx_pd->rx_pkt_offset,
					   MLAN_TX_DATA_BUF_SIZE_2K);
				newbuf->data_len =
					pmbuf->data_len - prx_pd->rx_pkt_offset;
				wlan_wmm_add_buf_txqueue(pmadapter, newbuf);
				if (util_scalar_read(
					    pmadapter->pmoal_handle,
					    &pmadapter->pending_bridge_pkts,
					    pmadapter->callbacks.moal_spin_lock,
					    pmadapter->callbacks
						    .moal_spin_unlock) >
				    RX_HIGH_THRESHOLD)
					wlan_drop_tx_pkts(priv);
				wlan_recv_event(
					priv, MLAN_EVENT_ID_DRV_DEFER_HANDLING,
					MNULL);
			}
		}
	} else {
		if ((!(priv->pkt_fwd & PKT_FWD_INTRA_UCAST)) &&
		    (wlan_get_station_entry(priv,
					    prx_pkt->eth803_hdr.dest_addr))) {
			/* Forwarding Intra-BSS packet */
#ifdef USB
			if (IS_USB(pmadapter->card_type)) {
				if (pmbuf->flags & MLAN_BUF_FLAG_RX_DEAGGR) {
					newbuf = wlan_alloc_mlan_buffer(
						pmadapter,
						MLAN_TX_DATA_BUF_SIZE_2K, 0,
						MOAL_MALLOC_BUFFER);
					if (newbuf) {
						newbuf->bss_index =
							pmbuf->bss_index;
						newbuf->buf_type =
							pmbuf->buf_type;
						newbuf->priority =
							pmbuf->priority;
						newbuf->in_ts_sec =
							pmbuf->in_ts_sec;
						newbuf->in_ts_usec =
							pmbuf->in_ts_usec;
						newbuf->data_offset =
							(sizeof(TxPD) +
							 priv->intf_hr_len +
							 DMA_ALIGNMENT);
						util_scalar_increment(
							pmadapter->pmoal_handle,
							&pmadapter->pending_bridge_pkts,
							pmadapter->callbacks
								.moal_spin_lock,
							pmadapter->callbacks
								.moal_spin_unlock);
						newbuf->flags |=
							MLAN_BUF_FLAG_BRIDGE_BUF;

						/* copy the data, skip rxpd */
						memcpy_ext(
							pmadapter,
							(t_u8 *)newbuf->pbuf +
								newbuf->data_offset,
							pmbuf->pbuf +
								pmbuf->data_offset +
								prx_pd->rx_pkt_offset,
							pmbuf->data_len -
								prx_pd->rx_pkt_offset,
							pmbuf->data_len -
								prx_pd->rx_pkt_offset);
						newbuf->data_len =
							pmbuf->data_len -
							prx_pd->rx_pkt_offset;
						wlan_wmm_add_buf_txqueue(
							pmadapter, newbuf);
						if (util_scalar_read(
							    pmadapter->pmoal_handle,
							    &pmadapter->pending_bridge_pkts,
							    pmadapter->callbacks
								    .moal_spin_lock,
							    pmadapter->callbacks
								    .moal_spin_unlock) >
						    RX_HIGH_THRESHOLD)
							wlan_drop_tx_pkts(priv);
						wlan_recv_event(
							priv,
							MLAN_EVENT_ID_DRV_DEFER_HANDLING,
							MNULL);
					}
					pmadapter->callbacks.moal_recv_complete(
						pmadapter->pmoal_handle, pmbuf,
						pmadapter->rx_data_ep, ret);
					goto done;
				}
			}
#endif
			pmbuf->data_len -= prx_pd->rx_pkt_offset;
			pmbuf->data_offset += prx_pd->rx_pkt_offset;
			pmbuf->flags |= MLAN_BUF_FLAG_BRIDGE_BUF;
			util_scalar_increment(
				pmadapter->pmoal_handle,
				&pmadapter->pending_bridge_pkts,
				pmadapter->callbacks.moal_spin_lock,
				pmadapter->callbacks.moal_spin_unlock);
			wlan_wmm_add_buf_txqueue(pmadapter, pmbuf);
			if (util_scalar_read(
				    pmadapter->pmoal_handle,
				    &pmadapter->pending_bridge_pkts,
				    pmadapter->callbacks.moal_spin_lock,
				    pmadapter->callbacks.moal_spin_unlock) >
			    RX_HIGH_THRESHOLD)
				wlan_drop_tx_pkts(priv);
			wlan_recv_event(priv, MLAN_EVENT_ID_DRV_DEFER_HANDLING,
					MNULL);
			goto done;
		} else if (MLAN_STATUS_FAILURE ==
			   wlan_check_unicast_packet(
				   priv, prx_pkt->eth803_hdr.dest_addr)) {
			PRINTM(MDATA, "Drop Pkts: Rx dest " MACSTR "\n",
			       MAC2STR(prx_pkt->eth803_hdr.dest_addr));
			pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
			pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
			goto done;
		}
	}

upload:
	/* Chop off RxPD */
	pmbuf->data_len -= prx_pd->rx_pkt_offset;
	pmbuf->data_offset += prx_pd->rx_pkt_offset;
	pmbuf->pparent = MNULL;

	pmadapter->callbacks.moal_get_system_time(pmadapter->pmoal_handle,
						  &pmbuf->out_ts_sec,
						  &pmbuf->out_ts_usec);
	PRINTM_NETINTF(MDATA, priv);
	PRINTM(MDATA, "%lu.%06lu : Data => kernel seq_num=%d tid=%d\n",
	       pmbuf->out_ts_sec, pmbuf->out_ts_usec, prx_pd->seq_num,
	       prx_pd->priority);
	if (pmbuf->flags & MLAN_BUF_FLAG_NET_MONITOR) {
		// Use some rxpd space to save rxpd info for radiotap header
		// We should insure radiotap_info is not bigger than RxPD
		wlan_rxpdinfo_to_radiotapinfo(
			priv, (RxPD *)prx_pd,
			(radiotap_info *)(pmbuf->pbuf + pmbuf->data_offset -
					  sizeof(radiotap_info)));
	}

	ret = pmadapter->callbacks.moal_recv_packet(pmadapter->pmoal_handle,
						    pmbuf);
	if (ret == MLAN_STATUS_FAILURE) {
		PRINTM(MERROR,
		       "uAP Rx Error: moal_recv_packet returned error\n");
		pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
	}

	if (ret != MLAN_STATUS_PENDING)
		pmadapter->ops.data_complete(pmadapter, pmbuf, ret);
#ifdef USB
	else if (IS_USB(pmadapter->card_type))
		pmadapter->callbacks.moal_recv_complete(pmadapter->pmoal_handle,
							MNULL,
							pmadapter->rx_data_ep,
							ret);
#endif
done:
	LEAVE();
	return ret;
}
