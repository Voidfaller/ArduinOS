# ArduinOS

**A lightweight operating system for Arduino, with a built-in bytecode interpreter.**

ArduinOS is an experimental operating system designed to run on Arduino hardware. The project explores how operating system concepts can be implemented in a resource-constrained environment.

It includes a small bytecode interpreter that allows programs to be represented and executed independently of the main firmware.

## Overview

The project is an exploration of low-level software development, memory constraints and program execution on embedded hardware.

Rather than running a single hardcoded application, ArduinOS provides a small runtime environment capable of interpreting simple bytecode instructions.

## Features

- Lightweight runtime designed for Arduino hardware.
- Custom bytecode interpreter for executing instructions.
- Separation between the operating system and interpreted programs.
- Modular codebase with separate source, header and library directories.

## Project structure

| Directory | Description |
|---|---|
| `src/` | Main application and implementation |
| `include/` | Header files and declarations |
| `lib/` | Supporting libraries |
| `test/` | Tests and experimental code |

## Technologies

- **C++** – Core implementation.
- **Arduino** – Embedded hardware platform.
- **Bytecode interpretation** – Execution of instructions through a custom interpreter.

## Project Status

ArduinOS is an experimental project. Its primary focus is understanding and implementing the fundamentals of program execution and operating system design within the constraints of Arduino hardware.

## License

See [LICENSE](LICENSE) for licensing information.
