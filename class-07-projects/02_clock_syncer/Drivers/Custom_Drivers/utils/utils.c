/*
 * utils.c
 *
 *  Created on: Aug 5, 2025
 *      Author: Orr
 */
#include "main.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;

void uart_test_main(void) {
    char buffer[64];
    char rx_buffer[50];

    uint32_t counter = 0;

    while (1) {
        sprintf(buffer, "Hi I'm here %lu\r\n", counter);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
        if (HAL_UART_Receive(&huart1, (uint8_t*)rx_buffer, 5, 1000) == HAL_OK) {
            rx_buffer[sizeof(rx_buffer)-1] = '\0';

            sprintf(buffer, "Received: %s\r\n", rx_buffer);
            HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
        }

        counter++;
        HAL_Delay(200);
    }
}

void i2c_scanner(void) {
    char buffer[100];
    uint8_t devices_found = 0;

    sprintf(buffer, "Scanning I2C addresses...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 100) == HAL_OK) {
            sprintf(buffer, "Device found at 0x%02X\r\n", addr);
            HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
            devices_found++;
        }
    }

    if (devices_found == 0) {
        sprintf(buffer, "No I2C devices found\r\n");
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    }

    sprintf(buffer, "Scan complete. Found %d devices.\r\n", devices_found);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
}


void uart_interactive_test(void) {
    char rx_buffer[64];
    char tx_buffer[100];
    uint32_t counter = 0;

    sprintf(tx_buffer, "UART Test Ready - Send any message\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);

    while (1) {
        // Send periodic heartbeat
        sprintf(tx_buffer, "Heartbeat: %lu\r\n", counter++);
        HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);

        // Check for incoming data
        if (HAL_UART_Receive(&huart1, (uint8_t*)rx_buffer, 3, 100) == HAL_OK) {
            rx_buffer[sizeof(rx_buffer)-1] = '\0';

            sprintf(tx_buffer, "Received: %s\r\n", rx_buffer);
            HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);
        }

        HAL_Delay(2000);  // 2 second intervals
    }
}
