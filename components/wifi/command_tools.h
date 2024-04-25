#ifndef _COMMAND_TOOLS_H_
#define _COMMAND_TOOLS_H_

#include <stdbool.h>
#include <stdint.h>

#define TCP_AT_COMMAND_PREFIX       "tcp_at+"
#define TCP_AT_COMMAND_PREFIX_LED   7

bool is_command(char *command, uint16_t len);

#endif // _COMMAND_TOOLS_H_
