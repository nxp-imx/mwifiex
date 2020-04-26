/** @file  mlanconfig.c
  *
  * @brief Program to configure addition parameters into the mlandriver
  *
  *
  * Copyright 2014-2020 NXP
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
     11/26/2008: initial version
     03/10/2009:  add setuserscan, getscantable etc. commands
     08/11/2009:  add addts, regclass, setra, scanagent etc. commands
************************************************************************/

#include    "mlanconfig.h"
#include    "mlanhostcmd.h"
#include    "mlanmisc.h"

/** mlanconfig version number */
#define MLANCONFIG_VER "M2.0"

/** Initial number of total private ioctl calls */
#define IW_INIT_PRIV_NUM    128
/** Maximum number of total private ioctl calls supported */
#define IW_MAX_PRIV_NUM     1024

/********************************************************
		Local Variables
********************************************************/

/** Private ioctl commands */
enum COMMANDS {
	CMD_HOSTCMD,
	CMD_MEFCFG,
	CMD_CFG_DATA,
#ifdef SDIO
	CMD_CMD52RW,
	CMD_CMD53RW,
#endif
	CMD_GET_SCAN_RSP,
	CMD_SET_USER_SCAN,
	CMD_ADD_TS,
	CMD_DEL_TS,
	CMD_QCONFIG,
	CMD_QSTATS,
	CMD_TS_STATUS,
	CMD_WMM_QSTATUS,
	CMD_REGRW,
	CMD_MEMRW,
	CMD_STA_CUSTOM_IE,
	CMD_STA_MGMT_FRAME_TX,
#ifdef OPCHAN
	CMD_OPCHAN,
	CMD_CHANGROUP,
#endif
};

static t_s8 *commands[] = {
	"hostcmd",
	"mefcfg",
	"cfgdata",
#ifdef SDIO
	"sdcmd52rw",
	"sdcmd53rw",
#endif
	"getscantable",
	"setuserscan",
	"addts",
	"delts",
	"qconfig",
	"qstats",
	"ts_status",
	"qstatus",
	"regrdwr",
	"memrdwr",
	"customie",
	"mgmtframetx",
#ifdef OPCHAN
	"opchan",
	"changroup",
#endif
};

static t_s8 *usage[] = {
	"Usage: ",
	"   mlanconfig -v  (version)",
	"   mlanconfig <mlanX> <cmd> [...]",
	"   where",
	"   mlanX : wireless network interface",
	"   cmd : hostcmd",
	"     : mefcfg",
	"     : customie",
	"     : mgmtframetx",
	"     : cfgdata",
#ifdef SDIO
	"     : sdcmd52rw, sdcmd53rw",
#endif
	"     : getscantable, setuserscan",
	"     : addts, delts, qconfig, qstats, ts_status, qstatus",
	"     : regrdwr, memrdwr",
#ifdef OPCHAN
	"     : opchan, changroup",
#endif
	"     : additional parameter for hostcmd",
	"     :   <filename> <cmd>",
	" 	  : additional parameters for mefcfg are:",
	"     :   <filename>",
	"      : additional parameters for customie are:",
	"      :  <index> <mask> <IE buffer>",
	"      : additional parameters for mgmtframetx are:",
	"      : <pkt file>",
	"     : additional parameter for cfgdata",
	"     :   <type> <filename>",
#ifdef SDIO
	"     : additional parameter for sdcmd52rw",
	"     :   <function> <reg addr.> <data>",
	"     : additional parameter for sdcmd53rw",
	"     :   <func> <addr> <mode> <blksiz> <blknum> <data1> ... ...<dataN> ",
#endif
	"     : additional parameter for addts",
	"     :   <filename.conf> <section# of tspec> <timeout in ms>",
	"     : additional parameter for delts",
	"     :   <filename.conf> <section# of tspec>",
	"     : additional parameter for qconfig",
	"     :   <[set msdu <lifetime in TUs> [Queue Id: 0-3]]",
	"     :    [get [Queue Id: 0-3]] [def [Queue Id: 0-3]]>",
	"     : additional parameter for qstats",
	"     :   <[get [User Priority: 0-7]]>",
	"     : additional parameter for regrdwr",
	"     :   <type> <offset> [value]",
	"     : additional parameter for memrdwr",
	"     :   <address> [value]",
};

t_s32 sockfd;  /**< socket */
t_s8 dev_name[IFNAMSIZ + 1];	/**< device name */
static struct iw_priv_args *priv_args = NULL;	   /**< private args */
static int we_version_compiled = 0;
				  /**< version compiled */

/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/
/**
 *    @brief isdigit for String.
 *
 *    @param x            Char string
 *    @return             MLAN_STATUS_FAILURE for non-digit.
 *                        MLAN_STATUS_SUCCESS for digit
 */
static int
ISDIGIT(t_s8 *x)
{
	unsigned int i;
	for (i = 0; i < strlen(x); i++)
		if (isdigit(x[i]) == 0)
			return MLAN_STATUS_FAILURE;
	return MLAN_STATUS_SUCCESS;
}

/**
 * Check of decimal or hex string
 * @param   num string
 */
#define IS_HEX_OR_DIGIT(num) \
    (strncasecmp("0x", (num), 2)?ISDIGIT((num)):ishexstring((num)))

/**
 *  @brief Get private info.
 *
 *  @param ifname   A pointer to net name
 *  @return 	    MLAN_STATUS_SUCCESS--success, otherwise --fail
 */
