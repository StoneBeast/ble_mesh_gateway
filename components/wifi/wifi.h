#ifndef _WIFI_H_
#define _WIFI_H_

#include "esp_bit_defs.h"

typedef enum
{
    DEFAULT_TYPE,
    LAST_USE_TYPE
} wifi_config_type;

void wifi_init(void);

#endif // _WIFI_H_
