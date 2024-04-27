#include "command_tools.h"

#include <string.h>

/*
    command format: tcp_at+<commadn>=[arg1][,arg2][,argn]$}\r\n
*/
bool is_command(char *command, uint16_t len)
{
    if (len < TCP_AT_COMMAND_PREFIX_LEN)
    {
        return false;
    }

    if ((memcmp(command, TCP_AT_COMMAND_PREFIX, TCP_AT_COMMAND_PREFIX_LEN) == 0) && (strstr(command, "$}\r\n") != NULL))
    {
        return true;
    }

    return false;
}

tcp_command_t get_command(char *command, uint16_t len, tcp_server_type type)
{
    char *p = strchr(command, '=');
    char *temp_command = NULL;

    if (p == NULL)
    {
        return TCP_AT_NOT_DEFFINE;
    }

    uint16_t command_str_len = p-command;
    temp_command = (char*)malloc(command_str_len+1);
    temp_command[command_str_len] = '\0';

    memcpy(temp_command, command, (size_t)command_str_len);

    if (strncmp(temp_command, "set_sta", strlen(" set_sta ")) == 0)
    {
        return TCP_AT_SET_STA;
    }
    /*
    else if (strncmp(temp_command, "examp", strlen("examp")) == 0 && type == TCP_AT_SET_STA)
    {

    }
    */

        return TCP_AT_NOT_DEFFINE;
}
