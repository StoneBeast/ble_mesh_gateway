#include <string.h>

#include "wifi.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs.h"

#define TAG "WIFI_C"
#define WIFI_STORE_NAMESPACE_NAME "wifi"

typedef enum
{
    KEY_SSID,
    KEY_PASS
} nvs_key_t;

static EventGroupHandle_t wifi_event_group;
static esp_netif_t *sta_netif = NULL;
static bool reconnect = true;

const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
static const char *ap_ssid = "Redmi_91AE";
static const char *ap_pass = "13503408891";

nvs_handle_t wifi_store_handle;

static void store_wifi_data(const char *ssid, 
                            uint16_t ssid_len, 
                            const char *pass, 
                            uint16_t pass_len)
{
    esp_err_t err = nvs_open(WIFI_STORE_NAMESPACE_NAME, NVS_READWRITE, &wifi_store_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_blob(wifi_store_handle, "ssid", ssid, (size_t) ssid_len);
        ESP_LOGI(TAG, "save ssid %s", (err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_set_u16(wifi_store_handle, "ssid_len", ssid_len);
        ESP_LOGI(TAG, "save ssid_len %s", (err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_set_blob(wifi_store_handle, "pass", pass, (size_t) pass_len);
        ESP_LOGI(TAG, "save pass %s", (err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_set_u16(wifi_store_handle, "pass_len", pass_len);
        ESP_LOGI(TAG, "save pass_len %s", (err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_commit(wifi_store_handle);

        // Close
        nvs_close(wifi_store_handle);
    }
}

static void get_wifi_data(nvs_key_t key, char *out_val, uint16_t *val_len)
{
    esp_err_t err = nvs_open(WIFI_STORE_NAMESPACE_NAME, NVS_READONLY, &wifi_store_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        switch (key)
        {
            case KEY_SSID:
                err = nvs_get_u16(wifi_store_handle, "ssid_len", val_len);
                out_val = malloc(val_len+1);
                err = nvs_get_blob(wifi_store_handle, "ssid", out_val, val_len);
                break;
            case KEY_PASS:
            err = nvs_get_u16(wifi_store_handle, "pass_len", val_len);
                out_val = malloc(val_len+1);
                err = nvs_get_blob(wifi_store_handle, "pass", out_val, val_len);
                break;
            default :
                break;
        }

        out_val[*val_len] = '\0';
    }

}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_SCAN_DONE:
        // scan_done_handler();
        ESP_LOGI(TAG, "sta scan done");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "L2 connected");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        if (reconnect)
        {
            ESP_LOGI(TAG, "sta disconnect, reconnect...");
            esp_wifi_connect();
        }
        else
        {
            ESP_LOGI(TAG, "sta disconnect");
        }
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, DISCONNECTED_BIT);
        break;
    default:
        break;
    }
    return;
}

static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case IP_EVENT_STA_GOT_IP:
        xEventGroupClearBits(wifi_event_group, DISCONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        ESP_LOGI(TAG, "got ip");
        break;
    default:
        break;
    }
    return;
}

static bool wifi_connect_to_ap(const char *ssid, const char *pass)
{
    int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 0);

    wifi_config_t wifi_config = {0};

    strlcpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    if (pass)
    {
        strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
    }

    if (bits & CONNECTED_BIT)
    {
        reconnect = false;
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(wifi_event_group, DISCONNECTED_BIT, 0, 1, portTICK_PERIOD_MS);
    }

    reconnect = true;
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 5000 / portTICK_PERIOD_MS);

    return true;
}

void wifi_init(void)
{
    esp_log_level_set("wifi", ESP_LOG_WARN);
    static bool initialized = false;

    if (initialized)
    {
        return;
    }

    wifi_event_group = xEventGroupCreate();
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM)); // must call this
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    initialized = true;

    wifi_connect_to_ap(ap_ssid, ap_pass);
}