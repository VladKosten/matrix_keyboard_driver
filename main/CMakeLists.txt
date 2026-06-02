# ─── Core libraries ──────────────────────────────────────────────────────────
add_library(MatrixKbd STATIC
    matrix_kbd.c
)

add_library(MatrixKbdHal STATIC
    matrix_kbd_hal.c
)

add_library(MatrixKbdOsal STATIC
    matrix_kbd_osal.c
)

# ─── Include directories ────────────────────────────────────────────────────
target_include_directories(MatrixKbd    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                             ${CMAKE_SOURCE_DIR})
target_include_directories(MatrixKbdHal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                             ${CMAKE_SOURCE_DIR})
target_include_directories(MatrixKbdOsal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                             ${CMAKE_SOURCE_DIR})

# ─── C standard ─────────────────────────────────────────────────────────────
target_compile_features(MatrixKbd    PUBLIC c_std_11)
target_compile_features(MatrixKbdHal PUBLIC c_std_11)
target_compile_features(MatrixKbdOsal PUBLIC c_std_11)

# ─── Dependencies ───────────────────────────────────────────────────────────
target_link_libraries(MatrixKbd
    PUBLIC MatrixKbdHal
    PUBLIC MatrixKbdOsal
)

# ─── Driver interface library ───────────────────────────────────────────────
add_library(MatrixKbdDriver INTERFACE)

if(MATRIX_KBD_BUILD_CORE_ONLY)
    target_link_libraries(MatrixKbdDriver INTERFACE MatrixKbd)
elseif(MATRIX_KBD_BUILD_FOR_PROJECT)
    target_link_libraries(MatrixKbdOsal PUBLIC DriverConfig)
    target_link_libraries(MatrixKbdHal  PUBLIC DriverConfig)

    target_link_libraries(MatrixKbdDriver
        INTERFACE MatrixKbd
        INTERFACE MatrixKbdHalPort
        INTERFACE MatrixKbdOsalFreeRtos
    )
endif()
