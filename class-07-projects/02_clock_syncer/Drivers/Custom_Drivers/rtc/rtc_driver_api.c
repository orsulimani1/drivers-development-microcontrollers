/*
 * rtc_driver.h
 *
 *  Created on: Aug 4, 2025
 *      Author: Orr
 */

#include "rtc_driver_api.h"

#if SELECTED_RTC == RTC_DS3231
#include "rtc_driver.h"
#elif SELECTED_RTC == RTC_INTERNAL
#include "rtc_internal.h"
#else
#error "Invalid RTC selection"
#endif


HAL_StatusTypeDef rtc_write_time(RTC_Time *time) {
#if SELECTED_RTC == RTC_DS3231
    return rtc_external_set_time(time);
#elif SELECTED_RTC == RTC_INTERNAL
    return rtc_internal_set_time(time);
#endif
}

HAL_StatusTypeDef rtc_read_time(RTC_Time *time) {
#if SELECTED_RTC == RTC_DS3231
    return rtc_external_get_time(time);
#elif SELECTED_RTC == RTC_INTERNAL
    return rtc_internal_get_time(time);
#endif
}


void rtc_init(void) {
#if SELECTED_RTC == RTC_DS3231
    rtc_external_init();
#elif SELECTED_RTC == RTC_INTERNAL
    rtc_internal_init();
#endif
}

int parse_pc_time(char *cmd, RTC_Time *time) {
    if (strncmp(cmd, "SYNC:", 5) != 0) return 0;

    int year, month, date, hours, minutes, seconds;
    if (sscanf(cmd + 5, "%d-%d-%d:%d:%d:%d",
               &year, &month, &date, &hours, &minutes, &seconds) == 6) {

        time->year = year - 2000;
        time->month = month;
        time->date = date;
        time->hours = hours;
        time->minutes = minutes;
        time->seconds = seconds;
        time->day_of_week = 1;

        return 1;
    }
    return 0;
}

// ========== PROTOCOL FUNCTIONS ==========
void send_rtc_time(RTC_Time *time) {
    char buffer[100];
    sprintf(buffer, "RTC_TIME: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            time->year, time->month, time->date,
            time->hours, time->minutes, time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 200);
}

// Main RTC protocol state machine
void rtc_protocol_main(void) {
    static rtc_protocol_state_t state = RTC_STATE_INIT;
    RTC_Time current_time, set_time;
    char rx_buffer[64];
    char tx_buffer[100];

    while (1) {
        switch (state) {
            case RTC_STATE_INIT:
                rtc_init();
                sprintf(tx_buffer, "RTC Auto-Sync Protocol Ready - Using DS3231\r\n");
                HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);
                state = RTC_STATE_SEND_TIME;
                break;

            case RTC_STATE_SEND_TIME:
                if (rtc_read_time(&current_time) == HAL_OK) {
                    send_rtc_time(&current_time);
                    state = RTC_STATE_WAIT_VALIDATION;
                } else {
                    sprintf(tx_buffer, "ERROR: Cannot read RTC\r\n");
                    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 1000);
                    HAL_Delay(2000);  // Wait before retry
                }
                break;

            case RTC_STATE_WAIT_VALIDATION:
                memset(rx_buffer, 0, sizeof(rx_buffer));
                HAL_UART_Receive(&huart1, (uint8_t*)rx_buffer, sizeof(rx_buffer)-1, 500);
				if (strncmp(rx_buffer, "TIME_VALID", 10) == 0) {
					sprintf(tx_buffer, "PC confirmed time is valid, entering normal operation\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);
					state = RTC_STATE_RUNNING;
                    return;
				} else if (strncmp(rx_buffer, "RESYNC", 6) == 0) {
					sprintf(tx_buffer, "PC requested resync, time difference too large\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);
					state = RTC_STATE_WAIT_SYNC;
				} else {
					// Timeout - assume time is valid
					sprintf(tx_buffer, "No PC response, assuming time is valid\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);
					state = RTC_STATE_SEND_TIME;
				}
                break;

            case RTC_STATE_WAIT_SYNC:
                sprintf(tx_buffer, "SYNC_REQUEST\r\n");
                HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);

                memset(rx_buffer, 0, sizeof(rx_buffer));
                HAL_UART_Receive(&huart1, (uint8_t*)rx_buffer, sizeof(rx_buffer)-1, 500);
				if (parse_pc_time(rx_buffer, &set_time)) {
					if (rtc_write_time(&set_time) == HAL_OK) {
						sprintf(tx_buffer, "Time updated, checking again\r\n");
						HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);
						HAL_Delay(100);  // Wait for write to complete
						state = RTC_STATE_SEND_TIME;  // Loop back to send time again
					} else {
						sprintf(tx_buffer, "ERROR: Cannot write to RTC\r\n");
						HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 500);
					}
				}
                break;

            case RTC_STATE_RUNNING:
                // Check for manual commands (non-blocking)
                memset(rx_buffer, 0, sizeof(rx_buffer));
                HAL_UART_Receive(&huart1, (uint8_t*)rx_buffer, sizeof(rx_buffer)-1, 100);
				if (strncmp(rx_buffer, "CHECK", 5) == 0) {
					if (rtc_read_time(&current_time) == HAL_OK) {
						send_rtc_time(&current_time);
					}
				} else if (strncmp(rx_buffer, "RESYNC", 6) == 0) {
					state = RTC_STATE_SEND_TIME;  // Force check
				}
                break;
        }

        HAL_Delay(10);  // Small delay to prevent busy loop
    }
}
