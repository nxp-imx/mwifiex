/** @file  mlanutl.c
 *
 * @brief Program to control parameters in the mlandriver
 *
 *
 * Copyright 2011-2021 NXP
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
     11/04/2011: initial version
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <sys/stat.h>
#include <net/ethernet.h>

#include "mlanutl.h"

/** Supported stream modes */
#define HT_STREAM_MODE_1X1 0x11
#define HT_STREAM_MODE_2X2 0x22

/** mlanutl version number */
#define MLANUTL_VER "M1.3.02"

/** Termination flag */
int terminate_flag = 0;

/********************************************************
			Local Variables
********************************************************/

/** Stringification of rateId enumeration */
const char *rateIdStr[] = {"1",	 "2",  "5.5", "11", "--", "6",	"9",  "12",
			   "18", "24", "36",  "48", "54", "--", "M0", "M1",
			   "M2", "M3", "M4",  "M5", "M6", "M7", "H0", "H1",
			   "H2", "H3", "H4",  "H5", "H6", "H7"};

#ifdef DEBUG_LEVEL1
#define MMSG MBIT(0)
#define MFATAL MBIT(1)
#define MERROR MBIT(2)
#define MDATA MBIT(3)
#define MCMND MBIT(4)
#define MEVENT MBIT(5)
#define MINTR MBIT(6)
#define MIOCTL MBIT(7)

#define MREG_D MBIT(9)

#define MMPA_D MBIT(15)
#define MDAT_D MBIT(16)
#define MCMD_D MBIT(17)
#define MEVT_D MBIT(18)
#define MFW_D MBIT(19)
#define MIF_D MBIT(20)

#ifdef DEBUG_LEVEL2
#define MENTRY MBIT(28)
#define MWARN MBIT(29)
#define MINFO MBIT(30)
#endif
#endif

