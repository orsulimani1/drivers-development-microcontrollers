# Drivers Development on Microcontrollers

A comprehensive course on bare-metal driver development for STM32 microcontrollers, focusing on low-level hardware abstraction and embedded systems programming.

## Course Overview

Learn to develop efficient, portable drivers for microcontroller peripherals without relying on manufacturer HAL libraries. This course emphasizes bare-metal programming, hardware understanding, and professional embedded development practices.

## Prerequisites

- Basic C programming knowledge
- Understanding of digital electronics
- Familiarity with microcontroller concepts
- No prior STM32 experience required

## Hardware Requirements

- **STM32F411E-DISCO** development board (recommended)
- Arduino board (for slave/master emulation)
- USB cables and jumper wires
- Breadboard for prototyping

## Software Setup

### Required Tools
- **STM32Cube IDE** - Primary development environment
- **Git Bash** (Windows) - Version control and terminal
- **Oscilloscope/Logic Analyzer** (optional but recommended)

### Installation Steps
1. Install STM32Cube IDE from [ST website](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Download Git Bash from [git-scm.com](https://git-scm.com/download/win)
3. Clone repository: `git clone https://github.com/orsulimani1/drivers-development-microcontrollers.git`
4. Verify setup: `git --version`

## Course Structure

### Class Topics
1. **Introduction** - Driver concepts and development environment
2. **GPIO Deep Dive** - Pin configuration, control, and optimization
3. **I2C Driver Development** - Master/slave communication implementation
4. **SPI Driver Development** - High-speed serial communication
5. **UART Introduction** - Asynchronous serial communication
6. **STM32 HAL Integration** - Working with manufacturer libraries
7. **Low Power Modes** - Power management and optimization
8. **Personal Projects** - Individual driver implementation
9. **Project Integration** - Testing and system integration

### Learning Objectives
- Develop hardware abstraction layers for any MCU platform
- Master low-level register manipulation and timing
- Implement interrupt-driven communication protocols
- Create portable, reusable driver code
- Understand bus architectures and peripheral interfaces

## Repository Structure

```
drivers-development-microcontrollers/
├── class-00-introduction/
├── class-01-gpio/
├── class-02-i2c/
├── class-03-spi/
├── class-04-uart/
├── class-05-hal/
├── class-06-low-power/
├── class-07-projects/
├── class-08-integration/
├── examples/
├── reference/
└── assignments/
```

## Getting Started

### Before Each Class
1. Fetch latest changes: `git fetch`
2. Switch to class branch: `git checkout class-XX`
3. Review previous materials and assignments

### Development Workflow
1. Create a feature branch for assignments
2. Implement driver functions
3. Test on hardware
4. Commit with descriptive messages
5. Push to personal repository

## Course Projects

### Option 1: Custom Project
- Choose your components and application
- Receive personalized guidance
- Focus on areas of interest

### Option 2: Task Scheduler Integration
- Extend the existing task scheduler project
- Integrate multiple sensor drivers
- Implement real-time data processing
- System-level optimization

## Key Concepts Covered

- **Bare-Metal Programming** - Direct register manipulation
- **Memory Architecture** - Stack, heap, and memory-mapped I/O
- **Bus Interfaces** - AHB/APB architecture and timing
- **Interrupt Handling** - NVIC configuration and ISR development
- **Communication Protocols** - I2C, SPI, UART implementation
- **Hardware Abstraction** - Portable driver design patterns
- **Real-Time Systems** - Timing constraints and optimization

## Code Examples

Each class includes working code examples demonstrating:
- Driver initialization and configuration
- Data transmission and reception
- Interrupt service routines
- Error handling and recovery
- Performance optimization techniques

## Assignment Guidelines

- Complete assignments before the next class
- Test thoroughly on actual hardware
- Document your implementation approach

## Resources

- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Cortex-M4 Programming Manual](https://developer.arm.com/documentation/dui0553/latest/)
- Course presentation materials and examples will be provided to sigend students

## Communication

- **WhatsApp Group** - Real-time questions and discussions
- **Forum** - Technical questions and code review
- **Email**: Orr.Foxx@gmail.com for course-related inquiries

## Assessment

- Practical assignments after each class
- Final project demonstrating driver integration
- Code quality and documentation
- Hardware testing and validation

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Students are encouraged to:
- Report bugs in the example code
- Suggest improvements to course materials
- Share interesting driver implementations
- Help fellow students in forum discussions
