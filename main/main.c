#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "ble_mesh.h"
#include "board.h"
#include "main.h"

#define TAG "MAIN_C"

provisioner_status_t status = PROVISIONER_STATUS_NORMAL;

void status_changed_cb(void *args);
static void user_button_cb(void *args);

void app_main(void)
{
    esp_err_t err;
    TaskHandle_t *task_status_changed = NULL;

    ESP_LOGI(TAG, "Initializing...");

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
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
