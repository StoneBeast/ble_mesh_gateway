#include <string.h>

#include "ble_mesh_utils.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define TAG "BLE_MESH_UTILS_C"

/*
    device_identify_data 目前设置为4字节, 0-1字节设置为typeid, 2字节设置为frature_num, 3字节保留
*/
void ble_mesh_get_dev_uuid(uint8_t *dev_uuid,
                           uint8_t *match_data,
                           uint8_t *device_identify_data)
{
    if (dev_uuid == NULL)
    {
        ESP_LOGE(TAG, "%s, Invalid device uuid", __func__);
        return;
    }

    memcpy(dev_uuid, match_data, MATCH_DATA_LEN);
    memcpy(dev_uuid + MATCH_DATA_LEN, device_identify_data, DEVICE_IDENTIFY_DATA_LEN);
    memcpy(dev_uuid + MATCH_DATA_LEN + DEVICE_IDENTIFY_DATA_LEN, esp_bt_dev_get_address(), BD_ADDR_LEN);
}

/****************************************** APP KEY PART **************************************************/

void add_app_key(app_key_manager_handle_t manager, uint16_t app_idx, uint8_t app_key[APP_KEY_LEN])
{
    app_key_t *node = (app_key_t *)malloc(sizeof(app_key_t));

    node->app_idx = app_idx;
    memcpy(node->app_key, app_key, APP_KEY_LEN);

    ((link_list_manager *)manager)->add2list(((link_list_manager *)manager)->list, node, sizeof(app_key_t), &(node->app_idx), sizeof(node->app_idx));

    free(node);
}

app_key_manager_handle_t app_key_manager_get(void)
{
    return (app_key_manager_handle_t )link_list_manager_get();
}

app_key_t *get_app_key_node(app_key_manager_handle_t manager, uint16_t index)
{
    return (app_key_t *)((link_list_manager *)manager)->find_by_index(((link_list_manager *)manager)->list, index);
}

/****************************************** APP KEY PART **************************************************/

/***************************************** NODE INFO PART **************************************************/

void add_dev_node_info(dev_node_manager_handle_t manager,
                       const uint8_t uuid[DEV_UUID_LEN],
                       uint16_t unicast)
{
    dev_node_info_t *node = (dev_node_info_t *)malloc(sizeof(dev_node_info_t));

    memcpy(node->uuid, uuid, DEV_UUID_LEN);
    node->unicast = unicast;
    node->feature_num = uuid[4];
    node->type_id = (((uint16_t)(uuid[3])) << 8) | uuid[2];
    node->data = (dev_data_info_t*)malloc(sizeof(dev_data_info_t)*node->feature_num);

    ((link_list_manager *)manager)->add2list(((link_list_manager *)manager)->list, node, sizeof(dev_node_info_t), &(node->unicast), sizeof(unicast));

    free(node);
}

dev_node_manager_handle_t dev_node_info_manager_get(void)
{
    return (dev_node_manager_handle_t )link_list_manager_get();
}

dev_node_info_t *get_dev_info_node(dev_node_manager_handle_t manager, uint16_t unicast)
{
    return (dev_node_info_t *)((link_list_manager *)manager)->find_by_id(((link_list_manager *)manager)->list, &unicast);
}

/***************************************** NODE INFO PART **************************************************/
