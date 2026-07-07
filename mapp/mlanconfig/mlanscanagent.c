/** @file  mlanscanagent.c
 *
 * @brief This files contains mlanconfig scanagent command handling.
 *
 *
 * Copyright 2008-2021 NXP
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
     08/11/2009: initial version
************************************************************************/

#include "mlanconfig.h"
#include "mlanhostcmd.h"
#include "mlanmisc.h"
#include "mlanoffload.h"
#include "mlanscanagent.h"

/********************************************************
		Local Variables
********************************************************/

/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/

/**
 *  @brief scanagent configure scan table
 *
 *  @param age_limit    age limit
 *  @param hold_limit   hold limit
 *
 *  @return             MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_cfg_scan_table_limits(t_u32 age_limit, t_u32 hold_limit)
{
	int ioctl_val, subioctl_val, cmd_len;
	struct iwreq iwr;
	t_u8 *buf, *pos;
	HostCmd_DS_GEN *hostcmd;
	HostCmd_DS_SCANAGENT_SCAN_TABLE_LIMITS *scan_table_limits;

	if (get_priv_ioctl("hostcmd", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	cmd_len = sizeof(HostCmd_DS_GEN) +
		  sizeof(HostCmd_DS_SCANAGENT_SCAN_TABLE_LIMITS);
	buf = (t_u8 *)calloc(sizeof(t_u8), cmd_len);
	if (!buf) {
		printf("Memory allocation failed\n");
		return -ENOMEM;
	}

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_SCANAGENT_SCAN_TABLE_LIMITS);
	hostcmd->size = cpu_to_le16(cmd_len);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	pos = buf + sizeof(HostCmd_DS_GEN);

	scan_table_limits = (HostCmd_DS_SCANAGENT_SCAN_TABLE_LIMITS *)pos;
	scan_table_limits->table_age_limit = cpu_to_le16(age_limit);
	scan_table_limits->table_hold_limit = cpu_to_le16(hold_limit);

	/* 0 set values are ignored by firmware */
	scan_table_limits->action = cpu_to_le16(HostCmd_ACT_GEN_SET);

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.flags = 0;
	iwr.u.data.pointer = (caddr_t)buf;
	iwr.u.data.length = cmd_len;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: scanAgentIoctl ioctl");
		free(buf);
		return -EFAULT;
	}

	printf("\nAge limit  = %7d seconds\n",
	       le16_to_cpu(scan_table_limits->table_age_limit));
	printf("Hold limit = %7d seconds\n\n",
	       le16_to_cpu(scan_table_limits->table_hold_limit));
	free(buf);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set scanagent age limit
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_age_limit(int argc, char *argv[])
{
	t_u32 age_limit = 0;

	if (argc) {
		age_limit = atoi(argv[0]);
	}

	return (scanagent_cfg_scan_table_limits(age_limit, 0));
}

