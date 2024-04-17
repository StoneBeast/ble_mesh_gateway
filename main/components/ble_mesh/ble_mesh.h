#ifndef _BLE_MESH_H_
#define _BLE_MESH_H_

#include "esp_err.h"

typedef struct app_key_node
{
    uint16_t app_idx;
    uint8_t app_key[16];
    struct app_key_node *next;
} app_key_t;

typedef struct 
{
    app_key_t *app_key_list;
    
}app_key_manager;

esp_err_t ble_mesh_init(void);

#endif
