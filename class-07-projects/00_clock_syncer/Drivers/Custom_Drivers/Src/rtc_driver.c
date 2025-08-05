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
    if (time->year < 25 || time->year > 99) return 0;  // 2025-2099
    if (time->month < 1 || time->month > 12) return 0;
    if (time->date < 1 || time->date > 31) return 0;
    if (time->hours > 23) return 0;
    if (time->minutes > 59) return 0;
    if (time->seconds > 59) return 0;
    return 1;
}

// Generic RTC write function
HAL_StatusTypeDef rtc_write_time(RTC_Time *time) {
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
HAL_StatusTypeDef rtc_read_time(RTC_Time *time) {
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

// Send RTC time to PC
void send_rtc_time(RTC_Time *time) {
    char buffer[100];
    sprintf(buffer, "RTC_TIME: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            time->year, time->month, time->date,
            time->hours, time->minutes, time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 200);
}

// Request sync from PC
void request_sync(void) {
    char msg[] = "SYNC_REQUEST\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
}

// Initialize RTC module
void rtc_init(void) {
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

// Parse time from PC command: "SYNC:YYYY-MM-DD:HH:MM:SS"
int parse_pc_time(char *cmd, RTC_Time *time) {
    if (strncmp(cmd, "SYNC:", 5) != 0) return 0;

    int year, month, date, hours, minutes, seconds;
    if (sscanf(cmd + 5, "%d-%d-%d:%d:%d:%d",
               &year, &month, &date, &hours, &minutes, &seconds) == 6) {

        time->year = year - 2000;  // RTC stores year as offset from 2000
        time->month = month;
        time->date = date;
        time->hours = hours;
        time->minutes = minutes;
        time->seconds = seconds;
        time->day_of_week = 1;  // Default, can be calculated if needed

        return 1;
    }
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

// Request verification from PC
void request_verification(RTC_Time *set_time, RTC_Time *read_time) {
    char buffer[200];

    sprintf(buffer, "VERIFY_REQUEST\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    // Send set time
    sprintf(buffer, "SET: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            set_time->year, set_time->month, set_time->date,
            set_time->hours, set_time->minutes, set_time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    // Send read time
    sprintf(buffer, "READ: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
            read_time->year, read_time->month, read_time->date,
            read_time->hours, read_time->minutes, read_time->seconds);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);

    // Calculate and send verification result
    int time_diff = abs((set_time->hours * 3600 + set_time->minutes * 60 + set_time->seconds) -
                       (read_time->hours * 3600 + read_time->minutes * 60 + read_time->seconds));

    if (time_diff <= 2) {
        sprintf(buffer, "VERIFY: OK (diff: %d sec)\r\n", time_diff);
    } else {
        sprintf(buffer, "VERIFY: FAIL (diff: %d sec)\r\n", time_diff);
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
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
                // Periodic time reporting (every 30 seconds)
                if (HAL_GetTick() - last_time_report > 3000) {
                    if (rtc_read_time(&current_time) == HAL_OK) {
                        send_rtc_time(&current_time);
                        last_time_report = HAL_GetTick();
                    }
                }

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
