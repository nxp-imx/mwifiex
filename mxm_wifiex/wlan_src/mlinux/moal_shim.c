/** @file moal_shim.c
 *
 * @brief This file contains the callback functions registered to MLAN
 *
 *
 * Copyright 2008-2022 NXP
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

/********************************************************
Change log:
    10/21/2008: initial version
********************************************************/

#include "moal_main.h"
#ifdef USB
#include "moal_usb.h"
#endif
#ifdef SDIO
#include "moal_sdio.h"
#endif
#ifdef PCIE
#include "moal_pcie.h"
#endif
#ifdef UAP_SUPPORT
#include "moal_uap.h"
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#include "moal_cfg80211.h"
#include "moal_cfg80211_util.h"
#endif
#include <asm/div64.h>

#if defined(PCIE) || defined(SDIO)
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 70)
#ifdef IMX_SUPPORT
#include <linux/busfreq-imx.h>
#endif
#endif

#include <linux/etherdevice.h>

#endif /*defined(PCIE) || defined(SDIO)*/

/********************************************************
		Local Variables
********************************************************/
/** moal_lock */
typedef struct _moal_lock {
	/** Lock */
	spinlock_t lock;
	/** Flags */
	unsigned long flags;
} moal_lock;

/********************************************************
		Global Variables
********************************************************/

extern int wifi_status;

/********************************************************
		Local Functions
********************************************************/

/********************************************************
		Global Functions
********************************************************/
/**
 *  @brief Alloc a buffer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size     The size of the buffer to be allocated
 *  @param flag     The type of the buffer to be allocated
 *  @param ppbuf    Pointer to a buffer location to store buffer pointer
 * allocated
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_malloc(t_void *pmoal, t_u32 size, t_u32 flag, t_u8 **ppbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;
	gfp_t mem_flag = (in_interrupt() || in_atomic() || irqs_disabled()) ?
				 GFP_ATOMIC :
				 GFP_KERNEL;

	if (flag & MLAN_MEM_FLAG_ATOMIC)
		mem_flag = GFP_ATOMIC;

#ifdef USB
	if (!IS_USB(handle->card_type))
#endif
	{
		if (flag & MLAN_MEM_DMA)
			mem_flag |= GFP_DMA;
	}
	*ppbuf = kzalloc(size, mem_flag);
	if (*ppbuf == NULL) {
		PRINTM(MERROR, "%s: allocate memory (%d bytes) failed!\n",
		       __func__, (int)size);
		return MLAN_STATUS_FAILURE;
	}
	atomic_inc(&handle->malloc_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free a buffer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pbuf     Pointer to the buffer to be freed
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_mfree(t_void *pmoal, t_u8 *pbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;

	if (!pbuf)
		return MLAN_STATUS_FAILURE;
	kfree(pbuf);
	atomic_dec(&handle->malloc_count);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Alloc a vitual-address-continuous buffer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size     The size of the buffer to be allocated
 *  @param ppbuf    Pointer to a buffer location to store buffer pointer
 * allocated
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_vmalloc(t_void *pmoal, t_u32 size, t_u8 **ppbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;

	*ppbuf = vmalloc(size);
	if (*ppbuf == NULL) {
		PRINTM(MERROR, "%s: vmalloc (%d bytes) failed!", __func__,
		       (int)size);
		return MLAN_STATUS_FAILURE;
	}
	atomic_inc(&handle->vmalloc_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free a buffer allocated by vmalloc
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pbuf     Pointer to the buffer to be freed
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_vfree(t_void *pmoal, t_u8 *pbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;

	if (!pbuf)
		return MLAN_STATUS_FAILURE;
	vfree(pbuf);
	atomic_dec(&handle->vmalloc_count);
	return MLAN_STATUS_SUCCESS;
}

#ifdef PCIE
/**
 *  @brief Alloc a consistent block of memory
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size         The size of the buffer to be allocated
 *  @param ppbuf        Pointer to a buffer location to store memory allocated
 *  @param pbuf_pa      Pointer to a buffer location to store physical address
 * of above memory
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_malloc_consistent(t_void *pmoal, t_u32 size, t_u8 **ppbuf,
				   t_pu64 pbuf_pa)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = (pcie_service_card *)handle->card;
	dma_addr_t dma;
	*pbuf_pa = 0;

	if (!card)
		return MLAN_STATUS_FAILURE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	*ppbuf = (t_u8 *)dma_alloc_coherent(&card->dev->dev, size,
					    (dma_addr_t *)&dma, GFP_KERNEL);
#else
	*ppbuf = (t_u8 *)pci_alloc_consistent(card->dev, size,
					      (dma_addr_t *)&dma);
#endif
	if (*ppbuf == NULL) {
		PRINTM(MERROR,
		       "%s: allocate consistent memory (%d bytes) failed!\n",
		       __func__, (int)size);
		return MLAN_STATUS_FAILURE;
	}
	*pbuf_pa = (t_u64)dma;
	atomic_inc(&handle->malloc_cons_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free a consistent block of memory
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size         Size of them memory to be freed
 *  @param pbuf         Pointer to the memory to be freed
 *  @param buf_pa       Physical address of the memory to be freed
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_mfree_consistent(t_void *pmoal, t_u32 size, t_u8 *pbuf,
				  t_u64 buf_pa)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = handle->card;

	if (!pbuf || !card)
		return MLAN_STATUS_FAILURE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	dma_free_coherent(&card->dev->dev, size, pbuf, buf_pa);
#else
	pci_free_consistent(card->dev, size, pbuf, buf_pa);
#endif
	atomic_dec(&handle->malloc_cons_count);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Map a block of memory to device
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pbuf         Pointer to the buffer to be mapped
 *  @param pbuf_pa      Pointer to store the physical address of buffer
 *  @param size         Size of the buffer to be mapped
 *  @param flag         Flags for mapping IO
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_map_memory(t_void *pmoal, t_u8 *pbuf, t_u64 *pbuf_pa,
			    t_u32 size, t_u32 flag)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = (pcie_service_card *)handle->card;

	dma_addr_t dma;

	if (!card)
		return MLAN_STATUS_FAILURE;

		/* Init memory to device */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	dma = dma_map_single(&card->dev->dev, pbuf, size, flag);
	if (dma_mapping_error(&card->dev->dev, dma)) {
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
	dma = pci_map_single(card->dev, pbuf, size, flag);
	if (pci_dma_mapping_error(card->dev, dma)) {
#else
	dma = pci_map_single(card->dev, pbuf, size, flag);
	if (pci_dma_mapping_error(dma)) {
#endif
		PRINTM(MERROR, "Tx ring: failed to dma_map_single\n");
		return MLAN_STATUS_FAILURE;
	}

	*pbuf_pa = dma;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Unmap a block of memory from device
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pbuf         Pointer to the buffer to unmap
 *  @param buf_pa       Physical address of buffer to unmap
 *  @param size         Size of the buffer to unmap
 *  @param flag         Flags for mapping IO
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_unmap_memory(t_void *pmoal, t_u8 *pbuf, t_u64 buf_pa,
			      t_u32 size, t_u32 flag)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = (pcie_service_card *)handle->card;

	if (!card)
		return MLAN_STATUS_FAILURE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	dma_unmap_single(&card->dev->dev, buf_pa, size, flag);
#else
	pci_unmap_single(card->dev, buf_pa, size, flag);
#endif

	return MLAN_STATUS_SUCCESS;
}
#endif /* PCIE */

/**
 *  @brief Fill memory with constant byte
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmem     Pointer to the memory area
 *  @param byte     A constant byte
 *  @param num      Number of bytes to fill
 *
 *  @return         Pointer to the memory area
 */
t_void *moal_memset(t_void *pmoal, t_void *pmem, t_u8 byte, t_u32 num)
{
	t_void *p = pmem;

	if (pmem && num)
		p = memset(pmem, byte, num);

	return p;
}

/**
 *  @brief Copy memory from one area to another
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pdest    Pointer to the dest memory
 *  @param psrc     Pointer to the src memory
 *  @param num      Number of bytes to move
 *
 *  @return         Pointer to the dest memory
 */
t_void *moal_memcpy(t_void *pmoal, t_void *pdest, const t_void *psrc, t_u32 num)
{
	t_void *p = pdest;

	if (pdest && psrc && num)
		p = memcpy(pdest, psrc, num);

	return p;
}

/**
 *  @brief Copy memory from one area to another
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pdest    Pointer to the dest memory
 *  @param psrc     Pointer to the src memory
 *  @param num      Number of bytes to move
 *  @param dest_size size of dest memory.
 *
 *  @return         Pointer to the dest memory
 */
t_void *moal_memcpy_ext(t_void *pmoal, t_void *pdest, const t_void *psrc,
			t_u32 num, t_u32 dest_size)
{
	t_void *p = pdest;
	if (pdest && psrc && num && dest_size)
		p = memcpy(pdest, psrc, MIN(num, dest_size));

	return p;
}

/**
 *  @brief Move memory from one area to another
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pdest    Pointer to the dest memory
 *  @param psrc     Pointer to the src memory
 *  @param num      Number of bytes to move
 *
 *  @return         Pointer to the dest memory
 */
t_void *moal_memmove(t_void *pmoal, t_void *pdest, const t_void *psrc,
		     t_u32 num)
{
	t_void *p = pdest;

	if (pdest && psrc && num)
		p = memmove(pdest, psrc, num);

	return p;
}

/**
 *  @brief Compare two memory areas
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmem1    Pointer to the first memory
 *  @param pmem2    Pointer to the second memory
 *  @param num      Number of bytes to compare
 *
 *  @return         Compare result returns by memcmp
 */
t_s32 moal_memcmp(t_void *pmoal, const t_void *pmem1, const t_void *pmem2,
		  t_u32 num)
{
	t_s32 result;

	result = memcmp(pmem1, pmem2, num);

	return result;
}

/**
 *  @brief Delay function
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param delay  delay in micro-second
 *
 *  @return       N/A
 */
t_void moal_udelay(t_void *pmoal, t_u32 delay)
{
	if (delay >= 1000)
		msleep(delay / 1000);
	if (delay % 1000)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
		usleep_range(delay % 1000 - 1, delay % 1000);
#else
		udelay(delay % 1000);
#endif
}

/**
 *  @brief  usleep_range function
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param min_delay  minimal delay in micro-second
 *  @param max_delay  delay in micro-second
 *
 *  @return       N/A
 */
t_void moal_usleep_range(t_void *pmoal, t_u32 min_delay, t_u32 max_delay)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	usleep_range(min_delay, max_delay);
#endif
}
/**
 *  @brief Retrieves the current system time
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param psec     Pointer to buf for the seconds of system time
 *  @param pusec    Pointer to buf the micro seconds of system time
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_get_system_time(t_void *pmoal, t_u32 *psec, t_u32 *pusec)
{
	wifi_timeval t;

	woal_get_monotonic_time(&t);
	*psec = t.time_sec;
	*pusec = t.time_usec;

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Retrieves the current boot time
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pnsec     Pointer to buf for the Nanoseconds of boot time
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_get_boot_ktime(t_void *pmoal, t_u64 *pnsec)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
	ktime_t time;

	time = ktime_get_with_offset(TK_OFFS_BOOT);
	*pnsec = *(t_u64 *)&(time);
#endif
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Initializes the timer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pptimer      Pointer to the timer
 *  @param callback     Pointer to callback function
 *  @param pcontext     Pointer to context
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_init_timer(t_void *pmoal, t_void **pptimer,
			    IN t_void (*callback)(t_void *pcontext),
			    t_void *pcontext)
{
	moal_drv_timer *timer = NULL;
	gfp_t mem_flag = (in_interrupt() || in_atomic() || irqs_disabled()) ?
				 GFP_ATOMIC :
				 GFP_KERNEL;

	timer = kmalloc(sizeof(moal_drv_timer), mem_flag);
	if (timer == NULL)
		return MLAN_STATUS_FAILURE;
	woal_initialize_timer(timer, callback, pcontext);
	*pptimer = (t_void *)timer;

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free the timer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param ptimer   Pointer to the timer
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_free_timer(t_void *pmoal, t_void *ptimer)
{
	moal_drv_timer *timer = (moal_drv_timer *)ptimer;

	if (timer) {
		if ((timer->timer_is_canceled == MFALSE) &&
		    timer->time_period) {
			PRINTM(MWARN,
			       "mlan try to free timer without stop timer!\n");
			woal_cancel_timer(timer);
		}
		kfree(timer);
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Start the timer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param ptimer       Pointer to the timer
 *  @param periodic     Periodic timer
 *  @param msec         Timer value in milliseconds
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_start_timer(t_void *pmoal, t_void *ptimer, t_u8 periodic,
			     t_u32 msec)
{
	if (!ptimer)
		return MLAN_STATUS_FAILURE;

	((moal_drv_timer *)ptimer)->timer_is_periodic = periodic;
	woal_mod_timer((moal_drv_timer *)ptimer, msec);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Stop the timer
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param ptimer   Pointer to the timer
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_stop_timer(t_void *pmoal, t_void *ptimer)
{
	if (!ptimer)
		return MLAN_STATUS_FAILURE;
	woal_cancel_timer((moal_drv_timer *)ptimer);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Initializes the lock
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pplock   Pointer to the lock
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_init_lock(t_void *pmoal, t_void **pplock)
{
	moal_handle *handle = (moal_handle *)pmoal;
	moal_lock *mlock = NULL;

	mlock = kmalloc(sizeof(moal_lock), GFP_ATOMIC);
	if (!mlock)
		return MLAN_STATUS_FAILURE;
	spin_lock_init(&mlock->lock);
	*pplock = (t_void *)mlock;

	atomic_inc(&handle->lock_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free the lock
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param plock    Lock
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_free_lock(t_void *pmoal, t_void *plock)
{
	moal_handle *handle = (moal_handle *)pmoal;
	moal_lock *mlock = plock;

	kfree(mlock);
	if (mlock)
		atomic_dec(&handle->lock_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Request a spin lock
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param plock    Pointer to the lock
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_spin_lock(t_void *pmoal, t_void *plock)
	__acquires(&plock->lock)
{
	moal_lock *mlock = plock;
	unsigned long flags = 0;
	spin_lock_irqsave(&mlock->lock, flags);
	mlock->flags = flags;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Request a spin_unlock
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param plock    Pointer to the lock
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_spin_unlock(t_void *pmoal, t_void *plock)
	__releases(&plock->lock)
{
	moal_lock *mlock = (moal_lock *)plock;
	spin_unlock_irqrestore(&mlock->lock, mlock->flags);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief  This function collects AMSDU TP statistics.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param amsdu_process_delay          amsdu process time
 *  @param amsdu_copy_delay             amsdu copy time
 *
 *  @return         N/A
 */
void moal_amsdu_tp_accounting(t_void *pmoal, t_s32 amsdu_process_delay,
			      t_s32 amsdu_copy_delay)
{
	moal_handle *handle = (moal_handle *)pmoal;
	handle->tp_acnt.rx_amsdu_delay[handle->tp_acnt.rx_amsdu_index] =
		amsdu_process_delay;
	handle->tp_acnt.rx_amsdu_copy_delay[handle->tp_acnt.rx_amsdu_index] =
		amsdu_copy_delay;
	handle->tp_acnt.rx_amsdu_index++;
	if (handle->tp_acnt.rx_amsdu_index >= TXRX_MAX_SAMPLE)
		handle->tp_acnt.rx_amsdu_index = 0;
}

/**
 *  @brief  This function collects TP statistics.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param buf          pointer to the buffer of a packet
 *  @param drop_point   Drop pointer user set
 *
 *  @return         N/A
 */
void moal_tp_accounting(t_void *pmoal, void *buf, t_u32 drop_point)
{
	struct sk_buff *skb = NULL;
	moal_handle *handle = (moal_handle *)pmoal;
	pmlan_buffer pmbuf = (pmlan_buffer)buf;
	t_s32 delay;
	wifi_timeval t;

	if (drop_point < MAX_TP_ACCOUNT_DROP_POINT_NUM) {
		if (drop_point == 4) {
			handle->tp_acnt.tx_bytes[drop_point] += pmbuf->data_len;
		} else {
			skb = (struct sk_buff *)buf;
			handle->tp_acnt.tx_bytes[drop_point] += skb->len;
		}
		handle->tp_acnt.tx_packets[drop_point]++;
	} else if (drop_point <= RX_DROP_P5) {
		t_u16 rx_len = 0;
		if (drop_point == RX_DROP_P1 || drop_point == RX_DROP_P2)
			rx_len = pmbuf->data_len -
				 *((t_u16 *)(pmbuf->pbuf + pmbuf->data_offset) +
				   2); // remove rx_pkt_offset
		else if (drop_point == RX_DROP_P3) // aggr pkt
			rx_len = pmbuf->data_len;
		else if (drop_point == RX_DROP_P4) { // before to kernel
			skb = (struct sk_buff *)buf;
			rx_len = skb->len;
		}
		handle->tp_acnt
			.rx_bytes[drop_point - MAX_TP_ACCOUNT_DROP_POINT_NUM] +=
			rx_len;
		handle->tp_acnt.rx_packets[drop_point -
					   MAX_TP_ACCOUNT_DROP_POINT_NUM]++;
	} else if (drop_point == RX_TIME_PKT) {
		woal_get_monotonic_time(&t);
		/* deque - pcie receive */
		delay = (t_s32)(pmbuf->extra_ts_sec - pmbuf->in_ts_sec) *
			1000000;
		delay += (t_s32)(pmbuf->extra_ts_usec - pmbuf->in_ts_usec);
		handle->tp_acnt.rx_delay1_driver[handle->tp_acnt.rx_index] =
			delay;
		/* before netif_rx - deque */
		delay = (t_s32)(pmbuf->out_ts_sec - pmbuf->extra_ts_sec) *
			1000000;
		delay += (t_s32)(pmbuf->out_ts_usec - pmbuf->extra_ts_usec);
		handle->tp_acnt.rx_delay2_driver[handle->tp_acnt.rx_index] =
			delay;
		/* netif_rx to netif_rx return */
		delay = (t_s32)(t.time_sec - pmbuf->out_ts_sec) * 1000000;
		delay += (t_s32)(t.time_usec - pmbuf->out_ts_usec);
		handle->tp_acnt.rx_delay_kernel[handle->tp_acnt.rx_index] =
			delay;
		handle->tp_acnt.rx_index++;
		if (handle->tp_acnt.rx_index >= TXRX_MAX_SAMPLE)
			handle->tp_acnt.rx_index = 0;
	} else if (drop_point == TX_TIME_PKT) {
		delay = (t_s32)(pmbuf->out_ts_sec - pmbuf->in_ts_sec) * 1000000;
		delay += (t_s32)(pmbuf->out_ts_usec - pmbuf->in_ts_usec);
		handle->tp_acnt.tx_delay_driver[handle->tp_acnt.tx_index] =
			delay;
		handle->tp_acnt.tx_index++;
		if (handle->tp_acnt.tx_index >= TXRX_MAX_SAMPLE)
			handle->tp_acnt.tx_index = 0;
	}
}

