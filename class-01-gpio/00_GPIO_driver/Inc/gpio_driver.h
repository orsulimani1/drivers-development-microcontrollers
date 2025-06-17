/*
 * GPIO_driver.h
 *
 *  Created on: Jun 17, 2025
 *      Author: admin
 */

#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_
#include "stm32f411_base.h"

typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_ALTERNATE = 2,
    GPIO_MODE_ANALOG = 3
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

typedef enum {
    GPIO_SPEED_LOW = 0,     // 2 MHz
    GPIO_SPEED_MEDIUM = 1,  // 25 MHz
    GPIO_SPEED_HIGH = 2,    // 50 MHz
    GPIO_SPEED_VERY_HIGH = 3 // 100 MHz
} gpio_speed_t;

typedef enum {
    GPIO_OTYPE_PUSH_PULL = 0,
    GPIO_OTYPE_OPEN_DRAIN = 1
} gpio_otype_t;

typedef struct {
    GPIO_TypeDef    *port;      // GPIO port (GPIOA, GPIOB, etc.)
    uint8_t         pin;        // Pin number (0-15)
    gpio_mode_t     mode;       // Input/Output/Alternate/Analog
    gpio_pull_t     pull;       // Pull-up/Pull-down/None
    gpio_speed_t    speed;      // Output speed (for output modes)
    gpio_otype_t    output_type; // Push-pull/Open-drain
    uint8_t         alternate;  // Alternate function number (0-15)
} gpio_config_t;


// Driver status enumeration
typedef enum {
    GPIO_OK = 0,
    GPIO_ERROR = 1,
    GPIO_ERROR_INVALID_PARAM = 2
} gpio_status_t;


//External API
// Initialize GPIO pin with configuration
gpio_status_t gpio_init(const gpio_config_t *config);

// Read GPIO pin state
uint8_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin);

// Write GPIO pin state
void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t state);

// Toggle GPIO pin state
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);

// Read entire GPIO port
uint16_t gpio_read_port(GPIO_TypeDef *port);

// Write entire GPIO port
void gpio_write_port(GPIO_TypeDef *port, uint16_t value);


// HW Add more functionality
// 1. gpio_status_t gpio_modify(const gpio_config_t *config);
// 2. gpio_status_t gpio_deinit(const gpio_config_t *config);

#endif /* GPIO_DRIVER_H_ */
