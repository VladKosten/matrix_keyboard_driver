/**
* \file      matrix_kbd.c
* \brief     Matrix Kbd driver main module
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \copyright Copyright (c) 
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include "matrix_kbd.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief MATRIX_KBD_ASSERT macro definition
*/
#ifndef MATRIX_KBD_ASSERT
    #define MATRIX_KBD_ASSERT(cond)
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================


//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief The thread worker of the matrix kbd
 * \param[in] const void* const kbdInstance - the kbd descriptor;
 * \param[out] no;
 * \return no;
 */
static void matrixKbdWorker(const void* const kbdInstance);

/**
* \brief This function locks the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdLock(const MatrixKbd_s* const kbd);

/**
* \brief This function unlocks the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdUnlock(const MatrixKbd_s* const kbd);

/**
* \brief This function adds a key to the log of pressed keys
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[out] no;
 */
static inline void matrixKbdKeyLogAdd(MatrixKbdKeyLog_s* const keyLog, const MatrixKbdKey_s* const key);

/**
* \brief This function flushes the log of pressed keys (clears the log)
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdLastPressedFlush(MatrixKbdKeyLog_s* const keyLog);

/**
* \brief This function reads the state of the matrix keyboard
* \note This function is called from the worker thread and is not intended for direct use.
*       For poll need buffer for store state of key and after poll need update state of key
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] state - the state of the matrix keyboard;
* \param[out] no;
 */
static inline void matrixKbdStatesPoll(MatrixKbd_s* const kbd, MatrixKbdState_s* const state);

/**
* \brief This function updates the state of the matrix keyboard
* \note This function is called from the worker thread and is not intended for direct use.
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] state - the state of the matrix keyboard (buffer, after poll);
* \param[out] no;
 */
static inline void matrixKbdStatesUpdate(MatrixKbd_s* const kbd, const MatrixKbdState_s* const state);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief      Initialize MatrixKbd module
 * \param[in]  MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]  const MatrixKbdRow_t rowsCount - count of rows;
 * \param[in]  const MatrixKbdColumn_t columnCount - count of columns;
 * \param[in]  const void* const parent - parent object;
 * \param[out] no;
 * \return     MatrixKbdErr_e error code, non zero = error.
 */
MatrixKbdErr_e MatrixKbdInit(MatrixKbd_s* const kbd,
                             MatrixKbdOsal_s* const osal,
                             MatrixKbdHal_s* const hal,
                             const MatrixKbdRow_t rowsCount,
                             const MatrixKbdColumn_t columnsCount,
                             const void* const parent,
                             const char* const name)
{
    /* Checking of params */
    if((NULL == kbd)                            ||
       (NULL == hal)                            ||
       (NULL == osal)                           ||
       (0 == rowsCount)                         ||
       (0 == columnsCount)                      ||
       (rowsCount > MATRIX_KBD_ROWS_COUNT_MAX)  ||
       (columnsCount > MATRIX_KBD_COLUMNS_COUNT_MAX))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Set matrix dimensions */
    kbd->rowsCount = rowsCount;
    kbd->columnsCount = columnsCount;

    /* Set the sampling rate of the matrix */
    kbd->scanRateMs = MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS;

    /* Save the HAL and OSAL descriptors */
    kbd->hal = hal;
    kbd->osal = osal;

    /* Set parent pointer of the kbd object */
    kbd->parent = (void*)parent;

    /* Set the name of the matrix keyboard */
    kbd->name = (char*)name;

    /* Deploying RT environment */
    MatrixKbdErr_e status = MATRIX_KBD_NO_ERR;
    (void)status;
    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalThreadWorkerAttach(osal,matrixKbdWorker);
    if(osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        status = MatrixKbdDeinit(kbd);
        MATRIX_KBD_ASSERT(status == MATRIX_KBD_NO_ERR);
        return MATRIX_KBD_INIT_ERR;             // Exit: Error: Init error
    }

    /* Set parent pointer for osal and hal layers */
    MatrixKbdHalErr_e halStatus = MatrixKbdHalParentSet((MatrixKbdHal_s*)kbd->hal, (void*)kbd);
    if(halStatus != MATRIX_KBD_HAL_NO_ERR)
    {
        status = MatrixKbdDeinit(kbd);
        MATRIX_KBD_ASSERT(status == MATRIX_KBD_NO_ERR);
        return MATRIX_KBD_INIT_ERR;             // Exit: Error: Init error
    }
    osalStatus = MatrixKbdOsalParentSet((MatrixKbdOsal_s*)kbd->osal, (void*)kbd);
    if(osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        status = MatrixKbdDeinit(kbd);
        MATRIX_KBD_ASSERT(status == MATRIX_KBD_NO_ERR);
        return MATRIX_KBD_INIT_ERR;             // Exit: Error: Init error
    }

    /* Clear states and update info on keys */
    for(MatrixKbdRow_t row = 0; row < rowsCount; row++)
    {
        for(MatrixKbdColumn_t column = 0; column < columnsCount; column++)
        {
            kbd->state.key[row][column] = false;
            kbd->keys[row][column].id = column + row * columnsCount;
            kbd->keys[row][column].parent = (void*)kbd;
            kbd->keys[row][column].matrixKbdKeyPressedCb = NULL;
            kbd->keys[row][column].matrixKbdKeyPressingCb = NULL;
            kbd->keys[row][column].matrixKbdKeyReleasedCb = NULL;
            kbd->keys[row][column].matrixKbdKeyUnpressedCb = NULL;
        }
    }

    /* Clear the log of pressed keys */
    matrixKbdLastPressedFlush(&kbd->keyLog);

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
 * \brief      Deinitialize MatrixKbd module
 * \param[in]  MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] no;
 * \return     MatrixKbdErr_e error code, non zero = error;
 */