void moal_tp_accounting_rx_param(t_void *pmoal, unsigned int type,
				 unsigned int rsvd1)
{
	moal_handle *phandle = (moal_handle *)pmoal;
	switch (type) {
	case 0: // Rx interrupt
		phandle->tp_acnt.rx_intr_cnt++;
		break;
	case 1: // rx_pkts_queued
		phandle->tp_acnt.rx_pending = rsvd1;
		break;
	case 2: // paused
		phandle->tp_acnt.rx_paused_cnt++;
		break;
	case 3: // tx interrupt count
		phandle->tp_acnt.tx_intr_cnt++;
		break;
	case 4: // rx amsdu count
		phandle->tp_acnt.rx_amsdu_cnt++;
		phandle->tp_acnt.rx_amsdu_pkt_cnt += rsvd1;
		break;
	case 5: // tx amsdu count
		phandle->tp_acnt.tx_amsdu_cnt++;
		phandle->tp_acnt.tx_amsdu_pkt_cnt += rsvd1;
		break;
	case 6: // rxbd rdptr full count
		phandle->tp_acnt.rx_rdptr_full_cnt++;
		break;
	case 7: // tx hard xmit skb realloc count
		phandle->tp_acnt.tx_xmit_skb_realloc_cnt++;
		break;
	case 8: // tx stop queue count
		phandle->tp_acnt.tx_stop_queue_cnt++;
		break;

	default:
		break;
	}
}

/**
 *  @brief This function reads one block of firmware data from MOAL
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param offset       Offset from where the data will be copied
 *  @param len          Length to be copied
 *  @param pbuf         Buffer where the data will be copied
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_get_fw_data(t_void *pmoal, t_u32 offset, t_u32 len, t_u8 *pbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;

	if (!pbuf || !len || !handle->firmware)
		return MLAN_STATUS_FAILURE;

	if (offset + len > handle->firmware->size)
		return MLAN_STATUS_FAILURE;

	moal_memcpy_ext(handle, pbuf, handle->firmware->data + offset, len,
			len);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function get vdll data from moal
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param len          Length to be copied
 *  @param pbuf         Buffer where the data will be copied
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_get_vdll_data(t_void *pmoal, t_u32 len, t_u8 *pbuf)
{
	moal_handle *handle = (moal_handle *)pmoal;
	mlan_status status = MLAN_STATUS_FAILURE;
	t_u32 offset = 0;
	t_u8 req_fw = MFALSE;

	if (!handle->firmware) {
		req_fw = MTRUE;
		woal_vdll_req_fw(handle);
	}

	if (handle->firmware) {
		if (len < handle->firmware->size) {
			offset = handle->firmware->size - len;
			moal_memcpy_ext(handle, pbuf,
					handle->firmware->data + offset, len,
					len);
			status = MLAN_STATUS_SUCCESS;
		} else {
			PRINTM(MERROR, "Invalid VDLL length = %d, fw_len=%d\n",
			       len, (int)handle->firmware->size);
		}
		if (req_fw) {
			release_firmware(handle->firmware);
			handle->firmware = NULL;
		}
	}
	return status;
}

/**
 *  @brief This function is called when MLAN completes the initialization
 * firmware.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param status   The status code for mlan_init_fw request
 *  @param phw      pointer to mlan_hw_info
 *  @param ptbl     pointer to mplan_bss_tbl
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_get_hw_spec_complete(t_void *pmoal, mlan_status status,
				      mlan_hw_info *phw, pmlan_bss_tbl ptbl)
{
	moal_handle *handle = (moal_handle *)pmoal;
	int i;
	t_u32 drv_mode = handle->params.drv_mode;

	ENTER();
	if (status == MLAN_STATUS_SUCCESS) {
		PRINTM(MCMND, "Get Hw Spec done, fw_cap=0x%x\n", phw->fw_cap);
#ifdef PCIE9098
		/** Special/Temporary handling to manage the driver version
		 * string to identify Seahawk/AW690 (skyhawk based) based on
		 * fw_cap_ext value set by Fw */
		if (phw->fw_cap_ext & MBIT(31) &&
		    IS_PCIE9098(handle->card_type)) {
			moal_memcpy_ext(handle, driver_version, CARD_PCIEAW690,
					strlen(CARD_PCIEAW690),
					strlen(driver_version));
			moal_memcpy_ext(handle,
					driver_version + strlen(INTF_CARDTYPE) +
						strlen(KERN_VERSION),
					V17, strlen(V17),
					strlen(driver_version) -
						strlen(INTF_CARDTYPE) -
						strlen(KERN_VERSION));
			moal_memcpy_ext(handle, handle->driver_version,
					driver_version, strlen(driver_version),
					MLAN_MAX_VER_STR_LEN - 1);
		}
