/*
 * stm32f411_base.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Orr
 */

#ifndef STM32F411_BASE_H_
#define STM32F411_BASE_H_

#include <stdio.h>
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
    I2C1_EV_IRQn            = 31,
    I2C1_ER_IRQn                ,
    I2C2_EV_IRQn                ,
    I2C2_ER_IRQn                ,
	SPI1_IRQn					,
	SPI2_IRQn					,

	EXTI15_10_IRQn			= 40,
	I2C3_EV_IRQn            = 72, 
    I2C3_ER_IRQn                ,
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

#define RCC_APB1ENR_I2C1EN      (1UL << 21)
#define RCC_APB1ENR_I2C2EN      (1UL << 22)
#define RCC_APB1ENR_I2C3EN      (1UL << 23)

#define RCC_APB2ENR_SPI1EN      (1UL << 12)
#define RCC_APB2ENR_SPI4EN      (1UL << 13)
#define RCC_APB1ENR_SPI1EN      (1UL << 21)
#define RCC_APB1ENR_SPI2EN      (1UL << 14)
#define RCC_APB1ENR_SPI3EN      (1UL << 25)

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


/**
 * @defgroup I2C_Base_Definitions Base Type Definitions
 * @{
 */

/**
 * @brief I2C register bitfield structures
 * Based on STM32F411 Reference Manual Chapter 18
 */

// I2C Control Register 1 bitfield
typedef struct {
    uint32_t PE          : 1;  // Bit 0: Peripheral Enable
    uint32_t SMBUS       : 1;  // Bit 1: SMBus Mode
    uint32_t RESERVED_2  : 1;  // Bit 2: Reserved
    uint32_t SMBTYPE     : 1;  // Bit 3: SMBus Type
    uint32_t ENARP       : 1;  // Bit 4: ARP Enable
    uint32_t ENPEC       : 1;  // Bit 5: PEC Enable
    uint32_t ENGC        : 1;  // Bit 6: General Call Enable
    uint32_t NOSTRETCH   : 1;  // Bit 7: Clock Stretching Disable
    uint32_t START       : 1;  // Bit 8: Start Generation
    uint32_t STOP        : 1;  // Bit 9: Stop Generation
    uint32_t ACK         : 1;  // Bit 10: Acknowledge Enable
    uint32_t POS         : 1;  // Bit 11: Acknowledge Position
    uint32_t PEC         : 1;  // Bit 12: Packet Error Checking
    uint32_t ALERT       : 1;  // Bit 13: SMBus Alert
    uint32_t RESERVED_14 : 1;  // Bit 14: Reserved
    uint32_t SWRST       : 1;  // Bit 15: Software Reset
    uint32_t RESERVED_31_16 : 16; // Bits 31:16: Reserved
} I2C_CR1_BitField;

// I2C Control Register 2 bitfield
typedef struct {
    uint32_t FREQ        : 6;  // Bits 5:0: Peripheral Clock Frequency
    uint32_t RESERVED_7_6 : 2; // Bits 7:6: Reserved
    uint32_t ITERREN     : 1;  // Bit 8: Error Interrupt Enable
    uint32_t ITEVTEN     : 1;  // Bit 9: Event Interrupt Enable
    uint32_t ITBUFEN     : 1;  // Bit 10: Buffer Interrupt Enable
    uint32_t DMAEN       : 1;  // Bit 11: DMA Requests Enable
    uint32_t LAST        : 1;  // Bit 12: DMA Last Transfer
    uint32_t RESERVED_31_13 : 19; // Bits 31:13: Reserved
} I2C_CR2_BitField;

// I2C Own Address Register 1 bitfield
typedef struct {
    uint32_t ADD0        : 1;  // Bit 0: Interface Address (10-bit mode)
    uint32_t ADD7_1      : 7;  // Bits 7:1: Interface Address bits 7:1
    uint32_t ADD9_8      : 2;  // Bits 9:8: Interface Address bits 9:8 (10-bit mode)
    uint32_t RESERVED_14_10 : 5; // Bits 14:10: Reserved
    uint32_t ADDMODE     : 1;  // Bit 15: Addressing Mode (7-bit/10-bit)
    uint32_t RESERVED_31_16 : 16; // Bits 31:16: Reserved
} I2C_OAR1_BitField;

