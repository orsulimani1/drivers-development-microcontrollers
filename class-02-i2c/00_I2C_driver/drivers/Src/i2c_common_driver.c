/*
 * i2c_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#include <stdio.h>
#include "i2c_common_driver.h"


// Helper functions for clock management
static void i2c_enable_clock(I2C_TypeDef *i2c);
static void i2c_disable_clock(I2C_TypeDef *i2c);
static i2c_status_t i2c_configure_timing(I2C_TypeDef *i2c, uint32_t clock_speed);


// protocol functions
i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout);

// Status and error checking
uint8_t i2c_is_busy(I2C_TypeDef *i2c);
i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c);


/******************************************************/
/******************************************************/
/*********************Driver Imp***********************/
/******************************************************/
/******************************************************/
// Status and error checking
uint8_t i2c_is_busy(I2C_TypeDef *i2c){
}
i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c){
   // Clear error flags by writing 0 to them
   i2c->SR1.value &= ~(I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_OVR);
   return I2C_STATUS_OK;
}

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


i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout) {
   // 1. Loop while timeout > 0
   while (timeout > 0) {
      uint32_t sr1_value = i2c->SR1.value;
      
      // Check if flag matches expected status
      if (((sr1_value & flag) != 0) == status) {
         return I2C_STATUS_OK;
      }
      
      // Check for error flags
      if (sr1_value & I2C_SR1_BERR) {
         i2c_clear_error_flags(i2c);
         return I2C_STATUS_BERR;
      }
      if (sr1_value & I2C_SR1_ARLO) {
         i2c_clear_error_flags(i2c);
         return I2C_STATUS_ARLO;
      }
      if (sr1_value & I2C_SR1_AF) {
         i2c_clear_error_flags(i2c);
         return I2C_STATUS_NACK;
      }
      if (sr1_value & I2C_SR1_OVR) {
         i2c_clear_error_flags(i2c);
         return I2C_STATUS_OVR;
      }
      
      timeout--;
   }
   
   // 2. Return timeout if loop expires
   return I2C_STATUS_TIMEOUT;
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
   if(config->no_stretch < I2C_STRETCH_ENABLE || config->no_stretch > I2C_STRETCH_DISABLE){
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
   I2C_TypeDef *i2c = &config->i2c_config;

//    - Initialize SCL and SDA pins with gpio_init()

// 3. Enable I2C peripheral clock using i2c_enable_clock()

// 4. Disable I2C peripheral (PE=0) and perform software reset
   i2c->CR1.fields.PE = 0;
   i2c->CR1.fields.SWRST = 1;
   i2c->CR1.fields.SWRST = 0;

// 5. Configure timing (master mode only) using i2c_configure_timing()

// 6. Configure own address in OAR1 register

// 7. Configure CR1 register (ACK, NOSTRETCH)

// 9. Enable I2C peripheral (PE=1)

   return I2C_STATUS_OK;
}


i2c_status_t i2c_deinit(i2c_config_t *config){
      // 1. Validate parameters
   if (!config) {
      return I2C_STATUS_INVALID;
   }
   
   I2C_TypeDef *i2c = &config->i2c_config;
   
   // 2. Disable I2C peripheral
   
   // 3. Reset all registers to default values
   
   // 4. Disable peripheral clock
   
   return I2C_STATUS_OK;
}