#endif
		/** FW should only enable DFS on one mac */
		if (!(phw->fw_cap & FW_CAPINFO_ZERO_DFS))
			handle->params.drv_mode &= ~DRV_MODE_DFS;

		if (!(phw->fw_cap & FW_CAPINFO_80211BGA))
			handle->params.drv_mode &= ~DRV_MODE_WLAN;
		if (!handle->params.drv_mode ||
		    MLAN_STATUS_SUCCESS !=
			    woal_update_drv_tbl(handle,
						handle->params.drv_mode)) {
			PRINTM(MERROR,
			       "Get_hw_spec_complete: Fail to update drv_tbl\n");
			LEAVE();
			return MLAN_STATUS_FAILURE;
		}
		memset(ptbl, 0, sizeof(mlan_bss_tbl));
		for (i = 0; i < handle->drv_mode.intf_num; i++) {
			ptbl->bss_attr[i].bss_type =
				handle->drv_mode.bss_attr[i].bss_type;
			ptbl->bss_attr[i].frame_type =
				handle->drv_mode.bss_attr[i].frame_type;
			ptbl->bss_attr[i].active =
				handle->drv_mode.bss_attr[i].active;
			ptbl->bss_attr[i].bss_priority =
				handle->drv_mode.bss_attr[i].bss_priority;
			ptbl->bss_attr[i].bss_num =
				handle->drv_mode.bss_attr[i].bss_num;
			ptbl->bss_attr[i].bss_virtual =
				handle->drv_mode.bss_attr[i].bss_virtual;
		}
		PRINTM(MCMND, "org_drv_mode=0x%x drv_mode=0x%x\n", drv_mode,
		       handle->params.drv_mode);
	}
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function is called when MLAN completes the initialization
 * firmware.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param status   The status code for mlan_init_fw request
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_init_fw_complete(t_void *pmoal, mlan_status status)
{
	moal_handle *handle = (moal_handle *)pmoal;
	ENTER();
	if (status == MLAN_STATUS_SUCCESS)
		handle->hardware_status = HardwareStatusReady;
	handle->init_wait_q_woken = MTRUE;
	wake_up(&handle->init_wait_q);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function is called when MLAN shutdown firmware is completed.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param status   The status code for mlan_shutdown request
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_shutdown_fw_complete(t_void *pmoal, mlan_status status)
{
	moal_handle *handle = (moal_handle *)pmoal;
	ENTER();
	handle->hardware_status = HardwareStatusNotReady;
	handle->init_wait_q_woken = MTRUE;
	wake_up_interruptible(&handle->init_wait_q);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function is called when an MLAN IOCTL is completed.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pioctl_req	pointer to structure mlan_ioctl_req
 *  @param status   The status code for mlan_ioctl request
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_ioctl_complete(t_void *pmoal, pmlan_ioctl_req pioctl_req,
				mlan_status status)
{
	moal_handle *handle = (moal_handle *)pmoal;
	moal_private *priv = NULL;
	wait_queue *wait;
	unsigned long flags = 0;
	ENTER();

	if (!atomic_read(&handle->ioctl_pending))
		PRINTM(MERROR, "ERR: Unexpected IOCTL completed: %p\n",
		       pioctl_req);
	else
		atomic_dec(&handle->ioctl_pending);
	priv = woal_bss_index_to_priv(handle, pioctl_req->bss_index);
	if (!priv) {
		PRINTM(MERROR,
		       "IOCTL %p complete with NULL priv, bss_index=%d\n",
		       pioctl_req, pioctl_req->bss_index);
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}

	if (status != MLAN_STATUS_SUCCESS && status != MLAN_STATUS_COMPLETE)
		PRINTM(MERROR,
		       "IOCTL failed: %p id=0x%x, sub_id=0x%x action=%d, status_code=0x%x\n",
		       pioctl_req, pioctl_req->req_id,
		       (*(t_u32 *)pioctl_req->pbuf), (int)pioctl_req->action,
		       pioctl_req->status_code);
	else
		PRINTM(MIOCTL,
		       "IOCTL completed: %p id=0x%x sub_id=0x%x, action=%d,  status=%d, status_code=0x%x\n",
		       pioctl_req, pioctl_req->req_id,
		       (*(t_u32 *)pioctl_req->pbuf), (int)pioctl_req->action,
		       status, pioctl_req->status_code);

	spin_lock_irqsave(&handle->driver_lock, flags);
	wait = (wait_queue *)pioctl_req->reserved_1;
	if (wait) {
		wait->condition = MTRUE;
		wait->status = status;
		if (wait->wait_timeout) {
			wake_up(&wait->wait);
		} else {
			if ((status != MLAN_STATUS_SUCCESS) &&
			    (pioctl_req->status_code ==
			     MLAN_ERROR_CMD_TIMEOUT)) {
				PRINTM(MERROR, "IOCTL: command timeout\n");
			} else {
				wake_up_interruptible(&wait->wait);
			}
		}
		spin_unlock_irqrestore(&handle->driver_lock, flags);
	} else {
		spin_unlock_irqrestore(&handle->driver_lock, flags);
		if ((status == MLAN_STATUS_SUCCESS) &&
		    (pioctl_req->action == MLAN_ACT_GET))
			woal_process_ioctl_resp(priv, pioctl_req);
		kfree(pioctl_req);
	}
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function allocates mlan_buffer.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size     allocation size requested
 *  @param pmbuf    pointer to pointer to the allocated buffer
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_alloc_mlan_buffer(t_void *pmoal, t_u32 size,
				   pmlan_buffer *pmbuf)
{
	*pmbuf = woal_alloc_mlan_buffer((moal_handle *)pmoal, size);
	if (NULL == *pmbuf)
		return MLAN_STATUS_FAILURE;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function frees mlan_buffer.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    pointer to buffer to be freed
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_free_mlan_buffer(t_void *pmoal, pmlan_buffer pmbuf)
{
	if (!pmbuf)
		return MLAN_STATUS_FAILURE;
	woal_free_mlan_buffer((moal_handle *)pmoal, pmbuf);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function is called when MLAN complete send data packet.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *  @param status   The status code for mlan_send_packet request
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_send_packet_complete(t_void *pmoal, pmlan_buffer pmbuf,
				      mlan_status status)
{
	moal_private *priv = NULL;
	moal_handle *handle = (moal_handle *)pmoal;
	struct sk_buff *skb = NULL;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	t_u32 index = 0;
#endif

	ENTER();
	if (pmbuf && pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA) {
		woal_free_mlan_buffer(handle, pmbuf);
		atomic_dec(&handle->tx_pending);
		goto done;
	}
	if (pmbuf) {
		priv = woal_bss_index_to_priv(pmoal, pmbuf->bss_index);
		skb = (struct sk_buff *)pmbuf->pdesc;
		if (priv) {
			woal_set_trans_start(priv->netdev);
			if (skb) {
				if (status == MLAN_STATUS_SUCCESS) {
					priv->stats.tx_packets++;
					priv->stats.tx_bytes += skb->len;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
					woal_packet_fate_monitor(
						priv, PACKET_TYPE_TX,
						TX_PKT_FATE_SENT,
						FRAME_TYPE_ETHERNET_II, 0, 0,
						skb->data, skb->data_len);
#endif
#endif
				} else {
					priv->stats.tx_errors++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
					woal_packet_fate_monitor(
						priv, PACKET_TYPE_TX,
						TX_PKT_FATE_DRV_DROP_OTHER,
						FRAME_TYPE_ETHERNET_II, 0, 0,
						skb->data, skb->data_len);
#endif
#endif
				}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
				index = skb_get_queue_mapping(skb);
				if (index < 4) {
					atomic_dec(&handle->tx_pending);
					if (atomic_dec_return(
						    &priv->wmm_tx_pending[index]) ==
					    LOW_TX_PENDING) {
						struct netdev_queue *txq =
							netdev_get_tx_queue(
								priv->netdev,
								index);
						if (netif_tx_queue_stopped(
							    txq)) {
							netif_tx_wake_queue(
								txq);
							PRINTM(MINFO,
							       "Wakeup Kernel Queue:%d\n",
							       index);
						}
					}
				} else {
					PRINTM(MERROR,
					       "Invalid queue index for skb\n");
				}
#else /*#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29)*/
				if (atomic_dec_return(&handle->tx_pending) <
				    LOW_TX_PENDING) {
					int i;
					for (i = 0; i < handle->priv_num; i++) {
#ifdef STA_SUPPORT
						if ((GET_BSS_ROLE(
							     handle->priv[i]) ==
						     MLAN_BSS_ROLE_STA) &&
						    (handle->priv[i]
							     ->media_connected ||
						     priv->is_adhoc_link_sensed)) {
							woal_wake_queue(
								handle->priv[i]
									->netdev);
						}
#endif
#ifdef UAP_SUPPORT
						if ((GET_BSS_ROLE(
							     handle->priv[i]) ==
						     MLAN_BSS_ROLE_UAP) &&
						    (handle->priv[i]
							     ->media_connected)) {
							woal_wake_queue(
								handle->priv[i]
									->netdev);
						}
#endif
					}
				}
#endif /*#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29)*/
			}
		}
		if (skb)
			dev_kfree_skb_any(skb);
	}

done:
	if ((atomic_read(&handle->tx_pending) == 0) &&
	    !is_zero_timeval(handle->tx_time_start)) {
		woal_get_monotonic_time(&handle->tx_time_end);
		handle->tx_time +=
			(t_u64)(timeval_to_usec(handle->tx_time_end) -
				timeval_to_usec(handle->tx_time_start));
		PRINTM(MINFO,
		       "%s : start_timeval=%d:%d end_timeval=%d:%d inter=%llu tx_time=%llu\n",
		       __func__, handle->tx_time_start.time_sec,
		       handle->tx_time_start.time_usec,
		       handle->tx_time_end.time_sec,
		       handle->tx_time_end.time_usec,
		       (t_u64)(timeval_to_usec(handle->tx_time_end) -
			       timeval_to_usec(handle->tx_time_start)),
		       handle->tx_time);
		handle->tx_time_start.time_sec = 0;
		handle->tx_time_start.time_usec = 0;
	}
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

#ifdef USB
/**
 *  @brief This function is called when MLAN complete receiving
 *         data/event/command
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *  @param port     Port number for receive
 *  @param status   The status code for mlan_receive request
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_recv_complete(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
			       mlan_status status)
{
	moal_private *priv = NULL;
	moal_handle *handle = (moal_handle *)pmoal;
	struct usb_card_rec *cardp = (struct usb_card_rec *)handle->card;
	ENTER();

	if ((pmbuf && (pmbuf->flags & MLAN_BUF_FLAG_RX_DEAGGR)) || !pmbuf)
		atomic_dec(&handle->rx_pending);

	if (pmbuf) {
		priv = woal_bss_index_to_priv(handle, pmbuf->bss_index);
		if (priv && (pmbuf->buf_type == MLAN_BUF_TYPE_DATA) &&
		    (status == MLAN_STATUS_FAILURE)) {
			priv->stats.rx_dropped++;
		}
		/* Reuse the buffer in case of command/event */
		if (port == cardp->rx_cmd_ep)
			woal_submit_rx_urb(handle, port);
		else {
			woal_free_mlan_buffer(handle, pmbuf);
			if ((atomic_read(&handle->rx_pending) <
			     USB_LOW_RX_PENDING) &&
			    atomic_read(&cardp->rx_data_urb_pending) <
				    MVUSB_RX_DATA_URB)
				woal_usb_submit_rx_data_urbs(handle);
		}
	} else if (port == cardp->rx_data_ep) {
		if ((atomic_read(&handle->rx_pending) < USB_LOW_RX_PENDING) &&
		    atomic_read(&cardp->rx_data_urb_pending) <
			    MVUSB_RX_DATA_URB)
			woal_usb_submit_rx_data_urbs(handle);
	}
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function write a command/data packet to card.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *  @param port     Port number for sent
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE or
 * MLAN_STATUS_PENDING or MLAN_STATUS_RESOURCE
 */
mlan_status moal_write_data_async(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port)
{
	moal_handle *handle = (moal_handle *)pmoal;
	mlan_status ret = MLAN_STATUS_SUCCESS;
	ENTER();
	if (handle->is_suspended == MTRUE) {
		PRINTM(MERROR,
		       "write_data_async is not allowed while suspended\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	ret = woal_write_data_async((moal_handle *)pmoal, pmbuf, (t_u8)port);
	LEAVE();
	return ret;
}
#endif /* USB */

/**
 *  @brief This function write a command/data packet to card.
 *         This function blocks the call until it finishes
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *  @param port     Port number for sent
 *  @param timeout  Timeout value in milliseconds (if 0 the wait is forever)
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_write_data_sync(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
				 t_u32 timeout)
{
	moal_handle *handle = (moal_handle *)pmoal;
	return handle->ops.write_data_sync(handle, pmbuf, port, timeout);
}

/**
 *  @brief This function read data packet/event/command from card.
 *         This function blocks the call until it finish
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *  @param port     Port number for read
 *  @param timeout  Timeout value in milliseconds (if 0 the wait is forever)
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_read_data_sync(t_void *pmoal, pmlan_buffer pmbuf, t_u32 port,
				t_u32 timeout)
{
	moal_handle *handle = (moal_handle *)pmoal;
	return handle->ops.read_data_sync(handle, pmbuf, port, timeout);
}

#if defined(SDIO) || defined(PCIE)
/**
 *  @brief This function writes data into card register.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param reg          register offset
 *  @param data         value
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_write_reg(t_void *pmoal, t_u32 reg, t_u32 data)
{
	int ret = MLAN_STATUS_FAILURE;
	moal_handle *handle = (moal_handle *)pmoal;
	if (handle->ops.write_reg)
		ret = handle->ops.write_reg(handle, reg, data);
	return ret;
}

/**
 *  @brief This function reads data from card register.
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param reg          register offset
 *  @param data         value
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_read_reg(t_void *pmoal, t_u32 reg, t_u32 *data)
{
	int ret = MLAN_STATUS_FAILURE;
	moal_handle *handle = (moal_handle *)pmoal;
	if (handle->ops.read_reg)
		ret = handle->ops.read_reg(handle, reg, data);
	return ret;
}

#endif /* SDIO || PCIE */

#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
/**
 *  @brief This function uploads the packet to the network stack monitor
 * interface
 *
 *  @param handle Pointer to the MOAL context
 *  @param pmbuf    Pointer to mlan_buffer
 *
 *  @return  MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING/MLAN_STATUS_FAILURE
 */
static mlan_status moal_recv_packet_to_mon_if(moal_handle *handle,
					      pmlan_buffer pmbuf)
{
	mlan_status status = MLAN_STATUS_SUCCESS;
	struct sk_buff *skb = NULL;
	struct radiotap_header *rth = NULL;
	radiotap_info rt_info = {};
	t_u8 format = 0;
	t_u8 bw = 0;
	t_u8 gi = 0;
	t_u8 ldpc = 0;
	t_u8 chan_num;
	t_u8 band = 0;
	struct ieee80211_hdr *dot11_hdr = NULL;
	t_u8 *payload = NULL;
	t_u32 vht_sig1 = 0;
	t_u32 vht_sig2 = 0;
	ENTER();
	if (!pmbuf->pdesc) {
		LEAVE();
		return status;
	}

	skb = (struct sk_buff *)pmbuf->pdesc;

	if ((handle->mon_if) && netif_running(handle->mon_if->mon_ndev)) {
		if (handle->mon_if->radiotap_enabled) {
			if (skb_headroom(skb) < sizeof(*rth)) {
				PRINTM(MERROR,
				       "%s No space to add Radio TAP header\n",
				       __func__);
				status = MLAN_STATUS_FAILURE;
				handle->mon_if->stats.rx_dropped++;
				goto done;
			}
			dot11_hdr =
				(struct ieee80211_hdr *)(pmbuf->pbuf +
							 pmbuf->data_offset);
			moal_memcpy_ext(handle, &rt_info,
					pmbuf->pbuf + pmbuf->data_offset -
						sizeof(rt_info),
					sizeof(rt_info), sizeof(rt_info));
			ldpc = (rt_info.rate_info.rate_info & 0x20) >> 5;
			format = (rt_info.rate_info.rate_info & 0x18) >> 3;
			bw = (rt_info.rate_info.rate_info & 0x06) >> 1;
			gi = rt_info.rate_info.rate_info & 0x01;
			skb_push(skb, sizeof(*rth));
			rth = (struct radiotap_header *)skb->data;
			memset(skb->data, 0, sizeof(*rth));
			rth->hdr.it_version = PKTHDR_RADIOTAP_VERSION;
			rth->hdr.it_pad = 0;
			rth->hdr.it_len = cpu_to_le16(sizeof(*rth));
			rth->hdr.it_present = cpu_to_le32(
				(1 << IEEE80211_RADIOTAP_TSFT) |
				(1 << IEEE80211_RADIOTAP_FLAGS) |
				(1 << IEEE80211_RADIOTAP_CHANNEL) |
				(1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
				(1 << IEEE80211_RADIOTAP_DBM_ANTNOISE) |
				(1 << IEEE80211_RADIOTAP_ANTENNA));
			/** Timstamp */
			rth->body.timestamp = woal_cpu_to_le64(jiffies);
			/** Flags */
			rth->body.flags = (rt_info.extra_info.flags &
					   ~(RADIOTAP_FLAGS_USE_SGI_HT |
					     RADIOTAP_FLAGS_WITH_FRAGMENT |
					     RADIOTAP_FLAGS_WEP_ENCRYPTION |
					     RADIOTAP_FLAGS_FAILED_FCS_CHECK));
			/** reverse fail fcs, 1 means pass FCS in FW, but means
			 * fail FCS in radiotap */
			rth->body.flags |= (~rt_info.extra_info.flags) &
					   RADIOTAP_FLAGS_FAILED_FCS_CHECK;
			if ((format == MLAN_RATE_FORMAT_HT) && (gi == 1))
				rth->body.flags |= RADIOTAP_FLAGS_USE_SGI_HT;
			if (ieee80211_is_mgmt(dot11_hdr->frame_control) ||
			    ieee80211_is_data(dot11_hdr->frame_control)) {
				if ((ieee80211_has_morefrags(
					    dot11_hdr->frame_control)) ||
				    (!ieee80211_is_first_frag(
					    dot11_hdr->seq_ctrl))) {
					rth->body.flags |=
						RADIOTAP_FLAGS_WITH_FRAGMENT;
				}
			}
			if (ieee80211_is_data(dot11_hdr->frame_control) &&
			    ieee80211_has_protected(dot11_hdr->frame_control)) {
				payload = (t_u8 *)dot11_hdr +
					  ieee80211_hdrlen(
						  dot11_hdr->frame_control);
				if (!(*(payload + 3) & 0x20)) /** ExtIV bit
								 shall be 0 for
								 WEP frame */
					rth->body.flags |=
						RADIOTAP_FLAGS_WEP_ENCRYPTION;
			}
			/** Rate, t_u8 only apply for LG mode */
			if (format == MLAN_RATE_FORMAT_LG) {
				rth->hdr.it_present |= cpu_to_le32(
					1 << IEEE80211_RADIOTAP_RATE);
				rth->body.rate = rt_info.rate_info.bitrate;
			}
			/** Channel */
			rth->body.channel.flags = 0;
			if (rt_info.chan_num)
				chan_num = rt_info.chan_num;
			else
				chan_num =
					handle->mon_if->band_chan_cfg.channel;
			band = (chan_num <= 14) ? IEEE80211_BAND_2GHZ :
						  IEEE80211_BAND_5GHZ;
			rth->body.channel.frequency = woal_cpu_to_le16(
				ieee80211_channel_to_frequency(chan_num, band));
			rth->body.channel.flags |=
				woal_cpu_to_le16((band == IEEE80211_BAND_2GHZ) ?
							 CHANNEL_FLAGS_2GHZ :
							 CHANNEL_FLAGS_5GHZ);
			if (rth->body.channel.flags &
			    woal_cpu_to_le16(CHANNEL_FLAGS_2GHZ))
				rth->body.channel.flags |= woal_cpu_to_le16(
					CHANNEL_FLAGS_DYNAMIC_CCK_OFDM);
			else
				rth->body.channel.flags |=
					woal_cpu_to_le16(CHANNEL_FLAGS_OFDM);
			if (handle->mon_if->chandef.chan &&
			    (handle->mon_if->chandef.chan->flags &
			     (IEEE80211_CHAN_PASSIVE_SCAN |
			      IEEE80211_CHAN_RADAR)))
				rth->body.channel.flags |= woal_cpu_to_le16(
					CHANNEL_FLAGS_ONLY_PASSIVSCAN_ALLOW);
			/** Antenna */
			rth->body.antenna_signal = -(rt_info.nf - rt_info.snr);
			rth->body.antenna_noise = -rt_info.nf;
			/* Convert FW antenna value to radiotap spec */
			rth->body.antenna = (t_u16)rt_info.antenna >> 1;
			/** MCS */
			if (format == MLAN_RATE_FORMAT_HT) {
				rth->hdr.it_present |= cpu_to_le32(
					1 << IEEE80211_RADIOTAP_MCS);
				rth->body.u.mcs.known =
					rt_info.extra_info.mcs_known;
				rth->body.u.mcs.flags =
					rt_info.extra_info.mcs_flags;
				/** MCS mcs */
				rth->body.u.mcs.known |=
					MCS_KNOWN_MCS_INDEX_KNOWN;
				rth->body.u.mcs.mcs =
					rt_info.rate_info.mcs_index;
				/** MCS bw */
				rth->body.u.mcs.known |= MCS_KNOWN_BANDWIDTH;
				rth->body.u.mcs.flags &= ~(0x03); /** Clear,
								     20MHz as
								     default */
				if (bw == 1)
					rth->body.u.mcs.flags |= RX_BW_40;
				/** MCS gi */
				rth->body.u.mcs.known |=
					MCS_KNOWN_GUARD_INTERVAL;
				rth->body.u.mcs.flags &= ~(1 << 2);
				if (gi)
					rth->body.u.mcs.flags |= gi << 2;
				/** MCS FEC */
				rth->body.u.mcs.known |= MCS_KNOWN_FEC_TYPE;
				rth->body.u.mcs.flags &= ~(1 << 4);
				if (ldpc)
					rth->body.u.mcs.flags |= ldpc << 4;
			}
			/** VHT */
			if (format == MLAN_RATE_FORMAT_VHT) {
				vht_sig1 = rt_info.extra_info.vht_sig1;
				vht_sig2 = rt_info.extra_info.vht_sig2;
				/** Present Flag */
				rth->hdr.it_present |= cpu_to_le32(
					1 << IEEE80211_RADIOTAP_VHT);
				/** STBC */
				rth->body.u.vht.known |=
					woal_cpu_to_le16(VHT_KNOWN_STBC);
				if (vht_sig1 & MBIT(3))
					rth->body.u.vht.flags |= VHT_FLAG_STBC;
				/** TXOP_PS_NA */
				/** TODO: Not support now */
				/** GI */
				rth->body.u.vht.known |=
					woal_cpu_to_le16(VHT_KNOWN_GI);
				if (vht_sig2 & MBIT(0))
					rth->body.u.vht.flags |= VHT_FLAG_SGI;
				/** SGI NSYM DIS */
				rth->body.u.vht.known |= woal_cpu_to_le16(
					VHT_KNOWN_SGI_NSYM_DIS);
				if (vht_sig2 & MBIT(1))
					rth->body.u.vht.flags |=
						VHT_FLAG_SGI_NSYM_M10_9;
				/** LDPC_EXTRA_OFDM_SYM */
				/** TODO: Not support now */
				/** BEAMFORMED */
				rth->body.u.vht.known |=
					woal_cpu_to_le16(VHT_KNOWN_BEAMFORMED);
				if (vht_sig2 & MBIT(8))
					rth->body.u.vht.flags |=
						VHT_FLAG_BEAMFORMED;
				/** BANDWIDTH */
				rth->body.u.vht.known |=
					woal_cpu_to_le16(VHT_KNOWN_BANDWIDTH);
				if (bw == 1)
					rth->body.u.vht.bandwidth = RX_BW_40;
				else if (bw == 2)
					rth->body.u.vht.bandwidth = RX_BW_80;
				/** GROUP_ID */
				rth->body.u.vht.known |=
					woal_cpu_to_le16(VHT_KNOWN_GROUP_ID);
				rth->body.u.vht.group_id =
					(vht_sig1 & (0x3F0)) >> 4;
				/** PARTIAL_AID */
				/** TODO: Not support now */
				/** mcs_nss */
				rth->body.u.vht.mcs_nss[0] = vht_sig2 & (0xF0);
				/* Convert FW NSS value to radiotap spec */
				rth->body.u.vht.mcs_nss[0] |=
					((vht_sig1 & (0x1C00)) >> 10) + 1;
				/** coding */
				if (vht_sig2 & MBIT(2))
					rth->body.u.vht.coding |=
						VHT_CODING_LDPC_USER0;
			}
		}
		skb_set_mac_header(skb, 0);
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		skb->pkt_type = PACKET_OTHERHOST;
		skb->protocol = htons(ETH_P_802_2);
		memset(skb->cb, 0, sizeof(skb->cb));
		skb->dev = handle->mon_if->mon_ndev;

		handle->mon_if->stats.rx_bytes += skb->len;
		handle->mon_if->stats.rx_packets++;

		if (in_interrupt())
			netif_rx(skb);
		else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
			netif_rx(skb);
#else
			netif_rx_ni(skb);
#endif

		status = MLAN_STATUS_PENDING;
	}

done:

	LEAVE();
	return status;
}
#endif
#endif

/**
 *  @brief This function uploads amsdu packet to the network stack
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *
 *  @return         MLAN_STATUS_PENDING or MLAN_STATUS_FAILURE
 */
mlan_status moal_recv_amsdu_packet(t_void *pmoal, pmlan_buffer pmbuf)
{
	mlan_status status = MLAN_STATUS_FAILURE;
	struct sk_buff *skb = NULL;
	struct sk_buff *frame = NULL;
	int remaining;
	const struct ethhdr *eth;
	u8 dst[ETH_ALEN], src[ETH_ALEN];
	moal_handle *handle = (moal_handle *)pmoal;
	moal_private *priv = NULL;
	struct net_device *netdev = NULL;
	u8 *payload;
	mlan_buffer mbuf;
	t_u8 drop = 0;
	t_u8 rfc1042_eth_hdr[MLAN_MAC_ADDR_LENGTH] = {0xaa, 0xaa, 0x03,
						      0x00, 0x00, 0x00};

	wifi_timeval t1, t2;
	t_s32 delay;
	t_u32 in_ts_sec = 0;
	t_u32 in_ts_usec = 0;

	ENTER();
	if (!pmbuf) {
		PRINTM(MERROR, "%s: pmbuf is null\n", __func__);
		goto done;
	}

	memset(&mbuf, 0, sizeof(mlan_buffer));
	mbuf.bss_index = pmbuf->bss_index;

	priv = woal_bss_index_to_priv(pmoal, pmbuf->bss_index);
	if (priv == NULL) {
		PRINTM(MERROR, "%s: priv is null\n", __func__);
		goto done;
	}
	netdev = priv->netdev;
	skb = (struct sk_buff *)pmbuf->pdesc;
	if (!skb)
		goto done;

	skb_reserve(skb, pmbuf->data_offset);
	if (skb_tailroom(skb) < (int)pmbuf->data_len) {
		PRINTM(MERROR, "skb overflow: tail room=%d, data_len=%d\n",
		       skb_tailroom(skb), pmbuf->data_len);
		goto done;
	}
	skb_put(skb, pmbuf->data_len);

	// rx_trace 8
	if (handle->tp_acnt.on) {
		moal_tp_accounting(pmoal, skb, RX_DROP_P4);
		woal_get_monotonic_time(&t1);
		in_ts_sec = t1.time_sec;
		in_ts_usec = t1.time_usec;
		if (pmbuf->in_ts_sec) {
			pmbuf->out_ts_sec = t1.time_sec;
			pmbuf->out_ts_usec = t1.time_usec;
		}
	}
	if (handle->tp_acnt.drop_point == RX_DROP_P4) {
		status = MLAN_STATUS_PENDING;
		dev_kfree_skb(skb);
		goto done;
	}

	while (skb != frame) {
		__be16 len;
		u8 padding;
		unsigned int subframe_len;
		eth = (struct ethhdr *)skb->data;
		len = ntohs(eth->h_proto);
		subframe_len = sizeof(struct ethhdr) + len;
		remaining = skb->len;

		if (subframe_len > remaining) {
			PRINTM(MERROR,
			       "Error in len: remaining = %d, subframe_len = %d\n",
			       remaining, subframe_len);
			break;
		}
		memcpy(dst, eth->h_dest, ETH_ALEN);
		memcpy(src, eth->h_source, ETH_ALEN);

		padding = (4 - subframe_len) & 0x3;

		skb_pull(skb, sizeof(struct ethhdr));

		if (remaining <= (subframe_len + padding)) {
			frame = skb;
			status = MLAN_STATUS_PENDING;
		} else {
			frame = skb_clone(skb, GFP_ATOMIC);
			skb_trim(frame, len);
			eth = (struct ethhdr *)skb_pull(skb, len + padding);
			if (!eth) {
				PRINTM(MERROR, "Invalid amsdu packet\n");
				break;
			}
		}
		skb_reset_network_header(frame);
		frame->dev = netdev;
		frame->priority = skb->priority;
		payload = frame->data;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
		if (ether_addr_equal(payload, rfc1042_eth_hdr)) {
#else
		if (!memcmp(payload, rfc1042_eth_hdr,
			    sizeof(rfc1042_eth_hdr))) {
#endif
			/* Remove RFC1042 */
			skb_pull(frame, 6);
			memcpy(skb_push(frame, ETH_ALEN), src, ETH_ALEN);
			memcpy(skb_push(frame, ETH_ALEN), dst, ETH_ALEN);
		} else {
			memcpy(skb_push(frame, sizeof(__be16)), &len,
			       sizeof(__be16));
			memcpy(skb_push(frame, ETH_ALEN), src, ETH_ALEN);
			memcpy(skb_push(frame, ETH_ALEN), dst, ETH_ALEN);
		}
		mbuf.pbuf = frame->data;
		mbuf.data_len = frame->len;
		mlan_process_deaggr_pkt(handle->pmlan_adapter, &mbuf, &drop);
		if (drop) {
			dev_kfree_skb(frame);
			continue;
		}
		frame->protocol = eth_type_trans(frame, netdev);
		frame->ip_summed = CHECKSUM_NONE;
		if (in_interrupt())
			netif_rx(frame);
		else {
			if (atomic_read(&handle->rx_pending) >
			    MAX_RX_PENDING_THRHLD)
				netif_rx(frame);
			else {
				if (handle->params.net_rx == MTRUE) {
					local_bh_disable();
					netif_receive_skb(frame);
					local_bh_enable();
				} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
					netif_rx(frame);
#else
					netif_rx_ni(frame);
#endif
				}
			}
		}
	}
	if (handle->tp_acnt.on) {
		if (pmbuf->in_ts_sec)
			moal_tp_accounting(handle, pmbuf, RX_TIME_PKT);

		woal_get_monotonic_time(&t2);
		delay = (t_s32)(t2.time_sec - in_ts_sec) * 1000000;
		delay += (t_s32)(t2.time_usec - in_ts_usec);
		moal_amsdu_tp_accounting(pmoal, delay, 0);
	}
done:
	if (status == MLAN_STATUS_PENDING)
		atomic_dec(&handle->mbufalloc_count);
	LEAVE();
	return status;
}

/**
 *  @brief This function uploads the packet to the network stack
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf    Pointer to the mlan buffer structure
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_recv_packet(t_void *pmoal, pmlan_buffer pmbuf)
{
	mlan_status status = MLAN_STATUS_SUCCESS;
	moal_private *priv = NULL;
	struct sk_buff *skb = NULL;
	moal_handle *handle = (moal_handle *)pmoal;
#if defined(USB) || defined(PCIE)
	t_u32 max_rx_data_size = MLAN_RX_DATA_BUF_SIZE;
#endif
	dot11_rxcontrol rxcontrol;
	t_u8 rx_info_flag = MFALSE;
	int j;
	struct ethhdr *ethh = NULL;
	struct net_device *netdev = NULL;

	ENTER();
	if (pmbuf) {
#ifdef USB
#ifdef STA_SUPPORT
		if (IS_USB(handle->card_type)) {
			struct usb_card_rec *cardp =
				(struct usb_card_rec *)handle->card;
			if (cardp->rx_deaggr_ctrl.enable) {
				max_rx_data_size =
					cardp->rx_deaggr_ctrl.aggr_max;
				if (cardp->rx_deaggr_ctrl.aggr_mode ==
				    MLAN_USB_AGGR_MODE_NUM) {
					max_rx_data_size *=
						MAX(MLAN_USB_MAX_PKT_SIZE,
						    cardp->rx_deaggr_ctrl
							    .aggr_align);
					max_rx_data_size =
						MAX(max_rx_data_size,
						    MLAN_RX_DATA_BUF_SIZE);
				}
			}
		}
#endif
#endif

		priv = woal_bss_index_to_priv(pmoal, pmbuf->bss_index);
		skb = (struct sk_buff *)pmbuf->pdesc;
		if (priv) {
			if (skb) {
				skb_reserve(skb, pmbuf->data_offset);
				if (skb_tailroom(skb) < (int)pmbuf->data_len) {
					PRINTM(MERROR,
					       "skb overflow: tail room=%d, data_len=%d\n",
					       skb_tailroom(skb),
					       pmbuf->data_len);
					status = MLAN_STATUS_FAILURE;
					priv->stats.rx_dropped++;
					goto done;
				}
				skb_put(skb, pmbuf->data_len);
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
				if (pmbuf->flags & MLAN_BUF_FLAG_NET_MONITOR) {
					status = moal_recv_packet_to_mon_if(
						pmoal, pmbuf);
					if (status == MLAN_STATUS_PENDING)
						atomic_dec(
							&handle->mbufalloc_count);
					goto done;
				}
#endif
#endif
				pmbuf->pdesc = NULL;
				pmbuf->pbuf = NULL;
				pmbuf->data_offset = pmbuf->data_len = 0;
				/* pkt been submit to kernel, no need to
				 * free by mlan*/
				status = MLAN_STATUS_PENDING;
				atomic_dec(&handle->mbufalloc_count);
			} else {
				PRINTM(MERROR, "%s without skb attach!!!\n",
				       __func__);
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
				/** drop the packet without skb in
				 * monitor mode */
				if (pmbuf->flags & MLAN_BUF_FLAG_NET_MONITOR) {
					PRINTM(MINFO,
					       "%s Drop packet without skb\n",
					       __func__);
					status = MLAN_STATUS_FAILURE;
					priv->stats.rx_dropped++;
					goto done;
				}
#endif
#endif
				skb = dev_alloc_skb(pmbuf->data_len +
						    MLAN_NET_IP_ALIGN);
				if (!skb) {
					PRINTM(MERROR, "%s fail to alloc skb\n",
					       __func__);
					status = MLAN_STATUS_FAILURE;
					priv->stats.rx_dropped++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
					woal_packet_fate_monitor(
						priv, PACKET_TYPE_RX,
						RX_PKT_FATE_DRV_DROP_NOBUFS,
						FRAME_TYPE_ETHERNET_II, 0, 0,
						(t_u8 *)(pmbuf->pbuf +
							 pmbuf->data_offset),
						pmbuf->data_len);
#endif
#endif
					goto done;
				}
				skb_reserve(skb, MLAN_NET_IP_ALIGN);
				moal_memcpy_ext(handle, skb->data,
						(t_u8 *)(pmbuf->pbuf +
							 pmbuf->data_offset),
						pmbuf->data_len,
						pmbuf->data_len);
				skb_put(skb, pmbuf->data_len);
			}
			ethh = (struct ethhdr *)(skb->data);
			if (ntohs(ethh->h_proto) == ETH_P_PAE)
				PRINTM(MEVENT,
				       "wlan: %s Rx EAPOL pkt from " MACSTR
				       "\n",
				       priv->netdev->name,
				       MAC2STR(ethh->h_source));
			if (!netdev)
				netdev = priv->netdev;
			skb->dev = netdev;
			skb->protocol = eth_type_trans(skb, netdev);
			skb->ip_summed = CHECKSUM_NONE;

#if defined(USB) || defined(PCIE)
			/* This is only required only in case of 11n and
			 USB as we alloc if(skb_tailroom(skb) <
			 pmbuf->data_len){ PRINTM(MERROR,"skb overflow:
			 tail room=%d, data_len\n", skb_tailroom(skb),
			 pmbuf->data_len); status = MLAN_STATUS_FAILURE;
				priv->stats.rx_dropped++;
				goto done;
			    }
			 * a buffer of 4K only if its 11N (to be able to
			 receive 4K AMSDU
			 * packets). In case of SD we allocate buffers
			 based on the size
			 * of packet and hence this is not needed.
			*/
			/* Modifying the truesize here as our allocation
			 * for each skb is 4K but we only receive 2K
			 * packets and this cause the kernel to start
			 * dropping packets in case where application
			 * has allocated buffer based on 2K size i.e. if
			 * there a 64K packet received (in IP fragments
			 * and application allocates 64K to receive this
			 * packet but this packet would almost double up
			 * because we allocate each 1.5K fragment in 4K
			 * and pass it up. As soon as the 64K limit hits
			 * kernel will start to drop rest of the
			 * fragments. Currently we fail the
			 * Filesndl-ht.scr script for UDP, hence this
			 * fix
			 */
			if (!IS_SD(priv->phandle->card_type)) {
				if (skb->truesize > max_rx_data_size)
					skb->truesize +=
						(skb->len - max_rx_data_size);
			}
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (!woal_filter_packet(priv, skb->data, skb->len, 0)) {
				PRINTM(MEVENT, "drop filtered packet %s\n",
				       priv->netdev->name);
				status = MLAN_STATUS_FAILURE;
				priv->stats.rx_dropped++;
				woal_packet_fate_monitor(
					priv, PACKET_TYPE_RX,
					RX_PKT_FATE_DRV_DROP_FILTER,
					FRAME_TYPE_ETHERNET_II, 0, 0, skb->data,
					skb->len);
				dev_kfree_skb(skb);
				goto done;
			}
#endif
#endif
			priv->stats.rx_bytes += skb->len;
			priv->stats.rx_packets++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			woal_packet_fate_monitor(priv, PACKET_TYPE_RX,
						 RX_PKT_FATE_SUCCESS,
						 FRAME_TYPE_ETHERNET_II, 0, 0,
						 skb->data, skb->len);
#endif
#endif
#ifdef ANDROID_KERNEL
			if (handle->params.wakelock_timeout) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
				__pm_wakeup_event(
					&handle->ws,
					handle->params.wakelock_timeout);
#else
				wake_lock_timeout(
					&handle->wake_lock,
					msecs_to_jiffies(
						handle->params
							.wakelock_timeout));
#endif
			}
#endif
			if (priv->rx_protocols.protocol_num) {
				for (j = 0; j < priv->rx_protocols.protocol_num;
				     j++) {
					if (htons((__force t_u16)
							  skb->protocol) ==
					    priv->rx_protocols.protocols[j])
						rx_info_flag = MTRUE;
				}
			}
			if (rx_info_flag &&
			    (skb_tailroom(skb) > (int)sizeof(rxcontrol))) {
				memset(&rxcontrol, 0, sizeof(dot11_rxcontrol));
				rxcontrol.datarate = pmbuf->u.rx_info.data_rate;
				rxcontrol.channel = pmbuf->u.rx_info.channel;
				rxcontrol.antenna = pmbuf->u.rx_info.antenna;
				rxcontrol.rssi = pmbuf->u.rx_info.rssi;
				skb_put(skb, sizeof(dot11_rxcontrol));
				memmove(skb->data + sizeof(dot11_rxcontrol),
					skb->data,
					skb->len - sizeof(dot11_rxcontrol));
				moal_memcpy_ext(handle, skb->data, &rxcontrol,
						sizeof(dot11_rxcontrol),
						sizeof(dot11_rxcontrol));
			}
			// rx_trace 8
			if (priv->phandle->tp_acnt.on) {
				wifi_timeval t;
				moal_tp_accounting(handle, skb, RX_DROP_P4);
				if (pmbuf && pmbuf->in_ts_sec) {
					woal_get_monotonic_time(&t);
					pmbuf->out_ts_sec = t.time_sec;
					pmbuf->out_ts_usec = t.time_usec;
				}
			}
			if (priv->phandle->tp_acnt.drop_point == RX_DROP_P4) {
				status = MLAN_STATUS_PENDING;
				dev_kfree_skb(skb);
			} else if (in_interrupt())
				netif_rx(skb);
			else {
				if (atomic_read(&handle->rx_pending) >
				    MAX_RX_PENDING_THRHLD)
					netif_rx(skb);
				else {
					if (handle->params.net_rx == MTRUE) {
						local_bh_disable();
						netif_receive_skb(skb);
						local_bh_enable();
					} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
						netif_rx(skb);
#else
						netif_rx_ni(skb);
#endif
					}
				}
			}
			if (priv->phandle->tp_acnt.on) {
				if (pmbuf && pmbuf->in_ts_sec)
					moal_tp_accounting(handle, pmbuf,
							   RX_TIME_PKT);
			}
		}
	}
done:
	LEAVE();
	return status;
}

#if defined(PCIE) || defined(SDIO)
void woal_request_busfreq_pmqos_add(t_void *handle)
{
	moal_handle *pmhandle = (moal_handle *)handle;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 70)
#ifdef IMX_SUPPORT
	if (IS_PCIE(pmhandle->card_type)) {
		request_bus_freq(BUS_FREQ_HIGH);
	}
#endif
#endif
	if (moal_extflg_isset(pmhandle, EXT_PMQOS)) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5, 6, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#ifdef IMX_SUPPORT
		if (!pm_qos_request_active(&pmhandle->woal_pm_qos_req))
			pm_qos_add_request(&pmhandle->woal_pm_qos_req,
					   PM_QOS_CPU_DMA_LATENCY, 0);
		else
			PRINTM(MERROR, "PM-QOS request already active\n");
#endif
#endif
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
#ifdef IMX_SUPPORT
		if (!cpu_latency_qos_request_active(&pmhandle->woal_pm_qos_req))
			cpu_latency_qos_add_request(&pmhandle->woal_pm_qos_req,
						    0);
		else
			PRINTM(MERROR, "PM-QOS request already active\n");
#endif
#endif
	}
	return;
}

