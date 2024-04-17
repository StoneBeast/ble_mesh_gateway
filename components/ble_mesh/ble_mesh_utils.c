#include <string.h>

#include "ble_mesh_utils.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define TAG "BLE_MESH_UTILS_C"

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

void add_app_key(app_key_manager *manager, uint16_t app_idx, uint8_t app_key[APP_KEY_LEN])
{
    app_key_t *node = (app_key_t *)malloc(sizeof(app_key_t));

    node->app_idx = app_idx;
    memcpy(node->app_key, app_key, APP_KEY_LEN);

    manager->add2list(manager->list, node, sizeof(app_key_t), &(node->app_idx), sizeof(node->app_idx));

    free(node);
}

app_key_manager *app_key_manager_get(void)
{
    return (app_key_manager*) link_list_manager_get();
}

app_key_t *get_app_key_node(app_key_manager *manager, uint16_t index)
{
    return (app_key_t*)manager->find_by_index(manager->list, index);
}

/****************************************** APP KEY PART **************************************************/

/***************************************** NODE INFO PART **************************************************/

/***************************************** NODE INFO PART **************************************************/
