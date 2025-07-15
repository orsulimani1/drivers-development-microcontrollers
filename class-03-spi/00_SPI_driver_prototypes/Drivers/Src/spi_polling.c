/*
 * spi_polling.h
 *
 *  Created on: Jul 15, 2025
 *      Author: Orr
 */
#include "spi_polling.h"

extern void spi_clear_ovrflag(SPI_TypeDef *spi);


/**
 * @brief Transmit data in polling mode
 * @param hspi: SPI handle pointer
 * @param data: Data buffer to transmit
 * @param size: Number of bytes to transmit
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_transmit_polling(spi_handle_t *hspi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (!hspi || !data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }

    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }

    hspi->state = SPI_STATE_BUSY_TX;

    for (uint16_t i = 0; i < size; i++) {
        // Wait for TXE flag (transmit buffer empty)
        spi_status_t status = spi_wait_flag_txe(hspi->instance, timeout);
        if (status != SPI_STATUS_OK) {
            hspi->state = SPI_STATE_READY;
            return status;
        }

        // Send data
        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            *(volatile uint8_t *)&hspi->instance->DR = data[i];
        } else {
            hspi->instance->DR = ((uint16_t*)data)[i];
        }
    }

    // Wait for transmission complete (TXE and not BSY)
    spi_status_t status = spi_wait_flag_txe(hspi->instance, timeout);
    if (status != SPI_STATUS_OK) {
        hspi->state = SPI_STATE_READY;
        return status;
    }

    status = spi_wait_flag_bsy(hspi->instance, false, timeout);
    if (status != SPI_STATUS_OK) {
        hspi->state = SPI_STATE_READY;
        return status;
    }

    // Clear overrun flag by reading DR and SR
    spi_clear_ovrflag(hspi->instance);

    hspi->state = SPI_STATE_READY;
    return SPI_STATUS_OK;
}

/**
 * @brief Receive data in polling mode
 * @param hspi: SPI handle pointer
 * @param data: Buffer to store received data
 * @param size: Number of bytes to receive
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_receive_polling(spi_handle_t *hspi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (!hspi || !data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }

    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }

    hspi->state = SPI_STATE_BUSY_RX;

    for (uint16_t i = 0; i < size; i++) {
        // Send dummy data to generate clock
        spi_status_t status = spi_wait_flag_txe(hspi->instance, timeout);
        if (status != SPI_STATUS_OK) {
            hspi->state = SPI_STATE_READY;
            return status;
        }

        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            *(volatile uint8_t *)&hspi->instance->DR = 0xFF;  // Dummy byte
        } else {
            hspi->instance->DR = 0xFFFF;  // Dummy word
        }

        // Wait for RXNE flag (receive buffer not empty)
        status = spi_wait_flag_rxne(hspi->instance, timeout);
        if (status != SPI_STATUS_OK) {
            hspi->state = SPI_STATE_READY;
            return status;
        }

        // Read received data
        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            data[i] = *(volatile uint8_t *)&hspi->instance->DR;
        } else {
            ((uint16_t*)data)[i] = hspi->instance->DR;
        }
    }

    hspi->state = SPI_STATE_READY;
    return SPI_STATUS_OK;
}

/**
 * @brief Transmit and receive data simultaneously in polling mode
 * @param hspi: SPI handle pointer
 * @param tx_data: Data buffer to transmit
 * @param rx_data: Buffer to store received data
 * @param size: Number of bytes to transfer
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_transmit_receive_polling(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout)
{
    if (!hspi || !tx_data || !rx_data || size == 0) {
        return SPI_STATUS_INVALID_PARAM;
    }

    if (hspi->state != SPI_STATE_READY) {
        return SPI_STATUS_BUSY;
    }

    hspi->state = SPI_STATE_BUSY_TX_RX;

    for (uint16_t i = 0; i < size; i++) {
        // Wait for TXE flag
        spi_status_t status = spi_wait_flag_txe(hspi->instance, timeout);
        if (status != SPI_STATUS_OK) {
            hspi->state = SPI_STATE_READY;
            return status;
        }

        // Send data
        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            *(volatile uint8_t *)&hspi->instance->DR = tx_data[i];
        } else {
            hspi->instance->DR = ((uint16_t*)tx_data)[i];
        }

        // Wait for RXNE flag
        status = spi_wait_flag_rxne(hspi->instance, timeout);
        if (status != SPI_STATUS_OK) {
            hspi->state = SPI_STATE_READY;
            return status;
        }

        // Read received data
        if (hspi->config.datasize == SPI_DATASIZE_8BIT) {
            rx_data[i] = *(volatile uint8_t *)&hspi->instance->DR;
        } else {
            ((uint16_t*)rx_data)[i] = hspi->instance->DR;
        }
    }

    hspi->state = SPI_STATE_READY;
    return SPI_STATUS_OK;
}



/**
 * @brief Wait for TXE flag with timeout
 * @param spi: SPI peripheral pointer
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_wait_flag_txe(SPI_TypeDef *spi, uint32_t timeout)
{
    while (timeout > 0) {
        if (spi->SR.fields.TXE) {
            return SPI_STATUS_OK;
        }

        // Decrement timeout (simplified - in real implementation use proper timing)
        timeout--;

        // Simple delay (in real implementation use proper delay function)
        for (volatile int i = 0; i < 1000; i++);
    }

    return SPI_STATUS_TIMEOUT;
}

/**
 * @brief Wait for RXNE flag with timeout
 * @param spi: SPI peripheral pointer
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_wait_flag_rxne(SPI_TypeDef *spi, uint32_t timeout)
{

    while (timeout > 0) {
        if (spi->SR.fields.RXNE) {
            return SPI_STATUS_OK;
        }

        // Decrement timeout (simplified - in real implementation use proper timing)
        timeout--;

        // Simple delay (in real implementation use proper delay function)
        for (volatile int i = 0; i < 1000; i++);
    }

    return SPI_STATUS_TIMEOUT;
}

/**
 * @brief Wait for BSY flag state with timeout
 * @param spi: SPI peripheral pointer
 * @param state: Desired flag state (true = busy, false = not busy)
 * @param timeout: Timeout in milliseconds
 * @return spi_status_t: Operation status
 */
spi_status_t spi_wait_flag_bsy(SPI_TypeDef *spi, bool state, uint32_t timeout)
{

    while (timeout > 0) {
        bool busy_status = spi->SR.fields.BSY;
        if (busy_status == state) {
            return SPI_STATUS_OK;
        }

        // Decrement timeout (simplified - in real implementation use proper timing)
        timeout--;

        // Simple delay (in real implementation use proper delay function)
        for (volatile int i = 0; i < 1000; i++);
    }

    return SPI_STATUS_TIMEOUT;
}


