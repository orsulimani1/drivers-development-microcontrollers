/*
 * rtc_driver.h
 *
 *  Created on: Aug 4, 2025
 *      Author: Orr
 */

#ifndef RTC_DRIVER_API_H_
#define RTC_DRIVER_API_H_
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// RTC Module Selection
#define RTC_INTERNAL  1
#define RTC_DS3231    2
#define SELECTED_RTC  RTC_INTERNAL  // Change this to select RTC type



// I2C addresses for different RTC modules RTC_DS3231
#define RTC_I2C_ADDR    0x68
#define RTC_REG_SEC     0x00
#define RTC_REG_MIN     0x01
#define RTC_REG_HOUR    0x02
#define RTC_REG_DOW     0x03
#define RTC_REG_DATE    0x04
#define RTC_REG_MONTH   0x05
#define RTC_REG_YEAR    0x06



typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week; // 1-7 (1=Monday)
    uint8_t date;
    uint8_t month;
    uint8_t year;
} RTC_Time;

// Protocol states
typedef enum {
    RTC_STATE_INIT,
    RTC_STATE_SEND_TIME,
    RTC_STATE_WAIT_VALIDATION,
    RTC_STATE_WAIT_SYNC,
    RTC_STATE_RUNNING
} rtc_protocol_state_t;

// Time validation function
static inline int is_valid_time(RTC_Time *time) {
    // Check if time is reasonable (not default/corrupted values)
    if (time->year < 25 || time->year > 99) return 0;  // 2025-2099
    if (time->month < 1 || time->month > 12) return 0;
    if (time->date < 1 || time->date > 31) return 0;
    if (time->hours > 23) return 0;
    if (time->minutes > 59) return 0;
    if (time->seconds > 59) return 0;
    return 1;
}

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
extern RTC_HandleTypeDef hrtc;

// Function prototypes
HAL_StatusTypeDef rtc_write_time(RTC_Time *time);
HAL_StatusTypeDef rtc_read_time(RTC_Time *time);
void rtc_init(void);
int parse_pc_time(char *cmd, RTC_Time *time);
void report_time_verification(RTC_Time *set_time, RTC_Time *read_time);
void rtc_protocol_main(void);
#endif // RTC_DRIVER_API_H_
