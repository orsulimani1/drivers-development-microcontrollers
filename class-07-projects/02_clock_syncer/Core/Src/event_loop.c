/*
 * event_loop.c
 *
 *  Created on: Aug 11, 2025
 *      Author: Orr
 */


#include "event_loop.h"


uint64_t lptimer_tick_count;
uint8_t event_flags;
uint32_t last_systick = 0;

// Configure HC-SR04 (example pins)
hcsr04_config_t hcsr04_config = {
    .trig_port = GPIOC,
    .trig_pin = GPIO_PIN_1,
    .echo_port = GPIOC,
    .echo_pin = GPIO_PIN_0,
    .htim = &htim2  // Use a timer configured for 1MHz (1μs resolution)
};
// Configure HW-512 (example pins)
hw512_config_t buzzer_config = {
    .htim = &htim3,
    .tim_channel = TIM_CHANNEL_2  // PA7
};
void systick_calc_passed_time(void){

    uint32_t current_systick = HAL_GetTick();
    if(last_systick){
        uint32_t diff_ms = current_systick - last_systick;

/*         char buffer[50];
        sprintf(buffer, "LPTIM: %lu ms\r\n", diff_ms);
        HAL_UART_Transmit(&UART_HANDLE, (uint8_t*)buffer, strlen(buffer), 100); */
    }

    last_systick = current_systick;
}

uint16_t distance_to_note(float distance_cm) {
   // Map distance ranges to musical notes (closer = higher pitch)
   if (distance_cm < 6) return NOTE_C5;       // Very close - highest note
   else if (distance_cm < 20) return NOTE_B4;
   else if (distance_cm < 40) return NOTE_A4;
   else if (distance_cm < 60) return NOTE_G4;
   else if (distance_cm < 85) return NOTE_F4;
   else if (distance_cm < 110) return NOTE_E4;
   else if (distance_cm < 160) return NOTE_D4;
   else return NOTE_C4;                       // Far - lowest note
}