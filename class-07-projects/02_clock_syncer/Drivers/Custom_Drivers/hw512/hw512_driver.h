#ifndef HW512_DRIVER_H_
#define HW512_DRIVER_H_

#include "main.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t tim_channel;
} hw512_config_t;

typedef enum {
    HW512_OK = 0,
    HW512_ERROR,
    HW512_NOT_INITIALIZED
} hw512_status_t;

// Musical notes (frequencies in Hz)
#define NOTE_C4     262
#define NOTE_D4     294
#define NOTE_E4     330
#define NOTE_F4     349
#define NOTE_G4     392
#define NOTE_A4     440
#define NOTE_B4     494
#define NOTE_C5     523

hw512_status_t hw512_init(hw512_config_t *config);
hw512_status_t hw512_tone(uint16_t frequency_hz, uint32_t duration_ms);
hw512_status_t hw512_beep(uint32_t duration_ms);
hw512_status_t hw512_off(void);
hw512_status_t hw512_melody(const uint16_t *notes, const uint32_t *durations, uint8_t count);

#endif /* HW512_DRIVER_H_ */
