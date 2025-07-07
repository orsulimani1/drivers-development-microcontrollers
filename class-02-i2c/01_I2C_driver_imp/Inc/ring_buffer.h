/*
 * ring_buffer.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Orr
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>


// Ring buffer size (must be power of 2)
#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE    256
#endif

/**
 * @brief Ring buffer status enumeration
 */
typedef enum {
    RING_BUFFER_OK = 0,
    RING_BUFFER_FULL,
    RING_BUFFER_EMPTY,
    RING_BUFFER_ERROR
} ring_buffer_status_t;

/**
 * @brief Ring buffer structure
 */
typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} ring_buffer_t;

/**
 * @brief Initialize ring buffer
 */
static inline ring_buffer_status_t ring_buffer_init(ring_buffer_t *rb) {
    if (!rb) return RING_BUFFER_ERROR;
    
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    
    return RING_BUFFER_OK;
}

/**
 * @brief Put data into ring buffer
 */
static inline ring_buffer_status_t ring_buffer_put(ring_buffer_t *rb, uint8_t data) {
    if (!rb) return RING_BUFFER_ERROR;
    
    if (rb->count >= RING_BUFFER_SIZE) {
        return RING_BUFFER_FULL;
    }
    
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) & (RING_BUFFER_SIZE - 1);
    rb->count++;
    
    return RING_BUFFER_OK;
}

/**
 * @brief Get data from ring buffer
 */
static inline ring_buffer_status_t ring_buffer_get(ring_buffer_t *rb, uint8_t *data) {
    if (!rb || !data) return RING_BUFFER_ERROR;
    
    if (rb->count == 0) {
        return RING_BUFFER_EMPTY;
    }
    
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & (RING_BUFFER_SIZE - 1);
    rb->count--;
    
    return RING_BUFFER_OK;
}

/**
 * @brief Get available data count
 */
static inline uint16_t ring_buffer_available(ring_buffer_t *rb) {
    return rb ? rb->count : 0;
}

/**
 * @brief Get free space count
 */
static inline uint16_t ring_buffer_free_space(ring_buffer_t *rb) {
    return rb ? (RING_BUFFER_SIZE - rb->count) : 0;
}

/**
 * @brief Check if buffer is empty
 */
static inline bool ring_buffer_is_empty(ring_buffer_t *rb) {
    return rb ? (rb->count == 0) : true;
}

/**
 * @brief Check if buffer is full
 */
static inline bool ring_buffer_is_full(ring_buffer_t *rb) {
    return rb ? (rb->count >= RING_BUFFER_SIZE) : true;
}

/**
 * @brief Peek at next data without removing
 */
static inline ring_buffer_status_t ring_buffer_peek(ring_buffer_t *rb, uint8_t *data) {
    if (!rb || !data) return RING_BUFFER_ERROR;
    
    if (rb->count == 0) {
        return RING_BUFFER_EMPTY;
    }
    
    *data = rb->buffer[rb->tail];
    return RING_BUFFER_OK;
}

/**
 * @brief Flush buffer (clear all data)
 */
static inline ring_buffer_status_t ring_buffer_flush(ring_buffer_t *rb) {
    if (!rb) return RING_BUFFER_ERROR;
    
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    
    return RING_BUFFER_OK;
}

/**
 * @brief Put multiple bytes
 */
static inline ring_buffer_status_t ring_buffer_put_multiple(ring_buffer_t *rb, 
                                                            const uint8_t *data, 
                                                            uint16_t length) {
    if (!rb || !data) return RING_BUFFER_ERROR;
    
    if (ring_buffer_free_space(rb) < length) {
        return RING_BUFFER_FULL;
    }
    
    for (uint16_t i = 0; i < length; i++) {
        ring_buffer_put(rb, data[i]);
    }
    
    return RING_BUFFER_OK;
}

/**
 * @brief Get multiple bytes
 */
static inline uint16_t ring_buffer_get_multiple(ring_buffer_t *rb, 
                                                uint8_t *data, 
                                                uint16_t max_length) {
    if (!rb || !data) return 0;
    
    uint16_t count = 0;
    uint16_t available = ring_buffer_available(rb);
    uint16_t to_read = (available < max_length) ? available : max_length;
    
    for (uint16_t i = 0; i < to_read; i++) {
        if (ring_buffer_get(rb, &data[i]) == RING_BUFFER_OK) {
            count++;
        } else {
            break;
        }
    }
    
    return count;
}

typedef ring_buffer_t i2c_ring_buffer_t;
#define i2c_ring_buffer_init    ring_buffer_init
#define i2c_ring_buffer_put     ring_buffer_put


#endif /* RING_BUFFER_H_ */