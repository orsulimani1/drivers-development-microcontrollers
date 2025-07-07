/*
 * i2c_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef I2C_POLLING_DRIVER_H_
#define I2C_POLLING_DRIVER_H_

#include "stm32f411_base.h"
#include "i2c_common_driver.h"



// Master mode operations
i2c_status_t i2c_master_transmit(i2c_config_t *i2c, uint8_t slave_addr,
                                 const uint8_t *data, uint16_t length,
                                 uint32_t timeout);
i2c_status_t i2c_master_receive(i2c_config_t *i2c, uint8_t slave_addr,
                                uint8_t *data, uint16_t length,
                                uint32_t timeout);
i2c_status_t i2c_master_write_read(i2c_config_t *i2c, uint8_t slave_addr,
                                   const uint8_t *write_data, uint16_t write_len,
                                   uint8_t *read_data, uint16_t read_len,
                                   uint32_t timeout);


#endif /* I2C_DRIVER_H_ */
