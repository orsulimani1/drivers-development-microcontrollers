/*
 * stm32f411_base.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Orr
 */

#ifndef STM32F411_BASE_H_
#define STM32F411_BASE_H_


#include <stdint.h>

// STM32F411 Base Addresses
#define PERIPH_BASE           0x40000000UL
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)

// GPIO Base Addresses
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE  	  	  (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE  	      (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE   	      (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE   	  	  (AHB1PERIPH_BASE + 0x1000UL)
#define GPIOH_BASE   	  	  (AHB1PERIPH_BASE + 0x1C00UL)


// RCC Base Address
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)

// SYSCFG Base Address
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800UL)

// EXTI Base Address
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00UL)

// NVIC Base Addresses
#define NVIC_BASE             0xE000E100UL
#define SCB_BASE              0xE000ED00UL

/* NVIC IRQ numbers for STM32F411VET6 */
typedef enum {
    NonMaskableInt_IRQn     = -14,
    HardFault_IRQn          = -13,
    MemoryManagement_IRQn   = -12,
    BusFault_IRQn           = -11,
    UsageFault_IRQn         = -10,
    SVCall_IRQn             = -5,
    DebugMonitor_IRQn       = -4,
    PendSV_IRQn             = -2,
    SysTick_IRQn            = -1,
    /* STM32 specific interrupts */
    EXTI0_IRQn              = 6,
    EXTI1_IRQn              = 7,
    EXTI2_IRQn              = 8,
    EXTI3_IRQn              = 9,
    EXTI4_IRQn              = 10,
    /* Add other peripheral interrupts as needed */
	EXTI5_9_IRQn			= 23,
	EXTI15_10_IRQn			= 40,
	CUSTOM_IRQ2             = 43   /* Example custom interrupt */
} IRQn_Type;

// GPIO Register Structure
typedef struct {
    volatile uint32_t MODER;    // Mode register
    volatile uint32_t OTYPER;   // Output type register
    volatile uint32_t OSPEEDR;  // Output speed register
    volatile uint32_t PUPDR;    // Pull-up/pull-down register
    volatile uint32_t IDR;      // Input data register
    volatile uint32_t ODR;      // Output data register
    volatile uint32_t BSRR;     // Bit set/reset register
    volatile uint32_t LCKR;     // Configuration lock register
    volatile uint32_t AFR[2];   // Alternate function registers
} GPIO_TypeDef;

// RCC Register Structure
typedef struct {
    volatile uint32_t CR;         // Clock control register
    volatile uint32_t PLLCFGR;    // PLL configuration register
    volatile uint32_t CFGR;       // Clock configuration register
    volatile uint32_t CIR;        // Clock interrupt register
    volatile uint32_t AHB1RSTR;   // AHB1 peripheral reset register
    volatile uint32_t AHB2RSTR;   // AHB2 peripheral reset register
    volatile uint32_t AHB3RSTR;   // AHB3 peripheral reset register
    volatile uint32_t Reserved0;  // Reserved
    volatile uint32_t APB1RSTR;   // APB1 peripheral reset register
    volatile uint32_t APB2RSTR;   // APB2 peripheral reset register
    volatile uint32_t Reserved1[2]; // Reserved
    volatile uint32_t AHB1ENR;    // AHB1 peripheral clock enable register
    volatile uint32_t AHB2ENR;    // AHB2 peripheral clock enable register
    volatile uint32_t AHB3ENR;    // AHB3 peripheral clock enable register
    volatile uint32_t Reserved2;  // Reserved
    volatile uint32_t APB1ENR;    // APB1 peripheral clock enable register
    volatile uint32_t APB2ENR;    // APB2 peripheral clock enable register
} RCC_TypeDef;

// SYSCFG Register Structure
typedef struct {
    volatile uint32_t MEMRMP;     // Memory remap register
    volatile uint32_t PMC;        // Peripheral mode configuration register
    volatile uint32_t EXTICR[4];  // External interrupt configuration registers
    volatile uint32_t Reserved[2]; // Reserved
    volatile uint32_t CMPCR;      // Compensation cell control register
} SYSCFG_TypeDef;

// EXTI Register Structure
typedef struct {
    volatile uint32_t IMR;    // Interrupt mask register
    volatile uint32_t EMR;    // Event mask register
    volatile uint32_t RTSR;   // Rising trigger selection register
    volatile uint32_t FTSR;   // Falling trigger selection register
    volatile uint32_t SWIER;  // Software interrupt event register
    volatile uint32_t PR;     // Pending register
} EXTI_TypeDef;

// NVIC Register Structure
typedef struct {
    volatile uint32_t ISER[8];    // Interrupt Set Enable Register
    volatile uint32_t Reserved0[24];
    volatile uint32_t ICER[8];    // Interrupt Clear Enable Register
    volatile uint32_t Reserved1[24];
    volatile uint32_t ISPR[8];    // Interrupt Set Pending Register
    volatile uint32_t Reserved2[24];
    volatile uint32_t ICPR[8];    // Interrupt Clear Pending Register
    volatile uint32_t Reserved3[24];
    volatile uint32_t IABR[8];    // Interrupt Active bit Register
    volatile uint32_t Reserved4[56];
    volatile uint8_t  IP[240];    // Interrupt Priority Register
} NVIC_TypeDef;

// Peripheral Definitions
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)

#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define NVIC                ((NVIC_TypeDef *) NVIC_BASE)

// Register Bit Definitions
#define RCC_AHB1ENR_GPIOAEN     (1UL << 0)
#define RCC_APB2ENR_SYSCFGEN    (1UL << 14)

// GPIO MODER bits
#define GPIO_MODER_INPUT        (0UL)
#define GPIO_MODER_OUTPUT       (1UL)
#define GPIO_MODER_AF           (2UL)
#define GPIO_MODER_ANALOG       (3UL)

// GPIO PUPDR bits
#define GPIO_PUPDR_NOPULL       (0UL)
#define GPIO_PUPDR_PULLUP       (1UL)
#define GPIO_PUPDR_PULLDOWN     (2UL)

// SYSCFG EXTICR bits
#define SYSCFG_EXTICR1_EXTI0_PA (0UL << 0)
#define SYSCFG_EXTICR1_EXTI0_PB (1UL << 0)
#define SYSCFG_EXTICR1_EXTI0_PC (2UL << 0)

// EXTI bits
#define EXTI_IMR_MR0            (1UL << 0)
#define EXTI_FTSR_TR0           (1UL << 0)
#define EXTI_RTSR_TR0           (1UL << 0)
#define EXTI_PR_PR0             (1UL << 0)

// NVIC Helper Functions
static inline void NVIC_EnableIRQ(uint8_t IRQn) {
    NVIC->ISER[IRQn >> 5] = (1 << (IRQn & 0x1F));
}

static inline void NVIC_SetPriority(uint8_t IRQn, uint32_t priority) {
    NVIC->IP[IRQn >> 5] = (uint8_t)(priority << 4);

}

static inline void NVIC_SetPending(uint8_t IRQn){
	NVIC->ISPR[IRQn >> 5] = (1 << (IRQn & 0x1F));
}

#endif /* STM32F411_BASE_H_ */
