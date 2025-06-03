/*
 * rs485_driver.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Orr
 */

#ifndef RS485_DRIVER_H_
#define RS485_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

// Driver return codes
typedef enum {
    RS485_OK = 0,
    RS485_ERROR_INVALID_PARAM,
    RS485_ERROR_TIMEOUT,
    RS485_ERROR_BUSY,
    RS485_ERROR_NOT_INITIALIZED
} rs485_status_t;

// Driver configuration structure
typedef struct {
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    bool parity_enable;
    uint32_t timeout_ms;
    void (*tx_complete_callback)(void);
    void (*rx_complete_callback)(uint8_t *data, uint16_t length);
} rs485_config_t;


// Driver API functions

// Initializes the RS485 driver with specified configuration parameters and callback functions.
rs485_status_t rs485_init(const rs485_config_t *config);
//Deinitializes the RS485 driver, disabling peripherals and freeing allocated resources.
rs485_status_t rs485_deinit(void);
// Transmits data over the RS485 bus by switching to transmit mode, sending the specified bytes, then returning to receive mode.
rs485_status_t rs485_transmit(const uint8_t *data, uint16_t length);
// Receives data from the RS485 bus into the provided buffer with specified maximum length.
rs485_status_t rs485_receive(uint8_t *buffer, uint16_t max_length);
// Controls the RS485 transceiver direction by setting the driver enable pin for either transmit or receive mode.
rs485_status_t rs485_set_mode(bool transmit_mode);
// Returns the current busy status of the driver to check if a transmission or reception operation is in progress.
bool rs485_is_busy(void);


#endif /* RS485_DRIVER_H_ */
