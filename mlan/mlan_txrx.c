/**
 * @file mlan_txrx.c
 *
 *  @brief This file contains the handling of TX/RX in MLAN
 *
 *
 *  Copyright 2009-2021, 2024-2025 NXP
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

/*************************************************************
Change Log:
    05/11/2009: initial version
************************************************************/

#include "mlan.h"
#ifdef STA_SUPPORT
#include "mlan_join.h"
#endif
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_main.h"
#include "mlan_wmm.h"

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
 *   @brief This function processes the received buffer
 *
 *   @param pmadapter A pointer to mlan_adapter
 *   @param pmbuf     A pointer to the received buffer
 *
 *   @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status wlan_handle_rx_packet(pmlan_adapter pmadapter, pmlan_buffer pmbuf)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	pmlan_private priv = MNULL;
	RxPD *prx_pd;
#ifdef DEBUG_LEVEL1
	t_u32 sec = 0, usec = 0;
#endif

	ENTER();

	prx_pd = (RxPD *)(pmbuf->pbuf + pmbuf->data_offset);

	/* Get the BSS number from RxPD, get corresponding priv */
	priv = wlan_get_priv_by_id(pmadapter, prx_pd->bss_num & BSS_NUM_MASK,
				   prx_pd->bss_type);
	if (!priv)
		priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
	if (!priv) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	pmbuf->bss_index = priv->bss_index;
	PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
	PRINTM_NETINTF(MDATA, priv);
	PRINTM(MDATA, "%lu.%06lu : Data <= FW\n", sec, usec);
	ret = priv->ops.process_rx_packet(pmadapter, pmbuf);

done:
	LEAVE();
	return ret;
}
Stats_mcast_drv_t gmcast_stats = {0};
/* This flag is used to protect the mcast drv stat update
 * when it's value is copied to provide to mlanutl
 * MTRUE = Update is allowed
 * MFALSE = Update is not allowed
 */
t_u8 mcast_drv_update_allow_flag = MTRUE;
#define DEST_MAC_OFFSET 10
#define CYCLE_START 1
#define FIVE_SEC 5000000 /* 1000000 usec = 1sec*/
/**
 *  @brief This function calculates the cycle delta and driver time delta
 *  for Mcast packets
 *
 *  @param pmadapter    A pointer to pmlan_adapter structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *
 *  @return         Nothing
 */
static void wlan_drv_mcast_cycle_delay_calulation(pmlan_adapter pmadapter,
						  pmlan_buffer pmbuf)
{
	static t_u32 prev_mcast_sec = 0;
	static t_u32 prev_mcast_usec = 0;
	t_u32 curr_ts_sec = 0;
	t_u32 curr_ts_usec = 0;
	t_s64 cycle_delta = 0;
	t_s64 profile_delta = 0;

	if (mcast_drv_update_allow_flag == MFALSE)
		return;
	/* Take current time */
	if (pmadapter && pmadapter->pmoal_handle)
		pmadapter->callbacks.moal_get_system_time(
			pmadapter->pmoal_handle, &curr_ts_sec, &curr_ts_usec);
	else
		PRINTM(MERROR, "ERR: pmadapter or pmoal_handle NULL\n",
		       __func__);

	if (curr_ts_sec || curr_ts_usec) {
		/* Calculate profile delta */
		profile_delta = (curr_ts_sec - pmbuf->in_ts_sec) * 1000000;
		profile_delta += curr_ts_usec - pmbuf->in_ts_usec;

		if ((profile_delta >= 0) && (profile_delta <= 1000))
			gmcast_stats.spent_time_under_1000usec++;
		else if ((profile_delta > 1000) && (profile_delta <= 2000))
			gmcast_stats.spent_time_over_1000usec++;
		else if ((profile_delta > 2000) && (profile_delta <= 3000))
			gmcast_stats.spent_time_over_2000usec++;
		else if (profile_delta > 3000)
			gmcast_stats.spent_time_over_3000usec++;
	}
	/* Process the start cycle data */
	cycle_delta = ((t_s64)pmbuf->in_ts_sec - (t_s64)prev_mcast_sec) *
		      (t_s64)1000000;
	cycle_delta += pmbuf->in_ts_usec - prev_mcast_usec;

	/* If start cycle delta is more than 5 sec ignore*/
	if ((pmbuf->u.mc_tx_info.mc_pkt_flags & (1 << CYCLE_START)) &&
	    (cycle_delta < FIVE_SEC)) {
		if ((cycle_delta >= 0) && (cycle_delta <= 2300))
			gmcast_stats.cycle_recv_under_2300usec++;
		if ((cycle_delta > 2300) && (cycle_delta <= 2900))
			gmcast_stats.cycle_recv_in_time++;
		if ((cycle_delta > 2900) && (cycle_delta <= 3500))
			gmcast_stats.cycle_recv_over_2900usec++;
		if ((cycle_delta > 3500) && (cycle_delta <= 5000))
			gmcast_stats.cycle_recv_over_3500usec++;
		if ((cycle_delta > 5000) && (cycle_delta <= 10000))
			gmcast_stats.cycle_recv_over_5000usec++;
		if ((cycle_delta > 10000) && (cycle_delta <= 15000))
			gmcast_stats.cycle_recv_over_10000usec++;
		if (cycle_delta > 15000)
			gmcast_stats.cycle_recv_over_15000usec++;
	}
	/* Update the last received mcast cycle value */
	prev_mcast_sec = pmbuf->in_ts_sec;
	prev_mcast_usec = pmbuf->in_ts_usec;
}
/**
 *  @brief This function checks the conditions and sends packet to device
 *
 *  @param priv	   A pointer to mlan_private structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *  @param tx_param A pointer to mlan_tx_param structure
 *
 *  @return         MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise
 * failure
 */
