# Class #1 Assignment - GPIO Driver Development
**Due: Before next class**

## Overview
Building on today's GPIO concepts, you will implement a complete GPIO driver with interrupt support, LED control, and button handling for the STM32F411E-DISCO board.

---

## Assignment Requirements

### 1. Complete GPIO Driver Implementation

#### Core Driver Files
Create the following file structure:
```
assignment-1/
├── Inc/
│   ├── gpio_driver.h
│   ├── gpio_driver_interrupt.h
│   └── stm32f411_base.h
├── Src/
│   ├── gpio_driver.c
│   ├── gpio_driver_interrupt.c
│   └── main.c
```

#### Required Functions in gpio_driver.h/c

**Configuration Functions:**
```c
// Initialize GPIO pin with configuration
gpio_status_t gpio_init(const gpio_config_t *config);

// Deinitialize GPIO pin (reset to default state)
gpio_status_t gpio_deinit(GPIO_TypeDef *port, uint8_t pin);

// Enable GPIO port clock
static gpio_status_t gpio_enable_clock(GPIO_TypeDef *port);

// Disable GPIO port clock  
static gpio_status_t gpio_disable_clock(GPIO_TypeDef *port);
```

**I/O Functions:**
```c
// Read GPIO pin state
bool gpio_read_pin(GPIO_TypeDef *port, uint8_t pin);

// Write GPIO pin state
void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, bool state);

// Toggle GPIO pin state
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);

// Read entire GPIO port
uint16_t gpio_read_port(GPIO_TypeDef *port);

// Write entire GPIO port
void gpio_write_port(GPIO_TypeDef *port, uint16_t value);
```

**Configuration Helper Functions:**
```c
// Set GPIO mode (input/output/alternate/analog)
static gpio_status_t gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode);

// Set output type (push-pull/open-drain)
static gpio_status_t gpio_set_output_type(GPIO_TypeDef *port, uint8_t pin, gpio_otype_t otype);

// Set output speed
static gpio_status_t gpio_set_speed(GPIO_TypeDef *port, uint8_t pin, gpio_speed_t speed);

// Set pull-up/pull-down configuration
static gpio_status_t gpio_set_pull(GPIO_TypeDef *port, uint8_t pin, gpio_pull_t pull);

// Set alternate function
static gpio_status_t gpio_set_alternate_function(GPIO_TypeDef *port, uint8_t pin, uint8_t af);
```

#### Code Comments for Init/Deinit Functions

**gpio_init() Implementation Notes:**
```c
gpio_status_t gpio_init(const gpio_config_t *config) {
    // TODO: Validate input parameters (config pointer, port, pin range)
    
    // TODO: Enable GPIO port clock using RCC register
    // Hint: Use RCC->AHB1ENR register with appropriate bit
    
    // TODO: Configure GPIO mode using MODER register
    // Hint: Clear existing 2-bit field, then set new mode
    
    // TODO: Configure pull-up/pull-down using PUPDR register
    // Hint: Similar to MODER, but for pull configuration
    
    // TODO: For output modes, configure output type using OTYPER
    // Hint: Single bit per pin (0=push-pull, 1=open-drain)
    
    // TODO: For output modes, configure speed using OSPEEDR
    // Hint: 2-bit field per pin for speed selection
    
    // TODO: For alternate function mode, configure AFR registers
    // Hint: Use AFR[0] for pins 0-7, AFR[1] for pins 8-15
    
    return GPIO_OK;
}
```

**gpio_init() Implementation Notes:**
```c
gpio_status_t gpio_mod(const gpio_config_t *config) {
    // TODO: Validate input parameters (config pointer, port, pin range)
    
    // TODO: Configure GPIO mode using MODER register
    // Hint: Clear existing 2-bit field, then set new mode
    
    // TODO: Configure pull-up/pull-down using PUPDR register
    // Hint: Similar to MODER, but for pull configuration
    
    // TODO: For output modes, configure output type using OTYPER
    // Hint: Single bit per pin (0=push-pull, 1=open-drain)
    
    // TODO: For output modes, configure speed using OSPEEDR
    // Hint: 2-bit field per pin for speed selection
    
    // TODO: For alternate function mode, configure AFR registers
    // Hint: Use AFR[0] for pins 0-7, AFR[1] for pins 8-15
    
    return GPIO_OK;
}
```

