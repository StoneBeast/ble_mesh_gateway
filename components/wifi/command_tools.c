#include "command_tools.h"

#include <string.h>

bool is_command(char *command, uint16_t len)
{
    if (len < TCP_AT_COMMAND_PREFIX_LED)
    {
        return false;
    }

    if (memcmp(command, TCP_AT_COMMAND_PREFIX, TCP_AT_COMMAND_PREFIX_LED) == 0)
    {
        return true;
    }

    return false;
}