mlan_status wlan_process_tx(pmlan_private priv, pmlan_buffer pmbuf,
			    mlan_tx_param *tx_param)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	pmlan_adapter pmadapter = priv->adapter;
	t_u8 *head_ptr = MNULL;
#ifdef DEBUG_LEVEL1
	t_u32 sec = 0, usec = 0;
#endif
#ifdef STA_SUPPORT
	PTxPD plocal_tx_pd = MNULL;
#endif
	t_u8 dest_mac_first_octet = 0;

	ENTER();
	head_ptr = (t_u8 *)priv->ops.process_txpd(priv, pmbuf);
	if (!head_ptr) {
		pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
#ifdef STA_SUPPORT
	if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
		plocal_tx_pd = (TxPD *)(head_ptr + priv->intf_hr_len);
#endif
	dest_mac_first_octet = *(head_ptr + priv->intf_hr_len +
				 Tx_PD_SIZEOF(pmadapter) + DEST_MAC_OFFSET);

	if (dest_mac_first_octet & 0x01)
		wlan_drv_mcast_cycle_delay_calulation(pmadapter, pmbuf);
	if (pmadapter->tp_state_on)
		pmadapter->callbacks.moal_tp_accounting(pmadapter->pmoal_handle,
							pmbuf, 4);
	if (pmadapter->tp_state_drop_point == 4)
		goto done;
	else {
		ret = pmadapter->ops.host_to_card(priv, MLAN_TYPE_DATA, pmbuf,
						  tx_param);
	}
done:
	switch (ret) {
#ifdef USB
	case MLAN_STATUS_PRESOURCE:
		PRINTM(MINFO, "MLAN_STATUS_PRESOURCE is returned\n");
		DBG_HEXDUMP(MDAT_D, "Tx", head_ptr + priv->intf_hr_len,
			    MIN(pmbuf->data_len + Tx_PD_SIZEOF(pmadapter),
				MAX_DATA_DUMP_LEN));
		break;
#endif
	case MLAN_STATUS_RESOURCE:
#ifdef STA_SUPPORT
		if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
		    pmadapter->pps_uapsd_mode &&
		    (pmadapter->tx_lock_flag == MTRUE)) {
			pmadapter->tx_lock_flag = MFALSE;
			if (plocal_tx_pd != MNULL)
				plocal_tx_pd->flags = 0;
		}
#endif
		PRINTM(MINFO, "MLAN_STATUS_RESOURCE is returned\n");
		break;
	case MLAN_STATUS_FAILURE:
		pmadapter->dbg.num_tx_host_to_card_failure++;
		pmbuf->status_code = MLAN_ERROR_DATA_TX_FAIL;
		wlan_write_data_complete(pmadapter, pmbuf, ret);
		break;
	case MLAN_STATUS_PENDING:
		DBG_HEXDUMP(MDAT_D, "Tx", head_ptr + priv->intf_hr_len,
			    MIN(pmbuf->data_len + Tx_PD_SIZEOF(pmadapter),
				MAX_DATA_DUMP_LEN));
		break;
	case MLAN_STATUS_SUCCESS:
		DBG_HEXDUMP(MDAT_D, "Tx", head_ptr + priv->intf_hr_len,
			    MIN(pmbuf->data_len + Tx_PD_SIZEOF(pmadapter),
				MAX_DATA_DUMP_LEN));
		wlan_write_data_complete(pmadapter, pmbuf, ret);
		break;
	default:
		break;
	}

	if ((ret == MLAN_STATUS_SUCCESS) || (ret == MLAN_STATUS_PENDING)) {
		PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
		PRINTM_NETINTF(MDATA, priv);
		PRINTM(MDATA, "%lu.%06lu : Data => FW\n", sec, usec);
	}
	LEAVE();
	return ret;
}

