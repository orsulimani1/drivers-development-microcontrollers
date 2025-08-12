/*
 * rtc_driver.h
 *
 *  Created on: Aug 4, 2025
 *      Author: Orr
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

#include "rtc_driver_api.h"


HAL_StatusTypeDef rtc_external_get_time(RTC_Time *time);
HAL_StatusTypeDef rtc_external_set_time(RTC_Time *time); 
void rtc_external_init(void);


#endif /* RTC_DRIVER_H_ */
