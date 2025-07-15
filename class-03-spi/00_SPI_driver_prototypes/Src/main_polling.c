/**
 * @file main_polling.c
 * @brief SPI Driver Polling Mode Example
 * @author Drivers Development Course
 * @date 2024
 */

#include "stm32f411_base.h"
#include "spi_polling.h"
#include "ring_buffer.h"

/* ============================== Global Variables ============================== */
static spi_handle_t hspi1;
static ring_buffer_t tx_ring_buffer;
static ring_buffer_t rx_ring_buffer;


/* ============================== Function Prototypes ============================== */
static void system_init(void);
static void spi_config_init(void);
static void delay_ms(uint32_t ms);

/* ============================== Main Function ============================== */
int main(void)
{
    // Initialize system
    system_init();

    // Initialize SPI
    spi_config_init();

    // Test data
    uint8_t tx_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    uint8_t rx_data[5] = {0};

    while (1) {
        // Set CS low (start communication)
        spi_set_cs(&hspi1, false);
        delay_ms(1);

        // Transmit data in polling mode
        spi_status_t status = spi_transmit_polling(&hspi1, tx_data, sizeof(tx_data), 1000);

        if (status == SPI_STATUS_OK) {
            // Transmission successful
        }

        delay_ms(10);

        // Receive data in polling mode
        status = spi_receive_polling(&hspi1, rx_data, sizeof(rx_data), 1000);

        if (status == SPI_STATUS_OK) {
            // Reception successful
        }

        // Set CS high (end communication)
        spi_set_cs(&hspi1, true);

        delay_ms(1000);
    }
}

/* ============================== Configuration Functions ============================== */
static void system_init(void)
{
    // Initialize ring buffers with actual buffer arrays
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
}

static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 1000; i++) {
        __asm("nop");
    }
}