/**
 *  @brief Packet send completion handling
 *
 *  @param pmadapter		A pointer to mlan_adapter structure
 *  @param pmbuf		A pointer to mlan_buffer structure
 *  @param status		Callback status
 *
 *  @return			MLAN_STATUS_SUCCESS
 */
mlan_status wlan_write_data_complete(pmlan_adapter pmadapter,
				     pmlan_buffer pmbuf, mlan_status status)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	pmlan_callbacks pcb;

	ENTER();

	MASSERT(pmadapter && pmbuf);
	if (!pmadapter || !pmbuf) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	pcb = &pmadapter->callbacks;

	if ((pmbuf->buf_type == MLAN_BUF_TYPE_DATA) ||
	    (pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA)) {
		PRINTM(MINFO, "wlan_write_data_complete: DATA %p\n", pmbuf);
#if defined(USB)
		if ((pmbuf->flags & MLAN_BUF_FLAG_USB_TX_AGGR) &&
		    pmbuf->use_count) {
			pmlan_buffer pmbuf_next;
			t_u32 i, use_count = pmbuf->use_count;
			for (i = 0; i <= use_count; i++) {
				pmbuf_next = pmbuf->pnext;
				if (pmbuf->flags & MLAN_BUF_FLAG_MOAL_TX_BUF)
					pcb->moal_send_packet_complete(
						pmadapter->pmoal_handle, pmbuf,
						status);
				else
					wlan_free_mlan_buffer(pmadapter, pmbuf);
				pmbuf = pmbuf_next;
			}
		} else {
#endif
			if (pmbuf->flags & MLAN_BUF_FLAG_MOAL_TX_BUF) {
				/* pmbuf was allocated by MOAL */
				pcb->moal_send_packet_complete(
					pmadapter->pmoal_handle, pmbuf, status);
			} else {
				/* pmbuf was allocated by MLAN */
				wlan_free_mlan_buffer(pmadapter, pmbuf);
			}
#if defined(USB)
		}
#endif
	}

	LEAVE();
	return ret;
}

/**
 *  @brief Packet receive completion callback handler
 *
 *  @param pmadapter		A pointer to mlan_adapter structure
 *  @param pmbuf		A pointer to mlan_buffer structure
 *  @param status		Callback status
 *
 *  @return			MLAN_STATUS_SUCCESS
 */
mlan_status wlan_recv_packet_complete(pmlan_adapter pmadapter,
				      pmlan_buffer pmbuf, mlan_status status)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;

	ENTER();

	MASSERT(pmadapter && pmbuf);
	if (!pmadapter || !pmbuf) {
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	MASSERT(pmbuf->bss_index < pmadapter->priv_num);

	if (pmbuf->pparent) {
		/** we will free the pparaent at the end of deaggr */
		wlan_free_mlan_buffer(pmadapter, pmbuf);
	} else {
		pmadapter->ops.data_complete(pmadapter, pmbuf, status);
	}

	LEAVE();
	return ret;
}

