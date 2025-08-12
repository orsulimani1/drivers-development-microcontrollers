#ifndef RTC_DRIVER_INT_H_
#define RTC_DRIVER_INT_H_

#include "rtc_driver_api.h"


HAL_StatusTypeDef rtc_internal_set_time(RTC_Time *time);   
HAL_StatusTypeDef rtc_internal_get_time(RTC_Time *time);   
void rtc_internal_init(void);                     

#endif //RTC_DRIVER_INT_H_