#include "spi_common.h"

/**
* @brief Transmit data in interrupt mode
 * @param hspi: SPI handle pointer
 * @param data: Data buffer to transmit
 * @param size: Number of bytes to transmit
 * @return spi_status_t: Operation status
 */
spi_status_t spi_transmit_it(spi_handle_t *hspi, uint8_t *data, uint16_t size)
{
    if (!hspi || !data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }
    
    // Write data to transmit ring buffer
    for (uint16_t i = 0; i < size; i++) {
        if (!ring_buffer_write(hspi->tx_ring_buffer, data[i])) {
            return SPI_STATUS_ERROR;  // Buffer full
        }
    }
    
    // Set up single transfer mode
    hspi->state = SPI_STATE_BUSY_TX;
    hspi->tx_remaining = size;
    hspi->single_transfer_mode = true;
    
    // Enable TXE interrupt to start transmission
    hspi->instance->CR2.fields.TXEIE = 1;
    
    return SPI_STATUS_OK;
}

/**
 * @brief Receive data in interrupt mode
 * @param hspi: SPI handle pointer
 * @param data: Buffer to store received data
 * @param size: Number of bytes to receive
 * @return spi_status_t: Operation status
 */
spi_status_t spi_receive_it(spi_handle_t *hspi, uint16_t expected_bytes)
{
    if (!hspi || expected_bytes == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }
    
    // Set up single transfer mode
    hspi->state = SPI_STATE_BUSY_RX;
    hspi->rx_expected = expected_bytes;
    hspi->tx_remaining = expected_bytes;  // Need to send dummy bytes
    hspi->single_transfer_mode = true;
    
    // Enable TXE and RXNE interrupts
    hspi->instance->CR2.fields.TXEIE = 1;
    hspi->instance->CR2.fields.RXNEIE = 1;
    
    return SPI_STATUS_OK;
}


/**
 * @brief Transmit and receive data simultaneously in interrupt mode
 * @param hspi: SPI handle pointer
 * @param tx_data: Data buffer to transmit
 * @param rx_data: Buffer to store received data
 * @param size: Number of bytes to transfer
 * @return spi_status_t: Operation status
 */
spi_status_t spi_transmit_receive_it(spi_handle_t *hspi, uint8_t *tx_data, uint16_t size)
{
    if (!hspi || !tx_data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }
    
    // Write data to transmit ring buffer
    for (uint16_t i = 0; i < size; i++) {
        if (!ring_buffer_write(hspi->tx_ring_buffer, tx_data[i])) {
            return SPI_STATUS_ERROR;  // Buffer full
        }
    }
    
    // Set up single transfer mode
    hspi->state = SPI_STATE_BUSY_TX_RX;
    hspi->tx_remaining = size;
    hspi->rx_expected = size;
    hspi->single_transfer_mode = true;
    
    // Enable TXE and RXNE interrupts
    hspi->instance->CR2.fields.TXEIE = 1;
    hspi->instance->CR2.fields.RXNEIE = 1;
    
    return SPI_STATUS_OK;
}

/**
 * @brief SPI interrupt service routine - State Machine Implementation
 * @param hspi: SPI handle pointer
 */
