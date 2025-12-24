/** @file mlan_shc.c
 *
 *  @brief This file contains the secure host interface functions.
 *
 *
 *  Copyright 2025 NXP
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

#include "nanotls-device.h"
#include "nanotls-host.h"
#include "nanotls-common.h"

#ifdef INT_MAX
#undef INT_MAX
#endif

#ifdef UINT_MAX
#undef UINT_MAX
#endif

#include "mlan.h"
#include "mlan_init.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_shc.h"

/********************************************************
			Local Variables
********************************************************/

enum action_status {
	ACTION_HANDSHAKE = 0,
	ACTION_HANDSHAKE_SUCCESS = 1,
	ACTION_HANDSHAKE_FAIL = 2,
};

/********************************************************
			Global Variables
********************************************************/
#define SECURE_HOST_TAG_LEN 16

/********************************************************
			Local Functions
********************************************************/
/**
 *  @brief This function decrypts command response received
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @param buf        A pointer to command payload
 *  @param len        Command payload length
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
static mlan_status mlan_shc_data_decrypt(pmlan_adapter pmadapter, t_u8 *buf,
					 t_u32 len)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;

	pmlan_callbacks pcb = &pmadapter->callbacks;
	t_void *enc_data = MNULL;

	ret = pcb->moal_malloc(pmadapter->pmoal_handle, len, MLAN_MEM_DEF,
			       (t_u8 **)&enc_data);
	if (ret != MLAN_STATUS_SUCCESS)
		return ret;

	ret = pcb->moal_secure_host_data_decrypt(pmadapter->pmoal_handle,
						 &enc_data, (void *)&buf, len);
	pcb->moal_mfree(pmadapter->pmoal_handle, enc_data);

	return ret;
}

/**
 *  @brief This function encrypts command request to be transmitted
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @param buf        A pointer to command payload
 *  @param len        Command payload length
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
static mlan_status mlan_shc_data_encrypt(pmlan_adapter pmadapter, t_u8 *buf,
					 t_u32 len)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	pmlan_callbacks pcb = &pmadapter->callbacks;
	t_void *enc_data = MNULL;

	ret = pcb->moal_malloc(pmadapter->pmoal_handle, len, MLAN_MEM_DEF,
			       (t_u8 **)&enc_data);
	if (ret != MLAN_STATUS_SUCCESS)
		return ret;

	ret = pcb->moal_secure_host_data_encrypt(pmadapter->pmoal_handle,
						 &enc_data, (void *)&buf, len);
	pcb->moal_mfree(pmadapter->pmoal_handle, enc_data);

	return ret;
}

/**
 *  @brief This function prepares secure host handshake msg to be sent to fw
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @msg              A pointer to message
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
static mlan_status mlan_shc_prepare_msg(pmlan_adapter pmadapter, t_void *msg)
{
	mlan_private *pmpriv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);

	mlan_status ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_SECURE_HOST, 0,
					   0, MNULL, msg);
	if (ret != MLAN_STATUS_SUCCESS)
		PRINTM(MERROR, "secure host: command prepare failed\n");

	return ret;
}

/**
 *  @brief This function initializes data path post secure host handshake
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
static mlan_status mlan_shc_data_path_init(pmlan_adapter pmadapter)
{
	pmlan_callbacks pcb = &pmadapter->callbacks;

	if (!pcb->moal_secure_host_derive_traffic_keys(
		    pmadapter->pmoal_handle) &&
	    !pcb->moal_secure_host_data_ctx_init(pmadapter->pmoal_handle))
		return MLAN_STATUS_SUCCESS;

	return MLAN_STATUS_FAILURE;
}

/********************************************************
			Global Functions
********************************************************/
/**
 *  @brief This function determines if command in secured or non-secured
 *
 *  @param cmd_id     Unique command id
 *  @return           secured:MTRUE, MFALSE otherwise
 */
t_bool wlan_is_secure_host_cmd(t_u16 cmd_id)
{
	switch (cmd_id) {
		/** non-secure commands sent prior host and fw handshake */
	case HostCmd_CMD_SECURE_HOST:
	case HostCmd_CMD_FUNC_INIT:
	case HostCmd_CMD_GET_HW_SPEC:
	case HostCmd_CMD_MCLIENT_SCHEDULE_CFG:
	case HostCmd_CMD_802_11_SNMP_MIB:
	case HostCmd_CMD_RECONFIGURE_TX_BUFF:
	case HostCmd_CMD_802_11_FW_WAKE_METHOD:
	case HostCmd_CMD_PCIE_ADMA_INIT:
	case HostCmd_CMD_802_11_RF_ANTENNA:
	case HostCmd_CMD_DBGS_CFG:
		return MFALSE;

	/** list of secure host commands */
	case HostCmd_CMD_802_11_KEY_MATERIAL:
	case HostCmd_CMD_SUPPLICANT_PMK:
	case HostCmd_CMD_SUPPLICANT_PROFILE:
	case HostCmd_CMD_REGION_POWER_CFG:
	case HostCmd_CMD_VERSION_EXT:
	case HostCmd_CMD_CFG_DATA:
		return MTRUE;
	}

	return MTRUE;
}

