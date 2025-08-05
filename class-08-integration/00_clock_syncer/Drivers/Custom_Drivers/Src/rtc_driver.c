/* rtc_driver.c - DS1307 RTC Driver Implementation
 * For NUCLEO-L073RZ board
 * PC Clock synchronization with UART verification
 */

#include "rtc_driver.h"
#include <stdlib.h>
#include <string.h>

// Protocol states
typedef enum {
    RTC_STATE_INIT,
    RTC_STATE_SEND_TIME,
    RTC_STATE_WAIT_VALIDATION,
    RTC_STATE_WAIT_SYNC,
    RTC_STATE_RUNNING
} rtc_protocol_state_t;

// Time validation function
int is_valid_time(RTC_Time *time) {
    // Check if time is reasonable (not default/corrupted values)
    if (time->year ) return 0;  // 2025-2099
    if (time->month ) return 0;
    if (time->date ) return 0;
    if (time->hours) return 0;
    if (time->minutes) return 0;
    if (time->seconds) return 0;
    return 1;
}

// Generic RTC write function
HAL_StatusTypeDef rtc_write_time(RTC_Time *time) {
    uint8_t data[8] = {0};

    return HAL_I2C_Master_Transmit(NULL, 0, data, 0, 0);
}

// Generic RTC read function
HAL_StatusTypeDef rtc_read_time(RTC_Time *time) {
    uint8_t data[7];
    uint8_t reg = RTC_REG_SEC;

    // Write register address
    if (HAL_I2C_Master_Transmit()) {
        return HAL_ERROR;
    }

    // Read time data
    if (HAL_I2C_Master_Receive()) {
        return HAL_ERROR;
    }

//    time->seconds =  // Mask CH bit for DS1307
//    time->minutes =
//    time->hours =   // Mask 12/24 bit
//    time->day_of_week =
//    time->date =
//    time->month =
//    time->year =


    return HAL_OK;
}

// Send RTC time to PC
void send_rtc_time(RTC_Time *time) {
    char buffer[100];
    sprintf(buffer, "RTC_TIME: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            time->year, time->month, time->date,
            time->hours, time->minutes, time->seconds);
}

// Request sync from PC
void request_sync(void) {
    char msg[] = "SYNC_REQUEST\r\n";
}

// Initialize RTC module
void rtc_init(void) {
   RTC_Time current_time;
	char buffer[100];

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, 0x57 << 1, 3, 1000);
    sprintf(buffer, "I2C Device Ready: %d\r\n", status);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    // Read current time from RTC
//    send result over uart
}

// Parse time from PC command: "SYNC:YYYY-MM-DD:HH:MM:SS"
int parse_pc_time(char *cmd, RTC_Time *time) {
    if (strncmp(cmd, "SYNC:", 5) != 0) return 0;

    return 0;
}

// Report time verification over UART
void report_time_verification(RTC_Time *set_time, RTC_Time *read_time) {
    char buffer[200];

    sprintf(buffer, "SET: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            set_time->year, set_time->month, set_time->date,
            set_time->hours, set_time->minutes, set_time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    sprintf(buffer, "READ: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            read_time->year, read_time->month, read_time->date,
            read_time->hours, read_time->minutes, read_time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    // Check if times match (allowing 1-2 second difference)
    int time_diff = abs((set_time->hours * 3600 + set_time->minutes * 60 + set_time->seconds) -
                       (read_time->hours * 3600 + read_time->minutes * 60 + read_time->seconds));

    if (time_diff <= 2) {
        sprintf(buffer, "VERIFY: OK (diff: %d sec)\r\n", time_diff);
    } else {
        sprintf(buffer, "VERIFY: FAIL (diff: %d sec)\r\n", time_diff);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 1000);
}


// Main RTC protocol state machine
void rtc_protocol_main(void) {
    rtc_protocol_state_t state = RTC_STATE_INIT;
    RTC_Time current_time, set_time;
    char rx_buffer[64];
    char tx_buffer[100];
    uint32_t last_time_report = 0;

    while (1) {
        switch (state) {
            case RTC_STATE_INIT:

                state = RTC_STATE_INIT;
                break;

            case RTC_STATE_SEND_TIME:
				state = RTC_STATE_SEND_TIME;
                break;

            case RTC_STATE_WAIT_VALIDATION:
				state = RTC_STATE_WAIT_VALIDATION;

                break;

            case RTC_STATE_WAIT_SYNC:
				state = RTC_STATE_WAIT_SYNC;
                break;

            case RTC_STATE_RUNNING:
				state = RTC_STATE_RUNNING;
                break;
        }

        HAL_Delay(10);  // Small delay to prevent busy loop
    }
}
