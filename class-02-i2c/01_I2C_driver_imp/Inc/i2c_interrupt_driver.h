/*
 * i2c_interrupt_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef I2C_INTERRUPT_DRIVER_H_
#define I2C_INTERRUPT_DRIVER_H_

#include "i2c_common_driver.h"

/**
 * @brief I2C transfer completion callback function type
 * @param user_data: User-defined data pointer
 * @return status: Callback execution status
 */
typedef i2c_status_t (*i2c_transfer_callback_t)(void *user_data);

/**
 * @brief I2C interrupt configuration structure
 */
typedef struct {
    i2c_transfer_callback_t tx_complete_callback;  ///< TX completion callback
    i2c_transfer_callback_t rx_complete_callback;  ///< RX completion callback
    i2c_transfer_callback_t error_callback;        ///< Error callback
} i2c_interrupt_config_t;

// Initialization with interrupt support
i2c_status_t i2c_init_interrupt(const i2c_config_t *config, 
                                const i2c_interrupt_config_t *int_config);

// Non-blocking master operations
i2c_status_t i2c_master_transmit_it(I2C_TypeDef *i2c, uint8_t slave_addr,
                                    const uint8_t *data, uint16_t length);

i2c_status_t i2c_master_receive_it(I2C_TypeDef *i2c, uint8_t slave_addr,
                                   uint8_t *data, uint16_t length);

i2c_status_t i2c_master_write_read_it(I2C_TypeDef *i2c, uint8_t slave_addr,
                                      const uint8_t *write_data, uint16_t write_len,
                                      uint8_t *read_data, uint16_t read_len);

// Transfer status and control
uint8_t i2c_is_transfer_complete(I2C_TypeDef *i2c);
i2c_status_t i2c_get_transfer_status(I2C_TypeDef *i2c);
i2c_status_t i2c_abort_transfer(I2C_TypeDef *i2c);

// Interrupt service routines (to be called from ISR vectors)
void i2c_event_irq_handler(I2C_TypeDef *i2c);
void i2c_error_irq_handler(I2C_TypeDef *i2c);

// Interrupt enable/disable
i2c_status_t i2c_enable_interrupts(I2C_TypeDef *i2c);
i2c_status_t i2c_disable_interrupts(I2C_TypeDef *i2c);

#endif /* I2C_INTERRUPT_DRIVER_H_ */