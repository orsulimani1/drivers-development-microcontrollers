# Course Assignments - Class #0
**Due: Before next class**

## Assignment 1: EXTI Button Interrupt (Hardware + Code)

### Objective
Configure an external interrupt on PA1 with internal pull-up and handle button press using a wire short to ground.

### Requirements
1. **Hardware Setup**:
   - Connect a wire to PXx pin on your STM32F411E-DISCO board / or any other STM board you have
   - Use internal pull-up resistor (no external components needed)
   - Simulate button press by briefly touching wire to GND

2. **Software Implementation**:
   - Create bare-metal code (no STM32 libraries)
   - Configure EXTIx for falling edge interrupt
   - Toggle onboard LED on each interrupt

3. **Testing**:
   - Verify interrupt triggers on wire-to-ground contact
   - LED should toggle with each "button press"
   - Test multiple rapid presses

### Deliverables
- Complete `.c` file with your implementation
- Brief comment explaining your debouncing approach
- Video/photo showing LED toggle on wire contact

---

## Assignment 2: Communication Interface Header Design

### Objective
Design a driver API header file for a communication interface.
You can choose a communication we'll cover later in class or something else.

### Options (Choose One)
- **I2C Master Driver**
- **SPI Master Driver** 
- **UART Driver**
- **Something else**

### Requirements
1. **Header File Structure**:
   - Configuration structure with relevant parameters
   - Status/error code enumeration
   - Complete function prototypes (init, deinit, transmit, receive, etc.)
   - Proper include guards and documentation

2. **API Design Principles**:
   - Consistent naming convention
   - Clear parameter types
   - Return status codes for all functions
   - Configuration flexibility

3. **Reference**: Use the RS485 example from class as a template

### Deliverables
- Single `.h` header file
- Brief comments explaining your design choices
- No implementation required - header only

---

## Assignment 3: Component Integration Research

### Objective
Research and analyze integration requirements for real components with your STM32F411E board.

### Task
Choose **3 different components** from different categories:
- **Sensor** (temperature, accelerometer, pressure, etc.)
- **Display** (LCD, OLED, LED matrix, etc.)  
- **Communication/Storage** (WiFi module, SD card, EEPROM, etc.)

### Requirements for Each Component
1. **Find actual part numbers** and datasheets
2. **Write one paragraph** covering:
   - Communication interface used (I2C, SPI, UART, etc.)
   - Pin connections to STM32 Board
   - Power requirements and compatibility
   - Key integration challenges or considerations
   - Estimated development complexity

### Deliverables
- Google Doc with 3 paragraphs (one per component)
- Include component part numbers and datasheet links
- Share document link in class forum or submit as instructed

### Evaluation Criteria
- No wrong answers - focus on learning
- Thoughtful analysis of integration requirements
- Consideration of electrical and software challenges
- Optional: Review feedback available next class

---

## Submission Guidelines

1. **File Organization**:
   ```
   assignment-1/
   ├── main.c
   └── README.md
   
   assignment-2/
   ├── [interface]_driver.h
   └── design_notes.txt
   
   assignment-3/
   └── [shared Google Doc link]
   ```

2. **Code Standards**:
   - Use bare-metal register manipulation
   - Add meaningful comments
   - Follow consistent naming conventions
   - Include basic error handling

3. **Testing**:
   - Verify all code compiles
   - Test on actual hardware where applicable
   - Document any issues encountered

## Resources

- [Course Repository](https://github.com/orsulimani1/drivers-development-microcontrollers)
- STM32F411E-DISCO User Manual
- STM32F411 Reference Manual (for register details)
- Class presentation materials

## Questions?

Post in the course forum or WhatsApp group. Include:
- Assignment number
- Specific question or issue
- What you've already tried

**Good luck and have fun with hands-on embedded development!**