MatrixKbdErr_e MatrixKbdDeinit(MatrixKbd_s* const kbd)
{
    /* Checking of params */
    if(NULL == kbd)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Destroy the entire object */
    memset(kbd, 0, sizeof(MatrixKbd_s));

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
* \brief Start polling the status of the keys
* \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[out] no;
* \return MatrixKbdErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdScanStart(const MatrixKbd_s* const kbd)
{
    /* Checking of params */
    if(NULL == kbd)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    if(NULL == kbd->osal)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Not init
    }

    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalThreadResume(kbd->osal);
    if(osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_PORT_ERR;             // Exit: port error
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
* \brief Stop polling the status of the keys
* \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[out] no;
* \return MatrixKbdErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdScanSuspend(const MatrixKbd_s* const kbd)
{
    /* Checking of params */
    if(NULL == kbd)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    if(kbd->osal == NULL)
    {
        return MATRIX_KBD_NOT_INIT_ERR;
    }

    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalThreadSuspend(kbd->osal);
    if(osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_PORT_ERR;             // Exit: port error
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
 * \brief       Get the parent
 * \param[in]   MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdErr_e error code, non zero = error;
 */
MatrixKbdErr_e MatrixKbdParentGet(const MatrixKbd_s* const kbd, void** const parent)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == parent))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Output the parent value */
    *parent = (void*)kbd->parent;

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdErr_e error code, non-zero = error;
 */
MatrixKbdErr_e MatrixKbdParentSet(MatrixKbd_s* const kbd, const void* const parent)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == parent))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Set the parent */
    kbd->parent = (void*)parent;

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
 * \brief     Set the name of the matrix keyboard
 * \param[in] MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[in] const char* const name - the name of the matrix keyboard;
 * \param[out] no;
 * \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdNameSet(MatrixKbd_s* const kbd, const char* const name)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == name))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Set the name */
    kbd->name = (char*)name;

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
 * \brief     Get the name of the matrix keyboard
 * \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] char** const name - buffer for get the name;
 * \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdNameGet(const MatrixKbd_s* const kbd, char** const name)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == name))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Get the name */
    *name = (char*)kbd->name;

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors

}

/**
* \brief Get the id of key in kbd
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[out] id - buffer for the key identifier;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyIdGet(const MatrixKbd_s* const kbd, const MatrixKbdKey_s* const key, uint8_t* const  id)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key) ||
       (NULL == id))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }
    /* Check is kbd key */
    if(kbd != (MatrixKbd_s* )key->parent)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    *id = key->id;

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
* \brief Get the key by its identifier
* \param[in] kbd -  the matrix keyboard descriptor to get the key;
* \param[in] id - the key identifier;
* \param[out] key - buffer for the key descriptor;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyGetById(const MatrixKbd_s* const kbd, const MatrixKbdKeyId_t id, MatrixKbdKey_s** const key)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Search the key by its identifier */
    for(MatrixKbdRow_t row = 0; row < kbd->rowsCount; row++)
    {
        for(MatrixKbdColumn_t column = 0; column < kbd->columnsCount; column++)
        {
            if(kbd->keys[row][column].id == id)
            {
                *key = (MatrixKbdKey_s*)&kbd->keys[row][column];
                return MATRIX_KBD_NO_ERR;       // Exit: no errors
            }
        }
    }

    return MATRIX_KBD_INVALID_ARGS_ERR;           // Exit: Error: Not found
}