/**
 *  @brief Set scanagent hold limit
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_hold_limit(int argc, char *argv[])
{
	t_u32 hold_limit = 0;

	if (argc) {
		hold_limit = atoi(argv[0]);
	}

	return (scanagent_cfg_scan_table_limits(0, hold_limit));
}

/**
 *  @brief Set scanagent scan timing
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_timing(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	HostCmd_DS_GEN *hostcmd;
	t_u8 *buf, *pos;
	HostCmd_DS_SCANAGENT_CONFIG_TIMING *cfg_timing_cmd;
	MrvlIEtypes_ConfigScanTiming_t *cfg_timing_tlv;
	int idx, cmd_len, rsp_len;
	t_u8 sel = 0;
	t_u16 tlv_len = 0;
	timing_sel_t sel_str[] = {{"disconnected", 1},
				  {"adhoc", 1},
				  {"fullpower", 1},
				  {"ieeeps", 1},
				  {"periodic", 1}};

	if (get_priv_ioctl("hostcmd", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	cmd_len = (sizeof(HostCmd_DS_GEN) + sizeof(t_u16) +
		   sizeof(MrvlIEtypes_ConfigScanTiming_t) * 5);

	buf = (t_u8 *)calloc(sizeof(t_u8), cmd_len);

	if (!buf) {
		printf("Memory allocation failed\n");
		return -ENOMEM;
	}

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_SCANAGENT_SCAN_TIMING);
	hostcmd->size = (sizeof(HostCmd_DS_GEN) + sizeof(t_u16));
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	pos = buf + sizeof(HostCmd_DS_GEN);

	cfg_timing_cmd = (HostCmd_DS_SCANAGENT_CONFIG_TIMING *)pos;
	cfg_timing_cmd->action = cpu_to_le16(HostCmd_ACT_GEN_GET);

	cfg_timing_tlv =
		(MrvlIEtypes_ConfigScanTiming_t *)cfg_timing_cmd->tlv_buffer;

	if (argc == 5) {
		cfg_timing_cmd->action = cpu_to_le16(HostCmd_ACT_GEN_SET);
		cfg_timing_tlv->header.type = cpu_to_le16(TLV_TYPE_SCAN_TIMING);
		cfg_timing_tlv->header.len =
			(sizeof(MrvlIEtypes_ConfigScanTiming_t) -
			 sizeof(cfg_timing_tlv->header));

		for (idx = 0; (unsigned int)idx < NELEMENTS(sel_str); idx++) {
			if (strncmp(argv[0], sel_str[idx].str,
				    sel_str[idx].match_len) == 0) {
				sel = idx + 1;
				break;
			}
		}

		if (idx == NELEMENTS(sel_str)) {
			printf("Wrong argument for mode selected \"%s\"\n",
			       argv[0]);
			free(buf);
			return -EINVAL;
		}

		/*
		 *  HostCmd_DS_ScanagentTimingMode_e;
		 *     TIMING_MODE_INVALID        = 0,
		 *     TIMING_MODE_DISCONNECTED   = 1,
		 *     TIMING_MODE_ADHOC          = 2,
		 *     TIMING_MODE_FULL_POWER     = 3,
		 *     TIMING_MODE_IEEE_PS        = 4,
		 *     TIMING_MODE_PERIODIC_PS    = 5,
		 */
		cfg_timing_tlv->mode = cpu_to_le32(sel);
		cfg_timing_tlv->dwell = cpu_to_le32(atoi(argv[1]));
		cfg_timing_tlv->max_off = cpu_to_le32(atoi(argv[2]));
		cfg_timing_tlv->min_link = cpu_to_le32(atoi(argv[3]));
		cfg_timing_tlv->rsp_timeout = cpu_to_le32(atoi(argv[4]));

		hostcmd->size += sizeof(MrvlIEtypes_ConfigScanTiming_t);
	}

	hostcmd->size = cpu_to_le16(hostcmd->size);

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.flags = 0;
	iwr.u.data.pointer = (caddr_t)buf;
	iwr.u.data.length = cmd_len;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: scanAgentIoctl ioctl");
		free(buf);
		return -EFAULT;
	}

	printf("-----------------------------------------------"
	       "--------------------------------\n");
	printf("%57s\n", "ScanAgent Timing Profiles (in ms)");
	printf("-----------------------------------------------"
	       "--------------------------------\n");
	printf("%13s | %10s | %12s | %14s | %12s\n", "Profile", "Dwell",
	       "MaxOffChan", "MinLinkReturn", "RspTimeout");
	printf("-----------------------------------------------"
	       "--------------------------------\n");

	/* Changed to TLV parsing */
	rsp_len = le16_to_cpu(hostcmd->size);
	rsp_len -= (sizeof(HostCmd_DS_GEN) + sizeof(t_u16));
	pos = buf + sizeof(HostCmd_DS_GEN) + sizeof(t_u16);

	while ((unsigned int)rsp_len > sizeof(MrvlIEtypesHeader_t)) {
		switch (le16_to_cpu(*(t_u16 *)(pos))) {
		case TLV_TYPE_SCAN_TIMING:
			cfg_timing_tlv = (MrvlIEtypes_ConfigScanTiming_t *)pos;
			tlv_len = le16_to_cpu(cfg_timing_tlv->header.len);
			printf("%13s | %10d | %12d | %14d | %12d\n",
			       sel_str[le32_to_cpu(cfg_timing_tlv->mode) - 1]
				       .str,
			       (int)le32_to_cpu(cfg_timing_tlv->dwell),
			       (int)le32_to_cpu(cfg_timing_tlv->max_off),
			       (int)le32_to_cpu(cfg_timing_tlv->min_link),
			       (int)le32_to_cpu(cfg_timing_tlv->rsp_timeout));
			break;
		}
		pos += tlv_len + sizeof(MrvlIEtypesHeader_t);
		rsp_len -= tlv_len + sizeof(MrvlIEtypesHeader_t);
		rsp_len = (rsp_len > 0) ? rsp_len : 0;
	}

	printf("\n");
	free(buf);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set scanagent profile scan period
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_profile_period(int argc, char *argv[])
{
	int ioctl_val, subioctl_val, cmd_len;
	struct iwreq iwr;
	t_u8 *buf, *pos;
	HostCmd_DS_GEN *hostcmd;
	HostCmd_DS_SCANAGENT_CONFIG_PROFILE_SCAN *cfg_profile_scan;

	if (get_priv_ioctl("hostcmd", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	cmd_len = sizeof(HostCmd_DS_GEN) +
		  sizeof(HostCmd_DS_SCANAGENT_CONFIG_PROFILE_SCAN);

	buf = (t_u8 *)calloc(sizeof(t_u8), cmd_len);
	if (!buf) {
		printf("Memory allocation failed\n");
		return -ENOMEM;
	}

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command =
		cpu_to_le16(HostCmd_CMD_SCANAGENT_CONFIG_PROFILE_SCAN);
	hostcmd->size = cpu_to_le16(cmd_len);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	pos = buf + sizeof(HostCmd_DS_GEN);

	cfg_profile_scan = (HostCmd_DS_SCANAGENT_CONFIG_PROFILE_SCAN *)pos;
	if (argc == 1) {
		cfg_profile_scan->action = cpu_to_le16(HostCmd_ACT_GEN_SET);
		cfg_profile_scan->scan_interval = cpu_to_le16(atoi(argv[0]));
	} else {
		cfg_profile_scan->action = cpu_to_le16(HostCmd_ACT_GEN_GET);
	}

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.flags = 0;
	iwr.u.data.pointer = (caddr_t)buf;
	iwr.u.data.length = cmd_len;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: scanAgentIoctl ioctl");
		free(buf);
		return -EFAULT;
	}

	cfg_profile_scan->scan_interval =
		le16_to_cpu(cfg_profile_scan->scan_interval);
	if ((int)cfg_profile_scan->scan_interval == 0) {
		printf("\nProfile Scan interval: <disabled>\n\n");
	} else {
		printf("\nProfile Scan interval: %d seconds\n\n",
		       (int)cfg_profile_scan->scan_interval);
	}

	free(buf);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief scanagent parse entry selection
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @param psel     A pointer to scanagent entry selection
 *
 *  @return         None
 */
static void scanAgentParseEntrySel(int argc, char *argv[],
				   HostCmd_DS_SCANAGENT_TABLE_MAINTENANCE *psel,
				   int *cmd_len)
{
	int arg_idx, tmp_idx;
	t_u8 *tlv_pos;
	MrvlIEtypes_SsIdParamSet_t *ssid;
	MrvlIEtypes_Bssid_List_t *bssid;
	unsigned int mac[ETH_ALEN];

	tlv_pos = (t_u8 *)psel->tlv_buffer;

	for (arg_idx = 0; arg_idx < argc; arg_idx++) {
		if (strncmp(argv[arg_idx], "ssid=", strlen("ssid=")) == 0) {
			ssid = (MrvlIEtypes_SsIdParamSet_t *)tlv_pos;
			ssid->header.type = cpu_to_le16(TLV_TYPE_SSID);
			ssid->header.len =
				strlen(argv[arg_idx]) - strlen("ssid=");
			strncpy((char *)ssid->ssid,
				(argv[arg_idx] + strlen("ssid=")),
				ssid->header.len);
			tlv_pos +=
				ssid->header.len + sizeof(MrvlIEtypesHeader_t);
			ssid->header.len = cpu_to_le16(ssid->header.len);

		} else if (strncmp(argv[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			bssid = (MrvlIEtypes_Bssid_List_t *)tlv_pos;
			bssid->header.type = cpu_to_le16(TLV_TYPE_BSSID);
			bssid->header.len = ETH_ALEN;
			/*
			 *  "bssid" token string handler
			 */
			sscanf(argv[arg_idx] + strlen("bssid="),
			       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0, mac + 1,
			       mac + 2, mac + 3, mac + 4, mac + 5);
			for (tmp_idx = 0;
			     (unsigned int)tmp_idx < NELEMENTS(mac);
			     tmp_idx++) {
				bssid->bssid[tmp_idx] = (t_u8)mac[tmp_idx];
			}
			tlv_pos +=
				bssid->header.len + sizeof(MrvlIEtypesHeader_t);
			bssid->header.len = cpu_to_le16(bssid->header.len);

		} else if (strncmp(argv[arg_idx], "age=", strlen("age=")) ==
			   0) {
			psel->age = cpu_to_le32(
				atoi(argv[arg_idx] + strlen("age=")));

		} else if (strncmp(argv[arg_idx], "id=", strlen("id=")) == 0) {
			psel->scan_request_id = cpu_to_le32(
				atoi(argv[arg_idx] + strlen("id=")));
		}
	}

	*cmd_len += (tlv_pos - psel->tlv_buffer);
}

/**
 *  @brief scanagent execute scan
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_exec_scan(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	int arg_idx, cmd_len, tmp_idx;
	t_u8 *buf, *pos, *tlv_pos;
	HostCmd_DS_GEN *hostcmd;
	HostCmd_DS_SCANAGENT_SCAN_EXEC *scan_exec;
	MrvlIEtypes_SsIdParamSet_t *ssid;
	MrvlIEtypes_Bssid_List_t *bssid;
	MrvlIEtypes_ConfigScanTiming_t *cfg_timing_tlv;
	unsigned int mac[ETH_ALEN];

	if (get_priv_ioctl("hostcmd", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	cmd_len = (sizeof(HostCmd_DS_GEN) +
		   sizeof(HostCmd_DS_SCANAGENT_SCAN_EXEC) -
		   sizeof(scan_exec->tlv_buffer));

	/* Not sure of the length so, allocate Max command size */
	buf = (t_u8 *)calloc(sizeof(t_u8), MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buf) {
		printf("Memory allocation failed\n");
		return -ENOMEM;
	}

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_SCANAGENT_SCAN_EXEC);
	hostcmd->size = 0;
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	pos = buf + sizeof(HostCmd_DS_GEN);

	scan_exec = (HostCmd_DS_SCANAGENT_SCAN_EXEC *)pos;
	tlv_pos = scan_exec->tlv_buffer;

	for (arg_idx = 0; arg_idx < argc; arg_idx++) {
		if (strncmp(argv[arg_idx], "ssid=", strlen("ssid=")) == 0) {
			/*
			 *  "ssid" token string handler
			 */
			ssid = (MrvlIEtypes_SsIdParamSet_t *)tlv_pos;
			ssid->header.type = cpu_to_le16(TLV_TYPE_SSID);
			ssid->header.len =
				strlen(argv[arg_idx]) - strlen("ssid=");
			strncpy((char *)ssid->ssid,
				argv[arg_idx] + strlen("ssid="),
				ssid->header.len);
			tlv_pos +=
				ssid->header.len + sizeof(MrvlIEtypesHeader_t);
			ssid->header.len = cpu_to_le16(ssid->header.len);

		} else if (strncmp(argv[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			bssid = (MrvlIEtypes_Bssid_List_t *)tlv_pos;
			bssid->header.type = cpu_to_le16(TLV_TYPE_BSSID);
			bssid->header.len = ETH_ALEN;
			/*
			 *  "bssid" token string handler
			 */
			sscanf(argv[arg_idx] + strlen("bssid="),
			       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0, mac + 1,
			       mac + 2, mac + 3, mac + 4, mac + 5);
			for (tmp_idx = 0;
			     (unsigned int)tmp_idx < NELEMENTS(mac);
			     tmp_idx++) {
				bssid->bssid[tmp_idx] = (t_u8)mac[tmp_idx];
			}
			tlv_pos +=
				bssid->header.len + sizeof(MrvlIEtypesHeader_t);
			bssid->header.len = cpu_to_le16(bssid->header.len);

		} else if (strncmp(argv[arg_idx], "type=", strlen("type=")) ==
			   0) {
			/*
			if (strcmp(argv[arg_idx] + strlen("type="), "prof") ==
			0) { scan_exec->scan_type = CONFIG_PROFILE; } else {
			    scan_exec->scan_type = CONFIG_SITE_SURVEY;
			}
			*/
			scan_exec->scan_type = CONFIG_SITE_SURVEY;
			scan_exec->scan_type =
				cpu_to_le16(scan_exec->scan_type);

		} else if (strncmp(argv[arg_idx], "group=", strlen("group=")) ==
			   0) {
			sscanf(argv[arg_idx] + strlen("group="), "0x%x",
			       &tmp_idx);
			scan_exec->chan_group = cpu_to_le32(tmp_idx);

		} else if (strncmp(argv[arg_idx], "delay=", strlen("delay=")) ==
			   0) {
			/*
			 *  "delay" token string handler
			 */
			sscanf(argv[arg_idx] + strlen("delay="), "%d",
			       (int *)&scan_exec->delay);
			scan_exec->delay = cpu_to_le32(scan_exec->delay);

		} else if (strncmp(argv[arg_idx],
				   "timing=", strlen("timing=")) == 0) {
			cfg_timing_tlv =
				(MrvlIEtypes_ConfigScanTiming_t *)tlv_pos;
			cfg_timing_tlv->header.type =
				cpu_to_le16(TLV_TYPE_SCAN_TIMING);
			cfg_timing_tlv->header.len =
				((sizeof(cfg_timing_tlv) -
				  sizeof(cfg_timing_tlv->header)));
			/*
			 *  "timing" token string handler
			 */
			sscanf(argv[arg_idx] + strlen("timing="), "%d,%d,%d,%d",
			       (int *)&cfg_timing_tlv->dwell,
			       (int *)&cfg_timing_tlv->max_off,
			       (int *)&cfg_timing_tlv->min_link,
			       (int *)&cfg_timing_tlv->rsp_timeout);

			cfg_timing_tlv->mode = 0;

			tlv_pos += sizeof(MrvlIEtypesHeader_t);
			tlv_pos += cfg_timing_tlv->header.len;
			cfg_timing_tlv->header.len =
				cpu_to_le16(cfg_timing_tlv->header.len);
		}
	}

	cmd_len += (tlv_pos - scan_exec->tlv_buffer);
	hostcmd->size = cpu_to_le16(cmd_len);

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.flags = 0;
	iwr.u.data.pointer = (caddr_t)buf;
	iwr.u.data.length = cmd_len;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: scanAgentIoctl ioctl");
		free(buf);
		return -EFAULT;
	} else {
		printf("\nScan Scheduled, ID = %d\n\n",
		       (int)le32_to_cpu(scan_exec->scan_req_id_out));
	}

	free(buf);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Issue a scanagent cmd_type subcommand
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @param cmd_type command type
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int
scanagent_table_entry_sub_cmd(int argc, char *argv[],
			      HostCmd_DS_ScanagentTableMaintenance_e cmd_type)
{
	int ioctl_val, subioctl_val, cmd_len;
	struct iwreq iwr;
	t_u8 *buf, *pos;
	HostCmd_DS_GEN *hostcmd;
	HostCmd_DS_SCANAGENT_TABLE_MAINTENANCE *table_maintenance;

	if (get_priv_ioctl("hostcmd", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	cmd_len = (sizeof(HostCmd_DS_GEN) +
		   sizeof(HostCmd_DS_SCANAGENT_TABLE_MAINTENANCE) -
		   sizeof(table_maintenance->tlv_buffer));

	/* Not sure of the length so, allocate Max command size */
	buf = (t_u8 *)calloc(sizeof(t_u8), MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buf) {
		printf("Memory allocation failed\n");
		return -ENOMEM;
	}

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_SCANAGENT_TABLE_MAINTENANCE);
	hostcmd->size = 0;
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	pos = buf + sizeof(HostCmd_DS_GEN);
	table_maintenance = (HostCmd_DS_SCANAGENT_TABLE_MAINTENANCE *)pos;
	table_maintenance->action = cpu_to_le16((t_u16)cmd_type);

	scanAgentParseEntrySel(argc, argv, table_maintenance, &cmd_len);

	hostcmd->size = cpu_to_le16(cmd_len);
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.flags = 0;
	iwr.u.data.pointer = (caddr_t)buf;
	iwr.u.data.length = cmd_len;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: scanAgentIoctl ioctl");
		free(buf);
		return -EFAULT;
	}

	free(buf);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Issue a scanagent table lock subcommand
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_table_lock(int argc, char *argv[])
{
	return (scanagent_table_entry_sub_cmd(argc, argv, SCAN_TABLE_OP_LOCK));
}

/**
 *  @brief Issue a scanagent table unlock subcommand
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_table_unlock(int argc, char *argv[])
{
	return (scanagent_table_entry_sub_cmd(argc, argv,
					      SCAN_TABLE_OP_UNLOCK));
}

/**
 *  @brief Issue a scanagent table purge subcommand
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS on success, otherwise error code
 */
static int scanagent_table_purge(int argc, char *argv[])
{
	return (scanagent_table_entry_sub_cmd(argc, argv, SCAN_TABLE_OP_PURGE));
}

/**
 *  @brief Issue a scanagent command
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int process_scanagent(int argc, char *argv[])
{
	sub_cmd_exec_t sub_cmd[] = {{"timing", 2, 1, scanagent_timing},
				    {"scan", 2, 1, scanagent_exec_scan},
				    {"lock", 2, 1, scanagent_table_lock},
				    {"unlock", 2, 1, scanagent_table_unlock},
				    {"purge", 2, 1, scanagent_table_purge},
				    {"profile", 2, 1, scanagent_profile_period},
				    {"holdlimit", 2, 1, scanagent_hold_limit},
				    {"agelimit", 2, 1, scanagent_age_limit}};

	return (process_sub_cmd(sub_cmd, NELEMENTS(sub_cmd), argc, argv));
}

/**
 *  @brief Retrieve and display the contents of the driver scan table.
 *
 *  The ioctl to retrieve the scan table contents will be invoked, and portions
 *   of the scan data will be displayed on stdout.  The entire beacon or
 *   probe response is also retrieved (if available in the driver).  This
 *   data would be needed in case the application was explicitly controlling
 *   the association (inserting IEs, TLVs, etc).
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int process_getscantable(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	t_u8 scanRspBuffer[600]; /* Stack buffer can be as large as ioctl allows
				  */

	t_u8 scanStart;
	int idx;
	int detail_idx;
	t_u8 scanReqId;

	t_u8 *pCurrent;
	t_u8 *pNext;
	IEEEtypes_ElementId_e *pElementId;
	t_u8 *pElementLen;
	int ssid_idx;
	char ssid[33];
	int ssidLen;

	IEEEtypes_CapInfo_t capInfo = {0};
	t_u8 tsf[8];
	t_u16 beaconInterval;

	IEEEtypes_VendorSpecific_t *pWpaIe;
	const t_u8 wpa_oui[4] = {0x00, 0x50, 0xf2, 0x01};

	IEEEtypes_WmmParameter_t *pWmmIe;
	const t_u8 wmm_oui[4] = {0x00, 0x50, 0xf2, 0x02};
	IEEEtypes_VendorSpecific_t *pwps_ie;
	const t_u8 wps_oui[4] = {0x00, 0x50, 0xf2, 0x04};

	IEEEtypes_MobilityDomain_t *pMdie;
	char wmmCap;
	char wps_cap;
	char dot11kCap;
	char privCap;
	char ht_cap;
	char vht_cap[2];
	int arg_idx;
	int tmp_idx;

	t_u8 entryFlags;
	t_u8 channel = 0;
	t_u16 ageInSecs;
	t_u8 in_bssid[ETH_ALEN];

	unsigned int mac[ETH_ALEN];

	wlan_ioctl_user_scan_ssid ssidList[10];

	int num_ssid;
	boolean displayHelp = FALSE;
	int displayedInfo;
	int display;

	wlan_get_scan_table_fixed fixed_fields;
	t_u32 fixed_field_length;
	t_u32 bss_info_length;

	wlan_ioctl_get_scan_table_info *pRspInfo;

	pRspInfo = (wlan_ioctl_get_scan_table_info *)scanRspBuffer;

	if (get_priv_ioctl("getscantable", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	entryFlags = 0;
	ageInSecs = 0;
	scanReqId = 0;
	memset(in_bssid, 0x00, sizeof(in_bssid));
	memset(ssidList, 0x00, sizeof(ssidList));
	idx = 0;
	detail_idx = 0;
	num_ssid = 0;

	for (arg_idx = 0; arg_idx < argc; arg_idx++) {
		if (strncmp(argv[arg_idx], "id=", strlen("id=")) == 0) {
			scanReqId =
				strtol(argv[arg_idx] + strlen("id="), NULL, 10);
		} else if ((strncmp(argv[arg_idx], "held", strlen("held")) ==
			    0) ||
			   (strncmp(argv[arg_idx], "hold", strlen("hold")) ==
			    0)) {
			entryFlags |= SCAN_QUERY_ENTRY_FLAGS_HELD;
		} else if (strncmp(argv[arg_idx], "lock", strlen("lock")) ==
			   0) {
			entryFlags |= SCAN_QUERY_ENTRY_FLAGS_LOCKED;
		} else if (strncmp(argv[arg_idx], "help", strlen("help")) ==
			   0) {
			displayHelp = TRUE;
		} else if (strncmp(argv[arg_idx], "age=", strlen("age=")) ==
			   0) {
			ageInSecs = strtol(argv[arg_idx] + strlen("age="), NULL,
					   10);
		} else if ((strncmp(argv[arg_idx], "ssid=", strlen("ssid=")) ==
			    0) &&
			   (num_ssid < NELEMENTS(ssidList))) {
			ssidList[num_ssid].max_len =
				strlen(argv[arg_idx]) - strlen("ssid=");
			strncpy(ssidList[num_ssid].ssid,
				argv[arg_idx] + strlen("ssid="),
				ssidList[num_ssid].max_len);
			num_ssid++;
		} else if (strncmp(argv[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			sscanf(argv[arg_idx] + strlen("bssid="),
			       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0, mac + 1,
			       mac + 2, mac + 3, mac + 4, mac + 5);
			for (idx = 0; (unsigned int)idx < NELEMENTS(mac);
			     idx++) {
				in_bssid[idx] = (t_u8)mac[idx];
			}
		} else if (strncmp(argv[arg_idx],
				   "detail=", strlen("detail=")) == 0) {
			detail_idx = strtol(argv[arg_idx] + strlen("detail="),
					    NULL, 10);
		}
	}

	if (detail_idx) {
		pRspInfo->scan_number = detail_idx;
		pRspInfo->reqId = scanReqId;
		pRspInfo->entryFlags = entryFlags;
		pRspInfo->ageInSecs = ageInSecs;
		memcpy(pRspInfo->bssid, in_bssid, sizeof(pRspInfo->bssid));
		if (num_ssid <= 1) {
			/* Let the driver/fw filter if only 1 SSID is specified
			 */
			memcpy(pRspInfo->ssid, ssidList[0].ssid,
			       sizeof(pRspInfo->ssid));
			pRspInfo->ssid_len = ssidList[0].max_len;
		} else {
			/* Application filters for multiple SSIDs, don't pass
			 * down */
			memset(pRspInfo->ssid, 0x00, sizeof(pRspInfo->ssid));
			pRspInfo->ssid_len = 0;
		}
		return process_getscantable_idx(pRspInfo);
	}

	displayedInfo = FALSE;
	scanStart = 1;

	printf("---------------------------------------");
	printf("---------------------------------------\n");
	printf("# | ch  | ss  | npi | ld  |       bssid       |   cap    |   "
	       "SSID \n");
	printf("---------------------------------------");
	printf("---------------------------------------\n");

	do {
		pRspInfo->scan_number = scanStart;
		pRspInfo->reqId = scanReqId;
		pRspInfo->entryFlags = entryFlags;
		pRspInfo->ageInSecs = ageInSecs;
		memcpy(pRspInfo->bssid, in_bssid, sizeof(pRspInfo->bssid));
		if (num_ssid <= 1) {
			/* Let the driver/fw filter if only 1 SSID is specified
			 */
			memcpy(pRspInfo->ssid, ssidList[0].ssid,
			       sizeof(pRspInfo->ssid));
			pRspInfo->ssid_len = ssidList[0].max_len;
		} else {
			/* Application filters for multiple SSIDs, don't pass
			 * down */
			memset(pRspInfo->ssid, 0x00, sizeof(pRspInfo->ssid));
			pRspInfo->ssid_len = 0;
		}

		/*
		 * Set up and execute the ioctl call
		 */
		strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
		iwr.u.data.pointer = (caddr_t)pRspInfo;
		iwr.u.data.length = sizeof(scanRspBuffer);
		iwr.u.data.flags = subioctl_val;

		if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
			perror("mlanconfig: getscantable ioctl");
			return -EFAULT;
		}

		pCurrent = 0;
		pNext = pRspInfo->scan_table_entry_buf;

		for (idx = 0; idx < pRspInfo->scan_number; idx++) {
			ssidLen = 0;
			display = FALSE;

			/*
			 * Set pCurrent to pNext in case pad bytes are at the
			 * end of the last IE we processed.
			 */
			pCurrent = pNext;

			memcpy((t_u8 *)&fixed_field_length, (t_u8 *)pCurrent,
			       sizeof(fixed_field_length));
			pCurrent += sizeof(fixed_field_length);

			memcpy((t_u8 *)&bss_info_length, (t_u8 *)pCurrent,
			       sizeof(bss_info_length));
			pCurrent += sizeof(bss_info_length);

			memcpy((t_u8 *)&fixed_fields, (t_u8 *)pCurrent,
			       sizeof(fixed_fields));
			pCurrent += fixed_field_length;

			/* Set next to be the start of the next scan entry */
			pNext = pCurrent + bss_info_length;

			if (bss_info_length >=
			    (sizeof(tsf) + sizeof(beaconInterval) +
			     sizeof(capInfo))) {
				/* time stamp is 8 byte long */
				memcpy(tsf, pCurrent, sizeof(tsf));
				pCurrent += sizeof(tsf);
				bss_info_length -= sizeof(tsf);

				/* beacon interval is 2 byte long */
				memcpy(&beaconInterval, pCurrent,
				       sizeof(beaconInterval));
				pCurrent += sizeof(beaconInterval);
				bss_info_length -= sizeof(beaconInterval);

				/* capability information is 2 byte long */
				memcpy(&capInfo, pCurrent, sizeof(capInfo));
				pCurrent += sizeof(capInfo);
				bss_info_length -= sizeof(capInfo);
			}

			wmmCap = ' '; /* M (WMM), C (WMM-Call Admission Control)
				       */
			wps_cap = ' '; /* "S" */
			ht_cap = ' '; /* "N" */

			/* "P" for Privacy (WEP) since "W" is WPA, and "2" is
			 * RSN/WPA2 */
			privCap = capInfo.privacy ? 'P' : ' ';

			dot11kCap = capInfo.radio_measurement ? 'K' : ' ';
			pMdie = NULL;

			memset(ssid, 0, MRVDRV_MAX_SSID_LENGTH + 1);

			while (bss_info_length >= 2) {
				pElementId = (IEEEtypes_ElementId_e *)pCurrent;
				pElementLen = pCurrent + 1;
				pCurrent += 2;

				switch (*pElementId) {
				case SSID:
					if (*pElementLen &&
					    *pElementLen <=
						    MRVDRV_MAX_SSID_LENGTH) {
						memcpy(ssid, pCurrent,
						       *pElementLen);
						ssidLen = *pElementLen;
					}
					break;

				case DS_PARAM_SET:
					channel = *pCurrent;
					break;

				case WPA_IE:
					pWpaIe = (IEEEtypes_VendorSpecific_t *)
						pElementId;
					if ((memcmp(pWpaIe->vend_hdr.oui,
						    wpa_oui,
						    sizeof(pWpaIe->vend_hdr
								   .oui)) ==
					     0) &&
					    (pWpaIe->vend_hdr.oui_type ==
					     wpa_oui[3])) {
						/* Don't set privcap if WPA2/RSN
						 * already found */
						if (privCap != '2') {
							/* WPA IE found, 'W' for
							 * WPA */
							privCap = 'W';
						}
					} else {
						pWmmIe =
							(IEEEtypes_WmmParameter_t
								 *)pElementId;
						if ((memcmp(pWmmIe->vend_hdr.oui,
							    wmm_oui,
							    sizeof(pWmmIe->vend_hdr
									   .oui)) ==
						     0) &&
						    (pWmmIe->vend_hdr.oui_type ==
						     wmm_oui[3])) {
							/* Check the subtype: 1
							 * == parameter, 0 ==
							 * info */
							if ((pWmmIe->vend_hdr
								     .oui_subtype ==
							     1) &&
							    pWmmIe->ac_params[WMM_AC_VO]
								    .aci_aifsn
								    .acm) {
								/* Call
								 * admission on
								 * VO; 'C' for
								 * CAC */
								wmmCap = 'C';
							} else {
								/* No CAC; 'M'
								 * for uh, WMM
								 */
								wmmCap = 'M';
							}
						}
					}
					break;

				case RSN_IE:
					/* RSN IE found; '2' for WPA2 (RSN) */
					privCap = '2';
					break;
				case HT_CAPABILITY:
					ht_cap = 'N';
					break;
				case VHT_CAPABILITY:
					vht_cap[0] = 'A';
					vht_cap[1] = 'C';
					break;
				case MDIE:
					/* Mobility domain IE.  Use to indicate
					 * 11r and MDID */
					pMdie = (IEEEtypes_MobilityDomain_t *)
						pElementId;
					break;

				default:
					break;
				}

				pCurrent += *pElementLen;
				bss_info_length -= (2 + *pElementLen);
			}

			if (num_ssid) {
				/* Display filter based on potential multiple
				 * SSID input */

				if (ssidLen) {
					for (tmp_idx = 0;
					     !display && (tmp_idx < num_ssid);
					     tmp_idx++) {
						if ((ssidLen ==
						     ssidList[tmp_idx].max_len) &&
						    (memcmp(ssid,
							    ssidList[tmp_idx]
								    .ssid,
							    ssidLen) == 0)) {
							display = TRUE;
						}
					}
				}
			} else {
				display = TRUE;
			}

			if (display) {
				displayedInfo = TRUE;

				printf("%02u| %03d |%4d |%4d |%3d%% | "
				       "%02x:%02x:%02x:%02x:%02x:%02x |",
				       scanStart + idx, channel,
				       fixed_fields.rssi, fixed_fields.anpi,
				       fixed_fields.chan_load,
				       fixed_fields.bssid[0],
				       fixed_fields.bssid[1],
				       fixed_fields.bssid[2],
				       fixed_fields.bssid[3],
				       fixed_fields.bssid[4],
				       fixed_fields.bssid[5]);

				/* "A" for Adhoc
				 * "I" for Infrastructure,
				 * "D" for DFS (Spectrum Mgmt)
				 */
				printf(" %c%c%c%c%c%c%c%c%c%c | ",
				       capInfo.ibss ? 'A' : 'I',
				       privCap, /* P (WEP), W (WPA), 2 (WPA2) */
				       capInfo.spectrum_mgmt ? 'D' : ' ',
				       wmmCap, /* M (WMM), C (WMM-Call Admission
						  Control) */
				       dot11kCap, /* K */
				       pMdie ? 'R' : ' ', /* 802.11r */
				       wps_cap, /* S */
				       ht_cap, /* N */
				       vht_cap[0], vht_cap[1]); /* AC */

				/* Print out the ssid or the hex values if
				 * non-printable */
				for (ssid_idx = 0; ssid_idx < ssidLen;
				     ssid_idx++) {
					if (isprint(ssid[ssid_idx])) {
						printf("%c", ssid[ssid_idx]);
					} else {
						printf("\\%02x",
						       ssid[ssid_idx]);
					}
				}

				if (pMdie) {
					printf(" [%04x]", pMdie->MDID);
				}
				printf("\n");

				if (argc > 3 && strcmp(argv[3], "tsf") == 0) {
					t_u8 *pByte;

					/* TSF is a t_u64, some formatted
					 * printing libs have trouble printing
					 * long longs, so cast and dump as bytes
					 */
					pByte = (t_u8 *)&fixed_fields
							.network_tsf;
					printf("    TSF=%02x%02x%02x%02x%02x%02x%02x%02x\n",
					       pByte[7], pByte[6], pByte[5],
					       pByte[4], pByte[3], pByte[2],
					       pByte[1], pByte[0]);
				}
			}
		}
		scanStart += pRspInfo->scan_number;

	} while (pRspInfo->scan_number);

	pRspInfo->scan_number = -1;
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t)pRspInfo;
	iwr.u.data.length = sizeof(scanRspBuffer);
	iwr.u.data.flags = subioctl_val;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: getscantable ioctl");
		return -EFAULT;
	}

	if (displayedInfo) {
		if (displayHelp) {
			printf("\n\n"
			       "Capability Legend (Not all may be supported)\n"
			       "-----------------\n"
			       " I [ Infrastructure ]\n"
			       " A [ Ad-hoc ]\n"
			       " P [ WEP ]\n"
			       " W [ WPA IE ]\n"
			       " 2 [ WPA2/RSN IE ]\n"
			       " M [ WMM IE ]\n"
			       " C [ Call Admission Control - WMM IE, VO ACM set ]\n"
			       " D [ Spectrum Management - DFS (11h) ]\n"
			       " K [ 11k ]\n"
			       " R [ 11r ]\n"
			       " S [ WPS ]\n"
			       " N [ HT (11n) ]\n"
			       " AC [HT (11ac) ]\n"
			       "\n\n");
		}
	} else {
		printf("< No Scan Results >\n");
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Request a scan from the driver and display the scan table afterwards
 *
 *  Command line interface for performing a specific immediate scan based
 *    on the following keyword parsing:
 *
 *     bssid=xx:xx:xx:xx:xx:xx  specify a BSSID filter for the scan
 *     ssid="[SSID]"            specify a SSID filter for the scan

 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int process_setuserscan(int argc, char *argv[])
{
	wlan_ioctl_user_scan_cfg scan_req;
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	int arg_idx;
	int tmp_idx;
	int num_ssid;
	unsigned int mac[ETH_ALEN];

	memset(&scan_req, 0x00, sizeof(scan_req));
	num_ssid = 0;

	if (get_priv_ioctl("setuserscan", &ioctl_val, &subioctl_val) ==
	    MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	for (arg_idx = 0; arg_idx < argc; arg_idx++) {
		if (strncmp(argv[arg_idx], "ssid=", strlen("ssid=")) == 0) {
			if (num_ssid < MRVDRV_MAX_SSID_LIST_LENGTH) {
				/*
				 *  "ssid" token string handler
				 */
				strncpy(scan_req.ssid_list[num_ssid].ssid,
					argv[arg_idx] + strlen("ssid="),
					sizeof(scan_req.ssid_list[num_ssid]
						       .ssid));
				scan_req.ssid_list[num_ssid].max_len = 0;
				num_ssid++;
			}
		} else if (strncmp(argv[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			/*
			 *  "bssid" token string handler
			 */
			sscanf(argv[arg_idx] + strlen("bssid="),
			       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0, mac + 1,
			       mac + 2, mac + 3, mac + 4, mac + 5);

			for (tmp_idx = 0;
			     (unsigned int)tmp_idx < NELEMENTS(mac);
			     tmp_idx++) {
				scan_req.specific_bssid[tmp_idx] =
					(t_u8)mac[tmp_idx];
			}
		} else if (strncmp(argv[arg_idx], "group=", strlen("group=")) ==
			   0) {
			sscanf(argv[arg_idx] + strlen("group="), "0x%x",
			       &tmp_idx);
			scan_req.chan_group = tmp_idx;
		}
	}

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t)&scan_req;
	iwr.u.data.length = sizeof(scan_req);
	iwr.u.data.flags = subioctl_val;

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: setuserscan ioctl");
		return -EFAULT;
	}

	process_getscantable(argc, argv);

	return MLAN_STATUS_SUCCESS;
}
