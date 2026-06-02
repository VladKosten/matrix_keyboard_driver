# ─── Test runner executable ──────────────────────────────────────────────────
add_executable(MatrixKbdUnitTestRun
    matrix_kbd_test_run.c
)

target_link_libraries(MatrixKbdUnitTestRun
    PUBLIC  MatrixKbdUnitTest
    PRIVATE freertos_kernel
)
