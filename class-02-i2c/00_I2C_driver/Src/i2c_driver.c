/*
 * i2c_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */


#include "i2c_driver.h"

// static protocol functions
static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c);
static i2c_status_t i2c_generate_stop(I2C_TypeDef *i2c);
static i2c_status_t i2c_send_address(I2C_TypeDef *i2c, uint8_t address, i2c_direction_t direction);
static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data);
static uint8_t i2c_receive_data(I2C_TypeDef *i2c);
static i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout);

// Status and error checking
static uint8_t i2c_is_busy(I2C_TypeDef *i2c);
static i2c_status_t i2c_clear_error_flags(I2C_TypeDef *i2c);



// Initialization and configuration
i2c_status_t i2c_init(I2C_TypeDef *i2c, const i2c_config_t *config);
// 1. Validate parameters (i2c, config, address validity)

// 2. Configure GPIO pins using config->I2C_SCL and config->I2C_SDA
//    - Enable GPIO clocks
//    - Initialize SCL and SDA pins with gpio_init()

// 3. Enable I2C peripheral clock using i2c_enable_clock()

// 4. Disable I2C peripheral (PE=0) and perform software reset

// 5. Configure timing (master mode only) using i2c_configure_timing()

// 6. Configure own address in OAR1 register

// 7. Configure CR1 register (ACK, NOSTRETCH)

// 8. Initialize slave context if mode == I2C_MODE_SLAVE

// 9. Enable I2C peripheral (PE=1)


i2c_status_t i2c_deinit(I2C_TypeDef *i2c);
// 1. Validate parameters

 // 2. Disable I2C peripheral (PE=0)

 // 3. Reset all registers to default values

 // 4. Disable peripheral clock using i2c_disable_clock()


// Master mode operations
i2c_status_t i2c_master_transmit(I2C_TypeDef *i2c, uint8_t slave_addr,
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

i2c_status_t i2c_master_receive(I2C_TypeDef *i2c, uint8_t slave_addr,
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
i2c_status_t i2c_master_write_read(I2C_TypeDef *i2c, uint8_t slave_addr,
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


// static protocol functions
static i2c_status_t i2c_generate_start(I2C_TypeDef *i2c) {
   // 1. Check if bus is busy using i2c_is_busy()

   // 2. Set START bit in CR1 register (CR1.START = 1)

   // 3. Wait for SB flag using i2c_wait_flag(i2c, I2C_SR1_SB, true, timeout)
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

   // 3. Wait for ADDR flag using i2c_wait_flag(i2c, I2C_SR1_ADDR, true, timeout)

   // 4. Clear ADDR flag by reading SR1 then SR2 registers
}

static i2c_status_t i2c_send_data(I2C_TypeDef *i2c, uint8_t data) {
   // 1. Wait for TXE flag using i2c_wait_flag(i2c, I2C_SR1_TXE, true, timeout)

   // 2. Write data byte to DR register

   // 3. Wait for BTF flag using i2c_wait_flag(i2c, I2C_SR1_BTF, true, timeout)
   //    BTF ensures byte was actually transmitted
}

static uint8_t i2c_receive_data(I2C_TypeDef *i2c) {
   // 1. Wait for RXNE flag using i2c_wait_flag(i2c, I2C_SR1_RXNE, true, timeout)

   // 2. Read and return data from DR register
   //    Return 0 on timeout/error
}

static i2c_status_t i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, uint8_t status, uint32_t timeout) {
   // 1. Loop while timeout > 0:
   //    - Check if (SR1.value & flag) matches expected status
   //    - Return I2C_STATUS_OK if match found
   //    - Check for error flags (BERR, ARLO, AF, OVR) and return error
   //    - Decrement timeout

   // 2. Return I2C_STATUS_TIMEOUT if loop expires
}
