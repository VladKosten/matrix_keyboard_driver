
#ifndef MATRIX_KBD_HAL_PORT_CMOCK_H_
#define MATRIX_KBD_HAL_PORT_CMOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stddef.h>

#include "matrix_kbd_hal.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Port HAL errors enumerator
 */
typedef enum
{
    MATRIX_KBD_HAL_PORT_NO_ERR = 0,          ///< Exit: no errors
    MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers
    MATRIX_KBD_HAL_PORT_INIT_ERR,            ///< The HAL PORT was not initialized
    MATRIX_KBD_HAL_PORT_IO_ERR,              ///< IO error
} MatrixKbdHalPortErr_e;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    MatrixKbdHal_s base;    ///< Object parent

    /* The port-specific part */
    uint8_t numberOfColumns;    ///< Number of hardware-accessible columns
    uint8_t numberOfRows;       ///< Number of hardware-accessible rows
    uint8_t* columnsPin;        ///< Pointer to an array that stores the MC pin number of the corresponding column
    uint8_t* rowsPin;           ///< Pointer to an array that stores the MC pin number of the corresponding row
} MatrixKbdHalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init the HAL portable layer for the particular platform
 * \param[in] MatrixKbd_HalPort_t* const halPort - the HAL port descriptor;
 * \param[in] const uint8_t countColumns - number of columns available in hardware;
 * \param[in] const uint8_t countRows - number of rows available in hardware;
 * \param[in] const uint32_t* const columns - pointer to an array that stores pin numbers MCU for columns;
 * \param[in] const uint32_t* const rows - pointer to an array that stores pin numbers MCU for rows;
 * \param[in] const void* const parent - the parent object;
 * \param[in] const char* const name - the name of the matrix keyboard;
 * \param[out] no;
 * \return MatrixKbdHalPortErr_e  - error code. non-zero = an error has occurred;
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
 * \param[in] MatrixKbdHalPort_s* const halPort - const halPort - the HAL port descriptor;
 * \param[out] no;
 * \return MatrixKbdHalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdHalPortErr_e MatrixKbdHalPortDeinit(MatrixKbdHalPort_s* const halPort);

/**
 * \brief Selecting a specific column to read its corresponding rows
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param[out] no;
 * \return an error code, non-zero == an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalPortColumnSelect(const void* const hal, const uint8_t number);

/**
 * \brief Return the selected column to its default state
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param [out] no;
 * \return an error code, non-zero == an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalPortColumnDeselect(const void* const hal, const uint8_t number);

/**
 * \brief Get the state of a specific row
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - row number to be selected;
 * \param[out] bool* const data - pointer to a variable to transfer the state of rows;
 * \return an error code, non-zero == an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalPortRowStateGet(const void* const hal, const uint8_t number, bool* const data);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_HAL_PORT_CMOCK_H_ */
