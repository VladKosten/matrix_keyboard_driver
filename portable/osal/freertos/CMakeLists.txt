# ─── FreeRTOS OSAL port ─────────────────────────────────────────────────────
if(NOT MATRIX_KBD_BUILD_FOR_PROJECT)
    FetchContent_Declare(
        freertos_kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG        V10.6.0
    )

    set(FREERTOS_HEAP    3       CACHE STRING "" FORCE)
    set(FREERTOS_PORT    GCC_POSIX CACHE STRING "" FORCE)

    FetchContent_MakeAvailable(freertos_kernel)
endif()

# ─── Library ────────────────────────────────────────────────────────────────
add_library(MatrixKbdOsalFreeRtos STATIC
    matrix_kbd_osal_freertos.c
)

target_include_directories(MatrixKbdOsalFreeRtos PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MatrixKbdOsalFreeRtos
    PRIVATE freertos_config
    PUBLIC  freertos_kernel
    PUBLIC  MatrixKbdOsal
)

if(MATRIX_KBD_BUILD_FOR_PROJECT)
    target_link_libraries(MatrixKbdOsalFreeRtos PRIVATE CompileOptions)
endif()
