/** @file moal_sdio.h
 *
 * @brief This file contains definitions for SDIO interface.
 * driver.
 *
 *
 * Copyright 2008-2022, 2024-2025 NXP
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
/****************************************************
Change log:
****************************************************/

#ifndef _MOAL_SDIO_H
#define _MOAL_SDIO_H

#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "moal_main.h"

#ifndef BLOCK_MODE
/** Block mode */
#define BLOCK_MODE 1
#endif

#ifndef BYTE_MODE
/** Byte Mode */
#define BYTE_MODE 0
#endif

/** SDIO bus width 1 bit mode */
#define SDIO_BUS_WIDTH_1 1
/** SDIO bus width 4 bit mode */
#define SDIO_BUS_WIDTH_4 4

#ifndef FIXED_ADDRESS
/** Fixed address mode */
#define FIXED_ADDRESS 0
#endif

/** SDIO register card control 3 */
#define SD_CARD_CTRL3 0xF3

/** Default firmware name */

#define SD8997_DEFAULT_COMBO_FW_NAME "nxp/sduart8997_combo_v4.bin"
#define SDUART8997_DEFAULT_COMBO_FW_NAME "nxp/sduart8997_combo_v4.bin"
#define SDSD8997_DEFAULT_COMBO_FW_NAME "nxp/sdsd8997_combo_v4.bin"
#define SD8997_DEFAULT_WLAN_FW_NAME "nxp/sd8997_wlan_v4.bin"

/********************************************************
		Global Functions
********************************************************/

/** Register to bus driver function */
mlan_status woal_sdiommc_bus_register(void);
/** Unregister from bus driver function */
void woal_sdiommc_bus_unregister(void);

int woal_sdio_set_bus_clock(moal_handle *handle, t_u8 option);
int woal_sdio_set_buswidth(moal_handle *handle, t_u8 bus_width);

#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_FUNC_SUSPENDED
/** Notify SDIO bus driver that WLAN is suspended */
void woal_wlan_is_suspended(moal_handle *handle);
#endif
/** SDIO Suspend */
int woal_sdio_suspend(struct device *dev);
/** SDIO Resume */
int woal_sdio_resume(struct device *dev);
#endif /* SDIO_SUSPEND_RESUME */

/** Structure: SDIO MMC card */
typedef struct _sdio_mmc_card {
	/** sdio_func structure pointer */
	struct sdio_func *func;
	/** moal_handle structure pointer */
	moal_handle *handle;
	/** reset work*/
	struct work_struct reset_work;
	/** work flag */
	t_u8 work_flags;
	/** saved host clock value */
	unsigned int host_clock;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
	/** oob irq */
	int oob_irq;
	/** irq enabled */
	int irq_enabled;
	/** sdio func intr enabled **/
	int sdio_func_intr_enabled;
	/** irq registered */
	int irq_registered;
	/* SDIO OOB Interrupt handling workqueue */
	struct workqueue_struct *sdio_oob_irq_workqueue;
	/* SDIO OOB Interrupt handler work */
	struct work_struct sdio_oob_irq_work;
#endif
} sdio_mmc_card;
void woal_sdio_reset_hw(moal_handle *handle);

/** cmd52 read write */
int woal_sdio_read_write_cmd52(moal_handle *handle, int func, int reg, int val);
#endif /* _MOAL_SDIO_H */