void spi_irq_handler(spi_handle_t *hspi)
{
    if (!hspi || !hspi->instance) {
        return;
    }
    
    // Handle error interrupts first - can occur in any state
    if (hspi->instance->CR2.fields.ERRIE && 
        (hspi->instance->SR.fields.MODF || hspi->instance->SR.fields.OVR || hspi->instance->SR.fields.CRCERR)) {
        
        // Clear error flags
        if (hspi->instance->SR.fields.OVR) {
            spi_clear_ovrflag(hspi->instance);
        }
        if (hspi->instance->SR.fields.MODF) {
            hspi->instance->CR1.fields.SPE = 0;  // Clear MODF by disabling SPI
            hspi->instance->CR1.fields.SPE = 1;  // Re-enable SPI
        }
        
        // Disable all interrupts and reset state
        hspi->instance->CR2.fields.TXEIE = 0;
        hspi->instance->CR2.fields.RXNEIE = 0;
        hspi->instance->CR2.fields.ERRIE = 0;
        hspi->state = SPI_STATE_ERROR;
        hspi->single_transfer_mode = false;
        hspi->tx_remaining = 0;
        hspi->rx_expected = 0;
        
        if (hspi->error_callback) {
            hspi->error_callback();
        }
        return;
    }
    
    // State machine for SPI interrupt handling
    switch (hspi->state) {
        
        case SPI_STATE_BUSY_TX:
        {
            // Handle TXE interrupt for transmit-only operations
            if (hspi->instance->CR2.fields.TXEIE && hspi->instance->SR.fields.TXE) {
                uint8_t tx_data;
                
                if (hspi->single_transfer_mode && hspi->tx_remaining > 0) {
                    // Single transfer mode - read from ring buffer
                    if (ring_buffer_read(hspi->tx_ring_buffer, &tx_data)) {
                        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                            *(volatile uint8_t *)&hspi->instance->DR = tx_data;
                        } else {
                            uint8_t tx_data_high = 0xFF;
                            ring_buffer_read(hspi->tx_ring_buffer, &tx_data_high);
                            hspi->instance->DR = (tx_data_high << 8) | tx_data;
                        }
                        hspi->tx_remaining--;
                    }
                    
                    // Check if transmission complete
                    if (hspi->tx_remaining == 0) {
                        hspi->instance->CR2.fields.TXEIE = 0;
                        hspi->state = SPI_STATE_READY;
                        hspi->single_transfer_mode = false;
                        
                        if (hspi->tx_complete_callback) {
                            hspi->tx_complete_callback();
                        }
                    }
                } else {
                    // Continuous mode - read from ring buffer if available
                    if (ring_buffer_read(hspi->tx_ring_buffer, &tx_data)) {
                        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                            *(volatile uint8_t *)&hspi->instance->DR = tx_data;
                        } else {
                            uint8_t tx_data_high = 0xFF;
                            ring_buffer_read(hspi->tx_ring_buffer, &tx_data_high);
                            hspi->instance->DR = (tx_data_high << 8) | tx_data;
                        }
                    } else {
                        // No data to send, disable TXE interrupt
                        hspi->instance->CR2.fields.TXEIE = 0;
                    }
                }
            }
            break;
        }
        
        case SPI_STATE_BUSY_RX:
        {
            // Handle TXE interrupt - send dummy bytes to generate clock
            if (hspi->instance->CR2.fields.TXEIE && hspi->instance->SR.fields.TXE) {
                if (hspi->single_transfer_mode && hspi->tx_remaining > 0) {
                    // Send dummy data to generate clock
                    if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                        *(volatile uint8_t *)&hspi->instance->DR = 0xFF;
                    } else {
                        hspi->instance->DR = 0xFFFF;
                    }
                    hspi->tx_remaining--;
                    
                    if (hspi->tx_remaining == 0) {
                        hspi->instance->CR2.fields.TXEIE = 0;
                    }
                } else {
                    // Continuous mode - send dummy byte to keep clock running
                    if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                        *(volatile uint8_t *)&hspi->instance->DR = 0xFF;
                    } else {
                        hspi->instance->DR = 0xFFFF;
                    }
                }
            }
            
            // Handle RXNE interrupt - receive data
            if (hspi->instance->CR2.fields.RXNEIE && hspi->instance->SR.fields.RXNE) {
                // Store received data in ring buffer
                if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                    uint8_t rx_data = *(volatile uint8_t *)&hspi->instance->DR;
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_data)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                } else {
                    uint16_t rx_data = hspi->instance->DR;
                    uint8_t rx_low = rx_data & 0xFF;
                    uint8_t rx_high = (rx_data >> 8) & 0xFF;
                    
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_low) ||
                        !ring_buffer_write(hspi->rx_ring_buffer, rx_high)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                }
                
                // Handle single transfer completion
                if (hspi->single_transfer_mode && hspi->rx_expected > 0) {
                    hspi->rx_expected--;
                    
                    if (hspi->rx_expected == 0) {
                        hspi->instance->CR2.fields.RXNEIE = 0;
                        hspi->state = SPI_STATE_READY;
                        hspi->single_transfer_mode = false;
                        
                        if (hspi->rx_complete_callback) {
                            hspi->rx_complete_callback();
                        }
                    }
                }
                
                // Trigger data ready callback
                if (hspi->rx_data_ready_callback) {
                    uint16_t available = ring_buffer_available(hspi->rx_ring_buffer);
                    hspi->rx_data_ready_callback(available);
                }
            }
            break;
        }
        
        case SPI_STATE_BUSY_TX_RX:
        {
            // Handle TXE interrupt - transmit data
            if (hspi->instance->CR2.fields.TXEIE && hspi->instance->SR.fields.TXE) {
                uint8_t tx_data;
                
                if (hspi->single_transfer_mode && hspi->tx_remaining > 0) {
                    // Single transfer mode - read from ring buffer
                    if (ring_buffer_read(hspi->tx_ring_buffer, &tx_data)) {
                        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                            *(volatile uint8_t *)&hspi->instance->DR = tx_data;
                        } else {
                            uint8_t tx_data_high = 0xFF;
                            ring_buffer_read(hspi->tx_ring_buffer, &tx_data_high);
                            hspi->instance->DR = (tx_data_high << 8) | tx_data;
                        }
                        hspi->tx_remaining--;
                    }
                    
                    if (hspi->tx_remaining == 0) {
                        hspi->instance->CR2.fields.TXEIE = 0;
                    }
                } else {
                    // Continuous mode
                    if (ring_buffer_read(hspi->tx_ring_buffer, &tx_data)) {
                        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                            *(volatile uint8_t *)&hspi->instance->DR = tx_data;
                        } else {
                            uint8_t tx_data_high = 0xFF;
                            ring_buffer_read(hspi->tx_ring_buffer, &tx_data_high);
                            hspi->instance->DR = (tx_data_high << 8) | tx_data;
                        }
                    } else {
                        hspi->instance->CR2.fields.TXEIE = 0;
                    }
                }
            }
            
            // Handle RXNE interrupt - receive data
            if (hspi->instance->CR2.fields.RXNEIE && hspi->instance->SR.fields.RXNE) {
                // Store received data in ring buffer
                if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                    uint8_t rx_data = *(volatile uint8_t *)&hspi->instance->DR;
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_data)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                } else {
                    uint16_t rx_data = hspi->instance->DR;
                    uint8_t rx_low = rx_data & 0xFF;
                    uint8_t rx_high = (rx_data >> 8) & 0xFF;
                    
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_low) ||
                        !ring_buffer_write(hspi->rx_ring_buffer, rx_high)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                }
                
                // Handle single transfer completion
                if (hspi->single_transfer_mode && hspi->rx_expected > 0) {
                    hspi->rx_expected--;
                    
                    if (hspi->rx_expected == 0 && hspi->tx_remaining == 0) {
                        hspi->instance->CR2.fields.RXNEIE = 0;
                        hspi->state = SPI_STATE_READY;
                        hspi->single_transfer_mode = false;
                        
                        if (hspi->tx_rx_complete_callback) {
                            hspi->tx_rx_complete_callback();
                        }
                    }
                }
                
                // Trigger data ready callback
                if (hspi->rx_data_ready_callback) {
                    uint16_t available = ring_buffer_available(hspi->rx_ring_buffer);
                    hspi->rx_data_ready_callback(available);
                }
            }
            break;
        }
        
        case SPI_STATE_READY:
        {
            // Continuous mode operations when driver is ready
            // Handle TXE interrupt for continuous transmission
            if (hspi->instance->CR2.fields.TXEIE && hspi->instance->SR.fields.TXE) {
                uint8_t tx_data;
                if (ring_buffer_read(hspi->tx_ring_buffer, &tx_data)) {
                    if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                        *(volatile uint8_t *)&hspi->instance->DR = tx_data;
                    } else {
                        uint8_t tx_data_high = 0xFF;
                        ring_buffer_read(hspi->tx_ring_buffer, &tx_data_high);
                        hspi->instance->DR = (tx_data_high << 8) | tx_data;
                    }
                } else {
                    // No data to send, disable TXE interrupt
                    hspi->instance->CR2.fields.TXEIE = 0;
                }
            }
            
            // Handle RXNE interrupt for continuous reception
            if (hspi->instance->CR2.fields.RXNEIE && hspi->instance->SR.fields.RXNE) {
                // Store received data in ring buffer
                if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                    uint8_t rx_data = *(volatile uint8_t *)&hspi->instance->DR;
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_data)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                } else {
                    uint16_t rx_data = hspi->instance->DR;
                    uint8_t rx_low = rx_data & 0xFF;
                    uint8_t rx_high = (rx_data >> 8) & 0xFF;
                    
                    if (!ring_buffer_write(hspi->rx_ring_buffer, rx_low) ||
                        !ring_buffer_write(hspi->rx_ring_buffer, rx_high)) {
                        if (hspi->error_callback) {
                            hspi->error_callback();
                        }
                    }
                }
                
                // Trigger data ready callback
                if (hspi->rx_data_ready_callback) {
                    uint16_t available = ring_buffer_available(hspi->rx_ring_buffer);
                    hspi->rx_data_ready_callback(available);
                }
                
                // Send dummy byte to keep continuous reception going
                if (hspi->instance->SR.fields.TXE) {
                    if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
                        *(volatile uint8_t *)&hspi->instance->DR = 0xFF;
                    } else {
                        hspi->instance->DR = 0xFFFF;
                    }
                }
            }
            break;
        }
        
        case SPI_STATE_RESET:
        case SPI_STATE_ERROR:
        default:
        {
            // Invalid states - disable all interrupts
            hspi->instance->CR2.fields.TXEIE = 0;
            hspi->instance->CR2.fields.RXNEIE = 0;
            hspi->instance->CR2.fields.ERRIE = 0;
            break;
        }
    }
}

