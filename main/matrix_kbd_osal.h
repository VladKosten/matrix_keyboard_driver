#ifndef MATRIX_KBD_OSAL_H_
#define MATRIX_KBD_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "matrix_kbd_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Thread handle
 */
typedef void* MatrixKbdOsalThreadHandle_t;

/**
 * \brief Lock/unlock mutex handle
 */
typedef void* MatrixKbdOsalMutexHandle_t;

/**
 * \brief This enum describes possible errors
 */
typedef enum
{
    MATRIX_KBD_OSAL_NO_ERR = 0,          ///< Exit: no errors;
    MATRIX_KBD_OSAL_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers;
    MATRIX_KBD_OSAL_NOT_INIT_ERR,        ///< Exit: error - the object is not correctly initialized;
    MATRIX_KBD_OSAL_PORT_LAYER_ERR       ///< Exit: error - port level,(invalid arg,not memory,from isr;

} MatrixKbdOsalErr_e;

/**
 * \brief Type of pointer to a matrix kbd osal worker
 */
typedef void (*MatrixKbdOsalThreadWorkerPtr_f)(const void* const kbd);

/**
 * \brief Interfaces functions
 */
typedef struct
{
    MatrixKbdOsalErr_e (*threadResume)(const void* const osal);                           ///< OSAL resume thread;
    MatrixKbdOsalErr_e (*threadSuspend)(const void* const osal);                          ///< OSAL suspend thread;
    MatrixKbdOsalErr_e (*threadDelay)(const void* const osal, const uint32_t msDelay);    ///< OSAL delay function;
    MatrixKbdOsalErr_e (*lock)(const void* const osal);                                   ///< OSAL lock resources;
    MatrixKbdOsalErr_e (*unlock)(const void* const osal);                                 ///< OSAL unlock resources;

} MatrixKbdOsalPortable_s;

/**
 * \brief Matrix keyboard OSAL instance
 */
typedef struct
{
    /* Optional fields: */
    const void* parent;    ///< Parent object handle;
    const void* name;      ///< Name of the OSAL instance;

    /* Mandatory fields: */
    MatrixKbdOsalThreadWorkerPtr_f threadWorker;    ///< Thread worker;
    MatrixKbdOsalMutexHandle_t mutexHandle;         ///< Mutex handle;
    MatrixKbdOsalThreadHandle_t threadHandle;       ///< Thread handle;

    /*  Portable methods table */
    const MatrixKbdOsalPortable_s* portable;    ///< Portable functions table;

} MatrixKbdOsal_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief       Initialize MatrixKbd OSAL
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptors;
 * \param[in]   const void* const parent - parent object;
 * \param[in]   const void* const name - name of the OSAL instance;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalInit(MatrixKbdOsal_s* const osal, const void* const parent, const char* const name);

/**
 * \brief       Deinitialize MatrixKbd OSAL
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptors
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;

 */
MatrixKbdOsalErr_e MatrixKbdOsalDeinit(MatrixKbdOsal_s* const osal);

/**
 * \brief       Get the parent
 * \param[in]   const MatrixKbdOsal_s* const osal - OSAL descriptors;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdOsalErr_e error code, non zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalParentGet(const MatrixKbdOsal_s* const osal, void** const parent);

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalParentSet(MatrixKbdOsal_s* const osal, const void* const parent);

/**
 * \brief The function returns the name of the OSAL instance
 * \param[in] osal - OSAL descriptor to get the name from;
 * \param[out] name - buffer to store the name;
 * \return MatrixKbdOsalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalNameGet(const MatrixKbdOsal_s* const osal, char** const name);

/**
 * \brief The function sets the name of the OSAL instance
 * \param[in] osal - the OSAL descriptor to set the name to;
 * \param[in] name - the name to set;
 * \param[out] no;
 * \return MatrixKbdOsalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalNameSet(MatrixKbdOsal_s* const osal, const char* const name);

/**
 * \brief       Resume thread
 * \param[in]   const MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadResume(const MatrixKbdOsal_s* const osal);

/**
 * \brief       Suspend thread
 * \param[in]   const MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e error code, non-zero = error;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadSuspend(const MatrixKbdOsal_s* const osal);

/**
 * \brief       Attach thread
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]   workerPtr - pointer to a function for a thread;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadWorkerAttach(MatrixKbdOsal_s* const osal, MatrixKbdOsalThreadWorkerPtr_f workerPtr);

/**
 * \brief       Detach thread
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]   workerPtr - pointer to a function for a thread;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadWorkerDetach(MatrixKbdOsal_s* const osal);

/**
 * \brief       Perform some delay
 * \param[in]   MatrixKbdOsal_s* const osal - OSAL descriptor;
 * \param[in]   const uint32_t msDelay - the delay timeout value expressed in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;

 */
MatrixKbdOsalErr_e MatrixKbdOsalThreadDelay(const MatrixKbdOsal_s* const osal, const uint32_t msDelay);

/**
 * \brief       Perform lock operation (acquire a mutex)
 * \param[in]   MatrixKbdOsal_s* const osal  -  OSAL descriptor;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
MatrixKbdOsalErr_e MatrixKbdOsalLock(const MatrixKbdOsal_s* const osal);

/**
 * \brief      Perform unlock operation (release a mutex)
 * \param[in]  MatrixKbdOsal_s* const osal - osal descriptor;
 * \param[out] no;
 * \return     MatrixKbdOsalErr_e error code, non zero = err;
 */
MatrixKbdOsalErr_e MatrixKbdOsalUnlock(const MatrixKbdOsal_s* const osal);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_OSAL_H_ */