/*
 * rs485_driver.c
 *
 *  Created on: Jun 3, 2025
 *      Author: Orr
 */


#include "rs485_driver.h"
#include "stm32f4xx.h"

// Private variables
static rs485_config_t driver_config;
static bool is_initialized = false;
static volatile bool is_busy = false;

// Private function prototypes
static void configure_uart(void);
static void configure_direction_pin(void);
static void enable_interrupts(void);

rs485_status_t rs485_init(const rs485_config_t *config) {
    if (config == NULL) {
        return RS485_ERROR_INVALID_PARAM;
    }

    // Copy configuration
    driver_config = *config;

    // Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // GPIO clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // USART clock

    // Configure UART peripheral
    configure_uart();

    // Configure direction control pin
    configure_direction_pin();

    // Enable interrupts
    enable_interrupts();

    is_initialized = true;
    return RS485_OK;
}

rs485_status_t rs485_transmit(const uint8_t *data, uint16_t length) {
    if (!is_initialized || data == NULL || length == 0) {
        return RS485_ERROR_INVALID_PARAM;
    }

    if (is_busy) {
        return RS485_ERROR_BUSY;
    }

    is_busy = true;

    // Set to transmit mode
    rs485_set_mode(true);

    // Start transmission
    for (uint16_t i = 0; i < length; i++) {
        while (!(USART2->SR & USART_SR_TXE));
        USART2->DR = data[i];
    }

    // Wait for transmission complete
    while (!(USART2->SR & USART_SR_TC));

    // Set to receive mode
    rs485_set_mode(false);

    is_busy = false;

    if (driver_config.tx_complete_callback) {
        driver_config.tx_complete_callback();
    }

    return RS485_OK;
}

rs485_status_t rs485_set_mode(bool transmit_mode) {
    if (transmit_mode) {
        GPIOA->BSRR = GPIO_BSRR_BS8;  // Set PA8 high (transmit)
    } else {
        GPIOA->BSRR = GPIO_BSRR_BR8;  // Set PA8 low (receive)
    }
    return RS485_OK;
}

// Private function implementations
static void configure_uart(void) {
    // Configure GPIO pins for UART
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFRL2_Pos) | (7 << GPIO_AFRL_AFRL3_Pos);

    // Configure UART
    USART2->BRR = SystemCoreClock / driver_config.baudrate;
    USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}