**gpio_deinit() Implementation Notes:**
```c
gpio_status_t gpio_deinit(GPIO_TypeDef *port, uint8_t pin) {
    // TODO: Validate input parameters
    
    // TODO: Reset pin to default state (analog mode, no pull)
    // Hint: Set MODER to 11 (analog), PUPDR to 00 (no pull)
    
    // TODO: Clear output type and speed settings
    // Hint: Set OTYPER bit to 0, OSPEEDR to 00
    
    // TODO: Clear alternate function setting
    // Hint: Set appropriate AFR bits to 0000
    
    // TODO: Optionally disable port clock if no other pins are used
    // Advanced: Check if other pins in port are still configured
    
    return GPIO_OK;
}
```

#### Required Functions in gpio_driver_interrupt.h/c

```c
// Initialize GPIO pin with interrupt configuration
gpio_status_t gpio_interrupt_init(const gpio_interrupt_config_t *config);

// Deinitialize GPIO interrupt
gpio_status_t gpio_interrupt_deinit(uint8_t pin);

// Enable/disable GPIO interrupt
gpio_status_t gpio_interrupt_enable(uint8_t pin);
gpio_status_t gpio_interrupt_disable(uint8_t pin);

// Register/unregister callback function
gpio_status_t gpio_register_callback(uint8_t pin, void (*callback)(uint8_t));
gpio_status_t gpio_unregister_callback(uint8_t pin);
```

---

### 2. LED Control Implementation

#### Requirements
Implement control for **multiple LEDs** with different configurations:

**LED Configurations:**
```c
// Onboard LED (PC13) - Active LOW
gpio_config_t onboard_led = {
    .port = GPIOC,
    .pin = 13,
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OTYPE_PUSH_PULL,
    .speed = GPIO_SPEED_LOW,
    .pull = GPIO_PULL_NONE
};

// External LED 1 (PA5) - Active HIGH, High Speed
gpio_config_t external_led1 = {
    .port = GPIOA,
    .pin = 5,
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OTYPE_PUSH_PULL,
    .speed = GPIO_SPEED_HIGH,
    .pull = GPIO_PULL_NONE
};
```

**LED Control Functions:**
```c
// LED control enumeration
typedef enum {
    LED_ONBOARD = 0,
    LED_EXTERNAL1 = 1,
    LED_EXTERNAL2 = 2,
    LED_COUNT = 3
} led_id_t;

// LED control functions
void led_init_all(void);
void led_on(led_id_t led);
void led_off(led_id_t led);
void led_toggle(led_id_t led);
void led_set_state(led_id_t led, bool state);
```

**Demonstration Sequence:**
1. Initialize all LEDs
2. Blink each LED individually (2 second intervals)
3. Create a "running light" pattern across all LEDs
4. Test rapid toggling to verify speed configurations

---

### 3. Button Interrupt with Debouncing

#### Button Configuration
```c
// Button on PA1 (wire to ground simulation)
gpio_interrupt_config_t button_config = {
    .gpio_config = {
        .port = GPIOA,
        .pin = 1,
        .mode = GPIO_MODE_INPUT,
        .pull = GPIO_PULL_UP
    },
    .trigger_type = EXTI_TRIGGER_FALLING,
    .priority = 5,
    .callback = button_pressed_callback
};
```

#### Debouncing Requirements