/* ============================== Ring Buffer Functions ============================== */

/**
 * @brief Initialize ring buffers for continuous SPI operation
 * @param hspi: SPI handle pointer
 * @param tx_rb: Transmit ring buffer pointer (can be NULL)
 * @param rx_rb: Receive ring buffer pointer (can be NULL)
 * @return spi_status_t: Operation status
 */
spi_status_t spi_init_ring_buffers(spi_handle_t *hspi, ring_buffer_t *tx_rb, ring_buffer_t *rx_rb)
{
    if (!hspi) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    hspi->tx_ring_buffer = tx_rb;
    hspi->rx_ring_buffer = rx_rb;
    
    return SPI_STATUS_OK;
}

/**
 * @brief Start continuous receive operation using ring buffer
 * @param hspi: SPI handle pointer
 * @return spi_status_t: Operation status
 */
spi_status_t spi_start_continuous_receive(spi_handle_t *hspi)
{
    if (!hspi || !hspi->rx_ring_buffer) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }
    
    hspi->state = SPI_STATE_BUSY_RX;
    
    // Enable RXNE interrupt for continuous reception
    hspi->instance->CR2.fields.RXNEIE = 1;
    
    // Send first dummy byte to start clock
    if (hspi->instance->SR.fields.TXE) {
        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            *(volatile uint8_t *)&hspi->instance->DR = 0xFF;
        } else {
            hspi->instance->DR = 0xFFFF;
        }
    }
    
    return SPI_STATUS_OK;
}

