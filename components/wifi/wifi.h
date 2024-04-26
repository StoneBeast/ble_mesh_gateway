#ifndef _WIFI_H_
#define _WIFI_H_

#include "esp_bit_defs.h"

void store_wifi_data(const char *ssid, 
                     uint16_t ssid_len, 
                     const char *pass, 
                     uint16_t pass_len);

void wifi_init(void);

#endif // _WIFI_H_