// I2C Data Register bitfield
typedef struct {
    uint32_t DR          : 8;  // Bits 7:0: 8-bit Data Register
    uint32_t RESERVED_31_8 : 24; // Bits 31:8: Reserved
} I2C_DR_BitField;

// I2C Status Register 1 bitfield
typedef struct {
    uint32_t SB          : 1;  // Bit 0: Start Bit
    uint32_t ADDR        : 1;  // Bit 1: Address Sent/Matched
    uint32_t BTF         : 1;  // Bit 2: Byte Transfer Finished
    uint32_t ADD10       : 1;  // Bit 3: 10-bit Header Sent
    uint32_t STOPF       : 1;  // Bit 4: Stop Detection
    uint32_t RESERVED_5  : 1;  // Bit 5: Reserved
    uint32_t RXNE        : 1;  // Bit 6: Data Register Not Empty (RX)
    uint32_t TXE         : 1;  // Bit 7: Data Register Empty (TX)
    uint32_t BERR        : 1;  // Bit 8: Bus Error
    uint32_t ARLO        : 1;  // Bit 9: Arbitration Lost
    uint32_t AF          : 1;  // Bit 10: Acknowledge Failure
    uint32_t OVR         : 1;  // Bit 11: Overrun/Underrun
    uint32_t PECERR      : 1;  // Bit 12: PEC Error
    uint32_t RESERVED_13 : 1;  // Bit 13: Reserved
    uint32_t TIMEOUT     : 1;  // Bit 14: Timeout Error
    uint32_t SMBALERT    : 1;  // Bit 15: SMBus Alert
    uint32_t RESERVED_31_16 : 16; // Bits 31:16: Reserved
} I2C_SR1_BitField;

// I2C Status Register 2 bitfield
typedef struct {
    uint32_t MSL         : 1;  // Bit 0: Master/Slave
    uint32_t BUSY        : 1;  // Bit 1: Bus Busy
    uint32_t TRA         : 1;  // Bit 2: Transmitter/Receiver
    uint32_t RESERVED_3  : 1;  // Bit 3: Reserved
    uint32_t GENCALL     : 1;  // Bit 4: General Call Address
    uint32_t SMBDEFAULT  : 1;  // Bit 5: SMBus Device Default Address
    uint32_t SMBHOST     : 1;  // Bit 6: SMBus Host Header
    uint32_t DUALF       : 1;  // Bit 7: Dual Flag
    uint32_t PEC         : 8;  // Bits 15:8: Packet Error Checking Register
    uint32_t RESERVED_31_16 : 16; // Bits 31:16: Reserved
} I2C_SR2_BitField;

// I2C Clock Control Register bitfield
typedef struct {
    uint32_t CCR         : 12; // Bits 11:0: Clock Control Register
    uint32_t RESERVED_13_12 : 2; // Bits 13:12: Reserved
    uint32_t DUTY        : 1;  // Bit 14: Fast Mode Duty Cycle
    uint32_t FS          : 1;  // Bit 15: I2C Master Mode Selection
    uint32_t RESERVED_31_16 : 16; // Bits 31:16: Reserved
} I2C_CCR_BitField;

// I2C TRISE Register bitfield
typedef struct {
    uint32_t TRISE       : 6;  // Bits 5:0: Maximum Rise Time
    uint32_t RESERVED_31_6 : 26; // Bits 31:6: Reserved
} I2C_TRISE_BitField;

/**
 * @brief I2C peripheral register structure
 */
typedef struct {
    union {
        volatile uint32_t     value;
        volatile I2C_CR1_BitField  fields;
    } CR1;      // Control register 1

    union {
        volatile uint32_t     value;
        volatile I2C_CR2_BitField  fields;
    } CR2;      // Control register 2

    union {
        volatile uint32_t     value;
        volatile I2C_OAR1_BitField fields;
    } OAR1;     // Own address register 1

    volatile uint32_t     OAR2;     // Own address register 2

    union {
        volatile uint32_t     value;
        volatile I2C_DR_BitField   fields;
    } DR;       // Data register

    union {
        volatile uint32_t     value;
        volatile I2C_SR1_BitField  fields;
    } SR1;      // Status register 1

    union {
        volatile uint32_t     value;
        volatile I2C_SR2_BitField  fields;
    } SR2;      // Status register 2

    union {
        volatile uint32_t     value;
        volatile I2C_CCR_BitField  fields;
    } CCR;      // Clock control register

    union {
        volatile uint32_t     value;
        volatile I2C_TRISE_BitField fields;
    } TRISE;    // TRISE register

    volatile uint32_t     FLTR;     // FLTR register
} I2C_TypeDef;