void woal_release_busfreq_pmqos_remove(t_void *handle)
{
	moal_handle *pmhandle = (moal_handle *)handle;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 70)
#ifdef IMX_SUPPORT
	if (IS_PCIE(pmhandle->card_type)) {
		release_bus_freq(BUS_FREQ_HIGH);
	}
#endif
#endif

	if (moal_extflg_isset(pmhandle, EXT_PMQOS)) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5, 6, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#ifdef IMX_SUPPORT
		if (pm_qos_request_active(&pmhandle->woal_pm_qos_req))
			pm_qos_remove_request(&pmhandle->woal_pm_qos_req);
		else
			PRINTM(MERROR, "PM-QOS request already removed\n");
#endif
#endif
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
#ifdef IMX_SUPPORT
		if (cpu_latency_qos_request_active(&pmhandle->woal_pm_qos_req))
			cpu_latency_qos_remove_request(
				&pmhandle->woal_pm_qos_req);
		else
			PRINTM(MERROR, "PM-QOS request already removed\n");
#endif
#endif
	}
	return;
}
#endif /*defined(PCIE) || defined(SDIO)*/

/**
 *  @brief This function checks media_connected state for
 *  BSS types UAP/STA/P2P_GO/GC
 *
 *  @param pmoal Pointer to the MOAL context
 *
 */
int woal_check_media_connected(t_void *pmoal)
{
	int i;
	moal_handle *pmhandle = (moal_handle *)pmoal;
	moal_private *pmpriv = NULL;
	for (i = 0; i < pmhandle->priv_num; i++) {
		pmpriv = pmhandle->priv[i];
		if (!pmpriv)
			continue;
		if (pmpriv->media_connected == MTRUE) {
			return MTRUE;
		}
	}
	return MFALSE;
}

/**
 *  @brief This function checks connect and disconnect
 *  events for BSS types UAP/STA/P2P_GO/GC
 *
 *  @param pmoal Pointer to the MOAL context
 *
 */
void moal_connection_status_check_pmqos(t_void *pmoal)
{
	moal_handle *pmhandle = (moal_handle *)pmoal;
	if ((woal_check_media_connected(pmoal) == MTRUE)) {
		if (pmhandle->request_pm == MFALSE) {
			pmhandle->request_pm = MTRUE;
#if defined(PCIE) || defined(SDIO)
			woal_request_busfreq_pmqos_add(pmhandle);
#endif
		}
	} else {
		if (pmhandle->request_pm == MTRUE) {
			pmhandle->request_pm = MFALSE;
#if defined(PCIE) || defined(SDIO)
			woal_release_busfreq_pmqos_remove(pmhandle);
#endif
		}
	}
}

#ifdef UAP_SUPPORT
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
/**
 * @brief   Handle RX MGMT PKT event
 *
 * @param priv          A pointer moal_private structure
 * @param pkt        A pointer to pkt
 * @param len        length of pkt
 *
 * @return          N/A
 */
static void woal_rx_mgmt_pkt_event(moal_private *priv, t_u8 *pkt, t_u16 len)
{
	struct woal_event *evt;
	unsigned long flags;
	moal_handle *handle = priv->phandle;

	evt = kzalloc(sizeof(struct woal_event), GFP_ATOMIC);
	if (evt) {
		evt->priv = priv;
		evt->type = WOAL_EVENT_RX_MGMT_PKT;
		evt->evt.event_len = len;
		moal_memcpy_ext(priv->phandle, evt->evt.event_buf, pkt,
				evt->evt.event_len, sizeof(evt->evt.event_buf));
		INIT_LIST_HEAD(&evt->link);
		spin_lock_irqsave(&handle->evt_lock, flags);
		list_add_tail(&evt->link, &handle->evt_queue);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		queue_work(handle->evt_workqueue, &handle->evt_work);
	}
}
#endif
#endif
#endif

/**
 *  @brief This function handles event receive
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param pmevent  Pointer to the mlan event structure
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_recv_event(t_void *pmoal, pmlan_event pmevent)
{
#ifdef STA_SUPPORT
	int custom_len = 0;
#ifdef STA_CFG80211
	unsigned long flags;
#endif
#endif
	moal_private *priv = NULL;
#if defined(STA_SUPPORT) || defined(UAP_SUPPORT)
	moal_private *pmpriv = NULL;
#endif
#if defined(STA_WEXT) || defined(UAP_WEXT)
#if defined(STA_SUPPORT) || defined(UAP_WEXT)
#if defined(UAP_SUPPORT) || defined(STA_WEXT)
	union iwreq_data wrqu;
#endif
#endif
#endif
	mlan_ds_ps_info pm_info;
	moal_handle *handle = (moal_handle *)pmoal;
	moal_handle *ref_handle = NULL;

#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	t_u8 enable = 1;
#endif
	t_u8 *req_ie = NULL;
	t_u16 ie_len = 0;
	apinfo *pinfo = NULL, *req_tlv = NULL;
	MrvlIEtypesHeader_t *tlv = NULL;
	t_u16 tlv_type = 0, tlv_len = 0, tlv_buf_left = 0;
#endif
#ifdef STA_CFG80211
	t_u8 hw_test;
#endif
	int cfg80211_wext;

#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
	struct cfg80211_roam_info *roam_info = NULL;
#endif
#endif
	t_u16 csi_len;
#ifdef STA_CFG80211
	t_u8 channel_status;
	moal_private *remain_priv = NULL;
#endif
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
	chan_band_info *pchan_info = NULL;
#endif
	t_u8 radar_detected;
	t_u8 bandwidth;
	t_u8 event_buf[64];
	t_u8 radar_chan;
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	moal_private *cfg_priv = NULL;
#endif
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
#ifdef UAP_CFG80211
	unsigned long wait_time, wait_time_ms, timeout;
#endif
#endif

	t_u8 auto_fw_dump = MFALSE;
	ENTER();
	if (pmevent->event_id == MLAN_EVENT_ID_FW_DUMP_INFO) {
		if (!handle->is_fw_dump_timer_set) {
			PRINTM(MMSG, "FW trigger fw dump\n");
			handle->is_fw_dump_timer_set = MTRUE;
			woal_mod_timer(&handle->fw_dump_timer, MOAL_TIMER_5S);
		}
		woal_store_firmware_dump(pmoal, pmevent);
		handle->driver_status = MTRUE;
		wifi_status = WIFI_STATUS_FW_DUMP;
		ref_handle = (moal_handle *)handle->pref_mac;
		if (ref_handle)
			ref_handle->driver_status = MTRUE;
		goto done;
	}
	if ((pmevent->event_id != MLAN_EVENT_ID_DRV_DEFER_RX_WORK) &&
	    (pmevent->event_id != MLAN_EVENT_ID_DRV_DEFER_HANDLING) &&
	    (pmevent->event_id != MLAN_EVENT_ID_DRV_MGMT_FRAME))
		PRINTM(MEVENT, "event id:0x%x\n", pmevent->event_id);
#if defined(PCIE)
	if (pmevent->event_id == MLAN_EVENT_ID_SSU_DUMP_FILE) {
		goto done;
	}
#endif /* SSU_SUPPORT */
	if (pmevent->event_id == MLAN_EVENT_ID_STORE_HOST_CMD_RESP) {
		goto done;
	}
	priv = woal_bss_index_to_priv(pmoal, pmevent->bss_index);
	if (priv == NULL) {
		PRINTM(MERROR, "%s: priv is null\n", __func__);
		goto done;
	}
	if (priv->netdev == NULL) {
		PRINTM(MERROR, "%s: netdev is null\n", __func__);
		goto done;
	}

	cfg80211_wext = priv->phandle->params.cfg80211_wext;