/**
* \brief Set the id of the key in the keyboard
* \note By default, the key identifier is equal to the number of the key in the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[in] id - the key identifier;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyIdSet(const MatrixKbd_s* const kbd, MatrixKbdKey_s* const key, const uint8_t id)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }
    /* Check is kbd key */
    if(kbd != (MatrixKbd_s* )key->parent)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    key->id = id;

    return MATRIX_KBD_NO_ERR;               // Exit: no errors

}

/**
 * \brief      Get the state of the matrix keyboard
 * \param[in]  MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] MatrixKbdState_s* const kbdState - the last state (scan) of the keyboard;
 * \return     MatrixKbdErr_e error code, non zero = error;
 */
MatrixKbdErr_e MatrixKbdKeysStateGet(const MatrixKbd_s* const kbd, MatrixKbdState_s* const kbdState)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == kbdState))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if(NULL == kbd->osal)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Init error
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Output the last keyboard scan result */
    *kbdState = kbd->state;

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
* \brief        Get the state of the key
* \param[in]    const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[in]    const MatrixKbdKey_s* const key - the key descriptor;
* \param[in]    MatrixKbdKeyState_t* const keyState - the state of the key;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyStateGet(const MatrixKbd_s* const kbd, const MatrixKbdKey_s* const key, MatrixKbdKeyState_t* const keyState)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key) ||
       (NULL == keyState))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Check is kbd key */
    if(kbd != (MatrixKbd_s* )key->parent)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Get the state of the key */
    *keyState = key->state;

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
 * \brief       Get the count of rows
 * \param[in]   MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  MatrixKbdRow_t* const rowsCount - the count of matrix rows;
 * \return      MatrixKbdErr_e an error code, non zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdRowsCountGet(const MatrixKbd_s* const kbd, MatrixKbdRow_t* const rowsCount)
{
    /* Checking of params */
    if((NULL == kbd)    ||
       (NULL == rowsCount))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    *rowsCount = kbd->rowsCount;

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
 * \brief       Get the count of columns
 * \param[in]   MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  MatrixKbdColumn_t* const columnCount - the count of matrix columns;
 * \return      MatrixKbdErr_e error code, non zero = error;
 */
MatrixKbdErr_e MatrixKbdColumnsCountGet(const MatrixKbd_s* const kbd, MatrixKbdColumn_t* const columnCount)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == columnCount))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    *columnCount = kbd->columnsCount;

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
* \brief Get the key by its identifier (row and column)
* \param[in] kbd -  the matrix keyboard descriptor to get the key;
* \param[in] column - the number of the column;
* \param[in] row - the number of the row;
* \param[out] key - buffer for the key descriptor;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyGet(const MatrixKbd_s* const kbd, const MatrixKbdColumn_t column, const MatrixKbdRow_t row, MatrixKbdKey_s** const key)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key) ||
       (column > MATRIX_KBD_COLUMNS_COUNT_MAX) ||
       (row > MATRIX_KBD_ROWS_COUNT_MAX))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    *key = (MatrixKbdKey_s*)&kbd->keys[row][column];

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
* \brief Get number of keys in the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
* \param[out] keysCount - buffer for the number of keys;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeysCountGet(const MatrixKbd_s* const kbd, uint32_t* const keysCount)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == keysCount))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    *keysCount = kbd->rowsCount * kbd->columnsCount;

    return MATRIX_KBD_NO_ERR;               // Exit: no errors
}

