/*
 * hcsr04_driver.h
 *
 *  Created on: Aug 12, 2025
 *      Author: Generated
 */

#ifndef HCSR04_DRIVER_H_
#define HCSR04_DRIVER_H_

#include "main.h"
#include <stdint.h>

// HC-SR04 Configuration
typedef struct {
    GPIO_TypeDef *trig_port;
    uint16_t trig_pin;
    GPIO_TypeDef *echo_port;
    uint16_t echo_pin;
    TIM_HandleTypeDef *htim;        // Timer for microsecond measurements
} hcsr04_config_t;

// Status codes
typedef enum {
    HCSR04_OK = 0,
    HCSR04_ERROR,
    HCSR04_TIMEOUT,
    HCSR04_NOT_INITIALIZED
} hcsr04_status_t;

// Distance measurement structure
typedef struct {
    float distance_cm;
    uint32_t pulse_duration_us;
    hcsr04_status_t status;
} hcsr04_result_t;

// Function prototypes
hcsr04_status_t hcsr04_init(hcsr04_config_t *config);
hcsr04_status_t hcsr04_measure_distance(hcsr04_result_t *result);
hcsr04_status_t hcsr04_get_distance_cm(float *distance);

// Unit conversion functions
float hcsr04_cm_to_meters(float cm);
float hcsr04_cm_to_feet(float cm);
float hcsr04_cm_to_inches(float cm);
float hcsr04_meters_to_cm(float meters);
float hcsr04_feet_to_cm(float feet);
float hcsr04_inches_to_cm(float inches);

// Utility functions
uint8_t hcsr04_is_distance_valid(float distance_cm);
const char* hcsr04_status_to_string(hcsr04_status_t status);

// Configuration constants
#define HCSR04_MAX_DISTANCE_CM      400.0f    // HC-SR04 max range
#define HCSR04_MIN_DISTANCE_CM      2.0f      // HC-SR04 min range
#define HCSR04_TIMEOUT_US           30000     // 30ms timeout
#define HCSR04_TRIGGER_PULSE_US     10        // 10μs trigger pulse
#define HCSR04_SOUND_SPEED_CM_US    0.0343f   // 343 m/s = 0.0343 cm/μs

// Conversion constants
#define CM_TO_METERS_FACTOR         0.01f
#define CM_TO_FEET_FACTOR          0.0328084f
#define CM_TO_INCHES_FACTOR        0.393701f

#endif /* HCSR04_DRIVER_H_ */