#ifdef STA_CFG80211
	hw_test = moal_extflg_isset(pmoal, EXT_HW_TEST);
#endif
	switch (pmevent->event_id) {
#ifdef STA_SUPPORT
	case MLAN_EVENT_ID_FW_ADHOC_LINK_SENSED:
		priv->is_adhoc_link_sensed = MTRUE;
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_wake_queue(priv->netdev);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_ADHOC_LINK_SENSED);
#endif
		woal_broadcast_event(priv, CUS_EVT_ADHOC_LINK_SENSED,
				     strlen(CUS_EVT_ADHOC_LINK_SENSED));
		break;

	case MLAN_EVENT_ID_FW_ADHOC_LINK_LOST:
		woal_stop_queue(priv->netdev);
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
		priv->is_adhoc_link_sensed = MFALSE;
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_ADHOC_LINK_LOST);
#endif
		woal_broadcast_event(priv, CUS_EVT_ADHOC_LINK_LOST,
				     strlen(CUS_EVT_ADHOC_LINK_LOST));
		break;

	case MLAN_EVENT_ID_DRV_CONNECTED:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext) &&
		    pmevent->event_len == ETH_ALEN) {
			memset(wrqu.ap_addr.sa_data, 0x00, ETH_ALEN);
			moal_memcpy_ext(priv->phandle, wrqu.ap_addr.sa_data,
					pmevent->event_buf, ETH_ALEN,
					sizeof(wrqu.ap_addr.sa_data));
			wrqu.ap_addr.sa_family = ARPHRD_ETHER;
			wireless_send_event(priv->netdev, SIOCGIWAP, &wrqu,
					    NULL);
		}
#endif
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			moal_memcpy_ext(priv->phandle, priv->cfg_bssid,
					pmevent->event_buf, ETH_ALEN, ETH_ALEN);
			woal_set_scan_time(priv, ACTIVE_SCAN_CHAN_TIME,
					   PASSIVE_SCAN_CHAN_TIME,
					   MIN_SPECIFIC_SCAN_CHAN_TIME);
		}
#endif
		custom_len = strlen(CUS_EVT_AP_CONNECTED);
		memmove(pmevent->event_buf + custom_len, pmevent->event_buf,
			pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				CUS_EVT_AP_CONNECTED, custom_len, custom_len);
		pmevent->event_len += custom_len;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
		woal_update_dscp_mapping(priv);
		priv->media_connected = MTRUE;
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_wake_queue(priv->netdev);
		moal_connection_status_check_pmqos(pmoal);
		break;

	case MLAN_EVENT_ID_DRV_ASSOC_SUCC_LOGGER:
	case MLAN_EVENT_ID_DRV_ASSOC_FAILURE_LOGGER:
	case MLAN_EVENT_ID_DRV_DISCONNECT_LOGGER:
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		if (IS_STA_CFG80211(cfg80211_wext))
			woal_ring_event_logger(priv, VERBOSE_RING_ID, pmevent);
#endif
#endif
		break;

	case MLAN_EVENT_ID_DRV_SCAN_REPORT:
		PRINTM(MINFO, "Scan report\n");

		if (priv->report_scan_result) {
			priv->report_scan_result = MFALSE;
#ifdef STA_CFG80211
			if (IS_STA_CFG80211(cfg80211_wext)) {
				if (priv->phandle->scan_request) {
					PRINTM(MINFO,
					       "Reporting scan results\n");
					woal_inform_bss_from_scan_result(
						priv, NULL, MOAL_NO_WAIT);
					if (!priv->phandle->first_scan_done) {
						priv->phandle->first_scan_done =
							MTRUE;
						woal_set_scan_time(
							priv,
							ACTIVE_SCAN_CHAN_TIME,
							PASSIVE_SCAN_CHAN_TIME,
							SPECIFIC_SCAN_CHAN_TIME);
					}
					spin_lock_irqsave(
						&priv->phandle->scan_req_lock,
						flags);
					if (priv->phandle->scan_request) {
						cancel_delayed_work(
							&priv->phandle
								 ->scan_timeout_work);
						woal_cfg80211_scan_done(
							priv->phandle
								->scan_request,
							MFALSE);
						priv->phandle->scan_request =
							NULL;
					}
					spin_unlock_irqrestore(
						&priv->phandle->scan_req_lock,
						flags);
				}
			}
#endif /* STA_CFG80211 */

#ifdef STA_WEXT
			if (IS_STA_WEXT(cfg80211_wext)) {
				memset(&wrqu, 0, sizeof(union iwreq_data));
				wireless_send_event(priv->netdev, SIOCGIWSCAN,
						    &wrqu, NULL);
			}
#endif
			woal_broadcast_event(priv, (t_u8 *)&pmevent->event_id,
					     sizeof(mlan_event_id));
		}

		if (!is_zero_timeval(priv->phandle->scan_time_start)) {
			woal_get_monotonic_time(&priv->phandle->scan_time_end);
			priv->phandle->scan_time += (t_u64)(
				timeval_to_usec(priv->phandle->scan_time_end) -
				timeval_to_usec(
					priv->phandle->scan_time_start));
			PRINTM(MINFO,
			       "%s : start_timeval=%d:%d end_timeval=%d:%d inter=%llu scan_time=%llu\n",
			       __func__,
			       priv->phandle->scan_time_start.time_sec,
			       priv->phandle->scan_time_start.time_usec,
			       priv->phandle->scan_time_end.time_sec,
			       priv->phandle->scan_time_end.time_usec,
			       (t_u64)(timeval_to_usec(
					       priv->phandle->scan_time_end) -
				       timeval_to_usec(
					       priv->phandle->scan_time_start)),
			       priv->phandle->scan_time);
			priv->phandle->scan_time_start.time_sec = 0;
			priv->phandle->scan_time_start.time_usec = 0;
		}

		if (priv->phandle->scan_pending_on_block == MTRUE) {
			priv->phandle->scan_pending_on_block = MFALSE;
			priv->phandle->scan_priv = NULL;
			MOAL_REL_SEMAPHORE(&priv->phandle->async_sem);
		}
		break;

	case MLAN_EVENT_ID_DRV_OBSS_SCAN_PARAM:
		memmove((pmevent->event_buf + strlen(CUS_EVT_OBSS_SCAN_PARAM) +
			 1),
			pmevent->event_buf, pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				(t_u8 *)CUS_EVT_OBSS_SCAN_PARAM,
				strlen(CUS_EVT_OBSS_SCAN_PARAM),
				strlen(CUS_EVT_OBSS_SCAN_PARAM));
		pmevent->event_buf[strlen(CUS_EVT_OBSS_SCAN_PARAM)] = 0;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len +
					     strlen(CUS_EVT_OBSS_SCAN_PARAM));

#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext)) {
			memset(&wrqu, 0, sizeof(union iwreq_data));
			wrqu.data.pointer = (t_u8 __user *)pmevent->event_buf;
			wrqu.data.length = pmevent->event_len +
					   strlen(CUS_EVT_OBSS_SCAN_PARAM) + 1;
			wireless_send_event(priv->netdev, IWEVCUSTOM, &wrqu,
					    pmevent->event_buf);
		}
#endif
		break;
	case MLAN_EVENT_ID_FW_BW_CHANGED:
		memmove((pmevent->event_buf + strlen(CUS_EVT_BW_CHANGED) + 1),
			pmevent->event_buf, pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				(t_u8 *)CUS_EVT_BW_CHANGED,
				strlen(CUS_EVT_BW_CHANGED),
				strlen(CUS_EVT_BW_CHANGED));
		pmevent->event_buf[strlen(CUS_EVT_BW_CHANGED)] = 0;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len +
					     strlen(CUS_EVT_BW_CHANGED));

#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext)) {
			memset(&wrqu, 0, sizeof(union iwreq_data));
			wrqu.data.pointer = (t_u8 __user *)pmevent->event_buf;
			wrqu.data.length = pmevent->event_len +
					   strlen(CUS_EVT_BW_CHANGED) + 1;
			wireless_send_event(priv->netdev, IWEVCUSTOM, &wrqu,
					    pmevent->event_buf);
		}
#endif
		break;

	case MLAN_EVENT_ID_FW_DISCONNECTED:
		woal_send_disconnect_to_system(priv,
					       (t_u16)*pmevent->event_buf);
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		priv->auth_flag = 0;
		priv->host_mlme = MFALSE;
		priv->auth_alg = 0xFFFF;
#endif
#endif
#ifdef STA_WEXT
		/* Reset wireless stats signal info */
		if (IS_STA_WEXT(cfg80211_wext)) {
			priv->w_stats.qual.level = 0;
			priv->w_stats.qual.noise = 0;
		}
#endif
#ifdef REASSOCIATION
		if (priv->reassoc_on == MTRUE) {
			if (priv->auto_assoc_priv.auto_assoc_type_on &
			    (0x1 << (AUTO_ASSOC_TYPE_DRV_RECONN - 1))) {
				PRINTM(MINFO,
				       " auto assoc: trigger driver auto re-connect\n");
				priv->auto_assoc_priv.auto_assoc_trigger_flag =
					AUTO_ASSOC_TYPE_DRV_RECONN;
				priv->auto_assoc_priv.drv_reconnect.status =
					MTRUE;
				PRINTM(MINFO, "Reassoc: trigger the timer\n");
				priv->reassoc_required = MTRUE;
				priv->phandle->is_reassoc_timer_set = MTRUE;
				woal_mod_timer(&priv->phandle->reassoc_timer,
					       REASSOC_TIMER_DEFAULT);
			}
		} else {
			priv->rate_index = AUTO_RATE;
		}
#endif /* REASSOCIATION */
		moal_connection_status_check_pmqos(pmoal);
		break;

	case MLAN_EVENT_ID_FW_MIC_ERR_UNI:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext)) {
#if WIRELESS_EXT >= 18
			woal_send_mic_error_event(priv,
						  MLAN_EVENT_ID_FW_MIC_ERR_UNI);
#else
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_MLME_MIC_ERR_UNI);
#endif
		}
#endif /* STA_WEXT */
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			cfg80211_michael_mic_failure(priv->netdev,
						     priv->cfg_bssid,
						     NL80211_KEYTYPE_PAIRWISE,
						     -1, NULL, GFP_KERNEL);
		}
#endif
		woal_broadcast_event(priv, CUS_EVT_MLME_MIC_ERR_UNI,
				     strlen(CUS_EVT_MLME_MIC_ERR_UNI));
		break;
	case MLAN_EVENT_ID_FW_MIC_ERR_MUL:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext)) {
#if WIRELESS_EXT >= 18
			woal_send_mic_error_event(priv,
						  MLAN_EVENT_ID_FW_MIC_ERR_MUL);
#else
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_MLME_MIC_ERR_MUL);
#endif
		}
#endif /* STA_WEXT */
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			cfg80211_michael_mic_failure(priv->netdev,
						     priv->cfg_bssid,
						     NL80211_KEYTYPE_GROUP, -1,
						     NULL, GFP_KERNEL);
		}
#endif
		woal_broadcast_event(priv, CUS_EVT_MLME_MIC_ERR_MUL,
				     strlen(CUS_EVT_MLME_MIC_ERR_MUL));
		break;
	case MLAN_EVENT_ID_FW_BCN_RSSI_LOW:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_BEACON_RSSI_LOW);
#endif
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
#if CFG80211_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
			cfg80211_cqm_rssi_notify(
				priv->netdev,
				NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
				*(t_s16 *)pmevent->event_buf,
#endif
				GFP_KERNEL);
			priv->last_event |= EVENT_BCN_RSSI_LOW;
#endif
			if (!hw_test && priv->roaming_enabled)
				woal_config_bgscan_and_rssi(priv, MTRUE);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			woal_cfg80211_rssi_monitor_event(
				priv, *(t_s16 *)pmevent->event_buf);
#endif
		}
#endif
		woal_broadcast_event(priv, CUS_EVT_BEACON_RSSI_LOW,
				     strlen(CUS_EVT_BEACON_RSSI_LOW));
		break;
	case MLAN_EVENT_ID_FW_BCN_RSSI_HIGH:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_BEACON_RSSI_HIGH);
#endif
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			if (!priv->mrvl_rssi_low) {
#if CFG80211_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
				cfg80211_cqm_rssi_notify(
					priv->netdev,
					NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
					*(t_s16 *)pmevent->event_buf,
#endif
					GFP_KERNEL);
#endif
				woal_set_rssi_threshold(
					priv, MLAN_EVENT_ID_FW_BCN_RSSI_HIGH,
					MOAL_NO_WAIT);
			}
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			woal_cfg80211_rssi_monitor_event(
				priv, *(t_s16 *)pmevent->event_buf);
#endif
		}
#endif
		woal_broadcast_event(priv, CUS_EVT_BEACON_RSSI_HIGH,
				     strlen(CUS_EVT_BEACON_RSSI_HIGH));
		break;
	case MLAN_EVENT_ID_FW_BCN_SNR_LOW:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_BEACON_SNR_LOW);
#endif
		woal_broadcast_event(priv, CUS_EVT_BEACON_SNR_LOW,
				     strlen(CUS_EVT_BEACON_SNR_LOW));
		break;
	case MLAN_EVENT_ID_FW_BCN_SNR_HIGH:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_BEACON_SNR_HIGH);
#endif
		woal_broadcast_event(priv, CUS_EVT_BEACON_SNR_HIGH,
				     strlen(CUS_EVT_BEACON_SNR_HIGH));
		break;
	case MLAN_EVENT_ID_FW_MAX_FAIL:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_MAX_FAIL);
#endif
		woal_broadcast_event(priv, CUS_EVT_MAX_FAIL,
				     strlen(CUS_EVT_MAX_FAIL));
		break;
	case MLAN_EVENT_ID_FW_DATA_RSSI_LOW:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_DATA_RSSI_LOW);
#endif
		woal_broadcast_event(priv, CUS_EVT_DATA_RSSI_LOW,
				     strlen(CUS_EVT_DATA_RSSI_LOW));
		break;
	case MLAN_EVENT_ID_FW_DATA_SNR_LOW:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_DATA_SNR_LOW);
#endif
		woal_broadcast_event(priv, CUS_EVT_DATA_SNR_LOW,
				     strlen(CUS_EVT_DATA_SNR_LOW));
		break;
	case MLAN_EVENT_ID_FW_DATA_RSSI_HIGH:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_DATA_RSSI_HIGH);
#endif
		woal_broadcast_event(priv, CUS_EVT_DATA_RSSI_HIGH,
				     strlen(CUS_EVT_DATA_RSSI_HIGH));
		break;
	case MLAN_EVENT_ID_FW_DATA_SNR_HIGH:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_DATA_SNR_HIGH);
#endif
		woal_broadcast_event(priv, CUS_EVT_DATA_SNR_HIGH,
				     strlen(CUS_EVT_DATA_SNR_HIGH));
		break;
	case MLAN_EVENT_ID_FW_LINK_QUALITY:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_LINK_QUALITY);
#endif
		woal_broadcast_event(priv, CUS_EVT_LINK_QUALITY,
				     strlen(CUS_EVT_LINK_QUALITY));
		break;
	case MLAN_EVENT_ID_FW_PORT_RELEASE:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_PORT_RELEASE);
#endif
		woal_broadcast_event(priv, CUS_EVT_PORT_RELEASE,
				     strlen(CUS_EVT_PORT_RELEASE));
		break;
	case MLAN_EVENT_ID_FW_PRE_BCN_LOST:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_PRE_BEACON_LOST);
#endif
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
		if (IS_STA_CFG80211(cfg80211_wext)) {
			struct cfg80211_bss *bss = NULL;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
			bss = cfg80211_get_bss(priv->wdev->wiphy, NULL,
					       priv->cfg_bssid, NULL, 0,
					       IEEE80211_BSS_TYPE_ESS,
					       IEEE80211_PRIVACY_ANY);

#else
			bss = cfg80211_get_bss(priv->wdev->wiphy, NULL,
					       priv->cfg_bssid, NULL, 0,
					       WLAN_CAPABILITY_ESS,
					       WLAN_CAPABILITY_ESS);
#endif
			if (bss) {
				cfg80211_unlink_bss(priv->wdev->wiphy, bss);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
				cfg80211_put_bss(priv->wdev->wiphy, bss);
#else
				cfg80211_put_bss(bss);
#endif
			}
			if (!hw_test && priv->roaming_enabled)
				woal_config_bgscan_and_rssi(priv, MFALSE);
			else {
				cfg80211_cqm_rssi_notify(
					priv->netdev,
					NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
					0,
#endif
					GFP_KERNEL);
			}
			priv->last_event |= EVENT_PRE_BCN_LOST;
		}
#endif
#endif
		woal_broadcast_event(priv, CUS_EVT_PRE_BEACON_LOST,
				     strlen(CUS_EVT_PRE_BEACON_LOST));
		break;
	case MLAN_EVENT_ID_FW_DEBUG_INFO:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, pmevent->event_buf);
