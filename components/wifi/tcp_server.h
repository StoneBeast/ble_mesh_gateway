#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "wifi.h"

typedef enum
{
    TCP_SERVER_STA_TYPE,
    TCP_SERVER_AP_TYPE
}tcp_server_type;

void tcp_server_start(tcp_server_type type);

#endif // _TCP_SERVER_H_
