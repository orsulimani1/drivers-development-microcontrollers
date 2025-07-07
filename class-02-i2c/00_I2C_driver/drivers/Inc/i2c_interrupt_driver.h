/*
 * i2c_interrupt_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef I2C_INTERRUPT_DRIVER_H_
#define I2C_INTERRUPT_DRIVER_H_

#include "i2c_common_driver.h"
#include "ring_buffer.h"

/**
 * I2C driver states
 */
typedef enum {
    I2C_STATE_IDLE = 0,
    I2C_STATE_START_SENT,
    I2C_STATE_ADDRESS_SENT_TX,
    I2C_STATE_ADDRESS_SENT_RX,
    I2C_STATE_DATA_TX,
    I2C_STATE_DATA_RX,
    I2C_STATE_STOP_SENT,
    I2C_STATE_ERROR
} i2c_state_t;

/**
 * @brief I2C transfer completion callback function type
 * @param user_data: User-defined data pointer
 * @return status: Callback execution status
 */
typedef i2c_status_t (*i2c_transfer_callback_t)(void *user_data);

/**
 * @brief I2C transfer structure
 */
typedef struct {
    I2C_TypeDef *i2c_instance;
    ring_buffer_t tx_buffer;
    ring_buffer_t rx_buffer;
    
    volatile i2c_state_t state;
    volatile i2c_status_t status;
    
    i2c_transfer_callback_t tx_complete_callback;  ///< TX completion callback
    i2c_transfer_callback_t rx_complete_callback;  ///< RX completion callback
    void *user_data;
} i2c_transfer_t;


// Initialization with interrupt and transfer handle
i2c_status_t i2c_init_interrupt(const i2c_config_t *config, i2c_transfer_t *transfer);

// Non-blocking master operations
i2c_status_t i2c_master_transmit_it(i2c_transfer_t *transfer, uint8_t slave_addr,
                                    const uint8_t *data, uint16_t length);

i2c_status_t i2c_master_receive_it(i2c_transfer_t *transfer, uint8_t slave_addr,
                                   uint8_t *data, uint16_t length);

i2c_status_t i2c_master_write_read_it(i2c_transfer_t *transfer, uint8_t slave_addr,
                                      const uint8_t *write_data, uint16_t write_len,
                                      uint8_t *read_data, uint16_t read_len);

// Buffered operations (using ring buffers)
i2c_status_t i2c_transmit_buffered(i2c_transfer_t *transfer, uint8_t slave_addr,
                                   const uint8_t *data, uint16_t length);
i2c_status_t i2c_receive_buffered(i2c_transfer_t *transfer, uint8_t slave_addr,
                                  uint16_t length);

// Transfer status and control
uint8_t i2c_is_transfer_complete(i2c_transfer_t *transfer);
i2c_status_t i2c_get_transfer_status(i2c_transfer_t *transfer);
i2c_status_t i2c_abort_transfer(i2c_transfer_t *transfer);



// Interrupt service routines (to be called from ISR vectors)
void i2c_event_irq_handler(i2c_transfer_t *transfer);
void i2c_error_irq_handler(i2c_transfer_t *transfer);

// Interrupt enable/disable
i2c_status_t i2c_enable_interrupts(I2C_TypeDef *i2c);
i2c_status_t i2c_disable_interrupts(I2C_TypeDef *i2c);

#endif /* I2C_INTERRUPT_DRIVER_H_ */