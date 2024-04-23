#ifndef _MESH_CONFIG_H_
#define _MESH_CONFIG_H_

#define NET_INX ESP_BLE_MESH_KEY_PRIMARY

#define MSG_SEND_TTL             3
#define MSG_SEND_REL             false
#define MSG_TIMEOUT              0
#define MSG_ROLE                 ROLE_PROVISIONER

#define DEV_UUID_LEN             16
#define APP_KEY_LEN              16
#define MATCH_DATA_LEN           2
#define DEVICE_IDENTIFY_DATA_LEN 4

#define APP_INDEX_0 0x0000
#define APP_KEY_0 "stoneBeastkey001"

#define CID_STONE 0x9942

#define PROV_OWN_ADDR 0x0001
#define COMP_DATA_PAGE_0 0x00;

#define TASK_NORMAL_PRIORITY 1

typedef enum
{
    PROVISIONER_STATUS_NORMAL = 0,
    PROVISIONER_STATUS_SCAN,
    PROVISIONER_STATUS_PROVISIONING
} provisioner_status_t;

#endif // _MESH_CONFIG_H_
