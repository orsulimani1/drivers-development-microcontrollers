#include "rtc_internal.h"

HAL_StatusTypeDef rtc_internal_set_time(RTC_Time *time) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Set time
    sTime.Hours = time->hours;
    sTime.Minutes = time->minutes;
    sTime.Seconds = time->seconds;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Set date
    sDate.WeekDay = time->day_of_week;
    sDate.Month = time->month;
    sDate.Date = time->date;
    sDate.Year = time->year;
    
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef rtc_internal_get_time(RTC_Time *time) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Get time first (updates shadow registers)
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Get date (must be called after GetTime)
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    time->hours = sTime.Hours;
    time->minutes = sTime.Minutes;
    time->seconds = sTime.Seconds;
    time->day_of_week = sDate.WeekDay;
    time->month = sDate.Month;
    time->date = sDate.Date;
    time->year = sDate.Year;
    
    return HAL_OK;
}

void rtc_internal_init(void) {
    char buffer[100];
    
    // RTC is already initialized by MX_RTC_Init() in main
    // Just report status
    RTC_Time current_time;
    
    if (rtc_internal_get_time(&current_time) == HAL_OK) {
        sprintf(buffer, "Internal RTC initialized - Time: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
                current_time.year, current_time.month, current_time.date,
                current_time.hours, current_time.minutes, current_time.seconds);
    } else {
        sprintf(buffer, "Internal RTC initialization - Read error\r\n");
    }
    
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
}