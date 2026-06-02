# ─── Unity + CMock (fetch once, used by tests) ──────────────────────────────
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.5.2
)

set(UNITY_EXTENSION_FIXTURE "ON" CACHE STRING "" FORCE)
set(UNITY_EXTENSION_MEMORY  "ON" CACHE STRING "" FORCE)

FetchContent_MakeAvailable(unity)

# ─── Test runner ────────────────────────────────────────────────────────────
add_subdirectory(test_runner)

# ─── Driver config stub (empty for host builds) ────────────────────────────
add_library(DriverConfig INTERFACE)
target_include_directories(DriverConfig INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ─── Unit test library ─────────────────────────────────────────────────────
add_library(MatrixKbdUnitTest STATIC
    matrix_kbd_osal_test.c
    matrix_kbd_osal_freertos_test.c
    matrix_kbd_hal_test.c
    matrix_kbd_test.c
)

target_compile_definitions(MatrixKbdUnitTest PUBLIC TEST_ON_HOST)

target_link_libraries(MatrixKbdUnitTest
    PUBLIC MatrixKbd
    PUBLIC MatrixKbdOsalFreeRtos
    PUBLIC MatrixKbdHalPortCmock
)
