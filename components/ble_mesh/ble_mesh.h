#ifndef _BLE_MESH_H_
#define _BLE_MESH_H_

#include "esp_err.h"
#include "link_list.h"
#include "mesh_config.h"
// #include "esp_ble_mesh_defs.h"

typedef struct app_key_node
{
    uint16_t app_idx;
    uint8_t app_key[APP_KEY_LEN];
} app_key_t;

typedef struct dev_node_info_node
{
    uint8_t uuid[16];
    uint16_t unicast;
    uint8_t elem_num;
    uint8_t onoff;
} dev_node_info_t;

typedef link_list_manager app_key_manager;

esp_err_t ble_mesh_init(void);

esp_err_t bluetooth_init(void);

#endif
