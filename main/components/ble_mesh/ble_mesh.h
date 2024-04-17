#ifndef _BLE_MESH_H_
#define _BLE_MESH_H_

#include "esp_err.h"
#include "link_list.h"
#include "mesh_config.h"
#include "esp_ble_mesh_defs.h"

typedef struct app_key_node
{
    uint16_t app_idx;
    uint8_t app_key[APP_KEY_LEN];
} app_key_t;

typedef link_list_manager app_key_manager;

esp_err_t ble_mesh_init(void);

#endif
