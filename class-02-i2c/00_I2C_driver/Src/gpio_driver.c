/*
 * GPIO_driver.c
 *
 *  Created on: Jun 17, 2025
 *      Author: admin
 */
#include "gpio_driver.h"

// Internal functions
static gpio_status_t gpio_set_output_type(GPIO_TypeDef *port, uint8_t pin, gpio_otype_t otype);
static gpio_status_t gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode);
static gpio_status_t gpio_set_pull(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull);
static gpio_status_t gpio_set_alternate_function(GPIO_TypeDef *port, uint8_t pin, uint8_t af);


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


