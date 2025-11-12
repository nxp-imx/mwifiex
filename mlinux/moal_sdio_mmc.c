/** @file moal_sdio_mmc.c
 *
 *  @brief This file contains SDIO MMC IF (interface) module
 *  related functions.
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
	02/25/09: Initial creation -
		  This file supports SDIO MMC only
****************************************************/

#include <linux/firmware.h>
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
#include "moal_cfg80211.h"
#endif
#include "moal_sdio.h"
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
#include <net/addrconf.h>
#endif
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
#include <linux/gpio.h>
#include <uapi/linux/sched/types.h>
#endif

/** define nxp vendor id */
#define NXP_VENDOR_ID 0x0471
#define MRVL_VENDOR_ID 0x02df
/* The macros below are hardware platform dependent.
   The definition should match the actual platform */
/** Initialize GPIO port */
#define GPIO_PORT_INIT()
/** Set GPIO port to high */
#define GPIO_PORT_TO_HIGH()
/** Set GPIO port to low */
#define GPIO_PORT_TO_LOW()

/********************************************************
		Local Variables
********************************************************/
/* moal interface ops */
static moal_if_ops sdiommc_ops;
/********************************************************
		Global Variables
********************************************************/

#ifdef SD8887
/** Device ID for SD8887 */
#define SD_DEVICE_ID_8887 (0x9135)
#endif
#ifdef SD8897
/** Device ID for SD8897 */
#define SD_DEVICE_ID_8897 (0x912d)
#endif
#ifdef SD8977
/** Device ID for SD8977 */
#define SD_DEVICE_ID_8977 (0x9145)
#endif
#ifdef SD8978
/** Device ID for SD8978 */
#define SD_DEVICE_ID_8978 (0x9159)
#endif
#ifdef SD8997
/** Device ID for SD8997 */
#define SD_DEVICE_ID_8997 (0x9141)
#endif
#ifdef SD8987
/** Device ID for SD8987 */
#define SD_DEVICE_ID_8987 (0x9149)
#endif
#ifdef SDAW693
/** Device ID for SDAW693 */
#define SD_DEVICE_ID_AW693_FN1 (0x0211)
/** Device ID for SDAW693 */
#define SD_DEVICE_ID_AW693_FN2 (0x0212)
#endif
#ifdef SD9098
/** Device ID for SD9098 */
#define SD_DEVICE_ID_9098_FN1 (0x914D)
/** Device ID for SD9098 */
#define SD_DEVICE_ID_9098_FN2 (0x914E)
#endif
#ifdef SD9097
/** Device ID for SD9097 */
#define SD_DEVICE_ID_9097 (0x9155)
#endif
#ifdef SD9177
/** Device ID for SD9177 */
#define SD_DEVICE_ID_9177 (0x0205)
#endif
#ifdef SDIW624
/** Device ID for SDIW624 */
#define SD_DEVICE_ID_IW624 (0x020D)
#endif
#ifdef SDIW610
/** Device ID for SDIW610 */
#define SD_DEVICE_ID_IW610 (0x0215)
#endif

/** WLAN IDs */
static const struct sdio_device_id wlan_ids[] = {
#ifdef SD8887
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8887)},
#endif
#ifdef SD8897
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8897)},
#endif
#ifdef SD8977
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8977)},
#endif
#ifdef SD8978
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8978)},
#endif
#ifdef SD8997
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8997)},
#endif
#ifdef SD8987
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_8987)},
#endif
#ifdef SDAW693
	{SDIO_DEVICE(NXP_VENDOR_ID, SD_DEVICE_ID_AW693_FN1)},
	{SDIO_DEVICE(NXP_VENDOR_ID, SD_DEVICE_ID_AW693_FN2)},
#endif
#ifdef SD9098
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_9098_FN1)},
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_9098_FN2)},
#endif
#ifdef SD9097
	{SDIO_DEVICE(MRVL_VENDOR_ID, SD_DEVICE_ID_9097)},
#endif
#ifdef SD9177
	{SDIO_DEVICE(NXP_VENDOR_ID, SD_DEVICE_ID_9177)},
#endif
#ifdef SDIW624
	{SDIO_DEVICE(NXP_VENDOR_ID, SD_DEVICE_ID_IW624)},
#endif
#ifdef SDIW610
	{SDIO_DEVICE(NXP_VENDOR_ID, SD_DEVICE_ID_IW610)},
#endif
	{},
};


int woal_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id);
void woal_sdio_remove(struct sdio_func *func);
static void woal_sdiommc_work(struct work_struct *work);
#ifdef SDIO
static void woal_sdiommc_reg_dbg(pmoal_handle handle);
#endif

#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
int woal_sdio_suspend(struct device *dev);
int woal_sdio_resume(struct device *dev);

static struct dev_pm_ops wlan_sdio_pm_ops = {
	.suspend = woal_sdio_suspend,
	.resume = woal_sdio_resume,
};

void woal_sdio_shutdown(struct device *dev);
#endif
#endif

// clang-format off
static struct sdio_driver REFDATA wlan_sdio = {
	.name = "wlan_sdio",
	.id_table = wlan_ids,
	.probe = woal_sdio_probe,
	.remove = woal_sdio_remove,
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 29)
	.drv = {
		.owner = THIS_MODULE,
#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
		.pm = &wlan_sdio_pm_ops,
		.shutdown = woal_sdio_shutdown,
#endif
#endif

	}
#else
#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
	.drv = {
		.pm = &wlan_sdio_pm_ops,
		.shutdown = woal_sdio_shutdown,
	}
#endif
#endif
#endif
};
// clang-format on

/********************************************************
		Local Functions
********************************************************/
static void woal_sdiommc_dump_fw_info(moal_handle *phandle);
static void woal_trigger_nmi_on_no_dump_event(moal_handle *phandle);
#if 0
/**  @brief This function dump the sdio register
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @return         N/A
 */
static void woal_dump_sdio_reg(moal_handle *handle)
{
	int ret = 0;
	t_u8 data, i;
	int fun0_reg[] = {0x05, 0x04};
	t_u8 array_size = 0;
#ifdef SD8897
	int fun1_reg_8897[] = {0x03, 0x04, 0x05, 0x06, 0x07, 0xC0, 0xC1};
#endif
	int fun1_reg_other[] = {0x03, 0x04, 0x05, 0x60, 0x61};
	int *fun1_reg = NULL;

	for (i = 0; i < ARRAY_SIZE(fun0_reg); i++) {
		data = sdio_f0_readb(
			((sdio_mmc_card *)handle->card)->func,
			fun0_reg[i], &ret);
		PRINTM(MMSG, "fun0: reg 0x%02x=0x%02x ret=%d\n", fun0_reg[i],
		       data, ret);
	}

#ifdef SD8897
	if (IS_SD8897(handle->card_type)) {
		fun1_reg = fun1_reg_8897;
		array_size = sizeof(fun1_reg_8897) / sizeof(int);
	} else {
#endif
		fun1_reg = fun1_reg_other;
		array_size = sizeof(fun1_reg_other) / sizeof(int);
#ifdef SD8897
	}
#endif
	for (i = 0; i < array_size; i++) {
		data = sdio_readb(((sdio_mmc_card *)handle->card)->func,
				  fun1_reg[i], &ret);
		PRINTM(MMSG, "fun1: reg 0x%02x=0x%02x ret=%d\n", fun1_reg[i],
		       data, ret);
	}
	return;
}
#endif
/********************************************************
		Global Functions
********************************************************/
/**
 *  @brief This function handles the interrupt.
 *
 *  @param func     A pointer to the sdio_func structure
 *  @return         N/A
 */
