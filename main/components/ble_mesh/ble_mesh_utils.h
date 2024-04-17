#ifndef _BLE_MESH_UTILS_H_
#define _BLE_MESH_UTILS_H_

#include "ble_mesh.h"

void ble_mesh_get_dev_uuid(uint8_t *dev_uuid, 
                           uint8_t *match_data,
                           uint8_t *device_identify_data);

void add_app_key(app_key_manager *manager, 
                 uint16_t app_idx, 
                 uint8_t app_key[APP_KEY_LEN]);

app_key_manager * app_key_manager_get(void);

#endif
