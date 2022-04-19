/** @file  mlanwls.c
 *
 * @brief 11mc/11az Wifi location services application
 *
 *
 * Copyright 2022 NXP
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
/************************************************************************
Change log:
     01/24/2022: initial version
************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/ethernet.h>
#include "mlanutl.h"
#include "mlanwls.h"

/** WLS application's version number */
#define WLS_VER "M1.0"

/*Command arguments index*/
#define NXP_ADDR "530 Holgerway SanJose"

#define PROTO_DOT11AZ 1
#define PROTO_DOT11MC 0

#define FTM_SUBCMD_INDEX 3
#define FTM_CFG_SET_CMD_LEN 6
#define FTM_CFG_GET_CMD_LEN 4
#define FTM_CFG_PROTOCOL_INDEX 4
#define FTM_CFG_FILE_ARG_INDEX 5

#define FTM_SESSION_SUBCMD_LEN 8
#define FTM_SESSION_SUBCMD_NONSTOP_LEN 7
#define FTM_SESSION_ACTION_OFFSET 4
#define FTM_SESSION_CHANNEL_OFFSET 5
#define FTM_SESSION_PEER_ADDR_OFFSET 6
#define FTM_SESSION_LOOP_OFFSET 7

#define FTM_ACTION_START 1
#define FTM_ACTION_STOP 2

/********************************************************
		Functions Declarations
********************************************************/

t_u8 hexc2bin(char chr);
t_u32 a2hex(char *s);
t_u32 a2hex_or_atoi(char *value);
t_void hexdump(char *prompt, t_void *p, t_s32 len, char delim);
extern char *config_get_line(char *s, int size, FILE *stream, int *line,
			     char **_pos);
int parse_line(char *line, char *args[], t_u16 args_count);
int mac2raw(char *mac, t_u8 *raw);

static t_void display_help(t_u32 n, char **data);
static int mlanwls_prepare_buffer(t_u8 *buffer, char *cmd, t_u32 num,
				  char *args[]);
static int mlanwls_send_ioctl(t_u8 *cmd_buf);
static void mlanwls_event_handler(int nl_sk);
static int mlanwls_init(void);
static int mlanwls_read_ftm_config(char *file_name);

static int process_subcommand(int argc, char *argv[]);
static int process_ftm_session_cfg(int argc, char *argv[], void *param);
static int process_ftm_session_ctrl(int argc, char *argv[], void *param);

static int process_dot11az_ntb_cfg(int argc, char *argv[], void *param);
static int process_dot11mc_ftm_cfg(int argc, char *argv[], void *param);
static int process_ftm_start(int argc, char *argv[], void *param);
static int process_ftm_stop(int argc, char *argv[], void *param);
static int process_ftm_hostcmd_resp(char *cmd_name, t_u8 *buf);
static int process_ftm_complete_event(t_u8 *buffer, t_u16 size, char *if_name);

static int get_connstatus(int *data);
static void print_event_drv_connected(t_u8 *buffer, t_u16 size);
static int drv_nlevt_handler(struct nlmsghdr *nlh, int bytes_read,
			     int *evt_conn);
static int read_event(int nl_sk, struct msghdr *msg, struct timeval *ptv);
static int get_netlink_num(int dev_index);
static int open_netlink(int dev_index);
static t_void mlanwls_terminate_handler(int signal);

/********************************************************
		Local Variables
********************************************************/
/** WLS app usage */
static char *mlanwls_help[] = {
	"Usage: ",
	"	mlanutl mlan0 ftm <subcommand> [<arg>]",
	"	where subcommand is,",
	"		session_cfg  [<ftm_protocol> <config_file>]",
	"		session_ctrl [<action> <chan> <mac_address> <loop_cnt>]",
	"	For help on each subcommand,",
	"		mlanutl mlan0 ftm <subcommand> -h"
	""};

static char *ftm_session_cfg_help[] = {
	"Usage: ",
	"	mlanutl mlan0 ftm session_cfg [<config_file>] ",
	" 	where,",
	"	<ftm_protocol> : 0:Dot11mc, 1:Dot11az_ntb",
	"	<config_file> : Config file with Dot11mc / dot11az parameters",
	"",
	"	eg:	mlanutl mlan0 ftm session_cfg 0 config/ftm.conf - Sets dot11mc FTM session params from config file",
	"		mlanutl mlan0 ftm session_cfg 1 config/ftm.conf - Sets dot11az ntb session params from donfig file",
	" "};

static char *ftm_session_ctrl_help[] = {
	"Usage: ",
	"	mlanutl mlan0 ftm session_ctrl [<action> <chan> <mac_address> <loop_cnt>] ",
	" 	where,",
	"	<action> : 1: Start, 2: Stop ",
	"	<loop_cnt> : number of ftm sessions to run repeatedly ( default:1,  0:non-stop, n>1: n times)",
	"	<chan> 	: Channel on which FTM must be started",
	"	<mac_address> : Mac address of the peer with whom FTM session is required",
	" ",
	"	eg: mlanutl mlan0 ftm session_ctrl 1 6 00:50:43:20:bc:4  	- Starts FTM session and runs it once",
	"		mlanutl mlan0 ftm session_ctrl 1 6 00:50:43:20:bc:4 2 	- Starts FTM session and runs it for 2 cycles",
	"		mlanutl mlan0 ftm session_ctrl 1 6 00:50:43:20:bc:4 0	- Runs FTM session non stop until user terminate",
	"		mlanutl mlan0 ftm session_ctrl 2 						- Stop the FTM session",
	" "};

/** WLS app command ID */
enum user_command_id {
	FTM_SESSION_CFG_CMD_ID = 0,
	FTM_SESSION_CTRL_CMD_ID,
};

/** WLS app command table */
static wls_app_command_table wls_app_command[] = {
	{FTM_SESSION_CFG_CMD_ID, "session_cfg", process_ftm_session_cfg,
	 ftm_session_cfg_help},
	{FTM_SESSION_CTRL_CMD_ID, "session_ctrl", process_ftm_session_ctrl,
	 ftm_session_ctrl_help}};

/** WLS app data*/
wls_app_data_t gwls_data;

/**DBG Printf*/
#define DBG_LOG(x)                                                             \
	if (gwls_data.debug_level == 2) {                                      \
		printf(x);                                                     \
	}
#define DBG_ERROR(x)                                                           \
	if (gwls_data.debug_level >= 0) {                                      \
		printf(x);                                                     \
	}
#define PRINT_CFG(x, y) printf(x, y)

/********************************************************
		Global Variables
********************************************************/
/**< socket descriptor */
t_s32 nl_sk = 0; /**< netlink socket descriptor to receive an event */

/** Flag: is associated */
int assoc_flag = FALSE;

/** terminate flag */
int mlanwls_terminate_flag = FALSE;

