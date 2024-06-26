#include <string.h>

#include "wifi.h"
#include "tcp_server.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs.h"

#define TAG "WIFI_C"
#define WIFI_STORE_NAMESPACE_NAME "wifi"
#define DEFAULT_SSID         "StoneBeast_Mesh_Gateway"
#define DEFAULT_PASSWORD     "default_pass"

typedef enum
{
    KEY_SSID,
    KEY_PASS
} nvs_key_t;

typedef enum
{
    CONNECT_FAIL_TYPE_NO_INFO,
    CONNECT_FAIL_TYPE_CANNT_CONNECT
}fail_type;

static EventGroupHandle_t wifi_event_group;
static bool reconnect = true;
static uint8_t reconnect_count = 0;

typedef struct {
    char ssid[32];
    size_t ssid_len;
    char pass[64];
    size_t pass_len;
} wifi_acc_t;

static wifi_acc_t wifi_acc_info = {
    .pass = {0},
    .pass_len = 0,
    .ssid = {0},
    .ssid_len = 0,
};
static tcp_server_type current_type = TCP_SERVER_AP_TYPE;

const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;

nvs_handle_t wifi_store_handle;

void store_wifi_data(const char *ssid, 
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

static esp_err_t get_wifi_data(nvs_key_t key, char *out_val, size_t *val_len)
{
    esp_err_t err = nvs_open(WIFI_STORE_NAMESPACE_NAME, NVS_READONLY, &wifi_store_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ESP_FAIL;
    }
    else
    {
        *val_len = 0;
        switch (key)
        {
            case KEY_SSID:
                err = nvs_get_u16(wifi_store_handle, "ssid_len", (uint16_t*)val_len);
                if (err != ESP_OK)
                {
                    return err;
                }
                err = nvs_get_blob(wifi_store_handle, "ssid", out_val, val_len);
                break;
            case KEY_PASS:
                err = nvs_get_u16(wifi_store_handle, "pass_len", (uint16_t *)val_len);
                if (err != ESP_OK)
                {
                    return err;
                }
                err = nvs_get_blob(wifi_store_handle, "pass", out_val, val_len);
                break;
            default :
                return ESP_FAIL;
                break;
        }

        out_val[*val_len] = '\0';
        return ESP_OK;
    }

}

static void wifi_open_ap(void)
{

    esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = strlen(DEFAULT_SSID),
            .channel = 1,
            .max_connection = 2,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    // wifi_config_t wifi_config = {0};

    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    strlcpy((char *)wifi_config.ap.ssid, DEFAULT_SSID, sizeof(wifi_config.ap.ssid));
    strncpy((char *)wifi_config.ap.password, DEFAULT_PASSWORD, sizeof(wifi_config.ap.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}

static void connect_fail_handler(fail_type type)
{
    reconnect = false;
    reconnect_count = 0;

    printf("CONNECT_FAIL, can't connect to '%s', pw: %s, gatway will be set ap mod.\n", wifi_acc_info.ssid, wifi_acc_info.pass);
    current_type = TCP_SERVER_AP_TYPE;
    wifi_open_ap();
    tcp_server_start(current_type);
}

static void connect_success_handler(void)
{
    current_type = TCP_SERVER_STA_TYPE;
    tcp_server_start(current_type);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_SCAN_DONE:
        ESP_LOGI(TAG, "sta scan done");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "L2 connected");
        connect_success_handler();
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        if (reconnect)
        {

            ESP_LOGI(TAG, "sta disconnect, reconnect...");

            if (reconnect_count == 1)
            {
                connect_fail_handler(current_type);
            }
            else
            {
                esp_wifi_connect();
                reconnect_count++;
            }
        }
        else
        {
            connect_fail_handler(current_type);
        }
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, DISCONNECTED_BIT);
        break;
    case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG, "soft-ap start");
        break;
    case WIFI_EVENT_AP_STOP:
        ESP_LOGI(TAG, "soft-ap stop");
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "a sta connect to soft-ap");
        current_type = TCP_SERVER_AP_TYPE;
        tcp_server_start(current_type);
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "a sta disconnect to soft-ap");
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
    ESP_LOGI("CONNECT_WIFI", "try to connect to %s, password: %s", wifi_config.sta.ssid, wifi_config.sta.password);
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
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM)); // must call this
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    esp_err_t err = get_wifi_data(KEY_SSID, wifi_acc_info.ssid, &(wifi_acc_info.ssid_len));
    if (err != ESP_OK)
    {
        initialized = true;
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_start());
        wifi_open_ap();
    }
    else 
    {
        err = get_wifi_data(KEY_PASS, wifi_acc_info.pass, &(wifi_acc_info.pass_len));
        esp_netif_create_default_wifi_sta();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        initialized = true;
        wifi_connect_to_ap(wifi_acc_info.ssid, wifi_acc_info.pass);
    }

}