#ifndef _TCP_AT_COMMAND_DEF_H_
#define _TCP_AT_COMMAND_DEF_H_

/*
    TCP_AT_SET_STA,<SSID>,<PASSWORD>
*/
typedef enum
{
    TCP_AT_NOT_DEFFINE = 0,

    /*
        tcp_at+set_sta=<ssid>,<password>$}\r\n
    */
    TCP_AT_SET_STA,

    /*
        tcp_at+set_prov=<status>$}\r\n
    */
    TCP_AT_SET_PROV,
} tcp_command_t;

#endif // _TCP_AT_COMMAND_DEF_H_
