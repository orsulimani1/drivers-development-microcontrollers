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
gpio_status_t gpio_init(const gpio_config_t *config){
	if(config == NULL){
		return GPIO_ERROR;
	}
//	enable GPIO clock
	uint8_t port_num = ((uint32_t)&config->port - AHB1PERIPH_BASE) / 0x400;
	RCC->AHB1ENR |= (1 <<port_num);

	gpio_set_mode(config->port, config->pin, config->mode);

	gpio_set_output_type(config->port, config->pin, config->output_type);

	gpio_set_pull(config->port, config->pin, config->pull);

	// GPIO set speed
	gpio_set_alternate_function(config->port, config->pin, config->alternate);

	return GPIO_OK;
}

// Read GPIO pin state
uint8_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin){
	if(port == NULL || pin < 0 || pin > 15){
		return GPIO_ERROR_INVALID_PARAM;
	}
	return port->IDR | (0x1 << pin);
}

// Write GPIO pin state
void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t state){
	if(port == NULL || pin < 0 || pin > 15 ||state < 0 || state > 1){
		return;
	}
	if (state == 1){
		port->ODR |= (state << pin);
	} else {
		port->ODR &= ~(0x1 << pin);
	}
}

// Toggle GPIO pin state
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin){
	if(port == NULL || pin < 0 || pin > 15){
		return;
	}
	port->ODR ^= (0x1 << pin);
}

// Read entire GPIO port
uint16_t gpio_read_port(GPIO_TypeDef *port);

// Write entire GPIO port
void gpio_write_port(GPIO_TypeDef *port, uint16_t value);

static gpio_status_t gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode){
	if(port == NULL || pin < 0 || pin > 15 || mode < GPIO_MODE_INPUT || mode > GPIO_MODE_ANALOG){
		return GPIO_ERROR_INVALID_PARAM;
	}
	port->MODER &= ~(0x3 << (2*pin)); // clear
	port->MODER |= (mode << (2*pin)); // set

	return GPIO_OK;
}
static gpio_status_t gpio_set_output_type(GPIO_TypeDef *port, uint8_t pin, gpio_otype_t otype){
	if(port == NULL || pin < 0 || pin > 15 || otype < GPIO_OTYPE_PUSH_PULL || otype > GPIO_OTYPE_OPEN_DRAIN){
		return GPIO_ERROR_INVALID_PARAM;
	}
	port->OTYPER |= (otype << pin);
	return GPIO_OK;
}
static gpio_status_t gpio_set_pull(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull){
	if(port == NULL || pin < 0 || pin > 15 || pull < GPIO_PULL_NONE || pull > GPIO_PULL_DOWN){
		return GPIO_ERROR_INVALID_PARAM;
	}
	port->PUPDR &= ~(0x3 << (2*pin)); // clear
	port->PUPDR |= (pull << (2*pin)); // set

	return GPIO_OK;
}
static gpio_status_t gpio_set_alternate_function(GPIO_TypeDef *port, uint8_t pin, uint8_t af){
	if(port == NULL || pin < 0 || pin > 15 || af < 0 || af > 15){
		return GPIO_ERROR_INVALID_PARAM;
	}
	switch (pin) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			port->AFR[0] &= ~(0xF<< (pin * 4));
			port->AFR[0] |= (af << (pin * 4));
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			port->AFR[1] &= ~(0xF<< (pin * 4));
			port->AFR[1] |= (af << (pin * 4));
			break;
	}
	return GPIO_OK;

}



// HW Add more functionality
// 1. gpio_status_t gpio_modify(const gpio_config_t *config);
// 2. gpio_status_t gpio_deinit(const gpio_config_t *config);
// 3. SET SPEED