/********************************************************
		Local Functions
********************************************************/
/**
 *  @brief Display help text
 *
 *  @return       NA
 */
static t_void display_help(t_u32 n, char **data)
{
	t_u32 i;
	for (i = 0; i < n; i++)
		fprintf(stderr, "%s\n", data[i]);
}

/**
 *  @brief Prepare command buffer
 *  @param buffer   Command buffer to be filled
 *  @param cmd      Command id
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int mlanwls_prepare_buffer(t_u8 *buffer, char *cmd, t_u32 num,
				  char *args[])
{
	t_u8 *pos = NULL;
	unsigned int i = 0;

	memset(buffer, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

	/* Flag it for our use */
	pos = buffer;
	memcpy((char *)pos, CMD_NXP, strlen(CMD_NXP));
	pos += (strlen(CMD_NXP));

	/* Insert command */
	strncpy((char *)pos, (char *)cmd, strlen(cmd));
	pos += (strlen(cmd));

	/* Insert arguments */
	for (i = 0; i < num; i++) {
		strncpy((char *)pos, args[i], strlen(args[i]));
		pos += strlen(args[i]);
		if (i < (num - 1)) {
			memcpy((char *)pos, " ", strlen(" "));
			pos += 1;
		}
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process host_cmd response
 *
 *  @param cmd_name	The command string
 *  @param buf		A pointer to the response buffer
 *
 *  @return      	MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_ftm_hostcmd_resp(char *cmd_name, t_u8 *buf)
{
	t_u32 hostcmd_size = 0;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	hostcmd_ds_ftm_session_cmd *phostcmd = NULL;

	buf += strlen(CMD_NXP) + strlen(cmd_name);
	memcpy((t_u8 *)&hostcmd_size, buf, sizeof(t_u32));
	buf += sizeof(t_u32);

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = le16_to_cpu(hostcmd->command);
	hostcmd->size = le16_to_cpu(hostcmd->size);

	hostcmd->command &= ~HostCmd_RET_BIT;

	switch (hostcmd->command) {
	case HostCmd_CMD_FTM_SESSION_CTRL:
		phostcmd = (hostcmd_ds_ftm_session_cmd *)buf;
		if (!le16_to_cpu(phostcmd->cmd_hdr.result)) {
			if (phostcmd->cmd.ftm_session_ctrl.action == 1) {
				printf("[INFO] FTM Session Control Started on %d channel with Peer %02X:%02X:%02X:%02X:%02X:%02X \n",
				       phostcmd->cmd.ftm_session_ctrl.chan,
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[0],
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[1],
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[2],
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[3],
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[4],
				       phostcmd->cmd.ftm_session_ctrl
					       .peer_mac[5]);
			} else if (phostcmd->cmd.ftm_session_ctrl.action == 2) {
				printf("[INFO] FTM Session Stopped\n");
			} else {
				printf("[ERROR] Hostcmd action not supported\n");
			}
		} else {
			printf("[ERROR] Hostcmd failed: ReturnCode=%#04x, Result=%#04x\n",
			       le16_to_cpu(phostcmd->cmd_hdr.command),
			       le16_to_cpu(phostcmd->cmd_hdr.result));
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		break;

	case HostCmd_CMD_FTM_SESSION_CFG:
		phostcmd = (hostcmd_ds_ftm_session_cmd *)buf;
		if ((le16_to_cpu(phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
					 .ntb_tlv.type) ==
		     FTM_NTB_RANGING_CFG_TLV_ID)) {
			if (le16_to_cpu(phostcmd->cmd.ftm_session_cfg.action) ==
			    MLAN_ACT_GET) {
				/* Get */
				printf("\n\nGet NTB Ranging Parameters: \n");
			} else {
				/* Set */
				printf("\n\nSet NTB Ranging Parameters: \n");
			}
			printf("---------------------------------\n");
			printf("format_bw:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
				       .ntb_tlv.val.format_bw);
			printf("az_measurement_freq:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
				       .ntb_tlv.val.az_measurement_freq);
			printf("az_number_of_measurements:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
				       .ntb_tlv.val.az_number_of_measurements);
			printf("max_i2r_sts_upto80:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
				       .ntb_tlv.val.max_i2r_sts_upto80);
			printf("max_r2i_sts_upto80:%d \n\n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az
				       .ntb_tlv.val.max_r2i_sts_upto80);
		} else if ((le16_to_cpu(phostcmd->cmd.ftm_session_cfg.tlv
						.cfg_11mc.sess_tlv.type) ==
			    FTM_SESSION_CFG_INITATOR_TLV_ID)) {
			if (le16_to_cpu(phostcmd->cmd.ftm_session_cfg.action) ==
			    MLAN_ACT_GET) {
				/* Get */
				printf("\n\nGet dot11mc ftm session config: \n");
			} else {
				/* Set */
				printf("\n\nSet dot11mc ftm session config: \n");
			}
			printf("---------------------------------\n");
			printf("burst_exponent:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.burst_exponent);
			printf("burst_duration:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.burst_duration);
			printf("min_delta_FTM:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.min_delta_FTM);
			printf("is_ASAP:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.is_ASAP);
			printf("per_burst_FTM:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.per_burst_FTM);
			printf("channel_spacing:%d \n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.channel_spacing);
			printf("burst_period:%d \n\n",
			       phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
				       .sess_tlv.val.burst_period);
		} else {
			printf("[ERROR] Hostcmd failed: Invalid TLV ReturnCode=%#04x, Result=%#04x\n",
			       le16_to_cpu(phostcmd->cmd_hdr.command),
			       le16_to_cpu(phostcmd->cmd_hdr.result));
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		break;

	default:
		ret = MLAN_STATUS_FAILURE;
		printf("[ERROR] Invalid Hostcmd %x\n",
		       phostcmd->cmd_hdr.command);
		break;
	}

done:
	return ret;
}

/**
 *  @brief Send hostcmd IOCTL to driver
 *  @param cmd_buf  pointer to Host Command buffer
 *
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int mlanwls_send_ioctl(t_u8 *cmd_buf)
{
	struct ifreq ifr;
	struct eth_priv_cmd *cmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;

	/*hexdump((void *) cmd_buf, MRVDRV_SIZE_OF_CMD_BUFFER, ' ');*/

	if (!cmd_buf) {
		printf("ERR:IOCTL Failed due to null cmd buffer!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, cmd_buf, sizeof(cmd_buf));
#else
	cmd->buf = cmd_buf;
#endif
	cmd->used_len = 0;
	cmd->total_len = MRVDRV_SIZE_OF_CMD_BUFFER;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanwls");
		fprintf(stderr, "IOCTL fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	ret = process_ftm_hostcmd_resp("hostcmd", cmd_buf);

done:
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief              get connection status
 *
 *  @param data         Pointer to the output buffer holding connection status
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int get_connstatus(int *data)
{
	struct ether_addr apaddr;
	struct ether_addr etherzero = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buffer) {
		DBG_ERROR("[ERROR] Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* buffer = CMD_NXP + <cmd_string>*/
	mlanwls_prepare_buffer(buffer, "getwap", 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		DBG_ERROR("[ERROR] Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
	memset(cmd, 0, sizeof(struct eth_priv_cmd));
	memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
	cmd->buf = buffer;
#endif
	cmd->used_len = 0;
	cmd->total_len = MRVDRV_SIZE_OF_CMD_BUFFER;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		DBG_ERROR("[ERROR] mlanwls: getwap fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	memset(&apaddr, 0, sizeof(struct ether_addr));
	memcpy(&apaddr, (struct ether_addr *)(buffer),
	       sizeof(struct ether_addr));
	memcpy(&gwls_data.ap_mac[0], (t_u8 *)(buffer),
	       sizeof(gwls_data.ap_mac));

	if (!memcmp(&apaddr, &etherzero, sizeof(struct ether_addr))) {
		/* not associated */
		*data = FALSE;
	} else {
		/* associated */
		*data = TRUE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Parse FTM complete event data
 *
 *  @param buffer   Pointer to received event buffer
 *  @param size     Length of the received event data
 *  @return         N/A
 */
static int process_ftm_complete_event(t_u8 *buffer, t_u16 size, char *if_name)
{
	wls_event_t *ftm_event = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	float distance = 0.0;

	if (!buffer) {
		DBG_ERROR("[ERROR] Event buffer null\n");
		return MLAN_STATUS_FAILURE;
	}
	ftm_event = (wls_event_t *)buffer;

	printf("[INFO] Event received for interface %s\n", if_name);
	printf("[INFO] EventID: 0x%x SubeventID:%d\n", ftm_event->event_id,
	       ftm_event->sub_event_id);
	hexdump("EventData:", (void *)buffer, size, ' ');

	switch (ftm_event->sub_event_id) {
	case WLS_SUB_EVENT_FTM_COMPLETE:

		printf("\n\nFTM Session Complete:\n");
		printf("=====================\n");
		printf("Average RTT: %d ns\n",
		       ftm_event->e.ftm_complete.avg_rtt);
		printf("Average Clockoffset:%d ns\n",
		       ftm_event->e.ftm_complete.avg_clk_offset);

		distance = ((ftm_event->e.ftm_complete.avg_clk_offset / 2) *
			    (0.0003));
		printf("Distance: %f meters\n\n", distance);

		gwls_data.loop_cnt--;
		if ((gwls_data.loop_cnt > 0) || (gwls_data.run_nonstop)) {
			/*Stop and restart the FTM*/
			process_ftm_start(0, NULL, &gwls_data);
		} else {
			process_ftm_stop(0, NULL, &gwls_data);
			gwls_data.terminate_app = TRUE;
		}

		break;
	case WLS_SUB_EVENT_RADIO_RECEIVED:
		printf("WLS_SUB_EVENT_RADIO_RECEIVED\n");
		break;
	case WLS_SUB_EVENT_RADIO_RPT_RECEIVED:
		printf("WLS_SUB_EVENT_RADIO_RPT_RECEIVED\n");
		break;
	case WLS_SUB_EVENT_ANQP_RESP_RECEIVED:
		printf("WLS_SUB_EVENT_ANQP_RESP_RECEIVED\n");
		break;
	default:
		printf("[ERROR] Unknown sub event\n");
		break;
	}
	return ret;
}

/**
 *  @brief Print connect and disconnect event related information
 *
 *  @param buffer   Pointer to received event buffer
 *  @param size     Length of the received event
 *
 *  @return         N/A
 */
static void print_event_drv_connected(t_u8 *buffer, t_u16 size)
{
	struct ether_addr *wap;
	struct ether_addr etherzero = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
	char buf[32];

	wap = (struct ether_addr *)(buffer + strlen(CUS_EVT_AP_CONNECTED));

	if (!memcmp(wap, &etherzero, sizeof(struct ether_addr))) {
		printf("---< Disconnected from AP >---\n");
		assoc_flag = FALSE;

	} else {
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
			 wap->ether_addr_octet[0], wap->ether_addr_octet[1],
			 wap->ether_addr_octet[2], wap->ether_addr_octet[3],
			 wap->ether_addr_octet[4], wap->ether_addr_octet[5]);
		printf("---< Connected to AP: %s >---\n", buf);
		/** set TRUE, if connected */
		assoc_flag = TRUE;
	}
}

/**
 *  @brief              This function parses for NETLINK events
 *
 *  @param nlh          Pointer to Netlink message header
 *  @param bytes_read   Number of bytes to be read
 *  @param evt_conn     A pointer to a output buffer. It sets TRUE when it gets
 *  					the event CUS_EVT_OBSS_SCAN_PARAM, otherwise
 * FALSE
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int drv_nlevt_handler(struct nlmsghdr *nlh, int bytes_read,
			     int *evt_conn)
{
	int len, plen;
	t_u8 *buffer = NULL;
	t_u32 event_id = 0;
	event_header *event = NULL;
	char if_name[IFNAMSIZ + 1];

	/* Initialize receive buffer */
	buffer = (t_u8 *)malloc(NL_MAX_PAYLOAD);
	if (!buffer) {
		printf("ERR: Could not alloc buffer\n");
		return MLAN_STATUS_FAILURE;
	}
	memset(buffer, 0, NL_MAX_PAYLOAD);

	*evt_conn = FALSE;
	while ((unsigned int)bytes_read >= NLMSG_HDRLEN) {
		len = nlh->nlmsg_len; /* Length of message including header */
		plen = len - NLMSG_HDRLEN;
		if (len > bytes_read || plen < 0) {
			free(buffer);
			/* malformed netlink message */
			return MLAN_STATUS_FAILURE;
		}
		if ((unsigned int)len > NLMSG_SPACE(NL_MAX_PAYLOAD)) {
			printf("ERR:Buffer overflow!\n");
			free(buffer);
			return MLAN_STATUS_FAILURE;
		}
		memset(buffer, 0, NL_MAX_PAYLOAD);
		memcpy(buffer, NLMSG_DATA(nlh), plen);

		if (NLMSG_OK(nlh, len)) {
			memcpy(&event_id, buffer, sizeof(event_id));

			if (((event_id & 0xFF000000) == 0x80000000) ||
			    ((event_id & 0xFF000000) == 0)) {
				event = (event_header *)buffer;
			} else {
				memset(if_name, 0, IFNAMSIZ + 1);
				memcpy(if_name, buffer, IFNAMSIZ);
				event = (event_header *)(buffer + IFNAMSIZ);
			}
		}

		/*Prints the events*/
		if (event) {
			switch (event->event_id) {
			case EVENT_WLS_FTM_COMPLETE:
				process_ftm_complete_event((t_u8 *)event,
							   bytes_read, if_name);
				break;
			default:
				if (!strncmp(CUS_EVT_AP_CONNECTED,
					     (char *)event,
					     strlen(CUS_EVT_AP_CONNECTED))) {
					if (strlen(if_name))
						printf("EVENT for interface %s\n",
						       if_name);
					print_event_drv_connected((t_u8 *)event,
								  bytes_read);
				}
				break;
			}
		}
		len = NLMSG_ALIGN(len);
		bytes_read -= len;
		nlh = (struct nlmsghdr *)((char *)nlh + len);
	}
	free(buffer);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Configure and read event data from netlink socket
 *
 *  @param nl_sk        Netlink socket handler
 *  @param msg          Pointer to message header
 *  @param ptv          Pointer to struct timeval
 *
 *  @return             Number of bytes read or MLAN_STATUS_FAILURE
 */
static int read_event(int nl_sk, struct msghdr *msg, struct timeval *ptv)
{
	int count = -1;
	int ret = MLAN_STATUS_FAILURE;
	fd_set rfds;

	/* Setup read fds and initialize event buffers */
	FD_ZERO(&rfds);
	FD_SET(nl_sk, &rfds);

	/* Wait for reply */
	ret = select(nl_sk + 1, &rfds, NULL, NULL, ptv);

	if (ret == MLAN_STATUS_FAILURE) {
		/* Error */
		mlanwls_terminate_flag++;
		ptv->tv_sec = DEFAULT_SCAN_INTERVAL;
		ptv->tv_usec = 0;
		goto done;
	}
	if (!FD_ISSET(nl_sk, &rfds)) {
		/* Unexpected error. Try again */
		ptv->tv_sec = DEFAULT_SCAN_INTERVAL;
		ptv->tv_usec = 0;
		goto done;
	}
	/* Success */
	count = recvmsg(nl_sk, msg, 0);

done:
	return count;
}

/**
 *  @brief Run the application
 *
 *  @param nl_sk    Netlink socket
 *
 *  @return         N/A
 */
static void mlanwls_event_handler(int nl_sk)
{
	struct timeval tv;
	int bytes_read, evt_conn;
	struct msghdr msg;
	struct sockaddr_nl dest_addr;
	struct nlmsghdr *nlh;
	struct iovec iov;

	/* Initialize timeout value */
	tv.tv_sec = DEFAULT_SCAN_INTERVAL;
	tv.tv_usec = 0;

	/* Initialize netlink header */
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(NL_MAX_PAYLOAD));
	if (!nlh) {
		printf("[ERROR] Could not allocate space for netlink header\n");
		goto done;
	}
	memset(nlh, 0, NLMSG_SPACE(NL_MAX_PAYLOAD));
	/* Fill the netlink message header */
	nlh->nlmsg_len = NLMSG_SPACE(NL_MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid(); /* self pid */
	nlh->nlmsg_flags = 0;

	/* Initialize I/O vector */
	memset(&iov, 0, sizeof(struct iovec));
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;

	/* Set destination address */
	memset(&dest_addr, 0, sizeof(struct sockaddr_nl));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* Kernel */
	dest_addr.nl_groups = NL_MULTICAST_GROUP;

	/* Initialize message header */
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	while (!gwls_data.terminate_app) {
		/* event buffer is received for all the interfaces */
		bytes_read = read_event(nl_sk, &msg, &tv);
		/* handle only NETLINK events here */
		drv_nlevt_handler((struct nlmsghdr *)nlh, bytes_read,
				  &evt_conn);
	}

done:
	if (nl_sk > 0)
		close(nl_sk);
	if (nlh)
		free(nlh);
	return;
}

/**
 *  @brief Process ftm session ntb ranging configuration
 *  @param argc   Number of arguments for the ntb_ranging_cfg command
 *  @param argv   A pointer to start of ntb_ranging_cfg cmd arguments list
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_dot11az_ntb_cfg(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	wls_app_data_t *app_data = NULL;
	hostcmd_ds_ftm_session_cmd *phostcmd = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buffer) {
		DBG_ERROR("[ERROR] Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	mlanwls_prepare_buffer(buffer, "hostcmd", 0, NULL);
	phostcmd = (hostcmd_ds_ftm_session_cmd *)(buffer + (strlen(CMD_NXP) +
							    strlen("hostcmd") +
							    sizeof(t_u32)));

	/*Parse the arguments*/
	phostcmd->cmd_hdr.command = cpu_to_le16(HostCmd_CMD_FTM_SESSION_CFG);
	phostcmd->cmd_hdr.size = S_DS_GEN + sizeof(hostcmd_ds_ftm_session_cmd);
	phostcmd->cmd_hdr.size = cpu_to_le16(phostcmd->cmd_hdr.size);
	phostcmd->cmd.ftm_session_cfg.action =
		cpu_to_le16(app_data->hostcmd_action);
	phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.type =
		cpu_to_le16(FTM_NTB_RANGING_CFG_TLV_ID);
	phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.len =
		cpu_to_le16(sizeof(ntb_ranging_cfg_t));

	if (app_data->hostcmd_action == MLAN_ACT_SET) {
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.val
			.format_bw = app_data->ntb_cfg.format_bw;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.val
			.max_i2r_sts_upto80 =
			app_data->ntb_cfg.max_i2r_sts_upto80;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.val
			.max_r2i_sts_upto80 =
			app_data->ntb_cfg.max_r2i_sts_upto80;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.val
			.az_measurement_freq =
			app_data->ntb_cfg.az_measurement_freq;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11az.ntb_tlv.val
			.az_number_of_measurements =
			app_data->ntb_cfg.az_number_of_measurements;
	}
	/*Perform ioctl and process response*/
	ret = mlanwls_send_ioctl(buffer);

done:
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief Process dot11mc ftm session  configuration
 *  @param argc   Number of arguments for the ntb_ranging_cfg command
 *  @param argv   A pointer to start of ntb_ranging_cfg cmd arguments list
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_dot11mc_ftm_cfg(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	wls_app_data_t *app_data = NULL;
	hostcmd_ds_ftm_session_cmd *phostcmd = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buffer) {
		DBG_ERROR("[ERROR] Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	mlanwls_prepare_buffer(buffer, "hostcmd", 0, NULL);
	phostcmd = (hostcmd_ds_ftm_session_cmd *)(buffer + (strlen(CMD_NXP) +
							    strlen("hostcmd") +
							    sizeof(t_u32)));

	/*Parse the arguments*/
	phostcmd->cmd_hdr.command = cpu_to_le16(HostCmd_CMD_FTM_SESSION_CFG);
	phostcmd->cmd_hdr.size =
		S_DS_GEN + sizeof(t_u16) + sizeof(dot11mc_ftm_cfg_t);
	phostcmd->cmd.ftm_session_cfg.action =
		cpu_to_le16(app_data->hostcmd_action);

	if (app_data->hostcmd_action == MLAN_ACT_SET) {
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.type =
			cpu_to_le16(FTM_SESSION_CFG_INITATOR_TLV_ID);
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.len =
			cpu_to_le16(sizeof(ftm_session_cfg_t) + sizeof(t_u16));
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.burst_exponent = app_data->session_cfg.burst_exponent;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.burst_duration = app_data->session_cfg.burst_duration;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.min_delta_FTM = app_data->session_cfg.min_delta_FTM;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val.is_ASAP =
			app_data->session_cfg.is_ASAP;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.per_burst_FTM = app_data->session_cfg.per_burst_FTM;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.channel_spacing =
			app_data->session_cfg.channel_spacing;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.val
			.burst_period =
			cpu_to_le16(app_data->session_cfg.burst_period);
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.civic_req =
			app_data->civic_request;
		phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.sess_tlv.lci_req =
			app_data->lci_request;

		if (app_data->lci_request) {
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.type =
				cpu_to_le16(FTM_SESSION_CFG_LCI_TLV_ID);
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.len =
				cpu_to_le16(sizeof(lci_cfg_t));
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.altitude = app_data->lci_cfg.altitude;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.alt_unc = app_data->lci_cfg.alt_unc;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.latitude = app_data->lci_cfg.latitude;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.lat_unc = app_data->lci_cfg.lat_unc;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.longitude = app_data->lci_cfg.longitude;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.lci_tlv.val
				.long_unc = app_data->lci_cfg.long_unc;
		}

		if (app_data->civic_request) {
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv
				.type = cpu_to_le16(
				FTM_SESSION_CFG_LOCATION_CIVIC_TLV_ID);
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv
				.len = cpu_to_le16(
				sizeof(civic_loc_cfg_t) +
				app_data->civic_cfg.civic_address_length - 1);
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv.val
				.civic_address_type =
				app_data->civic_cfg.civic_address_type;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv.val
				.civic_location_type =
				app_data->civic_cfg.civic_location_type;
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv.val
				.country_code =
				cpu_to_le16(app_data->civic_cfg.country_code);
			phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc.civic_tlv.val
				.civic_address_length =
				app_data->civic_cfg.civic_address_length;
			memcpy(&(phostcmd->cmd.ftm_session_cfg.tlv.cfg_11mc
					 .civic_tlv.val.civic_address[0]),
			       &app_data->civic_cfg.civic_address[0],
			       app_data->civic_cfg.civic_address_length);
			phostcmd->cmd_hdr.size +=
				app_data->civic_cfg
					.civic_address_length; /*copy the
								  variable len
								  addr size*/
		}
		phostcmd->cmd_hdr.size = cpu_to_le16(phostcmd->cmd_hdr.size);
	}
	/*Perform ioctl and process response*/
	ret = mlanwls_send_ioctl(buffer);

done:
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief Process 11az/11mc ftm_start command
 *  @param argc   Number of arguments for ftm_start command
 *  @param argv   A pointer to first argument of ftm_start command
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_ftm_start(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	wls_app_data_t *app_data = NULL;
	hostcmd_ds_ftm_session_cmd *phostcmd = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	/** Get connection status */
	if (get_connstatus(&assoc_flag) == MLAN_STATUS_FAILURE) {
		DBG_ERROR("[ERROR] Cannot Start FTM, STA not associated !\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	mlanwls_prepare_buffer(buffer, "hostcmd", 0, NULL);
	phostcmd = (hostcmd_ds_ftm_session_cmd *)(buffer + (strlen(CMD_NXP) +
							    strlen("hostcmd") +
							    sizeof(t_u32)));

	/*Parse the arguments*/
	phostcmd->cmd_hdr.command = cpu_to_le16(HostCmd_CMD_FTM_SESSION_CTRL);
	phostcmd->cmd_hdr.size = S_DS_GEN + sizeof(hostcmd_ftm_session_ctrl);
	phostcmd->cmd_hdr.size = cpu_to_le16(phostcmd->cmd_hdr.size);
	phostcmd->cmd.ftm_session_ctrl.action = cpu_to_le16(FTM_ACTION_START);
	phostcmd->cmd.ftm_session_ctrl.chan = app_data->channel;
	memcpy(&phostcmd->cmd.ftm_session_ctrl.peer_mac[0],
	       &app_data->peer_mac[0], ETH_ALEN);

	/*Perform ioctl and process response*/
	ret = mlanwls_send_ioctl(buffer);

	if (ret == MLAN_STATUS_SUCCESS) {
		app_data->ftm_started = TRUE;

		DBG_LOG("[INFO] Wait for session complete event.. \n");
		/** run the application */
		mlanwls_event_handler(nl_sk);
	} else {
		DBG_ERROR("[ERROR] Starting FTM Session failed\n");
		app_data->ftm_started = FALSE;
	}

done:
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief Handle ftm stop procedure
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_ftm_stop(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	wls_app_data_t *app_data = NULL;
	hostcmd_ds_ftm_session_cmd *phostcmd = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	if (!app_data->ftm_started) {
		DBG_LOG("[INFO] FTM Session already stopped!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		DBG_ERROR("[ERROR] Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	mlanwls_prepare_buffer(buffer, "hostcmd", 0, NULL);
	phostcmd = (hostcmd_ds_ftm_session_cmd *)(buffer + (strlen(CMD_NXP) +
							    strlen("hostcmd") +
							    sizeof(t_u32)));

	/*Parse the arguments*/
	phostcmd->cmd_hdr.command = cpu_to_le16(HostCmd_CMD_FTM_SESSION_CTRL);
	phostcmd->cmd_hdr.size = S_DS_GEN + sizeof(hostcmd_ftm_session_ctrl);
	phostcmd->cmd_hdr.size = cpu_to_le16(phostcmd->cmd_hdr.size);
	phostcmd->cmd.ftm_session_ctrl.action = cpu_to_le16(FTM_ACTION_STOP);
	phostcmd->cmd.ftm_session_ctrl.chan = app_data->channel;
	memcpy(&phostcmd->cmd.ftm_session_ctrl.peer_mac[0],
	       &app_data->peer_mac[0], ETH_ALEN);

	/*Perform ioctl and process response*/
	ret = mlanwls_send_ioctl(buffer);
	app_data->ftm_started = FALSE;

	if (ret == MLAN_STATUS_FAILURE) {
		DBG_ERROR("[ERROR] Hostcmd ftm stop failed\n");
	}

done:
	if (buffer)
		free(buffer);
	return ret;
}

/**
 *  @brief Determine the netlink number
 *
 *  @return         Netlink number to use
 */
static int get_netlink_num(int dev_index)
{
	FILE *fp = NULL;
	int netlink_num = -1;
	char str[64];
	char *srch = "netlink_num";
	char filename[64];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* if dev_index is specified by user */
	if (dev_index >= 0) {
		/* Try to open old driver proc: /proc/mwlan/configX first */
		if (dev_index == 0)
			strcpy(filename, "/proc/mwlan/config");
		else if (dev_index > 0)
			sprintf(filename, "/proc/mwlan/config%d", dev_index);
		fp = fopen(filename, "r");
		if (!fp) {
			/* Try to open multi-adapter driver proc:
			 * /proc/mwlan/adapterX/config if old proc access fail
			 */
			snprintf(filename, sizeof(filename),
				 "/proc/mwlan/adapter%d/config", dev_index);
			fp = fopen(filename, "r");
		}

		if (fp) {
			while (fgets(str, sizeof(str), fp)) {
				if (strncmp(str, srch, strlen(srch)) == 0) {
					netlink_num =
						atoi(str + strlen(srch) + 1);
					break;
				}
			}
			fclose(fp);
		} else {
			return -1;
		}
	} else {
		/* Start preparing the buffer */
		/* Initialize buffer */
		buffer = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
		if (!buffer) {
			DBG_ERROR(
				"[ERROR] Cannot allocate buffer for command!\n");
			return -1;
		}
		/* buffer = CMD_NXP + <cmd_string>*/
		mlanwls_prepare_buffer(buffer, "getnlnum", 0, NULL);

		cmd = (struct eth_priv_cmd *)malloc(
			sizeof(struct eth_priv_cmd));
		if (!cmd) {
			DBG_ERROR(
				"[ERROR] Cannot allocate buffer for command!\n");
			goto done;
		}

		/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
		memset(cmd, 0, sizeof(struct eth_priv_cmd));
		memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
		cmd->buf = buffer;
#endif
		cmd->used_len = 0;
		cmd->total_len = MRVDRV_SIZE_OF_CMD_BUFFER;

		/* Perform IOCTL */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			DBG_ERROR("[ERROR] mlanwls: getnlnum fail\n");
			goto done;
		}
		netlink_num = *(int *)(buffer);
	}

done:
	if (cmd)
		free(cmd);
	if (buffer)
		free(buffer);
	printf("[INFO] Netlink number = %d\n", netlink_num);
	return netlink_num;
}

/**
 *  @brief opens netlink socket to receive NETLINK events
 *  @return  socket id --success, otherwise--MLAN_STATUS_FAILURE
 */
static int open_netlink(int dev_index)
{
	int sk = -1;
	struct sockaddr_nl src_addr;
	int netlink_num = 0;

	netlink_num = get_netlink_num(dev_index);
	if (netlink_num < 0) {
		DBG_ERROR(
			"[ERROR] Could not get netlink socket. Invalid device number.\n");
		return sk;
	}

	/* Open netlink socket */
	sk = socket(PF_NETLINK, SOCK_RAW, netlink_num);
	if (sk < 0) {
		DBG_ERROR("[ERROR] Could not open netlink socket.\n");
		return sk;
	}

	/* Set source address */
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	src_addr.nl_groups = NL_MULTICAST_GROUP;

	/* Bind socket with source address */
	if (bind(sk, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
		DBG_ERROR("[ERROR] Could not bind socket!\n");
		close(sk);
		return -1;
	}
	return sk;
}

/**
 *  @brief Terminate signal handler
 *  @param signal   Signal to handle
 *  @return         NA
 */
static t_void mlanwls_terminate_handler(int signal)
{
	printf("[INFO] Stopping application.\n");
#if DEBUG
	printf("Process ID of process killed = %d\n", getpid());
#endif
	gwls_data.run_nonstop = 0;
	gwls_data.terminate_app = 1;
	process_ftm_stop(0, NULL, &gwls_data);
}

/**
 *  @brief Process session ctrl cmd to send hostcmd
 *  @param param pointer to cmd priv data
 *  @return MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_ftm_session_cfg(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	wls_app_data_t *app_data = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	if (app_data->protocol_type == PROTO_DOT11AZ) {
		printf("[INFO] Set/Get DOT11AZ Config \n");
		ret = process_dot11az_ntb_cfg(argc, argv, param);
	} else {
		printf("[INFO] Set/Get DOT11MC (Legacy) Config \n");
		ret = process_dot11mc_ftm_cfg(argc, argv, param);
	}

done:
	return ret;
}

/**
 *  @brief  Process session ctrl cmd to send hostcmd
 *  @param param pointer to cmd priv data
 *  @return MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_ftm_session_ctrl(int argc, char *argv[], void *param)
{
	int ret = MLAN_STATUS_SUCCESS;
	wls_app_data_t *app_data = NULL;

	if (!param) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	app_data = (wls_app_data_t *)param;

	if (app_data->hostcmd_action == FTM_ACTION_START) {
		ret = process_ftm_start(argc, argv, param);
	} else {
		ret = process_ftm_stop(argc, argv, param);
	}
done:
	return ret;
}

/**
 *  @brief Parse the user command and process it
 *  @param start_idx   Start of ftm command argument in user command
 *  @return MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int process_subcommand(int argc, char *argv[])
{
	int i, ret;

	/*Parse the user command to update the priv data and call subcommand
	 * handlers*/
	for (i = 0; wls_app_command[i].cmd; i++) {
		if (strncmp(wls_app_command[i].cmd, argv[FTM_SUBCMD_INDEX],
			    strlen(wls_app_command[i].cmd)))
			continue;
		if (strlen(wls_app_command[i].cmd) !=
		    strlen(argv[FTM_SUBCMD_INDEX]))
			continue;

		switch (wls_app_command[i].cmd_id) {
		case FTM_SESSION_CFG_CMD_ID:
			if (FTM_CFG_SET_CMD_LEN == argc) {
				gwls_data.hostcmd_action = MLAN_ACT_SET;
				gwls_data.protocol_type =
					atoi(argv[FTM_CFG_PROTOCOL_INDEX]);
				mlanwls_read_ftm_config(
					argv[FTM_CFG_FILE_ARG_INDEX]);

			} else if (FTM_CFG_GET_CMD_LEN == argc) {
				/*ToDo: FW Implemention for GET operation*/
				gwls_data.hostcmd_action = MLAN_ACT_GET;
			} else {
				DBG_ERROR(
					"[ERROR] Invalid number of arguments\n\n");
				display_help(NELEMENTS(ftm_session_cfg_help),
					     ftm_session_cfg_help);
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			break;
		case FTM_SESSION_CTRL_CMD_ID:

			if (FTM_SESSION_SUBCMD_LEN == (argc) ||
			    (FTM_SESSION_SUBCMD_NONSTOP_LEN == (argc))) {
				gwls_data.channel = a2hex_or_atoi(
					argv[FTM_SESSION_CHANNEL_OFFSET]);
				gwls_data.hostcmd_action = a2hex_or_atoi(
					argv[FTM_SESSION_ACTION_OFFSET]);

				ret = mac2raw(
					argv[FTM_SESSION_PEER_ADDR_OFFSET],
					&(gwls_data.peer_mac[0]));
				if (ret != MLAN_STATUS_SUCCESS) {
					printf("[ERROR] %s Address \n",
					       ret == MLAN_STATUS_FAILURE ?
						       "Invalid MAC" :
						       ret == MAC_BROADCAST ?
						       "Broadcast" :
						       "Multicast");
					ret = MLAN_STATUS_FAILURE;
					goto done;
				}

				if (FTM_SESSION_SUBCMD_LEN == (argc)) {
					gwls_data.loop_cnt = a2hex_or_atoi(
						argv[FTM_SESSION_LOOP_OFFSET]);
					gwls_data.run_nonstop =
						(gwls_data.loop_cnt == 0) ? 1 :
									    0;

				} else {
					gwls_data.loop_cnt = 1;
				}

			} else {
				DBG_ERROR(
					"[ERROR] Invalid number of arguments\n");
				display_help(NELEMENTS(ftm_session_ctrl_help),
					     ftm_session_ctrl_help);
				ret = MLAN_STATUS_FAILURE;
				goto done;
			}
			break;
		default:
			printf("[ERROR] SubCommand %s is not supported\n",
			       argv[FTM_SUBCMD_INDEX]);
			display_help(NELEMENTS(mlanwls_help), mlanwls_help);
			ret = MLAN_STATUS_FAILURE;
			goto done;
			break;
		}

		ret = wls_app_command[i].func(argc, argv, &gwls_data);
		printf("[INFO] Command %s processed. Return:%d \n",
		       wls_app_command[i].cmd, ret);
		break;
	}

done:
	if (!wls_app_command[i].cmd)
		display_help(NELEMENTS(mlanwls_help), mlanwls_help);
	return ret;
}

/**
 *  @brief Read ftm config param from conf file
 *  @param file_name  config file name
 *
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int mlanwls_read_ftm_config(char *file_name)
{
	int ret = MLAN_STATUS_SUCCESS;
	FILE *config_file = NULL;
	char *line = NULL;
	char *data = NULL;
	int arg_num, li;
	char *args[30];
	t_u8 param;

	// read config
	config_file = fopen(file_name, "r");
	if (config_file == NULL) {
		perror("CONFIG");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	printf("[INFO] Read FTM config from file %s\n", file_name);
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li,
			       &data)) {
		arg_num = parse_line(line, args, 30);

		if (arg_num > 1)
			param = atoi(args[1]);

		if (gwls_data.protocol_type == PROTO_DOT11MC) {
			if (strcmp(args[0], "DOT11MC_CFG") == 0) {
				printf("DOT11MC_CFG\n");
			} else if (strcmp(args[0], "BURST_EXP") == 0) {
				gwls_data.session_cfg.burst_exponent =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t BURST_EXP=%d\n", param);
			} else if (strcmp(args[0], "BURST_DURATION") == 0) {
				gwls_data.session_cfg.burst_duration =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t BURST_DURATION=%d\n", param);
			} else if (strcmp(args[0], "MIN_DELTA") == 0) {
				gwls_data.session_cfg.min_delta_FTM =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t MIN_DELTA=%d\n", param);
			} else if (strcmp(args[0], "IS_ASAP") == 0) {
				gwls_data.session_cfg.is_ASAP =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t IS_ASAP=%d\n", param);
			} else if (strcmp(args[0], "FTM_PER_BURST") == 0) {
				gwls_data.session_cfg.per_burst_FTM =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t FTM_PER_BURST=%d\n", param);
			} else if (strcmp(args[0], "BW") == 0) {
				gwls_data.session_cfg.channel_spacing =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t BW=%d\n", param);
			} else if (strcmp(args[0], "BURST_PERIOD") == 0) {
				gwls_data.session_cfg.burst_period =
					(t_u16)(atoi(args[1]));
				PRINT_CFG("\t BURST_PERIOD=%d\n", param);

			} else if (strcmp(args[0], "LCI_REQUEST") == 0) {
				gwls_data.lci_request = (t_u8)(atoi(args[1]));
				PRINT_CFG("\t LCI_REQUEST=%d\n", param);

			} else if (strcmp(args[0], "LATITIUDE") == 0) {
				gwls_data.lci_cfg.latitude =
					(double)(atof(args[1]));
				PRINT_CFG("\t LATITIUDE=%lf\n",
					  gwls_data.lci_cfg.latitude);

			} else if (strcmp(args[0], "LONGITUDE") == 0) {
				gwls_data.lci_cfg.longitude =
					(double)(atof(args[1]));
				PRINT_CFG("\t LATITIUDE=%lf\n",
					  gwls_data.lci_cfg.longitude);

			} else if (strcmp(args[0], "LATITUDE_UNCERTAINITY") ==
				   0) {
				gwls_data.lci_cfg.lat_unc =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t LATITUDE_UNCERTAINITY=%d\n",
					  param);

			} else if (strcmp(args[0], "LONGITUDE_UNCERTAINITY") ==
				   0) {
				gwls_data.lci_cfg.long_unc =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t LONGITUDE_UNCERTAINITY=%d\n",
					  param);

			} else if (strcmp(args[0], "ALTITUDE") == 0) {
				gwls_data.lci_cfg.altitude =
					(double)(atof(args[1]));
				PRINT_CFG("\t ALTITUDE=%lf\n",
					  gwls_data.lci_cfg.altitude);

			} else if (strcmp(args[0], "ALTITUDE_UNCERTAINITY") ==
				   0) {
				gwls_data.lci_cfg.alt_unc =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t ALTITUDE_UNCERTAINITY=%d\n",
					  param);

			} else if (strcmp(args[0], "CIVIC_LOCATION") == 0) {
				gwls_data.civic_request = (t_u8)(atoi(args[1]));
				PRINT_CFG("\t CIVIC_LOCATION=%d\n", param);

			} else if (strcmp(args[0], "CIVIC_LOCATION_TYPE") ==
				   0) {
				gwls_data.civic_cfg.civic_location_type =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t CIVIC_LOCATION_TYPE=%d\n", param);

			} else if (strcmp(args[0], "COUNTRY_CODE") == 0) {
				gwls_data.civic_cfg.country_code =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t COUNTRY_CODE=%d\n", param);

			} else if (strcmp(args[0], "CIVIC_ADDRESS_TYPE") == 0) {
				gwls_data.civic_cfg.civic_address_type =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t CIVIC_ADDRESS_TYPE=%d\n", param);

			} else if (strcmp(args[0], "ADDRESS") == 0) {
				memcpy(&(gwls_data.civic_cfg.civic_address[0]),
				       &args[1], strlen(args[1]));
				gwls_data.civic_cfg.civic_address_length =
					strlen(args[1]);
				PRINT_CFG("\t ADDRESS=%s\n", args[1]);

			} else {
				// printf("Invalid line entry\n %s",args[1]);
			}
		}

		if (gwls_data.protocol_type == PROTO_DOT11AZ) {
			if (strcmp(args[0], "DOT11AZ_CFG") == 0) {
				printf("DOT11AZ_CFG\n\n");
			} else if (strcmp(args[0], "FORMAT_BW") == 0) {
				gwls_data.ntb_cfg.format_bw =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t FORMAT_BW=%d\n", param);
			} else if (strcmp(args[0], "MAX_I2R_STS_UPTO80") == 0) {
				gwls_data.ntb_cfg.max_i2r_sts_upto80 =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t MAX_I2R_STS_UPTO80=%d\n", param);
			} else if (strcmp(args[0], "MAX_R2I_STS_UPTO80") == 0) {
				gwls_data.ntb_cfg.max_r2i_sts_upto80 =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t MAX_R2I_STS_UPTO80=%d\n", param);
			} else if (strcmp(args[0], "AZ_MEASUREMENT_FREQ") ==
				   0) {
				gwls_data.ntb_cfg.az_measurement_freq =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t AZ_MEASUREMENT_FREQ=%d\n", param);
			} else if (strcmp(args[0],
					  "AZ_NUMBER_OF_MEASUREMENTS") == 0) {
				gwls_data.ntb_cfg.az_number_of_measurements =
					(t_u8)(atoi(args[1]));
				PRINT_CFG("\t AZ_NUMBER_OF_MEASUREMENTS=%d\n",
					  param);
			} else {
				// printf("Invalid line entry%s\n",args[1]);
			}
		}
	}

done:
	if (line)
		free(line);
	if (config_file)
		fclose(config_file);
	return ret;
}

/**
 *  @brief Initialize ftm command private data
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int mlanwls_init(void)
{
	int ret = MLAN_STATUS_SUCCESS;

	memset(&gwls_data, 0, sizeof(wls_app_data_t));

	/*Initialize app private data with default values*/
	gwls_data.associated = 0;
	gwls_data.bss_type = 0;
	gwls_data.channel = 0;
	gwls_data.loop_cnt = 1;
	gwls_data.run_nonstop = 0;
	gwls_data.debug_level = 1;
	gwls_data.ftm_started = 0;
	gwls_data.terminate_app = 0;
	gwls_data.protocol_type = 0;

	/*DOT11AZ NTB Ranging default config*/
	gwls_data.ntb_cfg.az_measurement_freq = 1;
	gwls_data.ntb_cfg.az_number_of_measurements = 6;
	gwls_data.ntb_cfg.format_bw = 2;
	gwls_data.ntb_cfg.max_i2r_sts_upto80 = 0;
	gwls_data.ntb_cfg.max_r2i_sts_upto80 = 1;

	/*DOT11MC  FTM session default config*/
	gwls_data.session_cfg.burst_exponent = 0;
	gwls_data.session_cfg.burst_duration = 10;
	gwls_data.session_cfg.burst_period = 5;
	gwls_data.session_cfg.is_ASAP = 1;
	gwls_data.session_cfg.per_burst_FTM = 10;
	gwls_data.session_cfg.min_delta_FTM = 10;
	gwls_data.session_cfg.channel_spacing = 13;

	/*DOT11MC  FTM civic location config*/
	gwls_data.civic_request = 1;
	strncpy((char *)&gwls_data.civic_cfg.civic_address[0], NXP_ADDR,
		strlen(NXP_ADDR));
	gwls_data.civic_cfg.civic_address_length = strlen(NXP_ADDR);
	gwls_data.civic_cfg.civic_address_type = 22;
	gwls_data.civic_cfg.civic_location_type = 1;
	gwls_data.civic_cfg.country_code = 0;

	/*DOT11MC  FTM LCI config*/
	gwls_data.lci_request = 1;
	gwls_data.lci_cfg.altitude = 11.2;
	gwls_data.lci_cfg.alt_unc = 15;
	gwls_data.lci_cfg.latitude = -33.8570095;
	gwls_data.lci_cfg.lat_unc = 18;
	gwls_data.lci_cfg.longitude = +151.2152005;
	gwls_data.lci_cfg.long_unc = 18;
	gwls_data.lci_cfg.z_info = 0;

	return ret;
}

/********************************************************
		Global Functions
********************************************************/
/**
 *  @brief Entry function for mlan location service
 *  @param argc		number of arguments
 *  @param argv     A pointer to arguments array
 *  @return      	MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int mlanwls_main(int argc, char *argv[])
{
	int dev_index = 0; //-1; /** initialise with -1 to open multiple NETLINK
			   //Sockets */
	int ret = MLAN_STATUS_SUCCESS;

	printf("\n\n---------------------------------------------\n");
	printf("------- NXP Wifi Location Service (WLS)------\n");
	printf("--------------------------------------------------\n\n");

	if (argc < 4) {
		display_help(NELEMENTS(mlanwls_help), mlanwls_help);
		return MLAN_STATUS_FAILURE;
	}

	/*Initialize private data*/
	printf("[INFO] Initializing App\n");
	ret = mlanwls_init();

	/*Set the interface*/
	memset(dev_name, 0, sizeof(dev_name));
	strncpy(dev_name, argv[1], strlen(argv[1]));

	/* create a socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		DBG_ERROR("[ERROR] mlanwls: Cannot open socket.\n");
		goto done;
	}
	/* create netlink sockets and bind them with app side addr */
	nl_sk = open_netlink(dev_index);

	if (nl_sk < 0) {
		DBG_ERROR("[ERROR] mlanwls: Cannot open netlink socket.\n");
		goto done;
	}
	signal(SIGHUP, mlanwls_terminate_handler); /* catch hangup signal */
	signal(SIGTERM, mlanwls_terminate_handler); /* catch kill signal */
	signal(SIGINT, mlanwls_terminate_handler); /* catch kill signal */
	signal(SIGALRM, mlanwls_terminate_handler); /* catch kill signal */

	/*Process the wlscmd sub command argument*/
	ret = process_subcommand(argc, argv);

done:
	if (sockfd > 0)
		close(sockfd);
	if (nl_sk > 0)
		close(nl_sk);

	return ret;
}
