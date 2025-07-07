/*
 * i2c_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#include <stdio.h>
#include "i2c_driver.h"


// Helper functions for clock management
static void i2c_enable_clock(I2C_TypeDef *i2c);
static void i2c_disable_clock(I2C_TypeDef *i2c);


// static protocol functions
static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c);
static i2c_status_t i2c_generate_stop(I2C_TypeDef *i2c);
static i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, i2c_direction_t direction);
static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data);
static i2c_status_t i2c_receive_data(I2C_TypeDef *i2c, uint8_t *data);
static i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout);
static i2c_status_t i2c_configure_timing(I2C_TypeDef *i2c, uint32_t clock_speed);

// Status and error checking
static uint8_t i2c_is_busy(I2C_TypeDef *i2c);
static i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c);


/******************************************************/
/******************************************************/
/*********************Driver Imp***********************/
/******************************************************/
/******************************************************/

static void i2c_enable_clock(I2C_TypeDef *i2c) {
   if (i2c == I2C1) {
      RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
   } else if (i2c == I2C2) {
      RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
   } else if (i2c == I2C3) {
      RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
   }
}

static void i2c_disable_clock(I2C_TypeDef *i2c) {
    if (i2c == I2C1) {
        RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;
    } else if (i2c == I2C2) {
        RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN;
    } else if (i2c == I2C3) {
        RCC->APB1ENR &= ~RCC_APB1ENR_I2C3EN;
    }
}

// Configure I2C timing - SET AS HOMEWORK
// TODO: Implement proper CCR and TRISE calculation based on clock_speed
// For now, use default standard mode (100kHz) configuration
static i2c_status_t i2c_configure_timing(I2C_TypeDef *i2c, uint32_t clock_speed) {
    // HOMEWORK: Calculate proper timing values
    // Temporary default configuration for 100kHz standard mode
    i2c->CR2.fields.FREQ = 42;  // 42MHz APB1 clock
    i2c->CCR.fields.FS = 0;    // Standard mode
    i2c->CCR.fields.CCR = 210;  // Default value for 100kHz
    i2c->TRISE.value = 43;      // Default TRISE value
    
    return I2C_STATUS_OK;
}

static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c) {
   // 1. Check if bus is busy using i2c_is_busy()

   // 2. Set START bit in CR1 register (CR1.START = 1)

   // 3. Wait for SB flag using i2c_wait_flag()
}

static i2c_status_t i2c_generate_stop(I2C_TypeDef *i2c) {
   // 1. Set STOP bit in CR1 register (CR1.STOP = 1)

   // 2. Wait for STOP bit to be cleared by hardware (poll CR1.STOP == 0)
   //    Hardware clears this bit automatically when STOP is transmitted
}

static i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, i2c_direction_t direction) {
   // 1. Prepare address byte: (address << 1) | direction
   //    direction: 0=write, 1=read

   // 2. Write address byte to DR register

   // 3. Wait for ADDR flag using i2c_wait_flag()

   // 4. Clear ADDR flag by reading SR1 then SR2 registers
}

static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data) {
   // 1. Wait for TXE flag using i2c_wait_flag(i2c, I2C_SR1_TXE, true, timeout)

   // 2. Write data byte to DR register
}

static i2c_status_t i2c_receive_data(I2C_TypeDef *i2c, uint8_t *data){
   // 1. Wait for RXNE flag using i2c_wait_flag(i2c, I2C_SR1_RXNE, true, timeout)

   // 2. Read and return data from DR register
   //    Return 0 on timeout/error
}

static i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout) {
   // 1. Loop while timeout > 0:
   //    - Check if (SR1.value & flag)|| (SR2.value & flag) matches expected status
   //    - Return I2C_STATUS_OK if match found
   //    - Check for error flags (BERR, ARLO, AF, OVR) and return error
   //    - Decrement timeout

   // 2. Return I2C_STATUS_TIMEOUT if loop expires
}

// SR2.fields.BUSY
static uint8_t i2c_is_busy(I2C_TypeDef *i2c){
}
static i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c){
   // Clear error flags by writing 0 to them
   return I2C_STATUS_OK;
}

