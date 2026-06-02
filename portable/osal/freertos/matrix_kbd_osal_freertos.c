/**
 * \file      matrix_kbd_osal_freertos.c
 * \brief     Matrix Kbd operation system abstraction layer (OSAL) FreeRTOS port
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "matrix_kbd_osal_freertos.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief MATRIX_KBD_OSAL_FREERTOS_ASSERT macro definition
 */
#ifndef MATRIX_KBD_OSAL_FREERTOS_ASSERT
    #ifdef MATRIX_KBD_ASSERT
        #define MATRIX_KBD_OSAL_FREERTOS_ASSERT(cond) MATRIX_KBD_ASSERT(cond)
    #else
        #define MATRIX_KBD_OSAL_FREERTOS_ASSERT(cond)
    #endif
#endif

/**
 * \brief Name for the thread required for driver operation
 */
#ifndef MATRIX_KBD_OSAL_FREERTOS_THREAD_NAME
    #define MATRIX_KBD_OSAL_FREERTOS_THREAD_NAME "KBD"
#endif

/**
 * \brief Stack size (byte) for the thread required for driver operation
 */
#ifndef MATRIX_KBD_OSAL_FREERTOS_THREAD_STACK_SIZE
    #define MATRIX_KBD_OSAL_FREERTOS_THREAD_STACK_SIZE ((uint16_t) 256)
#endif

/**
 * \brief  priority levels for thread
 */
#ifndef MATRIX_KBD_OSAL_FREERTOS_THREAD_PRIO
    #define MATRIX_KBD_OSAL_FREERTOS_THREAD_PRIO (tskIDLE_PRIORITY + 1)
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/* OSAL port specific functions */
/**
 * \brief       Create thread
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[in]   MatrixKbdOsalFreertosThreadCfg_s* const threadCfg - the thread config
 * \param[out]  no
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalFreertosErr_e matrixKbdOsalFreeRtosThreadCreate(MatrixKbdOsalFreertos_s* const osalFreeRtos, const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg);

/**
 * \brief       Thread Freertos
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 */
static void matrixKbdOsalFreertosThread(void* const osalFreeRtos);

/* OSAL portable functions */

/**
 * \brief Thread resume
 * \param[in] const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosThreadResume(const void* const osal);

/**
 * \brief Thread suspend
 * \param[in] const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosThreadSuspend(const void* const osal);

/**
 * \brief       Perform some delay
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[in]   const uint32_t msDelay - delay in ms
 * \param[out]  no
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosDelayMs(const void* const osal, const uint32_t msDelay);

/**
 * \brief       Perform lock operation (acquire a mutex)
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosLock(const void* const osal);

/**
 * \brief       Perform unlock operation (release a mutex)
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosUnlock(const void* const osal);

/* Portable functions table */
static const MatrixKbdOsalPortable_s matrixkbdOsalFreeRtosPortableFunc =
    {
        .threadResume = matrixKbdOsalFreeRtosThreadResume,      ///< OSAL freertos port thread resume
        .threadSuspend = matrixKbdOsalFreeRtosThreadSuspend,    ///< OSAL freertos port thread suspend
        .threadDelay = matrixKbdOsalFreeRtosDelayMs,            ///< OSAL freertos port delay
        .lock = matrixKbdOsalFreeRtosLock,                      ///< OSAL freertos port lock resources
        .unlock = matrixKbdOsalFreeRtosUnlock                   ///< OSAL freertos port unlock resources
};

/**
 * \brief Standard parameters for the thread
 */
static const MatrixKbdOsalFreertosThreadCfg_s matrixKbdOsalFreertosThreadCfg =
    {
        .name = MATRIX_KBD_OSAL_FREERTOS_THREAD_NAME,
        .stackSize = MATRIX_KBD_OSAL_FREERTOS_THREAD_STACK_SIZE,
        .threadPriority = MATRIX_KBD_OSAL_FREERTOS_THREAD_PRIO};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

