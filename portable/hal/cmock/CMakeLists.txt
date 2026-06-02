# ─── CMock HAL port (for host-side testing) ─────────────────────────────────
FetchContent_Declare(
    cmock
    GIT_REPOSITORY https://github.com/grinch0/CMock.git
)

FetchContent_MakeAvailable(cmock)

# ─── Library ────────────────────────────────────────────────────────────────
add_library(MatrixKbdHalPortCmock STATIC
    matrix_kbd_hal_cmock.c
    mocks.c
)

target_include_directories(MatrixKbdHalPortCmock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MatrixKbdHalPortCmock
    PUBLIC MatrixKbdHal
    PUBLIC unity
    PUBLIC cmock
)
