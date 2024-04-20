#ifndef _BOARD_H_
#define _BOARD_H_

#include "driver/gpio.h"

#define LED_PIN         GPIO_NUM_13
#define USER_BUTTON_PIN GPIO_NUM_12

#define PIN_MASK(pin) ((uint64_t)(1 << pin))

void board_init(void);

#endif // _BOARD_H_