/**
 *  @brief Add packet to Bypass TX queue
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *  @param pmbuf      Pointer to the mlan_buffer data struct
 *
 *  @return         N/A
 */
t_void wlan_add_buf_bypass_txqueue(mlan_adapter *pmadapter, pmlan_buffer pmbuf)
{
	pmlan_private priv = pmadapter->priv[pmbuf->bss_index];
	ENTER();

	if (pmbuf->buf_type != MLAN_BUF_TYPE_RAW_DATA)
		pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
	util_scalar_increment(pmadapter->pmoal_handle,
			      &pmadapter->bypass_pkt_count,
			      pmadapter->callbacks.moal_spin_lock,
			      pmadapter->callbacks.moal_spin_unlock);
	util_enqueue_list_tail(pmadapter->pmoal_handle, &priv->bypass_txq,
			       (pmlan_linked_list)pmbuf,
			       pmadapter->callbacks.moal_spin_lock,
			       pmadapter->callbacks.moal_spin_unlock);
	LEAVE();
}

/**
 *  @brief Check if packets are available in Bypass TX queue
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *
 *  @return         MFALSE if not empty; MTRUE if empty
 */
INLINE t_u8 wlan_bypass_tx_list_empty(mlan_adapter *pmadapter)
{
	t_u32 bypass_pkt_count;
#if defined(USB)
	if (IS_USB(pmadapter->card_type)) {
		pmlan_callbacks pcb = &pmadapter->callbacks;
		pmlan_private priv;
		int j = 0;
		for (j = 0; j < pmadapter->priv_num; ++j) {
			priv = pmadapter->priv[j];
			if (priv) {
				if (!wlan_is_port_ready(pmadapter,
							priv->port_index))
					continue;
				if (util_peek_list(pmadapter->pmoal_handle,
						   &priv->bypass_txq,
						   pcb->moal_spin_lock,
						   pcb->moal_spin_unlock))
					return MFALSE;
			}
		}
		return MTRUE;
	}
#endif
	bypass_pkt_count =
		util_scalar_read(pmadapter->pmoal_handle,
				 &pmadapter->bypass_pkt_count,
				 pmadapter->callbacks.moal_spin_lock,
				 pmadapter->callbacks.moal_spin_unlock);
	return (bypass_pkt_count) ? MFALSE : MTRUE;
}

/**
 *  @brief Clean up the By-pass TX queue
 *
 *  @param priv     Pointer to the mlan_private data struct
 *
 *  @return      N/A
 */
t_void wlan_cleanup_bypass_txq(mlan_private *priv)
{
	pmlan_buffer pmbuf;
	mlan_adapter *pmadapter = priv->adapter;
	ENTER();
	pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
					    priv->bypass_txq.plock);
	while ((pmbuf = (pmlan_buffer)util_peek_list(pmadapter->pmoal_handle,
						     &priv->bypass_txq, MNULL,
						     MNULL))) {
		util_unlink_list(pmadapter->pmoal_handle, &priv->bypass_txq,
				 (pmlan_linked_list)pmbuf, MNULL, MNULL);
		wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
		util_scalar_decrement(pmadapter->pmoal_handle,
				      &pmadapter->bypass_pkt_count,
				      pmadapter->callbacks.moal_spin_lock,
				      pmadapter->callbacks.moal_spin_unlock);
	}
	pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
					      priv->bypass_txq.plock);
	LEAVE();
}

/**
 *  @brief Transmit the By-passed packet awaiting in by-pass queue
 *
 *  @param pmadapter Pointer to the mlan_adapter driver data struct
 *
 *  @return        N/A
 */
