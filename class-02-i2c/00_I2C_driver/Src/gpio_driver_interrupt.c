/*
 * gpio_driver_interrupt.c
 *
 *  Created on: Jun 17, 2025
 *      Author: admin
 */


#include "gpio_driver_interrupt.h"

gpio_status_t gpio_interrupt_init(const gpio_interrupt_config_t *config);

// SYSCFG needed for EXTI line mapping
static void gpio_enable_syscfg_clock(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

// Map PA1 to EXTI1 line via SYSCFG
static gpio_status_t gpio_map_exti_line(GPIO_TypeDef *port, uint8_t pin) {
    if (pin > 15) return GPIO_ERROR_INVALID_PARAM;

    // Determine port number (0=A, 1=B, 2=C, etc.)
    uint8_t port_num = ((uint32_t)port - GPIOA_BASE) / 0x400;

    // Calculate SYSCFG_EXTICR register index and bit position
    uint8_t reg_index = pin / 4;        // EXTICR[0-3]
    uint8_t bit_pos = (pin % 4) * 4;    // 4 bits per pin

    // Clear existing mapping and set new one
    SYSCFG->EXTICR[reg_index] &= ~(0xF << bit_pos);
    SYSCFG->EXTICR[reg_index] |= (port_num << bit_pos);

    return GPIO_OK;
}

static gpio_status_t gpio_configure_exti_trigger(uint8_t pin, exti_trigger_t trigger) {
    if (pin > 15) return GPIO_ERROR_INVALID_PARAM;

    uint32_t pin_mask = (1U << pin);

    // Clear existing trigger configuration
    EXTI->RTSR &= ~pin_mask;  // Clear rising trigger
    EXTI->FTSR &= ~pin_mask;  // Clear falling trigger

    // Set new trigger configuration
    if (trigger & EXTI_TRIGGER_RISING) {
        EXTI->RTSR |= pin_mask;
    }
    if (trigger & EXTI_TRIGGER_FALLING) {
        EXTI->FTSR |= pin_mask;
    }

    return GPIO_OK;
}

static gpio_status_t gpio_enable_exti_interrupt(uint8_t pin) {
    if (pin > 15) return GPIO_ERROR_INVALID_PARAM;

    // Enable interrupt mask for this EXTI line
    EXTI->IMR |= (1U << pin);

    return GPIO_OK;
}

static gpio_status_t gpio_configure_nvic(uint8_t pin, uint8_t priority) {
    IRQn_Type irq_num;
    switch (pin) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
	        irq_num = (IRQn_Type)(EXTI0_IRQn + pin);  // EXTI0-EXTI4 individual IRQs
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			irq_num = EXTI5_9_IRQn;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			irq_num = EXTI15_10_IRQn;
			break;
		default:
			return GPIO_ERROR;//add a new error
			break;
	}

    // Set priority and enable interrupt in NVIC
    NVIC_SetPriority(irq_num, priority);
    NVIC_EnableIRQ(irq_num);

    return GPIO_OK;
}

gpio_status_t gpio_interrupt_init(const gpio_interrupt_config_t *config) {
    if (!config) return GPIO_ERROR_INVALID_PARAM;

    // Step 1: Configure GPIO

    // Step 2: Enable SYSCFG clock

    // Step 3: Map GPIO to EXTI line

    // Step 4: Configure trigger type

    // Step 5: Enable EXTI interrupt

    // Step 6: Configure NVIC

    // Step 7: Store callback function

    return GPIO_OK;
}
