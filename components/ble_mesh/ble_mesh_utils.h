#ifndef _BLE_MESH_UTILS_H_
#define _BLE_MESH_UTILS_H_

#include "ble_mesh.h"

typedef void* app_key_manager_handle_t;
typedef void* dev_node_manager_handle_t;

void ble_mesh_get_dev_uuid(uint8_t *dev_uuid, 
                           uint8_t *match_data,
                           uint8_t *device_identify_data);

/****************************************** APP KEY PART **************************************************/

void add_app_key(app_key_manager_handle_t manager,
                 uint16_t app_idx,
                 uint8_t app_key[APP_KEY_LEN]);

app_key_manager_handle_t app_key_manager_get(void);

app_key_t *get_app_key_node(app_key_manager_handle_t manager, uint16_t index);

/****************************************** APP KEY PART **************************************************/

/***************************************** NODE INFO PART **************************************************/

void add_dev_node_info(dev_node_manager_handle_t manager,
                       const uint8_t uuid[DEV_UUID_LEN],
                       uint16_t unicast,
                       uint8_t elem_num,
                       uint8_t data);

dev_node_manager_handle_t dev_node_info_manager_get(void);

dev_node_info_t *get_dev_info_node(dev_node_manager_handle_t manager, uint16_t unicast);

/***************************************** NODE INFO PART **************************************************/
#endif