/**
* \brief This function attaches the callback to the key
* \param [in] kbd - the matrix keyboard descriptor;
* \param [in] cbType - the callback type;
* \param [in] cb - the callback;
* \param [out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdAllKeyCbAttach(MatrixKbd_s* const kbd, const MatrixKbdKeyCbType_e cbType, const MatrixKbdKeyCb_f cb)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == cb))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Check is init obj */
    if(kbd->osal == NULL)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Not init
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Attach the callback */
    uint8_t cbAttached = 0;
    if(cbType & MATRIX_KBD_KEY_PRESSED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyPressedCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_PRESSING_CB_TYPE)
    {
        kbd->matrixKbdAllKeyPressingCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_RELEASED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyReleasedCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_UNPRESSED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyUnpressedCb = cb;
        cbAttached++;
    }

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    /* Check is attached */
    if(!cbAttached)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors

}

/**
* \brief This function detaches the callback from the all keys
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] cbType - the callback type;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdAllKeyCbDetach(MatrixKbd_s* const kbd, const MatrixKbdKeyCbType_e cbType)
{
    /* Checking of params */
    if(NULL == kbd)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Attach the callback */
    uint8_t cbAttached = 0;
    if(cbType & MATRIX_KBD_KEY_PRESSED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyPressedCb = NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_PRESSING_CB_TYPE)
    {
        kbd->matrixKbdAllKeyPressingCb= NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_RELEASED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyReleasedCb = NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_UNPRESSED_CB_TYPE)
    {
        kbd->matrixKbdAllKeyUnpressedCb = NULL;
        cbAttached++;
    }

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    /* Check is attached */
    if(!cbAttached)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
* \brief This function attaches the callback to the key
* \param [in] kbd - the matrix keyboard descriptor;
* \param [in] key - the key descriptor;
* \param [in] cbType - the callback type;
* \param [in] cb - the callback;
* \param [out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyCbAttach (MatrixKbd_s* const kbd, MatrixKbdKey_s* const key, const MatrixKbdKeyCbType_e cbType, const MatrixKbdKeyCb_f cb)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == cb) ||
       (NULL == key))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Check is init obj */
    if(kbd->osal == NULL)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Not init
    }

    /* Check this key is part of kbd */
    if(kbd != (MatrixKbd_s* )key->parent)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR; // Exit: Error: Invalid args
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Attach the callback */
    uint8_t cbAttached = 0;
    if(cbType & MATRIX_KBD_KEY_PRESSED_CB_TYPE)
    {
        key->matrixKbdKeyPressedCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_PRESSING_CB_TYPE)
    {
        key->matrixKbdKeyPressingCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_RELEASED_CB_TYPE)
    {
        key->matrixKbdKeyReleasedCb = cb;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_UNPRESSED_CB_TYPE)
    {
        key->matrixKbdKeyUnpressedCb = cb;
        cbAttached++;
    }

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    /* Check is attached */
    if(!cbAttached)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
* \brief This function detaches the callback from the key
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[in] cbType - the callback type;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyCbDetach(MatrixKbd_s* const kbd,  MatrixKbdKey_s* const key, const MatrixKbdKeyCbType_e cbType)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == key))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Check this key is part of kbd */
    if(kbd != (MatrixKbd_s* )key->parent)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Check is init obj */
    if(kbd->osal == NULL)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Not init
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Attach the callback */
    uint8_t cbAttached = 0;
    if(cbType & MATRIX_KBD_KEY_PRESSED_CB_TYPE)
    {
        key->matrixKbdKeyPressedCb = NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_PRESSING_CB_TYPE)
    {
        key->matrixKbdKeyPressingCb = NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_RELEASED_CB_TYPE)
    {
        key->matrixKbdKeyReleasedCb = NULL;
        cbAttached++;
    }
    if(cbType & MATRIX_KBD_KEY_UNPRESSED_CB_TYPE)
    {
        key->matrixKbdKeyUnpressedCb = NULL;
        cbAttached++;
    }

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    /* Check is attached */
    if(!cbAttached)
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors
}

