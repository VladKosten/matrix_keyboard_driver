#ifndef MATRIX_KBD_CFG_H_
#define MATRIX_KBD_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

/*===========================================================[MACRO DEFINITIONS]============================================*/

/* Override these macros in your project-specific matrix_kbd_cfg.h */

#ifndef MATRIX_KBD_ROWS_COUNT_MAX
    #define MATRIX_KBD_ROWS_COUNT_MAX       6U
#endif

#ifndef MATRIX_KBD_COLUMNS_COUNT_MAX
    #define MATRIX_KBD_COLUMNS_COUNT_MAX    6U
#endif

#ifndef MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS
    #define MATRIX_KBD_WORKING_THREAD_SCAN_RATE_MS  10U
#endif

#ifndef MATRIX_KBD_KEY_LOG_SIZE
    #define MATRIX_KBD_KEY_LOG_SIZE         32U
#endif

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_KBD_CFG_H_ */