static int
get_private_info(const t_s8 *ifname)
{
	/* This function sends the SIOCGIWPRIV command, which is
	 * handled by the kernel and gets the total number of
	 * private ioctl's available in the host driver.
	 */
	struct iwreq iwr;
	int s, ret = MLAN_STATUS_SUCCESS;
	struct iw_priv_args *ppriv = NULL;
	struct iw_priv_args *new_priv;
	int result = 0;
	size_t size = IW_INIT_PRIV_NUM;

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("socket[PF_INET,SOCK_DGRAM]");
		return MLAN_STATUS_FAILURE;
	}

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, ifname, IFNAMSIZ - 1);

	do {
		/* (Re)allocate the buffer */
		new_priv = realloc(ppriv, size * sizeof(ppriv[0]));
		if (new_priv == NULL) {
			printf("Error: Buffer allocation failed\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
		ppriv = new_priv;

		iwr.u.data.pointer = (caddr_t) ppriv;
		iwr.u.data.length = size;
		iwr.u.data.flags = 0;

		if (ioctl(s, SIOCGIWPRIV, &iwr) < 0) {
			result = errno;
			ret = MLAN_STATUS_FAILURE;
			if (result == E2BIG) {
				/* We need a bigger buffer. Check if kernel gave us any hints. */
				if (iwr.u.data.length > size) {
					/* Kernel provided required size */
					size = iwr.u.data.length;
				} else {
					/* No hint from kernel, double the buffer size */
					size *= 2;
				}
			} else {
				/* ioctl error */
				perror("ioctl[SIOCGIWPRIV]");
				break;
			}
		} else {
			/* Success. Return the number of private ioctls */
			priv_args = ppriv;
			ret = iwr.u.data.length;
			break;
		}
	} while (size <= IW_MAX_PRIV_NUM);

	if ((ret == MLAN_STATUS_FAILURE) && (ppriv))
		free(ppriv);

	close(s);

	return ret;
}

/**
 *  @brief Get Sub command ioctl number
 *
 *  @param i        command index
 *  @param priv_cnt     Total number of private ioctls availabe in driver
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return 	        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int
nxp_get_subioctl_no(t_s32 i, t_s32 priv_cnt, int *ioctl_val, int *subioctl_val)
{
	t_s32 j;

	if (priv_args[i].cmd >= SIOCDEVPRIVATE) {
		*ioctl_val = priv_args[i].cmd;
		*subioctl_val = 0;
		return MLAN_STATUS_SUCCESS;
	}

	j = -1;

	/* Find the matching *real* ioctl */

	while ((++j < priv_cnt)
	       && ((priv_args[j].name[0] != '\0') ||
		   (priv_args[j].set_args != priv_args[i].set_args) ||
		   (priv_args[j].get_args != priv_args[i].get_args))) {
	}

	/* If not found... */
	if (j == priv_cnt) {
		printf("%s: Invalid private ioctl definition for: 0x%x\n",
		       dev_name, priv_args[i].cmd);
		return MLAN_STATUS_FAILURE;
	}

	/* Save ioctl numbers */
	*ioctl_val = priv_args[j].cmd;
	*subioctl_val = priv_args[i].cmd;

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get ioctl number
 *
 *  @param ifname   	A pointer to net name
 *  @param priv_cmd	A pointer to priv command buffer
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return 	        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int
nxp_get_ioctl_no(const t_s8 *ifname,
		 const t_s8 *priv_cmd, int *ioctl_val, int *subioctl_val)
{
	t_s32 i;
	t_s32 priv_cnt;
	int ret = MLAN_STATUS_FAILURE;

	priv_cnt = get_private_info(ifname);

	/* Are there any private ioctls? */
	if (priv_cnt <= 0 || priv_cnt > IW_MAX_PRIV_NUM) {
		/* Could skip this message ? */
		printf("%-8.8s  no private ioctls.\n", ifname);
	} else {
		for (i = 0; i < priv_cnt; i++) {
			if (priv_args[i].name[0] &&
			    !strcmp(priv_args[i].name, priv_cmd)) {
				ret = nxp_get_subioctl_no(i, priv_cnt,
							  ioctl_val,
							  subioctl_val);
				break;
			}
		}
	}

	if (priv_args) {
		free(priv_args);
		priv_args = NULL;
	}

	return ret;
}

/**
 *  @brief Retrieve the ioctl and sub-ioctl numbers for the given ioctl string
 *
 *  @param ioctl_name   Private IOCTL string name
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
int
get_priv_ioctl(char *ioctl_name, int *ioctl_val, int *subioctl_val)
{
	int retval;

	retval = nxp_get_ioctl_no(dev_name,
				  ioctl_name, ioctl_val, subioctl_val);

	return retval;
}

/**
 *  @brief Process host_cmd
 *  @param argc		number of arguments
 *  @param argv		A pointer to arguments array
 *  @return      	MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_host_cmd(int argc, char *argv[])
{
	t_s8 cmdname[256];
	t_u8 *buf;
	HostCmd_DS_GEN *hostcmd;
	struct iwreq iwr;
	int ret = MLAN_STATUS_SUCCESS;
	int ioctl_val, subioctl_val;
	FILE *fp = NULL;

	if (get_priv_ioctl("hostcmd",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanconfig mlanX hostcmd <hostcmd.conf> <cmdname>\n");
		exit(1);
	}

	if ((fp = fopen(argv[3], "r")) == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		exit(1);
	}

	buf = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (buf == NULL) {
		printf("Error: allocate memory for hostcmd failed\n");
		fclose(fp);
		return -ENOMEM;
	}
	snprintf(cmdname, sizeof(cmdname), "%s", argv[4]);
	ret = prepare_host_cmd_buffer(fp, cmdname, buf);
	fclose(fp);

	if (ret == MLAN_STATUS_FAILURE)
		goto _exit_;

	hostcmd = (HostCmd_DS_GEN *)buf;
	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = (t_u8 *)hostcmd;
	iwr.u.data.length = le16_to_cpu(hostcmd->size);

	iwr.u.data.flags = 0;
	if (ioctl(sockfd, ioctl_val, &iwr)) {
		fprintf(stderr,
			"mlanconfig: MLANHOSTCMD is not supported by %s\n",
			dev_name);
		ret = MLAN_STATUS_FAILURE;
		goto _exit_;
	}
	ret = process_host_cmd_resp(buf);

_exit_:
	if (buf)
		free(buf);

	return ret;
}

/**
 *  @brief  get range
 *
 *  @return	MLAN_STATUS_SUCCESS--success, otherwise --fail
 */
static int
get_range(t_void)
{
	struct iw_range *range;
	struct iwreq iwr;
	size_t buf_len;

	buf_len = sizeof(struct iw_range) + 500;
	range = malloc(buf_len);
	if (range == NULL) {
		printf("Error: allocate memory for iw_range failed\n");
		return -ENOMEM;
	}
	memset(range, 0, buf_len);
	memset(&iwr, 0, sizeof(struct iwreq));
	iwr.u.data.pointer = (caddr_t) range;
	iwr.u.data.length = buf_len;
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);

	if ((ioctl(sockfd, SIOCGIWRANGE, &iwr)) < 0) {
		printf("Get Range Results Failed\n");
		free(range);
		return MLAN_STATUS_FAILURE;
	}
	we_version_compiled = range->we_version_compiled;
	printf("Driver build with Wireless Extension %d\n",
	       range->we_version_compiled);
	free(range);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Display usage
 *
 *  @return       NA
 */
static t_void
display_usage(t_void)
{
	t_u32 i;
	for (i = 0; i < NELEMENTS(usage); i++)
		fprintf(stderr, "%s\n", usage[i]);
}

/**
 *  @brief Find command
 *
 *  @param maxcmds  max command number
 *  @param cmds     A pointer to commands buffer
 *  @param cmd      A pointer to command buffer
 *  @return         index of command or MLAN_STATUS_FAILURE
 */
static int
findcommand(t_s32 maxcmds, t_s8 *cmds[], t_s8 *cmd)
{
	t_s32 i;

	for (i = 0; i < maxcmds; i++) {
		if (!strcasecmp(cmds[i], cmd)) {
			return i;
		}
	}

	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief Process cfgdata
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cfg_data(int argc, char *argv[])
{
	t_u8 *buf;
	HostCmd_DS_GEN *hostcmd;
	struct iwreq iwr;
	int ret = MLAN_STATUS_SUCCESS;
	int ioctl_val, subioctl_val;
	FILE *fp = NULL;

	if (get_priv_ioctl("hostcmd",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	if (argc < 4 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanconfig mlanX cfgdata <register type> <filename>\n");
		exit(1);
	}

	if (argc == 5) {
		if ((fp = fopen(argv[4], "r")) == NULL) {
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			exit(1);
		}
	}
	buf = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (buf == NULL) {
		printf("Error: allocate memory for hostcmd failed\n");
		if (argc == 5)
			fclose(fp);
		return -ENOMEM;
	}
	ret = prepare_cfg_data_buffer(argc, argv, fp, buf);
	if (argc == 5)
		fclose(fp);

	if (ret == MLAN_STATUS_FAILURE)
		goto _exit_;

	hostcmd = (HostCmd_DS_GEN *)buf;
	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = (t_u8 *)hostcmd;
	iwr.u.data.length = le16_to_cpu(hostcmd->size);

	iwr.u.data.flags = 0;
	if (ioctl(sockfd, ioctl_val, &iwr)) {
		fprintf(stderr,
			"mlanconfig: MLANHOSTCMD is not supported by %s\n",
			dev_name);
		ret = MLAN_STATUS_FAILURE;
		goto _exit_;
	}
	ret = process_host_cmd_resp(buf);

_exit_:
	if (buf)
		free(buf);

	return ret;
}

/**
 *  @brief read current command
 *  @param ptr      A pointer to data
 *  @param curCmd   A pointer to the buf which will hold current command
 *  @return         NULL or the pointer to the left command buf
 */
static t_s8 *
readCurCmd(t_s8 *ptr, t_s8 *curCmd)
{
	t_s32 i = 0;
#define MAX_CMD_SIZE 64	/**< Max command size */

	while (*ptr != ']' && i < (MAX_CMD_SIZE - 1))
		curCmd[i++] = *(++ptr);

	if (*ptr != ']')
		return NULL;

	curCmd[i - 1] = '\0';

	return ++ptr;
}

/**
 *  @brief parse command and hex data
 *  @param fp       A pointer to FILE stream
 *  @param dst      A pointer to the dest buf
 *  @param cmd      A pointer to command buf for search
 *  @return         Length of hex data or MLAN_STATUS_FAILURE
 */
static int
fparse_for_cmd_and_hex(FILE * fp, t_u8 *dst, t_u8 *cmd)
{
	t_s8 *ptr;
	t_u8 *dptr;
	t_s8 buf[256], curCmd[64];
	t_s32 isCurCmd = 0;

	dptr = dst;
	while (fgets(buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && isspace(*ptr))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (*ptr == '[' && *(ptr + 1) != '/') {
				if (!(ptr = readCurCmd(ptr, curCmd)))
					return MLAN_STATUS_FAILURE;

				if (strcasecmp(curCmd, (char *)cmd))	/* Not equal */
					isCurCmd = 0;
				else
					isCurCmd = 1;
			}

			/* Ignore the rest if it is not correct cmd */
			if (!isCurCmd)
				break;

			if (*ptr == '[' && *(ptr + 1) == '/')
				return (dptr - dst);

			if (isxdigit(*ptr)) {
				ptr = convert2hex(ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief Send an ADDTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in an ADDTS request to the associated AP.
 *
 *  Return the execution status of the command as well as the ADDTS response
 *    from the AP if any.
 *
 *  mlanconfig mlanX addts <filename.conf> <section# of tspec> <timeout in ms>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addts(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_addts_req_t addtsReq;

	FILE *fp = NULL;
	char filename[48];
	char config_id[20];

	memset(&addtsReq, 0x00, sizeof(addtsReq));
	memset(filename, 0x00, sizeof(filename));

	if (get_priv_ioctl("addts",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	if (argc != 6) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	strncpy(filename, argv[3], sizeof(filename) - 1);

	if ((fp = fopen(filename, "r")) == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		return -EFAULT;;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	addtsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    addtsReq.ieData,
						    (t_u8 *)config_id);

	if (addtsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)addtsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		if (fp)
			fclose(fp);
		return -EFAULT;
	}

	addtsReq.timeout_ms = atoi(argv[5]);

	printf("Cmd Input:\n");
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.flags = subioctl_val;
	iwr.u.data.pointer = (caddr_t) & addtsReq;
	iwr.u.data.length = (sizeof(addtsReq)
			     - sizeof(addtsReq.ieData)
			     + addtsReq.ieDataLen);

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: addts ioctl");
		if (fp)
			fclose(fp);
		return -EFAULT;
	}

	printf("Cmd Output:\n");
	printf("ADDTS Command Result = %d\n", addtsReq.commandResult);
	printf("ADDTS IEEE Status    = %d\n", addtsReq.ieeeStatusCode);
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');

	if (fp)
		fclose(fp);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Send a DELTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in a DELTS request to the associated AP.
 *
 *  Return the execution status of the command.  There is no response to a
 *    DELTS from the AP.
 *
 *  mlanconfig mlanX delts <filename.conf> <section# of tspec>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_delts(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_delts_req_t deltsReq;

	FILE *fp = NULL;
	char filename[48];
	char config_id[20];

	memset(&deltsReq, 0x00, sizeof(deltsReq));
	memset(filename, 0x00, sizeof(filename));

	if (get_priv_ioctl("delts",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	if (argc != 5) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	strncpy(filename, argv[3], sizeof(filename) - 1);

	if ((fp = fopen(filename, "r")) == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		return -EFAULT;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	deltsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    deltsReq.ieData,
						    (t_u8 *)config_id);

	if (deltsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)deltsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		if (fp)
			fclose(fp);
		return -EFAULT;
	}

	printf("Cmd Input:\n");
	hexdump(config_id, deltsReq.ieData, deltsReq.ieDataLen, ' ');

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.flags = subioctl_val;
	iwr.u.data.pointer = (caddr_t) & deltsReq;
	iwr.u.data.length = (sizeof(deltsReq)
			     - sizeof(deltsReq.ieData)
			     + deltsReq.ieDataLen);

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: delts ioctl");
		if (fp)
			fclose(fp);
		return -EFAULT;
	}

	printf("Cmd Output:\n");
	printf("DELTS Command Result = %d\n", deltsReq.commandResult);
	if (fp)
		fclose(fp);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Send a WMM AC Queue configuration command to get/set/default params
 *
 *  Configure or get the parameters of a WMM AC queue. The command takes
 *    an optional Queue Id as a last parameter.  Without the queue id, all
 *    queues will be acted upon.
 *
 *  mlanconfig mlanX qconfig set msdu <lifetime in TUs> [Queue Id: 0-3]
 *  mlanconfig mlanX qconfig get [Queue Id: 0-3]
 *  mlanconfig mlanX qconfig def [Queue Id: 0-3]
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qconfig(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_queue_config_t queue_config_cmd;
	mlan_wmm_ac_e ac_idx;
	mlan_wmm_ac_e ac_idx_start;
	mlan_wmm_ac_e ac_idx_stop;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (argc < 4) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	if (get_priv_ioctl("qconfig",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	memset(&queue_config_cmd, 0x00, sizeof(queue_config_cmd));
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = (caddr_t) & queue_config_cmd;
	iwr.u.data.length = sizeof(queue_config_cmd);
	iwr.u.data.flags = subioctl_val;

	if (strcmp(argv[3], "get") == 0) {
		/*    3     4    5   */
		/* qconfig get [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				return -EINVAL;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_GET;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
				perror("qconfig ioctl");
			} else {
				printf("qconfig %s(%d): MSDU Lifetime GET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else if (strcmp(argv[3], "set") == 0) {
		if ((argc >= 5) && strcmp(argv[4], "msdu") == 0) {
			/*    3     4    5     6      7   */
			/* qconfig set msdu <value> [qid] */
			if (argc == 6) {
				ac_idx_start = WMM_AC_BK;
				ac_idx_stop = WMM_AC_VO;
			} else if (argc == 7) {
				if (atoi(argv[6]) < WMM_AC_BK ||
				    atoi(argv[6]) > WMM_AC_VO) {
					fprintf(stderr,
						"ERROR: Invalid Queue ID!\n");
					return -EINVAL;
				}
				ac_idx_start = atoi(argv[6]);
				ac_idx_stop = ac_idx_start;
			} else {
				fprintf(stderr,
					"Invalid number of parameters!\n");
				return -EINVAL;
			}
			queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_SET;
			queue_config_cmd.msduLifetimeExpiry = atoi(argv[5]);

			for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop;
			     ac_idx++) {
				queue_config_cmd.accessCategory = ac_idx;
				if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
					perror("qconfig ioctl");
				} else {
					printf("qconfig %s(%d): MSDU Lifetime SET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
				}
			}
		} else {
			/* Only MSDU Lifetime provisioning accepted for now */
			fprintf(stderr, "Invalid set parameter: s/b [msdu]\n");
			return -EINVAL;
		}
	} else if (strncmp(argv[3], "def", strlen("def")) == 0) {
		/*    3     4    5   */
		/* qconfig def [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				return -EINVAL;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_DEFAULT;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
				perror("qconfig ioctl");
			} else {
				printf("qconfig %s(%d): MSDU Lifetime DEFAULT = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else {
		fprintf(stderr,
			"Invalid qconfig command; s/b [set, get, default]\n");
		return -EINVAL;
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Turn on/off or retrieve and clear the queue statistics for a UP
 *
 *  Turn the statistics collection on/off for a given UP or retrieve the
 *    current accumulated stats and clear them from the firmware.  The command
 *    takes an optional Queue Id as a last parameter.  Without the queue id,
 *    all queues will be acted upon.
 *
 *  mlanconfig mlanX qstats get [User Priority: 0-7]
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qstats(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_queue_stats_t queue_stats_cmd;
	t_u8 up_idx;
	t_u8 up_idx_start;
	t_u8 up_idx_stop;
	t_u16 usedTime[MAX_USER_PRIORITIES];
	t_u16 policedTime[MAX_USER_PRIORITIES];

	const char *ac_str_tbl[] = { "BE", "BK", "BK", "BE",
		"VI", "VI", "VO", "VO"
	};

	if (argc < 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	if (get_priv_ioctl("qstats",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	printf("\n");

	memset(usedTime, 0x00, sizeof(usedTime));
	memset(policedTime, 0x00, sizeof(policedTime));
	memset(&queue_stats_cmd, 0x00, sizeof(queue_stats_cmd));

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = (caddr_t) & queue_stats_cmd;
	iwr.u.data.length = sizeof(queue_stats_cmd);
	iwr.u.data.flags = subioctl_val;

	if ((argc > 3) && strcmp(argv[3], "on") == 0) {
		if (argc == 4) {
			up_idx_start = 0;
			up_idx_stop = 7;
		} else if (argc == 5) {
			if (atoi(argv[4]) < 0 || atoi(argv[4]) > 7) {
				fprintf(stderr,
					"ERROR: Invalid User Priority!\n");
				return -EINVAL;
			}
			up_idx_start = atoi(argv[4]);
			up_idx_stop = up_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_stats_cmd.action = WMM_STATS_ACTION_START;
		for (up_idx = up_idx_start; up_idx <= up_idx_stop; up_idx++) {
			queue_stats_cmd.userPriority = up_idx;
			if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
				perror("qstats ioctl");
			} else {
				printf("qstats UP%d, %s turned on\n",
				       up_idx, ac_str_tbl[up_idx]);
			}
		}
	} else if ((argc > 3) && strcmp(argv[3], "off") == 0) {
		if (argc == 4) {
			up_idx_start = 0;
			up_idx_stop = 7;
		} else if (argc == 5) {
			if (atoi(argv[4]) < 0 || atoi(argv[4]) > 7) {
				fprintf(stderr,
					"ERROR: Invalid User Priority!\n");
				return -EINVAL;
			}
			up_idx_start = atoi(argv[4]);
			up_idx_stop = up_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_stats_cmd.action = WMM_STATS_ACTION_STOP;
		for (up_idx = up_idx_start; up_idx <= up_idx_stop; up_idx++) {
			queue_stats_cmd.userPriority = up_idx;
			if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
				perror("qstats ioctl");
			} else {
				printf("qstats UP%d, %s turned off\n",
				       up_idx, ac_str_tbl[up_idx]);
			}
		}
	} else if ((argc >= 3) &&
		   ((argc == 3) ? 1 : (strcmp(argv[3], "get") == 0))) {
		/* If the user types: "mlanconfig mlanX qstats" without get argument.
		 *   The mlanconfig application invokes "get" option for all UPs
		 */
		if ((argc == 4) || (argc == 3)) {
			up_idx_start = 0;
			up_idx_stop = 7;
		} else if (argc == 5) {
			if (atoi(argv[4]) < 0 || atoi(argv[4]) > 7) {
				fprintf(stderr,
					"ERROR: Invalid User Priority!\n");
				return -EINVAL;
			}
			up_idx_start = atoi(argv[4]);
			up_idx_stop = up_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		printf("%s %6s %5s %8s %8s %6s %6s %6s %6s %6s %6s\n",
		       "AC-UP", "Count", "Loss", "TxDly", "QDly",
		       "<=5", "<=10", "<=20", "<=30", "<=50", ">50");
		printf("----------------------------------"
		       "---------------------------------------------\n");
		queue_stats_cmd.action = WMM_STATS_ACTION_GET_CLR;

		for (up_idx = up_idx_start; up_idx <= up_idx_stop; up_idx++) {
			queue_stats_cmd.userPriority = up_idx;
			if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
				perror("qstats ioctl");
			} else {
				printf(" %s-%d %6u %5u %8u %8u %6u %6u %6u %6u %6u %6u\n", ac_str_tbl[up_idx], up_idx, queue_stats_cmd.pktCount, queue_stats_cmd.pktLoss, (unsigned int)queue_stats_cmd.avgTxDelay, (unsigned int)queue_stats_cmd.avgQueueDelay, queue_stats_cmd.delayHistogram[0], queue_stats_cmd.delayHistogram[1], queue_stats_cmd.delayHistogram[2], queue_stats_cmd.delayHistogram[3], (queue_stats_cmd.delayHistogram[4] + queue_stats_cmd.delayHistogram[5]), queue_stats_cmd.delayHistogram[6]);

				usedTime[up_idx] = queue_stats_cmd.usedTime;
				policedTime[up_idx] =
					queue_stats_cmd.policedTime;
			}
		}

		printf("----------------------------------"
		       "---------------------------------------------\n");
		printf("\nAC-UP      UsedTime      PolicedTime\n");
		printf("------------------------------------\n");

		for (up_idx = up_idx_start; up_idx <= up_idx_stop; up_idx++) {
			printf(" %s-%d        %6u           %6u\n",
			       ac_str_tbl[up_idx],
			       up_idx,
			       (unsigned int)usedTime[up_idx],
			       (unsigned int)policedTime[up_idx]);
		}
	} else {
		fprintf(stderr, "Invalid qstats command;\n");
		return -EINVAL;
	}
	printf("\n");

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get the current status of the WMM Queues
 *
 *  Command: mlanconfig mlanX qstatus
 *
 *  Retrieve the following information for each AC if wmm is enabled:
 *        - WMM IE ACM Required
 *        - Firmware Flow Required
 *        - Firmware Flow Established
 *        - Firmware Queue Enabled
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_qstatus(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_queue_status_t qstatus;
	mlan_wmm_ac_e acVal;

	if (get_priv_ioctl("qstatus",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	memset(&qstatus, 0x00, sizeof(qstatus));

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.flags = subioctl_val;
	iwr.u.data.pointer = (caddr_t) & qstatus;
	iwr.u.data.length = (sizeof(qstatus));

	if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
		perror("mlanconfig: qstatus ioctl");
		return -EFAULT;
	}

	for (acVal = WMM_AC_BK; acVal <= WMM_AC_VO; acVal++) {
		switch (acVal) {
		case WMM_AC_BK:
			printf("BK: ");
			break;
		case WMM_AC_BE:
			printf("BE: ");
			break;
		case WMM_AC_VI:
			printf("VI: ");
			break;
		case WMM_AC_VO:
			printf("VO: ");
			break;
		default:
			printf("??: ");
		}

		printf("ACM[%c], FlowReq[%c], FlowCreated[%c], Enabled[%c],"
		       " DE[%c], TE[%c]\n",
		       (qstatus.acStatus[acVal].wmmAcm ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowRequired ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowCreated ? 'X' : ' '),
		       (qstatus.acStatus[acVal].disabled ? ' ' : 'X'),
		       (qstatus.acStatus[acVal].deliveryEnabled ? 'X' : ' '),
		       (qstatus.acStatus[acVal].triggerEnabled ? 'X' : ' '));
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get the current status of the WMM Traffic Streams
 *
 *  Command: mlanconfig mlanX ts_status
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_ts_status(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	struct iwreq iwr;
	wlan_ioctl_wmm_ts_status_t ts_status;
	int tid;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (get_priv_ioctl("ts_status",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	printf("\nTID   Valid    AC   UP   PSB   FlowDir  MediumTime\n");
	printf("---------------------------------------------------\n");

	for (tid = 0; tid <= 7; tid++) {
		memset(&ts_status, 0x00, sizeof(ts_status));
		ts_status.tid = tid;

		strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
		iwr.u.data.flags = subioctl_val;
		iwr.u.data.pointer = (caddr_t) & ts_status;
		iwr.u.data.length = (sizeof(ts_status));

		if (ioctl(sockfd, ioctl_val, &iwr) < 0) {
			perror("mlanconfig: ts_status ioctl");
			return -EFAULT;
		}

		printf(" %02d     %3s    %2s    %u     %c    ",
		       ts_status.tid,
		       (ts_status.valid ? "Yes" : "No"),
		       (ts_status.
			valid ? ac_str_tbl[ts_status.accessCategory] : "--"),
		       ts_status.userPriority, (ts_status.psb ? 'U' : 'L'));

		if ((ts_status.flowDir & 0x03) == 0) {
			printf("%s", " ---- ");
		} else {
			printf("%2s%4s",
			       (ts_status.flowDir & 0x01 ? "Up" : ""),
			       (ts_status.flowDir & 0x02 ? "Down" : ""));
		}

		printf("%12u\n", ts_status.mediumTime);
	}

	printf("\n");

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Provides interface to perform read/write operations on regsiter
 *
 *  Command: mlanconfig mlanX regrdwr <type> <offset> [value]
 *
 *  @param argc     Number of arguments
 *  @param argv     Pointer to the arguments
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_regrdwr(int argc, char *argv[])
{
	struct iwreq iwr;
	int ioctl_val, subioctl_val;
	t_u32 type, offset, value;
	t_u8 buf[100];
	HostCmd_DS_GEN *hostcmd = (HostCmd_DS_GEN *)buf;
	int ret = MLAN_STATUS_SUCCESS;

	/* Check arguments */
	if ((argc < 5) || (argc > 6)) {
		printf("Parameters for regrdwr: <type> <offset> [value]\n");
		return -EINVAL;
	}

	if (get_priv_ioctl("hostcmd",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	type = a2hex_or_atoi(argv[3]);
	offset = a2hex_or_atoi(argv[4]);
	if (argc > 5)
		value = a2hex_or_atoi(argv[5]);
	if ((ret = prepare_hostcmd_regrdwr(type, offset,
					   (argc > 5) ? &value : NULL, buf))) {
		return ret;
	}

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = buf;
	iwr.u.data.length = le16_to_cpu(hostcmd->size);
	iwr.u.data.flags = 0;

	if (ioctl(sockfd, ioctl_val, &iwr)) {
		fprintf(stderr,
			"mlanconfig: MLANHOSTCMD is not supported by %s\n",
			dev_name);
		return MLAN_STATUS_FAILURE;
	}
	ret = process_host_cmd_resp(buf);

	return ret;
}

/**
 *  @brief Provides interface to perform read/write the adapter memory
 *
 *  Command: mlanconfig mlanX memrdwr <address> [value]
 *
 *  @param argc     Number of arguments
 *  @param argv     Pointer to the arguments
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_memrdwr(int argc, char *argv[])
{
	struct iwreq iwr;
	int ioctl_val, subioctl_val;
	t_u32 address, value;
	t_u8 buf[100] = { 0 };
	HostCmd_DS_MEM *pmem;
	HostCmd_DS_GEN *hostcmd = (HostCmd_DS_GEN *)buf;
	int ret = MLAN_STATUS_SUCCESS;

	pmem = (HostCmd_DS_MEM *)(buf + S_DS_GEN);

	/* Check arguments */
	if ((argc < 4) || (argc > 5)) {
		printf("Parameters for memrdwr: <address> [value]\n");
		return -EINVAL;
	}

	if (get_priv_ioctl("hostcmd",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}

	address = a2hex_or_atoi(argv[3]);
	pmem->addr = cpu_to_le32(address);
	if (argc > 4) {
		pmem->action = cpu_to_le16(HostCmd_ACT_GEN_SET);
		value = a2hex_or_atoi(argv[4]);
		pmem->value = cpu_to_le32(value);
	} else {
		pmem->action = cpu_to_le16(HostCmd_ACT_GEN_GET);
		pmem->value = 0;
	}
	hostcmd->command = cpu_to_le16(HostCmd_CMD_MEM_ACCESS);
	hostcmd->size = cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_MEM));
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	memset(&iwr, 0, sizeof(iwr));
	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = buf;
	iwr.u.data.length = le16_to_cpu(hostcmd->size);
	iwr.u.data.flags = 0;

	if (ioctl(sockfd, ioctl_val, &iwr)) {
		fprintf(stderr,
			"mlanconfig: MLANHOSTCMD is not supported by %s\n",
			dev_name);
		return MLAN_STATUS_FAILURE;
	}
	ret = process_host_cmd_resp(buf);

	return ret;
}

/** custom IE, auto mask value */
#define	CUSTOM_IE_AUTO_MASK	0xffff

/**
 * @brief Show usage information for the customie
 * command
 *
 * $return         N/A
 **/
void
print_custom_ie_usage(void)
{
	printf("\nUsage : customie [INDEX] [MASK] [IEBuffer]");
	printf("\n         empty - Get all IE settings\n");
	printf("\n         INDEX:  0 - Get/Set IE index 0 setting");
	printf("\n                 1 - Get/Set IE index 1 setting");
	printf("\n                 2 - Get/Set IE index 2 setting");
	printf("\n                 3 - Get/Set IE index 3 setting");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                -1 - Append/Delete IE automatically");
	printf("\n                     Delete will delete the IE from the matching IE buffer");
	printf("\n                     Append will append the IE to the buffer with the same mask");
	printf("\n         MASK :  Management subtype mask value as per bit defintions");
	printf("\n              :  Bit 0 - Association request.");
	printf("\n              :  Bit 1 - Association response.");
	printf("\n              :  Bit 2 - Reassociation request.");
	printf("\n              :  Bit 3 - Reassociation response.");
	printf("\n              :  Bit 4 - Probe request.");
	printf("\n              :  Bit 5 - Probe response.");
	printf("\n              :  Bit 8 - Beacon.");
	printf("\n         MASK :  MASK = 0 to clear the mask and the IE buffer");
	printf("\n         IEBuffer :  IE Buffer in hex (max 256 bytes)\n\n");
	return;
}

/**
 * @brief Converts a string to hex value
 *
 * @param str      A pointer to the string
 * @param raw      A pointer to the raw data buffer
 * @return         Number of bytes read
 **/
int
string2raw(char *str, unsigned char *raw)
{
	int len = (strlen(str) + 1) / 2;

	do {
		if (!isxdigit(*str)) {
			return -1;
		}
		*str = toupper(*str);
		*raw = CHAR2INT(*str) << 4;
		++str;
		*str = toupper(*str);
		if (*str == '\0')
			break;
		*raw |= CHAR2INT(*str);
		++raw;
	} while (*++str != '\0');
	return len;
}

/**
 * @brief Creates a hostcmd request for custom IE settings
 * and sends to the driver
 *
 * Usage: "customie [INDEX] [MASK] [IEBuffer]"
 *
 * Options: INDEX :      0 - Get/Delete IE index 0 setting
 *                       1 - Get/Delete IE index 1 setting
 *                       2 - Get/Delete IE index 2 setting
 *                       3 - Get/Delete IE index 3 setting
 *                       .
 *                       .
 *                       .
 *                      -1 - Append IE at the IE buffer with same MASK
 *          MASK  :      Management subtype mask value
 *          IEBuffer:    IE Buffer in hex
 *   					       empty - Get all IE settings
 *
 * @param argc     Number of arguments
 * @param argv     Pointer to the arguments
 * @return         N/A
 **/
static int
process_custom_ie(int argc, char *argv[])
{
	tlvbuf_custom_ie *tlv = NULL;
	tlvbuf_max_mgmt_ie *max_mgmt_ie_tlv = NULL;
	custom_ie *ie_ptr = NULL;
	t_u8 *buffer = NULL;
	t_u16 buf_len = 0;
	t_u16 mgmt_subtype_mask = 0;
	int ie_buf_len = 0, ie_len = 0, i = 0;
	struct ifreq ifr;

	/* mlanconfig mlan0 customie idx flag buf */
	if (argc > 6) {
		printf("ERR:Too many arguments.\n");
		print_custom_ie_usage();
		return MLAN_STATUS_FAILURE;
	}
	/* Error checks and initialize the command length */
	if (argc > 3) {
		if (((IS_HEX_OR_DIGIT(argv[3]) == MLAN_STATUS_FAILURE) &&
		     (atoi(argv[3]) != -1)) || (atoi(argv[3]) < -1)) {
			printf("ERR:Illegal index %s\n", argv[3]);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
	}
	switch (argc) {
	case 3:
		buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
		break;
	case 4:
		if (atoi(argv[3]) < 0) {
			printf("ERR:Illegal index %s. Must be either greater than or equal to 0 for Get Operation \n", argv[3]);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
		break;
	case 5:
		if (MLAN_STATUS_FAILURE == ishexstring(argv[4]) ||
		    A2HEXDECIMAL(argv[4]) != 0) {
			printf("ERR: Mask value should be 0 to clear IEBuffers.\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		if (atoi(argv[3]) == -1) {
			printf("ERR: You must provide buffer for automatic deletion.\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		buf_len = sizeof(tlvbuf_custom_ie) + sizeof(custom_ie);
		break;
	case 6:
		/* This is to check negative numbers and special symbols */
		if (MLAN_STATUS_FAILURE == IS_HEX_OR_DIGIT(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		/* If above check is passed and mask is not hex, then it must be 0 */
		if ((ISDIGIT(argv[4]) == MLAN_STATUS_SUCCESS) && atoi(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n ");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		if (MLAN_STATUS_FAILURE == ishexstring(argv[5])) {
			printf("ERR:Only hex digits are allowed\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		ie_buf_len = strlen(argv[5]);
		if (!strncasecmp("0x", argv[5], 2)) {
			ie_len = (ie_buf_len - 2 + 1) / 2;
			argv[5] += 2;
		} else
			ie_len = (ie_buf_len + 1) / 2;
		if (ie_len > MAX_IE_BUFFER_LEN) {
			printf("ERR:Incorrect IE length %d\n", ie_buf_len);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		mgmt_subtype_mask = (t_u16)A2HEXDECIMAL(argv[4]);
		buf_len = sizeof(tlvbuf_custom_ie) + sizeof(custom_ie) + ie_len;
		break;
	}
	/* Initialize the command buffer */
	buffer = (t_u8 *)malloc(buf_len);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	memset(buffer, 0, buf_len);
	tlv = (tlvbuf_custom_ie *)buffer;
	tlv->tag = MRVL_MGMT_IE_LIST_TLV_ID;
	if (argc == 3 || argc == 4) {
		if (argc == 3)
			tlv->length = 0;
		else {
			tlv->length = sizeof(t_u16);
			ie_ptr = (custom_ie *)(tlv->ie_data);
			ie_ptr->ie_index = (t_u16)(atoi(argv[3]));
		}
	} else {
		/* Locate headers */
		ie_ptr = (custom_ie *)(tlv->ie_data);
		/* Set TLV fields */
		tlv->length = sizeof(custom_ie) + ie_len;
		ie_ptr->ie_index = atoi(argv[3]);
		ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
		ie_ptr->ie_length = ie_len;
		if (argc == 6)
			string2raw(argv[5], ie_ptr->ie_buffer);
	}
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)buffer;
	/* Perform ioctl */
	if (ioctl(sockfd, CUSTOM_IE_CFG, &ifr)) {
		perror("ioctl[CUSTOM_IE_CFG]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Print response */
	if (argc > 4) {
		printf("custom IE setting successful\n");
	} else {
		printf("Querying custom IE successful\n");
		tlv = (tlvbuf_custom_ie *)buffer;
		ie_len = tlv->length;
		ie_ptr = (custom_ie *)(tlv->ie_data);
		while ((unsigned int)ie_len >= sizeof(custom_ie)) {
			printf("Index [%d]\n", ie_ptr->ie_index);
			if (ie_ptr->ie_length)
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask) == 0 ?
				       CUSTOM_IE_AUTO_MASK :
				       (ie_ptr->mgmt_subtype_mask));
			else
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask));
			hexdump("IE Buffer", (void *)ie_ptr->ie_buffer,
				ie_ptr->ie_length, ' ');
			ie_len -= sizeof(custom_ie) + ie_ptr->ie_length;
			ie_ptr = (custom_ie *)((t_u8 *)ie_ptr +
					       sizeof(custom_ie) +
					       ie_ptr->ie_length);
		}
	}
	max_mgmt_ie_tlv =
		(tlvbuf_max_mgmt_ie *)(buffer + sizeof(tlvbuf_custom_ie) +
				       tlv->length);
	if (max_mgmt_ie_tlv) {
		if (max_mgmt_ie_tlv->tag == MRVL_MAX_MGMT_IE_TLV_ID) {
			for (i = 0; i < max_mgmt_ie_tlv->count; i++) {
				printf("buf%d_size = %d\n", i,
				       max_mgmt_ie_tlv->info[i].buf_size);
				printf("number of buffers = %d\n",
				       max_mgmt_ie_tlv->info[i].buf_count);
				printf("\n");
			}
		}
	}
	if (buffer)
		free(buffer);

	return MLAN_STATUS_SUCCESS;
}

/********************************************************
		Global Functions
********************************************************/
/**
 *  @brief Get one line from the File
 *
 *  @param fp       File handler
 *  @param str	    Storage location for data.
 *  @param size 	Maximum number of characters to read.
 *  @param lineno	A pointer to return current line number
 *  @return         returns string or NULL
 */
char *
mlan_config_get_line(FILE * fp, t_s8 *str, t_s32 size, int *lineno)
{
	char *start, *end;
	int out, next_line;

	if (!fp || !str)
		return NULL;

	do {
read_line:
		if (!fgets(str, size, fp))
			break;
		start = str;
		start[size - 1] = '\0';
		end = start + strlen(str);
		(*lineno)++;

		out = 1;
		while (out && (start < end)) {
			next_line = 0;
			/* Remove empty lines and lines starting with # */
			switch (start[0]) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
				start++;
				break;
			case '#':
			case '\n':
			case '\0':
				next_line = 1;
				break;
			case '\r':
				if (start[1] == '\n')
					next_line = 1;
				else
					start++;
				break;
			default:
				out = 0;
				break;
			}
			if (next_line)
				goto read_line;
		}

		/* Remove # comments unless they are within a double quoted
		 * string. Remove trailing white space. */
		if ((end = strstr(start, "\""))) {
			if (!(end = strstr(end + 1, "\"")))
				end = start;
		} else
			end = start;

		if ((end = strstr(end + 1, "#")))
			*end-- = '\0';
		else
			end = start + strlen(start) - 1;

		out = 1;
		while (out && (start < end)) {
			switch (*end) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
			case '\n':
			case '\r':
				*end = '\0';
				end--;
				break;
			default:
				out = 0;
				break;
			}
		}

		if (*start == '\0')
			continue;

		return start;
	} while (1);

	return NULL;
}

/**
 *  @brief parse hex data
 *  @param fp       File handler
 *  @param dst      A pointer to receive hex data
 *  @return         length of hex data
 */
int
fparse_for_hex(FILE * fp, t_u8 *dst)
{
	t_s8 *ptr;
	t_u8 *dptr;
	t_s8 buf[256];

	dptr = dst;
	while (fgets(buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && (isspace(*ptr) || *ptr == '\t'))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (isxdigit(*ptr)) {
				ptr = convert2hex(ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return (dptr - dst);
}

#define STACK_NBYTES            	100	/**< Number of bytes in stack */
#define MAX_BYTESEQ 		       	6	/**< Maximum byte sequence */
#define TYPE_DNUM           		1 /**< decimal number */
#define TYPE_BYTESEQ        		2 /**< byte sequence */
#define MAX_OPERAND         		0x40	/**< Maximum operands */
#define TYPE_EQ         		(MAX_OPERAND+1)	/**< byte comparison:    == operator */
#define TYPE_EQ_DNUM    		(MAX_OPERAND+2)	/**< decimal comparison: =d operator */
#define TYPE_EQ_BIT     		(MAX_OPERAND+3)	/**< bit comparison:     =b operator */
#define TYPE_AND        		(MAX_OPERAND+4)	/**< && operator */
#define TYPE_OR         		(MAX_OPERAND+5)	/**< || operator */

typedef struct {
	t_u16 sp;		      /**< Stack pointer */
	t_u8 byte[STACK_NBYTES];      /**< Stack */
} mstack_t;

typedef struct {
	t_u8 type;		  /**< Type */
	t_u8 reserve[3];   /**< so 4-byte align val array */
	/* byte sequence is the largest among all the operands and operators. */
	/* byte sequence format: 1 byte of num of bytes, then variable num bytes */
	t_u8 val[MAX_BYTESEQ + 1];/**< Value */
} op_t;

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param nbytes		number of byte to push to stack
 *
 *  @param val			a pointer to data buffer
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push_n(mstack_t * s, t_u8 nbytes, t_u8 *val)
{
	if ((s->sp + nbytes) < STACK_NBYTES) {
		memcpy((void *)(s->byte + s->sp), (const void *)val,
		       (size_t) nbytes);
		s->sp += nbytes;
		/* printf("push: n %d sp %d\n", nbytes, s->sp); */
		return TRUE;
	} else			/* stack full */
		return FALSE;
}

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param op			a pointer to op_t structure
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push(mstack_t * s, op_t * op)
{
	t_u8 nbytes;
	switch (op->type) {
	case TYPE_DNUM:
		if (push_n(s, 4, op->val))
			return (push_n(s, 1, &op->type));
		return FALSE;
	case TYPE_BYTESEQ:
		nbytes = op->val[0];
		if (push_n(s, nbytes, op->val + 1) &&
		    push_n(s, 1, op->val) && push_n(s, 1, &op->type))
			return TRUE;
		return FALSE;
	default:
		return (push_n(s, 1, &op->type));
	}
}

/**
 *  @brief parse RPN string
 *
 *  @param s			a pointer to Null-terminated string to scan.
 *
 *  @param first_time		a pointer to return first_time
 *
 *  @return			A pointer to the last token found in string.
 *  				NULL is returned when there are no more tokens to be found.
 *
 */
static char *
getop(char *s, int *first_time)
{
	const char delim[] = " \t\n";
	char *p;
	if (*first_time) {
		p = strtok(s, delim);
		*first_time = FALSE;
	} else {
		p = strtok(NULL, delim);
	}
	return (p);
}

/**
 *  @brief Verify hex digit.
 *
 *  @param c			input ascii char
 *  @param h			a pointer to return integer value of the digit char.
 *  @return			TURE -- c is hex digit, FALSE -- c is not hex digit.
 */
static int
ishexdigit(char c, t_u8 *h)
{
	if (c >= '0' && c <= '9') {
		*h = c - '0';
		return (TRUE);
	} else if (c >= 'a' && c <= 'f') {
		*h = c - 'a' + 10;
		return (TRUE);
	} else if (c >= 'A' && c <= 'F') {
		*h = c - 'A' + 10;
		return (TRUE);
	}
	return (FALSE);
}

/**
 *  @brief convert hex string to integer.
 *
 *  @param s			A pointer to hex string, string length up to 2 digits.
 *  @return			integer value.
 */
static t_u8
hex_atoi(char *s)
{
	int i;
	t_u8 digit;		/* digital value */
	t_u8 t = 0;		/* total value */

	for (i = 0, t = 0; ishexdigit(s[i], &digit) && i < 2; i++)
		t = 16 * t + digit;
	return (t);
}

/**
 *  @brief Parse byte sequence in hex format string to a byte sequence.
 *
 *  @param opstr		A pointer to byte sequence in hex format string, with ':' as delimiter between two byte.
 *  @param val			A pointer to return byte sequence string
 *  @return			NA
 */
static void
parse_hex(char *opstr, t_u8 *val)
{
	char delim = ':';
	char *p;
	char *q;
	t_u8 i;

	/* +1 is for skipping over the preceding h character. */
	p = opstr + 1;

	/* First byte */
	val[1] = hex_atoi(p++);

	/* Parse subsequent bytes. */
	/* Each byte is preceded by the : character. */
	for (i = 1; *p; i++) {
		q = strchr(p, delim);
		if (!q)
			break;
		p = q + 1;
		val[i + 1] = hex_atoi(p);
	}
	/* Set num of bytes */
	val[0] = i;
}

/**
 *  @brief str2bin, convert RPN string to binary format
 *
 *  @param str			A pointer to rpn string
 *  @param stack		A pointer to mstack_t structure
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
str2bin(char *str, mstack_t * stack)
{
	int first_time;
	char *opstr;
	op_t op;		/* operator/operand */
	int dnum;
	int ret = MLAN_STATUS_SUCCESS;

	memset(stack, 0, sizeof(mstack_t));
	first_time = TRUE;
	while ((opstr = getop(str, &first_time)) != NULL) {
		if (isdigit(*opstr)) {
			op.type = TYPE_DNUM;
			dnum = cpu_to_le32(atoi(opstr));
			memcpy((t_u8 *)op.val, &dnum, sizeof(dnum));
			if (!push(stack, &op)) {
				printf("push decimal number failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (*opstr == 'h') {
			op.type = TYPE_BYTESEQ;
			parse_hex(opstr, op.val);
			if (!push(stack, &op)) {
				printf("push byte sequence failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "==")) {
			op.type = TYPE_EQ;
			if (!push(stack, &op)) {
				printf("push byte cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=d")) {
			op.type = TYPE_EQ_DNUM;
			if (!push(stack, &op)) {
				printf("push decimal cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=b")) {
			op.type = TYPE_EQ_BIT;
			if (!push(stack, &op)) {
				printf("push bit cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "&&")) {
			op.type = TYPE_AND;
			if (!push(stack, &op)) {
				printf("push AND operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "||")) {
			op.type = TYPE_OR;
			if (!push(stack, &op)) {
				printf("push OR operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else {
			printf("Unknown operand\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
	}
	return ret;
}

/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 *                  MAC_BROADCAST   - if breadcast mac
 *                  MAC_MULTICAST   - if multicast mac
 */
static int
mac2raw(char *mac, t_u8 *raw)
{
	unsigned int temp_raw[ETH_ALEN];
	int num_tokens = 0;
	int i;

	if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1))) {
		return MLAN_STATUS_FAILURE;
	}
	num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x",
			    temp_raw + 0, temp_raw + 1, temp_raw + 2,
			    temp_raw + 3, temp_raw + 4, temp_raw + 5);
	if (num_tokens != ETH_ALEN) {
		return MLAN_STATUS_FAILURE;
	}
	for (i = 0; i < num_tokens; i++)
		raw[i] = (t_u8)temp_raw[i];

	if (memcmp(raw, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) {
		return MAC_BROADCAST;
	} else if (raw[0] & 0x01) {
		return MAC_MULTICAST;
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief          Parse function for a configuration line
 *
 *  @param s        Storage buffer for data
 *  @param size     Maximum size of data
 *  @param stream   File stream pointer
 *  @param line     Pointer to current line within the file
 *  @param _pos     Output string or NULL
 *  @return         String or NULL
 */
static char *
config_get_line(char *s, int size, FILE * stream, int *line, char **_pos)
{
	*_pos = mlan_config_get_line(stream, s, size, line);
	return *_pos;
}

/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @return         Number of arguments in the line or EOF
 */
static int
parse_line(char *line, char *args[])
{
	int arg_num = 0;
	int is_start = 0;
	int is_quote = 0;
	int length = 0;
	int i = 0;

	arg_num = 0;
	length = strlen(line);
	/* Process line */

	/* Find number of arguments */
	is_start = 0;
	is_quote = 0;
	for (i = 0; i < length; i++) {
		/* Ignore leading spaces */
		if (is_start == 0) {
			if (line[i] == ' ') {
				continue;
			} else if (line[i] == '\t') {
				continue;
			} else if (line[i] == '\n') {
				break;
			} else {
				is_start = 1;
				args[arg_num] = &line[i];
				arg_num++;
			}
		}
		if (is_start == 1) {
			/* Ignore comments */
			if (line[i] == '#') {
				if (is_quote == 0) {
					line[i] = '\0';
					arg_num--;
				}
				break;
			}
			/* Separate by '=' */
			if (line[i] == '=') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Separate by ',' */
			if (line[i] == ',') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Change ',' to ' ', but not inside quotes */
			if ((line[i] == ',') && (is_quote == 0)) {
				line[i] = ' ';
				continue;
			}
		}
		/* Remove newlines */
		if (line[i] == '\n') {
			line[i] = '\0';
		}
		/* Check for quotes */
		if (line[i] == '"') {
			is_quote = (is_quote == 1) ? 0 : 1;
			continue;
		}
		if (((line[i] == ' ') || (line[i] == '\t')) && (is_quote == 0)) {
			line[i] = '\0';
			is_start = 0;
			continue;
		}
	}
	return arg_num;
}

#define FILTER_BYTESEQ 		TYPE_EQ	/**< byte sequence */
#define FILTER_DNUM    		TYPE_EQ_DNUM /**< decimal number */
#define FILTER_BITSEQ		TYPE_EQ_BIT /**< bit sequence */
#define FILTER_TEST		FILTER_BITSEQ+1	/**< test */

#define NAME_TYPE		1	    /**< Field name 'type' */
#define NAME_PATTERN		2	/**< Field name 'pattern' */
#define NAME_OFFSET		3	    /**< Field name 'offset' */
#define NAME_NUMBYTE		4	/**< Field name 'numbyte' */
#define NAME_REPEAT		5	    /**< Field name 'repeat' */
#define NAME_BYTE		6	    /**< Field name 'byte' */
#define NAME_MASK		7	    /**< Field name 'mask' */
#define NAME_DEST		8	    /**< Field name 'dest' */

static struct mef_fields {
	t_s8 *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name Id. */
} mef_fields[] = {
	{
	"type", NAME_TYPE}, {
	"pattern", NAME_PATTERN}, {
	"offset", NAME_OFFSET}, {
	"numbyte", NAME_NUMBYTE}, {
	"repeat", NAME_REPEAT}, {
	"byte", NAME_BYTE}, {
	"mask", NAME_MASK}, {
	"dest", NAME_DEST}
};

/**
 *  @brief get filter data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_filter_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s32 errors = 0, i;
	t_s8 line[256], *pos, *pos1;
	t_u16 type = 0;
	t_u32 pattern = 0;
	t_u16 repeat = 0;
	t_u16 offset = 0;
	t_s8 byte_seq[50];
	t_s8 mask_seq[50];
	t_u16 numbyte = 0;
	t_s8 type_find = 0;
	t_s8 pattern_find = 0;
	t_s8 offset_find = 0;
	t_s8 numbyte_find = 0;
	t_s8 repeat_find = 0;
	t_s8 byte_find = 0;
	t_s8 mask_find = 0;
	t_s8 dest_find = 0;
	t_s8 dest_seq[50];

	*size = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_filter line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		for (i = 0; (t_u32)i < NELEMENTS(mef_fields); i++) {
			if (strncmp
			    (pos, mef_fields[i].name,
			     strlen(mef_fields[i].name)) == 0) {
				switch (mef_fields[i].nameid) {
				case NAME_TYPE:
					type = a2hex_or_atoi(pos1);
					if ((type != FILTER_DNUM) &&
					    (type != FILTER_BYTESEQ)
					    && (type != FILTER_BITSEQ) &&
					    (type != FILTER_TEST)) {
						printf("Invalid filter type:%d\n", type);
						return MLAN_STATUS_FAILURE;
					}
					type_find = 1;
					break;
				case NAME_PATTERN:
					pattern = a2hex_or_atoi(pos1);
					pattern_find = 1;
					break;
				case NAME_OFFSET:
					offset = a2hex_or_atoi(pos1);
					offset_find = 1;
					break;
				case NAME_NUMBYTE:
					numbyte = a2hex_or_atoi(pos1);
					numbyte_find = 1;
					break;
				case NAME_REPEAT:
					repeat = a2hex_or_atoi(pos1);
					repeat_find = 1;
					break;
				case NAME_BYTE:
					memset(byte_seq, 0, sizeof(byte_seq));
					strncpy(byte_seq, pos1,
						(sizeof(byte_seq) - 1));
					byte_find = 1;
					break;
				case NAME_MASK:
					memset(mask_seq, 0, sizeof(mask_seq));
					strncpy(mask_seq, pos1,
						(sizeof(mask_seq) - 1));
					mask_find = 1;
					break;
				case NAME_DEST:
					memset(dest_seq, 0, sizeof(dest_seq));
					strncpy(dest_seq, pos1,
						(sizeof(dest_seq) - 1));
					dest_find = 1;
					break;
				}
				break;
			}
		}
		if (i == NELEMENTS(mef_fields)) {
			printf("Line %d: unknown mef field '%s'.\n",
			       *line, pos);
			errors++;
		}
	}
	if (type_find == 0) {
		printf("Can not find filter type\n");
		return MLAN_STATUS_FAILURE;
	}
	switch (type) {
	case FILTER_DNUM:
		if (!pattern_find || !offset_find || !numbyte_find) {
			printf("Missing field for FILTER_DNUM: pattern=%d,offset=%d,numbyte=%d\n", pattern_find, offset_find, numbyte_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d %d %d =d ", pattern, offset,
			 numbyte);
		break;
	case FILTER_BYTESEQ:
		if (!byte_find || !offset_find || !repeat_find) {
			printf("Missing field for FILTER_BYTESEQ: byte=%d,offset=%d,repeat=%d\n", byte_find, offset_find, repeat_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d h%s %d == ", repeat, byte_seq,
			 offset);
		break;
	case FILTER_BITSEQ:
		if (!byte_find || !offset_find || !mask_find) {
			printf("Missing field for FILTER_BITSEQ: byte=%d,offset=%d,mask_find=%d\n", byte_find, offset_find, mask_find);
			return MLAN_STATUS_FAILURE;
		}
		if (strlen(byte_seq) != strlen(mask_seq)) {
			printf("byte string's length is different with mask's length!\n");
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s =b ", byte_seq, offset,
			 mask_seq);
		break;
	case FILTER_TEST:
		if (!byte_find || !offset_find || !repeat_find || !dest_find) {
			printf("Missing field for FILTER_TEST: byte=%d,offset=%d,repeat=%d,dest=%d\n", byte_find, offset_find, repeat_find, dest_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s %d ", dest_seq, repeat,
			 byte_seq, offset);
		break;
	}
	memcpy(buf, line, strlen(line));
	*size = strlen(line);
	return MLAN_STATUS_SUCCESS;
}

#define NAME_MODE	1	/**< Field name 'mode' */
#define NAME_ACTION	2	/**< Field name 'action' */
#define NAME_FILTER_NUM	3   /**< Field name 'filter_num' */
#define NAME_RPN	4	/**< Field name 'RPN' */
static struct mef_entry_fields {
	t_s8 *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name id */
} mef_entry_fields[] = {
	{
	"mode", NAME_MODE}, {
	"action", NAME_ACTION}, {
	"filter_num", NAME_FILTER_NUM}, {
"RPN", NAME_RPN},};

typedef struct _MEF_ENTRY {
    /** Mode */
	t_u8 Mode;
    /** Size */
	t_u8 Action;
    /** Size of expression */
	t_u16 ExprSize;
} MEF_ENTRY;

/**
 *  @brief get mef_entry data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_mef_entry_data(FILE * fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s8 line[256], *pos, *pos1;
	t_u8 mode, action, filter_num = 0;
	t_s8 rpn[256];
	t_s8 mode_find = 0;
	t_s8 action_find = 0;
	t_s8 filter_num_find = 0;
	t_s8 rpn_find = 0;
	t_s8 rpn_str[256];
	int rpn_len = 0;
	t_s8 filter_name[50];
	t_s8 name_found = 0;
	t_u16 len = 0;
	int i;
	int first_time = TRUE;
	char *opstr;
	t_s8 filter_action[10];
	t_s32 errors = 0;
	MEF_ENTRY *pMefEntry = (MEF_ENTRY *) buf;
	mstack_t stack;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_entry line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		if (!mode_find || !action_find || !filter_num_find || !rpn_find) {
			for (i = 0;
			     (unsigned int)i < NELEMENTS(mef_entry_fields);
			     i++) {
				if (strncmp
				    (pos, mef_entry_fields[i].name,
				     strlen(mef_entry_fields[i].name)) == 0) {
					switch (mef_entry_fields[i].nameid) {
					case NAME_MODE:
						mode = a2hex_or_atoi(pos1);
						if (mode & ~0x7) {
							printf("invalid mode=%d\n", mode);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Mode = mode;
						mode_find = 1;
						break;
					case NAME_ACTION:
						action = a2hex_or_atoi(pos1);
						if (action & ~0xff) {
							printf("invalid action=%d\n", action);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Action = action;
						action_find = 1;
						break;
					case NAME_FILTER_NUM:
						filter_num =
							a2hex_or_atoi(pos1);
						filter_num_find = 1;
						break;
					case NAME_RPN:
						memset(rpn, 0, sizeof(rpn));
						strncpy(rpn, pos1,
							(sizeof(rpn) - 1));
						rpn_find = 1;
						break;
					}
					break;
				}
			}
			if (i == NELEMENTS(mef_fields)) {
				printf("Line %d: unknown mef_entry field '%s'.\n", *line, pos);
				return MLAN_STATUS_FAILURE;
			}
		}
		if (mode_find && action_find && filter_num_find && rpn_find) {
			for (i = 0; i < filter_num; i++) {
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				snprintf(filter_name, sizeof(filter_name),
					 "%s={", opstr);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     ln))) {
					if (strncmp
					    (pos, filter_name,
					     strlen(filter_name)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanconfig: %s not found in file\n",
						filter_name);
					return MLAN_STATUS_FAILURE;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_filter_data(fp, ln,
							 (t_u8 *)(rpn_str +
								  rpn_len),
							 &len))
					break;
				rpn_len += len;
				if (i > 0) {
					memcpy(rpn_str + rpn_len, filter_action,
					       strlen(filter_action));
					rpn_len += strlen(filter_action);
				}
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				memset(filter_action, 0, sizeof(filter_action));
				snprintf(filter_action, sizeof(filter_action),
					 "%s ", opstr);
			}
			/* Remove the last space */
			if (rpn_len > 0) {
				rpn_len--;
				rpn_str[rpn_len] = 0;
			}
			if (MLAN_STATUS_FAILURE == str2bin(rpn_str, &stack)) {
				printf("Fail on str2bin!\n");
				return MLAN_STATUS_FAILURE;
			}
			*size = sizeof(MEF_ENTRY);
			pMefEntry->ExprSize = cpu_to_le16(stack.sp);
			memmove(buf + sizeof(MEF_ENTRY), stack.byte, stack.sp);
			*size += stack.sp;
			break;
		} else if (mode_find && action_find && filter_num_find &&
			   (filter_num == 0)) {
			pMefEntry->ExprSize = 0;
			*size = sizeof(MEF_ENTRY);
			break;
		}
	}
	return MLAN_STATUS_SUCCESS;
}

#define MEFCFG_CMDCODE	0x009a
/**
 *  @brief Process mef cfg
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mef_cfg(int argc, char *argv[])
{
	int ioctl_val, subioctl_val;
	t_s8 line[256], cmdname[256], *pos;
	int cmdname_found = 0, name_found = 0;
	int ln = 0;
	int ret = MLAN_STATUS_SUCCESS;
	int i;
	t_u8 *buf;
	t_u16 buf_len = 0;
	t_u16 len = 0;
	struct iwreq iwr;
	HostCmd_DS_MEF_CFG *mefcmd;
	HostCmd_DS_GEN *hostcmd;
	FILE *fp = NULL;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanconfig eth1 mefcfg <mef.conf>\n");
		exit(1);
	}
	if (get_priv_ioctl("hostcmd",
			   &ioctl_val, &subioctl_val) == MLAN_STATUS_FAILURE) {
		return -EOPNOTSUPP;
	}
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[2]);
	cmdname_found = 0;
	if ((fp = fopen(argv[3], "r")) == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[4]);
		exit(1);
	}

	buf = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (buf == NULL) {
		fclose(fp);
		fprintf(stderr, "Cannot alloc memory\n");
		exit(1);
	}
	memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

	hostcmd = (HostCmd_DS_GEN *)(buf);
	hostcmd->command = cpu_to_le16(MEFCFG_CMDCODE);
	mefcmd = (HostCmd_DS_MEF_CFG *)(buf + S_DS_GEN);
	buf_len = sizeof(HostCmd_DS_MEF_CFG) + S_DS_GEN;

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "Criteria=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->Criteria =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanconfig: criteria not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "NumEntries=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->NumEntries =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanconfig: NumEntries not found in file '%s'\n",
					argv[3]);
				break;
			}
			for (i = 0; i < mefcmd->NumEntries; i++) {
				snprintf(cmdname, sizeof(cmdname),
					 "mef_entry_%d={", i);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanconfig: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_mef_entry_data(fp, &ln,
							    buf + buf_len,
							    &len)) {
					ret = MLAN_STATUS_FAILURE;
					break;
				}
				buf_len += len;
			}
			break;
		}
	}
	fclose(fp);
	/* hexdump("mef_cfg",buf,buf_len, ' '); */
	if (!cmdname_found)
		fprintf(stderr,
			"mlanconfig: cmdname '%s' not found in file '%s'\n",
			argv[4], argv[3]);

	if (!cmdname_found || !name_found) {
		ret = MLAN_STATUS_FAILURE;
		goto mef_exit;
	}
	hostcmd->size = cpu_to_le16(buf_len);
	mefcmd->Criteria = cpu_to_le32(mefcmd->Criteria);
	mefcmd->NumEntries = cpu_to_le16(mefcmd->NumEntries);

	strncpy(iwr.ifr_name, dev_name, IFNAMSIZ - 1);
	iwr.u.data.pointer = buf;
	iwr.u.data.length = buf_len;
	iwr.u.data.flags = 0;
	if (ioctl(sockfd, ioctl_val, &iwr)) {
		fprintf(stderr, "mlanconfig: MEFCFG is not supported by %s\n",
			dev_name);
		ret = MLAN_STATUS_FAILURE;
		goto mef_exit;
	}
	ret = process_host_cmd_resp(buf);

mef_exit:
	if (buf)
		free(buf);
	return ret;

}

/**
 *  @brief Process transmission of mgmt frames
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmt_frame_tx(int argc, char *argv[])
{
	struct ifreq ifr;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, arg_num = 0, ret = 0, i = 0;
	char *args[100] = { NULL }, *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 data_len = 0, subtype = 0;
	wlan_mgmt_frame_tx *pmgmt_frame;
	t_u8 *buffer = NULL;
	pkt_header *hdr = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanconfig mlanX mgmtframetx <config/pkt.conf>\n");
		exit(1);
	}

	data_len = sizeof(wlan_mgmt_frame_tx);

	buffer = (t_u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
	hdr = (pkt_header *)buffer;
	pmgmt_frame = (wlan_mgmt_frame_tx *)(buffer + sizeof(pkt_header));

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		arg_num = parse_line(line, args);
		if (strcmp(args[0], "PktSubType") == 0) {
			subtype = (t_u16)A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl |= subtype << 4;
		} else if (strncmp(args[0], "Addr", 4) == 0) {
			strncpy(mac_addr, args[1], 20);
			if ((ret =
			     mac2raw(mac_addr,
				     peer_mac)) != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			i = atoi(args[0] + 4);
			switch (i) {
			case 1:
				memcpy(pmgmt_frame->addr1, peer_mac, ETH_ALEN);
				break;
			case 2:
				memcpy(pmgmt_frame->addr2, peer_mac, ETH_ALEN);
				break;
			case 3:
				memcpy(pmgmt_frame->addr3, peer_mac, ETH_ALEN);
				break;
			case 4:
				memcpy(pmgmt_frame->addr4, peer_mac, ETH_ALEN);
				break;
			}
		} else if (strcmp(args[0], "Data") == 0) {
			for (i = 0; i < arg_num - 1; i++)
				pmgmt_frame->payload[i] =
					(t_u8)A2HEXDECIMAL(args[i + 1]);
			data_len += arg_num - 1;
		}
	}
	pmgmt_frame->frm_len = data_len - sizeof(pmgmt_frame->frm_len);
#define MRVL_PKT_TYPE_MGMT_FRAME 0xE5
	hdr->pkt_len = data_len;
	hdr->TxPktType = MRVL_PKT_TYPE_MGMT_FRAME;
	hdr->TxControl = 0;
	hexdump("Frame Tx", buffer, data_len + sizeof(pkt_header), ' ');
	/* Send collective command */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)buffer;

	/* Perform ioctl */
	if (ioctl(sockfd, FRAME_TX_IOCTL, &ifr)) {
		perror("");
		printf("ERR:Could not send management frame.\n");
	} else {
		printf("Mgmt Frame sucessfully sent.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Entry function for mlanconfig
 *  @param argc		number of arguments
 *  @param argv     A pointer to arguments array
 *  @return      	MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
main(int argc, char *argv[])
{
	t_s32 cmd;

	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		fprintf(stdout, "NXP mlanconfig version %s\n", MLANCONFIG_VER);
		exit(0);
	}
	if (argc < 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		display_usage();
		exit(1);
	}

	strncpy(dev_name, argv[1], IFNAMSIZ - 1);

	/*
	 * create a socket
	 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "mlanconfig: Cannot open socket.\n");
		exit(1);
	}
	if (get_range() < 0) {
		fprintf(stderr, "mlanconfig: Cannot get range.\n");
		close(sockfd);
		exit(1);
	}
	switch ((cmd = findcommand(NELEMENTS(commands), commands, argv[2]))) {
	case CMD_HOSTCMD:
		process_host_cmd(argc, argv);
		break;
	case CMD_MEFCFG:
		process_mef_cfg(argc, argv);
		break;
	case CMD_CFG_DATA:
		process_cfg_data(argc, argv);
		break;
#ifdef SDIO
	case CMD_CMD52RW:
		process_sdcmd52rw(argc, argv);
		break;
	case CMD_CMD53RW:
		process_sdcmd53rw(argc, argv);
		break;
#endif
	case CMD_GET_SCAN_RSP:
		process_getscantable(argc, argv);
		break;
	case CMD_SET_USER_SCAN:
		process_setuserscan(argc, argv);
		break;
	case CMD_ADD_TS:
		process_addts(argc, argv);
		break;
	case CMD_DEL_TS:
		process_delts(argc, argv);
		break;
	case CMD_QCONFIG:
		process_qconfig(argc, argv);
		break;
	case CMD_QSTATS:
		process_qstats(argc, argv);
		break;
	case CMD_TS_STATUS:
		process_wmm_ts_status(argc, argv);
		break;
	case CMD_WMM_QSTATUS:
		process_wmm_qstatus(argc, argv);
		break;
	case CMD_REGRW:
		process_regrdwr(argc, argv);
		break;
	case CMD_MEMRW:
		process_memrdwr(argc, argv);
		break;
#ifdef OPCHAN
	case CMD_OPCHAN:
		process_opchan(argc, argv);
		break;

	case CMD_CHANGROUP:
		process_changroup(argc, argv);
		break;
#endif
	case CMD_STA_CUSTOM_IE:
		process_custom_ie(argc, argv);
		break;
	case CMD_STA_MGMT_FRAME_TX:
		process_mgmt_frame_tx(argc, argv);
		break;
	default:
		fprintf(stderr, "Invalid command specified!\n");
		display_usage();
		close(sockfd);
		exit(1);
	}

	close(sockfd);
	return MLAN_STATUS_SUCCESS;
}
