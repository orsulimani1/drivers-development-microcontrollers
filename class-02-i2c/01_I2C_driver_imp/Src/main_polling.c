/*
 * main_polling.c
 *
 *  Created on: Jul 8, 2025
 *      Author: Orr
 * I2C Polling Driver Example
 */

#include "stm32f411_base.h"
#include "gpio_driver.h"
#include "i2c_common_driver.h"
#include "i2c_polling_driver.h"

// I2C configuration
i2c_config_t i2c1_config;

// Test data
uint8_t tx_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t rx_data[10];

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
    
    // Initialize I2C
    i2c_status_t status = i2c_init(&i2c1_config);
    if (status != I2C_STATUS_OK) {
        // Initialization failed
        while(1);
    }
    
    while (1) {
        // Example 1: Simple transmit to slave address 0x50
        status = i2c_master_transmit(&i2c1_config, 0x50, tx_data, sizeof(tx_data), 1000);
        if (status == I2C_STATUS_OK) {
            // Transmission successful
        }
        
        // Delay
        for (volatile int i = 0; i < 1000000; i++);
        
        // Example 2: Simple receive from slave address 0x50
        status = i2c_master_receive(&i2c1_config, 0x50, rx_data, 5, 1000);
        if (status == I2C_STATUS_OK) {
            // Reception successful - process rx_data
        }
        
        // Delay
        for (volatile int i = 0; i < 1000000; i++);
        
        // Example 3: Write-Read operation (write register address, read data)
        uint8_t reg_addr = 0x00;  // Register to read
        status = i2c_master_write_read(&i2c1_config, 0x50, 
                                       &reg_addr, 1, 
                                       rx_data, 4, 1000);
        if (status == I2C_STATUS_OK) {
            // Write-read successful - process rx_data
        }
        
        // Delay before next cycle
        for (volatile int i = 0; i < 2000000; i++);
    }
    
    return 0;
}