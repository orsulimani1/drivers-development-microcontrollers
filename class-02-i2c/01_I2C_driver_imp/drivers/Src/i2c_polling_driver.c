/*
 * i2c_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#include <stdio.h>
#include "i2c_polling_driver.h"

extern i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout);
extern uint8_t i2c_is_busy(I2C_TypeDef *i2c);
extern i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c);
// static protocol functions
static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c);
static i2c_status_t i2c_generate_stop(I2C_TypeDef *i2c);
static i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, i2c_direction_t direction);
static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data);
static i2c_status_t i2c_receive_data(I2C_TypeDef *i2c, uint8_t *data);


/******************************************************/
/******************************************************/
/*********************Driver Imp***********************/
/******************************************************/
/******************************************************/

static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c) {
   // 1. Check if bus is busy
   if (i2c_is_busy(i2c)) {
      return I2C_STATUS_BUSY;
   }
   
   // 2. Set START bit in CR1 register
   i2c->CR1.fields.START = 1;
   
   // 3. Wait for SB flag
   return i2c_wait_flag(i2c, I2C_SR1_SB, 1, I2C_TIMEOUT_DEFAULT);
}

static i2c_status_t i2c_generate_stop(I2C_TypeDef *i2c) {
   // 1. Set STOP bit in CR1 register (CR1.STOP = 1)
   i2c->CR1.fields.STOP = 1;

   // 2. Wait for STOP bit to be cleared by hardware (poll CR1.STOP == 0)
   //    Hardware clears this bit automatically when STOP is transmitted
   return i2c_wait_flag(i2c, I2C_SR1_STOPF, 1, I2C_TIMEOUT_DEFAULT);
}

static i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, i2c_direction_t direction) {
   // 1. Prepare address byte: (address << 1) | direction
   //    direction: 0=write, 1=read
   uint8_t addr_byte = (address << 1) | direction;

   // 2. Write address byte to DR register
   i2c->DR.value = addr_byte;

   // 3. Wait for ADDR flag using i2c_wait_flag()
   i2c_status_t status = i2c_wait_flag(i2c, I2C_SR1_ADDR, 1, I2C_TIMEOUT_DEFAULT);
   if (status != I2C_STATUS_OK) {
      return status;
   }
   // 4. Clear ADDR flag by reading SR1 then SR2 registers
   volatile uint32_t temp = i2c->SR1.value;
   temp = i2c->SR2.value;
   (void)temp; // Avoid unused variable warning

   return I2C_STATUS_OK;
}

static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data) {
   // 1. Wait for TXE flag using i2c_wait_flag(i2c, I2C_SR1_TXE, true, timeout)
   i2c_status_t status = i2c_wait_flag(i2c, I2C_SR1_TXE, 1, I2C_TIMEOUT_DEFAULT);
   if (status != I2C_STATUS_OK) {
      return status;
   }
   // 2. Write data byte to DR register
   i2c->DR.value = data;
       
   return I2C_STATUS_OK;
}

static i2c_status_t i2c_receive_data(I2C_TypeDef *i2c, uint8_t *data){
   // 1. Wait for RXNE flag using i2c_wait_flag(i2c, I2C_SR1_RXNE, true, timeout)
   i2c_status_t status = i2c_wait_flag(i2c, I2C_SR1_RXNE, 1, I2C_TIMEOUT_DEFAULT);
   if (status != I2C_STATUS_OK) {
      return status;
   }
   // 2. Read and return data from DR register
   *data = (uint8_t)i2c->DR.value;
    
   return I2C_STATUS_OK;
}

