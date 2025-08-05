/*
 * rtc_driver.h
 *
 *  Created on: Aug 4, 2025
 *      Author: Orr
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

#include "main.h"
#include <stdio.h>
#include <string.h>

// RTC Module Selection
#define RTC_DS3231    2
#define SELECTED_RTC  RTC_DS3231  // Change this to select RTC type

// I2C addresses for different RTC modules
#if SELECTED_RTC == RTC_DS3231
    #define RTC_I2C_ADDR    0x68
    #define RTC_REG_SEC     0x00
    #define RTC_REG_MIN     0x01
    #define RTC_REG_HOUR    0x02
    #define RTC_REG_DOW     0x03
    #define RTC_REG_DATE    0x04
    #define RTC_REG_MONTH   0x05
    #define RTC_REG_YEAR    0x06
#endif


typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} RTC_Time;


// BCD conversion functions
static inline uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

static inline uint8_t bcd_to_dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}


// External I2C and UART handles (defined in main.c)
extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;


// Function prototypes
uint8_t dec_to_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);
HAL_StatusTypeDef rtc_write_time(RTC_Time *time);
HAL_StatusTypeDef rtc_read_time(RTC_Time *time);
void rtc_init(void);
int parse_pc_time(char *cmd, RTC_Time *time);
void report_time_verification(RTC_Time *set_time, RTC_Time *read_time);
void rtc_protocol_main(void);

#endif /* RTC_DRIVER_H_ */
