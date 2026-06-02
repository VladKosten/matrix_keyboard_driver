#ifndef MATRIX_KBD_HAL_H_
#define MATRIX_KBD_HAL_H_

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
* \brief HAL errors enumerator
*/
typedef enum
{
    MATRIX_KBD_HAL_NO_ERR = 0,          ///< Exit: no errors;
    MATRIX_KBD_HAL_NOT_INIT_ERR, ///< Exit: not init;
    MATRIX_KBD_HAL_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers;
    MATRIX_KBD_HAL_PORT_LAYER_ERR       ///< Exit: error - port level error;

} MatrixKbdHalErr_e;

/**
* \brief HAL portable functions table
*/
typedef struct
{
    /* HAl select column */
    MatrixKbdHalErr_e (*columnSelect)(const void* const hal, const uint8_t number);

    /* HAl reject column */
    MatrixKbdHalErr_e (*columnDeselect)(const void* const hal, const uint8_t number);

    /* Hal getting row state*/
    MatrixKbdHalErr_e (*rowStateGet)(const void* const hal, const uint8_t number, bool* const data);

} MatrixKbdHalPortable_s;


/**
 * \brief Matrix keyboard HAL descriptor
 */
typedef struct
{
    /* Optional fields: */
    const void* parent;
    const char* name;

    /* Portable functions table */
    const MatrixKbdHalPortable_s* portable;

} MatrixKbdHal_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief      Initialize MatrixKbd HAL
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const void* const parent - the parent object;
 * \param[in]  const char* const name - the name of the matrix keyboard;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalInit(MatrixKbdHal_s* const hal, const void* const parent, const char* const name);

/**
 * \brief      Deinitialize MatrixKbd HAL
 * \param[in]  MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalDeinit(MatrixKbdHal_s* const hal);

/**
 * \brief       Get the parent
 * \param[in]   const MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalParentGet(const MatrixKbdHal_s* const hal, void** const parent);

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdHalErr_e error code, non-zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalParentSet(MatrixKbdHal_s* const hal, const void* const parent);

/**
* \brief     Get the name of the HAL matrix keyboard
* \param[in] hal - HAL descriptor;
* \param[out] name - pointer to buffer that store the name;
* \return MatrixKbdHalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalNameGet(const MatrixKbdHal_s* const hal, char** const name);

/**
* \brief     Set the name of the HAL matrix keyboard
* \param[in] hal - the HAL descriptor;
* \param[in] name - the name of the matrix keyboard;
* \param[out] no;
* \return MatrixKbdHalErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdHalErr_e MatrixKbdHalNameSet(MatrixKbdHal_s* const hal, const char* const name);

/**
 * \brief      Select the column of the matrix keyboard
 * \param[in]  const MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const init16_t number - the column number to be selected;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalColumnSelect(const MatrixKbdHal_s* const hal, const int16_t number);

/**
 * \brief      Reject the column of the matrix keyboard
 * \param[in]  const MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const init16_t number - the column number to be selected;
 * \param[out] no;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalColumnDeselect(const MatrixKbdHal_s* const hal, const int16_t number);

/**
 * \brief      Getting the rows state of the matrix keyboard
 * \param[in]  const MatrixKbdHal_s* const hal - HAL descriptor;
 * \param[in]  const uint8_t numberOfRows - Number of rows to get
 * \param[out] uint8_t* const data - pointer to buffer that store the data;
 * \return     MatrixKbdHalErr_e error code, non zero = error;
 */
MatrixKbdHalErr_e MatrixKbdHalRowStateGet(const MatrixKbdHal_s* const hal, const uint8_t numberOfRow, bool* const data);


#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_HAL_H_ */