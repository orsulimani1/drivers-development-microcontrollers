#include "spi_common.h"

// Common Functions
spi_status_t spi_deinit(spi_handle_t *hspi);
void spi_set_cs(spi_handle_t *hspi, bool state);
spi_state_t spi_get_state(spi_handle_t *hspi);


/* ============================== Function Prototypes ============================== */
static spi_status_t spi_configure_gpio(const spi_pins_t *pins);
spi_status_t spi_configure_peripheral(SPI_TypeDef *spi, const spi_config_t *config);
void spi_enable_clock(SPI_TypeDef *spi);
void spi_disable_clock(SPI_TypeDef *spi);
spi_status_t spi_wait_flag(SPI_TypeDef *spi, uint32_t flag, bool state, uint32_t timeout);
void spi_clear_ovrflag(SPI_TypeDef *spi);

/* ============================== Common Functions ============================== */

/**
 * @brief Initialize SPI peripheral with ring buffers
 * @param hspi: SPI handle pointer
 * @param config: SPI configuration structure
 * @param tx_rb: Transmit ring buffer pointer
 * @param rx_rb: Receive ring buffer pointer
 * @return spi_status_t: Operation status
 */
spi_status_t spi_init(spi_handle_t *hspi, const spi_config_t *config, ring_buffer_t *tx_rb, ring_buffer_t *rx_rb)
{
    if (!hspi || !config || !config->instance || !tx_rb || !rx_rb) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    // Copy configuration
    hspi->instance = config->instance;
    hspi->config = *config;
    hspi->state = SPI_STATE_RESET;
    
    // Initialize ring buffers
    hspi->tx_ring_buffer = tx_rb;
    hspi->rx_ring_buffer = rx_rb;
    
    // Initialize transfer tracking
    hspi->tx_remaining = 0;
    hspi->rx_expected = 0;
    hspi->single_transfer_mode = false;
    
    // Initialize callback pointers to NULL
    hspi->tx_complete_callback = NULL;
    hspi->rx_complete_callback = NULL;
    hspi->tx_rx_complete_callback = NULL;
    hspi->error_callback = NULL;
    hspi->rx_data_ready_callback = NULL;
    
    // Enable peripheral clock
    spi_enable_clock(hspi->instance);
    
    // Configure GPIO pins
    spi_status_t status = spi_configure_gpio(&config->pins);
    if (status != SPI_STATUS_OK) {
        return status;
    }
    
    // Configure SPI peripheral
    status = spi_configure_peripheral(hspi->instance, config);
    if (status != SPI_STATUS_OK) {
        return status;
    }
    
    // Enable SPI peripheral
    hspi->instance->CR1.fields.SPE = 1;
    
    hspi->state = SPI_STATE_READY;
    return SPI_STATUS_OK;
}

/**
 * @brief Deinitialize SPI peripheral
 * @param hspi: SPI handle pointer
 * @return spi_status_t: Operation status
 */
spi_status_t spi_deinit(spi_handle_t *hspi)
{
    if (!hspi || !hspi->instance) {
        return SPI_STATUS_INVALID_PARAM;
    }
    
    // Disable SPI peripheral
    hspi->instance->CR1.fields.SPE = false;
    
    // Disable all interrupts
    hspi->instance->CR2.fields.TXEIE = false;
    hspi->instance->CR2.fields.RXNEIE = 0;
    hspi->instance->CR2.fields.ERRIE = 0;
    
    // Reset registers to default values
    hspi->instance->CR1.value =0x0000;
    hspi->instance->CR2.value = 0x0000;
    
    // Clear status flags
    volatile uint32_t temp = hspi->instance->DR;  // Read DR to clear RXNE
    temp = hspi->instance->SR.value;                    // Read SR to clear other flags
    (void)temp;  // Suppress unused variable warning
    
    // Disable peripheral clock
    spi_disable_clock(hspi->instance);
    
    hspi->state = SPI_STATE_RESET;
    return SPI_STATUS_OK;
}

/**
 * @brief Control chip select signal (software NSS mode)
 * @param hspi: SPI handle pointer
 * @param state: false = CS LOW (active), true = CS HIGH (inactive)
 */
void spi_set_cs(spi_handle_t *hspi, bool state)
{
    if (!hspi || hspi->config.nss != SPI_NSS_SOFT) {
        return;
    }
    
    if (state) {
        // Set CS HIGH (inactive)
        hspi->config.pins.cs_port->BSRR = (1UL << hspi->config.pins.cs_pin);
    } else {
        // Set CS LOW (active)
        hspi->config.pins.cs_port->BSRR = (1UL << (hspi->config.pins.cs_pin + 16));
    }
}

/**
 * @brief Get current SPI state
 * @param hspi: SPI handle pointer
 * @return spi_state_t: Current driver state
 */
spi_state_t spi_get_state(spi_handle_t *hspi)
{
    if (!hspi) {
        return SPI_STATE_RESET;
    }
    return hspi->state;
}



/**
 * @brief Check if SPI is busy
 * @param hspi: SPI handle pointer
 * @return bool: true if busy, false if ready
 */
bool spi_is_busy(spi_handle_t *hspi)
{
    if (!hspi) {
        return false;
    }
    return (hspi->state != SPI_STATE_READY && hspi->state != SPI_STATE_RESET);
}

/**
 * @brief Get SPI clock frequency
 * @param spi: SPI peripheral pointer
 * @return uint32_t: Clock frequency in Hz
 */
