#ifndef _COMMAND_TOOLS_H_
#define _COMMAND_TOOLS_H_

#include <stdbool.h>
#include <stdint.h>
#include "tcp_at_command_def.h"
#include "tcp_server.h"

#define TCP_AT_COMMAND_PREFIX       "tcp_at+"
#define TCP_AT_COMMAND_PREFIX_LEN   7

bool is_command(char *command, uint16_t len);
tcp_command_t get_command(char *command, uint16_t len, tcp_server_type type);

#endif // _COMMAND_TOOLS_H_
