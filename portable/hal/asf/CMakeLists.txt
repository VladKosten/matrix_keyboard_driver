# ─── ASF HAL port (for Microchip/Atmel hardware) ────────────────────────────
add_library(MatrixKbdHalPort STATIC
    matrix_kbd_hal_asf.c
)

target_include_directories(MatrixKbdHalPort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MatrixKbdHalPort
    PUBLIC  MatrixKbdHal
    PUBLIC  AtmelStart
    PRIVATE CompileOptions
)
