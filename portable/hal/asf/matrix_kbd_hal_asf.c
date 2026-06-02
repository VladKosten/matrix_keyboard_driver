/**
 * \file      matrix_kbd_hal_asf.c
 * \brief     Matrix Kbd hardware abstraction layer (HAL) portable module for particular platform
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "matrix_kbd_hal_asf.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief MATRIX_KBD_HAL_PORT_ASSERT macro definition
 */
#ifndef MATRIX_KBD_HAL_PORT_ASSERT
    #ifdef MATRIX_KBD_ASSERT
        #define MATRIX_KBD_HAL_PORT_ASSERT(cond) MATRIX_KBD_ASSERT(cond)
    #else
        #define MATRIX_KBD_HAL_PORT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Selecting a specific column to read its corresponding rows
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param[out] no;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalErr_e matrixKbdHalPortColumnSelect(const void* const hal, const uint8_t number);

/**
 * \brief Return the selected column to its default state
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param [out] no;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalErr_e matrixKbdHalPortColumnDeselect(const void* const hal, const uint8_t number);

/**
 * \brief Get the state of a specific row
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - row number to be selected;
 * \param[out] bool* const data - pointer to a variable to transfer the state of rows;
 * \return an error code, non-zero == an error has occurred;
 */
static MatrixKbdHalErr_e matrixKbdHalPortRowGet(const void* const hal, const uint8_t numberOfRows, bool* const data);

/**
 * \brief Check the configuration of input-output ports responsible for the column for correct operation of the matrix driver
 * \param[in] const uint32_t* const columns - pointer to an array that stores pin numbers MCU for columns;
 * \param[in] const uint8_t countColumns - number of columns;
 * \param[out] no;
 * \return MatrixKbdHalPortErr_e - error code. non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortColumnsCheck(const uint8_t* const columns, const uint8_t countColumns);

/**
 * \brief Check the configuration of input-output ports responsible for the row for correct operation of the matrix driver
 * \param[in] const uint32_t* const rows - pointer to an array that stores pin numbers MCU for rows;
 * \param[in] const uint8_t countRows - number of rows;
 * \param[out] no;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortRowsCheck(const uint8_t* const rows, const uint8_t countRows);

/**
 * \brief Table of ported functions for correct operation of HAL level
 */
static const MatrixKbdHalPortable_s matrixKbdHalPortableFuncTab =
    {
        .columnSelect = matrixKbdHalPortColumnSelect,
        .columnDeselect = matrixKbdHalPortColumnDeselect,
        .rowStateGet = matrixKbdHalPortRowGet,
};

