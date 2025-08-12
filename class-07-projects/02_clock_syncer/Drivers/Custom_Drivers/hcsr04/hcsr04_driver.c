#include "hcsr04_driver.h"
#include <string.h>

// Static variables
static hcsr04_config_t g_hcsr04_config;
static uint8_t g_hcsr04_initialized = 0;

// Private function prototypes
static void hcsr04_trigger_pulse(void);
static uint32_t hcsr04_measure_echo_pulse(void);
static void hcsr04_delay_us(uint32_t us);

hcsr04_status_t hcsr04_init(hcsr04_config_t *config) {
    if (!config || !config->trig_port || !config->echo_port || !config->htim) {
        return HCSR04_ERROR;
    }
    
    // Copy configuration
    memcpy(&g_hcsr04_config, config, sizeof(hcsr04_config_t));
    
    // Configure GPIO pins
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Configure TRIG pin as output
    GPIO_InitStruct.Pin = config->trig_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(config->trig_port, &GPIO_InitStruct);
    
    // Configure ECHO pin as input
    GPIO_InitStruct.Pin = config->echo_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(config->echo_port, &GPIO_InitStruct);
    
    // Set TRIG pin low initially
    HAL_GPIO_WritePin(config->trig_port, config->trig_pin, GPIO_PIN_RESET);
    
    // Start timer for microsecond timing
    HAL_TIM_Base_Start(config->htim);
    
    g_hcsr04_initialized = 1;
    return HCSR04_OK;
}

hcsr04_status_t hcsr04_measure_distance(hcsr04_result_t *result) {
    if (!g_hcsr04_initialized || !result) {
        return HCSR04_NOT_INITIALIZED;
    }
    
    // Clear result
    memset(result, 0, sizeof(hcsr04_result_t));
    
    // Send trigger pulse
    hcsr04_trigger_pulse();
    
    // Measure echo pulse duration
    uint32_t pulse_duration = hcsr04_measure_echo_pulse();
    
    if (pulse_duration == 0) {
        result->status = HCSR04_TIMEOUT;
        return HCSR04_TIMEOUT;
    }
    
    // Calculate distance: distance = (pulse_duration * sound_speed) / 2
    // Sound travels to object and back, so divide by 2
    float distance = (pulse_duration * HCSR04_SOUND_SPEED_CM_US) / 2.0f;
    
    result->pulse_duration_us = pulse_duration;
    result->distance_cm = distance;
    
    // Validate distance range
    if (hcsr04_is_distance_valid(distance)) {
        result->status = HCSR04_OK;
        return HCSR04_OK;
    } else {
        result->status = HCSR04_ERROR;
        return HCSR04_ERROR;
    }
}

hcsr04_status_t hcsr04_get_distance_cm(float *distance) {
    if (!distance) return HCSR04_ERROR;
    
    hcsr04_result_t result;
    hcsr04_status_t status = hcsr04_measure_distance(&result);
    
    if (status == HCSR04_OK) {
        *distance = result.distance_cm;
    }
    
    return status;
}

// Unit conversion functions
float hcsr04_cm_to_meters(float cm) {
    return cm * CM_TO_METERS_FACTOR;
}

float hcsr04_cm_to_feet(float cm) {
    return cm * CM_TO_FEET_FACTOR;
}

float hcsr04_cm_to_inches(float cm) {
    return cm * CM_TO_INCHES_FACTOR;
}

float hcsr04_meters_to_cm(float meters) {
    return meters / CM_TO_METERS_FACTOR;
}

float hcsr04_feet_to_cm(float feet) {
    return feet / CM_TO_FEET_FACTOR;
}

float hcsr04_inches_to_cm(float inches) {
    return inches / CM_TO_INCHES_FACTOR;
}

// Utility functions
uint8_t hcsr04_is_distance_valid(float distance_cm) {
    return (distance_cm >= HCSR04_MIN_DISTANCE_CM && distance_cm <= HCSR04_MAX_DISTANCE_CM);
}

const char* hcsr04_status_to_string(hcsr04_status_t status) {
    switch (status) {
        case HCSR04_OK: return "OK";
        case HCSR04_ERROR: return "ERROR";
        case HCSR04_TIMEOUT: return "TIMEOUT";
        case HCSR04_NOT_INITIALIZED: return "NOT_INITIALIZED";
        default: return "UNKNOWN";
    }
}

// Private functions
static void hcsr04_trigger_pulse(void) {
    // Send 10μs high pulse on TRIG pin
    HAL_GPIO_WritePin(g_hcsr04_config.trig_port, g_hcsr04_config.trig_pin, GPIO_PIN_SET);
    hcsr04_delay_us(HCSR04_TRIGGER_PULSE_US);
    HAL_GPIO_WritePin(g_hcsr04_config.trig_port, g_hcsr04_config.trig_pin, GPIO_PIN_RESET);
}

static uint32_t hcsr04_measure_echo_pulse(void) {
    uint32_t start_time, end_time;
    uint32_t timeout_counter = 0;
    
    // Wait for ECHO pin to go HIGH (start of pulse)
    while (HAL_GPIO_ReadPin(g_hcsr04_config.echo_port, g_hcsr04_config.echo_pin) == GPIO_PIN_RESET) {
        if (++timeout_counter > HCSR04_TIMEOUT_US) {
            return 0; // Timeout
        }
        hcsr04_delay_us(1);
    }
    
    // Record start time
    start_time = __HAL_TIM_GET_COUNTER(g_hcsr04_config.htim);
    
    // Wait for ECHO pin to go LOW (end of pulse)
    timeout_counter = 0;
    while (HAL_GPIO_ReadPin(g_hcsr04_config.echo_port, g_hcsr04_config.echo_pin) == GPIO_PIN_SET) {
        if (++timeout_counter > HCSR04_TIMEOUT_US) {
            return 0; // Timeout
        }
        hcsr04_delay_us(1);
    }
    
    // Record end time
    end_time = __HAL_TIM_GET_COUNTER(g_hcsr04_config.htim);
    
    // Calculate pulse duration (handle timer overflow)
    if (end_time >= start_time) {
        return end_time - start_time;
    } else {
        // Timer overflow occurred
        return (0xFFFFFFFF - start_time) + end_time;
    }
}

static void hcsr04_delay_us(uint32_t us) {
    uint32_t start = __HAL_TIM_GET_COUNTER(g_hcsr04_config.htim);
    while ((__HAL_TIM_GET_COUNTER(g_hcsr04_config.htim) - start) < us) {
        // Busy wait
    }
}