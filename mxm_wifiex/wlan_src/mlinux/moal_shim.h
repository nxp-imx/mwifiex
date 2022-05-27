/** @file moal_shim.h
 *
 * @brief This file contains declaration referring to
 * functions defined in moal module
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
/*************************************************************
 * Change Log:
 *	10/21/2008: initial version
 ************************************************************/

#ifndef _MOAL_H
#define _MOAL_H

mlan_status moal_get_fw_data(t_void *pmoal, t_u32 offset, t_u32 len,
			     t_u8 *pbuf);
mlan_status moal_get_vdll_data(t_void *pmoal, t_u32 len, t_u8 *pbuf);
mlan_status moal_get_hw_spec_complete(t_void *pmoal, mlan_status status,
				      mlan_hw_info *phw, pmlan_bss_tbl ptbl);
mlan_status moal_init_fw_complete(t_void *pmoal, mlan_status status);
mlan_status moal_shutdown_fw_complete(t_void *pmoal, mlan_status status);
mlan_status moal_ioctl_complete(t_void *pmoal, pmlan_ioctl_req pioctl_req,
				mlan_status status);
mlan_status moal_alloc_mlan_buffer(t_void *pmoal, t_u32 size,
				   pmlan_buffer *pmbuf);
mlan_status moal_free_mlan_buffer(t_void *pmoal, pmlan_buffer pmbuf);
mlan_status moal_send_packet_complete(t_void *pmoal, pmlan_buffer pmbuf,
				      mlan_status status);
#ifdef USB
mlan_status moal_recv_complete(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
			       mlan_status status);
mlan_status moal_write_data_async(t_void *pmoal, pmlan_buffer pmbuf,
				  t_u32 port);
#endif

#if defined(SDIO) || defined(PCIE)
/** moal_write_reg */
mlan_status moal_write_reg(t_void *pmoal, t_u32 reg, t_u32 data);
/** moal_read_reg */
mlan_status moal_read_reg(t_void *pmoal, t_u32 reg, t_u32 *data);
#endif /* SDIO || PCIE */
mlan_status moal_write_data_sync(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
				 t_u32 timeout);
mlan_status moal_read_data_sync(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
				t_u32 timeout);
mlan_status moal_recv_amsdu_packet(t_void *pmoal, pmlan_buffer pmbuf);
mlan_status moal_recv_packet(t_void *pmoal, pmlan_buffer pmbuf);
mlan_status moal_recv_event(t_void *pmoal, pmlan_event pmevent);
mlan_status moal_malloc(t_void *pmoal, t_u32 size, t_u32 flag, t_u8 **ppbuf);
mlan_status moal_mfree(t_void *pmoal, t_u8 *pbuf);
mlan_status moal_vmalloc(t_void *pmoal, t_u32 size, t_u8 **ppbuf);
mlan_status moal_vfree(t_void *pmoal, t_u8 *pbuf);
#ifdef PCIE
mlan_status moal_malloc_consistent(t_void *pmoal, t_u32 size, t_u8 **ppbuf,
				   t_pu64 pbuf_pa);
mlan_status moal_mfree_consistent(t_void *pmoal, t_u32 size, t_u8 *pbuf,
				  t_u64 buf_pa);
mlan_status moal_map_memory(t_void *pmoal, t_u8 *pbuf, t_u64 *pbuf_pa,
			    t_u32 size, t_u32 flag);
mlan_status moal_unmap_memory(t_void *pmoal, t_u8 *pbuf, t_u64 buf_pa,
			      t_u32 size, t_u32 flag);
#endif /* PCIE */
t_void *moal_memset(t_void *pmoal, t_void *pmem, t_u8 byte, t_u32 num);
t_void *moal_memcpy(t_void *pmoal, t_void *pdest, const t_void *psrc,
		    t_u32 num);
t_void *moal_memcpy_ext(t_void *pmoal, t_void *pdest, const t_void *psrc,
			t_u32 num, t_u32 dest_size);

t_void *moal_memmove(t_void *pmoal, t_void *pdest, const t_void *psrc,
		     t_u32 num);
t_s32 moal_memcmp(t_void *pmoal, const t_void *pmem1, const t_void *pmem2,
		  t_u32 num);
/** moal_udelay */
t_void moal_udelay(t_void *pmoal, t_u32 udelay);
t_void moal_usleep_range(t_void *pmoal, t_u32 min_delay, t_u32 max_delay);
mlan_status moal_get_boot_ktime(t_void *pmoal, t_u64 *pnsec);
mlan_status moal_get_system_time(t_void *pmoal, t_u32 *psec, t_u32 *pusec);
mlan_status moal_init_lock(t_void *pmoal, t_void **pplock);
mlan_status moal_free_lock(t_void *pmoal, t_void *plock);
mlan_status moal_spin_lock(t_void *pmoal, t_void *plock);
mlan_status moal_spin_unlock(t_void *pmoal, t_void *plock);
#if defined(DRV_EMBEDDED_AUTHENTICATOR) || defined(DRV_EMBEDDED_SUPPLICANT)
mlan_status moal_wait_hostcmd_complete(t_void *pmoal, t_u32 bss_index);
mlan_status moal_notify_hostcmd_complete(t_void *pmoal, t_u32 bss_index);
#endif
t_void moal_print(t_void *pmoal, t_u32 level, char *pformat, IN...);
t_void moal_print_netintf(t_void *pmoal, t_u32 bss_index, t_u32 level);
t_void moal_assert(t_void *pmoal, t_u32 cond);
t_void moal_hist_data_add(t_void *pmoal, t_u32 bss_index, t_u16 rx_rate,
			  t_s8 snr, t_s8 nflr, t_u8 antenna);

t_void moal_updata_peer_signal(t_void *pmoal, t_u32 bss_index, t_u8 *peer_addr,
			       t_s8 snr, t_s8 nflr);
t_u64 moal_do_div(t_u64 num, t_u32 base);

mlan_status moal_init_timer(t_void *pmoal, t_void **pptimer,
			    IN t_void (*callback)(t_void *pcontext),
			    t_void *pcontext);
mlan_status moal_free_timer(t_void *pmoal, t_void *ptimer);
mlan_status moal_start_timer(t_void *pmoal, t_void *ptimer, t_u8 periodic,
			     t_u32 msec);
mlan_status moal_stop_timer(t_void *pmoal, t_void *ptimer);
void moal_tp_accounting(t_void *pmoal, void *buf, t_u32 drop_point);
void moal_tp_accounting_rx_param(t_void *pmoal, unsigned int type,
				 unsigned int rsvd1);
void moal_amsdu_tp_accounting(t_void *pmoal, t_s32 amsdu_process_delay,
			      t_s32 amsdu_copy_delay);

void moal_connection_status_check_pmqos(t_void *pmoal);
#if defined(PCIE) || defined(SDIO)
/* pmqos busfreq add request handler*/
void woal_request_busfreq_pmqos_add(t_void *pmhandle);
/* pmqos busfreq remove handler*/
void woal_release_busfreq_pmqos_remove(t_void *pmhandle);
#endif

#endif /*_MOAL_H */
