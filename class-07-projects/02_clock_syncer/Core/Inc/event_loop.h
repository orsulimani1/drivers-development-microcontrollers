/*
 * event_loop.h
 *
 *  Created on: Aug 11, 2025
 *      Author: Orr
 */

#ifndef INC_EVENT_LOOP_H_
#define INC_EVENT_LOOP_H_

#include "main.h"
#include "hcsr04_driver.h"
#include "hw512_driver.h"
#include "rtc_driver.h"


#define EVENT_DISTANCE_READ         0b00001
#define EVENT_BUZZER_BEEP           0b00010
#define EVENT_3                     0b00100
#define EVENT_SYNC_REQ              0b01000

extern uint64_t lptimer_tick_count;
extern uint8_t event_flags;
extern uint32_t last_systick;
void systick_calc_passed_time(void);
uint16_t distance_to_note(float distance_cm);

extern hcsr04_config_t hcsr04_config;
extern hw512_config_t buzzer_config;

static inline void set_event_flags(void){
    lptimer_tick_count++;
    
    // Event 1 & 2: Every 500ms (2 ticks)
    if (lptimer_tick_count % 4 == 0) {
        event_flags |= EVENT_DISTANCE_READ  | EVENT_BUZZER_BEEP ;
    }
    
    // Event 3: Every 3s (12 ticks)
    if (lptimer_tick_count % 12 == 0) {
        event_flags |= EVENT_3;
    }
    
    // Event 4: Every minute (240 ticks)
	if (lptimer_tick_count % 240 == 0) {
        event_flags |= EVENT_SYNC_REQ;
        lptimer_tick_count = 0;
    }
}

static inline void process_events(void) {
    char buffer[128];
    volatile uint32_t systick = last_systick;
    static uint16_t freq = 0;
    if (event_flags & EVENT_DISTANCE_READ ) {
        float distance;
        if (hcsr04_get_distance_cm(&distance) == HCSR04_OK) {
            sprintf(buffer, "DISTANCE: %.1f cm (%.2f ft, %.1f in): %lu ms\r\n", 
                    distance, hcsr04_cm_to_feet(distance), 
                    hcsr04_cm_to_inches(distance), last_systick);
            freq = distance_to_note(distance);
        } else {
            sprintf(buffer, "DISTANCE: Read Error: %lu ms\r\n", last_systick);
        }
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        event_flags &= ~EVENT_DISTANCE_READ;
    }
    
    if (event_flags & EVENT_BUZZER_BEEP ) {
        // Check GPIO state
        hw512_tone(freq, 200);
        sprintf(buffer, "BUZZER_BEEP: %lu ms\r\n", systick);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        event_flags &= ~EVENT_BUZZER_BEEP ;
    }
    
    if (event_flags & EVENT_3) {
        // Send UART message
        sprintf(buffer, "EVENT_3: %lu ms\r\n", systick);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        event_flags &= ~EVENT_3;
    }
    
    if (event_flags & EVENT_SYNC_REQ) {
        // Send sync request
        // sprintf(buffer, "SYNC_REQ: %lu ms\r\n", systick);
        // HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        rtc_protocol_main();
        event_flags &= ~EVENT_SYNC_REQ;
    }
}

#endif /* INC_EVENT_LOOP_H_ */
