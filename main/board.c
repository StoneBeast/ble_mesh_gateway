#include "board.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/*
    指示灯:
        正常工作      缓慢闪烁
        正在添加节点   常亮
        接收广播      快速闪烁
        使用信号量或队列，当触发状态改变时唤醒任务
*/

static void board_led_init(void)
{
    gpio_reset_pin(LED_PIN);

    gpio_config_t *led_config_t = (gpio_config_t *)malloc(sizeof(gpio_config_t));

    led_config_t->intr_type = GPIO_INTR_DISABLE;
    led_config_t->mode = GPIO_MODE_OUTPUT;
    led_config_t->pin_bit_mask = PIN_MASK(LED_PIN);
    led_config_t->pull_down_en = GPIO_PULLDOWN_DISABLE;
    led_config_t->pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(led_config_t);

    free(led_config_t);
}

void board_init(void)
{
    board_led_init();
}

void board_led_start(provisioner_status_t *status)
{
    uint8_t led_level = 0;
    while (1)
    {
        switch (*status)
        {
        case PROVISIONER_STATUS_PROVISIONING:
            gpio_set_level(LED_PIN, 1);
            led_level = 1;
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        case PROVISIONER_STATUS_SCAN:
            gpio_set_level(LED_PIN, !led_level);
            led_level = !led_level;
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        case PROVISIONER_STATUS_NORMAL:
            gpio_set_level(LED_PIN, !led_level);
            led_level = !led_level;
            vTaskDelay(pdMS_TO_TICKS(500));
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    
}