/**
* \brief Get the last pressed keys from the keyboard
* \note Maximum number of keys is limited by the size of the buffer (default = 32 keys)
* \param[in] kbd - the matrix keyboard descriptor;
* \param[out] keys - the buffer for the keys;
* \param[in] numberOfLastPressedKeys - the number of the last pressed keys;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyLastPressedGet(const MatrixKbd_s* const kbd, MatrixKbdKey_s** const keys, const uint8_t numberOfLastPressedKeys)
{
    /* Checking of params */
    if((NULL == kbd) ||
       (NULL == keys) ||
       (0 == numberOfLastPressedKeys))
    {
        return MATRIX_KBD_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Check is init obj */
    if(kbd->osal == NULL)
    {
        return MATRIX_KBD_NOT_INIT_ERR;         // Exit: Error: Not init
    }

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Get the last pressed keys */
    uint8_t numberOfLastPressedKeysReal = 0;
    for(uint8_t i = kbd->keyLog.start; i != kbd->keyLog.end; i = (i + 1) % MATRIX_KBD_KEY_LOG_SIZE)
    {
        keys[numberOfLastPressedKeysReal++] = kbd->keyLog.lastPressedKey[i];
        if(numberOfLastPressedKeysReal >= numberOfLastPressedKeys)
        {
            break;
        }
    }

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);

    return MATRIX_KBD_NO_ERR;                   // Exit: no errors

}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
* \brief This function adds a key to the log of pressed keys
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[out] no;
 */
static inline void matrixKbdKeyLogAdd(MatrixKbdKeyLog_s* const keyLog, const MatrixKbdKey_s* const key)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(keyLog);
    MATRIX_KBD_ASSERT(key);

    /* Add the key to the log */
    keyLog->lastPressedKey[keyLog->end] = (MatrixKbdKey_s*)key;
    keyLog->end = (keyLog->end + 1) % MATRIX_KBD_KEY_LOG_SIZE;
    if (keyLog->end == keyLog->start)
    {
        keyLog->start = (keyLog->start + 1) % MATRIX_KBD_KEY_LOG_SIZE;
    }
}

/**
* \brief This function flushes the log of pressed keys (clears the log)
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdLastPressedFlush(MatrixKbdKeyLog_s* const keyLog)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(keyLog);

    /* Clear the log */
    keyLog->start = 0;
    keyLog->end = 0;
    for (uint8_t i = 0; i < MATRIX_KBD_KEY_LOG_SIZE; i++)
    {
        keyLog->lastPressedKey[i] = NULL;
    }
}

/**
* \brief This function locks the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdLock(const MatrixKbd_s* const kbd)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(kbd);
    MATRIX_KBD_ASSERT(kbd->osal);

    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalLock(kbd->osal);
    (void)osalStatus;
    MATRIX_KBD_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);
}

/**
* \brief This function unlocks the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
 */
static inline void matrixKbdUnlock(const MatrixKbd_s* const kbd)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(kbd);
    MATRIX_KBD_ASSERT(kbd->osal);

    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalUnlock(kbd->osal);
    (void)osalStatus;
    MATRIX_KBD_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);
}

/**
 * \brief The thread worker of the matrix kbd
 * \note The main independent stream that is required for the driver to work.
 *       The matrix is queried one button at a time, after which it is written
 *       to a temporary storage buffer and passed to the corresponding button object (if attached).
 *       After polling, state update and suspend thread
 * \param[in] void* const kbd - the kbd descriptor;
 * \param[out] no;
 */
static void matrixKbdWorker(const void* const kbdInstance)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(kbdInstance);

    /* Checking is init obj */
    MatrixKbd_s* kbd = (MatrixKbd_s*)kbdInstance;
    MATRIX_KBD_ASSERT(kbd->hal);
    MATRIX_KBD_ASSERT(kbd->osal);

    /* Preparing the environment */
    MatrixKbdOsalErr_e osalStatus = MATRIX_KBD_OSAL_NO_ERR;
    (void)osalStatus;

    /* : variables for storage and processing with state of key */
    MatrixKbdState_s currentStates = {0};

    /* Matrix kbd worker main loop */
    while(1)
    {
        /* Reading keyboard state in buffer */
        matrixKbdStatesPoll(kbd, &currentStates);

        /* Update the state of the keyboard */
        matrixKbdStatesUpdate(kbd, &currentStates);

        /* Sleep */
        osalStatus = MatrixKbdOsalThreadDelay(kbd->osal, kbd->scanRateMs);
        MATRIX_KBD_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);
    }
}

/**
* \brief This function reads the state of the matrix keyboard
* \note This function is called from the worker thread and is not intended for direct use.
*       For poll need buffer for store state of key and after poll need update state of key
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] state - the state of the matrix keyboard;
* \param[out] no;
 */
