#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "tcp_server.h"
#include "command_tools.h"
#include "wifi.h"
#include "ble_mesh.h"

#define PORT 5001
#define KEEPALIVE_IDLE 5
#define KEEPALIVE_INTERVAL 5
#define KEEPALIVE_COUNT 3

static const char *TAG = "TCP_SERVER_C";

static void tcp_at_command_handler(const char *command, 
                                   uint16_t command_len,
                                   tcp_server_type type)
{
    char *temp_command = (char *)malloc(command_len + 1);
    char *temp_end = NULL;
    strcpy(temp_command, command);

    switch (get_command(temp_command, command_len, type))
    {
    case TCP_AT_NOT_DEFFINE:
        ESP_LOGE("COMMAND_HANDLER", "command not define");
        break;
    case TCP_AT_SET_STA:
        /*
            tcp_at+set_sta=ssid,pass$}\r\n
        */
        char *ssid = NULL;
        char *pass = NULL;
        ssid = (strchr(temp_command, '=') + 1);
        pass = (strchr(temp_command, ',') + 1);

        ESP_LOGW("ID&SS", "ssid: %s, pass: %s", ssid, pass);

        *(pass-1) = '\0';
        temp_end = (strchr(pass, '$'));
        if (*(temp_end+1) == '}')
        {
            *temp_end = 0;
        }

        store_wifi_data(ssid, strlen(ssid), pass, strlen(pass));
        ESP_LOGW("COMMAND_HANDLER", "info saved, restart...");
        esp_restart();

        break;
    case TCP_AT_SET_PROV:
        ESP_LOGI("TCP_SERVER", "command TCP_AT_SET_PROV");
        char *prov_status = 1 + strchr(temp_command, '=');
        temp_end = (strchr(temp_command, '$'));
        if (*(temp_end + 1) == '}')
        {
            *temp_end = 0;
        }

        if (strcmp(prov_status, "1") == 0)
        {
            ESP_LOGI("TCP_COMMAND", "prov enable");
            ble_mesh_scanner_start();
        }
        else if (strcmp(prov_status, "0") == 0)
        {
            ESP_LOGI("TCP_COMMAND", "prov disable");
            ble_mesh_scanner_stop();
        }
        break;
    default:
        break;
    }

    free(temp_command);
}

    static void do_retransmit(const int sock, tcp_server_type type)
{
    int len;
    char rx_buffer[128];

    do
    {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0;
            ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
            if (is_command(rx_buffer, len) == true)
            {
                //  是tcp at 命令
                //  handler command
                char *p_command = rx_buffer+TCP_AT_COMMAND_PREFIX_LEN;
                int command_len = len - TCP_AT_COMMAND_PREFIX_LEN;

                tcp_at_command_handler(p_command, command_len, type);
            }
            else
            {
                //  不是tcp at 命令
                //  other
            }

            int to_write = len;
            while (to_write > 0)
            {
                int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
                if (written < 0)
                {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    // Failed to retransmit, giving up
                    return;
                }
                to_write -= written;
            }
        }
    } while (len > 0);
}

static void tcp_server_task(void *pvParameters)
{
    ESP_LOGI(TAG, "tcp server task start");

    tcp_server_type type = (tcp_server_type)pvParameters;

    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        do_retransmit(sock, type);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void tcp_server_start(tcp_server_type type)
{
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)type, 5, NULL);
}
