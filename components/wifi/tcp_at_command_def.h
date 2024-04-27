#ifndef _TCP_AT_COMMAND_DEF_H_
#define _TCP_AT_COMMAND_DEF_H_

/*
    TCP_AT_SET_STA,<SSID>,<PASSWORD>
*/
typedef enum
{
    TCP_AT_NOT_DEFFINE = 0,
    TCP_AT_SET_STA,
    TCP_AT_ENABLE_PROV,
    TCP_AT_DISABLE_PROV,
} tcp_command_t;

#endif // _TCP_AT_COMMAND_DEF_H_
