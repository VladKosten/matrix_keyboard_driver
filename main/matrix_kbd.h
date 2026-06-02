#ifndef MATRIX_KBD_H_
#define MATRIX_KBD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "matrix_kbd_osal.h"
#include "matrix_kbd_hal.h"
#include "matrix_kbd_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
* \brief Max matrix keyboard rows count
*/
#ifndef MATRIX_KBD_ROWS_COUNT_MAX
    #define MATRIX_KBD_ROWS_COUNT_MAX       6U
#endif

/**
* \brief Max Matrix keyboard columns count
 */
#ifndef MATRIX_KBD_COLUMNS_COUNT_MAX
    #define MATRIX_KBD_COLUMNS_COUNT_MAX    6U
#endif

/**
* \brief Max size of the matrix keyboard
 */
#ifndef MATRIX_KBD_SIZE_MAX
    #define MATRIX_KBD_SIZE_MAX             36U
#endif

/**
* \brief Delay for thread worker
*/
#ifndef MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS
    #define MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS      10U
#endif

/**
* \brief The size of the circular buffer for storing the last pressed keys
 */
#ifndef MATRIX_KBD_KEY_LOG_SIZE
    #define MATRIX_KBD_KEY_LOG_SIZE         32U
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
* \brief This enum describes possible errors
*/
typedef enum
{
    MATRIX_KBD_NO_ERR = 0,          ///< Exit: no errors
    MATRIX_KBD_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers
    MATRIX_KBD_NOT_INIT_ERR,        ///< Exit: error - init error
    MATRIX_KBD_INIT_ERR,            ///< Exit: error - init error
    MATRIX_KBD_PORT_ERR,            ///< Exit: error - port error
    MATRIX_KBD_KEY_ERR              ///< Exit: error - key error

} MatrixKbdErr_e;

/**
* \brief Matrix keyboard key state data type definition
 */
typedef bool MatrixKbdKeyState_t;

/**
* \brief The entire keyboard state data type definition
*/
typedef struct
{
    uint32_t updtCount;
    MatrixKbdKeyState_t key[MATRIX_KBD_ROWS_COUNT_MAX][MATRIX_KBD_COLUMNS_COUNT_MAX];
} MatrixKbdState_s;

/**
* \brief Define the type for scar rate
 */
typedef uint32_t MatrixKbdScanRateMs_t ;

/**
* \brief Define the type for the work related to the "column"
 */
typedef uint8_t MatrixKbdColumn_t;

/**
* \brief Define the type for the work related to the "row"
 */
typedef uint8_t MatrixKbdRow_t;

/**
* \brief Definition data type for key state
 */
typedef bool MatrixKbdKeyState_t;

/**
* \brief Key callbacks types
*/
typedef enum
{
    MATRIX_KBD_KEY_UNPRESSED_CB_TYPE = 0x01, ///< Key unpressed (curent status:"0"; previous status:"0";)
    MATRIX_KBD_KEY_PRESSING_CB_TYPE  = 0x02, ///< Key unpressed (curent status:"1"; previous status:"0";)
    MATRIX_KBD_KEY_PRESSED_CB_TYPE   = 0x04, ///< Key unpressed (curent status:"1"; previous status:"1";)
    MATRIX_KBD_KEY_RELEASED_CB_TYPE  = 0x08, ///< Key released  (curent status:"0"; previous status:"1";)
    MATRIX_KBD_ALL_CB_TYPE           = 0x0F, ///< All callback

}MatrixKbdKeyCbType_e;

/**
 * \brief Key identifier data type
 */
typedef uint8_t MatrixKbdKeyId_t;

/**
 * \brief CallBack For all key
 * \param[in] void* const kbd - matrix keyboard descriptor instance;
 * \param[in] const MatrixKbdKey_s* const key - matrix key descriptor instance;
 * \param[in] const MatrixKbdKeyCbType_e cbType - the callback type;
 * \param[out] no;
 * \return no;
 */
typedef void (*MatrixKbdKeyCb_f)(void* const kbd, void* const key, const MatrixKbdKeyCbType_e cbType);

/**
* \brief Matrix keyboard key instance
 */
