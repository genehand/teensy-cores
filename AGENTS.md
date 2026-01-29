# Agent Guidelines for Teensy Core Libraries

This document provides guidelines for AI coding agents working on the Teensy core libraries for Arduino.

## Project Overview

**Language:** C/C++ (embedded systems)  
**Platform:** Teensy microcontrollers (2.0, LC, 3.x, 4.x series)  
**Purpose:** Arduino core libraries for Teensy boards manufactured by PJRC.COM  
**License:** Custom permissive license (see file headers)

## Project Structure

```
teensy/     - Teensy 2.0 core (legacy, AVR-based)
teensy3/    - Teensy 3.x core (Cortex-M4/M0+)
teensy4/    - Teensy 4.x core (Cortex-M7, 600MHz)
usb_*/      - USB device implementations (serial, MIDI, HID, etc.)
```

## Build System

### Build Commands

This project uses traditional Makefiles with ARM GCC toolchain.

```bash
# Build for Teensy 3.x
cd teensy3/
make                    # Build main.hex with default settings
make MCU=MK20DX256     # Build for specific MCU
make clean             # Remove build artifacts

# Build for Teensy 4.x
cd teensy4/
make                    # Build main.hex (Teensy 4.0 by default)
make clean             # Remove build artifacts
```

### Compiler Details

- **Toolchain:** arm-none-eabi-gcc/g++
- **C++ Standard:** gnu++17
- **Teensy 3.x:** `-Os -mcpu=cortex-m4 -mthumb`
- **Teensy 4.x:** `-O2 -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16`

### No Automated Testing

**Important:** This project has no unit testing framework. Testing is performed manually on physical hardware. Do not attempt to create or run automated tests.

### No Linting/Formatting Tools

**Important:** This project does not use clang-format, clang-tidy, or similar tools. Follow the observed code style patterns instead.

## Code Style Guidelines

### File Organization

- **Headers:** `.h` files with `#pragma once` or include guards `#ifndef _filename_h_`
- **Implementation:** Corresponding `.c` (C code) or `.cpp` (C++ code) files
- **Platform-specific:** Separate implementations in teensy3/ vs teensy4/ directories
- **USB modules:** Modular implementations in separate usb_* directories

### Naming Conventions

- **Macros/Constants:** `UPPERCASE_WITH_UNDERSCORES` (e.g., `HIGH`, `LOW`, `USB_SERIAL`)
- **Classes:** `PascalCase` (e.g., `HardwareSerial`, `Print`, `Stream`)
- **Functions:** `camelCase` (e.g., `digitalWrite`, `pinMode`, `availableForWrite`)
- **Variables:** `lowercase_with_underscores` or `camelCase`
- **Private members:** No specific prefix convention observed
- **Pin macros:** `CORE_PIN0_BIT`, `CORE_PIN0_BITMASK` pattern

### Formatting

- **Indentation:** Tabs (not spaces)
- **Braces:** K&R style - opening brace on same line
  ```cpp
  void function() {
      // code
  }
  ```
- **Line length:** No strict limit, typically ~80-100 characters
- **Spacing:** Space after `if`, `while`, `for`; no space after function names
  ```cpp
  if (condition) {  // YES
  if(condition) {   // NO
  ```

### Header Files

- Use `#pragma once` (preferred in newer files) or traditional include guards
- Include guard format: `#ifndef _filename_h_` followed by `#define _filename_h_`
- Include order:
  1. Arduino/core headers (`<Arduino.h>`, `<core_pins.h>`)
  2. System headers (`<stdint.h>`, `<string.h>`)
  3. Local headers (`"specific_header.h"`)

### Comments and Documentation

- **File headers:** Block comments with copyright notice (PJRC.COM, LLC)
- **Required license header:** Include custom license text in all new files (see existing files)
- **Inline comments:** `//` for single-line comments
- **Block comments:** `/* */` for multi-line explanations
- **API documentation:** Brief inline comments above function declarations

Example license header:
```cpp
/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2024 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction...
 */
```

### Types

- Prefer fixed-width types: `uint8_t`, `uint16_t`, `uint32_t`, `int32_t`, etc.
- Use `size_t` for sizes and array indices
- Avoid `int` for hardware-related code; use explicit types
- Use `constexpr` for compile-time constants in C++ code

### Memory and Performance

- **Minimal overhead:** This is embedded code; optimize for size and speed
- **Static allocation:** Prefer static/stack allocation over dynamic (heap)
- **Volatile:** Use `volatile` for hardware registers and interrupt-shared variables
- **Inline:** Small, frequently-called functions should be inline or in headers
- **Flash vs RAM:** Be mindful of flash/RAM usage on constrained MCUs

### Error Handling

- **Return codes:** Functions typically return status (0 for success, -1/error codes for failure)
- **Null checks:** Check pointer parameters: `if (ptr == nullptr) return;`
- **No exceptions:** C++ exceptions are disabled (`-fno-exceptions`)
- **Assertions:** Minimal use; prefer graceful degradation for production code

### Platform-Specific Code

Use conditional compilation for platform differences:

```cpp
#if defined(__IMXRT1062__)  // Teensy 4.x
    // Teensy 4 implementation
#elif defined(__MK20DX256__)  // Teensy 3.2
    // Teensy 3 implementation
#endif
```

### Arduino API Compatibility

- Follow Arduino API conventions for compatibility
- Use Arduino types: `String`, `Print`, `Stream` classes
- Implement standard functions: `pinMode()`, `digitalWrite()`, `analogRead()`
- Maintain backward compatibility with existing sketches

## Development Workflow

### Making Changes

1. **Identify target platform:** Determine if changes affect teensy3/, teensy4/, or both
2. **Maintain compatibility:** Preserve existing API behavior
3. **Test on hardware:** Changes must be tested on physical Teensy boards
4. **No CI/CD:** Manual validation only

### Git and GitHub

- **Issues:** Only for specific bug reports, not general troubleshooting
- **Forum first:** Discuss features on PJRC forum before opening GitHub issues
- **Commit messages:** Clear, concise descriptions of changes
- **Upstream:** This is a fork of https://github.com/PaulStoffregen/cores

### Important Constraints

- **No breaking changes:** Maintain API compatibility
- **Hardware-specific:** Changes often require specific Teensy board models
- **USB configurations:** Multiple USB device types require careful handling
- **License compliance:** Must include PJRC devices in build system target lists

## Common Patterns

### USB Device Implementations

Each USB device type (serial, MIDI, HID) has its own directory:
- `usb_api.cpp` - High-level API
- `usb_*.c` - Low-level USB descriptors and handlers

### Hardware Abstraction

- `core_pins.h` - Pin definitions and macros
- `HardwareSerial.cpp` - UART wrappers
- Direct register access via `imxrt.h` (Teensy 4) or MCU-specific headers

### Interrupt Handling

- Functions used in ISRs should be fast and minimal
- Use `__attribute__((interrupt))` where appropriate
- Avoid function calls from ISRs when possible

## Reference Resources

- **Main site:** http://www.pjrc.com/teensy/
- **Forum:** https://forum.pjrc.com/
- **Troubleshooting:** https://www.pjrc.com/teensy/troubleshoot.html
- **Teensyduino:** http://www.pjrc.com/teensy/td_download.html

## AI Agent Reminders

- This is embedded C/C++ - optimize for constrained resources
- No unit tests exist; do not create test infrastructure
- Build artifacts: `*.elf`, `*.hex`, `*.o`, `*.d` (excluded from git)
- When in doubt about style, match the surrounding code exactly
- Always include the required license header in new files