/**
 *  @brief This function process command request to be transmitted
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @param pcmd       A pointer to command request
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status wlan_shc_secure_hostcmd_process(pmlan_adapter pmadapter,
					    HostCmd_DS_COMMAND *pcmd)
{
	t_u32 cmd_size = 0;

	if ((!(pcmd->command & HostCmd_Encrypted_BIT)) &&
	    pcmd->size >= S_DS_GEN &&
	    (mlan_shc_data_encrypt(pmadapter, ((t_u8 *)pcmd + S_DS_GEN),
				   (pcmd->size - S_DS_GEN)) !=
	     MLAN_STATUS_FAILURE)) {
		pcmd->command |= HostCmd_Encrypted_BIT;
		cmd_size = wlan_le16_to_cpu(pcmd->size);
		cmd_size += SECURE_HOST_TAG_LEN;
		pcmd->size = wlan_cpu_to_le16(cmd_size);

		return MLAN_STATUS_SUCCESS;
	}

	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief This function process received command response
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @param resp       A pointer to command response
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status wlan_shc_secure_hostresp_process(pmlan_adapter pmadapter,
					     HostCmd_DS_COMMAND *resp)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;

	t_u16 cmd = wlan_le16_to_cpu(resp->command);
	t_u16 cmd_size = wlan_le16_to_cpu(resp->size);

	if ((cmd & HostCmd_Encrypted_BIT)) {
		if (cmd_size <= S_DS_GEN)
			return MLAN_STATUS_FAILURE;

		ret = mlan_shc_data_decrypt(pmadapter,
					    ((t_u8 *)resp + S_DS_GEN),
					    (cmd_size - S_DS_GEN));
		if (ret != MLAN_STATUS_FAILURE) {
			resp->command &= ~HostCmd_Encrypted_BIT;
			resp->size = wlan_le16_to_cpu(resp->size -
						      SECURE_HOST_TAG_LEN);
			DBG_HEXDUMP(MSHC_D, "Decrypted CMD_RESP", resp,
				    resp->size);
		}
	}

	return ret;
}

/**
 *  @brief This function handles firmware event post secure host handshake
 *
 *  @param pmadapter  A pointer to mlan private structure
 *  @param type       A pointer to event data
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status wlan_shc_process_secure_host_event(pmlan_private pmpriv, t_u8 *data,
					       t_u32 len)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	pmlan_adapter pmadapter = pmpriv->adapter;
	pmlan_callbacks pcb = &pmadapter->callbacks;
	t_u8 type;

	SECURE_HOST_EVENT_HEADER *pevent = (SECURE_HOST_EVENT_HEADER *)data;

	switch (pevent->tls_action) {
	case ACTION_HANDSHAKE:
		type = pcb->moal_secure_host_get_msg_id(
			data + sizeof(SECURE_HOST_EVENT_HEADER));
		ret = mlan_shc_handshake(
			pmadapter, type,
			data + sizeof(SECURE_HOST_EVENT_HEADER));
		if (ret != MLAN_STATUS_SUCCESS) {
			pmadapter->hw_status = WlanHardwareStatusNotReady;
			wlan_init_fw_complete(pmadapter);
		}
		break;
	case ACTION_HANDSHAKE_SUCCESS:
		PRINTM(MMSG, "secure host handshake success\n");
		if (!mlan_shc_data_path_init(pmadapter)) {
			wlan_adapter_get_hw_spec(pmadapter);
			ret = MLAN_STATUS_SUCCESS;
		} else {
			PRINTM(MERROR, "secure host data init failed\n");
		}
		break;
	case ACTION_HANDSHAKE_FAIL:
		PRINTM(MERROR,
		       "secure host handshake fail, event status (0x%x) received\n",
		       pevent->tls_action);
		break;
	default:
		PRINTM(MERROR,
		       "secure host, invalid event status (0x%x) received\n");
		break;
	}

	return ret;
}

/**
 *  @brief This function handles security handshake between host and FW
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *  @param type       One of the three message types
 *  @return           success:MLAN_STATUS_SUCCESS, MLAN_STATUS_FAILURE otherwise
 */
mlan_status mlan_shc_handshake(pmlan_adapter pmadapter, t_u8 type, t_void *msg)
{
	mlan_status ret = MLAN_STATUS_FAILURE;
	pmlan_callbacks pcb = &pmadapter->callbacks;
	t_void *buf = MNULL;

	switch (type) {
	case TLS_HOST_HELLO:
		if (!pcb->moal_secure_host_init(pmadapter->pmoal_handle,
						pmadapter->key,
						pmadapter->uuid) &&
		    !pcb->moal_secure_host_do_hello(pmadapter->pmoal_handle,
						    &buf)) {
			ret = mlan_shc_prepare_msg(pmadapter, buf);
		}
		break;
	case TLS_DEVICE_HELLO:
		/** Process TLS device hello (M2) message */
		if (pcb->moal_secure_host_device_hello_rcvd(
			    pmadapter->pmoal_handle, msg))
			break;
		fallthrough;
	case TLS_HOST_FINISHED:
		if (!pcb->moal_secure_host_do_finished(pmadapter->pmoal_handle,
						       &buf) &&
		    !mlan_shc_prepare_msg(pmadapter, buf)) {
			pmadapter->hw_status = WlanHardwareStatusGetHwSpec;
			ret = MLAN_STATUS_SUCCESS;
		}
		break;
	default:
		PRINTM(MMSG, "Invalid shc message type:%x\n", type);
		break;
	}

	return ret;
}