// Master mode operations
i2c_status_t i2c_master_transmit(i2c_config_t *config, uint8_t slave_addr,
                                 const uint8_t *data, uint16_t length,
                                 uint32_t timeout){
// 1. Validate parameters and check if bus busy using i2c_is_busy()
   if (!config || !data || length == 0) {
         return I2C_STATUS_INVALID;
   }
   I2C_TypeDef *i2c = config->i2c_config;
   if (i2c_is_busy(i2c)) {
      return I2C_STATUS_BUSY;
   }
   // 2. Generate START condition using i2c_generate_start()
   i2c_status_t status;
    
   // 2. Generate START condition
   status = i2c_generate_start(i2c);
   if (status != I2C_STATUS_OK) {
      return status;
   }
   // 3. Send slave address with write bit using i2c_send_address()
// 3. Send slave address with write bit
   status = i2c_send_address(i2c, slave_addr, I2C_DIRECTION_WRITE);
   if (status != I2C_STATUS_OK) {
      i2c_generate_stop(i2c);
      return status;
   }
// 4. Loop through data bytes:
   for (uint16_t i = 0; i < length; i++) {
      status = i2c_send_data(i2c, data[i]);
      if (status != I2C_STATUS_OK) {
         i2c_generate_stop(i2c);
         return status;
      }
    }
// 5. Wait for BTF flag to ensure last byte transmitted
   status = i2c_wait_flag(i2c, I2C_SR1_BTF, 1, timeout);
   if (status != I2C_STATUS_OK) {
      i2c_generate_stop(i2c);
      return status;
   }
    
   // 6. Generate STOP condition
   return i2c_generate_stop(i2c);
}
i2c_status_t i2c_master_receive(i2c_config_t *config, uint8_t slave_addr,
                                uint8_t *data, uint16_t length,
                                uint32_t timeout){

// 1. Validate parameters and check if bus busy using i2c_is_busy()
   if (!config || !data || length == 0) {
      return I2C_STATUS_INVALID;
   }
   
   I2C_TypeDef *i2c = config->i2c_config;
   
   if (i2c_is_busy(i2c)) {
      return I2C_STATUS_BUSY;
   }
    i2c_status_t status;
    
// 2. Enable ACK for multi-byte reception
   if (length > 1) {
      i2c->CR1.fields.ACK = 1;
   }   
// 3. Generate START condition using i2c_generate_start()
   status = i2c_generate_start(i2c);
   if (status != I2C_STATUS_OK) {
      return status;
   }
// 4. Send slave address with read bit using i2c_send_address()
   status = i2c_send_address(i2c, slave_addr, I2C_DIRECTION_READ);
   if (status != I2C_STATUS_OK) {
      i2c_generate_stop(i2c);
      return status;
   }
// 5. Loop through expected data bytes:
   for (uint16_t i = 0; i < length; i++) {
      if (i == length - 1) {
         // For last byte: disable ACK and generate STOP
         i2c->CR1.fields.ACK = 0;
         i2c_generate_stop(i2c);
      }
      
      status = i2c_receive_data(i2c, &data[i]);
      if (status != I2C_STATUS_OK) {
         i2c_generate_stop(i2c);
         return status;
      }
   }

// 6. Re-enable ACK for future transactions
   i2c->CR1.fields.ACK = 1;

   return I2C_STATUS_OK;
}
i2c_status_t i2c_master_write_read(i2c_config_t *config, uint8_t slave_addr,
                                   const uint8_t *write_data, uint16_t write_len,
                                   uint8_t *read_data, uint16_t read_len,
                                   uint32_t timeout){
// 1. Validate parameters
   if (!config || !write_data || !read_data || 
      write_len == 0 || read_len == 0) {
      return I2C_STATUS_INVALID;
   }
   
   I2C_TypeDef *i2c = config->i2c_config;
   i2c_status_t status;
   // 2. WRITE PHASE:
//    - Generate START using i2c_generate_start()
   status = i2c_generate_start(i2c);
   if (status != I2C_STATUS_OK) {
      return status;
   }
//    - Send address+write using i2c_send_address()

   status = i2c_send_address(i2c, slave_addr, I2C_DIRECTION_WRITE);
   if (status != I2C_STATUS_OK) {
      i2c_generate_stop(i2c);
      return status;
   }
//    - Send all write data bytes

   for (uint16_t i = 0; i < write_len; i++) {
      status = i2c_send_data(i2c, write_data[i]);
      if (status != I2C_STATUS_OK) {
         i2c_generate_stop(i2c);
         return status;
      }
   }

//    - Wait for BTF flag
    status = i2c_wait_flag(i2c, I2C_SR1_BTF, 1, timeout);
    if (status != I2C_STATUS_OK) {
        i2c_generate_stop(i2c);
        return status;
    }
// 3. REPEATED START PHASE:
//    - Generate repeated START using i2c_generate_start()
   status = i2c_generate_start(i2c);
   if (status != I2C_STATUS_OK) {
      return status;
   }
    
   //    - Send address+read using i2c_send_address()
   status = i2c_send_address(i2c, slave_addr, I2C_DIRECTION_READ);
   if (status != I2C_STATUS_OK) {
      i2c_generate_stop(i2c);
      return status;
   }

// 4. READ PHASE:
//    - Enable ACK for multi-byte
if (read_len > 1) {
   i2c->CR1.fields.ACK = 1;
}

//    - Receive all read data bytes
   for (uint16_t i = 0; i < read_len; i++) {
      if (i == read_len - 1) {
         i2c->CR1.fields.ACK = 0;
         i2c_generate_stop(i2c);
      }
   
      status = i2c_receive_data(i2c, &read_data[i]);
      if (status != I2C_STATUS_OK) {
         i2c_generate_stop(i2c);
         return status;
      }
   }
// 5. Re-enable ACK for future transactions
   i2c->CR1.fields.ACK = 1;
   
   return I2C_STATUS_OK;
}