typedef struct MatrixKbdKey_t
{
    /* Optional fields: */
    const void* parent;                                                                                       ///< pointer to the parent object

    MatrixKbdKeyId_t id;                                                                                               ///< key identifier
    MatrixKbdKeyState_t state;                                                                                         ///< key state

    /* Key level callbacks */
    MatrixKbdKeyCb_f matrixKbdKeyUnpressedCb;     ///< Key unpressed (curent status:"0"; previous status:"0";)
    MatrixKbdKeyCb_f matrixKbdKeyPressingCb;      ///< Key unpressed (curent status:"1"; previous status:"0";)
    MatrixKbdKeyCb_f matrixKbdKeyPressedCb;       ///< Key unpressed (curent status:"1"; previous status:"1";)
    MatrixKbdKeyCb_f matrixKbdKeyReleasedCb;      ///< Key released  (curent status:"0"; previous status:"1";)

}MatrixKbdKey_s;


/**
* \brief Matrix keyboard log of the last pressed keys
 */
typedef struct
{
    MatrixKbdKey_s* lastPressedKey[MATRIX_KBD_KEY_LOG_SIZE];    ///< buffer for the last pressed keys (circular buffer)
    uint8_t start;                                      ///< start index of the buffer
    uint8_t end;                                        ///< end index of the buffer

}MatrixKbdKeyLog_s;



/**
 * \brief Matrix keyboard instance
 */
typedef struct
{
    /* Optional fields: */
    const void* parent;     ///< pointer to the parent object
    const char* name;       ///< name of the keyboard

    /* HAL */
    const MatrixKbdHal_s* hal;

    /* OSAL */
    const MatrixKbdOsal_s* osal;

    /* Keys */
    MatrixKbdKey_s keys[MATRIX_KBD_ROWS_COUNT_MAX][MATRIX_KBD_COLUMNS_COUNT_MAX];

    /* Matrix dimension */
    MatrixKbdRow_t rowsCount;
    MatrixKbdColumn_t columnsCount;

    /* Frequency of matrix interrogation */
    MatrixKbdScanRateMs_t scanRateMs;

    /* The last keyboard scan result */
    MatrixKbdState_s state;

    /* The last pressed keys */
    MatrixKbdKeyLog_s keyLog;

    /*Callback for all key*/
    MatrixKbdKeyCb_f matrixKbdAllKeyUnpressedCb;
    MatrixKbdKeyCb_f matrixKbdAllKeyPressingCb;
    MatrixKbdKeyCb_f matrixKbdAllKeyPressedCb;
    MatrixKbdKeyCb_f matrixKbdAllKeyReleasedCb;

} MatrixKbd_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

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
                                const char* const name);


/**
 * \brief      Deinitialize MatrixKbd module
 * \param[in]  MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] no;
 * \return     MatrixKbdErr_e error code, non zero = error.
 */
MatrixKbdErr_e MatrixKbdDeinit(MatrixKbd_s* const kbd);

/**
 * \brief       Get the parent
 * \param[in]   const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  void** const parent - parent object;
 * \return      MatrixKbdErr_e error code, non zero = error.
 */
MatrixKbdErr_e MatrixKbdParentGet(const MatrixKbd_s* const kbd, void** const parent);

/**
 * \brief       Set the parent
 * \param[in]   MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[in]   const void* const parent - parent object;
 * \param[out]  no;
 * \return      MatrixKbdErr_e error code, non-zero = error.
 */
MatrixKbdErr_e MatrixKbdParentSet(MatrixKbd_s* const kbd, const void* const parent);

/**
 * \brief     Set the name of the matrix keyboard
 * \param[in] MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[in] const char* const name - the name of the matrix keyboard;
 * \param[out] no;
 * \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdNameSet(MatrixKbd_s* const kbd, const char* const name);

/**
 * \brief     Get the name of the matrix keyboard
 * \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] char** const name - buffer for get the name;
 * \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdNameGet(const MatrixKbd_s* const kbd, char** const name);

/**
* \brief Get the key by its identifier (row and column)
* \param[in] kbd -  the matrix keyboard descriptor to get the key;
* \param[in] column - the number of the column;
* \param[in] row - the number of the row;
* \param[out] key - buffer for the key descriptor;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyGet(const MatrixKbd_s* const kbd, const MatrixKbdColumn_t column, const MatrixKbdRow_t row, MatrixKbdKey_s** const key);

/**
* \brief Get the key by its identifier
* \param[in] kbd -  the matrix keyboard descriptor to get the key;
* \param[in] id - the key identifier;
* \param[out] key - buffer for the key descriptor;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyGetById(const MatrixKbd_s* const kbd, const MatrixKbdKeyId_t id, MatrixKbdKey_s** const key);

/**
* \brief Get the id of key in kbd
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[out] id - buffer for the key identifier;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyIdGet(const MatrixKbd_s* const kbd, const MatrixKbdKey_s* const key, MatrixKbdKeyId_t* const  id);

/**
* \brief Set the id of the key in the keyboard
* \note By default, the key identifier is equal to the number of the key in the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[in] id - the key identifier;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyIdSet(const MatrixKbd_s* const kbd, MatrixKbdKey_s* const key, const MatrixKbdKeyId_t id);

/**
 * \brief      Get the state of the matrix keyboard
 * \param[in]  const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out] MatrixKbdState_s* const kbdState - the last state (scan) of the keyboard;
 * \return     MatrixKbdErr_e error code, non zero = error.
 */