#endif
		memmove((pmevent->event_buf + strlen(FW_DEBUG_INFO) + 1),
			pmevent->event_buf, pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				(t_u8 *)FW_DEBUG_INFO, strlen(FW_DEBUG_INFO),
				strlen(FW_DEBUG_INFO));
		pmevent->event_buf[strlen(FW_DEBUG_INFO)] = 0;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len +
					     strlen(FW_DEBUG_INFO) + 1);
		break;
	case MLAN_EVENT_ID_FW_WMM_CONFIG_CHANGE:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(
				priv, WMM_CONFIG_CHANGE_INDICATION);
#endif
		woal_broadcast_event(priv, WMM_CONFIG_CHANGE_INDICATION,
				     strlen(WMM_CONFIG_CHANGE_INDICATION));
		break;

	case MLAN_EVENT_ID_DRV_REPORT_STRING:
		PRINTM(MINFO, "Report string %s\n", pmevent->event_buf);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, pmevent->event_buf);
#endif
		woal_broadcast_event(priv, pmevent->event_buf,
				     strlen(pmevent->event_buf));
		break;
	case MLAN_EVENT_ID_FW_WEP_ICV_ERR:
		DBG_HEXDUMP(MCMD_D, "WEP ICV error", pmevent->event_buf,
			    pmevent->event_len);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv, CUS_EVT_WEP_ICV_ERR);
#endif
		woal_broadcast_event(priv, CUS_EVT_WEP_ICV_ERR,
				     strlen(CUS_EVT_WEP_ICV_ERR));
		break;

	case MLAN_EVENT_ID_DRV_DEFER_HANDLING:
		queue_work(priv->phandle->workqueue, &priv->phandle->main_work);
		break;
	case MLAN_EVENT_ID_DRV_FLUSH_RX_WORK:
		if (moal_extflg_isset(priv->phandle, EXT_NAPI)) {
			napi_synchronize(&priv->phandle->napi_rx);
			break;
		}
		flush_workqueue(priv->phandle->rx_workqueue);
		break;
	case MLAN_EVENT_ID_DRV_FLUSH_MAIN_WORK:
		flush_workqueue(priv->phandle->workqueue);
		break;
	case MLAN_EVENT_ID_DRV_DEFER_RX_WORK:
		if (moal_extflg_isset(priv->phandle, EXT_NAPI)) {
			napi_schedule(&priv->phandle->napi_rx);
			break;
		}
		queue_work(priv->phandle->rx_workqueue,
			   &priv->phandle->rx_work);
		break;
	case MLAN_EVENT_ID_DRV_DBG_DUMP:
		priv->phandle->driver_status = MTRUE;
		ref_handle = (moal_handle *)priv->phandle->pref_mac;
		if (ref_handle)
			ref_handle->driver_status = MTRUE;
#ifdef DEBUG_LEVEL1
		if (drvdbg & MFW_D)
			auto_fw_dump = MTRUE;
#endif

		woal_moal_debug_info(priv, NULL, MFALSE);
		if (!auto_fw_dump && !handle->fw_dump)
			woal_process_hang(priv->phandle);
		wifi_status = WIFI_STATUS_WIFI_HANG;
		break;
	case MLAN_EVENT_ID_DRV_WIFI_STATUS:
		wifi_status = *(t_u16 *)(pmevent->event_buf + sizeof(t_u32));
		break;
	case MLAN_EVENT_ID_FW_BG_SCAN:
		if (priv->media_connected == MTRUE)
			priv->bg_scan_start = MFALSE;
		priv->bg_scan_reported = MTRUE;
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext)) {
			memset(&wrqu, 0, sizeof(union iwreq_data));
			wireless_send_event(priv->netdev, SIOCGIWSCAN, &wrqu,
					    NULL);
		}
#endif
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			priv->last_event |= EVENT_BG_SCAN_REPORT;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
			if (priv->sched_scanning &&
			    !priv->phandle->cfg80211_suspend) {
				mlan_scan_resp scan_resp;
				if (MLAN_STATUS_SUCCESS ==
				    woal_get_scan_table(priv, MOAL_NO_WAIT,
							&scan_resp))
					PRINTM(MIOCTL,
					       "Triggered mlan get bgscan result\n");
			}
#endif
			if (!hw_test && priv->roaming_enabled
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
			    && !priv->phandle->cfg80211_suspend
#endif
			) {
				priv->roaming_required = MTRUE;
#ifdef ANDROID_KERNEL
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
				__pm_wakeup_event(&priv->phandle->ws,
						  ROAMING_WAKE_LOCK_TIMEOUT);
#else
				wake_lock_timeout(
					&priv->phandle->wake_lock,
					msecs_to_jiffies(
						ROAMING_WAKE_LOCK_TIMEOUT));
#endif
#endif
#ifdef REASSOCIATION
				wake_up_interruptible(
					&priv->phandle->reassoc_thread.wait_q);
#endif
			} else {
#if CFG80211_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
				if (priv->mrvl_rssi_low) {
					cfg80211_cqm_rssi_notify(
						priv->netdev,
						NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
						0,
#endif
						GFP_KERNEL);
				}
#endif
			}
		}
#endif
		break;
	case MLAN_EVENT_ID_FW_BG_SCAN_STOPPED:
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
		if (IS_STA_CFG80211(cfg80211_wext)) {
			if (priv->sched_scanning)
				woal_bgscan_stop_event(priv);
		}
#endif
#endif
		break;
	case MLAN_EVENT_ID_DRV_BGSCAN_RESULT:
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
		if (IS_STA_CFG80211(cfg80211_wext)) {
			if (priv->sched_scanning &&
			    !priv->phandle->cfg80211_suspend) {
				woal_inform_bss_from_scan_result(priv, NULL,
								 MOAL_NO_WAIT);
				PRINTM(MMSG,
				       "wlan: Report sched_scan result\n");
				woal_report_sched_scan_result(priv);
				priv->last_event = 0;
				PRINTM(MEVENT,
				       "Reporting Sched_Scan results\n");
			}
		}
#endif
#endif
		break;
#endif /* STA_SUPPORT */

	case MLAN_EVENT_ID_FW_CHANNEL_REPORT_RDY:
		radar_detected = pmevent->event_buf[0];
		bandwidth = pmevent->event_buf[2];
#ifdef UAP_SUPPORT
		if (priv->chan_rpt_req.chanNum && priv->chan_rpt_pending) {
			radar_chan = pmevent->event_buf[1];
			if (radar_detected) {
				snprintf(event_buf, sizeof(event_buf) - 1,
					 "%s %d", CUS_EVT_RADAR_DETECTED,
					 radar_chan);
				woal_broadcast_event(priv, event_buf,
						     strlen(event_buf));
			} else {
				snprintf(event_buf, sizeof(event_buf) - 1,
					 "%s %d", CUS_EVT_CAC_FINISHED,
					 priv->chan_rpt_req.chanNum);
				woal_broadcast_event(priv, event_buf,
						     strlen(event_buf));
			}
			if (priv->bss_type == MLAN_BSS_TYPE_DFS)
				woal_chan_event(priv, WOAL_EVENT_CHAN_RPT,
						priv->chan_rpt_req.chanNum,
						radar_detected);
		}
#endif

#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		if (!IS_STA_OR_UAP_CFG80211(cfg80211_wext))
			break;
		if (priv->chan_rpt_req.chanNum && priv->chan_rpt_pending) {
			priv->chan_rpt_pending = MFALSE;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
			if (priv->radar_background) {
				PRINTM(MEVENT,
				       "%s radar found when background CAC \n",
				       radar_detected ? "" : "No");
				if (radar_detected)
					cfg80211_background_radar_event(
						priv->phandle->wiphy,
						&priv->radar_background_chan,
						GFP_ATOMIC);
				break;
			}
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			cfg_priv = woal_get_priv_with_wdev(priv->phandle);
			if (cfg_priv) {
				if (radar_detected)
					woal_update_channels_dfs_state(
						cfg_priv,
						priv->chan_rpt_req.chanNum,
						priv->chan_rpt_req.bandcfg
							.chanWidth,
						DFS_UNAVAILABLE);
				else
					woal_update_channels_dfs_state(
						cfg_priv,
						priv->chan_rpt_req.chanNum,
						priv->chan_rpt_req.bandcfg
							.chanWidth,
						DFS_AVAILABLE);
			}
#endif
			break;
		}

		if (priv->phandle->is_cac_timer_set) {
			PRINTM(MEVENT, "%s radar found when CAC \n",
			       radar_detected ? "" : "No");
			moal_stop_timer(priv->phandle,
					&priv->phandle->cac_timer);
			priv->phandle->is_cac_timer_set = MFALSE;
			if (radar_detected) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL);
#else
				cfg80211_cac_event(priv->netdev,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL);
#endif
				cfg80211_radar_event(
					priv->wdev->wiphy,
					&priv->phandle->dfs_channel,
					GFP_KERNEL);
			} else {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
				if (!priv->user_cac_period_msec) {
					// host and device timer isn't sync,add
					// delay to avoid kernel warning
					// WARN_ON(!time_after_eq(jiffies,
					// timeout)); mdelay(100); Using
					// optimized delay
					timeout =
						(priv->wdev->cac_start_time +
						 msecs_to_jiffies(
							 priv->wdev
								 ->cac_time_ms));
					if (!time_after_eq(jiffies, timeout)) {
						/* Exact time to make host and
						 * device timer in sync */
						wait_time = timeout - jiffies;
						wait_time_ms =
							jiffies_to_msecs(
								wait_time) +
							3;
						PRINTM(MEVENT,
						       "Waiting for %ld ms for syncing\n",
						       wait_time_ms);
						mdelay(wait_time_ms);
					}
				}
#endif

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_FINISHED,
						   GFP_KERNEL);
#else
				cfg80211_cac_event(priv->netdev,
						   NL80211_RADAR_CAC_FINISHED,
						   GFP_KERNEL);
#endif
			}
			memset(&priv->phandle->dfs_channel, 0,
			       sizeof(struct cfg80211_chan_def));
			priv->phandle->cac_bss_index = 0xff;
		}
#endif /* CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0) */
#endif /* UAP_CFG80211 */
		break;
	case MLAN_EVENT_ID_FW_RADAR_DETECTED:
		radar_chan = pmevent->event_buf[0];
		bandwidth = pmevent->event_buf[1];
		snprintf(event_buf, sizeof(event_buf) - 1, "%s %d",
			 CUS_EVT_RADAR_DETECTED, radar_chan);
		woal_broadcast_event(priv, event_buf, strlen(event_buf));
		PRINTM(MEVENT, "Radar detected on channel %d\n", radar_chan);

#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		if (!IS_STA_OR_UAP_CFG80211(cfg80211_wext))
			break;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
		if (priv->radar_background) {
			cfg80211_background_radar_event(
				priv->phandle->wiphy,
				&priv->radar_background_chan, GFP_ATOMIC);
			break;
		}
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		cfg_priv = woal_get_priv_with_wdev(priv->phandle);
		if (cfg_priv)
			woal_update_channels_dfs_state(cfg_priv, radar_chan,
						       bandwidth,
						       DFS_UNAVAILABLE);
#endif
#ifdef UAP_SUPPORT
		if ((priv->target_chan && priv->bss_started &&
		     (priv->target_chan != radar_chan)) ||
		    priv->backup_chan) {
			PRINTM(MEVENT, "Move to target or backup chan %d %d\n",
			       priv->target_chan, priv->backup_chan);
			woal_move_to_next_channel(priv);
			priv->target_chan = 0;
			break;
		}
		if (priv->bss_type == MLAN_BSS_TYPE_DFS) {
			woal_chan_event(priv, WOAL_EVENT_RADAR,
					priv->chan_rpt_req.chanNum, MTRUE);
			break;
		}
#endif
		if (priv->phandle->is_cac_timer_set) {
			if (priv->bss_index == priv->phandle->cac_bss_index) {
				PRINTM(MEVENT, "radar detected during CAC \n");
				woal_cancel_timer(&priv->phandle->cac_timer);
				priv->phandle->is_cac_timer_set = MFALSE;
				/* downstream: cancel the unfinished CAC in
				 * Firmware*/
				woal_11h_cancel_chan_report_ioctl(priv,
								  MOAL_NO_WAIT);
				/* upstream: inform cfg80211 */
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL);
#else
				cfg80211_cac_event(priv->netdev,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL);
#endif
				cfg80211_radar_event(
					priv->wdev->wiphy,
					&priv->phandle->dfs_channel,
					GFP_KERNEL);

				memset(&priv->phandle->dfs_channel, 0,
				       sizeof(priv->phandle->dfs_channel));
				priv->phandle->cac_bss_index = 0xff;
			} else {
				PRINTM(MERROR,
				       " Radar event for incorrect inferface \n");
			}
		} else {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (moal_extflg_isset(priv->phandle, EXT_DFS_OFFLOAD))
				woal_cfg80211_dfs_vendor_event(
					priv, event_dfs_radar_detected,
					&priv->chan);
			else {
#endif
				if (priv->uap_host_based && priv->bss_started)
					cfg80211_radar_event(priv->wdev->wiphy,
							     &priv->chan,
							     GFP_KERNEL);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			}
#endif
		}
#endif /* CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0 */
#endif /* UAP_CFG80211 */
		break;
	case MLAN_EVENT_ID_FW_CHANNEL_SWITCH_ANN:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext))
			woal_send_iwevcustom_event(priv,
						   CUS_EVT_CHANNEL_SWITCH_ANN);
#endif
		woal_broadcast_event(priv, CUS_EVT_CHANNEL_SWITCH_ANN,
				     strlen(CUS_EVT_CHANNEL_SWITCH_ANN));
		break;

	case MLAN_EVENT_ID_FW_CHAN_SWITCH_COMPLETE:
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
		pchan_info = (chan_band_info *)pmevent->event_buf;
#ifdef UAP_SUPPORT
		if (priv->bss_role == MLAN_BSS_ROLE_UAP) {
			if (priv->uap_tx_blocked) {
				if (!netif_carrier_ok(priv->netdev))
					netif_carrier_on(priv->netdev);
				woal_start_queue(priv->netdev);
				priv->uap_tx_blocked = MFALSE;
			}
			priv->phandle->chsw_wait_q_woken = MTRUE;
			wake_up_interruptible(&priv->phandle->chsw_wait_q);
		}
#endif

		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext)) {
			PRINTM(MMSG,
			       "CSA/ECSA: Switch to new channel %d complete!\n",
			       pchan_info->channel);
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
			if (priv->csa_chan.chan &&
			    (pchan_info->channel ==
			     priv->csa_chan.chan->hw_value)) {
				moal_memcpy_ext(
					priv->phandle, &priv->chan,
					&priv->csa_chan,
					sizeof(struct cfg80211_chan_def),
					sizeof(struct cfg80211_chan_def));
			}
#endif
#endif
			if (priv->channel == pchan_info->channel)
				break;
			priv->channel = pchan_info->channel;

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			if (MFALSE
#ifdef UAP_CFG80211
			    || priv->uap_host_based
#endif
#ifdef STA_CFG80211
			    || priv->sme_current.ssid_len
#endif
			) {
				PRINTM(MEVENT,
				       "CHAN_SWITCH: 11n=%d, chan=%d, center_chan=%d, band=%d, width=%d, 2Offset=%d\n",
				       pchan_info->is_11n_enabled,
				       pchan_info->channel,
				       pchan_info->center_chan,
				       pchan_info->bandcfg.chanBand,
				       pchan_info->bandcfg.chanWidth,
				       pchan_info->bandcfg.chan2Offset);
				woal_channel_switch_event(priv, pchan_info);
			}
#endif
		}
#endif
		break;
	case MLAN_EVENT_ID_FW_STOP_TX:
		woal_stop_queue(priv->netdev);
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
		break;
	case MLAN_EVENT_ID_FW_START_TX:
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_wake_queue(priv->netdev);
		break;
	case MLAN_EVENT_ID_FW_HS_WAKEUP:
		/* simulate HSCFG_CANCEL command */
		woal_cancel_hs(priv, MOAL_NO_WAIT);
#ifdef STA_SUPPORT
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_STA);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext) && pmpriv)
			woal_send_iwevcustom_event(pmpriv, CUS_EVT_HS_WAKEUP);
#endif /* STA_WEXT */
		if (pmpriv)
			woal_broadcast_event(pmpriv, CUS_EVT_HS_WAKEUP,
					     strlen(CUS_EVT_HS_WAKEUP));
#endif /*STA_SUPPORT */
#ifdef UAP_SUPPORT
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_UAP);
		if (pmpriv) {
			pmevent->event_id = UAP_EVENT_ID_HS_WAKEUP;
			woal_broadcast_event(pmpriv, (t_u8 *)&pmevent->event_id,
					     sizeof(t_u32));
		}
#endif /* UAP_SUPPORT */
		break;
	case MLAN_EVENT_ID_DRV_HS_ACTIVATED:
#ifdef STA_SUPPORT
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_STA);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext) && pmpriv)
			woal_send_iwevcustom_event(pmpriv,
						   CUS_EVT_HS_ACTIVATED);
#endif /* STA_WEXT */
		if (pmpriv)
			woal_broadcast_event(pmpriv, CUS_EVT_HS_ACTIVATED,
					     strlen(CUS_EVT_HS_ACTIVATED));
#endif /* STA_SUPPORT */
#if defined(UAP_SUPPORT)
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_UAP);
		if (pmpriv) {
			pmevent->event_id = UAP_EVENT_ID_DRV_HS_ACTIVATED;
			woal_broadcast_event(pmpriv, (t_u8 *)&pmevent->event_id,
					     sizeof(t_u32));
		}
#endif
		memset(&pm_info, 0, sizeof(mlan_ds_ps_info));
		if (priv->phandle->suspend_fail == MFALSE) {
			woal_get_pm_info(priv, &pm_info);
			if (pm_info.is_suspend_allowed == MTRUE) {
				priv->phandle->hs_activated = MTRUE;
#ifdef MMC_PM_FUNC_SUSPENDED
				woal_wlan_is_suspended(priv->phandle);
#endif
			}
			priv->phandle->hs_activate_wait_q_woken = MTRUE;
			wake_up(&priv->phandle->hs_activate_wait_q);
		}
		break;
	case MLAN_EVENT_ID_DRV_HS_DEACTIVATED:
