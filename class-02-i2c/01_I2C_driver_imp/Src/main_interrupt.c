/*
 * main_interrupt.c
 *
 *  Created on: Jul 8, 2025
 *      Author: Orr
 */


#include "stm32f411_base.h"
#include "gpio_driver.h"
#include "i2c_common_driver.h"
#include "i2c_interrupt_driver.h"

// I2C configuration and transfer handle
i2c_config_t i2c1_config;
i2c_transfer_t i2c1_transfer;


// Test data
uint8_t tx_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
uint8_t rx_buffer[20];

// Callback functions
i2c_status_t tx_complete_callback(void *user_data) {
    // TX operation completed
    // Toggle LED or set flag
    return I2C_STATUS_OK;
}

i2c_status_t rx_complete_callback(void *user_data) {
    // RX operation completed
    // Process received data from rx_buffer
    return I2C_STATUS_OK;
}


int main(void) {


    // Configure I2C1 GPIO pins (PB6=SCL, PB7=SDA)
    gpio_config_t i2c_scl_config = {
        .port = GPIOB,
        .pin = 6,
        .mode = GPIO_MODE_ALTERNATE,
        .output_type = GPIO_OTYPE_OPEN_DRAIN,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULL_UP,
        .alternate = 4  // AF4 for I2C1
    };

    gpio_config_t i2c_sda_config = {
        .port = GPIOB,
        .pin = 7,
        .mode = GPIO_MODE_ALTERNATE,
        .output_type = GPIO_OTYPE_OPEN_DRAIN,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULL_UP,
        .alternate = 4  // AF4 for I2C1
    };

    // Setup I2C configuration
    i2c1_config.i2c_config = (I2C_TypeDef *)I2C1;
    i2c1_config.i2c_scl_gpio = &i2c_scl_config;
    i2c1_config.i2c_sda_gpio = &i2c_sda_config;
    i2c1_config.clock_speed = 100000;  // 100kHz
    i2c1_config.duty_cycle = I2C_DUTY_CYCLE_2;
    i2c1_config.addr_mode = I2C_ADDR_7BIT;
    i2c1_config.ack_enable = I2C_ACK_ENABLE;
    i2c1_config.no_stretch = I2C_STRETCH_ENABLE;

    // Initialize I2C with interrupt support
    i2c_status_t status = i2c_init_interrupt(&i2c1_config, &i2c1_transfer);
    if (status != I2C_STATUS_OK) {
        // Initialization failed
        while(1);
    }

    // Set callbacks
    i2c1_transfer.tx_complete_callback = tx_complete_callback;
    i2c1_transfer.rx_complete_callback = rx_complete_callback;
    i2c1_transfer.user_data = NULL;

    while (1) {
        // Example 1: Non-blocking transmit
        if (i2c_is_transfer_complete(&i2c1_transfer)) {
            status = i2c_master_transmit_it(&i2c1_transfer, 0x50, tx_data, sizeof(tx_data));
            if (status == I2C_STATUS_OK) {
                // Transfer started successfully
                // Continue with other tasks while transfer happens in background
            }
        }

        // Do other work while I2C transfer happens
        for (volatile int i = 0; i < 500000; i++);

        // Example 2: Non-blocking receive
        if (i2c_is_transfer_complete(&i2c1_transfer)) {
            status = i2c_master_receive_it(&i2c1_transfer, 0x50, rx_buffer, 8);
            if (status == I2C_STATUS_OK) {
                // Receive started successfully
            }
        }

        // Do other work
        for (volatile int i = 0; i < 500000; i++);

    // Example 3: Buffered operations for continuous data streaming
        uint8_t stream_data[] = {0x11, 0x22, 0x33};
        status = i2c_transmit_buffered(&i2c1_transfer, 0x50, stream_data, 3);
        
        // Do other application tasks
        for (volatile int i = 0; i < 2000000; i++); 
    }

    return 0;
}
