/*
 * i2c_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#include "stm32f411_base.h"

/**
 * @defgroup I2C_Status_Definitions Status and Error Codes
 */

/**
 * @brief I2C status enumeration
 */
typedef enum {
    I2C_STATUS_OK       = 0x00,    ///< Operation completed successfully
    I2C_STATUS_ERROR    = 0x01,    ///< Generic error occurred
    I2C_STATUS_BUSY     = 0x02,    ///< Peripheral is busy
    I2C_STATUS_TIMEOUT  = 0x03,    ///< Operation timed out
    I2C_STATUS_NACK     = 0x04,    ///< Slave sent NACK (not acknowledge)
    I2C_STATUS_BERR     = 0x05,    ///< Bus error occurred
    I2C_STATUS_ARLO     = 0x06,    ///< Arbitration lost (multi-master)
    I2C_STATUS_OVR      = 0x07,    ///< Overrun/underrun error
    I2C_STATUS_INVALID  = 0x08     ///< Invalid parameter
} i2c_status_t;


/**
 * @brief I2C slave operation states
 */
typedef enum {
    I2C_SLAVE_STATE_IDLE            = 0x00,    ///< Slave idle, waiting for address match
    I2C_SLAVE_STATE_ADDR_MATCHED    = 0x01,    ///< Address matched, ready for data
    I2C_SLAVE_STATE_RECEIVING       = 0x02,    ///< Receiving data from master
    I2C_SLAVE_STATE_TRANSMITTING    = 0x03,    ///< Transmitting data to master
    I2C_SLAVE_STATE_ERROR           = 0x04     ///< Error state
} i2c_slave_state_t;
/**
 * @brief I2C operating mode
 */
typedef enum {
    I2C_MODE_MASTER     = 0x00,    ///< Master mode operation
    I2C_MODE_SLAVE      = 0x01     ///< Slave mode operation
} i2c_mode_t;

/**
 * @brief I2C slave device types for simulation
 */
typedef enum {
    I2C_SLAVE_DEVICE_MEMORY         = 0x00,    ///< Memory device (EEPROM-like)
    I2C_SLAVE_DEVICE_REGISTER_MAP   = 0x01,    ///< Register-based device
    I2C_SLAVE_DEVICE_ECHO           = 0x02     ///< Simple echo device
} i2c_slave_device_type_t;

/**
 * @brief I2C slave operation states
 */
typedef enum {
    I2C_SLAVE_STATE_IDLE            = 0x00,    ///< Slave idle, waiting for address match
    I2C_SLAVE_STATE_ADDR_MATCHED    = 0x01,    ///< Address matched, ready for data
    I2C_SLAVE_STATE_RECEIVING       = 0x02,    ///< Receiving data from master
    I2C_SLAVE_STATE_TRANSMITTING    = 0x03,    ///< Transmitting data to master
    I2C_SLAVE_STATE_ERROR           = 0x04     ///< Error state
} i2c_slave_state_t;

/**
 * @brief I2C duty cycle configuration (Fast mode only)
 */
typedef enum {
    I2C_DUTY_CYCLE_2    = 0x00,    ///< Standard duty cycle (tlow/thigh = 2)
    I2C_DUTY_CYCLE_16_9 = 0x01     ///< Fast mode duty cycle (tlow/thigh = 16/9)
} i2c_duty_cycle_t;

/**
 * @brief I2C acknowledge configuration
 */
typedef enum {
    I2C_ACK_DISABLE     = 0x00,    ///< Disable automatic ACK generation
    I2C_ACK_ENABLE      = 0x01     ///< Enable automatic ACK generation
} i2c_ack_t;

/**
 * @brief I2C clock stretching configuration
 */
typedef enum {
    I2C_STRETCH_ENABLE  = 0x00,    ///< Enable clock stretching
    I2C_STRETCH_DISABLE = 0x01     ///< Disable clock stretching (master mode only)
} i2c_stretch_t;

/**
 * @brief I2C transfer direction
 */
typedef enum {
    I2C_DIRECTION_WRITE = 0x00,    ///< Write operation (master to slave)
    I2C_DIRECTION_READ  = 0x01     ///< Read operation (slave to master)
} i2c_direction_t;

/**
 * @brief I2C addressing mode
 */
