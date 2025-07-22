/*
 * spi_common.h
 *
 *  Created on: Jul 15, 2025
 *      Author: Orr
 */

#ifndef INC_SPI_COMMON_H_
#define INC_SPI_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f411_base.h"
#include "ring_buffer.h"
#include "gpio_driver.h"




/**
 * @brief SPI Status Codes
 */
typedef enum {
    SPI_STATUS_OK = 0,
    SPI_STATUS_ERROR,
    SPI_STATUS_BUSY,
    SPI_STATUS_TIMEOUT,
    SPI_STATUS_INVALID_PARAM,
    SPI_STATUS_NOT_INITIALIZED
} spi_status_t;

/**
 * @brief SPI Clock Prescaler
 */
typedef enum {
    SPI_PRESCALER_2 = 0,    // fPCLK/2
    SPI_PRESCALER_4,        // fPCLK/4  
    SPI_PRESCALER_8,        // fPCLK/8
    SPI_PRESCALER_16,       // fPCLK/16
    SPI_PRESCALER_32,       // fPCLK/32
    SPI_PRESCALER_64,       // fPCLK/64
    SPI_PRESCALER_128,      // fPCLK/128
    SPI_PRESCALER_256       // fPCLK/256
} spi_prescaler_t;

/**
 * @brief SPI Clock Polarity
 */
typedef enum {
    SPI_CPOL_LOW = 0,       // Clock idle state is LOW
    SPI_CPOL_HIGH           // Clock idle state is HIGH
} spi_cpol_t;

/**
 * @brief SPI Clock Phase
 */
typedef enum {
    SPI_CPHA_1EDGE = 0,     // Data sampled on first clock edge
    SPI_CPHA_2EDGE          // Data sampled on second clock edge
} spi_cpha_t;

/**
 * @brief SPI Data Size
 */
typedef enum {
    SPI_DATASIZE_8BIT = 0,  // 8-bit data frame
    SPI_DATASIZE_16BIT      // 16-bit data frame
} spi_datasize_t;

/**
 * @brief SPI Bit Order
 */
typedef enum {
    SPI_FIRSTBIT_MSB = 0,   // MSB transmitted first
    SPI_FIRSTBIT_LSB        // LSB transmitted first
} spi_firstbit_t;

/**
 * @brief SPI NSS Management
 */
typedef enum {
    SPI_NSS_SOFT = 0,       // Software NSS management
    SPI_NSS_HARD_INPUT,     // Hardware NSS input
    SPI_NSS_HARD_OUTPUT     // Hardware NSS output
} spi_nss_t;

/**
 * @brief SPI Driver State
 */
typedef enum {
    SPI_STATE_RESET = 0,
    SPI_STATE_READY,
    SPI_STATE_BUSY_TX,
    SPI_STATE_BUSY_RX,
    SPI_STATE_BUSY_TX_RX,
    SPI_STATE_ERROR
} spi_state_t;

/**
 * @brief SPI Pin Configuration
 */
typedef struct {
    GPIO_TypeDef *sclk_port;
    uint8_t sclk_pin;
    uint8_t sclk_af;
    
    GPIO_TypeDef *mosi_port;
    uint8_t mosi_pin;
    uint8_t mosi_af;
    
    GPIO_TypeDef *miso_port;
    uint8_t miso_pin;
    uint8_t miso_af;
    
    GPIO_TypeDef *cs_port;      // For software CS
    uint8_t cs_pin;             // For software CS
} spi_pins_t;

/**
 * @brief SPI Configuration Structure
 */
typedef struct {
    SPI_TypeDef *instance;      // SPI1, SPI2, or SPI3
    spi_prescaler_t prescaler;  // Clock prescaler
    spi_cpol_t cpol;            // Clock polarity
    spi_cpha_t cpha;            // Clock phase
    spi_datasize_t datasize;    // Data frame size
    spi_firstbit_t firstbit;    // Bit order
    spi_nss_t nss;              // NSS management
    spi_pins_t pins;            // Pin configuration
} spi_config_t;


#define SPI_CR1_BR_Pos              3
#define SPI_CR1_BR                  (0x7UL << SPI_CR1_BR_Pos)

/**
 * @brief SPI Handle Structure
 */
typedef struct {
    SPI_TypeDef *instance;
    spi_config_t config;
    spi_state_t state;
    
    // Ring buffer integration for all operations
    ring_buffer_t *tx_ring_buffer;  // Transmit ring buffer
    ring_buffer_t *rx_ring_buffer;  // Receive ring buffer
    
    // Transfer tracking for single operations
    uint16_t tx_remaining;          // Bytes remaining to transmit
    uint16_t rx_expected;           // Bytes expected to receive
    bool single_transfer_mode;      // Flag for single transfer vs continuous
    
    // Callback functions
    void (*tx_complete_callback)(void);
    void (*rx_complete_callback)(void);
    void (*tx_rx_complete_callback)(void);
    void (*error_callback)(void);
    void (*rx_data_ready_callback)(uint16_t available_bytes);  // Called when data arrives
} spi_handle_t;

// Common Functions
spi_status_t spi_init(spi_handle_t *hspi, const spi_config_t *config, ring_buffer_t *tx_rb, ring_buffer_t *rx_rb);
spi_status_t spi_deinit(spi_handle_t *hspi);
void spi_set_cs(spi_handle_t *hspi, bool state);
spi_state_t spi_get_state(spi_handle_t *hspi);

#endif /* INC_SPI_COMMON_H_ */
