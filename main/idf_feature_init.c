#include "idf_feature_init.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"

esp_err_t idf_feature_init_nvs(void)
{
    esp_err_t err = ESP_OK;
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}
esp_err_t idf_feature_init_net(void)
{
    esp_err_t err = ESP_OK;
    err = esp_netif_init();

    return err;
}

esp_err_t idf_feature_init_os(void)
{
    esp_err_t err = esp_event_loop_create_default();

    return err;
}