/**
 * \brief       Initialize the matrix kbd FreeRTOS osal instance
 * \param[in]   MatrixKbdOsalFreertos_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[in]   const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg - thread parameters descriptor;
 * \param[in]   const void *const parent - pointer to a parent object;
 * \param[in]   const char* const name - the name of the osal object;
 * \param[out]  no;
 * \return      MatrixKbdOsalFreertosErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalFreertosErr_e MatrixKbdOsalFreertosInit(MatrixKbdOsalFreertos_s* const osalFreeRtos, const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg, const void* const parent, const char* const name)
{
    /* Checking of params */
    if (NULL == osalFreeRtos)
    {
        return MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_FREERTOS_CALL_FROM_ISR_ERR;    // Exit: Error: Call from isr
    }

    /* Init the OSAL  */
    MatrixKbdOsal_s* osal = &osalFreeRtos->base;
    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalInit(osal, parent, name);
    if (osalStatus != MATRIX_KBD_OSAL_NO_ERR)
    {
        return MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR;    // Exit: Error: Could not init the base
    }

    /* Try to create mutex */
    SemaphoreHandle_t mutexHandle = xSemaphoreCreateRecursiveMutex();
    if (NULL == mutexHandle)
    {
        osalStatus = MatrixKbdOsalDeinit(osal);
        MATRIX_KBD_OSAL_FREERTOS_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);
        return MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR;    // Exit: Error: Could not create the lock/unlock mutex
    }
    osalFreeRtos->base.mutexHandle = mutexHandle;

    /* Create the thread */
    MatrixKbdOsalFreertosErr_e osalFreeRtosStatus = matrixKbdOsalFreeRtosThreadCreate(osalFreeRtos, threadCfg);
    if (MATRIX_KBD_OSAL_FREERTOS_NO_ERR != osalFreeRtosStatus)
    {
        osalFreeRtosStatus = MatrixKbdOsalFreertosDeinit(osalFreeRtos);
        MATRIX_KBD_OSAL_FREERTOS_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);
        return MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR;    // Exit: Error: Could not init the base
    }

    /* FreeRTOS port specific methods */
    osal->portable = &matrixkbdOsalFreeRtosPortableFunc;

    return MATRIX_KBD_OSAL_FREERTOS_NO_ERR;    // Exit: no errors
}

/**
 * \brief       Deinitialize the matrix kbd FreeRTOS osal instance
 * \param[in]   MatrixKbdOsalFreertos_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalFreertosErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalFreertosErr_e MatrixKbdOsalFreertosDeinit(MatrixKbdOsalFreertos_s* const osalFreeRtos)
{
    /* Checking of params */
    if (NULL == osalFreeRtos)
    {
        return MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_FREERTOS_CALL_FROM_ISR_ERR;    // Exit: Error: Call from isr
    }

    /* equating pointers */
    MatrixKbdOsal_s* osal = (MatrixKbdOsal_s*) osalFreeRtos;

    /* Delete the requested resources */
    if (NULL != osal->mutexHandle)
    {
        vSemaphoreDelete(osal->mutexHandle);
    }

    if (NULL != osal->threadHandle)
    {
        vTaskDelete(osal->threadHandle);
    }

    /* Flush thread parameter*/
    osalFreeRtos->lastWakeTime = 0;
    osalFreeRtos->threadCfg.stackSize = 0;
    osalFreeRtos->threadCfg.threadPriority = 0;
    osalFreeRtos->threadCfg.name = NULL;

    /* Deinit the parent */
    MatrixKbdOsalErr_e osalStatus = MatrixKbdOsalDeinit(osal);
    (void) osalStatus;
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osalStatus == MATRIX_KBD_OSAL_NO_ERR);

    return MATRIX_KBD_OSAL_FREERTOS_NO_ERR;    // Exit: no errors
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================

/**
 * \brief       Create thread
 * \param[in]   MatrixKbdOsalFreertos_s* const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[in]   MatrixKbdOsalFreertosThreadCfg_s* const threadCfg - OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
static MatrixKbdOsalFreertosErr_e matrixKbdOsalFreeRtosThreadCreate(MatrixKbdOsalFreertos_s* const osalFreeRtos, const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osalFreeRtos);
    if (NULL != osalFreeRtos->base.threadHandle)
    {
        return MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR;    // Exit: Error: Invalid args
    }

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_FREERTOS_CALL_FROM_ISR_ERR;    // Exit: Error: Call from isr
    }

    /* Check the thread parameters */
    MatrixKbdOsalFreertosThreadCfg_s thisThreadCfg = {0};
    if (NULL != threadCfg)
    {
        if ((configMINIMAL_STACK_SIZE > threadCfg->stackSize) ||
            (configMAX_PRIORITIES < threadCfg->threadPriority))
        {
            return MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR;
        }
        thisThreadCfg = *threadCfg;
    }
    else
    {
        thisThreadCfg = matrixKbdOsalFreertosThreadCfg;
    }

    /* Creating a thread  */
    BaseType_t xReturned = pdFALSE;
    TaskHandle_t threadHandle = NULL;
    xReturned = xTaskCreate(matrixKbdOsalFreertosThread,
                            thisThreadCfg.name,
                            thisThreadCfg.stackSize,
                            (void*) osalFreeRtos,
                            thisThreadCfg.threadPriority,
                            &threadHandle);

    if (pdPASS != xReturned)
    {
        return MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR;    // Exit: error - memory required to create the thread could not be allocated
    }

    vTaskSuspend(threadHandle);
    osalFreeRtos->base.threadHandle = threadHandle;
    osalFreeRtos->threadCfg = thisThreadCfg;

    return MATRIX_KBD_OSAL_FREERTOS_NO_ERR;    // Exit: no errors
}