/**
 * @brief I2C base addresses
 */
#define I2C1_BASE           0x40005400UL
#define I2C2_BASE           0x40005800UL
#define I2C3_BASE           0x40005C00UL

#define I2C1                ((I2C_TypeDef *)I2C1_BASE)
#define I2C2                ((I2C_TypeDef *)I2C2_BASE)
#define I2C3                ((I2C_TypeDef *)I2C3_BASE)



// SPI Base addresses
#define SPI1_BASE           0x40013000UL
#define SPI2_BASE           0x40003800UL
#define SPI3_BASE           0x40003C00UL
#define SPI4_BASE           0x40013400UL

typedef union {
    struct {
        uint32_t CPHA       : 1;    // Clock Phase
        uint32_t CPOL       : 1;    // Clock Polarity
        uint32_t MSTR       : 1;    // Master Selection
        uint32_t BR         : 3;    // Baud Rate Control
        uint32_t SPE        : 1;    // SPI Enable
        uint32_t LSBFIRST   : 1;    // Frame Format
        uint32_t SSI        : 1;    // Internal Slave Select
        uint32_t SSM        : 1;    // Software Slave Management
        uint32_t RXONLY     : 1;    // Receive Only
        uint32_t DFF        : 1;    // Data Frame Format
        uint32_t CRCNEXT    : 1;    // CRC Transfer Next
        uint32_t CRCEN      : 1;    // Hardware CRC Calculation Enable
        uint32_t BIDIOE     : 1;    // Output Enable in Bidirectional Mode
        uint32_t BIDIMODE   : 1;    // Bidirectional Data Mode Enable
        uint32_t RESERVED   : 16;   // Reserved bits
    } fields;
    uint32_t value;
} SPI_CR1_TypeDef;

typedef union {
    struct {
        uint32_t RXDMAEN    : 1;    // Rx Buffer DMA Enable
        uint32_t TXDMAEN    : 1;    // Tx Buffer DMA Enable
        uint32_t SSOE       : 1;    // SS Output Enable
        uint32_t RESERVED1  : 1;    // Reserved
        uint32_t FRF        : 1;    // Frame Format
        uint32_t ERRIE      : 1;    // Error Interrupt Enable
        uint32_t RXNEIE     : 1;    // RX Buffer Not Empty Interrupt Enable
        uint32_t TXEIE      : 1;    // Tx Buffer Empty Interrupt Enable
        uint32_t RESERVED2  : 24;   // Reserved bits
    } fields;
    uint32_t value;
} SPI_CR2_TypeDef;

typedef union {
    struct {
        uint32_t RXNE       : 1;    // Receive Buffer Not Empty
        uint32_t TXE        : 1;    // Transmit Buffer Empty
        uint32_t CHSIDE     : 1;    // Channel Side
        uint32_t UDR        : 1;    // Underrun Flag
        uint32_t CRCERR     : 1;    // CRC Error Flag
        uint32_t MODF       : 1;    // Mode Fault
        uint32_t OVR        : 1;    // Overrun Flag
        uint32_t BSY        : 1;    // Busy Flag
        uint32_t FRE        : 1;    // Frame Format Error
        uint32_t RESERVED   : 23;   // Reserved bits
    } fields;
    uint32_t value;
} SPI_SR_TypeDef;

typedef struct {
    SPI_CR1_TypeDef CR1;        // Control Register 1
    SPI_CR2_TypeDef CR2;        // Control Register 2
    SPI_SR_TypeDef  SR;         // Status Register
    volatile uint32_t DR;       // Data Register
    volatile uint32_t CRCPR;    // CRC Polynomial Register
    volatile uint32_t RXCRCR;   // RX CRC Register
    volatile uint32_t TXCRCR;   // TX CRC Register
    volatile uint32_t I2SCFGR;  // I2S Configuration Register
    volatile uint32_t I2SPR;    // I2S Prescaler Register
} SPI_TypeDef;


// SPI Peripheral definitions
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define SPI4                ((SPI_TypeDef *) SPI4_BASE)


#endif /* STM32F411_BASE_H_ */
