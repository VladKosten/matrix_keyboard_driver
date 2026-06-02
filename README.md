![CI Develop](https://github.com/LTD-PASSATINNOVATION/matrix_keyboard_driver/actions/workflows/CI-develop.yml/badge.svg)
![CI Release](https://github.com/LTD-PASSATINNOVATION/matrix_keyboard_driver/actions/workflows/CI-release.yml/badge.svg)

# Matrix Keyboard Driver

This is a simple matrix keyboard driver. It is designed to be used with a 6x6 matrix keyboard, but it can be easily adapted to other sizes. It can be used with any type of key (push button, membrane, etc). It can be used with any microcontroller and any RTOS. It is thread safe and multi instance. It has independent thread for polling the keyboard and ring buffer for last pressed keys.

## Table of contents

- [Features](#features)
- [Dependencies](#dependencies)
- [Get started](#get-started)
- [Unit tests](#unit-tests)
- [Contributing](#contributing)

## Features

- OS abstraction layer
- Hardware abstraction layer
- Thread safe support
- Multiply Instance support
- Independent thread for polling the keyboard.
- Ring buffer for last pressed keys.

## Dependencies

- [FreeRTOS](https://www.freertos.org/) (Optional for port layer)
- [Advanced Software Framework](https://asf.microchip.com/docs/latest/) (Optional for port layer)
- [CMake](https://cmake.org/) (Optional for building unit tests)
- [Unity](https://github.com/ThrowTheSwitch/Unity) (Optional for unit tests)
- [Cmock](https://github.com/ThrowTheSwitch/CMock) (Optional for generating mocks for unit tests)
- [gcc](https://gcc.gnu.org/) (Optional for building unit tests)
- [CppCheck](https://cppcheck.sourceforge.io/) (Continuous integration)
- [Doxygen](https://www.doxygen.nl/index.html)

## Get started

1. Clone the repository
2. Include the header file in your project and link the source files to your project.
3. Implement the port layer functions.
4. Call the initialization function.
5. Use the API functions.

## Unit tests

1. Clone the repository
2. Generate the build files: `cmake . -B build -G Ninja`
3. Build the unit tests: `cmake --build build`
4. Run the unit tests: `./build/test/test_runner/matrix_kbd_test_run`

## Contributing

To contribute to the project, follow these steps:

1. Create a new branch(like `feature/feature_name` or `bugfix/bug_name`) from the `develop` branch.
2. Make your changes, and commit them.
3. Push your changes to the repository.
4. Create a pull request to the `develop` branch.