/**
 * @brief Stop continuous receive operation
 * @param hspi: SPI handle pointer
 * @return spi_status_t: Operation status
 */
spi_status_t spi_stop_continuous_receive(spi_handle_t *hspi)
{
    if (!hspi) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    // Disable RXNE interrupt
    hspi->instance->CR2.fields.RXNEIE = 0;
    
    hspi->state = SPI_STATE_READY;
    return SPI_STATUS_OK;
}

/**
 * @brief Write data to transmit ring buffer
 * @param hspi: SPI handle pointer
 * @param data: Data to write
 * @param size: Number of bytes to write
 * @return spi_status_t: Operation status
 */
spi_status_t spi_write_ring_buffer(spi_handle_t *hspi, uint8_t *data, uint16_t size)
{
    if (!hspi || !hspi->tx_ring_buffer || !data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    // Write data to ring buffer
    for (uint16_t i = 0; i < size; i++) {
        if (!ring_buffer_write(hspi->tx_ring_buffer, data[i])) {
            return SPI_STATUS_ERROR;  // Buffer full
        }
    }
    
    // Enable TXE interrupt to start transmission
    hspi->instance->CR2.fields.TXEIE = 1;
    
    return SPI_STATUS_OK;
}

/**
 * @brief Read data from receive ring buffer
 * @param hspi: SPI handle pointer
 * @param data: Buffer to store read data
 * @param max_size: Maximum number of bytes to read
 * @return uint16_t: Number of bytes actually read
 */
uint16_t spi_read_ring_buffer(spi_handle_t *hspi, uint8_t *data, uint16_t max_size)
{
    if (!hspi || !hspi->rx_ring_buffer || !data) {
        return 0;
    }
    
    uint16_t bytes_read = 0;
    
    for (uint16_t i = 0; i < max_size; i++) {
        if (ring_buffer_read(hspi->rx_ring_buffer, &data[i])) {
            bytes_read++;
        } else {
            break;  // Buffer empty
        }
    }
    
    return bytes_read;
}

/**
 * @brief Get number of bytes available in receive ring buffer
 * @param hspi: SPI handle pointer
 * @return uint16_t: Number of available bytes
 */
uint16_t spi_get_rx_data_count(spi_handle_t *hspi)
{
    if (!hspi || !hspi->rx_ring_buffer) {
        return 0;
    }
    
    return ring_buffer_available(hspi->rx_ring_buffer);
}

/**
 * @brief Check if receive data is available
 * @param hspi: SPI handle pointer
 * @return bool: true if data available, false otherwise
 */
bool spi_is_rx_data_available(spi_handle_t *hspi)
{
    if (!hspi || !hspi->rx_ring_buffer) {
        return false;
    }
    
    return !ring_buffer_is_empty(hspi->rx_ring_buffer);
}
