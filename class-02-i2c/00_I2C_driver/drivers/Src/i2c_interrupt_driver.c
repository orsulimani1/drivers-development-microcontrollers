/*
 * i2c_interrupt_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#include "i2c_interrupt_driver.h"

// Static helper functions for student implementation
static i2c_status_t i2c_set_callbacks(i2c_transfer_t *transfer,
                                      i2c_transfer_callback_t tx_callback,
                                      i2c_transfer_callback_t rx_callback,
                                      void *user_data);

static void i2c_handle_start_sent(i2c_transfer_t *transfer, uint8_t slave_addr, i2c_direction_t direction);
static void i2c_handle_address_sent(i2c_transfer_t *transfer);
static void i2c_handle_tx_empty(i2c_transfer_t *transfer);
static void i2c_handle_rx_not_empty(i2c_transfer_t *transfer);
static void i2c_handle_transfer_complete(i2c_transfer_t *transfer);
static void configure_i2c_nvic(I2C_TypeDef *i2c);

// State machine tracking variables (could be per-transfer in real implementation)
static uint8_t current_slave_addr = 0;
static i2c_direction_t current_direction = I2C_DIRECTION_WRITE;
static uint16_t rx_bytes_remaining = 0;

// Initialization with interrupt support
i2c_status_t i2c_init_interrupt(const i2c_config_t *config, i2c_transfer_t *transfer) {
    // TODO: Student Implementation
    // 1. Validate parameters (config and transfer not NULL)
    
    // 2. Initialize common I2C using i2c_init() from common driver
    
    // 3. Store I2C instance in transfer structure
    
    // 4. Initialize ring buffers using ring_buffer_init()
    
    // 5. Set initial state to I2C_STATE_IDLE
    
    // 6. Configure NVIC interrupts for I2C event and error
    //    - Enable I2C_EV_IRQn and I2C_ER_IRQn
    //    - Set appropriate priorities
    
    // 7. Enable I2C interrupts using i2c_enable_interrupts()
    
    if (!config || !transfer) {
        return I2C_STATUS_INVALID;
    }
    
    // Initialize common I2C
    i2c_status_t status = i2c_init(config);
    if (status != I2C_STATUS_OK) {
        return status;
    }
    
    // Setup transfer structure
    transfer->i2c_instance = &config->i2c_config;
    ring_buffer_init(&transfer->tx_buffer);
    ring_buffer_init(&transfer->rx_buffer);
    transfer->state = I2C_STATE_IDLE;
    transfer->status = I2C_STATUS_OK;
    transfer->tx_complete_callback = NULL;
    transfer->rx_complete_callback = NULL;
    transfer->user_data = NULL;
    
    // Configure NVIC
    I2C_TypeDef *i2c = transfer->i2c_instance;
    configure_i2c_nvic(i2c);

    return i2c_enable_interrupts(i2c);
}

void configure_i2c_nvic(I2C_TypeDef *i2c)
{
    if (i2c == I2C1)
    {
        NVIC_EnableIRQ(I2C1_EV_IRQn);
        NVIC_EnableIRQ(I2C1_ER_IRQn);
        NVIC_SetPriority(I2C1_EV_IRQn, 5);
        NVIC_SetPriority(I2C1_ER_IRQn, 4);
    }
    else if (i2c == I2C2)
    {
        NVIC_EnableIRQ(I2C2_EV_IRQn);
        NVIC_EnableIRQ(I2C2_ER_IRQn);
        NVIC_SetPriority(I2C2_EV_IRQn, 5);
        NVIC_SetPriority(I2C2_ER_IRQn, 4);
    }
    else if (i2c == I2C3)
    {
        NVIC_EnableIRQ(I2C3_EV_IRQn);
        NVIC_EnableIRQ(I2C3_ER_IRQn);
        NVIC_SetPriority(I2C3_EV_IRQn, 5);
        NVIC_SetPriority(I2C3_ER_IRQn, 4);
    }
}

// Non-blocking master transmit
i2c_status_t i2c_master_transmit_it(i2c_transfer_t *transfer, uint8_t slave_addr,
                                    const uint8_t *data, uint16_t length) {
    if (!transfer || !data || length == 0) {
        return I2C_STATUS_INVALID;
    }
    
    if (transfer->state != I2C_STATE_IDLE) {
        return I2C_STATUS_BUSY;
    }

    // Check if TX buffer has enough space
    // Copy data to TX buffer
    if(ring_buffer_put_multiple(&transfer->tx_buffer, data, length) == RING_BUFFER_FULL){
        return I2C_STATUS_ERROR;
    }
    
    // Setup transfer state machine
    transfer->state = I2C_STATE_START_SENT;
    transfer->status = I2C_STATUS_OK;
    current_slave_addr = slave_addr;
    current_direction = I2C_DIRECTION_WRITE;
    
    // Generate START condition (triggers SB interrupt)
    transfer->i2c_instance->CR1.fields.START = 1;
    
    return I2C_STATUS_OK;
}

// Non-blocking master receive
i2c_status_t i2c_master_receive_it(i2c_transfer_t *transfer, uint8_t slave_addr,
                                   uint8_t *data, uint16_t length) {
    if (!transfer || !data || length == 0) {
        return I2C_STATUS_INVALID;
    }
    
    if (transfer->state != I2C_STATE_IDLE) {
        return I2C_STATUS_BUSY;
    }
    
    // Setup for receive operation
    transfer->state = I2C_STATE_START_SENT;
    transfer->status = I2C_STATUS_OK;
    current_slave_addr = slave_addr;
    current_direction = I2C_DIRECTION_READ;
    rx_bytes_remaining = length;
    
    // Enable ACK for multi-byte reception
    if (length > 1) {
        transfer->i2c_instance->CR1.fields.ACK = 1;
    }
    
    // Generate START condition
    transfer->i2c_instance->CR1.fields.START = 1;
    
    return I2C_STATUS_OK;
}

// Buffered transmit using ring buffer
i2c_status_t i2c_transmit_buffered(i2c_transfer_t *transfer, uint8_t slave_addr,
                                   const uint8_t *data, uint16_t length) {
    // TODO: Student Implementation
    // 1. Add data to TX ring buffer using ring_buffer_put_multiple()
    
    // 2. If transfer is idle, start transmission using i2c_master_transmit_it()
    
    // 3. If transfer is busy, data will be sent automatically by ISR
        
    return I2C_STATUS_OK;
}

// Transfer status functions
uint8_t i2c_is_transfer_complete(i2c_transfer_t *transfer) {
    return transfer ? (transfer->state == I2C_STATE_IDLE) : 1;
}

i2c_status_t i2c_get_transfer_status(i2c_transfer_t *transfer) {
    return transfer ? transfer->status : I2C_STATUS_INVALID;
}

i2c_status_t i2c_abort_transfer(i2c_transfer_t *transfer) {
    // TODO: Student Implementation
    // 1. Generate STOP condition
    
    // 2. Reset state to IDLE
    
    // 3. Flush ring buffers
    
    if (!transfer) {
        return I2C_STATUS_INVALID;
    }
        
    return I2C_STATUS_OK;
}

// Event interrupt handler
void i2c_event_irq_handler(i2c_transfer_t *transfer) {
    if (!transfer) return;
    
    I2C_TypeDef *i2c = transfer->i2c_instance;
    uint32_t sr1 = i2c->SR1.value;
    
    // State machine: Handle events based on current state
    switch (transfer->state) {
        case I2C_STATE_START_SENT:

            break;
            
        case I2C_STATE_ADDRESS_SENT_TX:
        case I2C_STATE_ADDRESS_SENT_RX:

            break;
            
        case I2C_STATE_DATA_TX:

            break;
            
        case I2C_STATE_DATA_RX:

            break;
            
        default:
            // Unexpected state - reset to idle
            transfer->state = I2C_STATE_IDLE;
            break;
    }
}

// Error interrupt handler
void i2c_error_irq_handler(i2c_transfer_t *transfer) {
    // TODO: Student Implementation
    // 1. Read SR1 register to identify error
    
    // 2. Handle different errors:
    //    - AF (NACK): Set status to I2C_STATUS_NACK
    //    - BERR: Set status to I2C_STATUS_BERR
    //    - ARLO: Set status to I2C_STATUS_ARLO
    //    - OVR: Set status to I2C_STATUS_OVR
    
    // 3. Clear error flags using i2c_clear_error_flags()
    
    // 4. Set state to I2C_STATE_ERROR
    
    // 5. Generate STOP condition
    
    if (!transfer) return;
    
    I2C_TypeDef *i2c = transfer->i2c_instance;
    uint32_t sr1 = i2c->SR1.value;
    
    // Determine error type
    if (sr1 & I2C_SR1_AF) {
        transfer->status = I2C_STATUS_NACK;
    } else if (sr1 & I2C_SR1_BERR) {
        transfer->status = I2C_STATUS_BERR;
    } else if (sr1 & I2C_SR1_ARLO) {
        transfer->status = I2C_STATUS_ARLO;
    } else if (sr1 & I2C_SR1_OVR) {
        transfer->status = I2C_STATUS_OVR;
    } else {
        transfer->status = I2C_STATUS_ERROR;
    }
    
    // Clear error flags
    
    // Set error state
    transfer->state = -1;
    
    // Generate STOP
}

// Interrupt control functions
i2c_status_t i2c_enable_interrupts(I2C_TypeDef *i2c) {
    // TODO: Student Implementation
    // Enable event, error, and buffer interrupts in CR2 register
    
    if (!i2c) return I2C_STATUS_INVALID;
    
    // Enable event interrupts
    // Enable error interrupts
    // Enable buffer interrupts
    
    return I2C_STATUS_OK;
}

i2c_status_t i2c_disable_interrupts(I2C_TypeDef *i2c) {
    // TODO: Student Implementation
    // Disable all interrupts in CR2 register
    
    if (!i2c) return I2C_STATUS_INVALID;

    // event interrupts
    // error interrupts
    // buffer interrupts

    return I2C_STATUS_OK;
}

// Static helper functions
static i2c_status_t i2c_set_callbacks(i2c_transfer_t *transfer,
                                      i2c_transfer_callback_t tx_callback,
                                      i2c_transfer_callback_t rx_callback,
                                      void *user_data) {
    if (!transfer) return I2C_STATUS_INVALID;
    
    transfer->tx_complete_callback = tx_callback;
    transfer->rx_complete_callback = rx_callback;
    transfer->user_data = user_data;
    
    return I2C_STATUS_OK;
}

static void i2c_handle_start_sent(i2c_transfer_t *transfer, uint8_t slave_addr, i2c_direction_t direction) {
    I2C_TypeDef *i2c = transfer->i2c_instance;
    
    // Send slave address with direction bit
    uint8_t addr_byte = (slave_addr << 1) | direction;
    i2c->DR.value = addr_byte;
    
    // Update state based on direction
    if (direction == I2C_DIRECTION_WRITE) {
    } else {
    }
}

static void i2c_handle_address_sent(i2c_transfer_t *transfer) {
    I2C_TypeDef *i2c = transfer->i2c_instance;
    
    // Clear ADDR flag by reading SR1 then SR2
    volatile uint32_t temp = i2c->SR1.value;
    temp = i2c->SR2.value;
    (void)temp;
    
    // Transition to data phase based on current state
    if (transfer->state == I2C_STATE_ADDRESS_SENT_TX) {
        transfer->state = I2C_STATE_DATA_TX;
        // TXE interrupt will be triggered automatically
    } else if (transfer->state == I2C_STATE_ADDRESS_SENT_RX) {
        transfer->state = I2C_STATE_DATA_RX;
        
        // Handle single byte read
        if (rx_bytes_remaining == 1) {
            // Disable ACK and generate STOP for single byte
            i2c->CR1.fields.ACK = 0;
            i2c->CR1.fields.STOP = 1;
        }
        // RXNE interrupt will be triggered when data arrives
    }
}

static void i2c_handle_tx_empty(i2c_transfer_t *transfer) {
    uint8_t data;
    
    // Get next byte from TX ring buffer
    if (ring_buffer_get(&transfer->tx_buffer, &data) == RING_BUFFER_OK) {
        // Send next byte
        transfer->i2c_instance->DR.value = data;
    } else {
        // No more data - BTF interrupt will complete transfer
        // Do nothing here, wait for BTF
    }
}

static void i2c_handle_rx_not_empty(i2c_transfer_t *transfer) {
    I2C_TypeDef *i2c = transfer->i2c_instance;
    
    // Read received data
    uint8_t data = i2c->DR.value;
    ring_buffer_put(&transfer->rx_buffer, data);
    
    rx_bytes_remaining--;
    
    // Handle last byte logic
    if (rx_bytes_remaining == 1) {
        // Prepare for last byte: disable ACK and generate STOP
        i2c->CR1.fields.ACK = 0;
        i2c->CR1.fields.STOP = 1;
    } else if (rx_bytes_remaining == 0) {
        // All bytes received - complete transfer
        i2c_handle_transfer_complete(transfer);
    }
}

static void i2c_handle_transfer_complete(i2c_transfer_t *transfer) {
    I2C_TypeDef *i2c = transfer->i2c_instance;
    
    // Ensure STOP condition is generated for TX transfers
    if (transfer->state == I2C_STATE_DATA_TX) {
        i2c->CR1.fields.STOP = 1;
    }
    
    // Reset to idle state
    transfer->state = I2C_STATE_IDLE;
    transfer->status = I2C_STATUS_OK;
    
    // Re-enable ACK for future transfers
    i2c->CR1.fields.ACK = 1;
    
    // Call appropriate callback
    if (current_direction == I2C_DIRECTION_WRITE && transfer->tx_complete_callback) {
        transfer->tx_complete_callback(transfer->user_data);
    } else if (current_direction == I2C_DIRECTION_READ && transfer->rx_complete_callback) {
        transfer->rx_complete_callback(transfer->user_data);
    }
}