t_void wlan_process_bypass_tx(pmlan_adapter pmadapter)
{
	pmlan_buffer pmbuf;
	mlan_tx_param tx_param;
	mlan_status status = MLAN_STATUS_SUCCESS;
	pmlan_private priv;
	int j = 0;
	ENTER();
	do {
		for (j = 0; j < pmadapter->priv_num; ++j) {
			priv = pmadapter->priv[j];
			if (priv) {
#if defined(USB)
				if (!wlan_is_port_ready(pmadapter,
							priv->port_index))
					continue;
#endif
				pmbuf = (pmlan_buffer)util_dequeue_list(
					pmadapter->pmoal_handle,
					&priv->bypass_txq,
					pmadapter->callbacks.moal_spin_lock,
					pmadapter->callbacks.moal_spin_unlock);
				if (pmbuf) {
					util_scalar_decrement(
						pmadapter->pmoal_handle,
						&pmadapter->bypass_pkt_count,
						pmadapter->callbacks
							.moal_spin_lock,
						pmadapter->callbacks
							.moal_spin_unlock);
					PRINTM(MINFO,
					       "Dequeuing bypassed packet %p\n",
					       pmbuf);
					if (wlan_bypass_tx_list_empty(
						    pmadapter))
						tx_param.next_pkt_len = 0;
					else
						tx_param.next_pkt_len =
							pmbuf->data_len;
					status = wlan_process_tx(
						pmadapter->priv[pmbuf->bss_index],
						pmbuf, &tx_param);

					if (status == MLAN_STATUS_RESOURCE) {
						/* Queue the packet again so
						 * that it will be TX'ed later
						 */
						util_scalar_increment(
							pmadapter->pmoal_handle,
							&pmadapter->bypass_pkt_count,
							pmadapter->callbacks
								.moal_spin_lock,
							pmadapter->callbacks
								.moal_spin_unlock);

						util_enqueue_list_head(
							pmadapter->pmoal_handle,
							&priv->bypass_txq,
							(pmlan_linked_list)pmbuf,
							pmadapter->callbacks
								.moal_spin_lock,
							pmadapter->callbacks
								.moal_spin_unlock);
					}
					break;
				} else {
					PRINTM(MINFO, "Nothing to send\n");
				}
			}
		}
	} while (!pmadapter->data_sent && !pmadapter->tx_lock_flag &&
		 !wlan_bypass_tx_list_empty(pmadapter));
	LEAVE();
}

/**
 *  @brief This function will convert 802.11 header to 802.3 header
	   and save the backhaul station aid to pmbuf
 *
 *  @param priv    A pointer to mlan_private
 *  @param pmbuf   A pointer to mlan_buffer
 *  @param prx_pd  A pointer to RxPD
 *
 *  @return	MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status wlan_check_easymesh_pkt(mlan_private *priv, pmlan_buffer pmbuf,
				    RxPD *prx_pd)
{
	Eth803Hdr_t *eth_header = MNULL;
	sta_node *sta_ptr = MNULL;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	t_u8 *pos = MNULL;
	t_u32 tmp = 0;

	ENTER();

	pos = (t_u8 *)prx_pd + prx_pd->rx_pkt_offset;
	eth_header = (Eth803Hdr_t *)pos;

	PRINTM(MDAT_D,
	       "Rx Easymesh ETH header destination address: " FULL_MACSTR
	       "\nETH header source address: " FULL_MACSTR "\n",
	       FULL_MAC2STR(eth_header->dest_addr),
	       FULL_MAC2STR(eth_header->src_addr));

	if (priv->bss_type == MLAN_BSS_TYPE_UAP) {
		pmbuf->flags |= MLAN_BUF_FLAG_EASYMESH;
		memcpy_ext(priv->adapter, pmbuf->mac, prx_pd->ta_mac,
			   MLAN_MAC_ADDR_LENGTH, MLAN_MAC_ADDR_LENGTH);

		sta_ptr = wlan_get_station_entry(priv, prx_pd->ta_mac);
		if (!sta_ptr) {
			PRINTM(MERROR,
			       "Easymesh Error! Can't find station in the station list\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Save station aid to pmbuf and send it to moal */
		tmp = (t_u32)sta_ptr->aid;
		pmbuf->priority |= (tmp << 24);
		PRINTM(MDAT_D, "Easymesh: Rx for VLAN " FULL_MACSTR "\n",
		       FULL_MAC2STR(prx_pd->ta_mac));
	}
done:
	LEAVE();
	return ret;
}