/**
 * \brief Check the configuration of input-output ports responsible for the column for correct operation of the matrix driver.
 * \param[in] const uint32_t* const columns - pointer to an array that stores pin numbers MCU for columns
 * \param[in] const uint8_t countColumns - number of columns
 * \param[out] no
 * \return MatrixKbdHalPortErr_e - error code. non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortColumnsCheck(const uint8_t* const columns, const uint8_t countColumns);

/**
 * \brief Check the configuration of input-output ports responsible for the row for correct operation of the matrix driver.
 * \param[in] const uint32_t* const rows - pointer to an array that stores pin numbers MCU for rows
 * \param[in] const uint8_t countRows - number of rows
 * \param[out] no
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortRowsCheck(const uint8_t* const rows, const uint8_t countRows);

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
    if ((NULL == halPort) ||
        (NULL == columnsPin) ||
        (NULL == rowsPin) ||
        (countColumns > MATRIX_KBD_HAL_PORT_COLUMN_COUNT_MAX) ||
        (countRows > MATRIX_KBD_HAL_PORT_ROW_COUNT_MAX))
    {
        return MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Init the HAL base object */
    MatrixKbdHalErr_e halStatus = MatrixKbdHalInit(&halPort->base, parent, name);
    if (halStatus != MATRIX_KBD_HAL_NO_ERR)
    {
        return MATRIX_KBD_HAL_PORT_INIT_ERR;    // Exit: Error: Init error
    }

    /* Check setting the HAL port column */
    MatrixKbdHalPortErr_e halPortStatus = matrixKbdHalPortColumnsCheck(columnsPin, countColumns);
    if (halPortStatus != MATRIX_KBD_HAL_PORT_NO_ERR)
    {
        halStatus = MatrixKbdHalDeinit(&halPort->base);
        MATRIX_KBD_HAL_PORT_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
        return MATRIX_KBD_HAL_PORT_INIT_ERR;    // Exit: Error: Init error
    }
    halPort->numberOfColumns = countColumns;
    for (uint8_t index = 0; index < countColumns; index++)
    {
        halPort->columnsPin [index] = columnsPin [index];
    }

    /* Check setting the HAL port rows */
    halPortStatus = matrixKbdHalPortRowsCheck(rowsPin, countRows);
    if (halPortStatus != MATRIX_KBD_HAL_PORT_NO_ERR)
    {
        halStatus = MatrixKbdHalDeinit(&halPort->base);
        MATRIX_KBD_HAL_PORT_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
        return MATRIX_KBD_HAL_PORT_INIT_ERR;
    }
    halPort->numberOfRows = countRows;
    for (uint8_t index = 0; index < countRows; index++)
    {
        halPort->rowsPin [index] = rowsPin [index];
    }

    /* Init the portable methods table */
    halPort->base.portable = &matrixKbdHalPortableFuncTab;

    return MATRIX_KBD_HAL_PORT_NO_ERR;    // Exit: no errors
}

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] MatrixKbdHalPort_s* const halPort - const halPort - the HAL port descriptor;
 * \param[out] no;
 * \return MatrixKbdHalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdHalPortErr_e MatrixKbdHalPortDeinit(MatrixKbdHalPort_s* const halPort)
{
    if (NULL == halPort)
    {
        return MATRIX_KBD_HAL_PORT_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    halPort->numberOfColumns = 0;
    halPort->numberOfRows = 0;
    MatrixKbdHalErr_e halStatus = MatrixKbdHalDeinit(&halPort->base);
    (void) halStatus;
    MATRIX_KBD_HAL_PORT_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
    for (uint8_t index = 0; index < MATRIX_KBD_HAL_PORT_COLUMN_COUNT_MAX; index++)
    {
        halPort->columnsPin [index] = 0;
    }
    for (uint8_t index = 0; index < MATRIX_KBD_HAL_PORT_ROW_COUNT_MAX; index++)
    {
        halPort->rowsPin [index] = 0;
    }
    return MATRIX_KBD_HAL_PORT_NO_ERR;    // Exit: no errors
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================

/**
 * \brief Check the configuration of input-output ports responsible for the column for correct operation of the matrix driver.
 * \param[in] const uint32_t* const columns - pointer to an array that stores pin numbers MCU for columns;
 * \param[in] const uint8_t countColumns - number of columns available in hardware;
 * \param[out] no;
 * \return MatrixKbdHalPortErr_e - error code. non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortColumnsCheck(const uint8_t* const columns, const uint8_t countColumns)
{
    /* Checking of params */
    MATRIX_KBD_HAL_PORT_ASSERT(columns);
    if ((0 >= countColumns))
    {
        return MATRIX_KBD_HAL_PORT_INIT_ERR;
    }

    /* Check the halStatus of all pins MCU */
    bool pinStatus = true;
    for (uint8_t index = 0; index < countColumns; index++)
    {
        pinStatus = gpio_get_pin_level(columns [index]);
        if (pinStatus == false)
        {
            return MATRIX_KBD_HAL_PORT_INIT_ERR;    // Exit: pin MCU is not configured
        }
    }

    return MATRIX_KBD_HAL_PORT_NO_ERR;    // Exit: no errors
}

/**
 * \brief Check the configuration of input-output ports responsible for the row for correct operation of the matrix driver
 * \param[in] const uint32_t* const rows - pointer to an array that stores pin numbers MCU for rows;
 * \param[in] const uint8_t countRows - number of rows;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalPortErr_e matrixKbdHalPortRowsCheck(const uint8_t* const rows, const uint8_t countRows)
{

    /* Checking of params */
    MATRIX_KBD_HAL_PORT_ASSERT(rows);
    if ((0 >= countRows))
    {
        return MATRIX_KBD_HAL_PORT_INIT_ERR;
    }

    /* Check the halStatus of all pins MCU */
    bool pinStatus = true;
    for (uint8_t index = 0; index < countRows; index++)
    {
        pinStatus = gpio_get_pin_level(rows [index]);
        if (pinStatus == false)
        {
            return MATRIX_KBD_HAL_PORT_IO_ERR;    // Exit: pin MCU is not configured
        }
    }

    return MATRIX_KBD_HAL_PORT_NO_ERR;    // Exit: no errors
}