static inline void matrixKbdStatesPoll(MatrixKbd_s* const kbd, MatrixKbdState_s* const state)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(state);

    for(MatrixKbdColumn_t columnNumber = 0; columnNumber < kbd->columnsCount; columnNumber++)
        {
            MatrixKbdHalErr_e halStatus = MatrixKbdHalColumnSelect(kbd->hal,columnNumber);
            (void)halStatus;
            MATRIX_KBD_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
            /* : one row at a time */
            for(MatrixKbdRow_t rowNumber = 0; rowNumber < kbd->rowsCount; rowNumber++)
            {
                /* : read state */
                halStatus = MatrixKbdHalRowStateGet(kbd->hal, rowNumber, &state->key[rowNumber][columnNumber]);
                MATRIX_KBD_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
            }
            halStatus = MatrixKbdHalColumnDeselect(kbd->hal,columnNumber);
            MATRIX_KBD_ASSERT(halStatus == MATRIX_KBD_HAL_NO_ERR);
        }
}

/**
* \brief This function updates the state of the matrix keyboard
* \note This function is called from the worker thread and is not intended for direct use.
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] state - the state of the matrix keyboard (buffer, after poll);
* \param[out] no;
 */
static inline void matrixKbdStatesUpdate(MatrixKbd_s* const kbd, const MatrixKbdState_s* const stateCurrent)
{
    /* Checking of params */
    MATRIX_KBD_ASSERT(stateCurrent);
    MATRIX_KBD_ASSERT(kbd);

    /* Lock the keyboard */
    matrixKbdLock(kbd);

    /* Update the state of the keyboard */
    for(MatrixKbdRow_t row = 0; row < kbd->rowsCount; row++)
    {
        for(MatrixKbdColumn_t column = 0; column < kbd->columnsCount; column++)
        {
            if(stateCurrent->key[row][column] != kbd->state.key[row][column])
            {
                if(stateCurrent->key[row][column] == true)
                {
                    /* Add the key to the log of pressed keys */
                    matrixKbdKeyLogAdd(&kbd->keyLog, &kbd->keys[row][column]);

                    /* Update the state of the key */
                    kbd->keys[row][column].state = true;

                    /* CallBack for all key */
                    if(kbd->matrixKbdAllKeyPressingCb != NULL)
                    {
                        kbd->matrixKbdAllKeyPressingCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_PRESSING_CB_TYPE);
                    }

                    if(kbd->keys[row][column].matrixKbdKeyPressingCb != NULL)
                    {
                        kbd->keys[row][column].matrixKbdKeyPressingCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_PRESSING_CB_TYPE);
                    }


                }
                else
                {
                    /* Update the state of the key */
                    kbd->keys[row][column].state = false;

                    /* CallBack for all key */
                    if(kbd->matrixKbdAllKeyReleasedCb != NULL)
                    {
                        kbd->matrixKbdAllKeyReleasedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_RELEASED_CB_TYPE);
                    }

                    if(kbd->keys[row][column].matrixKbdKeyReleasedCb != NULL)
                    {
                        kbd->keys[row][column].matrixKbdKeyReleasedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_RELEASED_CB_TYPE);
                    }
                }
            }
            else
            {
                if(stateCurrent->key[row][column] == true)
                {

                    /*CallBack for all key*/
                    if(kbd->matrixKbdAllKeyPressedCb != NULL)
                    {
                        kbd->matrixKbdAllKeyPressedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_PRESSED_CB_TYPE);
                    }

                    if(kbd->keys[row][column].matrixKbdKeyPressedCb != NULL)
                    {
                        kbd->keys[row][column].matrixKbdKeyPressedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_PRESSED_CB_TYPE);
                    }
                }
                else
                {
                    /* CallBack for all key */
                    if(kbd->matrixKbdAllKeyUnpressedCb!= NULL)
                    {
                        kbd->matrixKbdAllKeyUnpressedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
                    }

                    if(kbd->keys[row][column].matrixKbdKeyUnpressedCb != NULL)
                    {
                        kbd->keys[row][column].matrixKbdKeyUnpressedCb(kbd, &kbd->keys[row][column], MATRIX_KBD_KEY_UNPRESSED_CB_TYPE);
                    }
                }
            }
            /* Update the state of the key */
            kbd->state.key[row][column] = stateCurrent->key[row][column];
        }
    }

    /* Update the counter of the keyboard state updates */
    kbd->state.updtCount++;

    /* Unlock the keyboard */
    matrixKbdUnlock(kbd);
}