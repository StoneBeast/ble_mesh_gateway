#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"

#include "ble_mesh.h"
#include "board.h"
#include "main.h"
#include "wifi.h"
#include "tcp_server.h"
#include "idf_feature_init.h"

#define TAG "MAIN_C"

provisioner_status_t status = PROVISIONER_STATUS_NORMAL;

void status_changed_cb(void *args);

void app_main(void)
{
    esp_err_t err;
    TaskHandle_t *task_status_changed = NULL;

    ESP_LOGI(TAG, "Initializing...");

    err = idf_feature_init_nvs();
    ESP_ERROR_CHECK(err);

    err = idf_feature_init_os();
    ESP_ERROR_CHECK(err);

    err = idf_feature_init_net();
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }

    wifi_init();
    tcp_server_start();

    xTaskCreate(status_changed_cb,
                "status_change",
                4096,
                &status,
                TASK_NORMAL_PRIORITY,
                task_status_changed);
}

void status_changed_cb(void *args)
{
    board_init();
    provisioner_status_t *pro_status = (provisioner_status_t*) args;

    board_led_start(pro_status);
}
