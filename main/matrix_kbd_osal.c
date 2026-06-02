/**
* \file      matrix_kbd_osal.c
* \brief     Matrix Kbd os abstraction layer (OSAL) generic module
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "matrix_kbd_osal.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief Assert macro definition
*/
#ifndef MATRIX_KBD_OSAL_ASSERT
    #ifdef MATRIX_KBD_ASSERT
        #define MATRIX_KBD_OSAL_ASSERT(cond)  MATRIX_KBD_ASSERT(cond)
    #else
        #define MATRIX_KBD_OSAL_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================


//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================


//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

/**
 * \brief      Initialize MatrixKbd OSAL module
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]  void const *parent - pointer to a parent object;
 * \param[in]  const char* const name - name of the OSAL instance;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalInit(MatrixKbdOsal_s* const osal, const void* const parent, const char* const name)
{
    /* Checking of params */
    if (osal == NULL)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args;
    }

    /* Initialize the object */
    osal->name = (void*)name;
    osal->parent = (void*)parent;
    osal->portable = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;
    osal->threadWorker = NULL;

    return MATRIX_KBD_OSAL_NO_ERR;
}

/**
 * \brief      Deinitialize MatrixKbd OSAL module
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalDeinit(MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if (osal == NULL)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Null pointers*/
    osal->name = NULL;
    osal->parent = NULL;
    osal->portable = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;
    osal->threadWorker = NULL;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief       Get the parent
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalParentGet(const MatrixKbdOsal_s* const osal, void** const parent)
{
    /* Checking of params */
    if((NULL == osal) ||
       (NULL == parent))
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    *parent = (void*)osal->parent;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalParentSet(MatrixKbdOsal_s* const osal, const void* const parent)
{
    /* Checking of params */
    if((NULL == parent) ||
       (NULL == osal))
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    osal->parent = (void*)parent;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
* \brief The function returns the name of the OSAL instance
* \param[in] osal - OSAL descriptor to get the name from;
* \param[out] name - buffer to store the name;
* \return MatrixKbdOsalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalNameGet(const MatrixKbdOsal_s* const osal, char** const name)
{
    /* Checking of params */
    if((NULL == osal) ||
       (NULL == name))
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    *name = (void*)osal->name;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors

}

/**
* \brief The function sets the name of the OSAL instance
* \param[in] osal - the OSAL descriptor to set the name to;
* \param[in] name - the name to set;
* \param[out] no;
* \return MatrixKbdOsalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalNameSet(MatrixKbdOsal_s* const osal, const char* const name)
{
    /* Checking of params */
    if((NULL == osal) ||
       (NULL == name))
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    osal->name = (void*)name;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors

}

/**
 * \brief       Resume thread
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadResume(const MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if(NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->threadHandle) ||
       (NULL == osal->portable)     ||
       (NULL == osal->portable->threadResume))
    {
        return MATRIX_KBD_OSAL_NOT_INIT_ERR;        // Exit: Error: Init obj
    }

    MatrixKbdOsalErr_e osalStatus = osal->portable->threadResume(osal);
    if( osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;      // Exit: Error: Port layer
    }

    return MATRIX_KBD_OSAL_NO_ERR;                   // Exit: no errors
}

/**
 * \brief       Suspend thread
 * \param[in]   const MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadSuspend(const MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if(NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->threadHandle) ||
       (NULL == osal->portable)     ||
       (NULL == osal->portable->threadSuspend))
    {
        return MATRIX_KBD_OSAL_NOT_INIT_ERR;        // Exit: Error: Init obj
    }

    MatrixKbdOsalErr_e osalStatus = osal->portable->threadSuspend(osal);
    if( osalStatus != MATRIX_KBD_OSAL_NO_ERR )
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;      // Exit: Error: Port layer
    }

    return MATRIX_KBD_OSAL_NO_ERR;                   // Exit: no errors
}

/**
 * \brief      Assign pointer for a OSAL worker
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]  matrixKbdOsalWorkerPtr workerPtr - pointer to a worker;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadWorkerAttach(MatrixKbdOsal_s* const osal, MatrixKbdOsalThreadWorkerPtr_f workerPtr)
{
    /* Checking of params */
    if((NULL == osal) ||
       (NULL == workerPtr))
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    osal->threadWorker = workerPtr;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief      Detach thread worker
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadWorkerDetach(MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if(NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;     // Exit: Error: Invalid args
    }

    osal->threadWorker = NULL;

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief      OSAL delay function
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor
 * \param[in]  uint32_t msDelay - delay value in ms
 * \param[out] no
 * \return     MatrixKbdOsalErr_e error code, non zero = error
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadDelay(const MatrixKbdOsal_s* const osal, uint32_t msDelay)
{
    /* Checking of params */
    if (NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->threadHandle) ||
       (NULL == osal->portable)     ||
       (NULL == osal->portable->threadDelay))
    {
        return MATRIX_KBD_OSAL_NOT_INIT_ERR;        // Exit: Error: init obj
    }

    MatrixKbdOsalErr_e osalStatus = osal->portable->threadDelay(osal,msDelay);
    if( osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;      // Exit: Error: Port layer
    }

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief      OSAL lock resources
 * \param[in]  MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = err;
 */
MatrixKbdOsalErr_e MatrixKbdOsalLock(const MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if (NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->mutexHandle)  ||
       (NULL == osal->portable)     ||
       (NULL == osal->portable->lock))
    {
        return MATRIX_KBD_OSAL_NOT_INIT_ERR;        // Exit: Error: init obj;
    }

    MatrixKbdOsalErr_e osalStatus = osal->portable->lock(osal);
    if( osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;      // Exit: Error: Port layer
    }

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief      OSAL unlock resources
 * \param[in]  MatrixKbdOsal_s* const osal - osal descriptor
 * \param[out] no
 * \return     MatrixKbdOsalErr_e error code, non zero = err
 */
MatrixKbdOsalErr_e MatrixKbdOsalUnlock(const MatrixKbdOsal_s* const osal)
{
    /* Checking of params */
    if(NULL == osal)
    {
        return MATRIX_KBD_OSAL_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->mutexHandle)  ||
       (NULL == osal->portable)     ||
       (NULL == osal->portable->unlock))
    {
        return MATRIX_KBD_OSAL_NOT_INIT_ERR;    // Exit: Error: init obj;
    }

    MatrixKbdOsalErr_e osalStatus = osal->portable->unlock(osal);
    if( osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;      // Exit: Error: Port layer
    }

    return MATRIX_KBD_OSAL_NO_ERR;                  // Exit: no errors
}
//============================================================================[PRIVATE FUNCTIONS]===================================================================================