#ifdef STA_SUPPORT
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_STA);
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext) && pmpriv)
			woal_send_iwevcustom_event(pmpriv,
						   CUS_EVT_HS_DEACTIVATED);
#endif /* STA_WEXT */
		if (pmpriv)
			woal_broadcast_event(pmpriv, CUS_EVT_HS_DEACTIVATED,
					     strlen(CUS_EVT_HS_DEACTIVATED));
#endif /* STA_SUPPORT */
#if defined(UAP_SUPPORT)
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_UAP);
		if (pmpriv) {
			pmevent->event_id = UAP_EVENT_ID_DRV_HS_DEACTIVATED;
			woal_broadcast_event(pmpriv, (t_u8 *)&pmevent->event_id,
					     sizeof(t_u32));
		}
#endif
		priv->phandle->hs_activated = MFALSE;
		break;
#ifdef UAP_SUPPORT
	case MLAN_EVENT_ID_UAP_FW_BSS_START:
		woal_hist_data_reset(priv);
		priv->bss_started = MTRUE;
		priv->skip_cac = MFALSE;
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_start_queue(priv->netdev);
		moal_memcpy_ext(priv->phandle, priv->current_addr,
				pmevent->event_buf + 6, ETH_ALEN, ETH_ALEN);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
		eth_hw_addr_set(priv->netdev, priv->current_addr);
#else
		moal_memcpy_ext(priv->phandle, priv->netdev->dev_addr,
				priv->current_addr, ETH_ALEN, ETH_ALEN);
#endif
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
#ifdef STA_SUPPORT
#ifdef STA_CFG80211
		pmpriv = woal_get_priv((moal_handle *)pmoal, MLAN_BSS_ROLE_STA);
		if (IS_STA_CFG80211(cfg80211_wext) && pmpriv)
			woal_set_scan_time(pmpriv, ACTIVE_SCAN_CHAN_TIME,
					   PASSIVE_SCAN_CHAN_TIME,
					   MIN_SPECIFIC_SCAN_CHAN_TIME);
#endif
#endif
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		if (priv->chan_under_nop) {
			PRINTM(MMSG,
			       "Channel Under Nop: notify cfg80211 new channel=%d\n",
			       priv->channel);
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || IMX_ANDROID_13)
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan, 0);
#else
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan);
#endif
			priv->chan_under_nop = MFALSE;
		}
#endif
#endif
		break;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
	case MLAN_EVENT_ID_DRV_UAP_CHAN_INFO:
#ifdef UAP_CFG80211
		if (IS_UAP_CFG80211(cfg80211_wext)) {
			pchan_info = (chan_band_info *)pmevent->event_buf;
			PRINTM(MEVENT,
			       "UAP: 11n=%d, chan=%d, center_chan=%d, band=%d, width=%d, 2Offset=%d\n",
			       pchan_info->is_11n_enabled, pchan_info->channel,
			       pchan_info->center_chan,
			       pchan_info->bandcfg.chanBand,
			       pchan_info->bandcfg.chanWidth,
			       pchan_info->bandcfg.chan2Offset);
			if (priv->uap_host_based &&
			    (priv->channel != pchan_info->channel))
				woal_channel_switch_event(priv, pchan_info);
		}
#endif
		break;
#endif
	case MLAN_EVENT_ID_UAP_FW_BSS_ACTIVE:
		priv->media_connected = MTRUE;
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		woal_wake_queue(priv->netdev);
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
		moal_connection_status_check_pmqos(pmoal);
		break;
	case MLAN_EVENT_ID_UAP_FW_BSS_IDLE:
		priv->media_connected = MFALSE;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
		moal_connection_status_check_pmqos(pmoal);
		break;
	case MLAN_EVENT_ID_UAP_FW_MIC_COUNTERMEASURES: {
		t_u16 status = 0;
		status = *(t_u16 *)(pmevent->event_buf + 4);
		if (status) {
			priv->media_connected = MFALSE;
			woal_stop_queue(priv->netdev);
			if (netif_carrier_ok(priv->netdev))
				netif_carrier_off(priv->netdev);
		} else {
			priv->media_connected = MTRUE;
			if (!netif_carrier_ok(priv->netdev))
				netif_carrier_on(priv->netdev);
			woal_wake_queue(priv->netdev);
		}
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
	} break;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
	case MLAN_EVENT_ID_FW_REMAIN_ON_CHAN_EXPIRED:
		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext)) {
			PRINTM(MEVENT,
			       "FW_REMAIN_ON_CHANNEL_EXPIRED cookie = %#llx\n",
			       priv->phandle->cookie);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			if (priv->host_mlme &&
			    (priv->auth_flag & HOST_MLME_AUTH_PENDING)) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
				if (priv->assoc_bss) {
					PRINTM(MEVENT,
					       "wlan: HostMlme auth timeout\n");
					cfg80211_auth_timeout(
						priv->netdev,
						priv->assoc_bss->bssid);
				}
#endif
				priv->auth_flag = 0;
				priv->host_mlme = MFALSE;
				priv->auth_alg = 0xFFFF;
			}
#endif
			priv->phandle->remain_on_channel = MFALSE;
			if (priv->phandle->cookie &&
			    !priv->phandle->is_remain_timer_set) {
				cfg80211_remain_on_channel_expired(
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
					priv->netdev,
#else
					priv->wdev,
#endif
					priv->phandle->cookie,
					&priv->phandle->chan,
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
					priv->phandle->channel_type,
#endif
					GFP_ATOMIC);
				priv->phandle->cookie = 0;
			}
		}
		break;
#endif
#endif
	case MLAN_EVENT_ID_UAP_FW_STA_CONNECT:
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext)) {
			struct station_info *sinfo = NULL;
			t_u8 addr[ETH_ALEN];
			sinfo = kzalloc(sizeof(struct station_info),
					GFP_ATOMIC);
			if (sinfo) {
				sinfo->filled = 0;
				sinfo->generation = 0;
				/* copy the station mac address */
				memset(addr, 0xFF, ETH_ALEN);
				moal_memcpy_ext(priv->phandle, addr,
						pmevent->event_buf, ETH_ALEN,
						ETH_ALEN);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
				if (pmevent->event_len > ETH_ALEN) {
#if CFG80211_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
					/* set station info filled flag */
					sinfo->filled |=
						STATION_INFO_ASSOC_REQ_IES;
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
					sinfo->pertid = NULL;
#endif
					/* get the assoc request ies and length
					 */
					sinfo->assoc_req_ies =
						(const t_u8
							 *)(pmevent->event_buf +
							    ETH_ALEN);
					sinfo->assoc_req_ies_len =
						pmevent->event_len - ETH_ALEN;
				}
#endif /* KERNEL_VERSION */
				if (priv->netdev && priv->wdev)
					cfg80211_new_sta(priv->netdev,
							 (t_u8 *)addr, sinfo,
							 GFP_KERNEL);
				kfree(sinfo);
			}
		}
#endif /* UAP_CFG80211 */
		memmove((pmevent->event_buf + strlen(CUS_EVT_STA_CONNECTED) +
			 1),
			pmevent->event_buf, pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				(t_u8 *)CUS_EVT_STA_CONNECTED,
				strlen(CUS_EVT_STA_CONNECTED),
				strlen(CUS_EVT_STA_CONNECTED));
		pmevent->event_buf[strlen(CUS_EVT_STA_CONNECTED)] = 0;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len +
					     strlen(CUS_EVT_STA_CONNECTED));
#ifdef UAP_WEXT
		if (IS_UAP_WEXT(cfg80211_wext)) {
			memset(&wrqu, 0, sizeof(union iwreq_data));
			wrqu.data.pointer = (t_u8 __user *)pmevent->event_buf;
			if ((pmevent->event_len +
			     strlen(CUS_EVT_STA_CONNECTED) + 1) > IW_CUSTOM_MAX)
				wrqu.data.length =
					ETH_ALEN +
					strlen(CUS_EVT_STA_CONNECTED) + 1;
			else
				wrqu.data.length =
					pmevent->event_len +
					strlen(CUS_EVT_STA_CONNECTED) + 1;
			wireless_send_event(priv->netdev, IWEVCUSTOM, &wrqu,
					    pmevent->event_buf);
		}
#endif /* UAP_WEXT */
		break;
	case MLAN_EVENT_ID_UAP_FW_STA_DISCONNECT:
#ifdef UAP_CFG80211
		if (IS_UAP_CFG80211(cfg80211_wext)) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			/**Forward Deauth, Auth and disassoc frame to Host*/
			if (moal_extflg_isset(priv->phandle, EXT_HOST_MLME)) {
				t_u16 reason_code = woal_le16_to_cpu(
					*(t_u16 *)pmevent->event_buf);
				PRINTM(MCMND, "deauth reason code =0x%x\n",
				       reason_code);
				/** BIT 14 indicate deauth is initiated by FW */
				if (reason_code & MBIT(14))
					woal_host_mlme_disconnect(
						priv, 0,
						pmevent->event_buf + 2);
			} else
#endif
				if (priv->netdev && priv->wdev)
				cfg80211_del_sta(priv->netdev,
						 pmevent->event_buf + 2,
						 GFP_KERNEL);

#endif /* KERNEL_VERSION */
		}
#endif /* UAP_CFG80211 */
		memmove((pmevent->event_buf + strlen(CUS_EVT_STA_DISCONNECTED) +
			 1),
			pmevent->event_buf, pmevent->event_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf,
				(t_u8 *)CUS_EVT_STA_DISCONNECTED,
				strlen(CUS_EVT_STA_DISCONNECTED),
				strlen(CUS_EVT_STA_DISCONNECTED));
		pmevent->event_buf[strlen(CUS_EVT_STA_DISCONNECTED)] = 0;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len +
					     strlen(CUS_EVT_STA_DISCONNECTED));

#ifdef UAP_WEXT
		if (IS_UAP_WEXT(cfg80211_wext)) {
			memset(&wrqu, 0, sizeof(union iwreq_data));
			wrqu.data.pointer = (t_u8 __user *)pmevent->event_buf;
			wrqu.data.length = pmevent->event_len +
					   strlen(CUS_EVT_STA_DISCONNECTED) + 1;
			wireless_send_event(priv->netdev, IWEVCUSTOM, &wrqu,
					    pmevent->event_buf);
		}
#endif /* UAP_WEXT */
		break;
	case MLAN_EVENT_ID_DRV_MGMT_FRAME:
#ifdef UAP_WEXT
		if (IS_UAP_WEXT(cfg80211_wext)) {
			woal_broadcast_event(priv, pmevent->event_buf,
					     pmevent->event_len);
		}
#endif /* UAP_WEXT */

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext)) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
			if (priv->netdev &&
			    priv->netdev->ieee80211_ptr->wiphy->mgmt_stypes &&
			    priv->mgmt_subtype_mask) {
				/* frmctl + durationid + addr1 + addr2 + addr3 +
				 * seqctl */
#define PACKET_ADDR4_POS (2 + 2 + 6 + 6 + 6 + 2)
				t_u8 *pkt;
				int freq =
					priv->phandle->remain_on_channel ?
						priv->phandle->chan.center_freq :
						woal_get_active_intf_freq(priv);
				if (!freq) {
					if (!priv->phandle->chan.center_freq) {
						PRINTM(MINFO,
						       "Skip to report mgmt packet to cfg80211\n");
						break;
					}
					freq = priv->phandle->chan.center_freq;
				}

				pkt = ((t_u8 *)pmevent->event_buf +
				       sizeof(pmevent->event_id));

				/* move addr4 */
				memmove(pkt + PACKET_ADDR4_POS,
					pkt + PACKET_ADDR4_POS + ETH_ALEN,
					pmevent->event_len -
						sizeof(pmevent->event_id) -
						PACKET_ADDR4_POS - ETH_ALEN);
#ifdef WIFI_DIRECT_SUPPORT
				if (ieee80211_is_action(
					    ((struct ieee80211_mgmt *)pkt)
						    ->frame_control))
					woal_cfg80211_display_p2p_actframe(
						pkt,
						pmevent->event_len -
							sizeof(pmevent->event_id) -
							MLAN_MAC_ADDR_LENGTH,
						ieee80211_get_channel(
							priv->wdev->wiphy,
							freq),
						MFALSE);
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
				/**Forward Deauth, Auth and disassoc frame to
				 * Host*/
				if (priv->host_mlme &&
				    (GET_BSS_ROLE(priv) != MLAN_BSS_ROLE_UAP) &&
				    (ieee80211_is_deauth(
					     ((struct ieee80211_mgmt *)pkt)
						     ->frame_control) ||
				     ieee80211_is_auth(
					     ((struct ieee80211_mgmt *)pkt)
						     ->frame_control) ||
				     ieee80211_is_disassoc(
					     ((struct ieee80211_mgmt *)pkt)
						     ->frame_control))) {
					if (ieee80211_is_auth(
						    ((struct ieee80211_mgmt *)
							     pkt)
							    ->frame_control)) {
						PRINTM(MEVENT,
						       "HostMlme %s: Received auth frame type = 0x%x\n",
						       priv->netdev->name,
						       priv->auth_alg);

						if (priv->auth_flag &
						    HOST_MLME_AUTH_PENDING) {
							if (priv->auth_alg !=
							    WLAN_AUTH_SAE) {
								priv->auth_flag &=
									~HOST_MLME_AUTH_PENDING;
								priv->auth_flag |=
									HOST_MLME_AUTH_DONE;
								priv->phandle
									->host_mlme_priv =
									priv;
								queue_work(
									priv->phandle
										->evt_workqueue,
									&priv->phandle
										 ->host_mlme_work);
							}
						} else {
							PRINTM(MERROR,
							       "HostMlme %s: Drop auth frame, auth_flag=0x%x auth_alg=0x%x\n",
							       priv->netdev
								       ->name,
							       priv->auth_flag,
							       priv->auth_alg);
							break;
						}
					} else {
						PRINTM(MEVENT,
						       "HostMlme %s: Receive deauth/disassociate\n",
						       priv->netdev->name);
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || IMX_ANDROID_13)
						if (!priv->wdev->connected) {
#else
						if (!priv->wdev->current_bss) {
#endif
							PRINTM(MEVENT,
							       "HostMlme: Drop deauth/disassociate, current_bss = null\n");
							break;
						}
						priv->cfg_disconnect = MTRUE;
						woal_mgmt_frame_register(
							priv,
							IEEE80211_STYPE_DEAUTH,
							MFALSE);
						woal_mgmt_frame_register(
							priv,
							IEEE80211_STYPE_DISASSOC,
							MFALSE);
						woal_send_disconnect_to_system(
							priv,
							DEF_DEAUTH_REASON_CODE);
						priv->host_mlme = MFALSE;
						priv->auth_flag = 0;
						priv->auth_alg = 0xFFFF;
					}
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
					woal_rx_mgmt_pkt_event(
						priv, pkt,
						pmevent->event_len -
							sizeof(pmevent->event_id) -
							MLAN_MAC_ADDR_LENGTH);
#else
					if (ieee80211_is_deauth(
						    ((struct ieee80211_mgmt *)
							     pkt)
							    ->frame_control))
						cfg80211_send_deauth(
							priv->netdev, pkt,
							pmevent->event_len -
								sizeof(pmevent->event_id) -
								MLAN_MAC_ADDR_LENGTH);
					else if (ieee80211_is_auth(
							 ((struct ieee80211_mgmt
								   *)pkt)
								 ->frame_control))
						cfg80211_send_rx_auth(
							priv->netdev, pkt,
							pmevent->event_len -
								sizeof(pmevent->event_id) -
								MLAN_MAC_ADDR_LENGTH);
					else if (ieee80211_is_disassoc(
							 ((struct ieee80211_mgmt
								   *)pkt)
								 ->frame_control))
						cfg80211_send_disassoc(
							priv->netdev, pkt,
							pmevent->event_len -
								sizeof(pmevent->event_id) -
								MLAN_MAC_ADDR_LENGTH);

#endif
				} else
#endif
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
					cfg80211_rx_mgmt(
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
						priv->wdev,
#else
						priv->netdev,
#endif
						freq, 0,
						((const t_u8 *)
							 pmevent->event_buf) +
							sizeof(pmevent->event_id),
						pmevent->event_len -
							sizeof(pmevent->event_id) -
							MLAN_MAC_ADDR_LENGTH
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
						,
						0
#endif
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
						,
						GFP_ATOMIC
#endif
					);
#else
				cfg80211_rx_mgmt(
					priv->netdev, freq,
					((const t_u8 *)pmevent->event_buf) +
						sizeof(pmevent->event_id),
					pmevent->event_len -
						sizeof(pmevent->event_id) -
						MLAN_MAC_ADDR_LENGTH,
					GFP_ATOMIC);
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
				woal_packet_fate_monitor(
					priv, PACKET_TYPE_RX,
					RX_PKT_FATE_SUCCESS,
					FRAME_TYPE_80211_MGMT, 0, 0,
					((t_u8 *)pmevent->event_buf) +
						sizeof(pmevent->event_id),
					pmevent->event_len -
						sizeof(pmevent->event_id) -
						MLAN_MAC_ADDR_LENGTH);
#endif
#endif
			}
#endif /* KERNEL_VERSION */
		}
#endif /* STA_CFG80211 || UAP_CFG80211 */
		break;
