#ifndef _BOARD_H_
#define _BOARD_H_

#include "driver/gpio.h"
#include "mesh_config.h"

#define LED_PIN         GPIO_NUM_13

#define PIN_MASK(pin) ((uint64_t)(1 << pin))

void board_init(void);

void board_led_start(provisioner_status_t *status);

#endif // _BOARD_H_
