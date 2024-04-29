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

    /*
        tcp_at+set_subaddr=<subscribe(2byte)>,<unicast(2byte)>$}\r\n
    */
    TCP_AT_SET_SUBSC,

    /*
        tcp_at+delete_subaddr=<subscribe(2byte)>,<unicast(2byte)>$}\r\n
    */
    TCP_AT_DEL_SUBSC,
} tcp_command_t;

#endif // _TCP_AT_COMMAND_DEF_H_