uint32_t spi_get_clock_freq(SPI_TypeDef *spi)
{
    uint32_t pclk;
    uint32_t prescaler_value;
    
    // Determine which APB bus the SPI is on
    if (spi == SPI1) {
        pclk = 84000000;  // APB2 clock (assuming 84MHz)
    } else {
        pclk = 42000000;  // APB1 clock (assuming 42MHz)
    }
    
    // Get prescaler value from BR bits
    uint32_t br = (spi->CR1.value & SPI_CR1_BR) >> SPI_CR1_BR_Pos;
    prescaler_value = 2 << br;  // 2^(br+1)
    
    return pclk / prescaler_value;
}


/**
 * @brief Configure GPIO pins for SPI using GPIO driver
 * @param pins: Pin configuration structure
 * @return spi_status_t: Operation status
 */
static spi_status_t spi_configure_gpio(const spi_pins_t *pins)
{
    gpio_config_t gpio_config;
    
    // Configure SCLK pin
    gpio_config.port = pins->sclk_port;
    gpio_config.pin = pins->sclk_pin;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.output_type = GPIO_OTYPE_PUSH_PULL;
    gpio_config.speed = GPIO_SPEED_HIGH;
    gpio_config.pull = GPIO_PULL_NONE;
    gpio_config.alternate = pins->sclk_af;
    
    if (gpio_init(&gpio_config) != GPIO_OK) {
        return SPI_STATUS_ERROR;
    }
    
    // Configure MOSI pin
    gpio_config.port = pins->mosi_port;
    gpio_config.pin = pins->mosi_pin;
    gpio_config.alternate = pins->mosi_af;
    
    if (gpio_init(&gpio_config) != GPIO_OK) {
        return SPI_STATUS_ERROR;
    }
    
    // Configure MISO pin
    gpio_config.port = pins->miso_port;
    gpio_config.pin = pins->miso_pin;
    gpio_config.pull = GPIO_PULL_UP;  // Pull-up for MISO
    gpio_config.alternate = pins->miso_af;
    
    if (gpio_init(&gpio_config) != GPIO_OK) {
        return SPI_STATUS_ERROR;
    }
    
    // Configure CS pin (if software NSS)
    if (pins->cs_port) {
        gpio_config.port = pins->cs_port;
        gpio_config.pin = pins->cs_pin;
        gpio_config.mode = GPIO_MODE_OUTPUT;
        gpio_config.pull = GPIO_PULL_NONE;
        gpio_config.alternate = 0;  // Not used for output mode
        
        if (gpio_init(&gpio_config) != GPIO_OK) {
            return SPI_STATUS_ERROR;
        }
        
        // Set CS high initially (inactive)
        gpio_write_pin(pins->cs_port, pins->cs_pin, 1);
    }
    
    return SPI_STATUS_OK;
}


/* Implementation of missing utility function */
void spi_clear_ovrflag(SPI_TypeDef *spi)
{
    /* Clear OVR flag by reading DR then SR */
    volatile uint32_t temp;
    temp = spi->DR;     // Read DR
    temp = spi->SR.value;    // Read SR
    (void)temp;         // Suppress unused variable warning
}


/**
 * @brief Enable SPI peripheral clock
 * @param spi: SPI peripheral pointer
 */
void spi_enable_clock(SPI_TypeDef *spi)
{
    if (spi == SPI1) {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    } else if (spi == SPI2) {
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    } else if (spi == SPI3) {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    } else if(spi == SPI4) {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    }
}

/**
 * @brief Disable SPI peripheral clock
 * @param spi: SPI peripheral pointer
 */
void spi_disable_clock(SPI_TypeDef *spi)
{
    if (spi == SPI1) {
        RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    } else if (spi == SPI2) {
        RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
    } else if (spi == SPI3) {
        RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
    }else if(spi == SPI4) {
        RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
    }
}

/**
 * @brief Configure SPI peripheral registers
 * @param spi: SPI peripheral pointer
 * @param config: SPI configuration structure
 * @return spi_status_t: Operation status
 */
spi_status_t spi_configure_peripheral(SPI_TypeDef *spi, const spi_config_t *config)
{
    // Disable SPI before configuration
    spi->CR1.fields.SPE = 0;

    // Reset CR1 register
    spi->CR1.value = 0;

    // Set clock polarity and phase
    spi->CR1.fields.CPOL = (config->cpol == SPI_CPOL_HIGH) ? 1 : 0;
    spi->CR1.fields.CPHA = (config->cpha == SPI_CPHA_2EDGE) ? 1 : 0;

    // Set master mode
    spi->CR1.fields.MSTR = 1;

    // Set baud rate prescaler
    spi->CR1.fields.BR = config->prescaler;

    // Set bit order
    spi->CR1.fields.LSBFIRST = (config->firstbit == SPI_FIRSTBIT_LSB) ? 1 : 0;

    // Set data frame format
    spi->CR1.fields.DFF = (config->datasize == SPI_DATASIZE_16BIT) ? 1 : 0;

    // Set NSS management
    if (config->nss == SPI_NSS_SOFT) {
        spi->CR1.fields.SSM = 1;
        spi->CR1.fields.SSI = 1;
    }

    // Configure CR2 register
    spi->CR2.value = 0;

    // Enable error interrupts
    spi->CR2.fields.ERRIE = 1;

    return SPI_STATUS_OK;
}
