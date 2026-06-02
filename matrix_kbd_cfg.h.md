#ifndef MATRIX_KBD_CFG_H_
#define MATRIX_KBD_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif


/*================================================================[INCLUDE]=================================================*/

#include <atmel_start.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
* \brief Assert macro
 */
#define MATRIX_KBD_ASSERT(cond)                              ASSERT((cond))

/**
* \brief Maximum number of rows
 */
#define MATRIX_KBD_ROWS_COUNT_MAX                               4U

/**
* \brief Maximum number of columns
 */
#define MATRIX_KBD_COLUMNS_COUNT_MAX                            5U

/**
* \brief Scan rate in milliseconds
 */
#define MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS                  10U



#ifdef __cplusplus
}
#endif

#endif