cmake_minimum_required(VERSION 3.14...3.30)

project(MatrixKbd
    VERSION 0.1.0
    LANGUAGES C
    DESCRIPTION "Portable matrix keyboard driver for embedded systems"
)

# ─── Build options ────────────────────────────────────────────────────────────
option(MATRIX_KBD_BUILD_FOR_PROJECT  "Build for real hardware (ASF HAL + FreeRTOS)" OFF)
option(MATRIX_KBD_BUILD_CORE_ONLY    "Build only core libraries, skip port layers"  OFF)
option(MATRIX_KBD_BUILD_TESTS        "Build unit tests"                             ON)

# ─── Global compile options ──────────────────────────────────────────────────
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# ─── FetchContent (used by test dependencies) ───────────────────────────────
include(FetchContent)

# ─── Subdirectories ─────────────────────────────────────────────────────────
add_subdirectory(main)

# FreeRTOSConfig must be defined before portable (OSAL depends on it)
if(MATRIX_KBD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test/FreeRTOSConfig)
endif()

add_subdirectory(portable)

if(MATRIX_KBD_BUILD_TESTS)
    add_subdirectory(test)
endif()
