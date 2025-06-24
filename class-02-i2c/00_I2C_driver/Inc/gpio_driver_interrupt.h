/*
 * gpio_driver_interrupt.h
 *
 *  Created on: Jun 17, 2025
 *      Author: admin
 */

#ifndef GPIO_DRIVER_INTERRUPT_H_
#define GPIO_DRIVER_INTERRUPT_H_

#include "gpio_driver.h"
typedef enum {
    EXTI_TRIGGER_RISING = 1,
    EXTI_TRIGGER_FALLING = 2,
    EXTI_TRIGGER_BOTH = 3
} exti_trigger_t;


typedef struct {
    gpio_config_t   gpio_config;    // Base GPIO configuration
    uint8_t         interrupt_enable; // Enable EXTI interrupt
    exti_trigger_t  trigger_type;   // Rising/Falling/Both
    uint8_t         priority;       // NVIC priority
    void (*callback)(uint8_t pin);  // User callback function
} gpio_interrupt_config_t;

gpio_status_t gpio_interrupt_init(const gpio_interrupt_config_t *config);

#endif /* GPIO_DRIVER_INTERRUPT_H_ */
