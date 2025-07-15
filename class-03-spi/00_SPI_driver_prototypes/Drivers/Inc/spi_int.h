/*
 * spi_int.h
 *
 *  Created on: Jul 15, 2025
 *      Author: Orr
 */

#ifndef INC_SPI_INT_H_
#define INC_SPI_INT_H_

#include "spi_common.h"

// Interrupt Functions (using ring buffers)
spi_status_t spi_transmit_it(spi_handle_t *hspi, uint8_t *data, uint16_t size);
spi_status_t spi_receive_it(spi_handle_t *hspi, uint16_t expected_bytes);
spi_status_t spi_transmit_receive_it(spi_handle_t *hspi, uint8_t *tx_data, uint16_t size);


#endif /* INC_SPI_INT_H_ */
