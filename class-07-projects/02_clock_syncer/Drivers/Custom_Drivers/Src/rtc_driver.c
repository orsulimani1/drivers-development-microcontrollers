/* rtc_driver.c - DS1307 RTC Driver Implementation
 * For NUCLEO-L073RZ board
 * PC Clock synchronization with UART verification
 */

#include "rtc_driver.h"
#include <stdlib.h>
#include <string.h>

// Generic RTC write function
HAL_StatusTypeDef rtc_external_set_time(RTC_Time *time) {
    uint8_t data[8];

    data[0] = RTC_REG_SEC;
    data[1] = dec_to_bcd(time->seconds);
    data[2] = dec_to_bcd(time->minutes);
    data[3] = dec_to_bcd(time->hours);
    data[4] = dec_to_bcd(time->day_of_week);
    data[5] = dec_to_bcd(time->date);
    data[6] = dec_to_bcd(time->month);
    data[7] = dec_to_bcd(time->year);

    return HAL_I2C_Master_Transmit(&hi2c1, RTC_I2C_ADDR << 1, data, 8, 1000);
}

// Generic RTC read function
HAL_StatusTypeDef rtc_external_get_time(RTC_Time *time) {
    uint8_t data[7];
    uint8_t reg = RTC_REG_SEC;

    // Write register address
    if (HAL_I2C_Master_Transmit(&hi2c1, RTC_I2C_ADDR << 1, &reg, 1, 1000) != HAL_OK) {
        return HAL_ERROR;
    }

    // Read time data
    if (HAL_I2C_Master_Receive(&hi2c1, RTC_I2C_ADDR << 1, data, 7, 1000) != HAL_OK) {
        return HAL_ERROR;
    }

    time->seconds = bcd_to_dec(data[0] & 0x7F);  // Mask CH bit for DS1307
    time->minutes = bcd_to_dec(data[1]);
    time->hours = bcd_to_dec(data[2] & 0x3F);    // Mask 12/24 bit
    time->day_of_week = bcd_to_dec(data[3]);
    time->date = bcd_to_dec(data[4]);
    time->month = bcd_to_dec(data[5]);
    time->year = bcd_to_dec(data[6]);


    return HAL_OK;
}


// Request sync from PC
void request_sync(void) {
    char msg[] = "SYNC_REQUEST\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
}

// Initialize RTC module
void rtc_external_init(void) {
   RTC_Time current_time;
	char buffer[100];

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, 0x57 << 1, 3, 1000);
    sprintf(buffer, "I2C Device Ready: %d\r\n", status);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    // Read current time from RTC
    if (rtc_read_time(&current_time) == HAL_OK) {
        sprintf(buffer, "RTC Current Time: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
                current_time.year, current_time.month, current_time.date,
                current_time.hours, current_time.minutes, current_time.seconds);
    } else {
        sprintf(buffer, "RTC Read Error\r\n");
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
}


