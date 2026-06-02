/**
* \file      matrix_kbd_hal.c
* \brief     Matrix Kbd hardware abstraction layer (HAL) generic module
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "matrix_kbd_hal.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief MATRIX_KBD_HAL_ASSERT macro definition
*/
#ifndef MATRIX_KBD_HAL_ASSERT
    #ifdef MATRIX_KBD_ASSERT
        #define MATRIX_KBD_HAL_ASSERT(cond)  MATRIX_KBD_ASSERT(cond)
    #else
        #define MATRIX_KBD_HAL_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

/**
 * \brief      Initialize MatrixKbd HAL module
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const void* const parent - the parent object;
 * \param[in]  const char* const name - the name of the matrix keyboard;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalInit(MatrixKbdHal_s* const hal, const void* const parent, const char* const name)
{
    /* Checking of params */
    if(NULL == hal)
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }
    /* Init obj */
    hal->name = (char*)name;
    hal->parent = (void*)parent;
    hal->portable = NULL;

    return MATRIX_KBD_HAL_NO_ERR;                   // Exit: no errors
}

/**
 * \brief      Deinitialize MatrixKbd HAL module
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalDeinit(MatrixKbdHal_s* const hal)
{
    /* Checking of params */
    if(NULL == hal)
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Deinit obj */
    hal->name = NULL;
    hal->parent = NULL;
    hal->portable = NULL;

    return MATRIX_KBD_HAL_NO_ERR;               // Exit: no errors
}

/**
 * \brief       Get the parent
 * \param[in]   MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalParentGet(const MatrixKbdHal_s* const hal, void** const parent)
{
    /* Checking of params */
    if((NULL == hal) ||
       (NULL == parent))
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Get the parent */
    *parent = (void*)hal->parent;

    return MATRIX_KBD_HAL_NO_ERR;               // Exit: no errors
}

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdHalErr_e error code, non-zero = error.
 */
MatrixKbdHalErr_e MatrixKbdHalParentSet(MatrixKbdHal_s* const hal, const void* const parent)
{
    /* Checking of params */
    if((NULL == hal) ||
       (NULL == parent))
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Set the parent */
    hal->parent = (void*)parent;

    return MATRIX_KBD_HAL_NO_ERR;               // Exit: no errors
}

/**
* \brief     Get the name of the HAL matrix keyboard
* \param[in] hal - HAL descriptor;
* \param[out] name - pointer to buffer that store the name;
* \return MatrixKbdHalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalNameGet(const MatrixKbdHal_s* const hal, char** const name)
{
    /* Checking of params */
    if((NULL == hal) ||
       (NULL == name))
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Get the name */
    *name = (char*)hal->name;

    return MATRIX_KBD_HAL_NO_ERR;                   // Exit: no errors

}

/**
* \brief     Set the name of the HAL matrix keyboard
* \param[in] hal - the HAL descriptor;
* \param[in] name - the name of the matrix keyboard;
* \param[out] no;
* \return MatrixKbdHalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalNameSet(MatrixKbdHal_s* const hal, const char* const name)
{
    /* Checking of params */
    if((NULL == hal) ||
       (NULL == name))
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Set the name */
    hal->name = (char*)name;

    return MATRIX_KBD_HAL_NO_ERR;                   // Exit: no errors

}

/**
 * \brief       Select the column of the matrix keyboard
 * \param[in]   MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]   const init16_t number - the column number to be selected;
 * \param[out]  no;
 * \return      MatrixKbdHalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalColumnSelect(const MatrixKbdHal_s* const hal, const int16_t number)
{
    /* Checking of params */
    if(NULL == hal)
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == hal->portable) ||
       (NULL == hal->portable->columnSelect))
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;  // Exit: Error: Init error
    }

    /* Selecting a column to read rows */
    MatrixKbdHalErr_e halStatus = hal->portable->columnSelect(hal, number);
    if(halStatus != MATRIX_KBD_HAL_NO_ERR )
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;       // Exit: Error: Init error
    }

    return MATRIX_KBD_HAL_NO_ERR;
}

/**
 * \brief       Reject the column of the matrix keyboard
 * \param[in]   MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]   const init16_t number - the column number to be selected;
 * \param[out]  no;
 * \return      MatrixKbdHalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalColumnDeselect(const MatrixKbdHal_s* const hal, const int16_t number)
{
    /* Checking of params */
    if(NULL == hal)
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == hal->portable) ||
       (NULL == hal->portable->columnDeselect))
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;  // Exit: Error: Init error
    }

    /* Return the column to its original state */
    MatrixKbdHalErr_e halStatus = hal->portable->columnDeselect(hal, number);
    if(halStatus != MATRIX_KBD_HAL_NO_ERR )
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;       // Exit: Error: Port error
    }

    return MATRIX_KBD_HAL_NO_ERR;
}

/**
 * \brief      Getting the rows state of the matrix keyboard
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const uint8_t numberOfRows - Number of rows to get
 * \param[out] uint8_t* const data - pointer to buffer that store the data;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalRowStateGet(const MatrixKbdHal_s* const hal, const uint8_t numberOfRow, bool* const data)
{
    /* Checking of params */
    if((NULL == hal) ||
       (NULL == data))
    {
        return MATRIX_KBD_HAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == hal->portable) ||
       (NULL == hal->portable->rowStateGet))
    {
        return MATRIX_KBD_HAL_NOT_INIT_ERR;  // Exit: Error: Init error
    }

    /* Read the value of the row */
    MatrixKbdHalErr_e halStatus = hal->portable->rowStateGet(hal, numberOfRow, data);
    if(halStatus != MATRIX_KBD_HAL_NO_ERR )
    {
        return MATRIX_KBD_HAL_PORT_LAYER_ERR;       // Exit: Error: Port error
    }

    return MATRIX_KBD_HAL_NO_ERR;
}
//============================================================================[PRIVATE FUNCTIONS]===================================================================================