/**
 * \brief  MatrixKbd OSAL FreeRtos thread(task) function
 * \param[in] MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out] no;
 */
static void matrixKbdOsalFreertosThread(void* const osalFreeRtos)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osalFreeRtos);

    /* equating pointers */
    MatrixKbdOsalFreertos_s* thisOsalFreeRtos = (MatrixKbdOsalFreertos_s*) osalFreeRtos;

    thisOsalFreeRtos->lastWakeTime = xTaskGetTickCount();
    /* Thread execution loop */
    do
    {
        /* If the load (an worker) of the thread was assigned then carry out it */
        if (NULL != thisOsalFreeRtos->base.threadWorker)
        {
            MATRIX_KBD_OSAL_FREERTOS_ASSERT(thisOsalFreeRtos->base.parent);
            thisOsalFreeRtos->base.threadWorker(thisOsalFreeRtos->base.parent);
        }
        else
        {
            /* If the load of the thread (an worker) was not assigned then suspend the current thread */
            vTaskSuspend(NULL);
        }

    } while (1);

    /* Delete the current thread */
    thisOsalFreeRtos->base.threadHandle = NULL;
    vTaskDelete(NULL);
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(0);
    while (1)
        ;
}

/**
 * \brief Thread Resume
 * \param[in] const MatrixKbdOsal_s* const osal - the OSAL port descriptor;
 * \param[out] no;
 * \return MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosThreadResume(const void* const osal)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osal);

    /* Equate pointers */
    MatrixKbdOsalFreertos_s* osalFreertos = (MatrixKbdOsalFreertos_s*) osal;

    /* Check where the call is coming from */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Call from isr
    }

    vTaskResume(osalFreertos->base.threadHandle);

    return MATRIX_KBD_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Thread suspend
 * \param[in] const MatrixKbdOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred.
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosThreadSuspend(const void* const osal)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osal);

    /* Equate pointers */
    MatrixKbdOsal_s* thisOsal = (MatrixKbdOsal_s*) osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Call from isr
    }

    vTaskSuspend(thisOsal->threadHandle);

    return MATRIX_KBD_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief       Perform some delay
 * \param[in]   const void* const osal - the OSAL port descriptor;
 * \param[in]   const uint32_t msDelay - delay in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosDelayMs(const void* const osal, const uint32_t msDelay)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osal);

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Call from isr
    }

    /* Equate pointers */
    MatrixKbdOsalFreertos_s* osalFreertos = (MatrixKbdOsalFreertos_s*) osal;

    vTaskDelayUntil(&(osalFreertos->lastWakeTime), msDelay);

    return MATRIX_KBD_OSAL_NO_ERR;    // Exit: no error
}

/**
 * \brief       Perform lock operation (acquire a mutex)
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;

 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosLock(const void* const osal)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osal);

    /* Equate pointers */
    MatrixKbdOsal_s* thisOsal = (MatrixKbdOsal_s*) osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Call from isr
    }

    BaseType_t smphrStatus = xSemaphoreTakeRecursive(thisOsal->mutexHandle, portMAX_DELAY);
    if (pdTRUE != smphrStatus)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Could not lock
    }

    return MATRIX_KBD_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief       Perform unlock operation (release a mutex)
 * \param[in]   const MatrixKbdOsal_s* const osal - the OSAL port descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;

 */
static MatrixKbdOsalErr_e matrixKbdOsalFreeRtosUnlock(const void* const osal)
{
    /* Checking of params */
    MATRIX_KBD_OSAL_FREERTOS_ASSERT(osal);

    /* Equate pointers */
    MatrixKbdOsal_s* thisOsal = (MatrixKbdOsal_s*) osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Call from isr
    }

    BaseType_t smphrStatus = xSemaphoreGiveRecursive(thisOsal->mutexHandle);
    if (pdTRUE != smphrStatus)
    {
        return MATRIX_KBD_OSAL_PORT_LAYER_ERR;    // Exit: Error: Could not unlock
    }

    return MATRIX_KBD_OSAL_NO_ERR;    // Exit: no errors
}