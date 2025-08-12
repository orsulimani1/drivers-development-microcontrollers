#include "hw512_driver.h"

static hw512_config_t g_config;
static uint8_t g_initialized = 0;

hw512_status_t hw512_init(hw512_config_t *config) {
    if (!config || !config->htim) return HW512_ERROR;
    
    g_config = *config;
    g_initialized = 1;
    return HW512_OK;
}

hw512_status_t hw512_tone(uint16_t frequency_hz, uint32_t duration_ms) {
    if (!g_initialized) return HW512_NOT_INITIALIZED;
    
    if (frequency_hz == 0) {
        return hw512_off();
    }
    
    // Calculate timer settings for frequency
    uint32_t timer_clock = HAL_RCC_GetPCLK1Freq() * 2; // APB1 timer clock
    uint32_t prescaler = (timer_clock / (frequency_hz * 1000)) - 1;
    uint32_t period = 999;
    
    // Configure timer
    g_config.htim->Init.Prescaler = prescaler;
    g_config.htim->Init.Period = period;
    HAL_TIM_PWM_Init(g_config.htim);
    
    // Set 50% duty cycle
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = period / 2;
    // sConfigOC.Pulse = period;

    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(g_config.htim, &sConfigOC, g_config.tim_channel);
    
    // Start PWM
    HAL_TIM_PWM_Start(g_config.htim, g_config.tim_channel);
    
    if (duration_ms > 0) {
        HAL_Delay(duration_ms);
        hw512_off();
    }
    
    return HW512_OK;
}

hw512_status_t hw512_beep(uint32_t duration_ms) {
    return hw512_tone(NOTE_A4, duration_ms);
}

hw512_status_t hw512_off(void) {
    if (!g_initialized) return HW512_NOT_INITIALIZED;
    
    HAL_TIM_PWM_Stop(g_config.htim, g_config.tim_channel);
    return HW512_OK;
}

hw512_status_t hw512_melody(const uint16_t *notes, const uint32_t *durations, uint8_t count) {
    if (!g_initialized || !notes || !durations) return HW512_ERROR;
    
    for (uint8_t i = 0; i < count; i++) {
        hw512_tone(notes[i], durations[i]);
        HAL_Delay(50); // Brief pause between notes
    }
    
    return HW512_OK;
}