**Software Debouncing Implementation:**
```c
// Debouncing parameters
#define DEBOUNCE_DELAY_MS    50
#define DEBOUNCE_SAMPLES     5

// Debouncing state structure
typedef struct {
    uint32_t last_interrupt_time;
    uint8_t  sample_count;
    bool     last_stable_state;
    bool     current_state;
} debounce_state_t;

// Debouncing functions
void debounce_init(void);
bool debounce_process(uint8_t pin);
void debounce_reset(uint8_t pin);
```

**Debouncing Algorithm:**
1. **Time-based filtering**: Ignore interrupts within 50ms of last valid press
2. **Sample-based filtering**: Require 5 consecutive stable readings
3. **State validation**: Only trigger callback on confirmed state change

#### Button Functionality
- **Single Press**: Cycle through LED patterns
- **Long Press (>1 second)**: Toggle all LEDs simultaneously
- **Double Press**: Reset to default pattern

---

### 4. Implementation Details

#### Error Handling
```c
// Comprehensive error checking
typedef enum {
    GPIO_OK = 0,
    GPIO_ERROR = 1,
    GPIO_ERROR_INVALID_PARAM = 2,
    GPIO_ERROR_INVALID_PORT = 3,
    GPIO_ERROR_INVALID_PIN = 4,
    GPIO_ERROR_CLOCK_DISABLED = 5,
    GPIO_ERROR_INTERRUPT_CONFIG = 6
} gpio_status_t;
```

#### Performance Requirements
- **Toggle frequency**: Minimum 1kHz for high-speed pins
- **Interrupt latency**: Maximum 10μs from trigger to callback execution
- **Debounce response**: Maximum 100ms from button press to action

#### Power Optimization
- Use lowest appropriate speed settings for each LED
- Configure unused pins as analog inputs
- Implement proper clock gating

---

### 5. Deliverables

#### Code Files
- Complete driver implementation with all required functions
- Well-commented code following consistent naming conventions
- Error handling for all edge cases
- Efficient register manipulation techniques

#### Documentation
**README.md should include:**
- Project overview and functionality description
- Hardware connections and pin assignments
- Usage examples and API reference
- Known limitations and future improvements

---

### 7. Bonus Challenges (Optional)

#### Advanced Features
1. **PWM-like LED dimming** using software timing
2. **Multiple button support** with different functionalities
3. **LED pattern programming** via button sequences
4. **Power state management** with sleep/wake functionality

#### Optimization Challenges
1. **Minimize code size** while maintaining functionality
2. **Optimize for speed** - fastest possible toggle rates
3. **Ultra-low power** configuration for battery applications

---

### 8. Resources and References

#### Documentation
- **STM32F411 Reference Manual**: GPIO and EXTI sections
- **STM32F411E-DISCO User Manual**: Pin assignments and board schematics
- **Course Repository**: Base register definitions and examples

#### Development Tools
- **STM32CubeIDE**: Primary development environment
- **ST-Link Utility**: Programming and debugging
- **Logic Analyzer/Oscilloscope**: Signal verification

#### Support
- **Course Forum**: Technical questions and discussions
- **WhatsApp Group**: Quick questions and peer support
- **Office Hours**: Schedule via email for complex issues

---

### 9. Common Pitfalls to Avoid

#### Hardware Issues
- **Clock not enabled**: GPIO won't respond without proper clock configuration
- **Incorrect pin mapping**: Verify STM32F411E-DISCO pin assignments
- **Pull resistor confusion**: Active-low buttons need pull-up configuration

#### Software Issues
- **Race conditions**: Use atomic operations (BSRR) for critical timing
- **Interrupt storms**: Implement proper debouncing to prevent false triggers
- **Memory management**: Static allocation for interrupt handlers

#### Integration Problems
- **Priority conflicts**: Configure NVIC priorities appropriately
- **Shared resources**: Handle multiple pins on same EXTI line correctly
- **Power management**: Balance performance vs. power consumption

---

**Good luck with your GPIO driver implementation!**  
*This assignment will build the foundation for all future peripheral drivers in the course.*