MatrixKbdErr_e MatrixKbdKeysStateGet(const MatrixKbd_s* const kbd, MatrixKbdState_s* const kbdState);

/**
* \brief        Get the state of the key
* \param[in]    const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[in]    const MatrixKbdKey_s* const key - the key descriptor;
* \param[in]    MatrixKbdKeyState_t* const keyState - the state of the key;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyStateGet(const MatrixKbd_s* const kbd, const MatrixKbdKey_s* const key, MatrixKbdKeyState_t* const keyState);

/**
 * \brief       Get the count of rows
 * \param[in]   const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  MatrixKbdRow_t* const rowsCount - the count of matrix rows;
 * \return      MatrixKbdErr_e an error code, non zero = an error has occurred.
 */
MatrixKbdErr_e MatrixKbdRowsCountGet(const MatrixKbd_s* const kbd, MatrixKbdRow_t* const rowsCount);

/**
 * \brief       Get the count of columnsWW
 * \param[in]   const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
 * \param[out]  MatrixKbdColumn_t* const columnCount - the count of matrix columns;
 * \return      MatrixKbdErr_e error code, non zero = error.
 */
MatrixKbdErr_e MatrixKbdColumnsCountGet(const MatrixKbd_s* const kbd, MatrixKbdColumn_t* const columnCount);

/**
* \brief Get number of keys in the keyboard
* \param[in] kbd - the matrix keyboard descriptor;
* \param[out] keysCount - buffer for the number of keys;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeysCountGet(const MatrixKbd_s* const kbd, uint32_t* const keysCount);

/**
* \brief This function attaches the callback to the key
* \param [in] kbd - the matrix keyboard descriptor;
* \param [in] cbType - the callback type;
* \param [in] cb - the callback;
* \param [out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdAllKeyCbAttach(MatrixKbd_s* const kbd, const MatrixKbdKeyCbType_e cbType, const MatrixKbdKeyCb_f cb);

/**
* \brief This function detaches the callback from the all keys
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] cbType - the callback type;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdAllKeyCbDetach(MatrixKbd_s* const kbd, const MatrixKbdKeyCbType_e cbType);

/**
* \brief This function attaches the callback to the key
* \param [in] kbd - the matrix keyboard descriptor;
* \param [in] key - the key descriptor;
* \param [in] cbType - the callback type;
* \param [in] cb - the callback;
* \param [out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyCbAttach(MatrixKbd_s* const kbd, MatrixKbdKey_s* const key, const MatrixKbdKeyCbType_e cbType, const MatrixKbdKeyCb_f cb);

/**
* \brief This function detaches the callback from the key
* \param[in] kbd - the matrix keyboard descriptor;
* \param[in] key - the key descriptor;
* \param[in] cbType - the callback type;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyCbDetach(MatrixKbd_s* const kbd,  MatrixKbdKey_s* const key, const MatrixKbdKeyCbType_e cbType);

/**
* \brief Get the last pressed keys from the keyboard
* \note Maximum number of keys is limited by the size of the buffer (default = 32 keys)
* \param[in] kbd - the matrix keyboard descriptor;
* \param[out] keys - the buffer for the keys;
* \param[in] numberOfLastPressedKeys - the number of the last pressed keys;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdKeyLastPressedGet(const MatrixKbd_s* const kbd, MatrixKbdKey_s** const keys, const uint8_t numberOfLastPressedKeys);

/**
* \brief Start polling the status of the keys
* \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdScanStart(const MatrixKbd_s* const kbd);

/**
* \brief Suspend polling the status of the keys
* \param[in] const MatrixKbd_s* const kbd - the matrix keyboard descriptor;
* \param[out] no;
* \return MatrixKbdErr_e - error code. non-zero = an error has occurred;
 */
MatrixKbdErr_e MatrixKbdScanSuspend(const MatrixKbd_s* const kbd);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_H_ */