#endif /* UAP_SUPPORT */
	case MLAN_EVENT_ID_DRV_PASSTHRU:
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
		break;
	case MLAN_EVENT_ID_DRV_ASSOC_FAILURE_REPORT:
		PRINTM(MINFO, "Assoc result\n");

		if (priv->media_connected) {
			PRINTM(MINFO, "Assoc_Rpt: Media Connected\n");
			if (!netif_carrier_ok(priv->netdev)) {
				PRINTM(MINFO, "Assoc_Rpt: Carrier On\n");
				netif_carrier_on(priv->netdev);
			}
			PRINTM(MINFO, "Assoc_Rpt: Queue Start\n");
			woal_wake_queue(priv->netdev);
		}
		break;
	case MLAN_EVENT_ID_DRV_MEAS_REPORT:
		/* We have received measurement report, wakeup measurement wait
		 * queue */
		PRINTM(MINFO, "Measurement Report\n");
		/* Going out of CAC checking period */
		if (priv->phandle->cac_period == MTRUE) {
			priv->phandle->cac_period = MFALSE;
			if (priv->phandle->meas_wait_q_woken == MFALSE) {
				priv->phandle->meas_wait_q_woken = MTRUE;
				wake_up_interruptible(
					&priv->phandle->meas_wait_q);
			}

			/* Execute delayed BSS START command */
			if (priv->phandle->delay_bss_start == MTRUE) {
				mlan_ioctl_req *req = NULL;
				mlan_ds_bss *bss = NULL;

				/* Clear flag */
				priv->phandle->delay_bss_start = MFALSE;

				PRINTM(MMSG,
				       "Now CAC measure period end. Execute delayed BSS Start command.\n");

				req = woal_alloc_mlan_ioctl_req(
					sizeof(mlan_ds_bss));
				if (!req) {
					PRINTM(MERROR,
					       "Failed to allocate ioctl request buffer\n");
					goto done;
				}
				bss = (mlan_ds_bss *)req->pbuf;
				req->req_id = MLAN_IOCTL_BSS;
				req->action = MLAN_ACT_SET;
				bss->sub_command = MLAN_OID_BSS_START;
				moal_memcpy_ext(
					priv->phandle, &bss->param.ssid_bssid,
					&priv->phandle->delay_ssid_bssid,
					sizeof(mlan_ssid_bssid),
					sizeof(mlan_ssid_bssid));

				if (woal_request_ioctl(priv, req,
						       MOAL_NO_WAIT) !=
				    MLAN_STATUS_PENDING) {
					PRINTM(MERROR,
					       "Delayed BSS Start operation failed!\n");
					kfree(req);
				}

				PRINTM(MMSG, "BSS START Complete!\n");
			}
#ifdef UAP_SUPPORT
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (priv->uap_host_based &&
			    moal_extflg_isset(priv->phandle, EXT_DFS_OFFLOAD))
				woal_cfg80211_dfs_vendor_event(
					priv, event_dfs_cac_finished,
					&priv->chan);
#endif
#endif
#endif
		}
		break;
	case MLAN_EVENT_ID_DRV_TDLS_TEARDOWN_REQ:
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			tdls_tear_down_event *tdls_event =
				(tdls_tear_down_event *)pmevent->event_buf;
			cfg80211_tdls_oper_request(priv->netdev,
						   tdls_event->peer_mac_addr,
						   NL80211_TDLS_TEARDOWN,
						   tdls_event->reason_code,
						   GFP_KERNEL);
		}
#endif
#endif
		break;
	case MLAN_EVENT_ID_FW_TX_STATUS: {
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		unsigned long flag;
		tx_status_event *tx_status =
			(tx_status_event *)(pmevent->event_buf + 4);
		struct tx_status_info *tx_info = NULL;
		PRINTM(MINFO,
		       "Receive Tx status: tx_token=%d, pkt_type=0x%x, status=%d tx_seq_num=%d\n",
		       tx_status->tx_token_id, tx_status->packet_type,
		       tx_status->status, priv->tx_seq_num);
		spin_lock_irqsave(&priv->tx_stat_lock, flag);
		tx_info = woal_get_tx_info(priv, tx_status->tx_token_id);
		if (tx_info) {
			bool ack;
			struct sk_buff *skb = (struct sk_buff *)tx_info->tx_skb;
			list_del(&tx_info->link);
			spin_unlock_irqrestore(&priv->tx_stat_lock, flag);
			if (!tx_status->status)
				ack = true;
			else
				ack = false;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
			if (priv->phandle->remain_on_channel &&
			    tx_info->cancel_remain_on_channel) {
				remain_priv =
					priv->phandle->priv
						[priv->phandle->remain_bss_index];
				if (remain_priv) {
					woal_cfg80211_remain_on_channel_cfg(
						remain_priv, MOAL_NO_WAIT,
						MTRUE, &channel_status, NULL, 0,
						0);
					priv->phandle->remain_on_channel =
						MFALSE;
				}
			}
#endif
			PRINTM(MEVENT, "Wlan: Tx status=%d\n", ack);
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
			if (tx_info->tx_cookie) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
				cfg80211_mgmt_tx_status(priv->netdev,
							tx_info->tx_cookie,
							skb->data, skb->len,
							ack, GFP_ATOMIC);
#else
				cfg80211_mgmt_tx_status(priv->wdev,
							tx_info->tx_cookie,
							skb->data, skb->len,
							ack, GFP_ATOMIC);
#endif
#endif
			}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			woal_packet_fate_monitor(priv, PACKET_TYPE_TX,
						 ack ? TX_PKT_FATE_ACKED :
						       TX_PKT_FATE_SENT,
						 FRAME_TYPE_80211_MGMT, 0, 0,
						 skb->data, skb->len);
#endif
#endif
#endif
			dev_kfree_skb_any(skb);
			kfree(tx_info);
		} else {
			spin_unlock_irqrestore(&priv->tx_stat_lock, flag);
		}
#endif
	} break;
	case MLAN_EVENT_ID_DRV_FT_RESPONSE:
		if (priv->phandle->fw_roam_enable)
			break;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			struct cfg80211_ft_event_params ft_event;
			if (priv->ft_pre_connect)
				break;
			memset(&ft_event, 0,
			       sizeof(struct cfg80211_ft_event_params));
			PRINTM(MMSG,
			       "wlan : FT response  target AP " MACSTR "\n",
			       MAC2STR((t_u8 *)pmevent->event_buf));
			DBG_HEXDUMP(MDAT_D, "FT-event ", pmevent->event_buf,
				    pmevent->event_len);
			moal_memcpy_ext(priv->phandle, priv->target_ap_bssid,
					pmevent->event_buf, ETH_ALEN, ETH_ALEN);
			ft_event.target_ap = priv->target_ap_bssid;
			ft_event.ies = pmevent->event_buf + ETH_ALEN;
			ft_event.ies_len = pmevent->event_len - ETH_ALEN;
			/*TSPEC info is needed by RIC, However the TS operation
			 * is configured by mlanutl*/
			/*So do not add RIC temporally*/
			/*when add RIC, 1. query TS status, 2. copy tspec from
			 * addts command*/
			ft_event.ric_ies = NULL;
			ft_event.ric_ies_len = 0;

			cfg80211_ft_event(priv->netdev, &ft_event);
			priv->ft_pre_connect = MTRUE;

			if (priv->ft_roaming_triggered_by_driver ||
			    !(priv->ft_cap & MBIT(0))) {
				priv->ft_wait_condition = MTRUE;
				wake_up(&priv->ft_wait_q);
			}
		}
#endif
#endif
		break;
	case MLAN_EVENT_ID_FW_ROAM_OFFLOAD_RESULT:
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		woal_cfg80211_vendor_event(priv, event_set_key_mgmt_offload,
					   &enable, sizeof(enable));
#endif
		moal_memcpy_ext(priv->phandle, priv->cfg_bssid,
				pmevent->event_buf, ETH_ALEN, ETH_ALEN);
		tlv = (MrvlIEtypesHeader_t *)((t_u8 *)pmevent->event_buf +
					      MLAN_MAC_ADDR_LENGTH);
		tlv_buf_left = pmevent->event_len - MLAN_MAC_ADDR_LENGTH;
		while (tlv_buf_left >= sizeof(MrvlIEtypesHeader_t)) {
			tlv_type = woal_le16_to_cpu(tlv->type);
			tlv_len = woal_le16_to_cpu(tlv->len);

			if (tlv_buf_left <
			    (tlv_len + sizeof(MrvlIEtypesHeader_t))) {
				PRINTM(MERROR,
				       "Error processing firmware roam success TLVs, bytes left < TLV length\n");
				break;
			}

			switch (tlv_type) {
			case TLV_TYPE_APINFO:
				pinfo = (apinfo *)tlv;
				break;
			case TLV_TYPE_ASSOC_REQ_IE:
				req_tlv = (apinfo *)tlv;
				break;
			default:
				break;
			}
			tlv_buf_left -= tlv_len + sizeof(MrvlIEtypesHeader_t);
			tlv = (MrvlIEtypesHeader_t
				       *)((t_u8 *)tlv + tlv_len +
					  sizeof(MrvlIEtypesHeader_t));
		}
		if (!pinfo) {
			PRINTM(MERROR,
			       "ERROR:AP info in roaming event buffer is NULL\n");
			goto done;
		}
		if (req_tlv) {
			req_ie = req_tlv->rsp_ie;
			ie_len = req_tlv->header.len;
		}
		woal_inform_bss_from_scan_result(priv, NULL, MOAL_NO_WAIT);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
		roam_info =
			kzalloc(sizeof(struct cfg80211_roam_info), GFP_ATOMIC);
		if (roam_info) {
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)) || IMX_ANDROID_13)
			roam_info->links[0].bssid = priv->cfg_bssid;
#else
			roam_info->bssid = priv->cfg_bssid;
#endif
			roam_info->req_ie = req_ie;
			roam_info->req_ie_len = ie_len;
			roam_info->resp_ie = pinfo->rsp_ie;
			roam_info->resp_ie_len = pinfo->header.len;
			cfg80211_roamed(priv->netdev, roam_info, GFP_KERNEL);
			kfree(roam_info);
		}
#else
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
		cfg80211_roamed(priv->netdev, NULL, priv->cfg_bssid, req_ie,
				ie_len, pinfo->rsp_ie, pinfo->header.len,
				GFP_KERNEL);
#else
		cfg80211_roamed(priv->netdev, priv->cfg_bssid, req_ie, ie_len,
				pinfo->rsp_ie, pinfo->header.len, GFP_KERNEL);
#endif
#endif

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		woal_roam_ap_info(priv, pmevent->event_buf, pmevent->event_len);
#endif
#endif
		PRINTM(MMSG, "FW Roamed to bssid " MACSTR " successfully\n",
		       MAC2STR(pmevent->event_buf));
		break;
	case MLAN_EVENT_ID_CSI:
		DBG_HEXDUMP(MEVT_D, "CSI dump", pmevent->event_buf,
			    pmevent->event_len);
#ifdef STA_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		if (priv->csi_enable)
			woal_cfg80211_event_csi_dump(priv, pmevent->event_buf,
						     pmevent->event_len);
#endif
#endif
		/* Send Netlink event */
		custom_len = strlen(CUS_EVT_CSI) + sizeof(priv->csi_seq);
		csi_len = pmevent->event_len;
		memmove(pmevent->event_buf + custom_len, pmevent->event_buf,
			csi_len);
		moal_memcpy_ext(priv->phandle, pmevent->event_buf, CUS_EVT_CSI,
				strlen(CUS_EVT_CSI), strlen(CUS_EVT_CSI));
		moal_memcpy_ext(priv->phandle,
				pmevent->event_buf + strlen(CUS_EVT_CSI),
				(t_u8 *)(&(priv->csi_seq)),
				sizeof(priv->csi_seq), sizeof(priv->csi_seq));
		woal_broadcast_event(priv, pmevent->event_buf,
				     custom_len + csi_len);
		priv->csi_seq++;
		break;
	default:
		break;
	}
done:
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prints the debug message in mlan
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param level    debug level
 *  @param pformat  point to string format buf
 *
 *  @return         N/A
 */
__attribute__((format(printf, 3, 4))) t_void
moal_print(t_void *pmoal, t_u32 level, char *pformat, IN...)
{
#ifdef DEBUG_LEVEL1
	va_list args;

	if (level & MHEX_DUMP) {
		t_u8 *buf = NULL;
		int len = 0;

		va_start(args, pformat);
		buf = (t_u8 *)va_arg(args, t_u8 *);
		len = (int)va_arg(args, int);
		va_end(args);

#ifdef DEBUG_LEVEL2
		if (level & MINFO)
			HEXDUMP((char *)pformat, buf, len);
		else
#endif /* DEBUG_LEVEL2 */
		{
			if (level & MERROR)
				DBG_HEXDUMP(MERROR, (char *)pformat, buf, len);
			if (level & MCMD_D)
				DBG_HEXDUMP(MCMD_D, (char *)pformat, buf, len);
			if (level & MDAT_D)
				DBG_HEXDUMP(MDAT_D, (char *)pformat, buf, len);
			if (level & MIF_D)
				DBG_HEXDUMP(MIF_D, (char *)pformat, buf, len);
			if (level & MFW_D)
				DBG_HEXDUMP(MFW_D, (char *)pformat, buf, len);
			if (level & MEVT_D)
				DBG_HEXDUMP(MEVT_D, (char *)pformat, buf, len);
		}
	} else {
		if (drvdbg & level) {
			va_start(args, pformat);
			vprintk(pformat, args);
			va_end(args);
		}
	}
#endif /* DEBUG_LEVEL1 */
}

/**
 *  @brief This function prints the network interface name
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param bss_index    BSS index
 *  @param level        debug level
 *
 *  @return            N/A
 */
t_void moal_print_netintf(t_void *pmoal, t_u32 bss_index, t_u32 level)
{
#ifdef DEBUG_LEVEL1
	moal_handle *phandle = (moal_handle *)pmoal;

	if (phandle) {
		if ((bss_index < MLAN_MAX_BSS_NUM) &&
		    phandle->priv[bss_index] &&
		    phandle->priv[bss_index]->netdev) {
			if (drvdbg & level)
				printk("%s: ",
				       phandle->priv[bss_index]->netdev->name);
		}
	}
#endif /* DEBUG_LEVEL1 */
}

/**
 *  @brief This function asserts the existence of the passed argument
 *
 *  @param pmoal     A pointer to moal_private structure
 *  @param cond             Condition to check
 *
 *  @return                 N/A
 */
t_void moal_assert(t_void *pmoal, t_u32 cond)
{
	if (!cond) {
		panic("Assert failed: Panic!");
	}
}

/**
 *  @brief This function save the histogram data
 *
 *  @param pmoal     A pointer to moal_private structure
 *  @param bss_index        BSS index
 *  @param rx_rate          rx rate index
 *  @param snr              snr
 *  @param nflr             noise floor
 *  @param antenna          antenna
 *
 *  @return                 N/A
 */
t_void moal_hist_data_add(t_void *pmoal, t_u32 bss_index, t_u16 rx_rate,
			  t_s8 snr, t_s8 nflr, t_u8 antenna)
{
	moal_private *priv = NULL;
	priv = woal_bss_index_to_priv(pmoal, bss_index);
	if (priv && antenna >= priv->phandle->card_info->histogram_table_num)
		antenna = 0;
	if (priv && priv->hist_data[antenna])
		woal_hist_data_add(priv, rx_rate, snr, nflr, antenna);
}

/**
 *  @brief This function update the peer signal
 *
 *  @param pmoal     A pointer to moal_private structure
 *  @param bss_index        BSS index
 *  @param peer_addr        peer address
 *  @param snr              snr
 *  @param nflr             noise floor
 *
 *  @return                 N/A
 */
t_void moal_updata_peer_signal(t_void *pmoal, t_u32 bss_index, t_u8 *peer_addr,
			       t_s8 snr, t_s8 nflr)
{
	moal_private *priv = NULL;
	struct tdls_peer *peer = NULL;
	unsigned long flags;
	priv = woal_bss_index_to_priv(pmoal, bss_index);
	if (priv && priv->enable_auto_tdls) {
		spin_lock_irqsave(&priv->tdls_lock, flags);
		list_for_each_entry (peer, &priv->tdls_list, link) {
			if (!memcmp(peer->peer_addr, peer_addr, ETH_ALEN)) {
				peer->rssi = nflr - snr;
				peer->rssi_jiffies = jiffies;
				break;
			}
		}
		spin_unlock_irqrestore(&priv->tdls_lock, flags);
	}
}

/**
 *  @brief Performs division of 64-bit num with base
 *  @brief do_div does two things
 *  @brief 1. modifies the 64-bit num in place with
 *  @brief the quotient, i.e., num becomes quotient
 *  @brief 2. do_div() returns the 32-bit reminder
 *
 *  @param num   dividend
 *  @param base  divisor
 *  @return      returns 64-bit quotient
 */
t_u64 moal_do_div(t_u64 num, t_u32 base)
{
	t_u64 val = num;
	do_div(val, base);
	return val;
}

#if defined(DRV_EMBEDDED_AUTHENTICATOR) || defined(DRV_EMBEDDED_SUPPLICANT)
/**
 *  @brief Performs wait event
 *
 *  @param pmoal   t_void
 *  @param bss_index      index of priv
 *  @return      MLAN_STATUS_SUCCESS
 */
mlan_status moal_wait_hostcmd_complete(t_void *pmoal, t_u32 bss_index)
{
	mlan_status status = MLAN_STATUS_SUCCESS;
	moal_handle *handle = (moal_handle *)pmoal;
	moal_private *priv = woal_bss_index_to_priv(handle, bss_index);
	long time_left = 0;

	ENTER();

	if (!priv) {
		PRINTM(MERROR, "moal_wait_event: priv is null!\n");
		goto done;
	}

	priv->hostcmd_wait_condition = MFALSE;
	time_left = wait_event_timeout(priv->hostcmd_wait_q,
				       priv->hostcmd_wait_condition,
				       MOAL_IOCTL_TIMEOUT);

	if (!time_left) {
		PRINTM(MERROR, "moal_wait_event: wait timeout ");
		status = MLAN_STATUS_FAILURE;
	}

done:
	LEAVE();
	return status;
}

/**
 *  @brief wake up esa wait_q
 *
 *  @param pmoal   t_void
 *  @param bss_index      index of priv
 *  @return      MLAN_STATUS_SUCCESS
 */
mlan_status moal_notify_hostcmd_complete(t_void *pmoal, t_u32 bss_index)
{
	mlan_status status = MLAN_STATUS_SUCCESS;
	moal_handle *handle = (moal_handle *)pmoal;
	moal_private *priv = woal_bss_index_to_priv(handle, bss_index);

	ENTER();

	priv->hostcmd_wait_condition = MTRUE;
	wake_up(&priv->hostcmd_wait_q);

	LEAVE();
	return status;
}
#endif
