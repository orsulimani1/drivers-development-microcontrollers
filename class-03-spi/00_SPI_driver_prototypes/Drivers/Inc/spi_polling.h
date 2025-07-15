/*
 * spi_polling.h
 *
 *  Created on: Jul 15, 2025
 *      Author: Orr
 */

#ifndef INC_SPI_POLLING_H_
#define INC_SPI_POLLING_H_
#include "spi_common.h"


// Polling Functions
spi_status_t spi_transmit_polling(spi_handle_t *hspi, uint8_t *data, uint16_t size, uint32_t timeout);
spi_status_t spi_receive_polling(spi_handle_t *hspi, uint8_t *data, uint16_t size, uint32_t timeout);
spi_status_t spi_transmit_receive_polling(spi_handle_t *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout);


spi_status_t spi_wait_flag_txe(SPI_TypeDef *spi, uint32_t timeout);
spi_status_t spi_wait_flag_rxne(SPI_TypeDef *spi, uint32_t timeout);
spi_status_t spi_wait_flag_bsy(SPI_TypeDef *spi, bool state, uint32_t timeout);


#endif //INC_SPI_POLLING_H_
