/**
 * \file MatrixKbdHalPort.h
 * \brief

 * \version 0.1
 * \date 2024-01-17
 * \copyright (c) 2022 All rights reserved
 */
#ifndef MATRIX_KBD_HAL_ASF_H_
#define MATRIX_KBD_HAL_ASF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stddef.h>

#include "matrix_kbd_hal.h"

#include "atmel_start.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

#ifndef MATRIX_KBD_HAL_PORT_COLUMN_COUNT_MAX
    #define MATRIX_KBD_HAL_PORT_COLUMN_COUNT_MAX 6U
#endif

#ifndef MATRIX_KBD_HAL_PORT_ROW_COUNT_MAX
    #define MATRIX_KBD_HAL_PORT_ROW_COUNT_MAX 6U
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
* \brief Port HAL errors enumerator
*/
typedef enum
{
    MATRIX_KBD_HAL_PORT_NO_ERR = 0,           ///< Exit: no errors
    MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR,     ///< Exit: error - invalid pointers
    MATRIX_KBD_HAL_PORT_INIT_ERR,             ///< The HAL PORT was not initialized
    MATRIX_KBD_HAL_PORT_IO_ERR,               ///< IO error
} MatrixKbdHalPortErr_e;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    MatrixKbdHal_s base;        ///< Object parent

    /* The port-specific part */
    uint8_t numberOfColumns;                                        ///< Number of hardware-accessible columns
    uint8_t numberOfRows;                                           ///< Number of hardware-accessible rows
    uint8_t columnsPin[MATRIX_KBD_HAL_PORT_COLUMN_COUNT_MAX];       ///< Pointer to an array that stores the MC pin number of the corresponding column
    uint8_t rowsPin[MATRIX_KBD_HAL_PORT_ROW_COUNT_MAX];             ///< Pointer to an array that stores the MC pin number of the corresponding row
}MatrixKbdHalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init the HAL portable layer for the particular platform
 * \param[in] MatrixKbd_HalPort_t* const halPort - the HAL port descriptor;
 * \param[in] const uint8_t countColumns - number of columns available in hardware
 * \param[in] const uint8_t countRows - number of rows available in hardware
 * \param[in] const uint32_t* const columns - pointer to an array that stores pin numbers MCU for columns
 * \param[in] const uint32_t* const rows - pointer to an array that stores pin numbers MCU for rows
 * \param[in] const void* const parent - the parent object
 * \param[in] const char* const name - the name of the object
 * \param[out] no
 * \return MatrixKbdHalPortErr_e  - error code. non-zero = an error has occurred.

 */
MatrixKbdHalPortErr_e MatrixKbdHalPortInit(MatrixKbdHalPort_s* const halPort,
                                                const uint8_t countColumns,
                                                const uint8_t countRows,
                                                const uint8_t* const columnsPin,
                                                const uint8_t* const rowsPin,
                                                const void* const parent,
                                                const char* const name);

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] MatrixKbdHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return MatrixKbdHalErr_e  - error code. non-zero = an error has occurred.
 */
MatrixKbdHalPortErr_e MatrixKbdHalPortDeinit(MatrixKbdHalPort_s* const halPort);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_HAL_ASF_H_ */



