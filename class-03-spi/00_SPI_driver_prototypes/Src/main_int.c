
/**
 * @file main_interrupt.c
 * @brief SPI Driver Interrupt Mode Example
 * @author Drivers Development Course
 * @date 2024
 */

#include "stm32f411_base.h"
#include "spi_int.h"
#include "ring_buffer.h"

/* ============================== Global Variables ============================== */
static spi_handle_t hspi1;
static ring_buffer_t tx_ring_buffer;
static ring_buffer_t rx_ring_buffer;


static volatile bool tx_complete = false;
static volatile bool rx_complete = false;
static volatile bool tx_rx_complete = false;

/* ============================== Function Prototypes ============================== */
static void system_init(void);
static void spi_config_init(void);
static void nvic_config(void);
static void delay_ms(uint32_t ms);

// Callback functions
static void spi_tx_complete_callback(void);
static void spi_rx_complete_callback(void);
static void spi_tx_rx_complete_callback(void);
static void spi_error_callback(void);

/* ============================== Main Function ============================== */
int main(void)
{
    // Initialize system
    system_init();

    // Initialize SPI
    spi_config_init();

    // Configure NVIC
    nvic_config();

    // Test data
    uint8_t tx_data[] = {0x11, 0x22, 0x33, 0x44, 0x55};

    while (1) {
        // Set CS low (start communication)
        spi_set_cs(&hspi1, false);
        delay_ms(1);

        // Transmit data in interrupt mode
        tx_complete = false;
        spi_transmit_it(&hspi1, tx_data, sizeof(tx_data));

        // Wait for transmission to complete
        while (!tx_complete) {
            // Do other work here
        }

        delay_ms(10);

        // Receive data in interrupt mode
        rx_complete = false;
        spi_receive_it(&hspi1, 5);

        // Wait for reception to complete
        while (!rx_complete) {
            // Do other work here
        }

        // Set CS high (end communication)
        spi_set_cs(&hspi1, true);

        delay_ms(1000);
    }
}

/* ============================== Interrupt Handler ============================== */
void SPI1_IRQHandler(void)
{
    spi_irq_handler(&hspi1);
}

/* ============================== Configuration Functions ============================== */
static void system_init(void)
{
    // Initialize ring buffers
    ring_buffer_init(&tx_ring_buffer);
    ring_buffer_init(&rx_ring_buffer);
}

static void spi_config_init(void)
{
    spi_config_t spi_config = {
        .instance = SPI1,
        .prescaler = SPI_PRESCALER_8,    // 84MHz/8 = 10.5MHz
        .cpol = SPI_CPOL_LOW,
        .cpha = SPI_CPHA_1EDGE,
        .datasize = SPI_DATASIZE_8BIT,
        .firstbit = SPI_FIRSTBIT_MSB,
        .nss = SPI_NSS_SOFT,
        .pins = {
            .sclk_port = GPIOA,
            .sclk_pin = 5,
            .sclk_af = 5,

            .mosi_port = GPIOA,
            .mosi_pin = 7,
            .mosi_af = 5,

            .miso_port = GPIOA,
            .miso_pin = 6,
            .miso_af = 5,

            .cs_port = GPIOA,
            .cs_pin = 4
        }
    };

    spi_init(&hspi1, &spi_config, &tx_ring_buffer, &rx_ring_buffer);

    // Set callbacks
    hspi1.tx_complete_callback = spi_tx_complete_callback;
    hspi1.rx_complete_callback = spi_rx_complete_callback;
    hspi1.tx_rx_complete_callback = spi_tx_rx_complete_callback;
    hspi1.error_callback = spi_error_callback;
}

static void nvic_config(void)
{
    // Enable SPI1 interrupt
    NVIC->ISER[1] |= (1 << (SPI1_IRQn - 32));

    // Set priority
    NVIC->IP[SPI1_IRQn] = (5 << 4);
}

static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 1000; i++) {
        __asm("nop");
    }
}

/* ============================== Callback Functions ============================== */
static void spi_tx_complete_callback(void)
{
    tx_complete = true;
}

static void spi_rx_complete_callback(void)
{
    rx_complete = true;
}

static void spi_tx_rx_complete_callback(void)
{
    tx_rx_complete = true;
}

static void spi_error_callback(void)
{
    // Handle error
}
