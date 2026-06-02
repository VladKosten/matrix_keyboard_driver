#ifndef MATRIX_KBD_OSAL_FREERTOS_H_
#define MATRIX_KBD_OSAL_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "matrix_kbd_osal.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief This enum describes possible errors
 */
typedef enum
{
    MATRIX_KBD_OSAL_FREERTOS_NO_ERR = 0,           ///< Exit: no errors
    MATRIX_KBD_OSAL_FREERTOS_INVALID_ARGS_ERR,     ///< Exit: error - invalid pointers
    MATRIX_KBD_OSAL_FREERTOS_NOT_INIT_ERR,         ///< Exit: error - init error
    MATRIX_KBD_OSAL_FREERTOS_CALL_FROM_ISR_ERR,    ///< Exit: error - call from isr

} MatrixKbdOsalFreertosErr_e;

/**
 * \brief
 */
typedef UBaseType_t MatrixKbdOsalFreertosThreadPriority_t;

/**
 * \brief Parameters of the thread responsible for the worker
 */
typedef struct
{
    char* name;                                              ///< A descriptive name for the task
    size_t stackSize;                                        ///< Stack size in bytes
    MatrixKbdOsalFreertosThreadPriority_t threadPriority;    ///< Priority at which the task is created

} MatrixKbdOsalFreertosThreadCfg_s;

/**
 * \brief Matrix KBD FreeRTOS OSAL structure
 */
typedef struct
{
    // Do not move the base structure from the first position
    MatrixKbdOsal_s base;

    TickType_t lastWakeTime;

    // Optional fields
    MatrixKbdOsalFreertosThreadCfg_s threadCfg;

} MatrixKbdOsalFreertos_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief       Initialize the matrix kbd FreeRTOS osal instance
 * \param[in]   MatrixKbdOsalFreertos_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[in]   const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg - thread parameters descriptor;
 * \param[in]   const void *const parent - pointer to a parent object;
 * \param[in]   const char* const name - the name of the osal object;
 * \param[out]  no;
 * \return      MatrixKbdOsalFreertosErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalFreertosErr_e MatrixKbdOsalFreertosInit(MatrixKbdOsalFreertos_s* const osalFreeRtos, const MatrixKbdOsalFreertosThreadCfg_s* const threadCfg, const void* const parent, const char* const name);

/**
 * \brief       Deinitialize the matrix kbd FreeRTOS osal instance;
 * \param[in]   MatrixKbdOsalFreertos_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalFreertosErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalFreertosErr_e MatrixKbdOsalFreertosDeinit(MatrixKbdOsalFreertos_s* const osalFreeRtos);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_OSAL_FREERTOS_H_ */
