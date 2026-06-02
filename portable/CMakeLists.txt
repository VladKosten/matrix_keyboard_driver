# ─── Port layers ─────────────────────────────────────────────────────────────
if(NOT MATRIX_KBD_BUILD_CORE_ONLY)
    add_subdirectory(osal/freertos)

    if(MATRIX_KBD_BUILD_FOR_PROJECT)
        add_subdirectory(hal/asf)
    else()
        add_subdirectory(hal/cmock)
    endif()
endif()