static int process_version(int argc, char *argv[]);
static int process_verext(int argc, char *argv[]);
static int process_hostcmd(int argc, char *argv[]);
#ifdef DEBUG_LEVEL1
static int process_drvdbg(int argc, char *argv[]);
#endif
static int process_datarate(int argc, char *argv[]);
static int process_getlog(int argc, char *argv[]);
static int process_get_txpwrlimit(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_get_signal(int argc, char *argv[]);
static int process_get_signal_ext(int argc, char *argv[]);
static int process_signalext_cfg(int argc, char *argv[]);
#endif
static int process_vhtcfg(int argc, char *argv[]);
static int process_dyn_bw(int argc, char *argv[]);
static int process_11axcfg(int argc, char *argv[]);
static int process_11axcmdcfg(int argc, char *argv[]);
static int process_txratecfg(int argc, char *argv[]);
static int process_httxcfg(int argc, char *argv[]);
static int process_htcapinfo(int argc, char *argv[]);
static int process_addbapara(int argc, char *argv[]);
static int process_aggrpriotbl(int argc, char *argv[]);
static int process_addbareject(int argc, char *argv[]);
static int process_hssetpara(int argc, char *argv[]);
static int process_mefcfg(int argc, char *argv[]);
static int process_cloud_keep_alive(int argc, char *argv[]);

struct command_node command_list[] = {
	{"version", process_version},
	{"verext", process_verext},
	{"hostcmd", process_hostcmd},
#ifdef DEBUG_LEVEL1
	{"drvdbg", process_drvdbg},
#endif
	{"getdatarate", process_datarate},
	{"getlog", process_getlog},
	{"get_txpwrlimit", process_get_txpwrlimit},
#ifdef STA_SUPPORT
	{"getsignal", process_get_signal},
	{"getsignalext", process_get_signal_ext},
	{"getsignalextv2", process_get_signal_ext},
	{"signalextcfg", process_signalext_cfg},
#endif
	{"vhtcfg", process_vhtcfg},
	{"dyn_bw", process_dyn_bw},
	{"11axcfg", process_11axcfg},
	{"11axcmd", process_11axcmdcfg},
	{"txratecfg", process_txratecfg},
	{"addbapara", process_addbapara},
	{"aggrpriotbl", process_aggrpriotbl},
	{"addbareject", process_addbareject},
	{"httxcfg", process_httxcfg},
	{"htcapinfo", process_htcapinfo},
	{"hssetpara", process_hssetpara},
	{"mefcfg", process_mefcfg},
	{"cloud_keep_alive", process_cloud_keep_alive},
};

static char *usage[] = {
	"Usage: ",
	"   mlanutl -v  (version)",
	"   mlanutl <ifname> <cmd> [...]",
	"   where",
	"   ifname : wireless network interface name, such as mlanX or uapX",
	"   cmd :",
	"         version",
	"         verext",
	"         hostcmd",
#ifdef DEBUG_LEVEL1
	"         drvdbg",
#endif
	"         getdatarate",
	"         getlog",
	"         get_txpwrlimit",
#ifdef STA_SUPPORT
	"         getsignal",
	"         signalextcfg",
	"         getsignalext",
	"         getsignalextv2",
#endif
	"         vhtcfg",
	"         dyn_bw",
	"         11axcfg",
	"         11axcmd",
	"         txratecfg",
	"         httxcfg",
	"         htcapinfo",
	"         aggrpriotbl",
	"         addbapara",
	"         addbareject",
	"         hssetpara",
	"         mefcfg",
	"         cloud_keep_alive",
};

/** Socket */
t_s32 sockfd;
/** Device name */
char dev_name[IFNAMSIZ + 1];
#define HOSTCMD "hostcmd"

static char *config_get_line(char *s, int size, FILE *stream, int *line,
			     char **_pos);
#define BSSID_FILTER 1
#define SSID_FILTER 2
/********************************************************
			Global Variables
********************************************************/

int setuserscan_filter = 0;
int num_ssid_filter = 0;
/********************************************************
			Local Functions
********************************************************/
/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char to convert
 *  @return         Hex integer or 0
 */
static int hexval(t_s32 chr)
{
	if (chr >= '0' && chr <= '9')
		return chr - '0';
	if (chr >= 'A' && chr <= 'F')
		return chr - 'A' + 10;
	if (chr >= 'a' && chr <= 'f')
		return chr - 'a' + 10;

	return 0;
}

/**
 *  @brief Hump hex data
 *
 *  @param prompt   A pointer prompt buffer
 *  @param p        A pointer to data buffer
 *  @param len      The len of data buffer
 *  @param delim    Delim char
 *  @return         Hex integer
 */
t_void hexdump(char *prompt, t_void *p, t_s32 len, char delim)
{
	t_s32 i;
	t_u8 *s = p;

	if (prompt) {
		printf("%s: len=%d\n", prompt, (int)len);
	}
	for (i = 0; i < len; i++) {
		if (i != len - 1)
			printf("%02x%c", *s++, delim);
		else
			printf("%02x\n", *s);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
}

/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char
 *  @return         Hex integer
 */
static t_u8 hexc2bin(char chr)
{
	if (chr >= '0' && chr <= '9')
		chr -= '0';
	else if (chr >= 'A' && chr <= 'F')
		chr -= ('A' - 10);
	else if (chr >= 'a' && chr <= 'f')
		chr -= ('a' - 10);

	return chr;
}

/**
 *  @brief Convert string to hex integer
 *
 *  @param s        A pointer string buffer
 *  @return         Hex integer
 */
static t_u32 a2hex(char *s)
{
	t_u32 val = 0;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}

	while (*s && isxdigit((unsigned char)*s)) {
		val = (val << 4) + hexc2bin(*s++);
	}

	return val;
}

/*
 *  @brief Convert String to integer
 *
 *  @param value    A pointer to string
 *  @return         Integer
 */
static t_u32 a2hex_or_atoi(char *value)
{
	if (value[0] == '0' && (value[1] == 'X' || value[1] == 'x')) {
		return a2hex(value + 2);
	} else {
		return (t_u32)atoi(value);
	}
}

/**
 *  @brief Convert string to hex
 *
 *  @param ptr      A pointer to data buffer
 *  @param chr      A pointer to return integer
 *  @return         A pointer to next data field
 */
static char *convert2hex(char *ptr, t_u8 *chr)
{
	t_u8 val;

	for (val = 0; *ptr && isxdigit((unsigned char)*ptr); ptr++) {
		val = (val * 16) + hexval(*ptr);
	}

	*chr = val;

	return ptr;
}

/**
 *  @brief Display usage
 *
 *  @return       NA
 */
static t_void display_usage(t_void)
{
	t_u32 i;
	for (i = 0; i < NELEMENTS(usage); i++)
		fprintf(stderr, "%s\n", usage[i]);
}

/**
 *  @brief Find and execute command
 *
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS for success, otherwise failure
 */
static int process_command(int argc, char *argv[])
{
	int i = 0, ret = MLAN_STATUS_NOTFOUND;
	struct command_node *node = NULL;

	for (i = 0; i < (int)NELEMENTS(command_list); i++) {
		node = &command_list[i];
		if (!strcasecmp(node->name, argv[2])) {
			ret = node->handler(argc, argv);
			break;
		}
	}

	return ret;
}

/**
 *  @brief Prepare command buffer
 *  @param buffer   Command buffer to be filled
 *  @param cmd      Command id
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int prepare_buffer(t_u8 *buffer, char *cmd, t_u32 num, char *args[])
{
	t_u8 *pos = NULL;
	unsigned int i = 0;

	memset(buffer, 0, BUFFER_LENGTH);

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
 *  @brief Trims leading and traling spaces only
 *  @param str  A pointer to argument string
 *  @return     pointer to trimmed string
 */
static char *trim_spaces(char *str)
{
	char *str_end = NULL;

	if (!str)
		return NULL;

	/* Trim leading spaces */
	while (!*str && isspace((unsigned char)*str))
		str++;

	if (*str == 0) /* All spaces? */
		return str;

	/* Trim trailing spaces */
	str_end = str + strlen(str) - 1;
	while (str_end > str && isspace((unsigned char)*str_end))
		str_end--;

	/* null terminate the string */
	*(str_end + 1) = '\0';

	return str;
}

/**
 *  @brief read current command
 *  @param ptr      A pointer to data
 *  @param curCmd   A pointer to the buf which will hold current command
 *  @return         NULL or the pointer to the left command buf
 */
static t_s8 *readCurCmd(t_s8 *ptr, t_s8 *curCmd)
{
	t_s32 i = 0;
#define MAX_CMD_SIZE 64 /**< Max command size */

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
static int fparse_for_cmd_and_hex(FILE *fp, t_u8 *dst, t_u8 *cmd)
{
	t_s8 *ptr;
	t_u8 *dptr;
	t_s8 buf[256], curCmd[64] = {0};
	t_s32 isCurCmd = 0;

	dptr = dst;
	while (fgets((char *)buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && isspace((unsigned char)*ptr))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (*ptr == '[' && *(ptr + 1) != '/') {
				ptr = readCurCmd(ptr, curCmd);
				if (!ptr)
					return MLAN_STATUS_FAILURE;

				if (strcasecmp((char *)curCmd,
					       (char *)cmd)) /* Not equal */
					isCurCmd = 0;
				else
					isCurCmd = 1;
			}

			/* Ignore the rest if it is not correct cmd */
			if (!isCurCmd)
				break;

			if (*ptr == '[' && *(ptr + 1) == '/')
				return dptr - dst;

			if (isxdigit((unsigned char)*ptr)) {
				ptr = (t_s8 *)convert2hex((char *)ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief Process version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_version(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: version fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Version string received: %s\n", buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process extended version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_verext(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX verext [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: verext fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (cmd->used_len)
		printf("Extended Version string received: %s\n", buffer);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

int process_host_cmd_resp(char *cmd_name, t_u8 *buf);

/**
 *  @brief Get one line from the File
 *
 *  @param fp       File handler
 *  @param str      Storage location for data.
 *  @param size     Maximum number of characters to read.
 *  @param lineno   A pointer to return current line number
 *  @return         returns string or NULL
 */
static char *mlan_config_get_line(FILE *fp, char *str, t_s32 size, int *lineno)
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
			case ' ': /* White space */
			case '\t': /* Tab */
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
		end = strstr(start, "\"");
		if (end) {
			end = strstr(end + 1, "\"");
			if (!end)
				end = start;
		} else
			end = start;

		end = strstr(end + 1, "#");
		if (end)
			*end-- = '\0';
		else
			end = start + strlen(start) - 1;

		out = 1;
		while (out && (start < end)) {
			switch (*end) {
			case ' ': /* White space */
			case '\t': /* Tab */
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
 *  @brief          Parse function for a configuration line
 *
 *  @param s        Storage buffer for data
 *  @param size     Maximum size of data
 *  @param stream   File stream pointer
 *  @param line     Pointer to current line within the file
 *  @param _pos     Output string or NULL
 *  @return         String or NULL
 */
static char *config_get_line(char *s, int size, FILE *stream, int *line,
			     char **_pos)
{
	*_pos = mlan_config_get_line(stream, s, size, line);
	return *_pos;
}

/**
 *  @brief get hostcmd data
 *
 *  @param ln           A pointer to line number
 *  @param buf          A pointer to hostcmd data
 *  @param size         A pointer to the return size of hostcmd buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static int mlan_get_hostcmd_data(FILE *fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s32 errors = 0, i;
	char line[512], *pos, *pos1, *pos2, *pos3;
	t_u16 len;

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		(*ln)++;
		if (strcmp(pos, "}") == 0) {
			break;
		}

		pos1 = strchr(pos, ':');
		if (pos1 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';

		pos2 = strchr(pos1, '=');
		if (pos2 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos2++ = '\0';

		len = a2hex_or_atoi(pos1);
		if (len < 1 || len > BUFFER_LENGTH) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}

		*size += len;

		if (*pos2 == '"') {
			pos2++;
			pos3 = strchr(pos2, '"');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = '\0';
			memset(buf, 0, len);
			memmove(buf, pos2, MIN(strlen(pos2), len));
			buf += len;
		} else if (*pos2 == '\'') {
			pos2++;
			pos3 = strchr(pos2, '\'');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = ',';
			for (i = 0; i < len; i++) {
				pos3 = strchr(pos2, ',');
				if (pos3 != NULL) {
					*pos3 = '\0';
					*buf++ = (t_u8)a2hex_or_atoi(pos2);
					pos2 = pos3 + 1;
				} else
					*buf++ = 0;
			}
		} else if (*pos2 == '{') {
			t_u16 tlvlen = 0, tmp_tlvlen;
			mlan_get_hostcmd_data(fp, ln, buf + len, &tlvlen);
			tmp_tlvlen = tlvlen;
			while (len--) {
				*buf++ = (t_u8)(tmp_tlvlen & 0xff);
				tmp_tlvlen >>= 8;
			}
			*size += tlvlen;
			buf += tlvlen;
		} else {
			t_u32 value = a2hex_or_atoi(pos2);
			while (len--) {
				*buf++ = (t_u8)(value & 0xff);
				value >>= 8;
			}
		}
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prepare host-command buffer
 *  @param fp       File handler
 *  @param cmd_name Command name
 *  @param buf      A pointer to comand buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int prepare_host_cmd_buffer(FILE *fp, char *cmd_name, t_u8 *buf)
{
	char line[256], cmdname[256], *pos, cmdcode[10];
	HostCmd_DS_GEN *hostcmd;
	t_u32 hostcmd_size = 0;
	int ln = 0;
	int cmdname_found = 0, cmdcode_found = 0;

	hostcmd = (HostCmd_DS_GEN *)(buf + sizeof(t_u32));
	hostcmd->command = 0xffff;

	snprintf(cmdname, sizeof(cmdname), "%s={", cmd_name);
	cmdname_found = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdcode, sizeof(cmdcode), "CmdCode=");
			cmdcode_found = 0;
			while ((pos = mlan_config_get_line(
					fp, line, sizeof(line), &ln))) {
				if (strncmp(pos, cmdcode, strlen(cmdcode)) ==
				    0) {
					t_u16 len = 0;
					cmdcode_found = 1;
					hostcmd->command = a2hex_or_atoi(
						pos + strlen(cmdcode));
					hostcmd->size = S_DS_GEN;
					mlan_get_hostcmd_data(
						fp, &ln,
						buf + sizeof(t_u32) +
							hostcmd->size,
						&len);
					hostcmd->size += len;
					break;
				}
			}
			if (!cmdcode_found) {
				fprintf(stderr,
					"mlanutl: CmdCode not found in conf file\n");
				return MLAN_STATUS_FAILURE;
			}
			break;
		}
	}

	if (!cmdname_found) {
		fprintf(stderr,
			"mlanutl: cmdname '%s' is not found in conf file\n",
			cmd_name);
		return MLAN_STATUS_FAILURE;
	}

	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	hostcmd->command = cpu_to_le16(hostcmd->command);
	hostcmd->size = cpu_to_le16(hostcmd->size);

	hostcmd_size = (t_u32)(hostcmd->size);
	memcpy(buf, (t_u8 *)&hostcmd_size, sizeof(t_u32));

	return MLAN_STATUS_SUCCESS;
}

#define SUBID_OFFSET 2
static t_u16 supported_subcmd[] = {0x104, 0x111, 0x11b, 0x11e, 0x27};

static int check_if_hostcmd_subcmd_allowed(t_u8 *buf)
{
	t_u32 maxcnt = sizeof(supported_subcmd) / sizeof(supported_subcmd[0]);

	for (int i = 0; i < maxcnt; i++) {
		if (!memcmp(buf + SUBID_OFFSET, (supported_subcmd + i),
			    sizeof(t_u16)))
			return MLAN_STATUS_SUCCESS;
	}
	return MLAN_STATUS_NOTFOUND;
}

/**
 *  @brief Process hostcmd command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_hostcmd(int argc, char *argv[])
{
	t_u8 *buffer = NULL, *raw_buf = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	FILE *fp_raw = NULL;
	FILE *fp_dtsi = NULL;
	char cmdname[256];
	boolean call_ioctl = TRUE;
	t_u32 buf_len = 0, i, j, k;
	char *line = NULL, *pos = NULL;
	int li = 0, blk_count = 0, ob = 0;
	int ret = MLAN_STATUS_SUCCESS;

	struct cmd_node {
		char cmd_string[256];
		struct cmd_node *next;
	};
	struct cmd_node *command = NULL, *header = NULL, *new_node = NULL;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> <cmdname>\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	snprintf(cmdname, sizeof(cmdname), "%s", argv[4]);

	if (!strcmp(cmdname, "generate_raw")) {
		call_ioctl = FALSE;
	}
	if (!call_ioctl && argc != 6) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> %s <raw_data_file>\n",
		       cmdname);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		fclose(fp);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	if (call_ioctl) {
		/* Prepare the hostcmd buffer */
		prepare_buffer(buffer, argv[2], 0, NULL);
		if (MLAN_STATUS_FAILURE ==
		    prepare_host_cmd_buffer(fp, cmdname,
					    buffer + strlen(CMD_NXP) +
						    strlen(argv[2]))) {
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		fclose(fp);
	} else {
		line = (char *)malloc(MAX_CONFIG_LINE);
		if (!line) {
			printf("ERR:Cannot allocate memory for line\n");
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		memset(line, 0, MAX_CONFIG_LINE);

		while (config_get_line(line, MAX_CONFIG_LINE, fp, &li, &pos)) {
			line = trim_spaces(line);
			if (line[strlen(line) - 1] == '{') {
				if (ob == 0) {
					new_node = (struct cmd_node *)malloc(
						sizeof(struct cmd_node));
					if (!new_node) {
						printf("ERR:Cannot allocate memory for cmd_node\n");
						fclose(fp);
						ret = MLAN_STATUS_FAILURE;
						goto done;
					}
					memset(new_node, 0,
					       sizeof(struct cmd_node));
					new_node->next = NULL;
					if (blk_count == 0) {
						header = new_node;
						command = new_node;
					} else {
						command->next = new_node;
						command = new_node;
					}
					strncpy(command->cmd_string, line,
						(strchr(line, '=') - line));
					memmove(command->cmd_string,
						trim_spaces(
							command->cmd_string),
						strlen(trim_spaces(
							command->cmd_string)) +
							1);
				}
				ob++;
				continue; /* goto while() */
			}
			if (line[strlen(line) - 1] == '}') {
				ob--;
				if (ob == 0)
					blk_count++;
				continue; /* goto while() */
			}
		}

		rewind(fp); /* Set the source file pointer to the beginning
			       again */
		command = header; /* Set 'command' at the beginning of the
				     command list */

		fp_raw = fopen(argv[5], "w");
		if (fp_raw == NULL) {
			fprintf(stderr,
				"Cannot open the destination raw_data file %s\n",
				argv[5]);
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* prepare .dtsi output */
		snprintf(cmdname, sizeof(cmdname), "%s.dtsi", argv[5]);
		fp_dtsi = fopen(cmdname, "w");
		if (fp_dtsi == NULL) {
			fprintf(stderr, "Cannot open the destination file %s\n",
				cmdname);
			fclose(fp);
			fclose(fp_raw);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		for (k = 0; k < (t_u32)blk_count && command != NULL; k++) {
			if (MLAN_STATUS_FAILURE ==
			    prepare_host_cmd_buffer(fp, command->cmd_string,
						    buffer))
				memset(buffer, 0, BUFFER_LENGTH);

			memcpy(&buf_len, buffer, sizeof(t_u32));
			if (buf_len) {
				raw_buf = buffer + sizeof(t_u32); /* raw_buf
								     points to
								     start of
								     actual <raw
								     data> */
				printf("buf_len = %d\n", (int)buf_len);
				if (k > 0)
					fprintf(fp_raw, "\n\n");
				fprintf(fp_raw, "%s={\n", command->cmd_string);
				fprintf(fp_dtsi,
					"/ {\n\tmarvell_cfgdata {\n\t\tmarvell,%s = /bits/ 8 <\n",
					command->cmd_string);
				i = j = 0;
				while (i < buf_len) {
					for (j = 0; j < 16; j++) {
						fprintf(fp_raw, "%02x ",
							*(raw_buf + i));
						if (i >= 8) {
							fprintf(fp_dtsi,
								"0x%02x",
								*(raw_buf + i));
							if ((j < 16 - 1) &&
							    (i < buf_len - 1))
								fprintf(fp_dtsi,
									" ");
						}
						if (++i >= buf_len)
							break;
					}
					fputc('\n', fp_raw);
					fputc('\n', fp_dtsi);
				}
				fprintf(fp_raw, "}");
				fprintf(fp_dtsi, "\t\t>;\n\t};\n};\n");
			}
			command = command->next;
			rewind(fp);
		}

		fclose(fp_dtsi);
		fclose(fp_raw);
		fclose(fp);
	}

	if (call_ioctl) {
		raw_buf = buffer + strlen(CMD_NXP) + strlen(argv[2]) +
			  sizeof(t_u32) + S_DS_GEN; /* raw_buf points to start
						       of actual <raw data> */
		if (check_if_hostcmd_subcmd_allowed(raw_buf) !=
		    MLAN_STATUS_SUCCESS) {
			printf("ERR:Entered hostcmd not allowed!\n");
			goto done;
		}
		cmd = (struct eth_priv_cmd *)malloc(
			sizeof(struct eth_priv_cmd));
		if (!cmd) {
			printf("ERR:Cannot allocate buffer for command!\n");
			ret = MLAN_STATUS_FAILURE;
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
		cmd->total_len = BUFFER_LENGTH;

		/* Perform IOCTL */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			perror("mlanutl");
			fprintf(stderr, "mlanutl: hostcmd fail\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Process result */
		process_host_cmd_resp(argv[2], buffer);
	}
done:
	while (header) {
		command = header;
		header = header->next;
		free(command);
	}
	if (line)
		free(line);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

#ifdef DEBUG_LEVEL1
/**
 *  @brief Process driver debug configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_drvdbg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 drvdbg;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: drvdbg config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&drvdbg, buffer, sizeof(drvdbg));
		printf("drvdbg: 0x%08x\n", drvdbg);
#ifdef DEBUG_LEVEL2
		printf("MINFO  (%08x) %s\n", MINFO,
		       (drvdbg & MINFO) ? "X" : "");
		printf("MWARN  (%08x) %s\n", MWARN,
		       (drvdbg & MWARN) ? "X" : "");
		printf("MENTRY (%08x) %s\n", MENTRY,
		       (drvdbg & MENTRY) ? "X" : "");
#endif
		printf("MMPA_D (%08x) %s\n", MMPA_D,
		       (drvdbg & MMPA_D) ? "X" : "");
		printf("MIF_D  (%08x) %s\n", MIF_D,
		       (drvdbg & MIF_D) ? "X" : "");
		printf("MFW_D  (%08x) %s\n", MFW_D,
		       (drvdbg & MFW_D) ? "X" : "");
		printf("MEVT_D (%08x) %s\n", MEVT_D,
		       (drvdbg & MEVT_D) ? "X" : "");
		printf("MCMD_D (%08x) %s\n", MCMD_D,
		       (drvdbg & MCMD_D) ? "X" : "");
		printf("MDAT_D (%08x) %s\n", MDAT_D,
		       (drvdbg & MDAT_D) ? "X" : "");
		printf("MREG_D (%08x) %s\n", MREG_D,
		       (drvdbg & MREG_D) ? "X" : "");
		printf("MIOCTL (%08x) %s\n", MIOCTL,
		       (drvdbg & MIOCTL) ? "X" : "");
		printf("MINTR  (%08x) %s\n", MINTR,
		       (drvdbg & MINTR) ? "X" : "");
		printf("MEVENT (%08x) %s\n", MEVENT,
		       (drvdbg & MEVENT) ? "X" : "");
		printf("MCMND  (%08x) %s\n", MCMND,
		       (drvdbg & MCMND) ? "X" : "");
		printf("MDATA  (%08x) %s\n", MDATA,
		       (drvdbg & MDATA) ? "X" : "");
		printf("MERROR (%08x) %s\n", MERROR,
		       (drvdbg & MERROR) ? "X" : "");
		printf("MFATAL (%08x) %s\n", MFATAL,
		       (drvdbg & MFATAL) ? "X" : "");
		printf("MMSG   (%08x) %s\n", MMSG, (drvdbg & MMSG) ? "X" : "");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

static char *rate_format[4] = {"LG", "HT", "VHT", "HE"};
static char *lg_rate[] = {"1 Mbps",  "2 Mbps",	"5.5 Mbps", "11 Mbps",
			  "6 Mbps",  "9 Mbps",	"12 Mbps",  "18 Mbps",
			  "24 Mbps", "36 Mbps", "48 Mbps",  "54 Mbps"};

/**
 *  @brief Process Get data rate
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_datarate(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_data_rate *datarate = NULL;
	struct ifreq ifr;
	char *bw[] = {"20 MHz", "40 MHz", "80 MHz", "160 MHz"};

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getdatarate fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	datarate = (struct eth_priv_data_rate *)buffer;
	printf("Data Rate:\n");
	printf("  TX: \n");
	if (datarate->tx_rate_format <= 3) {
		printf("    Type: %s\n", rate_format[datarate->tx_rate_format]);
		if ((datarate->tx_rate_format == 0) &&
		    datarate->tx_data_rate <= 11)
			/* LG */
			printf("    Rate: %s\n",
			       lg_rate[datarate->tx_data_rate]);
		else {
			/* HT and VHT*/
			if (datarate->tx_bw <= 3)
				printf("    BW:   %s\n", bw[datarate->tx_bw]);
			if (datarate->tx_rate_format < 3) {
				if (datarate->tx_gi == 0)
					printf("    GI:   Long\n");
				else
					printf("    GI:   Short\n");
			} else if (datarate->tx_rate_format == 3) {
				switch (datarate->tx_gi) {
				case 0:
					printf("    GI:   1xHELTF + GI 0.8us  \n");
					break;
				case 1:
					printf("    GI:   2xHELTF + GI 0.8us  \n");
					break;
				case 2:
					printf("    GI:   2xHELTF + GI 1.6us  \n");
					break;
				case 3:
					printf("    GI:   4xHELTF + GI 0.8us DCM=0 and STBC=0 or\n"
					       "          4xHELTF + GI 3.2us Otherwise  \n");
					break;
				}
			}
			if (datarate->tx_rate_format >= 2)
				printf("    NSS:  %d\n", datarate->tx_nss + 1);
			if (datarate->tx_mcs_index != 0xFF)
				printf("    MCS:  MCS %d\n",
				       (int)datarate->tx_mcs_index);
			else
				printf("    MCS:  Auto\n");
			if (datarate->tx_rate_format < 3)
				printf("    Rate: %f Mbps\n",
				       (float)datarate->tx_data_rate / 2);
		}
	}

	printf("  RX: \n");
	if (datarate->rx_rate_format <= 3) {
		printf("    Type: %s\n", rate_format[datarate->rx_rate_format]);
		if ((datarate->rx_rate_format == 0) &&
		    datarate->rx_data_rate <= 11)
			/* LG */
			printf("    Rate: %s\n",
			       lg_rate[datarate->rx_data_rate]);
		else {
			/* HT and VHT*/
			if (datarate->rx_bw <= 3)
				printf("    BW:   %s\n", bw[datarate->rx_bw]);
			if (datarate->rx_rate_format < 3) {
				if (datarate->rx_gi == 0)
					printf("    GI:   Long\n");
				else
					printf("    GI:   Short\n");
			} else if (datarate->rx_rate_format == 3) {
				switch (datarate->rx_gi) {
				case 0:
					printf("    GI:   1xHELTF + GI 0.8us  \n");
					break;
				case 1:
					printf("    GI:   2xHELTF + GI 0.8us  \n");
					break;
				case 2:
					printf("    GI:   2xHELTF + GI 1.6us  \n");
					break;
				case 3:
					printf("    GI:   4xHELTF + GI 0.8us DCM=0 and STBC=0 or\n"
					       "          4xHELTF + GI 3.2us Otherwise  \n");
					break;
				}
			}
			if (datarate->rx_rate_format >= 2)
				printf("    NSS:  %d\n", datarate->rx_nss + 1);
			if (datarate->rx_mcs_index != 0xFF)
				printf("    MCS:  MCS %d\n",
				       (int)datarate->rx_mcs_index);
			else
				printf("    MCS:  Auto\n");
			if (datarate->rx_rate_format < 3)
				printf("    Rate: %f Mbps\n",
				       (float)datarate->rx_data_rate / 2);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process get wireless stats
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_getlog(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_get_log *stats = NULL;
	struct ifreq ifr;
	struct timeval tv;
	int i = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getlog fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	gettimeofday(&tv, NULL);

	/* Process results */
	stats = (struct eth_priv_get_log *)buffer;
	printf("Get log: timestamp %d.%06d sec\n", (int)tv.tv_sec,
	       (int)tv.tv_usec);
	printf("dot11GroupTransmittedFrameCount    %u\n"
	       "dot11FailedCount                   %u\n"
	       "dot11RetryCount                    %u\n"
	       "dot11MultipleRetryCount            %u\n"
	       "dot11FrameDuplicateCount           %u\n"
	       "dot11RTSSuccessCount               %u\n"
	       "dot11RTSFailureCount               %u\n"
	       "dot11ACKFailureCount               %u\n"
	       "dot11ReceivedFragmentCount         %u\n"
	       "dot11GroupReceivedFrameCount       %u\n"
	       "dot11FCSErrorCount                 %u\n"
	       "dot11TransmittedFrameCount         %u\n"
	       "wepicverrcnt-1                     %u\n"
	       "wepicverrcnt-2                     %u\n"
	       "wepicverrcnt-3                     %u\n"
	       "wepicverrcnt-4                     %u\n"
	       "beaconReceivedCount                %u\n"
	       "beaconMissedCount                  %u\n",
	       stats->mcast_tx_frame, stats->failed, stats->retry,
	       stats->multi_retry, stats->frame_dup, stats->rts_success,
	       stats->rts_failure, stats->ack_failure, stats->rx_frag,
	       stats->mcast_rx_frame, stats->fcs_error, stats->tx_frame,
	       stats->wep_icv_error[0], stats->wep_icv_error[1],
	       stats->wep_icv_error[2], stats->wep_icv_error[3],
	       stats->bcn_rcv_cnt, stats->bcn_miss_cnt);

	if (argc == 4 && !(strcmp(argv[3], "ext"))) {
		printf("rxStuckIssueCount-1                %u\n"
		       "rxStuckIssueCount-2                %u\n"
		       "rxStuckRecoveryCount               %u\n"
		       "rxStuckTsf-1                       %llu\n"
		       "rxStuckTsf-2                       %llu\n"
		       "txWatchdogRecoveryCount            %u\n"
		       "txWatchdogTsf-1                    %llu\n"
		       "txWatchdogTsf-2                    %llu\n"
		       "channelSwitchAnnouncementSent      %u\n"
		       "channelSwitchState                 %u\n"
		       "registerClass                      %u\n"
		       "channelNumber                      %u\n"
		       "channelSwitchMode                  %u\n"
		       "RxResetRecoveryCount               %u\n"
		       "RxIsr2NotDoneCnt                   %u\n"
		       "gdmaAbortCnt                       %u\n"
		       "gResetRxMacCnt                     %u\n"
		       "gOwnrshpCtlErrCnt                  %u\n"
		       "gOwnrshpBcnErrCnt                  %u\n"
		       "gOwnrshpMgtErrCnt                  %u\n"
		       "gOwnrshpDatErrCnt                  %u\n"
		       "bigtk_mmeGoodCnt                   %u\n"
		       "bigtk_replayErrCnt                 %u\n"
		       "bigtk_micErrCnt                    %u\n"
		       "bigtk_mmeNotFoundCnt               %u\n",
		       stats->rx_stuck_issue_cnt[0],
		       stats->rx_stuck_issue_cnt[1],
		       stats->rx_stuck_recovery_cnt, stats->rx_stuck_tsf[0],
		       stats->rx_stuck_tsf[1], stats->tx_watchdog_recovery_cnt,
		       stats->tx_watchdog_tsf[0], stats->tx_watchdog_tsf[1],
		       stats->channel_switch_ann_sent,
		       stats->channel_switch_state, stats->reg_class,
		       stats->channel_number, stats->channel_switch_mode,
		       stats->rx_reset_mac_recovery_cnt,
		       stats->rx_Isr2_NotDone_Cnt, stats->gdma_abort_cnt,
		       stats->g_reset_rx_mac_cnt, stats->dwCtlErrCnt,
		       stats->dwBcnErrCnt, stats->dwMgtErrCnt,
		       stats->dwDatErrCnt, stats->bigtk_mmeGoodCnt,
		       stats->bigtk_replayErrCnt, stats->bigtk_micErrCnt,
		       stats->bigtk_mmeNotFoundCnt);
	}

	if (cmd->used_len == sizeof(struct eth_priv_get_log)) {
		printf("dot11TransmittedFragmentCount      %u\n",
		       stats->tx_frag_cnt);
		printf("dot11QosTransmittedFragmentCount   ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_tx_frag_cnt[i]);
		}
		printf("\ndot11QosFailedCount                ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_failed_cnt[i]);
		}
		printf("\ndot11QosRetryCount                 ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_retry_cnt[i]);
		}
		printf("\ndot11QosMultipleRetryCount         ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_multi_retry_cnt[i]);
		}
		printf("\ndot11QosFrameDuplicateCount        ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_frm_dup_cnt[i]);
		}
		printf("\ndot11QosRTSSuccessCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rts_suc_cnt[i]);
		}
		printf("\ndot11QosRTSFailureCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rts_failure_cnt[i]);
		}
		printf("\ndot11QosACKFailureCount            ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_ack_failure_cnt[i]);
		}
		printf("\ndot11QosReceivedFragmentCount      ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_rx_frag_cnt[i]);
		}
		printf("\ndot11QosTransmittedFrameCount      ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_tx_frm_cnt[i]);
		}
		printf("\ndot11QosDiscardedFrameCount        ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_discarded_frm_cnt[i]);
		}
		printf("\ndot11QosMPDUsReceivedCount         ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_mpdus_rx_cnt[i]);
		}
		printf("\ndot11QosRetriesReceivedCount       ");
		for (i = 0; i < 8; i++) {
			printf("%u ", stats->qos_retries_rx_cnt[i]);
		}
		printf("\ndot11RSNAStatsCMACICVErrors          %u\n"
		       "dot11RSNAStatsCMACReplays            %u\n"
		       "dot11RSNAStatsRobustMgmtCCMPReplays  %u\n"
		       "dot11RSNAStatsTKIPICVErrors          %u\n"
		       "dot11RSNAStatsTKIPReplays            %u\n"
		       "dot11RSNAStatsCCMPDecryptErrors      %u\n"
		       "dot11RSNAstatsCCMPReplays            %u\n"
		       "dot11TransmittedAMSDUCount           %u\n"
		       "dot11FailedAMSDUCount                %u\n"
		       "dot11RetryAMSDUCount                 %u\n"
		       "dot11MultipleRetryAMSDUCount         %u\n"
		       "dot11TransmittedOctetsInAMSDUCount   %llu\n"
		       "dot11AMSDUAckFailureCount            %u\n"
		       "dot11ReceivedAMSDUCount              %u\n"
		       "dot11ReceivedOctetsInAMSDUCount      %llu\n"
		       "dot11TransmittedAMPDUCount           %u\n"
		       "dot11TransmittedMPDUsInAMPDUCount    %u\n"
		       "dot11TransmittedOctetsInAMPDUCount   %llu\n"
		       "dot11AMPDUReceivedCount              %u\n"
		       "dot11MPDUInReceivedAMPDUCount        %u\n"
		       "dot11ReceivedOctetsInAMPDUCount      %llu\n"
		       "dot11AMPDUDelimiterCRCErrorCount     %u\n",
		       stats->cmacicv_errors, stats->cmac_replays,
		       stats->mgmt_ccmp_replays, stats->tkipicv_errors,
		       stats->tkip_replays, stats->ccmp_decrypt_errors,
		       stats->ccmp_replays, stats->tx_amsdu_cnt,
		       stats->failed_amsdu_cnt, stats->retry_amsdu_cnt,
		       stats->multi_retry_amsdu_cnt,
		       stats->tx_octets_in_amsdu_cnt,
		       stats->amsdu_ack_failure_cnt, stats->rx_amsdu_cnt,
		       stats->rx_octets_in_amsdu_cnt, stats->tx_ampdu_cnt,
		       stats->tx_mpdus_in_ampdu_cnt,
		       stats->tx_octets_in_ampdu_cnt, stats->ampdu_rx_cnt,
		       stats->mpdu_in_rx_ampdu_cnt,
		       stats->rx_octets_in_ampdu_cnt,
		       stats->ampdu_delimiter_crc_error_cnt);
	}
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef STA_SUPPORT
/**
 *  @brief Get signal
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_get_signal(int argc, char *argv[])
{
#define DATA_SIZE 12
	int ret = 0, data[DATA_SIZE], i = 0, copy_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	memset(data, 0, sizeof(data));
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX getsignal [m] [n]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getsignal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	copy_size =
		(int)MIN((int)cmd->used_len, (int)(DATA_SIZE * sizeof(int)));
	if (copy_size > 0)
		memcpy(&data, buffer, copy_size);
	printf("Get signal output is\t");
	for (i = 0; i < (int)(copy_size / sizeof(int)); i++)
		printf("%d\t", data[i]);
	printf("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Set signalext cfg
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_signalext_cfg(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX signalextcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: signalext cfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Get signal
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_get_signal_ext(int argc, char *argv[])
{
#define MAX_NUM_PATH 3
#define PATH_SIZE 13
#define PATH_A 1
#define PATH_B 2
#define PATH_AB 3
	int ret = 0, data[PATH_SIZE * MAX_NUM_PATH] = {0};
	int i = 0, copy_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u8 num_path = 0;

	memset(data, 0, sizeof(data));
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3 && argc != 4) {
		printf("Error: invalid no of arguments\n");
		if (strncmp(argv[2], "getsignalextv2",
			    strlen("getsignalextv2")) == 0)
			printf("mlanutl mlanX getsignalextv2 [m]\n");
		else if (strncmp(argv[2], "getsignalext",
				 strlen("getsignalext")) == 0)
			printf("mlanutl mlanX getsignalext [m]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getsignal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	copy_size = cmd->used_len;
	if (copy_size > 0)
		memcpy(&data, (int *)buffer, copy_size);

	num_path = copy_size / sizeof(int) / PATH_SIZE;
	for (i = 0; i < num_path; i++) {
		if (data[i * PATH_SIZE] == PATH_A)
			printf("PATH A:   %d %d %d %d %d %d %d %d %d %d %d %d\n",
			       data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2],
			       data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4],
			       data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6],
			       data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8],
			       data[i * PATH_SIZE + 9],
			       data[i * PATH_SIZE + 10],
			       data[i * PATH_SIZE + 11],
			       data[i * PATH_SIZE + 12]);
		else if (data[i * PATH_SIZE] == PATH_B)
			printf("PATH B:   %d %d %d %d %d %d %d %d %d %d %d %d\n",
			       data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2],
			       data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4],
			       data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6],
			       data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8],
			       data[i * PATH_SIZE + 9],
			       data[i * PATH_SIZE + 10],
			       data[i * PATH_SIZE + 11],
			       data[i * PATH_SIZE + 12]);
		else if (data[i * PATH_SIZE] == PATH_AB)
			printf("PATH A+B: %d %d %d %d %d %d %d %d %d %d %d %d\n",
			       data[i * PATH_SIZE + 1], data[i * PATH_SIZE + 2],
			       data[i * PATH_SIZE + 3], data[i * PATH_SIZE + 4],
			       data[i * PATH_SIZE + 5], data[i * PATH_SIZE + 6],
			       data[i * PATH_SIZE + 7], data[i * PATH_SIZE + 8],
			       data[i * PATH_SIZE + 9],
			       data[i * PATH_SIZE + 10],
			       data[i * PATH_SIZE + 11],
			       data[i * PATH_SIZE + 12]);
	}
	printf("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif /* #ifdef STA_SUPPORT */

/**
 * @brief      Get txpwrlimit
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int get_txpwrlimit(FILE *fp_raw, char *argv[], t_u16 sub_band,
			  t_u8 *buffer, t_u16 len, struct eth_priv_cmd *cmd)
{
	struct ifreq ifr;
	mlan_ds_misc_chan_trpc_cfg *trcp_cfg = NULL;
	MrvlIETypes_ChanTRPCConfig_t *trpc_tlv = NULL;
	MrvlIEtypes_Data_t *pTlvHdr;
	int left_len;
	int mod_num = 0;
	int i = 0;
	int j = 0;
	t_u8 *pByte = NULL;

	memset(buffer, 0, len);
	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));
	trcp_cfg = (mlan_ds_misc_chan_trpc_cfg *)(buffer + strlen(argv[2]));
	trcp_cfg->sub_band = sub_band;
	if (cmd) {
		/* Fill up buffer */
#ifdef USERSPACE_32BIT_OVER_KERNEL_64BIT
		memset(cmd, 0, sizeof(struct eth_priv_cmd));
		memcpy(&cmd->buf, &buffer, sizeof(buffer));
#else
		cmd->buf = buffer;
#endif
		cmd->used_len = 0;
		cmd->total_len = len;
	}
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get_txpwrlimit fail\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Process result */
	printf("------------------------------------------------------------------------------------\n");
	printf("Get txpwrlimit: sub_band=0x%x len=%d\n", trcp_cfg->sub_band,
	       trcp_cfg->length);
	pByte = trcp_cfg->trpc_buf + S_DS_GEN + 4;
	left_len = trcp_cfg->length - S_DS_GEN - 4;
	while (left_len >= (int)sizeof(pTlvHdr->header)) {
		pTlvHdr = (MrvlIEtypes_Data_t *)pByte;
		pTlvHdr->header.len = le16_to_cpu(pTlvHdr->header.len);

		switch (le16_to_cpu(pTlvHdr->header.type)) {
		case TLV_TYPE_CHAN_TRPC_CONFIG:
			trpc_tlv = (MrvlIETypes_ChanTRPCConfig_t *)pTlvHdr;
			printf("StartFreq: %d\n", trpc_tlv->start_freq);
			printf("ChanWidth: %d\n", trpc_tlv->width);
			printf("ChanNum:   %d\n", trpc_tlv->chan_num);
			mod_num = (pTlvHdr->header.len - 4) /
				  sizeof(mod_group_setting);
			printf("Pwr:");
			for (i = 0; i < mod_num; i++) {
				if (i == (mod_num - 1))
					printf("%d,%d",
					       trpc_tlv->mod_group[i].mod_group,
					       trpc_tlv->mod_group[i].power);
				else
					printf("%d,%d,",
					       trpc_tlv->mod_group[i].mod_group,
					       trpc_tlv->mod_group[i].power);
			}
			printf("\n");
			break;
		default:
			break;
		}
		left_len -= (pTlvHdr->header.len + sizeof(pTlvHdr->header));
		pByte += pTlvHdr->header.len + sizeof(pTlvHdr->header);
	}
	if (fp_raw) {
		switch (sub_band) {
		case 0:
			fprintf(fp_raw, "txpwrlimit_2g_get={\n");
			break;
		case 0x10:
			fprintf(fp_raw, "txpwrlimit_5g_sub0_get={\n");
			break;
		case 0x11:
			fprintf(fp_raw, "txpwrlimit_5g_sub1_get={\n");
			break;
		case 0x12:
			fprintf(fp_raw, "txpwrlimit_5g_sub2_get={\n");
			break;
		case 0x13:
			fprintf(fp_raw, "txpwrlimit_5g_sub3_get={\n");
			break;
		default:
			break;
		}
		i = j = 0;
		while (i < trcp_cfg->length) {
			for (j = 0; j < 16; j++) {
				fprintf(fp_raw, "%02x ", trcp_cfg->trpc_buf[i]);
				if (++i >= trcp_cfg->length)
					break;
			}
			fputc('\n', fp_raw);
		}
		fprintf(fp_raw, "}\n\n");
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get txpwrlimit
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int process_get_txpwrlimit(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	t_u16 sub_band = 0;
	FILE *fp_raw = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(sizeof(mlan_ds_misc_chan_trpc_cfg) +
				strlen(argv[2]));
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, sizeof(mlan_ds_misc_chan_trpc_cfg) + strlen(argv[2]));
	/* Sanity tests */
	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX/uapX get_txpwrlimit [0/0x10/0x11/0x12/0x13/0x1f/0xff]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	sub_band = a2hex_or_atoi(argv[3]);
	if (argc == 5) {
		fp_raw = fopen(argv[4], "w");
		if (fp_raw == NULL) {
			fprintf(stderr,
				"Cannot open the destination raw_data file %s\n",
				argv[4]);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}
	switch (sub_band) {
	case 0:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		ret = get_txpwrlimit(fp_raw, argv, sub_band, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		break;
	case 0x1f:
		ret = get_txpwrlimit(fp_raw, argv, 0x10, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x11, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x12, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x13, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		break;
	case 0xff:
		ret = get_txpwrlimit(fp_raw, argv, 0, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x10, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x11, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x12, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		ret = get_txpwrlimit(fp_raw, argv, 0x13, buffer,
				     sizeof(mlan_ds_misc_chan_trpc_cfg) +
					     strlen(argv[2]),
				     cmd);
		break;
	default:
		printf("Error: invalid arguments\n");
		printf("mlanutl mlanX/uapX get_txpwrlimit [0/0x10/0x11/0x12/0x13/0x1f/0xff]\n");
		break;
	}
done:
	if (fp_raw)
		fclose(fp_raw);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process VHT configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_vhtcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_vhtcfg vhtcfg;
	struct ifreq ifr;
	t_u8 i, num = 0;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 5) {
		printf("Insufficient parameters\n");
		printf("For STA interface: mlanutl mlanX vhtcfg <band> <txrx> [bwcfg] [vhtcap]\n");
		printf("For uAP interface: mlanutl uapX vhtcfg <band> <txrx> [bwcfg] [vhtcap] [vht_tx_mcs] [vht_rx_mcs]\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: vhtcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	/* the first attribute is the number of vhtcfg entries */
	num = *buffer;
	if (argc == 5) {
		/* GET operation */
		printf("11AC VHT Configuration: \n");
		for (i = 0; i < num; i++) {
			memcpy(&vhtcfg, buffer + 1 + i * sizeof(vhtcfg),
			       sizeof(vhtcfg));
			/* Band */
			if (vhtcfg.band == 1)
				printf("Band: 2.4G\n");
			else
				printf("Band: 5G\n");
			/* BW confi9 */

			if (vhtcfg.bwcfg == 0)
				printf("    BW config: Follow BW in the 11N config\n");
			else
				printf("    BW config: Follow BW in VHT Capabilities\n");

			/* Tx/Rx */
			if (vhtcfg.txrx & 0x1)
				printf("    VHT operation for Tx: 0x%08x\n",
				       vhtcfg.vht_cap_info);
			if (vhtcfg.txrx & 0x2)
				/* VHT capabilities */
				printf("    VHT Capabilities Info: 0x%08x\n",
				       vhtcfg.vht_cap_info);
			/* MCS */
			if (vhtcfg.txrx & 0x2) {
				printf("    Tx MCS set: 0x%04x\n",
				       vhtcfg.vht_tx_mcs);
				printf("    Rx MCS set: 0x%04x\n",
				       vhtcfg.vht_rx_mcs);
			}
		}
	} else {
		/* SET operation */
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process dynamic bandwidth set/get
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_dyn_bw(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int dyn_bw = 0;

	/* Check arguments */
	if (argc < 3 || argc > 4) {
		printf("ERR:Incorrect number of arguments!\n");
		printf("Syntax: ./mlanutl mlanX dyn_bw <bw>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dyn_bw fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	dyn_bw = *(int *)buffer;
	printf("Dynamic bandwidth: 0x%02x\n", dyn_bw);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Tx configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_httxcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	t_u32 *data = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get result */
		data = (t_u32 *)buffer;
		printf("HT Tx cfg: \n");
		printf("    BG band:  0x%08x\n", data[0]);
		printf("     A band:  0x%08x\n", data[1]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT capability configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_htcapinfo(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_htcapinfo *ht_cap = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		ht_cap = (struct eth_priv_htcapinfo *)buffer;
		printf("HT cap info: \n");
		printf("    BG band:  0x%08x\n", ht_cap->ht_cap_info_bg);
		printf("     A band:  0x%08x\n", ht_cap->ht_cap_info_a);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_addbapara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_addba *addba = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbapara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		addba = (struct eth_priv_addba *)buffer;
		printf("Add BA configuration: \n");
		printf("    Time out : %d\n", addba->time_out);
		printf("    TX window: %d\n", addba->tx_win_size);
		printf("    RX window: %d\n", addba->rx_win_size);
		printf("    TX AMSDU : %d\n", addba->tx_amsdu);
		printf("    RX AMSDU : %d\n", addba->rx_amsdu);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process Aggregation priority table parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_aggrpriotbl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: aggrpriotbl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Aggregation priority table cfg: \n");
		printf("    TID      AMPDU      AMSDU \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %3d        %3d \n", i,
			       buffer[2 * i], buffer[2 * i + 1]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA reject configurations
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_addbareject(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbareject fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Add BA reject configuration: \n");
		printf("    TID      Reject \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %d\n", i, buffer[i]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#define MASK_11AX_OM_CONTROL 0xFFF

/**
 * @brief      11ax HE capability and operation configure
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */

static int process_11axcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int id_len = 0;
	FILE *fp = NULL;
	int ret = 0, cmd_header_len = 0;
	char config_id[20];
	char filename[256];

	if (argc != 3 && argc != 4) {
		printf("Err: Invalid number of arguments\n");
		printf("Usage: ./mlanutl <interface> 11axcfg [11axcfg.conf]\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd_header_len = strlen(CMD_NXP) + strlen(argv[2]);
	prepare_buffer(buffer, argv[2], 0, NULL);
	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->used_len = cmd_header_len;
	cmd->total_len = BUFFER_LENGTH;

	if (argc == 4) {
		memset(filename, 0, sizeof(filename));
		strncpy(filename, argv[3], sizeof(filename) - 1);

		fp = fopen(filename, "r");
		if (fp == NULL) {
			perror("fopen");
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			ret = -EFAULT;
			;
			goto done;
		}

		snprintf(config_id, sizeof(config_id), "Band");
		id_len = fparse_for_cmd_and_hex(fp, buffer + cmd_header_len,
						(t_u8 *)config_id);

		snprintf(config_id, sizeof(config_id), "HECap");
		fparse_for_cmd_and_hex(fp, buffer + cmd_header_len + id_len,
				       (t_u8 *)config_id);
		hexdump("Set 11axcfg", buffer + cmd_header_len,
			sizeof(mlan_ds_11ax_he_cfg), ' ');
		cmd->used_len = cmd_header_len + sizeof(mlan_ds_11ax_he_cfg);
	}
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: 11axcfg");
		ret = -EFAULT;
		goto done;
	}
	hexdump("11axcfg", buffer + cmd_header_len, sizeof(mlan_ds_11ax_he_cfg),
		' ');
done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Process 11ax command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_11axcmdcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	mlan_ds_11ax_cmd_cfg *axcmd = NULL;
	t_u32 action = 0;
	t_u32 prefix_len = 0;

	if (strcmp(argv[3], "tx_omi") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_TX_OMI);
	} else if (strcmp(argv[3], "obssnbru_toltime") == 0) {
		sprintf(argv[3], "%d", MLAN_11AXCMD_CFG_ID_OBSSNBRU_TOLTIME);
	} else {
		printf("ERR:unknown command %s!\n", argv[3]);
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->used_len = strlen((char *)buffer);
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11axcmd fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	prefix_len += strlen(CMD_NXP) + strlen(argv[2]);
	action = *(t_u32 *)(buffer + prefix_len);
	if (action == MLAN_ACT_SET) {
		if (argv[4] == 0) {
			printf("Invalid OBSSNBRU tolerance time: Valid range[1..3600]\n");
			free(buffer);
			return MLAN_STATUS_FAILURE;
		}
	}
	if (action == MLAN_ACT_GET) {
		axcmd = (mlan_ds_11ax_cmd_cfg *)(buffer + prefix_len +
						 sizeof(t_u32));
		switch (axcmd->sub_id) {
		case MLAN_11AXCMD_TXOMI_SUBID:
			printf("tx OMI: 0x%x\n", axcmd->param.txomi_cfg.omi &
							 MASK_11AX_OM_CONTROL);
			break;
		case MLAN_11AXCMD_OBSS_TOLTIME_SUBID:
			if (axcmd->param.toltime_cfg.tol_time > 3600 ||
			    !axcmd->param.toltime_cfg.tol_time)
				printf("OBSS Narrow Bandwidth RU tolerance Time: disabled\n");
			else
				printf("OBSS Narrow Bandwidth RU Tolerance Time: %d sec\n",
				       axcmd->param.toltime_cfg.tol_time);
			break;
		default:
			printf("Unknown sub_command 0x%x\n", axcmd->sub_id);
			break;
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx rate configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_txratecfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_tx_rate_cfg *txratecfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txratecfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	txratecfg = (struct eth_priv_tx_rate_cfg *)buffer;
	if (argc == 3) {
		/* GET operation */
		printf("Tx Rate Configuration: \n");
		/* format */
		if (txratecfg->rate_format == 0xFF) {
			printf("    Type:       0xFF (Auto)\n");
		} else if (txratecfg->rate_format <= 3) {
			printf("    Type:       %d (%s)\n",
			       txratecfg->rate_format,
			       rate_format[txratecfg->rate_format]);
			if (txratecfg->rate_format == 0)
				printf("    Rate Index: %d (%s)\n",
				       txratecfg->rate_index,
				       lg_rate[txratecfg->rate_index]);
			else if (txratecfg->rate_format >= 1)
				printf("    MCS Index:  %d\n",
				       (int)txratecfg->rate_index);
			if (txratecfg->rate_format == 2 ||
			    txratecfg->rate_format == 3)
				printf("    NSS:        %d\n",
				       (int)txratecfg->nss);
			if (txratecfg->rate_setting == 0xffff)
				printf("Rate setting :Preamble type/BW/GI/STBC/.. : auto \n");
			else {
				printf("Preamble type: %x\n",
				       (txratecfg->rate_setting & 0x0003));
				printf("BW: %x\n",
				       (txratecfg->rate_setting & 0x001C) >> 2);
				printf("LTF + GI size %x\n",
				       (txratecfg->rate_setting & 0x0060) >> 5);
				printf("STBC %x\n",
				       (txratecfg->rate_setting & 0x0080) >> 7);
				printf("DCM %x\n",
				       (txratecfg->rate_setting & 0x0100) >> 8);
				printf("Coding %x\n",
				       (txratecfg->rate_setting & 0x0200) >> 9);
				printf("maxPE %x\n",
				       (txratecfg->rate_setting & 0x3000) >>
					       12);
			}
		} else {
			printf("    Unknown rate format.\n");
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process host_cmd response
 *  @param cmd_name Command name
 *  @param buf      A pointer to the response buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int process_host_cmd_resp(char *cmd_name, t_u8 *buf)
{
	t_u32 hostcmd_size = 0;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;

	buf += strlen(CMD_NXP) + strlen(cmd_name);
	memcpy((t_u8 *)&hostcmd_size, buf, sizeof(t_u32));
	buf += sizeof(t_u32);

	hostcmd = (HostCmd_DS_GEN *)buf;
	hostcmd->command = le16_to_cpu(hostcmd->command);
	hostcmd->size = le16_to_cpu(hostcmd->size);
	hostcmd->seq_num = le16_to_cpu(hostcmd->seq_num);
	hostcmd->result = le16_to_cpu(hostcmd->result);

	hostcmd->command &= ~HostCmd_RET_BIT;
	if (!hostcmd->result) {
		switch (hostcmd->command) {
		case HostCmd_CMD_CFG_DATA: {
			HostCmd_DS_802_11_CFG_DATA *pstcfgData =
				(HostCmd_DS_802_11_CFG_DATA *)(buf + S_DS_GEN);
			pstcfgData->data_len =
				le16_to_cpu(pstcfgData->data_len);
			pstcfgData->action = le16_to_cpu(pstcfgData->action);

			if (pstcfgData->action == HostCmd_ACT_GEN_GET) {
				hexdump("cfgdata", pstcfgData->data,
					pstcfgData->data_len, ' ');
			}
			break;
		}
		case HostCmd_CMD_802_11_TPC_ADAPT_REQ: {
			mlan_ioctl_11h_tpc_resp *tpcIoctlResp =
				(mlan_ioctl_11h_tpc_resp *)(buf + S_DS_GEN);
			if (tpcIoctlResp->status_code == 0) {
				printf("tpcrequest:  txPower(%d), linkMargin(%d), rssi(%d)\n",
				       tpcIoctlResp->tx_power,
				       tpcIoctlResp->link_margin,
				       tpcIoctlResp->rssi);
			} else {
				printf("tpcrequest:  failure, status = %d\n",
				       tpcIoctlResp->status_code);
			}
			break;
		}
		case HostCmd_CMD_802_11_CRYPTO: {
			t_u16 alg = le16_to_cpu(
				(t_u16) * (buf + S_DS_GEN + sizeof(t_u16)));
			if (alg == CIPHER_TEST_AES_CCM ||
			    alg == CIPHER_TEST_GCMP) {
				HostCmd_DS_802_11_CRYPTO_AES_CCM *cmd_aes_ccm =
					(HostCmd_DS_802_11_CRYPTO_AES_CCM
						 *)(buf + S_DS_GEN);

				cmd_aes_ccm->encdec =
					le16_to_cpu(cmd_aes_ccm->encdec);
				cmd_aes_ccm->algorithm =
					le16_to_cpu(cmd_aes_ccm->algorithm);
				cmd_aes_ccm->key_length =
					le16_to_cpu(cmd_aes_ccm->key_length);
				cmd_aes_ccm->nonce_length =
					le16_to_cpu(cmd_aes_ccm->nonce_length);
				cmd_aes_ccm->AAD_length =
					le16_to_cpu(cmd_aes_ccm->AAD_length);
				cmd_aes_ccm->data.header.type = le16_to_cpu(
					cmd_aes_ccm->data.header.type);
				cmd_aes_ccm->data.header.len = le16_to_cpu(
					cmd_aes_ccm->data.header.len);

				printf("crypto_result: encdec=%d algorithm=%d, KeyLen=%d,"
				       " NonceLen=%d,AADLen=%d,dataLen=%d\n",
				       cmd_aes_ccm->encdec,
				       cmd_aes_ccm->algorithm,
				       cmd_aes_ccm->key_length,
				       cmd_aes_ccm->nonce_length,
				       cmd_aes_ccm->AAD_length,
				       cmd_aes_ccm->data.header.len);

				hexdump("Key", cmd_aes_ccm->key,
					cmd_aes_ccm->key_length, ' ');
				hexdump("Nonce", cmd_aes_ccm->nonce,
					cmd_aes_ccm->nonce_length, ' ');
				hexdump("AAD", cmd_aes_ccm->AAD,
					cmd_aes_ccm->AAD_length, ' ');
				hexdump("Data", cmd_aes_ccm->data.data,
					cmd_aes_ccm->data.header.len, ' ');
			} else if (alg == CIPHER_TEST_WAPI) {
				HostCmd_DS_802_11_CRYPTO_WAPI *cmd_wapi =
					(HostCmd_DS_802_11_CRYPTO_WAPI
						 *)(buf + S_DS_GEN);

				cmd_wapi->encdec =
					le16_to_cpu(cmd_wapi->encdec);
				cmd_wapi->algorithm =
					le16_to_cpu(cmd_wapi->algorithm);
				cmd_wapi->key_length =
					le16_to_cpu(cmd_wapi->key_length);
				cmd_wapi->nonce_length =
					le16_to_cpu(cmd_wapi->nonce_length);
				cmd_wapi->AAD_length =
					le16_to_cpu(cmd_wapi->AAD_length);

				printf("crypto_result: encdec=%d algorithm=%d, KeyLen=%d,"
				       " NonceLen=%d,AADLen=%d,dataLen=%d\n",
				       cmd_wapi->encdec, cmd_wapi->algorithm,
				       cmd_wapi->key_length,
				       cmd_wapi->nonce_length,
				       cmd_wapi->AAD_length,
				       cmd_wapi->data_length);

				hexdump("Key", cmd_wapi->key,
					cmd_wapi->key_length, ' ');
				hexdump("Nonce", cmd_wapi->nonce,
					cmd_wapi->nonce_length, ' ');
				hexdump("AAD", cmd_wapi->AAD,
					cmd_wapi->AAD_length, ' ');
			} else {
				HostCmd_DS_802_11_CRYPTO *cmd =
					(HostCmd_DS_802_11_CRYPTO *)(buf +
								     S_DS_GEN);
				cmd->encdec = le16_to_cpu(cmd->encdec);
				cmd->algorithm = le16_to_cpu(cmd->algorithm);
				cmd->key_IV_length =
					le16_to_cpu(cmd->key_IV_length);
				cmd->key_length = le16_to_cpu(cmd->key_length);
				cmd->data.header.type =
					le16_to_cpu(cmd->data.header.type);
				cmd->data.header.len =
					le16_to_cpu(cmd->data.header.len);

				printf("crypto_result: encdec=%d algorithm=%d,KeyIVLen=%d,"
				       " KeyLen=%d,dataLen=%d\n",
				       cmd->encdec, cmd->algorithm,
				       cmd->key_IV_length, cmd->key_length,
				       cmd->data.header.len);
				hexdump("KeyIV", cmd->keyIV, cmd->key_IV_length,
					' ');
				hexdump("Key", cmd->key, cmd->key_length, ' ');
				hexdump("Data", cmd->data.data,
					cmd->data.header.len, ' ');
			}
			break;
		}
		case HostCmd_CMD_802_11_AUTO_TX: {
			HostCmd_DS_802_11_AUTO_TX *at =
				(HostCmd_DS_802_11_AUTO_TX *)(buf + S_DS_GEN);

			if (le16_to_cpu(at->action) == HostCmd_ACT_GEN_GET) {
				if (S_DS_GEN + sizeof(at->action) ==
				    hostcmd->size) {
					printf("auto_tx not configured\n");

				} else {
					MrvlIEtypesHeader_t *header =
						&at->auto_tx.header;

					header->type =
						le16_to_cpu(header->type);
					header->len = le16_to_cpu(header->len);

					if ((S_DS_GEN + sizeof(at->action) +
						     sizeof(MrvlIEtypesHeader_t) +
						     header->len ==
					     hostcmd->size) &&
					    (header->type ==
					     TLV_TYPE_AUTO_TX)) {
						AutoTx_MacFrame_t *atmf =
							&at->auto_tx
								 .auto_tx_mac_frame;

						printf("Interval: %d second(s)\n",
						       le16_to_cpu(
							       atmf->interval));
						printf("Priority: %#x\n",
						       atmf->priority);
						printf("Frame Length: %d\n",
						       le16_to_cpu(
							       atmf->frame_len));
						printf("Dest Mac Address: "
						       "%02x:%02x:%02x:%02x:%02x:%02x\n",
						       atmf->dest_mac_addr[0],
						       atmf->dest_mac_addr[1],
						       atmf->dest_mac_addr[2],
						       atmf->dest_mac_addr[3],
						       atmf->dest_mac_addr[4],
						       atmf->dest_mac_addr[5]);
						printf("Src Mac Address: "
						       "%02x:%02x:%02x:%02x:%02x:%02x\n",
						       atmf->src_mac_addr[0],
						       atmf->src_mac_addr[1],
						       atmf->src_mac_addr[2],
						       atmf->src_mac_addr[3],
						       atmf->src_mac_addr[4],
						       atmf->src_mac_addr[5]);

						hexdump("Frame Payload",
							atmf->payload,
							le16_to_cpu(
								atmf->frame_len) -
								MLAN_MAC_ADDR_LENGTH *
									2,
							' ');
					} else {
						printf("incorrect auto_tx command response\n");
					}
				}
			}
			break;
		}
		case HostCmd_CMD_802_11_SUBSCRIBE_EVENT: {
			HostCmd_DS_802_11_SUBSCRIBE_EVENT *se =
				(HostCmd_DS_802_11_SUBSCRIBE_EVENT *)(buf +
								      S_DS_GEN);
			if (le16_to_cpu(se->action) == HostCmd_ACT_GEN_GET) {
				int len =
					S_DS_GEN +
					sizeof(HostCmd_DS_802_11_SUBSCRIBE_EVENT);
				printf("\nEvent\t\tValue\tFreq\tsubscribed\n\n");
				while (len < hostcmd->size) {
					MrvlIEtypesHeader_t *header =
						(MrvlIEtypesHeader_t *)(buf +
									len);
					switch (le16_to_cpu(header->type)) {
					case TLV_TYPE_RSSI_LOW: {
						MrvlIEtypes_RssiThreshold_t *low_rssi =
							(MrvlIEtypes_RssiThreshold_t
								 *)(buf + len);
						printf("Beacon Low RSSI\t%d\t%d\t%s\n",
						       low_rssi->RSSI_value,
						       low_rssi->RSSI_freq,
						       (le16_to_cpu(se->events) &
							0x0001) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_SNR_LOW: {
						MrvlIEtypes_SnrThreshold_t *low_snr =
							(MrvlIEtypes_SnrThreshold_t
								 *)(buf + len);
						printf("Beacon Low SNR\t%d\t%d\t%s\n",
						       low_snr->SNR_value,
						       low_snr->SNR_freq,
						       (le16_to_cpu(se->events) &
							0x0002) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_FAILCOUNT: {
						MrvlIEtypes_FailureCount_t
							*failure_count =
								(MrvlIEtypes_FailureCount_t
									 *)(buf +
									    len);
						printf("Failure Count\t%d\t%d\t%s\n",
						       failure_count->fail_value,
						       failure_count->fail_freq,
						       (le16_to_cpu(se->events) &
							0x0004) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_BCNMISS: {
						MrvlIEtypes_BeaconsMissed_t
							*bcn_missed =
								(MrvlIEtypes_BeaconsMissed_t
									 *)(buf +
									    len);
						printf("Beacon Missed\t%d\tN/A\t%s\n",
						       bcn_missed->beacon_missed,
						       (le16_to_cpu(se->events) &
							0x0008) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_RSSI_HIGH: {
						MrvlIEtypes_RssiThreshold_t
							*high_rssi =
								(MrvlIEtypes_RssiThreshold_t
									 *)(buf +
									    len);
						printf("Bcn High RSSI\t%d\t%d\t%s\n",
						       high_rssi->RSSI_value,
						       high_rssi->RSSI_freq,
						       (le16_to_cpu(se->events) &
							0x0010) ?
							       "yes" :
							       "no");
						break;
					}

					case TLV_TYPE_SNR_HIGH: {
						MrvlIEtypes_SnrThreshold_t *high_snr =
							(MrvlIEtypes_SnrThreshold_t
								 *)(buf + len);
						printf("Beacon High SNR\t%d\t%d\t%s\n",
						       high_snr->SNR_value,
						       high_snr->SNR_freq,
						       (le16_to_cpu(se->events) &
							0x0020) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_RSSI_LOW_DATA: {
						MrvlIEtypes_RssiThreshold_t *low_rssi =
							(MrvlIEtypes_RssiThreshold_t
								 *)(buf + len);
						printf("Data Low RSSI\t%d\t%d\t%s\n",
						       low_rssi->RSSI_value,
						       low_rssi->RSSI_freq,
						       (le16_to_cpu(se->events) &
							0x0040) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_SNR_LOW_DATA: {
						MrvlIEtypes_SnrThreshold_t *low_snr =
							(MrvlIEtypes_SnrThreshold_t
								 *)(buf + len);
						printf("Data Low SNR\t%d\t%d\t%s\n",
						       low_snr->SNR_value,
						       low_snr->SNR_freq,
						       (le16_to_cpu(se->events) &
							0x0080) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_RSSI_HIGH_DATA: {
						MrvlIEtypes_RssiThreshold_t
							*high_rssi =
								(MrvlIEtypes_RssiThreshold_t
									 *)(buf +
									    len);
						printf("Data High RSSI\t%d\t%d\t%s\n",
						       high_rssi->RSSI_value,
						       high_rssi->RSSI_freq,
						       (le16_to_cpu(se->events) &
							0x0100) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_SNR_HIGH_DATA: {
						MrvlIEtypes_SnrThreshold_t *high_snr =
							(MrvlIEtypes_SnrThreshold_t
								 *)(buf + len);
						printf("Data High SNR\t%d\t%d\t%s\n",
						       high_snr->SNR_value,
						       high_snr->SNR_freq,
						       (le16_to_cpu(se->events) &
							0x0200) ?
							       "yes" :
							       "no");
						break;
					}
					case TLV_TYPE_LINK_QUALITY: {
						MrvlIEtypes_LinkQuality_t *link_qual =
							(MrvlIEtypes_LinkQuality_t
								 *)(buf + len);
						printf("Link Quality Parameters:\n");
						printf("------------------------\n");
						printf("Link Quality Event Subscribed\t%s\n",
						       (le16_to_cpu(se->events) &
							0x0400) ?
							       "yes" :
							       "no");
						printf("Link SNR Threshold   = %d\n",
						       le16_to_cpu(
							       link_qual
								       ->link_SNR_thrs));
						printf("Link SNR Frequency   = %d\n",
						       le16_to_cpu(
							       link_qual
								       ->link_SNR_freq));
						printf("Min Rate Value       = %d\n",
						       le16_to_cpu(
							       link_qual
								       ->min_rate_val));
						printf("Min Rate Frequency   = %d\n",
						       le16_to_cpu(
							       link_qual
								       ->min_rate_freq));
						printf("Tx Latency Value     = %d\n",
						       le32_to_cpu(
							       link_qual
								       ->tx_latency_val));
						printf("Tx Latency Threshold = %d\n",
						       le32_to_cpu(
							       link_qual
								       ->tx_latency_thrs));

						break;
					}
					case TLV_TYPE_PRE_BEACON_LOST: {
						MrvlIEtypes_PreBeaconLost_t
							*pre_bcn_lost =
								(MrvlIEtypes_PreBeaconLost_t
									 *)(buf +
									    len);
						printf("------------------------\n");
						printf("Pre-Beacon Lost Event Subscribed\t%s\n",
						       (le16_to_cpu(se->events) &
							0x0800) ?
							       "yes" :
							       "no");
						printf("Pre-Beacon Lost: %d\n",
						       pre_bcn_lost
							       ->pre_beacon_lost);
						break;
					}
					default:
						printf("Unknown subscribed event TLV Type=%#x,"
						       " Len=%d\n",
						       le16_to_cpu(
							       header->type),
						       le16_to_cpu(
							       header->len));
						break;
					}

					len += (sizeof(MrvlIEtypesHeader_t) +
						le16_to_cpu(header->len));
				}
			}
			break;
		}
		case HostCmd_CMD_MAC_REG_ACCESS:
		case HostCmd_CMD_BBP_REG_ACCESS:
		case HostCmd_CMD_RF_REG_ACCESS:
		case HostCmd_CMD_CAU_REG_ACCESS: {
			HostCmd_DS_REG *preg =
				(HostCmd_DS_REG *)(buf + S_DS_GEN);
			preg->action = le16_to_cpu(preg->action);
			if (preg->action == HostCmd_ACT_GEN_GET) {
				preg->value = le32_to_cpu(preg->value);
				printf("value = 0x%08x\n", preg->value);
			}
			break;
		}
		case HostCmd_CMD_MEM_ACCESS: {
			HostCmd_DS_MEM *pmem =
				(HostCmd_DS_MEM *)(buf + S_DS_GEN);
			pmem->action = le16_to_cpu(pmem->action);
			if (pmem->action == HostCmd_ACT_GEN_GET) {
				pmem->value = le32_to_cpu(pmem->value);
				printf("value = 0x%08x\n", pmem->value);
			}
			break;
		}
		case HostCmd_CMD_LINK_STATS_SUMMARY: {
			HostCmd_DS_LINK_STATS_SUMMARY *linkstats =
				(HostCmd_DS_LINK_STATS_SUMMARY *)(buf +
								  S_DS_GEN);
			/* GET operation */
			printf("Link Statistics: \n");
			/* format */
			printf("Duration:   %u\n",
			       (int)le32_to_cpu(
				       linkstats->timeSinceLastQuery_ms));

			printf("Beacon count:     %u\n",
			       le16_to_cpu(linkstats->bcnCnt));
			printf("Beacon missing:   %u\n",
			       le16_to_cpu(linkstats->bcnMiss));
			printf("Beacon RSSI avg:  %d\n",
			       le16_to_cpu(linkstats->bcnRssiAvg));
			printf("Beacon SNR avg:   %d\n",
			       le16_to_cpu(linkstats->bcnSnrAvg));

			printf("Rx packets:       %u\n",
			       (int)le32_to_cpu(linkstats->rxPkts));
			printf("Rx RSSI avg:      %d\n",
			       le16_to_cpu(linkstats->rxRssiAvg));
			printf("Rx SNR avg:       %d\n",
			       le16_to_cpu(linkstats->rxSnrAvg));

			printf("Tx packets:       %u\n",
			       (int)le32_to_cpu(linkstats->txPkts));
			printf("Tx Attempts:      %u\n",
			       (int)le32_to_cpu(linkstats->txAttempts));
			printf("Tx Failures:      %u\n",
			       (int)le32_to_cpu(linkstats->txFailures));
			printf("Tx Initial Rate:  %s\n",
			       rateIdStr[linkstats->txInitRate]);

			printf("Tx AC VO:         %u [ %u ]\n",
			       le16_to_cpu(linkstats->txQueuePktCnt[WMM_AC_VO]),
			       (int)le32_to_cpu(
				       linkstats->txQueueDelay[WMM_AC_VO]) /
				       1000);
			printf("Tx AC VI:         %u [ %u ]\n",
			       le16_to_cpu(linkstats->txQueuePktCnt[WMM_AC_VI]),
			       (int)le32_to_cpu(
				       linkstats->txQueueDelay[WMM_AC_VI]) /
				       1000);
			printf("Tx AC BE:         %u [ %u ]\n",
			       le16_to_cpu(linkstats->txQueuePktCnt[WMM_AC_BE]),
			       (int)le32_to_cpu(
				       linkstats->txQueueDelay[WMM_AC_BE]) /
				       1000);
			printf("Tx AC BK:         %u [ %u ]\n",
			       le16_to_cpu(linkstats->txQueuePktCnt[WMM_AC_BK]),
			       (int)le32_to_cpu(
				       linkstats->txQueueDelay[WMM_AC_BK]) /
				       1000);
			break;
		}
		case HostCmd_CMD_WMM_PARAM_CONFIG: {
			HostCmd_DS_WMM_PARAM_CONFIG *wmm_param =
				(HostCmd_DS_WMM_PARAM_CONFIG *)(buf + S_DS_GEN);
			printf("WMM Params: \n");
			printf("\tBE: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n",
			       wmm_param->ac_params[AC_BE].aci_aifsn.aifsn,
			       wmm_param->ac_params[AC_BE].ecw.ecw_max,
			       wmm_param->ac_params[AC_BE].ecw.ecw_min,
			       le16_to_cpu(
				       wmm_param->ac_params[AC_BE].tx_op_limit));
			printf("\tBK: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n",
			       wmm_param->ac_params[AC_BK].aci_aifsn.aifsn,
			       wmm_param->ac_params[AC_BK].ecw.ecw_max,
			       wmm_param->ac_params[AC_BK].ecw.ecw_min,
			       le16_to_cpu(
				       wmm_param->ac_params[AC_BK].tx_op_limit));
			printf("\tVI: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n",
			       wmm_param->ac_params[AC_VI].aci_aifsn.aifsn,
			       wmm_param->ac_params[AC_VI].ecw.ecw_max,
			       wmm_param->ac_params[AC_VI].ecw.ecw_min,
			       le16_to_cpu(
				       wmm_param->ac_params[AC_VI].tx_op_limit));
			printf("\tVO: AIFSN=%d, CW_MAX=%d CW_MIN=%d, TXOP=%d\n",
			       wmm_param->ac_params[AC_VO].aci_aifsn.aifsn,
			       wmm_param->ac_params[AC_VO].ecw.ecw_max,
			       wmm_param->ac_params[AC_VO].ecw.ecw_min,
			       le16_to_cpu(
				       wmm_param->ac_params[AC_VO].tx_op_limit));
			break;
		}
		default:
			printf("HOSTCMD_RESP: CmdCode=%#04x, Size=%#04x,"
			       " SeqNum=%#04x, Result=%#04x\n",
			       hostcmd->command, hostcmd->size,
			       hostcmd->seq_num, hostcmd->result);
			hexdump("payload", (t_void *)(buf + S_DS_GEN),
				hostcmd->size - S_DS_GEN, ' ');
			break;
		}
	} else {
		printf("HOSTCMD failed: CmdCode=%#04x, Size=%#04x,"
		       " SeqNum=%#04x, Result=%#04x\n",
		       hostcmd->command, hostcmd->size, hostcmd->seq_num,
		       hostcmd->result);
	}
	return ret;
}

/**
 *  @brief Process hssetpara configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_hssetpara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: hssetpara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#define STACK_NBYTES 100 /**< Number of bytes in stack */
#define MAX_BYTESEQ 6 /**< Maximum byte sequence */
#define TYPE_DNUM 1 /**< decimal number */
#define TYPE_BYTESEQ 2 /**< byte sequence */
#define MAX_OPERAND 0x40 /**< Maximum operands */
#define TYPE_EQ (MAX_OPERAND + 1) /**< byte comparison:    == operator */
#define TYPE_EQ_DNUM (MAX_OPERAND + 2) /**< decimal comparison: =d operator */
#define TYPE_EQ_BIT (MAX_OPERAND + 3) /**< bit comparison:     =b operator */
#define TYPE_AND (MAX_OPERAND + 4) /**< && operator */
#define TYPE_OR (MAX_OPERAND + 5) /**< || operator */

typedef struct {
	t_u16 sp; /**< Stack pointer */
	t_u8 byte[STACK_NBYTES]; /**< Stack */
} mstack_t;

typedef struct {
	t_u8 type; /**< Type */
	t_u8 reserve[3]; /**< so 4-byte align val array */
	/* byte sequence is the largest among all the operands and operators. */
	/* byte sequence format: 1 byte of num of bytes, then variable num bytes
	 */
	t_u8 val[MAX_BYTESEQ + 1]; /**< Value */
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
static int push_n(mstack_t *s, t_u8 nbytes, t_u8 *val)
{
	if ((s->sp + nbytes) < STACK_NBYTES) {
		memcpy((void *)(s->byte + s->sp), (const void *)val,
		       (size_t)nbytes);
		s->sp += nbytes;
		/* printf("push: n %d sp %d\n", nbytes, s->sp); */
		return TRUE;
	} else /* stack full */
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
static int push(mstack_t *s, op_t *op)
{
	t_u8 nbytes;
	switch (op->type) {
	case TYPE_DNUM:
		if (push_n(s, 4, op->val))
			return push_n(s, 1, &op->type);
		return FALSE;
	case TYPE_BYTESEQ:
		nbytes = op->val[0];
		if (push_n(s, nbytes, op->val + 1) && push_n(s, 1, op->val) &&
		    push_n(s, 1, &op->type))
			return TRUE;
		return FALSE;
	default:
		return push_n(s, 1, &op->type);
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
 *  				NULL is returned when there are no more tokens to be
 * found.
 *
 */
static char *getop(char *s, int *first_time)
{
	const char delim[] = " \t\n";
	char *p;
	if (*first_time) {
		p = strtok(s, delim);
		*first_time = FALSE;
	} else {
		p = strtok(NULL, delim);
	}
	return p;
}

/**
 *  @brief Verify hex digit.
 *
 *  @param c			input ascii char
 *  @param h			a pointer to return integer value of the digit
 * char.
 *  @return			TURE -- c is hex digit, FALSE -- c is not hex
 * digit.
 */
static int ishexdigit(char c, t_u8 *h)
{
	if (c >= '0' && c <= '9') {
		*h = c - '0';
		return TRUE;
	} else if (c >= 'a' && c <= 'f') {
		*h = c - 'a' + 10;
		return TRUE;
	} else if (c >= 'A' && c <= 'F') {
		*h = c - 'A' + 10;
		return TRUE;
	}
	return FALSE;
}

/**
 *  @brief convert hex string to integer.
 *
 *  @param s			A pointer to hex string, string length up to 2
 * digits.
 *  @return			integer value.
 */
static t_u8 hex_atoi(char *s)
{
	int i;
	t_u8 digit; /* digital value */
	t_u8 t = 0; /* total value */

	for (i = 0, t = 0; ishexdigit(s[i], &digit) && i < 2; i++)
		t = 16 * t + digit;
	return t;
}

/**
 *  @brief Parse byte sequence in hex format string to a byte sequence.
 *
 *  @param opstr		A pointer to byte sequence in hex format string, with
 * ':' as delimiter between two byte.
 *  @param val			A pointer to return byte sequence string
 *  @return			NA
 */
static void parse_hex(char *opstr, t_u8 *val)
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
static int str2bin(char *str, mstack_t *stack)
{
	int first_time;
	char *opstr;
	op_t op; /* operator/operand */
	int dnum;
	int ret = MLAN_STATUS_SUCCESS;

	memset(stack, 0, sizeof(mstack_t));
	first_time = TRUE;
	while ((opstr = getop(str, &first_time)) != NULL) {
		if (isdigit((unsigned char)*opstr)) {
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

#define FILTER_BYTESEQ TYPE_EQ /**< byte sequence */
#define FILTER_DNUM TYPE_EQ_DNUM /**< decimal number */
#define FILTER_BITSEQ TYPE_EQ_BIT /**< bit sequence */
#define FILTER_TEST (FILTER_BITSEQ + 1) /**< test */

#define NAME_TYPE 1 /**< Field name 'type' */
#define NAME_PATTERN 2 /**< Field name 'pattern' */
#define NAME_OFFSET 3 /**< Field name 'offset' */
#define NAME_NUMBYTE 4 /**< Field name 'numbyte' */
#define NAME_REPEAT 5 /**< Field name 'repeat' */
#define NAME_BYTE 6 /**< Field name 'byte' */
#define NAME_MASK 7 /**< Field name 'mask' */
#define NAME_DEST 8 /**< Field name 'dest' */

static struct mef_fields {
	char *name; /**< Name */
	t_s8 nameid; /**< Name Id. */
} mef_fields[] = {{"type", NAME_TYPE},	   {"pattern", NAME_PATTERN},
		  {"offset", NAME_OFFSET}, {"numbyte", NAME_NUMBYTE},
		  {"repeat", NAME_REPEAT}, {"byte", NAME_BYTE},
		  {"mask", NAME_MASK},	   {"dest", NAME_DEST}};

/**
 *  @brief get filter data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int mlan_get_filter_data(FILE *fp, int *ln, t_u8 *buf, t_u16 *size)
{
	t_s32 errors = 0, i;
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u16 type = 0;
	t_u32 pattern = 0;
	t_u16 repeat = 0;
	t_u16 offset = 0;
	char byte_seq[50];
	char mask_seq[50];
	t_u16 numbyte = 0;
	t_s8 type_find = 0;
	t_s8 pattern_find = 0;
	t_s8 offset_find = 0;
	t_s8 numbyte_find = 0;
	t_s8 repeat_find = 0;
	t_s8 byte_find = 0;
	t_s8 mask_find = 0;
	t_s8 dest_find = 0;
	char dest_seq[50];

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
			if (strncmp(pos, mef_fields[i].name,
				    strlen(mef_fields[i].name)) == 0) {
				switch (mef_fields[i].nameid) {
				case NAME_TYPE:
					type = a2hex_or_atoi(pos1);
					if ((type != FILTER_DNUM) &&
					    (type != FILTER_BYTESEQ) &&
					    (type != FILTER_BITSEQ) &&
					    (type != FILTER_TEST)) {
						printf("Invalid filter type:%d\n",
						       type);
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
			printf("Line %d: unknown mef field '%s'.\n", *line,
			       pos);
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
			printf("Missing field for FILTER_DNUM: pattern=%d,offset=%d,numbyte=%d\n",
			       pattern_find, offset_find, numbyte_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d %d %d =d ", pattern, offset,
			 numbyte);
		break;
	case FILTER_BYTESEQ:
		if (!byte_find || !offset_find || !repeat_find) {
			printf("Missing field for FILTER_BYTESEQ: byte=%d,offset=%d,repeat=%d\n",
			       byte_find, offset_find, repeat_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d h%s %d == ", repeat, byte_seq,
			 offset);
		break;
	case FILTER_BITSEQ:
		if (!byte_find || !offset_find || !mask_find) {
			printf("Missing field for FILTER_BITSEQ: byte=%d,offset=%d,mask_find=%d\n",
			       byte_find, offset_find, mask_find);
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
			printf("Missing field for FILTER_TEST: byte=%d,offset=%d,repeat=%d,dest=%d\n",
			       byte_find, offset_find, repeat_find, dest_find);
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

#define NAME_MODE 1 /**< Field name 'mode' */
#define NAME_ACTION 2 /**< Field name 'action' */
#define NAME_FILTER_NUM 3 /**< Field name 'filter_num' */
#define NAME_RPN 4 /**< Field name 'RPN' */
static struct mef_entry_fields {
	char *name; /**< Name */
	t_s8 nameid; /**< Name id */
} mef_entry_fields[] = {
	{"mode", NAME_MODE},
	{"action", NAME_ACTION},
	{"filter_num", NAME_FILTER_NUM},
	{"RPN", NAME_RPN},
};

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
static int mlan_get_mef_entry_data(FILE *fp, int *ln, t_u8 *buf, t_u16 *size)
{
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u8 mode, action, filter_num = 0;
	char rpn[256];
	t_s8 mode_find = 0;
	t_s8 action_find = 0;
	t_s8 filter_num_find = 0;
	t_s8 rpn_find = 0;
	char rpn_str[256];
	int rpn_len = 0;
	char filter_name[50];
	t_s8 name_found = 0;
	t_u16 len = 0;
	int i;
	int first_time = TRUE;
	char *opstr = NULL;
	char filter_action[10];
	t_s32 errors = 0;
	MEF_ENTRY *pMefEntry = (MEF_ENTRY *)buf;
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
		if (!mode_find || !action_find || !filter_num_find ||
		    !rpn_find) {
			for (i = 0;
			     (unsigned int)i < NELEMENTS(mef_entry_fields);
			     i++) {
				if (strncmp(pos, mef_entry_fields[i].name,
					    strlen(mef_entry_fields[i].name)) ==
				    0) {
					switch (mef_entry_fields[i].nameid) {
					case NAME_MODE:
						mode = a2hex_or_atoi(pos1);
						if (mode & ~0x7) {
							printf("invalid mode=%d\n",
							       mode);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Mode = mode;
						mode_find = 1;
						break;
					case NAME_ACTION:
						action = a2hex_or_atoi(pos1);
						if (action & ~0xff) {
							printf("invalid action=%d\n",
							       action);
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
				printf("Line %d: unknown mef_entry field '%s'.\n",
				       *line, pos);
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
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), ln))) {
					if (strncmp(pos, filter_name,
						    strlen(filter_name)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file\n",
						filter_name);
					return MLAN_STATUS_FAILURE;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_filter_data(
					    fp, ln, (t_u8 *)(rpn_str + rpn_len),
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

#define MEFCFG_CMDCODE 0x009a

/**
 *  @brief Process mefcfg command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_mefcfg(int argc, char *argv[])
{
	char line[256], cmdname[256], *pos = NULL;
	int cmdname_found = 0, name_found = 0;
	int ln = 0;
	int ret = MLAN_STATUS_SUCCESS;
	int i;
	t_u8 *buffer = NULL;
	t_u16 len = 0;
	HostCmd_DS_MEF_CFG *mefcmd = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	FILE *fp = NULL;
	t_u32 cmd_len = 0, cmd_header_len;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlan0 mefcfg <mef.conf>\n");
		exit(1);
	}

	cmd_header_len = strlen(CMD_NXP) + strlen("HOSTCMD");
	cmd_len = sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_MEF_CFG);
	buffer = (t_u8 *)malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return -ENOMEM;
	}

	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, HOSTCMD, 0, NULL);

	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN> */
	hostcmd = (HostCmd_DS_GEN *)(buffer + cmd_header_len + sizeof(t_u32));
	hostcmd->command = cpu_to_le16(MEFCFG_CMDCODE);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN><HostCmd_DS_MEF_CFG>
	 */
	mefcmd = (HostCmd_DS_MEF_CFG *)(buffer + cmd_header_len +
					sizeof(t_u32) + S_DS_GEN);

	/* Host Command Population */
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[2]);
	cmdname_found = 0;
	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[4]);
		exit(1);
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "Criteria=");
			name_found = 0;
			while ((pos = mlan_config_get_line(
					fp, line, sizeof(line), &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) ==
				    0) {
					name_found = 1;
					mefcmd->Criteria = a2hex_or_atoi(
						pos + strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: criteria not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "NumEntries=");
			name_found = 0;
			while ((pos = mlan_config_get_line(
					fp, line, sizeof(line), &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) ==
				    0) {
					name_found = 1;
					mefcmd->NumEntries = a2hex_or_atoi(
						pos + strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: NumEntries not found in file '%s'\n",
					argv[3]);
				break;
			}
			for (i = 0; i < mefcmd->NumEntries; i++) {
				snprintf(cmdname, sizeof(cmdname),
					 "mef_entry_%d={", i);
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_mef_entry_data(
					    fp, &ln, (t_u8 *)hostcmd + cmd_len,
					    &len)) {
					ret = MLAN_STATUS_FAILURE;
					break;
				}
				cmd_len += len;
			}
			break;
		}
	}
	fclose(fp);
	/* buf = MRVL_CMD<cmd><hostcmd_size> */
	memcpy(buffer + cmd_header_len, (t_u8 *)&cmd_len, sizeof(t_u32));

	if (!cmdname_found)
		fprintf(stderr,
			"mlanutl: cmdname '%s' not found in file '%s'\n",
			argv[4], argv[3]);

	if (!cmdname_found || !name_found) {
		ret = MLAN_STATUS_FAILURE;
		goto mef_exit;
	}
	hostcmd->size = cpu_to_le16(cmd_len);
	mefcmd->Criteria = cpu_to_le32(mefcmd->Criteria);
	mefcmd->NumEntries = cpu_to_le16(mefcmd->NumEntries);
	hexdump("mefcfg", buffer + cmd_header_len, cmd_len, ' ');

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[MEF_CFG]");
		printf("ERR:Command sending failed!\n");

		if (buffer)
			free(buffer);

		if (cmd)
			free(cmd);

		return MLAN_STATUS_FAILURE;
	}

	ret = process_host_cmd_resp(HOSTCMD, buffer);

mef_exit:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Check the Hex String
 *  @param s  A pointer to the string
 *  @return   MLAN_STATUS_SUCCESS --HexString, MLAN_STATUS_FAILURE --not
 * HexString
 */
static int ishexstring(char *s)
{
	int ret = MLAN_STATUS_FAILURE;
	t_s32 tmp;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}
	while (*s) {
		tmp = toupper((unsigned char)*s);
		if (((tmp >= 'A') && (tmp <= 'F')) ||
		    ((tmp >= '0') && (tmp <= '9'))) {
			ret = MLAN_STATUS_SUCCESS;
		} else {
			ret = MLAN_STATUS_FAILURE;
			break;
		}
		s++;
	}

	return ret;
}
/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 *                  MAC_BROADCAST   - if broadcast mac
 *                  MAC_MULTICAST   - if multicast mac
 */
static int mac2raw(char *mac, t_u8 *raw)
{
	unsigned int temp_raw[ETH_ALEN];
	int num_tokens = 0;
	int i;

	if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1))) {
		return MLAN_STATUS_FAILURE;
	}
	num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x", temp_raw + 0,
			    temp_raw + 1, temp_raw + 2, temp_raw + 3,
			    temp_raw + 4, temp_raw + 5);
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
 *  @brief Convert String to Integer
 *  @param buf      A pointer to the string
 *  @return         Integer
 */
static int atoval(char *buf)
{
	if (!strncasecmp(buf, "0x", 2))
		return a2hex(buf + 2);
	else if (!ishexstring(buf))
		return a2hex(buf);
	else
		return atoi(buf);
}

/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @param args_count Max number of elements which can be filled in buffer
 * 'args'
 *  @return         Number of arguments in the line or EOF
 */
static int parse_line(char *line, char *args[], t_u16 args_count)
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
	for (i = 0; (i < length) && (arg_num < args_count); i++) {
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
		if (((line[i] == ' ') || (line[i] == '\t')) &&
		    (is_quote == 0)) {
			line[i] = '\0';
			is_start = 0;
			continue;
		}
	}
	return arg_num;
}

/**
 *  @brief Process cloud keep alive command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int process_cloud_keep_alive(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	char line[256], cmdname[256], *pos = NULL;
	int cmdname_found = 0, name_found = 0, arg_num = 0;
	int ln = 0, i = 0;
	char *args[256];
	cloud_keep_alive *keep_alive = NULL;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX cloud_keep_alive <keep_alive.conf> <start/stop/reset>\n");
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
	memset(buffer, 0, BUFFER_LENGTH);

	/* Insert command */
	strncpy((char *)buffer, argv[2], strlen(argv[2]));

	keep_alive = (cloud_keep_alive *)(buffer + strlen(argv[2]));

	cmdname_found = 0;
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[4]);

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		if (buffer)
			free(buffer);
		goto done;
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "mkeep_alive_id=");
			name_found = 0;
			while ((pos = mlan_config_get_line(
					fp, line, sizeof(line), &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) ==
				    0) {
					name_found = 1;
					keep_alive->mkeep_alive_id =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: keep alive id not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "enable=");
			name_found = 0;
			while ((pos = mlan_config_get_line(
					fp, line, sizeof(line), &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) ==
				    0) {
					name_found = 1;
					keep_alive->enable = a2hex_or_atoi(
						pos + strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: enable not found in file '%s'\n",
					argv[3]);
				break;
			}
			if (strcmp(argv[4], "reset") == 0) {
				snprintf(cmdname, sizeof(cmdname), "reset=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive
							->reset = a2hex_or_atoi(
							pos + strlen(cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: reset not found in file '%s'\n",
						argv[3]);
					break;
				}
			}
			if (strcmp(argv[4], "start") == 0) {
				snprintf(cmdname, sizeof(cmdname),
					 "sendInterval=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->sendInterval =
							a2hex_or_atoi(
								pos +
								strlen(cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: sendInterval not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "retryInterval=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->retryInterval =
							a2hex_or_atoi(
								pos +
								strlen(cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: retryInterval not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "retryCount=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->retryCount =
							a2hex_or_atoi(
								pos +
								strlen(cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: retryCount not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "destMacAddr=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						mac2raw(pos + strlen(cmdname),
							keep_alive->dst_mac);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: destination MAC address not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname),
					 "srcMacAddr=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						mac2raw(pos + strlen(cmdname),
							keep_alive->src_mac);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: source MAC address not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "pktLen=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						keep_alive->pkt_len =
							a2hex_or_atoi(
								pos +
								strlen(cmdname));
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: ip packet length not found in file '%s'\n",
						argv[3]);
					break;
				}
				snprintf(cmdname, sizeof(cmdname), "ipPkt=");
				name_found = 0;
				while ((pos = mlan_config_get_line(
						fp, line, sizeof(line), &ln))) {
					if (strncmp(pos, cmdname,
						    strlen(cmdname)) == 0) {
						name_found = 1;
						arg_num = parse_line(line, args,
								     256);
						if (arg_num <
						    keep_alive->pkt_len) {
							fprintf(stderr,
								"Invalid ipPkt or pkt_len in '%s'\n",
								argv[3]);
							break;
						}
						for (i = 0;
						     i < keep_alive->pkt_len;
						     i++)
							keep_alive->pkt[i] =
								(t_u8)atoval(
									args[i +
									     1]);
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: ipPkt data not found in file '%s'\n",
						argv[3]);
					break;
				}
			}
		}
	}
	if (!cmdname_found) {
		fprintf(stderr, "mlanutl: ipPkt data not found in file '%s'\n",
			argv[3]);
		free(buffer);
		if (fp)
			fclose(fp);
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
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
	cmd->total_len = BUFFER_LENGTH;
	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cloud keep alive fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Process result */
	keep_alive = (cloud_keep_alive *)(buffer + strlen(argv[2]));
	if (strcmp(argv[4], "start") != 0) {
		hexdump("Last cloud keep alive packet info", keep_alive->pkt,
			keep_alive->pkt_len, ' ');
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

done:
	return ret;
}

/********************************************************
			Global Functions
********************************************************/

/**
 *  @brief Entry function for mlanutl
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int main(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;

	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		fprintf(stdout, "NXP mlanutl version %s\n", MLANUTL_VER);
		exit(0);
	}
	if (argc < 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		display_usage();
		exit(1);
	}

	strncpy(dev_name, argv[1], IFNAMSIZ - 1);

	/*
	 * Create a socket
	 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "mlanutl: Cannot open socket.\n");
		exit(1);
	}

	ret = process_command(argc, argv);

	if (ret == MLAN_STATUS_NOTFOUND) {
		if (ret) {
			fprintf(stderr, "Invalid command specified!\n");
			display_usage();
			ret = 1;
		}
	}

	close(sockfd);
	return ret;
}