static i2c_status_t validate_i2c_config(const i2c_config_t *config){
   if (config == NULL || config->i2c_sda_gpio == NULL || config->i2c_scl_gpio == NULL){
      return I2C_STATUS_ERROR;
   }
   if(config->ack_enable < I2C_ACK_DISABLE || config->ack_enable > I2C_ACK_ENABLE ){
      return I2C_STATUS_INVALID;
   }
   if(config->clock_speed < 100000 || config->clock_speed > 400000){
      return I2C_STATUS_INVALID;

   }
   if(config->duty_cycle < I2C_DUTY_CYCLE_2 || config->duty_cycle > I2C_DUTY_CYCLE_16_9){
      return I2C_STATUS_INVALID;
   }
   if(config->addr_mode < I2C_ADDR_7BIT || config->addr_mode > I2C_ADDR_10BIT){
      return I2C_STATUS_INVALID;
   }
   if(config->ack_enable < I2C_ACK_DISABLE || config->ack_enable > I2C_ACK_ENABLE){
      return I2C_STATUS_INVALID;
   }
   if(config->no_stretch < I2C_STRETCH_DISABLE || config->ack_enable > I2C_STRETCH_ENABLE){
      return I2C_STATUS_INVALID;
   }

   return I2C_STATUS_OK;
}

// Initialization and configuration
i2c_status_t i2c_init(const i2c_config_t *config){
   int ret;
// 1. Validate parameters (i2c, config, address validity)
   if((ret = validate_i2c_config(config)) || I2C_STATUS_INVALID || I2C_STATUS_ERROR){
      return ret;
   }
// 2. Configure GPIO pins using config->I2C_SCL and config->I2C_SDA
//    - Initialize SCL and SDA pins with gpio_init()

// 3. Enable I2C peripheral clock using i2c_enable_clock()

// 4. Disable I2C peripheral (PE=0) and perform software reset

// 5. Configure timing (master mode only) using i2c_configure_timing()

// 6. Configure own address in OAR1 register

// 7. Configure CR1 register (ACK, NOSTRETCH)

// 9. Enable I2C peripheral (PE=1)
}


i2c_status_t i2c_deinit(i2c_config_t *i2c);
// 1. Validate parameters

 // 2. Disable I2C peripheral (PE=0)

 // 3. Reset all registers to default values

 // 4. Disable peripheral clock using i2c_disable_clock()


// Master mode operations
i2c_status_t i2c_master_transmit(i2c_config_t *i2c, uint8_t slave_addr,
                                 const uint8_t *data, uint16_t length,
                                 uint32_t timeout);
// 1. Validate parameters and check if bus busy using i2c_is_busy()

// 2. Generate START condition using i2c_generate_start()

// 3. Send slave address with write bit using i2c_send_address()

// 4. Loop through data bytes:
//    - Wait for TXE flag using i2c_wait_flag()
//    - Send each byte using i2c_send_data()

// 5. Wait for BTF flag to ensure last byte transmitted

// 6. Generate STOP condition using i2c_generate_stop()

i2c_status_t i2c_master_receive(i2c_config_t *i2c, uint8_t slave_addr,
                                uint8_t *data, uint16_t length,
                                uint32_t timeout);
// 1. Validate parameters and check if bus busy using i2c_is_busy()

// 2. Enable ACK for multi-byte reception

// 3. Generate START condition using i2c_generate_start()

// 4. Send slave address with read bit using i2c_send_address()

// 5. Loop through expected data bytes:
//    - For last byte: disable ACK and generate STOP
//    - Wait for RXNE flag using i2c_wait_flag()
//    - Read byte using i2c_receive_data()

// 6. Re-enable ACK for future transactions
i2c_status_t i2c_master_write_read(i2c_config_t *i2c, uint8_t slave_addr,
                                   const uint8_t *write_data, uint16_t write_len,
                                   uint8_t *read_data, uint16_t read_len,
                                   uint32_t timeout);
// 1. Validate parameters

// 2. WRITE PHASE:
//    - Generate START using i2c_generate_start()
//    - Send address+write using i2c_send_address()
//    - Send all write data bytes
//    - Wait for BTF flag

// 3. REPEATED START PHASE:
//    - Generate repeated START using i2c_generate_start()
//    - Send address+read using i2c_send_address()

// 4. READ PHASE:
//    - Enable ACK for multi-byte
//    - Receive all read data bytes
//    - NACK and STOP on last byte

// 5. Re-enable ACK for future transactions


