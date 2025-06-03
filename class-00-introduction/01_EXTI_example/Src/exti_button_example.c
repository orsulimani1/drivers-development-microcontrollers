/*
 * exti_button_example.c
 *
 *  Created on: Jun 3, 2025
 *      Author: Orr
 */
#include "stm32f411_base.h"

// Function prototypes
void EXTI_Config(void);
void EXTI0_IRQHandler(void);
void NVIC_EnableIRQ(uint8_t IRQn);
void NVIC_SetPriority(uint8_t IRQn, uint32_t priority);

// Global variables
volatile uint8_t button_pressed = 0;

int main(void) {
    // Configure EXTI for PA0
    EXTI_Config();

    while (1) {
        if (button_pressed) {
            // Handle button press
            button_pressed = 0;
            // Add your application code here
        }

        // Main application loop
    }
}

void EXTI_Config(void) {
    // Step 1: Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;   // Enable SYSCFG clock for EXTI

    // Step 2: Configure PA0 as input with pull-up
    GPIOA->MODER &= ~(3 << (0 * 2));      // Clear mode bits for pin 0
    GPIOA->MODER |= (GPIO_MODER_INPUT << (0 * 2)); // Set input mode

//    GPIOA->PUPDR &= ~(3 << (0 * 2));      // Clear pull bits for pin 0
//    GPIOA->PUPDR |= (GPIO_PUPDR_PULLUP << (0 * 2)); // Set pull-up

    // Step 3: Map PA0 to EXTI0
    SYSCFG->EXTICR[0] &= ~(0xF << 0);     // Clear EXTI0 bits
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // Select PA0

    // Step 4: Configure EXTI0 for falling edge (button press)
    EXTI->FTSR |= EXTI_FTSR_TR0;            // Enable falling edge trigger
    EXTI->RTSR &= ~EXTI_RTSR_TR0;           // Disable rising edge trigger

    // Step 5: Enable EXTI0 interrupt
    EXTI->IMR |= EXTI_IMR_MR0;              // Enable EXTI0 interrupt mask

    // Step 6: Configure NVIC
    NVIC_SetPriority(EXTI0_IRQn, 2);        // Set priority (0-15)
    NVIC_EnableIRQ(EXTI0_IRQn);             // Enable EXTI0 interrupt
    NVIC_SetPending(EXTI0_IRQn);
}

// NVIC Helper Functions
void NVIC_EnableIRQ(uint8_t IRQn) {
    NVIC->ISER[IRQn >> 5] = (1 << (IRQn & 0x1F));
}

void NVIC_SetPriority(uint8_t IRQn, uint32_t priority) {
    NVIC->IP[IRQn >> 5] = (uint8_t)(priority << 4);

}

void NVIC_SetPending(uint8_t IRQn){
	NVIC->ISPR[IRQn >> 5] = (1 << (IRQn & 0x1F));
}
// Interrupt Service Routine
void EXTI0_IRQHandler(void) {
    // Check if EXTI0 triggered the interrupt
    if (EXTI->PR & EXTI_PR_PR0) {
        // Clear the pending bit
        EXTI->PR |= EXTI_PR_PR0;

        // Set flag for main loop
        button_pressed = 1;

        // Optional: Add debouncing delay
        for (volatile int i = 0; i < 10000; i++);
    }
}
