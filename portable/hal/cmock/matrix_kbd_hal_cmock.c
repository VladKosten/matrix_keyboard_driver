/**
* \file      matrix_kbd_hal_cmock.c
* \brief     Matrix Kbd cmock portable module for particular platform
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "matrix_kbd_hal_cmock.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief MATRIX_KBD_HAL_PORT_ASSERT macro definition
*/
#ifndef MATRIX_KBD_HAL_PORT_ASSERT
    #ifdef MATRIX_KBD_ASSERT
        #define MATRIX_KBD_HAL_PORT_ASSERT(cond)  MATRIX_KBD_ASSERT(cond)
    #else
        #define MATRIX_KBD_HAL_PORT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================


//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Table of ported functions for correct operation of HAL level
 */
static const MatrixKbdHalPortable_s matrixKbdHalPortableFuncTab =
{
    .columnSelect       = MatrixKbdHalPortColumnSelect,
    .columnDeselect     = MatrixKbdHalPortColumnDeselect,
    .rowStateGet        = MatrixKbdHalPortRowStateGet,

};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================
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
                                                const char* const name)
{
    /* Checking of params */
    if((NULL == halPort) ||
       (NULL == columnsPin) ||
       (NULL == rowsPin) ||
       (0 >= countColumns) ||
       (0 >= countRows))
    {
        return MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Init the HAL base object */
    MatrixKbdHalErr_e halStatus = MatrixKbdHalInit(&halPort->base,parent, name);
    if(halStatus != MATRIX_KBD_HAL_NO_ERR)
    {
        return MATRIX_KBD_HAL_PORT_INIT_ERR;
    }

    /* Init the portable methods table */
    halPort->base.portable = &matrixKbdHalPortableFuncTab;

    return MATRIX_KBD_HAL_PORT_NO_ERR; // Exit: no errors
}

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] MatrixKbdHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return MatrixKbdHalErr_e  - error code. non-zero = an error has occurred.
 */
MatrixKbdHalPortErr_e MatrixKbdHalPortDeinit(MatrixKbdHalPort_s* const halPort)
{
    if (NULL == halPort)
    {
        return MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    halPort->columnsPin = NULL;
    halPort->numberOfColumns = 0;
    halPort->rowsPin = NULL;
    halPort->numberOfRows = 0;

    MatrixKbdHalErr_e status = MatrixKbdHalDeinit(&halPort->base);
    (void)status;
    MATRIX_KBD_HAL_PORT_ASSERT(!status);

    return MATRIX_KBD_HAL_PORT_NO_ERR;              // Exit: no errors
}
//============================================================================[PRIVATE FUNCTIONS]===================================================================================