typedef enum {
    I2C_ADDR_7BIT       = 0x00,    ///< 7-bit addressing mode
    I2C_ADDR_10BIT      = 0x01     ///< 10-bit addressing mode
} i2c_addr_mode_t;



/**
 * @brief I2C initialization configuration structure
 */
typedef struct {
    gpio_config_t       I2C_SDA;           ///< SDA pin GPIO configuration
    gpio_config_t       I2C_SCL;           ///< SCL pin GPIO configuration
    uint32_t            clock_speed;       ///< SCL frequency in Hz (100000 or 400000)
    i2c_mode_t          mode;              ///< Master or slave mode
    i2c_duty_cycle_t    duty_cycle;        ///< SCL duty cycle (fast mode only)
    uint16_t            own_address;       ///< Device own address (slave mode)
    i2c_addr_mode_t     addr_mode;         ///< 7-bit or 10-bit addressing
    i2c_ack_t           ack_enable;        ///< ACK generation enable/disable
    i2c_stretch_t       no_stretch;        ///< Clock stretching enable/disable
} i2c_config_t;

/**
 * @brief I2C slave context structure
 */
typedef struct {
    i2c_slave_state_t   state;              ///< Current slave state
    uint8_t             address;            ///< Slave address
    uint8_t             rx_buffer[64];      ///< Receive buffer
    uint8_t             tx_buffer[64];      ///< Transmit buffer
    uint16_t            rx_index;           ///< Receive buffer index
    uint16_t            tx_index;           ///< Transmit buffer index
    uint16_t            rx_size;            ///< Received data size
    uint16_t            tx_size;            ///< Data to transmit size
    uint16_t            memory_pointer;     ///< Memory device pointer
    uint8_t             register_map[32];   ///< Register map for register device
    uint8_t                first_byte_received; ///< Flag for register address
    uint8_t                is_transmit_mode;   ///< True if master is reading from slave
} i2c_slave_context_t;

/**
 * @defgroup I2C_Flag_Definitions Register Flag Definitions
 */

// SR1 register flags
#define I2C_SR1_SB              0x00000001U    ///< Start bit flag
#define I2C_SR1_ADDR            0x00000002U    ///< Address sent/matched flag
#define I2C_SR1_BTF             0x00000004U    ///< Byte transfer finished flag
#define I2C_SR1_ADD10           0x00000008U    ///< 10-bit header sent flag
#define I2C_SR1_STOPF           0x00000010U    ///< Stop detection flag
#define I2C_SR1_RXNE            0x00000040U    ///< Data register not empty flag
#define I2C_SR1_TXE             0x00000080U    ///< Data register empty flag
#define I2C_SR1_BERR            0x00000100U    ///< Bus error flag
#define I2C_SR1_ARLO            0x00000200U    ///< Arbitration lost flag
#define I2C_SR1_AF              0x00000400U    ///< Acknowledge failure flag
#define I2C_SR1_OVR             0x00000800U    ///< Overrun/underrun flag
#define I2C_SR1_PECERR          0x00001000U    ///< PEC error flag
#define I2C_SR1_TIMEOUT         0x00004000U    ///< Timeout error flag
#define I2C_SR1_SMBALERT        0x00008000U    ///< SMBus alert flag

// SR2 register flags
#define I2C_SR2_MSL             0x00000001U    ///< Master/slave flag
#define I2C_SR2_BUSY            0x00000002U    ///< Bus busy flag
#define I2C_SR2_TRA             0x00000004U    ///< Transmitter/receiver flag


// Initialization and configuration
i2c_status_t i2c_init(I2C_TypeDef *i2c, const i2c_config_t *config);
i2c_status_t i2c_deinit(I2C_TypeDef *i2c);

// Master mode operations
i2c_status_t i2c_master_transmit(I2C_TypeDef *i2c, uint8_t slave_addr,
                                 const uint8_t *data, uint16_t length,
                                 uint32_t timeout);
i2c_status_t i2c_master_receive(I2C_TypeDef *i2c, uint8_t slave_addr,
                                uint8_t *data, uint16_t length,
                                uint32_t timeout);
i2c_status_t i2c_master_write_read(I2C_TypeDef *i2c, uint8_t slave_addr,
                                   const uint8_t *write_data, uint16_t write_len,
                                   uint8_t *read_data, uint16_t read_len,
                                   uint32_t timeout);


#endif /* I2C_DRIVER_H_ */