/**
 * \brief Selecting a specific column to read its corresponding rows
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param[out] no;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalErr_e matrixKbdHalPortColumnSelect(const void* const hal, const uint8_t number)
{
    /* Checking of params */
    MATRIX_KBD_HAL_PORT_ASSERT(hal);

    /* Let's check the value of the need column */
    MatrixKbdHalPort_s* halPort = (MatrixKbdHalPort_s*) hal;
    if (0 == halPort->numberOfColumns)
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;    // Exit: Error: obj is not init
    }
    if (number > halPort->numberOfColumns)
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;    // Exit: Error: Column number exceeds the number of hardware accessible columns
    }

    gpio_set_pin_level(halPort->columnsPin [number], false);

    return MATRIX_KBD_HAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Return the selected column to its default state
 * \param[in] void* const hal - the HAL port descriptor;
 * \param[in] const uint8_t number - column number to be selected;
 * \param [out] no;
 * \return an error code, non-zero = an error has occurred;
 */
static MatrixKbdHalErr_e matrixKbdHalPortColumnDeselect(const void* const hal, const uint8_t number)
{
    /* Checking of params */
    MATRIX_KBD_HAL_PORT_ASSERT(hal);

    /* Let's check the value of the need column */
    MatrixKbdHalPort_s* halPort = (MatrixKbdHalPort_s*) hal;
    if (0 == halPort->numberOfColumns)
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;    // Exit: Error: obj is not init
    }
    if (number > halPort->numberOfColumns)
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;    // Exit: Error: Column number exceeds the number of hardware accessible columns
    }

    gpio_set_pin_level(halPort->columnsPin [number], true);

    return MATRIX_KBD_HAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Get the state of a specific row
 * \param[in] void* const hal - the HAL port descriptor
 * \param[in] const uint8_t number - row number to be selected
 * \param[out] bool* const data - pointer to a variable to transfer the state of rows
 * \return an error code, non-zero = an error has occurred;

 */
static MatrixKbdHalErr_e matrixKbdHalPortRowGet(const void* const hal, const uint8_t numberOfRows, bool* const data)
{
    /* Checking of params */
    MATRIX_KBD_HAL_PORT_ASSERT(hal);
    MATRIX_KBD_HAL_PORT_ASSERT(data);

    /* Let's check the value of the required row */
    MatrixKbdHalPort_s* halPort = (MatrixKbdHalPort_s*) hal;
    if (0 >= halPort->numberOfRows)
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;    // Exit: Error: obj is not init
    }
    if (numberOfRows > halPort->numberOfRows)
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;    // Exit: Error: unacceptable number of columns
    }

    /* Return rows value */
    *data = !gpio_get_pin_level(halPort->rowsPin [numberOfRows]);

    return MATRIX_KBD_HAL_NO_ERR;    // Exit: no errors
}
