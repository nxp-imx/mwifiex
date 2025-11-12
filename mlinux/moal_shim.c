/** @file moal_shim.c
 *
 * @brief This file contains the callback functions registered to MLAN
 *
 *
 * Copyright 2008-2025 NXP
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
#include "moal_cfg80211.h"
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
#ifdef XDP_SUPPORT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
#include <linux/filter.h>
#endif
#endif
#endif
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
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

#define NXP_ETH_P_WAPI 0x88B4

#include <linux/crc32.h>

#if defined(UAP_SUPPORT) && defined(XDP_SUPPORT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
static struct sk_buff *woal_process_xdp(moal_private *priv,
					struct net_device *ndev,
					pmlan_buffer pmbuf);
#endif
#endif
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

	if (flag & MLAN_MEM_FLAG_DIRTY)
		*ppbuf = kmalloc(size, mem_flag);
	else
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
	gfp_t flag;

	*pbuf_pa = 0;

	if (!card)
		return MLAN_STATUS_FAILURE;

	flag = in_atomic()     ? GFP_ATOMIC :
	       irqs_disabled() ? GFP_ATOMIC :
				 GFP_KERNEL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	*ppbuf = (t_u8 *)dma_alloc_coherent(&card->dev->dev, size,
					    (dma_addr_t *)&dma, flag);
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
#ifdef PCIEAW693
	if (IS_PCIEAW693(handle->card_type) &&
	    (handle->card_rev == CHIP_AW693_REV_A0))
		dma |= 0x100000000;
#endif
	*pbuf_pa = (t_u64)dma;
	atomic_inc(&handle->malloc_cons_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Alloc a non-coherent block of memory
 *
 *  @param pmoal Pointer to the MOAL context
 *  @param size         The size of the buffer to be allocated
 *  @param ppbuf        Pointer to a buffer location to store memory allocated
 *  @param pbuf_pa      Pointer to a buffer location to store physical address
 * of above memory
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_malloc_cached(t_void *pmoal, t_u32 size, t_u8 **ppbuf,
			       t_pu64 pbuf_pa)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = (pcie_service_card *)handle->card;
	dma_addr_t dma;
	gfp_t flag;

	*pbuf_pa = 0;

	if (unlikely(!card))
		return MLAN_STATUS_FAILURE;

	flag = in_atomic()     ? GFP_ATOMIC :
	       irqs_disabled() ? GFP_ATOMIC :
				 GFP_KERNEL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	*ppbuf = dma_alloc_noncoherent(&card->dev->dev, size, &dma,
				       DMA_BIDIRECTIONAL, flag);
#else
	*ppbuf = dma_alloc_attrs(&card->dev->dev, size, &dma, flag,
				 DMA_ATTR_NON_CONSISTENT);
#endif

	if (unlikely(*ppbuf == NULL)) {
		PRINTM(MERROR,
		       "%s: allocate noncoherent memory (%d bytes) failed!\n",
		       __func__, (int)size);
		return MLAN_STATUS_FAILURE;
	}

	*pbuf_pa = (t_u64)dma;
	atomic_inc(&handle->malloc_cons_count);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Free a non-coherent block of memory
 *
 *  @param pmoal        Pointer to the MOAL context
 *  @param size         The size of the buffer to be freed
 *  @param pbuf         Pointer to the buffer to be freed
 *  @param buf_pa       Physical address of the buffer to be freed
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_mfree_cached(t_void *pmoal, t_u32 size, t_u8 *pbuf,
			      t_u64 buf_pa)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = handle->card;

	if (unlikely(!pbuf || !card))
		return MLAN_STATUS_FAILURE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	dma_free_noncoherent(&card->dev->dev, size, pbuf, buf_pa,
			     DMA_BIDIRECTIONAL);
#else
	dma_free_attrs(&card->dev->dev, size, pbuf, buf_pa,
		       DMA_ATTR_NON_CONSISTENT);
#endif

	atomic_dec(&handle->malloc_cons_count);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Sync DMA buffer for CPU access
 *
 *  @param pmoal        Pointer to the MOAL context
 *  @param size         Size of the buffer to sync
 *  @param buf_pa       Physical address of the buffer
 *  @param direction    DMA sync direction
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_dma_sync_to_cpu(t_void *pmoal, t_u32 size, t_u64 buf_pa,
				 moal_dma_sync_direction_t direction)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = handle->card;

	if (unlikely(!card))
		return MLAN_STATUS_FAILURE;

	size = (size + card->cache_alignment_mask) &
	       ~card->cache_alignment_mask;

	// make sure we have one-to-one mapping to Linux`s enum
	// dma_data_direction
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_BIDIR != (int)DMA_BIDIRECTIONAL);
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_TO_DEVICE != (int)DMA_TO_DEVICE);
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_FROM_DEVICE != (int)DMA_FROM_DEVICE);

	dma_sync_single_range_for_cpu(&card->dev->dev, buf_pa, 0, size,
				      (enum dma_data_direction)direction);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Sync DMA buffer for device access
 *
 *  @param pmoal        Pointer to the MOAL context
 *  @param size         Size of the buffer to sync
 *  @param buf_pa       Physical address of the buffer
 *  @param direction    DMA sync direction
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status moal_dma_sync_to_device(t_void *pmoal, t_u32 size, t_u64 buf_pa,
				    moal_dma_sync_direction_t direction)
{
	moal_handle *handle = (moal_handle *)pmoal;
	pcie_service_card *card = handle->card;

	if (unlikely(!card))
		return MLAN_STATUS_FAILURE;

	size = (size + card->cache_alignment_mask) &
	       ~card->cache_alignment_mask;

	// make sure we have one-to-one mapping to Linux`s enum
	// dma_data_direction
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_BIDIR != (int)DMA_BIDIRECTIONAL);
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_TO_DEVICE != (int)DMA_TO_DEVICE);
	BUILD_BUG_ON((int)MOAL_DMA_SYNC_FROM_DEVICE != (int)DMA_FROM_DEVICE);

	dma_sync_single_for_device(&card->dev->dev, buf_pa, size,
				   (enum dma_data_direction)direction);

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
#ifdef PCIEAW693
	if (IS_PCIEAW693(handle->card_type) &&
	    (handle->card_rev == CHIP_AW693_REV_A0))
		buf_pa &= 0xffffffff;
#endif
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
#ifdef PCIEAW693
	if (IS_PCIEAW693(handle->card_type) &&
	    (handle->card_rev == CHIP_AW693_REV_A0))
		dma |= 0x100000000;
#endif
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
#ifdef PCIEAW693
	if (IS_PCIEAW693(handle->card_type) &&
	    (handle->card_rev == CHIP_AW693_REV_A0))
		buf_pa &= 0xffffffff;
#endif
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
	unsigned long delay;
	wifi_timeval t;

	if (drop_point < MAX_TP_ACCOUNT_DROP_POINT_NUM) {
		if (drop_point == 4 || drop_point == 3) {
			handle->tp_acnt.tx_bytes[drop_point] += pmbuf->data_len;
		} else {
			skb = (struct sk_buff *)buf;
			handle->tp_acnt.tx_bytes[drop_point] += skb->len;
		}
		handle->tp_acnt.tx_packets[drop_point]++;
		if (drop_point == 3) {
			woal_get_monotonic_time(&t);
			pmbuf->extra_ts_sec = t.time_sec;
			pmbuf->extra_ts_usec = t.time_usec;
		}
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
		delay = ((unsigned long)pmbuf->extra_ts_sec -
			 (unsigned long)pmbuf->in_ts_sec) *
			1000000UL;
		delay += (pmbuf->extra_ts_usec - pmbuf->in_ts_usec);
		handle->tp_acnt.rx_delay1_driver[handle->tp_acnt.rx_index] =
			(unsigned long)delay;
		/* before netif_rx - deque */
		delay = ((unsigned long)pmbuf->out_ts_sec -
			 (unsigned long)pmbuf->extra_ts_sec) *
			1000000UL;
		delay += (pmbuf->out_ts_usec - pmbuf->extra_ts_usec);
		handle->tp_acnt.rx_delay2_driver[handle->tp_acnt.rx_index] =
			(unsigned long)delay;
		/* netif_rx to netif_rx return */
		delay = ((unsigned long)t.time_sec -
			 (unsigned long)pmbuf->out_ts_sec) *
			1000000UL;
		delay += (t.time_usec - pmbuf->out_ts_usec);
		handle->tp_acnt.rx_delay_kernel[handle->tp_acnt.rx_index] =
			(unsigned long)delay;
		handle->tp_acnt.rx_index++;
		if (handle->tp_acnt.rx_index >= TXRX_MAX_SAMPLE)
			handle->tp_acnt.rx_index = 0;
	} else if (drop_point == TX_TIME_PKT) {
		delay = ((unsigned long)pmbuf->extra_ts_sec -
			 (unsigned long)pmbuf->in_ts_sec) *
			1000000UL;
		delay += (pmbuf->extra_ts_usec - pmbuf->in_ts_usec);
		handle->tp_acnt.tx_delay1_driver[handle->tp_acnt.tx_index] =
			(unsigned long)delay;

		delay = ((unsigned long)pmbuf->out_ts_sec -
			 (unsigned long)pmbuf->in_ts_sec) *
			1000000UL;
		delay += (pmbuf->out_ts_usec - pmbuf->in_ts_usec);
		handle->tp_acnt.tx_delay_driver[handle->tp_acnt.tx_index] =
			(unsigned long)delay;

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
#if defined(PCIE9098) || defined(PCIEAW693) || defined(SDAW693)
	size_t drv_ver_len = strlen(driver_version);
#endif
	ENTER();
	if (status == MLAN_STATUS_SUCCESS) {
		PRINTM(MCMND, "Get Hw Spec done, fw_cap=0x%x\n", phw->fw_cap);
#ifdef PCIE9098
		/** Special/Temporary handling to manage the driver version
		 * string to identify AW690/AW590/AW592 (skyhawk based) based on
		 * fw_cap_ext value set by Fw */
		if (phw->fw_cap_ext & (MBIT(31) | MBIT(30) | MBIT(29)) &&
		    IS_PCIE9098(handle->card_type)) {
			if (phw->fw_cap_ext & MBIT(29)) {
				moal_memcpy_ext(handle, driver_version,
						CARD_PCIEAW592,
						strlen(CARD_PCIEAW592),
						strlen(driver_version));
			} else if (phw->fw_cap_ext & MBIT(30)) {
				moal_memcpy_ext(handle, driver_version,
						CARD_PCIEAW590,
						strlen(CARD_PCIEAW590),
						strlen(driver_version));
			} else if (phw->fw_cap_ext & MBIT(31)) {
				moal_memcpy_ext(handle, driver_version,
						CARD_PCIEAW690,
						strlen(CARD_PCIEAW690),
						strlen(driver_version));
			}
			/* we are copying card name in middle of full version,
			 * we can not copy null termination. This was already
			 * tried and reverted as full version got terminated in
			 * middle(See commit
			 * 57c27201f9a23562337491f3cbb9833ca348076c). thus
			 * suppressing the coverity warning for all card types
			 * in this function.
			 */
			// coverity[string_null:SUPPRESS]
			// coverity[cert_str32_c_violation:SUPPRESS]
			moal_memcpy_ext(handle,
					driver_version + strlen(INTF_CARDTYPE) +
						strlen(KERN_VERSION),
					V17, strlen(V17),
					strlen(driver_version) -
						strlen(INTF_CARDTYPE) -
						strlen(KERN_VERSION));
			if (drv_ver_len >= MLAN_MAX_VER_STR_LEN - 1) {
				drv_ver_len = MLAN_MAX_VER_STR_LEN - 1;
			}
			/* drv_ver_len is explicitly capped to
			 * MLAN_MAX_VER_STR_LEN - 1 (34 bytes), which matches
			 * the size of driver_version. No buffer overrun is
			 * possible.
			 */
			// coverity[overrun-buffer-arg:SUPPRESS]
			// coverity[cert_arr30_c_violation:SUPPRESS]
			moal_memcpy_ext(handle, handle->driver_version,
					driver_version, drv_ver_len,
					MLAN_MAX_VER_STR_LEN - 1);
			handle->driver_version[drv_ver_len] = '\0';
		}
#endif
#ifdef PCIEAW693
		/**
		 *  Special/Temporary handling to manage the driver version
		 * string to identify AW693/IW623 based on fw_cap value set by
		 * Fw
		 */
		if ((phw->fw_cap_ext & MBIT(23)) &&
		    IS_PCIEAW693(handle->card_type)) {
			moal_memcpy_ext(handle, driver_version, CARD_PCIEIW623,
					strlen(CARD_PCIEIW623),
					strlen(driver_version));
			if (drv_ver_len >= MLAN_MAX_VER_STR_LEN - 1) {
				drv_ver_len = MLAN_MAX_VER_STR_LEN - 1;
			}
			moal_memcpy_ext(handle, handle->driver_version,
					driver_version, drv_ver_len,
					MLAN_MAX_VER_STR_LEN - 1);
			handle->driver_version[drv_ver_len] = '\0';
		}
#endif
#ifdef SDAW693
		/**
		 *  Special/Temporary handling to manage the driver version
		 * string to identify AW693/IW623 based on fw_cap value set by
		 * Fw
		 */
		if ((phw->fw_cap_ext & MBIT(23)) &&
		    IS_SDAW693(handle->card_type)) {
			moal_memcpy_ext(handle, driver_version, CARD_SDIW623,
					strlen(CARD_SDIW623),
					strlen(driver_version));
			if (drv_ver_len >= MLAN_MAX_VER_STR_LEN - 1) {
				drv_ver_len = MLAN_MAX_VER_STR_LEN - 1;
			}
			moal_memcpy_ext(handle, handle->driver_version,
					driver_version, drv_ver_len,
					MLAN_MAX_VER_STR_LEN - 1);
			handle->driver_version[drv_ver_len] = '\0';
		}
#endif

		if (phw->fw_cap & FW_CAPINFO_DISABLE_NAN)
			handle->params.drv_mode &= ~DRV_MODE_NAN;
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

		moal_memcpy_ext(handle, &(handle->hw_info), phw,
				sizeof(mlan_hw_info), sizeof(mlan_hw_info));
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
		if (handle->rf_test_mode == MTRUE)
			PRINTM(MERROR,
			       "Operation id=0x%x not allowed in rf-mode\n",
			       pioctl_req->req_id);
		else
			PRINTM(MERROR,
			       "IOCTL failed: %p id=0x%x, sub_id=0x%x action=%d, status_code=0x%x [%s]\n",
			       pioctl_req, pioctl_req->req_id,
			       (*(t_u32 *)pioctl_req->pbuf),
			       (int)pioctl_req->action, pioctl_req->status_code,
			       wlan_errorcode_get_name(
				       pioctl_req->status_code));
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

#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
	struct net_device *dev = NULL;
#endif
#endif

	ENTER();

#ifdef XDP_SUPPORT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	if (pmbuf && (pmbuf->flags & MLAN_BUF_FLAG_XDP)) {
		woal_free_mlan_buffer(handle, pmbuf);
		atomic_dec(&handle->tx_pending);
		return MLAN_STATUS_SUCCESS;
	}
#endif
#endif

	if (pmbuf && pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA) {
		woal_free_mlan_buffer(handle, pmbuf);
		atomic_dec(&handle->tx_pending);
		goto done;
	}
	if (pmbuf) {
		priv = woal_bss_index_to_priv(pmoal, pmbuf->bss_index);
		skb = (struct sk_buff *)pmbuf->pdesc;
#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
		if (priv && priv->multi_ap_flag && skb) {
			dev = skb->dev;
			if (dev)
				priv = (moal_private *)netdev_priv(dev);
		}
#endif
#endif
		if (priv) {
			woal_set_trans_start(priv->netdev);
			if (skb) {
				if (status == MLAN_STATUS_SUCCESS) {
					priv->stats.tx_packets++;
					priv->stats.tx_bytes += skb->len;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
					if (drvdbg & MDAT_D)
						woal_packet_fate_monitor(
							priv, PACKET_TYPE_TX,
							TX_PKT_FATE_SENT,
							FRAME_TYPE_ETHERNET_II,
							0, 0, skb->data,
							skb->data_len);
#endif
#endif
				} else {
					priv->stats.tx_errors++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
					if (drvdbg & MDAT_D)
						woal_packet_fate_monitor(
							priv, PACKET_TYPE_TX,
							TX_PKT_FATE_DRV_DROP_OTHER,
							FRAME_TYPE_ETHERNET_II,
							0, 0, skb->data,
							skb->data_len);
#endif
#endif
				}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
				index = skb_get_queue_mapping(skb);
				if (index < 4) {
					atomic_dec(&handle->tx_pending);
					if (atomic_dec_return(
						    &priv->wmm_tx_pending[index]) ==
					    priv->low_tx_pending) {
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
						if (!handle->priv[i])
							continue;
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

#define DOT11_MAX_PRIORITY 8
#define IEEE80211_RADIOTAP_HE 23

t_u8 ru_signal[16][9] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08},
			 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x07},
			 {0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x07},
			 {0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0xff, 0x01, 0x06},
			 {0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x07},
			 {0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x06},
			 {0x00, 0x00, 0xff, 0x01, 0xff, 0x01, 0x00, 0x00, 0x06},
			 {0x00, 0x00, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0x05},
			 {0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07},
			 {0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x06},
			 {0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x06},
			 {0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0xff, 0x01, 0x05},
			 {0xff, 0x01, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x06},
			 {0xff, 0x01, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x05},
			 {0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0x00, 0x00, 0x05},
			 {0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01,
			  0x04}};

t_u8 ru_signal_106[14][9] = {
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00},
	{0xff, 0x01, 0xff, 0x01, 0xff, 0xff, 0xff, 0x02, 0x03},
	{0xff, 0xff, 0xff, 0x02, 0xff, 0x01, 0xff, 0x01, 0x03},
	{0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x02, 0x05},
	{0x00, 0x00, 0xff, 0x01, 0xff, 0xff, 0xff, 0x02, 0x04},
	{0xff, 0x01, 0x00, 0x00, 0xff, 0xff, 0xff, 0x02, 0x04},
	{0xff, 0x01, 0xff, 0x01, 0xff, 0xff, 0xff, 0x02, 0x03},
	{0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x05},
	{0xff, 0xff, 0xff, 0x02, 0x00, 0x00, 0xff, 0x01, 0x04},
	{0xff, 0xff, 0xff, 0x02, 0xff, 0x01, 0x00, 0x00, 0x04},
	{0xff, 0xff, 0xff, 0x02, 0xff, 0x01, 0xff, 0x01, 0x03},
	{0xff, 0xff, 0xff, 0x02, 0xff, 0xff, 0xff, 0x02, 0x02},
	{0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0x04}};

t_u8 ru_signal_52[9] = {0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0x04};
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
	struct ieee80211_radiotap_header *rth_hdr = NULL;
	struct radiotap_body *rth_body = NULL;
	t_u32 it_present_1;
	t_u32 it_present_2;
	radiotap_timestamp *ts_info = NULL;
	t_u8 *radiotap_buf = NULL;
	t_u8 *radiotap_pos = NULL;
	t_u32 radiotap_max_len = 0;
	t_u32 radiotap_len = 0;
	radiotap_info rt_info = {};
	t_u8 format = 0;
	t_u8 mcs = 0;
	t_u8 nss = 0;
	t_u8 bw = 0;
	t_u8 gi = 0;
	t_u8 ldpc = 0;
	t_u8 chan_num;
	t_u8 band = 0;
	struct ieee80211_hdr *dot11_hdr = NULL;
	t_u8 *payload = NULL;
	t_u32 vht_sig1 = 0;
	t_u32 vht_sig2 = 0;
	t_u32 he_sig1 = 0;
	t_u32 he_sig2 = 0;
	t_u32 usr_idx = 0;
	t_u32 out = 0;
	t_u32 tone = 0;
	t_u8 dcm = 0;
	ENTER();
	if (!pmbuf->pdesc) {
		LEAVE();
		return status;
	}

	radiotap_max_len =
		sizeof(struct radiotap_header) + sizeof(struct mcs_field) +
		sizeof(radiotap_timestamp) + sizeof(struct vht_field) +
		sizeof(struct he_field) + 12; /* 12 is max alignment bytes */
	radiotap_buf = kzalloc(radiotap_max_len, GFP_ATOMIC);
	if (radiotap_buf == NULL)
		goto done;

	radiotap_pos = radiotap_buf;

	skb = (struct sk_buff *)pmbuf->pdesc;

	if ((handle->mon_if) && (handle->mon_if->mon_ndev)) {
		if (netif_running(handle->mon_if->mon_ndev)) {
			if (handle->mon_if->radiotap_enabled) {
				if (skb_headroom(skb) < radiotap_max_len) {
					PRINTM(MERROR,
					       "%s No space to add Radio TAP header\n",
					       __func__);
					status = MLAN_STATUS_FAILURE;
					handle->mon_if->stats.rx_dropped++;
					goto done;
				}
				dot11_hdr = (struct ieee80211_hdr
						     *)(pmbuf->pbuf +
							pmbuf->data_offset);
				moal_memcpy_ext(
					handle, &rt_info,
					pmbuf->pbuf + pmbuf->data_offset -
						sizeof(rt_info),
					sizeof(rt_info), sizeof(rt_info));
				ldpc = (rt_info.rate_info.rate_info & 0x20) >>
				       5;
				format = (rt_info.rate_info.rate_info & 0x18) >>
					 3;
				bw = (rt_info.rate_info.rate_info & 0x06) >> 1;
				dcm = rt_info.rate_info.dcm;
				if (format == MLAN_RATE_FORMAT_HE)
					gi = (rt_info.rate_info.rate_info &
					      0xC0) >>
					     6;
				else

					gi = rt_info.rate_info.rate_info & 0x01;
				mcs = rt_info.rate_info.mcs_index;
				nss = rt_info.rate_info.nss_index;

				rth_hdr = (struct ieee80211_radiotap_header *)
					radiotap_pos;
				rth_hdr->it_version = PKTHDR_RADIOTAP_VERSION;
				rth_hdr->it_pad = 0;
				rth_hdr->it_present = cpu_to_le32(
					(1 << IEEE80211_RADIOTAP_TSFT) |
					(1 << IEEE80211_RADIOTAP_FLAGS) |
					(1 << IEEE80211_RADIOTAP_CHANNEL) |
					(1
					 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
					(1 << IEEE80211_RADIOTAP_DBM_ANTNOISE) |
					(1 << IEEE80211_RADIOTAP_ANTENNA) |
					(1 << IEEE80211_RADIOTAP_RX_FLAGS));
				radiotap_pos += sizeof(
					struct ieee80211_radiotap_header);
				radiotap_len += sizeof(
					struct ieee80211_radiotap_header);
				if (rt_info.radiotap_extra) {
					rth_hdr->it_present |= cpu_to_le32(
						(1
						 << IEEE80211_RADIOTAP_TIMESTAMP) |
						(1
						 << IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
						(1 << IEEE80211_RADIOTAP_EXT));
					it_present_1 = cpu_to_le32(
						(1
						 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
						(1
						 << IEEE80211_RADIOTAP_ANTENNA) |
						(1
						 << IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
						(1 << IEEE80211_RADIOTAP_EXT));
					it_present_2 = cpu_to_le32(
						(1
						 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
						(1
						 << IEEE80211_RADIOTAP_ANTENNA));

					moal_memcpy_ext(handle, radiotap_pos,
							&it_present_1,
							sizeof(t_u32),
							sizeof(t_u32));
					radiotap_pos += sizeof(t_u32);
					radiotap_len += sizeof(t_u32);
					moal_memcpy_ext(handle, radiotap_pos,
							&it_present_2,
							sizeof(t_u32),
							sizeof(t_u32));
					radiotap_pos += sizeof(t_u32);
					radiotap_len += sizeof(t_u32);
				}

				rth_body = (struct radiotap_body *)radiotap_pos;
				/** TSFT: bit number 0 */
				rth_body->timestamp = woal_cpu_to_le64(jiffies);
				/** Flags: bit number 1 */
				rth_body->flags =
					(rt_info.extra_info.flags &
					 ~(RADIOTAP_FLAGS_USE_SGI_HT |
					   RADIOTAP_FLAGS_WITH_FRAGMENT |
					   RADIOTAP_FLAGS_WEP_ENCRYPTION |
					   RADIOTAP_FLAGS_FAILED_FCS_CHECK));
				/** reverse fail fcs, 1 means pass FCS in FW,
				 * but means fail FCS in radiotap */
				rth_body->flags |=
					(~rt_info.extra_info.flags) &
					RADIOTAP_FLAGS_FAILED_FCS_CHECK;
				if ((format == MLAN_RATE_FORMAT_HT) &&
				    (gi == 1))
					rth_body->flags |=
						RADIOTAP_FLAGS_USE_SGI_HT;
				if (ieee80211_is_mgmt(
					    dot11_hdr->frame_control) ||
				    ieee80211_is_data(
					    dot11_hdr->frame_control)) {
					if ((ieee80211_has_morefrags(
						    dot11_hdr->frame_control)) ||
					    (!ieee80211_is_first_frag(
						    dot11_hdr->seq_ctrl))) {
						rth_body->flags |=
							RADIOTAP_FLAGS_WITH_FRAGMENT;
					}
				}
				if (ieee80211_is_data(
					    dot11_hdr->frame_control) &&
				    ieee80211_has_protected(
					    dot11_hdr->frame_control)) {
					payload =
						(t_u8 *)dot11_hdr +
						ieee80211_hdrlen(
							dot11_hdr->frame_control);
					if (!(*(payload + 3) & 0x20)) /** ExtIV
									 bit
									 shall
									 be 0
									 for WEP
									 frame
								       */
						rth_body->flags |=
							RADIOTAP_FLAGS_WEP_ENCRYPTION;
				}
				/** Rate: bit number 2, t_u8 only apply for LG
				 * mode */
				if (format == MLAN_RATE_FORMAT_LG) {
					// safe constant bitmask with explicit
					// endianness conversion
					// coverity[misra_c_2012_rule_10_8_violation:SUPPRESS]
					rth_hdr->it_present |= cpu_to_le32(
						1 << IEEE80211_RADIOTAP_RATE);
					rth_body->rate =
						rt_info.rate_info.bitrate;
				}
				/** Channel: bit number 3 */
				rth_body->channel.flags = 0;
				if (rt_info.chan_num &&
				    (handle->mon_if->band_chan_cfg.channel !=
				     rt_info.chan_num))
					handle->mon_if->band_chan_cfg.channel =
						rt_info.chan_num;
				chan_num =
					handle->mon_if->band_chan_cfg.channel;

				band = woal_radio_type_to_ieee_band(
					handle->mon_if->band_chan_cfg.band);

				rth_body->channel.frequency = woal_cpu_to_le16(
					ieee80211_channel_to_frequency(chan_num,
								       band));

				if (band == IEEE80211_BAND_2GHZ)
					rth_body->channel.flags |=
						woal_cpu_to_le16(
							CHANNEL_FLAGS_2GHZ);
				else if (band == IEEE80211_BAND_5GHZ
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
					 || band == IEEE80211_BAND_6GHZ
#endif
				)
					rth_body->channel.flags |=
						woal_cpu_to_le16(
							CHANNEL_FLAGS_5GHZ);

				if (rth_body->channel.flags &
				    woal_cpu_to_le16(CHANNEL_FLAGS_2GHZ))
					rth_body->channel
						.flags |= woal_cpu_to_le16(
						CHANNEL_FLAGS_DYNAMIC_CCK_OFDM);
				else
					rth_body->channel.flags |=
						woal_cpu_to_le16(
							CHANNEL_FLAGS_OFDM);
				if (handle->mon_if->chandef.chan &&
				    (handle->mon_if->chandef.chan->flags &
				     (IEEE80211_CHAN_PASSIVE_SCAN |
				      IEEE80211_CHAN_RADAR)))
					rth_body->channel
						.flags |= woal_cpu_to_le16(
						CHANNEL_FLAGS_ONLY_PASSIVSCAN_ALLOW);
				/** Antenna signal: bit number 5 */
				rth_body->antenna_signal =
					-(rt_info.nf - rt_info.snr);
				/** Antenna noise: bit number 6 */
				rth_body->antenna_noise = -rt_info.nf;
				/* Antenna: bit number 11, Convert FW antenna
				 * value to radiotap spec */
				rth_body->antenna = (t_u8)rt_info.antenna >> 1;

				/** rx_flags: bit number 14 */
				if (rt_info.radiotap_extra &&
				    (rt_info.extra_info.plcp_crc_failed == 1))
					rth_body->rx_flags = 0x0002;

				radiotap_pos += sizeof(struct radiotap_body);
				radiotap_len += sizeof(struct radiotap_body);
				/** MCS: bit number 19 */
				if (format == MLAN_RATE_FORMAT_HT) {
					struct mcs_field *mcs =
						(struct mcs_field *)radiotap_pos;
					// safe constant bitmask with explicit
					// endianness conversion
					// coverity[misra_c_2012_rule_10_8_violation:SUPPRESS]
					rth_hdr->it_present |= cpu_to_le32(
						1 << IEEE80211_RADIOTAP_MCS);
					mcs->known =
						rt_info.extra_info.mcs_known;
					mcs->flags =
						rt_info.extra_info.mcs_flags;
					/** MCS mcs */
					mcs->known |= MCS_KNOWN_MCS_INDEX_KNOWN;
					mcs->mcs = rt_info.rate_info.mcs_index;
					/** MCS bw */
					mcs->known |= MCS_KNOWN_BANDWIDTH;
					mcs->flags &= ~(0x03); /** Clear,
									     20MHz
								  as default */
					if (bw == 1)
						mcs->flags |= RX_BW_40;
					/** MCS gi */
					mcs->known |= MCS_KNOWN_GUARD_INTERVAL;
					mcs->flags &= ~(1 << 2);
					if (gi)
						mcs->flags |= gi << 2;
					/** MCS FEC */
					mcs->known |= MCS_KNOWN_FEC_TYPE;
					mcs->flags &= ~(1 << 4);
					if (ldpc)
						mcs->flags |= ldpc << 4;

					radiotap_pos +=
						sizeof(struct mcs_field);
					radiotap_len +=
						sizeof(struct mcs_field);
				}
				/** VHT: bit number 21, Required Alignment is 2
				 */
				if (format == MLAN_RATE_FORMAT_VHT) {
					struct vht_field *vht = NULL;

					/* ensure 2 byte alignment */
					if (radiotap_len & 1) {
						radiotap_pos++;
						radiotap_len++;
					}
					vht = (struct vht_field *)radiotap_pos;
					vht_sig1 =
						rt_info.extra_info.vht_he_sig1;
					vht_sig2 =
						rt_info.extra_info.vht_he_sig2;
					/** Present Flag */
					// safe constant bitmask with explicit
					// endianness conversion
					// coverity[misra_c_2012_rule_10_8_violation:SUPPRESS]
					rth_hdr->it_present |= cpu_to_le32(
						1 << IEEE80211_RADIOTAP_VHT);
					/** STBC */
					vht->known |= woal_cpu_to_le16(
						VHT_KNOWN_STBC);
					if (vht_sig1 & MBIT(3))
						vht->flags |= VHT_FLAG_STBC;
					/** TXOP_PS_NA */
					/** TODO: Not support now */
					/** GI */
					vht->known |=
						woal_cpu_to_le16(VHT_KNOWN_GI);
					if (vht_sig2 & MBIT(0))
						vht->flags |= VHT_FLAG_SGI;
					/** SGI NSYM DIS */
					vht->known |= woal_cpu_to_le16(
						VHT_KNOWN_SGI_NSYM_DIS);
					if (vht_sig2 & MBIT(1))
						vht->flags |=
							VHT_FLAG_SGI_NSYM_M10_9;
					/** LDPC_EXTRA_OFDM_SYM */
					/** TODO: Not support now */
					/** BEAMFORMED */
					vht->known |= woal_cpu_to_le16(
						VHT_KNOWN_BEAMFORMED);
					if (vht_sig2 & MBIT(8))
						vht->flags |=
							VHT_FLAG_BEAMFORMED;
					/** BANDWIDTH */
					vht->known |= woal_cpu_to_le16(
						VHT_KNOWN_BANDWIDTH);
					if (bw == 1)
						vht->bandwidth = RX_BW_40;
					else if (bw == 2)
						vht->bandwidth = RX_BW_80;
					/** GROUP_ID */
					vht->known |= woal_cpu_to_le16(
						VHT_KNOWN_GROUP_ID);
					vht->group_id =
						(vht_sig1 & (0x3F0)) >> 4;
					/** PARTIAL_AID */
					/** TODO: Not support now */
					/** mcs_nss */
					vht->mcs_nss[0] = vht_sig2 & (0xF0);
					/* Convert FW NSS value to radiotap spec
					 */
					vht->mcs_nss[0] |=
						((vht_sig1 & (0x1C00)) >> 10) +
						1;
					/** gi */
					vht->known |=
						woal_cpu_to_le16(VHT_KNOWN_GI);
					if (gi)
						vht->flags |= VHT_FLAG_SGI;
					/** coding */
					if (vht_sig2 & MBIT(2))
						vht->coding |=
							VHT_CODING_LDPC_USER0;

					radiotap_pos +=
						sizeof(struct vht_field);
					radiotap_len +=
						sizeof(struct vht_field);
				}
				/** Timstamp: bit number 22, Required Alignment
				 * is 8 */
				if (rt_info.radiotap_extra) {
					/* ensure 8 byte alignment */
					while (radiotap_len & 7) {
						radiotap_pos++;
						radiotap_len++;
					}
					ts_info = (radiotap_timestamp *)
						radiotap_pos;
					ts_info->device_timestamp = cpu_to_le64(
						rt_info.extra_info.timestamp
							.device_timestamp);
					ts_info->accuracy =
						rt_info.extra_info.timestamp
							.accuracy;
					ts_info->unit = rt_info.extra_info
								.timestamp.unit;
					ts_info->position =
						rt_info.extra_info.timestamp
							.position;
					ts_info->flags =
						rt_info.extra_info.timestamp
							.flags;
					radiotap_pos +=
						sizeof(radiotap_timestamp);
					radiotap_len +=
						sizeof(radiotap_timestamp);
				}

				/** HE: bit number 23, Required Alignment is 2
				 */
				if (format == MLAN_RATE_FORMAT_HE) {
					struct he_field *he = NULL;

					/* ensure 2 byte alignment */
					if (radiotap_len & 1) {
						radiotap_pos++;
						radiotap_len++;
					}
					he = (struct he_field *)radiotap_pos;
					he_sig1 =
						rt_info.extra_info.vht_he_sig1;
					he_sig2 =
						rt_info.extra_info.vht_he_sig2;
					usr_idx = rt_info.extra_info.user_idx;
					// safe constant bitmask with explicit
					// endianness conversion
					// coverity[misra_c_2012_rule_10_8_violation:SUPPRESS]
					rth_hdr->it_present |= cpu_to_le32(
						1 << IEEE80211_RADIOTAP_HE);
					he->data1 |= (HE_CODING_KNOWN);
					if (ldpc)
						he->data3 |=
							HE_CODING_LDPC_USER0;
					he->data1 |= (HE_BW_KNOWN);
					if (he_sig1)
						he->data1 |= (HE_MU_DATA);
					if (bw == 1) {
						he->data5 |= RX_HE_BW_40;
						if (he_sig2) {
							MLAN_DECODE_RU_SIGNALING_CH1(
								out, he_sig1,
								he_sig2);
							MLAN_DECODE_RU_TONE(
								out, usr_idx,
								tone);
							if (!tone) {
								MLAN_DECODE_RU_SIGNALING_CH3(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (tone != 0) {
								he->data5 &=
									~RX_HE_BW_40;
								he->data5 |=
									tone;
							}
						}
					} else if (bw == 2) {
						he->data5 |= RX_HE_BW_80;
						if (he_sig2) {
							MLAN_DECODE_RU_SIGNALING_CH1(
								out, he_sig1,
								he_sig2);
							MLAN_DECODE_RU_TONE(
								out, usr_idx,
								tone);
							if (!tone) {
								MLAN_DECODE_RU_SIGNALING_CH2(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (!tone) {
								if ((he_sig2 &
								     MLAN_80_CENTER_RU) &&
								    !usr_idx) {
									tone = RU_TONE_26;
								} else {
									usr_idx--;
								}
							}
							if (!tone) {
								MLAN_DECODE_RU_SIGNALING_CH3(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (!tone) {
								MLAN_DECODE_RU_SIGNALING_CH4(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (tone != 0) {
								he->data5 &=
									~RX_HE_BW_80;
								he->data5 |=
									tone;
							}
						}
					} else if (bw == 3) {
						he->data5 |= RX_HE_BW_160;
						if (he_sig2) {
							MLAN_DECODE_RU_SIGNALING_CH1(
								out, he_sig1,
								he_sig2);
							MLAN_DECODE_RU_TONE(
								out, usr_idx,
								tone);
							if (!tone) {
								MLAN_DECODE_RU_SIGNALING_CH2(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (!tone) {
								if ((he_sig2 &
								     MLAN_160_CENTER_RU) &&
								    !usr_idx) {
									tone = RU_TONE_26;
								} else {
									usr_idx--;
								}
							}
							if (!tone) {
								MLAN_DECODING_160_RU_CH3(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (!tone) {
								MLAN_DECODING_160_RU_CH3(
									out,
									he_sig1,
									he_sig2);
								MLAN_DECODE_RU_TONE(
									out,
									usr_idx,
									tone);
							}
							if (tone != 0) {
								he->data5 &=
									~RX_HE_BW_160;
								he->data5 |=
									tone;
							}
						}
					} else {
						if (he_sig2) {
							MLAN_DECODE_RU_SIGNALING_CH1(
								out, he_sig1,
								he_sig2);
							MLAN_DECODE_RU_TONE(
								out, usr_idx,
								tone);
							if (tone) {
								he->data5 |=
									tone;
							}
						}
					}

					he->data2 |= (HE_DATA_GI_KNOWN);
					he->data5 |= ((gi & 3) << 4);
					he->data1 |= (HE_MCS_KNOWN);

					he->data3 |= (mcs << 8);
					he->data6 |= nss;
					he->data1 |= (HE_DCM_KNOWN);
					he->data1 = cpu_to_le16(he->data1);
					he->data5 |= (dcm << 12);
					he->data5 = cpu_to_le16(he->data5);
					he->data3 = cpu_to_le16(he->data3);

					radiotap_pos += sizeof(struct he_field);
					radiotap_len += sizeof(struct he_field);
				}
				if (rt_info.radiotap_extra) {
					radiotap_pos[0] =
						rt_info.extra_info.rssi_dbm_a;
					radiotap_pos[1] = 0;
					radiotap_pos[2] =
						rt_info.extra_info.rssi_dbm_b;
					radiotap_pos[3] = 1;
					radiotap_pos += 4;
					radiotap_len += 4;
				}
				rth_hdr->it_len = cpu_to_le16(radiotap_len);
				skb_push(skb, radiotap_len);
				moal_memcpy_ext(handle, skb->data, radiotap_buf,
						radiotap_len, radiotap_len);
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
	}

done:
	if (radiotap_buf)
		kfree(radiotap_buf);

	LEAVE();
	return status;
}
#endif
#endif

#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
/**
 *  @brief This function get binded net_device from station list
 *
 *  @param priv Pointer to structure moal_private
 *  @param aid    station aid from mlan
 *
 *  @return    binded net_device pointer or NULL if not found
 */
static struct net_device *moal_get_netdev_from_stalist(moal_private *priv,
						       t_u16 aid)
{
	station_node *sta_node = NULL;

	ENTER();
	sta_node = priv->vlan_sta_list[(aid - 1) % MAX_STA_COUNT];
	if (sta_node) {
		LEAVE();
		return sta_node->netdev;
	}
	LEAVE();
	return NULL;
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
#if defined(UAP_SUPPORT) && defined(XDP_SUPPORT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
	struct sk_buff *xdp_skb = NULL;
#endif
#endif
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
#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
	t_u16 aid = 0;
#endif
#endif

	wifi_timeval t1, t2;
	unsigned long delay;
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
#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
	if (pmbuf->flags & MLAN_BUF_FLAG_EASYMESH) {
		aid = (pmbuf->priority & 0xFF000000) >> 24;
		if (!priv->vlan_sta_list[(aid - 1) % MAX_STA_COUNT]->is_valid) {
			priv->stats.rx_dropped++;
			goto done;
		}
		if (aid != 0)
			netdev = moal_get_netdev_from_stalist(priv, aid);
	}
#endif
#endif
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
			if (!frame) {
				PRINTM(MERROR,
				       " AMSDU RECV:skb_clone returned NULL\n");
				break;
			}

			skb_trim(frame, len);
			eth = (struct ethhdr *)skb_pull(skb, len + padding);
			if (!eth) {
				PRINTM(MERROR, "Invalid amsdu packet\n");
				dev_kfree_skb(frame);
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
#if defined(UAP_SUPPORT) && defined(XDP_SUPPORT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
		if (priv->xdp_prog) {
			xdp_skb = woal_process_xdp(priv, priv->netdev, &mbuf);
			if (xdp_skb)
				napi_gro_receive(&handle->napi_rx, xdp_skb);
			dev_kfree_skb(frame);
			continue;
		}
#endif
#endif
		frame->protocol = eth_type_trans(frame, netdev);
		frame->ip_summed = CHECKSUM_NONE;

		priv->stats.rx_bytes += frame->len;
		priv->stats.rx_packets++;

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
		delay = ((unsigned long)t2.time_sec -
			 (unsigned long)in_ts_sec) *
			1000000UL;
		delay += (t2.time_usec - in_ts_usec);
		moal_amsdu_tp_accounting(pmoal, (t_s32)delay, 0);
	}
done:
	if (status == MLAN_STATUS_PENDING)
		atomic_dec(&handle->mbufalloc_count);
	LEAVE();
	return status;
}

static t_u8 moal_user_priority_to_qos(t_u32 userPriority)
{
	t_u8 aci;

	switch (userPriority) {
	default:
	case 0:
	case 3:
		/* BE */
		aci = 3;
		break;

	case 1:
	case 2:
		/* BK */
		aci = 2;
		break;

	case 4:
	case 5:
		/* VI */
		aci = 1;
		break;

	case 6:
	case 7:
		/* VO */
		aci = 0;
		break;
	}

	return aci;
}

#if defined(UAP_SUPPORT) && defined(XDP_SUPPORT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
/**
 *  @brief This function acts based on action returned by bpf program
 *         XDP_DROP: drop packet
 *         XDP_PASS: upload packet to network stack
 *         XDP_REDIRECT: redirect packet to other XDP enabled interface
 *  @param pmoal Pointer to the MOAL context
 *  @param pmbuf Pointer to the mlan buffer structure
 *
 *  @return NULL or sk_buff
 */
static struct sk_buff *woal_process_xdp(moal_private *priv,
					struct net_device *ndev,
					pmlan_buffer pmbuf)
{
	t_u32 xdp_act;
	struct xdp_buff xdp_buff;
	struct sk_buff *skb = NULL;
	struct page *page;
	__u8 *data;
	int ret = 0;

	page = dev_alloc_page();
	if (unlikely(!page))
		netdev_err(ndev, "page alloc failed\n");

	data = page_address(page);

	memcpy(data + XDP_PACKET_HEADROOM, pmbuf->pbuf + pmbuf->data_offset,
	       pmbuf->data_len);
	xdp_init_buff(&xdp_buff, PAGE_SIZE - XDP_PACKET_HEADROOM,
		      &priv->xdp_rxq);
	xdp_prepare_buff(&xdp_buff, data, XDP_PACKET_HEADROOM, pmbuf->data_len,
			 false);
	xdp_buff_clear_frags_flag(&xdp_buff);

	xdp_act = bpf_prog_run_xdp(priv->xdp_prog, &xdp_buff);
	switch (xdp_act) {
	case XDP_DROP:
		__free_page(page);
		break;
	case XDP_PASS:
		skb = build_skb(xdp_buff.data_hard_start, PAGE_SIZE);
		skb_reserve(skb, XDP_PACKET_HEADROOM);
		skb_put(skb, xdp_buff.data_end - xdp_buff.data);
#if defined(CONFIG_PAGE_POOL)
		skb_mark_for_recycle(skb);
#endif
		skb->dev = ndev;
		skb->protocol = eth_type_trans(skb, skb->dev);
		skb->ip_summed = CHECKSUM_NONE;
		return skb;
	case XDP_REDIRECT:
		ret = xdp_do_redirect(ndev, &xdp_buff, priv->xdp_prog);
		if (ret)
			PRINTM(MERROR, "XDP: redirect failed:%d\n", ret);
		priv->phandle->xdp_rd++;
		break;
	default:
		return skb;
	}

	return skb;
}
#endif
#endif

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
#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
	t_u16 aid = 0;
#endif
#endif

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
#if defined(UAP_SUPPORT) && defined(XDP_SUPPORT)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
		if (priv->xdp_prog) {
			skb = woal_process_xdp(priv, priv->netdev, pmbuf);
			if (skb)
				napi_gro_receive(&handle->napi_rx, skb);
			goto done;
		}
#endif
#endif
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
					if (drvdbg & MDAT_D)
						woal_packet_fate_monitor(
							priv, PACKET_TYPE_RX,
							RX_PKT_FATE_DRV_DROP_NOBUFS,
							FRAME_TYPE_ETHERNET_II,
							0, 0,
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
			if (ntohs(ethh->h_proto) == ETH_P_PAE) {
				PRINTM(MEVENT,
				       "wlan: %s Rx EAPOL pkt from " MACSTR
				       "\n",
				       priv->netdev->name,
				       MAC2STR(ethh->h_source));
			} else if (ntohs(ethh->h_proto) == NXP_ETH_P_WAPI) {
				PRINTM(MEVENT,
				       "wlan: %s Rx WAPI pkt from " MACSTR "\n",
				       priv->netdev->name,
				       MAC2STR(ethh->h_source));
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
				priv->deauth_evt_cnt = 0;
#endif
			}

#ifdef UAP_SUPPORT
#if defined(UAP_CFG80211) || defined(STA_CFG80211)
			if (pmbuf->flags & MLAN_BUF_FLAG_EASYMESH) {
				aid = (pmbuf->priority & 0xFF000000) >> 24;
				if ((aid > 0) &&
				    (!priv->vlan_sta_list[(aid - 1) %
							  MAX_STA_COUNT]
					      ->is_valid)) {
					status = MLAN_STATUS_FAILURE;
					priv->stats.rx_dropped++;
					goto done;
				}
				if (aid != 0)
					netdev = moal_get_netdev_from_stalist(
						priv, aid);
			}
#endif
#endif
			if (priv->wdev->use_4addr &&
			    priv->wdev->iftype == NL80211_IFTYPE_STATION) {
				t_u32 transaction_id;
				t_u32 hash_key;
				transaction_id =
					woal_get_dhcp_discover_transation_id(
						skb);
				if (transaction_id &&
				    woal_get_dhcp_discover_info(
					    priv, transaction_id)) {
					status = MLAN_STATUS_FAILURE;
					PRINTM(MDATA,
					       "Drop dhcp pkt, transation_id=%x\n",
					       transaction_id);
					goto done;
				}

				hash_key = woal_generate_arp_request_hash(skb);
				// TODO: Drop too old entry
				if (hash_key) {
					struct arp_entry *node;
					// Coverity violation raised for
					// kernel's API
					// coverity[cert_arr39_c_violation:SUPPRESS]
					hash_for_each_possible (priv->hlist,
								node, arp_hlist,
								hash_key) {
						if (node->hash_key ==
						    hash_key) {
							status =
								MLAN_STATUS_FAILURE;
							PRINTM(MDATA,
							       "ARP entry exists, drop pkt\n");
							goto done;
						}
					}
				}
			}
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
				if (drvdbg & MDAT_D)
					woal_packet_fate_monitor(
						priv, PACKET_TYPE_RX,
						RX_PKT_FATE_DRV_DROP_FILTER,
						FRAME_TYPE_ETHERNET_II, 0, 0,
						skb->data, skb->len);
				goto done;
			}
#endif
#endif
			priv->stats.rx_bytes += skb->len;
			priv->stats.rx_packets++;
			priv->rx_pkt_ac[moal_user_priority_to_qos(
				pmbuf->priority)]++;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (drvdbg & MDAT_D)
				woal_packet_fate_monitor(priv, PACKET_TYPE_RX,
							 RX_PKT_FATE_SUCCESS,
							 FRAME_TYPE_ETHERNET_II,
							 0, 0, skb->data,
							 skb->len);
#endif
#endif
#ifdef ANDROID_KERNEL
			if (handle->params.wakelock_timeout) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
				__pm_wakeup_event(
					handle->ws,
					handle->params.wakelock_timeout);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
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
	if (status != MLAN_STATUS_PENDING && pmbuf && !pmbuf->pdesc && skb)
		dev_kfree_skb(skb);
	if (pmbuf && !pmbuf->pbuf)
		status = MLAN_STATUS_PENDING;
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
		if (!pm_qos_request_active(&pmhandle->woal_pm_qos_req))
			pm_qos_add_request(&pmhandle->woal_pm_qos_req,
					   PM_QOS_CPU_DMA_LATENCY, 0);
		else
			PRINTM(MERROR, "PM-QOS request already active\n");
#endif
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
		if (!cpu_latency_qos_request_active(&pmhandle->woal_pm_qos_req))
			cpu_latency_qos_add_request(&pmhandle->woal_pm_qos_req,
						    0);
		else
			PRINTM(MERROR, "PM-QOS request already active\n");
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
		if (pm_qos_request_active(&pmhandle->woal_pm_qos_req))
			pm_qos_remove_request(&pmhandle->woal_pm_qos_req);
		else
			PRINTM(MERROR, "PM-QOS request already removed\n");
#endif
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
		if (cpu_latency_qos_request_active(&pmhandle->woal_pm_qos_req))
			cpu_latency_qos_remove_request(
				&pmhandle->woal_pm_qos_req);
		else
			PRINTM(MERROR, "PM-QOS request already removed\n");
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
 * @brief   Handle CSI STATUS event
 *
 *  @param  pmevent  Pointer to the mlan event structure
 *
 * @return          N/A
 */
static void woal_process_csi_status_report(pmlan_event pmevent)
{
	csi_status_info *pcsi_status = (csi_status_info *)pmevent->event_buf;
	if (pcsi_status->status == CSI_STATUS_ENABLED) {
		PRINTM(MEVENT,
		       "csi status report: enable and start csi on channel %d \r\n",
		       pcsi_status->channel);
	} else if (pcsi_status->status == CSI_STATUS_DISABLED) {
		PRINTM(MEVENT, "csi status report: stop and disable csi \r\n");
	} else if (pcsi_status->status == CSI_STATUS_CONFIG_WRONG) {
		PRINTM(MEVENT,
		       "csi status report: channel or bandwidth config wrong \r\n");
	} else if (pcsi_status->status == CSI_STATUS_INTERNAL_RESET) {
		PRINTM(MEVENT,
		       "csi status report: FW internal restart csi on channel %d \r\n",
		       pcsi_status->channel);
	} else if (pcsi_status->status == CSI_STATUS_INTERNAL_STOP) {
		PRINTM(MEVENT, "csi status report: FW internal stop csi \r\n");
	} else if (pcsi_status->status == CSI_STATUS_INTERNAL_DISABLED) {
		PRINTM(MEVENT,
		       "csi status report: FW internal stop and disable csi, user should put in csi cmd to enable csi \r\n");
	}
}

/**
 *  @brief This function handles rgpower key mismatch event
 *
 *  @param priv pointer to the moal_private structure.
 *
 *  @return         N/A
 */
static void woal_rgpower_key_mismatch_event(moal_private *priv)
{
	struct woal_event *evt;
	unsigned long flags;
	moal_handle *handle = priv->phandle;

	evt = kzalloc(sizeof(struct woal_event), GFP_ATOMIC);
	if (evt) {
		evt->priv = priv;
		evt->type = WOAL_EVENT_RGPWR_KEY_MISMATCH;
		INIT_LIST_HEAD(&evt->link);
		spin_lock_irqsave(&handle->evt_lock, flags);
		list_add_tail(&evt->link, &handle->evt_queue);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		queue_work(handle->evt_workqueue, &handle->evt_work);
	}
}

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
/**
 *  @brief This function prepare wifi reset event
 *
 *  @param priv pointer to the moal_private structure.
 *
 *  @return         N/A
 */
static void woal_wifi_reset_event(moal_private *priv, t_u8 deauth_evt_cnt)
{
	struct woal_event *evt;
	unsigned long flags;
	moal_handle *handle = priv->phandle;

	evt = kzalloc(sizeof(struct woal_event), GFP_ATOMIC);
	if (evt) {
		evt->priv = priv;
		evt->type = WOAL_EVENT_RESET_WIFI;
		evt->deauth_evt_cnt = deauth_evt_cnt;
		INIT_LIST_HEAD(&evt->link);
		spin_lock_irqsave(&handle->evt_lock, flags);
		list_add_tail(&evt->link, &handle->evt_queue);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		queue_work(handle->evt_workqueue, &handle->evt_work);
	}
	/* evt is freed in woal_evt_work_queue, hence suppressed*/
	// coverity[leaked_storage]: SUPPRESS
}
#endif

/**
 *  @brief This function handles defer event receive
 *
 *  @param handle   Pointer to the moal_handle
 *  @param event_id 	event id
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_process_defer_event(moal_handle *handle,
					    mlan_event_id event_id)
{
	mlan_status status = MLAN_STATUS_FAILURE;
	ENTER();
	switch (event_id) {
#ifdef PCIE
	case MLAN_EVENT_ID_DRV_DEFER_RX_DATA:
		status = MLAN_STATUS_SUCCESS;
#ifdef TASKLET_SUPPORT
		tasklet_schedule(&handle->pcie_rx_task);
#else
		queue_work(handle->pcie_rx_workqueue, &handle->pcie_rx_work);
#endif
		break;
	case MLAN_EVENT_ID_DRV_DEFER_CMDRESP:
		status = MLAN_STATUS_SUCCESS;
		queue_work(handle->pcie_cmd_resp_workqueue,
			   &handle->pcie_cmd_resp_work);
		break;
	case MLAN_EVENT_ID_DRV_DEFER_TX_COMPLTE:
		status = MLAN_STATUS_SUCCESS;
#ifdef TASKLET_SUPPORT
		tasklet_schedule(&handle->pcie_tx_complete_task);
#else
		queue_work(handle->pcie_tx_complete_workqueue,
			   &handle->pcie_tx_complete_work);
#endif
		break;
	case MLAN_EVENT_ID_DRV_DELAY_TX_COMPLETE:
		status = MLAN_STATUS_SUCCESS;
		if (!handle->driver_status)
			schedule_delayed_work(&handle->pcie_delayed_tx_work, 1);
		break;
#endif /* PCIE */

	case MLAN_EVENT_ID_DRV_FLUSH_RX_WORK:
		status = MLAN_STATUS_SUCCESS;
		if (moal_extflg_isset(handle, EXT_NAPI)) {
			napi_synchronize(&handle->napi_rx);
			break;
		}
#ifdef PCIE
		if (IS_PCIE(handle->card_type)) {
#ifdef TASKLET_SUPPORT
			tasklet_kill(&handle->pcie_rx_task);
#else
			flush_workqueue(handle->pcie_rx_workqueue);
#endif
			break;
		}
#endif
#if defined(SDIO) || defined(USB)
		flush_workqueue(handle->rx_workqueue);
#endif
		break;
	case MLAN_EVENT_ID_DRV_FLUSH_MAIN_WORK:
		status = MLAN_STATUS_SUCCESS;
		flush_workqueue(handle->workqueue);
		break;
	case MLAN_EVENT_ID_DRV_DEFER_HANDLING:
		status = MLAN_STATUS_SUCCESS;
		queue_work(handle->workqueue, &handle->main_work);
		break;
	case MLAN_EVENT_ID_DRV_DEFER_RX_WORK:
		status = MLAN_STATUS_SUCCESS;
		if (moal_extflg_isset(handle, EXT_NAPI)) {
			napi_schedule(&handle->napi_rx);
			break;
		}
#ifdef PCIE
		if (IS_PCIE(handle->card_type)) {
#ifdef TASKLET_SUPPORT
			tasklet_schedule(&handle->pcie_rx_task);
#else
			queue_work(handle->pcie_rx_workqueue,
				   &handle->pcie_rx_work);
#endif
			break;
		}
#endif
#if defined(USB) || defined(SDIO)
		queue_work(handle->rx_workqueue, &handle->rx_work);
#endif
		break;
	default:
		break;
	}
	LEAVE();
	return status;
}

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
/**
 *  @brief This function handles event receive
 *
 *  @param priv Pointer to the MOAL private
 *  @param tx_status  pointer to tx_mgmt_status_event structure
 *
 *  @return         N/A
 */
static t_void woal_process_event_tx_status(moal_private *priv,
					   tx_mgmt_status_event *tx_status)
{
	unsigned long flag;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	moal_private *remain_priv = NULL;
	t_u8 channel_status;
#endif
	struct tx_status_info *tx_info = NULL;
	PRINTM(MEVENT,
	       "Wlan: Tx status: tx_token=%d, pkt_type=0x%x, status=%d priv->tx_seq_num=%d\n",
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
				priv->phandle
					->priv[priv->phandle->remain_bss_index];
			if (remain_priv) {
				if (woal_cfg80211_remain_on_channel_cfg(
					    remain_priv, MOAL_NO_WAIT, MTRUE,
					    &channel_status, NULL, 0, 0))
					PRINTM(MERROR,
					       "remain_on_channel: Failed to cancel\n");

				priv->phandle->remain_on_channel = MFALSE;
			}
		}
#endif
		PRINTM(MEVENT, "Wlan: Tx status=%d\n", ack);
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		if (tx_info->tx_cookie) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#if CFG80211_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
			cfg80211_mgmt_tx_status(priv->netdev,
						tx_info->tx_cookie, skb->data,
						skb->len, ack, GFP_ATOMIC);
#else
			cfg80211_mgmt_tx_status(priv->wdev, tx_info->tx_cookie,
						skb->data, skb->len, ack,
						GFP_ATOMIC);
#endif
#endif
		}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
		if (drvdbg & MDAT_D)
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
}
#endif

/**
 * @brief   This function handles survey dump reset event
 *
 * @param priv       A pointer moal_private structure
 *
 * @return          N/A
 */
static void woal_survey_dump_reset_event(moal_private *priv)
{
	struct woal_event *evt;
	unsigned long flags;
	moal_handle *handle = priv->phandle;

	evt = kzalloc(sizeof(struct woal_event), GFP_ATOMIC);
	if (evt) {
		evt->priv = priv;
		evt->type = WOAL_EVENT_SURVEY_DUMP_RESET;
		INIT_LIST_HEAD(&evt->link);
		spin_lock_irqsave(&handle->evt_lock, flags);
		list_add_tail(&evt->link, &handle->evt_queue);
		spin_unlock_irqrestore(&handle->evt_lock, flags);
		queue_work(handle->evt_workqueue, &handle->evt_work);
	}
	// coverity[misra_c_2012_rule_22_1_violation:SUPPRESS]
	// coverity[leaked_storage]: SUPPRESS
}

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

#if defined(STA_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
#endif
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

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	t_u8 band_config = 0;
	t_u8 chan_num = 0;
#endif
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
#if defined(STA_SUPPORT)
	chan_band_reginfo_t *psta_reg_info = NULL;
#endif
	wifi_chan_avoid_list_t *pwifi_chan_info = NULL;
	char iwevent_str[256];
	addba_timeout_event *evtbuf = NULL;

	t_u8 auto_fw_dump = MFALSE;
	static int fw_reset_cnt = 0;
	t_u8 fw_reset_time = 0;
	ENTER();
	if (pmevent->event_id == MLAN_EVENT_ID_FW_DUMP_INFO) {
		if (!handle->is_fw_dump_timer_set) {
			PRINTM(MMSG, "FW trigger fw dump\n");
			handle->is_fw_dump_timer_set = MTRUE;
			woal_mod_timer(&handle->fw_dump_timer,
				       MOAL_FW_DUMP_TIMER);
		}
		handle->init_wait_q_woken = MTRUE;
		wake_up(&handle->init_wait_q);
		woal_store_firmware_dump(pmoal, pmevent);
		handle->driver_status = MTRUE;
		mlan_set_driver_status(handle->pmlan_adapter,
				       handle->driver_status);
		wifi_status = WIFI_STATUS_FW_DUMP;
		ref_handle = (moal_handle *)handle->pref_mac;
		if (ref_handle) {
			ref_handle->driver_status = MTRUE;
			mlan_set_driver_status(ref_handle->pmlan_adapter,
					       ref_handle->driver_status);
		}
		goto done;
	}
	if (MLAN_STATUS_SUCCESS ==
	    wlan_process_defer_event(handle, pmevent->event_id))
		goto done;
	if (pmevent->event_id != MLAN_EVENT_ID_DRV_MGMT_FRAME)
		PRINTM(MEVENT, "event id:0x%x\n", pmevent->event_id);
#if defined(PCIE)
	if (pmevent->event_id == MLAN_EVENT_ID_SSU_DUMP_FILE) {
		woal_store_ssu_dump(pmoal, pmevent);
		goto done;
	}
#endif /* SSU_SUPPORT */
	if (pmevent->event_id == MLAN_EVENT_ID_STORE_HOST_CMD_RESP) {
#ifndef DUMP_TO_PROC
		woal_save_host_cmdresp(handle, (mlan_cmdresp_event *)pmevent);
#endif
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

	case MLAN_EVENT_ID_DRV_CONNECTED:
#ifdef STA_WEXT
		if (IS_STA_WEXT(cfg80211_wext) &&
		    pmevent->event_len == ETH_ALEN) {
			memset(wrqu.ap_addr.sa_data, 0x00, ETH_ALEN);
			moal_memcpy_ext(priv->phandle, wrqu.ap_addr.sa_data,
					pmevent->event_buf, ETH_ALEN,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 80)
					sizeof(wrqu.ap_addr.sa_data_min));
#else
					sizeof(wrqu.ap_addr.sa_data));
#endif
			wrqu.ap_addr.sa_family = ARPHRD_ETHER;
			wireless_send_event(priv->netdev, SIOCGIWAP, &wrqu,
					    NULL);
		}
#endif
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext)) {
			moal_memcpy_ext(priv->phandle, priv->cfg_bssid,
					pmevent->event_buf, ETH_ALEN, ETH_ALEN);

			if (!priv->phandle->user_scan_cfg)
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
		woal_survey_dump_reset_event(priv);

		break;
	case MLAN_EVENT_ID_DRV_ASSOC_FAILURE:
		PRINTM(MERROR, "wlan:MLAN_EVENT_ASSOC_FAILURE\n");
#ifdef STA_CFG80211
		priv->cfg_disconnect = MTRUE;
#endif
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
				spin_lock_irqsave(&priv->phandle->scan_req_lock,
						  flags);
				if (priv->phandle->scan_request) {
					PRINTM(MINFO,
					       "Reporting scan results\n");
					woal_inform_bss_from_scan_result(
						priv, NULL, MOAL_NO_WAIT);
					if (!priv->phandle->first_scan_done) {
						priv->phandle->first_scan_done =
							MTRUE;
						if (!priv->phandle
							     ->user_scan_cfg)
							woal_set_scan_time(
								priv,
								ACTIVE_SCAN_CHAN_TIME,
								PASSIVE_SCAN_CHAN_TIME,
								SPECIFIC_SCAN_CHAN_TIME);
					}
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
				}
				spin_unlock_irqrestore(
					&priv->phandle->scan_req_lock, flags);
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
		if (priv->phandle->scan_pending_on_block == MTRUE) {
			priv->phandle->scan_pending_on_block = MFALSE;
			priv->phandle->scan_priv = NULL;
			MOAL_REL_SEMAPHORE(&priv->phandle->async_sem);
		}
		if (!is_zero_timeval(priv->phandle->scan_time_start)) {
			woal_get_monotonic_time(&priv->phandle->scan_time_end);
			priv->phandle->scan_time +=
				(t_u64)(timeval_to_usec(
						priv->phandle->scan_time_end) -
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

		moal_agcs_trans_state(priv, AGCS_STATE_SCAN_REPORT);
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
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		/* 6E Indoor/Outdoor, download the default power table
		 * after disconnect/link-loss */
		if ((priv->phandle->fw_bands & BAND_6G) &&
		    (priv->sme_current.channel) &&
		    (priv->sme_current.channel->band == NL80211_BAND_6GHZ)) {
			PRINTM(MEVENT, "Downloading default 6E table!!\n");
			if (MLAN_STATUS_SUCCESS !=
			    woal_dnld_default_6e_psd_table(priv))
				PRINTM(MERROR,
				       "Default 6E table download failed!!\n");
		}
#endif
		if (priv->media_connected)
			woal_send_disconnect_to_system(
				priv, (t_u16)*pmevent->event_buf);
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
		if (priv->sinfo)
			priv->sinfo->signal = *(t_s16 *)pmevent->event_buf;
		if (IS_STA_CFG80211(cfg80211_wext)) {
#if CFG80211_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
			cfg80211_cqm_rssi_notify(
				priv->netdev,
				NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
				*(t_s16 *)pmevent->event_buf,
#endif
				GFP_KERNEL);
			woal_set_rssi_threshold(priv,
						MLAN_EVENT_ID_FW_BCN_RSSI_LOW,
						MOAL_NO_WAIT);
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
		if (priv->sinfo)
			priv->sinfo->signal = *(t_s16 *)pmevent->event_buf;
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
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		priv->deauth_evt_cnt = 0;
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
/* sending becon_loss event will help supplicant to roam to other APs if
 * available on the same Network */
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
				cfg80211_cqm_beacon_loss_notify(priv->netdev,
								GFP_KERNEL);
#endif
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
	case MLAN_EVENT_ID_DRV_DBG_DUMP:
		priv->phandle->driver_status = MTRUE;
		mlan_set_driver_status(handle->pmlan_adapter,
				       handle->driver_status);
		ref_handle = (moal_handle *)priv->phandle->pref_mac;
		if (ref_handle) {
			ref_handle->driver_status = MTRUE;
			mlan_set_driver_status(ref_handle->pmlan_adapter,
					       ref_handle->driver_status);
		}
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
				__pm_wakeup_event(priv->phandle->ws,
						  ROAMING_WAKE_LOCK_TIMEOUT);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
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
				woal_sched_timeout(50);
				woal_bgscan_stop_event(priv);
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
				if (snprintf(event_buf, sizeof(event_buf) - 1,
					     "%s %d", CUS_EVT_RADAR_DETECTED,
					     radar_chan) <= 0)
					PRINTM(MERROR,
					       "Failed to write event name and chan num in event_buf\n");
				woal_broadcast_event(priv, event_buf,
						     strlen(event_buf));
			} else {
				if (snprintf(event_buf, sizeof(event_buf) - 1,
					     "%s %d", CUS_EVT_CAC_FINISHED,
					     priv->chan_rpt_req.chanNum) <= 0)
					PRINTM(MERROR,
					       "Failed to write event name and chan num in event_buf\n");
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
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL, 0);
#elif CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
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
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
					timeout =
						(priv->wdev->links[0]
							 .cac_start_time +
						 msecs_to_jiffies(
							 priv->wdev->links[0]
								 .cac_time_ms));
#else
					timeout =
						(priv->wdev->cac_start_time +
						 msecs_to_jiffies(
							 priv->wdev
								 ->cac_time_ms));
#endif
					// coverity violation raised for
					// kernel's API
					// coverity[misra_c_2012_rule_10_8_violation:SUPPRESS]
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

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_FINISHED,
						   GFP_KERNEL, 0);
#elif CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
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
		if (snprintf(event_buf, sizeof(event_buf) - 1, "%s %d",
			     CUS_EVT_RADAR_DETECTED, radar_chan) <= 0) {
			PRINTM(MERROR, "Failed to write RADAR detect event\n");
			break;
		}
		woal_broadcast_event(priv, event_buf, strlen(event_buf));
		PRINTM(MEVENT, "%s: Radar detected on channel %d\n",
		       priv->netdev->name, radar_chan);
#ifdef UAP_SUPPORT
		if (priv->chan_rpt_req.chanNum && priv->chan_rpt_pending) {
			woal_11h_cancel_chan_report_ioctl(priv, MOAL_NO_WAIT);
			priv->chan_rpt_pending = MFALSE;
		}
#endif

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
		if (priv->bss_started &&
		    ((priv->target_chan && (priv->target_chan != radar_chan)) ||
		     priv->backup_chan)) {
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
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
				cfg80211_cac_event(priv->netdev,
						   &priv->phandle->dfs_channel,
						   NL80211_RADAR_CAC_ABORTED,
						   GFP_KERNEL, 0);
#elif CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
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
		if (snprintf(event_buf, sizeof(event_buf) - 1, "%s %d",
			     CUS_EVT_CHAN_SWITCH_COMPLETE,
			     pchan_info->channel) <= 0)
			PRINTM(MERROR,
			       "Failed to write switched chan num in event buf\n");
		woal_broadcast_event(priv, event_buf, strlen(event_buf));
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
			/* Block host event only for same channel, same
			 * bandwidth case */
			if ((priv->channel == pchan_info->channel) &&
			    (priv->bandwidth == pchan_info->bandcfg.chanWidth))
				break;
			PRINTM(MMSG, "OLD BW = %d NEW BW = %d", priv->bandwidth,
			       pchan_info->bandcfg.chanWidth);
			priv->channel = pchan_info->channel;
			priv->bandwidth = pchan_info->bandcfg.chanWidth;

#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
			if (MFALSE
#ifdef UAP_CFG80211
			    || priv->uap_host_based
#endif
#ifdef STA_CFG80211
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) ||                    \
     (defined(ANDROID_SDK_VERSION) && ANDROID_SDK_VERSION >= 31))
			    || priv->wdev->connected
#else
			    || priv->wdev->current_bss
#endif
#endif
			) {
				PRINTM(MMSG,
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
#ifdef UAP_SUPPORT
		moal_agcs_trans_state(priv, AGCS_STATE_COMPLETE);
#endif /* UAP_SUPPORT */
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
			if (woal_get_pm_info(priv, &pm_info))
				PRINTM(MEVENT, "get pm info failed\n");
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
		if (IS_STA_CFG80211(cfg80211_wext) && pmpriv) {
			if (!priv->phandle->user_scan_cfg) {
				woal_set_scan_time(pmpriv,
						   ACTIVE_SCAN_CHAN_TIME,
						   PASSIVE_SCAN_CHAN_TIME,
						   MIN_SPECIFIC_SCAN_CHAN_TIME);
			}
		}
#endif
#endif
#ifdef UAP_CFG80211
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
		if (priv->chan_under_nop) {
			PRINTM(MMSG,
			       "Channel Under Nop: notify cfg80211 new channel=%d\n",
			       priv->channel);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 3, 0) &&                        \
	CFG80211_VERSION_CODE < KERNEL_VERSION(6, 9, 0)
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan, 0,
						  0);
#elif ((CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 1, 0) &&                    \
	(defined(ANDROID_SDK_VERSION) && ANDROID_SDK_VERSION >= 33))) &&       \
	CFG80211_VERSION_CODE < KERNEL_VERSION(6, 9, 0)
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan, 0,
						  0);
#elif ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) ||                  \
       (defined(ANDROID_SDK_VERSION) && ANDROID_SDK_VERSION >= 31))
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan, 0);
#else
			cfg80211_ch_switch_notify(priv->netdev, &priv->chan);
#endif
			priv->chan_under_nop = MFALSE;
		}
#endif
		woal_survey_dump_reset_event(priv);
#endif
		break;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
	case MLAN_EVENT_ID_DRV_UAP_CHAN_INFO:
#ifdef UAP_CFG80211
		if (IS_UAP_CFG80211(cfg80211_wext)) {
			struct cfg80211_chan_def chandef;
			pchan_info = (chan_band_info *)pmevent->event_buf;
			if (woal_chandef_create(priv, &chandef, pchan_info))
				PRINTM(MERROR,
				       "Failed to create cfg80211_chan_def structure\n");
			PRINTM(MMSG,
			       "UAP: 11n=%d, chan=%d, center_chan=%d, band=%d, width=%d, 2Offset=%d\n",
			       pchan_info->is_11n_enabled, pchan_info->channel,
			       pchan_info->center_chan,
			       pchan_info->bandcfg.chanBand,
			       pchan_info->bandcfg.chanWidth,
			       pchan_info->bandcfg.chan2Offset);
			if (priv->uap_host_based &&
			    ((priv->chan.chan->hw_value !=
			      pchan_info->channel) ||
			     (priv->chan.width != chandef.width)))
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
		if ((drvdbg & MBIT(11)) &&
		    priv->plinkstats_cfg.enable == MTRUE) {
			handle->is_plinkstats_timer_set = MTRUE;
			woal_mod_timer(&handle->plinkstats_timer,
				       priv->plinkstats_cfg.interval *
					       MOAL_TIMER_1S);
		}
		break;
	case MLAN_EVENT_ID_UAP_FW_BSS_IDLE:
		priv->media_connected = MFALSE;
		woal_broadcast_event(priv, pmevent->event_buf,
				     pmevent->event_len);
		moal_connection_status_check_pmqos(pmoal);
		if (handle->is_plinkstats_timer_set) {
			woal_cancel_timer(&handle->plinkstats_timer);
			handle->is_plinkstats_timer_set = MFALSE;
		}
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
				// Consider it is fallback, if media is still
				// connected.
				if (priv->phandle->params.make_before_break &&
				    priv->media_connected) {
					remain_on_channel_info *roc_info =
						(remain_on_channel_info *)
							pmevent->event_buf;
					if (!roc_info->delay_link_lost) {
						PRINTM(MMSG,
						       "HostMlme: media connected, fallback to previous AP.\n");
						priv->host_mlme = MTRUE;
						woal_request_set_host_mlme(
							priv, priv->cfg_bssid);
					}
				}
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
			     strlen(CUS_EVT_STA_CONNECTED) + 1) > 256)
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
				if (reason_code & MBIT(14)) {
					if (reason_code & MBIT(1)) {
						reason_code =
							MLAN_REASON_DISASSOC_DUE_TO_INACTIVITY;
						woal_host_mlme_disconnect(
							priv, reason_code,
							pmevent->event_buf + 2);
					} else
						woal_host_mlme_disconnect(
							priv, 0,
							pmevent->event_buf + 2);
				}
			} else
#endif
				if (priv->netdev && priv->wdev)
				cfg80211_del_sta(priv->netdev,
						 pmevent->event_buf + 2,
						 GFP_KERNEL);

#endif /* KERNEL_VERSION */
			priv->plinkstats.num_evt_deauth_rx++;
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
		/** We use the event_id field to pass the band_config and
		 * chan_num */
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		band_config = pmevent->event_buf[0];
		chan_num = pmevent->event_buf[1];
#endif
		moal_memcpy_ext(priv->phandle, (t_u8 *)pmevent->event_buf,
				(t_u8 *)&pmevent->event_id,
				sizeof(pmevent->event_id),
				sizeof(pmevent->event_id));
#ifdef UAP_WEXT
		if (IS_UAP_WEXT(cfg80211_wext)) {
			woal_broadcast_event(priv, pmevent->event_buf,
					     pmevent->event_len);
		}
#endif /* UAP_WEXT */

#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		if (IS_STA_OR_UAP_CFG80211(cfg80211_wext)) {
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
			if (priv->netdev && priv->netdev->ieee80211_ptr &&
			    priv->netdev->ieee80211_ptr->wiphy->mgmt_stypes &&
			    priv->mgmt_subtype_mask) {
				/* frmctl + durationid + addr1 + addr2 + addr3 +
				 * seqctl */
#define PACKET_ADDR4_POS (2 + 2 + 6 + 6 + 6 + 2)
				t_u8 *pkt;
				int freq = woal_get_rx_freq(priv, band_config,
							    chan_num);
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
						priv->auth_tx_cnt = 0;
						PRINTM(MEVENT,
						       "HostMlme %s: Received auth frame type = 0x%x auth_mgmt_tx:%d\n",
						       priv->netdev->name,
						       priv->auth_alg,
						       priv->auth_mgmt_tx);

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

								if (priv->auth_mgmt_tx &&
								    (((struct ieee80211_mgmt
									       *)
									      pkt)
									     ->u
									     .auth
									     .auth_alg ==
								     WLAN_AUTH_FT)) {
									priv->auth_flag =
										0;
									priv->auth_mgmt_tx =
										0;
									woal_mgmt_frame_register(
										priv,
										IEEE80211_STYPE_AUTH,
										MFALSE);
									/* Need
									 * to
									 * indicate
									 * using
									 * cfg80211_rx_mgmt
									 */
									// coverity[misra_c_2012_rule_15_3_violation:SUPPRESS]
									goto rx_mgmt;
								}

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
						priv->plinkstats
							.num_evt_deauth_rx++;
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) ||                    \
     (defined(ANDROID_SDK_VERSION) && ANDROID_SDK_VERSION >= 31))
						if (!priv->wdev->connected) {
#else
						if (!priv->wdev->current_bss) {
#endif
							PRINTM(MEVENT,
							       "HostMlme: Drop deauth/disassociate, current_bss = null\n");
							break;
						}

						if (ieee80211_is_deauth(
							    ((struct ieee80211_mgmt
								      *)pkt)
								    ->frame_control)) {
							/* subtype 12 deauth
							 * packet */
							priv->deauth_evt_cnt++;
							if (handle->params
								    .wifi_reset_config &&
							    priv->deauth_evt_cnt >
								    handle->params
									    .wifi_reset_config) {
								woal_wifi_reset_event(
									priv,
									priv->deauth_evt_cnt);
								priv->deauth_evt_cnt =
									0;
							}
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
				rx_mgmt:
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
				// forward mgmt packet to kernel without (drvdbg
				// & MDAT_D) because
				//  (few) userspace process needs this for
				//  processing disassoc/deauth event with reason
				//  code
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
		PRINTM(MEVENT,
		       "Wlan: Tx status: tx_token=%d, pkt_type=0x%x, status=%d priv->tx_seq_num=%d\n",
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
					if (woal_cfg80211_remain_on_channel_cfg(
						    remain_priv, MOAL_NO_WAIT,
						    MTRUE, &channel_status,
						    NULL, 0, 0))
						PRINTM(MERROR,
						       "remain_on_channel: Failed to cancel\n");

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
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 3, 0)
				if (tx_info->send_tx_expired) {
					PRINTM(MINFO,
					       "NAN: send tx duration expired for cookie=%llx\n",
					       tx_info->tx_cookie);
					cfg80211_tx_mgmt_expired(
						priv->wdev, tx_info->tx_cookie,
						&priv->phandle->chan,
						GFP_ATOMIC);
				}
#endif
#endif
			}
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
			if (drvdbg & MDAT_D)
				woal_packet_fate_monitor(
					priv, PACKET_TYPE_TX,
					ack ? TX_PKT_FATE_ACKED :
					      TX_PKT_FATE_SENT,
					FRAME_TYPE_80211_MGMT, 0, 0, skb->data,
					skb->len);
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
	case MLAN_EVENT_ID_FW_TX_BULK_STATUS: {
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		tx_bulk_status_event *tx_status =
			(tx_bulk_status_event *)(pmevent->event_buf + 4);
		t_u8 curID = 0, num_of_events = 0, payload_len = 0;

		payload_len = (pmevent->event_len - 4);
		num_of_events =
			((payload_len) / (sizeof(tx_mgmt_status_event)));

		PRINTM(MEVENT,
		       "Wlan: Bulk Tx status total_len=%d num_of_events=%d\n",
		       payload_len, num_of_events);
		while (curID < num_of_events) {
			woal_process_event_tx_status(
				priv, &(tx_status->bulk_events[curID++]));
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
#if ((CFG80211_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)) ||                     \
     (defined(ANDROID_SDK_VERSION) && ANDROID_SDK_VERSION >= 31))
			roam_info->links[0].bssid = priv->cfg_bssid;
#else
			roam_info->bssid = priv->cfg_bssid;
#endif
			roam_info->req_ie = req_ie;
			roam_info->req_ie_len = ie_len;
			roam_info->resp_ie = pinfo->rsp_ie;
			roam_info->resp_ie_len = pinfo->header.len;
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(5, 19, 0)
			if (priv->wdev->u.client.ssid_len)
#else
			if (priv->wdev->ssid_len)
#endif
				cfg80211_roamed(priv->netdev, roam_info,
						GFP_KERNEL);
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
	case MLAN_EVENT_ID_DRV_RTT_RESULT:
		DBG_HEXDUMP(MEVT_D, "RTT result", pmevent->event_buf,
			    pmevent->event_len);
#if CFG80211_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
#ifdef STA_CFG80211
		if (IS_STA_CFG80211(cfg80211_wext))
			woal_cfg80211_event_rtt_result(priv, pmevent->event_buf,
						       pmevent->event_len);
#endif
#endif
		break;
	case MLAN_EVENT_ID_DRV_ADDBA_TIMEOUT:
		evtbuf = (addba_timeout_event *)(pmevent->event_buf);
		DBG_HEXDUMP(MEVT_D, "ADDBA_TIMEOUT", pmevent->event_buf,
			    pmevent->event_len);
		memset(iwevent_str, 0, sizeof(iwevent_str));
		snprintf(iwevent_str, sizeof(iwevent_str),
			 "%s, MAC:%02x%02x%02x%02x%02x%02x,TID=%u",
			 CUS_EVT_ADDBA_TIMEOUT, evtbuf->peer_mac_addr[0],
			 evtbuf->peer_mac_addr[1], evtbuf->peer_mac_addr[2],
			 evtbuf->peer_mac_addr[3], evtbuf->peer_mac_addr[4],
			 evtbuf->peer_mac_addr[5], evtbuf->tid);
#if defined(STA_SUPPORT) || defined(UAP_SUPPORT)
#if defined(STA_WEXT) || defined(UAP_WEXT)
		if (IS_STA_OR_UAP_WEXT(cfg80211_wext)) {
			woal_send_iwevcustom_event(priv, iwevent_str);
		}
#endif
#endif
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
	case MLAN_EVENT_ID_CSI_STATUS:
		woal_process_csi_status_report(pmevent);
		break;
	case MLAN_EVENT_ID_DRV_RGPWR_KEY_MISMATCH:
		if (handle->sec_rgpower)
			woal_rgpower_key_mismatch_event(priv);
		break;
	case MLAN_EVENT_ID_FW_CHAN_SWITCH_REGINFO:
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
		psta_reg_info = (chan_band_reginfo_t *)pmevent->event_buf;
		if (psta_reg_info->bandcfg.chanBand == BAND_6GHZ) {
			PRINTM(MEVENT,
			       "CSA/ECSA: STA switch to new channel=%d band=%d regInfo=%d!\n",
			       psta_reg_info->channel,
			       psta_reg_info->bandcfg.chanBand,
			       psta_reg_info->regInfo);
			woal_dnld_sta_6e_psd_table(priv, NULL, 0,
						   psta_reg_info);
		} else
			PRINTM(MEVENT,
			       "Ignoring the Channel Switch Reg Info Event\n");
#endif
		break;
	case MLAN_EVENT_ID_EMERGENCY_TEMP_REACHED:
		fw_reset_time = (t_u8)pmevent->event_buf[4];
		if (fw_reset_time <= 0)
			fw_reset_time = 60;
		fw_reset_cnt++;
		PRINTM(MEVENT,
		       "EMERGENCY TEMPRETURE REACHED: %d times...Wait for %d sec to cool down radio!!\n",
		       fw_reset_cnt, fw_reset_time);

		queue_delayed_work(priv->phandle->evt_workqueue,
				   &priv->phandle->emergency_reset_work,
				   msecs_to_jiffies(fw_reset_time * 1000));

		break;
#ifdef UAP_SUPPORT
	case MLAN_EVENT_ID_FW_AGCS_TRIGGER: {
		woal_agcs_event(priv, (pagcs_event)pmevent->event_buf);
	} break;
#endif /* UAP_SUPPORT */
	case MLAN_EVENT_ID_FW_WIFI_CHANNEL_AVOID_LIST:
		pwifi_chan_info =
			(wifi_chan_avoid_list_t *)(pmevent->event_buf +
						   sizeof(MrvlIEtypesHeader_t) +
						   4);
		process_wifi_channel_avoid_list_event(priv, pwifi_chan_info);
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
		// Coverity violation raised for kernel's API
		// coverity[cert_arr39_c_violation:SUPPRESS]
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
#if 0
/**
 *  @brief This function records host time in nano seconds
 *
 *  @return                 64 bit value of host time in nano seconds
 */
s64 get_host_time_ns(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	struct timespec64 ts;
#else
	struct timespec ts;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	ktime_get_real_ts64(&ts);
	return timespec64_to_ns(&ts);
#else
	getnstimeofday(&ts);
	return timespec_to_ns(&ts);
#endif
}
#endif

/**
 *  @brief Retrieves the current system time
 *
 *  @param time     Pointer for the seconds of system time
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status moal_get_host_time_ns(t_u64 *time)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	struct timespec64 ts;
#else
	struct timespec ts;
#endif
	t_u64 hclk_val;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	ktime_get_real_ts64(&ts);
#else
	getnstimeofday(&ts);
#endif
	hclk_val = (ts.tv_sec * 1000000000L) + ts.tv_nsec;
	*time = hclk_val;
	return MLAN_STATUS_SUCCESS;
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

mlan_status moal_calc_short_ssid(t_u8 *pssid, t_u32 ssid_len,
				 t_u32 *pshort_ssid)
{
	if (!pssid || !ssid_len) {
		PRINTM(MWARN, "%s: incorrect SSID\n", __func__);
		return MLAN_STATUS_FAILURE;
	}
	*pshort_ssid = ~crc32_le(~0, pssid, ssid_len);
	return MLAN_STATUS_SUCCESS;
}

inline t_u16 moal_read_unaligned_u16(const void *src)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
	return *((const t_u16 *)src);
#else
	t_u16 val = 0;
	memcpy(&val, src, sizeof(t_u16));
	return val;
#endif
}

inline t_u32 moal_read_unaligned_u32(const void *src)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
	return *((const t_u32 *)src);
#else
	t_u32 val = 0;
	memcpy(&val, src, sizeof(t_u32));
	return val;
#endif
}

inline void moal_write_unaligned_u16(void *dest, t_u16 val)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
	*((t_u16 *)dest) = val;
#else
	memcpy(dest, &val, sizeof(t_u16));
#endif
}

inline void moal_write_unaligned_u32(void *dest, t_u32 val)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
	*((t_u32 *)dest) = val;
#else
	memcpy(dest, &val, sizeof(t_u32));
#endif
}

/**
 *  @brief This function generates crc through kernek API
 *
 *  @param initial_crc     Starting crc
 *  @param data            A pointer to input data buffer
 *  @param len             Length of the buffer
 *
 *  @return                crc value
 */
t_u32 moal_crc32_be(t_u32 initial_crc, t_u8 const *data, unsigned long len)
{
	return crc32_be(initial_crc, data, len);
}
