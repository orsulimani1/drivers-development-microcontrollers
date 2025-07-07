/*
 * i2c_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef I2C_COMMON_DRIVER_H_
#define I2C_COMMON_DRIVER_H_

#include "stm32f411_base.h"
#include "gpio_driver.h"

// Default timeout value
#define I2C_TIMEOUT_DEFAULT     10000

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
    gpio_config_t       *i2c_sda_gpio;     ///< SDA pin GPIO configuration
    gpio_config_t       *i2c_scl_gpio;     ///< SCL pin GPIO configuration
    I2C_TypeDef         i2c_config;       /// i2c config struct
    uint32_t            clock_speed;      ///< SCL frequency in Hz (100000 or 400000)
    i2c_duty_cycle_t    duty_cycle;       ///< SCL duty cycle (fast mode only)
    i2c_addr_mode_t     addr_mode;        ///< 7-bit or 10-bit addressing
    i2c_ack_t           ack_enable;       ///< ACK generation enable/disable
    i2c_stretch_t       no_stretch;       ///< Clock stretching enable/disable
} i2c_config_t;


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
i2c_status_t i2c_init(const i2c_config_t *i2c);
i2c_status_t i2c_deinit(i2c_config_t *i2c);


#endif /* I2C_DRIVER_H_ */
