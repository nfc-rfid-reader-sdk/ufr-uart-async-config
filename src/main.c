/*
 * main.c
 *
 *  Created on: Jul 23, 2016
 *      Author: dlogic
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "uFCoder.h"

#define DEFAULT_PORT 		"/dev/ttyUSB0"
#define DEFAULT_MODE		1
#define DEFAULT_BAUD_RATE	1000000

void usage(void) {

	printf("\nD-Logic uFR asynchronous mode configuration tool\n"
			"------------------------------------------------\n"
			"Parameters:\n"
			"-c              - Com port name, on witch uFR is attached. Default is \"/dev/ttyUSB0\".\n"
			"-m [on|off|get] - Turn uFR asynchronous mode on or off. Default is [on]. Use [get] to read settings stored in uFR.\n"
			"-s com_speed    - Set communication baud_rate. Default is 1Mbps.\n"
			"-e [on|off]     - Enable prefix character.\n"
			"-r [on|off]     - Enable transmit on tag remove event.\n"
			"-p 00           - Define prefix character (ascii hex representation).\n"
			"-f 00           - Define sufix character (ascii hex representation).\n"
			"\n");
}

int main(int argc, char**argv)
{
	UFR_STATUS status;
	int c, i_temp;
	int user_c = 0, user_m = 0, user_s = 0, user_e = 0, user_r = 0, user_p = 0, user_f = 0;
	uint8_t mode = DEFAULT_MODE;
	char *port_name;
	uint8_t send_enable, prefix_enable, prefix, suffix, send_removed_enable;
	uint8_t user_prefix_enable = 0, user_prefix = 0, user_suffix = 0, user_send_removed_enable = 0;
	uint32_t async_baud_rate = DEFAULT_BAUD_RATE, user_baud_rate = DEFAULT_BAUD_RATE;

	if (argc < 2) {
		usage();
		return 1;
	}
	while ((c = getopt(argc, argv, "c:m:s:e:r:p:f:")) != -1) {
		switch (c) {
		case 'c':
			user_c = 1;
			port_name = malloc(strlen(optarg) + 1);
			strcpy(port_name, optarg);
			break;
		case 'm':
			if (optarg != NULL) {
				user_m = 1;
				if (!strcmp(optarg, "on")) {
					mode = 1;
				} else if (!strcmp(optarg, "off")) {
					mode = 0;
				} else if (!strcmp(optarg, "get")) {
					mode = 2;
				} else {
					printf("Error: wrong mode option \"%s\".\n", optarg);
					usage();
					return 1;
				}
			}
			break;
		case 's':
			user_s = 1;
			i_temp = strtol(optarg, NULL, 10);
			if ((i_temp < 0) || (i_temp > 3000000)) {
				printf("Error: wrong baud rate %d.\n", i_temp);
				usage();
				return 1;
			}
			user_baud_rate = i_temp;
			break;
		case 'e':
			user_e = 1;
			if (!strcmp(optarg, "on")) {
				user_prefix_enable = 1;
			} else if (!strcmp(optarg, "off")) {
				user_prefix_enable = 0;
			} else {
				printf("Error: wrong prefix enable option \"%s\".\n", optarg);
				usage();
				return 1;
			}
			break;
		case 'r':
			user_r = 1;
			if (!strcmp(optarg, "on")) {
				user_send_removed_enable = 1;
			} else if (!strcmp(optarg, "off")) {
				user_send_removed_enable = 0;
			} else {
				printf("Error: wrong transmit on tag remove event enable option \"%s\".\n", optarg);
				usage();
				return 1;
			}
			break;
		case 'p':
			user_p = 1;
			i_temp = strtol(optarg, NULL, 16);
			if ((i_temp < 0) || (i_temp > 0xFF)) {
				printf("Error: wrong prefix character 0x%02X.\n", i_temp);
				usage();
				return 1;
			}
			user_prefix = (unsigned char)i_temp;
			break;
		case 'f':
			user_f = 1;
			i_temp = strtol(optarg, NULL, 16);
			if ((i_temp < 0) || (i_temp > 0xFF)) {
				printf("Error: wrong suffix character 0x%02X.\n", i_temp);
				usage();
				return 1;
			}
			user_suffix = (unsigned char)i_temp;
			break;
		case '?':
			usage();
			return 1;
		default:
			usage();
			return 1;
		}
	}

	if (!user_c) {
		port_name = malloc(sizeof(DEFAULT_PORT));
		strcpy(port_name, DEFAULT_PORT);
	}

	status = ReaderOpenEx(1, port_name, 1, NULL);
	if (status != UFR_OK) {
		printf("Can't open port %s.\n", port_name);
		free(port_name);
		return 2;
	}
	free(port_name);

	if ((mode == 2) || !(user_m && user_s && user_e && user_r && user_p && user_f)) {
		status = GetAsyncCardIdSendConfig(&send_enable,
				                          &prefix_enable,
										  &prefix,
										  &suffix,
										  &send_removed_enable,
										  &async_baud_rate);
		if (status != UFR_OK) {
			printf("Error while trying to read uFR asynchronous mode configuration.\n");
			return 3;
		}
	}

	if (mode == 2) {
		printf("uFR configuration is:\n"
				"---------------------\n");
		if (send_enable) {
			printf("uFR asynchronous mode is ENABLED.\n");
		} else {
			printf("uFR asynchronous mode is DISABLED.\n");
		}
		if (prefix_enable) {
			printf("Prefix character is ENABLED.\n");
		} else {
			printf("Prefix character is DISABLED.\n");
		}
		printf("Prefix character is 0x%02X.\n", prefix);
		printf("Sufix character is 0x%02X.\n", suffix);
		if (send_removed_enable) {
			printf("Transmit on tag remove event is ENABLED.\n");
		} else {
			printf("Transmit on tag remove event is DISABLED.\n");
		}
		printf("Configured baud rate is: %d.\n", async_baud_rate);

		ReaderClose();
		return 0;
	}

	if (user_s) {
		async_baud_rate = user_baud_rate;
	}
	// user_r testing must be before user_e testing (uFR firmware parameters dependency)
	if (user_r) {
		send_removed_enable = user_send_removed_enable;
		if (!user_e && send_removed_enable) {
			prefix_enable = 1;
		}
	}
	if (user_e) {
		prefix_enable = user_prefix_enable;
		if (!prefix_enable) {
			send_removed_enable = 0;
		}
	}
	if (user_p) {
		prefix = user_prefix;
	}
	if (user_f) {
		suffix = user_suffix;
	}

	status = SetAsyncCardIdSendConfig(mode,
									  prefix_enable,
									  prefix,
									  suffix,
									  send_removed_enable,
									  async_baud_rate);
	if (status != UFR_OK) {
		printf("Error while trying to update uFR asynchronous mode configuration.\n");
	} else {
		printf("uFR asynchronous mode configuration updated successfully.\n");
	}

	ReaderClose();
	return 0;
}
