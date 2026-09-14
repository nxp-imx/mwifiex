/* SPDX-License-Identifier: GPL-2.0 */
/** @file moal_pcie.h
 *
 *  @brief This file contains definitions for PCIE interface.
 *  driver.
 *
 *
 * Copyright 2008-2026 NXP
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
 *
 */

/********************************************************
 * Change log:
 * 02/01/2012: initial version
 ********************************************************
 */

#ifndef _MOAL_PCIE_H_
#define _MOAL_PCIE_H_

#define PCIE_VENDOR_ID_MRVL (0x11ab)
#define PCIE_VENDOR_ID_V2_MRVL (0x1b4b)
#define PCIE_VENDOR_ID_NXP (0x1131)

#ifdef PCIE8897
/** PCIE device ID for 8897 card */
#define PCIE_DEVICE_ID_88W8897P (0x2b38)
#endif

#ifdef PCIE9097
/** PCIE device ID for 9097 card */
#define PCIE_DEVICE_ID_88W9097 (0x2b56)
#endif

#if defined(PCIE9098) || defined(PCIEAW693)
/** PCIE device ID for 9098 card FN0 */
#define PCIE_DEVICE_ID_88W9098P_FN0 (0x2b43)
/** PCIE device ID for 9098 card FN1 */
#define PCIE_DEVICE_ID_88W9098P_FN1 (0x2b44)
#endif

#ifdef PCIEIW624
/** PCIE device ID for IW624 card FN0 */
#define PCIE_DEVICE_ID_88WIW624 (0x3000)
#endif

#if defined(PCIE9098) || defined(PCIEAW693)
/** PCIE device ID for AW693 card FN0 */
#define PCIE_DEVICE_ID_88WAW693_FN0 (0x3003)
/** PCIE device ID for AW693 card FN1 */
#define PCIE_DEVICE_ID_88WAW693_FN1 (0x3004)
#endif

#include <linux/version.h>
#include <linux/pci.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)
#include <linux/pcieport_if.h>
#endif
#include <linux/interrupt.h>

#include "moal_main.h"

/** Default firmware name */

#ifdef PCIE8897
#define PCIE8897_DEFAULT_COMBO_FW_NAME "nxp/pcieuart8897_combo.bin"
#define PCIE8897_DEFAULT_WLAN_FW_NAME "nxp/pcie8897_wlan.bin"
#endif /* PCIE8897*/

#ifdef PCIEAW693
#define PCIEUARTAW693_DEFAULT_COMBO_FW_NAME "nxp/pcieuartaw693_combo.bin"
#define PCIEAW693_DEFAULT_COMBO_FW_NAME "nxp/pcieuartaw693_combo.bin"
#define PCIEUARTAW693_COMBO_V1_FW_NAME "nxp/pcieuartaw693_combo_v1.bin.se"
#define PCIEAW693_COMBO_V1_FW_NAME "nxp/pcieuartaw693_combo_v1.bin.se"
#define PCIEAW693_DEFAULT_WLAN_FW_NAME "nxp/pcieaw693_wlan.bin"
#define PCIEAW693_WLAN_V1_FW_NAME "nxp/pcieaw693_wlan_v1.bin.se"
#define PCIEAW693_A0 0x00
#define PCIEAW693_A1 0x01
#endif /* PCIEAW693*/

#ifdef PCIE9098
#define PCIE9098_Z1Z2 0x00
#define PCIE9098_A0 0x01
#define PCIE9098_A1 0x02
#define PCIE9098_A2 0x03
#define PCIE9098_DEFAULT_COMBO_FW_NAME "nxp/pcieuart9098_combo.bin"
#define PCIEUART9098_DEFAULT_COMBO_FW_NAME "nxp/pcieuart9098_combo.bin"
#define PCIEUSB9098_DEFAULT_COMBO_FW_NAME "nxp/pcieusb9098_combo.bin"
#define PCIEPCIE9098_DEFAULT_COMBO_FW_NAME "nxp/pciepcie9098_combo.bin"
#define PCIEUART9098_COMBO_V1_FW_NAME "nxp/pcieuart9098_combo_v1.bin"
#define PCIEUSB9098_COMBO_V1_FW_NAME "nxp/pcieusb9098_combo_v1.bin"
#define PCIEPCIE9098_COMBO_V1_FW_NAME "nxp/pciepcie9098_combo_v1.bin"
#define PCIE9098_DEFAULT_WLAN_FW_NAME "nxp/pcie9098_wlan.bin"
#define PCIE9098_WLAN_V1_FW_NAME "nxp/pcie9098_wlan_v1.bin"
#endif /* PCIE9098 */

#ifdef PCIE9097
#define PCIE9097_A0 0x00
#define PCIE9097_B0 0x01
#define PCIE9097_B1 0x02
#define PCIE9097_DEFAULT_COMBO_FW_NAME "nxp/pcieuartiw620_combo.bin"
#define PCIEUART9097_DEFAULT_COMBO_FW_NAME "nxp/pcieuartiw620_combo.bin"
#define PCIEUSB9097_DEFAULT_COMBO_FW_NAME "nxp/pcieusbiw620_combo.bin"
#define PCIEUART9097_COMBO_V1_FW_NAME "nxp/pcieuartiw620_combo_v1.bin"
#define PCIEUSB9097_COMBO_V1_FW_NAME "nxp/pcieusbiw620_combo_v1.bin"
#define PCIE9097_DEFAULT_WLAN_FW_NAME "nxp/pcieiw620_wlan.bin"
#define PCIE9097_WLAN_V1_FW_NAME "nxp/pcieiw620_wlan_v1.bin"
#endif /* PCIE9097 */

#ifdef PCIEIW624
#define PCIEIW624_DEFAULT_COMBO_FW_NAME "nxp/pcieuartiw624_combo.bin"
#define PCIEUARTIW624_DEFAULT_COMBO_FW_NAME "nxp/pcieuartiw624_combo.bin"
#define PCIEUSBIW624_DEFAULT_COMBO_FW_NAME "nxp/pcieusbiw624_combo.bin"
#define PCIEUARTUARTIW624_DEFAULT_COMBO_FW_NAME                                \
	"nxp/pcieuartuartiw624_combo.bin"
#define PCIEUARTSPIIW624_DEFAULT_COMBO_FW_NAME "nxp/pcieuartspiiw624_combo.bin"
#define PCIEUSBUSBIW624_DEFAULT_COMBO_FW_NAME "nxp/pcieusbusbiw624_combo.bin"
#define PCIEIW624_DEFAULT_WLAN_FW_NAME "nxp/pcieiw624_wlan.bin"
#endif /* PCIEIW624 */

/** Structure: PCIE service card */
typedef struct _pcie_service_card {
	/** pci_dev structure pointer */
	struct pci_dev *dev;
	/** moal_handle structure pointer */
	moal_handle *handle;
	/** reset work*/
	struct work_struct reset_work;
	/** work flag */
	t_u8 work_flags;
	/** I/O memory regions pointer to the bus */
	void __iomem *pci_mmap;
	/** I/O memory regions pointer to the bus */
	void __iomem *pci_mmap1;
	t_u32 cache_alignment_mask;
} pcie_service_card, *ppcie_service_card;

/** Register to bus driver function */
mlan_status woal_pcie_bus_register(void);
/** Unregister from bus driver function */
void woal_pcie_bus_unregister(void);

#endif /* _MOAL_PCIE_H_ */