static void woal_sdio_interrupt(struct sdio_func *func)
{
	moal_handle *handle;
	sdio_mmc_card *card;
	mlan_status status;
	t_u32 host_int_status_reg_val;
	ENTER();

	card = sdio_get_drvdata(func);
	if (!card || !card->handle) {
		PRINTM(MINFO,
		       "sdio_mmc_interrupt(func = %p) card or handle is NULL, card=%p\n",
		       func, card);
		LEAVE();
		return;
	}
	handle = card->handle;
	if (handle->surprise_removed == MTRUE) {
		LEAVE();
		return;
	}
	handle->main_state = MOAL_RECV_INT;
	PRINTM(MINFO, "*** IN SDIO IRQ ***\n");
	PRINTM(MINTR, "*\n");

	if (handle->fw_reseting == MTRUE && (!handle->pmlan_adapter)) {
		handle->ops.read_reg(handle, 0x0c, &host_int_status_reg_val);
		PRINTM(MERROR,
		       "*** Recv intr during fw reset, host int status reg value is %d, ignore it ***\n",
		       host_int_status_reg_val);
		LEAVE();
		return;
	}
	/* call mlan_interrupt to read int status */
	status = mlan_interrupt(0, handle->pmlan_adapter);
	if (status == MLAN_STATUS_FAILURE) {
		PRINTM(MINTR, "mlan interrupt failed\n");
	}
#ifdef SDIO_SUSPEND_RESUME
	if (handle->is_suspended) {
		PRINTM(MINTR, "Receive interrupt in hs_suspended\n");
		LEAVE();
		return;
	}
#endif
	handle->main_state = MOAL_START_MAIN_PROCESS;
	/* Call MLAN main process */
	status = mlan_main_process(handle->pmlan_adapter);
	if (status == MLAN_STATUS_FAILURE) {
		PRINTM(MINTR, "mlan main process exited with failure\n");
	}
	handle->main_state = MOAL_END_MAIN_PROCESS;
	LEAVE();
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
/**
 * @brief This work handles oob sdio top irq.
 */
static void woal_sdio_oob_irq_work(struct work_struct *work)
{
	sdio_mmc_card *card = NULL;
	struct mmc_card *mmc_card = NULL;
	struct sdio_func *func;
	unsigned char pending;
	int i;
	int ret;

	if (!work) {
		PRINTM(MERROR, "Invalid work structure pointer\n");
		return;
	}

	// Use safe container_of with proper type validation
	// Coverity raised error for kernel API.
	// coverity[cert_arr39_c_violation:SUPPRESS]
	card = container_of(work, sdio_mmc_card, sdio_oob_irq_work);

	// Validate the resulting card pointer and its members
	if (!card || !card->func || !card->func->card) {
		PRINTM(MERROR, "Invalid sdio_mmc_card structure or members\n");
		return;
	}

	mmc_card = card->func->card;

	for (i = 0; i < mmc_card->sdio_funcs; i++) {
		func = NULL;
		if (mmc_card->sdio_func[i]) {
			func = mmc_card->sdio_func[i];
		}
		if (func) {
			sdio_claim_host(func);
			pending = sdio_f0_readb(func, SDIO_CCCR_INTx, &ret);
			if (!ret && pending && func->irq_handler)
				func->irq_handler(func);
			sdio_release_host(func);
		}
	}

	if (card->irq_registered && !card->irq_enabled) {
		card->irq_enabled = MTRUE;
		enable_irq(card->oob_irq);
	}
}

/**
 *  @brief oob_sdio_irq interrupt handler.
 *
 *  @param irq     irq
 *  @param dev_id   a pointer to structure sdio_mmc_card
 *  @return         IRQ_HANDLED
 */
static irqreturn_t oob_sdio_irq(int irq, void *dev_id)
{
	sdio_mmc_card *card = (sdio_mmc_card *)dev_id;

	if (card->sdio_func_intr_enabled) {
		disable_irq_nosync(card->oob_irq);
		card->irq_enabled = MFALSE;
		queue_work(card->sdio_oob_irq_workqueue,
			   &card->sdio_oob_irq_work);
	}

	return IRQ_HANDLED;
}

/**
 *  @brief This function registers oob_sdio_irq
 *
 *  @param card    a pointer to sdio_mmc_card
 *  @return         0-success else failure
 */
static int oob_sdio_irq_register(sdio_mmc_card *card)
{
	int ret = 0;

	ret = devm_request_irq(card->handle->hotplug_device, card->oob_irq,
			       oob_sdio_irq, IRQF_TRIGGER_LOW | IRQF_SHARED,
			       "nxp_oob_sdio_irq", card);

	if (!ret) {
		card->irq_registered = MTRUE;
		card->irq_enabled = MTRUE;
		enable_irq_wake(card->oob_irq);
	}

	return ret;
}

/**
 *  @brief This function unregister oob_sdio_irq
 *
 *  @param card    a pointer to sdio_mmc_card
 *  @return         N/A
 */
static void oob_sdio_irq_unregister(sdio_mmc_card *card)
{
	if (card->irq_registered) {
		card->irq_registered = MFALSE;
		disable_irq_wake(card->oob_irq);
		if (card->irq_enabled) {
			disable_irq(card->oob_irq);
			card->irq_enabled = MFALSE;
		}
		devm_free_irq(card->handle->hotplug_device, card->oob_irq,
			      card);
	}
}

/**
 *  @brief This function enable interrupt in SDIO Func0 SDIO_CCCR_IENx
 *
 *  @param func    a pointer to struct sdio_func
 *  @param handler  sdio_irq_handler
 *  @return         0-success else failure
 */
static int sdio_func_intr_enable(struct sdio_func *func,
				 sdio_irq_handler_t *handler)
{
	int ret;
	unsigned char reg;

#ifdef MMC_QUIRK_LENIENT_FN0
	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
#endif
	reg = sdio_f0_readb(func, SDIO_CCCR_IENx, &ret);
	if (ret)
		return ret;
	reg |= 1 << func->num;
	reg |= 1;
	sdio_f0_writeb(func, reg, SDIO_CCCR_IENx, &ret);
	if (ret)
		return ret;

	func->irq_handler = handler;

	return ret;
}

/**
 *  @brief This function disable interrupt in SDIO Func0 SDIO_CCCR_IENx
 *
 *  @param func    a pointer to struct sdio_func
 *  @return         0-success else failure
 */
static int sdio_func_intr_disable(struct sdio_func *func)
{
	int ret;
	unsigned char reg;

#ifdef MMC_QUIRK_LENIENT_FN0
	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
#endif
	if (func->irq_handler)
		func->irq_handler = NULL;

	reg = sdio_f0_readb(func, SDIO_CCCR_IENx, &ret);
	if (ret)
		return ret;
	reg &= ~(1 << func->num);
	if (!(reg & 0xFE))
		reg = 0;
	sdio_f0_writeb(func, reg, SDIO_CCCR_IENx, &ret);

	return ret;
}

/**
 *  @brief This function claim the oob sdio irq
 *
 *  @param card    a pointer to sdio_mmc_card
 *  @param handler  sdio_irq_handler
 *  @return         0-success else failure
 */
static int woal_sdio_claim_irq(sdio_mmc_card *card, sdio_irq_handler_t *handler)
{
	int ret;
	struct sdio_func *func = card->func;

	BUG_ON(!func);
	BUG_ON(!func->card);

	card->sdio_oob_irq_workqueue = alloc_ordered_workqueue(
		"SDIO_OOB_IRQ_WORKQ",
		__WQ_LEGACY | WQ_MEM_RECLAIM | WQ_HIGHPRI);
	MLAN_INIT_WORK(&card->sdio_oob_irq_work, woal_sdio_oob_irq_work);
	ret = oob_sdio_irq_register(card);
	if (ret) {
		destroy_workqueue(card->sdio_oob_irq_workqueue);
		card->sdio_oob_irq_workqueue = NULL;
		return ret;
	}
	ret = sdio_func_intr_enable(func, handler);
	if (ret) {
		oob_sdio_irq_unregister(card);
		destroy_workqueue(card->sdio_oob_irq_workqueue);
		card->sdio_oob_irq_workqueue = NULL;
	}
	card->sdio_func_intr_enabled = MTRUE;
	return ret;
}

/**
 *  @brief This function release the oob sdio irq
 *
 *  @param card    a pointer to sdio_mmc_card
 *  @return         0-success else failure
 */
static int woal_sdio_release_irq(sdio_mmc_card *card)
{
	struct sdio_func *func = card->func;
	BUG_ON(!func);
	BUG_ON(!func->card);

	oob_sdio_irq_unregister(card);
	flush_workqueue(card->sdio_oob_irq_workqueue);
	destroy_workqueue(card->sdio_oob_irq_workqueue);
	card->sdio_oob_irq_workqueue = NULL;

	if (card->sdio_func_intr_enabled) {
		sdio_func_intr_disable(func);
		card->sdio_func_intr_enabled = MFALSE;
	}

	return 0;
}

/**
 *  @brief This function request oob gpio
 *
 *  @param card    a pointer to sdio_mmc_card
 *  @param oob_gpio  oob gpio
 *  @return         0-success else failure
 */
static int woal_request_gpio(sdio_mmc_card *card, t_u8 oob_gpio)
{
#if defined(IMX_SUPPORT)
	struct device_node *node;
	node = of_find_compatible_node(NULL, NULL, "nxp,wifi-oob-int");
	if (!node)
		return -1;
	card->oob_irq = irq_of_parse_and_map(node, 0);
	PRINTM(MMSG, "SDIO OOB IRQ: %d", card->oob_irq);
	return 0;
#else
	return -1;
#endif
}
#endif

/**  @brief This function updates the card types
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param card     A Pointer to card
 *
 *  @return         N/A
 */
static t_u16 woal_update_card_type(t_void *card)
{
	sdio_mmc_card *cardp_sd = (sdio_mmc_card *)card;
	t_u16 card_type = 0;

	/* Update card type */
#ifdef SD8887
	if (cardp_sd->func->device == SD_DEVICE_ID_8887) {
		card_type = CARD_TYPE_SD8887;
		moal_memcpy_ext(NULL, driver_version, CARD_SD8887,
				strlen(CARD_SD8887), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V15, strlen(V15),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif

#ifdef SD8897
	if (cardp_sd->func->device == SD_DEVICE_ID_8897) {
		card_type = CARD_TYPE_SD8897;
		moal_memcpy_ext(NULL, driver_version, CARD_SD8897,
				strlen(CARD_SD8897), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V15, strlen(V15),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD8977
	if (cardp_sd->func->device == SD_DEVICE_ID_8977) {
		card_type = CARD_TYPE_SD8977;
		moal_memcpy_ext(NULL, driver_version, CARD_SD8977,
				strlen(CARD_SD8977), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V16, strlen(V16),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD8978
	if (cardp_sd->func->device == SD_DEVICE_ID_8978) {
		card_type = CARD_TYPE_SD8978;
		moal_memcpy_ext(NULL, driver_version, "SDIW416",
				strlen("SDIW416"), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V16, strlen(V16),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD8997
	if (cardp_sd->func->device == SD_DEVICE_ID_8997) {
		card_type = CARD_TYPE_SD8997;
		moal_memcpy_ext(NULL, driver_version, CARD_SD8997,
				strlen(CARD_SD8997), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V16, strlen(V16),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD8987
	if (cardp_sd->func->device == SD_DEVICE_ID_8987) {
		card_type = CARD_TYPE_SD8987;
		moal_memcpy_ext(NULL, driver_version, CARD_SD8987,
				strlen(CARD_SD8987), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V16, strlen(V16),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SDIW624
	if (cardp_sd->func->device == SD_DEVICE_ID_IW624) {
		card_type = CARD_TYPE_SDIW624;
		moal_memcpy_ext(NULL, driver_version, CARD_SDIW624,
				strlen(CARD_SDIW624), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V18, strlen(V18),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SDAW693
	if (cardp_sd->func->device == SD_DEVICE_ID_AW693_FN1 ||
	    cardp_sd->func->device == SD_DEVICE_ID_AW693_FN2) {
		card_type = CARD_TYPE_SDAW693;
		moal_memcpy_ext(NULL, driver_version, CARD_SDAW693,
				strlen(CARD_SDAW693), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V17, strlen(V17),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD9097
	if (cardp_sd->func->device == SD_DEVICE_ID_9097) {
		card_type = CARD_TYPE_SD9097;
		moal_memcpy_ext(NULL, driver_version, CARD_SD9097,
				strlen(CARD_SD9097), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V17, strlen(V17),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD9098
	if (cardp_sd->func->device == SD_DEVICE_ID_9098_FN1 ||
	    cardp_sd->func->device == SD_DEVICE_ID_9098_FN2) {
		card_type = CARD_TYPE_SD9098;
		moal_memcpy_ext(NULL, driver_version, CARD_SD9098,
				strlen(CARD_SD9098), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V17, strlen(V17),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SD9177
	if (cardp_sd->func->device == SD_DEVICE_ID_9177) {
		card_type = CARD_TYPE_SD9177;
		moal_memcpy_ext(NULL, driver_version, CARD_SD9177,
				strlen(CARD_SD9177), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V18, strlen(V18),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
#ifdef SDIW610
	if (cardp_sd->func->device == SD_DEVICE_ID_IW610) {
		card_type = CARD_TYPE_SDIW610;
		moal_memcpy_ext(NULL, driver_version, CARD_SDIW610,
				strlen(CARD_SDIW610), strlen(driver_version));
		moal_memcpy_ext(
			NULL,
			driver_version + strlen(INTF_CARDTYPE) +
				strlen(KERN_VERSION),
			V18, strlen(V18),
			strnlen(driver_version, MLAN_MAX_VER_STR_LEN - 1) -
				(strlen(INTF_CARDTYPE) + strlen(KERN_VERSION)));
	}
#endif
	return card_type;
}

/**  @brief This function handles client driver probe.
 *
 *  @param func     A pointer to sdio_func structure.
 *  @param id       A pointer to sdio_device_id structure.
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE/error code
 */
int woal_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	int ret = MLAN_STATUS_SUCCESS;
	sdio_mmc_card *card = NULL;
	t_u16 card_type = 0;

	ENTER();

	PRINTM(MMSG, "vendor=0x%4.04X device=0x%4.04X class=%d function=%d\n",
	       func->vendor, func->device, func->class, func->num);

	card = kzalloc(sizeof(sdio_mmc_card), GFP_KERNEL);
	if (!card) {
		PRINTM(MFATAL,
		       "Failed to allocate memory in probe function!\n");
		LEAVE();
		return -ENOMEM;
	}

	card->func = func;

#ifdef MMC_QUIRK_BLKSZ_FOR_BYTE_MODE
	/* The byte mode patch is available in kernel MMC driver
	 * which fixes one issue in MP-A transfer.
	 * bit1: use func->cur_blksize for byte mode
	 */
	func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
	/* wait for chip fully wake up */
	if (!func->enable_timeout)
		func->enable_timeout = 200;
#endif
	sdio_claim_host(func);
	ret = sdio_enable_func(func);
	if (ret) {
		sdio_release_host(func);
		PRINTM(MFATAL, "sdio_enable_func() failed: ret=%d\n", ret);
		ret = -EIO;
		goto err;
	}
	sdio_release_host(func);

	card_type = woal_update_card_type(card);
	if (!card_type) {
		PRINTM(MERROR, "sdmmc probe: woal_update_card_type() failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto err;
	}
	INIT_WORK(&card->reset_work, woal_sdiommc_work);
	if (NULL ==
	    woal_add_card(card, &card->func->dev, &sdiommc_ops, card_type)) {
		PRINTM(MMSG, "woal_add_card failed\n");
		ret = MLAN_STATUS_FAILURE;
		goto err;
	}

#ifdef IMX_SUPPORT
	woal_regist_oob_wakeup_irq(card->handle);
#endif /* IMX_SUPPORT */

	LEAVE();
	return ret;
err:
	kfree(card);
	sdio_claim_host(func);
	sdio_disable_func(func);
	sdio_release_host(func);

	LEAVE();
	return ret;
}

/**  @brief This function handles client driver remove.
 *
 *  @param func     A pointer to sdio_func structure.
 *  @return         N/A
 */
void woal_sdio_remove(struct sdio_func *func)
{
	sdio_mmc_card *card;

	ENTER();

	if (func) {
		PRINTM(MINFO, "SDIO func=%d\n", func->num);
		card = sdio_get_drvdata(func);
		if (card) {
			/* We need to advance the time to set surprise_removed
			 * to MTRUE as fast as possible to avoid race condition
			 * with woal_sdio_interrupt()
			 *
			 * @todo: Due to woal_sdio_interrupt() is called in
			 * Linux's work queue, cannot be suspended to impact
			 * other works. Need a lock in these two functions:
			 * woal_sdio_remove() waits until woal_sdio_interrupt
			 * ends. woal_sdio_interrupt() returns if
			 * woal_sdio_remove() is running.
			 */
			if (card->handle != NULL) {
				card->handle->surprise_removed = MTRUE;

				/* check if woal_sdio_interrupt() is running */
				while (card->handle->main_state !=
					       MOAL_END_MAIN_PROCESS &&
				       card->handle->main_state !=
					       MOAL_STATE_IDLE)
					woal_sched_timeout(2); /* wait until
								  woal_sdio_interrupt
								  ends */
			}

#ifdef IMX_SUPPORT
			woal_unregist_oob_wakeup_irq(card->handle);
#endif /* IMX_SUPPORT */
			woal_remove_card(card);
			kfree(card);
		}
	}

	LEAVE();
}

#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
#ifdef MMC_PM_FUNC_SUSPENDED
/**  @brief This function tells lower driver that WLAN is suspended
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @return         N/A
 */
void woal_wlan_is_suspended(moal_handle *handle)
{
	ENTER();
	if (handle->suspend_notify_req == MTRUE) {
		handle->is_suspended = MTRUE;
		sdio_func_suspended(((sdio_mmc_card *)handle->card)->func);
	}
	LEAVE();
}
#endif

/**  @brief This function handles client driver shutdown
 *
 *  @param dev      A pointer to device structure
 *  @return         N/A
 */
void woal_sdio_shutdown(struct device *dev)
{
	moal_handle *handle = NULL;
	sdio_mmc_card *cardp;
	mlan_ds_ps_info pm_info;
	int i, retry_num = 8;
	struct sdio_func *func;

	ENTER();
	PRINTM(MCMND, "<--- Enter woal_sdio_shutdown --->\n");
	if (!dev) {
		PRINTM(MERROR, "Invalid device pointer in resume\n");
		LEAVE();
		return;
	}
	// Coverity violation raised for kernel's API
	// coverity[cert_arr39_c_violation:SUPPRESS]
	func = dev_to_sdio_func(dev);
	cardp = sdio_get_drvdata(func);
	if (!cardp || !cardp->handle) {
		PRINTM(MERROR, "Card or moal_handle structure is not valid\n");
		LEAVE();
		return;
	}
	handle = cardp->handle;
	for (i = 0; i < handle->priv_num; i++) {
		if (handle->priv[i])
			netif_device_detach(handle->priv[i]->netdev);
	}

	if (moal_extflg_isset(handle, EXT_SHUTDOWN_HS)) {
		handle->shutdown_hs_in_process = MTRUE;
		memset(&pm_info, 0, sizeof(pm_info));
		for (i = 0; i < retry_num; i++) {
			if (MLAN_STATUS_SUCCESS ==
			    woal_get_pm_info(woal_get_priv(handle,
							   MLAN_BSS_ROLE_ANY),
					     &pm_info)) {
				if (pm_info.is_suspend_allowed == MTRUE)
					break;
				else
					PRINTM(MMSG,
					       "Shutdown not allowed and retry again\n");
			}
			woal_sched_timeout(100);
		}
		if (pm_info.is_suspend_allowed == MFALSE) {
			PRINTM(MMSG, "Shutdown not allowed\n");
			goto done;
		}
		woal_enable_hs(woal_get_priv(handle, MLAN_BSS_ROLE_ANY));

		wait_event_interruptible_timeout(
			handle->hs_activate_wait_q,
			handle->hs_activate_wait_q_woken, HS_ACTIVE_TIMEOUT);
		if (handle->hs_activated == MTRUE)
			PRINTM(MMSG, "HS actived in shutdown\n");
		else
			PRINTM(MMSG, "Fail to enable HS in shutdown\n");
	} else {
		for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM); i++) {
			if (handle->priv[i]) {
				if (handle->priv[i]->media_connected == MTRUE
#ifdef UAP_SUPPORT
				    || (GET_BSS_ROLE(handle->priv[i]) ==
					MLAN_BSS_ROLE_UAP)
#endif
				) {
					PRINTM(MIOCTL,
					       "disconnect on suspend\n");
					if (woal_disconnect(
						    handle->priv[i],
						    MOAL_NO_WAIT, NULL,
						    DEF_DEAUTH_REASON_CODE))
						PRINTM(MERROR,
						       "failed to disconnect on suspend\n");
				}
			}
		}
	}

done:
	PRINTM(MCMND, "<--- Leave woal_sdio_shutdown --->\n");
	LEAVE();
	return;
}

/**  @brief This function handles client driver suspend
 *
 *  @param dev      A pointer to device structure
 *  @return         MLAN_STATUS_SUCCESS or error code
 */
int woal_sdio_suspend(struct device *dev)
{
	mmc_pm_flag_t pm_flags = 0;
	moal_handle *handle = NULL;
	sdio_mmc_card *cardp;
	int i, retry_num = 8;
	int ret = MLAN_STATUS_SUCCESS;
	int hs_actived = 0;
	mlan_ds_ps_info pm_info;

	struct sdio_func *func;

	ENTER();
	PRINTM(MCMND, "<--- Enter woal_sdio_suspend --->\n");
	if (!dev) {
		PRINTM(MERROR, "Invalid device pointer in resume\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	// coverity issue raised for kernel's API
	// coverity[cert_arr39_c_violation:SUPPRESS]
	func = dev_to_sdio_func(dev);
	pm_flags = sdio_get_host_pm_caps(func);
	PRINTM(MCMND, "%s: suspend: PM flags = 0x%x\n", sdio_func_id(func),
	       pm_flags);
	cardp = sdio_get_drvdata(func);
	if (!cardp || !cardp->handle) {
		PRINTM(MERROR, "Card or moal_handle structure is not valid\n");
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}

	handle = cardp->handle;

	if (moal_extflg_isset(handle, EXT_PM_KEEP_POWER) &&
	    !(pm_flags & MMC_PM_KEEP_POWER)) {
		PRINTM(MERROR,
		       "%s: cannot remain alive while host is suspended\n",
		       sdio_func_id(func));
		LEAVE();
		return -ENOSYS;
	}
	if (handle->is_suspended == MTRUE) {
		PRINTM(MWARN, "Device already suspended\n");
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}
	if (handle->fw_dump) {
		PRINTM(MMSG, "suspend not allowed while FW dump!");
		ret = -EBUSY;
		goto done;
	}
#ifdef STA_SUPPORT
	for (i = 0; i < MIN(handle->priv_num, MLAN_MAX_BSS_NUM); i++) {
		if (handle->priv[i])
			woal_cancel_scan(handle->priv[i], MOAL_IOCTL_WAIT);
	}
#endif
	handle->suspend_fail = MFALSE;
	memset(&pm_info, 0, sizeof(pm_info));
	for (i = 0; i < retry_num; i++) {
		if (MLAN_STATUS_SUCCESS ==
		    woal_get_pm_info(woal_get_priv(handle, MLAN_BSS_ROLE_ANY),
				     &pm_info)) {
			if (pm_info.is_suspend_allowed == MTRUE)
				break;
			else
				PRINTM(MMSG,
				       "Suspend not allowed and retry again\n");
		}
		woal_sched_timeout(100);
	}
	if (pm_info.is_suspend_allowed == MFALSE) {
		PRINTM(MMSG, "Suspend not allowed\n");
		ret = -EBUSY;
		goto done;
	}

	for (i = 0; i < handle->priv_num; i++) {
		if (handle->priv[i])
			netif_device_detach(handle->priv[i]->netdev);
	}

	if (moal_extflg_isset(handle, EXT_PM_KEEP_POWER)) {
		/* Enable the Host Sleep */
#ifdef MMC_PM_FUNC_SUSPENDED
		handle->suspend_notify_req = MTRUE;
#endif
		woal_sched_timeout(200);
		hs_actived = woal_enable_hs(
			woal_get_priv(handle, MLAN_BSS_ROLE_ANY));
#ifdef MMC_PM_FUNC_SUSPENDED
		handle->suspend_notify_req = MFALSE;
#endif
		if (hs_actived) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 3, 4)
			if (pm_flags & MMC_PM_WAKE_SDIO_IRQ) {
				ret = sdio_set_host_pm_flags(
					func, MMC_PM_WAKE_SDIO_IRQ);
				PRINTM(MCMND,
				       "suspend with MMC_PM_WAKE_SDIO_IRQ ret=%d\n",
				       ret);
			}
#endif
#ifdef MMC_PM_SKIP_RESUME_PROBE
			PRINTM(MCMND,
			       "suspend with MMC_PM_KEEP_POWER and MMC_PM_SKIP_RESUME_PROBE\n");
			ret = sdio_set_host_pm_flags(
				func,
				MMC_PM_KEEP_POWER | MMC_PM_SKIP_RESUME_PROBE);
#else
			PRINTM(MCMND, "suspend with MMC_PM_KEEP_POWER\n");
			ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);
#endif
		} else {
			PRINTM(MMSG, "HS not actived, suspend fail!");
			handle->suspend_fail = MTRUE;
			for (i = 0; i < handle->priv_num; i++) {
				if (handle->priv[i]) {
					netif_device_attach(
						handle->priv[i]->netdev);
				}
			}
			ret = -EBUSY;
			goto done;
		}
	}

	/* Indicate device suspended */
	handle->is_suspended = MTRUE;
#ifdef IMX_SUPPORT
	woal_enable_oob_wakeup_irq(handle);
#endif /* IMX_SUPPORT */
done:
	PRINTM(MCMND, "<--- Leave woal_sdio_suspend --->\n");
	LEAVE();
	return ret;
}

/**  @brief This function handles client driver resume
 *
 *  @param dev      A pointer to device structure
 *  @return         MLAN_STATUS_SUCCESS
 */
int woal_sdio_resume(struct device *dev)
{
	mmc_pm_flag_t pm_flags = 0;
	moal_handle *handle = NULL;
	sdio_mmc_card *cardp;
	int i;
	struct sdio_func *func;

	ENTER();
	PRINTM(MCMND, "<--- Enter woal_sdio_resume --->\n");
	if (!dev) {
		PRINTM(MERROR, "Invalid device pointer in resume\n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
	// coverity issue raised for Kernel's API
	// coverity[cert_arr39_c_violation:SUPPRESS]
	func = dev_to_sdio_func(dev);
	pm_flags = sdio_get_host_pm_caps(func);
	PRINTM(MCMND, "%s: resume: PM flags = 0x%x\n", sdio_func_id(func),
	       pm_flags);
	cardp = sdio_get_drvdata(func);
	if (!cardp || !cardp->handle) {
		PRINTM(MERROR, "Card or moal_handle structure is not valid\n");
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}
	handle = cardp->handle;

	if (handle->is_suspended == MFALSE) {
		PRINTM(MWARN, "Device already resumed\n");
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}
	handle->is_suspended = MFALSE;
	if (woal_check_driver_status(handle)) {
		PRINTM(MERROR, "Resuem, device is in hang state\n");
		LEAVE();
		return MLAN_STATUS_SUCCESS;
	}
	for (i = 0; i < handle->priv_num; i++) {
		if (handle->priv[i])
			netif_device_attach(handle->priv[i]->netdev);
	}

	/* Disable Host Sleep */
	woal_cancel_hs(woal_get_priv(handle, MLAN_BSS_ROLE_ANY), MOAL_NO_WAIT);
#ifdef IMX_SUPPORT
	woal_disable_oob_wakeup_irq(handle);
#endif /* IMX_SUPPORT */
	PRINTM(MCMND, "<--- Leave woal_sdio_resume --->\n");
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}
#endif
#endif /* SDIO_SUSPEND_RESUME */

/**
 *  @brief This function writes data into card register
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdiommc_write_reg(moal_handle *handle, t_u32 reg,
					  t_u32 data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	sdio_writeb(((sdio_mmc_card *)handle->card)->func, (t_u8)data, reg,
		    (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	PRINTM(MREG, "sdio w %x = %x (%x)\n", reg, data, ret);
	return ret;
}

/**
 *  @brief This function reads data from card register
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdiommc_read_reg(moal_handle *handle, t_u32 reg,
					 t_u32 *data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 val;
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	val = sdio_readb(((sdio_mmc_card *)handle->card)->func, reg,
			 (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	*data = val;
	PRINTM(MREG, "sdio r %x = %x (%x)\n", reg, *data, ret);

	return ret;
}

/**
 *  @brief This function writes data into card register
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdio_writeb(moal_handle *handle, t_u32 reg, t_u8 data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	sdio_writeb(((sdio_mmc_card *)handle->card)->func, (t_u8)data, reg,
		    (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	PRINTM(MREG, "sdio w %x = %x (%x)\n", reg, data, ret);
	return ret;
}

/**
 *  @brief This function reads data from card register
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdio_readb(moal_handle *handle, t_u32 reg, t_u8 *data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 val;
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	val = sdio_readb(((sdio_mmc_card *)handle->card)->func, reg,
			 (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	*data = val;
	PRINTM(MREG, "sdio r %x = %x (%x)\n", reg, *data, ret);

	return ret;
}

/**
 *  @brief This function reads data from card register FN0
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdio_f0_readb(moal_handle *handle, t_u32 reg,
				      t_u8 *data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 val;
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	val = sdio_f0_readb(((sdio_mmc_card *)handle->card)->func, reg,
			    (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	*data = val;
	PRINTM(MREG, "sdio f0 r %x = %x (%x)\n", reg, *data, ret);

	return ret;
}

/**
 *  @brief This function writes data to card register FN0
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param reg      Register offset
 *  @param data     Value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdio_f0_writeb(moal_handle *handle, t_u32 reg,
				       t_u8 data)
{
	mlan_status ret = MLAN_STATUS_FAILURE;

	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	sdio_f0_writeb(((sdio_mmc_card *)handle->card)->func, data, reg,
		       (int *)&ret);
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);

	PRINTM(MREG, "sdio f0 w %x = %x (%x)\n", reg, data, ret);

	return ret;
}

/**
 *  @brief This function use SG mode to read/write data into card memory
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param pmbuf_list   Pointer to a linked list of mlan_buffer structure
 *  @param port     Port
 *  @param write    write flag
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdio_rw_mb(moal_handle *handle, pmlan_buffer pmbuf_list,
				   t_u32 port, t_u8 write)
{
	struct scatterlist sg_list[SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX];
	int num_sg = pmbuf_list->use_count;
	int i = 0;
	mlan_buffer *pmbuf = NULL;
	struct mmc_request mmc_req;
	struct mmc_command mmc_cmd;
	struct mmc_data mmc_dat;
	struct sdio_func *func = ((sdio_mmc_card *)handle->card)->func;
	t_u32 ioport = (port & MLAN_SDIO_IO_PORT_MASK);
	t_u32 blkcnt = pmbuf_list->data_len / handle->sdio_blk_size;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
	int status;
#endif

	if (num_sg > SDIO_MP_AGGR_DEF_PKT_LIMIT_MAX) {
		PRINTM(MERROR, "ERROR: num_sg=%d", num_sg);
		return MLAN_STATUS_FAILURE;
	}
	sg_init_table(sg_list, num_sg);
	pmbuf = pmbuf_list->pnext;
	for (i = 0; i < num_sg; i++) {
		if (pmbuf == pmbuf_list)
			break;
		sg_set_buf(&sg_list[i], pmbuf->pbuf + pmbuf->data_offset,
			   pmbuf->data_len);
		pmbuf = pmbuf->pnext;
	}
	memset(&mmc_req, 0, sizeof(struct mmc_request));
	memset(&mmc_cmd, 0, sizeof(struct mmc_command));
	memset(&mmc_dat, 0, sizeof(struct mmc_data));

	mmc_dat.sg = sg_list;
	mmc_dat.sg_len = num_sg;
	mmc_dat.blksz = handle->sdio_blk_size;
	mmc_dat.blocks = blkcnt;
	mmc_dat.flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;

	mmc_cmd.opcode = SD_IO_RW_EXTENDED;
	mmc_cmd.arg = write ? 1 << 31 : 0;
	mmc_cmd.arg |= (func->num & 0x7) << 28;
	mmc_cmd.arg |= 1 << 27; /* block basic */
	mmc_cmd.arg |= 0; /* fix address */
	mmc_cmd.arg |= (ioport & 0x1FFFF) << 9;
	mmc_cmd.arg |= blkcnt & 0x1FF;
	mmc_cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

	mmc_req.cmd = &mmc_cmd;
	mmc_req.data = &mmc_dat;

	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	mmc_set_data_timeout(&mmc_dat,
			     ((sdio_mmc_card *)handle->card)->func->card);
	mmc_wait_for_req(((sdio_mmc_card *)handle->card)->func->card->host,
			 &mmc_req);

	if (mmc_cmd.error || mmc_dat.error) {
		PRINTM(MERROR, "CMD53 %s cmd_error = %d data_error=%d\n",
		       write ? "write" : "read", mmc_cmd.error, mmc_dat.error);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
		/* issue abort cmd52 command through F0*/
		sdio_f0_writeb(((sdio_mmc_card *)handle->card)->func, 0x01,
			       SDIO_CCCR_ABORT, &status);
#endif
		sdio_release_host(((sdio_mmc_card *)handle->card)->func);
		return MLAN_STATUS_FAILURE;
	}
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function writes multiple bytes into card memory
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param pmbuf    Pointer to mlan_buffer structure
 *  @param port     Port
 *  @param timeout  Time out value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdiommc_write_data_sync(moal_handle *handle,
						mlan_buffer *pmbuf, t_u32 port,
						t_u32 timeout)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 *buffer = (t_u8 *)(pmbuf->pbuf + pmbuf->data_offset);
	t_u8 blkmode =
		(port & MLAN_SDIO_BYTE_MODE_MASK) ? BYTE_MODE : BLOCK_MODE;
	t_u32 blksz = (blkmode == BLOCK_MODE) ? handle->sdio_blk_size : 1;
	t_u32 blkcnt = (blkmode == BLOCK_MODE) ?
			       (pmbuf->data_len / handle->sdio_blk_size) :
			       pmbuf->data_len;
	t_u32 ioport = (port & MLAN_SDIO_IO_PORT_MASK);
	int status = 0;
	if (pmbuf->use_count > 1)
		return woal_sdio_rw_mb(handle, pmbuf, port, MTRUE);
#ifdef SDIO_MMC_DEBUG
	handle->cmd53w = 1;
#endif
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	status = sdio_writesb(((sdio_mmc_card *)handle->card)->func, ioport,
			      buffer, blkcnt * blksz);
	if (!status)
		ret = MLAN_STATUS_SUCCESS;
	else {
		PRINTM(MERROR, "cmd53 write error=%d\n", status);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
		/* issue abort cmd52 command through F0*/
		sdio_f0_writeb(((sdio_mmc_card *)handle->card)->func, 0x01,
			       SDIO_CCCR_ABORT, &status);
#endif
	}
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
#ifdef SDIO_MMC_DEBUG
	handle->cmd53w = 2;
#endif
	return ret;
}

/**
 *  @brief This function reads multiple bytes from card memory
 *
 *  @param handle   A Pointer to the moal_handle structure
 *  @param pmbuf    Pointer to mlan_buffer structure
 *  @param port     Port
 *  @param timeout  Time out value
 *
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdiommc_read_data_sync(moal_handle *handle,
					       mlan_buffer *pmbuf, t_u32 port,
					       t_u32 timeout)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	t_u8 *buffer = (t_u8 *)(pmbuf->pbuf + pmbuf->data_offset);
	t_u8 blkmode =
		(port & MLAN_SDIO_BYTE_MODE_MASK) ? BYTE_MODE : BLOCK_MODE;
	t_u32 blksz = (blkmode == BLOCK_MODE) ? handle->sdio_blk_size : 1;
	t_u32 blkcnt = (blkmode == BLOCK_MODE) ?
			       (pmbuf->data_len / handle->sdio_blk_size) :
			       pmbuf->data_len;
	t_u32 ioport = (port & MLAN_SDIO_IO_PORT_MASK);
	int status = 0;
	if (pmbuf->use_count > 1)
		return woal_sdio_rw_mb(handle, pmbuf, port, MFALSE);
#ifdef SDIO_MMC_DEBUG
	handle->cmd53r = 1;
#endif
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	status = sdio_readsb(((sdio_mmc_card *)handle->card)->func, buffer,
			     ioport, blkcnt * blksz);
	if (!status) {
		ret = MLAN_STATUS_SUCCESS;
	} else {
		PRINTM(MERROR, "cmd53 read error=%d\n", status);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
		/* issue abort cmd52 command through F0*/
		sdio_f0_writeb(((sdio_mmc_card *)handle->card)->func, 0x01,
			       SDIO_CCCR_ABORT, &status);
#endif
	}
	sdio_release_host(((sdio_mmc_card *)handle->card)->func);
#ifdef SDIO_MMC_DEBUG
	handle->cmd53r = 2;
#endif
	return ret;
}

/**
 *  @brief This function registers the IF module in bus driver
 *
 *  @return    MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status woal_sdiommc_bus_register(void)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;

	ENTER();

	/* SDIO Driver Registration */
	if (sdio_register_driver(&wlan_sdio)) {
		PRINTM(MFATAL, "SDIO Driver Registration Failed \n");
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}

	/* init GPIO PORT for wakeup purpose */
	GPIO_PORT_INIT();
	/* set default value */
	GPIO_PORT_TO_HIGH();

	LEAVE();
	return ret;
}

/**
 *  @brief This function de-registers the IF module in bus driver
 *
 *  @return         N/A
 */
void woal_sdiommc_bus_unregister(void)
{
	ENTER();

	/* SDIO Driver Unregistration */
	sdio_unregister_driver(&wlan_sdio);

	LEAVE();
}

/**
 *  @brief This function de-registers the device
 *
 *  @param handle A pointer to moal_handle structure
 *  @return         N/A
 */
static void woal_sdiommc_unregister_dev(moal_handle *handle)
{
	ENTER();
	if (handle->card) {
		sdio_mmc_card *card = handle->card;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
		struct sdio_func *func = card->func;
#endif
		/* Release the SDIO IRQ */
		sdio_claim_host(card->func);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
		if (moal_extflg_isset(handle, EXT_INTMODE))
			woal_sdio_release_irq(card);
		else
#endif
			sdio_release_irq(card->func);
		sdio_disable_func(card->func);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
		if (handle->driver_status)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
			mmc_hw_reset(func->card);
#else
			mmc_hw_reset(func->card->host);
#endif
#endif
		sdio_release_host(card->func);

		sdio_set_drvdata(card->func, NULL);

		GPIO_PORT_TO_LOW();
		PRINTM(MWARN, "Making the sdio dev card as NULL\n");
		card->handle = NULL;
	}

	LEAVE();
}

/**
 *  @brief This function registers the device
 *
 *  @param handle  A pointer to moal_handle structure
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_sdiommc_register_dev(moal_handle *handle)
{
	int ret = MLAN_STATUS_SUCCESS;
	sdio_mmc_card *card = handle->card;
	struct sdio_func *func;

	ENTER();

	GPIO_PORT_INIT();
	GPIO_PORT_TO_HIGH();

	/* save adapter pointer in card */
	card->handle = handle;
	func = card->func;
	sdio_claim_host(func);

	/* Request the SDIO IRQ */
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
	if (moal_extflg_isset(handle, EXT_INTMODE)) {
		ret = woal_request_gpio(card, card->handle->params.gpiopin);
		if (ret) {
			PRINTM(MERROR, "Fail to request gpio\n");
			goto release_host;
		}
		ret = woal_sdio_claim_irq(card, woal_sdio_interrupt);
		/* For SDIO over SPI, set CCCR_IF register bit5 ECSI to enable
		 * IRQ mode. Set CCCR CARD_CTRL3 bit 1 to configure start token
		 * is 0xFE for CMD53 single block write operation.
		 */
		if ((((sdio_mmc_card *)handle->card)->func->card->host->caps &
		     MMC_CAP_SPI)) {
			t_u8 data = 0;
			woal_sdio_f0_readb(handle, SDIO_CCCR_IF, &data);
			data |= SDIO_BUS_ECSI;
			woal_sdio_f0_writeb(handle, SDIO_CCCR_IF, data);

			woal_sdio_f0_readb(handle, SD_CARD_CTRL3, &data);
			data |= 0x02;
			woal_sdio_f0_writeb(handle, SD_CARD_CTRL3, data);
		}
	} else
#endif
		ret = sdio_claim_irq(func, woal_sdio_interrupt);
	if (ret) {
		PRINTM(MFATAL, "sdio_claim_irq failed: ret=%d\n", ret);
		goto release_host;
	}

	/* Set block size */
	ret = sdio_set_block_size(card->func, handle->sdio_blk_size);
	if (ret) {
		PRINTM(MERROR,
		       "sdio_set_block_seize(): cannot set SDIO block size\n");
		ret = MLAN_STATUS_FAILURE;
		goto release_irq;
	}

	sdio_release_host(func);
	sdio_set_drvdata(func, card);

	LEAVE();
	return MLAN_STATUS_SUCCESS;

release_irq:
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
	if (moal_extflg_isset(handle, EXT_INTMODE))
		woal_sdio_release_irq(card);
	else
#endif
		sdio_release_irq(func);
release_host:
	sdio_release_host(func);
	handle->card = NULL;

	LEAVE();
	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief This function set the sdio bus width
 *
 *  @param handle   A pointer to moal_handle structure
 *  @param mode     1--1 bit mode, 4--4 bit mode
 *  @return         MLAN_STATUS_SUCCESS
 */
int woal_sdio_set_buswidth(moal_handle *handle, t_u8 bus_width)
{
	sdio_mmc_card *cardp = (sdio_mmc_card *)handle->card;
	struct mmc_host *host = cardp->func->card->host;
	int ret = 0;
	t_u8 val;

	ENTER();

	if (bus_width != SDIO_BUS_WIDTH_1 && bus_width != SDIO_BUS_WIDTH_4) {
		LEAVE();
		return -EFAULT;
	}
	sdio_claim_host(((sdio_mmc_card *)handle->card)->func);
	val = sdio_f0_readb(((sdio_mmc_card *)handle->card)->func, SDIO_CCCR_IF,
			    (int *)&ret);
	val &= ~SDIO_BUS_WIDTH_MASK;
	if (bus_width == SDIO_BUS_WIDTH_1)
		host->ios.bus_width = MMC_BUS_WIDTH_1;
	else {
		host->ios.bus_width = MMC_BUS_WIDTH_4;
		val |= SDIO_BUS_WIDTH_4BIT;
	}
	sdio_f0_writeb(((sdio_mmc_card *)handle->card)->func, val, SDIO_CCCR_IF,
		       &ret);
	sdio_release_host(cardp->func);
	host->ops->set_ios(host, &host->ios);
	LEAVE();
	return ret;
}

/**
 *  @brief This function set bus clock on/off
 *
 *  @param handle   A pointer to moal_handle structure
 *  @param option   TRUE--on , FALSE--off
 *  @return         MLAN_STATUS_SUCCESS
 */
int woal_sdio_set_bus_clock(moal_handle *handle, t_u8 option)
{
	sdio_mmc_card *cardp = (sdio_mmc_card *)handle->card;
	struct mmc_host *host = cardp->func->card->host;

	ENTER();
	if (option == MTRUE) {
		/* restore value if non-zero */
		if (cardp->host_clock)
			host->ios.clock = cardp->host_clock;
	} else {
		/* backup value if non-zero, then clear */
		if (host->ios.clock)
			cardp->host_clock = host->ios.clock;
		host->ios.clock = 0;
	}

	host->ops->set_ios(host, &host->ios);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function updates card reg based on the Cmd52 value in dev
 * structure
 *
 *  @param handle   A pointer to moal_handle structure
 *  @param func     A pointer to store func variable
 *  @param reg      A pointer to store reg variable
 *  @param val      A pointer to store val variable
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
int woal_sdio_read_write_cmd52(moal_handle *handle, int func, int reg, int val)
{
	int ret = MLAN_STATUS_SUCCESS;
	sdio_mmc_card *card = (sdio_mmc_card *)handle->card;

	ENTER();
	/* Save current func and reg for read */
	handle->cmd52_func = func;
	handle->cmd52_reg = reg;
	sdio_claim_host(card->func);
	if (val >= 0) {
		/* Perform actual write only if val is provided */
		if (func)
			sdio_writeb(card->func, val, reg, &ret);
		else
			sdio_f0_writeb(card->func, val, reg, &ret);
		if (ret) {
			PRINTM(MERROR,
			       "Cannot write value (0x%x) to func %d reg 0x%x\n",
			       val, func, reg);
		} else {
			PRINTM(MMSG, "write value (0x%x) to func %d reg 0x%x\n",
			       (u8)val, func, reg);
			handle->cmd52_val = val;
		}
	} else {
		if (func)
			val = sdio_readb(card->func, reg, &ret);
		else
			val = sdio_f0_readb(card->func, reg, &ret);
		if (ret) {
			PRINTM(MERROR,
			       "Cannot read value from func %d reg 0x%x\n",
			       func, reg);
		} else {
			PRINTM(MMSG,
			       "read value (0x%x) from func %d reg 0x%x\n",
			       (u8)val, func, reg);
			handle->cmd52_val = val;
		}
	}
	sdio_release_host(card->func);
	LEAVE();
	return ret;
}

/**
 *  @brief This function check if this is second mac
 *
 *  @param handle   A pointer to moal_handle structure
 *  @return         MTRUE/MFALSE
 *
 */
static t_u8 woal_sdiommc_is_second_mac(moal_handle *handle)
{
#if defined(SDAW693) || defined(SD9098)
	sdio_mmc_card *card = (sdio_mmc_card *)handle->card;
#endif

#ifdef SDAW693
	if (card->func->device == SD_DEVICE_ID_AW693_FN2)
		return MTRUE;
#endif
#ifdef SD9098
	if (card->func->device == SD_DEVICE_ID_9098_FN2)
		return MTRUE;
#endif
	return MFALSE;
}

static mlan_status woal_sdiommc_get_fw_name(moal_handle *handle)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	sdio_mmc_card *card = (sdio_mmc_card *)handle->card;
	t_u32 revision_id = 0;
	t_u32 rev_id_reg = handle->card_info->rev_id_reg;

#if defined(SD8987) || defined(SD8997) || defined(SD9098) ||                   \
	defined(SD9097) || defined(SDIW624) || defined(SDAW693) ||             \
	defined(SD8978) || defined(SD9177) || defined(SDIW610)
	t_u32 magic_reg = handle->card_info->magic_reg;
	t_u32 magic = 0;
	t_u32 host_strap_reg = handle->card_info->host_strap_reg;
	t_u32 strap = 0;
#endif
#if defined(SDAW693) || defined(SDIW610)
	char *se_pos = NULL;
#endif

	ENTER();

	handle->sdio_blk_size =
		MIN(MLAN_SDIO_BLOCK_SIZE, card->func->card->host->max_blk_size);
	if (handle->params.fw_name)
		goto done;
	/** Revision ID register */
	woal_sdiommc_read_reg(handle, rev_id_reg, &revision_id);
	PRINTM(MCMND, "revision_id=0x%x sdio_blk_size=%d\n", revision_id,
	       handle->sdio_blk_size);

#if defined(SD8987) || defined(SD8997) || defined(SD9098) ||                   \
	defined(SD9097) || defined(SDIW624) || defined(SDAW693) ||             \
	defined(SD8978) || defined(SD9177) || defined(SDIW610)
	/** Revision ID register */
	woal_sdiommc_read_reg(handle, magic_reg, &magic);
	/** Revision ID register */
	woal_sdiommc_read_reg(handle, host_strap_reg, &strap);
	if (IS_SDIW624(handle->card_type))
		strap &= 0x07;
	else
		strap &= 0x1;
	magic &= 0xFF;
	/* 1 = SDSD, 0 --SD UART */
	PRINTM(MCMND, "magic=0x%x strap=0x%x\n", magic, strap);
#endif
#if defined(SD8977)
	if (IS_SD8977(handle->card_type)) {
		switch (revision_id) {
		case SD8977_V0:
			strncpy(handle->card_info->fw_name, SD8977_V0_FW_NAME,
				FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SD8977_WLAN_V0_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SD8977_V1:
			strncpy(handle->card_info->fw_name, SD8977_V1_FW_NAME,
				FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SD8977_WLAN_V1_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SD8977_V2:
			strncpy(handle->card_info->fw_name, SD8977_V2_FW_NAME,
				FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SD8977_WLAN_V2_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		default:
			break;
		}
	}
#endif
#if defined(SD8887)
	if (IS_SD8887(handle->card_type)) {
		/* Check revision ID */
		switch (revision_id) {
		case SD8887_A0:
			strncpy(handle->card_info->fw_name, SD8887_A0_FW_NAME,
				FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SD8887_WLAN_A0_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SD8887_A2:
			strncpy(handle->card_info->fw_name, SD8887_A2_FW_NAME,
				FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SD8887_WLAN_A2_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		default:
			break;
		}
	}
#endif

#ifdef SD8997
	if (IS_SD8997(handle->card_type)) {
		if (magic == CHIP_MAGIC_VALUE) {
			if (strap == CARD_TYPE_SD_UART)
				strncpy(handle->card_info->fw_name,
					SDUART8997_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDSD8997_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		}
	}
#endif

#ifdef SD8987
	if (IS_SD8987(handle->card_type)) {
		if (magic == CHIP_MAGIC_VALUE) {
			if (strap == CARD_TYPE_SD_UART)
				strncpy(handle->card_info->fw_name,
					SDUART8987_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDSD8987_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		}
	}
#endif

#ifdef SD8978
	if (IS_SD8978(handle->card_type)) {
		if (magic == CHIP_MAGIC_VALUE) {
			if (strap == CARD_TYPE_SD_UART)
				strncpy(handle->card_info->fw_name,
					SDUART8978_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDSD8978_DEFAULT_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		}
	}
#endif

#ifdef SD9098
	if (IS_SD9098(handle->card_type) &&
	    (card->func->device == SD_DEVICE_ID_9098_FN1)) {
		switch (revision_id) {
		case SD9098_Z1Z2:
			if (magic == CHIP_MAGIC_VALUE) {
				if (strap == CARD_TYPE_SD_UART)
					strncpy(handle->card_info->fw_name,
						SDUART9098_DEFAULT_COMBO_FW_NAME,
						FW_NAMW_MAX_LEN);
				else
					strncpy(handle->card_info->fw_name,
						SDSD9098_DEFAULT_COMBO_FW_NAME,
						FW_NAMW_MAX_LEN);
			}
			strncpy(handle->card_info->fw_name_wlan,
				SD9098_DEFAULT_WLAN_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SD9098_A0:
		case SD9098_A1:
		case SD9098_A2:
			if (magic == CHIP_MAGIC_VALUE) {
				if (strap == CARD_TYPE_SD_UART)
					strncpy(handle->card_info->fw_name,
						SDUART9098_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
				else
					strncpy(handle->card_info->fw_name,
						SDSD9098_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
			} else {
				strncpy(handle->card_info->fw_name,
					SDUART9098_COMBO_V1_FW_NAME,
					FW_NAMW_MAX_LEN);
			}
			strncpy(handle->card_info->fw_name_wlan,
				SD9098_WLAN_V1_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		default:
			break;
		}
	}
#endif
#ifdef SD9097
	if (IS_SD9097(handle->card_type)) {
		switch (revision_id) {
		case SD9097_B0:
		case SD9097_B1:
			if (magic == CHIP_MAGIC_VALUE) {
				if (strap == CARD_TYPE_SD_UART)
					strncpy(handle->card_info->fw_name,
						SDUART9097_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
				else
					strncpy(handle->card_info->fw_name,
						SDSD9097_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
			}
			strncpy(handle->card_info->fw_name_wlan,
				SD9097_WLAN_V1_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		default:
			break;
		}
	}
#endif
#ifdef SDAW693
	if (IS_SDAW693(handle->card_type)) {
		magic &= 0x03;
		if (magic == 0x03)
			PRINTM(MMSG, "wlan: SDAW693 in secure-boot mode\n");

		switch (revision_id) {
		case SDAW693_A0:
			if (strap == CARD_TYPE_SDAW693_UART)
				strncpy(handle->card_info->fw_name,
					SDUARTAW693_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDSDAW693_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SDAW693_DEFAULT_WLAN_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SDAW693_A1:
			if (strap == CARD_TYPE_SDAW693_UART)
				strncpy(handle->card_info->fw_name,
					SDUARTAW693_COMBO_V1_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDSDAW693_COMBO_V1_FW_NAME,
					FW_NAMW_MAX_LEN);
			strncpy(handle->card_info->fw_name_wlan,
				SDAW693_WLAN_V1_FW_NAME, FW_NAMW_MAX_LEN);
			if (magic != 0x03) {
				/* remove extension .se */
				se_pos = strstr(handle->card_info->fw_name,
						".se");
				if (se_pos) {
					memset(se_pos, '\0', sizeof(".se"));
				}

				se_pos = strstr(handle->card_info->fw_name_wlan,
						".se");
				if (se_pos) {
					memset(se_pos, '\0', sizeof(".se"));
				}
			}
			break;
		default:
			break;
		}
	}
#endif

#ifdef SDIW624
	if (IS_SDIW624(handle->card_type)) {
		magic &= 0x03;
		if (magic == 0x03)
			PRINTM(MMSG, "wlan: SDIW624 in secure-boot mode\n");
		if (strap == CARD_TYPE_SDIW624_UARTSPI) {
			if (handle->params.dual_nb)
				strncpy(handle->card_info->fw_name,
					SDUARTSPIIW624_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDUARTIW624_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		} else if (strap == CARD_TYPE_SDIW624_UARTUART) {
			if (handle->params.dual_nb)
				strncpy(handle->card_info->fw_name,
					SDUARTUARTIW624_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDUARTIW624_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		} else {
			strncpy(handle->card_info->fw_name,
				SDSDIW624_COMBO_FW_NAME, FW_NAMW_MAX_LEN);
		}
	}
#endif

#ifdef SD9177
	if (IS_SD9177(handle->card_type)) {
		switch (revision_id) {
		case SD9177_A0:
			if (magic == CHIP_MAGIC_VALUE) {
				if (strap == CARD_TYPE_SD9177_UART)
					strncpy(handle->card_info->fw_name,
						SDUART9177_DEFAULT_COMBO_FW_NAME,
						FW_NAMW_MAX_LEN);
				else
					strncpy(handle->card_info->fw_name,
						SDSD9177_DEFAULT_COMBO_FW_NAME,
						FW_NAMW_MAX_LEN);
			}
			strncpy(handle->card_info->fw_name_wlan,
				SD9177_DEFAULT_WLAN_FW_NAME, FW_NAMW_MAX_LEN);
			break;
		case SD9177_A1:
			if (magic == CHIP_MAGIC_VALUE) {
				if (strap == CARD_TYPE_SD9177_UART) {
					if (handle->params.rf_test_mode)
						strncpy(handle->card_info
								->fw_name,
							SDUART9177_DEFAULT_RFTM_COMBO_V1_FW_NAME,
							FW_NAMW_MAX_LEN);
					else
						strncpy(handle->card_info
								->fw_name,
							SDUART9177_DEFAULT_COMBO_V1_FW_NAME,
							FW_NAMW_MAX_LEN);
				} else {
					if (handle->params.rf_test_mode)
						strncpy(handle->card_info
								->fw_name,
							SDSD9177_DEFAULT_RFTM_COMBO_V1_FW_NAME,
							FW_NAMW_MAX_LEN);
					else
						strncpy(handle->card_info
								->fw_name,
							SDSD9177_DEFAULT_COMBO_V1_FW_NAME,
							FW_NAMW_MAX_LEN);
				}
			} else {
				if (handle->params.rf_test_mode)
					strncpy(handle->card_info->fw_name,
						SDUART9177_DEFAULT_RFTM_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
				else
					strncpy(handle->card_info->fw_name,
						SD9177_DEFAULT_COMBO_V1_FW_NAME,
						FW_NAMW_MAX_LEN);
			}
			if (handle->params.rf_test_mode)
				strncpy(handle->card_info->fw_name,
					SD9177_DEFAULT_RFTM_WLAN_V1_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name_wlan,
					SD9177_DEFAULT_WLAN_V1_FW_NAME,
					FW_NAMW_MAX_LEN);
			break;
		default:
			break;
		}
	}
#endif

#ifdef SDIW610
	if (IS_SDIW610(handle->card_type)) {
		magic &= 0x03;
		if (magic == 0x03)
			PRINTM(MMSG, "wlan: SDIW610 in secure-boot mode\n");
		if (strap == CARD_TYPE_SDIW610_UART) {
			if (handle->params.dual_nb)
				strncpy(handle->card_info->fw_name,
					SDUARTSPIIW610_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
			else
				strncpy(handle->card_info->fw_name,
					SDUARTIW610_COMBO_FW_NAME,
					FW_NAMW_MAX_LEN);
		}
		strncpy(handle->card_info->fw_name_wlan,
			SDIW610_DEFAULT_WLAN_FW_NAME, FW_NAMW_MAX_LEN);
		if (magic != 0x03) {
			/* remove extension .se */
			se_pos = strstr(handle->card_info->fw_name, ".se");
			if (se_pos) {
				memset(se_pos, '\0', sizeof(".se"));
			}

			se_pos = strstr(handle->card_info->fw_name_wlan, ".se");
			if (se_pos) {
				memset(se_pos, '\0', sizeof(".se"));
			}
		}
	}
#endif
done:
	PRINTM(MCMND, "combo fw:%s wlan fw:%s \n", handle->card_info->fw_name,
	       handle->card_info->fw_name_wlan);
	LEAVE();
	return ret;
}

#define DEBUG_FW_DONE 0xFF
#define DEBUG_MEMDUMP_FINISH 0xFE
#define MAX_POLL_TRIES 100

#define HOST_TO_CARD_EVENT_REG 0x00
#define HOST_TO_CARD_EVENT MBIT(3)
#define HOST_RST_EVENT MBIT(4)

typedef enum {
	DUMP_TYPE_ITCM = 0,
	DUMP_TYPE_DTCM = 1,
	DUMP_TYPE_SQRAM = 2,
	DUMP_TYPE_APU_REGS = 3,
	DUMP_TYPE_CIU_REGS = 4,
	DUMP_TYPE_ICU_REGS = 5,
	DUMP_TYPE_MAC_REGS = 6,
	DUMP_TYPE_EXTEND_7 = 7,
	DUMP_TYPE_EXTEND_8 = 8,
	DUMP_TYPE_EXTEND_9 = 9,
	DUMP_TYPE_EXTEND_10 = 10,
	DUMP_TYPE_EXTEND_11 = 11,
	DUMP_TYPE_EXTEND_12 = 12,
	DUMP_TYPE_EXTEND_13 = 13,
	DUMP_TYPE_EXTEND_LAST = 14
} dumped_mem_type;

#define MAX_NAME_LEN 8

typedef struct {
	t_u8 mem_name[MAX_NAME_LEN];
	t_u8 *mem_Ptr;
	struct file *pfile_mem;
	t_u8 done_flag;
	t_u8 type;
} memory_type_mapping;

static memory_type_mapping mem_type_mapping_tbl[] = {
	{"ITCM", NULL, NULL, 0xF0, FW_DUMP_TYPE_MEM_ITCM},
	{"DTCM", NULL, NULL, 0xF1, FW_DUMP_TYPE_MEM_DTCM},
	{"SQRAM", NULL, NULL, 0xF2, FW_DUMP_TYPE_MEM_SQRAM},
	{"APU", NULL, NULL, 0xF3, FW_DUMP_TYPE_REG_APU},
	{"CIU", NULL, NULL, 0xF4, FW_DUMP_TYPE_REG_CIU},
	{"ICU", NULL, NULL, 0xF5, FW_DUMP_TYPE_REG_ICU},
	{"MAC", NULL, NULL, 0xF6, FW_DUMP_TYPE_REG_MAC},
	{"EXT7", NULL, NULL, 0xF7, 0},
	{"EXT8", NULL, NULL, 0xF8, 0},
	{"EXT9", NULL, NULL, 0xF9, 0},
	{"EXT10", NULL, NULL, 0xFA, 0},
	{"EXT11", NULL, NULL, 0xFB, 0},
	{"EXT12", NULL, NULL, 0xFC, 0},
	{"EXT13", NULL, NULL, 0xFD, 0},
	{"EXTLAST", NULL, NULL, 0xFE, 0},
};
static memory_type_mapping mem_type_mapping_tbl_8977_8997 = {"DUMP", NULL, NULL,
							     0xDD, 0};
/**
 *  @brief This function read/write firmware via cmd52
 *
 *  @param phandle   A pointer to moal_handle
 *  @param doneflag  A flag
 *  @param trigger   trigger FW dump flag
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
static rdwr_status woal_cmd52_rdwr_firmware(moal_handle *phandle, t_u8 doneflag,
					    t_u8 trigger)
{
	int ret = 0;
	int tries = 0;
	t_u8 ctrl_data = 0;
	t_u8 dbg_dump_ctrl_reg = phandle->card_info->dump_fw_ctrl_reg;
	t_u8 debug_host_ready = phandle->card_info->dump_fw_host_ready;

#if defined(SDAW693) || defined(SD9177) || defined(SD9098)
	if (IS_SD9177(phandle->card_type) || IS_SDAW693(phandle->card_type) ||
	    IS_SD9098(phandle->card_type)) {
		if (phandle->event_fw_dump)
			debug_host_ready = 0xAA;
	}
#endif
	ret = woal_sdio_writeb(phandle, dbg_dump_ctrl_reg, debug_host_ready);
	if (ret) {
		PRINTM(MERROR, "SDIO Write ERR\n");
		return RDWR_STATUS_FAILURE;
	}
	if (trigger) {
		PRINTM(MMSG, "Trigger FW dump...\n");
		if (IS_SDIW610(phandle->card_type)) {
			ret = woal_sdio_writeb(phandle, HOST_TO_CARD_EVENT_REG,
					       HOST_RST_EVENT);
		} else {
			ret = woal_sdio_writeb(phandle, HOST_TO_CARD_EVENT_REG,
					       HOST_TO_CARD_EVENT);
		}
		if (ret) {
			PRINTM(MERROR, "Fail to set HOST_TO_CARD_EVENT_REG\n");
			return RDWR_STATUS_FAILURE;
		}
	}

#if defined(SDAW693) || defined(SD9177) || defined(SD9098)
	if (IS_SD9177(phandle->card_type) || IS_SDAW693(phandle->card_type) ||
	    IS_SD9098(phandle->card_type)) {
		if (phandle->event_fw_dump)
			return RDWR_STATUS_SUCCESS;
	}
#endif
	for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
		ret = woal_sdio_readb(phandle, dbg_dump_ctrl_reg, &ctrl_data);
		if (ret) {
			PRINTM(MERROR, "SDIO READ ERR\n");
			return RDWR_STATUS_FAILURE;
		}
		if (ctrl_data == DEBUG_FW_DONE)
			break;
		if (doneflag && ctrl_data == doneflag)
			return RDWR_STATUS_DONE;
		if (ctrl_data != debug_host_ready) {
			PRINTM(MMSG,
			       "The ctrl reg was changed, re-try again!\n");
			ret = woal_sdio_writeb(phandle, dbg_dump_ctrl_reg,
					       debug_host_ready);
			if (ret) {
				PRINTM(MERROR, "SDIO Write ERR\n");
				return RDWR_STATUS_FAILURE;
			}
		}
		udelay(100);
	}
	if (ctrl_data == debug_host_ready || tries == MAX_POLL_TRIES) {
		PRINTM(MERROR, "Fail to pull ctrl_data\n");
		return RDWR_STATUS_FAILURE;
	}
	return RDWR_STATUS_SUCCESS;
}

/**
 *  @brief This function dump firmware memory to file
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         N/A
 */
void woal_dump_firmware_info_v2(moal_handle *phandle)
{
	int ret = 0;
	unsigned int reg, reg_start, reg_end;
	t_u8 *dbg_ptr = NULL;
	t_u32 sec, usec;
	t_u8 dump_num = 0;
	t_u8 idx = 0;
	t_u8 doneflag = 0;
	rdwr_status stat;
	t_u8 i = 0;
	t_u8 read_reg = 0;
	t_u32 memory_size = 0;
#ifndef DUMP_TO_PROC
	t_u8 path_name[64], file_name[32], firmware_dump_file[128];
#endif
	t_u8 *end_ptr = NULL;
	t_u8 dbg_dump_start_reg = 0;
	t_u8 dbg_dump_end_reg = 0;
	t_u8 dbg_dump_ctrl_reg = 0;

	if (!phandle) {
		PRINTM(MERROR, "Could not dump firmwware info\n");
		return;
	}

	dbg_dump_start_reg = phandle->card_info->dump_fw_start_reg;
	dbg_dump_end_reg = phandle->card_info->dump_fw_end_reg;
	dbg_dump_ctrl_reg = phandle->card_info->dump_fw_ctrl_reg;

#ifdef DUMP_TO_PROC
	if (!phandle->fw_dump_buf) {
		ret = moal_vmalloc(phandle, FW_DUMP_INFO_LEN,
				   &(phandle->fw_dump_buf));
		if (ret != MLAN_STATUS_SUCCESS || !phandle->fw_dump_buf) {
			PRINTM(MERROR, "Failed to vmalloc fw dump bufffer\n");
			return;
		}
	} else {
		memset(phandle->fw_dump_buf, 0x00, FW_DUMP_INFO_LEN);
	}
	phandle->fw_dump_len = 0;
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
	/** Create dump directort*/
	woal_create_dump_dir(phandle, path_name, sizeof(path_name));
#else
	memset(path_name, 0, sizeof(path_name));
	strncpy(path_name, "/data", sizeof(path_name));
#endif
	PRINTM(MMSG, "Directory name is %s\n", path_name);

	woal_dump_drv_info(phandle, path_name);
#endif

	/* start dump fw memory */
	moal_get_system_time(phandle, &sec, &usec);
	PRINTM(MMSG, "==== DEBUG MODE OUTPUT START: %u.%06u ====\n", sec, usec);
	/* read the number of the memories which will dump */
	if (RDWR_STATUS_FAILURE ==
	    woal_cmd52_rdwr_firmware(phandle, doneflag, MFALSE)) {
		if (RDWR_STATUS_FAILURE ==
		    woal_cmd52_rdwr_firmware(phandle, doneflag, MTRUE))
			goto done;
	}
	reg = dbg_dump_start_reg;
	ret = woal_sdio_readb(phandle, reg, &dump_num);
	if (ret) {
		PRINTM(MMSG, "SDIO READ MEM NUM ERR\n");
		goto done;
	}

	/* read the length of every memory which will dump */
	for (idx = 0; idx < dump_num; idx++) {
		if (RDWR_STATUS_FAILURE ==
		    woal_cmd52_rdwr_firmware(phandle, doneflag, MFALSE))
			goto done;
		memory_size = 0;
		reg = dbg_dump_start_reg;
		for (i = 0; i < 4; i++) {
			ret = woal_sdio_readb(phandle, reg, &read_reg);
			if (ret) {
				PRINTM(MMSG, "SDIO READ ERR\n");
				goto done;
			}
			memory_size |= (read_reg << i * 8);
			reg++;
		}
		if (memory_size == 0) {
			PRINTM(MMSG, "Firmware Dump Finished!\n");
			ret = woal_sdiommc_write_reg(phandle, dbg_dump_ctrl_reg,
						     DEBUG_MEMDUMP_FINISH);
			if (ret) {
				PRINTM(MERROR,
				       "SDIO Write MEMDUMP_FINISH ERR\n");
				goto done;
			}
			break;
		} else {
			PRINTM(MMSG, "%s_SIZE=0x%x\n",
			       mem_type_mapping_tbl[idx].mem_name, memory_size);
			ret = moal_vmalloc(
				phandle, memory_size + 1,
				(t_u8 **)&mem_type_mapping_tbl[idx].mem_Ptr);
			if ((ret != MLAN_STATUS_SUCCESS) ||
			    !mem_type_mapping_tbl[idx].mem_Ptr) {
				PRINTM(MERROR,
				       "Error: vmalloc %s buffer failed!!!\n",
				       mem_type_mapping_tbl[idx].mem_name);
				goto done;
			}
			dbg_ptr = mem_type_mapping_tbl[idx].mem_Ptr;
			end_ptr = dbg_ptr + memory_size;
		}
		doneflag = mem_type_mapping_tbl[idx].done_flag;
		moal_get_system_time(phandle, &sec, &usec);
		PRINTM(MMSG, "Start %s output %u.%06u, please wait...\n",
		       mem_type_mapping_tbl[idx].mem_name, sec, usec);
		do {
			stat = woal_cmd52_rdwr_firmware(phandle, doneflag,
							MFALSE);
			if (RDWR_STATUS_FAILURE == stat)
				goto done;
			reg_start = dbg_dump_start_reg;
			reg_end = dbg_dump_end_reg;
			for (reg = reg_start; reg <= reg_end; reg++) {
				ret = woal_sdio_readb(phandle, reg, dbg_ptr);
				if (ret) {
					PRINTM(MMSG, "SDIO READ ERR\n");
					goto done;
				}
				if (dbg_ptr < end_ptr)
					dbg_ptr++;
				else
					PRINTM(MINFO,
					       "pre-allocced buf is not enough\n");
			}
			if (RDWR_STATUS_DONE == stat) {
#ifdef MLAN_64BIT
				PRINTM(MMSG,
				       "%s done:"
				       "size = 0x%lx\n",
				       mem_type_mapping_tbl[idx].mem_name,
				       dbg_ptr - mem_type_mapping_tbl[idx]
							 .mem_Ptr);
#else
				PRINTM(MMSG,
				       "%s done:"
				       "size = 0x%x\n",
				       mem_type_mapping_tbl[idx].mem_name,
				       dbg_ptr - mem_type_mapping_tbl[idx]
							 .mem_Ptr);
#endif
#ifdef DUMP_TO_PROC
				woal_save_dump_info_to_buf(
					phandle,
					mem_type_mapping_tbl[idx].mem_Ptr,
					memory_size,
					mem_type_mapping_tbl[idx].type);
#else
				memset(file_name, 0, sizeof(file_name));
				snprintf(file_name, sizeof(file_name), "%s%s",
					 "file_sdio_",
					 mem_type_mapping_tbl[idx].mem_name);
				if (MLAN_STATUS_SUCCESS !=
				    woal_save_dump_info_to_file(
					    path_name, file_name,
					    mem_type_mapping_tbl[idx].mem_Ptr,
					    memory_size))
					PRINTM(MERROR,
					       "Can't save dump file %s in %s\n",
					       file_name, path_name);
#endif
				moal_vfree(phandle,
					   mem_type_mapping_tbl[idx].mem_Ptr);
				mem_type_mapping_tbl[idx].mem_Ptr = NULL;
				break;
			}
		} while (1);
	}
#ifdef DUMP_TO_PROC
	woal_append_end_block(phandle);
#endif
	moal_get_system_time(phandle, &sec, &usec);
	PRINTM(MMSG, "==== DEBUG MODE OUTPUT END: %u.%06u ====\n", sec, usec);
	/* end dump fw memory */
#ifndef DUMP_TO_PROC
	memset(firmware_dump_file, 0, sizeof(firmware_dump_file));
	snprintf(firmware_dump_file, sizeof(firmware_dump_file), "%s/%s",
		 path_name, file_name);
	moal_memcpy_ext(phandle, phandle->firmware_dump_file,
			firmware_dump_file, sizeof(firmware_dump_file),
			sizeof(phandle->firmware_dump_file));
#endif
done:
	for (idx = 0; idx < dump_num; idx++) {
		if (mem_type_mapping_tbl[idx].mem_Ptr) {
			moal_vfree(phandle, mem_type_mapping_tbl[idx].mem_Ptr);
			mem_type_mapping_tbl[idx].mem_Ptr = NULL;
		}
	}
	PRINTM(MMSG, "==== DEBUG MODE END ====\n");
	return;
}

static void woal_trigger_nmi_on_no_dump_event(moal_handle *phandle)
{
	int ret = 0;
	t_u8 ctrl_data = 0;
	t_u8 dbg_dump_ctrl_reg = phandle->card_info->dump_fw_ctrl_reg;
	phandle->init_wait_q_woken = MFALSE;
	ret = woal_sdio_readb(phandle, dbg_dump_ctrl_reg, &ctrl_data);
	if (ctrl_data == 0xAA) {
		ret = wait_event_timeout(phandle->init_wait_q,
					 phandle->init_wait_q_woken, 2 * HZ);
		if (!ret) {
			PRINTM(MMSG, "Trigger NMI FW dump...\n");
			ret = woal_sdio_writeb(phandle, HOST_TO_CARD_EVENT_REG,
					       HOST_RST_EVENT);
		}
	}
}

/**
 *  @brief This function dump firmware memory to file
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         N/A
 */
void woal_dump_firmware_info_v3(moal_handle *phandle)
{
	int ret = 0;
	int tries = 0;
	unsigned int reg, reg_start, reg_end;
	t_u8 *dbg_ptr = NULL;
	t_u8 *temp_Ptr = NULL;
	t_u32 sec, usec;
	t_u8 start_flag = 0;
	t_u8 doneflag = 0;
	rdwr_status stat;
	t_u32 memory_size = 0;
#ifndef DUMP_TO_PROC
	t_u8 path_name[64], file_name[32], firmware_dump_file[128];
	moal_handle *ref_handle;
#endif
	t_u8 *end_ptr = NULL;
	t_u8 dbg_dump_start_reg = 0;
	t_u8 dbg_dump_end_reg = 0;
	memory_type_mapping *pmem_type_mapping_tbl =
		&mem_type_mapping_tbl_8977_8997;

	if (!phandle) {
		PRINTM(MERROR, "Could not dump firmwware info\n");
		return;
	}
#if defined(SDAW693) || defined(SD9177) || defined(SD9098)
	if (IS_SD9177(phandle->card_type) || IS_SDAW693(phandle->card_type) ||
	    IS_SD9098(phandle->card_type)) {
		if (phandle->event_fw_dump) {
			if (RDWR_STATUS_FAILURE !=
			    woal_cmd52_rdwr_firmware(phandle, doneflag,
						     MFALSE)) {
				PRINTM(MMSG,
				       "====SDIO FW DUMP EVENT MODE START ====\n");
				return;
			}
			if (RDWR_STATUS_FAILURE !=
			    woal_cmd52_rdwr_firmware(phandle, doneflag,
						     MTRUE)) {
				PRINTM(MMSG,
				       "====SDIO FW DUMP EVENT MODE START ====\n");
				return;
			}
		}
	}
#endif

	dbg_dump_start_reg = phandle->card_info->dump_fw_start_reg;
	dbg_dump_end_reg = phandle->card_info->dump_fw_end_reg;

#ifndef DUMP_TO_PROC
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
	/** Create dump directort*/
	woal_create_dump_dir(phandle, path_name, sizeof(path_name));
#else
	memset(path_name, 0, sizeof(path_name));
	strncpy(path_name, "/data", sizeof(path_name));
#endif
	PRINTM(MMSG, "Directory name is %s\n", path_name);
	ref_handle = (moal_handle *)phandle->pref_mac;
	if (ref_handle)
		woal_dump_drv_info(ref_handle, path_name);
	woal_dump_drv_info(phandle, path_name);
#endif

	/* start dump fw memory */
	moal_get_system_time(phandle, &sec, &usec);
	PRINTM(MMSG, "==== DEBUG MODE OUTPUT START: %u.%06u ====\n", sec, usec);
	/* read the number of the memories which will dump */
	if (RDWR_STATUS_FAILURE ==
	    woal_cmd52_rdwr_firmware(phandle, doneflag, MFALSE)) {
		if (RDWR_STATUS_FAILURE ==
		    woal_cmd52_rdwr_firmware(phandle, doneflag, MTRUE))
			goto done;
	}

	/** check the reg which indicate dump starting */
	for (reg = dbg_dump_start_reg; reg <= dbg_dump_end_reg; reg++) {
		for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
			ret = woal_sdio_readb(phandle, reg, &start_flag);
			if (ret) {
				PRINTM(MMSG, "SDIO READ ERR\n");
				goto done;
			}
			/** 0 means dump starting*/
			if (start_flag == 0)
				break;
			udelay(100);
		}
		if (tries == MAX_POLL_TRIES) {
			PRINTM(MMSG, "FW not ready to dump\n");
			goto done;
		}
	}
	memory_size = 0xF0000;
	PRINTM(MMSG, "%s_SIZE=0x%x\n", pmem_type_mapping_tbl->mem_name,
	       memory_size);
	ret = moal_vmalloc(phandle, memory_size + 1,
			   (t_u8 **)&pmem_type_mapping_tbl->mem_Ptr);
	if ((ret != MLAN_STATUS_SUCCESS) || !pmem_type_mapping_tbl->mem_Ptr) {
		PRINTM(MERROR, "Error: vmalloc %s buffer failed!!!\n",
		       pmem_type_mapping_tbl->mem_name);
		goto done;
	}
	dbg_ptr = pmem_type_mapping_tbl->mem_Ptr;
	end_ptr = dbg_ptr + memory_size;
	doneflag = pmem_type_mapping_tbl->done_flag;
	moal_get_system_time(phandle, &sec, &usec);
	PRINTM(MMSG, "Start %s output %u.%06u, please wait...\n",
	       pmem_type_mapping_tbl->mem_name, sec, usec);
	do {
		stat = woal_cmd52_rdwr_firmware(phandle, doneflag, MFALSE);
		if (RDWR_STATUS_FAILURE == stat)
			goto done;
		reg_start = dbg_dump_start_reg;
		reg_end = dbg_dump_end_reg;
		for (reg = reg_start; reg <= reg_end; reg++) {
			ret = woal_sdio_readb(phandle, reg, dbg_ptr);
			if (ret) {
				PRINTM(MMSG, "SDIO READ ERR\n");
				goto done;
			}
			dbg_ptr++;
			if (dbg_ptr >= end_ptr) {
				PRINTM(MINFO,
				       "pre-allocced buf is not enough\n");

				ret = moal_vmalloc(phandle,
						   memory_size + 0x2000 + 1,
						   (t_u8 **)&temp_Ptr);
				if ((ret != MLAN_STATUS_SUCCESS) || !temp_Ptr) {
					PRINTM(MERROR,
					       "Error: vmalloc  buffer failed!!!\n");
					goto done;
				}

				moal_memcpy_ext(phandle, temp_Ptr,
						pmem_type_mapping_tbl->mem_Ptr,
						memory_size,
						memory_size + 0x2000);
				moal_vfree(phandle,
					   pmem_type_mapping_tbl->mem_Ptr);
				pmem_type_mapping_tbl->mem_Ptr = temp_Ptr;
				temp_Ptr = NULL;
				dbg_ptr = pmem_type_mapping_tbl->mem_Ptr +
					  memory_size;

				memory_size += 0x2000;
				end_ptr = pmem_type_mapping_tbl->mem_Ptr +
					  memory_size;
			}
		}
		if (RDWR_STATUS_DONE == stat) {
#ifdef MLAN_64BIT
			PRINTM(MMSG,
			       "%s done:"
			       "size = 0x%lx\n",
			       pmem_type_mapping_tbl->mem_name,
			       dbg_ptr - pmem_type_mapping_tbl->mem_Ptr);
#else
			PRINTM(MMSG,
			       "%s done:"
			       "size = 0x%x\n",
			       pmem_type_mapping_tbl->mem_name,
			       dbg_ptr - pmem_type_mapping_tbl->mem_Ptr);

#endif
#ifdef DUMP_TO_PROC
			if (phandle->fw_dump_buf) {
				moal_vfree(phandle, phandle->fw_dump_buf);
				phandle->fw_dump_buf = NULL;
				phandle->fw_dump_len = 0;
			}
			phandle->fw_dump_buf = pmem_type_mapping_tbl->mem_Ptr;
			phandle->fw_dump_len =
				dbg_ptr - pmem_type_mapping_tbl->mem_Ptr;
			pmem_type_mapping_tbl->mem_Ptr = NULL;
#else
			memset(file_name, 0, sizeof(file_name));
			snprintf(file_name, sizeof(file_name), "%s%s",
				 "file_sdio_", pmem_type_mapping_tbl->mem_name);
			if (MLAN_STATUS_SUCCESS !=
			    woal_save_dump_info_to_file(
				    path_name, file_name,
				    pmem_type_mapping_tbl->mem_Ptr,
				    dbg_ptr - pmem_type_mapping_tbl->mem_Ptr))
				PRINTM(MERROR,
				       "Can't save dump file %s in %s\n",
				       file_name, path_name);
			moal_vfree(phandle, pmem_type_mapping_tbl->mem_Ptr);
			pmem_type_mapping_tbl->mem_Ptr = NULL;
#endif
			break;
		}
	} while (1);
	moal_get_system_time(phandle, &sec, &usec);
	PRINTM(MMSG, "==== DEBUG MODE OUTPUT END: %u.%06u ====\n", sec, usec);
	/* end dump fw memory */
#ifndef DUMP_TO_PROC
	memset(firmware_dump_file, 0, sizeof(firmware_dump_file));
	snprintf(firmware_dump_file, sizeof(firmware_dump_file), "%s/%s",
		 path_name, file_name);
	moal_memcpy_ext(phandle, phandle->firmware_dump_file,
			firmware_dump_file, sizeof(firmware_dump_file),
			sizeof(phandle->firmware_dump_file));
#endif
done:
	if (pmem_type_mapping_tbl->mem_Ptr) {
		moal_vfree(phandle, pmem_type_mapping_tbl->mem_Ptr);
		pmem_type_mapping_tbl->mem_Ptr = NULL;
	}
	PRINTM(MMSG, "==== DEBUG MODE END ====\n");
	return;
}

/**
 *  @brief This function reads and displays SDIO registers for debugging
 *
 *  @param phandle  A pointer to moal_handle
 *
 *  @return         N/A
 */
static void woal_sdiommc_reg_dbg(moal_handle *phandle)
{
	int ret = 0;
	t_u8 loop, index = 0, func, data;
	unsigned int reg, reg_start, reg_end;
	unsigned int scratch_reg = phandle->card_info->scratch_reg;
	t_u8 *reg_table = phandle->card_info->dump_reg.reg_table;
	t_u8 reg_table_size = phandle->card_info->dump_reg.reg_table_size;
	char buf[256], *ptr;

	mlan_pm_wakeup_card(phandle->pmlan_adapter, MTRUE);

	reg = phandle->card_info->fw_stuck_code_reg;
	if (reg != 0) {
		ret = woal_sdio_readb(phandle, reg, &data);
		if (!ret && data) {
			PRINTM(MERROR, "FW in debug mode (0x%x)\n", data);
			if (data == FW_STUCK_CODE_VERSION_MISMATCH) {
				phandle->driver_init = MFALSE;
				PRINTM(MERROR,
				       "WLAN and BT FW version mismatch !! Redownload both FWs with correct and same version\n");
			}
		}
	}

	for (loop = 0; loop < 5; loop++) {
		memset(buf, 0, sizeof(buf));
		ptr = buf;
		if (loop == 0) {
			/* Read the registers of SDIO function0 */
			func = loop;
			reg_start = 0;
			reg_end = 9;
		} else if (loop == 1) {
			/* Read the registers of SDIO function1 */
			func = loop;
			reg_start = phandle->card_info->func1_reg_start;
			reg_end = phandle->card_info->func1_reg_end;
		} else if (loop == 2) {
			/* Read specific registers of SDIO function1 */
			index = 0;
			func = 1;
			reg_start = reg_table[index++];
			reg_end = reg_table[reg_table_size - 1];
		} else {
			/* Read the scratch registers of SDIO function1 */
			if (loop == 4)
				mdelay(100);
			func = 1;
			reg_start = scratch_reg;
			reg_end = scratch_reg + 23;
		}
		if (loop != 2)
			ptr += snprintf(ptr, sizeof(buf),
					"SDIO Func%d (%#x-%#x): ", func,
					reg_start, reg_end);
		else
			ptr += snprintf(ptr, sizeof(buf),
					"SDIO Func%d: ", func);
		for (reg = reg_start; reg <= reg_end;) {
			if (func == 0)
				ret = woal_sdio_f0_readb(phandle, reg, &data);
			else
				ret = woal_sdio_readb(phandle, reg, &data);
			if (loop == 2)
				ptr += snprintf(ptr, sizeof(buf), "(%#x) ",
						reg);
			if (!ret)
				ptr += snprintf(ptr, sizeof(buf), "%02x ",
						data);
			else {
				ptr += snprintf(ptr, sizeof(buf) - (ptr - buf),
						"ERR");
				break;
			}
			if (loop == 2 && reg < reg_end)
				reg = reg_table[index++];
			else
				reg++;
		}
		PRINTM(MMSG, "%s\n", buf);
	}
	mlan_pm_wakeup_card(phandle->pmlan_adapter, MFALSE);
}

/**
 *  @brief This function dump firmware memory to file
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         N/A
 */
static void woal_sdiommc_dump_fw_info(moal_handle *phandle)
{
	moal_private *priv = NULL;

	if (!phandle) {
		PRINTM(MERROR, "Could not dump firmwware info\n");
		return;
	}
#ifdef DUMP_TO_PROC
	if (phandle->fw_dump_buf) {
		PRINTM(MMSG, "FW dump already exist, free existing dump\n");
		moal_vfree(phandle, phandle->fw_dump_buf);
		phandle->fw_dump_buf = NULL;
		phandle->fw_dump_len = 0;
	}
#endif
	/** cancel all pending commands */
	mlan_ioctl(phandle->pmlan_adapter, NULL);

	mlan_pm_wakeup_card(phandle->pmlan_adapter, MTRUE);
	msleep(5);
	phandle->fw_dump = MTRUE;
	if (phandle->card_info->dump_fw_info == DUMP_FW_SDIO_V2) {
		woal_dump_firmware_info_v2(phandle);
	} else if (phandle->card_info->dump_fw_info == DUMP_FW_SDIO_V3) {
		woal_dump_firmware_info_v3(phandle);
		if (phandle->event_fw_dump) {
			woal_trigger_nmi_on_no_dump_event(phandle);
			queue_work(phandle->workqueue, &phandle->main_work);
			phandle->is_fw_dump_timer_set = MTRUE;
			woal_mod_timer(&phandle->fw_dump_timer,
				       MOAL_FW_DUMP_TIMER);
			return;
		}
	}
	phandle->fw_dump = MFALSE;
	woal_sdiommc_reg_dbg(phandle);
	if (!phandle->priv_num)
		return;
	priv = woal_get_priv(phandle, MLAN_BSS_ROLE_ANY);
	if (priv) {
		woal_send_fw_dump_complete_event(priv);
	} else {
		PRINTM(MERROR, "Failed to get private structure for BSS\n");
	}
	mlan_pm_wakeup_card(phandle->pmlan_adapter, MFALSE);
	queue_work(phandle->workqueue, &phandle->main_work);
	woal_process_hang(phandle);
	return;
}

/**
 *  @brief This function save sdio reg info
 *
 *  @param phandle   A pointer to moal_handle
 *  @param buf       A pointer buffer saving log
 *
 *  @return          The length of this log
 */
static int woal_sdiommc_dump_reg_info(moal_handle *phandle, t_u8 *drv_buf)
{
	char *drv_ptr = (char *)drv_buf;
	int ret = 0;
	t_u8 loop, index = 0, func, data;
	unsigned int reg, reg_start, reg_end;
	unsigned int scratch_reg = 0;
	t_u8 *reg_table = NULL;
	t_u8 reg_table_size = 0;
	char buf[256], *ptr;

	ENTER();

	if (!phandle || !drv_buf) {
		PRINTM(MMSG, "%s: can't retreive info\n", __func__);
		LEAVE();
		return 0;
	}

	scratch_reg = phandle->card_info->scratch_reg;
	reg_table = phandle->card_info->dump_reg.reg_table;
	reg_table_size = phandle->card_info->dump_reg.reg_table_size;

	mlan_pm_wakeup_card(phandle->pmlan_adapter, MTRUE);

	reg = phandle->card_info->fw_stuck_code_reg;
	if (reg != 0) {
		ret = woal_sdio_readb(phandle, reg, &data);
		if (!ret && data) {
			PRINTM(MERROR, "FW in debug mode (0x%x)\n", data);
			drv_ptr += snprintf(drv_ptr, MAX_BUF_LEN,
					    "FW in debug mode (0x%x)\n", data);
			if (data == FW_STUCK_CODE_VERSION_MISMATCH) {
				phandle->driver_init = MFALSE;
				PRINTM(MERROR,
				       "WLAN and BT FW version mismatch !! Redownload both FWs with correct and same version\n");
				drv_ptr += snprintf(
					drv_ptr, MAX_BUF_LEN,
					"WLAN and BT FW version mismatch !! Redownload both FWs with correct and same version!\n");
			}
		}
	}

	drv_ptr += snprintf(drv_ptr, MAX_BUF_LEN,
			    "--------sdio_reg_debug_info---------\n");
	for (loop = 0; loop < 5; loop++) {
		memset(buf, 0, sizeof(buf));
		ptr = buf;
		if (loop == 0) {
			/* Read the registers of SDIO function0 */
			func = loop;
			reg_start = 0;
			reg_end = 9;

		} else if (loop == 1) {
			/* Read the registers of SDIO function1 */
			func = loop;
			reg_start = phandle->card_info->func1_reg_start;
			reg_end = phandle->card_info->func1_reg_end;
		} else if (loop == 2) {
			/* Read specific registers of SDIO function1 */
			index = 0;
			func = 1;
			reg_start = reg_table[index++];
			reg_end = reg_table[reg_table_size - 1];
		} else {
			/* Read the scratch registers of SDIO function1 */
			if (loop == 4)
				mdelay(100);
			func = 1;
			reg_start = scratch_reg;
			reg_end = scratch_reg + 10;
		}
		if (loop != 2)
			ptr += snprintf(ptr, sizeof(buf),
					"SDIO Func%d (%#x-%#x): ", func,
					reg_start, reg_end);
		else
			ptr += snprintf(ptr, sizeof(buf),
					"SDIO Func%d: ", func);
		for (reg = reg_start; reg <= reg_end;) {
			if (func == 0)
				ret = woal_sdio_f0_readb(phandle, reg, &data);
			else
				ret = woal_sdio_readb(phandle, reg, &data);

			if (loop == 2)
				ptr += snprintf(ptr, sizeof(buf), "(%#x) ",
						reg);
			if (!ret)
				ptr += snprintf(ptr, sizeof(buf), "%02x ",
						data);
			else {
				ptr += snprintf(ptr, sizeof(buf), "ERR");
				break;
			}
			if (loop == 2 && reg < reg_end)
				reg = reg_table[index++];
			else
				reg++;
		}
		drv_ptr += snprintf(drv_ptr, MAX_BUF_LEN, "%s\n", buf);
	}

	drv_ptr += snprintf(drv_ptr, MAX_BUF_LEN,
			    "--------sdio_reg_debug_info End---------\n");
	mlan_pm_wakeup_card(phandle->pmlan_adapter, MFALSE);

	LEAVE();
	return (int)(drv_ptr - (char *)drv_buf);
}

/**
 *  @brief This function reset sdio through sdio bus driver
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return          N/A
 */
void woal_sdio_reset_hw(moal_handle *handle)
{
	sdio_mmc_card *card = handle->card;
	struct sdio_func *func = card->func;
	ENTER();
	sdio_claim_host(func);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
	if (moal_extflg_isset(handle, EXT_INTMODE))
		woal_sdio_release_irq(card);
	else
#endif
		sdio_release_irq(card->func);
	sdio_disable_func(card->func);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
	mmc_hw_reset(func->card);
#else
	mmc_hw_reset(func->card->host);
#endif
#endif

#ifdef MMC_QUIRK_BLKSZ_FOR_BYTE_MODE
	/* The byte mode patch is available in kernel MMC driver
	 * which fixes one issue in MP-A transfer.
	 * bit1: use func->cur_blksize for byte mode
	 */
	func->card->quirks |= MMC_QUIRK_BLKSZ_FOR_BYTE_MODE;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0)
	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
	/* wait for chip fully wake up */
	if (!func->enable_timeout)
		func->enable_timeout = 200;
#endif
	sdio_enable_func(func);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 11, 0)
	if (moal_extflg_isset(handle, EXT_INTMODE))
		woal_sdio_claim_irq(card, woal_sdio_interrupt);
	else
#endif
		sdio_claim_irq(func, woal_sdio_interrupt);
	sdio_set_block_size(card->func, handle->sdio_blk_size);
	sdio_release_host(func);
	LEAVE();
	return;
}

/**
 *  @brief This function check fw winner status
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return        1--success, otherwise failure
 */
static BOOLEAN woal_sdiommc_check_winner_status(moal_handle *handle)
{
	t_u32 value = 1;
	t_u32 winner_status_reg = handle->card_info->fw_winner_status_reg;

	ENTER();
	handle->ops.read_reg(handle, winner_status_reg, &value);
	LEAVE();
	return (value == 0);
}

/**
 *  @brief This function reload fw
 *
 *  @param handle   A pointer to moal_handle structure
 *
 *  @return        0--success, otherwise failure
 */
static int woal_sdiommc_reset_fw(moal_handle *handle)
{
	int ret = 0, tries = 0;
	t_u32 value = 1;
	t_u32 reset_reg = handle->card_info->fw_reset_reg;
	t_u8 reset_val = handle->card_info->fw_reset_val;
	t_u32 wakeup_reg = handle->card_info->fw_wakeup_reg;
	t_u8 wakeup_val = handle->card_info->fw_wakeup_val;

	ENTER();
	/** write power up bit to wakeu device */
	if (handle->ops.write_reg(handle, wakeup_reg, wakeup_val) !=
	    MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to write wakeup register.\n");
		ret = -EFAULT;
		goto done;
	}
	udelay(2000);
	udelay(2000);
	/** wait SOC fully wake up */
	for (tries = 0; tries < MAX_POLL_TRIES; ++tries) {
		ret = handle->ops.write_reg(handle, reset_reg, 0xba);
		if (ret == MLAN_STATUS_SUCCESS) {
			handle->ops.read_reg(handle, reset_reg, &value);
			if (value == 0xba) {
				PRINTM(MMSG, "FW wake up\n");
				break;
			}
		}
		udelay(1000);
	}
	/* Write register to notify FW */
	if (handle->ops.write_reg(handle, reset_reg, reset_val) !=
	    MLAN_STATUS_SUCCESS) {
		PRINTM(MERROR, "Failed to write reset register.\n");
		ret = -EFAULT;
		goto done;
	}
#if defined(SD9098) || defined(SD9097) || defined(SDIW624) ||                  \
	defined(SDAW693) || defined(SD9177) || defined(SDIW610)
	if (IS_SD9098(handle->card_type) || IS_SD9097(handle->card_type) ||
	    IS_SDIW624(handle->card_type) || IS_SD9177(handle->card_type) ||
	    IS_SDIW610(handle->card_type) || IS_SDAW693(handle->card_type))
		handle->ops.write_reg(handle, 0x00, 0x10);
#endif
	/* Poll register around 100 ms */
	for (tries = 0; tries < MAX_POLL_TRIES; ++tries) {
		handle->ops.read_reg(handle, reset_reg, &value);
		if (value == 0)
			/* FW is ready */
			break;
		udelay(1000);
	}

	if (value) {
		PRINTM(MERROR, "Failed to poll FW reset register %X=0x%x\n",
		       reset_reg, value);
		ret = -EFAULT;
		goto done;
	} else {
		for (tries = 0; tries < 1000; ++tries) {
			if (woal_sdiommc_check_winner_status(handle)) {
				break;
			}
			udelay(1000);
		}
	}
	PRINTM(MMSG, "SDIO Trigger FW In-band Reset success");
done:
	LEAVE();
	return ret;
}

/**
 * @brief Function to process pre/post SDIO function level reset
 *
 * @param handle    A pointer to moal_handle structure
 * @param prepare   True :- its a prepare reset
 *		    False :- its a post reset
 * @param flr       True: flag for wifi-only firmware
 *
 * Note: This function is mix of woal_switch_drv_mode() and
 * remove_card(). Idea is to cleanup the software only.
 * post reset: add_card()
 *
 * @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status woal_do_sdiommc_flr(moal_handle *handle, bool prepare,
				       bool flr_flag)
{
	unsigned int i;
	int index = 0;
	mlan_status status = MLAN_STATUS_SUCCESS;
	moal_private *priv = NULL;
	int fw_serial_bkp = 0;
	sdio_mmc_card *card = NULL;

	ENTER();

	if (!handle) {
		PRINTM(MINFO, "\n Handle null during prepare=%d\n", prepare);
		LEAVE();
		return status;
	}
	card = (sdio_mmc_card *)handle->card;
	if (MOAL_ACQ_SEMAPHORE_BLOCK(&AddRemoveCardSem))
		goto exit_sem_err;

	if (!prepare)
		goto perform_init;

	if (!(handle->pmlan_adapter)) {
		PRINTM(MINFO, "\n Handle null 2 during prepare=%d\n", prepare);
		LEAVE();
		return status;
	}

	/* Reset all interfaces */
	priv = woal_get_priv(handle, MLAN_BSS_ROLE_ANY);
	mlan_disable_host_int(handle->pmlan_adapter);
	woal_reset_intf(priv, MOAL_IOCTL_WAIT, MTRUE);
	woal_clean_up(handle);
	mlan_ioctl(handle->pmlan_adapter, NULL);

	/* Shutdown firmware */
	handle->init_wait_q_woken = MFALSE;
	status = mlan_shutdown_fw(handle->pmlan_adapter);

	if (status == MLAN_STATUS_PENDING)
		wait_event_interruptible(handle->init_wait_q,
					 handle->init_wait_q_woken);

	if (atomic_read(&handle->rx_pending) ||
	    atomic_read(&handle->tx_pending) ||
	    atomic_read(&handle->ioctl_pending)) {
		PRINTM(MERROR,
		       "ERR: rx_pending=%d,tx_pending=%d,ioctl_pending=%d\n",
		       atomic_read(&handle->rx_pending),
		       atomic_read(&handle->tx_pending),
		       atomic_read(&handle->ioctl_pending));
	}

	unregister_inetaddr_notifier(&handle->woal_notifier);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0)
#if IS_ENABLED(CONFIG_IPV6)
	unregister_inet6addr_notifier(&handle->woal_inet6_notifier);
#endif
#endif

#ifdef WIFI_DIRECT_SUPPORT
#if defined(STA_CFG80211) && defined(UAP_CFG80211)
#if CFG80211_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
	/* Remove virtual interface */
	woal_remove_virtual_interface(handle);
#endif
#endif
#endif
	/* Remove interface */
	for (i = 0; i < handle->priv_num; i++)
		woal_remove_interface(handle, i);
	handle->priv_num = 0;
#if defined(STA_CFG80211) || defined(UAP_CFG80211)
	/* Unregister wiphy device and free */
	if (handle->wiphy) {
		wiphy_unregister(handle->wiphy);
		woal_cfg80211_free_bands(handle->wiphy);
		wiphy_free(handle->wiphy);
		handle->wiphy = NULL;
	}
#endif
	/* Unregister mlan */
	if (handle->pmlan_adapter) {
		mlan_unregister(handle->pmlan_adapter);
		if (atomic_read(&handle->lock_count) ||
		    atomic_read(&handle->malloc_count) ||
		    atomic_read(&handle->mbufalloc_count)) {
			PRINTM(MERROR,
			       "mlan has memory leak: lock_count=%d,"
			       " malloc_count=%d, mbufalloc_count=%d\n",
			       atomic_read(&handle->lock_count),
			       atomic_read(&handle->malloc_count),
			       atomic_read(&handle->mbufalloc_count));
		}
		handle->pmlan_adapter = NULL;
	}
	handle->fw_dump = MFALSE;

	goto exit;

perform_init:
	handle->priv_num = 0;

	/* Init SW */
	if (woal_init_sw(handle)) {
		PRINTM(MFATAL, "Software Init Failed\n");
		goto err_init_fw;
	}
	/* Init FW and HW */
	/* Load wlan only binary */
	if (flr_flag) {
		fw_serial_bkp = moal_extflg_isset(handle, EXT_FW_SERIAL);
		moal_extflg_clear(handle, EXT_FW_SERIAL);
		woal_update_firmware_name(handle);
	}
	if (woal_init_fw(handle)) {
		PRINTM(MFATAL, "Firmware Init Failed\n");
		woal_sdiommc_reg_dbg(handle);
		if (fw_serial_bkp)
			moal_extflg_set(handle, EXT_FW_SERIAL);
		goto err_init_fw;
	}
	if (flr_flag && fw_serial_bkp)
		moal_extflg_set(handle, EXT_FW_SERIAL);
exit:
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);

exit_sem_err:
	LEAVE();
	return status;

err_init_fw:
	if (handle->is_fw_dump_timer_set) {
		woal_cancel_timer(&handle->fw_dump_timer);
		handle->is_fw_dump_timer_set = MFALSE;
	}

	if ((handle->hardware_status == HardwareStatusFwReady) ||
	    (handle->hardware_status == HardwareStatusReady)) {
		PRINTM(MINFO, "shutdown mlan\n");
		handle->init_wait_q_woken = MFALSE;
		status = mlan_shutdown_fw(handle->pmlan_adapter);
		if (status == MLAN_STATUS_PENDING)
			wait_event_interruptible(handle->init_wait_q,
						 handle->init_wait_q_woken);
	}
#ifdef ANDROID_KERNEL
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
	wakeup_source_trash(handle->ws);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
	wakeup_source_trash(&handle->ws);
#else
	wake_lock_destroy(&handle->wake_lock);
#endif
#endif
#ifdef CONFIG_PROC_FS
	woal_proc_exit(handle);
#endif
	/* Unregister device */
	PRINTM(MINFO, "unregister device\n");
	woal_sdiommc_unregister_dev(handle);
	handle->surprise_removed = MTRUE;
#ifdef REASSOCIATION
	if (handle->reassoc_thread.pid)
		wake_up_interruptible(&handle->reassoc_thread.wait_q);
	/* waiting for main thread quit */
	while (handle->reassoc_thread.pid)
		woal_sched_timeout(2);
#endif /* REASSOCIATION */
	if (moal_extflg_isset(handle, EXT_NAPI))
		netif_napi_del(&handle->napi_rx);
	woal_terminate_workqueue(handle);
	woal_free_moal_handle(handle);

	for (index = 0; index < MAX_MLAN_ADAPTER; index++) {
		if (m_handle[index] == handle)
			break;
	}
	if (index < MAX_MLAN_ADAPTER)
		m_handle[index] = NULL;
	card->handle = NULL;
	MOAL_REL_SEMAPHORE(&AddRemoveCardSem);
	LEAVE();
	return (mlan_status)MLAN_STATUS_FAILURE;
}

/**
 *  @brief This function handle the sdio work
 *
 *  @param WORK   A pointer to work_struct
 *  @return         N/A
 *
 */
static void woal_sdiommc_work(struct work_struct *work)
{
	sdio_mmc_card *card = NULL;
	moal_handle *handle = NULL;
	moal_handle *ref_handle = NULL;

	// Validate work pointer before container_of operation
	if (!work) {
		PRINTM(MERROR, "Invalid work structure pointer\n");
		return;
	}

	// Use safe container_of with proper type validation
	// Coverity raised error for kernel API
	// coverity[cert_arr39_c_violation:SUPPRESS]
	card = container_of(work, sdio_mmc_card, reset_work);

	// Validate the resulting card pointer
	if (!card) {
		PRINTM(MERROR,
		       "Failed to get sdio_mmc_card from work structure\n");
		return;
	}

	PRINTM(MMSG, "========START IN-BAND RESET===========\n");
	handle = card->handle;
	// handle-> mac0 , ref_handle->second mac
	if (handle->pref_mac) {
		if (handle->second_mac) {
			handle = (moal_handle *)handle->pref_mac;
			ref_handle = (moal_handle *)handle->pref_mac;
		} else {
			ref_handle = (moal_handle *)handle->pref_mac;
		}
		if (ref_handle) {
			ref_handle->surprise_removed = MTRUE;
			woal_clean_up(ref_handle);
			mlan_ioctl(ref_handle->pmlan_adapter, NULL);
		}
	}
	handle->surprise_removed = MTRUE;
	handle->fw_reseting = MTRUE;
	woal_do_sdiommc_flr(handle, true, true);
	if (ref_handle) {
		ref_handle->surprise_removed = MTRUE;
		ref_handle->fw_reseting = MTRUE;
		woal_do_sdiommc_flr(ref_handle, true, true);
	}
	if (woal_sdiommc_reset_fw(handle)) {
		PRINTM(MERROR, "SDIO In-band Reset Fail\n");
		woal_send_auto_recovery_failure_event(handle);
		wifi_status = WIFI_STATUS_FW_RECOVERY_FAIL;
		return;
	}

	handle->surprise_removed = MFALSE;

	woal_free_module_param(handle);
	woal_init_module_param(handle);

	if (MLAN_STATUS_SUCCESS == woal_do_sdiommc_flr(handle, false, true))
		handle->fw_reseting = MFALSE;
	else {
		handle = NULL;
		wifi_status = WIFI_STATUS_FW_RECOVERY_FAIL;
		return;
	}

	if (ref_handle) {
		ref_handle->surprise_removed = MFALSE;

		woal_free_module_param(ref_handle);
		woal_init_module_param(ref_handle);

		if (MLAN_STATUS_SUCCESS ==
		    woal_do_sdiommc_flr(ref_handle, false, true))
			ref_handle->fw_reseting = MFALSE;
	}
	card->work_flags = MFALSE;
	wifi_status = WIFI_STATUS_OK;
	woal_send_auto_recovery_complete_event(handle);
	PRINTM(MMSG, "========END IN-BAND RESET===========\n");
	return;
}

/**
 *  @brief This function start reset_work
 *
 *  @param handle   A pointer to moal_handle structure
 *  @return         MTRUE/MFALSE
 *
 */
static void woal_sdiommc_card_reset(moal_handle *handle)
{
	sdio_mmc_card *card = handle->card;
	if (!card->work_flags) {
		card->work_flags = MTRUE;
		schedule_work(&card->reset_work);
	}
}

static moal_if_ops sdiommc_ops = {
	.register_dev = woal_sdiommc_register_dev,
	.unregister_dev = woal_sdiommc_unregister_dev,
	.read_reg = woal_sdiommc_read_reg,
	.write_reg = woal_sdiommc_write_reg,
	.read_data_sync = woal_sdiommc_read_data_sync,
	.write_data_sync = woal_sdiommc_write_data_sync,
	.get_fw_name = woal_sdiommc_get_fw_name,
	.dump_fw_info = woal_sdiommc_dump_fw_info,
	.dump_reg_info = woal_sdiommc_dump_reg_info,
	.reg_dbg = woal_sdiommc_reg_dbg,
	.card_reset = woal_sdiommc_card_reset,
	.is_second_mac = woal_sdiommc_is_second_mac,
};
